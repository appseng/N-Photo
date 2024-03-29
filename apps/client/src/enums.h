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

enum MessageType : unsigned char
{
    None = 0,
    File = 4,
    List = 8,
    More = 2
};

enum GameType : bool
{
    JigSaw = false,
    NPhoto = true
};

#endif // ENUMS_H
