#ifndef SERIALPORTLIST_H
#define SERIALPORTLIST_H
#include <QAbstractTableModel>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QThread>
#include <QMutex>
#include <canmsgmodel.h>


class SerialPortList : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit SerialPortList(QObject *parent = nullptr);
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    void getPortList();

private:
    QVector<QString> _serialPortList;
};


class SerialWorker : public QObject
{
    Q_OBJECT

public:
    explicit SerialWorker(QThread* thread = nullptr);
    ~SerialWorker();
    bool getPortStatus(); //포트 상태 return (connect, disconnect)
    void requestedWork();
    bool setSerialPort();


public slots:
    void doWork();
    void setSingleShotItem(const CAN_ITEM& value);
    void disconnect();
    void setPort(const QString& value);
    void abort();
    void checkedListChanged(const CAN_ITEM& value);
    void readData();

signals:
    void finished();
    void workRequested();
    void countChanged(const int& value);
    void serialReadData(const QString& signal);

private:
    QThread* _thread = nullptr;
    QSerialPort* _serial = nullptr;
    QMutex _mutex;
    QByteArray _writebuf;
    bool _abort;
    bool _working;
    bool _portstatus = false;
    bool _singleShotEvent = false;
    QList<CAN_ITEM> _checkedList;
    CanMsgModel _signalConverter;
    CAN_ITEM _singleShotItem;
    QByteArray _serialReadData;
    bool unique = false;

    QString _portName = "COM5";
    qint32 _baudRate;
    QSerialPort::DataBits _dataBits;
    QSerialPort::Parity _parityBits;
    QSerialPort::StopBits _stopBits;
    QSerialPort::FlowControl _flowControl;


};
#endif // SERIALPORTLIST_H
