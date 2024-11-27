#include "map.h"

Map::Map(double friction, QWidget *parent) : QWidget{parent}, friction(friction) {
    setFixedSize(MAP_WIDTH, MAP_HEIGHT);
    pn = new PerlinNoise(QDateTime::currentSecsSinceEpoch());

    obstacleCache = QVector<QVector<bool>>(CACHE_HEIGHT / GRID_SIZE,
                                           QVector<bool>(CACHE_WIDTH / GRID_SIZE));
    updateObstacle(QPoint(0, 0));
}

Map::~Map() {
    delete pn;
}

bool Map::isObstacle(int x, int y) {
    // 初始的一圈无障碍
    if (abs(x) < INIT_WIDHT && abs(y) < INIT_HEIGHT) {
        return false;
    }

    double noise = pn->noise(x * NOISE_SCALE, y * NOISE_SCALE, NOISE_SCALE);
    return noise < OBSTACLE_PROPORTION;
}

double Map::getFriction() const {
    return friction;
}

void Map::render(QPainter *painter, QPoint viewpoint) const {
    painter->setPen(Qt::black);
    painter->setBrush(Qt::gray);

    int gridX = viewpoint.x() - viewpoint.x() % GRID_SIZE - GRID_SIZE,
        gridY = viewpoint.y() - viewpoint.y() % GRID_SIZE - GRID_SIZE;

    // 取 +1 以渲染右边缘和下边缘
    for (int i = 0; i <= MAP_WIDTH / GRID_SIZE + 1; i++) {
        for (int j = 0; j <= MAP_HEIGHT / GRID_SIZE + 1; j++) {
            int xIndex = (gridX - lastViewPoint.x() + CACHE_WIDTH / 2) / GRID_SIZE + i;
            int yIndex = (gridY - lastViewPoint.y() + CACHE_HEIGHT / 2) / GRID_SIZE + j;

            if (!obstacleCache[yIndex][xIndex]) {
                painter->setBrush(Qt::green);
            } else {
                painter->setBrush(Qt::gray);
            }
            QRect grid(gridX + i * GRID_SIZE, gridY + j * GRID_SIZE, GRID_SIZE, GRID_SIZE);
            painter->drawRect(grid);
        }
    }
}

bool Map::getObstacle(QPoint pos) {
    int gridX = pos.x() - pos.x() % GRID_SIZE, gridY = pos.y() - pos.y() % GRID_SIZE;
    int xIndex = (gridX - lastViewPoint.x() + CACHE_WIDTH / 2) / GRID_SIZE;
    int yIndex = (gridY - lastViewPoint.y() + CACHE_HEIGHT / 2) / GRID_SIZE;
    return obstacleCache[yIndex][xIndex];
}

void Map::updateObstacle(QPoint viewpoint) {
    int safeWidth = MAP_WIDTH * (CACHE_MAGNIFICATION - 2) / 2,
        safeHeight = MAP_HEIGHT * (CACHE_MAGNIFICATION - 2) / 2;
    QRect safeRange(lastViewPoint.x() - safeWidth / 2,
                    lastViewPoint.y() - safeHeight / 2,
                    safeWidth,
                    safeHeight);
    if (safeRange.contains(viewpoint)) {
        return;
    }

    lastViewPoint = viewpoint;

    int startX = viewpoint.x() - viewpoint.x() % GRID_SIZE - CACHE_WIDTH / 2;
    int startY = viewpoint.y() - viewpoint.y() % GRID_SIZE - CACHE_HEIGHT / 2;

    for (int i = 0; i < CACHE_WIDTH / GRID_SIZE; i++) {
        for (int j = 0; j < CACHE_HEIGHT / GRID_SIZE; j++) {
            obstacleCache[j][i] = isObstacle(startX + i * GRID_SIZE, startY + j * GRID_SIZE);
        }
    }
}
