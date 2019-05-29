import QtQuick 2.0

Item {
    id: maskSlider
    width: 200
    height: 24

    property alias toleranceMask: buttonTolerance.value;
    property int minToleranceValue: 0;
    property int maxToleranceValue: 25;

    property alias opacityMask: buttonOpacity.value;
    property int minOpacityValue: 0;
    property int maxOpacityValue: 100;

    //property bool enabled: true;

    property string rulerColor: "#FF000000"


    onRulerColorChanged: toleranceCanvas.requestPaint();

    signal editingStarted;
    signal editingFinished;


    Rectangle
    {
        color: "#3a3d48"
        anchors.fill: parent
        radius: height/2

    }

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

    Item
    {
        id: toleranceItem
        width: parent.width/2
        height: 24

        Image
        {
            id: imagetolerance
            x: 13
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left;
            anchors.right: parent.right;
            anchors.leftMargin: 6
            anchors.rightMargin: 6
            source: "images/slider_tolerance_scale_78x12_1.png"
        }

        SliderButton
        {
            id: buttonTolerance
            x: 22
            y: 4
            minValue: minToleranceValue
            maxValue: maxToleranceValue

            onEditStarted:
            {
                maskSlider.editingStarted();
            }
            onEditFinished:
            {
                maskSlider.editingFinished();
            }
        }
    }

    Item
    {
        id: opacityItem
        x:  parent.width/2
        width: parent.width/2
        height: 24
        Image
        {
            id: imageOpacity
            x: 13
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left;
            anchors.right: parent.right;
            anchors.leftMargin: 6
            anchors.rightMargin: 6

            source: "images/slider_opacity_scale_78x8_1.png"
        }


/*
        MaskOpacityRuler
        {
            id: maskOpacityRuler1
            height: 5
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 8
            anchors.right: parent.right
            anchors.rightMargin: 6
        }
*/
        SliderButton
        {
            id: buttonOpacity
            x: 130
            y: 4
            minValue: minOpacityValue
            maxValue: maxOpacityValue
            offEnabled: true
            offValue: 0

            onEditStarted:
            {
                maskSlider.editingStarted();
            }
            onEditFinished:
            {
                maskSlider.editingFinished();
            }
            onOffChanged:
            {
            //    if (off)
            //        maskSlider.enabled = false;
            //    else
            //        maskSlider.enabled = true;
            }

        }

    }
}
