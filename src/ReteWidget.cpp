#include "ReteWidget.hpp"
#include "../ui/ui_retewidget.h"

#include "ReteVisualSerialization.hpp"
#include "ReteNodeItem.hpp"
#include "ReteEdgeItem.hpp"

#include <QRectF>
#include <cmath>
#include <utility>

namespace sempr { namespace gui {

ReteWidget::ReteWidget(QWidget* parent)
    : QWidget(parent),
      form_(new Ui::ReteWidget)
{
    form_->setupUi(this);
    form_->graphicsView->setScene(&scene_);

    connect(form_->btnUpdate, &QPushButton::clicked,
            this, &ReteWidget::rebuild);
}

ReteWidget::~ReteWidget()
{
    delete form_;
}


void ReteWidget::setConnection(AbstractInterface::Ptr conn)
{
    sempr_ = conn;
}


void ReteWidget::rebuild()
{
    graph_ = sempr_->getReteNetworkRepresentation();

    for (auto node : graph_.nodes)
    {
        auto item = new ReteNodeItem(QString::fromStdString(node.label));
        nodes_[node.id] = item;
        scene_.addItem(item); // scene takes ownership

        item->setFlag(QGraphicsItem::ItemIsMovable);
        nodes_[node.id] = item;
    }

    for (auto edge : graph_.edges)
    {
        auto from = nodes_[edge.from];
        auto to = nodes_[edge.to];

        auto edgeItem = new ReteEdgeItem(from, to);
        edgeItem->adjust();
        scene_.addItem(edgeItem);
    }


    resetLayout();
}


void ReteWidget::resetLayout()
{
    // step 1: assign levels to the nodes
    std::map<int, std::vector<ReteNodeItem*>> levelToNodes;
    std::set<ReteNodeItem*> assignedNodes;


    // search for the root node -- the only one that is not at the end of
    // and edge:
    auto allNodes = graph_.nodes;
    for (auto edge : graph_.edges)
    {
        Node n;
        n.id = edge.to;
        allNodes.erase(n);
    }

    // from the root node, traverse the graph to assign levels
    std::vector<std::pair<int, std::string>> toVisit;
    std::set<ReteNodeItem*> visited;

    toVisit.push_back({0, allNodes.begin()->id}); // root node

    int maxLevel = 0;

    while (!toVisit.empty())
    {
        auto entry = toVisit.back(); toVisit.pop_back();
        auto node = nodes_[entry.second];

        if (visited.find(node) == visited.end())
        {
            levelToNodes[entry.first].push_back(node);
            visited.insert(node);

            // add all child nodes to be visited
            for (auto edge : graph_.edges)
            {
                if (edge.from == entry.second)
                {
                    toVisit.push_back({entry.first+1, edge.to});
                    if (entry.first+1 > maxLevel) maxLevel = entry.first+1;
                }
            }
        }
    }


    // step 2: set positions
    double y = 0;
    const double yOffset = 100;
    const double xOffset = 50;

    for (int level = 0; level <= maxLevel; level++)
    {
        // compute the overall width
        double width = 0;
        for (auto node : levelToNodes[level])
        {
            width += node->boundingRect().width() + xOffset;
        }
        width += xOffset;

        // start shifted to the left (-> align center)
        double x = -width/2.;

        // set the nodes position
        for (auto node : levelToNodes[level])
        {
            node->setPos(x, y);
            x += node->boundingRect().width() + xOffset;
        }

        y += yOffset;
    }
}


}}
