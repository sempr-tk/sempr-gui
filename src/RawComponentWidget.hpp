#ifndef SEMPR_GUI_RAWCOMPONENTWIDGET_HPP_
#define SEMPR_GUI_RAWCOMPONENTWIDGET_HPP_

#include <QtCore>
#include <QtWidgets>

#include "UsefulWidget.hpp"

namespace Ui {
    class RawComponent;
}

namespace sempr { namespace gui {

class RawComponentWidget : public UsefulWidget {
    Q_OBJECT

    Ui::RawComponent* form_;

    /// Uses the currentIndex_ to access the data from the model and display it
    void updateWidget() override;

private slots:
    // saves the content of the edit field in the currently selected component
    void save();

public:
    RawComponentWidget(QWidget* parent = nullptr);
    ~RawComponentWidget();

};

}}

#endif /* include guard: SEMPR_GUI_RAWCOMPONENTWIDGET_HPP_ */
