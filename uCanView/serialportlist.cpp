#include "serialportlist.h"
#include <QDebug>
#include <QTimer>
#include <QEventLoop>
#include <QElapsedTimer>
#include <QDateTime>

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
    qDebug() << "Serial worker Created in Thread ";
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
    setSerialPort(_portname);

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
        if(_serial->bytesAvailable()) {
            QByteArray newData = _serial->readAll();
//            qDebug() << newData;
            _serialReadData.append( newData );
            if(_serialReadData.contains(CR)) {
                emit serialReadData(_serialReadData.mid(0, _serialReadData.indexOf(CR) + 1));
                _serialReadData = _serialReadData.mid(_serialReadData.indexOf(CR) + 1);
            }
        }

        QEventLoop loop;
        QTimer::singleShot(1, &loop, SLOT(quit()));
        loop.exec();

        if(_portstatus == false)
            break;
//        qDebug() << "The elapsed time : " << timer.elapsed() << "millisecondes";
//        qDebug() << "one Process : " << _proccount;
    }

    if( _serial->isOpen())
        _serial->close();
//    qDebug() <<"2. Serial is open? : " << _serial->isOpen();

    _mutex.lock();
    _working = false;
    _checkedList.clear();
    _mutex.unlock();

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
    _mutex.lock();
    _portname = value;
    _mutex.unlock();
}

void SerialWorker::requestedWork()
{
    _mutex.lock();
    _working = true;
    _abort = false;
    _mutex.unlock();

    emit workRequested();
}

void SerialWorker::setSerialPort(QString portName)
{
    _serial = new QSerialPort;
    _serial->setPortName(portName);
    _serial->setBaudRate(QSerialPort::Baud115200);
    _serial->setDataBits(QSerialPort::Data8);
    _serial->setParity(QSerialPort::NoParity);
    _serial->setStopBits(QSerialPort::OneStop);
    _serial->open(QIODevice::ReadWrite);
//    qDebug() << _serial->readBufferSize();

    _portstatus = true;

//    qDebug() << "1. Serial isOpen:" <<_serial->isOpen();
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
