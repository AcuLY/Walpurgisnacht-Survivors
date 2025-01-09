#include "utils.h"

QPointF MathUtils::velocityDecomQPointF(double velocity, double degree) {
    double vX = velocity * cos(degree);
    double vY = velocity * sin(degree);
    return QPointF(vX, -vY); // y 轴数值是反的
}

double MathUtils::calculateDegree(QPointF pos1, QPointF pos2) {
    double dx = pos2.x() - pos1.x();
    double dy = pos1.y() - pos2.y();
    return atan2(dy, dx);
}

double MathUtils::euclideanDistance(QPointF pos1, QPointF pos2) {
    return std::sqrt(std::pow(pos1.x() - pos2.x(), 2.0) + std::pow(pos1.y() - pos2.y(), 2.0));
}

QJsonArray FileUtils::loadJsonFile(const QString &path) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open file:" << path;

        return QJsonArray();
    }

    QByteArray fileData = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(fileData);
    return doc.array();
}
