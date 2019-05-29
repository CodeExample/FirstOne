import QtQuick 2.0

import com.vectools.DataProvider 1.0

Item {
    id: halftoneListView;

    width: 120
    height: 32

    property int objectId;

    onObjectIdChanged:
    {
        updateModel();
    }

    ListModel
    {
        id: listModel;
    }

    function updateModel()
    {
        var modelItem = dataServer.modelItemByObjectId(objectId);
        var n = modelItem.halftoneItemsCount();
        listModel.clear();
        var icon_pref = "image://previews/hfitem-" + objectId;
        for(var i=0; i < n; ++i)
        {
            var item_id = modelItem.halftoneItemIdAt(i);

            var icon_href = icon_pref + "-" + item_id + "-" + i;

            listModel.append({
                              "index": i,
                              "icon": icon_href
                             });
        }

        listModel.append({
                          "index": -1,
                          "icon": ""
                         });

        listView.model = listModel;
        listView.currentIndex = -1;
    }

    Keys.onPressed:
    {
        if ((listView.currentIndex >= 0) &&
            (event.key === Qt.Key_Backspace ||
             event.key === Qt.Key_Delete))
        {
            dataServer.removeHalftoneItem(listView.currentIndex, objectId);
        }
    }


    ListView
    {
        id: listView;
        spacing: 2
        anchors.fill: parent;
        orientation: ListView.Horizontal;
        highlight: Rectangle { color: "#677992"; radius: 3}
        focus: true
        currentIndex: -1

        delegate: Rectangle
        {
            width: 24
            height: 24
            color: "transparent"

            MouseArea
            {
                anchors.fill: parent
                onClicked:
                {
                    listView.currentIndex = index;
                    halftoneListView.forceActiveFocus();
                }
            }

            Rectangle
            {
                visible: model.index >= 0;
                anchors.fill: parent;
                anchors.topMargin: 1;
                anchors.bottomMargin: 1;
                anchors.leftMargin: 1;
                anchors.rightMargin: 1;
                color: "white"
                radius: 4
                opacity: 0.6
            }

            Image
            {
                visible: model.index >= 0;
                anchors.fill: parent
                anchors.topMargin: 2;
                anchors.bottomMargin: 2;
                anchors.leftMargin: 2;
                anchors.rightMargin: 2;
                source: icon;
            }

            AddHalftoneItemButton
            {
                visible: model.index < 0;
                objectId: halftoneListView.objectId;
                onButtonClcked:
                {
                    dataServer.requestAddHalftoneItemTo(listView.currentIndex, objectId);
                }
            }

        }
    }
}
