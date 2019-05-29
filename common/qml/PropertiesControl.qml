import QtQuick 2.0

import com.vectools.DataProvider 1.0

Item
{
    id: propertiesControl
    width: 200
    height: 280
    property DataProvider dataProvider;
    property variant codeControl;

    property bool selected: false;
    property bool ignoreEvents: false;

    signal editingStarted();
    signal editingFinished();

    function getPropertiesCount()
    {
        var properties_cnt = dataProvider.getPropertiesCount(codeControl);
        var has_halftone = false;
        var has_editor_widget = false;
        for(var i=0; i < properties_cnt; ++i)
        {
            var prop_id = dataProvider.getPropertyId(codeControl, i);
            switch(prop_id)
            {
            case DataProvider.HalftoneItems:
                has_halftone = true;
                --properties_cnt;
                break;
            case DataProvider.EditorWidget:
                has_editor_widget = true;
                --properties_cnt;
                break;
            }
        }

        halftoneItemsRect.visible = false;
        editorWidgetRect.visible = false;
        if (has_halftone)
        {
            halftoneItemsList.objectId = dataProvider.getObjectId(codeControl);
            halftoneItemsRect.visible = true;
        } else
        if (has_editor_widget)
        {
            editorWidgetRect.objectId = dataProvider.getObjectId(codeControl);
            editorWidgetRect.visible = true;
        }

        return properties_cnt;
    }

    Connections
    {
        target: dataProvider
        onValuesChanged:
        {
            selectionBox.selected = dataProvider.getBoolValue(codeControl, DataProvider.SelectedPreview);
        }

        onUnselectAllPreview:
        {
            if (!ignoreEvents)
                selectionBox.selected = false;
        }

        onTemplatesChanged:
        {
            propertiesRepetear.model = getPropertiesCount(codeControl);
        }
    }


    SelectionBox
    {
        id: selectionBox;
        anchors.fill: parent;
        anchors.bottomMargin: 6;
        selected: propertiesControl.selected;

        onSelectedChanged:
        {
            ignoreEvents = true;
            dataProvider.setBoolValue(codeControl, DataProvider.SelectedPreview, selected);
            ignoreEvents = false;
        }
    }


    Rectangle
    {
        id: rectangle1
        width: 180;
        anchors.top: parent.top
        anchors.topMargin: 10
        anchors.horizontalCenter: parent.horizontalCenter;
        color: "transparent"

        enabled: dataProvider.getBoolValue(codeControl, DataProvider.IsEnabled);

        onEnabledChanged:
        {
            if (enabled)
            {
                propertiesControl.opacity = 1;
            } else {
                propertiesControl.opacity = 0.6;
            }
        }


        Flow
        {
            id: flowItem
            spacing: 10
            anchors.fill: parent
            Repeater
            {
                id: propertiesRepetear
                model: getPropertiesCount();
                PropertyEdit
                {
                    dataProvider: propertiesControl.dataProvider;
                    codeControl: propertiesControl.codeControl;
                    propertyId: propertiesControl.dataProvider.getPropertyId(propertiesControl.codeControl, index);
                }
            }
        }
    }

    Rectangle
    {
        id: halftoneItemsRect;
        width: 152
        height: 32
        visible: false;
        anchors.top: parent.top
        anchors.topMargin: 110
        anchors.horizontalCenterOffset: 0
        anchors.horizontalCenter: parent.horizontalCenter
        radius: 5
        color: "#404451"
        smooth:true;
        border.width: 1;
        border.color: "#393d49"

        HalftoneItemsList
        {
            anchors.fill: parent
            id: halftoneItemsList;
            x: 40
            anchors.topMargin: 4
            anchors.bottomMargin: 4
            anchors.leftMargin: 4
            anchors.rightMargin: 4
        }
    }

    Rectangle
    {
        id: editorWidgetRect;

        width: 152
        height: 32
        visible: false;
        anchors.top: parent.top
        anchors.topMargin: 110
        anchors.horizontalCenterOffset: 0
        anchors.horizontalCenter: parent.horizontalCenter
        radius: 5
        color: "#404451"
        smooth:true;
        border.width: 1;
        border.color: "#393d49"
        property int objectId;
        Text {
            id: name
            text: "Edit"
            anchors.fill: parent;
            color: "lightgray"
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }
        MouseArea
        {
            hoverEnabled: true;
            anchors.fill: parent;
            onEntered:
            {
              editorWidgetRect.color = "#3a3d4b"
            }
            onPressed:
            {
              editorWidgetRect.color = "#303441"
            }
            onReleased:
            {
              editorWidgetRect.color = "#383b49"
            }
            onExited:
            {
              editorWidgetRect.color = "#404451"
            }
            onClicked:
            {
                //console.log("clicked", editorWidgetRect.objectId);
                dataProvider.showCustomTemplateWidget(propertiesControl.codeControl);
            }
        }
    }

}
