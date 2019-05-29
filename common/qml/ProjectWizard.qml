import QtQuick 2.0

Item {
    width: 800
    height: 600

    Rectangle
    {
        id: rectangle2
        color: "#434650"
        anchors.fill: parent

        Rectangle {
            id: leftButton
            width: 34
            color: "#000000"
            opacity: 0.3
            anchors.top: parent.top
            anchors.topMargin: 0
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 0
            anchors.left: parent.left
            anchors.leftMargin: 0
        }

        Rectangle {
            id: rightButton
            x: 766
            width: 34
            color: "#000000"
            opacity: 0.3
            anchors.top: parent.top
            anchors.topMargin: 0
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 0
            anchors.right: parent.right
            anchors.rightMargin: 0
        }

        ListView {
            id: listView1
            x: 0
            y: 0
            width: 800
            height: 561
            delegate: Item {
                x: 5
                width: 80
                height: 40
                Row {
                    id: row1
                    Rectangle {
                        width: 40
                        height: 40
                        color: colorCode
                    }

                    Text {
                        text: name
                        font.bold: true
                        anchors.verticalCenter: parent.verticalCenter
                    }
                    spacing: 10
                }
            }
            model: ListModel {
                ListElement {
                    name: "Grey"
                    colorCode: "grey"
                    itemText: ""
                }

                ListElement {
                    name: "Red"
                    colorCode: "red"
                    itemText: ""
                }

                ListElement {
                    name: "Blue"
                    colorCode: "blue"
                    itemText: ""
                }

                ListElement {
                    name: "Green"
                    colorCode: "green"
                    itemText: ""
                }
            }
        }
    }

}
