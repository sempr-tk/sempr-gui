import QtQuick 2.0
import QtLocation 5.9

CoordinateDelegate {
    id: coordDelegate
    minCoordinates: 2

    MapPolyline {
        id: lineString
        path: model.coordinates
        line.color: (isCurrentItem ? "#60808000" : "#60008000") // semi-transparent color
        line.width: 3

        // enable shifting of the whole linestring
        MouseArea {
            property var lastPath: null
            anchors.fill: parent
            drag.target: parent
            drag.axis: (model.isMutable && boxAllowEditing.checked ? Drag.XAndYAxis : Drag.None)
            drag.smoothed: false

            onClicked: geoWidget.geometryDelegateClicked(coordDelegate.geometryIndex)

            drag.onActiveChanged: {
                if (drag.active)
                {
                    // remember old positions in order to reset on failure
                    lastPath = parent.path
                }

                if (!drag.active)
                {
                    // drop. update coordinates
                    var modelIndex = geometryModel.index(index, 0)
                    var role = geometryModel.roleId("coordinates")
                    var ok = geometryModel.setData(modelIndex, parent.path, role)
                    if (!ok)
                    {
                        console.log("failed setting coordinates")
                        parent.path = lastPath
                    }
                }
            }

            Text {
                anchors.fill: parent
                text: model.entityId
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter

                visible: lineString.width^2 + lineString.height^2 > 400
            }
        }
    }
}
