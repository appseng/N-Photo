#include "solvethread.h"

SolveThread::SolveThread(QObject* main, QVector<char> *nodes, Heuristic heuristic)
{
    this->UIThread = main;
    this->nodes = nodes;
    this->heuristic = heuristic;
}

void SolveThread::run()
{
    strategy = new PuzzleStrategy(this);

    connect(strategy, SIGNAL(onPuzzleSolved(Param*)), UIThread, SLOT(onPuzzleSolved(Param*)));
    connect(strategy, SIGNAL(onTimerStart(QStack<State*>*)), UIThread, SLOT(startTimer(QStack<State*>*)));
    connect(strategy, SIGNAL(onStateChanged(Param*)), UIThread, SLOT(displayState(Param*)));

    strategy->IDAStar(nodes, heuristic);
}
