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

    connect(strategy, SIGNAL(onTimerStart(QStack<State*>*)), UIThread, SLOT(startTimer(QStack<State*>*)));
    connect(strategy, SIGNAL(onPuzzleSolved(StepParam*)), UIThread, SLOT(onPuzzleSolved(StepParam*)));
    connect(strategy, SIGNAL(onStateChanged(StateParam*)), UIThread, SLOT(displayState(StateParam*)));

    strategy->IDAStar(nodes, heuristic);
}
