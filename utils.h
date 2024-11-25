#ifndef UTILS_H
#define UTILS_H

#include <QPointF>

#include <cmath>

const double INF = 1e9;

namespace MathUtils {

// 将速度正交分解
std::pair<double, double> velocityDecomQPointF(double velocity, double degree);

double calculateDegree(QPointF pos1, QPointF pos2);

double euclideanDistance(QPointF pos1, QPointF pos2);

} // namespace MathUtils

#endif // UTILS_H
