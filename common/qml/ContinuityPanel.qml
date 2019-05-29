import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.1

Rectangle
{
    id: rectangle2
    width: 200
    height: 320

    color: "#434651"
    border.width: 0

    Rectangle {
        width: 200
        height: 300

        id: rectangle1
        color: "#434651"
        radius: 3
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter
        border.width: 0


        Grid {
            id: grid2
            x: 160
            width: 200
            spacing: 2
            flow: Grid.LeftToRight
            layoutDirection: Qt.LeftToRight
            columns: 1
            anchors.fill: parent

            Item
            {
                width: 5;
                height: 20
            }

            Item
            {
                id: item1
                width: 200
                height: 33
                Image {
                    id: image1
                    x: 0
                    y: 0
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.verticalCenter: parent.verticalCenter
                    source: "images/continuity_label_48x7.png"
                }

                ToolButton {
                    id: tanleButton
                    iconSource: "images/table_13_18.png"
                    checkable: true
                    checked: false
                    width: 20
                    height: 17
                    anchors.right: parent.right
                    anchors.rightMargin: 12
                    anchors.verticalCenter: parent.verticalCenter

                    onCheckedChanged:
                    {
                        if (checked)
                        {
                            continuityControl.state = "ROW"
                            continuityControl1.state = "ROW"
                            continuityControl2.state = "ROW"
                            continuityControl3.state = "ROW"
                            continuityControl4.state = "ROW"
                        } else {
                            continuityControl.state = "SLIDER"
                            continuityControl1.state = "SLIDER"
                            continuityControl2.state = "SLIDER"
                            continuityControl3.state = "SLIDER"
                            continuityControl4.state = "SLIDER"
                        }
                    }
                }

            }

            ContinuityControl {
                id: continuityControl
                x: 121
                y: 335
                width: 200
                height: 33
            }

            ContinuityControl {
                id: continuityControl1
                x: 89
                y: 329
                width: 200
                height: 33
            }

            ContinuityControl {
                id: continuityControl2
                x: 75
                y: 393
                width: 200
                height: 33
            }

            ContinuityControl {
                id: continuityControl3
                x: 66
                y: 406
                width: 200
                height: 33
            }

            ContinuityControl {
                id: continuityControl4
                x: 94
                y: 438
                width: 200
                height: 33
            }

        }
    }
}
