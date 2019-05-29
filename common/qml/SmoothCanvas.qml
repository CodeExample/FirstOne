import QtQuick 2.0

Canvas
{
    id: smoothCanvas;

    property color lineColor: "black";

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
        var w = smoothCanvas.width;
        var my = smoothCanvas.height/2;
        var a = 4*devicePixelRatio;
        var s = 8*devicePixelRatio;
        var n = w/s

        var ctx = getContext("2d");
        ctx.save();
        ctx.clearRect(0,0,smoothCanvas.width, smoothCanvas.height);

        ctx.fillStyle = "transparent";
        ctx.strokeStyle = lineColor;
        ctx.lineWidth= 2*devicePixelRatio;

        ctx.lineCap="round";
        ctx.lineJoin="round";
        ctx.beginPath();
        var sign = 1;
        ctx.moveTo(1.5*s, my);
        for(var i=2; i < n-1; ++i)
        {
            if (i == 2)
            {
                ctx.lineTo(i*s, my + sign*a);
            } else {
                var bcp_1 = 0.8*s*(i - 2)/n;
                var bcp_2 = 0.8*s*(i - 1)/n;

                var y = my + sign*a;
                ctx.bezierCurveTo((i-1)*s + bcp_1, my - sign*a,
                                  i*s  - bcp_2, my + sign*a,
                                  i*s, my + sign*a);
            }
            sign *= -1;
        }


        ctx.stroke();

        ctx.restore();
    }
}
