import QtQuick 2.0
import QtLocation 5.9
import QtQml 2.2

MapItemGroup {
    id: delegate
    property var item: model

    Instantiator {
        model: item.coordinates
        delegate: MapCircle {
            center: modelData
            radius: 100.0
            color: "green"
            visible: true
            parent: delegate
        }
        onObjectAdded: {
            // MapItemGroups are buggy. They just vanish when something is changed,
            // but removing and re-adding it seems to work... :/
            map.removeMapItemGroup(delegate);
            map.addMapItemGroup(delegate);
        }
    }
}
