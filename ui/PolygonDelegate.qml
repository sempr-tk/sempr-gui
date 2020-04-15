import QtQuick 2.0
import QtLocation 5.9

CoordinateDelegate {
    firstEqualsLast: true

    MapPolygon {
        path: model.coordinates
        color: "green"
        opacity: 0.2

        // enable shifting of the whole polygon
        MouseArea {
            property var lastPath: null
            anchors.fill: parent
            drag.target: parent
            drag.axis: (model.isMutable ? Drag.XAndYAxis : Drag.None)
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
                    if (ok)
                        console.log("set polygon coordinates to:", path)
                    else
                    {
                        console.log("failed setting coordinates")
                        parent.path = lastPath
                    }
                }
            }
        }
    } // MapPolygon
}
