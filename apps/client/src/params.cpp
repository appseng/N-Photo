#include "params.h"

StateParam::StateParam(QObject *parent, const QVector<char> *s, bool f)
    :QObject(parent)
{
    state = s;
    finalState = f;
}
const QVector<char>* StateParam::getState() const
{
    return state;
}
bool StateParam::isFinalState() const
{
    return finalState;
}

StepParam::StepParam(QObject *parent, int i, int j)
    :QObject(parent)
{
    steps = i;
    states = j;
}
int StepParam::getStates() const
{
    return states;
}
int StepParam::getSteps() const
{
    return steps;
}
