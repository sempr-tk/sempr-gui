#include "AnyColumnFilterProxyModel.hpp"

#include <QDebug>

namespace sempr { namespace gui {

bool AnyColumnFilterProxyModel::filterAcceptsRow(
        int sourceRow, const QModelIndex& sourceParent) const
{
    QString concat;
    int numColumns = sourceModel()->columnCount(sourceParent);
    for (int i = 0; i < numColumns; i++)
    {
        auto index = sourceModel()->index(sourceRow, i, sourceParent);
        concat.append(index.data().toString())
              .append(" ");
    }

    return concat.contains(filterRegExp());
}


}}
