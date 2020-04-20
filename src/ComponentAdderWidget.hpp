#ifndef SEMPR_GUI_COMPONENTADDERWIDGET_HPP
#define SEMPR_GUI_COMPONENTADDERWIDGET_HPP

#include <QWidget>
#include <QMap>
#include <QString>

#include <sempr/Component.hpp>
#include <QItemSelectionModel>
#include "ECModel.hpp"

namespace Ui {
    class ComponentAdderWidget;
}

namespace sempr { namespace gui {

/**
    This widget provides functionalities to add new components to an existing
    or new entity. It stores a map of functions to create those components.
*/
class ComponentAdderWidget : public QWidget
{
    Q_OBJECT

protected:
    /**
        Fills the combo box with the registered component types
    */
    void rebuildComboBox();

protected slots:
    // pre-fills the manual entity id
    void onCurrentRowChanged(const QModelIndex& current, const QModelIndex& previous);

    // creates a new component and submits it to sempr
    // (-> for auto generated ids etc, do it directly!)
    void createComponent();

public:
    explicit ComponentAdderWidget(QWidget *parent = 0);
    ~ComponentAdderWidget();

    typedef std::function<sempr::Component::Ptr(void)> creatorFunction_t;


    /**
        Adds a function to create the given component type from a name by
        just default-constructing it.
    */
    template <class T>
    void registerComponentType(const QString& name)
    {
        creatorFunctions_[name] =
            []()
            {
                auto ptr = std::make_shared<T>();
                return ptr;
            };

        rebuildComboBox();
    }


    void setModel(ECModel* model);
    void setSelectionModel(QItemSelectionModel* model);

    /**
        Adds a custom creation-function
    */
    void registerComponentType(const QString& name, creatorFunction_t creator);

private:
    Ui::ComponentAdderWidget* ui_;
    QMap<QString, creatorFunction_t> creatorFunctions_;

    ECModel* model_; // to get the entity id of the currently selected item
    QItemSelectionModel* selectionModel_; // to get thte currently selected item

};

}}
#endif // SEMPR_GUI_COMPONENTADDERWIDGET_HPP
