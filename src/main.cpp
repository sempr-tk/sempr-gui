#include <iostream>
#include <QtCore>

#include "../ui/ui_main.h"
#include "ECModel.hpp"

using namespace sempr::gui;

int main(int argc, char** args)
{
    QApplication app(argc, args);
    QWidget widget;


    ECModel model;
    ECModel::ModelEntryGroup g1, g2;
    ModelEntry m11{"Entity_1", "1", {}, true, nullptr};
    ModelEntry m12{"Entity_1", "2", {}, true, nullptr};
    g1.push_back(m11);
    g1.push_back(m12);

    ModelEntry m21{"Entity_2", "3", {}, true, nullptr};
    ModelEntry m22{"Entity_2", "4", {}, true, nullptr};
    ModelEntry m23{"Entity_2", "5", {}, true, nullptr};
    g2.push_back(m21);
    g2.push_back(m22);
    g2.push_back(m23);

    model.data_.push_back(g1);
    model.data_.push_back(g2);

    Ui_Form form;
    form.setupUi(&widget);
    form.treeView->setModel(&model);
    form.listView->setModel(&model);

    form.treeView->setSelectionModel(form.listView->selectionModel());


    widget.show();
    return app.exec();
}

