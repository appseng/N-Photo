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
        State(QObject*, State*, QVector<int>*, Heuristic );
        State(QObject* , State*, QVector<int>*);
        bool equals(const State*) const;
        int compareTo(const State*) const;
        bool isCostlierThan(State*) const;
        QString getStateCode() const;
        bool isFinalState() const;
        State* getParent() const;
        QList<State*> *getNextStates(QList<State*>*);
        QString toString() const;
        void calculateCost();
        int getHeuristicCost();
        int getMisplacedTilesCost();
        int getManhattanDistanceCost();
        QString generateStateCode();
        QVector<int>* getState() const;
        State* getNextState(Direction);
        void swap(QVector<int>*, int, int) const;
        bool canMove(Direction, int&) const;
};

class Param : public QObject
{
    Q_OBJECT
public:
    Param(QObject*, QVector<int>*, bool);
    Param(QObject*, int, int);
    QVector<int>* getState() const;
    int getStates() const;
    int getSteps() const;
    bool isFinalState() const;
private:
    int steps;
    int states;
    QVector<int> *state;
    bool finalState;
};
#endif // SOLVER_H
