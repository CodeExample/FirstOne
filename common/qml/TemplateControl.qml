import QtQuick 2.0

import com.vectools.ModelItem 1.0
import com.vectools.DataServer 1.0

Item
{
    id: templateControl
    width: 200
    height: 40

    property ModelItem modelItem;

    Connections
    {
        target: modelItem;

        onSelectedChanged:
        {
            updateData();
        }

        onPreviewChanged:
        {
            updateData();
        }
    }

    function updateData()
    {
        caption.color = modelItem.isSelected() ?"#30343a" : "#dcdfe9";
        caption.text = modelItem.caption();
        var page_mode = modelItem.pageMode();
        if (modelItem.gathersEnabled())
        {
            imagePageMode.visible = true;

            if (page_mode === 0)
            {
                imagePageMode.source = "images/soda/icob_both_l.png";
            } else
                if (page_mode === 1)
                {
                    imagePageMode.source = "images/soda/icob_front_l.png";
                } else
                    if (page_mode === 2)
                    {
                        imagePageMode.source = "images/soda/icob_back_l.png";
                    }
        } else {
            imagePageMode.visible = false;
        }
    }

    onModelItemChanged:
    {
        updateData();
    }

    Row {
        id: row
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 0
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.left: parent.left
        spacing: 0

        PreviewItem
        {
            modelItem: templateControl.modelItem;
            width: 29
            height: 29
            radius: 14
            anchors.verticalCenter: parent.verticalCenter
            color: modelItem ? modelItem.backColor() : "red";
        }

        Item
        {
            width: 4
            height: 4
        }

        Text
        {
            id: caption
            color: "#dcdfe9"
            height: 20
            text: modelItem.caption();
            anchors.verticalCenterOffset: 2
            anchors.verticalCenter: parent.verticalCenter
            font.pixelSize: 12
            elide: Text.ElideMiddle
        }

        LayerItemIcon
        {
            id: lockedIcon
            modelItem: freemeshControl.modelItem
            anchors.verticalCenter: parent.verticalCenter
            state: "LOCKED"
        }

        MouseArea
        {
            anchors.verticalCenter: parent.verticalCenter
            hoverEnabled: true
            width: 14
            height: 14
            Image
            {
                id: imagePageMode
                anchors.fill: parent
                opacity: 0.8
            }

            onEntered:
            {
                imagePageMode.opacity = 1;
            }
            onExited:
            {
                imagePageMode.opacity = 0.8;
            }
            onClicked:
            {
                if (modelItem.pageMode() >= 0)
                    modelItem.setPageMode((modelItem.pageMode() + 1)%3);
                update();
            }

        }

    }


    MouseArea
    {
        x: -22
        anchors.verticalCenter: parent.verticalCenter
        width: 14
        height: 14

        hoverEnabled: true;

        onEntered:
        {
            borderColorRect.visible = true;
        }

        onExited:
        {
            borderColorRect.visible = false;
        }

        onClicked:
        {
            var panel_p = templateControl.mapToItem(null, 0,0);
            dataServer.changeColor(modelItem.objectId(), DataServer.LayerPanel, panel_p.x, panel_p.y);
            borderColorRect.border.color = modelItem.color();
            colorRect.color = modelItem.color();
        }
    }

    Rectangle
    {
        id: borderColorRect
        visible: false
        anchors.centerIn: colorRect
        anchors.verticalCenter: parent.verticalCenter
        width: 16
        height: 16
        radius: 8
        border.color: modelItem.color()
        border.width: 2
        color: 'transparent'
    }


    Rectangle
    {
        id: colorRect
        x: -23
        anchors.verticalCenter: parent.verticalCenter
        width: 12
        height: 12
        radius: 6
        color: modelItem.color()
    }

}
