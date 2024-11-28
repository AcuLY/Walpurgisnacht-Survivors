#ifndef DIRECTION_H
#define DIRECTION_H

#include <QPair>
#include <cmath>

const double DIAGONAL_FACTOR = 1 / std::sqrt(2.0);

enum BiDirection { Negative = -1, Neutral = 0, Positive = 1 };

enum Direction {
    West = 'W',
    South = 'S',
    North = 'N',
    East = 'E',
    Center = 'C',
    NorthEast = '1',
    NorthWest = '2',
    SouthEast = '3',
    SouthWest = '4'
};

QPair<BiDirection, BiDirection> operator~(Direction dir);

Direction pairBiDirection(BiDirection d1, BiDirection d2);

#endif // DIRECTION_H
