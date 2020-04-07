#ifndef SEMPR_GUI_TRIPLEPROPERTYMAPWIDGET_HPP_
#define SEMPR_GUI_TRIPLEPROPERTYMAPWIDGET_HPP_

#include <QtCore>
#include <QtWidgets>

// needed ad SingleComponentWidget already implements a dynamic cast to this type
#include <sempr/component/TriplePropertyMap.hpp>

#include "../ui/ui_triplepropertymap.h"
#include "ECModel.hpp"
#include "SingleComponentWidget.hpp"

namespace sempr { namespace gui {

class TriplePropertyMapWidget : public SingleComponentWidget<TriplePropertyMap> {
    Q_OBJECT

    Ui_TriplePropertyMapWidget form_;

    bool updateComponentWidget(std::shared_ptr<TriplePropertyMap> map, bool isMutable) override;

private slots:
    // updates the model with the data from the TableWidget
    void save();

    // removes the currently selected entry from the TableWidget
    void remove();

    // adds a new entry to the TableWidget
    void add();

public:
    TriplePropertyMapWidget(QWidget* parent = nullptr);
};

}}

#endif /* include guard: SEMPR_GUI_TRIPLEPROPERTYMAPWIDGET_HPP_ */
