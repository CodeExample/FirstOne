import QtQuick 2.0

import com.vectools.DataProvider 1.0

Item {
    id: basreliefControl
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

            basreliefSlider.enabled = dataProvider.getBoolValue(codeControl, DataProvider.BasreliefEnabled);
            basreliefSlider.rise = dataProvider.getDoubleValue(codeControl, DataProvider.BasreliefRise);
            basreliefSlider.smooth = dataProvider.getIntValue(codeControl, DataProvider.BasreliefSmooth);
            basreliefSlider.front = dataProvider.getIntValue(codeControl, DataProvider.BasreliefFront);
            basreliefSlider.rulerColor = dataProvider.getVariantValue(codeControl, DataProvider.Color);

            basreliefRow.enabled = dataProvider.getBoolValue(codeControl, DataProvider.BasreliefEnabled);
            basreliefRow.rise = dataProvider.getDoubleValue(codeControl, DataProvider.BasreliefRise);
            basreliefRow.smooth = dataProvider.getIntValue(codeControl, DataProvider.BasreliefSmooth);
            basreliefRow.front = dataProvider.getIntValue(codeControl, DataProvider.BasreliefFront);
            basreliefRow.trembling = dataProvider.getDoubleValue(codeControl, DataProvider.BasreliefRandom);
        }

        onUnselectAllPreview:
        {
            if (!ignoreEvents)
                selectionBox.selected = false;
        }

        onViewModeChanged:
        {
            basreliefControl.state = getControlState();
        }
    }

    function doChangeEditMode()
    {
        if (!basreliefSlider.enabled)
            return;

        if (basreliefControl.state == "SLIDER")
        {
            basreliefControl.state = "ROW";
            basreliefControl.height = 64;
            basreliefRow.fetchValues();
            itemHeightChanged();
        } else {
            basreliefControl.state = "SLIDER";
            basreliefControl.height = 32;
            itemHeightChanged();
            basreliefRow.finishEditing();
            basreliefRow.applyValues();
        }
    }

    SelectionBox
    {
        id: selectionBox;
        anchors.fill: parent;
        selected: basreliefControl.selected;

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


    property int frontValue: 0;

    onFrontValueChanged:
    {
        basreliefSlider.front = frontValue;
    }


    BasreliefSlider
    {
        id: basreliefSlider
        anchors.left: parent.left;
        anchors.right: parent.right;
        anchors.verticalCenter: parent.verticalCenter

        enabled: dataProvider.getBoolValue(codeControl, DataProvider.BasreliefEnabled) &&
                 dataProvider.getBoolValue(codeControl, DataProvider.IsEnabled);
        rise: dataProvider.getDoubleValue(codeControl, DataProvider.BasreliefRise);
        smooth: dataProvider.getIntValue(codeControl, DataProvider.BasreliefSmooth);
        front: dataProvider.getIntValue(codeControl, DataProvider.BasreliefFront);
        rulerColor: dataProvider.getVariantValue(codeControl, DataProvider.Color);

        minRise: -2
        maxRise: 2

        minBalloonValue: -10
        maxBalloonValue: 10


        onEnabledChanged:
        {
            if (enabled || dataProvider.getBoolValue(codeControl, DataProvider.IsEnabled))
            {
                basreliefControl.opacity = 1;
            } else {
                basreliefControl.opacity = 0.6;
            }
        }

        onOpacityChanged:
        {
            if (opacity < 1)
                visible = false;
            else
                visible = true;
        }        

        onEditingStarted:
        {
            dataProvider.editStarted();
        }

        onRiseChanged:
        {
            if (Math.abs(rise) > maxRise)
            {
                maxRise = Math.ceil(rise) + 1
                minRise = -Math.ceil(rise)- 1
            }

        }

        onEditingFinished:
        {
            if (enabled)
            {
                dataProvider.setDoubleValue(codeControl, DataProvider.BasreliefRise, rise);
                dataProvider.setIntValue(codeControl, DataProvider.BasreliefSmooth, smooth);
                dataProvider.setIntValue(codeControl, DataProvider.BasreliefFront, front);
            }

            dataProvider.setBoolValue(codeControl, DataProvider.BasreliefEnabled, enabled);

            dataProvider.notifyValuesChanged()
            dataProvider.editFinished()
        }
    }

    BasreliefRow
    {
        id: basreliefRow
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
            enabled = dataProvider.getBoolValue(codeControl, DataProvider.BasreliefEnabled);
            trembling = dataProvider.getDoubleValue(codeControl, DataProvider.BasreliefRandom);
            rise = dataProvider.getDoubleValue(codeControl, DataProvider.BasreliefRise);
            smooth = dataProvider.getIntValue(codeControl, DataProvider.BasreliefSmooth);
            front = dataProvider.getIntValue(codeControl, DataProvider.BasreliefFront);
            rulerColor = dataProvider.getVariantValue(codeControl, DataProvider.Color);
        }


        function applyValues()
        {
            if (!valueChanged)
                return;

            dataProvider.setDoubleValue(codeControl, DataProvider.BasreliefRise, rise);
            dataProvider.setIntValue(codeControl, DataProvider.BasreliefSmooth, smooth);
            dataProvider.setIntValue(codeControl, DataProvider.BasreliefFront, front);
            dataProvider.setDoubleValue(codeControl, DataProvider.BasreliefRandom, trembling);

            dataProvider.notifyValuesChanged()
            dataProvider.editFinished()
        }

    }

    states: [
        State {
            name: "SLIDER"

            PropertyChanges {
                target: basreliefRow
                opacity: 0
            }

            PropertyChanges {
                target: basreliefSlider
                opacity: 1
            }


        },
        State {
            name: "ROW"

            PropertyChanges {
                target: basreliefRow
                opacity: 1
            }

            PropertyChanges {
                target: basreliefSlider
                opacity: 0
            }
        }
    ]

    transitions: Transition {
        NumberAnimation { properties: "opacity"; easing.type: Easing.InOutQuad; duration: 200 }
    }


}
