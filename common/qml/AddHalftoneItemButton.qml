import QtQuick 2.0

import com.vectools.DataProvider 1.0

Item
{
    id: addHalftoneItemButton

    width: 24
    height: 24

    property int objectId;

    signal buttonClcked();

    Rectangle
    {
        id: backImage;
        anchors.fill: parent;
        color: "black"
        visible: false;
    }

    Image
    {
        id: imageBtn
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        opacity: 0.6
        source: "images/HalftonePlus.png"
    }

    MouseArea
    {
        anchors.fill: parent;
        hoverEnabled: true;
        onEntered:
        {
            imageBtn.opacity = 1
        }
        onExited:
        {
            imageBtn.opacity = 0.6
        }
        onClicked:
        {
            buttonClcked();
        }

        onPressed:
        {
            backImage.visible = true;
        }
        onReleased:
        {
            backImage.visible = false;
        }
    }



}
