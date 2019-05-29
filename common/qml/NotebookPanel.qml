import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Controls.Styles 1.1

import com.vectools.DataProvider 1.0

Item
{
    id: notebookPanel

    property DataProvider dataProvider;

    property var control_codes: []
    property int delegateHeight: getDelegateHeight()
    property int itemsOnPage: -1
    property int templatesViewHeight: 0
    property int pagesCount: 0

    signal itemsHeightChanged(int itemsHeight)


    ListModel {
        id: pagesModel
    }

    function getDelegateHeight()
    {
        if (dataProvider.controlId != DataProvider.PropertiesControl)
            return 32;

        if (pagesCount == 1)
            return templatesViewHeight - pagesNavigatorContainer.height;

        return templatesViewHeight - 16 - pagesNavigatorContainer.height;
    }

    Timer
    {
        id: updateModelsTimer;
        repeat: false;
        running: false;
        triggeredOnStart: false;
        interval: 200;
        onTriggered:
        {
            updateModels();
        }
    }

    function getPagesModel()
    {
        pagesModel.clear();

        pagesCount = 0;

        templatesViewHeight = pagesView.height;

        if (templatesViewHeight == 0)
            return;

        delegateHeight = getDelegateHeight()

        control_codes = []

        itemsOnPage = (templatesViewHeight - 16)/delegateHeight;

        if (itemsOnPage == 0)
            return;

        for(var j=0; j < dataProvider.controlsCount; ++j)
        {
            control_codes.push(dataProvider.controlCodeAt(j));
        }

        pagesCount = Math.ceil(control_codes.length/itemsOnPage);

        if (pagesCount > 1 && itemsOnPage > 1)
        {
            pagesCount = Math.ceil(control_codes.length/itemsOnPage);
        }


        if (templatesViewHeight == 0)
            return pagesModel;

        for(var i=0; i < pagesCount; ++i)
        {
            pagesModel.append({pageNum: i});
        }

        return pagesModel;
    }


    function updateModels()
    {
        pagesView.model = getPagesModel();
        pagesNavigator.model = pagesCount == 1 ? 0 : pagesCount;
    }

    function notifyPageChanged(currentIndex)
    {
        if (dataProvider.controlId == DataProvider.PropertiesControl)
        {
            if (currentIndex >= 0)
                dataProvider.notifyPropertiesControlSelected(control_codes[currentIndex])
            else
                dataProvider.notifyPropertiesControlSelected(0)
        }
    }


    Connections
    {
        target: dataProvider
        onDataChanged:
        {
            updateModels()
        }
    }

    Rectangle
    {
        id: viewContainer

        anchors.fill: parent
        clip: true
        focus: false;

        color: "transparent"

        ListView
        {
            id: pagesView

            focus: false

            highlightMoveDuration: 400

            onHeightChanged:
            {
                updateModelsTimer.stop();
                updateModelsTimer.start();
            }

            onCountChanged:
            {
                if (count > 0)
                    notifyPageChanged(currentIndex);
                else
                    notifyPageChanged(-1);

            }

            onModelChanged:
            {
                currentIndex = 0;
                if (count > 0)
                    notifyPageChanged(currentIndex);
                else
                    notifyPageChanged(-1);
            }

            onCurrentIndexChanged:
            {
                pagesNavigatorContainer.updatePosition();
                if (count > 0)
                    notifyPageChanged(currentIndex);
                else
                    notifyPageChanged(-1);
            }


            Rectangle
            {
                id: scrollRight
                x: 234
                width: 34
                color: "transparent"
                radius: 10
                anchors.rightMargin: -16
                anchors.top: parent.top
                anchors.topMargin: 8
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 32
                anchors.right: parent.right
                opacity: 0

                Canvas
                {
                    id: canvasRight
                    width: 10
                    height: 50
                    anchors.leftMargin: 6
                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter

                    onPaint:
                    {
                        var x0 = 0;
                        var x1 = width-1;
                        var y0 = 0;
                        var y1 = height/2;
                        var y2 = height-1;

                        var ctx = getContext("2d");
                        ctx.save();
                        ctx.clearRect(0,0,width, height);

                        ctx.fillStyle = "#FFFFFFFF";
                        ctx.lineWidth="2.5";
                        ctx.beginPath();
                        ctx.moveTo(x0, y0);
                        ctx.lineTo(x1, y1);
                        ctx.lineTo(x0, y2);
                        ctx.lineTo(x0, y0);
                        ctx.closePath();
                        ctx.fill();

                        ctx.restore();

                    }
                }

                MouseArea
                {
                    anchors.fill: parent
                    hoverEnabled: true
                    preventStealing: true

                    onEntered:
                    {
                        if (pagesView.currentIndex < pagesView.model.count-1)
                        {
                            scrollRight.state = "HOVERED"
                        }
                    }
                    onExited:
                    {
                        if (scrollRight.opacity > 0)
                        {
                            scrollRight.state = ""
                        }
                    }
                    onClicked:
                    {
                        if (pagesView.currentIndex < pagesView.model.count-1)
                        {
                            pagesView.currentIndex += 1;
                            if (pagesView.currentIndex == pagesView.model.count-1)
                            {
                                scrollRight.state = ""
                            }
                        }
                    }
                    onPressed:
                    {
                        if (scrollRight.state == "HOVERED")
                        {
                            scrollRight.state = "PRESSED";
                        }
                    }
                    onReleased:
                    {
                        if (scrollRight.state == "PRESSED")
                        {
                            scrollRight.state = "HOVERED";
                        }
                    }
                }

                states: [
                    State {
                        name: "HOVERED"
                        PropertyChanges {
                            target: scrollRight
                            opacity: 0.2
                        }
                    },
                    State {
                        name: "PRESSED"
                        PropertyChanges {
                            target: canvasRight
                            scale: 0.8
                        }
                        PropertyChanges {
                            target: scrollRight
                            opacity: 0.2
                        }
                    }

                ]

                transitions: Transition {
                    NumberAnimation { properties: "opacity"; easing.type: Easing.InOutQuad; duration: 200 }
                    NumberAnimation { properties: "scale"; easing.type: Easing.InOutQuad; duration: 100 }
                }

            }

            Rectangle {
                id: scrollLeft
                x: -17
                width: 36
                color: "transparent"
                radius: 10
                anchors.top: parent.top
                anchors.topMargin: 8
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 32
                opacity: 0

                Canvas
                {
                    id: canvasLeft
                    width: 10
                    height: 50
                    anchors.rightMargin: 6
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter

                    onPaint:
                    {
                        var x0 = width-1;
                        var x1 = 0;
                        var y0 = 0;
                        var y1 = height/2;
                        var y2 = height-1;

                        var ctx = getContext("2d");
                        ctx.save();
                        ctx.clearRect(0,0,width, height);

                        ctx.fillStyle = "#FFFFFFFF";
                        ctx.lineWidth="2.5";
                        ctx.beginPath();
                        ctx.moveTo(x0, y0);
                        ctx.lineTo(x1, y1);
                        ctx.lineTo(x0, y2);
                        ctx.lineTo(x0, y0);
                        ctx.closePath();
                        ctx.fill();

                        ctx.restore();
                    }
                }
                MouseArea
                {
                    anchors.fill: parent
                    hoverEnabled: true
                    preventStealing: true

                    onEntered:
                    {
                        if (pagesView.currentIndex > 0)
                        {
                            scrollLeft.state = "HOVERED";
                        }
                    }
                    onExited:
                    {
                        if (scrollLeft.opacity > 0)
                        {
                            scrollLeft.state = "";
                        }
                    }
                    onClicked:
                    {
                        if (pagesView.currentIndex > 0)
                        {
                            pagesView.currentIndex -= 1;
                            if (pagesView.currentIndex == 0)
                            {
                                scrollLeft.state = "";
                            }
                        }
                    }
                    onPressed:
                    {
                        if (scrollLeft.state == "HOVERED")
                        {
                            scrollLeft.state = "PRESSED";
                        }
                    }
                    onReleased:
                    {
                        if (scrollLeft.state == "PRESSED")
                        {
                            scrollLeft.state = "HOVERED";
                        }
                    }
                }

                states: [
                    State {
                        name: "HOVERED"
                        PropertyChanges {
                            target: scrollLeft
                            opacity: 0.2
                        }
                    },
                    State {
                        name: "PRESSED"
                        PropertyChanges {
                            target: canvasLeft
                            scale: 0.8
                        }
                        PropertyChanges {
                            target: scrollLeft
                            opacity: 0.2
                        }
                    }

                ]

                transitions: Transition {
                    NumberAnimation { properties: "opacity"; easing.type: Easing.InOutQuad; duration: 200 }
                    NumberAnimation { properties: "scale"; easing.type: Easing.InOutQuad; duration: 100 }
                }
            }

            keyNavigationWraps: false
            highlightRangeMode: ListView.StrictlyEnforceRange
            orientation: ListView.Horizontal
            anchors.fill: parent;
            anchors.leftMargin: 0;
            anchors.rightMargin: 0;
            activeFocusOnTab: false;
            interactive: false

            snapMode: ListView.SnapOneItem;

            delegate: Item
            {
                id: container
                width: viewContainer.width-8;
                activeFocusOnTab: true

                property int pageNum: model.pageNum

                Column
                {
                    id: content
                    antialiasing: true
                    anchors.fill: parent;
                    anchors.margins: 6;

                    NotebookPage
                    {
                        id: notebookPage
                        model: getPageModel(container.pageNum)
                        state: dataProvider.pageState
                        controlId: dataProvider.controlId;
                        dataProvider: notebookPanel.dataProvider;
                        ListModel
                        {
                            id: pageModel
                        }

                        onItemHeightChanged:
                        {
                            itemsHeightChanged(notebookPage.getItemsHeight() + pagesNavigatorContainer.height+32);
                            pagesView.height = notebookPage.getItemsHeight();
                            updateModelsTimer.stop();
                        }

                        function getPageModel(page_num)
                        {
                            var n = dataProvider.controlsCount;
                            var i0 = page_num*itemsOnPage;
                            var i1 = page_num*itemsOnPage + itemsOnPage;
                            if (i1 > n) i1 = n;

                            pageModel.clear();

                            for(var i=i0; i < i1; ++i)
                            {
                                var code_prop = control_codes[i];
                                var o = {
                                    codeControl: code_prop
                                };
                                pageModel.append(o);
                            }

                            return pageModel;
                        }

                }
            }
        }
    }

    //
    // Pages navigator
    // == o o o o o ==
    //
    Rectangle
    {
        id: pagesNavigatorContainer
        y: 372
        height: 20
        anchors {bottom: parent.bottom }
        color: "transparent"
        anchors.right: parent.right
        anchors.rightMargin: 45
        anchors.left: parent.left
        anchors.leftMargin: 45
        clip: true
        anchors.bottomMargin: 0

        onWidthChanged:
        {
            updatePosition();
        }

        NumberAnimation { id: rowAnim; target: navigatorRow; property: "x"; to: 0; duration: 400; easing.type: Easing.OutQuart }


        function updatePosition()
        {
            if (pagesNavigator.count == 0)
                return;

            var curr_index_x = pagesView.currentIndex*30;
            var curr_pos_x = navigatorRow.x + curr_index_x;
            var navigator_w = 30*pagesNavigator.model - 20

            if (navigator_w <= pagesNavigatorContainer.width)
            {
                var center_pos_x = pagesNavigatorContainer.width/2 - navigator_w/2
                navigatorRow.x = center_pos_x;
                return;
            }

            if (curr_pos_x <= 30)
            {
                var row_x = -curr_index_x + 30;
                if (row_x > 0)
                    row_x = 0;
                rowAnim.to = row_x;
                rowAnim.start();
                return;
            }


            if (curr_pos_x >= pagesNavigatorContainer.width-30)
            {
                row_x = 30*Math.ceil(pagesNavigatorContainer.width/30) - 2*30 - curr_index_x;
                if (row_x > 0)
                    row_x = 0;
                rowAnim.to = row_x;
                rowAnim.start();
                return;
            }
        }

        Row
        {
            id: navigatorRow
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 0
            anchors.top: parent.top
            anchors.topMargin: 0
            spacing: 20

            Timer
            {
                id: updatePosTimer;
                repeat: false;
                interval: 300;

                onTriggered:
                {
                    pagesNavigatorContainer.updatePosition();
                    updatePosTimer.stop();
                }
            }

            Repeater
            {
                id: pagesNavigator

                onModelChanged:
                {
                    navigatorRow.x = 0;
                    updatePosTimer.restart();
                }

                Item
                {
                    width: 10
                    height: 10

                    Rectangle
                    {
                        id: buttonRect
                        width: mouseArea.hovered ? 6 : 4;
                        height: mouseArea.hovered ? 6 : 4;
                        radius: mouseArea.hovered ? 3 : 2;

                        color: pagesView.currentIndex == index ? "lightgray" : "black"

                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.verticalCenter: parent.verticalCenter

                        MouseArea
                        {
                            id: mouseArea

                            width: 20; height: 20
                            anchors.centerIn: parent

                            hoverEnabled: true;
                            property bool hovered: false;

                            onEntered:
                            {
                                hovered = true;
                            }

                            onExited:
                            {
                                hovered = false;
                            }

                            onClicked:
                            {
                                pagesView.currentIndex = index
                                pagesNavigatorContainer.updatePosition();
                                hovered = false;
                            }
                        }
                    }
                }
            }
        }
    }
    }


}
