#include "puzzlestrategy.h"

PuzzleStrategy::PuzzleStrategy(QObject *parent)
    :QObject(parent)
{
    timer = new QTimer(this);
    timer->setInterval(1200);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateState()));
}

void PuzzleStrategy::start(QVector<int>* nodes, Heuristic heuristic)
{
    int openStateIndex = -1;
    int stateCount = -1;
    State *currentState;
    QList<State*> *nextStates = new QList<State*>();
    QSet<QString> openStates;

    //MinPriorityQueue2 openStateQueue(nodes->length() * 3);
    MinPriorityQueue openStateQueue;
    QHash<QString, State*> closedQueue;

    State* state = new State(this, nullptr, nodes, heuristic);
    openStateQueue.enqueue(state);
    openStates.insert(state->getStateCode());

    while (!openStateQueue.isEmpty())
    {
        currentState = openStateQueue.dequeue();
        openStates.remove(currentState->getStateCode());

        stateCount++;

        // Is this final state
        if (currentState->isFinalState())
        {
            break;
        }

        // Look into next state
        currentState->getNextStates(nextStates);

        if (nextStates->count() > 0)
        {
            State *closedState;
            State *openState;
            State *nextState;

            for (int i = 0; i < nextStates->count(); i++)
            {
                closedState = nullptr;
                openState = nullptr;
                nextState = nextStates->at(i);

                if (openStates.contains(nextState->getStateCode()))
                {
                    // We already have same state in the open queue.
                    openState = openStateQueue.find(nextState, openStateIndex);

                    if (openState != nullptr && openState->isCostlierThan(nextState))
                    {
                        // We have found a better way to reach at this state. Discard the costlier one
                        openStateQueue.remove(openStateIndex);
                        openStateQueue.enqueue(nextState);
                    }
                }
                else
                {
                    // Check if state is in closed queue
                    QString stateCode = nextState->getStateCode();

                    if (closedQueue.contains(stateCode))
                    {
                        closedState = closedQueue.value(stateCode);
                        // We have found a better way to reach at this state. Discard the costlier one
                        if (closedState->isCostlierThan(nextState))
                        {
                            closedQueue[stateCode] = nextState;
                        }
                    }
                }
                // Either this is a new state, or better than previous one.
                if (openState == nullptr && closedState == nullptr)
                {
                    openStateQueue.enqueue(nextState);
                    openStates.insert(nextState->getStateCode());
                }
            }
            //closedQueue.remove(currentState->getStateCode());
            closedQueue[currentState->getStateCode()] = currentState;
        }
    }
    if (currentState != nullptr && !currentState->isFinalState())
    {
        // No solution
        currentState = nullptr;
    }
    puzzleSolved(currentState, stateCount);
    onFinalState(currentState);
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
        Param *param = new Param(this, path.pop()->getState(), path.count() == 0);
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
