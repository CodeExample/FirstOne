import QtQuick 2.0

Item {
    id: maskRow
    width: 200
    height: 24

    property alias opacityValue: editOpacity.value;
    property alias toleranceValue: editTolerance.value;

    property string rulerColor: "0xFF000000";

    signal editingStarted();
    signal editingFinished();
    signal changesApply();

    onRulerColorChanged:
    {
    }

    Row {
        id: row
        x: 0
        y: 0
        width: 110
        height: 30
        anchors.horizontalCenterOffset: 0
        layoutDirection: Qt.RightToLeft
        anchors.bottomMargin: 0
        anchors.topMargin: 0
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: 4

        Item
        {
            width: 5
            height: 7;
            anchors.verticalCenter: parent.verticalCenter
        }

        SpinEdit {
            id: editOpacity
            width: 50
            inputMask: "xxx"
            suffix: "%"
            minValue: 0
            maxValue: 100
            anchors.verticalCenter: parent.verticalCenter

            toolTipText: "Opacity"

            onEditingStarted:
                maskRow.editingStarted();

            onEditingFinished:
                maskRow.editingFinished();
        }


        Image {
            id: image2
            anchors.verticalCenter: parent.verticalCenter
            source: "images/slider_mask_opacity_8x19_1.png"
        }

        SpinEdit {
            id: editTolerance
            y: -1
            width: 50
            inputMask: "xx"
            suffix: "px"
            minValue: 0
            maxValue: 100
            anchors.verticalCenter: parent.verticalCenter

            toolTipText: "Tolerance"

            onEditingStarted:
                maskRow.editingStarted();

            onEditingFinished:
                maskRow.editingFinished();
        }

        Image {
            id: image1
            anchors.verticalCenter: parent.verticalCenter
            source: "images/slider_mask_tolerance_8x19_1.png"
        }
    }

    ButtonApply
    {
        x: 178
        y: 3
        anchors.right: parent.right
        anchors.rightMargin: 3
        onChangesApply:
        {
            maskRow.changesApply();
        }

    }

}
