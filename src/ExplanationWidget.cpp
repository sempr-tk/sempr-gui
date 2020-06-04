#include "ExplanationWidget.hpp"
#include "../ui/ui_explanationwidget.h"

#include "GraphEdgeItem.hpp"
#include "GraphvizLayout.hpp"

namespace sempr { namespace gui {

ExplanationWidget::ExplanationWidget(QWidget* parent)
    : QWidget(parent), form_(new Ui::ExplanationWidget)
{
    form_->setupUi(this);
    form_->graphicsView->setScene(&scene_);

}

ExplanationWidget::~ExplanationWidget()
{
    delete form_;
}

void ExplanationWidget::display(const ExplanationGraph& graph)
{
    nodes_.clear();
    nodeList_.clear();
    edgeList_.clear();

    scene_.clear();

    graph_ = graph;

    for (auto node : graph_.nodes)
    {
        auto item = new GraphNodeItem(QString::fromStdString(node.str), GraphNodeItem::Rectangle);
        nodes_[node.id] = item;
        scene_.addItem(item);
        item->setFlag(QGraphicsItem::ItemIsMovable);

        nodeList_.push_back(item);
    }

    for (auto edge : graph_.edges)
    {
        auto from = nodes_[std::get<0>(edge)];
        auto to = nodes_[std::get<1>(edge)];
        auto edgeItem = new GraphEdgeItem(from, to);
        edgeItem->adjust();
        scene_.addItem(edgeItem);

        edgeList_.push_back(edgeItem);
    }

    GraphvizLayout::layout(nodeList_, edgeList_);
}


}}
