import QtQuick 2.0

Item
{
    id: toolTip
    property alias text: textInput.text;

    property int verticalPadding: 1;
    property int horizontalPadding: 5;
    property bool hidden: false;


    width: textInput.width + horizontalPadding * 2;
    height: 17;
    state: "HIDDEN"

    Rectangle {
        id: background
        anchors.fill: parent
        color: "#1e7fdc"
        border.color: "white"
        border.width: 1
    }

    onWidthChanged:
    {
        x = parent.width/2 - width/2;
    }


    TextInput
    {
        id: textInput
        anchors.top: parent.top
        anchors.topMargin: 1
        anchors.horizontalCenter: parent.horizontalCenter

        color: "white"
        text: value
        anchors.verticalCenter: parent.verticalCenter
        horizontalAlignment: TextInput.AlignHCenter
        font.pixelSize: 12
        enabled: false

    }


    function show()
    {
        deactivationTimer.stop();
        activationTimer.start();
        hidden = false;
    }


    function hide()
    {
        activationTimer.stop();
        deactivationTimer.start();
        hidden = true;
    }


    Timer
    {
        id: activationTimer
        interval: 100; running: false; repeat: false
        onTriggered:
        {
            toolTip.visible = true;
            state = "SHOWED"
        }
    }


    Timer {
        id: deactivationTimer
        interval: 100; running: false; repeat: false
        onTriggered:
        {
            if (toolTip.state === "SHOWED")
            {
                state = "HIDDEN"
            }
        }
    }

    onOpacityChanged:
    {
        if (opacity == 0 && visible)
        {
            visible = false;
        }
    }

    states: [
        State {
            name: "HIDDEN"
            PropertyChanges { target: toolTip; opacity: 0}
            PropertyChanges { target: toolTip; focus: false}
        },
        State {
            name: "SHOWED"
            PropertyChanges { target: toolTip; opacity: 1}
            PropertyChanges { target: toolTip; focus: false}
        }
    ]

    transitions: Transition {
        NumberAnimation { properties: "opacity"; easing.type: Easing.InOutQuad; duration: 200 }
    }


}
