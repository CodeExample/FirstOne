import QtQuick 2.0

ItemsListBox
{
    id: gradientListBox
    model: gradientModel
    x: 0
    y: 0
    width: 24
    anchors.verticalCenter: parent.verticalCenter

    ListModel {
        id: gradientModel

        ListElement {
            itemId: 0
            icon: "images/thickness_wb.png"
            itemText: ""
        }
        ListElement {
            itemId: 1
            icon: "images/thickness_bw.png"
            itemText: ""
        }
    }
}
