#include "SemprGui.hpp"
#include "../ui/ui_main.h"

#include "DragDropTabBar.hpp"
#include "CustomDataRoles.hpp"

#include <sempr/component/GeosGeometry.hpp>

namespace sempr { namespace gui {

// helper to create geometries from wkt
GeosGeometry::Ptr fromWKT(const std::string& wkt)
{
    geos::io::WKTReader reader(geos::geom::GeometryFactory::getDefaultInstance());
    return std::make_shared<GeosGeometry>(reader.read(wkt));
}


SemprGui::SemprGui(AbstractInterface::Ptr interface)
    : dataModel_(interface), form_(new Ui_Form()), sempr_(interface)
{
    // register metatypes
    qRegisterMetaType<ModelEntry>();
    qRegisterMetaType<ECData>();

    form_->setupUi(this);

    form_->utilTabWidget_21->hide();
    form_->utilTabWidget_22->hide();
    changeWidgetLayout("1 x 2");
    form_->comboLayout->setCurrentText("1 x 2");

    // context menu for the treeview
    form_->treeView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(form_->treeView, &QTreeView::customContextMenuRequested,
            this, &SemprGui::onECCustomContextMenu);

    // explain triples
    connect(form_->tripleLiveViewWidget, &TripleLiveViewWidget::requestExplanation,
            this, &SemprGui::onExplainRequest);

    // both views use the same model
    form_->treeView->setModel(&dataModel_);
    auto selectionModel = form_->treeView->selectionModel();
    // raw json editing
    form_->tabRawComponent->setModel(&dataModel_);
    form_->tabRawComponent->setSelectionModel(selectionModel);
    // simple text component
    form_->tabTextComponent->setModel(&dataModel_);
    form_->tabTextComponent->setSelectionModel(selectionModel);
    // read-only triple container
    form_->tabTripleContainer->setModel(&dataModel_);
    form_->tabTripleContainer->setSelectionModel(selectionModel);
    // triple vector
    form_->tabTripleVector->setModel(&dataModel_);
    form_->tabTripleVector->setSelectionModel(selectionModel);
    // property maps
    form_->tabTriplePropertyMap->setModel(&dataModel_);
    form_->tabTriplePropertyMap->setSelectionModel(selectionModel);

    // setup the GeoMapWidget
    form_->geoMapWidget->setup(&dataModel_, selectionModel);

    // setup ReteWidget
    form_->reteWidget->setConnection(interface);

    // debug stuff
    connect(
        form_->debugButton, &QPushButton::clicked,
        this,
        [this]()
        {
            qDebug() << "tl: " << this->form_->geoMapWidget->mapTopLeft();
            qDebug() << "br: " << this->form_->geoMapWidget->mapBottomRight();
        }
    );


    // setup component adder
    form_->componentAdder->setModel(&dataModel_);
    form_->componentAdder->setSelectionModel(selectionModel);

    // add methods to add geometries
    form_->componentAdder->registerComponentType("Point",
        [this]()
        {
            auto center = form_->geoMapWidget->mapCenter();
            QString wkt("POINT (%1 %2)");
            wkt = wkt.arg(center.longitude())
                     .arg(center.latitude());
            return fromWKT(wkt.toStdString());
        }
    );

    form_->componentAdder->registerComponentType("Polygon",
        [this]()
        {
            auto tl = form_->geoMapWidget->mapTopLeft();
            auto br = form_->geoMapWidget->mapBottomRight();
            auto center = form_->geoMapWidget->mapCenter();

            double minLon = tl.longitude() + 1*(br.longitude() - tl.longitude())/4.;
            double maxLon = tl.longitude() + 3*(br.longitude() - tl.longitude())/4.;
            double minLat = tl.latitude() + 1*(br.latitude() - tl.latitude())/4.;
            double maxLat = tl.latitude() + 3*(br.latitude() - tl.latitude())/4.;

            QString wkt(
                "POLYGON (("
                    "%1 %2,"
                    "%3 %4,"
                    "%5 %6,"
                    "%1 %2"
                "))"
            );

            wkt = wkt.arg(minLon).arg(minLat)
                     .arg(maxLon).arg(minLat)
                     .arg(center.longitude()).arg(maxLat);
            return fromWKT(wkt.toStdString());
        }
    );

    form_->componentAdder->registerComponentType("LineString",
        [this]()
        {
            auto tl = form_->geoMapWidget->mapTopLeft();
            auto br = form_->geoMapWidget->mapBottomRight();

            double minLon = tl.longitude() + 1*(br.longitude() - tl.longitude())/4.;
            double maxLon = tl.longitude() + 3*(br.longitude() - tl.longitude())/4.;
            double minLat = tl.latitude() + 1*(br.latitude() - tl.latitude())/4.;
            double maxLat = tl.latitude() + 3*(br.latitude() - tl.latitude())/4.;

            QString wkt(
                "LINESTRING ("
                    "%1 %2,"
                    "%3 %4"
                ")"
            );

            wkt = wkt.arg(minLon).arg(minLat)
                     .arg(maxLon).arg(maxLat);
            return fromWKT(wkt.toStdString());
 
        }
    );



    // connection to hide/show tabs depending on selected component type
    connect(form_->tabRawComponent, &UsefulWidget::isUseful,
            this, &SemprGui::updateTabStatus);
    connect(form_->tabTextComponent, &UsefulWidget::isUseful,
            this, &SemprGui::updateTabStatus);
    connect(form_->tabTripleContainer, &UsefulWidget::isUseful,
            this, &SemprGui::updateTabStatus);
    connect(form_->tabTripleVector, &UsefulWidget::isUseful,
            this, &SemprGui::updateTabStatus);
    connect(form_->tabTriplePropertyMap, &UsefulWidget::isUseful,
            this, &SemprGui::updateTabStatus);


    // connect incoming updates with the history
    connect(&dataModel_, &ECModel::gotEntryAdd, this,
            [this](const ECData& entry)
            {
                this->logUpdate(entry, "ADD");
            });

    connect(&dataModel_, &ECModel::gotEntryRemove, this,
            [this](const ECData& entry)
            {
                this->logUpdate(entry, "REM");
            });

    connect(&dataModel_, &ECModel::gotEntryUpdate, this,
            [this](const ECData& entry)
            {
                this->logUpdate(entry, "UPD");
            });

    // log errors
    connect(&dataModel_, &ECModel::error, this, &SemprGui::logError);

    // hide all tabs in the beginning, as no data is selected yet.
    updateTabStatus(form_->tabRawComponent, false);
    updateTabStatus(form_->tabTextComponent, false);
    updateTabStatus(form_->tabTripleContainer, false);
    updateTabStatus(form_->tabTriplePropertyMap, false);
    updateTabStatus(form_->tabTripleVector, false);

    connect(form_->comboLayout, &QComboBox::currentTextChanged,
            this, &SemprGui::changeWidgetLayout);

    // connect the reset and commit buttons to the model
    connect(form_->btnReset, &QPushButton::clicked,
            &dataModel_, &ECModel::reset);
    connect(form_->btnCommit, &QPushButton::clicked,
            &dataModel_, &ECModel::commit);

    // initialize the triple live widget and the sparql widget
    auto triples = interface->listTriples();
    for (auto& triple : triples)
    {
        form_->tripleLiveViewWidget->tripleUpdate(triple, AbstractInterface::Notification::ADDED);
        form_->sparqlWidget->update(triple, AbstractInterface::Notification::ADDED);
    }

    interface->setTripleUpdateCallback(
        [this](sempr::Triple triple, AbstractInterface::Notification action) -> void
        {
            this->form_->tripleLiveViewWidget->tripleUpdate(triple, action);
            this->form_->sparqlWidget->update(triple, action);
        }
    );

    interface->setLoggingCallback(
        [this](LogData data) -> void
        {
            this->addLogData(data);
        }
    );

    connect(form_->errorList, &QTreeWidget::itemDoubleClicked,
            this, &SemprGui::displayLogDataEntry);
}


SemprGui::~SemprGui()
{
    delete form_;
}


void SemprGui::logUpdate(const ECData& entry, const QString& mod)
{
    QTreeWidgetItem* item = new QTreeWidgetItem();

    item->setText(0, mod);
    item->setText(1, QString::fromStdString(entry.entityId));
    item->setText(2, QString::fromStdString(entry.componentId));

    form_->historyList->insertTopLevelItem(0, item);
}

void SemprGui::logError(const QString& what)
{
    int num = form_->errorList->topLevelItemCount();
    QTreeWidgetItem* item = new QTreeWidgetItem();
    item->setText(0, QString::number(num));
    item->setText(1, what);
    form_->errorList->insertTopLevelItem(0, item);
}


void SemprGui::addLogData(LogData log)
{
    auto sinceEpoch = log.timestamp.time_since_epoch();
    auto sinceEpochSeconds =
        std::chrono::duration_cast<std::chrono::seconds>(sinceEpoch);

    QTreeWidgetItem* item = new QTreeWidgetItem();
    item->setData(0, Qt::DisplayRole,
                  QDateTime::fromSecsSinceEpoch(sinceEpochSeconds.count()));
    item->setData(0, Qt::UserRole,
                  QVariant::fromValue(static_cast<size_t>(log.level)));

    item->setText(1, QString::fromStdString(log.name));
    item->setText(2, QString::fromStdString(log.message));
    item->setToolTip(2, QString::fromStdString(log.message));

    QFontMetrics metrics(QApplication::font());
    item->setSizeHint(2, metrics.boundingRect(item->text(2)).size());

    switch (log.level) {
        case LogData::ERROR:
            item->setBackgroundColor(0, Qt::red);
            break;
        case LogData::WARNING:
            item->setBackgroundColor(0, Qt::yellow);
            break;
        case LogData::INFO:
            item->setBackgroundColor(0, Qt::blue);
            break;
        case LogData::DEBUG:
            item->setBackgroundColor(0, Qt::lightGray);
            break;
    }

    form_->errorList->insertTopLevelItem(0, item);
}

void SemprGui::displayLogDataEntry(QTreeWidgetItem* item, int /*column*/)
{
    QMessageBox popup(this);
    LogData::Level level =
        static_cast<LogData::Level>(item->data(0, Qt::UserRole).value<size_t>());

    switch (level) {
        case LogData::ERROR:
            popup.setIcon(QMessageBox::Icon::Critical);
            break;
        case LogData::WARNING:
            popup.setIcon(QMessageBox::Icon::Warning);
            break;
        case LogData::INFO:
            popup.setIcon(QMessageBox::Icon::Information);
            break;
        case LogData::DEBUG:
            break;
    }

    popup.setWindowTitle(item->text(1));
    popup.setText(item->text(2));
    popup.setStandardButtons(QMessageBox::Close);
    popup.setFont(this->font());
    popup.exec();
}

void SemprGui::updateTabStatus(UsefulWidget* widget, bool visible)
{
    if (visible)
    {
        int index = form_->tabWidget->indexOf(widget);
        if (index == -1)
        {
            int index = form_->tabWidget->addTab(widget, widget->windowTitle());
            form_->tabWidget->setCurrentIndex(index);
        }
    }
    else
    {
        int index = form_->tabWidget->indexOf(widget);
        if (index != -1)
        {
            form_->tabWidget->removeTab(index);
        }
    }
}


void SemprGui::changeWidgetLayout(const QString& layout)
{
    if (layout == "1 x 1")
    {
        form_->utilTabWidget_12->hide();
        form_->utilTabWidget_21->hide();
        form_->utilTabWidget_22->hide();

        emptyUtilTabWidget(form_->utilTabWidget_12);
        emptyUtilTabWidget(form_->utilTabWidget_21);
        emptyUtilTabWidget(form_->utilTabWidget_22);
    }
    else if (layout == "1 x 2")
    {
        form_->utilTabWidget_12->show();
        form_->utilTabWidget_21->hide();
        form_->utilTabWidget_22->hide();

        emptyUtilTabWidget(form_->utilTabWidget_21);
        emptyUtilTabWidget(form_->utilTabWidget_22);
    }
    else if (layout == "2 x 1")
    {
        form_->utilTabWidget_21->show();
        form_->utilTabWidget_12->hide();
        form_->utilTabWidget_22->hide();

        emptyUtilTabWidget(form_->utilTabWidget_12);
        emptyUtilTabWidget(form_->utilTabWidget_22);
    }
    else if (layout == "2 x 2")
    {
        form_->utilTabWidget_12->show();
        form_->utilTabWidget_21->show();
        form_->utilTabWidget_22->show();
    }
}



void SemprGui::emptyUtilTabWidget(QTabWidget* widget)
{
    QTabWidget* target = nullptr;
    if      (widget == form_->utilTabWidget_12) target = form_->utilTabWidget_11;
    else if (widget == form_->utilTabWidget_21) target = form_->utilTabWidget_11;
    else if (widget == form_->utilTabWidget_22)
    {
        if      (form_->utilTabWidget_12->isVisible()) target = form_->utilTabWidget_12;
        else if (form_->utilTabWidget_21->isVisible()) target = form_->utilTabWidget_21;
        else                                           target = form_->utilTabWidget_11;
    }

    if (target)
    {
        int numTabs = widget->count();
        for (int i = numTabs-1; i >= 0; i--)
        {
            auto tab = widget->widget(i);
            auto label = widget->tabText(i);

            widget->removeTab(i);
            target->addTab(tab, label);
        }
    }
}


void SemprGui::onECCustomContextMenu(const QPoint& point)
{
    QModelIndex index = form_->treeView->indexAt(point);
    if (index.isValid() && index.parent().isValid())
    {
        QMenu menu;
        auto actionExplain = menu.addAction("explain");
        auto selectedAction = menu.exec(form_->treeView->viewport()->mapToGlobal(point));

        if (selectedAction == actionExplain)
        {
            auto entityId = index.data(Role::EntityIdRole).toString();
            auto componentId = index.data(Role::ComponentIdRole).toString();

            ECData data;
            data.entityId = entityId.toStdString();
            data.componentId = componentId.toStdString();
            auto graph = sempr_->getExplanation(data);

            form_->explanationWidget->display(graph);
            for (auto tabWidget : { form_->utilTabWidget_11,
                                    form_->utilTabWidget_12,
                                    form_->utilTabWidget_21,
                                    form_->utilTabWidget_22 })
            {
                int index = tabWidget->indexOf(form_->explanationWidget);
                if (index != -1)
                {
                    tabWidget->setCurrentIndex(index);
                    break;
                }
            }
        }
    }
}

void SemprGui::onExplainRequest(const QString& s, const QString& p, const QString& o)
{
    auto triple = std::make_shared<sempr::Triple>(s.toStdString(),
                                                  p.toStdString(),
                                                  o.toStdString());
    auto graph = sempr_->getExplanation(triple);
    form_->explanationWidget->display(graph);
    for (auto tabWidget : { form_->utilTabWidget_11,
                            form_->utilTabWidget_12,
                            form_->utilTabWidget_21,
                            form_->utilTabWidget_22 })
    {
        int index = tabWidget->indexOf(form_->explanationWidget);
        if (index != -1)
        {
            tabWidget->setCurrentIndex(index);
            break;
        }
    }
}


}}
