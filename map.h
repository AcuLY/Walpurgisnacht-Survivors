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

const int MAP_WIDTH = 1280;
const int MAP_HEIGHT = 720;

const int INIT_WIDHT = 320;
const int INIT_HEIGHT = 320;

const int CACHE_MAGNIFICATION = 5;
const int CACHE_WIDTH = MAP_WIDTH * CACHE_MAGNIFICATION;
const int CACHE_HEIGHT = MAP_HEIGHT * CACHE_MAGNIFICATION;
const int CACHE_COL = CACHE_WIDTH / GRID_SIZE;
const int CACHE_ROW = CACHE_HEIGHT / GRID_SIZE;

const double FRICTION = 0.2;

const double TEXTURE_NOISE_SCALE = 0.002;
const double OBSTACLE_NOISE_SCALE = 0.004;
const double OBSTACLE_PROPORTION = 0.4;

const int wallTextureTypes = 3;
const int groundTextureTypes = 50;

class Map : public QWidget {
    Q_OBJECT

private:
    QVector<QPixmap> wallTileTextures;
    QVector<QPixmap> groundTileTextures;

    QVector<QVector<int>> textureIndices;
    int setTextureIndex(int x, int y, int typeNum);

    QVector<QVector<bool>> obstacleCache;
    QVector<QVector<bool>> obstaclePaddingCache;
    QVector<QVector<int>> integrationField;
    QVector<QVector<Direction>> flowField;
    double friction;

    PerlinNoise *pn;
    QPoint lastViewPort = QPoint(99999, 99999); // 设一个比较大的初值, 使初始化的时候可以生成地图

    QPoint getOffset(const QPoint &pos) const;
    QPoint getIndex(const QPoint &pos) const;

    bool setObstacle(int x, int y);
    void updateIntegrationField(const QPoint &targetPos);

public:
    explicit Map(double friction, QWidget *parent = nullptr);
    ~Map();

    QPoint getGridCornerPos(QPoint pos) const; // 获得当前这格左上角的位置

    double getFriction() const;

    void render(QPainter *painter, const QPoint &pos) const;

    bool isOutOfBoundry(const QPoint &pos) const;

    bool isObstacle(const QPoint &pos) const;
    bool isObstaclePadding(const QPoint &pos) const;
    bool isOnlyObstacle(const QPoint &pos) const;
    bool isOnlyPadding(const QPoint &pos) const;
    void updateObstacleAndTextureIndex(const QPoint &viewport);

    QPainterPath getPartialPath(const QPoint start, const QPoint end);

    void updateFlowField(const QPoint &targetPos);
    Direction getFlow(const QPoint &pos) const;
};

class FlowFieldWorker : public QObject {
    Q_OBJECT

private:
    Map *map;
    QPoint targetPos;

public:
    explicit FlowFieldWorker(Map *map, const QPoint &targetPos);

    void updateFlowField();

signals:
    void flowFieldUpdated();
};

#endif // MAP_H
