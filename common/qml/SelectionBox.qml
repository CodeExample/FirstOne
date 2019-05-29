import QtQuick 2.0
import "Global.js" as Global

Item
{
    id: selectionBox
    width: 200
    height: 32

    state: "UNSELECTED"

    property bool selected: false;

    function selectionUpdate()
    {
        if (selected && dataServer.isShowParametersSelection())
        {
            selectionBox.state = "SELECTED";
        } else {
            selectionBox.state = "UNSELECTED";
        }
    }

    onSelectedChanged:
    {
        selectionUpdate();
    }

    Connections
    {
        target: dataServer
        onParameterChanged:
        {
            if (!dataServer.isShowParametersSelection())
            {
                selected = false;
            }
            selectionUpdate();
        }
    }

    signal changeEditMode();

    Rectangle
    {
        id: selectionRect
        anchors.fill: parent
        border.width: 2
        radius: 16
        anchors.rightMargin: -6
        anchors.leftMargin: -6
        opacity: 0.8;
    }

    MouseArea
    {
        id: mouseArea
        anchors.fill: parent

        propagateComposedEvents: true;

        onClicked:
        {
            if (dataServer.isShowParametersSelection())
            {
             //   mouse.accepted = false
                selected = !selected;
            }
        }

        onDoubleClicked:
        {
            //mouse.accepted = false
            selected = false;
            Global.controlEditModeChanged = true;
            changeEditMode();
        }

    }


    states: [
        State {
            name: "SELECTED"

            PropertyChanges {
                target: selectionRect
                color: "transparent"
                border.color: "#a4cdfd"
                visible: true
            }

        },
        State {
            name: "UNSELECTED"

            PropertyChanges {
                target: selectionRect
                color: "transparent"
                border.color: "transparent"
                visible: false
            }

        }
    ]



}
