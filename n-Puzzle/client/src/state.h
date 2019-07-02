#ifndef SOLVER_H
#define SOLVER_H

#include <QtGlobal>
#include <QtMath>
#include <QObject>
#include <QVector>
#include <QString>
#include <QList>

#include <cstddef>

#include "enums.h"

class State : public QObject
{
    Q_OBJECT
    public:
        State(QObject*, State*, QVector<char>*, Heuristic );
        State(QObject* , State*, QVector<char>*);
        bool equals(const State*) const;
        int compareTo(const State*) const;
        bool isCostlierThan(State*) const;
        bool isFinalState() const;
        State* getParent() const;
        QList<State*> *getNextStates(QList<State*>*);
        void calculateCost();
        int getCost() const;
        int getHeuristicCost();
        int getMisplacedTilesCost();
        int getManhattanDistanceCost();
        QVector<char>* getState() const;
        State* getNextState(Direction);
        void swap(QVector<char>*, int, int) const;
        bool canMove(Direction, char&) const;
        char getSpaceIndex() const;
        Heuristic mHeuristic;
    private:
        QVector<char> mNodes;
        char mSpaceIndex;
        char mOldSpaceIndex;
        int mCostf;
        int mCosth;
        int mCostg;
        State *mParent;
};

class Param : public QObject
{
    Q_OBJECT
public:
    Param(QObject*, QVector<char>*, bool);
    Param(QObject*, int, int);
    QVector<char>* getState() const;
    int getStates() const;
    int getSteps() const;
    bool isFinalState() const;
private:
    int steps;
    int states;
    QVector<char> *state;
    bool finalState;
};
#endif // SOLVER_H
