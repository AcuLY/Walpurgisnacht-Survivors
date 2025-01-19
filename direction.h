#ifndef DIRECTION_H
#define DIRECTION_H

#include <QPair>
#include <cmath>

// 方向辅助量
const double DIAGONAL_FACTOR = 1 / std::sqrt(2.0);

enum BiDirection { Negative = -1, Neutral = 0, Positive = 1 };

enum Direction { West, South, North, East, Center, NorthEast, NorthWest, SouthEast, SouthWest };

const Direction fourDirections[4] = {North, East, South, West};
const Direction eightDirections[8]
    = {North, NorthEast, East, SouthEast, South, SouthWest, West, NorthWest};

QPair<BiDirection, BiDirection> operator~(Direction dir); // 将八向解析为 x 和 y 方向

Direction pairBiDirection(BiDirection d1, BiDirection d2); // 将 x 和 y 方向合并为八向

#endif // DIRECTION_H
