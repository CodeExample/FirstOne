import QtQuick 2.0

import com.vectools.DataProvider 1.0

Item {
    id: maskControl
    width: 302
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
            maskSlider.opacityMask = 100-dataProvider.getIntValue(codeControl, DataProvider.MaskOpacity);
            maskSlider.toleranceMask = dataProvider.getIntValue(codeControl, DataProvider.MaskTolerance);

            maskRow.opacityValue = 100-dataProvider.getIntValue(codeControl, DataProvider.MaskOpacity);
            maskRow.toleranceValue = dataProvider.getIntValue(codeControl, DataProvider.MaskTolerance);

            selectionBox.selected = dataProvider.getBoolValue(codeControl, DataProvider.SelectedPreview);
        }

        onUnselectAllPreview:
        {
            if (!ignoreEvents)
                selectionBox.selected = false;
        }

        onViewModeChanged:
        {
            maskControl.state = getControlState();
        }
    }


    function doChangeEditMode()
    {
        if (!maskSlider.enabled)
            return;

        if (maskControl.state == "SLIDER")
        {
            maskControl.state = "ROW";
            maskControl.height = 32;
            maskRow.fetchValues();
            itemHeightChanged();
        } else {
            maskControl.state = "SLIDER";
            maskControl.height = 32;
            itemHeightChanged();
            maskRow.applyValues();
        }
    }


    SelectionBox
    {
        id: selectionBox;
        anchors.fill: parent;
        selected: maskControl.selected;

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

    MaskSlider
    {
        id: maskSlider
        anchors.left: parent.left;
        anchors.right: parent.right;
        anchors.verticalCenter: parent.verticalCenter

        opacityMask: 100-dataProvider.getIntValue(codeControl, DataProvider.MaskOpacity);
        toleranceMask: dataProvider.getIntValue(codeControl, DataProvider.MaskTolerance);

        enabled: dataProvider.getBoolValue(codeControl, DataProvider.IsEnabled);

        onEnabledChanged:
        {
            if (enabled)
            {
                maskControl.opacity = 1;
            } else {
                maskControl.opacity = 0.6;
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
            //dataProvider.setBoolValue(codeControl, DataProvider.MaskOn, enabled);
            //dataProvider.setBoolValue(codeControl, DataProvider.MaskIsTrnsparent, opacityMask == 0);
            dataProvider.setIntValue(codeControl, DataProvider.MaskOpacity, 100-opacityMask);
            dataProvider.setIntValue(codeControl, DataProvider.MaskTolerance, toleranceMask);

            dataProvider.notifyValuesChanged()
            dataProvider.editFinished()
        }


    }

    MaskRow
    {
        id: maskRow
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
            //enabled = dataProvider.getBoolValue(codeControl, DataProvider.MaskOn);
            opacityValue = 100-dataProvider.getIntValue(codeControl, DataProvider.MaskOpacity);
            toleranceValue = dataProvider.getIntValue(codeControl, DataProvider.MaskTolerance);
        }


        function applyValues()
        {
            if (!valueChanged)
                return;

            //dataProvider.setBoolValue(codeControl, DataProvider.MaskIsTrnsparent, opacityValue == 0);
            dataProvider.setIntValue(codeControl, DataProvider.MaskOpacity, 100-opacityValue);
            dataProvider.setIntValue(codeControl, DataProvider.MaskTolerance, toleranceValue);

            dataProvider.notifyValuesChanged()
            dataProvider.editFinished()
        }

    }

    states: [
        State {
            name: "SLIDER"

            PropertyChanges {
                target: maskRow
                opacity: 0
            }

            PropertyChanges {
                target: maskSlider
                opacity: 1
            }


        },
        State {
            name: "ROW"

            PropertyChanges {
                target: maskRow
                anchors.rightMargin: 0
                anchors.leftMargin: 0
                opacity: 1
            }

            PropertyChanges {
                target: maskSlider
                opacity: 0
            }
        }
    ]

    transitions: Transition {
        NumberAnimation { properties: "opacity"; easing.type: Easing.InOutQuad; duration: 200 }
    }


}
