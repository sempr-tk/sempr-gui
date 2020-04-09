#include "GeometryFilterProxyModel.hpp"
#include "CustomDataRoles.hpp"
#include "GeosQCoordinateTranform.hpp"

namespace sempr { namespace gui {

GeometryFilterProxyModel::GeometryFilterProxyModel(QObject* parent)
    : QSortFilterProxyModel(parent)
{
}

std::shared_ptr<GeosGeometryInterface> GeometryFilterProxyModel::geomPointerFromIndex(const QModelIndex& index)
{
    // use the ComponentPtrRole to get a Component::Ptr
    auto varPtr = index.data(Role::ComponentPtrRole);

    if (varPtr.canConvert<Component::Ptr>())
    {
        // and try to cast it to a GeosGeometryInterface::Ptr
        auto component = varPtr.value<Component::Ptr>();
        auto geo = std::dynamic_pointer_cast<GeosGeometryInterface>(component);

        return geo;
    }
    return nullptr;
}


bool GeometryFilterProxyModel::filterAcceptsRow(
        int sourceRow, const QModelIndex& sourceParent) const
{
    auto index = sourceModel()->index(sourceRow, 0, sourceParent);
    auto geo = GeometryFilterProxyModel::geomPointerFromIndex(index);

    qDebug() << "GeometryFilterProxyModel accept? " << (geo ? "yes" : "no");

    if (geo) return true;
    return false;
}

QVariant GeometryFilterProxyModel::data(const QModelIndex& index, int role) const
{
    if (role == Role::CoordinatesRole)
    {
        auto sourceIndex = this->mapToSource(index);
        auto geo = GeometryFilterProxyModel::geomPointerFromIndex(sourceIndex);
        if (geo && geo->geometry())
        {
            ReadCoordinates filter;
            geo->geometry()->apply_ro(filter);

            return filter.coordinates();
        }
    }

    return QSortFilterProxyModel::data(index, role);
}


QHash<int, QByteArray> GeometryFilterProxyModel::roleNames() const
{
    auto names = QSortFilterProxyModel::roleNames();
    names[Role::CoordinatesRole] = "coordinates";
    return names;
}

}}
