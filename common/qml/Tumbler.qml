import QtQuick 2.0


Item {
    id: tumbler

    property bool on: false

    function toggle()
    {
        if (tumbler.state == "on")
            tumbler.state = "off";
        else
            tumbler.state = "on";
    }

    function releaseSwitch()
    {
        if (button.x < 10) {
            tumbler.state = "on";
            button.x = 1;
            return;
        }
        if (button.x > 20) {
            tumbler.state = "off";
            button.x = 29;
            return;
        }
        toggle();
    }

    width: 74
    height: 38

    Image {
        id: imageBack
        x: 0
        y: 0
        source: "images/tumbler_back.png"
        MouseArea {
            anchors.fill: parent
            onClicked: toggle()
        }
    }

    Image {
        id: button
        x: 29
        y: 0
        source: "images/tumbler_button.png"

        MouseArea {
            anchors.fill: parent
            onClicked: toggle()
            drag.target: button; drag.axis: Drag.XAxis; drag.minimumX: 1; drag.maximumX: 29
            onReleased: releaseSwitch()
        }
    }

    states: [
        State {
            name: "off"
            PropertyChanges { target: button; x: 29; y: 0 }
            PropertyChanges { target: tumbler; on: false }
        },
        State {
            name: "on"
            PropertyChanges { target: button; x: 1 }
            PropertyChanges { target: tumbler; on: true }
        }
    ]

    transitions: Transition {
        NumberAnimation { properties: "x"; easing.type: Easing.InOutQuad; duration: 100 }
    }
}
