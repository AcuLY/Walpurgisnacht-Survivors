#include "utils.h"

std::pair<double, double> MathUtils::velocityDecomQPointF(double velocity, double degree) {
    double vX = velocity * cos(degree);
    double vY = velocity * sin(degree);
    return std::make_pair(vX, vY);
}

double MathUtils::calculateDegree(QPointF pos1, QPointF pos2) {
    double dx = pos2.x() - pos1.x();
    double dy = pos2.y() - pos1.y();
    return atan2(dy, dx);
}

double MathUtils::euclideanDistance(QPointF pos1, QPointF pos2) {
    return std::sqrt(std::pow(pos1.x() - pos2.x(), 2.0) + std::pow(pos1.y() - pos2.y(), 2.0));
}
