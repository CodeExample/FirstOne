import QtQuick 2.0

import com.vectools.ModelItem 1.0

Rectangle
{
    id: previewItem
    property ModelItem modelItem;
    clip: true
    color: "lightgray"

    Connections
    {
        target: modelItem;
        onPreviewChanged:
        {
            previewImage.source = ""
            previewImage.source = "image://previews/" + modelItem.objectId();
        }

        onVisibleChanged:
        {
            shadowItem.visible = !modelItem.isVisible();
        }
    }

    onModelItemChanged:
    {
        shadowItem.visible = !modelItem.isVisible();
        previewImage.source = "image://previews/" + modelItem.objectId();
    }

    Image
    {
        id: previewImage
        anchors.fill: parent;
        antialiasing: true
        cache: false
        source: "image://previews/" + modelItem.objectId();
        sourceSize.width: width
        sourceSize.height: height
    }


    MouseArea
    {
        anchors.fill: parent;

        onClicked:
        {
            modelItem.setVisible(!modelItem.isVisible());
        }
    }

    Item
    {
        id: shadowItem

        anchors.fill: parent;


        Rectangle
        {
            anchors.fill: parent;

            color: "black"
            opacity: 0.5
            radius: previewItem.radius
        }

        Image
        {
            id: eyeImage
            anchors.centerIn: parent;
            source: "images/soda/eye.png"
        }
    }


}
