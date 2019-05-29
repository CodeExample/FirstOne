import QtQuick 2.0

Rectangle
{
    id: enterRectangle
    width: 18
    height: 18
    radius: 9
    color: "transparent"
    border.width: 1
    border.color: "transparent"

    signal changesApply();

    MouseArea
    {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true;

        onEntered:
        {
            buttonCanvas.opacity = 1;
        }
        onExited:
        {
            buttonCanvas.opacity = 0.8;
        }
        onClicked:
        {
            changesApply();
        }
        onPressed:
        {
            enterRectangle.color = "#1a1b26";
        }
        onReleased:
        {
            enterRectangle.color = "#3a3c49";
        }

/*        Image {
            id: imageApply
            x: 0
            y: -14
            width: 16
            height: 16
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            antialiasing: true
            source: "images/svg/ok_btn.svg"
        }
        */


        Canvas
        {
            id: buttonCanvas
            x: 1
            y: 1
            width: 16*devicePixelRatio
            height: 16*devicePixelRatio


            opacity: 0.8

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

            function initializeCanvas()
            {
                //var context = buttonCanvas.getContext('2d');
                //context.scale(devicePixelRatio, devicePixelRatio);
            }

            function drawCanvas()
            {
                var context = buttonCanvas.getContext('2d');
                var color0 = 'rgba(255, 255, 255, 1)';
                context.beginPath();
                context.moveTo(8, 14.61);
                context.bezierCurveTo(11.65, 14.61, 14.61, 11.65, 14.61, 8);
                context.bezierCurveTo(14.61, 4.35, 11.65, 1.39, 8, 1.39);
                context.bezierCurveTo(4.35, 1.39, 1.39, 4.35, 1.39, 8);
                context.bezierCurveTo(1.39, 11.65, 4.35, 14.61, 8, 14.61);
                context.closePath();
                context.moveTo(3.83, 7.98);
                context.bezierCurveTo(4.23, 7.46, 4.98, 7.35, 5.5, 7.75);
                context.lineTo(7.05, 8.92);
                context.lineTo(10.06, 5.17);
                context.bezierCurveTo(10.47, 4.65, 11.22, 4.57, 11.73, 4.98);
                context.bezierCurveTo(12.24, 5.39, 12.33, 6.14, 11.91, 6.65);
                context.lineTo(8.19, 11.31);
                context.bezierCurveTo(7.96, 11.6, 7.61, 11.75, 7.26, 11.75);
                context.bezierCurveTo(7.01, 11.75, 6.76, 11.67, 6.54, 11.51);
                context.lineTo(4.07, 9.65);
                context.bezierCurveTo(3.54, 9.25, 3.44, 8.5, 3.83, 7.98);
                context.closePath();
                context.miterLimit = 4;
                context.fillStyle = color0;
                context.fill();
            }
        }

    }
}
