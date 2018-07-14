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
        int mCount;
    public:
        MinPriorityQueue(QObject *parent = 0);
        bool isEmpty();
        void enqueue(State* item);
        State* dequeue();
        State* find(State* item, int &index);
        void remove(int index);
    private:
        void expand(int capacity);
        bool less(int i, int j);
        void swap(int i, int j);
        void sink(int index);
        void swim(int index);
};

#endif // MINPRIORITYQUEUE_H
