/****************************************************************************
 **
 ** Copyright (C) 2009 Digia Plc and/or its subsidiary(-ies).
 ** Contact: http://www.qt-project.org/legal
 **
 ** Copyright (C) 2015 Dmitry Kuznetsov
 **
 ** $QT_BEGIN_LICENSE:BSD$
 ** You may use this file under the terms of the BSD license as follows:
 **
 ** "Redistribution and use in source and binary forms, with or without
 ** modification, are permitted provided that the following conditions are
 ** met:
 **   * Redistributions of source code must retain the above copyright
 **     notice, this list of conditions and the following disclaimer.
 **   * Redistributions in binary form must reproduce the above copyright
 **     notice, this list of conditions and the following disclaimer in
 **     the documentation and/or other materials provided with the
 **     distribution.
 **   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
 **     of its contributors may be used to endorse or promote products derived
 **     from this software without specific prior written permission.
 **
 **
 ** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 ** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 ** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 ** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 ** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 ** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 ** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 ** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 ** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 ** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 ** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
 **
 ** $QT_END_LICENSE$
 **
 ****************************************************************************/

#include <QPainter>
#include <QVector>
#include <QMimeData>
#include <QDrag>
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
                emit puzzleCompleted();
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

    if (found == -1)
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
        if (inPlace == (relation.x()*relation.y() - 1))
            emit puzzleCompleted();
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
void PuzzleWidget::scramble()
{
    qsrand(QCursor::pos().x() ^ QCursor::pos().y());
    moves = 0;
    int maxid = relation.x()*relation.y() - 1;
    QRect buf;
    int freeRect = qrand()%maxid;
//    maxid--;
    int i = 100 + qrand()%21;
    while (i)
    {
        switch(int(qrand()%4))
        {
            case 0:
                if (freeRect-relation.x() >= 0) {
                    buf = pieceRects[freeRect];
                    pieceRects[freeRect] = pieceRects[freeRect-relation.x()];
                    pieceRects[freeRect-relation.x()] = buf;
                    freeRect = freeRect-relation.x();
                    i--;
                    break;
                }
            case 1:
                if ((freeRect%relation.x()) != (relation.x()-1)) {
                    buf = pieceRects[freeRect];
                    pieceRects[freeRect] = pieceRects[freeRect+1];
                    pieceRects[freeRect+1] = buf;
                    freeRect = freeRect+1;
                    i--;
                    break;
                }
            case 2:
                if (freeRect+relation.x() <= maxid) {
                    buf = pieceRects[freeRect];
                    pieceRects[freeRect] = pieceRects[freeRect+relation.x()];
                    pieceRects[freeRect+relation.x()] = buf;
                    freeRect = freeRect+relation.x();
                    i--;
                    break;
                }
            default:
                if ((freeRect%relation.x()) != 0) {
                    buf = pieceRects[freeRect];
                    pieceRects[freeRect] = pieceRects[freeRect-1];
                    pieceRects[freeRect-1] = buf;
                    freeRect = freeRect-1;
                    i--;
                }
        }
    }

    pieceLocations.removeAt(freeRect);
    piecePixmaps.removeAt(freeRect);
    pieceRects.removeAt(freeRect);
    inPlace = 0;
    for (int k = 0; k < pieceRects.size(); ++k) {
        if (pieceLocations[k] == QPoint(pieceRects[k].x()/pnt.x(), pieceRects[k].y()/pnt.y()))
            inPlace++;
    }
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
