#ifndef RECENTFILEMODEL_H
#define RECENTFILEMODEL_H
#include <QAbstractListModel>


class RecentFileModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit RecentFileModel();

    virtual int rowCount(const QModelIndex& = QModelIndex()) const override;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    void addFileUrl(const QString& fileurl);


private:
    QList<QString> _recentFileList;
};

#endif // RECENTFILEMODEL_H
