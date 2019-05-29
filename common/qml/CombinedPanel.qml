import QtQuick 2.0

import com.vectools.DataProvider 1.0

Rectangle
{
    id: combinedPanel
    width: 100

    color: "transparent"

    state: "MAXIMIZED"

    property DataProvider dataProvider;
    property int maximized_height: 100;
    property int controlsOnPage: 5
    property int controlId: 0;

    property int controlHeight: 0;
    property int navigatorHeight: 0;
    property int controlsCount: 0;

    signal beginDrag();
    signal endDrag();

    Connections
    {
        target: dataProvider

        onDataChanged:
        {
            controlId = dataProvider.controlId;
            controlsCount = dataProvider.controlsCount;

            if (controlsCount == 0)
            {
                //combinedPanel.visible = false;
                //combinedPanel.height = 0;
                return;
            }

            combinedPanel.visible = true;

            var controls_on_page = dataProvider.controlId == DataProvider.PropertiesControl ? 1 : controlsOnPage;

            if (dataProvider.controlId == DataProvider.PropertiesControl)
            {
                controlHeight = 200;
            } else {
                controlHeight = 32;
            }

            if (controlsCount <= controls_on_page)
            {
                controls_on_page = controlsCount;
                navigatorHeight = 16;
            } else {
                navigatorHeight = 35;
            }

            var height_controls_page = controlHeight*controls_on_page;
            maximized_height = caption.height + height_controls_page + navigatorHeight;

            if (state == "MAXIMIZED")
            {
                combinedPanel.height = maximized_height;
            }

            if (controlId == DataProvider.PropertiesControl)
            {
                propertiesPanel.visible = true;
                notebookPanel.visible = false;
                propertiesPanel.height = combinedPanel.height - 12
            } else {
                propertiesPanel.visible = false;
                notebookPanel.visible = true;
                notebookPanel.height = combinedPanel.height - 12
            }
        }
    }

    Text
    {
        id: caption
        anchors.horizontalCenter: parent.horizontalCenter
        y: 0;
        height: 20
        color: "#dcdfe9"
        text: dataProvider.getTitle();
        font.family: "ProximaNova-Semibold"
        font.letterSpacing: 1
        font.pointSize: 10
        font.bold: true
        verticalAlignment: Text.AlignBottom

        MouseArea
        {
            anchors.fill: parent;
            hoverEnabled: true
            onDoubleClicked:
            {
                if (combinedPanel.state == "MAXIMIZED")
                    combinedPanel.state = "MINIMIZED";
                else
                    combinedPanel.state = "MAXIMIZED";
            }

            onEntered:
            {
                caption.color = "#f0f4fd"
            }

            onExited:
            {
                caption.color = "#dcdfe9"
            }
        }
    }

    PropertiesPanel
    {
        id: propertiesPanel;
        y: 20;
        dataProvider: combinedPanel.dataProvider;
        width: combinedPanel.width
    }

    NotebookPanel
    {
        id: notebookPanel;
        y: 10;
        dataProvider: combinedPanel.dataProvider;
        width: combinedPanel.width
        onItemsHeightChanged:
        {
            combinedPanel.height = itemsHeight;
            notebookPanel.height = combinedPanel.height - 10;
        }
    }

    Rectangle
    {
        id: separator
        color: "black"
        height: 5;
        width: parent.width;
        y: parent.height - 5;
        opacity: 0
        state: "EXITED"
        MouseArea
        {
            id: separatorMouseArea
            anchors.fill: parent;
            anchors.topMargin: -5;
            anchors.bottomMargin: -5;
            hoverEnabled: true;
            property bool hovered: false;
            property bool pressed: false;
            property int mouse_y: 0;

            NumberAnimation {
                id: combinedPanel_height_anim;
                target: combinedPanel;
                property: "height";
                duration: 200;
                easing.type: Easing.InOutQuad
                onStopped:
                {
                    notebookPanel.height = combinedPanel.height - 10
                    maximized_height = combinedPanel.height;
                }
            }

            onEntered:
            {
                if (combinedPanel.state == "MINIMIZED")
                    return;

                if (controlId == DataProvider.PropertiesControl)
                    return;

                if (controlsCount == 0)
                    return;

                separator.state = "HOVERED"

                hovered = true;
            }

            onExited:
            {
                hovered = false;
                separator.state = "EXITED"
            }

            onPressed:
            {
                if (!hovered)
                    return;

                pressed = true;
                mouse_y = mouseY;
                mouse.accepted = true;

                beginDrag();
            }

            onDoubleClicked:
            {
                navigatorHeight = 16;
                if (controlsOnPage == controlsCount)
                {
                    controlsOnPage = 1;
                    if (controlsCount > 1)
                        navigatorHeight = 35;
                } else {
                    controlsOnPage = controlsCount;
                }

                var height_controls_page = controlHeight*controlsOnPage;
                maximized_height = caption.height + height_controls_page + navigatorHeight;

                combinedPanel_height_anim.to = maximized_height;
                combinedPanel_height_anim.start();
            }

            onReleased:
            {
                if (!pressed)
                    return;

                pressed = false;
                mouse.accepted = true;

                var height_controls_page = combinedPanel.height - caption.height - navigatorHeight;
                controlsOnPage = Math.ceil(height_controls_page/controlHeight);

                if (controlsOnPage < 1)
                    controlsOnPage = 1;

                if (controlsOnPage > controlsCount)
                    controlsOnPage = controlsCount;

                if (controlsCount <= controlsOnPage)
                {
                    navigatorHeight = 16;
                } else {
                    navigatorHeight = 35;
                }

                height_controls_page = controlHeight*controlsOnPage;
                maximized_height = caption.height + height_controls_page + navigatorHeight;

                combinedPanel.height = maximized_height;
                notebookPanel.height = combinedPanel.height - 10

                endDrag();

            }

            onPositionChanged:
            {
                if (!pressed)
                    return;

                var dlt_y = mouseY - mouse_y;
                combinedPanel.height += dlt_y;
                notebookPanel.height = combinedPanel.height - 10

                mouse.accepted = true;
            }
        }

        states: [
            State {
                name: "HOVERED"

                PropertyChanges {
                    target: separator;
                    opacity: 0.1;
                }
            },
            State {
                name: "EXITED"

                PropertyChanges {
                    target: separator;
                    opacity: 0;
                }

            }
        ]

        transitions: Transition {
            NumberAnimation { properties: "opacity"; easing.type: Easing.InOutQuad; duration: 200 }
        }

    }

    states: [
        State {
            name: "MAXIMIZED"

            PropertyChanges {
                target: combinedPanel;
                height: maximized_height;
            }

            PropertyChanges {
                target: propertiesPanel;
                opacity: 1;
                enabled: true;
            }

            PropertyChanges {
                target: notebookPanel;
                opacity: 1;
                enabled: true;
            }

            PropertyChanges {
                target: separatorMouseArea;
                enabled: true;
            }
        },
        State {
            name: "MINIMIZED"

            PropertyChanges {
                target: combinedPanel
                height: caption.height;
            }

            PropertyChanges {
                target: propertiesPanel;
                opacity: 0;
                enabled: false;
            }

            PropertyChanges {
                target: notebookPanel;
                opacity: 0;
                enabled: false;
            }

            PropertyChanges {
                target: separatorMouseArea;
                enabled: false;
            }
        }
    ]

    transitions: Transition {
        NumberAnimation { properties: "height"; easing.type: Easing.InOutQuad; duration: 400 }
        NumberAnimation { properties: "opacity"; easing.type: Easing.InOutQuad; duration: 200 }
    }

}
