#ifndef UTILS_H
#define UTILS_H

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QPointF>

#include <cmath>

const double INF = 1e9;

namespace MathUtils {

// 将速度正交分解
QPointF velocityDecomQPointF(double velocity, double degree);

double calculateDegree(QPointF pos1, QPointF pos2);

double euclideanDistance(QPointF pos1, QPointF pos2);

} // namespace MathUtils

namespace FileUtils {

QJsonArray loadJsonFile(const QString &path);

} // namespace FileUtils

#endif // UTILS_H
