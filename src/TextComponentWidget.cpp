#include "TextComponentWidget.hpp"
#include "../ui/ui_textcomponent.h"

namespace sempr { namespace gui {

TextComponentWidget::TextComponentWidget(QWidget* parent)
    : SingleComponentWidget(parent),
      form_(new Ui::TextComponentWidget)
{
    form_->setupUi(this);
    connect(form_->textEdit, &QTextEdit::textChanged,
            this, &TextComponentWidget::save);
}

TextComponentWidget::~TextComponentWidget()
{
    delete form_;
}

bool TextComponentWidget::updateComponentWidget(TextComponent::Ptr text, bool isMutable)
{
    if (text)
    {
        std::string formText = form_->textEdit->toPlainText().toStdString();

        if (text->text() != formText)
            form_->textEdit->setText(QString::fromStdString(text->text()));

        this->setEnabled(isMutable);
        return true;
    }

    return false;
}


void TextComponentWidget::save()
{
    auto text = currentPtr();
    std::string formText = form_->textEdit->toPlainText().toStdString();
    if (text && text->text() != formText)
    {
        text->setText(formText);

        model_->setData(
                currentIndex_,
                QVariant::fromValue(std::static_pointer_cast<Component>(text)),
                Role::ComponentPtrRole);
    }
}





}}
