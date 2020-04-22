import QtQuick 2.0
import QtLocation 5.9

CoordinateDelegate {
    id: coordDelegate
    firstEqualsLast: true
    minCoordinates: 1

    MapQuickItem {
        id: marker
        anchorPoint.x: image.width/2
        anchorPoint.y: image.height

        coordinate: model.coordinates[0]
        zoomLevel: mymap.maximumZoomLevel

        sourceItem: Image {
            id: image
            source: (isCurrentItem ? "https://upload.wikimedia.org/wikipedia/commons/d/d1/Google_Maps_pin.svg" :
                         "https://upload.wikimedia.org/wikipedia/commons/8/88/Map_marker.svg")
            sourceSize.height: 50
            sourceSize.width: 50



            MouseArea {
                property var lastPosition: null
                anchors.fill: parent
                drag.target: marker
                drag.axis: (model.isMutable && boxAllowEditing.checked ? Drag.XAndYAxis : Drag.None)
                drag.smoothed: false

                onClicked: geoWidget.geometryDelegateClicked(coordDelegate.geometryIndex)

                drag.onActiveChanged: {
                    if (drag.active)
                    {
                        lastPosition = parent.coordinate
                    }

                    if (!drag.active)
                    {
                        // drop. update coordinate
                        var modelIndex = geometryModel.index(index, 0)
                        var role = geometryModel.roleId("coordinates")
                        var coords = model.coordinates
                        coords[0] = marker.coordinate
                        var ok = geometryModel.setData(modelIndex, coords, role)
                        if (!ok)
                        {
                            console.log("failed setting coordinates")
                            parent.path = lastPath
                        }
                    }
                }
            }
        }
    }

    MapQuickItem {
        id: textMarker
        anchorPoint.x: text.height/2
        anchorPoint.y: text.width/2

        coordinate: marker.coordinate

        visible: map.zoomLevel >= map.maximumZoomLevel-2

        sourceItem: Text {
            id: text
            anchors.fill: parent
            text: model.entityId
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }
    }
}
