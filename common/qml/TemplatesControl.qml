import QtQuick 2.0

import com.vectools.DataProvider 1.0

Item {
    id: templatesControl
    width: 104
    height: 40

    clip: true

    property DataProvider dataProvider;
    property variant codeControl;

    property int pageWidth: 35
    property int itemsOnPage: 5
    property int pagesCount: 2

    property int currentIndex: -1
    property int page: -1

    onCurrentIndexChanged:
    {
        page = Math.floor(currentIndex/itemsOnPage)
    }

    onDataProviderChanged:
    {
        var tmpls_str = dataProvider.getTemplatesStr();

        var model_arr = tmpls_str.split(',');
        repeater.model = model_arr;
        pagesCount = Math.floor((model_arr.length-1)/itemsOnPage);
        switch(pagesCount)
        {
        case 0: pagesNavigator.model = [0];
            break;
        case 1: pagesNavigator.model = [0,1];
            break;
        case 2: pagesNavigator.model = [0,1,2];
            break;
        }
        navigatorRow.width = pagesCount*(10 + 10) - 10
    }

    onPageChanged:
    {
        rowAnim.to = -pageWidth*itemsOnPage*page + 2;
        rowAnim.start();
    }

    enabled: dataProvider.getBoolValue(codeControl, DataProvider.IsEnabled);

    onEnabledChanged:
    {
        if (enabled)
        {
            opacity = 1;
        } else {
            opacity = 0.6;
        }
    }

    Connections
    {
        target: dataProvider;

        onPropertiesControlSelected:
        {
            if (control_code === 0)
            {
                visible = false;
                return;
            }

            var tmpl_type = dataProvider.getVariantValue(control_code, DataProvider.TemplateType);
            var tmpl_index = getItemIndex(tmpl_type);

            if (tmpl_index < 0)
                return;

            visible = true;

            currentIndex = tmpl_index;
            codeControl = control_code;
        }
    }


    function getItemIndex(tmpl_type)
    {
        var tmpl_index=-1;
        for(var i=0; i < repeater.model.length; ++i)
        {
            if (parseInt(repeater.model[i], 10) === tmpl_type)
                return i;
        }
        return -1;
    }

    NumberAnimation { id: rowAnim; target: itemsRow; property: "x"; to: 0; duration: 600; easing.type: Easing.OutQuart }

    Row
    {
        y: 2;
        x: 2;
        id: itemsRow;
        spacing: 6;

        Repeater
        {
            id: repeater

        /*    model: [
                DataProvider.Linear,
                DataProvider.Sigmoid,
                DataProvider.Circles,
                DataProvider.Radial,
                DataProvider.Spiral,
                DataProvider.Halftone,
                DataProvider.Wareframes,
                DataProvider.Peano,
                DataProvider.Trace,
                DataProvider.Painting
            ]*/

            TemplateItem
            {
                templateId: modelData;
                selected: index == currentIndex;
                itemIndex: index;

                onTemplateSelected:
                {
                    if (!dataProvider.getBoolValue(codeControl, DataProvider.IsEnabled))
                        return;
                    templatesControl.currentIndex = selectedIndex;
                    var tmpl_id = parseInt(repeater.model[selectedIndex], 10);
                    dataProvider.setVariantValue(codeControl, DataProvider.TemplateType, tmpl_id);
                    dataProvider.notifyValuesChanged();
                    dataProvider.editFinished();
                }
            }
        }

    }

    Rectangle
    {
        color: 'transparent'
        height: 20
        width: (pagesCount + 1)*(10 + navigatorRow.spacing) - navigatorRow.spacing
        anchors.horizontalCenter: parent.horizontalCenter
        y: itemsRow.height+4
        Row
        {
            id: navigatorRow
            spacing: 10
            anchors.fill: parent

            Repeater
            {
                id: pagesNavigator
                model: [0, 1, 2]

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

                        color: templatesControl.page == index ? "lightgray" : "black"

                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.verticalCenter: parent.verticalCenter

                        MouseArea
                        {
                            id: mouseArea

                            width: 15; height: 15
                            anchors.centerIn: parent

                            hoverEnabled: true;
                            property bool hovered: false;

                            onEntered:
                            {
                                if (dataProvider.getBoolValue(codeControl, DataProvider.IsEnabled))
                                {
                                    hovered = true;
                                }
                            }

                            onExited:
                            {
                                hovered = false;
                            }

                            onClicked:
                            {
                                if (dataProvider.getBoolValue(codeControl, DataProvider.IsEnabled))
                                {
                                    templatesControl.page = index
                                }
                                hovered = false;
                            }

                            onHoveredChanged:
                            {

                            }
                        }
                    }
                }
            }
        }

    }

}
