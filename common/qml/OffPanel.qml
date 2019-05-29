import QtQuick 2.0


Rectangle
{
    id: offPanel;
/*    color: "#434650"
    anchors.fill: parent
    radius: height/2
    opacity: 0
    state: "ON"
*/

        color: "#3a3d48"
        anchors.fill: parent
        radius: height/2

    property bool enabled: true;
    property double offOpacity: 1;

    onEnabledChanged:
    {
        if (enabled)
            state = "ON"
        else
            state = "OFF"
    }




    Text
    {
        id: offLabel
        color: "#acacac"
        text: "Off"
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        verticalAlignment: Text.AlignVCenter
        font.pointSize: 9
        font.family: "Verdana"
        font.italic: true

    }

    states: [
        State {
            name: "ON"
            PropertyChanges {
                target: offPanel
                opacity: 0
            }
        },
        State {
            name: "OFF"
            PropertyChanges {
                target: offPanel
                opacity: offOpacity;
            }
        }
    ]

    transitions: Transition {
        NumberAnimation { properties: "opacity"; easing.type: Easing.InOutQuad; duration: 100 }
    }


}
