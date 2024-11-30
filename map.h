#ifndef MAP_H
#define MAP_H

#include <QDateTime>
#include <QPainter>
#include <QPainterPath>
#include <QQueue>
#include <QRandomGenerator>
#include <QVector>
#include <QWidget>

#include "./third_party/perlinnoise.h"
#include "direction.h"

const int GRID_SIZE = 20;
const int FLOW_GRID_SIZE = 20;

const int MAP_WIDTH = 1280;
const int MAP_HEIGHT = 720;

const int INIT_WIDHT = 160;
const int INIT_HEIGHT = 160;

const int CACHE_MAGNIFICATION = 7;
const int CACHE_WIDTH = MAP_WIDTH * CACHE_MAGNIFICATION;
const int CACHE_HEIGHT = MAP_HEIGHT * CACHE_MAGNIFICATION;

const int FLOW_MAGNIFICATION = 7;
const int FLOW_WIDTH = MAP_WIDTH * FLOW_MAGNIFICATION;
const int FLOW_HEIGHT = MAP_HEIGHT * FLOW_MAGNIFICATION;

const double FRICTION = 0.2;

const double NOISE_SCALE = 0.006;
const double OBSTACLE_PROPORTION = 0.35;

class Map : public QWidget {
    Q_OBJECT

private:
    QVector<QVector<bool>> obstacleCache;
    QVector<QVector<bool>> obstaclePaddingCache;
    QVector<QVector<int>> integrationField;
    QVector<QVector<Direction>> flowField;
    double friction;

    PerlinNoise *pn;
    QPoint lastViewPoint = QPoint(99999, 99999); // 设一个比较大的初值, 使初始化的时候可以生成

    QPoint getOffset(const QPoint &pos) const;
    QPoint getFlowFieldOffset(const QPoint &pos) const;
    QPoint getIndex(const QPoint &pos) const;
    QPoint getFlowFieldIndex(const QPoint &pos) const;

    bool setObstacle(int x, int y);
    void updateIntegrationField(const QPoint &targetPos);

public:
    explicit Map(double friction, QWidget *parent = nullptr);
    ~Map();

    double getFriction() const;

    void render(QPainter *painter, const QPoint &pos) const;

    bool isObstacle(const QPoint &pos) const;
    bool isObstaclePadding(const QPoint &pos) const;
    bool isOnlyObstacle(const QPoint &pos) const;
    bool isOnlyPadding(const QPoint &pos) const;
    void updateObstacle(const QPoint &viewpoint);

    QPainterPath getPartialPath(const QPoint begin, const QPoint end);

    void updateFlowField(const QPoint &targetPos);
    Direction getFlow(const QPoint &pos) const;
};

#endif // MAP_H
