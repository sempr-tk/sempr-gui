import QtQuick 2.0
import QtLocation 5.9
import QtQml 2.2

MapItemGroup {
    id: coordDelegate
    property var item: model

    // if set to true the last coordinate will also modify the first, and vice versa
    property bool firstEqualsLast: false

    Instantiator {
        id: coordInstantiator
        model: item.coordinates
        delegate: MapCircle {
            center: modelData
            radius: 100.0
            color: "green"
            opacity: 1
            visible: true
            parent: coordDelegate
            MouseArea {
                anchors.fill: parent
                drag.target: parent
                drag.axis: (item.isMutable ? Drag.XAndYAxis : Drag.None)
                drag.smoothed: false

                drag.onActiveChanged: {
                    if (!drag.active)
                    {
                        // drop. update coordinate
                        console.log("coord index ", index, "val", center)
                        var modelIndex = geometryModel.index(coordDelegate.index, 0)
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
            }
        }
        onObjectAdded: {
            // MapItemGroups are buggy. They just vanish when something is changed,
            // but removing and re-adding it seems to work... :/
            map.removeMapItemGroup(coordDelegate);
            map.addMapItemGroup(coordDelegate);
        }
    }
}
