#ifndef TOTALMANAGER_H
#define TOTALMANAGER_H
#include <QObject>
#include <QDebug>
#include "canmsgmodel.h"
#include "serialportlist.h"
#include "recentfilemodel.h"

class TotalManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(CanMsgModel *transmitModel READ gettransmitModel NOTIFY transmitModelChanged)
    Q_PROPERTY(SerialPortList *portList READ getportList NOTIFY portListChanged)
    Q_PROPERTY(CanMsgModel *reciveModel READ getreciveModel NOTIFY reciveModelChanged)
    Q_PROPERTY(RecentFileModel *recentFileModel READ getrecentFileModel NOTIFY recentFileModelChanged)
    Q_PROPERTY(bool serialPortStatus READ getserialPortStatus NOTIFY serialPortStatusChanged)
//edit message Q_PROPERTY 로 보내서 버그가 생기나
    Q_PROPERTY(QString canId READ getcanId NOTIFY canIdChanged)
    Q_PROPERTY(QByteArray messageData READ getmessageData NOTIFY messageDataChanged)
    Q_PROPERTY(int dataLength READ getdataLength NOTIFY dataLengthChanged)
    Q_PROPERTY(int cycleTime READ getcycleTime NOTIFY cycleTimeChanged)
    Q_PROPERTY(QString comment READ getcomment NOTIFY commentChanged)

//    Q_PROPERTY(bool portStatus READ getportStatus NOTIFY portStatusChanged())
//_msgModel에 있는리스트에 접근해서 해당 index에 있는


public:
    explicit TotalManager(QObject *parent = nullptr);
    ~TotalManager();

    Q_INVOKABLE void selectedTransmitRow(int tpos) { _selectedTransmitRow = tpos; /*qDebug() << tpos;*/}
    Q_INVOKABLE void selectedReciveRow(int rpos) { _selectedReciveRow = rpos;}

    //Transmit Works
    Q_INVOKABLE void openFile(QString fileUrl) { return _msgTransmitModel.openFile(fileUrl); }
    Q_INVOKABLE void saveFile(QString fileUrl) { return _msgTransmitModel.saveFile(fileUrl); }
    Q_INVOKABLE void transmitClear() {return _msgTransmitModel.clear();}
    Q_INVOKABLE void checkBoxChanged(const int& idx, const int& state);
    Q_INVOKABLE void getNewMessageFromQML(QString rowData) { return _msgTransmitModel.append(_msgTransmitModel.parserStringToItem(rowData));}
    Q_INVOKABLE void getEditMessageFromQML(QString rowData, int idx) { return _msgTransmitModel.swapListItem(_msgTransmitModel.parserStringToItem(rowData),idx); }
    Q_INVOKABLE void write(int index){ emit transmitSingleShotSignal(_msgTransmitModel.getIndexItem(index)); }
    Q_INVOKABLE void sort(int idx, Qt::SortOrder order) { _msgTransmitModel.sort(idx, order); }

//Edit Tap 기능
    Q_INVOKABLE void transmitCut(int idx) { _msgTransmitModel.cut(idx);}
    Q_INVOKABLE void transmitCopy(int idx) { _msgTransmitModel.copy(idx); }
    Q_INVOKABLE void transmitPaste(int idx) { _msgTransmitModel.paste(idx);}
    Q_INVOKABLE void transmitDelete(int idx) { _msgTransmitModel.removeAt(idx); }

//Recive Works
    Q_INVOKABLE void reciveCut(int idx) { _msgReciveModel.cut(idx);}
    Q_INVOKABLE void reciveCopy(int idx) { _msgReciveModel.copy(idx);}
    Q_INVOKABLE void reciveDelete(int idx) { _msgReciveModel.removeAt(idx);}
    Q_INVOKABLE void reciveClear() { _msgReciveModel.clear();}

//SeiralPort Works
    Q_INVOKABLE void updateSerialPort() { return _portList.getPortList();}  //컴퓨터에 연결된 포트 리스트 최신화   // 좋은 코드는 아님.. 임시용

//CAN Tap
    Q_INVOKABLE void openPort(QString portname);
    Q_INVOKABLE void closePort();
    Q_INVOKABLE void countReset();

//우클릭 popup
    Q_INVOKABLE void changeIDFormatHexadecimal(int index) { _msgTransmitModel.changeIDFormatHexadecimal(index);}
    Q_INVOKABLE void changeIDFormatDecimal(int index) { _msgTransmitModel.changeIDFormatDecimal(index);}
    Q_INVOKABLE void changeDATAFormatHexadecimal(int index) { _msgTransmitModel.changeDATAFormatHexadecimal(index);}
    Q_INVOKABLE void changeDATAFormatDecimal(int index) { _msgTransmitModel.changeDATAFormatDecimal(index);}
    Q_INVOKABLE void changeDATAFormatASCII(int index) { _msgTransmitModel.changeDATAFormatASCII(index);}

// RecentFileModel
    Q_INVOKABLE void addRecentFileUrl(QString fileurl) { _recentFileModel.addFileUrl(fileurl);}

//edit message
    QString getcanId();
    QByteArray getmessageData();
    int getdataLength() { return _msgTransmitModel.getdataLength(_selectedTransmitRow);}
    int getcycleTime() { return _msgTransmitModel.getcycletime(_selectedTransmitRow); }
    QString getcomment(){ return _msgTransmitModel.getcomment(_selectedTransmitRow); }
    Q_INVOKABLE int getidformat() { return _msgTransmitModel.getFormat(_selectedTransmitRow); }
    Q_INVOKABLE int getDataFormat() { return _msgTransmitModel.getDataFormatStatus(_selectedTransmitRow);}

// Q_PROPERTY
    SerialPortList* getportList() { return &_portList;} //컴퓨터에 연결된 시리얼 포트 리스트 MODEL Q_PROPERTY로 연결
    CanMsgModel* gettransmitModel() { return &_msgTransmitModel;} //DATA MODEL Q_PROPERTY로 연결됨
    CanMsgModel* getreciveModel() { return &_msgReciveModel;}
    RecentFileModel* getrecentFileModel() { return &_recentFileModel;}
    bool getserialPortStatus() { return _threadStatus;}

//Thread
//    Q_INVOKABLE void startThreadWork();

public slots:
    void increaseCount(const int& value);
    void processingReadData(const QString& value);


signals:
    void transmitModelChanged();
    void reciveModelChanged();
    void recentFileModelChanged();
    void portListChanged();
    void checkModelChanged(const CAN_ITEM& newValue);
    void finished();
    void portNameChanged(const QString& value);
    void transmitSingleShotSignal(const CAN_ITEM& value);
    void serialPortStatusChanged();


//edit message
    void canIdChanged();
    void messageDataChanged();
    void dataLengthChanged();
    void cycleTimeChanged();
    void commentChanged();

private:
    CanMsgModel _msgTransmitModel;
    CanMsgModel _msgReciveModel;
    RecentFileModel _recentFileModel;

    SerialPortList _portList;
    SerialWorker* _serialWorker = nullptr;
    QThread* _workerThread = nullptr;
//    SerialPort _serialPort;
    int _selectedTransmitRow =0;
    int _selectedReciveRow;
    bool _threadStatus;
};

#endif // TOTALMANAGER_H
