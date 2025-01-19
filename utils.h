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

QPointF velocityDecomQPointF(double velocity, double degree); // 将速度正交分解

double calculateDegree(QPointF pos1, QPointF pos2); // 计算 2 到 1 的角度

double euclideanDistance(QPointF pos1, QPointF pos2); // 计算直线距离

} // namespace MathUtils

namespace FileUtils {

QJsonArray loadJsonFile(const QString &path); // 从一个 json 列表加载数据

} // namespace FileUtils

#endif // UTILS_H
