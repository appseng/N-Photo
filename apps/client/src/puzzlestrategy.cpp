#include "puzzlestrategy.h"

PuzzleStrategy::PuzzleStrategy(QObject *parent)
    :QObject(parent)
{
    statesList.append(Right);
    statesList.append(Down);
    statesList.append(Left);
    statesList.append(Up);
    path = new QStack<State*>();
}

void PuzzleStrategy::IDAStar(QVector<char>* nodes, Heuristic heuristic)
{
    if (nodes == nullptr)
        return;

    initState = new State(nullptr, new QVector<char>(*nodes), heuristic, this);
    threshold  = initState->getCost();
    State* solution = nullptr;
    steps = 0;

    while (true) {
        minThreshold = INT_MAX;
        solution = dfs(initState);
        if (solution != nullptr && solution->isFinalState()) break;
        delete initState;
        initState = new State(nullptr, new QVector<char>(*nodes), heuristic, this);
        threshold = minThreshold;
    }
    onPuzzleSolved(solution, steps);
    onFinalState(solution);
}

void PuzzleStrategy::onPuzzleSolved(State *state, int states)
{
    steps = -1;
    while (state != nullptr) {
        state = state->getParent();
        steps++;
    }
    StepParam *param = new StepParam(this, steps, states);
    emit onPuzzleSolved(param);
}

void PuzzleStrategy::onFinalState(State *state)
{
    if (state != nullptr) {
        path->clear();
        while (state != nullptr) {
            path->push(state);
            state = state->getParent();
        }
        emit onTimerStart(path);
    } else {
        StateParam *param = new StateParam(this, nullptr, true);
        emit onStateChanged(param);
    }
}

State* PuzzleStrategy::dfs(State* current)
{
    steps++;

    int cost = current->getCost();
    if (cost <= threshold && current->isFinalState()) {
        return current;
    }

    if (cost > threshold) {
        if (cost < minThreshold) minThreshold = cost;
        return current;
    }

    State *next;

    foreach (Direction direction, statesList) {
        next = current->getNextState(direction);

        if (next != nullptr) {
            State* possibleSolution = dfs(next);
            if (possibleSolution->isFinalState()) {
                return possibleSolution;
            }
            delete next;
        }
    }

    return current;
}
