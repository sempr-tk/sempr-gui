#include "RoleNameProxyModel.hpp"

namespace sempr { namespace gui {

int RoleNameProxyModel::roleId(const QByteArray& name) const
{
    return this->roleNames().key(name, -1);
}

QByteArray RoleNameProxyModel::roleName(int id) const
{
    return this->roleNames().value(id, "");
}

}}
