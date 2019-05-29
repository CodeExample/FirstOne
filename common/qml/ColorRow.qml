import QtQuick 2.0

Item {
    id: styleRow
    width: 200
    height: 24

    property string colorTemplate: "black";
    property int colorMode: 0;

    signal editingStarted;
    signal editingFinished;

    Row {

        SpinEdit
        {
            id: editColor
            y: -1
            width: 40
            inputMask: "#xxxxxx"
            anchors.verticalCenter: parent.verticalCenter
            minValue: 0
            maxValue: 20

            onEditingStarted:
            {
                styleRow.editingStarted();
            }

            onEditingFinished:
            {
                styleRow.editingFinished();
            }
        }
    }

}
