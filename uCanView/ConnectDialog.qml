import QtQuick 2.5
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.2


MessageDialog{
    id: connectmsg
    title : "Connect"
    property var curPort

    RowLayout {
        GroupBox{
            id: gridbox
            Layout.fillWidth: parent/2
            width: parent/2

            GridLayout {
                rows:  2
                flow: GridLayout.TopToBottom
                anchors.fill: parent

                Label{
                    text: "Select Serial Port!"
                }
                Rectangle {
                    width: 150
                    height: 150
                    border.color: "black"
                    ListView {
                        anchors.fill: parent
                        model : mytotalmanager.portList
                        delegate: ItemDelegate {
                            text: model.display
                            onClicked: {
                                    curPort = model.display
                                console.log("Clicked: ", model.display)
                            }
                        }
                    }
                }
                ColumnLayout{
                //splitter
                }

                ColumnLayout{
                    Button {
                        id: connectYesbtn
                        text: "Connect"
                        onClicked: {
//                            console.log("baudrate:", baudRate.currentValue, "Data bits: ", databits.currentValue
//                                        ,"Parity bits: ", paritybits.currentValue, "stop bits: ", stopbits.currentValue
//                                        ,"Flow contorl: ", flowcontorl.currentValue)
                            mytotalmanager.openPort(curPort + ' ' +  baudRate.currentValue + ' '+ databits.currentValue
                                                    + ' ' + paritybits.currentValue + ' '+ stopbits.currentValue
                                                    + ' ' + flowcontorl.currentValue)
                            connectmsg.close()
                        }
                    }

                    Button {
                        text: "Cancel"
                        onClicked: connectmsg.close()
                        Shortcut {
                            sequence: "Esc"
                            onActivated: connectmsg.close()
                        }
                    }
                    Button {
                        text: "Update"
                        onClicked: mytotalmanager.updateSerialPort()
                    }
                }
            }
        }

        GroupBox {
            Layout.fillWidth: parent/2
            ColumnLayout {
                anchors.fill: parent

                Label{
                    text: "Select parameters"
                }
                RowLayout {
                    Label { text:"Baud Rate :       " }
                    ComboBox {
                        id: baudRate
                        model: ["9600", "19200", "38400", "57600", "115200"]
                        currentIndex: 4
                    }
                }
                RowLayout {
                    Label { text:"Data bits :         " }
                    ComboBox {
                        id: databits
                        model: ["8"]
                        enabled: false
                    }
                }
                RowLayout {
                    Label { text:"Parity bits :        " }
                    ComboBox {
                        id : paritybits
                        model: ["NoParity", "EvenParity", "OddParity", "SpaceParity", "MarkParity", "UnknownParity"]
                    }
                }
                RowLayout {
                    Label { text:"Stop bits :          " }
                    ComboBox {
                        id : stopbits
                        model: ["1", "2"]
                    }
                }
                RowLayout {
                    Label { text:"Flow control :     " }
                    ComboBox {
                        id:flowcontorl
                        model: ["NoFlowContorl", "HardwareControl"]
                    }
                }
            }
        }


    }


}
