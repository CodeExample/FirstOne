import QtQuick 2.0

import com.vectools.DataProvider 1.0
import com.vectools.DataServer 1.0

Item {
    id: colorControl
    width: 200
    height: 32
    state: getControlState()

    property DataProvider dataProvider;
    property variant codeControl;

    property bool selected: false;
    property bool ignoreEvents: false;

    signal editingStarted();
    signal editingFinished();

    function getControlState()
    {
        var viewMode = dataProvider.getControlViewMode();
        switch(viewMode)
        {
        case 1:
            return "SLIDER";
        case 2:
            return "ROW";
        }
        return "SLIDER"
    }


    Connections
    {
        target: dataProvider
        onValuesChanged:
        {
            colorSlider.colorTemplate = dataProvider.getVariantValue(codeControl, DataProvider.Color);
            colorSlider.colorMode = dataProvider.getIntValue(codeControl, DataProvider.ColorMode);

            colorRow.colorTemplate = dataProvider.getVariantValue(codeControl, DataProvider.Color);

            selectionBox.selected = dataProvider.getBoolValue(codeControl, DataProvider.SelectedPreview);
        }

        onUnselectAllPreview:
        {
            if (!ignoreEvents)
                selectionBox.selected = false;
        }

        onViewModeChanged:
        {
            colorControl.state = getControlState();
        }
    }


    SelectionBox
    {
        id: selectionBox;
        anchors.fill: parent;
        selected: colorControl.selected;

        onSelectedChanged:
        {
            ignoreEvents = true;
            dataProvider.setBoolValue(codeControl, DataProvider.SelectedPreview, selected);
            ignoreEvents = false;
        }

        onChangeEditMode:
        {
        /*    if (!colorSlider.enabled)
                return;

            if (parent.state == "SLIDER")
            {
                parent.state = "ROW";
                colorRow.fetchValues();
            } else {
                parent.state = "SLIDER";
                colorRow.applyValues();
            }*/
        }
    }

    ColorSlider
    {
        id: colorSlider
        anchors.left: parent.left;
        anchors.right: parent.right;
        anchors.verticalCenter: parent.verticalCenter;

        colorTemplate: dataProvider.getVariantValue(codeControl, DataProvider.Color);
        colorMode: dataProvider.getIntValue(codeControl, DataProvider.ColorMode);

        enabled: dataProvider.getBoolValue(codeControl, DataProvider.IsEnabled);

        onEnabledChanged:
        {
            if (enabled)
            {
                colorControl.opacity = 1;
            } else {
                colorControl.opacity = 0.6;
            }
        }

        onOpacityChanged:
        {
            if (opacity == 0)
                visible = false;
            else
                visible = true;
        }

        onEditingStarted:
        {
            dataProvider.editStarted();
        }

        onEditingFinished:
        {
            //dataProvider.setVariantValue(codeControl, DataProvider.Color, colorSlider.colorTemplate);
            if (colorSlider.colorMode != 2)
            {
                dataProvider.setIntValue(codeControl, DataProvider.ColorMode, colorSlider.colorMode);
            } else {
                var panel_p = colorSlider.mapToItem(null, 0,0);
                dataProvider.changeColors(codeControl, DataServer.Panel, panel_p.x, panel_p.y);
            }

            dataProvider.notifyValuesChanged()
            dataProvider.editFinished()
        }

    }

    ColorRow
    {
        id: colorRow
        anchors.horizontalCenter: colorControl.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 3


        onOpacityChanged:
        {
            if (opacity == 0)
                visible = false;
            else
                visible = true;
        }

        property bool valueChanged: false;

        onEditingStarted:
        {
            dataProvider.editStarted();
        }

        onEditingFinished:
        {
            valueChanged = true;
        }

        function fetchValues()
        {
            colorTemplate: dataProvider.getVariantValue(codeControl, DataProvider.Color);
            colorMode: dataProvider.getIntValue(codeControl, DataProvider.ColorMode);
        }

        function applyValues()
        {
            if (!valueChanged)
                return;
            //dataProvider.setVariantValue(codeControl, DataProvider.Color, colorRow.colorTemplate);
            //dataProvider.setIntValue(codeControl, DataProvider.ColorMode, colorRow.colorMode);

            dataProvider.notifyValuesChanged()
            dataProvider.editFinished()
        }

    }

    states: [
        State {
            name: "SLIDER"

            PropertyChanges {
                target: colorRow
                opacity: 0
            }

            PropertyChanges {
                target: colorSlider
                opacity: 1
            }


        },
        State {
            name: "ROW"

            PropertyChanges {
                target: colorRow
                opacity: 1
            }

            PropertyChanges {
                target: colorSlider
                opacity: 0
            }
        }
    ]

    transitions: Transition {
        NumberAnimation { properties: "opacity"; easing.type: Easing.InOutQuad; duration: 200 }
    }


}
