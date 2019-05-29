import QtQuick 2.0

Item
{
    id: item
    width: 110;
    height: 5
    clip: true
    property int transparent: 100
    Grid
    {
        id: grid
        anchors.fill: parent;

        property int cellSide: 5

        rows: 1
        columns: 145/cellSide
        Repeater {
            model: grid.columns*grid.rows
            Rectangle {
                width: grid.cellSide; height: grid.cellSide
                color: index%2 ? "transparent" : "black"
                radius: 3
                antialiasing: true
            }
        }
    }
/*
    Rectangle
    {
        width: 110
        height: item.width
        anchors.verticalCenter: parent.verticalCenter
        rotation: -90
        gradient: Gradient {
            GradientStop {
                position: 0.0; color: "#00000000"
            }
            GradientStop {
                position: 1.0; color: "#FF000000"
            }
        }
    }
*/
}
