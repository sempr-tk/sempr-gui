#include "GeometryFilterProxyModel.hpp"
#include "CustomDataRoles.hpp"
#include "GeosQCoordinateTranform.hpp"

namespace sempr { namespace gui {

GeometryFilterProxyModel::GeometryFilterProxyModel(QObject* parent)
    : QSortFilterProxyModel(parent)
{
    /*
        From the Qt documentation:

        > Note that you should not update the source model through the proxy model when dynamicSortFilter is true.
        > [...]
        > The default value is true.
    */
    this->setDynamicSortFilter(false);
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

bool GeometryFilterProxyModel::setData(
        const QModelIndex& index,
        const QVariant& value,
        int role)
{
    auto sourceIndex = this->mapToSource(index);

    qDebug() << "index: " << index;
    qDebug() << "sourceIndex: " << sourceIndex;

    if (role == Role::CoordinatesRole)
    {
        qDebug() << value;

        auto geo = GeometryFilterProxyModel::geomPointerFromIndex(sourceIndex);
        if (!geo)
        {
            qDebug() << "no component ptr!";
            return false;
        }
        if (!geo->geometry())
        {
            qDebug() << "no geom in component!";
            return false;
        }

        qDebug() << "QVariantList? " << value.canConvert<QVariantList>();
        // qml <-> c++ provides automatic conversion between javascript arrays
        // and QVariantLists, but this method accepts only QVariants, so the
        // QVariant holds a QVariantList
        if (!value.canConvert<QVariantList>()) return false;

        // all entries in the list must be converted to QGeoCoordinate
        auto varList = value.value<QVariantList>();
        QList<QGeoCoordinate> coords;
        for (auto var : varList)
        {
            if (var.canConvert<QGeoCoordinate>())
                coords << var.value<QGeoCoordinate>();
        }

        // first, create a backup of the geometries coordinates in case things
        // go wrong
        ReadCoordinates coordReader;
        geo->geometry()->apply_ro(coordReader);

        // apply the new coordinates
        WriteCoordinates coordWriter(coords);
        geo->geometry()->apply_rw(coordWriter);
        geo->geometry()->geometryChanged();

        // check if things went wrong (geometry became invalid)
        bool stillValid = geo->geometry()->isValid();
        if (!stillValid)
        {
            // revert to the old coordinates
            QVariantList varList = coordReader.coordinates();
            QList<QGeoCoordinate> backup;
            for (auto c : varList)
            {
                backup.push_back(c.value<QGeoCoordinate>());
            }

            WriteCoordinates coordWriter(backup);
            geo->geometry()->apply_rw(coordWriter);
            return false;
        }

        // signal that the component changed
        return sourceModel()->setData(
                sourceIndex,
                QVariant::fromValue(std::static_pointer_cast<Component>(geo)),
                Role::ComponentPtrRole);
    }

    return sourceModel()->setData(sourceIndex, value, role);
}


QHash<int, QByteArray> GeometryFilterProxyModel::roleNames() const
{
    auto names = QSortFilterProxyModel::roleNames();
    names[Role::CoordinatesRole] = "coordinates";
    return names;
}

}}
