#include "puzzlestrategy.h"

PuzzleStrategy::PuzzleStrategy(QObject *parent)
    :QObject(parent)
{
    statesList.append(Right);
    statesList.append(Down);
    statesList.append(Left);
    statesList.append(Up);

    timer = new QTimer(this);
    timer->setInterval(1200);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateState()));
}

void PuzzleStrategy::IDAStar(QVector<char>* nodes, Heuristic heuristic)
{
    initState = new State(this, nullptr, new QVector<char>(*nodes), heuristic);
    int nextCostBound  = initState->getCost();
    int minNextThreshold;
    State* solution = nullptr;
    steps = 0;
    const int MAX_DEPTH = 80;

    while (true) {
        minNextThreshold = INT_MAX;
        if (nextCostBound > MAX_DEPTH) break;
        solution = search(initState, nextCostBound, minNextThreshold);
        if (solution != nullptr && solution->isFinalState()) break;
        initState->setParent(nullptr);
        delete initState;
        initState = new State(this, nullptr, new QVector<char>(*nodes), heuristic);
        nextCostBound = minNextThreshold;
    }
    puzzleSolved(solution, steps);
    onFinalState(solution);
}

State* PuzzleStrategy::search(State* current, int bound, int& minCost) {
    steps++;

    int cost = current->getCost();
    if (cost > bound || current->isFinalState()){
        minCost = cost;
        return current;
    }

    State *next;

    foreach (Direction direction, statesList)
    {
        next = current->getNextState(direction);

        if (next != nullptr)
        {
            int minThreshold = minCost;
            State* possibleSolution = search(next, bound, minThreshold);
            if (possibleSolution->isFinalState()) {
                minCost = minThreshold;
                return possibleSolution;
            }
            if (minThreshold < minCost) minCost = minThreshold;
            delete next;
        }
    }

    return current;
}

void PuzzleStrategy::onFinalState(State *state)
{
    if (state != nullptr)
    {
        // We have a solution for this puzzle
        // Backtrac to the root of the path in the tree
        path.clear();
        while (state != nullptr)
        {
            path.push(state);
            state = state->getParent();
        }
        timer->start();
    }
    else
    {
        // No solution
        Param *param = new Param(this, nullptr, true);
        emit onStateChanged(param);
    }
}

void PuzzleStrategy::updateState() {
    if (path.count() > 0)
    {
        // Move one by one down the path
        const QVector<char>* nodes = path.pop()->getState();
        Param *param = new Param(this, nodes, path.count() == 0);
        emit onStateChanged(param);
    } else {
        timer->stop();
    }
}

void PuzzleStrategy::puzzleSolved(State *state, int states)
{
    steps = -1;
    while (state != nullptr)
    {
        state = state->getParent();
        steps++;
    }
    Param *param = new Param(this, steps, states);
    emit onPuzzleSolved(param);
}
