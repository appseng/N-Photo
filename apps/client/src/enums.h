#ifndef ENUMS_H
#define ENUMS_H

enum Heuristic
{
    MisplacedTiles,
    ManhattanDistance
};

enum Direction
{
    Up,
    Left,
    Down,
    Right
};

enum ImageSourceType
{
    Net=1,
    Internet,
    Local
};

enum MessageType : unsigned int
{
    None = 0,
    File = 4,
    List = 8
};

#endif // ENUMS_H
