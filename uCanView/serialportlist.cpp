#include "serialportlist.h"
#include <QDebug>
#include <QTimer>
#include <QEventLoop>
#include <QElapsedTimer>
#include <QDateTime>
#include <QCoreApplication>

SerialPortList::SerialPortList(QObject *parent)
    : QAbstractListModel(parent)
{
    qDebug() << "SerialPortList - Created";
    getPortList();
}

int SerialPortList::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return _serialPortList.count();
}

QVariant SerialPortList::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();

    if(index.row() >= _serialPortList.count())
    {
        return QVariant();
    }

    if(role == Qt::DisplayRole || role == Qt::EditRole)
    {
        return _serialPortList.at(index.row());
    }
    return QVariant();
}

void SerialPortList::getPortList()
{
    _serialPortList.clear();
    for(const QSerialPortInfo &info : QSerialPortInfo::availablePorts())
    {
        _serialPortList.push_back(info.portName());
    }
    emit layoutChanged();
}


SerialWorker::SerialWorker(QThread *thread) : _thread(thread)
{
    qDebug() << "Serial worker Created in Thread " << thread->currentThreadId();
    _working = false;
    _abort = false;
}

SerialWorker::~SerialWorker()
{
    qDebug() << "SerialWorker Distroyed";
    if(_serial) {
        if(_serial->isOpen())
            _serial->close();
        delete _serial;
        _serial =nullptr;
    }
    emit finished();
}

bool SerialWorker::getPortStatus()
{
    return _serial->isOpen();
}


void SerialWorker::doWork()
{
    qDebug() << "Starting worker process in Thread " << thread()->currentThreadId();

    if(!setSerialPort()){
        qDebug() << "SerialPort Open!!";
        emit finished();
    }

    while(true){

        _mutex.lock();
        bool abort = _abort;
        _mutex.unlock();

        if(abort)
            break;

        //WRITE DATA
        qint64 curTime = QDateTime::currentMSecsSinceEpoch();
        for(int i = 0; i < _checkedList.size(); ++i) {
            if (curTime - _checkedList[i]._lastSentTime >= _checkedList[i]._cycletime
            || _checkedList[i]._lastSentTime == 0) {
                _serial->write(_signalConverter.makeTransmituCanFormat(_checkedList[i]).toUtf8());
                _checkedList[i]._lastSentTime = curTime;

//                qDebug() << "Checkd list Unique Number in do WOrk : " << _checkedList[i]._uniquenumber;
                emit countChanged(_checkedList[i]._uniquenumber);
            }
        }

        if(_singleShotEvent == true) {
            _serial->write(_signalConverter.makeTransmituCanFormat(_singleShotItem).toUtf8());
            emit countChanged(_singleShotItem._uniquenumber);
            _mutex.lock();
            _singleShotEvent = false;
            _mutex.unlock();
        }



        //READ DATA
//        qDebug() << "_serial->bytesAvailable()" <<_serial->bytesAvailable();
//        if(_serial->bytesAvailable()) {
//            QByteArray newData = _serial->readAll();
////            qDebug() << newData;
//            _serialReadData.append( newData );

//            if(_serialReadData.contains(CR)) {
//                emit serialReadData(_serialReadData.mid(0, _serialReadData.indexOf(CR) + 1));
//                _serialReadData = _serialReadData.mid(_serialReadData.indexOf(CR) + 1);
//            }


////            if(_serialReadData.contains(CR)) {
////                emit serialReadData(_serialReadData.mid(0, _serialReadData.indexOf(CR) + 1));
////                _serialReadData = _serialReadData.mid(_serialReadData.indexOf(CR) + 1);
////            }
//        }

        QCoreApplication::processEvents();

        if(_portstatus == false)
            break;
//        qDebug() << "The elapsed time : " << timer.elapsed() << "millisecondes";
//        qDebug() << "one Process : " << _proccount;
    }

    if( _serial->isOpen())
        _serial->close();

    _mutex.lock();
    _working = false;
    _checkedList.clear();
    _mutex.unlock();
    qDebug() << "End Serial";
    emit finished();
}

void SerialWorker::setSingleShotItem(const CAN_ITEM& value)
{
    _mutex.lock();
    _singleShotItem = value;
    _singleShotEvent = true;
    _mutex.unlock();
}

void SerialWorker::disconnect()
{
    if(_portstatus == true)
        _portstatus = false;

    if(_serial) {
        if(_serial->isOpen())
            _serial->close();
    }
    qDebug() << "Disconnected!";
}

void SerialWorker::setPort(const QString &value)
{
//    qDebug() << "setPort:" <<  value;
    QStringList splitPart;

    splitPart = value.split(QLatin1Char(' '), Qt::SkipEmptyParts);

    qDebug() <<"Port: " << splitPart.at(0);
    qDebug() <<"baud: " << splitPart.at(1);
    qDebug() <<"data: " << splitPart.at(2);
    qDebug() <<"parity: " << splitPart.at(3);
    qDebug() <<"stop: " << splitPart.at(4);
    qDebug() <<"FlowControl: " << splitPart.at(5);

    _portName = splitPart.at(0);

    _baudRate = splitPart.at(1).toInt();

    _dataBits = QSerialPort::Data8; //Databit는 8로 고정한다.

    if("NoParity" == splitPart.at(3))
        _parityBits = QSerialPort::NoParity;
    else if("EvenParity" == splitPart.at(3))
        _parityBits = QSerialPort::EvenParity;
    else if("OddParity" == splitPart.at(3))
        _parityBits = QSerialPort::OddParity;
    else if("SpaceParity" == splitPart.at(3))
        _parityBits = QSerialPort::SpaceParity;
    else if("MarkParity" == splitPart.at(3))
        _parityBits = QSerialPort::MarkParity;
    else if("UnknownParity" == splitPart.at(3))
        _parityBits = QSerialPort::UnknownParity;

    if( splitPart.at(4) == "1")
        _stopBits = QSerialPort::OneStop;
    else if(splitPart.at(4) == "2")
        _stopBits = QSerialPort::TwoStop;

    if(splitPart.at(5) == "NoFlowControl")
        _flowControl = QSerialPort::NoFlowControl;
    else if(splitPart.at(5) == "HardwareControl")
        _flowControl = QSerialPort::HardwareControl;
}

void SerialWorker::requestedWork()
{
    _mutex.lock();
    _working = true;
    _abort = false;
    _mutex.unlock();

    emit workRequested();
}

bool SerialWorker::setSerialPort()
{
    _serial = new QSerialPort;
    _serial->setPortName(_portName);
    qDebug()<< _baudRate;
    _serial->setBaudRate(_baudRate);//int
    _serial->setDataBits(_dataBits);//int
    _serial->setParity(_parityBits);//int
    _serial->setStopBits(_stopBits);//
    _serial->setFlowControl(_flowControl);
    _serial->setReadBufferSize(100000);
    _serial->open(QIODevice::ReadWrite);
    qDebug() << _serial->currentReadChannel();

     connect(_serial, SIGNAL(readyRead()), this, SLOT(readData()));
    _portstatus = true;

    if(_serial->isOpen())
    {
        return true;
    }

    return false;

}

void SerialWorker::abort()
{
    _mutex.lock();
    if(_working)
        _abort = true;
    _mutex.unlock();
}

void SerialWorker::checkedListChanged(const CAN_ITEM &value)
{
//    qDebug() << "Checkedlist unique number : " << value._uniquenumber;

    if(_checkedList.size() == 0) {
        if(value._cyclecheck == true) {
        _mutex.lock();
        _checkedList.append(value);
        _mutex.unlock();
        }
    } else {
        unique = true;

        for(int i = 0; i < _checkedList.size(); ++i) {
            if(_checkedList[i]._uniquenumber == value._uniquenumber) {
                unique = false;
                if(value._cyclecheck == false){
                    _mutex.lock();
                    _checkedList.removeAt(i);
                    _mutex.unlock();
                }
            }
        }

        if(unique == true) {
            if(value._cyclecheck == true){
                _mutex.lock();
                _checkedList.append(value);
                _mutex.unlock();
            }
        }
    }
}

void SerialWorker::readData()
{
    if(_serial->bytesAvailable()) {
        QByteArray newData = _serial->readAll();
//            qDebug() << newData;
        _serialReadData.append( newData );

        if(_serialReadData.contains(CR)) {
            emit serialReadData(_serialReadData.mid(0, _serialReadData.indexOf(CR) + 1));
            _serialReadData = _serialReadData.mid(_serialReadData.indexOf(CR) + 1);
        }
    }
}
