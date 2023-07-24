import QtQuick 2.3
import QtQuick.Window 2.2
import QtQuick.Controls 1.4 as QQC1
import QtQuick.Controls 2.14 as QQC2
import QtQuick.Layouts 1.2
import QtQuick.Dialogs 1.3
import TotalManager 1.0
import QtQml.Models 2.15


QQC1.ApplicationWindow
//https://doc.qt.io/qt-5/qml-qtquick-controls-applicationwindow.html
{
    id : root
    visible: true
    width: 1280
    height: 720
    title: qsTr("uCanView")
    onClosing: {
        mytotalmanager.closePort()
    }

    FontLoader {
        id:d2font
        source: "qrc:D2Coding.ttf"
    }

//    font.family: d2font

    property int transmitCurrentRow: transmitTable.currentRow
    property int reciveCurrentRow: reciveTable.currentRow
    property bool idFormatTransmit: mytotalmanager.idformat > 0 ? true: false

    property bool clickTransmit: false

    function sendCan(index) {
            mytotalmanager.write(index)
    }

    QQC1.ExclusiveGroup {
      id: idformat
    }
    QQC1.ExclusiveGroup {
      id: dataformat
    }

    menuBar: QQC1.MenuBar {
        //https://doc.qt.io/qt-5/qml-qtquick-controls-menubar.html
        id: menubar

        QQC1.Menu {
            title:"File"
            QQC1.MenuItem { text : "Open"; shortcut: "Ctrl+O"; onTriggered: fileDialog.open()}
            QQC1.Menu {
                id: recentFileSubMenu
                title : qsTr("Recent File")
//                width: 800
                Instantiator {
                    id: recentFilesInstantiator
                    model: mytotalmanager.recentFileModel
                        QQC1.MenuItem{
                            text: model.display
                            onTriggered: mytotalmanager.openFile(model.display)
                        }
                    onObjectAdded: recentFileSubMenu.insertItem(index, object)
                    onObjectRemoved: recentFileSubMenu.removeItem(object)
                }
            }
            QQC1.MenuItem { text : "Save"; shortcut: "Ctrl+S"; enabled: transmitCurrentRow > 0 ; onTriggered: savefileDialog.open()}

            QQC1.MenuSeparator{}

            QQC1.MenuItem { text: "Exit"; onTriggered: Qt.quit()}
        }

       QQC1. Menu{
            title:"CAN"
            QQC1.MenuItem { text:"Connect"; shortcut: "Ctrl+B"; onTriggered: myconnectdialog.open() }
            QQC1.MenuItem { text:"Disconnect"; shortcut: "Ctrl+D"; onTriggered: {mytotalmanager.closePort();} }
            QQC1.MenuItem { text:"Reset"; shortcut: "Esc";  onTriggered: mytotalmanager.countReset()}
        }

//        EditTap{ id : edittap}

        QQC1.Menu{
            title:"Edit Transmit"
            QQC1.MenuItem{ text:"Cut"; shortcut:"Ctrl+X"; onTriggered : clickTransmit ? mytotalmanager.transmitCut(transmitCurrentRow) : mytotalmanager.reciveCut(reciveCurrentRow)  }
            QQC1.MenuItem{ text:"Copy"; shortcut:"Ctrl+C"; onTriggered :  clickTransmit ? mytotalmanager.transmitCopy(transmitCurrentRow) :mytotalmanager.reciveCopy(reciveCurrentRow)  }
            QQC1.MenuItem{ text:"Paste"; shortcut:"Ctrl+V"; onTriggered : mytotalmanager.transmitPaste(transmitCurrentRow) }
            QQC1.MenuItem{ text:"Delete"; shortcut:"Del"; onTriggered : clickTransmit ? mytotalmanager.transmitDelete(transmitCurrentRow) : mytotalmanager.reciveDelete(reciveCurrentRow) }
            QQC1.MenuItem{ text:"Clear All"; shortcut:"Shift+Esc"; onTriggered: mytotalmanager.transmitClear()}
        }

        QQC1.Menu{
            title:"Transmit"
            QQC1.MenuItem { text: "New Message..."; shortcut:"Ins"; onTriggered: mynewdialog.open()}
            QQC1.MenuItem { text: "Edit Message..."; shortcut:"Enter"; enabled: transmitCurrentRow > 0; }
            QQC1.MenuItem { text: "Send"; shortcut:"Space"; onTriggered: transmitCurrentRow >= 0 ? sendCan(transmitCurrentRow) : ''}
        }


    }

    toolBar: QQC1.ToolBar {
    //https://doc.qt.io/qt-5/qml-qtquick-controls-toolbar.html
        RowLayout{
            anchors.fill: parent
            QQC1.ToolButton{
                iconSource: "icons/openfile.png"
                onClicked: fileDialog.open()
            }
        }
    }

    statusBar: QQC1.StatusBar{
         //https://doc.qt.io/qt-5/qml-qtquick-controls-statusbar.html
        RowLayout {
            anchors.fill: parent
            RowLayout{
                Text{
                    text: mytotalmanager.serialPortStatus ? "Connect" : "Disconnect"
                }
            }
        }
    }

    TotalManager { id: mytotalmanager }
    ConnectDialog { id: myconnectdialog }
    NewTransmitDialog { id :mynewdialog }
    EditTransmitDialog { id : myeditdialog }

    FileDialog{
        id: fileDialog
//        folder: "/Users/KyunU/Desktop/pet_project/uCanTools/common"
        title: "Please choose a file"
        nameFilters: [ "xmt files (*.xmt)"]
        folder: "file:../../common"
        onAccepted: {
            console.log(fileDialog.fileUrl)
            mytotalmanager.openFile(fileDialog.fileUrl)
            mytotalmanager.addRecentFileUrl(fileDialog.fileUrl);
        }

        onRejected: {
            console.log("Canceled")
        }
    }

    FileDialog{
        id: savefileDialog
        selectExisting: false
        folder: "file:/Users/KyunU/Desktop/pet_project/uCanTools/common"
        title: "Save Dialog"
        onAccepted: {
            console.log("User has selected :   " + savefileDialog.fileUrl)
            mytotalmanager.saveFile(savefileDialog.fileUrl)
        }
    }

    QQC1.SplitView {
        anchors.fill: parent
        orientation: Qt.Vertical

        RowLayout{
            Rectangle{
                color:"#3F51B5"
                width: 25
                Layout.fillHeight: parent

                Text {
                    text: qsTr("Recive")
                    color:"black"
                    anchors.centerIn: parent
                    font.pixelSize: 25
                    rotation: -90

                }
            }

            QQC1.TableView {
//            https://doc.qt.io/qt-5/qml-qtquick-controls-tableview.html
                id : reciveTable
                Layout.fillWidth: true
                Layout.fillHeight: true
                clip: true
                model: mytotalmanager.reciveModel //Edit Recive Model
                onCurrentRowChanged:  { mytotalmanager.selectedReciveRow(currentRow) }
                onClicked: {
                    clickTransmit = false;
//                    console.log("transmit click: " + clickTransmit)
                }

                QQC1.TableViewColumn{
    //                https://doc.qt.io/qt-5/qml-qtquick-controls-tableviewcolumn.html
                    role:"identifier"
                    title:"CAN-ID"
                    width:100
                }

                QQC1.TableViewColumn{
                    role:"frametype"
                    title:"Type"
                    width:50
                }

                QQC1.TableViewColumn{
                    role:"datalength"
                    title:"Length"
                    width: 60
                }

                QQC1.TableViewColumn{
                    role:"messagedata"
                    title:"Data"
                    width:160
                    delegate:
                        Text {
                        text: styleData.value
    //                     font.family: d2font.name
                        elide: Text.ElideRight
                        }
                }

                QQC1.TableViewColumn{
                    role:"count"
                    title:"Count"
                    width: 50
                }
            }


        }

        ColumnLayout{
            RowLayout{
                Rectangle{
                    color:"#90CAF9"
                    width: 25
                    Layout.fillHeight: parent

                    Text {
                        text: qsTr("Transmit")
                        color:"black"
                        anchors.centerIn: parent
                        font.pixelSize: 25
                        rotation: -90
                    }
                }
                MouseArea{
                    Layout.fillHeight : parent
                    Layout.fillWidth : parent
                    propagateComposedEvents : true
                    acceptedButtons: Qt.RightButton
                    onClicked: {
                        var row = transmitTable.rowAt(mouseX, mouseY)
                        if(row >= 0) {
                            transmitTable.selection.clear()
                            transmitTable.selection.select(row, row)
                            transmitTable.currentRow = row
//                            mytotalmanager.selectedTransmitRow(row)
                            contextMenu.popup()
                        }
                    }
                    QQC1.Menu{
                        id: contextMenu
                        QQC1.MenuItem { text: "New Message..."; /*shortcut:"Ins";*/ onTriggered: mynewdialog.open()}
                        QQC1.MenuItem { text: "Edit Message..."; /*shortcut:"Enter";*/ enabled: transmitCurrentRow > 0; }
                        QQC1.MenuSeparator{}
                        QQC1.MenuItem{ text:"Cut";/* shortcut:"Ctrl+X";*/ onTriggered : clickTransmit ? mytotalmanager.transmitCut(transmitCurrentRow) : mytotalmanager.reciveCut(reciveCurrentRow)  }
                        QQC1.MenuItem{ text:"Copy"; /*shortcut:"Ctrl+C";*/ onTriggered :  clickTransmit ? mytotalmanager.transmitCopy(transmitCurrentRow) :mytotalmanager.reciveCopy(reciveCurrentRow)  }
                        QQC1.MenuItem{ text:"Paste"; /*shortcut:"Ctrl+V";*/ onTriggered : mytotalmanager.transmitPaste(transmitCurrentRow) }
                        QQC1.MenuItem{ text:"Delete"; /*shortcut:"Del";*/ onTriggered : clickTransmit ? mytotalmanager.transmitDelete(transmitCurrentRow) : mytotalmanager.reciveDelete(reciveCurrentRow) }
                        QQC1.MenuItem{ text:"Clear All"; /*shortcut:"Shift+Esc";*/ onTriggered: mytotalmanager.transmitClear()}
                        QQC1.MenuSeparator{}
                        QQC1.Menu{
                            title:"CAN ID Format"
                            QQC1.MenuItem{ id:idformathex; onTriggered: {mytotalmanager.changeIDFormatHexadecimal(transmitCurrentRow ); console.log(mytotalmanager.idformat)} text:"Hexadeicmal"; checkable: true; checked: mytotalmanager.idformat === 0 ? true: false; exclusiveGroup: idformat }
                            QQC1.MenuItem{ id:idformatdec; onTriggered: {mytotalmanager.changeIDFormatDecimal(transmitCurrentRow); console.log(mytotalmanager.idformat)} text:"Decimal"; checkable: true ; checked: false; exclusiveGroup: idformat }
                        }
                        QQC1.Menu{
                            title:"Data Bytes Format"
                            QQC1.MenuItem{ id:dataformathex; text:"Hexadeicmal"; checkable: true; checked: true; exclusiveGroup: dataformat }
                            QQC1.MenuItem{ id:dataformatdec; text:"Decimal"; checkable: true; checked: false; exclusiveGroup: dataformat }
                            QQC1.MenuItem{ id:dataformatASCII; text:"ASCII"; checkable: true;checked: false; exclusiveGroup: dataformat }
                        }
                    }

                    QQC1.TableView {
            //            QML
            //            https://doc.qt.io/qt-5/qml-qtquick-controls-tableview.html
                        id : transmitTable
                        anchors.fill: parent
    //                    Layout.fillWidth: parent
    //                    Layout.fillHeight: parent
                        clip: true
                        model: mytotalmanager.transmitModel //Edit Recive Model
                        onDoubleClicked: myeditdialog.open()
                        onClicked: clickTransmit = true;

                        Keys.onReturnPressed: {
    //                        var selectedRowData = model.get(row)
                            myeditdialog.open();
                        }
    //                    sortIndicatorVisible : true
                        onCurrentRowChanged: mytotalmanager.selectedTransmitRow(currentRow)

                        headerDelegate: Rectangle {
                            width: Column.width
                            height: 15

                            color: "lightblue"
                            Text {
                                anchors.fill: parent
                                font.pixelSize: 12
                                text: styleData.value
                            }

                            MouseArea{
                                anchors.fill : parent
                                onClicked: {
                                    console.log(styleData.column)
                                }
                            }
                        }

                        QQC1.TableViewColumn{
            //                https://doc.qt.io/qt-5/qml-qtquick-controls-tableviewcolumn.html
                            role:"identifier"
                            title:"CAN-ID"
                            width:100


                        }

                        QQC1.TableViewColumn{
                            role:"frametype"
                            title:"Type"
                            width:50
                        }

                        QQC1.TableViewColumn{
                            role:"datalength"
                            title:"Length"
                            width: 60
                        }

                        QQC1.TableViewColumn{
                            role:"messagedata"
                            title:"Data"
                            width:160
                            delegate:
                                Text {
                                text: styleData.value
                                elide: Text.ElideRight

                                }
                        }

                        QQC1.TableViewColumn{
                            role:"cycletime"
                            title:"Cycle Time"
                            width: 80
                            delegate:
                                RowLayout{
                                    anchors.fill : parent
                                    QQC1.CheckBox{
                                    id: transmitcheckbox
                                    checked: checkedState
                                        onCheckedChanged: {
                                            mytotalmanager.checkBoxChanged(styleData.row, checkedState)
    //                                        sendCan(styleData.row)  //마지막 누를 때 한번 더 보내짐 개선 필요함
    //                                        console.log(styleData.row + ':' + styleData.row)
                                        }
                                    }
                                    Text{
                                        text: styleData.value
                                        elide: Text.ElideRight
                                        horizontalAlignment: Text.AlignRight
                                    }
                                }
                        }

                        QQC1.TableViewColumn{
                            role:"count"
                            title:"Count"
                            width: 50
                        }

                        QQC1.TableViewColumn{
                            role:"comment"
                            title:"Comment"
                            width: 720
                        }
                    } //Table View
                } // mouseArea
            }
        }
    }
}
