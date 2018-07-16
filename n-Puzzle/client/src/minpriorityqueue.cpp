#include "minpriorityqueue.h"
#include <QtAlgorithms>

MinPriorityQueue::MinPriorityQueue(QObject *parent)
    :QObject(parent)
{
    mArray = new QList<State*>();
}

bool MinPriorityQueue::isEmpty() const
{
    return mArray->isEmpty();
}

bool MinPriorityQueue::less (const int a, const int b) const
{
   return mArray->at(a)->compareTo(mArray->at(b)) < 0;
}
void MinPriorityQueue::enqueue(State *item)
{
    mArray->append(item);
    int pos_min = -1;
    State* temp = nullptr;

    for (int i = 0; i < mArray->length()-1; i++) {
        pos_min = i;
        for (int j = i+1; j < mArray->length(); j++) {
            if (less(j, i)) {
                pos_min = j;
            }
        }
        if (pos_min != i)
        {
            temp = mArray->at(i);
            mArray->replace(i, mArray->at(pos_min));
            mArray->replace(pos_min, temp);
        }
    }
}

State* MinPriorityQueue::dequeue()
{
    if (!mArray->isEmpty())
    {
        return mArray->takeFirst();;
    }
    return nullptr;
}

State* MinPriorityQueue::find(State *item, int &index) const
{
    index = -1;

    if (!mArray->isEmpty())
    {
        int i = 0;
        int length = mArray->length();
        State *state = nullptr;
        while (i < length)
        {
            state = mArray->at(i);
            if (state->equals(item))
            {
                index = i;
                return state;
            }
            i++;
        }
    }
    return nullptr;
}

void MinPriorityQueue::remove(int index)
{
    if (index >= 0 && index < mArray->length())
    {
        mArray->removeAt(index);
    }
}
