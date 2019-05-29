import QtQuick 2.0

Item {
    width: 200
    height: 300

    property variant abcArray;

    onActiveFocusChanged:
    {
    }

    NotebookPanel
    {
        id: notebookPanel
    }

}
