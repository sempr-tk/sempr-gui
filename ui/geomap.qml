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
        }
    }
}
