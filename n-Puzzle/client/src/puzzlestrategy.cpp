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
    threshold  = initState->getCost();
    State* solution = nullptr;
    steps = 0;

    while (true) {
        minThreshold = INT_MAX;
        solution = dfs(initState);
        if (solution != nullptr && solution->isFinalState()) break;
        delete initState;
        initState = new State(this, nullptr, new QVector<char>(*nodes), heuristic);
        threshold = minThreshold;
    }
    puzzleSolved(solution, steps);
    onFinalState(solution);
}

State* PuzzleStrategy::dfs(State* current) {
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

void PuzzleStrategy::onFinalState(State *state)
{
    if (state != nullptr) {
        path.clear();
        while (state != nullptr) {
            path.push(state);
            state = state->getParent();
        }
        timer->start();
    } else {
        Param *param = new Param(this, nullptr, true);
        emit onStateChanged(param);
    }
}

void PuzzleStrategy::updateState() {
    if (path.count() > 0) {
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
    while (state != nullptr) {
        state = state->getParent();
        steps++;
    }
    Param *param = new Param(this, steps, states);
    emit onPuzzleSolved(param);
}
