#include "totalmanager.h"
#include <QDebug>
#include <QQmlEngine>
//#include <iostream>
#include <QFile>
#include <QSaveFile>


TotalManager::TotalManager(QObject *parent) : QObject(parent)
{
    qDebug() << "Created Total Manager";
//    CAN_ITEM test;
//    QString testId = "00CF1250Yeee";
//    bool ok;
//    testId.toUInt(&ok, 16);
//    qDebug() << testId << ok;
//    test._identifier = testId.toLocal8Bit();
//    test._cycletime = 00;
//    test._datalength = 0;
//    test._frametype = 'D';
//    test._messagedata = QByteArray::fromHex("121212121212121212").toHex(' ');
//    test._comment = "COMMENT";

//    _msgModel.append(test);

    qDebug() << "TOtal worker process in Thread " << thread()->currentThreadId();
//    qDebug() << canitemTostring(test);
//    _msgModel.append(stringTocanitem("00CF1250Yeeeh 0 0 D 12 12 12 12 12 12 12 12 12 ;COMMENT"));
//    _msgTransmitModel.openFile("file:///C:/Users/KyunU/Desktop/pet_project/uCanTools/common/Tractor_CAN_SIGNAL/TS130_CP6ANB/T130.xmt");
//    _msgModel.saveFile("file:///C:/Users/KyunU/Desktop/pet_project/uCanTools/common/Tractor_CAN_SIGNAL/TS130_CP6ANB/saveTest.xmt");
    _threadStatus = false;
    _workerThread = new QThread();
    _serialWorker = new SerialWorker(_workerThread);

    connect(this, SIGNAL(checkModelChanged(CAN_ITEM)), _serialWorker, SLOT(checkedListChanged(CAN_ITEM))); //QList를 넘겨준다?
    connect(this, SIGNAL(transmitSingleShotSignal(CAN_ITEM)), _serialWorker, SLOT(setSingleShotItem(CAN_ITEM)));

    connect(_serialWorker, SIGNAL(serialReadData(QString)), this, SLOT(processingReadData(QString))); // Read Data

    connect(this, SIGNAL(portNameChanged(QString)), _serialWorker, SLOT(setPort(QString)));
    connect(_serialWorker, SIGNAL(countChanged(int)), this, SLOT(increaseCount(int)),Qt::QueuedConnection);      //count++

    connect(_serialWorker, SIGNAL(workRequested()), _workerThread, SLOT(start()));
    connect(_workerThread, SIGNAL(started()), _serialWorker, SLOT(doWork()));
    connect(_serialWorker, SIGNAL(finished()), _workerThread, SLOT(quit()), Qt::DirectConnection);
}

TotalManager::~TotalManager()
{
    _serialWorker->abort();
    _workerThread->wait();
    delete _workerThread;
    delete _serialWorker;

    qDebug() << "TOtalman destroyed";
}

void TotalManager::checkBoxChanged(const int& idx, const int& state)
{
    qDebug() << "checkBoxChanged:" << idx;
    emit checkModelChanged(_msgTransmitModel.onCheckedChanged(idx, state));

}

void TotalManager::openPort(QString portname)
{
    emit portNameChanged(portname);
    _threadStatus = true;
    emit serialPortStatusChanged();
    _serialWorker->requestedWork();
//    _threadStatus = true;
}

void TotalManager::closePort()
{
    _serialWorker->abort();
    emit serialPortStatusChanged();
    _threadStatus = false;
}

void TotalManager::countReset()
{
    _msgTransmitModel.countReset();
    _msgReciveModel.countReset();
}

QString TotalManager::getcanId()
{
    return _msgTransmitModel.getcanId(_selectedTransmitRow);
}

QByteArray TotalManager::getmessageData()
{
//    if(!_msgTransmitModel.isEmpty())
//        qDebug() << "get Message Data : " << _msgTransmitModel.getmessageData(_selectedTransmitRow);
        return _msgTransmitModel.getmessageData(_selectedTransmitRow);
//    else
//        return "00 00 00 00 00 00 00 00";
}

//void TotalManager::startThreadWork()
//{
//    _serialWorker->requestedWork();
//    _threadStatus = true;
//}

void TotalManager::increaseCount(const int& value)
{
    _msgTransmitModel.increaseCount(value);

}

void TotalManager::processingReadData(const QString& value)
{
    _msgReciveModel.processingReadData(value);
}

