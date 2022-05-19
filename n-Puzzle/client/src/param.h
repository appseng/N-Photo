#ifndef PARAM_H
#define PARAM_H

#include <QObject>

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
#endif // PARAM_H
