import QtQuick 2.0

Item 
{
    id: checkBox;
    width: 50;
    height: 20;

    state: "UNCHECKED"

    property bool checked;
    property alias text: textInput.text;
    property alias toolTipText: toolTip.text;

    signal editingFinished;

    onCheckedChanged:
    {
        if (checked)
        {
            checkBox.state = "CHECKED"
        } else {
            checkBox.state = "UNCHECKED"
        }
    }

    MouseArea
    {
        anchors.fill: parent;
        hoverEnabled: true;

        onClicked:
        {
            checked = !checked;
            editingFinished();
        }

        onEntered:
        {
            imageCheck.opacity = 1;
        }
        onExited:
        {
            imageCheck.opacity = 0.8;
        }
    }

    Row
    {
        id: row1
        spacing: 2
        anchors.fill: parent;
        
        
        Image {
            id: imageCheck
            width: 16
            height: 16
            opacity: 0.8
            anchors.verticalCenter: parent.verticalCenter
            sourceSize.height: 16
            sourceSize.width: 16
            source: "images/soda/check_box_unchecked.png"
        }
        
        TextInput
        {
            id: textInput
            y: 8
            height: 10
            color: "#babfc9"
            text: "123"
            anchors.verticalCenterOffset: 0
            anchors.verticalCenter: parent.verticalCenter
            horizontalAlignment: TextInput.AlignLeft
            font.pixelSize: 10
            readOnly: true;
        }
        
        
        
        ToolTip
        {
            id: toolTip;
            y: -20;
            text: "";
            visible: false;
        }
        
        
    }
    states: [
        State {
            name: "CHECKED"

            PropertyChanges {
                target: imageCheck
                source: "images/soda/check_box_checked.png"
            }
        },
        State {
            name: "UNCHECKED"
        }
    ]
}
