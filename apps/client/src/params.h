#ifndef PARAM_H
#define PARAM_H

#include <QObject>
#include <QVector>

class StateParam : public QObject
{
    Q_OBJECT
public:
    StateParam(QObject*, const QVector<char>*, bool);
    const QVector<char>* getState() const;
    bool isFinalState() const;
private:
    const QVector<char> *state;
    bool finalState;
};

class StepParam : public QObject
{
    Q_OBJECT
public:
    StepParam(QObject*, int, int);
    int getStates() const;
    int getSteps() const;
private:
    int steps;
    int states;
};
#endif // PARAM_H
