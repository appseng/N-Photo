#ifndef SOLVER_H
#define SOLVER_H

#include <QVector>
#include <QString>
#include <QHash>

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

class State
{
private :
    QVector mNodes;
    int mSpaceIndex;
    QString mStateCode;
    int mCostf;
    int mCosth;
    int mCostg;
    Heuristic mHeuristic;
    State mParent;

public:
    State(State* parent, QVector nodes, Heuristic heuristic);
    bool Equals(State* obj);
    int GetHashCode();
    int CompareTo(State* that);
    bool IsCostlierThan(State thatState);
    bool IsCostlierThan(State thatState);
    QString GetStateCode();
    bool IsFinalState();
    State GetParent();
    QList<State> GetNextStates(QList<State> nextStates);
    QString ToString();
    State(State parent, QVector nodes);
    void CalculateCost();
    int GetHeuristicCost();
    int GetMisplacedTilesCost();
    int GetManhattanDistanceCost();
    QString GenerateStateCode();
    QVector GetState();
    State* GetNextState(Direction direction);
    void Swap(QVector nodes, int i, int j);
    bool CanMove(Direction direction, int newPosition);
};

class PuzzleStrategy
{
public:
    PuzzleStrategy();
private:
    void Start(QVector nodes, Heuristic heuristic);
    void StartMeasure();
    void EndMeasure(int stateCount);
    void OnFinalState(State state);
    void PuzzleSolved(State state, int states);
}
#endif // SOLVER_H
