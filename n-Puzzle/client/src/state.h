#ifndef SOLVER_H
#define SOLVER_H

#include <QtGlobal>
#include <QtMath>
#include <QObject>
#include <QVector>
#include <QString>
#include <QList>

#include "enums.h"

class State : public QObject
{
    Q_OBJECT
    public:
        State(State*, QVector<char>*, Heuristic, QObject* = nullptr);
        State(State*, QVector<char>*, QObject* = nullptr);
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


#endif // SOLVER_H
