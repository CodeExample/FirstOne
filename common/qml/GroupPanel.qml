import QtQuick 2.0

import com.vectools.DataProvider 1.0

Item {
    id: groupPanel

    width: 100
    height: 62

    Rectangle
    {
        anchors.fill: parent;
        color: "#434650"
    }

    ListModel
    {
        id: listModel;
    }


    Component.onCompleted:
    {
        for(var i=0; i < dataServer.getPanelsCount(); ++i)
        {
            listModel.append({"provider": dataServer.getDataProvider(i)});
        }
    }


    Rectangle
    {
        id: contentRect

        anchors.fill: parent;
        color: "#434650"

        anchors.topMargin: 10
        anchors.leftMargin: 2
        anchors.rightMargin: 2

        ListView
        {
            id: listView;
            cacheBuffer: 1000;
            anchors.fill: parent;
            model: listModel;
            focus: false;

            delegate: CombinedPanel
            {
              dataProvider: model.provider;
              width: groupPanel.width;
              onBeginDrag:
              {
                  listView.interactive = false;
              }
              onEndDrag:
              {
                  listView.interactive = true;
              }
            }
        }
    }

    Keys.onPressed:
    {
        if (event.key === Qt.Key_Escape && dropDown.visible)
        {
            dropDown.visible = false;
            dropDown.chosedItem(dropDown.dropDownListView.currentItem);
            event.accepted = true;
        }
    }

    Rectangle
    {
      id: dropDown
      clip: true;
      radius: 4;
      y: 100;
      x: 20;
      anchors.margins: 2;
      color: "#3f434f"

      height:0;
      opacity: 0

      border.width: 1;
      border.color: "#393d49"
      property alias listModel: dropDownListView.model;
      property int itemHeight: 20;

      signal chosedItem(int index);


    FocusScope
    {
        anchors.fill: parent;

        onActiveFocusChanged:
        {
            if (!activeFocus)
            {
                dropDown.visible = false;
                //dropDown.chosedItem(index);
            }
        }

        Component
        {
            id: highlight
            Rectangle
            {
                color: "#677992";
                radius: 4
            }
        }

        ListView
        {
            id: dropDownListView
            height:500;
            highlightFollowsCurrentItem: true
            highlight: highlight
            highlightMoveDuration : 0
            keyNavigationWraps: true

            anchors.fill: parent;

            model: ListModel {}


            delegate: Item
            {
                width: dropDown.width;
                height: dropDown.itemHeight;

                Image
                {
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.margins: 1;
                    source: icon;
                    visible: itemText == "";
                }
                Text
                {
                    anchors.verticalCenter: parent.verticalCenter
                    font.pixelSize:  10
                    x: 5
                    color: "white"
                    text: itemText;
                    visible: itemText != "";
                }


                MouseArea
                {
                    id: chimv
                    anchors.fill: parent;

                    onClicked:
                    {
                        dropDown.visible = false;
                        dropDown.chosedItem(index);
                    }
                }
            }
        }
        MouseArea
        {
            anchors.fill: parent;
            hoverEnabled: true;
            propagateComposedEvents: true;

            onClicked:
            {
                mouse.accepted = false;
            }
        }
    }
}

}
