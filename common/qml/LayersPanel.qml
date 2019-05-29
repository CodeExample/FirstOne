import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.1

import com.vectools.ModelItem 1.0
import com.vectools.DataProvider 1.0
import com.vectools.DataServer 1.0

Item {
    id: layersPanel
    width: 200
    height: 360

    state: "SHOWED"

    function updateControls()
    {
    console.log("update controls");
        var selected_cnt = dataServer.selectionCount();

        addLayer.setEnabled(true);
        addTmpl.setEnabled(false);
        removeSelected.setEnabled(selected_cnt > 0);

        pinSelected.setEnabled(false);
        lockSelected.setEnabled(selected_cnt > 0);
        cloneTmpl.setEnabled(true);


        if (selected_cnt === 1)
        {
            var modelItem = dataServer.selectedItemAt(0);
            if (modelItem)
            {
                if (modelItem.isSection())
                {
                    pinSelected.setEnabled(false);
                    pinSelected.checked = false

                    lockSelected.setEnabled(true);
                    lockSelected.checked = modelItem.isLocked();

                    fusionOfTemplates.setEnabled(true);
                    fusionOfTemplates.visible = true;
                    fusionOfTemplates.checked = modelItem.isFusionOfTemplates();

                    addLayer.visible = true;
                    addTmpl.visible = false;
                    pinSelected.visible = false;

                    layerMask.checked = modelItem.isMaskTransparent();
                    layerMask.visible = true;
                    layerMask.setEnabled(true);

                    cloneTmpl.visible = false;
                }

                if (modelItem.isFreemesh())
                {
                    addTmpl.setEnabled(true);

                    pinSelected.setEnabled(true);
                    pinSelected.checked = modelItem.isPinnedMask();

                    lockSelected.setEnabled(true);
                    lockSelected.checked = modelItem.isLocked();

                    addLayer.visible = false;
                    addTmpl.visible = true;
                    fusionOfTemplates.visible = false;
                    pinSelected.visible = true;

                    layerMask.visible = true;
                    layerMask.checked = modelItem.isMaskTransparent();
                    layerMask.setEnabled(true);

                    cloneTmpl.visible = false;
                }

                if (modelItem.isTemplate())
                {
                    addTmpl.setEnabled(true);

                    pinSelected.setEnabled(false);
                    pinSelected.checked = false

                    lockSelected.setEnabled(true);
                    lockSelected.checked = modelItem.isLocked();

                    addLayer.visible = true;
                    addTmpl.visible = true;
                    fusionOfTemplates.visible = false;
                    pinSelected.visible = false;
                    layerMask.visible = false;
                    cloneTmpl.visible = true;
                    cloneTmpl.setEnabled(!modelItem.isCloneStrokes())
                }
            }

        } else {
            addLayer.visible = true;
            pinSelected.visible = false;
            layerMask.visible = false;
            cloneTmpl.visible = false;
            fusionOfTemplates.visible = false;
        }
    }

    Connections
    {
        target: dataServer;

        onHideLayers:
        {
            layersPanel.state = "HIDDEN";
        }

        onShowLayers:
        {
            layersPanel.state = "SHOWED";
        }

        onSelectionChanged:
        {
            updateControls();
        }

        onUpdateUIRequest:
        {
            updateControls();
        }
    }


    Rectangle
    {
        id: contentRect

        anchors.fill: parent;
        color: "#434650"
        focus: false;

        ModelItemsPanel
        {
            id: modelItemsPanel
            anchors.topMargin: 2
            anchors.bottomMargin: 40
            anchors.fill: parent
        }


        Rectangle
        {
            id: row
            y: 321
            height: 40
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 0
            anchors.right: parent.right
            anchors.rightMargin: 0
            anchors.left: parent.left
            anchors.leftMargin: 0
            color: "#3a3d48"
            focus: false;

            Row
            {
                id: buttonsRow
                anchors.leftMargin: 8
                anchors.topMargin: 2
                spacing: 2
                anchors.fill: parent;
                focus: false;

                ActionButton {
                    id: addLayer
                    anchors.verticalCenter: parent.verticalCenter
                    iconSource: "images/soda/add_layer.png"
                    iconDisabled: "images/soda/add_layer_dsbl.png"
                    toolTipText: "New Layer"
                    focus: false;

                    onClicked:
                    {
                        dataServer.doAction(DataServer.AddLayer);
                    }
                }

                ActionButton {
                    id: fusionOfTemplates
                    anchors.verticalCenter: parent.verticalCenter
                    iconSource: "images/soda/level_painting.png"
                    iconDisabled: "images/soda/level_painting_dsbl.png"
                    focus: false;
                    checkable: true
                    toolTipText: "Fusion Templates"
                    onClicked:
                    {
                        dataServer.doAction(DataServer.FusionOfTemplates, fusionOfTemplates.checked);
                    }
                }

                ActionButton {
                    id: addTmpl
                    anchors.verticalCenter: parent.verticalCenter
                    iconSource: "images/soda/add_tmpl.png"
                    iconDisabled: "images/soda/add_tmpl_dsbl.png"
                    focus: false;
                    toolTipText: "Add Template"

                    onClicked:
                    {
                        dataServer.doAction(DataServer.AddTmpl);
                    }
                }

                ActionButton {
                    id: pinSelected
                    anchors.verticalCenter: parent.verticalCenter
                    iconSource: "images/soda/pin.png"
                    iconDisabled: "images/soda/pin_dsbl.png"
                    checkable: true;
                    focus: false;

                    toolTipText: "Pin Mask"
                    onClicked:
                    {
                        dataServer.doAction(DataServer.PinSelected, pinSelected.checked);
                    }
                }


                ActionButton {
                    id: layerMask
                    anchors.verticalCenter: parent.verticalCenter
                    iconSource: "images/soda/section_mask.png"
                    iconDisabled: "images/soda/section_mask.png"
                    focus: false;

                    toolTipText: "Local Mask"
                    checkable: true;
                    onClicked:
                    {
                        dataServer.doAction(DataServer.MaskTransparent, layerMask.checked);
                    }
                }


                ActionButton {
                    id: cloneTmpl
                    anchors.verticalCenter: parent.verticalCenter
                    iconSource: "images/soda/button_clone_tmpl.png"
                    focus: false;

                    toolTipText: "Clone Template"
                    onClicked:
                    {
                        dataServer.doAction(DataServer.CloneStrokes);
                    }
                }


                ActionButton {
                    id: lockSelected
                    anchors.verticalCenter: parent.verticalCenter
                    iconSource: "images/soda/lock.png"
                    iconDisabled: "images/soda/lock_dsbl.png"
                    checkable: true
                    focus: false;

                    toolTipText: "Lock Selected"

                    onClicked:
                    {
                        dataServer.doAction(DataServer.LockSelected, lockSelected.checked);
                    }
                }


                ActionButton {
                    id: removeSelected
                    anchors.verticalCenter: parent.verticalCenter
                    iconSource: "images/soda/remove.png"
                    iconDisabled: "images/soda/remove_dsbl.png"
                    focus: false;

                    toolTipText: "Remove"

                    onClicked:
                    {
                        dataServer.doAction(DataServer.RemoveSelected);
                    }
                }

            }
        }
    }


    states: [
        State {
            name: "SHOWED"

            PropertyChanges {
                target: contentRect;
                opacity: 1;
            }
        },
        State {
            name: "HIDDEN"

            PropertyChanges {
                target: contentRect;
                opacity: 0;
            }
        },
        State {
            name: "DUSKY"

            PropertyChanges {
                target: contentRect;
                opacity: 0.2;
            }
        }

    ]

    transitions: Transition {
        NumberAnimation { properties: "opacity"; easing.type: Easing.InOutQuad; duration: 100 }
    }

}




