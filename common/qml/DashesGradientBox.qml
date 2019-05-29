import QtQuick 2.0

ItemsListBox
{
    id: dashesGradientBox
    model: gradientModel
    x: 0
    y: 0
    width: 24
    anchors.verticalCenter: parent.verticalCenter

    ListModel {
        id: gradientModel

        ListElement {
            itemId: 0
            icon: "images/dashes_bw.png"
            itemText: ""
        }
        ListElement {
            itemId: 1
            icon: "images/dashes_wb.png"
            itemText: ""
        }
    }
}
