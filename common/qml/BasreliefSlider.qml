import QtQuick 2.0

import com.vectools.DataProvider 1.0

Item {
    id: basreliefSlider
    width: 200
    height: 24

    property int front: 0;
    property int minFront: -100;
    property int maxFront: 100;

    property alias rise: buttonRise.value;
    property alias minRise: buttonRise.minValue;
    property alias maxRise: buttonRise.maxValue;

    property alias minBalloonValue: buttonRise.minBalloonValue;
    property alias maxBalloonValue: buttonRise.maxBalloonValue;

    property alias smooth: buttonSmooth.value;
    property alias minSmooth: buttonSmooth.minValue;
    property alias maxSmooth: buttonSmooth.maxValue;

    property double offRise: 0.0;
    property alias enabled: offPanel.enabled;

    property string rulerColor: "#FF000000"

    onRulerColorChanged: riseCanvas.requestPaint();

    onRiseChanged:
    {
        buttonRise.value = enabled ? rise : offRise;
        riseCanvas.requestPaint();
    }

    onEnabledChanged:
    {
        buttonSmooth.enabled = dataProvider.getBoolValue(codeControl, DataProvider.IsEnabled);
        buttonRise.enabled = dataProvider.getBoolValue(codeControl, DataProvider.IsEnabled);
    }

    onSmoothChanged:
    {
        if (smooth > maxSmooth)
            maxSmooth = smooth*1.5;

        riseCanvas.requestPaint();
    }


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
            height: parent.height
            width: 6
            x: parent.width/2 -3;
        }
    }


    Canvas
    {
        id: riseCanvas;
        anchors.fill: riseItem;

        onPaint:
        {
            if (!dataProvider.getBoolValue(codeControl, DataProvider.IsEnabled))
                return;

            var bt_h = buttonRise.width/2;
            var x0 = bt_h;
            var x1 = riseCanvas.width - bt_h;
            var m_y = riseCanvas.height/2;
            var d_r  = bt_h*rise/(maxRise - minRise);
            var y0 = m_y - d_r;
            var y1 = m_y + d_r;
            var p_w = riseCanvas.width;
            var f_x = p_w/2 + p_w*front/(maxFront - minFront);

            var s_d = 0.2*p_w*basreliefSlider.smooth/(maxSmooth - minSmooth);
            var s_x0 = f_x - s_d;
            var s_x1 = f_x + s_d;

            var ctx = getContext("2d");
            ctx.save();
            ctx.clearRect(0,0,riseCanvas.width, riseCanvas.height);

            ctx.fillStyle = basreliefSlider.rulerColor;
            ctx.strokeStyle = basreliefSlider.rulerColor;
            ctx.lineWidth="2.5";

            ctx.beginPath();
            ctx.moveTo(x0, y0);
            ctx.lineTo(s_x0, y0);
            ctx.lineTo(s_x1, y1);
            ctx.lineTo(x1, y1);
            ctx.stroke();

            ctx.restore();
        }
    }


    SmoothCanvas
    {
        x: parent.width/2;
        y: 0;
        width: smoothItem.width*devicePixelRatio;
        height: smoothItem.height*devicePixelRatio;
        lineColor: basreliefSlider.rulerColor;
        visible: basreliefSlider.enabled;
    }

    Item
    {
        id: smoothItem
        x:  parent.width/2
        width: parent.width/2
        height: 24


        SliderButton
        {
            id: buttonSmooth
            x: 22
            y: 4
            minValue: 0
            maxValue: 20
            enabled: dataProvider.getBoolValue(codeControl, DataProvider.IsEnabled);

            onValueChanged:
            {
                riseCanvas.requestPaint();
            }
            onEditStarted:
            {
                basreliefSlider.editingStarted();
            }
            onEditFinished:
            {
                basreliefSlider.editingFinished();
            }

        }
    }


    OffPanel
    {
        id: offPanel

        anchors.fill: parent

        onStateChanged:
        {
            if (state == "OFF")
            {
                buttonSmooth.enabled = false;
                buttonSmooth.visible = false;
            } else {
                buttonSmooth.enabled = true;
                buttonSmooth.visible = true;
            }
        }
    }


    Item
    {
        id: riseItem;
        width: parent.width/2;
        height: 24;

        SliderButton
        {
            id: buttonRise
            x: 66
            y: 4

            offEnabled: true
            offValue: offRise
            enabled: dataProvider.getBoolValue(codeControl, DataProvider.IsEnabled);
            isOff: !enabled

            value: rise

            decimals: 2

            onValueChanged:
            {
                riseCanvas.requestPaint();
            }
            onEditStarted:
            {
                basreliefSlider.editingStarted();
            }
            onEditFinished:
            {
                rise = v;
                basreliefSlider.editingFinished();
            }
            onOffChanged:
            {
                if (off || !dataProvider.getBoolValue(codeControl, DataProvider.IsEnabled))
                {
                    offPanel.state = "OFF";
                    basreliefSlider.enabled = false;
                } else {
                    offPanel.state = "ON"
                    basreliefSlider.enabled = true;
                }
            }
        }
    }


}
