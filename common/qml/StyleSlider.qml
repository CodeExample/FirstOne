import QtQuick 2.0

Item {
    id: styleSlider
    width: 200
    height: 24

    property alias terminalNodes: buttonTerminal.value;
    property alias smoothThickness: buttonSmooth.value;
    property alias maxValue: buttonTerminal.maxValue;

    property string rulerColor: "#FF000000"

    onTerminalNodesChanged:
    {
        if (buttonTerminal.value != terminalNodes)
        {
            buttonTerminal.value = terminalNodes;
        }
        terminalCanvas.requestPaint();
    }

    signal editingStarted();
    signal editingFinished();

    onSmoothThicknessChanged:
    {
        if (buttonSmooth.value != smoothThickness)
        {
            buttonSmooth.value = smoothThickness;
        }
    }

    onRulerColorChanged: terminalCanvas.requestPaint();

    Rectangle
    {
        color: "#3a3d48"
        anchors.fill: parent
        radius: height/2
        Rectangle
        {
            color: "#444751"
            height: parent.height
            width: 6
            x: parent.width/2 -3;
        }
    }

    Item
    {
        id: terminalItem
        width: parent.width/2
        height: 24

        Canvas
        {
            id: terminalCanvas;
            anchors.fill: parent;

            onPaint:
            {
                var bt_h = buttonTerminal.width/2;
                var m_y = parent.height/2;

                var x0 = bt_h;
                var x1 = buttonTerminal.x + bt_h;
                var x2 = parent.width - bt_h;
                var dy = 3.5;

                var ctx = getContext("2d");
                ctx.save();
                ctx.clearRect(0,0,terminalCanvas.width, terminalCanvas.height);

                ctx.fillStyle = styleSlider.rulerColor;

                ctx.beginPath();
                ctx.moveTo(x0, m_y);
                ctx.lineTo(x1, m_y - dy);
                ctx.lineTo(x2, m_y - dy);
                ctx.lineTo(x2, m_y + dy);
                ctx.lineTo(x1, m_y + dy);
                ctx.lineTo(x0, m_y);
                ctx.closePath();
                ctx.fill();

                ctx.restore();
            }
        }

        SliderButton
        {
            id: buttonTerminal
            x: 22
            y: 4
            minValue: 0
            maxValue: 5
            decimals: 1
            onValueChanged:
            {
                terminalCanvas.requestPaint();
            }
            onEditStarted:
            {
                styleSlider.editingStarted();
            }
            onEditFinished:
            {
                styleSlider.editingFinished();
            }
        }
    }

    Item
    {
        id: smoothItem
        x:  parent.width/2
        width: parent.width/2
        height: 24

        SmoothCanvas
        {
            x: 0;
            y: 0;
            width: parent.width*devicePixelRatio;
            height: parent.height*devicePixelRatio;

            lineColor: styleSlider.rulerColor;
        }

        SliderButton
        {
            id: buttonSmooth
            y: 4
            minValue: 0
            maxValue: 20

            onValueChanged:
            {
                terminalCanvas.requestPaint();
            }
            onEditStarted:
            {
                styleSlider.editingStarted();
            }
            onEditFinished:
            {
                styleSlider.editingFinished();
            }
        }

    }
}
