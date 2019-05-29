import QtQuick 2.0

import com.vectools.ModelItem 1.0
import com.vectools.DataServer 1.0

Item
{
    id: layerRow
    width: 200
    height: 42
    z: 1
    state: "SECTION"

    property ModelItem modelItem;

    property int itemIndex;

    property bool isClosed: modelItem.isClosed();
    property bool isClosable: false;

    signal closed();
    signal opened();

    function updateTypeOfRow()
    {
        layerItem.modelItem = modelItem;

        if (modelItem && modelItem.isSection())
        {
            layerItem.state = "SECTION";
            isClosable = true;
            return;
        }

        if (modelItem.isFreemesh())
        {
            layerItem.state = "FREEMESH";
            isClosable = true;
            return;
        }

        if (modelItem.isTemplate())
        {
            layerItem.state = "TEMPLATE";
            expanderButton.state = "DISABLED";
            isClosable = false;
            return;
        }
    }

    onModelItemChanged:
    {
        updateTypeOfRow();

        if (isClosable)
        {
            if (modelItem.isClosed())
            {
                expanderButton.state = "CLOSED"
            } else {
                expanderButton.state = "OPENED"
            }
        }

        selectionRect.visible = modelItem.isSelected();

    }

    onIsClosedChanged:
    {
        if (isClosable)
        {
            if (isClosed)
            {
                expanderButton.state = "CLOSED";
            } else {
                expanderButton.state = "OPENED";
            }
        }
    }

    onIsClosableChanged:
    {
        if (isClosable)
        {
            if (isClosed)
            {
                expanderButton.state = "CLOSED";
            } else {
                expanderButton.state = "OPENED";
            }
        }

    }

    onStateChanged:
    {
        if (isClosable && state == "HIDDEN")
        {
            isClosed = true;
        }
    }


    Connections
    {
        target: modelItem
        onSelectedChanged:
        {
            selectionRect.visible = modelItem.isSelected();
        }
        onItemExpanded:
        {
            isClosed = false;
            layerRow.opened();
        }
        onItemClosed:
        {
            isClosed = true;
            layerRow.closed();
        }

    }

    Connections
    {
        target: dataServer
        onDragStarted:
        {
            if (modelItem.isSelected())
                sourceRect.visible = true;
        }
        onDragFinished:
        {
          if (sourceRect.visible)
            sourceRect.visible = false;
        }
    }

    Rectangle
    {
        id: selectionRect
        anchors.fill: parent
        color: "#9fb1cb"
        visible: modelItem.isSelected();
    }

    Rectangle
    {
        id: sourceRect
        anchors.fill: parent
        color: "blue"
        border.width: 0
        visible: false;
        opacity: 0.2
    }


    Rectangle
    {
        id: dragRect
        color: "transparent"
        border.color: "blue"
        border.width: 2
        visible: false;
        z: 100;
        opacity: 0.6;
    }

    Rectangle
    {
        id: insertRect
        x: 0;
        z: 100;
        color: "transparent"
        border.color: "blue"
        border.width: 2
        visible: false;
        opacity: 0.6;
        height: 4;
    }

    MouseArea
    {
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton | Qt.RightButton

        onClicked:
        {
            if (mouse.button & Qt.RightButton)
            {
                dataServer.requestContextMenu(DataServer.LayersPanel, mouseX, mouseY);
            } else {
                if (mouse.modifiers & Qt.ControlModifier)
                {
                    dataServer.setSelected(modelItem.objectId(), !modelItem.isSelected(), DataServer.SelectAdd);
                } else {
                    if (mouse.modifiers & Qt.ShiftModifier)
                    {
                        dataServer.setSelected(modelItem.objectId(), !modelItem.isSelected(), DataServer.SelectBetween);
                    } else {
                        if (modelItem.isSelected())
                            modelItem.requestEditCaption();
                        else
                            dataServer.setSelected(modelItem.objectId(), true, DataServer.SelectOne);
                    }
                }
            }
        }
        property bool pressed: false;
        property var mouseY;

        property var target_model_item;

        property int insert_to: 0;

        onPressAndHold: {
            preventStealing = true;
            pressed = true;
            mouseY = mouse.y;

            dragRect.parent = layerRow.parent;
            insertRect.parent = layerRow.parent;
            dragRect.x = layerRow.x;
            dragRect.y = layerRow.y;

            dragRect.width = layerRow.width;
            dragRect.height = layerRow.height;
            dragRect.visible = true;
            sourceRect.visible = true;

            insertRect.width = layerRow.width-4;
            insertRect.x = 2;

            dataServer.notifyDragStarted();
        }

        onReleased:
        {
            if (!pressed)
                return;

            preventStealing = false;
            pressed = false;
            dragRect.visible = false
            sourceRect.visible = false;
            insertRect.visible = false;

            switch(insert_to)
            {
            case 1:
                dataServer.insertBefore(modelItem, target_model_item);
                break;
            case 2:
                dataServer.insertAfter(modelItem, target_model_item);
                break;
            case 3:
                dataServer.insertTo(modelItem, target_model_item);
                break;
            }
            dataServer.notifyDragFinished();
        }

        onPositionChanged:
        {
            if (!pressed)
                return;

            var drag_r_p = mapToItem(dragRect, mouse.x, mouse.y);
            dragRect.y += drag_r_p.y - mouseY;

            var view_p = mapToItem(layersView, mouse.x, mouse.y);

            view_p.y += layersView.contentY;
            var h_item = layerPanel.findHoveredRow(view_p.x, view_p.y);

            if (!h_item)
            {
                insertRect.visible = false;
                return;
            }

            var h_model_item = h_item.modelItem;

            var h_item_p = mapToItem(h_item, mouse.x, mouse.y);

            var h = h_item.height;

            var pos = h_item_p.y/h;
            var accepted = false;

            target_model_item = null;
            insert_to = 0;

            if (pos < 0.3)
            {
                var top = layerRow.parent.mapFromItem(h_item, 0, 0);
                insertRect.height = 4;
                insertRect.x = 2;
                insertRect.y = top.y - insertRect.height/2;
                insertRect.width = layerRow.width-4;
                target_model_item = h_model_item;

                if (modelItem.isTemplate() && h_model_item.isFreemesh() && !h_model_item.isClosed() ||
                    modelItem.isFreemesh() && h_model_item.isSection() && !h_model_item.isClosed())
                {
                    insert_to = 0;
                } else {
                    if ((modelItem.isTemplate() && (h_model_item.isFreemesh() || h_model_item.isTemplate())) ||
                        (modelItem.isFreemesh() && (h_model_item.isSection() || h_model_item.isFreemesh())) ||
                        (modelItem.isSection() && (h_model_item.isSection() ||
                        (h_model_item.isFreemesh() && modelItem.level() >= h_model_item.level()))))
                    {
                        insert_to = 2;
                        accepted = true;
                    }
                }

            } else
            if (pos > 0.7)
            {
                var bot = layerRow.parent.mapFromItem(h_item, 0, h_item.height);
                insertRect.height = 4;
                insertRect.x = 2;
                insertRect.y = bot.y - insertRect.height/2;
                insertRect.width = layerRow.width-4;
                target_model_item = h_model_item;
                if (modelItem.isTemplate() && h_model_item.isFreemesh() && !h_model_item.isClosed() ||
                    modelItem.isFreemesh() && h_model_item.isSection() && !h_model_item.isClosed())
                {
                    insert_to = 3;
                    accepted = true;
                } else {
                    if ((modelItem.isTemplate() && (h_model_item.isFreemesh() || h_model_item.isTemplate())) ||
                        (modelItem.isFreemesh() && (h_model_item.isSection() || h_model_item.isFreemesh())) ||
                        (modelItem.isSection() && (h_model_item.isSection() ||
                        (h_model_item.isFreemesh() && modelItem.level() >= h_model_item.level()))))
                    {
                        insert_to = 1;
                        accepted = true;
                    }
                }
            }

            if (!accepted)
            {
                if (modelItem.isTemplate() && h_model_item.isFreemesh() ||
                    modelItem.isFreemesh() && h_model_item.isSection() ||
                    modelItem.isSection() && h_model_item.isSection() && modelItem !== h_model_item)
                {
                    var cen = layerRow.parent.mapFromItem(h_item, 0, h_item.height/2);
                    insertRect.height = 4;
                    insertRect.width = layerRow.width-24;
                    insertRect.x = 16;
                    insertRect.y = cen.y - insertRect.height/2;
                    target_model_item = h_model_item;
                    insert_to = 3;
                    accepted = true;
                }
            }


            if (accepted)
            {
                if (!insertRect.visible)
                    insertRect.visible = true;
            } else {
                insertRect.visible = false;
            }
        }
    }

    Row
    {
        id: row
        spacing: 4
        anchors.fill: parent
        anchors.leftMargin: 17*(modelItem.level()-1);// + (modelItem.isCloneStrokes() ? 30 : 0);

        ExpanderButton
        {
            id: expanderButton
            anchors.verticalCenter: parent.verticalCenter

            onClosed:
            {
                isClosed = true;
                layerRow.closed();
            }

            onOpened:
            {
                isClosed = false;
                layerRow.opened();
            }
        }

        LayerItem
        {
            id: layerItem;
            anchors.verticalCenter: parent.verticalCenter
            modelItem: layerRow.modelItem;
            state: "SECTION"
        }

    }

    states: [
        State {
            name: "SHOWN"
            PropertyChanges { target: layerRow; height: 42; opacity: 1; enabled: true;}
        },
        State {
            name: "HIDDEN"
            PropertyChanges { target: layerRow; height: 0; opacity: 0; enabled: false;}
        }
    ]

    transitions: Transition {
        //NumberAnimation { properties: "height"; easing.type: Easing.InOutQuad; duration: 200 }
        //NumberAnimation { properties: "opacity"; easing.type: Easing.InOutQuad; duration: 200 }
    }
}
