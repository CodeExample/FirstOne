import QtQuick 2.0

import com.vectools.DataProvider 1.0

Item {
    id: dashesControl
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

            dashesSlider.enabled = dataProvider.getBoolValue(codeControl, DataProvider.DashEnabled);
            dashesSlider.inverted = dataProvider.getBoolValue(codeControl, DataProvider.DashInverted);
            dashesSlider.rulerColor = dataProvider.getVariantValue(codeControl, DataProvider.Color);
            dashesSlider.treshold = dataProvider.getIntValue(codeControl, DataProvider.DashTreshold);

            dashesRow.enabled = dataProvider.getBoolValue(codeControl, DataProvider.DashEnabled);
            dashesRow.treshold = dataProvider.getIntValue(codeControl, DataProvider.DashTreshold);
            dashesRow.dashMax = dataProvider.getDoubleValue(codeControl, DataProvider.DashMax);
            dashesRow.dashMin = dataProvider.getDoubleValue(codeControl, DataProvider.DashMin);
            dashesRow.spaceMax = dataProvider.getDoubleValue(codeControl, DataProvider.DashSpaceMax);
            dashesRow.spaceMin = dataProvider.getDoubleValue(codeControl, DataProvider.DashSpaceMin);
        }

        onUnselectAllPreview:
        {
            if (!ignoreEvents)
                selectionBox.selected = false;
        }

        onViewModeChanged:
        {
            dashesControl.state = getControlState();
        }
    }

    function doChangeEditMode()
    {
        if (!dashesSlider.enabled)
            return;

        if (dashesControl.state == "SLIDER")
        {
            dashesControl.state = "ROW";
            dashesControl.height = 64;
            dashesRow.fetchValues();
            itemHeightChanged();
        } else {
            dashesControl.state = "SLIDER";
            dashesControl.height = 32;
            itemHeightChanged();
            dashesRow.applyValues();
        }
    }


    SelectionBox
    {
        id: selectionBox;
        anchors.fill: parent;

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

    DashesSlider
    {
        id: dashesSlider
        anchors.left: parent.left;
        anchors.right: parent.right;
        anchors.verticalCenter: parent.verticalCenter

        enabled: dataProvider.getBoolValue(codeControl, DataProvider.DashEnabled) &&
                 dataProvider.getBoolValue(codeControl, DataProvider.IsEnabled);

        treshold: dataProvider.getIntValue(codeControl, DataProvider.DashTreshold);
        inverted: dataProvider.getBoolValue(codeControl, DataProvider.DashInverted);
        rulerColor: dataProvider.getVariantValue(codeControl, DataProvider.Color);

        onEnabledChanged:
        {
            if (enabled || dataProvider.getBoolValue(codeControl, DataProvider.IsEnabled))
            {
                dashesControl.opacity = 1;
            } else {
                dashesControl.opacity = 0.6;
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
            if (enabled)
            {
                dataProvider.setIntValue(codeControl, DataProvider.DashTreshold, treshold);
            }

            dataProvider.setBoolValue(codeControl, DataProvider.DashEnabled, enabled);

            dataProvider.notifyValuesChanged()
            dataProvider.editFinished()
        }
    }

    DashesRow
    {
        id: dashesRow
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
            enabled = dataProvider.getBoolValue(codeControl, DataProvider.DashEnabled);
            treshold = dataProvider.getIntValue(codeControl, DataProvider.DashTreshold);
            dashMax = dataProvider.getDoubleValue(codeControl, DataProvider.DashMax);
            dashMin = dataProvider.getDoubleValue(codeControl, DataProvider.DashMin);
            spaceMax = dataProvider.getDoubleValue(codeControl, DataProvider.DashSpaceMax);
            spaceMin = dataProvider.getDoubleValue(codeControl, DataProvider.DashSpaceMin);
            dashesRow.edgedDashes = dataProvider.getBoolValue(codeControl, DataProvider.EdgedDashesTemplate);
        }

        function applyValues()
        {
            if (!valueChanged)
                return;

            dataProvider.setIntValue(codeControl, DataProvider.DashTreshold, treshold);
            dataProvider.setDoubleValue(codeControl, DataProvider.DashMax, dashMax);
            dataProvider.setDoubleValue(codeControl, DataProvider.DashMin, dashMin);
            dataProvider.setDoubleValue(codeControl, DataProvider.DashSpaceMax, spaceMax);
            dataProvider.setDoubleValue(codeControl, DataProvider.DashSpaceMin, spaceMin);

            dataProvider.setBoolValue(codeControl, DataProvider.DashInverted, inverted);

            dataProvider.notifyValuesChanged()
            dataProvider.editFinished();
        }
    }

    states: [
        State {
            name: "SLIDER"

            PropertyChanges {
                target: dashesRow
                opacity: 0
            }

            PropertyChanges {
                target: dashesSlider
                opacity: 1
            }


        },
        State {
            name: "ROW"

            PropertyChanges {
                target: dashesRow
                opacity: 1
            }

            PropertyChanges {
                target: dashesSlider
                opacity: 0
            }
        }
    ]

    transitions: Transition {
        NumberAnimation { properties: "opacity"; easing.type: Easing.InOutQuad; duration: 200 }
    }


}
