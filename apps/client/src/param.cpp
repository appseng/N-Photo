#include "param.h"

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
