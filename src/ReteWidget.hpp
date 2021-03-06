#ifndef SEMPR_GUI_RETEWIDGET_HPP_
#define SEMPR_GUI_RETEWIDGET_HPP_

#include <QtWidgets>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>
#include <QGraphicsRectItem>
#include <QGraphicsLineItem>

#include <map>

#include "AbstractInterface.hpp"
#include "GraphNodeItem.hpp"

namespace Ui {
    class ReteWidget;
}


namespace sempr { namespace gui {

/**
    A widget with a QGraphicsView to display the rete network
*/
class ReteWidget : public QWidget {
    Q_OBJECT

    Ui::ReteWidget* form_;

    QGraphicsScene scene_;
    AbstractInterface::Ptr sempr_;

    // a list of graphics items that were added to the scene
    std::map<std::string, GraphNodeItem*> nodes_;

    // for layouting
    std::vector<GraphNodeItem*> nodeList_;
    std::vector<GraphEdgeItem*> edgeList_;

    // store the graph, non-visual, abstract representation
    Graph graph_;
    std::map<QTreeWidgetItem*, Rule> rules_;


    // timer for dynamic updates of the nodes position
    int timerId_;

    /**
        Requests the network information from the sempr core and re-builds the
        visual representation the in graphics scene
    */
    void rebuild();

    /**
        Updates the visibility of graph nodes based on the checke items in
        the rules tree widget.
    */
    void updateGraphVisibility();

    /**
        Requests the rules from the sempr core and rebuilds the tree widget
    */
    void populateTreeWidget();


    /**
        Highlights the part of the graph connected to the given node
    */
    void highlight(const std::string& id);
    void highlight(GraphNodeItem* node);
    void highlight(const std::vector<std::string>& ids);

private slots:
    // highlight selected items (clicks in graph!)
    void onSelectionChanged();

    // highlight a selected rule
    void onSelectedRuleChanged(QTreeWidgetItem* current);

    // animate graph
    void timerEvent(QTimerEvent* event) override;

public:
    ReteWidget(QWidget* parent = nullptr);
    virtual ~ReteWidget();

    /**
        Initializes the connection to sempr
    */
    void setConnection(AbstractInterface::Ptr);


    /**
        Resets the layout of the nodes
    */
    virtual void resetLayout();
};

}}

#endif /* include guard: SEMPR_GUI_RETEWIDGET_HPP_ */
