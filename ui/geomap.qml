import QtQml 2.2
import QtQuick 2.9
import QtLocation 5.9
import QtPositioning 5.8
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

Item {
    objectName: "geoWidget"

    // The plugin definition we want to use for the map.
    // "osm" is OpenStreetMap, which is the only one you don't need any other
    // parameters for.
    Plugin {
        id: mapPlugin
        name: "osm"
    }

    // The ColumnLayout is just used to be able to add some debug stuff
    // below the map.
    ColumnLayout {
        anchors.fill: parent

        // The Map. Obviously.
        Map {
            // a small button to reset the view to the visible items
            Button {
                text: "reset view"
                onClicked: {
                    map.fitViewportToVisibleMapItems()
                    if (!map.visibleChildren)
                    {
                        text: "nothing to display"
                    }
                }
            }

            id: map
            // Layout parameters that are used by the ColumnLayout
            Layout.preferredWidth: 200
            Layout.preferredHeight: 200
            Layout.fillWidth: true
            Layout.fillHeight: true

            // specify the plugin to use
            plugin : mapPlugin
            // on startup: center the view on osnabrück
            center: QtPositioning.coordinate(52.283, 8.050)
            zoomLevel: 14

            // since MapItemView does not work with MapItemGroups, an Instantiator is used as
            // a workaround
            Instantiator {
                model: geometryModel
                delegate: Loader {
                    source:
                        // here we select what type of delegate we want
                        if (model.geometryType === "Polygon")
                            return "PolygonDelegate.qml"
                        else
                            return "CoordinateDelegate.qml"
                }

                onObjectAdded: {
                    map.addMapItemGroup(object)
                }
                onObjectRemoved: {
                    map.removeMapItemGroup(object)
                }
            }

            // This map view is responsible to display the data that is stored
            // in a model on the c++ side here in the qml map widget.
            // It creates a delegate for every data entry it gets, and this
            // delegate then manages interaction etc.
            // This MapItemView instance is only used for displaying circles --
            // it is my first attempt at creating a connection between c++ and
            // qml. Just debug stuff. There will be additional views for the
            // different types of geometries to display (as they will need
            // specific delegates), and I guess on the c++ side I'll implement
            // different filer models that just pass the corresponding object
            // types and provide the methods for manipulating the data.
            /*
            MapItemView {
                // The "circle_model" variable is not defined in the qml. It
                // is bound to the actual model in the c++ code before loading
                // this qml file. Look for a "setContextProperty" call.
                model: geometryModel

                // The definition of the delegate to use. In this (debug) case
                // all we want is circles.
                delegate: MapCircle {
                    // The "model" variable gives us access to the data of the
                    // item in the model that the current instance of the
                    // delegate handles. The properties here correspond to the
                    // roleNames as defined in the c++. So, if the c++ model
                    // returns a coordinate in data() with role = Qt::UserRole,
                    // and returns the QByteArray 'coordinate_foo' for the
                    // roleName of Qt::UseRole, then you can access it here as
                    // "model.coordinate_foo". Setting the center here once is
                    // enough, as qml creates a binding here so that the center
                    // variable is updated whenever model.coordinate_foo
                    // changes.
                    center: model.coordinates[1]
                    radius: 100.0
                    color: 'green'
                    border.width: 3

                    // MapCircles do not emit mouse events etc. So to make the
                    // circle draggable, we overlay it with a (transparent)
                    // MouseArea.
                    MouseArea {
                        id: mouseArea
                        anchors.fill: parent
                        drag.target: parent
                        drag.axis: Drag.XAxis | Drag.YAxis
                        drag.smoothed: false

                        // just for testing - the circle_model has a coordinate
                        // and a display value. The TextInput allows changing
                        // the latter.
                        TextInput {
                            text: model.json
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.horizontalCenter: parent.horizontalCenter

                            onEditingFinished: {
                                // Normally, this would be the way to handle
                                // changing the model, as it would automatically
                                // call the models setData method:
                                // model.display = text
                                // But there is a bug in the MapItemViews
                                // delegates that prevent this. The "model"
                                // variable seems to be not a real item delegate
                                // but only a QObject with some properties, as
                                // if they were copied (not referenced).
                                // So instead, we need to call
                                // circle_model.setData(index, data, role)
                                // directly:
                                var modelIndex = geometryModel.index(index, 0)
                                var role = geometryModel.roleId("json")
                                circle_model.setData(modelIndex, text, role)
                                console.log("onEditingFinished - " + text)
                            }
                        }

                        drag.onActiveChanged: {
                            if (!drag.active)
                            {
                                // dropped. update models coordinates
                                var modelIndex = circle_model.index(index, 0)
                                var role = circle_model.roleId("coordinate_foo")
                                circle_model.setData(modelIndex, center, role) //0x0100+1000)
                                console.log("drop. Set coordinate_foo to " + center)
                            }
                        }
                    }
                }
            }*/
        }
    }
}
