import QtQml 2.2
import QtQuick 2.9
import QtLocation 5.9
import QtPositioning 5.8
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

Item {
    id: geoWidget

    // The plugin definition we want to use for the map.
    // "osm" is OpenStreetMap, which is the only one you don't need any other
    // parameters for.
    Plugin {
        id: mapPlugin
        name: "osm"
    }

    // The Map. Obviously.
    Map {
        id: map
        anchors.fill: parent

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
    } // map

    // some controls, overlaying the map:
    ColumnLayout {
        // a button to reset the view to the visible items
        Button {
            id: btnResetView
            text: "reset view"
            font.pointSize: 10

            onClicked: {
                map.fitViewportToVisibleMapItems()
                if (!map.visibleChildren)
                {
                    text: "nothing to display"
                }
            }
        }

        // a checkbox to toggle whether the vertex-circles shall be visible
        CheckBox {
            id: boxShowVertices
            text: "show vertices"
            font.pointSize: 10
            checked: true
        }

        // a checkbox to globally disable editing
        CheckBox {
            id: boxAllowEditing
            text: "allow editing"
            font.pointSize: 10
            checked: false
        }

        // a slider to control the vertex circles size
        Slider {
            id: sliderVertexSize
            from: 1
            to: 1000
            value: 100
            Text {
                text: "vertex size"
                font.pointSize: 10
                anchors.left: parent.left
                anchors.leftMargin: parent.leftPadding
                anchors.bottom: parent.bottom
            }
        }
    }
}
