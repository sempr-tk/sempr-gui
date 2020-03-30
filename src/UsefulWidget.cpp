#include "UsefulWidget.hpp"


namespace sempr { namespace gui {

UsefulWidget::UsefulWidget(QWidget* parent)
    : QWidget(parent), model_(nullptr), selectionModel_(nullptr)
{
}

void UsefulWidget::setUseful(bool u)
{
    emit isUseful(this, u);
}


void UsefulWidget::onDataChanged(
        const QModelIndex& topleft, const QModelIndex& bottomright,
        const QVector<int>&)
{
    if (!currentIndex_.isValid()) return;

    // to find the current item between topleft and bottomright, they must have
    // the same parent
    if (topleft.parent() == currentIndex_.parent())
    {
        if (topleft.row() <= currentIndex_.row() &&
            bottomright.row() >= currentIndex_.row())
        {
            // -> item behind currentIndex was updated!
            updateWidget();
        }
    }
}

void UsefulWidget::onCurrentRowChanged(const QModelIndex& current, const QModelIndex&)
{
    currentIndex_ = QPersistentModelIndex(current);
    updateWidget();
}

void UsefulWidget::setModel(ECModel* model)
{
    // disconnect from previous model
    if (model_)
    {
        disconnect(model_, &ECModel::dataChanged,
                   this, &UsefulWidget::onDataChanged);
    }

    // set new model
    model_ = model;

    // connect to dataChanged signal
    if(model_)
    {
        connect(model_, &ECModel::dataChanged,
                this, &UsefulWidget::onDataChanged);
    }
}

void UsefulWidget::setSelectionModel(QItemSelectionModel* model)
{
    // disconnect from previous
    if (selectionModel_)
    {
        disconnect(selectionModel_, &QItemSelectionModel::currentRowChanged,
                   this, &UsefulWidget::onCurrentRowChanged);
    }

    // set new model
    selectionModel_ = model;

    // connect to currentRowCanged signal
    if (selectionModel_)
    {
        connect(selectionModel_, &QItemSelectionModel::currentRowChanged,
                this, &UsefulWidget::onCurrentRowChanged);
    }
}



}}
