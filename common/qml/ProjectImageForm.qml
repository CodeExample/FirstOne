import QtQuick 2.0
import QtQuick.Controls 1.2

Item {
    width: 800
    height: 600

    Rectangle
    {
        id: rectangle2
        color: "#434650"
        anchors.fill: parent

        Rectangle {
            id: rectangle3
            x: 8
            y: 8
            width: 575
            height: 584
            color: "#00000000"

            Image {
                id: image
                x: 238
                y: 242
                width: 100
                height: 100
                source: "qrc:/qtquickplugin/images/template_image.png"
            }

            MouseArea {
                id: mouseArea1
                x: 8
                y: 0
                width: 567
                height: 584
            }
        }
    }

    Rectangle {
        id: rectangle1
        x: 592
        y: 8
        width: 200
        height: 584
        color: "#2c2e34"
        clip: false
        opacity: 1

        Text {
            id: text1
            x: 29
            y: 46
            color: "#d2d2d2"
            text: qsTr("Width:")
            font.pixelSize: 12
        }

        SpinBox {
            id: widthEdit
            x: 86
            y: 41
            width: 70

        }

        Text {
            id: text2
            x: 24
            y: 85
            color: "#d2d2d2"
            text: qsTr("Height:")
            font.pixelSize: 12
        }

        SpinBox {
            id: heightEdit
            x: 86
            y: 80
            width: 70
            height: 25
            prefix: "mm"
        }

        SpinEdit {
            id: resolutionEdit
            x: 86
            y: 121
            width: 70
            height: 25
            incriment: 10
            inputMask: "xxxx"
            maxValue: 1000
            minValue: 72
        }

        Text {
            id: text3
            x: 15
            y: 126
            color: "#d2d2d2"
            text: qsTr("Resolution:")
            font.pixelSize: 12
        }

        CheckBox {
            id: resample
            x: 86
            y: 161
            width: 19
            height: 20
        }

        Text {
            id: text4
            x: 20
            y: 164
            color: "#d2d2d2"
            text: qsTr("Resample:")
            font.pixelSize: 12
        }
    }

}
