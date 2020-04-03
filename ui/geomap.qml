import QtQuick 2.0
import QtLocation 5.6
import QtPositioning 5.6

Item {
    objectName: "geoWidget"

    Plugin {
        id: mapPlugin
        name: "osm"
    }

    Map {
        anchors.fill: parent
        id: map
        plugin : mapPlugin
        center: QtPositioning.coordinate(52.283, 8.050)
        zoomLevel: 14
        objectName: "myMap"

        MapItemView {
            model: circle_model
            delegate: MapCircle {
                center: model.coordinate
                radius: 100.0
                color: 'green'
                border.width: 3
            }
        }
    }
}
