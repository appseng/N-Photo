#include "solvethread.h"

SolveThread::SolveThread(PuzzleStrategy* strategy, QVector<char> *nodes, Heuristic heuristic)
{
    this->strategy = strategy;
    this->nodes = nodes;
    this->heuristic = heuristic;
}

void SolveThread::run()
{
        strategy->IDAStar(nodes, heuristic);
}
