#include "direction.h"

QPair<BiDirection, BiDirection> operator~(Direction dir) {
    switch (dir) {
        case West:
            return {Negative, Neutral};
        case South:
            return {Neutral, Positive};
        case North:
            return {Neutral, Negative};
        case East:
            return {Positive, Neutral};
        case Center:
            return {Neutral, Neutral};
        case NorthEast:
            return {Positive, Negative};
        case NorthWest:
            return {Negative, Negative};
        case SouthEast:
            return {Positive, Positive};
        case SouthWest:
            return {Negative, Positive};
    }
    return {Neutral, Neutral};
}

Direction pairBiDirection(BiDirection d1, BiDirection d2) {
    if (d1 == Negative && d2 == Neutral)
        return West;
    if (d1 == Neutral && d2 == Positive)
        return South;
    if (d1 == Neutral && d2 == Negative)
        return North;
    if (d1 == Positive && d2 == Neutral)
        return East;
    if (d1 == Positive && d2 == Negative)
        return NorthEast;
    if (d1 == Negative && d2 == Negative)
        return NorthWest;
    if (d1 == Positive && d2 == Positive)
        return SouthEast;
    if (d1 == Negative && d2 == Positive)
        return SouthWest;

    return Center;
}
