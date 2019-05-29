import QtQuick 2.0

import com.vectools.DataProvider 1.0

Item {
    id: pageItem

    property alias spacing: column.spacing;

    property int controlId: 0;

    property variant model;

    property DataProvider dataProvider;

    signal itemHeightChanged();

    onModelChanged:
    {
        installModel();
    }

    onStateChanged:
    {
        installModel();
    }

    function installModel()
    {
        switch(state)
        {
        case "CONTINUITY":
            thicknessView.model = 0;
            continuityView.model = model;
            dashesView.model = 0;
            styleView.model = 0;
            basreliefView.model = 0;
            maskView.model = 0;
            colorView.model = 0;
            propertiesView.model = 0;
            return;

        case "THICKNESS":
            continuityView.model = 0;
            thicknessView.model = model;
            dashesView.model = 0;
            styleView.model = 0;
            basreliefView.model = 0;
            maskView.model = 0;
            colorView.model = 0;
            propertiesView.model = 0;
            return;

        case "DASHES":
            continuityView.model = 0;
            thicknessView.model = 0;
            dashesView.model = model;
            styleView.model = 0;
            basreliefView.model = 0;
            maskView.model = 0;
            colorView.model = 0;
            propertiesView.model = 0;
            return;

        case "STYLE":
            continuityView.model = 0;
            thicknessView.model = 0;
            dashesView.model = 0;
            styleView.model = model;
            basreliefView.model = 0;
            maskView.model = 0;
            colorView.model = 0;
            propertiesView.model = 0;
            return;

        case "BASRELIEF":
            continuityView.model = 0;
            thicknessView.model = 0;
            dashesView.model = 0;
            styleView.model = 0;
            basreliefView.model = model;
            maskView.model = 0;
            colorView.model = 0;
            propertiesView.model = 0;
            return;

        case "MASK":
            continuityView.model = 0;
            thicknessView.model = 0;
            dashesView.model = 0;
            styleView.model = 0;
            basreliefView.model = 0;
            maskView.model = model;
            colorView.model = 0;
            propertiesView.model = 0;
            return;

        case "PROPERTIES":
            continuityView.model = 0;
            thicknessView.model = 0;
            dashesView.model = 0;
            styleView.model = 0;
            basreliefView.model = 0;
            maskView.model = 0;
            colorView.model = 0;
            propertiesView.model = model;
            return;

        case "COLOR":
            continuityView.model = 0;
            thicknessView.model = 0;
            dashesView.model = 0;
            styleView.model = 0;
            basreliefView.model = 0;
            maskView.model = 0;
            colorView.model = model;
            propertiesView.model = 0;
            return;

        }

    }

    function getItemsHeight()
    {
        switch(state)
        {
        case "CONTINUITY":
            return continuityView.getItemsHeight();

        case "THICKNESS":
            return thicknessView.getItemsHeight();

        case "DASHES":
            return dashesView.getItemsHeight();

        case "STYLE":
            return styleView.getItemsHeight();

        case "BASRELIEF":
            return basreliefView.getItemsHeight();

        case "MASK":
            return maskView.getItemsHeight();

        case "PROPERTIES":
            return propertiesView.height;

        case "COLOR":
            return colorView.height;

        }

        return 0;
    }

    Column
    {
        id: column
        anchors.fill: parent;
        anchors.topMargin: 10;
        anchors.bottomMargin: 10;
        anchors.leftMargin: 10;
        anchors.rightMargin: 10;

        // continuityView
        Repeater
        {
            id: continuityView
            visible: false

            delegate: ContinuityControl
            {
                dataProvider: pageItem.dataProvider
                codeControl: model.codeControl
                width: content.width - content.anchors.leftMargin - content.anchors.rightMargin

                onEditingStarted:
                {
                    dataProvider.editStarted();
                }
                onEditingFinished:
                {
                    dataProvider.editFinished();
                }
                onItemHeightChanged:
                {
                    pageItem.itemHeightChanged();
                }
            }

            function getItemsHeight()
            {
                var h=0;
                for(var i=0; i < continuityView.count; ++i)
                {
                    var item = continuityView.itemAt(i);
                    h +=item.height;
                }
                return h;
            }
        }

        // thicknessView
        Repeater
        {
            id: thicknessView
            visible: false
            delegate: ThicknessControl
            {
                dataProvider: pageItem.dataProvider
                codeControl: model.codeControl
                width: content.width - content.anchors.leftMargin - content.anchors.rightMargin

                onEditingStarted:
                {
                    dataProvider.editStarted();
                }
                onEditingFinished:
                {
                    dataProvider.editFinished();
                }
                //onItemHeightChanged:
                //{
                //    pageItem.itemHeightChanged();
                //}
            }

            function getItemsHeight()
            {
                var h=0;
                for(var i=0; i < thicknessView.count; ++i)
                {
                    var item = thicknessView.itemAt(i);
                    h +=item.height;
                }
                return h;
            }
        }


        // dashesView
        Repeater
        {
            id: dashesView
            visible: false
            delegate: DashesControl
            {
                dataProvider: pageItem.dataProvider
                codeControl: model.codeControl
                width: content.width - content.anchors.leftMargin - content.anchors.rightMargin

                onEditingStarted:
                {
                    dataProvider.editStarted();
                }
                onEditingFinished:
                {
                    dataProvider.editFinished();
                }
                onItemHeightChanged:
                {
                    pageItem.itemHeightChanged();
                }
            }

            function getItemsHeight()
            {
                var h=0;
                for(var i=0; i < dashesView.count; ++i)
                {
                    var item = dashesView.itemAt(i);
                    h +=item.height;
                }
                return h;
            }
        }


        // lineStyle
        Repeater
        {
            id: styleView
            visible: false
            delegate: StyleControl
            {
                dataProvider: pageItem.dataProvider
                codeControl: model.codeControl
                width: content.width - content.anchors.leftMargin - content.anchors.rightMargin

                onEditingStarted:
                {
                    dataProvider.editStarted();
                }
                onEditingFinished:
                {
                    dataProvider.editFinished();
                }
                onItemHeightChanged:
                {
                    pageItem.itemHeightChanged();
                }
            }

            function getItemsHeight()
            {
                var h=0;
                for(var i=0; i < styleView.count; ++i)
                {
                    var item = styleView.itemAt(i);
                    h +=item.height;
                }
                return h;
            }
        }

        // basreliefView
        Repeater
        {
            id: basreliefView
            visible: false

            delegate: BasreliefControl
            {
                dataProvider: pageItem.dataProvider
                codeControl: model.codeControl
                width: content.width - content.anchors.leftMargin - content.anchors.rightMargin

                onEditingStarted:
                {
                    dataProvider.editStarted();
                }
                onEditingFinished:
                {
                    dataProvider.editFinished();
                }
                onItemHeightChanged:
                {
                    pageItem.itemHeightChanged();
                }
            }

            function getItemsHeight()
            {
                var h=0;
                for(var i=0; i < basreliefView.count; ++i)
                {
                    var item = basreliefView.itemAt(i);
                    h +=item.height;
                }
                return h;
            }
        }

        // maskView
        Repeater
        {
            id: maskView
            visible: false

            delegate: MaskControl
            {
                dataProvider: pageItem.dataProvider
                codeControl: model.codeControl
                width: content.width - content.anchors.leftMargin - content.anchors.rightMargin

                onEditingStarted:
                {
                    dataProvider.editStarted();
                }
                onEditingFinished:
                {
                    dataProvider.editFinished();
                }
                onItemHeightChanged:
                {
                    pageItem.itemHeightChanged();
                }
            }

            function getItemsHeight()
            {
                var h=0;
                for(var i=0; i < maskView.count; ++i)
                {
                    var item = maskView.itemAt(i);
                    h +=item.height;
                }
                return h;
            }
        }

        // colorView
        Repeater
        {
            id: colorView
            visible: false

            delegate: ColorControl
            {
                dataProvider: pageItem.dataProvider
                codeControl: model.codeControl
                width: content.width - content.anchors.leftMargin - content.anchors.rightMargin
                height: getDelegateHeight()

                onEditingStarted:
                {
                    dataProvider.editStarted();
                }
                onEditingFinished:
                {
                    dataProvider.editFinished();
                }
                //onItemHeightChanged:
                //{
                //   pageItem.itemHeightChanged();
                //}
            }

            function getItemsHeight()
            {
                var h=0;
                for(var i=0; i < colorView.count; ++i)
                {
                    var item = colorView.itemAt(i);
                    h +=item.height;
                }
                return h;
            }
        }

        // propertiesView
        Repeater
        {
            id: propertiesView
            visible: false

            delegate: PropertiesControl
            {
                dataProvider: pageItem.dataProvider
                codeControl: model.codeControl
                width: content.width - content.anchors.leftMargin - content.anchors.rightMargin
                height: getDelegateHeight()

                onEditingStarted:
                {
                    dataProvider.editStarted();
                }
                onEditingFinished:
                {
                    dataProvider.editFinished();
                }
            }
        }

    }
    states: [
        State {
            name: "CONTINUITY"
            PropertyChanges { target: continuityView; visible: true}
            PropertyChanges { target: thicknessView; visible: false}
            PropertyChanges { target: dashesView; visible: false}
            PropertyChanges { target: styleView; visible: false}
            PropertyChanges { target: basreliefView; visible: false}
            PropertyChanges { target: maskView; visible: false}
            PropertyChanges { target: colorView; visible: false}
            PropertyChanges { target: propertiesView; visible: false}
        },
        State {
            name: "THICKNESS"
            PropertyChanges { target: continuityView; visible: false}
            PropertyChanges { target: thicknessView; visible: true}
            PropertyChanges { target: dashesView; visible: false}
            PropertyChanges { target: styleView; visible: false}
            PropertyChanges { target: basreliefView; visible: false}
            PropertyChanges { target: maskView; visible: false}
            PropertyChanges { target: colorView; visible: false}
            PropertyChanges { target: propertiesView; visible: false}
        },
        State {
            name: "DASHES"
            PropertyChanges { target: continuityView; visible: false}
            PropertyChanges { target: thicknessView; visible: false}
            PropertyChanges { target: dashesView; visible: true}
            PropertyChanges { target: styleView; visible: false}
            PropertyChanges { target: basreliefView; visible: false}
            PropertyChanges { target: maskView; visible: false}
            PropertyChanges { target: colorView; visible: false}
            PropertyChanges { target: propertiesView; visible: false}
        },
        State {
            name: "STYLE"
            PropertyChanges { target: continuityView; visible: false}
            PropertyChanges { target: thicknessView; visible: false}
            PropertyChanges { target: dashesView; visible: false}
            PropertyChanges { target: styleView; visible: true}
            PropertyChanges { target: basreliefView; visible: false}
            PropertyChanges { target: maskView; visible: false}
            PropertyChanges { target: colorView; visible: false}
            PropertyChanges { target: propertiesView; visible: false}
        },
        State {
            name: "BASRELIEF"
            PropertyChanges { target: continuityView; visible: false}
            PropertyChanges { target: thicknessView; visible: false}
            PropertyChanges { target: dashesView; visible: false}
            PropertyChanges { target: styleView; visible: false}
            PropertyChanges { target: basreliefView; visible: true}
            PropertyChanges { target: maskView; visible: false}
            PropertyChanges { target: colorView; visible: false}
            PropertyChanges { target: propertiesView; visible: false}
        },
        State {
            name: "MASK"
            PropertyChanges { target: continuityView; visible: false}
            PropertyChanges { target: thicknessView; visible: false}
            PropertyChanges { target: dashesView; visible: false}
            PropertyChanges { target: styleView; visible: false}
            PropertyChanges { target: basreliefView; visible: false}
            PropertyChanges { target: maskView; visible: true}
            PropertyChanges { target: colorView; visible: false}
            PropertyChanges { target: propertiesView; visible: false}
        },
        State {
            name: "PROPERTIES"
            PropertyChanges { target: continuityView; visible: false}
            PropertyChanges { target: thicknessView; visible: false}
            PropertyChanges { target: dashesView; visible: false}
            PropertyChanges { target: styleView; visible: false}
            PropertyChanges { target: basreliefView; visible: false}
            PropertyChanges { target: maskView; visible: false}
            PropertyChanges { target: colorView; visible: false}
            PropertyChanges { target: propertiesView; visible: true}
        },
        State {
            name: "COLOR"
            PropertyChanges { target: continuityView; visible: false}
            PropertyChanges { target: thicknessView; visible: false}
            PropertyChanges { target: dashesView; visible: false}
            PropertyChanges { target: styleView; visible: false}
            PropertyChanges { target: basreliefView; visible: false}
            PropertyChanges { target: maskView; visible: false}
            PropertyChanges { target: colorView; visible: true}
            PropertyChanges { target: propertiesView; visible: false}
        }





    ]

}
