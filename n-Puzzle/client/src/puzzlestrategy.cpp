#include "puzzlestrategy.h"

#include <QHash>
#include <QHash>
#include <QSet>
#include <QStack>

#include "minpriorityqueue.h"

PuzzleStrategy::PuzzleStrategy(QObject* parent)
    :QObject(parent)
{

}

void PuzzleStrategy::start(QVector<int>* nodes, Heuristic heuristic)
{
    int openStateIndex;
    int stateCount = -1;
    State *currentState;
    QList<State*> *nextStates = new QList<State*>();
    QSet<QString> openStates;

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

                    if (openState->isCostlierThan(nextState))
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
                        // We have found a better way to reach at this state. Discard the costlier one
                        if (closedState->isCostlierThan(nextState))
                        {
                            closedQueue.remove(stateCode);
                            closedQueue.insert(stateCode, nextState);
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
            closedQueue.remove(currentState->getStateCode());
            closedQueue.insert(currentState->getStateCode(), currentState);
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
        QStack<State*> path;

        while (state != nullptr)
        {
            path.push(state);
            state = state->getParent();
        }

        while (path.count() > 0)
        {
            // Move one by one down the path
            emit onStateChanged(std::make_pair(path.pop()->getState(), path.count() == 0));
        }
    }
    else
    {
        // No solution
        emit onStateChanged(std::make_pair(nullptr, true));
    }
}

void PuzzleStrategy::puzzleSolved(State *state, int states)
{
    int steps = -1;
    while (state != nullptr)
    {
        state = state->getParent();
        steps++;
    }
    emit onPuzzleSolved(std::make_pair(steps, states));
}
