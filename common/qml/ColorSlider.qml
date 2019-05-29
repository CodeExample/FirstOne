import QtQuick 2.0
import com.vectools.DataServer 1.0

Item {
    id: colorSlider
    width: 200
    height: 24

    property string colorTemplate: "black";
    property int colorMode: -1;


    signal editingStarted();
    signal editingFinished();

    onColorTemplateChanged:
    {
    }


    onColorModeChanged:
    {
        switch(colorMode)
        {
        case 0:
            selected.anchors.centerIn = blackRect;
            selected.border.color = blackRect.color;
            break;
        case 1:
            selected.anchors.centerIn = whiteRect;
            selected.border.color = whiteRect.color;
            break;
        case 2:
            selected.anchors.centerIn = paletteRect;
            selected.border.color = colorTemplate;
            paletteRect.color = colorTemplate;
            break;
        //case 3:
        //    selected.anchors.centerIn = autoRect;
        //    selected.border.color = autoRect.color;
        //    break;
        }
    }



    Item
    {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        width: 76
        height: 24
        Rectangle
        {
            id: blackRect;
            x: 8
            color: 'black';
            width: 12;
            height: 12;
            radius: height/2;
            anchors.verticalCenter: parent.verticalCenter


            MouseArea
            {
                anchors.fill: parent;

                hoverEnabled: true;

                onEntered:
                {
                    blackRect.x = 7
                    blackRect.width = 14;
                    blackRect.height = 14;
                }

                onExited:
                {
                    blackRect.x = 8
                    blackRect.width = 12;
                    blackRect.height = 12;
                }

                onClicked:
                {
                    colorMode = 0;
                    editingFinished();
                }
            }
        }

        Rectangle
        {
            id: whiteRect;
            x: 8+24
            color: 'white';
            width: 12;
            height: 12;
            radius: height/2;
            anchors.verticalCenter: parent.verticalCenter

            MouseArea
            {
                anchors.fill: parent;

                hoverEnabled: true;

                onEntered:
                {
                    whiteRect.x = 7+24
                    whiteRect.width = 14;
                    whiteRect.height = 14;
                }

                onExited:
                {
                    whiteRect.x = 8+24
                    whiteRect.width = 12;
                    whiteRect.height = 12;
                }

                onClicked:
                {
                    colorMode = 1;
                    editingFinished();
                }
            }
        }

        Rectangle {
            id: paletteRect
            x: 8+2*24
            width: 12
            height: 12
            color: "#ffffff"
            radius: height/2
            opacity: 0.5
            anchors.verticalCenter: parent.verticalCenter

            MouseArea
            {
                anchors.fill: parent;

                hoverEnabled: true;

                onEntered:
                {
                    paletteRect.x = 7+2*24
                    paletteRect.width = 14;
                    paletteRect.height = 14;
                }

                onExited:
                {
                    paletteRect.x = 8+2*24
                    paletteRect.width = 12;
                    paletteRect.height = 12;
                }

                onClicked:
                {
                    colorMode = 2;
                    editingFinished();
                }

            }

        }

       /* Rectangle {
            id: auroRect
            x: 8+3*24
            width: 13
            height: 13
            radius: height/2
            color: "#22262f"
            anchors.verticalCenter: parent.verticalCenter
            Text {
                id: text
                color: "#ffffff"
                anchors.fill: parent;
                text: qsTr("A")
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                font.pixelSize: 8
            }

            MouseArea
            {
                anchors.fill: parent;

                hoverEnabled: true;

                onEntered:
                {
                    auroRect.x = 7+3*24
                    auroRect.width = 14;
                    auroRect.height = 14;
                }

                onExited:
                {
                    auroRect.x = 8+3*24
                    auroRect.width = 12;
                    auroRect.height = 12;
                }

                onClicked:
                {
                    colorMode = 3;
                    editingFinished();
                }

            }
        }*/

        Rectangle
        {
            id: selected
            color: "transparent"
            anchors.centerIn: blackRect;
            width: 18;
            height: 18;
            radius: height/2;

            border.width: 2;
            border.color: 'black';
        }
    }

}
