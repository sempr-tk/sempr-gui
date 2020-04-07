#ifndef SEMPR_GUI_ROLENAMEPROXYMODEL_HPP_
#define SEMPR_GUI_ROLENAMEPROXYMODEL_HPP_

#include <QIdentityProxyModel>

namespace sempr { namespace gui {

/**
    This proxy only exports methods to transform a roleName into a roleId to
    the qml world.
*/
class RoleNameProxyModel : public QIdentityProxyModel {
    Q_OBJECT
public:
    Q_INVOKABLE int roleId(const QByteArray& name) const;
    Q_INVOKABLE QByteArray roleName(int id) const;

};


}}

#endif /* include guard: SEMPR_GUI_ROLENAMEPROXYMODEL_HPP_ */
