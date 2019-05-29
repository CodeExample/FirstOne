import QtQuick 2.0
import "Global.js" as Global
import com.vectools.DataServer 1.0
Item {
    id: edit
    width: 52
    height: 25

    state: "INIT"
    property alias inputMask: textInput.inputMask;
    property alias toolTipText: toolTip.text;
    property int init_width: -1;
    property bool shift_pressed: false;

    property double incriment: 1;
    property double minValue:-100.0;
    property double maxValue: 100.0;
    property double decimals: 0;
    property double old_value: -1;
    property double value: -1
    property alias suffix: suffixText.text;

    property bool was_changed: false;

    signal editingStarted;
    signal editingFinished;
    signal editEnterPressed;


    onValueChanged:
    {
        if (!was_changed)
        {
            old_value = value;
        }
        if (decimals > 0)
            textInput.text = Number(validateValue(value)).toLocaleString(Qt.locale("us_US"), 'g', decimals);
        else
            textInput.text = validateValue(value);
    }

    function isActivated()
    {
        return edit.state === "ACTIVE";
    }

    function finishTooltip()
    {
        tooltipTimer.stop();
        if (toolTip.visible)
        {
            toolTip.hide();
        }
    }

    function activateEdit()
    {
        if (init_width < 0)
            init_width = width;

        edit.state = "ACTIVE";
        dataServer.doAction(DataServer.EditingStarted);
    }

    function deactivateEdit()
    {
        if (init_width < 0)
            init_width = width;

        edit.state = "PASSIVE";
        stopEditing();
        dataServer.doAction(DataServer.EditingFinished);
    }


    function stopEditing()
    {
        Global.hasActivatedSpin = false;
        textInput.focus = false;
        textInput.deselect();

        value = parseFloat(textInput.text);

        if (was_changed)
        {
            was_changed = false;

            editingFinished();

            dataServer.doAction(DataServer.EditingFinished);
        }
    }


    function incValue(i)
    {
        was_changed = true;
        value += shift_pressed ? i*10 : i;
    }

    function decValue(i)
    {
        was_changed = true;
        value -= shift_pressed ? i*10 : i;
    }


    function validateValue(value)
    {
        if (isNaN(value))
            return old_value;

        if (value === "")
            return old_value;

        var v = value;

        if (v < minValue)
            v = minValue;

        if (v > maxValue)
            v = maxValue;

        return Math.floor((v + 0.0001)*1000)/1000;
    }


    onFocusChanged:
    {
        if (focus)
        {
            textInput.forceActiveFocus();
        }
    }


    Image {
        id: image1
        anchors.left: parent.left
        anchors.leftMargin: 0
        source: "images/edit_l.png"
    }


    MouseArea
    {
        id: mouseArea

        anchors.fill: parent

        hoverEnabled: true

        onEntered:
        {
            if (Global.controlEditModeChanged)
            {
                Global.controlEditModeChanged = false;
                return;
            }

            if (toolTipText != "")
            {
                tooltipTimer.start();
            }
        }

        onExited:
        {            
            if (!textInput.focus)
            {
                activationTimer.stop();
                if (edit.state !== "PASSIVE")
                {
                    deactivationTimer.start();
                }
            }
            finishTooltip();
        }

        onClicked:
        {
            textInput.forceActiveFocus();
            Global.hasActivatedSpin = true;
        }

        onDoubleClicked:
        {
            textInput.forceActiveFocus();
            textInput.selectAll();
        }

        Timer
        {
            id: activationTimer
            interval: 100; running: false; repeat: false
            onTriggered:
            {
                activateEdit();
            }
        }

        Timer {
            id: deactivationTimer
            interval: 100; running: false; repeat: false
            onTriggered:
            {
                if (spinner.state !== "HOVERED")
                {
                    deactivateEdit();
                }
            }
        }

        Timer {
            id: tooltipTimer
            interval: 1000; running: false; repeat: false
            onTriggered:
            {
                if (!toolTip.visible)
                {
                    toolTip.show();
                }
            }
        }


        Image
        {
            id: image2
            y: 0
            height: 25
            anchors.right: parent.right
            anchors.rightMargin: 5
            anchors.left: parent.left
            anchors.leftMargin: 5
            source: "images/edit_m.png"
        }
    }

    Image {
        id: image3
        x: 60
        y: 0
        anchors.right: parent.right
        anchors.rightMargin: 0
        source: "images/edit_r.png"
    }


    Text
    {
        id: suffixText;
        y: 7
        height: 10
        anchors.left: textInput.right
        anchors.leftMargin: 2
        font: textInput.font
        visible:  false
        color: "#babfc9"

        onTextChanged:
        {
            if (text !== "")
            {
                visible = true;
            } else {
                visible = false;
            }
        }

    }

    TextInput
    {
        id: textInput
        y: 8
        height: 10
        color: "#babfc9"
        text: "123"
        anchors.left: parent.left
        anchors.leftMargin: 5
        anchors.verticalCenter: parent.verticalCenter
        anchors.verticalCenterOffset: 0
        selectionColor: "#656565"
        horizontalAlignment: Text.AlignLeft
        font.pixelSize: 10
        selectByMouse: true
        maximumLength: 5

        onFocusChanged:
        {
            if (!textInput.focus)
            {
                stopEditing();
                deactivateEdit();
            } else {
                finishTooltip();
                activateEdit();
            }
        }

        onContentWidthChanged:
        {
            width = contentWidth;
        }

        Keys.onPressed:
        {
            event.accepted = false;

            if (event.key === Qt.Key_Shift)
            {
                shift_pressed = true;
                event.accepted = true;
                return;
            }

            if (event.key === Qt.Key_Enter || event.key === Qt.Key_Return)
            {
                value = parseFloat(textInput.text);
                event.accepted = true;
                stopEditing();
                //dataProvider.editEnterPressed();
                return;
            }

            if (event.key === Qt.Key_Escape)
            {
                text = old_value;
                value = old_value;
                textInput.focus = false;
                textInput.deselect();
                event.accepted = true;
                return;
            }

            if (event.key === Qt.Key_Up)
            {
                incValue(incriment)
                event.accepted = true;
                return;
            }

            if (event.key === Qt.Key_Down)
            {
                decValue(incriment)
                event.accepted = true;
                return;
            }

            was_changed = true;
        }

        Keys.onReleased:
        {
            event.accepted = false;

            if (event.key === Qt.Key_Shift)
            {
                shift_pressed = false;
                event.accepted = true;
                return;
            }
        }
    }

    Spinner
    {
        id: spinner
        x: 35
        y: 1
        opacity: 0
        anchors.right: parent.right
        anchors.rightMargin: 0
        visible: true

        onPlusClicked:
        {
            incValue(incriment);
            textInput.forceActiveFocus();
        }

        onMinusClicked:
        {
            decValue(incriment);
            textInput.forceActiveFocus();
        }
    }


    ToolTip
    {
        id: toolTip;
        y: -20;
        text: "";
        visible: false;
    }


    states: [
        State {
            name: "PASSIVE"
            PropertyChanges { target: edit; width: init_width}
            PropertyChanges { target: spinner; opacity: 0}
        },
        State {
            name: "ACTIVE"
            PropertyChanges { target: edit; width: init_width + 10 }
            PropertyChanges { target: spinner; opacity: 1}
        }
    ]

    transitions: Transition {
        NumberAnimation { properties: "width"; easing.type: Easing.InOutQuad; duration: 100 }
        NumberAnimation { properties: "opacity"; easing.type: Easing.InOutQuad; duration: 200 }
    }
}
