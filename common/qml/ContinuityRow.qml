import QtQuick 2.0

Item {
    id: continuityRow
    width: 200
    height: 60

    property alias blackValue: editDown.value;
    property alias whiteValue: editUp.value;

    property bool inverted: false;
    property int breakStyle: 0;

    signal editingStarted();
    signal editingFinished();
    signal changesApply();

    Rectangle
    {
        anchors.fill: parent;
        anchors.topMargin: 2
        anchors.bottomMargin: 4
        color: "transparent"
        border.width: 2
        border.color: "#3a3c48"
        radius: 10
    }


    onInvertedChanged:
    {
        if (inverted)
        {
            gradientListBox.selectedIndex = 1;
        } else {
            gradientListBox.selectedIndex = 0;
        }
    }

    onBreakStyleChanged:
    {
        if (breakStyle < 0)
        {
            breakStyleListBox.enabled = false;
            return;
        }

        breakStyleListBox.enabled = true;
        breakStyleListBox.selectedIndex = breakStyle;
    }


    Row {
        id: row1
        x: 21
        y: 5
        width: 131
        height: 25
        anchors.horizontalCenter: parent.horizontalCenter
        layoutDirection: Qt.RightToLeft
        anchors.bottomMargin: 0
        anchors.topMargin: 0
        spacing: 0



        Item {
            id: item2
            y: 0
            width: 17
            height: 20
        }

        SpinEdit {
            id: editUp
            width: 40
            anchors.verticalCenter: parent.verticalCenter

            inputMask: "xxx"
            minValue: 0;
            maxValue: 255;

            toolTipText: "Upper"

            onEditingStarted:
            {
                continuityRow.editingStarted();
            }

            onEditingFinished:
            {
                continuityRow.editingFinished();
            }
        }


        Image {
            id: image2
            y: 5
            source: "images/slider_button_cup_white_17x19_1.png"
        }

        SpinEdit {
            id: editDown
            y: -1
            width: 40
            anchors.verticalCenter: parent.verticalCenter

            inputMask: "xxx"
            minValue: 0;
            maxValue: 255;

            toolTipText: "Lower"

            onEditingStarted:
            {
                continuityRow.editingStarted();
            }

            onEditingFinished:
            {
                continuityRow.editingFinished();
            }
        }

        Image {
            id: image1
            y: 5
            source: "images/slider_button_cup_black_17x19_1.png"
        }

    }

    Row {
        id: row2
        x: 22
        y: 28
        width: 109
        height: 24
        anchors.horizontalCenter: parent.horizontalCenter
        Item {
            x: 40
            y: 16
            width: 6
            height: 10
            anchors.verticalCenter: parent.verticalCenter
        }
        ItemsListBox
        {
            id: breakStyleListBox
            model: curvsModel
            x: 38
            y: 0
            anchors.verticalCenter: parent.verticalCenter

            ListModel {
                id: curvsModel

                ListElement {
                    itemId: 0
                    icon: "images/curv_hard_break.png"
                    itemText: ""
                }
                ListElement {
                    itemId: 1
                    icon: "images/curv_loss_null.png"
                    itemText: ""
                }
            }

            onComboClicked:
            {
                breakStyle = itemId;
                editingFinished();
            }

        }

        Item {
            x: 40
            y: 16
            width: 21
            height: 10
            anchors.verticalCenter: parent.verticalCenter
        }
        GradientListBox
        {
            id: gradientListBox;
            onComboClicked:
            {
                inverted = gradientListBox.itemId == 1;
                editingFinished();
            }
        }


    }

    ButtonApply {
        id: buttonApply1
        x: 179
        y: 5
        anchors.right: parent.right
        anchors.rightMargin: 3
        onChangesApply:
        {
            continuityRow.changesApply();
        }
    }


}
