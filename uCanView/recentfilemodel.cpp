#include "recentfilemodel.h"
#include <QDebug>
#include <QUrl>

RecentFileModel::RecentFileModel()
{

}

int RecentFileModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return _recentFileList.count();
}

QVariant RecentFileModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid()) {
        return QVariant();
    }

    if(index.row() >= _recentFileList.count()) {
        return QVariant();
    }

    if(role == Qt::DisplayRole || role == Qt::EditRole) {
        return _recentFileList.at(index.row());
    }
    return QVariant();
}

void RecentFileModel::addFileUrl(const QString &fileurl)
{
    QUrl url(fileurl);
//    qDebug() << "Recent FIle Model FileUrl :" << url.toLocalFile();
    QList<QString>::iterator iter; // who's fast?
    bool checker = false;

    if(!fileurl.isEmpty()){
        //need overlap word cut
        for(iter = _recentFileList.begin(); iter != _recentFileList.end(); ++iter) {
            if(*iter == fileurl) {
                checker = true;
            }
        }
//        _recentlist.push_back(fileurl);

        if(checker == false){
            _recentFileList << fileurl;
        }
    }

    for(int i = 0; i < _recentFileList.size(); ++i) {
//        qDebug() << "RecentList : " << _recentFileList[i];
    }

    emit layoutChanged();
}
