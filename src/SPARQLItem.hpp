#ifndef SEMPR_GUI_SPARQLITEM_HPP_
#define SEMPR_GUI_SPARQLITEM_HPP_

#include <sempr/nodes/SopranoModule.hpp>
#include <QStandardItem>

namespace sempr { namespace gui {

/**
    The SPARQLItem is a container for some information about a single
    sparql query. It is a QStandardItem that populates itself (and its children)
    from a SPARQLQuery.
*/
class SPARQLItem : public QStandardItem {
    sempr::SPARQLQuery query_;

public:
    SPARQLItem();
    void update(sempr::SPARQLQuery);
    void clear();

    size_t variableCount() const;
    QString variableName(size_t num) const;
};

}}


Q_DECLARE_METATYPE(sempr::SPARQLQuery::ValueType)

#endif /* include guard: SEMPR_GUI_SPARQLITEM_HPP_ */
