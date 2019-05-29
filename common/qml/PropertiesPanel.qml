import QtQuick 2.0

import com.vectools.DataProvider 1.0

Rectangle
{
    id: propertiesPanel
    width: 200
    height: 280
    color: "transparent"

    property DataProvider dataProvider;

    TemplatesControl
    {
        id: templatesControl
        dataProvider: propertiesPanel.dataProvider;
        y: 10
        width: 174
        height: 50
        anchors.horizontalCenter: parent.horizontalCenter
        visible: false
    }

    NotebookPanel
    {
        id: notebookPanel
        dataProvider: propertiesPanel.dataProvider;
        height: parent.height - templatesControl.height;
        width: parent.width;
        y: templatesControl.height - 8
    }

}
