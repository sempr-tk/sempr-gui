#include "ReteWidget.hpp"
#include "../ui/ui_retewidget.h"

#include "ReteVisualSerialization.hpp"
#include "GraphNodeItem.hpp"
#include "GraphEdgeItem.hpp"
#include "GraphvizLayout.hpp"

#include <QRectF>
#include <cmath>
#include <utility>

namespace sempr { namespace gui {

ReteWidget::ReteWidget(QWidget* parent)
    : QWidget(parent),
      form_(new Ui::ReteWidget),
      timerId_(0)
{
    form_->setupUi(this);
    form_->graphicsView->setScene(&scene_);
    form_->rulesTree->setColumnCount(2);
    form_->rulesTree->setHeaderLabels({"ID", "Name"});
    form_->ruleEdit->setReadOnly(true);
    form_->ruleEdit->setWordWrapMode(QTextOption::NoWrap);

    connect(form_->btnUpdate, &QPushButton::clicked,
            this, &ReteWidget::rebuild);
    connect(form_->btnUpdate, &QPushButton::clicked,
            this, &ReteWidget::populateTreeWidget);

    /* // this is rather distracting. Instead, only highlight selected *rules*.
    connect(&scene_, &QGraphicsScene::selectionChanged,
            this, &ReteWidget::onSelectionChanged);
    */

    connect(form_->rulesTree, &QTreeWidget::currentItemChanged,
            this, &ReteWidget::onSelectedRuleChanged);
    connect(form_->rulesTree, &QTreeWidget::itemChanged,
            this, &ReteWidget::updateGraphVisibility);
    connect(form_->boxDynamicPositioning, &QCheckBox::stateChanged,
            this, [this]()
            {
                if (this->form_->boxDynamicPositioning->isChecked())
                {
                    timerId_ = startTimer(1000 / 25);
                }
                else if (timerId_)
                {
                    killTimer(timerId_);
                    timerId_ = 0;
                }
            });
}

ReteWidget::~ReteWidget()
{
    delete form_;
}


void ReteWidget::timerEvent(QTimerEvent* /*event*/)
{
    for (auto node : nodes_)
    {
        node.second->calculateForces();
    }

    for  (auto node : nodes_)
    {
        node.second->advancePosition();
    }
}


void ReteWidget::setConnection(AbstractInterface::Ptr conn)
{
    sempr_ = conn;
    rebuild();
    populateTreeWidget();
}

void ReteWidget::onSelectionChanged()
{
    auto selectedItems = scene_.selectedItems();
    if (selectedItems.empty())
    {
        highlight("");
    }
    else
    {
        auto first = selectedItems[0];
        auto node = dynamic_cast<GraphNodeItem*>(first);
        if (node) highlight(node);
    }
}


void ReteWidget::onSelectedRuleChanged(QTreeWidgetItem* current)
{
    auto& rule = rules_[current];
    form_->ruleEdit->setText(QString::fromStdString(rule.ruleString));

    highlight(rule.effectNodes);
}



void ReteWidget::highlight(GraphNodeItem* node)
{
    for (auto entry : nodes_)
    {
        if (entry.second == node)
        {
            highlight(entry.first);
            return;
        }
    }
}

void ReteWidget::highlight(const std::string& id)
{
    std::vector<std::string> ids;
    ids.push_back(id);
    highlight(ids);
}

void ReteWidget::highlight(const std::vector<std::string>& ids)
{
    // un-highlight all
    for (auto entry : nodes_)
    {
        entry.second->setHighlighted(false);
        for (auto e : entry.second->edges())
        {
            e->setGlobalHighlighted(false);
        }
    }

    // highlight not only the one node, but all its ancestors and decendants,
    // too.
    std::set<std::pair<std::string, bool>> visited;
    std::vector<std::pair<std::string, bool>> toVisit; // which node to visit, and in which direction (only up / only down)

    for (auto& id : ids)
    {
        toVisit.push_back({id, false});
        toVisit.push_back({id, true});
    }

    while (!toVisit.empty())
    {
        auto entry = toVisit.back();
        toVisit.pop_back();

        if (visited.find(entry) != visited.end()) continue;

        auto node = nodes_.find(entry.first);
        if (node != nodes_.end())
        {
            node->second->setHighlighted(true);

            // highlight its edges?
            auto edges = node->second->edges();
            for (auto e : edges)
            {
                e->setGlobalHighlighted(
                    e->from()->isHighlighted() &&
                    e->to()->isHighlighted()
                );
            }
        }

        // dont visit this node again.
        visited.insert(entry);

        // find all ancestors and decendants
        for (auto edge : graph_.edges)
        {
            if (!entry.second && edge.from == entry.first) toVisit.push_back({edge.to, false});
            if (entry.second && edge.to == entry.first) toVisit.push_back({edge.from, true});
        }
    }
}


void ReteWidget::rebuild()
{
    nodes_.clear();
    nodeList_.clear();
    edgeList_.clear();
    scene_.clear();

    graph_ = sempr_->getReteNetworkRepresentation();

    for (auto node : graph_.nodes)
    {
        //auto item = new GraphNodeItem(node.type, QString::fromStdString(node.label));
        GraphNodeItem::Shape shape = GraphNodeItem::Ellipse;
        if (node.type == Node::Type::MEMORY) shape = GraphNodeItem::Rectangle;

        auto item = new GraphNodeItem(QString::fromStdString(node.label), shape);
        scene_.addItem(item); // scene takes ownership
        item->setFlag(QGraphicsItem::ItemIsMovable);

        nodes_[node.id] = item;
        nodeList_.push_back(item);
    }

    for (auto edge : graph_.edges)
    {
        auto from = nodes_[edge.from];
        auto to = nodes_[edge.to];

        auto edgeItem = new GraphEdgeItem(from, to);
        edgeItem->adjust();
        scene_.addItem(edgeItem);

        edgeList_.push_back(edgeItem);
    }


    resetLayout();
}


void ReteWidget::updateGraphVisibility()
{
    // hide all
    for (auto& entry : nodes_)
    {
        entry.second->hide();
        entry.second->update();
        for (auto edge : entry.second->edges())
        {
            edge->setVisible(false);
            edge->update();
        }
    }

    // start at the production nodes and enable all their ancestors
    std::vector<std::string> toVisit;
    for (auto& entry : rules_)
    {
        if (entry.first->checkState(0) == Qt::Checked)
        {
            toVisit.insert(toVisit.end(), entry.second.effectNodes.begin(),
                                          entry.second.effectNodes.end());
        }
    }

    while (!toVisit.empty())
    {
        auto toShow = toVisit.back(); toVisit.pop_back();
        auto item = nodes_[toShow];
        item->show();

        for (auto& e : item->edges())
        {
            if (e->to() == item) e->setVisible(true);
        }

        for (auto& e : graph_.edges)
        {
            if (e.to == toShow) toVisit.push_back(e.from);
        }
    }

    // redo the layout
    resetLayout();
}


void ReteWidget::populateTreeWidget()
{
    form_->rulesTree->clear();
    form_->ruleEdit->clear();
    rules_.clear();

    auto rules = sempr_->getRulesRepresentation();

    for (auto& rule : rules)
    {
        auto item = new QTreeWidgetItem();
        item->setText(0, QString::number(rule.id));
        item->setText(1, QString::fromStdString(rule.name));
        //item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(0, Qt::Checked);
        rules_[item] = rule;

        form_->rulesTree->addTopLevelItem(item);
    }
}


void ReteWidget::resetLayout()
{
    // collect all visible nodes and edges, as we only want to take those
    // into consideration during layouting.
    //

    std::vector<GraphNodeItem*> nodes;
    std::vector<GraphEdgeItem*> edges;

    for (auto node : nodeList_)
    {
        if (node->isVisible())
        {
            nodes.push_back(node);
        }
    }

    for (auto edge : edgeList_)
    {
        if (edge->from()->isVisible() && edge->to()->isVisible())
        {
            edges.push_back(edge);
        }
    }

    //GraphvizLayout::layout(nodeList_, edgeList_);
    GraphvizLayout::layout(nodes, edges);

    QRectF boundingRect;
    for (auto node : nodeList_)
    {
        if (node->isVisible())
        {
            boundingRect = boundingRect.united(
                    node->mapToScene(
                        node->boundingRect()
                        ).boundingRect()
                    );
        }

    }

    scene_.setSceneRect(boundingRect);

    form_->graphicsView->fitInView(
        boundingRect,
        Qt::KeepAspectRatio
    );
}


}}
