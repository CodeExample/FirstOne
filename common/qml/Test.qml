import QtQuick 2.0

Rectangle {
    id: box
    width: 100
    height: 62

    Rectangle {
        id: rectangle1
        x: 20
        y: 12
        width: 61
        height: 38
        color: "#ffffff"
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
    }
}
