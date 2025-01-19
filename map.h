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

const int GRID_SIZE = 20; // 格子宽度

const int MAP_WIDTH = 1280;
const int MAP_HEIGHT = 720;

const int INIT_WIDHT = 320;  // 初始无障碍区的宽度
const int INIT_HEIGHT = 320; // 初始无障碍区的高度

const int CACHE_MAGNIFICATION = 5; // 地图信息缓存的放大倍数
const int CACHE_WIDTH = MAP_WIDTH * CACHE_MAGNIFICATION;
const int CACHE_HEIGHT = MAP_HEIGHT * CACHE_MAGNIFICATION;
const int CACHE_COL = CACHE_WIDTH / GRID_SIZE;
const int CACHE_ROW = CACHE_HEIGHT / GRID_SIZE;

const double FRICTION = 0.2; // 地图摩擦力

const double TEXTURE_NOISE_SCALE = 0.002;  // 贴图的噪音缩放比例
const double OBSTACLE_NOISE_SCALE = 0.004; // 障碍物生成的噪音缩放比例
const double OBSTACLE_PROPORTION = 0.4;    // 障碍物的比例

const int WALL_TEXTURE_TYPES = 3;    // 障碍物贴图种类
const int GROUND_TEXTURE_TYPES = 50; // 地面贴图种类

class Map : public QWidget { // 地图类
    Q_OBJECT

private:
    QVector<QPixmap> wallTileTextures;   // 障碍物贴图
    QVector<QPixmap> groundTileTextures; // 地面贴图

    QVector<QVector<int>> textureIndices;           // 存储每个格子的贴图类型
    int setTextureIndex(int x, int y, int typeNum); // 根据坐标选择贴图

    QVector<QVector<bool>> obstacleCache;        // 存储障碍物信息
    QVector<QVector<bool>> obstaclePaddingCache; // 障碍物的周围一圈设有缓冲
    QVector<QVector<int>> integrationField;      // 积分场信息
    QVector<QVector<Direction>> flowField;       // 流场信息

    double friction; // 摩擦力

    PerlinNoise *pn; // 柏林噪声生成器

    QPoint lastViewPort = QPoint(99999, 99999); // 设一个比较大的初值, 使初始化的时候可以生成地图

    QPoint getOffset(const QPoint &pos) const; // 根据当前坐标生成到该坐标位于的格子的左上角的偏移量
    QPoint getIndex(const QPoint &pos) const; // 获取当前坐标对应 obstacleCache 等数组的下标

    bool setObstacle(int x, int y);                       // 根据坐标选择是否是障碍物
    void updateIntegrationField(const QPoint &targetPos); // 更新积分场

public:
    explicit Map(double friction, QWidget *parent = nullptr);
    ~Map();

    QPoint getGridCornerPos(QPoint pos) const; // 获得当前这格左上角的位置

    double getFriction() const;

    void render(QPainter *painter, const QPoint &pos) const;

    bool isOutOfBoundry(const QPoint &pos) const; // 当前位置是否超出了地图信息的缓存区

    bool isObstacle(const QPoint &pos) const;        // 当前坐标是否有障碍物
    bool isObstaclePadding(const QPoint &pos) const; // 当前坐标是否是障碍物或缓冲区
    bool isOnlyObstacle(const QPoint &pos) const; // 当前坐标是否只是障碍物不是缓冲区
    bool isOnlyPadding(const QPoint &pos) const; // 当前坐标是否只是缓冲区不是障碍物
    void updateObstacleAndTextureIndex(const QPoint &viewport); // 更新障碍物和贴图信息

    QPainterPath getPartialPath(const QPoint start,
                                const QPoint end); // 获取两点连线的障碍物组成的 QPainterPath

    void updateFlowField(const QPoint &targetPos); // 更新流场
    Direction getFlow(const QPoint &pos) const;    // 获取当前位置的流向
};

class FlowFieldWorker : public QObject { // 流场线程辅助类
    Q_OBJECT

private:
    Map *map;
    QPoint targetPos; // 中心位置

public:
    explicit FlowFieldWorker(Map *map, const QPoint &targetPos);

    void updateFlowField();

signals:
    void flowFieldUpdated();
};

#endif // MAP_H
