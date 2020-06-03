#ifndef SEMPR_GUI_EXPLANATIONWIDGET_HPP_
#define SEMPR_GUI_EXPLANATIONWIDGET_HPP_

#include <QWidget>
#include <QGraphicsScene>

#include "ExplanationNode.hpp"
#include "GraphNodeItem.hpp"

namespace Ui {
    class ExplanationWidget;
}


namespace sempr { namespace  gui {


class ExplanationWidget : public QWidget {
    Ui::ExplanationWidget* form_;

    QGraphicsScene scene_;
    std::map<std::string, GraphNodeItem*> nodes_;
    ExplanationGraph graph_;

public:
    ExplanationWidget(QWidget* parent = nullptr);
    ~ExplanationWidget();

    void display(const ExplanationGraph& graph);
};


}}

#endif /* include guard: SEMPR_GUI_EXPLANATIONWIDGET_HPP_ */
