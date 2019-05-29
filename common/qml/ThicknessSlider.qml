import QtQuick 2.0


Item {
    id: thicknessSlider
    width: 200
    height: 24

    property alias baseValue: buttonBase.value;
    property alias thickValue: buttonThick.value;

    property double minValue: -1;
    property double maxValue: -1;

    property bool inverted: false;

    property string rulerColor: "#FF000000"


    onRulerColorChanged:
    {
        canvas.requestPaint();
    }

    signal editingStarted();
    signal editingFinished();

    onMinValueChanged:
    {
        buttonBase.minValue = minValue;;
        buttonThick.minValue = minValue;
        canvas.requestPaint();
    }

    onMaxValueChanged:
    {
        buttonBase.maxValue = maxValue;
        buttonThick.maxValue = maxValue;
        canvas.requestPaint();
    }


    Rectangle
    {
        color: "#3a3d48"
        anchors.fill: parent
        radius: height/2
        Rectangle
        {
            color: "#444751"
            height: 2
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: parent.height/2
            anchors.right: parent.right
            anchors.rightMargin: parent.height/2
        }
    }

    Canvas
    {
        id: canvas;
        anchors.fill: parent;

        onPaint:
        {
            var ctx = getContext("2d");
            ctx.save();
            ctx.clearRect(0,0,canvas.width, canvas.height);

            var h = parent.height/4;
            var m = parent.height/2;
            var dv = maxValue - minValue;
            var dy = h/dv;

            var x0 = buttonBase.width/2;
            var x1 = buttonBase.x + buttonBase.width/2;
            var x2 = buttonThick.x + buttonThick.width/2;
            var y0 = dy*buttonThick.validateValue(baseValue);
            var y1 = dy*buttonThick.validateValue(baseValue);
            var y2 = dy*buttonThick.validateValue(thickValue);

            ctx.fillStyle = thicknessSlider.rulerColor;
            ctx.beginPath();
            ctx.moveTo(x0, m - y0);
            ctx.lineTo(x1, m - y1);
            ctx.lineTo(x2, m - y2);
            ctx.lineTo(x2, m + y2);
            ctx.lineTo(x1, m + y1);
            ctx.lineTo(x0, m + y0);
            ctx.closePath();
            ctx.fill();

            ctx.restore();
        }
    }


    SliderButton
    {
        id: buttonBase
        x: 22
        y: 4
        state: inverted ? "BLACK_CUP" : "WHITE_CUP"
        decimals: 3
        onValueChanged:
        {
            if (value > buttonThick.value)
            {
                value = buttonThick.value;
            }
            thicknessSlider.baseValue = value;
            canvas.requestPaint();
        }
        onEditStarted:
        {
            thicknessSlider.editingStarted();
        }
        onEditFinished:
        {
            thicknessSlider.editingFinished();
        }
    }

    SliderButton
    {
        id: buttonThick
        x: 130
        y: 4
        state: inverted ? "WHITE_CUP" : "BLACK_CUP"
        decimals: 3

        onValueChanged:
        {
            if (value < buttonBase.value)
            {
                buttonBase.value=value;
            }
            thicknessSlider.thickValue = validateValue(value);
            canvas.requestPaint();
        }
        onEditStarted:
        {
            thicknessSlider.editingStarted();
        }
        onEditFinished:
        {
            thicknessSlider.editingFinished();
        }
    }


}
