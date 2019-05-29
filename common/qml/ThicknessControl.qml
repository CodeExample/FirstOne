import QtQuick 2.0

import com.vectools.DataProvider 1.0

Item {
    id: thicknessControl
    width: 200
    height: 32
    state: getControlState();

    property DataProvider dataProvider;
    property variant codeControl;

    property bool selected: false;
    property bool ignoreEvents: false;

    signal editingStarted();
    signal editingFinished();
    signal itemHeightChanged();

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
            selectionBox.selected = dataProvider.getBoolValue(codeControl, DataProvider.SelectedPreview);

            thicknessSlider.thickValue = dataProvider.getDoubleValue(codeControl, DataProvider.Thickness);

            thicknessSlider.baseValue = dataProvider.getDoubleValue(codeControl, DataProvider.BaseThickness);

            thicknessSlider.rulerColor = dataProvider.getVariantValue(codeControl, DataProvider.Color);
            thicknessSlider.inverted = dataProvider.getBoolValue(codeControl, DataProvider.ThicknessInverted);

        }
        onUnselectAllPreview:
        {
            if (!ignoreEvents)
                selectionBox.selected = false;
        }

        onViewModeChanged:
        {
            thicknessControl.state = getControlState();
        }
    }

    function doChangeEditMode()
    {
        if (!thicknessSlider.enabled)
            return;

        if (thicknessControl.state == "SLIDER")
        {
            thicknessControl.state = "ROW";
            thicknessControl.height = 32;
            thicknessRow.fetchValues();
            itemHeightChanged();
        } else {
            thicknessControl.state = "SLIDER";
            thicknessControl.height = 32;
            thicknessRow.applyValues();
            itemHeightChanged();
        }
    }

    SelectionBox
    {
        id: selectionBox;
        anchors.fill: parent;
        selected: thicknessControl.selected;

        onSelectedChanged:
        {
            ignoreEvents = true;
            dataProvider.setBoolValue(codeControl, DataProvider.SelectedPreview, selected);
            ignoreEvents = false;
        }
        onChangeEditMode:
        {
            doChangeEditMode();
        }
    }


    ThicknessSlider
    {
        id: thicknessSlider
        anchors.right: parent.right
        anchors.left: parent.left
        anchors.verticalCenter: thicknessControl.verticalCenter

        minValue: dataProvider.getDoubleValue(codeControl, DataProvider.ThicknessMinValue);
        maxValue: dataProvider.getDoubleValue(codeControl, DataProvider.ThicknessMaxValue);
        thickValue: dataProvider.getDoubleValue(codeControl, DataProvider.Thickness);
        baseValue: dataProvider.getDoubleValue(codeControl, DataProvider.BaseThickness);
        rulerColor: dataProvider.getVariantValue(codeControl, DataProvider.Color);
        inverted:  dataProvider.getBoolValue(codeControl, DataProvider.ThicknessInverted);

        enabled: dataProvider.getBoolValue(codeControl, DataProvider.IsEnabled);

        onEnabledChanged:
        {
            if (enabled)
            {
                thicknessControl.opacity = 1;
            } else {
                thicknessControl.opacity = 0.6;
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
            dataProvider.setDoubleValue(codeControl, DataProvider.Thickness, thickValue);
            dataProvider.setDoubleValue(codeControl, DataProvider.BaseThickness, baseValue);

            dataProvider.notifyValuesChanged()
            dataProvider.editFinished()
        }

    }

    ThicknessRow
    {
        id: thicknessRow
        anchors.verticalCenter: thicknessControl.verticalCenter

        property bool valueChanged: false;
        anchors.left: parent.left
        anchors.leftMargin: 0
        anchors.right: parent.right
        anchors.rightMargin: 0

        onEditingStarted:
        {
            dataProvider.editStarted();
        }

        onEditingFinished:
        {
            valueChanged = true;
        }
        onChangesApply:
        {
            doChangeEditMode();
        }

        function fetchValues()
        {
            rulerColor = dataProvider.getVariantValue(codeControl, DataProvider.Color);
            inverted = dataProvider.getBoolValue(codeControl, DataProvider.ThicknessInverted);
            thickValue = dataProvider.getDoubleValue(codeControl, DataProvider.Thickness);
            baseValue = dataProvider.getDoubleValue(codeControl, DataProvider.BaseThickness);

            thickMinValue = dataProvider.getDoubleValue(codeControl, DataProvider.ThicknessMinValue);
            thickMaxValue = dataProvider.getDoubleValue(codeControl, DataProvider.ThicknessMaxValue);
            baseMinValue = dataProvider.getDoubleValue(codeControl, DataProvider.ThicknessMinValue);
            baseMaxValue = dataProvider.getDoubleValue(codeControl, DataProvider.ThicknessMaxValue);
        }

        function applyValues()
        {
            if (!valueChanged)
                return;

            dataProvider.setDoubleValue(codeControl, DataProvider.Thickness, thickValue);
            dataProvider.setDoubleValue(codeControl, DataProvider.BaseThickness, baseValue);
            dataProvider.setBoolValue(codeControl, DataProvider.ThicknessInverted, inverted);

            dataProvider.notifyValuesChanged()
            dataProvider.editFinished();
        }



        onOpacityChanged:
        {
            if (opacity == 0)
                visible = false;
            else
                visible = true;
        }
    }

    states: [
        State {
            name: "SLIDER"

            PropertyChanges {
                target: thicknessRow
                opacity: 0
            }

            PropertyChanges {
                target: thicknessSlider
                y: 24
                anchors.horizontalCenterOffset: 8
                anchors.leftMargin: 0
                opacity: 1
            }

            PropertyChanges {
                target: selectionBox
                width: 3024
            }


        },
        State {
            name: "ROW"

            PropertyChanges {
                target: thicknessRow
                anchors.topMargin: 0
                opacity: 1
            }

            PropertyChanges {
                target: thicknessSlider
                opacity: 0
            }
        }
    ]

    transitions: Transition {
        NumberAnimation { properties: "opacity"; easing.type: Easing.InOutQuad; duration: 200 }
    }


}
