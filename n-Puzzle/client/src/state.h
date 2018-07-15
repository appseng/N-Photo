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
        bool isCostlierThan(State*);
        QString getStateCode();
        bool isFinalState();
        State* getParent();
        QList<State*> *getNextStates(QList<State*>*);
        QString toString();
        void calculateCost();
        int getHeuristicCost();
        int getMisplacedTilesCost();
        int getManhattanDistanceCost();
        QString generateStateCode();
        QVector<int>* getState();
        State* getNextState(Direction);
        void swap(QVector<int>*, int, int);
        bool canMove(Direction, int&);
};

class Param : public QObject
{
    Q_OBJECT
public:
    Param(QObject*, QVector<int>*, bool);
    Param(QObject*, int, int);
    QVector<int>* getState();
    int getStates();
    int getSteps();
    bool isFinalState();
private:
    int steps;
    int states;
    QVector<int> *state;
    bool finalState;
};
#endif // SOLVER_H
