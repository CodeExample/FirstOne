import QtQuick 2.0

import com.vectools.DataProvider 1.0

Item {
    id: dashesSlider
    width: 200
    height: 24

    property int treshold: 0;

    property bool inverted: false;

    property alias enabled: offPanel.enabled;

    property string rulerColor: "#FF000000"

    property int offTreshold: 255;

    onTresholdChanged:
    {
        buttonThreshold.value = enabled ? treshold : offTreshold;
    }

    onEnabledChanged:
    {
        buttonThreshold.enabled = dataProvider.getBoolValue(codeControl, DataProvider.IsEnabled);
    }

    onRulerColorChanged: canvas.requestPaint();

    signal editingStarted();
    signal editingFinished();


    Rectangle
    {
        color: "#3a3d48"
        anchors.fill: parent
        radius: height/2
        Rectangle
        {
            color: "#434650"
            height: 2
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: parent.height/2
            anchors.right: parent.right
            anchors.rightMargin: parent.height/2
        }
    }


    onInvertedChanged:
    {
        if (inverted)
            buttonThreshold.state = "WB_CUP";
        else
            buttonThreshold.state = "BW_CUP";
    }


    Canvas
    {
        id: canvas;
        anchors.fill: parent;

        onPaint:
        {
            if (!dataProvider.getBoolValue(codeControl, DataProvider.IsEnabled))
                return;

            var ctx = getContext("2d");
            ctx.save();
            ctx.clearRect(0,0,canvas.width, canvas.height);

            ctx.fillStyle = dashesSlider.rulerColor;

            // solid
            var bt_h = buttonThreshold.width;
            var x0 = bt_h/2;
            var x1 = buttonThreshold.x + bt_h/2;
            var y0 = parent.height/2 - 2;
            var y1 = parent.height/2 + 2;
            ctx.fillRect(x0, y0, x1-x0, y1 - y0);


            // dashes
            var w = parent.width;
            var c = parent.height/2;
            var h = 2;
            var th_x = buttonThreshold.x*w/255.0;

            var d = (w - th_x)/5;
            if (d < 10)
                d = 10;
            if (d > 40)
                d = 40;

            var dx = th_x;

            while(dx + d < parent.width - bt_h/2)
            {
                dx += d;
                d /= 1.2;

                if (d < 10)
                    d = 10;

                var dh = h*(1.0 - 0.4*dx/w);

                ctx.beginPath();
                ctx.moveTo(dx - 0.7*d, c - dh);
                ctx.lineTo(dx - 0.7*d, c + dh);
                ctx.lineTo(dx, c + dh);
                ctx.lineTo(dx, c - dh);
                ctx.lineTo(dx - 0.7*d, c - dh);
                ctx.closePath();
                ctx.fill();
            }
        }
    }


    OffPanel
    {
        id: offPanel;
    }


    SliderButton
    {
        id: buttonThreshold
        x: 22
        y: 4
        state: "BW_CUP"
        minValue: 0
        maxValue: 255
        offValue: offTreshold
        offEnabled: true
        isOff: !enabled
        value: treshold
        onValueChanged: canvas.requestPaint();
        enabled: dataProvider.getBoolValue(codeControl, DataProvider.IsEnabled);

        onEditStarted:
        {
            dashesSlider.editingStarted();
        }
        onEditFinished:
        {
            treshold = v;
            dashesSlider.editingFinished();
        }

        onOffChanged:
        {
            if (off || !dataProvider.getBoolValue(codeControl, DataProvider.IsEnabled))
            {
                offPanel.state = "OFF";
                dashesSlider.enabled = false;
            } else {
                offPanel.state = "ON"
                dashesSlider.enabled = true;
            }
        }        
    }



}
