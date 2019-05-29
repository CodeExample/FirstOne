import QtQuick 2.0

import com.vectools.DataProvider 1.0

Item
{
    id: templateItem

    width: 29
    height: 33

    property int itemIndex;
    property int templateId;
    property bool selected;

    onSelectedChanged:
    {
        if (selected)
        {
            selectedColor.visible = true;
        } else {
            selectedColor.visible = false;
        }
    }

    Rectangle
    {
        x: -1.5
        y: devicePixelRatio === 2 ? 0 : -0.5
        width: 32
        height: 32

        id: selectedColor;
        color: "transparent";
        visible: false;

        border.color: "white";
        border.width: 2;
        radius: 33/2;
    }

    signal templateSelected(int selectedIndex);

    onTemplateIdChanged:
    {
        /*switch(templateId)
        {
        case DataProvider.Linear:
            state = "LINEAR";
            break;
        case DataProvider.Sigmoid:
            state = "SIGMOID";
            break;
        case DataProvider.Circles:
            state = "CIRCLES";
            break;
        case DataProvider.Radial:
            state = "RADIAL";
            break;
        case DataProvider.Spiral:
            state = "SPIRAL";
            break;
        case DataProvider.Halftone:
            state = "HALFTONE";
            break;
        case DataProvider.Wareframes:
            state = "WAREFRAMES";
            break;
        case DataProvider.Peano:
            state = "PEANO";
            break;
        case DataProvider.Trace:
            state = "TRACE";
            break;
        case DataProvider.Painting:
            state = "PAINTING";
            break;
        }*/
        templateIcon.source = "image://previews/tmplicon-" + templateId;
    }

    opacity: 0.9;

    Image
    {
        id: templateIcon
        anchors.centerIn: parent
    }

    MouseArea
    {
        anchors.fill: parent;
        onClicked:
        {
            templateSelected(itemIndex);
        }
    }


   /* states: [
        State {
            name: "CIRCLES"
            PropertyChanges { target: templateIcon; source: "images/tmpl_circles.png"}
        },
        State {
            name: "SIGMOID"
            PropertyChanges { target: templateIcon; source: "images/tmpl_sigmoids.png"}
        },
        State {
            name: "SPIRAL"
            PropertyChanges { target: templateIcon; source: "images/tmpl_spirales.png"}
        },
        State {
            name: "LINEAR"
            PropertyChanges { target: templateIcon; source: "images/tmpl_lines.png"}
        },
        State {
            name: "RADIAL"
            PropertyChanges { target: templateIcon; source: "images/tmpl_radials.png"}
        },
        State {
            name: "HALFTONE"
            PropertyChanges { target: templateIcon; source: "images/tmpl_halftones.png"}
        },
        State {
            name: "WAREFRAMES"
            PropertyChanges { target: templateIcon; source: "images/tmpl_wareframes.png"}
        },
        State {
            name: "PEANO"
            PropertyChanges { target: templateIcon; source: "images/tmpl_peano.png"}
        },
        State {
            name: "TRACE"
            PropertyChanges { target: templateIcon; source: "images/tmpl_trace.png"}
        },
        State {
            name: "PAINTING"
            PropertyChanges { target: templateIcon; source: "images/tmpl_painting.png"}
        }

    ]*/


}
