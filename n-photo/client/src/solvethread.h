#ifndef SOLVETHREAD_H
#define SOLVETHREAD_H

#include <QThread>

#include "puzzlestrategy.h"
#include "enums.h"

class SolveThread : public QThread
{
    Q_OBJECT
public:
    SolveThread(QObject*, QVector<char>*, Heuristic);
private:
    void run();
    QObject *UIThread;
    PuzzleStrategy *strategy;
    QVector<char> *nodes;
    Heuristic heuristic;
};

#endif // SOLVETHREAD_H
