#include "state.h"

#include <QtGlobal>
#include <QtMath>

#include <cstddef>

State::State(QObject* qparent, State* parent, QVector<int> *nodes, Heuristic heuristic)
    :QObject(qparent)
{
    mNodes = nodes;
    mParent = parent;
    mHeuristic = heuristic;
    calculateCost();
    mStateCode = generateStateCode();
}

bool State::equals(State *obj) {
    return obj != nullptr && this->mStateCode == obj->mStateCode;
}

int State::compareTo(State* that) {

    if (that != nullptr)
    {
        if (mCostf > that->mCostf)
            return 1;
        else if (mCostf < that->mCostf)
            return -1;
    }
    return 0;
}
bool State::isCostlierThan(State *thatState) {
    return this->mCostg > thatState->mCostg;
}
QString State::getStateCode() {
    return mStateCode;
}
bool State::isFinalState() {
    // If all tiles are at correct position, we are into final state.
    return mCosth == 0;
}
State* State::getParent() {
    return mParent;
}
QList<State*>* State::getNextStates(QList<State*> *nextStates) {
    nextStates->clear();
    State *state;

    QVector<Direction> statesList;
    statesList.append(Left);
    statesList.append(Right);
    statesList.append(Up);
    statesList.append(Down);

    int i = 0;
    foreach (Direction direction, statesList)
    {
        state = getNextState(direction);

        if (state != nullptr)
        {
            nextStates->insert(i++, state);
        }
    }
    return nextStates;
}
QString State::toString()
{
    return "State:" + mStateCode + ", g:" + mCostg + ", h:" + mCosth + ", f:" + mCostf;
}
State::State(QObject *qparent, State *parent, QVector<int>* nodes)
    :QObject(qparent)
{
    mNodes = nodes;
    mParent = parent;
    mHeuristic = parent->mHeuristic;
    calculateCost();
    mStateCode = generateStateCode();
}

void State::calculateCost()
{
    if (mParent == nullptr)
    {
        // We are at the first state - we assume we have been asked to be at this state, so no cost.
        mCostg = 0;
    }
    else
    {
        // Here, state transition cost is 1 unit. Since transition from one state to another is by moving he tile one step.
        mCostg = mParent->mCostg + 1;
    }

    // Heuristic cost
    mCosth = getHeuristicCost();

    mCostf = mCosth + mCostg;
}

int State::getHeuristicCost()
{
    if (mHeuristic == ManhattanDistance) {
        return getManhattanDistanceCost();
    }
    else {
        return getMisplacedTilesCost();
    }
}

int State::getMisplacedTilesCost() {
    int heuristicCost = 0;

    for (int i = 0; i < mNodes->length(); i++) {
        int value = mNodes->at(i) - 1;

        // Space tile's value is -1
        if (value == -2) {
            value = mNodes->length() - 1;
            mSpaceIndex = i;
        }

        if (value != i) {
            heuristicCost++;
        }
    }
    return heuristicCost;
}

int State::getManhattanDistanceCost()
{
    int heuristicCost = 0;
    int gridX = (int)qSqrt(mNodes->length());
    int idealX;
    int idealY;
    int currentX;
    int currentY;
    int value;

    for (int i = 0; i < mNodes->length(); i++) {
        value = mNodes->at(i) - 1;
        if (value == -2) {
            value = mNodes->length() - 1;
            mSpaceIndex = i;
        }

        if (value != i) {
            // Misplaced tile
            idealX = value % gridX;
            idealY = value / gridX;

            currentX = i % gridX;
            currentY = i / gridX;

            heuristicCost += (qAbs(idealY - currentY) + qAbs(idealX - currentX));
        }
    }

    return heuristicCost;
}

QString State::generateStateCode() {
    QString code;

    for (int i = 0; i < mNodes->length()-1; i++) {
        code.append(QString(mNodes->at(i))).append("*");
    }
    code.append(QString(mNodes->last()));

    return code;
}

QVector<int>* State::getState() {
    QVector<int> *state = nullptr;
    for(auto item : (*mNodes)){
        state = new QVector<int>(item);
    };
    return state;
}

State* State::getNextState(Direction direction)
{
    int position = -1;

    if (canMove(direction, position))
    {
        QVector<int> *nodes = nullptr;
        for(auto item : (*mNodes)){
            nodes = new QVector<int>(item);
        };

        // Get new state nodes
        swap(nodes, mSpaceIndex, position);

        return new State(this, this, nodes);
    }

    return nullptr;
}

void State::swap(QVector<int> *nodes, int i, int j)
{
    int t = nodes->at(i);
    nodes->replace(i, nodes->at(j));
    nodes->replace(j, t);
}

bool State::canMove(Direction direction, int &newPosition)
{
    int newX = -1;
    int newY = -1;
    int gridX = (int)qSqrt(mNodes->length());
    int currentX = mSpaceIndex % gridX;
    int currentY = mSpaceIndex / gridX;
    newPosition = -1;

    switch (direction)
    {
        case Up:
            // Can not move up if we are at the top
            if (currentY != 0)
            {
                newX = currentX;
                newY = currentY - 1;
            }
            break;
        case Down:
            // Can not move down if we are the lowest level
            if (currentY < (gridX - 1))
            {
                newX = currentX;
                newY = currentY + 1;
            }
            break;
        case Left:
            // Can not move left if we are at the left most position
            if (currentX != 0)
            {
                newX = currentX - 1;
                newY = currentY;
            }
            break;

        case Right:
            // Can not move right if we are at the right most position
            if (currentX < (gridX - 1))
            {
                newX = currentX + 1;
                newY = currentY;
            }
            break;
    }

    if (newX != -1 && newY != -1)
    {
        newPosition = newY * gridX + newX;
    }

    return newPosition != -1;
}

