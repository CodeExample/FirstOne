import QtQuick 2.0

import com.vectools.ModelItem 1.0

Item {
    id: sectionControl
    width: 200
    height: 44

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
            modelItem: sectionControl.modelItem;
            width: 38
            height: 38
            radius: 4
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
            modelItem: sectionControl.modelItem;
            anchors.verticalCenter: parent.verticalCenter
        }

        LayerItemIcon {
            id: maskedIcon
            modelItem: freemeshControl.modelItem
            anchors.verticalCenter: parent.verticalCenter
            state: "MASKED"
        }

        LayerItemIcon {
            id: lockedIcon
            modelItem: freemeshControl.modelItem
            anchors.verticalCenter: parent.verticalCenter
            state: "LOCKED"
        }

        LayerItemIcon {
            id: paintingByLevelIcon
            modelItem: freemeshControl.modelItem
            anchors.verticalCenter: parent.verticalCenter
            state: "PAINTING_BY_LEVEL"
        }
    }

}
