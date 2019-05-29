import QtQuick 2.0
import QtGraphicalEffects 1.0

Item {
    id: actionButton
    width: 36
    height: 36
    state: "DISABLED"

    property alias toolTipText: toolTip.text;
    property alias iconSource: icon.source;
    property alias iconDisabled: disabledIcon.source;

    property bool checked: false;
    property bool checkable: false;

    function setEnabled(b)
    {
        if (b)
        {
            actionButton.state = "RELEASED"
        } else {
            checked = false;
            toolTip.visible = false;
            actionButton.state = "DISABLED";
            tooltipTimer.stop();
        }
    }

    function finishTooltip()
    {
        tooltipTimer.stop();
        toolTip.hide();
    }

    signal clicked();


    onCheckedChanged:
    {
        if (checked)
            actionButton.state = actionButton.enabled ? "CHECKED" : "DISABLED";
        else
            actionButton.state = actionButton.enabled ? "RELEASED" : "DISABLED";
    }


    Timer {
        id: tooltipTimer
        interval: 1000; running: false; repeat: false
        onTriggered:
        {
            if (actionButton.state != "DISABLED")
            {
                if (!toolTip.visible)
                {
                    toolTip.show();
                }
            }
        }
    }

    Rectangle
    {
        id: buttonRect;
        anchors.fill: parent;
        radius: 4
        border.width: 0;
    }

    Image
    {
        id: icon
        anchors.centerIn: parent
    }

    Image
    {
        id: disabledIcon
        anchors.centerIn: parent
    }

    ToolTip
    {
        id: toolTip;
        y: -20;
        text: "";
        visible: false;
        onVisibleChanged:
        {
            var screen_p = mapToItem(null, 0,0);
            if (screen_p.x <= 0)
            {
                x = -screen_p.x;
            }
        }
    }

    MouseArea
    {
        anchors.fill: parent;
        hoverEnabled: true;
        enabled: parent.state != "DISABLED";

        onEntered:
        {
            if (actionButton.state != "DISABLED")
                actionButton.state = "HOVERED";

            tooltipTimer.start();
        }
        onExited:
        {
            if (actionButton.state != "DISABLED")
                actionButton.state = "RELEASED";
            finishTooltip();
        }
        onPressed:
        {
            if (actionButton.state != "DISABLED")
                actionButton.state = "PRESSED";
            finishTooltip();
        }
        onReleased:
        {
            if (actionButton.state != "DISABLED")
                actionButton.state = "HOVERED";
            finishTooltip();
        }

        onFocusChanged:
        {
            finishTooltip();
        }

        onClicked:
        {
            finishTooltip();

            if (checkable)
                checked = !checked;

            actionButton.state = "HOVERED";
            actionButton.clicked();
        }

    }

    states: [
        State {
            name: "HOVERED"
            PropertyChanges { target: buttonRect; color: checked ? "#1a1e26" : "#2d3139"}
            PropertyChanges { target: icon; visible: true}
            PropertyChanges { target: disabledIcon; visible: false}
        },
        State {
            name: "PRESSED"
            PropertyChanges { target: buttonRect; color: "#000000"}
            PropertyChanges { target: icon; visible: true}
            PropertyChanges { target: disabledIcon; visible: false}
        },
        State {
            name: "RELEASED"
            PropertyChanges { target: buttonRect; color: checked ? "#1a1e26" : "transparent"}
            PropertyChanges { target: icon; visible: true}
            PropertyChanges { target: disabledIcon; visible: false}
        },
        State {
            name: "CHECKED"
            PropertyChanges { target: buttonRect; color: "#1a1e26"}
            PropertyChanges { target: icon; visible: true}
            PropertyChanges { target: disabledIcon; visible: false}
        },
        State {
            name: "DISABLED"
            PropertyChanges { target: buttonRect; color: checked ? "#2d3039" : "transparent"}
            PropertyChanges { target: icon; visible: false}
            PropertyChanges { target: disabledIcon; visible: true}
        }
    ]

}
