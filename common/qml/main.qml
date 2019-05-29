import QtQuick 2.0
import QtQuick.Controls 1.0
import com.vectools 1.0

Rectangle {
    id: rectangle1
    width: 640
    height: 480
    color: "#434650"

    QMLGLWidget
    {
        id: gLWidget
        anchors.rightMargin: 8
        anchors.leftMargin: 8
        anchors.bottomMargin: 8
        anchors.topMargin: 8
        anchors.fill: parent
        color: "red"
    }



}
