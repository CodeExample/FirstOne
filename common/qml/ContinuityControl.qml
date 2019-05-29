import QtQuick 2.0

import com.vectools.DataProvider 1.0

Item {
    id: continuityControl
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

    function fetchUpValue()
    {
        var inv = dataProvider.getBoolValue(codeControl, DataProvider.ContinuityInverted);
        var b = 255 - dataProvider.getDoubleValue(codeControl, DataProvider.ContinuityDownLimit);
        var w = 255 - dataProvider.getDoubleValue(codeControl, DataProvider.ContinuityUpLimit);
        if (inv)
            return b;
        return w;
    }

    function fetchDownValue()
    {
        var inv = dataProvider.getBoolValue(codeControl, DataProvider.ContinuityInverted);
        var b = 255 - dataProvider.getDoubleValue(codeControl, DataProvider.ContinuityDownLimit);
        var w = 255 - dataProvider.getDoubleValue(codeControl, DataProvider.ContinuityUpLimit);
        if (inv)
            return w;
        return b;
    }


    Connections
    {
        target: dataProvider

        onValuesChanged:
        {
            continuitySlider.blackValue = fetchDownValue();
            continuitySlider.whiteValue = fetchUpValue();
            continuityRow.blackValue = fetchDownValue();
            continuityRow.whiteValue = fetchUpValue();
            selectionBox.selected = dataProvider.getBoolValue(codeControl, DataProvider.SelectedPreview);
        }

        onUnselectAllPreview:
        {
            if (!ignoreEvents)
                selectionBox.selected = false;
        }

        onViewModeChanged:
        {
            doChangeEditMode();
        }

    }

    function doChangeEditMode()
    {
        if (!continuitySlider.enabled)
            return;

        if (continuityControl.state == "SLIDER")
        {
            continuityControl.state = "ROW";
            continuityRow.fetchValues();
            continuityControl.height = 64;
            itemHeightChanged();
        } else {
            continuityControl.state = "SLIDER";
            continuityControl.height = 32;
            itemHeightChanged();
            continuityRow.applyValues();
        }
    }

    SelectionBox
    {
        id: selectionBox;
        anchors.fill: parent;
        selected: continuityControl.selected;

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


    ContinutySlider
    {
        id: continuitySlider
        anchors.left: parent.left;
        anchors.right: parent.right;
        anchors.verticalCenter: continuityControl.verticalCenter

        blackValue: fetchDownValue();
        whiteValue: fetchUpValue();
        rulerColor: dataProvider.getVariantValue(codeControl, DataProvider.Color);

        enabled: dataProvider.getBoolValue(codeControl, DataProvider.IsEnabled);

        onEnabledChanged:
        {
            if (enabled)
            {
                continuityControl.opacity = 1;
            } else {
                continuityControl.opacity = 0.6;
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
            if (blackValue <= whiteValue)
            {
                dataProvider.setBoolValue(codeControl, DataProvider.ContinuityInverted, false);
                dataProvider.setDoubleValue(codeControl, DataProvider.ContinuityDownLimit, 255 - blackValue);
                dataProvider.setDoubleValue(codeControl, DataProvider.ContinuityUpLimit, 255 - whiteValue);
            } else {
                dataProvider.setBoolValue(codeControl, DataProvider.ContinuityInverted, true);
                dataProvider.setDoubleValue(codeControl, DataProvider.ContinuityDownLimit, 255 - whiteValue);
                dataProvider.setDoubleValue(codeControl, DataProvider.ContinuityUpLimit, 255 - blackValue);
            }
            dataProvider.notifyValuesChanged()
            dataProvider.editFinished()
        }


    }

    ContinuityRow
    {
        id: continuityRow
        anchors.top: parent.top
        anchors.topMargin: 3

        property bool valueChanged: false;
        anchors.right: parent.right
        anchors.rightMargin: 0
        anchors.left: parent.left
        anchors.leftMargin: 0

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
            blackValue = fetchDownValue();
            whiteValue = fetchUpValue();
            inverted = dataProvider.getBoolValue(codeControl, DataProvider.ContinuityInverted);
            breakStyle = dataProvider.getIntValue(codeControl, DataProvider.ContinuityBreakStyle);
        }

        function applyValues()
        {
            if (!valueChanged)
                return;

            if (blackValue <= whiteValue)
            {
                dataProvider.setDoubleValue(codeControl, DataProvider.ContinuityDownLimit, 255 - blackValue);
                dataProvider.setDoubleValue(codeControl, DataProvider.ContinuityUpLimit, 255 - whiteValue);
            } else {
                dataProvider.setDoubleValue(codeControl, DataProvider.ContinuityDownLimit, 255 - whiteValue);
                dataProvider.setDoubleValue(codeControl, DataProvider.ContinuityUpLimit, 255 - blackValue);
            }

            dataProvider.setBoolValue(codeControl, DataProvider.ContinuityInverted, inverted);
            dataProvider.setIntValue(codeControl, DataProvider.ContinuityBreakStyle, breakStyle);

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
                target: continuityRow
                opacity: 0
            }

            PropertyChanges {
                target: continuitySlider
                opacity: 1
            }


        },
        State {
            name: "ROW"

            PropertyChanges {
                target: continuityRow
                opacity: 1
            }

            PropertyChanges {
                target: continuitySlider
                opacity: 0
            }
        }
    ]

    transitions: Transition {
        NumberAnimation { properties: "opacity"; easing.type: Easing.InOutQuad; duration: 200 }
    }


}
