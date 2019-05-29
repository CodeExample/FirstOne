import QtQuick 2.0

Rectangle {
    width: 100
    height: 62

    TextInput {
        id: textInput1
        x: 12
        y: 16
        width: 80
        height: 20
        text: qsTr("Text Input")
        font.pixelSize: 12
    }
}
