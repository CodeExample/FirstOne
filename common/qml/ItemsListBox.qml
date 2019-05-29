import QtQuick 2.0

import QtGraphicalEffects 1.0

Item
{
    id:comboBox

    width: 40
    height: 20

    state: "CLOSED"

    property variant model;
    property alias itemId: chosenItem.itemId;
    //property alias dropDownRectY: dropDownListView.y;
    property int selectedIndex: 0;
    property bool activeConnections: false;

    signal comboClicked;

    smooth: true;

    onSelectedIndexChanged:
    {
        updateSelectedItem();
    }

    onModelChanged:
    {
        updateSelectedItem();
    }


    function updateSelectedItem()
    {
        var i = selectedIndex;
        if (i < model.count)
        {
            if (model.get(i).itemText !== "")
            {
                chosenItemText.text = model.get(i).itemText;
                chosenItemText.visible = true;
                chosenItemImage.visible = false;
            } else {
                chosenItemImage.source = model.get(i).icon;
                chosenItemText.visible = false;
                chosenItemImage.visible = true;
            }

            chosenItem.itemId = model.get(i).itemId;
            dropDownListView.currentIndex = i;
        }
    }



    Rectangle
    {
        id: chosenItem
        radius:4;
        width:parent.width;
        height:comboBox.height;
        color: "#3f434f"
        smooth:true;
        border.width: 1;
        border.color: "#393d49"

        property int itemId: -1;//model.count > 0 ? model.get(0).itemId : -1;

        Image
        {
            id: chosenItemImage
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            source: "";//model.count > 0 ? model.get(0).icon : "";
            smooth: true
        }
        Text {
            font.pixelSize:  10
            x: 5
            verticalAlignment: Text.AlignVCenter
            color: "#babfc9"
            id: chosenItemText
            text: ""
            anchors.verticalCenter: parent.verticalCenter
        }

        Connections
        {
            target: dropDown;

            onChosedItem:
            {
                if (activeConnections)
                {
                    comboBox.selectedIndex = index;
                    activeConnections = false;
                    comboClicked();
                }
            }
        }

        MouseArea
        {
            anchors.fill: parent;
            hoverEnabled: true;

            onClicked:
            {
                var g = mapToItem(groupPanel, 0,0);
                dropDownListView.model = model;
                dropDownListView.currentIndex = selectedIndex;
                dropDown.height = 20*model.count;
                dropDown.x = g.x;
                dropDown.y = g.y-10*model.count+10;
                dropDown.width = comboBox.width;
                dropDown.opacity = 1;
                dropDown.visible = true;
                dropDownListView.forceActiveFocus();

                activeConnections = true;
            }
        }
    }

}
