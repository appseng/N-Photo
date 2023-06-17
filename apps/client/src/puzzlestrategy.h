#ifndef PUZZLESTRATEGY_H
#define PUZZLESTRATEGY_H

#include <QObject>
#include <QVector>
#include <QHash>
#include <QSet>
#include <QStack>
#include <QTimer>

#include "state.h"
#include "params.h"

class PuzzleStrategy : public QObject
{
    Q_OBJECT
    public:
        PuzzleStrategy(QObject* = nullptr);
        void IDAStar(QVector<char>*, Heuristic);
    private:
        State* dfs(State*);
        void onFinalState(State*);
        void onPuzzleSolved(State*, int);
    signals:
        void onStateChanged(StateParam*);
        void onPuzzleSolved(StepParam*);
        void onTimerStart(QStack<State*>*);
    private:
        int steps;
        bool isFinalState;
        State *initState;
        QStack<State*> *path;
        QVector<Direction> statesList;
        int threshold;
        int minThreshold;
};

#endif // PUZZLESTRATEGY_H
