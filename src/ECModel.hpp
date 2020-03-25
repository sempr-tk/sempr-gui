#ifndef SEMPR_GUI_ECMODEL_HPP_
#define SEMPR_GUI_ECMODEL_HPP_

#include <QAbstractItemModel>
#include <vector>
#include <map>
#include <string>

#include "ModelEntry.hpp"
#include "AbstractInterface.hpp"

namespace sempr { namespace gui {

/**
    The ECModel is an implementation of the QAbstractItemModel and provides
    access to the Entity-Component pairs stored in ModelEntry structs.
    The data is organized in a shallow tree structure, where the first level
    entries point to the goup defined by the entity id and the second level to
    the component / the ModelEntry belonging to the entity.

    The first level contains the entity ids, the second level the component
    type. There is **no** editing provided directly through this model --
    specialized views are to be used that work on the Qt::UserRole data, which
    points to the ModelEntry!

    Furthermore, the ECModel manages the access to sempr and provides methods
    to add/remove/change data remotely. Be aware that "changing" is not possible
    and components are removed and added instead (this has to do with the
    serialization/deserialization and the fact that there is not virtual
    loadFromJSON method in the components, but this could be added later).
    Also, to make sure that what you see in the gui is always what is known to
    be in the core, the updates do not happen instantly. Instead, the remote
    update is triggered which will result in a rule activation for all relevant
    entity-component pairs, which in turn calls the callback that is registered
    in the sempr-interface. Here we trigger the UI updates (i.e., we update the
    ECModel and emit corresponding signals).
*/
class ECModel : public QAbstractItemModel {
    Q_OBJECT

    typedef std::vector<ModelEntry> ModelEntryGroup;
    std::vector<ModelEntryGroup> data_;

    /// the connection to sempr
    AbstractInterface::Ptr semprInterface_;

signals:
    // These are emitted from the thread that calls the set callback in
    // semprInterface_, and are connected to the [add|remove|update]ModelEntry
    // slots below. This is done to make sure that the slots are called from
    // the main (gui) thread.
    void gotEntryAdd(const sempr::gui::ModelEntry&);
    void gotEntryUpdate(const sempr::gui::ModelEntry&);
    void gotEntryRemove(const sempr::gui::ModelEntry&);

private slots:
    /**
        Adds a model entry to the internal vector structure.
    */
    void addModelEntry(const ModelEntry&);

    /**
        Removes a model entry from the local vector structure as indicated
        by the entity- and component-id.
    */
    void removeModelEntry(const ModelEntry&);

    /**
        Updates a model entry in the internal vector structure.
    */
    void updateModelEntry(const ModelEntry&);

public:
    ECModel(AbstractInterface::Ptr interface);

    Qt::ItemFlags flags(const QModelIndex& index) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    int rowCount(const QModelIndex& parent) const override;
    int columnCount(const QModelIndex& parent) const override;
    QModelIndex index(int row, int col, const QModelIndex& parent) const override;
    QModelIndex parent(const QModelIndex& index) const override;

public slots:
    /**
        Adds a component to an entity as specified in the ModelEntry,
        only remotely!
        The entityId is used to find the entity where the component needs to
        be added, and the componentJSON string will be used to create the
        component. All other field are discarded.
    */
    void addComponent(const ModelEntry&);

    /**
        Uses the componentId and the entityId to remove the component from the
        entity, on the remote side.
    */
    void removeComponent(const ModelEntry&);

    /**
        Shortcut for removeComponent(ModelEntry) and addComponent(ModelEntry).
    */
    void updateComponent(const ModelEntry&);
};


}}

#endif /* include guard: SEMPR_GUI_ECMODEL_HPP_ */
