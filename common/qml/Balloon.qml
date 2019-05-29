import QtQuick 2.0
import "Global.js" as Global

Item
{
    id: balloon
    property alias text: textInput.text;
    property alias inputMask: textInput.inputMask;

    property int verticalPadding: 1;
    property int horizontalPadding: 5;
    property double minValue: 0;
    property double maxValue: 255;
    property double value: 0;
    property int decimals: 4;
    property double old_value: 0;
    property bool hidden: false;
    property bool shift_pressed: false;
    property double incriment: 1;

    width: textInput.width + horizontalPadding * 2;
    height: 18

    state: "HIDDEN"

    signal editingFinished(double v);

    onValueChanged:
    {
/*        if (!was_changed)
        {
            old_value = value;
        }*/
        textInput.text = Number(validateValue(value)).toLocaleString(Qt.locale("us_US"), 'g', 4);
    }

    Component.onCompleted:
    {
        textInput.text = Number(validateValue(value)).toLocaleString(Qt.locale("us_US"), 'g', 4);

        //value = parseFloat(textInput.text);

        //balloon.value = validateValue(balloon.value);
        //textInput.text = validateValue(balloon.value);
        //textInput.text = Number(balloon.value).toLocaleString(Qt.locale("us_US"), 'g', 2);

    }



    Canvas
    {
        id: balloonCanvas

        x: 0;
        y: 0;
        height: parent.height*devicePixelRatio;
        width: parent.width*devicePixelRatio;

        transform: Scale
        {
            xScale: 1/devicePixelRatio
            yScale: 1/devicePixelRatio
        }

        onPaint:
        {
            drawCanvas();
        }


        function drawCanvas()
        {
            var w_2 = devicePixelRatio*balloon.width/2;
            var c = 2*devicePixelRatio;
            var r = 3*devicePixelRatio;
            var s = 4*devicePixelRatio;
            var bh = devicePixelRatio*balloon.height - s;

            var y0 = 0;
            var yc0 = y0 + c;
            var y1 = y0 + r;
            var y2 = bh - r;
            var yc2 = y2 + c;
            var y3 = y2 + r;
            var y4 = devicePixelRatio*balloon.height-1

            var x0 = 0;
            var x1 = x0 + r;
            var xc0 = x1 - c;
            var x2 = w_2 - s;
            var x3 = w_2;
            var x4 = w_2 + s;
            var x6 = devicePixelRatio*balloon.width-1;
            var x5 = x6 - r;
            var xc5 = x5 + c;


            var ctx = getContext("2d");
            ctx.save();


            ctx.clearRect(0,0, balloonCanvas.width, balloonCanvas.height);

            ctx.fillStyle = "#D0969cac";
            ctx.lineWidth="0";

            ctx.beginPath();
            ctx.moveTo(x0, y1);
            ctx.lineTo(x0, y2);

            ctx.bezierCurveTo(x0,yc2, xc0,y3, x1,y3);

            ctx.lineTo(x2, y3);
            ctx.lineTo(x3, y4);
            ctx.lineTo(x4, y3);
            ctx.lineTo(x5, y3);

            ctx.bezierCurveTo(xc5,y3, x6,yc2, x6,y2);
            ctx.lineTo(x6, y1);

            ctx.bezierCurveTo(x6,yc0, xc5,y0, x5,y0);
            ctx.lineTo(x1, y0);

            ctx.bezierCurveTo(xc0,y0, x0,yc0, x0,y1);
            ctx.closePath();
            ctx.fill();

            ctx.restore();
        }
    }

    onWidthChanged:
    {
        //x = parent.width/2 - width/2)*;
    }


    function validateValue(v)
    {

        v = Math.round(v*1000.0)/1000.0;

        if (v < minValue)
            v = minValue;

        if (v > maxValue)
            v = maxValue;

        return v;
    }


    function finishEdit(notify)
    {
        value = parseFloat(textInput.text);
        textInput.deselect();
        textInput.focus = false;

        if (hidden)
            state = "HIDDEN";
        else
            state = "SHOWED";

        if (notify)
            editingFinished(value);
    }


    TextInput {
        id: labelOff
        anchors.top: parent.top
        anchors.topMargin: 1
        anchors.horizontalCenter: parent.horizontalCenter
        visible: false;

        color: "#222222"
        text: "off"
        selectionColor: "#656565"
        anchors.verticalCenter: parent.verticalCenter
        horizontalAlignment: TextInput.AlignLeft
        font.pixelSize: 10
        focus: false;
    }

    TextInput {
        id: textInput
        anchors.top: parent.top
        anchors.topMargin: 1
        anchors.horizontalCenter: parent.horizontalCenter
        visible: true;

        color: "#222222"
        selectionColor: "#656565"
        anchors.verticalCenter: parent.verticalCenter
        horizontalAlignment: TextInput.AlignLeft
        font.pixelSize: 10

        validator: DoubleValidator
        {
            bottom: balloon.minValue;
            top: balloon.maxValue;
            decimals: balloon.decimals;
        }


    Keys.onPressed:
    {
        if (event.key === Qt.Key_Enter || event.key === Qt.Key_Return)
        {
            textInput.focus = false;
            balloon.value = parseFloat(textInput.text);
            finishEdit(true);
        }

        if (state == "OFF")
            return;

        if (event.key === Qt.Key_Shift)
        {
            shift_pressed = true;
        }

        if (event.key === Qt.Key_Escape)
        {
            textInput.text = old_value;
            finishEdit(false);
        }

        if (event.key === Qt.Key_Up)
        {
            incValue(incriment)
        }

        if (event.key === Qt.Key_Down)
        {
            decValue(incriment)
        }
    }

    Keys.onReleased:
    {
        if (event.key === Qt.Key_Shift)
        {
            shift_pressed = false;
        }
    }
}


function incValue(i)
{
    value += shift_pressed ? i*10 : i;
    //textInput.text = validateValue(value);
}

function decValue(i)
{
    value -= shift_pressed ? i*10 : i;
    //textInput.text = validateValue(value);
}


function show()
{
    deactivationTimer.stop();
    activationTimer.start();
    hidden = false;
}


function hide()
{
    activationTimer.stop();
    deactivationTimer.start();
    hidden = true;
}


function startEdit()
{
    state = "EDITED";
    textInput.enabled = true;
    textInput.selectAll();
    if (!textInput.activeFocus)
    {
        textInput.forceActiveFocus();
    }
    old_value = value;
}


Timer
{
    id: activationTimer
    interval: 100; running: false; repeat: false
    onTriggered:
    {
        balloon.visible = true;
        state = "SHOWED"
    }
}


Timer
{
    id: deactivationTimer
    interval: 100; running: false; repeat: false
    onTriggered:
    {
        if (balloon.state === "SHOWED")
        {
            state = "HIDDEN"
        }
    }
}


states: [
State {
    name: "HIDDEN"
    PropertyChanges { target: balloon; opacity: 0}
    PropertyChanges { target: balloon; focus: false}
},
State {
    name: "SHOWED"
    PropertyChanges { target: balloon; opacity: 1}
    PropertyChanges { target: balloon; focus: false}
},
State {
    name: "EDITED"
    PropertyChanges { target: balloon; opacity: 1}
},
State {
    name: "OFF"
    PropertyChanges { target: textInput; visible: false}
    PropertyChanges { target: labelOff; visible: true}
}

]

transitions: Transition {
    NumberAnimation { properties: "opacity"; easing.type: Easing.InOutQuad; duration: 200 }
}


}
