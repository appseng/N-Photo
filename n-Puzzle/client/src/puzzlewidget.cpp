#include "puzzlewidget.h"

PuzzleWidget::PuzzleWidget(QWidget *parent, const QSize size)
    : QWidget(parent)
{
    setAcceptDrops(true);
    pnt.setX(size.width()/4);
    pnt.setY(size.height()/4);
    this->relation = QPoint(4,4);
    setMinimumSize(size);
    setMaximumSize(size);
    gameType = 0;
    moves = 0;
}

void PuzzleWidget::changeRelation(const QPoint relation)
{
    if (!relation.isNull()) {
        this->relation = relation;
        pnt.setX(minimumWidth()/relation.x());
        pnt.setY(minimumHeight()/relation.y());
    }
}

void PuzzleWidget::changeType(const int gameType)
{
    this->gameType = gameType;
}

void PuzzleWidget::clear()
{
    if (!pieceLocations.isEmpty())
        pieceLocations.clear();

    if (!piecePixmaps.isEmpty())
        piecePixmaps.clear();

    if (!pieceRects.isEmpty())
        pieceRects.clear();

    highlightedRect = QRect();
    inPlace= 0;
    update();
}

void PuzzleWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if (gameType)
        return;

    if (event->mimeData()->hasFormat("image/x-puzzle-piece"))
        event->accept();
    else
        event->ignore();
}

void PuzzleWidget::dragLeaveEvent(QDragLeaveEvent *event)
{
    event->accept();
    if (gameType)
        return;

    QRect updateRect = highlightedRect;
    highlightedRect = QRect();
    update(updateRect);
}

void PuzzleWidget::dragMoveEvent(QDragMoveEvent *event)
{
    if (gameType)
        return;

    QRect updateRect = highlightedRect.united(targetSquare(event->pos()));

    if (event->mimeData()->hasFormat("image/x-puzzle-piece")
        && findPiece(targetSquare(event->pos())) == -1) {

        highlightedRect = targetSquare(event->pos());
        event->setDropAction(Qt::MoveAction);
        event->accept();
    } else {
        highlightedRect = QRect();
        event->ignore();
    }
    update(updateRect);
}

void PuzzleWidget::dropEvent(QDropEvent *event)
{
    if (gameType)
        return;

    if (event->mimeData()->hasFormat("image/x-puzzle-piece")
        && findPiece(targetSquare(event->pos())) == -1) {

        QByteArray pieceData = event->mimeData()->data("image/x-puzzle-piece");
        QDataStream stream(&pieceData, QIODevice::ReadOnly);
        QRect square = targetSquare(event->pos());
        QPixmap pixmap;
        QPoint location;
        QRect prevPos;
        stream >> pixmap >> location >> prevPos;

        pieceLocations.append(location);
        piecePixmaps.append(pixmap);
        pieceRects.append(square);

        highlightedRect = QRect();
        update(square);

        event->setDropAction(Qt::MoveAction);
        event->accept();
        emit blockMoved();

        if (location == QPoint(square.x()/pnt.x(), square.y()/pnt.y())) {
            inPlace++;
            if (inPlace == relation.x()*relation.y())
                emit puzzleCompleted(true);
        }
    } else {
        highlightedRect = QRect();
        event->ignore();
    }
}

int PuzzleWidget::findPiece(const QRect &pieceRect) const
{
    for (int i = 0; i < pieceRects.size(); ++i) {
        if (pieceRect == pieceRects[i]) {
            return i;
        }
    }
    return -1;
}

QRect PuzzleWidget::findPieceToMove(const QRect found) const
{
    QRect oldPos = found;
    oldPos.moveTo(oldPos.x()+pnt.x(),oldPos.y());
    if (findPiece(oldPos) == -1 && (oldPos.x() < pnt.x()*relation.x()))
        return oldPos;

    oldPos.moveTo(oldPos.x()-2*pnt.x(),oldPos.y());
    if (findPiece(oldPos) == -1 && (oldPos.x() >= 0))
        return oldPos;

    oldPos.moveTo(oldPos.x()+pnt.x(),oldPos.y()+pnt.y());
    if (findPiece(oldPos) == -1 && (oldPos.y() < pnt.y()*relation.y()))
        return oldPos;

    oldPos.moveTo(oldPos.x(),oldPos.y()-2*pnt.y());
    if (findPiece(oldPos) == -1 && (oldPos.y() >= 0))
        return oldPos;

    return found;
}

void PuzzleWidget::mousePressEvent(QMouseEvent *event)
{
    QRect square = targetSquare(event->pos());
    int found = findPiece(square);

    if (found == -1 || busy)
        return;

    QPoint location = pieceLocations[found];
    QPixmap pixmap = piecePixmaps[found];
    pieceLocations.removeAt(found);
    piecePixmaps.removeAt(found);
    pieceRects.removeAt(found);

    if (location == QPoint(square.x()/pnt.x(), square.y()/pnt.y()))
        inPlace--;

    if (gameType) {
        QRect newpos = findPieceToMove(square);
        if (newpos != square) {
            square = newpos;
            emit blockMoved();
        }
        pieceLocations.insert(found, location);
        piecePixmaps.insert(found, pixmap);
        pieceRects.insert(found, square);
        update(targetSquare(event->pos()));
        update(square);
        if (location == QPoint(square.x()/pnt.x(), square.y()/pnt.y()))
            inPlace++;

        if (inPlace == pieceRects.length())
            emit puzzleCompleted(true);
    }
    else {
        QByteArray itemData;
        QDataStream dataStream(&itemData, QIODevice::WriteOnly);

        dataStream << pixmap << location << square;

        QMimeData *mimeData = new QMimeData;
        mimeData->setData("image/x-puzzle-piece", itemData);

        QDrag *drag = new QDrag(this);
        drag->setMimeData(mimeData);
        drag->setHotSpot(event->pos() - square.topLeft());
        drag->setPixmap(pixmap);

        if (drag->start(Qt::MoveAction) == 0) {
            pieceLocations.insert(found, location);
            piecePixmaps.insert(found, pixmap);
            pieceRects.insert(found, square);
            update(targetSquare(event->pos()));

            if (location == QPoint(square.x()/pnt.x(), square.y()/pnt.y()))
                inPlace++;
        }
        update(square);
    }
}

void PuzzleWidget::addPieces(const QPixmap& pixmap)
{
    QRect mRect;
    pieceLocations.clear();
    piecePixmaps.clear();
    pieceRects.clear();
    for (int y = 0; y < relation.y(); ++y) {
        for (int x = 0; x < relation.x(); ++x) {
                mRect = QRect(x*pixmap.width()/relation.x(),
                              y*pixmap.height()/relation.y(),
                              pixmap.width()/relation.x(),
                              pixmap.height()/relation.y());
                QPixmap pieceImage = pixmap.copy(mRect);
                pieceLocations.append(QPoint(x, y));
                piecePixmaps.append(pieceImage);
                pieceRects.append(mRect);
        }
    }
    update();
}

void PuzzleWidget::shuffle()
{
    moves = 0;
    int maxid = pieceRects.length()-1;
    QRect freeRect = QRect((relation.x()-1)*pnt.x(), (relation.y()-1)*pnt.y(), pnt.x(), pnt.y());
    int rect = pieceRects.indexOf(freeRect);
    int missing = rect;
    QRect nRect;
    int iRect;
    int i = qrand()%maxid + 150;
    while (i) {
        switch(int(qrand()%4)) {
        case Up:
            nRect = QRect(freeRect.x(),freeRect.y()-pnt.y(),pnt.x(),pnt.y());
            break;
        case Left:
            nRect = QRect(freeRect.x()-pnt.x(),freeRect.y(),pnt.x(),pnt.y());
            break;
        case Down:
            nRect = QRect(freeRect.x(),freeRect.y()+pnt.y(),pnt.x(),pnt.y());
            break;
        default: //right
            nRect = QRect(freeRect.x()+pnt.x(),freeRect.y(),pnt.x(),pnt.y());
        }
        iRect = pieceRects.indexOf(nRect);
        if (iRect != -1) {
            pieceRects.swap(iRect, rect);
            freeRect = nRect;
            i--;
        }
    }

    missingLocation.setX(freeRect.x()/pnt.x());
    missingLocation.setY(freeRect.y()/pnt.y());

    pieceLocations.removeAt(missing);
    piecePixmaps.removeAt(missing);
    pieceRects.removeAt(missing);

    countMatches();

    update();
}
void PuzzleWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter;
    painter.begin(this);

    painter.fillRect(event->rect(), Qt::white);

    if (gameType == 0) {
        if (highlightedRect.isValid()) {
            painter.setBrush(QColor("#ffcccc"));
            painter.setPen(Qt::NoPen);
            painter.drawRect(highlightedRect.adjusted(0, 0, -1, -1));
        }
    }

    for (int i = 0; i < pieceRects.size(); ++i) {
        painter.drawPixmap(pieceRects[i], piecePixmaps[i]);
    }

    painter.end();
}

const QRect PuzzleWidget::targetSquare(const QPoint &position) const
{
    return QRect((position.x()/pnt.x()) * pnt.x(), (position.y()/pnt.y()) * pnt.y(), pnt.x(), pnt.y());
}
int  PuzzleWidget::getTargetIndex(const QPoint &point) const
{
    QPoint pos(point.x()*pnt.x()+1,point.y()*pnt.y()+1);
    return pieceRects.indexOf(targetSquare(pos));
}
const QPoint PuzzleWidget::getRelation() const
{
    return relation;
}
void PuzzleWidget::countMatches()
{
    inPlace = 0;
    for (int k = 0; k < pieceRects.size(); ++k) {
        if (pieceLocations[k] == QPoint(pieceRects[k].x()/pnt.x(), pieceRects[k].y()/pnt.y()))
            inPlace++;
    }
}
void PuzzleWidget::setPieces(const QVector<char>* nodes)
{
    if (nodes == nullptr) return;

    char num;
    int range = nodes->length();
    for (int i = 0; i < range; i++) {// i: position in solved state
        num = nodes->at(i) - 1; // number on tile minus one
        if (num >= 0) {
            // i : position in solved state
            QPoint location(num%relation.x(), num/relation.x());
            // index of position element
            int index = pieceLocations.indexOf(location);

            // point on PuzzleWidget
            // num : number on title minus one
            QPoint pos((i%relation.x())*pnt.x(), (i/relation.x())*pnt.y());

            // rect accorging 'point'
            QRect ts = targetSquare(pos);
            if (index != -1 && index != pieceRects.indexOf(ts))
                pieceRects.replace(index, ts);
        }
    }
    update();
    countMatches();

    if (inPlace == relation.x()*relation.y()-1)
        emit puzzleCompleted(false);
}

void PuzzleWidget::moveMissingRectangle(Direction direction)
{
    if (busy)
        return;

    QPoint point(missingLocation.x() * pnt.x() + 1 ,  missingLocation.y() * pnt.y() + 1);
    QPoint oldPoint(point);
    switch (direction) {
        case Left:
            if (missingLocation.x() > 0) {
                point.setX(point.x() - pnt.x());
                missingLocation.setX(missingLocation.x() - 1);
            }
            break;
        case Up:
            if (missingLocation.y() > 0) {
                point.setY(point.y() - pnt.y());
                missingLocation.setY(missingLocation.y() - 1);
            }
            break;
        case Down:
            if (missingLocation.y() < relation.y() - 1) {
                point.setY(point.y() + pnt.y());
                missingLocation.setY(missingLocation.y() + 1);
            }
            break;
        default: // right
            if (missingLocation.x() < relation.x() - 1) {
                point.setX(point.x() + pnt.x());
                missingLocation.setX(missingLocation.x() + 1);
            }
            break;
    }
    if (oldPoint != point) {
        QMouseEvent event(QEvent::GraphicsSceneMousePress, point, Qt::NoButton, Qt::NoButton, Qt::NoModifier);
        mousePressEvent(&event);
    }
}

void PuzzleWidget::setBusy(bool busy)
{
    this->busy = busy;
}
