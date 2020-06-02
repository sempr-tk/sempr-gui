#include "ExplanationWidget.hpp"
#include "../ui/ui_explanationwidget.h"

#include "ExplanationEdgeItem.hpp"

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
    scene_.clear();

    graph_ = graph;

    for (auto node : graph_.nodes)
    {
        auto item = new ExplanationNodeItem(QString::fromStdString(node.str));
        nodes_[node.id] = item;
        scene_.addItem(item);
        item->setFlag(QGraphicsItem::ItemIsMovable);
    }

    for (auto edge : graph_.edges)
    {
        auto from = nodes_[std::get<0>(edge)];
        auto to = nodes_[std::get<1>(edge)];
        std::cout << from << ", " << to << std::endl;
        auto edgeItem = new ExplanationEdgeItem(from, to);
        edgeItem->adjust();
        scene_.addItem(edgeItem);
    }
}


}}
