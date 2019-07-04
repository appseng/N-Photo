#ifndef PUZZLESTRATEGY_H
#define PUZZLESTRATEGY_H

#include <QObject>
#include <QVector>
#include <QHash>
#include <QSet>
#include <QStack>
#include <QTimer>

#include <utility>

#include "state.h"

class PuzzleStrategy : public QObject
{
    Q_OBJECT
    private:
        int steps;
        bool isFinalState;
        State *initState;
        QStack<State*> path;
        QTimer *timer;
        QVector<Direction> statesList;
    public:
        PuzzleStrategy(QObject* = nullptr);
        void IDAStar(QVector<char>*, Heuristic);
    private:
        void onFinalState(State*);
        void puzzleSolved(State*, int);
        State* search(State*, int, int&);
    signals:
        void onStateChanged(Param*);
        void onPuzzleSolved(Param*);
    private slots:
        void updateState();
};

#endif // PUZZLESTRATEGY_H
