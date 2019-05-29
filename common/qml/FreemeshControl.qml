import QtQuick 2.0

import com.vectools.ModelItem 1.0

Item{
    id: freemeshControl
    width: 200
    height: 40

    property ModelItem modelItem;

    Connections
    {
        target: modelItem;
        onPreviewChanged:
        {
            background.color = modelItem.backColor();
        }
    }

    Row
    {
        id: row
        spacing: 0
        anchors.fill: parent

        PreviewItem
        {
            id: background;
            modelItem: freemeshControl.modelItem;
            width: 30
            height: 30
            radius: 2
            anchors.verticalCenter: parent.verticalCenter
            color: modelItem.backColor();
        }

        Item
        {
            width: 4
            height: 4
        }

        CaptionEdit
        {
            modelItem: freemeshControl.modelItem
            anchors.verticalCenter: parent.verticalCenter
        }

        LayerItemIcon {
            id: maskedIcon
            modelItem: freemeshControl.modelItem
            anchors.verticalCenter: parent.verticalCenter
            state: "MASKED"
        }

        LayerItemIcon {
            id: pinnedIcon
            modelItem: freemeshControl.modelItem
            anchors.verticalCenter: parent.verticalCenter
            state: "PINNED"
        }

        LayerItemIcon {
            id: lockedIcon
            modelItem: freemeshControl.modelItem
            anchors.verticalCenter: parent.verticalCenter
            state: "LOCKED"
        }

    }
}
