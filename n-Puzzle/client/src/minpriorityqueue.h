#ifndef MINPRIORITYQUEUE_H
#define MINPRIORITYQUEUE_H

#include <QList>
#include <QObject>

#include "state.h"

class MinPriorityQueue : public QObject
{
    Q_OBJECT
    private:
        QList<State*> *mArray;
    public:
        MinPriorityQueue(QObject *parent = 0);
        bool isEmpty() const;
        void enqueue(State*);
        State* dequeue();
        State* find(State*, int&) const;
        void remove(const int);
    private:
        bool less (const int, const int) const;
};

#endif // MINPRIORITYQUEUE_H
