import QtQuick 2.0

Item {
    id: spinner

    width: 18
    height: 25

    state: "ALIVED"

    signal hovered
    signal alived

    signal plusClicked
    signal minusClicked


    MouseArea
    {
        id: plusMouseArea
        x: 0
        y: 2
        width: 18
        height: 10
        hoverEnabled: true;
        Image {
            id: imagePlus
            width: 18
            anchors.fill: parent
            source: "images/spin_plus_n.png"
        }

        onEntered: {
            imagePlus.source = "images/spin_plus_h.png"
            spinner.state = "HOVERED"
            hovered()
        }
        onExited:
        {
            imagePlus.source = "images/spin_plus_n.png"
            spinner.state = "ALIVED"
            repeaterPlusTimer.stop();
            repeaterMinusTimer.stop();
            alived()
        }
        onPressed:
        {
            imagePlus.source = "images/spin_plus_p.png";
            activationPlusRepeaterTimer.start();
            repeaterMinusTimer.stop();
        }
        onReleased:
        {
            imagePlus.source = "images/spin_plus_h.png";
            activationPlusRepeaterTimer.stop();
            repeaterPlusTimer.stop();
            repeaterMinusTimer.stop();
            spinner.state = "ALIVED"
            plusClicked();
        }

        Timer
        {
            id: activationPlusRepeaterTimer
            interval: 400; running: false; repeat: false
            onTriggered:
            {
                repeaterPlusTimer.start();
            }
        }

        Timer
        {
            id: repeaterPlusTimer
            interval: 100; running: false; repeat: true
            onTriggered:
            {
                if (interval > 50)
                    interval -= 10;

                plusClicked();
            }
        }
    }


    MouseArea
    {
        id: minusMouseArea
        x: 0
        y: 12
        width: 18
        height: 10
        hoverEnabled: true;
        Image {
            id: imageMinus
            anchors.fill: parent
            source: "images/spin_minus_n.png"
        }
        onEntered: {
            imageMinus.source = "images/spin_minus_h.png";
            spinner.state = "HOVERED";
            hovered();
        }
        onExited:
        {
            imageMinus.source = "images/spin_minus_n.png";
            spinner.state = "ALIVED";
            repeaterPlusTimer.stop();
            repeaterMinusTimer.stop();
            alived();
        }
        onPressed:
        {
            imageMinus.source = "images/spin_minus_p.png";
            activationMinusRepeaterTimer.start();
            repeaterPlusTimer.stop();
        }

        onReleased:
        {
            imageMinus.source = "images/spin_minus_h.png";
            activationMinusRepeaterTimer.stop();
            repeaterPlusTimer.stop();
            repeaterMinusTimer.stop();
            spinner.state = "ALIVED"
            minusClicked();
        }

        Timer
        {
            id: activationMinusRepeaterTimer
            interval: 400; running: false; repeat: false
            onTriggered:
            {
                repeaterMinusTimer.start();
            }
        }

        Timer
        {
            id: repeaterMinusTimer
            interval: 100; running: false; repeat: true
            onTriggered:
            {
                if (interval > 50)
                    interval -= 10;

                minusClicked();
            }
        }

    }
    states: [
        State {
            name: "ALIVED"
        },
        State {
            name: "HOVERED"
        }
    ]

}
