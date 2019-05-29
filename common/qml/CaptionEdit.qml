import QtQuick 2.0

import com.vectools.ModelItem 1.0
import com.vectools.DataServer 1.0

Item
{
    id: captionEdit
    width: 200
    height: 40
    state: "NORMAL"

    property ModelItem modelItem;



    Connections
    {
        target: modelItem;

        onSelectedChanged:
        {
            captionPreview.color = modelItem.isSelected() ?"#30343a" : "#dcdfe9";
            captionEdit.state = "NORMAL"
        }

        onEditCaptionRequest:
        {
            if (captionEdit.state == "NORMAL")
            {
                captionEdit.state = "EDIT";
            }
        }
    }

    onStateChanged:
    {
        if (state == "EDIT")
        {
            activationTimer.start();

            dataServer.doAction(DataServer.EditingStarted);
        } else {
            activationTimer.stop();
            dataServer.doAction(DataServer.EditingFinished);
        }
        textInput.text = modelItem.caption();
        captionPreview.text = modelItem.caption();
    }


    onModelItemChanged:
    {
        textInput.text = modelItem.caption();
        captionPreview.text = modelItem.caption();
    }

    Timer
    {
        id: activationTimer;
        interval: 200;
        repeat: false;
        running: false;
        onTriggered:
        {
            textInput.forceActiveFocus();
            textInput.selectAll();
        }
    }

    Timer
    {
        id: updateWidthTimer
        interval: 10;
        repeat: false;
        running: false;
        onTriggered:
        {
            captionEdit.width = editRect.width;
            captionPreview.width= editRect.width;
        }
    }

    Row
    {
        id: row
        spacing: 4
        anchors.fill: parent
        Text
        {
            id: captionPreview
            color: "#dcdfe9"
            height: 20
            text: modelItem.caption();
            anchors.verticalCenterOffset: 2
            anchors.verticalCenter: parent.verticalCenter
            font.pixelSize: 12
            visible: true;
            enabled: true;
            focus: false;
            elide: Text.ElideMiddle
        }


        TextInput
        {
            id: textInput
            color: "black"
            height: 20
            anchors.verticalCenterOffset: 2
            anchors.verticalCenter: parent.verticalCenter
            font.pixelSize: 12
            selectionColor: "lightgray"
            selectedTextColor: "black"
            selectByMouse: true;
            property bool textChaged: false;
            Keys.onPressed:
            {
                if (event.key === Qt.Key_Escape)
                {
                    textInput.text = captionPreview.text;
                    captionEdit.state = "NORMAL";
                }
            }

            onTextChanged:
            {
                updateWidthTimer.start();
                textChaged = true;
            }

            onAccepted:
            {
                modelItem.setCaption(textInput.text);
                captionPreview.text = modelItem.caption();
                captionEdit.state = "NORMAL"
            }

            onFocusChanged:
            {
                if (!textInput.focus && textChaged)
                {
                    modelItem.setCaption(textInput.text);
                    captionPreview.text = modelItem.caption();
                }
                textChaged = false;
            }

            Rectangle
            {
                id: editRect
                radius: 4

                color: "transparent"
                border.color: "blue"
                border.width: 2
                smooth: true;
                opacity: 0.5

                anchors.left: parent.left
                anchors.leftMargin: -3

                anchors.right: parent.right
                anchors.rightMargin: -4

                anchors.top: parent.top
                anchors.topMargin: -3

                anchors.bottom: parent.bottom
                anchors.bottomMargin: 3
            }
        }
    }

    states: [
        State {
            name: "NORMAL"
            PropertyChanges { target: textInput; enabled: false}
            PropertyChanges { target: textInput; visible: false}
            PropertyChanges { target: textInput; focus: false}
            PropertyChanges { target: captionPreview; visible: true}
        },
        State {
            name: "EDIT"
            PropertyChanges { target: textInput; enabled: true}
            PropertyChanges { target: textInput; visible: true}
            PropertyChanges { target: textInput; focus: true}
            PropertyChanges { target: captionPreview; visible: false}
        }
    ]
}
