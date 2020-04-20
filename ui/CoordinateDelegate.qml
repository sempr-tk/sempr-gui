import QtQuick 2.0
import QtLocation 5.9
import QtPositioning 5.8
import QtQml 2.2

MapItemGroup {
    id: coordDelegate
    property var item: model
    property var geometryIndex: index

    // if set to true the last coordinate will also modify the first, and vice versa
    property bool firstEqualsLast: false
    // prevents removing too many coordinates
    property int minCoordinates: 2


    Component.onDestruction: {
        console.log("Destroyed MapItemGroup")
    }

    // create the handle that allows us to modify the existing coordinate
    Instantiator {
        id: coordInstantiator
        model: item.coordinates
        delegate: MapCircle {
            center: modelData
            radius: sliderVertexSize.value
            color: "green"
            opacity: 0.5
            visible: boxShowVertices.checked
            parent: coordDelegate
            MouseArea {
                anchors.fill: parent
                acceptedButtons: Qt.LeftButton
                drag.target: parent
                drag.axis: (item.isMutable && boxAllowEditing.checked ? Drag.XAndYAxis : Drag.None)
                drag.smoothed: false

                drag.onActiveChanged: {
                    if (!drag.active)
                    {
                        // drop. update coordinate
                        var modelIndex = geometryModel.index(coordDelegate.geometryIndex, 0)
                        var role = geometryModel.roleId("coordinates")

                        // copy
                        var coords = item.coordinates
                        // change
                        coords[index] = parent.center

                        if (firstEqualsLast && (index == 0 || index == coordInstantiator.count-1))
                        {
                            // edge case, also update the other index
                            var otherIndex = (coordInstantiator.count-1) - index
                            coords[otherIndex] = coords[index]
                        }
                        var ok = geometryModel.setData(modelIndex, coords, role)

                        if (!ok)
                        {
                            console.log("failed changing coordinate")
                            parent.center = modelData
                        }
                    }
                }

                // extra mouse area, to avoid dragging on middle button
                MouseArea {
                    anchors.fill: parent
                    acceptedButtons: Qt.MiddleButton
                    // handle clicks on coordinates -- if there are enough, allow removing them
                    onClicked: function (mouse){
                        if (mouse.button === Qt.MiddleButton && // the middle button triggered the event
                            mouse.buttons === Qt.NoButton && // no buttons are currently held (release!)
                            item.isMutable && // the geometry is allowed to be changed
                            boxAllowEditing.checked && // editing is enabled in the gui
                            coordInstantiator.count > minCoordinates) // there are more than enough coord
                        {
                            var coords = item.coordinates
                            coords.splice(index, 1)
                            if (firstEqualsLast)
                            {
                                if (index === 0)
                                {
                                    // there is a new first, and the last must be a duplicate of
                                    // the first
                                    coords[coords.length-1] = coords[0]
                                }
                                else if (index === coords.length)
                                {
                                    // there is a new last, and the first must be a duplicate of
                                    // the last
                                    coords[0] = coords[coords.length-1]
                                }
                            }

                            //item.coordinates = coords
                            var modelIndex = geometryModel.index(coordDelegate.geometryIndex, 0)
                            var role = geometryModel.roleId("coordinates")
                            var ok = geometryModel.setData(modelIndex, coords, role)
                            if (!ok) console.log("failed to remove vertex")
                        }
                    }
                }
            }
        }

        onObjectAdded: {
            // MapItemGroups are buggy. They just vanish when something is changed,
            // but removing and re-adding it seems to work... :/
            map.removeMapItemGroup(coordDelegate);
            map.addMapItemGroup(coordDelegate);
        }

    }


    // also, create handles in-between that allow us to add more coordinates
    Instantiator {
        id: coordAddingInstantiator
        model: item.coordinates
        delegate: Loader {
            // need to explicitely define the variables that shall be passed on to the
            // loaded component.
            property int myIndex: index
            property bool isMutable: coordDelegate.item.isMutable
            property var pos1: coordDelegate.item.coordinates[index]
            property var pos2: (index+1 < coordDelegate.item.coordinates.length ? coordDelegate.item.coordinates[index+1] : null)
            // only load something when for the first n-1 coordinates.
            // the index >= 0 check *should* not be necessary, but the loader somehow starts at
            // index = -1 ... o.O
            sourceComponent: (index >= 0 && index+1 < coordDelegate.item.coordinates.length ? inBetweenDelegate : null)
        }
    }


    // definition of the handler for in-between coordinates
    Component {
        id: inBetweenDelegate
        MapCircle {
            id: inBetweenCircle
            // center is set in onCompleted
            radius: 0.5 * sliderVertexSize.value
            color: "blue"
            opacity: 0.2
            visible: boxShowVertices.checked && isMutable && boxAllowEditing.checked
            parent: coordDelegate

            // Break the binding to the coordinates. The map items are re-constructed anyway
            // when the coordinates are changed, and this avoids some errors while trying to
            // access "distanceTo of undefined" in the process
            Component.onCompleted: resetCenter()

            function resetCenter() {
                // center on half the way from the first to the second point
                var pos = pos1.atDistanceAndAzimuth(0.5*pos1.distanceTo(pos2), pos1.azimuthTo(pos2))
                center = pos
            }

            // A mouse area to allow dragging the circle, to insert a new coordinate
            MouseArea {
                anchors.fill: parent
                drag.target: parent
                drag.axis: Drag.XAndYAxis
                drag.smoothed: false

                drag.onActiveChanged: {
                    if (!drag.active)
                    {
                        // released -> insert coordinate.
                        // therefore: copy
                        var coordinates = model.coordinates
                        // insert a new one after our index
                        coordinates.splice(myIndex+1, 0, parent.center)
                        // set new coordinates
                        //model.coordinates = coordinates // this should work, but does not. (bug in qt)
                        var modelIndex = geometryModel.index(geometryIndex, 0)
                        var role = geometryModel.roleId("coordinates")
                        var ok = geometryModel.setData(modelIndex, coordinates, role)
                        if (!ok)
                        {
                            console.log("failed adding coordinate")
                            // restore old position
                            parent.resetCenter()
                        }
                    }
                }
            } // MouseArea
        } // MapCircle
    } // Component
}
