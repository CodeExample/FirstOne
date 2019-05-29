import QtQuick 2.0

import com.vectools.DataProvider 1.0

Item {
    id: styleControl
    width: 200
    height: 32
    state: getControlState()

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

            styleSlider.terminalNodes = dataProvider.getDoubleValue(codeControl, DataProvider.TerminalNodes);
            styleSlider.smoothThickness = dataProvider.getIntValue(codeControl, DataProvider.SmoothThickness);
            styleSlider.rulerColor = dataProvider.getVariantValue(codeControl, DataProvider.Color);

            styleRow.terminalNodes = dataProvider.getDoubleValue(codeControl, DataProvider.TerminalNodes);
            styleRow.smoothThickness = dataProvider.getIntValue(codeControl, DataProvider.SmoothThickness);
            styleRow.thicknessCurvature = dataProvider.getIntValue(codeControl, DataProvider.ThicknessCurvature);
            styleRow.curveCup = dataProvider.getIntValue(codeControl, DataProvider.CurveCup);
            styleRow.thickCup = dataProvider.getIntValue(codeControl, DataProvider.ThickCup);

            styleRow.rulerColor = dataProvider.getVariantValue(codeControl, DataProvider.Color);
        }

        onUnselectAllPreview:
        {
            if (!ignoreEvents)
                selectionBox.selected = false;
        }

        onViewModeChanged:
        {
            styleControl.state = getControlState();
        }
    }


    function doChangeEditMode()
    {
        if (!styleSlider.enabled)
            return;

        if (styleControl.state == "SLIDER")
        {
            styleControl.state = "ROW";
            styleControl.height = 64;
            styleRow.fetchValues();
            itemHeightChanged();
        } else {
            styleControl.state = "SLIDER";
            styleControl.height = 32;
            itemHeightChanged();
            styleRow.applyValues();
        }
    }


    SelectionBox
    {
        id: selectionBox;
        anchors.fill: parent;
        selected: styleControl.selected;

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

    StyleSlider
    {
        id: styleSlider
        anchors.left: parent.left;
        anchors.right: parent.right;
        anchors.verticalCenter: parent.verticalCenter;

        terminalNodes: dataProvider.getDoubleValue(codeControl, DataProvider.TerminalNodes);
        maxValue: dataProvider.getDoubleValue(codeControl, DataProvider.TerminalNodesMax);
        smoothThickness: dataProvider.getIntValue(codeControl, DataProvider.SmoothThickness);
        rulerColor: dataProvider.getVariantValue(codeControl, DataProvider.Color);

        enabled: dataProvider.getBoolValue(codeControl, DataProvider.IsEnabled);

        onEnabledChanged:
        {
            if (enabled)
            {
                styleControl.opacity = 1;
            } else {
                styleControl.opacity = 0.6;
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
            dataProvider.setDoubleValue(codeControl, DataProvider.TerminalNodes, styleSlider.terminalNodes);
            dataProvider.setIntValue(codeControl, DataProvider.SmoothThickness, styleSlider.smoothThickness);

            dataProvider.notifyValuesChanged()
            dataProvider.editFinished()
        }

    }

    StyleRow
    {
        id: styleRow
        anchors.horizontalCenter: styleControl.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 3
        terminalNodesMax: dataProvider.getDoubleValue(codeControl, DataProvider.TerminalNodesMax);

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
            terminalNodes = dataProvider.getDoubleValue(codeControl, DataProvider.TerminalNodes);
            smoothThickness = dataProvider.getIntValue(codeControl, DataProvider.SmoothThickness);
            thicknessCurvature = dataProvider.getIntValue(codeControl, DataProvider.ThicknessCurvature);
            curveCup = dataProvider.getIntValue(codeControl, DataProvider.CurveCup);
            thickCup = dataProvider.getIntValue(codeControl, DataProvider.ThickCup);
            rulerColor = dataProvider.getVariantValue(codeControl, DataProvider.Color);
        }

        function applyValues()
        {
            if (!valueChanged)
                return;

            dataProvider.setDoubleValue(codeControl, DataProvider.TerminalNodes, styleRow.terminalNodes);
            dataProvider.setIntValue(codeControl, DataProvider.SmoothThickness, styleRow.smoothThickness);
            dataProvider.setIntValue(codeControl, DataProvider.ThicknessCurvature, styleRow.thicknessCurvature);
            dataProvider.setIntValue(codeControl, DataProvider.CurveCup, styleRow.curveCup);
            dataProvider.setIntValue(codeControl, DataProvider.ThickCup, styleRow.thickCup);

            dataProvider.notifyValuesChanged()
            dataProvider.editFinished()
        }

    }

    states: [
        State {
            name: "SLIDER"

            PropertyChanges {
                target: styleRow
                opacity: 0
            }

            PropertyChanges {
                target: styleSlider
                opacity: 1
            }


        },
        State {
            name: "ROW"

            PropertyChanges {
                target: styleRow
                anchors.rightMargin: 0
                anchors.leftMargin: 49
                opacity: 1
            }

            PropertyChanges {
                target: styleSlider
                opacity: 0
            }
        }
    ]

    transitions: Transition {
        NumberAnimation { properties: "opacity"; easing.type: Easing.InOutQuad; duration: 200 }
    }


}
