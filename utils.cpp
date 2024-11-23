#include "utils.h"

double MathUtils::euclideanDistance(double x1, double y1, double x2, double y2) {
    return std::sqrt(std::pow(x1 - x2, 2.0) + std::pow(y1 - y2, 2.0));
}
