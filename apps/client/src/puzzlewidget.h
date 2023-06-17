#ifndef PUZZLEWIDGET_H
#define PUZZLEWIDGET_H

#include <QList>
#include <QPoint>
#include <QRect>
#include <QPixmap>
#include <QWidget>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMouseEvent>
#include <QVector>
#include <QMimeData>

#include "enums.h"

class PuzzleWidget : public QWidget
{
    Q_OBJECT
public:
    PuzzleWidget(QWidget * = nullptr, const QSize = QSize(400,400));
    void clear();
    void changeRelation(const QPoint);
    void changeType(const GameType);
    void addPieces(const QPixmap& = QPixmap());
    void shuffle();
    int getTargetIndex(const QPoint&) const;
    const QPoint getRelation() const;
    void setPieces(const QVector<char>* nodes);
    void moveMissingRectangle(Direction);
    void setBusy(bool);
signals:
    void puzzleCompleted(bool);
    void blockMoved();
protected:
    void dragEnterEvent(QDragEnterEvent *);
    void dragLeaveEvent(QDragLeaveEvent *);
    void dragMoveEvent(QDragMoveEvent *);
    void dropEvent(QDropEvent *);
    void mousePressEvent(QMouseEvent *);
    void paintEvent(QPaintEvent *);
private:
    int findPiece(const QRect &) const;
    QRect findPieceToMove(const QRect) const;
    const QRect targetSquare(const QPoint &) const;
    void countMatches();

    QList<QPixmap> piecePixmaps;
    QList<QRect> pieceRects;
    QList<QPoint> pieceLocations;
    QRect highlightedRect;
    int inPlace;
    QSize tile;
    QPoint relation;
    GameType gameType;
    int moves;
    QPoint missingLocation;
    bool busy;
};

#endif // PUZZLEWIDGET_H
