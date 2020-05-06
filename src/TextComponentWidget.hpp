#ifndef SEMPR_GUI_TEXTCOMPONENTWIDGET_HPP_
#define SEMPR_GUI_TEXTCOMPONENTWIDGET_HPP_

#include <sempr/component/TextComponent.hpp>
#include "SingleComponentWidget.hpp"
#include <QtWidgets>

namespace Ui {
    class TextComponentWidget;
}


namespace sempr { namespace gui {

/**
    A very simple widget -- just a text edit. Could be extended for syntax
    highlighting later on.
*/
class TextComponentWidget : public SingleComponentWidget<TextComponent> {
    Q_OBJECT

    Ui::TextComponentWidget* form_;

    bool updateComponentWidget(std::shared_ptr<TextComponent> text, bool isMutable) override;

protected slots:
    void save();

public:
    TextComponentWidget(QWidget* parent = nullptr);
    ~TextComponentWidget();
};

}}

#endif /* include guard: SEMPR_GUI_TEXTCOMPONENTWIDGET_HPP_ */
