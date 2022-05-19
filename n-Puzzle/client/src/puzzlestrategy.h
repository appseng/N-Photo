#ifndef PUZZLESTRATEGY_H
#define PUZZLESTRATEGY_H

#include <QObject>
#include <QVector>
#include <QHash>
#include <QSet>
#include <QStack>
#include <QTimer>

#include "state.h"
#include "param.h"

class PuzzleStrategy : public QObject
{
    Q_OBJECT
    private:
        int steps;
        bool isFinalState;
        State *initState;
        QStack<State*> *path;
        QVector<Direction> statesList;
        int threshold;
        int minThreshold;
    public:
        PuzzleStrategy(QObject* = nullptr);
        void IDAStar(QVector<char>*, Heuristic);
    private:
        State* dfs(State*);
        void onFinalState(State*);
        void onPuzzleSolved(State*, int);
    signals:
        void onStateChanged(Param*);
        void onPuzzleSolved(Param*);
        void onTimerStart(QStack<State*>*);
};

#endif // PUZZLESTRATEGY_H
