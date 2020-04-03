import QtQuick 2.0
import QtLocation 5.6
import QtPositioning 5.6

Item {
    Plugin {
        id: mapPlugin
        name: "osm"
    }

    Map {
        anchors.fill: parent
        id: map
        plugin : mapPlugin
        center: QtPositioning.coordinate(52.283, 8.050)
        zoomLevel: 10
    }
}
