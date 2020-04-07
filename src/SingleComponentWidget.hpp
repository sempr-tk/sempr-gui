#ifndef SEMPR_GUI_SINGLECOMPONENTWIDGET_HPP_
#define SEMPR_GUI_SINGLECOMPONENTWIDGET_HPP_

#include "UsefulWidget.hpp"
#include <sempr/Component.hpp>
#include <memory>

namespace sempr { namespace gui {

/**
    The SingleComponentWidget is a UsefulWidget that is used to display and
    edit the contents of a single component, for a specific component type.
    The UsefulWidget exposes a virtual updateWidget() method, but to provide
    functionalities for a specific component type we always need to get the
    component pointer from the model and try to cast it to the specific type.
    This class provides a few convenience methods for that:
    It implements the updateWidget and sets the useful-flag itself if the
    component pointer is not valid. It then calls the updateComponentWidget
    method - which needs to be implemented by subclasses - and set itself to
    useful if that returns true.
*/
template <class T>
class SingleComponentWidget : public UsefulWidget {
protected:

    SingleComponentWidget(QWidget* parent = nullptr)
        : UsefulWidget(parent)
    {
    }

    /**
        Needs to be implemented by subclasses. The first argument is the
        component to display, the second is a flag to respect if it should be
        editable or not.
        Returns if the widget is useful and should be displayed.
    */
    virtual bool updateComponentWidget(std::shared_ptr<T> component, bool isMutable) = 0;

    /**
        Gets the Component::Ptr for the current entry and casts it to the
        specific pointer type.
    */
    std::shared_ptr<T> currentPtr()
    {
        if (!this->currentIndex_.isValid()) return nullptr;

        auto ptr = this->model_->data(this->currentIndex_, ECModel::Role::ComponentPtrRole);
        if (ptr.template canConvert<Component::Ptr>())
        {
            auto cptr = ptr.template value<Component::Ptr>();
            auto specific = std::dynamic_pointer_cast<T>(cptr);
            return specific;
        }

        return nullptr;
    }

    /**
        Tries to get the isComponentMutable flag for the current item.
        Failure returns false.
    */
    bool currentIsMutable() const
    {
        if (!this->currentIndex_.isValid()) return false;

        auto isMutable = this->model_->data(this->currentIndex_, ECModel::Role::ComponentMutableRole);
        if (isMutable.template canConvert<bool>())
        {
            return isMutable.template value<bool>();
        }
        return false;
    }

    /**
        Implements UsefulWidget::updateWidget()
    */
    void updateWidget() override
    {
        auto specific = currentPtr();
        if (specific)
        {
            bool isMutable = currentIsMutable();
            // all requirements met!
            bool useful = updateComponentWidget(specific, isMutable);
            this->setUseful(useful);
            return;
        }

        // couldn't get the specific pointer -- this widget is not useful then.
        this->setUseful(false);
    }

};

}}

#endif /* include guard: SEMPR_GUI_SINGLECOMPONENTWIDGET_HPP_ */
