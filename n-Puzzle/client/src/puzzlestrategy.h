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
#include "minpriorityqueue.h"

class PuzzleStrategy : public QObject
{
    Q_OBJECT
    private:
        int steps;
        bool isFinalState;
        State *state;
        State *finalState;
        QStack<State*> path;
        QTimer *timer;
    public:
        PuzzleStrategy(QObject* = 0);
        void start(QVector<int>*, Heuristic);
    private:
        void onFinalState(State*);
        void puzzleSolved(State*, int);
    signals:
        onStateChanged(Param*);
        onPuzzleSolved(Param*);
    private slots:
        void updateState();
};

#endif // PUZZLESTRATEGY_H
