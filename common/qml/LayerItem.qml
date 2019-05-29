import QtQuick 2.0

import com.vectools.ModelItem 1.0

Item
{
    id: layerItem
    width: 180
    height: 42
    state: "SECTION"
    property ModelItem modelItem;

    SectionControl
    {
        anchors.fill: parent;
        id: sectionControl
        modelItem: layerItem.modelItem;
        visible: false;
    }

    FreemeshControl
    {
        anchors.fill: parent;
        id: freemeshControl
        modelItem: layerItem.modelItem;
        visible: false;
    }

    TemplateControl
    {
        anchors.fill: parent;
        id: templateControl
        modelItem: layerItem.modelItem;
        visible: false;
    }


    states: [
        State {
            name: "SECTION"
            PropertyChanges { target: sectionControl; visible: true}
            PropertyChanges { target: freemeshControl; visible: false}
            PropertyChanges { target: templateControl; visible: false}
        },
        State {
            name: "FREEMESH"
            PropertyChanges { target: sectionControl; visible: false}
            PropertyChanges { target: freemeshControl; visible: true}
            PropertyChanges { target: templateControl; visible: false}
        },
        State {
            name: "TEMPLATE"
            PropertyChanges { target: sectionControl; visible: false}
            PropertyChanges { target: freemeshControl; visible: false}
            PropertyChanges { target: templateControl; visible: true}
        }

    ]

}
