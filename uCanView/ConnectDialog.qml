import QtQuick 2.5
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.2


MessageDialog{
    id: connectmsg
    title : "Connect"
    property var curPort

    GroupBox{
        id: gridbox
        Layout.fillWidth: true
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
                        mytotalmanager.openPort(curPort)
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
}
