#ifndef SEMPR_GUI_SPARQLWIDGET_HPP_
#define SEMPR_GUI_SPARQLWIDGET_HPP_

#include <QWidget>
#include <QStandardItemModel>

// the SopranoModule is used as a part of a special node, but also re-used here
// for the querying capabilities
#include <sempr/nodes/SopranoModule.hpp>
#include <sempr/component/TripleContainer.hpp> // for sempr::Triple
#include "AbstractInterface.hpp"

namespace Ui {
    class SPARQLWidget;
}


namespace sempr { namespace gui {

/**
    A widget which maintains a soprano module and allows sparql queries
*/
class SPARQLWidget : public QWidget {
    Q_OBJECT

    Ui::SPARQLWidget* form_;
    SopranoModule soprano_;
    QStandardItemModel queries_;
public:
    SPARQLWidget(QWidget* parent = nullptr);
    ~SPARQLWidget();

public slots:
    /**
        updates the internal representation
    */
    void update(sempr::Triple, AbstractInterface::Notification);

    /**
        evaluates the sparql query in the text entry
    */
    void submitClicked();

    /**
        displays the result of the clicked query item
    */
    void queryItemClicked(const QModelIndex&);

};


}}


#endif /* include guard: SEMPR_GUI_SPARQLWIDGET_HPP_ */
