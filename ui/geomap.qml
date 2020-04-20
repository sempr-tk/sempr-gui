import QtQml 2.2
import QtQuick 2.9
import QtLocation 5.9
import QtPositioning 5.8
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3

Item {
    id: geoWidget

    // to be emitted when some map delegate was clicked
    signal geometryDelegateClicked(int index)

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

    // some controls, overlaying the map, in a column layout:
    Item {
        width: 200
        id: controlsWidget
        // a button to show/hide the controls
        Button {
            id: btnShowHideControls
            text: "show controls"
            anchors.left: parent.left
            anchors.right: parent.right
            onClicked: {
                if (parent.state === "")
                    parent.state = "opened"
                else
                    parent.state = ""
            }
        }

        // a scrollview that contains all the controls,
        // and will be resized when show/hide is clicked
        ScrollView {
            id: controlsContainer
            anchors.top: btnShowHideControls.bottom
            width: parent.width
            height: 0
            clip: true

            // the controls are aligned in a column
            ColumnLayout{
                // a button to reset the view to the visible items
                Button {
                    id: btnResetView
                    text: "reset view"
                    font.pointSize: 10
                    Layout.maximumWidth: controlsContainer.width
                    implicitWidth: controlsContainer.width - 20

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
                    height: 50
                }

                // a slider to control the vertex circles size
                Slider {
                    id: sliderVertexSize
                    implicitWidth: controlsContainer.width-20
                    from: 1
                    to: 1000
                    value: 100
                    stepSize: 5
                    Text {
                        text: "vertex size"
                        font.pointSize: 10
                        anchors.left: parent.left
                        anchors.leftMargin: parent.leftPadding
                        anchors.bottom: parent.bottom
                    }
                    MouseArea {
                        anchors.fill: parent
                        acceptedButtons: Qt.NoButton
                        propagateComposedEvents: true
                        onWheel: {
                            wheel.accepted = true
                            if (wheel.angleDelta.y > 0)
                                parent.increase()
                            else
                                parent.decrease()
                        }
                    }
                }
            }
        }

        // define the closed state
        states: [
            State {
                name: "opened"
                PropertyChanges {
                    target: controlsContainer
                    height: 300
                }
                PropertyChanges {
                    target: btnShowHideControls
                    text: "hide controls"
                }
            }
        ]

        transitions: [
            Transition {
                NumberAnimation { properties: "height" }
            }
        ]
    }
}
