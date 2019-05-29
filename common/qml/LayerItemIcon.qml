import QtQuick 2.0

import com.vectools.ModelItem 1.0

Item
{
    id: iconItem
    width: 14
    height: 14

    Image {
        id: image
        anchors.fill: parent
    }
    state: "LOCKED"

    property ModelItem modelItem;

    Connections{
        target: modelItem;

        onPreviewChanged:
        {
            updateVisible();
        }

        onSelectedChanged:
        {
            //var s = iconItem.state
            //iconItem.state = ""
            //iconItem.state = iconItem.state
            updateVisible();
        }
    }

    onModelItemChanged:
    {
        updateVisible();
    }

    function updateVisible()
    {
        if (!modelItem)
            return;

        switch(state)
        {
        case "PINNED":
            image.source = modelItem.isSelected() ? "images/soda/icob_pin_d.png" : "images/soda/icob_pin_l.png";
            iconItem.visible = modelItem.isPinnedMask();
            break;

        case "LOCKED":
            image.source = modelItem.isSelected() ? "images/soda/icob_lock_d.png" : "images/soda/icob_lock_l.png";
            iconItem.visible = modelItem.isLocked();
            break;

        case "MASKED":
            image.source = modelItem.isSelected() ? "images/soda/icob_mask_d.png" : "images/soda/icob_mask_l.png";
            iconItem.visible = modelItem.isMaskTransparent();
            break;

        case "PAINTING_BY_LEVEL":
            image.source = modelItem.isSelected() ? "images/soda/icob_level_pnt_d.png" : "images/soda/icob_level_pnt_l.png";
            iconItem.visible = modelItem.isFusionOfTemplates();
            break;
        }
    }

    states: [
        State {
            name: "PINNED"
            PropertyChanges { target: image; source: modelItem.isSelected() ? "images/soda/icob_pin_d.png" : "images/soda/icob_pin_l.png"}

            PropertyChanges {
                target: iconItem
                width: 14
                height: 14
            }
        },
        State {
            name: "LOCKED"
            PropertyChanges { target: image;  source: modelItem.isSelected() ? "images/soda/icob_lock_d.png" : "images/soda/icob_lock_l.png"}

            PropertyChanges {
                target: iconItem
                width: 14
                height: 14
            }
        },
        State {
            name: "MASKED"
            PropertyChanges { target: image;  source: modelItem.isSelected() ? "images/soda/icob_mask_d.png" : "images/soda/icob_mask_l.png"}

            PropertyChanges {
                target: iconItem
                width: 14
                height: 14
            }
        },
        State {
            name: "PAINTING_BY_LEVEL"
            PropertyChanges { target: image;  source: modelItem.isSelected() ? "images/soda/icob_level_pnt_d.png" : "images/soda/icob_level_pnt_l.png"}

            PropertyChanges {
                target: iconItem
                width: 14
                height: 14
            }
        }

    ]
}
