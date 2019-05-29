import QtQuick 2.0

Item {
    id: dashesRow
    width: 200
    height: 60

    state: "RIGHT_SHOW"

    property int blackValue: 0;
    property int whiteValue: 255;

    property string rulerColor: "0xFF000000";

    property bool inverted: false;

    property alias treshold: editTreshold.value;
    property alias dashMax: editMaxStroke.value;
    property alias dashMin: editMinStroke.value;
    property alias spaceMax: editMaxBlank.value;
    property alias spaceMin: editMinBlank.value;

    property bool edgedDashes: false;

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

    onEdgedDashesChanged:
    {
        if (edgedDashes)
        {
            editMaxBlank.visible = false;
            editMinBlank.visible = false;
            imageMaxBlank.visible = false;
            imageMinBlank.visible = false;
        } else {
            editMaxBlank.visible = true;
            editMinBlank.visible = true;
            imageMaxBlank.visible = true;
            imageMinBlank.visible = true;
        }
    }


    onRulerColorChanged:
    {
        imageMaxStroke.requestPaint();
        imageMinStroke.requestPaint();
    }

    Row {
        id: row1
        x: 22
        y: 5
        width: 150
        height: 25
        anchors.horizontalCenter: parent.horizontalCenter
        layoutDirection: Qt.RightToLeft
        anchors.bottomMargin: 0
        anchors.topMargin: 0
        spacing: 2

        Item
        {
            width: 6
            height: 20
        }

        SpinEdit {
            id: editTreshold
            y: -1
            width: 32
            inputMask: "xxx"
            minValue: 0
            maxValue: 255
            anchors.verticalCenter: parent.verticalCenter

            toolTipText: "Threshold"

            onEditingStarted:
                dashesRow.editingStarted();

            onEditingFinished:
                dashesRow.editingFinished();

        }

        Image {
            id: imageTreshold
            y: 5
            anchors.verticalCenter: parent.verticalCenter
            source: "images/slider_button_cup_bw_7x7_1.png"
        }


        SpinEdit {
            id: editMaxStroke
            width: 38
            inputMask: "x.xx"
            decimals: 2
            incriment: 0.1
            minValue: 0
            maxValue: 9.99
            anchors.verticalCenter: parent.verticalCenter

            toolTipText: "Max Stroke"

            onEditingStarted:
                dashesRow.editingStarted();

            onEditingFinished:
                dashesRow.editingFinished();
        }

        Canvas {
            id: imageMaxStroke
            x: 86
            width: 7
            height: 16
            anchors.verticalCenter: parent.verticalCenter
            onPaint:
            {
                var ctx = getContext("2d");
                ctx.save();
                ctx.clearRect(0,0, width, height);

                ctx.fillStyle = dashesRow.rulerColor;
                ctx.moveTo(3, 0);
                ctx.lineTo(3,15);
                ctx.lineTo(6,15);
                ctx.lineTo(6, 0);
                ctx.closePath();
                ctx.fill();

                ctx.restore();
            }

        }

        SpinEdit {
            id: editMinStroke
            width: 38
            inputMask: "x.xx"
            decimals: 2
            incriment: 0.1
            minValue: 0
            maxValue: 9.99
            anchors.verticalCenter: parent.verticalCenter

            toolTipText: "Min Stroke"

            onEditingStarted:
                dashesRow.editingStarted();

            onEditingFinished:
                dashesRow.editingFinished();

        }


        Canvas
        {
            id: imageMinStroke
            x: 126
            width: 7
            height: 16
            anchors.verticalCenter: parent.verticalCenter
            onPaint:
            {
                var ctx = getContext("2d");
                ctx.save();
                ctx.clearRect(0,0, width, height);

                ctx.fillStyle = dashesRow.rulerColor;
                ctx.moveTo(3, 4);
                ctx.lineTo(3,11);
                ctx.lineTo(6,11);
                ctx.lineTo(6, 4);
                ctx.closePath();
                ctx.fill();

                ctx.restore();
            }
        }
    }

    Row {
        id: row2
        x: 20
        y: 28
        width: 149
        height: 25
        anchors.horizontalCenter: parent.horizontalCenter
        layoutDirection: Qt.RightToLeft
        anchors.bottomMargin: 0
        anchors.topMargin: 0
        spacing: 2

        Item
        {
            width: 10
            height: 20
        }

        DashesGradientBox
        {
            id: gradientListBox;
            onComboClicked:
            {
                inverted = gradientListBox.itemId == 1;
                editingFinished();
            }
        }

        Item
        {
            width: 10
            height: 20
        }

        SpinEdit {
            id: editMaxBlank
            width: 38
            opacity: 1
            inputMask: "x.xx"
            decimals: 2
            incriment: 0.1
            minValue: 0
            maxValue: 9.99
            anchors.verticalCenter: parent.verticalCenter
            onEditingStarted:
                dashesRow.editingStarted();

            onEditingFinished:
                dashesRow.editingFinished();

            toolTipText: "Max Space"
        }

        Image {
            id: imageMaxBlank
            x: 236
            y: 9
            opacity: 1
            anchors.verticalCenter: parent.verticalCenter
            source: "images/dash_blank_max_7x16_1.png"
        }

        SpinEdit {
            id: editMinBlank
            width: 38
            opacity: 1
            inputMask: "x.xx"
            decimals: 2
            incriment: 0.1
            minValue: 0
            maxValue: 9.99
            anchors.verticalCenter: parent.verticalCenter

            toolTipText: "Min Space"

            onEditingStarted:
                dashesRow.editingStarted();

            onEditingFinished:
                dashesRow.editingFinished();
        }

        Image {
            id: imageMinBlank
            x: 194
            y: 9
            opacity: 1
            visible: true
            anchors.verticalCenter: parent.verticalCenter
            source: "images/dash_blank_min_7x16_1.png"
        }
    }


    ButtonApply
    {
        x: 164
        y: 5
        anchors.right: parent.right
        anchors.rightMargin: 3
        onChangesApply:
        {
            dashesRow.changesApply();
        }

    }

}
