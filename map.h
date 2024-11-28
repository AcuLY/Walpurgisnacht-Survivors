#ifndef MAP_H
#define MAP_H

#include <QDateTime>
#include <QPainter>
#include <QRandomGenerator>
#include <QVector>
#include <QWidget>

#include "./third_party/perlinnoise.h"
#include "direction.h"

const int GRID_SIZE = 40;

const int MAP_WIDTH = 1280;
const int MAP_HEIGHT = 720;

const int INIT_WIDHT = 160;
const int INIT_HEIGHT = 160;

const int CACHE_MAGNIFICATION = 7;
const int CACHE_WIDTH = MAP_WIDTH * CACHE_MAGNIFICATION;
const int CACHE_HEIGHT = MAP_HEIGHT * CACHE_MAGNIFICATION;

const double FRICTION = 0.2;

const double NOISE_SCALE = 0.004;
const double OBSTACLE_PROPORTION = 0.35;

class Map : public QWidget {
    Q_OBJECT

private:
    QVector<QVector<bool>> obstacleCache;
    double friction;

    PerlinNoise *pn;
    QPointF lastViewPoint = QPoint(99999, 99999); // 设一个比较大的初值, 使初始化的时候可以生成

    bool setObstacle(int x, int y);

public:
    explicit Map(double friction, QWidget *parent = nullptr);
    ~Map();

    double getFriction() const;

    void render(QPainter *painter, QPoint &pos) const;

    bool isObstacle(QPoint &pos);
    void updateObstacle(QPoint &viewpoint);

    QPainterPath getWholePath();
};

#endif // MAP_H
