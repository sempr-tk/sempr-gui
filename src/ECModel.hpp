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
    The ModelEntryGroup is a simple helper class to combine multiple
    ModelEntry objects with the same entity id. It also has an internal id that
    is automatically incremented with each new ModelEntryGroup, which allows us
    to add this id as an internalId to every QModelIndex to keep track of the
    corresponing parent, regardless of its current position in the vector.

    The internal id (groupId_) starts at 1, so that 0 is reserved for indices
    that do not refer to a ModelEntry, but to a first-level entry. It sounds a
    bit counter-intuitive: The ModelEntryGroups are indexed by a QModelIndex
    where the parent is invalid, the row refers to the index in the data_
    vector, and the internalId is 0. A specific ModelEntry is indexed by a
    QModelIndex where the parent is as described above, the row refers to the
    index in the entries_ vector and the internalId is the groupId_ of the
    ModelEntryGroup it is located in.
    Actually, the parent of an index is not stored in the index, but computed
    by the ECModel::parent method. And this method uses to internalId as the
    groupId to find the row of the parent of an index.

    Only to be used in ECModel.

    See e.g. ECModel::parent
*/
class ModelEntryGroup {
    static int nextId()
    {
        static size_t id = 0;
        return ++id;
    }

public:
    size_t groupId_;
    std::string entityId_;
    std::vector<ModelEntry> entries_;

    ModelEntryGroup()
        : groupId_(ModelEntryGroup::nextId())
    {
    }
    ModelEntryGroup(const ModelEntryGroup&) = default;
    ModelEntryGroup& operator = (const ModelEntryGroup&) = default;
};

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

    std::vector<ModelEntryGroup> data_;

    /// the connection to sempr
    AbstractInterface::Ptr semprInterface_;

    /// compute the model index of the entry
    QModelIndex findEntry(const ModelEntry&) const;
    QModelIndex findEntry(const std::string& entityId,
                          const std::string& componentId,
                          const std::string& tag) const;

signals:
    // These are emitted from the thread that calls the set callback in
    // semprInterface_, and are connected to the [add|remove|update]ModelEntry
    // slots below. This is done to make sure that the slots are called from
    // the main (gui) thread.
    void gotEntryAdd(const sempr::gui::ECData&);
    void gotEntryUpdate(const sempr::gui::ECData&);
    void gotEntryRemove(const sempr::gui::ECData&);

    // signal exceptions/errors, e.g. when parsing json
    void error(const QString& what);

public slots:
    /**
        Adds a model entry to the internal vector structure, created from
        the ECData given.
    */
    void addModelEntry(const ECData&);

    /**
        Removes a model entry from the local vector structure as indicated
        by the entity- and component-id in the ECData.
    */
    void removeModelEntry(const ECData&);

    /**
        Updates a model entry in the internal vector structure from the given
        ECData.
    */
    void updateModelEntry(const ECData&);

public:
    ECModel(AbstractInterface::Ptr interface);
    ~ECModel();

    Qt::ItemFlags flags(const QModelIndex& index) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    int rowCount(const QModelIndex& parent) const override;
    int columnCount(const QModelIndex& parent) const override;
    QModelIndex index(int row, int col, const QModelIndex& parent) const override;
    QModelIndex parent(const QModelIndex& index) const override;
    QHash<int, QByteArray> roleNames() const override;

public slots:
    /**
        Note: Unused so far. Rather look at
              addComponent(Component::Ptr, const std::string&).

        Adds a component to an entity as specified in the ModelEntry,
        only remotely!
        The entityId is used to find the entity where the component needs to
        be added, and the componentJSON string will be used to create the
        component. All other field are discarded.
    */
    void addComponent(const ModelEntry&);

    /**
        Adds a component to an entity directly on the remote side.
        Convenience method, creates a json representation of the component
        and fills a ModelEntry.
    */
    void addComponent(Component::Ptr component, const std::string& entityId, const std::string& tag);

    /**
        Uses the componentId and the entityId to remove the component from the
        entity, on the remote side.
    */
    void removeComponent(const ModelEntry&);

    /**
        Shortcut for removeComponent(ModelEntry) and addComponent(ModelEntry).
    */
    void updateComponent(const ModelEntry&);


    /**
        Sends all updates for all modified entries to the sempr core
    */
    void commit();

    /**
        Resets all entries to the last received state from the sempr core
    */
    void reset();
};


}}

#endif /* include guard: SEMPR_GUI_ECMODEL_HPP_ */
