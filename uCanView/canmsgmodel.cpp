#include "canmsgmodel.h"
#include <QDebug>
#include <QFile>
#include <QSaveFile>
#include <stdexcept>
#include <QTimer>
#include <QEventLoop>
#include <QElapsedTimer>
#include <algorithm>
#include <QTextCodec>


CAN_ITEM::CAN_ITEM() : _identifier{}, _cycletime{}, _cyclecheck{false}, _datalength{}, _frametype{},
    _messagedata(), _comment{}, _count{}, _uniquenumber{}, _lastSentTime{}, _IDFormat{}, _DataFormat{}
{

}

CanMsgModel::CanMsgModel()
{
//    _list.append(CAN_ITEM());
    _defaultItem._identifier = "<Empty>";
    _list.append(_defaultItem);
    qDebug() << "Table Model created";
}

int CanMsgModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return _list.count();
}

int CanMsgModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 6;
}

QVariant CanMsgModel::data(const QModelIndex &index, int role) const
{
    Q_UNUSED(role)
    QVariant value;

    switch (role) {
    case identifier : value = _list.at(index.row())._identifier; break;
    case cycletime : value = _list.at(index.row())._cycletime; break;
    case datalength : value = _list.at(index.row())._datalength; break;
    case frametype : value = _list.at(index.row())._frametype; break;
    case messagedata : value = _list.at(index.row())._messagedata; break;
    case comment : value = _list.at(index.row())._comment; break;
    case count : value = _list.at(index.row())._count; break;

    default: break;
    }
    return value;
}

QHash<int, QByteArray> CanMsgModel::roleNames() const
{
    static QHash<int, QByteArray> roles;
    roles[identifier] = "identifier";
    roles[cycletime] = "cycletime";
    roles[datalength] = "datalength";
    roles[frametype] = "frametype";
    roles[messagedata] = "messagedata";
    roles[comment] = "comment";
    roles[count] = "count";
    return roles;
}

void CanMsgModel::sort(int column, Qt::SortOrder order)
{
    if(column == CAN_ID) {
        std::sort(_list.begin(), _list.end(),
                  [] (const CAN_ITEM& a, const CAN_ITEM& b) {
            return a._identifier < b._identifier;
        });
    } else if (column == Data) {
        std::sort(_list.begin(), _list.end(),
                  [] (const CAN_ITEM& a, const CAN_ITEM& b) {
            return a._messagedata < b._messagedata;
        });
    } else if( column == CycleTime) {
        std::sort(_list.begin(), _list.end(),
                  [] (const CAN_ITEM& a, const CAN_ITEM& b) {
            return a._cycletime < b._cycletime;
        });
    } else if(column == Comment) {
        std::sort(_list.begin(), _list.end(),
                  [] (const CAN_ITEM& a, const CAN_ITEM& b) {
            return a._comment < b._comment;
        });
    }
}

CAN_ITEM CanMsgModel::onCheckedChanged(int index, int state)
{
    if(state == 2) {        //true == 2
        _list[index]._cyclecheck = true;
//        qDebug() << "Index :" << index << "is " << _list[index]._cyclecheck;
        return _list[index];
    } else {                //else == false
        _list[index]._cyclecheck = false;
//        qDebug() << "Index :" << index << "is " << _list[index]._cyclecheck;
        return _list[index];
    }
}


void CanMsgModel::append( CAN_ITEM item)
{
    item._uniquenumber = ++_uniqueNumber;
//    qDebug() << "Append : " << _uniqueNumber;
    if (_list[0]._identifier == "<Empty>"){
        _list[0] = item;
    } else{
        _list.append(item);
    }

    emit layoutChanged();
}

void CanMsgModel::insert(CAN_ITEM item, int index )
{
    item._uniquenumber = ++_uniqueNumber;
//    qDebug() << "insert : " << _uniqueNumber;
    _list.insert(index, item);
    emit layoutChanged();
}

void CanMsgModel::swapListItem(CAN_ITEM item, int index)
{
    _list[index] = item;
    emit layoutChanged();
}

void CanMsgModel::clear()
{
    beginResetModel();
    _list.clear();
    _list.append(_defaultItem);
    endResetModel();
//    emit modelReset();
//    emit layoutChanged();
}

void CanMsgModel::removeAt(int index)
{
    qDebug() <<index;
    if(index != -1 && _list.size() > index){
        if(_list[index]._identifier !=  "<Empty>")
            _list.removeAt(index);

        if (_list.isEmpty())
            _list.append(_defaultItem);
    }
    emit layoutChanged();
}

void CanMsgModel::openFile(QString fileurl)
{
//    _list.clear();

    if(fileurl.contains("file:")) {
        QUrl url(fileurl);
       fileurl = url.toLocalFile();
//       qDebug() << "Fileurl : " << fileurl;
   }

    QFile file(fileurl);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }

    while (!file.atEnd()) {
//        QString line = QString::fromUtf8(file.readLine()); //ASCII data => encode 이전
        QString line = QString::fromLocal8Bit(file.readLine()); //ASCII data => encode 이전
//        QTextCodec* codec = QTextCodec::codecForUtfText(line);
        //양쪽 코덱 둘다 받기


//        qDebug() << "Line : " << line;
        //필터링 해야함.... 어떻게?
        if(line.length() <8 || line.startsWith(';')) {

        } else {
            if(_list[0]._identifier == "<Empty>") {
                _list[0] = parserStringToItem(line);
            } else {
                _list.append(parserStringToItem(line));
            }
        }
    }
    emit layoutChanged();
}

void CanMsgModel::saveFile(QString fileurl)
{
    QUrl url(fileurl);

//    qDebug() << "First input Address :" << url.toLocalFile();

    QString filepath = url.toLocalFile() +".xmt";

//    qDebug() << "filePath: "  <<filepath;
    QSaveFile file(filepath);
    // Trying to open in WriteOnly and Text modeS
    if(!file.open(QSaveFile::WriteOnly | QSaveFile::Text))
    {
        qDebug() << " Could not open file for writing";
        return;
    }

    QTextStream out(&file);
    out.setCodec("UTF-8"); //Linux

    QList<QString> tmplist;
    QString filedata = ";" + fileurl + ".xmt";
    tmplist.append(filedata);
    tmplist.append("; Columns descriptions:");
    tmplist.append("; ~~~~~~~~~~~~~~~~~~~~~");
    tmplist.append("; +Message ID");
    tmplist.append("; |       +Cycle time in ms (0=manual)");
    tmplist.append("; |       |  +Data length");
    tmplist.append("; |       |  | +Frame type");
    tmplist.append("; |       |  | | +Message data");
    tmplist.append("; |       |  | | |");

    QList<CAN_ITEM>::iterator iter;

    for(iter = _list.begin(); iter != _list.end(); ++iter) {
        tmplist.append(makePeckCanSaveFormat(*iter));
    }

    for(int i = 0; i != tmplist.size(); ++i){
        out << tmplist[i] << "\n";
    }
    tmplist.clear();
    file.commit();
    qDebug() << "File save";
}

qint16 CanMsgModel::getUniqueNumber(int idx)
{
    return _list[idx]._uniquenumber;
}

void CanMsgModel::cut(int index)
{

    if(_list[0]._identifier == "<Empty>") {

    } else if(_list.size() > 0) {
        _buffer = _list[index];
        if(index >0)
            _list.removeAt(index);
        else if (_list.isEmpty())
            _list.append(_defaultItem);
    }
    emit layoutChanged();

}

void CanMsgModel::copy(int index)
{
    if(index != -1 && _list.size() > index){
        if(_list[index]._identifier != "<Empty>") {
            _buffer = _list[index];
        }
    }
}

void CanMsgModel::paste(int index)
{
    if(index != -1 && _list.size() > index){
        if(_buffer._identifier != "<Empty>"){
            qDebug() <<_buffer._identifier;
            if(_list[0]._identifier == "<Empty>") {
                _list[0] = _buffer;
            } else {
                _list.insert(index, _buffer);
            }
            emit layoutChanged();
        }
    }
}

void CanMsgModel::countReset()
{   //속도 측면에서 idx 탐색보다 iterator 탐색이 더 빠르다고 한다.
    for(auto iter = _list.begin(); iter < _list.end(); ++iter) {
        iter->_count = 0;
    }
    emit layoutChanged();
}

void CanMsgModel::changeIDFormatHexadecimal(int idx)
{
    if(_list.size() > idx && idx != -1 ){
        _list[idx]._IDFormat = CAN_ITEM::hexa;
        emit dataChanged(index(idx,CAN_ID),index(idx,CAN_ID));
    }
}

void CanMsgModel::changeIDFormatDecimal(int idx)
{
    bool ok;
     if(_list.size() > idx && idx != -1 ) {
         int convdec = _list[idx]._identifier.toInt(&ok, 16);
         QByteArray tmp = _list[idx]._identifier;
//         qDebug() <<convdec;
         _list[idx]._identifier = QByteArray::number(convdec);
         _list[idx]._IDFormat = CAN_ITEM::dec;
         emit dataChanged(index(idx,CAN_ID),index(idx,CAN_ID));
         _list[idx]._identifier = tmp;
     }
}

void CanMsgModel::changeDATAFormatHexadecimal(int idx)
{
    if(_list.size() > idx && idx != -1 ){
        _list[idx]._DataFormat = CAN_ITEM::hexa;
        emit dataChanged(index(idx,Data),index(idx,Data));
    }
}

void CanMsgModel::changeDATAFormatDecimal(int idx) // 000 000 000 000 000 000 000 000 로 각각 변환하여 표시 해야함
{

    QByteArray rowHexData = _list[idx]._messagedata;
    QList<QByteArray> listItem = _list[idx]._messagedata.split(' ');
    int decimal;
    QByteArray qtemp;
    bool ok;
    for(int i = 0; i < listItem.size(); ++i) {
        decimal = listItem[i].toUInt(&ok, 16);
        QString temp = QString::number(decimal);
        temp = temp.rightJustified(3, '0');
        qtemp.append(temp.toLocal8Bit());

        if(i != listItem.size() - 1)
            qtemp.append(' ');
    }

//     qDebug() << "Decimal Data : " << qtemp;
    _list[idx]._messagedata = qtemp;
    _list[idx]._DataFormat = CAN_ITEM::dec;
    emit dataChanged(index(idx,Data),index(idx,Data));
    _list[idx]._messagedata = rowHexData;
}

void CanMsgModel::changeDATAFormatASCII(int idx)
{
    QByteArray rowHexData = _list[idx]._messagedata;
    QList<QByteArray> listItem = _list[idx]._messagedata.split(' ');
    QString ASCIIData;
//    QByteArray qtemp;
    bool ok;
    for(int i = 0; i < listItem.size(); ++i) {
        QChar ch = listItem.at(i).toInt(&ok, 16);
        ASCIIData.append(ch);
    }
    qDebug() << "ASCIIData : " << ASCIIData;
    QByteArray ASCIIByteArray;
    ASCIIByteArray.append(ASCIIData);
    _list[idx]._messagedata = ASCIIByteArray;
    _list[idx]._DataFormat = CAN_ITEM::ASCII;
    emit dataChanged(index(idx,Data),index(idx,Data));
    _list[idx]._messagedata = rowHexData;
}



QString CanMsgModel::getcanId(int idx)
{
    return _list[idx]._identifier;
}

int CanMsgModel::getdataLength(int idx)
{
    return _list[idx]._datalength;
}

int CanMsgModel::getcycletime(int idx)
{
    return _list[idx]._cycletime;
}

QByteArray CanMsgModel::getmessageData(int idx)
{
    return _list[idx]._messagedata;
}

QString CanMsgModel::getcomment(int idx)
{
    return _list[idx]._comment;
}

int CanMsgModel::getFormat(int idx)
{
    return _list[idx]._IDFormat;
}

int CanMsgModel::getIdFormatStatus(int idx)
{
    return _list[idx]._IDFormat;
}

int CanMsgModel::getDataFormatStatus(int idx)
{
    return _list[idx]._DataFormat;
}



bool CanMsgModel::processingReadData(const QString &value)
{
//    qDebug() << value; //추후 성능 개선을 위해 링버퍼를 사용 해야한다.
    CAN_ITEM temp;

    bool unique = true;
    QString _firstIn = value.mid(0,value.lastIndexOf(CR) + 1);

    // 1. 정상적인 포멧
    if(value.startsWith(RX_EXT_DATA) && value.endsWith(CR) && value.size() == 27) {
        //추후 성능 개선을 위해 링버퍼를 사용 해야한다.
        temp = parserReciveuCanSignal(_firstIn);
//        qDebug() << temp._count;
        _firstIn.clear();

        for(int i = 0; i < _list.size(); ++i) {
            if(_list[i]._identifier == temp._identifier) {
                ++_list[i]._count;
                emit dataChanged(index(i,Data),index(i,Data));
                unique = false;
            }
        }

        if(std::find(_list.begin(), _list.end(), temp) == _list.end())
        {
            qDebug() <<"std::find(_list.begin(), _list.end(), temp) - _list.begin()";
        }


        //find 알고리즘 사용
        if(unique == true) {
            ++temp._count;
            if (_list[0]._identifier == "<Empty>")
                _list[0] = temp;
             else
                _list.append(temp);

            emit layoutChanged();
        }
        return true;
    } // 정상적인 포맷
    return false;
}

void CanMsgModel::cycleCheckedList(int idx, QList<CAN_ITEM>& list)
{
    if(_list[idx]._cyclecheck == true) {
        list.append(_list[idx]);
    } else if(_list[idx]._cyclecheck == false) {

        for(int i = 0; i < list.size(); ++i) {
            if(_list[idx]._uniquenumber == list[i]._uniquenumber) {
                list.removeAt(i);
            }
        }
    }
}

void CanMsgModel::increaseCount(const int &uniquenumer) // 탐색 알고리즘 개선 필요함 성능이 너무 느림.......
{

    for(int i = 0; i < _list.size(); ++i) {
        if(_list[i]._uniquenumber == uniquenumer) {
            ++_list[i]._count;
            emit dataChanged(index(i,Data), index(i,Data));
        }
    }
}


CAN_ITEM CanMsgModel::parserStringToItem(const QString &msg)
{
//Msg Format
//  ID         CYCCLE
//  ┃            ┃  DLC FRAME TYPE
//  ┃            ┃   ┃   ┃
//  ┃            ┃   ┃   ┃   MESSAGE DATA
//  ┃            ┃   ┃   ┃     ┃                               COMMENT
//  ┃            ┃   ┃   ┃     ┃                                 ┃
// 00000000h    00  8   D   00h 00h 00h 00h 00h 00h 00h 00h ; COMMENT_COMMENT_COMMENT
// 명확하게 정확한 메시지가 넘어왔다고 가정하기?
// QML에서 입력받을 때 필터링?
// QFile에서 받을 때 필터링?
// 여기서 한번더 확인하기?
// DLC 가 8 일 경우 QStringList가 12
// DLC 숫자에 따라서 유동적으로 QStringList 길이를 정하고 필터링?
    CAN_ITEM canitem;
    QStringList parts;
    bool ok = false;
    bool ext_ok = false;

//    qDebug() << "PaserStringItem : " << msg;
    canitem._comment = msg.mid(msg.indexOf(';') + 1, msg.lastIndexOf("\n"));
    QString maindata = msg.mid(0, msg.indexOf(';'));

    parts = maindata.split(QLatin1Char(' '), Qt::SkipEmptyParts);

    parts.at(0).toUtf8().left(8).toUInt(&ok, 16);
    parts.at(0).toUtf8().left(3).toUInt(&ext_ok, 16);
//    qDebug() << parts.at(0).back();

    //버그 리포트 CAN SIGNAL이 STD Format 일경우 ex)000h 프로그램 종료됨

    if(ok || ext_ok){
        if(parts.at(0).back() == 'h') { //h -> Hexadecimal mark 저장할 때도 16진수는 h 붙여서...
            canitem._identifier = parts.at(0).toLocal8Bit().left(8);
        } else {
            canitem._identifier = parts.at(0).toLocal8Bit();
        }
    }
    else {
        throw std::out_of_range("Identifier Wrong Format");
    }

    parts.at(1).toUInt(&ok, 10);
    if(ok == true) {
        canitem._cycletime = parts.at(1).toUInt(&ok,10);
    } else{
        throw std::runtime_error("Cycle time is no Intiger");
    }

    parts.at(2).toUInt(&ok, 10);
    if(ok == true) {
        canitem._datalength = parts.at(2).toUInt(&ok,10);
    } else {
        throw std::out_of_range("DLC is no Intiger");
    }

    canitem._frametype = parts.at(3);

    QByteArray data;
    for(int i = 4; i <parts.size(); ++i){
//        qDebug() << "Parts : " << parts;
        parts.at(i).toUtf8().left(2).toUInt(&ok, 16);
//        qDebug() <<"parts.at(i) :" << parts.at(i) <<ok;

        if(ok == true){ //역할을 못함
            if(parts.at(i).back() == 'h') {
                data.push_back(parts.at(i).toUtf8().left(2));
//                qDebug() << "Data : " << data;

            } else {
                data.append(parts.at(i).toUtf8());
//                qDebug() << "Data : " << data;
            }
        } /*else if(ok == false) {
            throw std::out_of_range("Data$is not hexadecimal"); //catch 해줘야됨 abort() 오류
        }*/ //잘못만듦 paus나 이런거 때문에 무조건 exception 발생함
    }
//여기서 검사를 해줘야함
//    qDebug() << data.size();

    canitem._messagedata = QByteArray::fromHex(data).toHex(' ').toUpper();
    canitem._uniquenumber = ++_uniqueNumber;

//    qDebug() << "Canitem : " << canitem._messagedata;
    return canitem;
}

QString CanMsgModel::makePeckCanSaveFormat(const CAN_ITEM &item) //저장용
{
    QString messageData = item._messagedata;
    QString data = item._identifier +'h' + ' ' + QString::number(item._cycletime) + ' '
             + QString::number(item._datalength) +' '  + item._frametype
             + ' ' + messageData.replace(' ', "h ") + "h " + " ;" + item._comment.toUtf8();

     return data;
}

QString CanMsgModel::makeTransmituCanFormat(const int& index) //보내기용
{
    QString data = 'e' + _list[index]._identifier + QString::number(_list[index]._datalength)
            + _list[index]._messagedata.replace(' ',"") + CR;
    return data;
}

QString CanMsgModel::makeTransmituCanFormat(CAN_ITEM item)
{
    QString data = 'e' + item._identifier + QString::number(item._datalength)
            + item._messagedata.replace(' ',"") + CR;
    qDebug() << "Trasmit :" << data;
    return data;
}

CAN_ITEM CanMsgModel::parserReciveuCanSignal(const QString &signal)
{
    CAN_ITEM temp;
//    bool ok;
    qDebug() << "Recive : " << signal;
    if(signal.at(0) == RX_STD_DATA) {
//        temp._identifier = signal.mid(1, 3).toUtf8();
        temp._identifier = signal.mid(1, 3).toLocal8Bit().toUpper();
        temp._datalength = signal.at(4).digitValue();
        temp._messagedata = signal.mid(5, signal.lastIndexOf(CR)).toLocal8Bit().toUpper();

    } else if(signal.at(0) == RX_EXT_DATA) {
//        temp._identifier = signal.mid(1, 8).toUtf8();
        temp._identifier = signal.mid(1, 8).toLocal8Bit().toUpper();
        temp._datalength = signal.at(9).digitValue();
        temp._messagedata = signal.mid(10, signal.lastIndexOf(CR)).toLocal8Bit().toUpper();
    }
    return temp;
}
