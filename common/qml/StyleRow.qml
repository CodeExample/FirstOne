import QtQuick 2.0

Item {
    id: styleRow
    width: 200
    height: 60

    property alias terminalNodes: editTerminalNodes.value;
    property alias terminalNodesMax: editTerminalNodes.maxValue;
    property alias smoothThickness: editSmoothNodes.value;
    property int thicknessCurvature: 0;
    property int curveCup: 0;
    property int thickCup: 0;
    property string rulerColor: "0xFF000000";

    signal editingStarted;
    signal editingFinished;
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

    Row {
        id: row
        x: 37
        y: 5
        width: 107
        height: 25
        anchors.horizontalCenter: parent.horizontalCenter
        layoutDirection: Qt.RightToLeft
        anchors.bottomMargin: 0
        anchors.topMargin: 0
        spacing: 0


        Item {
            id: item2
            y: 0
            width: 4
            height: 20
        }

        SpinEdit
        {
            id: editSmoothNodes
            width: 40
            inputMask: "xxx"
            anchors.verticalCenter: parent.verticalCenter
            minValue: 0

            toolTipText: "Smooth"

            onEditingStarted:
            {
                styleRow.editingStarted();
            }

            onEditingFinished:
            {
                styleRow.editingFinished();
            }
        }


        Image {
            id: image2
            y: 5
            source: "images/slider_button_style_smooth_8x19_1.png"
        }

        SpinEdit
        {
            id: editTerminalNodes
            y: -1
            width: 60
            //inputMask: "xx.xx"
            inputMask: "x.xx"
            decimals: 2
            incriment: 0.1
            minValue: 0
            suffix: "mm"

            anchors.verticalCenter: parent.verticalCenter
            toolTipText: "Terminal length"
            onEditingStarted:
            {
                styleRow.editingStarted();
            }

            onEditingFinished:
            {
                styleRow.editingFinished();
            }
        }

        Image {
            id: image1
            y: 5
            source: "images/slider_button_style_terminal_8x19_1.png"
        }
    }

    Row {
        id: row1
        x: 50
        y: 29
        width: 99
        height: 24
        anchors.horizontalCenter: parent.horizontalCenter
        layoutDirection: Qt.RightToLeft

        ItemsListBox
        {
            id: itemsListBox
            model: curvsModel
            anchors.verticalCenter: parent.verticalCenter
            selectedIndex: styleRow.thicknessCurvature;

            ListModel {
                id: curvsModel

                ListElement {
                    itemId: 0
                    icon: "images/curv_norm_26x13_1.png"
                    itemText: ""
                }
                ListElement {
                    itemId: 1
                    icon: "images/curv_sqr_26x13_1.png"
                    itemText: ""
                }
                ListElement {
                    itemId: 2
                    icon: "images/curv_sqrt_26x13_1.png"
                    itemText: ""
                }
            }

            onComboClicked:
            {
                thicknessCurvature = itemsListBox.itemId;
                styleRow.editingFinished();
            }
        }

        ItemsListBox
        {
            id: curveCupListBox
            model: curveCupModel
            anchors.verticalCenter: parent.verticalCenter
            selectedIndex: styleRow.curveCup;

            ListModel {
                id: curveCupModel

                ListElement {
                    itemId: 0
                    icon: "images/style_quad_cup.png"
                    itemText: ""
                }
                ListElement {
                    itemId: 1
                    icon: "images/style_round_cup.png"
                    itemText: ""
                }
            }

            onComboClicked:
            {
                curveCup = curveCupListBox.itemId;
                styleRow.editingFinished();
            }
        }

        ItemsListBox
        {
            id: thickCompListBox
            model: thickCompModel
            anchors.verticalCenter: parent.verticalCenter
            selectedIndex: styleRow.thickCup;

            ListModel {
                id: thickCompModel

                ListElement {
                    itemId: 0
                    icon: "images/style_null_think_cup.png"
                    itemText: ""
                }
                ListElement {
                    itemId: 1
                    icon: "images/style_compensate_think_cup.png"
                    itemText: ""
                }
            }

            onComboClicked:
            {
                thickCup = thickCompListBox.itemId;
                styleRow.editingFinished();
            }
        }
    }

    ButtonApply
    {
        x: 182
        y: 5
        anchors.right: parent.right
        anchors.rightMargin: 3
        onChangesApply:
        {
            styleRow.changesApply();
        }
    }

}
