import QtQuick 2.0

Item {
    id: continuitySlider
    width: 200
    height: 24

    property alias blackValue: buttonBlack.value;
    property alias whiteValue: buttonWhite.value;

    property string rulerColor: "#FF000000"

    onRulerColorChanged: canvas.requestPaint();

    signal editingStarted();
    signal editingFinished();

    function requestRepaint()
    {
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

            var x0 = buttonBlack.x + buttonBlack.width/2;
            var x1 = buttonWhite.x + buttonWhite.width/2;
            var y0 = parent.height/2 - 4;
            var y1 = parent.height/2 + 3;

            ctx.fillStyle = continuitySlider.rulerColor;
            ctx.fillRect(x0, y0, x1-x0, y1 - y0);
        }
    }



    SliderButton
    {
        id: buttonBlack
        x: 22
        y: 4
        state: "BLACK_CUP"
        minValue: 0
        maxValue: 255
        onValueChanged:
        {
            canvas.requestPaint();
        }
        onEditStarted:
        {
            continuitySlider.editingStarted();
        }
        onEditFinished:
        {
            continuitySlider.editingFinished();
        }
    }

    SliderButton {
        id: buttonWhite
        x: 130
        y: 4
        state: "WHITE_CUP"
        minValue: 0
        maxValue: 255
        onValueChanged:
        {
            canvas.requestPaint();
        }
        onEditStarted:
        {
            continuitySlider.editingStarted();
        }
        onEditFinished:
        {
            continuitySlider.editingFinished();
        }
    }

}
