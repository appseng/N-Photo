#ifndef SOLVER_H
#define SOLVER_H

#include <QObject>
#include <QVector>
#include <QString>
#include <QList>

enum Heuristic {
    MisplacedTiles,
    ManhattanDistance
};

enum Direction
{
    Left,
    Right,
    Up,
    Down,
};

class State : public QObject
{
    Q_OBJECT
    public:
        Heuristic mHeuristic;
    private :
        QVector<int> *mNodes;
        int mSpaceIndex;
        QString mStateCode;
        int mCostf;
        int mCosth;
        int mCostg;
        State *mParent;
    public:
        State(QObject* qparent, State *parent, QVector<int> *nodes, Heuristic heuristic);
        State(QObject* qparent, State *parent, QVector<int> *nodes);
        bool equals(State *obj);
        int compareTo(State *that);
        bool isCostlierThan(State *thatState);
        QString getStateCode();
        bool isFinalState();
        State* getParent();
        QList<State*> *getNextStates(QList<State*> *nextStates);
        QString toString();
        void calculateCost();
        int getHeuristicCost();
        int getMisplacedTilesCost();
        int getManhattanDistanceCost();
        QString generateStateCode();
        QVector<int>* getState();
        State* getNextState(Direction direction);
        void swap(QVector<int> *nodes, int i, int j);
        bool canMove(Direction direction, int &newPosition);
};

#endif // SOLVER_H
