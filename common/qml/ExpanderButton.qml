import QtQuick 2.0

Item
{
    id: expanderButton;
    width: 20
    height: 40

    signal opened();
    signal closed();

    onStateChanged:
    {
        canvas.requestPaint();
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

            ctx.fillStyle = "gray";

            var d = 5;

            switch(expanderButton.state)
            {
            case "DISABLED":
                break;

            case "CLOSED":
                var x0 = 5;
                var y0 = 20;
                ctx.beginPath();
                ctx.moveTo(x0, y0);
                ctx.lineTo(x0, y0 - d);
                ctx.lineTo(x0 + d, y0);
                ctx.lineTo(x0, y0 + d);
                ctx.lineTo(x0, y0);
                ctx.fill();
                break;

            case "OPENED":
                x0 = 10;
                y0 = 20-d/2;
                ctx.beginPath();
                ctx.moveTo(x0, y0);
                ctx.lineTo(x0 + d, y0);
                ctx.lineTo(x0, y0 + d);
                ctx.lineTo(x0 - d, y0);
                ctx.lineTo(x0, y0);
                ctx.fill();
                break;
            }

            ctx.restore();
        }
    }

    MouseArea
    {
        anchors.fill: expanderButton;
        onClicked:
        {
            switch(expanderButton.state)
            {
            case "DISABLED":
                break;

            case "OPENED":
                expanderButton.state = "CLOSED";
                closed();
                break;

            case "CLOSED":
                expanderButton.state = "OPENED";
                opened();
                break;
            }
        }
    }

    states: [
        State {
            name: "CLOSED"
        },
        State {
            name: "OPENED"
        },
        State {
            name: "DISABLED"
        }
    ]
}
