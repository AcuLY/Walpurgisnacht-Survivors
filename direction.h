#ifndef DIRECTION_H
#define DIRECTION_H

#include <cmath>

const double DIAGONAL_FACTOR = 1 / std::sqrt(2.0);

enum BiDirection { Negative = -1, Neutral = 0, Positive = 1 };

enum Direction { West = 'W', South = 'S', North = 'N', East = 'E', Center = 'C' };

#endif // DIRECTION_H
