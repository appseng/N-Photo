#include "state.h"

State::State(QObject* qparent, State* parent, QVector<int>* nodes, Heuristic heuristic)
    :QObject(qparent)
{
    mNodes = nodes;
    mParent = parent;
    mHeuristic = heuristic;
    calculateCost();
    generateStateCode();
    mOldSpaceIndex = (parent != nullptr) ? parent->getSpaceIndex(): -1;
}

State::State(QObject* qparent, State* parent, QVector<int>* nodes)
    :State(qparent, parent, nodes, parent->mHeuristic)
{
}

bool State::equals(const State *obj) const
{
    return obj != nullptr && mStateCode == obj->getStateCode();
}

int State::compareTo(const State* that) const
{

    if (that != nullptr)
    {
        if (mCostf > that->mCostf)
            return 1;
        else if (mCostf < that->mCostf)
            return -1;
    }
    return 0;
}
bool State::isCostlierThan(State *thatState) const
{
    return mCostg > thatState->mCostg;
}
QString State::getStateCode() const
{
    return mStateCode;
}
bool State::isFinalState() const
{
    // If all tiles are at correct position, we are into final state.
    return mCosth == 0;
}
State* State::getParent() const
{
    return mParent;
}
QList<State*>* State::getNextStates(QList<State*> *nextStates) {
    State *state;

    QVector<Direction> statesList;
    statesList.append(Right);
    statesList.append(Down);
    statesList.append(Left);
    statesList.append(Up);

    foreach (Direction direction, statesList)
    {
        state = getNextState(direction);

        if (state != nullptr)
        {
            nextStates->append(state);
        }
    }
    return nextStates;
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

int State::getCost()
{
    return mCosth + mCostg;
}

int State::getCostg()
{
    return mCostg;
}

int State::getCosth()
{
    return mCosth;
}

void State::setCostg(int cost) {
    mCostg = cost;
}

int State::getSpaceIndex() {
    return mSpaceIndex;
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

        // Space tile's value is 11
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
    int gridX = int(qSqrt(mNodes->length()));
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
            idealX = value % gridX;
            idealY = value / gridX;

            currentX = i % gridX;
            currentY = i / gridX;

            heuristicCost += (qAbs(idealY - currentY) + qAbs(idealX - currentX));
        }
    }

    return heuristicCost;
}

void State::generateStateCode()
{
    mStateCode.clear();
    int s = mNodes->size()-1;
    for (int i = 0; i < s; i++) {
        mStateCode.append(trUtf8("%1*").arg(mNodes->at(i)));
    }
    mStateCode.append(trUtf8("%1").arg(mNodes->last()));
}

QVector<int>* State::getState() const
{
    QVector<int> *state = new QVector<int>(*mNodes);
    return state;
}

State* State::getNextState(Direction direction)
{
    int position = -1;

    if (canMove(direction, position))
    {
        QVector<int> *nodes = new QVector<int>(*mNodes);

        //swap(nodes, mSpaceIndex, position);
        nodes->replace(mSpaceIndex, nodes->at(position));
        nodes->replace(position, -1);

        return new State(this, this, nodes);
    }

    return nullptr;
}

void State::swap(QVector<int> *nodes, int i, int j) const
{
    int t = nodes->at(i);
    nodes->replace(i, nodes->at(j));
    nodes->replace(j, t);
}

bool State::canMove(Direction direction, int &newPosition) const
{
    int newX = -1;
    int newY = -1;
    int gridX = int(qSqrt(mNodes->length()));
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

    if (mOldSpaceIndex == newPosition)
        return false;

    return newPosition != -1;
}

Param::Param(QObject *parent, QVector<int> *s, bool f)
    :QObject(parent)
{
    state = s;
    finalState = f;
}
Param::Param(QObject *parent, int i, int j)
    :QObject(parent)
{
    steps = i;
    states = j;
}
QVector<int>* Param::getState() const
{
    return state;
}
bool Param::isFinalState() const
{
    return finalState;
}
int Param::getStates() const
{
    return states;
}
int Param::getSteps() const
{
    return steps;
}
