import QtQuick 2.0
import QtLocation 5.9

CoordinateDelegate {
    firstEqualsLast: true
    minCoordinates: 4 // 3 unique, but last == first to close the ring

    MapPolygon {
        path: model.coordinates
        color: "#60008000" // semi-transparent color
        opacity: 1

        // enable shifting of the whole polygon
        MouseArea {
            property var lastPath: null
            anchors.fill: parent
            drag.target: parent
            drag.axis: (model.isMutable && boxAllowEditing.checked ? Drag.XAndYAxis : Drag.None)
            drag.smoothed: false

            drag.onActiveChanged: {
                if (drag.active)
                {
                    // remember the old positions in order to reset on failure
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
        }
    } // MapPolygon
}
