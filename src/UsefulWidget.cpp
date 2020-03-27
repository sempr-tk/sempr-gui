#include "UsefulWidget.hpp"


namespace sempr { namespace gui {

UsefulWidget::UsefulWidget(QWidget* parent)
    : QWidget(parent)
{
}

void UsefulWidget::setUseful(bool u)
{
    emit isUseful(this, u);
}

}}
