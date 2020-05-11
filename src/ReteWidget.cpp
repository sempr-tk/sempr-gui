#include "ReteWidget.hpp"
#include "../ui/ui_retewidget.h"

#include "ReteVisualSerialization.hpp"
#include "ReteNodeItem.hpp"
#include "ReteEdgeItem.hpp"

#include <QRectF>
#include <cmath>

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
    auto graph = sempr_->getReteNetworkRepresentation();

    int numNodes = graph.nodes.size();
    int i = 0;
    for (auto node : graph.nodes)
    {
        float radius = 400;
        float x = radius * sin((float)i/numNodes * 2 * M_PI);
        float y = radius * cos((float)i/numNodes * 2 * M_PI);
        i++;

        auto item = new ReteNodeItem(QString::fromStdString(node.label));
        nodes_[node.id] = item;

        item->setPos({x, y});
        scene_.addItem(item); // scene takes ownership

        item->setFlag(QGraphicsItem::ItemIsMovable);
        nodes_[node.id] = item;
    }

    for (auto edge : graph.edges)
    {
        auto from = nodes_[edge.from];
        auto to = nodes_[edge.to];

        auto edgeItem = new ReteEdgeItem(from, to);
        edgeItem->adjust();
        scene_.addItem(edgeItem);
    }
}


}}
