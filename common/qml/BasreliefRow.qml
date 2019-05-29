import QtQuick 2.0

Item {
    id: basreliefRow
    width: 200
    height: 60

    property alias front: editFront.value;

    property alias rise: editRise.value;

    property alias smooth: editSmooth.value;

    property alias trembling: editTrembling.value;

    property string rulerColor: "#FF000000"

    signal editingStarted;
    signal editingFinished;
    signal changesApply();

    function finishEditing()
    {
        if (editRise.isActivated())
            editRise.stopEditing();

        if (editTrembling.isActivated())
            editTrembling.stopEditing();

        if (editSmooth.isActivated())
            editSmooth.stopEditing();

        if (editFront.isActivated())
            editFront.stopEditing();
    }

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


    FocusScope
    {
        anchors.fill: parent;
    }

    Row {
        id: row
        x: 38
        y: 5
        width: 100
        height: 25
        anchors.horizontalCenter: parent.horizontalCenter
        layoutDirection: Qt.RightToLeft
        anchors.bottomMargin: 0
        anchors.topMargin: 0
        spacing: 1

        Item
        {
            width: 8
            height: 8
            anchors.verticalCenter: parent.verticalCenter
        }

        SpinEdit {
            id: editTrembling
            width: 38
            decimals: 2
            inputMask: "xxxx"
            enabled: basreliefRow.enabled;
            minValue: 0
            maxValue: 100
            incriment: 0.1
            anchors.verticalCenter: parent.verticalCenter
            toolTipText: "Trembling"
            onEditingStarted:
                basreliefRow.editingStarted();

            onEditingFinished:
                basreliefRow.editingFinished();
        }

        Image {
            id: image4
            y: 5
            source: "images/slider_button_cup_bsrlf_trembling_8x19_1.png"
        }

        SpinEdit {
            id: editRise
            y: -1
            width: 38
            inputMask: "#xxxx"
            enabled: basreliefRow.enabled;
            minValue: -10
            maxValue: 10
            incriment: 0.1
            anchors.verticalCenter: parent.verticalCenter
            toolTipText: "Rise"
            onEditingStarted:
                basreliefRow.editingStarted();

            onEditingFinished:
                basreliefRow.editingFinished();

        }
        Image {
            id: image1
            y: 5
            source: "images/slider_button_cup_bsrlf_rise_8x19_1.png"
        }
    }

    ButtonApply
    {
        x: 183
        y: 5
        anchors.right: parent.right
        anchors.rightMargin: 3
        onChangesApply:
        {
            basreliefRow.changesApply();
        }

    }

    Row {
        id: row1
        x: 32
        y: 28
        width: 100
        height: 28
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: 1
        layoutDirection: Qt.RightToLeft
        anchors.bottomMargin: 0
        anchors.topMargin: 0

        Item
        {
            width: 8
            height: 8
            anchors.verticalCenter: parent.verticalCenter
        }

        SpinEdit {
            id: editFront
            width: 38
            inputMask: "#xx"
            enabled: basreliefRow.enabled;
            minValue: -100
            maxValue: 100
            anchors.verticalCenter: parent.verticalCenter
            toolTipText: "Shift"
            onEditingStarted:
                basreliefRow.editingStarted();

            onEditingFinished:
                basreliefRow.editingFinished();
        }
        Image {
            id: image3
            y: 5
            source: "images/slider_button_cup_bsrlf_front_8x19_1.png"
        }

        SpinEdit {
            id: editSmooth
            width: 38
            inputMask: "xx"
            enabled: basreliefRow.enabled;
            minValue: 0
            maxValue: 20
            anchors.verticalCenter: parent.verticalCenter
            toolTipText: "Smooth"
            onEditingStarted:
                basreliefRow.editingStarted();

            onEditingFinished:
                basreliefRow.editingFinished();
        }
        Image {
            id: image2
            y: 5
            source: "images/slider_button_cup_bsrlf_smooth_8x19_1.png"
        }

    }


}
