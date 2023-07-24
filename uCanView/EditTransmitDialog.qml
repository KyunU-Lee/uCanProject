import QtQuick 2.5
import QtQuick.Controls 2.5
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.3

Dialog{
    id:newtransmitdialog
    title: "Edit Transmit Message"

    property int dlcselect : dlccombo.currentValue;
    property var dataFields : []

    function myData() {
        dataFields = []
        for(var i = 0; i < repeater.count; ++i){
            dataFields += repeater.itemAt(i).text +  ' '
        }
        console.log("myData : " + dataFields + "repeater.count : " + repeater.count)
        return dataFields
    }

    property var maga: []
    function makeRowMessage() {
        maga = idfield.text + ' ' + cyclefield.text + ' ' + dlcselect + ' D ' + myData() + ';'
        + commentfield.text
        console.log("maga : " + maga)
        return maga;
    }

    property string messageData: transmitCurrentRow >= 0 ?  mytotalmanager.messageData : '' ; //list에 채워지기 전에 접근하는 것 같다 OUT OF RANGE
//    property string messageData: mytotalmanager.messageData
//    property var messageDataArray : messageData.split(" ")
    property var messageDataArray : messageData.length != 0 ? messageData.split(" ") : "00 00 00 00 00 00 00 00"

    width: 600
    height: 200
    standardButtons: Dialog.Ok | Dialog.Cancel | Dialog.Help
//  다이얼로그 버튼
    onAccepted: mytotalmanager.getEditMessageFromQML(makeRowMessage(), transmitCurrentRow);
//    onRejected:

    GridLayout{
        id:gridlayout; rows:2; columns: 3; columnSpacing: 5; anchors.fill: parent

        ColumnLayout{
            id:identifier

            Text { font.pixelSize: 15; text: qsTr("ID: Hex()") }
            TextField{ id: idfield; implicitHeight: 30; implicitWidth: 95;
                text: transmitCurrentRow >= 0 ?  mytotalmanager.canId : "";
//                text: mytotalmanager.canId;
                inputMask: "HHHHHHHH"; font.pixelSize: 15 }
        } // id field  1x1

        ColumnLayout{
            id:dlc

            Text { font.pixelSize: 15; text: qsTr("Length: ") }
            ComboBox{ id : dlccombo; implicitHeight: 30; implicitWidth: 80; font.pixelSize: 15
                currentIndex:7; model : [1,2,3,4,5,6,7,8]; editable: true;
                validator: IntValidator { top: 8; bottom: 1; } }
        } //  dlc field 1x2

        ColumnLayout{
            id: data
            Text { font.pixelSize: 15; text: qsTr("Data: (Hex)") }
            RowLayout{
                Repeater{
                    id: repeater
                    model: dlccombo.currentValue
                    TextField{
                        id: datafield; text: messageDataArray[index];
                        implicitHeight: 30; implicitWidth: 40; font.pixelSize: 15; inputMask: "HH";
                    }
                }
            }
        } // data field  1x3

        ColumnLayout {
            id: cycletime
            Text { font.pixelSize: 15; text: qsTr("Cycle Time:") }
            RowLayout{
                TextField{ id: cyclefield; implicitHeight: 30; implicitWidth: 60
                    font.pixelSize: 15; inputMask: "0000"; text: transmitCurrentRow >= 0 ?  mytotalmanager.cycleTime : ""; }

                Text { font.pixelSize: 15; text: qsTr("ms") }
            }
        } // cycle tiem 2x1

        ColumnLayout {

        } // black 2x2


        RowLayout {
            id : comment

            Text { font.pixelSize: 15; text: qsTr("Comment :") }
            TextField{
                id: commentfield
                implicitHeight: 30
                implicitWidth: 270
                font.pixelSize: 15
                text: transmitCurrentRow >= 0 ?  mytotalmanager.comment : "";
            }
        }
    }
}
