#include "SPARQLItem.hpp"

namespace sempr { namespace gui {

SPARQLItem::SPARQLItem()
{
    this->setEditable(false);
}

size_t SPARQLItem::variableCount() const
{
    if (query_.results.size() > 0)
    {
        return query_.results[0].size();
    }

    return 0;
}

QString SPARQLItem::variableName(size_t num) const
{
    if (query_.results.size() > num)
    {
        auto index = query_.results[0].begin();
        for (size_t i = 0; i < num; i++) index++;

        return QString::fromStdString(index->first);
    }
    return "";
}

void SPARQLItem::update(sempr::SPARQLQuery query)
{
    query_ = query;

    this->clear();

    // set number of results and the query string as the text of this item
    this->setText(
        QString().sprintf("[%3zu] %s", query.results.size(), query.query.c_str())
    );
    this->setData(QString::fromStdString(query.query));

    // set the new column count
    if (!query.results.empty()) this->setColumnCount(query.results[0].size());

    // insert the rows
    for (auto& result : query.results)
    {
        // one row per result
        QList<QStandardItem*> row;
        for (auto& binding : result)
        {
            // and a column per binding
            auto item = new QStandardItem();
            item->setText(QString::fromStdString(binding.second.second));
            item->setData(QVariant::fromValue(binding.second.first));
            row.append(item);
        }

        this->appendRow(row);
    }

    this->emitDataChanged();
}


void SPARQLItem::clear()
{
    // remove all children
    while (rowCount() > 0)
    {
        auto row = this->takeRow(0);
        for (auto item : row)
        {
            delete item;
        }
    }

    this->setText("");
    this->emitDataChanged();
}

}}
