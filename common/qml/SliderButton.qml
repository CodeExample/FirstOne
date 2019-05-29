import QtQuick 2.0


Item {
    id: button;
    width: 16
    height: 16

    property double offValue: 0;
    property bool offEnabled: false;
    property bool isOff: false;

    property double minValue: 0;
    property double maxValue: 100;
    property int decimals: 0;

    property alias minBalloonValue: balloon.minValue;
    property alias maxBalloonValue: balloon.maxValue;

    property double old_value: -1;
    property double value: 0;

    property int buttonAnchor: 5;
    property int minPos: buttonAnchor;
    property int maxPos: parent.width - buttonAnchor - width;
    property double currentValue: 0;
    property int button_x: 0;

    signal editStarted(double v);
    signal editFinished(double v);
    signal offChanged(bool off);


    function getButtonPos(v)
    {
        v = validateValue(v);
        return minPos + (maxPos - minPos)*(v - minValue)/(maxValue - minValue);
    }

    function updateButtonPos()
    {
        button.x = getButtonPos(value);
    }

    function validateValue(v)
    {
        if (isNaN(v))
            return old_value;

        if (v > maxValue)
            v = maxValue;
        if (v < minValue)
            v = minValue;
        return v;
    }

    onMinPosChanged: { updateButtonPos(); }

    onMaxPosChanged: { updateButtonPos(); }

    onMinValueChanged:  { updateButtonPos(); }

    onMaxValueChanged: { updateButtonPos(); }


    onOffEnabledChanged:
    {
        checkOff();
    }

    onOffValueChanged:
    {
        checkOff();
    }

    onValueChanged:
    {
        if (old_value != value)
        {
            updateButtonPos();
            balloon.value = value;
        }
        old_value = value;

        checkOff();
    }

    function checkOff()
    {
        if (offEnabled)
        {
            if (value == offValue)
            {
                isOff = true;
                offChanged(true);
                balloon.text = "off";
            } else {
                isOff = false;
                offChanged(false);
            }
        } else {
            isOff = false;
            offChanged(false);
        }
    }

    Canvas {
        id: buttonCanvas
        x: 0
        y: 0
        width: 16*devicePixelRatio
        height: 16*devicePixelRatio

        transform: Scale
        {
            xScale: 1/devicePixelRatio
            yScale: 1/devicePixelRatio
        }

        Component.onCompleted:
        {
            initializeCanvas();
        }

        onPaint:
        {
            drawCanvas();
        }


        function oval(context, x, y, w, h)
        {
            context.save();
            context.beginPath();
            context.translate(x, y);
            context.scale(w/2, h/2);
            context.arc(1, 1, 1, 0, 2*Math.PI, false);
            context.closePath();
            context.restore();
        }


        function arc(context, x, y, w, h, startAngle, endAngle, isClosed)
        {
            context.save();
            context.beginPath();
            context.translate(x, y);
            context.scale(w/2, h/2);
            context.arc(1, 1, 1, Math.PI/180*startAngle, Math.PI/180*endAngle, false);
            if (isClosed)
            {
                context.lineTo(1, 1);
                context.closePath();
            }
            context.restore();
        }


        function initializeCanvas()
        {
            //var context = buttonCanvas.getContext('2d');
            //context.scale(devicePixelRatio, devicePixelRatio);
        }

        function drawCanvas()
        {
            var context = buttonCanvas.getContext('2d');

            var blackColor = 'rgba(0, 0, 0, 1)';
            var whiteColor = mouseArea.hovered ? 'rgba(255, 255, 255, 1)' : 'rgba(245, 245, 245, 1)';
            var gradientColor = mouseArea.hovered ? 'rgba(211, 209, 209, 1)' : 'rgba(191, 189, 189, 1)';
            var gradientColor2 = 'rgba(255, 255, 255, 1)';

            function gradient(g)
            {
                g.addColorStop(0, gradientColor2);
                g.addColorStop(1, gradientColor);
                return g;
            }

            oval(context, 1.5*devicePixelRatio, 1.5*devicePixelRatio, 13*devicePixelRatio, 13*devicePixelRatio);
            context.fillStyle = gradient(context.createLinearGradient(8.5, -0.5, 8.5, 15.5));
            context.fill();

            switch(button.state)
            {
            case "BLACK_CUP":
                oval(context, 5*devicePixelRatio, 5*devicePixelRatio, 6*devicePixelRatio, 6*devicePixelRatio);
                context.fillStyle = blackColor;
                context.fill();
                context.strokeStyle = blackColor;
                context.lineWidth = 1;
                context.stroke();
                break;

            case "WHITE_CUP":
                oval(context, 5*devicePixelRatio, 5*devicePixelRatio, 6*devicePixelRatio, 6*devicePixelRatio);
                context.fillStyle = whiteColor;
                context.fill();
                context.strokeStyle = blackColor;
                context.lineWidth = 1;
                context.stroke();
                break;

            case "BW_CUP":
                oval(context, 5*devicePixelRatio, 5*devicePixelRatio, 6*devicePixelRatio, 6*devicePixelRatio);
                context.fillStyle = whiteColor;
                context.fill();
                context.strokeStyle = blackColor;
                context.lineWidth = 1;
                context.stroke();

                arc(context, 5*devicePixelRatio, 5*devicePixelRatio, 6*devicePixelRatio, 6*devicePixelRatio, 90, 270, true);
                context.fillStyle = blackColor;
                context.fill();
                break;

            case "WB_CUP":
                oval(context, 5*devicePixelRatio, 5*devicePixelRatio, 6*devicePixelRatio, 6*devicePixelRatio);
                context.fillStyle = whiteColor;
                context.fill();
                context.strokeStyle = blackColor;
                context.lineWidth = 1;
                context.stroke();

                arc(context, 5*devicePixelRatio, 5*devicePixelRatio, 6*devicePixelRatio, 6*devicePixelRatio, 270, 90, true);
                context.fillStyle = blackColor;
                context.fill();
                break;
            }

        }


    }

    function getValue(pos)
    {
        var dcml = Math.pow(10, decimals);
        return Math.round(dcml*(minValue + (maxValue - minValue)*(pos - minPos)/(maxPos - minPos)))/dcml;
    }

    MouseArea
    {
        id: mouseArea
        anchors.fill: parent;
        hoverEnabled: true;
        preventStealing: true;

        property int mouse_x;
        property int mouse_y;
        property bool mouse_pressed: false;
        property bool hovered: false;
        property bool mouse_moved: false;


        onEntered:
        {
            hovered = true;
            balloon.show();
            buttonCanvas.requestPaint();
            updateButtonPos();
        }

        onExited:
        {
            hovered = false;
            if (!mouse_pressed)
                balloon.hide();
            buttonCanvas.requestPaint();
            updateButtonPos();
        }

        onPressed:
        {
            mouse_x = mouseX;
            mouse_y = mouseY;
            mouse_pressed = true;
            mouse_moved = false;
            editStarted(getValue(button.x));
            updateButtonPos();
            mouse.accepted = true;
        }

        onReleased:
        {
            mouse_pressed = false;

            if (mouse_moved)
            {
                editFinished(value);
            } else {
                if (hovered)
                    balloon.startEdit();
            }

            if (!hovered)
                balloon.hide();

            mouse.accepted = true;
        }


        onPressAndHold:
        {
            if (offEnabled)
            {
                var off_x = getButtonPos(offValue);
                var button_x = button.x + mouseX - mouse_x;
                if (Math.abs(button_x - off_x) < 10)
                {
                    value = offValue;
                }
            }
        }

        onPositionChanged:
        {
            if (mouse_pressed)
            {
                var button_x = button.x + mouseX - mouse_x;

                if (button_x < minPos)
                    button_x = minPos;

                if (button_x > maxPos)
                    button_x = maxPos;

                var v = getValue(button_x);
                var off_value_is_now = false;
                if (offEnabled && value == offValue)
                {
                    var off_x = getButtonPos(offValue);
                    if (Math.abs(button_x - off_x) < 10)
                    {
                        off_value_is_now = true;
                    }
                }
                if (!off_value_is_now)
                {
                    if (v !== button.currentValue)
                    {
                        button.currentValue = v;
                        value = v;
                    }
                }

                mouse_moved = true;
                mouse.accepted = true;
            }
        }
    }

    Balloon
    {
        id: balloon;
        y: -20;
        anchors.horizontalCenter: parent.horizontalCenter
        decimals: button.decimals;
        minValue: button.minValue;
        maxValue: button.maxValue;

        onEditingFinished:
        {
            button.value = balloon.value;
            button.updateButtonPos();
            editFinished(getValue(button.x));
        }
    }


}
