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
        ~State();
        bool equals(const State*) const;
        int compareTo(const State*) const;
        bool isCostlierThan(State*) const;
        bool isFinalState() const;
        State* getParent() const;
        void calculateCost();
        int getCost() const;
        int getHeuristicCost();
        int getMisplacedTilesCost();
        int getManhattanDistanceCost();
        const QVector<char>* getState() const;
        State* getNextState(Direction);
        void swap(QVector<char>*, int, int) const;
        bool canMove(Direction, char&) const;
        char getSpaceIndex() const;
        int getCostG() const;
        Heuristic heuristic;
    private:
        QVector<char> *qNodes;
        char spaceIndex;
        char oldSpaceIndex;
        int costf;
        int costh;
        int costg;
        State *parent;
};

class Param : public QObject
{
    Q_OBJECT
public:
    Param(QObject*, const QVector<char>*, bool);
    Param(QObject*, int, int);
    const QVector<char>* getState() const;
    int getStates() const;
    int getSteps() const;
    bool isFinalState() const;
private:
    int steps;
    int states;
    const QVector<char> *state;
    bool finalState;
};
#endif // SOLVER_H
