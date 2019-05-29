import QtQuick 2.0

import com.vectools.DataProvider 1.0
import com.vectools.ModelItem 1.0

Item {
    id: layerPanel
    width: 200
    height: 362

    property ModelItem root;

    ListModel
    {
        id: rootModel
    }

    function updateModel()
    {
        while(rootModel.count > dataServer.modelItemsCount())
        {
            rootModel.remove(rootModel.count-1);
        }

        for(var i=0; i < dataServer.modelItemsCount(); ++i)
        {
            if (i >= rootModel.count)
            {
                rootModel.append({"visible": dataServer.modelItemAtIsShown(i),
                                   "level": dataServer.modelItemAtLevel(i),
                                   "modelItem": dataServer.modelItemAt(i)
                                 });
            } else {
                var item = rootModel.get(i);
                if (dataServer.modelItemAtObjectId(i) !== item.objectId)
                {
                    rootModel.set(i, {"visible": dataServer.modelItemAtIsShown(i),
                                      "level": dataServer.modelItemAtLevel(i),
                                      "modelItem": dataServer.modelItemAt(i)
                                  });
                }
            }
        }
    }


    Connections
    {
        target: dataServer;
        onModelInstalled:
        {
            rootModel.clear();
            updateModel();
        }

        onModelChanged:
        {
            updateModel();
            goSelected();
        }

        onSelectionChanged:
        {
            goSelected();
        }
    }

    focus: true;

    Keys.onPressed:
    {
        if (event.key === Qt.Key_Escape && dataServer.selectionCount() > 0)
        {
            dataServer.unselectAll();
            event.accepted = true;
        } else {
            event.accepted = false;
        }

        if (event.key === Qt.Key_Down && dataServer.selectionCount() > 0)
        {
            event.accepted = true;
            dataServer.selectNext(layersView.currentItem.modelItem, event.modifiers & Qt.ShiftModifier);
        } else {
            event.accepted = false;
        }

        if (event.key === Qt.Key_Up && dataServer.selectionCount() > 0)
        {
            event.accepted = true;
            dataServer.selectPrev(layersView.currentItem.modelItem, event.modifiers & Qt.ShiftModifier);
        } else {
            event.accepted = false;
        }

        if (event.key === Qt.Key_Right && dataServer.selectionCount() > 0)
        {
            event.accepted = true;
            var item = layersView.currentItem.modelItem;
            item.expand();

        } else {
            event.accepted = false;
        }

        if (event.key === Qt.Key_Left && dataServer.selectionCount() > 0)
        {
            event.accepted = true;
            var item = layersView.currentItem.modelItem;
            item.close();
            updateModel();
            goSelected();

        } else {
            event.accepted = false;
        }

    }

    function goSelected()
    {
        for(var i=0; i < rootModel.count; ++i)
        {
            var item = rootModel.get(i).modelItem;
            if (item && item.isSelected())
            {
                while(item.parentItem())
                {
                    item = item.parentItem();
                    item.expand();
                }
                layersView.currentIndex = i;
                break;
            }
        }
    }


    function hideChildrens(index, level)
    {
        for(var i=index+1; i < rootModel.count; ++i)
        {
            var item = rootModel.get(i);

            if (level >= item.level)
                break;

            item.visible = false;
        }
    }

    function showChildrens(index, level)
    {
        for(var i=index+1; i < rootModel.count; ++i)
        {
            var item = rootModel.get(i);

            if (item.level <= level)
                break;

            if (item.level === level+1)
            {
                item.visible = true;
            }
        }
    }



    function findHoveredRow(x, y)
    {
        var item = layersView.itemAt(x,y);
        return item;
    }

    ListView
    {
        id: layersView
        anchors.fill: parent
        model: rootModel
        //highlightMoveDuration: 10
        highlightFollowsCurrentItem: true
        //maximumFlickVelocity: 800
        boundsBehavior: Flickable.StopAtBounds

        delegate: LayerRow
        {
            modelItem: model.modelItem;
            isClosed: modelItem.isClosed();
            state: model.visible ? "SHOWN" : "HIDDEN"
            itemIndex: index;
            width: layersView.width


            onClosed:
            {
                modelItem.setClosed(true);
                hideChildrens(itemIndex, modelItem.level());
            }
            onOpened:
            {
                modelItem.setClosed(false);
                showChildrens(itemIndex, modelItem.level());
            }
        }
    }
}
