#ifndef CANMSGMODEL_H
#define CANMSGMODEL_H
#include <QAbstractTableModel>
#include <QList>
#include <QUrl>

/*************************************
        define
**************************************/
#define CR 0x0D
#define RX_EXT_DATA 0x65
#define RX_STD_DATA 0x74


class CAN_ITEM
{
public:
    CAN_ITEM();

    QByteArray _identifier;
    quint16 _cycletime;
    bool _cyclecheck = false;
    quint8 _datalength;
    QString _frametype;
    QByteArray _messagedata;
    QString _comment;
    quint16 _count;
    quint16 _uniquenumber;
    qint64 _lastSentTime;
    int _IDFormat;
    QString _DataFormat;

    enum FormatStatus {
        hexa = 0,
        dec,
        ASCII
    };
};


class CanMsgModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum ColumnNames {
        identifier = Qt::UserRole,
        cycletime,
        datalength,
        frametype,
        messagedata,
        comment,
        count
    };

    enum ViewIndex {
        CAN_ID = 1,
        Type,
        QVarLength,
        Data,
        CycleTime,
        Count,
        Comment
    };

    explicit CanMsgModel();

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;
    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) override;

    CAN_ITEM onCheckedChanged(int index, int state);
    void append(CAN_ITEM item);

    void insert(CAN_ITEM item, int index = 0);
    void swapListItem(CAN_ITEM item, int index =0);
    void clear();
    void removeAt(int index);
    void openFile(QString fileurl);
    void saveFile(QString fileurl);
    qint16 getUniqueNumber(int idx);
    void cut(int index);
    void copy(int index);
    void paste(int index);
    void countReset();

    void changeIDFormatHexadecimal(int index);
    void changeIDFormatDecimal(int index);


    QString getcanId(int idx);
    int getdataLength(int idx);
    int getcycletime(int idx);
    QByteArray getmessageData(int idx);
    QString getcomment(int idx);
    int getFormat(int idx);

    bool processingReadData(const QString& value);

    //임시
    CAN_ITEM getIndexItem(int idx) { return _list[idx];}
    void cycleCheckedList(int idx, QList<CAN_ITEM>& list);
    void increaseCount(const int& uniquenumer);

    CAN_ITEM parserStringToItem(const QString& msg);
    QString makePeckCanSaveFormat(const CAN_ITEM& item);
    QString makeTransmituCanFormat(const int& index);
    QString makeTransmituCanFormat(CAN_ITEM item); //Overloading
    CAN_ITEM parserReciveuCanSignal(const QString& signal);

private:
    QList<CAN_ITEM> _list;
    int _uniqueNumber = 0;//

    CAN_ITEM _buffer;

    CAN_ITEM _defaultItem;
    //processReadData

    QString _lastIn;


};
#endif // CANMSGMODEL_H
