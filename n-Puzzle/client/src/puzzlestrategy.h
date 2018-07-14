#ifndef PUZZLESTRATEGY_H
#define PUZZLESTRATEGY_H

#include <QObject>
#include <QVector>

#include <utility>

#include "state.h"

class PuzzleStrategy : public QObject
{
    Q_OBJECT
    public:
        PuzzleStrategy(QObject* parent = 0);
        void start(QVector<int> *nodes, Heuristic heuristic);
    private:
        void onFinalState(State *state);
        void puzzleSolved(State *state, int states);
    signals:
        onStateChanged(std::pair<QVector<int> *, bool>);
        onPuzzleSolved(std::pair<int, int>);
};
#endif // PUZZLESTRATEGY_H
