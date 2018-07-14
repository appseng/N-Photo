#include "minpriorityqueue.h"

MinPriorityQueue::MinPriorityQueue(QObject *parent)
    :QObject(parent)
{
    mArray = new QList<State*>();
    mCount = 0;
}

bool MinPriorityQueue::less(int i, int j)
{
    return mArray->at(i)->compareTo(mArray->at(j)) < 0;
}
void MinPriorityQueue::swap(int i, int j)
{
    State *temp = mArray->at(j);
    mArray->replace(j, mArray->at(i));
    mArray->replace(i, temp);
}

void MinPriorityQueue::sink(int index)
{
    int k;
    while (index * 2 < mCount)
    {
        k = index * 2;

        if (k + 1 <= mCount && less(k + 1, k))
        {
            k = k + 1;
        }

        if (!less(k, index))
        {
            break;
        }

        swap(index, k);
        index = k;
    }
}

void MinPriorityQueue::swim(int index)
{
    int k;

    while (index / 2 >= 0)
    {
        k = index / 2;

        if (!less(index, k))
        {
            break;
        }

        swap(index, k);
        index = k;
    }
}

bool MinPriorityQueue::isEmpty()
{
    return mCount == 0;
}

void MinPriorityQueue::enqueue(State *item)
{
    ++mCount;
    mArray->append(item);
    swim(mCount);
}

State* MinPriorityQueue::dequeue()
{
    if (!isEmpty())
    {
        State *item = mArray->at(0);
        mArray->replace(0, mArray->at(mCount));
        mArray->replace(mCount--, NULL);

        sink(0);

        return item;
    }
    return NULL;
}

State* MinPriorityQueue::find(State *item, int &index)
{
    index = -1;
    if (!isEmpty())
    {
        int i = 0;

        while (i++ <= mCount)
        {
            if (mArray->at(i)->equals(item))
            {
                index = i;
                return mArray->at(i);
            }
        }
    }

    return NULL;
}

void MinPriorityQueue::remove(int index)
{
    if (index >= 0 && index < mCount)
    {
        mArray->replace(index, mArray->at(mCount));
        mArray->replace(mCount--, NULL);
        sink(index);
    }
}
