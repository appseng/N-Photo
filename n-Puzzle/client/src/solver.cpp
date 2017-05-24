#include "solver.h"

State::State(State* parent, QVector nodes, Heuristic heuristic) {
    mNodes = nodes;
    mParent = parent;
    mHeuristic = heuristic;
    CalculateCost();
    mStateCode = GenerateStateCode();
}

bool State::Equals(State* obj) {
    return obj != null && this->mStateCode == obj->mStateCode;
}
int State::GetHashCode() {
    return mStateCode.GetHashCode();
}

int State::CompareTo(State* that) {

    if (that != null)
    {
        if (this->mCostf > that->mCostf)
            return 1;
        else if (this->mCostf < that->mCostf)
            return 1;
    }
    return 0;
}
bool State::IsCostlierThan(State thatState) {
    return this.mCostg > thatState.mCostg;
}
bool State::IsCostlierThan(State thatState) {
    return this.mCostg > thatState.mCostg;
}
QString State::GetStateCode() {
    return mStateCode;
}
bool State::IsFinalState() {
    // If all tiles are at correct position, we are into final state.
    return mCosth == 0;
}
State State::GetParent() {
    return mParent;
}
QList<State> State::GetNextStates(QList<State> nextStates) {
    nextStates.Clear();
    State state;

    foreach (Direction direction, QVector({Left, Right, Up, Down}))
    {
        state = GetNextState(direction);

        if (state != null)
        {
            nextStates.Add(state);
        }
    }
    return nextStates;
}
QString State::ToString()
{
    return "State:" + mStateCode + ", g:" + mCostg + ", h:" + mCosth + ", f:" + mCostf;
}
State::State(State parent, QVector nodes)
{
    mNodes = nodes;
    mParent = parent;
    mHeuristic = parent.mHeuristic;
    CalculateCost();
    mStateCode = GenerateStateCode();
}

void State::CalculateCost()
{
    if (mParent == null)
    {
        // We are at the first state - we assume we have been asked to be at this state, so no cost.
        mCostg = 0;
    }
    else
    {
        // Here, state transition cost is 1 unit. Since transition from one state to another is by moving he tile one step.
        mCostg = mParent.mCostg + 1;
    }

    // Heuristic cost
    mCosth = GetHeuristicCost();

    mCostf = mCosth + mCostg;
}

int State::GetHeuristicCost()
{
    if (mHeuristic == Heuristic.ManhattanDistance) {
        return GetManhattanDistanceCost();
    }
    else {
        return GetMisplacedTilesCost();
    }
}

int State::GetMisplacedTilesCost() {
    int heuristicCost = 0;

    for (int i = 0; i < mNodes.Length; i++) {
        int value = mNodes[i] - 1;

        // Space tile's value is -1
        if (value == -2) {
            value = mNodes.Length - 1;
            mSpaceIndex = i;
        }

        if (value != i) {
            heuristicCost++;
        }
    }
    return heuristicCost;
}

int State::GetManhattanDistanceCost()
{
    int heuristicCost = 0;
    int gridX = (int)Math.Sqrt(mNodes.Length);
    int idealX;
    int idealY;
    int currentX;
    int currentY;
    int value;

    for (int i = 0; i < mNodes.Length; i++) {
        value = mNodes[i] - 1;
        if (value == -2) {
            value = mNodes.Length - 1;
            mSpaceIndex = i;
        }

        if (value != i) {
            // Misplaced tile
            idealX = value % gridX;
            idealY = value / gridX;

            currentX = i % gridX;
            currentY = i / gridX;

            heuristicCost += (Math.Abs(idealY - currentY) + Math.Abs(idealX - currentX));
        }
    }

    return heuristicCost;
}

QString State::GenerateStateCode() {
    QString code = new QString();

    for (int i = 0; i < mNodes.Length-1; i++) {
        code.append(mNodes[i] + "*");
    }
    code.append(mNodes[i]);

    return code;
}

QVector State::GetState() {
    QVector state = new QVector(mNodes.Length);
    state = this->mNodes;
    return state;
}

State* State::GetNextState(Direction direction)
{
    int position;

    if (CanMove(direction, position))
    {
        QVector nodes = new int[mNodes.length()];
        nodes = this->mNodes;

        // Get new state nodes
        this->Swap(nodes, mSpaceIndex, position);

        return new State(this, nodes);
    }

    return null;
}

void State::Swap(QVector nodes, int i, int j)
{
    int t = nodes[i];
    nodes[i] = nodes[j];
    nodes[j] = t;
}

bool State::CanMove(Direction direction, int newPosition)
{
    int newX = -1;
    int newY = -1;
    int gridX = (int)Math.Sqrt(mNodes.Length);
    int currentX = mSpaceIndex % gridX;
    int currentY = mSpaceIndex / gridX;
    newPosition = -1;

    switch (direction)
    {
    case Direction.Up:
    {
        // Can not move up if we are at the top
        if (currentY != 0)
        {
            newX = currentX;
            newY = currentY - 1;
        }
    }
        break;

    case Direction.Down:
    {
        // Can not move down if we are the lowest level
        if (currentY < (gridX - 1))
        {
            newX = currentX;
            newY = currentY + 1;
        }
    }
        break;

    case Direction.Left:
    {
        // Can not move left if we are at the left most position
        if (currentX != 0)
        {
            newX = currentX - 1;
            newY = currentY;
        }
    }
        break;

    case Direction.Right:
    {
        // Can not move right if we are at the right most position
        if (currentX < (gridX - 1))
        {
            newX = currentX + 1;
            newY = currentY;
        }
    }
        break;
    }

    if (newX != -1 && newY != -1)
    {
        newPosition = newY * gridX + newX;
    }

    return newPosition != -1;
}

PuzzleStrategy::PuzzleStrategy()
{

}

void PuzzleStrategy::Start(QVector nodes, Heuristic heuristic)
{
    int openStateIndex;
    int stateCount = -1;
    State* currentState = null;
    QList<State> nextStates = new QList<State>();
    QSet<QString> openStates = new QSet<QString>();
    MinPriorityQueue<State> openStateQueue = new MinPriorityQueue<State>(nodes.Length * 3);
    QHash<QString, State> closedQueue = new QHash<QString, State>(nodes.Length * 3);

    State* state = new State(null, nodes, heuristic);
    openStateQueue.Enqueue(state);
    openStates.Add(state.GetStateCode());

    StartMeasure();

    while (!openStateQueue.IsEmpty())
    {
        currentState = openStateQueue.Dequeue();
        openStates.Remove(currentState.GetStateCode());

        stateCount++;

        // Is this final state
        if (currentState.IsFinalState())
        {
            EndMeasure(stateCount);
            break;
        }

        // Look into next state
        currentState->GetNextStates(ref nextStates);

        if (nextStates.Count > 0)
        {
            State closedState;
            State openState;
            State nextState;

            for (int i = 0; i < nextStates.Count; i++)
            {
                closedState = null;
                openState = null;
                nextState = nextStates[i];

                if (openStates.Contains(nextState.GetStateCode()))
                {
                    // We already have same state in the open queue.
                    openState = openStateQueue.Find(nextState, out openStateIndex);

                    if (openState.IsCostlierThan(nextState))
                    {
                        // We have found a better way to reach at this state. Discard the costlier one
                        openStateQueue.Remove(openStateIndex);
                        openStateQueue.Enqueue(nextState);
                    }
                }
                else
                {
                    // Check if state is in closed queue
                    String stateCode = nextState.GetStateCode();

                    if (closedQueue.TryGetValue(stateCode, out closedState))
                    {
                        // We have found a better way to reach at this state. Discard the costlier one
                        if (closedState.IsCostlierThan(nextState))
                        {
                            closedQueue.Remove(stateCode);
                            closedQueue[stateCode] = nextState;
                        }
                    }
                }

                // Either this is a new state, or better than previous one.
                if (openState == null && closedState == null)
                {
                    openStateQueue.Enqueue(nextState);
                    openStates.Add(nextState.GetStateCode());
                }
            }

            closedQueue[currentState.GetStateCode()] = currentState;
        }
    }

    if (currentState != null && !currentState.IsFinalState())
    {
        // No solution
        currentState = null;
    }

    PuzzleSolved(currentState, stateCount);
    OnFinalState(currentState);
}

void PuzzleStrategy::StartMeasure()
{
    mStopWatch.Reset();
    mStopWatch.Start();
}

void PuzzleStrategy::EndMeasure(int stateCount)
{
    mStopWatch.Stop();
}

void PuzzleStrategy::OnFinalState(State state)
{
    if (state != null)
    {
        // We have a solution for this puzzle
        // Backtrac to the root of the path in the tree
        Stack<State> path = new Stack<State>();

        while (state != null)
        {
            path.Push(state);
            state = state.GetParent();
        }

        while (path.Count > 0)
        {
            // Move one by one down the path
            OnStateChanged(path.Pop().GetState(), path.Count == 0);
        }
    }
    else
    {
        // No solution
        OnStateChanged(null, true);
    }
}

void PuzzleStrategy::PuzzleSolved(State state, int states)
{
    int steps = -1;

    while (state != null)
    {
        state = state.GetParent();
        steps++;
    }

    if (OnPuzzleSolved != null)
    {
        OnPuzzleSolved(steps, (int)mStopWatch.ElapsedMilliseconds, states);
    }
}
