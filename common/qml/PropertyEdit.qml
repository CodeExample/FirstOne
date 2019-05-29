import QtQuick 2.0

import com.vectools.DataProvider 1.0

Item {
    id: propertyEdit

    width: 80
    height: 25

    state: "SPINEDIT"

    property DataProvider dataProvider;
    property variant codeControl;
    property int propertyId: 0;
    property string suffix


    function updateSpinEdit(icon_src, min_value, max_value, value, inc, decimals, suffix, tool_tip)
    {
        icon.source = icon_src;
        spinEdit.minValue = min_value;
        spinEdit.maxValue = max_value;
        spinEdit.value = value;
        //spinEdit.inputMask = input_mask;
        spinEdit.suffix = suffix;
        spinEdit.incriment = inc;
        spinEdit.decimals = decimals;
        spinEdit.toolTipText = tool_tip;
    }



    ListModel
    {
        id: parityPosModel

        ListElement {
            itemId: 0
            icon: ""//"images/halftone/all.png"
            itemText: "all"

        }
        ListElement {
            itemId: 1
            icon: ""//"images/halftone/odd.png"
            itemText: "odd"
        }
        ListElement {
            itemId: 2
            icon: ""//"images/halftone/even.png"
            itemText: "even"
        }
    }

    ListModel
    {
        id: typeRotationModel

        ListElement {
            itemId: 0
            icon: "images/halftone/off.png"
            itemText: ""
        }
        ListElement {
            itemId: 1
            icon: "images/halftone/step.png"
            itemText: ""
        }
        ListElement {
            itemId: 2
            icon: "images/halftone/color.png"
            itemText: ""
        }
    }

    ListModel
    {
        id: morphingModel

        ListElement {
            itemId: 0
            icon: ""//"images/halftone/sharp.png"
            itemText: "sharp"
        }
        ListElement {
            itemId: 1
            icon: ""//"images/halftone/smooth.png"
            itemText: "smooth"
        }
    }


    ListModel
    {
        id: freeCurveModel

        ListElement {
            itemId: 0
            icon: ""//"images/freecurve/hand.png"
            itemText: "hand"
        }
        ListElement {
            itemId: 1
            icon: ""//"images/freecurve/morph.png"
            itemText: "blend"
        }
        ListElement {
            itemId: 2
            icon: ""//"images/freecurve/norm.png"
            itemText: "norm"
        }
        ListElement {
            itemId: 3
            icon: ""//"images/freecurve/quad.png"
            itemText: "quad"
        }
        ListElement {
            itemId: 4
            icon: ""//"images/freecurve/hex.png"
            itemText: "hex"
        }
        ListElement {
            itemId: 5
            icon: ""//"images/freecurve/circle.png"
            itemText: "circle"
        }
        ListElement {
            itemId: 6
            icon: ""//"images/freecurve/mask.png"
            itemText: "mask"
        }
    }

    ListModel
    {
        id: outlinesModel

        ListElement {
            itemId: 0
            icon: ""
            itemText: "hand"
        }
        ListElement {
            itemId: 1
            icon: ""
            itemText: "morph"
        }
        ListElement {
            itemId: 2
            icon: ""
            itemText: "edges"
        }
    }

    ListModel
    {
        id: traceDensityModel

        ListElement {
            itemId: 0
            icon: ""
            itemText: "high"
        }
        ListElement {
            itemId: 1
            icon: ""
            itemText: "normal"
        }
        ListElement {
            itemId: 2
            icon: ""
            itemText: "medium"
        }
        ListElement {
            itemId: 2
            icon: ""
            itemText: "low"
        }
    }

    ListModel
    {
        id: curveCupModel

        ListElement {
            itemId: 0
            icon: "images/style_quad_cup.png"
            itemText: ""
        }
        ListElement {
            itemId: 1
            icon: "images/style_round_cup.png"
            itemText: ""
        }
    }


    onPropertyIdChanged:
    {
    console.log("onPropertyIdChanged", propertyId);
        switch(propertyId)
        {
        case DataProvider.Angle:
            updateSpinEdit("images/prop_angle.png", -180, 180, dataProvider.getDoubleValue(codeControl, propertyId),
                           5, 0, "\u00b0", "Angle");
            break;

        case DataProvider.Interval:
            updateSpinEdit("images/prop_interval.png",
                           dataProvider.getDoubleValue(codeControl, DataProvider.IntervalMinValue),
                           dataProvider.getDoubleValue(codeControl, DataProvider.IntervalMaxValue),
                           dataProvider.getDoubleValue(codeControl, propertyId),
                           0.1, 2, "mm", "Interval");
            break;
        case DataProvider.Dispersion:
            updateSpinEdit("images/prop_dispersion.png", 0, 999, dataProvider.getDoubleValue(codeControl, propertyId),
                           0.1, 2, "mm", "Dispersion");
            break;
        case DataProvider.Middle:
            updateSpinEdit("images/prop_middle.png", 0, 100, dataProvider.getDoubleValue(codeControl, propertyId),
                           10, 0, "%", "Middle");
            break;

        case DataProvider.Shear:
            updateSpinEdit("images/soda/prop_shear.png", 0, 100, dataProvider.getDoubleValue(codeControl, propertyId),
                           10, 0, "%", "Shear");
            break;

        case DataProvider.Wave:
            updateSpinEdit("images/prop_wave.png", 0, 100, dataProvider.getDoubleValue(codeControl, propertyId),
                           1, 0, "mm", "Wave");
            break;
        case DataProvider.Fading:
            updateSpinEdit("images/prop_fading.png", -100, 100, dataProvider.getDoubleValue(codeControl, propertyId),
                           1, 0, "mm", "Fading");
            break;
        case DataProvider.Period:
            updateSpinEdit("images/prop_period.png", -100, 100, dataProvider.getDoubleValue(codeControl, propertyId),
                           0.5, 1, "", "Period");
            break;
        case DataProvider.Phase:
            updateSpinEdit("images/prop_phase.png", -180, 180, dataProvider.getDoubleValue(codeControl, propertyId),
                           15, 0, "\u00b0", "Phase");
            break;

        case DataProvider.CenterX:
            updateSpinEdit("images/prop_center_x.png", -1000, 1000, dataProvider.getDoubleValue(codeControl, propertyId),
                           10, 0, "mm", "Center X");
            break;
        case DataProvider.CenterY:
            updateSpinEdit("images/prop_center_y.png", -1000, 1000, dataProvider.getDoubleValue(codeControl, propertyId),
                           10, 0, "mm", "Center Y");
            break;

        case DataProvider.Radius:
            updateSpinEdit("images/prop_radius.png", 0, 1000, dataProvider.getDoubleValue(codeControl, propertyId),
                           10, 0, "mm", "Radius");
            break;

        case DataProvider.PeanoDepth:
            updateSpinEdit("images/soda/prop_peano_depth.png", 3, 10, dataProvider.getDoubleValue(codeControl, propertyId),
                           1, 0, "", "Depth");
            break;

        case DataProvider.Detailing:
            updateSpinEdit("images/soda/prop_detailing.png", 0, 100, dataProvider.getDoubleValue(codeControl, propertyId),
                           1, 0, "%", "Detailing");
            break;

        case DataProvider.ClearingLevel:

            updateSpinEdit("images/soda/prop_clearing_level.png", 0, 100, dataProvider.getDoubleValue(codeControl, propertyId),
                           1, 0, "mm", "Minimal size");
            break;

        case DataProvider.Smoothness:
            updateSpinEdit("images/soda/prop_smoothness.png", 0, 25, dataProvider.getDoubleValue(codeControl, propertyId),
                           1, 0, "", "Smoothness");
            break;

        case DataProvider.Averaging:
            updateSpinEdit("images/soda/prop_smoothness.png", 0, 25, dataProvider.getDoubleValue(codeControl, propertyId),
                           1, 0, "", "Smoothness");
            break;

        case DataProvider.CellSize:
            updateSpinEdit("images/prop_interval.png", 0.01, 10, dataProvider.getDoubleValue(codeControl, propertyId),
                           0.1, 2, "mm", "Cell Size");
            break;

        case DataProvider.Rotation:
            updateSpinEdit("images/prop_angle.png", -180, 180, dataProvider.getDoubleValue(codeControl, propertyId),
                           5, 0, "\u00b0", "Angle");
            break;

        case DataProvider.Auto:
            propertyEdit.state = "CHECKEDIT";
            spinEdit.visible = false;
            checkBox.visible = true;
            checkBox.checked = dataProvider.getBoolValue(codeControl, propertyId);
            icon.width = 4;
            checkBox.text = "Auto";
            checkBox.toolTipText = "Auto radius";
            break;

        case DataProvider.AutoRnd:
            propertyEdit.state = "CHECKEDIT";
            spinEdit.visible = false;
            checkBox.visible = true;
            checkBox.checked = dataProvider.getBoolValue(codeControl, propertyId);
            icon.width = 4;
            checkBox.text = "Random";
            checkBox.toolTipText = "Random radius";
            break;

        case DataProvider.ExpandLines:
            propertyEdit.state = "CHECKEDIT";
            spinEdit.visible = false;
            checkBox.visible = true;
            checkBox.checked = dataProvider.getBoolValue(codeControl, propertyId);
            icon.width = 4;
            checkBox.text = "Expand";
            checkBox.toolTipText = "Expand Lines";
            break;


        case DataProvider.ParityPos:
            propertyEdit.state = "ITEMSLIST";
            checkBox.visible = false;
            spinEdit.visible = false;
            itemsListRow.visible = true;
            itemsListIcon.source = "images/prop_item_pos.png"
            itemsListBox.model = parityPosModel;
            itemsListBox.selectedIndex = dataProvider.getIntValue(codeControl, propertyId);
            itemsListBox.forceActiveFocus();
            break;

        case DataProvider.TypeRotation:
            propertyEdit.state = "ITEMSLIST";
            checkBox.visible = false;
            spinEdit.visible = false;
            itemsListRow.visible = true;
            itemsListIcon.source = "images/prop_rotate.png"
            itemsListBox.model = typeRotationModel;
            itemsListBox.selectedIndex = dataProvider.getIntValue(codeControl, propertyId);
            itemsListBox.forceActiveFocus();
            break;

        case DataProvider.TypeMorphing:
            propertyEdit.state = "ITEMSLIST";
            checkBox.visible = false;
            spinEdit.visible = false;
            itemsListRow.visible = true;
            itemsListIcon.source = "images/prop_morph.png"
            itemsListBox.model = morphingModel;
            itemsListBox.selectedIndex = dataProvider.getIntValue(codeControl, propertyId);
            itemsListBox.forceActiveFocus();
            break;

        case DataProvider.ConvolutionType:
            propertyEdit.state = "ITEMSLIST";
            checkBox.visible = false;
            spinEdit.visible = false;
            itemsListRow.visible = true;
            itemsListIcon.source = "images/prop_conv.png"
            itemsListBox.model = freeCurveModel;
            itemsListBox.selectedIndex = dataProvider.getIntValue(codeControl, propertyId);
            itemsListBox.forceActiveFocus();
            break;

        case DataProvider.OutlinesType:
            propertyEdit.state = "ITEMSLIST";
            checkBox.visible = false;
            spinEdit.visible = false;
            itemsListRow.visible = true;
            itemsListIcon.source = "images/prop_conv.png"
            itemsListBox.model = outlinesModel;
            itemsListBox.selectedIndex = dataProvider.getIntValue(codeControl, propertyId);
            itemsListBox.forceActiveFocus();
            break;

        case DataProvider.CurveCup:
            propertyEdit.state = "ITEMSLIST";
            checkBox.visible = false;
            spinEdit.visible = false;
            itemsListRow.visible = true;
            itemsListIcon.source = ""
            itemsListBox.model = curveCupModel;
            itemsListBox.selectedIndex = dataProvider.getIntValue(codeControl, propertyId);
            itemsListBox.forceActiveFocus();
            break;

        case DataProvider.TraceDensityType:
            propertyEdit.state = "ITEMSLIST";
            checkBox.visible = false;
            spinEdit.visible = false;
            itemsListRow.visible = true;
            itemsListIcon.source = "images/prop_steps.png"
            itemsListBox.model = traceDensityModel;
            itemsListBox.selectedIndex = dataProvider.getIntValue(codeControl, propertyId);
            itemsListBox.forceActiveFocus();
            break;

        }
    }



    Row
    {
        id: row
        x: 0
        width: 90
        anchors.horizontalCenterOffset: 14
        anchors.bottomMargin: 0
        anchors.topMargin: 0
        anchors.leftMargin: 28
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.top: parent.top
        layoutDirection: Qt.RightToLeft

        SpinEdit {
            id: spinEdit
            width: 60
            anchors.verticalCenter: row.verticalCenter;
            visible: false;
            onEditingStarted:
            {
                dataProvider.editStarted()
            }

            onEditingFinished:
            {
                dataProvider.setDoubleValue(codeControl, propertyId, value);
                dataProvider.notifyValuesChanged()
                dataProvider.editFinished()
            }
        }


        CheckBox {
            id: checkBox;
            width: 76
            visible: false;
            anchors.verticalCenter: row.verticalCenter;
            onEditingFinished:
            {
                dataProvider.setBoolValue(codeControl, propertyId, checked);
                dataProvider.notifyValuesChanged()
                dataProvider.editFinished()
            }
        }

        Row
        {
            id: itemsListRow
            visible: false
            spacing: 1
            anchors.verticalCenter: row.verticalCenter;
            Image
            {
                id: itemsListIcon;
                width: 16;
                height: 16;
                anchors.verticalCenter: parent.verticalCenter
            }

            ItemsListBox
            {
                id: itemsListBox;
                width: 60;
                visible: false;
                anchors.verticalCenter: parent.verticalCenter

                onComboClicked:
                {
                    dataProvider.setIntValue(codeControl, propertyId, selectedIndex);
                    dataProvider.notifyValuesChanged()
                    dataProvider.editFinished()
                }
            }
        }

        Image {
            id: icon
            width: 16
            height: 16
            anchors.verticalCenter: parent.verticalCenter
        }

    }

    states: [
        State {
            name: "SPINEDIT"

            PropertyChanges {
                target: spinEdit
                visible: true
            }

            PropertyChanges {
                target: checkBox
                visible: false
            }
            PropertyChanges {
                target: itemsListBox
                visible: false
            }
        },
        State {
            name: "CHECKBOX"

            PropertyChanges {
                target: spinEdit
                visible: false
            }

            PropertyChanges {
                target: checkBox
                visible: true
            }
            PropertyChanges {
                target: itemsListBox
                visible: false
            }
        },
        State {
            name: "ITEMSLIST"

            PropertyChanges {
                target: spinEdit
                visible: false
            }

            PropertyChanges {
                target: checkBox
                visible: false
            }
            PropertyChanges {
                target: itemsListBox
                visible: true
            }
        }

    ]


}
