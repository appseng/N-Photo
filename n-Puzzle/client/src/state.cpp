#include "state.h"

State::State(QObject* qparent, State* parent, QVector<char>* nodes, Heuristic heuristic)
    :QObject(qparent)
{
    mNodes = nodes;
    mParent = parent;
    mHeuristic = heuristic;
    calculateCost();
    mOldSpaceIndex = (parent != nullptr) ? parent->getSpaceIndex(): -1;
}

State::State(QObject* qparent, State* parent, QVector<char>* nodes)
    :State(qparent, parent, nodes, parent->mHeuristic)
{
}

State::~State() {
    delete mNodes;
}

bool State::equals(const State *obj) const
{
    if (obj == nullptr) return false;

    for (int i = 0; i< mNodes->length(); i++) {
        if (mNodes->at(i) != obj->mNodes->at(i))
            return false;
    }
    return  true;
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

bool State::isFinalState() const
{
    // If all tiles are at correct position, we are into final state.
    return mCosth == 0;
}
State* State::getParent() const
{
    return mParent;
}
void State::getNextStates(QList<State*> *nextStates) {
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

int State::getCost() const
{
    return mCostf;
}

char State::getSpaceIndex() const
{
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

    for (char i = 0; i < mNodes->length(); i++) {
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
    char gridX = char(qSqrt(mNodes->length()));
    char idealX;
    char idealY;
    char currentX;
    char currentY;
    char value;

    for (char i = 0; i < mNodes->length(); i++) {
        value = mNodes->at(i) - 1;
        if (value == -2) {
            value = char(mNodes->length()) - 1;
            mSpaceIndex = char(i);
        }

        if (value != i) {
            idealX = value % gridX;
            idealY = value / gridX;

            currentX = i % gridX;
            currentY = i / gridX;

            heuristicCost += qAbs(idealY - currentY) + qAbs(idealX - currentX);
        }
    }

    return heuristicCost;
}

const QVector<char>* State::getState() const
{
    const QVector<char> *state = mNodes;
    return state;
}

State* State::getNextState(Direction direction)
{
    char position = -1;

    if (canMove(direction, position))
    {
        QVector<char> *nodes = new QVector<char>(*mNodes);

        //swap(nodes, mSpaceIndex, position);
        nodes->replace(mSpaceIndex, nodes->at(position));
        nodes->replace(position, -1);

        return new State(this, this, nodes);
    }

    return nullptr;
}

void State::swap(QVector<char> *nodes, int i, int j) const
{
    char t = nodes->at(i);
    nodes->replace(i, nodes->at(j));
    nodes->replace(j, t);
}

bool State::canMove(Direction direction, char &newPosition) const
{
    char newX = -1;
    char newY = -1;
    char gridX = char(qSqrt(mNodes->length()));
    char currentX = mSpaceIndex % gridX;
    char currentY = mSpaceIndex / gridX;
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

Param::Param(QObject *parent, const QVector<char> *s, bool f)
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
const QVector<char>* Param::getState() const
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
