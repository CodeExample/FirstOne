import QtQuick 2.0

Item {
    id: thicknessRow
    width: 200
    height: 24

    property string rulerColor: "0xFF000000";

    property alias baseValue: editBase.value;
    property alias baseMinValue: editBase.minValue;
    property alias baseMaxValue: editBase.maxValue;

    property alias thickValue: editThick.value;
    property alias thickMinValue: editThick.minValue;
    property alias thickMaxValue: editThick.maxValue;

    property bool inverted: false;

    onRulerColorChanged:
    {
        canvasBase.requestPaint();
        canvasThick.requestPaint();
    }


    signal editingStarted();
    signal editingFinished();
    signal changesApply();

    onInvertedChanged:
    {
        if (inverted)
        {
            gradientListBox.selectedIndex = 1;
        } else {
            gradientListBox.selectedIndex = 0;
        }
    }


    Row {
        id: row
        x: 0
        y: 0
        width: 151
        height: 25
        anchors.horizontalCenter: parent.horizontalCenter
        layoutDirection: Qt.RightToLeft
        anchors.bottomMargin: 0
        anchors.topMargin: 0
        spacing: 0

        Item {
            id: item2
            y: 0
            width: 10
            height: 20
        }

        SpinEdit {
            id: editThick
            width: 40
            inputMask: "x.xxx"
            incriment: 0.1
            minValue: 0
            maxValue: 10
            anchors.verticalCenter: parent.verticalCenter

            toolTipText: "Thickness"

            onEditingStarted:
            {
                thicknessRow.editingStarted();
            }

            onEditingFinished:
            {
                thicknessRow.editingFinished();
            }

        }

        Canvas
        {
            id: canvasThick
            y: 4
            width: 17
            height: 19
            onPaint:
            {
                var ctx = getContext("2d");
                ctx.save();
                ctx.clearRect(0,0, width, height);

                ctx.fillStyle = thicknessRow.rulerColor;
                ctx.moveTo(4, 8);
                ctx.lineTo(10,5);
                ctx.lineTo(10,11);
                ctx.lineTo(4, 8);
                ctx.closePath();
                ctx.fill();

                ctx.restore();
            }
        }

        SpinEdit {
            id: editBase
            y: -1
            width: 40
            inputMask: "x.xxx"
            incriment: 0.1
            minValue: 0
            maxValue: 10
            anchors.verticalCenter: parent.verticalCenter

            toolTipText: "Base Thickness"

            onEditingStarted:
            {
                thicknessRow.editingStarted();
            }

            onEditingFinished:
            {
                thicknessRow.editingFinished();
            }

        }

        Canvas
        {
            id: canvasBase
            y: 4
            width: 17
            height: 19
            onPaint:
            {
                var ctx = getContext("2d");
                ctx.save();
                ctx.clearRect(0,0, width, height);

                ctx.fillStyle = thicknessRow.rulerColor;
                ctx.moveTo( 4, 5);
                ctx.lineTo(10, 5);
                ctx.lineTo(10, 11);
                ctx.lineTo( 4, 11);
                ctx.lineTo( 4, 5);
                ctx.closePath();
                ctx.fill();
                ctx.restore();
            }
        }

        ThicknessGradientBox
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
        x: 183
        y: 3
        anchors.right: parent.right
        anchors.rightMargin: 3
        onChangesApply:
        {
            thicknessRow.changesApply();
        }


    }

}
