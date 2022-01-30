#ifndef PIECESLIST_H
#define PIECESLIST_H

#include <QtGui>
#include <QAbstractListModel>
#include <QList>
#include <QPixmap>
#include <QPoint>
#include <QMimeData>
#include <QStringList>

class PiecesModel : public QAbstractListModel
{
    Q_OBJECT
public:
    PiecesModel(QObject * = 0);

    QVariant data(const QModelIndex &, int = Qt::DisplayRole) const;
    Qt::ItemFlags flags(const QModelIndex &) const;
    bool removeRows(int, int, const QModelIndex &);

    bool dropMimeData(const QMimeData *, Qt::DropAction ,
                      int, int, const QModelIndex &);
    QMimeData *mimeData(const QModelIndexList &) const;
    QStringList mimeTypes() const;
    int rowCount(const QModelIndex &) const;
    Qt::DropActions supportedDropActions() const;

    void addPiece(const QPixmap&, const QPoint&);
    void addPieces(const QPixmap& , QPoint&);

private:
    QList<QPoint> locations;
    QList<QPixmap> pixmaps;
    QPoint pnt;
};

#endif // PIECESLIST_H
