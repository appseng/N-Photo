#include <QtMath>

#include "state.h"

State::State(State* p, const QVector<char>* nodes, Heuristic h, QObject* qparent)
    :QObject(qparent), heuristic(h), nodes(nodes), parent(p)
{
    calculateCost();
    oldSpaceIndex = (p != nullptr) ? p->getSpaceIndex(): -1;
}

State::State(State* parent, const QVector<char>* nodes, QObject* qparent)
    :State(parent, nodes, parent->heuristic, qparent)
{
}

State::~State() {
    delete nodes;
}

bool State::equals(const State *obj) const
{
    if (obj == nullptr) return false;

    for (int i = 0; i< nodes->length(); i++) {
        if (nodes->at(i) != obj->nodes->at(i))
            return false;
    }
    return  true;
}

int State::compareTo(const State* that) const
{
    if (that != nullptr) {
        if (costf > that->costf)
            return 1;
        else if (costf < that->costf)
            return -1;
    }
    return 0;
}
bool State::isFinalState() const
{
    return costh == 0;
}
State* State::getParent() const
{
    return parent;
}
void State::calculateCost()
{
    if (parent == nullptr) {
        costg = 0;
    } else {
        costg = parent->getCostG() + 1;
    }

    costh = getHeuristicCost();

    costf = costh + costg;
}

int State::getCostG() const
{
    return costg;
}

int State::getCost() const
{
    return costf;
}

char State::getSpaceIndex() const
{
    return spaceIndex;
}

int State::getHeuristicCost()
{
    if (heuristic == ManhattanDistance) {
        return getManhattanDistanceCost();
    } else {
        return getMisplacedTilesCost();
    }
}

int State::getMisplacedTilesCost()
{
    int heuristicCost = 0;

    for (char i = 0; i < nodes->length(); i++) {
        int value = nodes->at(i) - 1;

        // Space tile's value is 11
        if (value == -2) {
            value = nodes->length() - 1;
            spaceIndex = i;
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
    char gridX = char(qSqrt(nodes->length()));
    char idealX;
    char idealY;
    char currentX;
    char currentY;
    char value;

    for (char i = 0; i < nodes->length(); i++) {
        value = nodes->at(i) - 1;
        if (value == -2) {
            value = char(nodes->length()) - 1;
            spaceIndex = char(i);
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

const QVector<char>* State::getNodes() const
{
    return nodes;
}

State* State::getNextState(Direction direction)
{
    char position = -1;

    if (canMove(direction, position)) {
        QVector<char>* vector = new QVector<char>(*nodes);

        vector->replace(spaceIndex, vector->at(position));
        vector->replace(position, -1);

        return new State(this, vector, this);
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
    char gridX = char(qSqrt(nodes->length()));
    char currentX = spaceIndex % gridX;
    char currentY = spaceIndex / gridX;
    newPosition = -1;

    switch (direction)
    {
        case Up:
            // Can not move up if we are at the top
            if (currentY != 0) {
                newX = currentX;
                newY = currentY - 1;
            }
            break;
        case Down:
            // Can not move down if we are the lowest level
            if (currentY < (gridX - 1)) {
                newX = currentX;
                newY = currentY + 1;
            }
            break;
        case Left:
            // Can not move left if we are at the left most position
            if (currentX != 0) {
                newX = currentX - 1;
                newY = currentY;
            }
            break;

        case Right:
            // Can not move right if we are at the right most position
            if (currentX < (gridX - 1)) {
                newX = currentX + 1;
                newY = currentY;
            }
            break;
    }

    if (newX != -1 && newY != -1) {
        newPosition = newY * gridX + newX;
    }

    if (oldSpaceIndex == newPosition)
        return false;

    return newPosition != -1;
}
