#include "map.h"
#include "utils.h"

Map::Map(double friction, QWidget *parent) : QWidget{parent}, friction(friction) {
    pn = new PerlinNoise(QDateTime::currentSecsSinceEpoch());

    obstacleCache = QVector<QVector<bool>>(CACHE_ROW, QVector<bool>(CACHE_COL));
    obstaclePaddingCache = QVector<QVector<bool>>(CACHE_ROW, QVector<bool>(CACHE_COL));

    integrationField = QVector<QVector<int>>(CACHE_ROW, QVector<int>(CACHE_COL));
    flowField = QVector<QVector<Direction>>(CACHE_ROW,
                                            QVector<Direction>(CACHE_COL, Direction::Center));

    textureIndices = QVector<QVector<int>>(CACHE_ROW, QVector<int>(CACHE_COL));

    for (int i = 1; i <= wallTextureTypes; i++) {
        QString path = QString(":/images/map/wall/res/images/map/wall/wall_%1.png").arg(i);
        wallTileTextures.append(QPixmap(path));
    }
    for (int i = 1; i <= groundTextureTypes; i++) {
        QString path = QString(":/images/map/ground/res/images/map/ground/ground_%1.png").arg(i);
        groundTileTextures.append(QPixmap(path));
    }
}

Map::~Map() {
    delete pn;
}

QPoint Map::getGridCornerPos(QPoint pos) const {
    return pos - getOffset(pos);
}

int Map::setTextureIndex(int x, int y, int typeNum) {
    double noise = pn->noise(x * TEXTURE_NOISE_SCALE, y * TEXTURE_NOISE_SCALE, TEXTURE_NOISE_SCALE);
    return static_cast<int>(noise * typeNum) % typeNum;
}

QPoint Map::getOffset(const QPoint &pos) const {
    int offsetX = (pos.x() % GRID_SIZE + GRID_SIZE) % GRID_SIZE;
    int offsetY = (pos.y() % GRID_SIZE + GRID_SIZE) % GRID_SIZE;
    return QPoint(offsetX, offsetY);
}

QPoint Map::getIndex(const QPoint &pos) const {
    QPoint offset = getOffset(pos);
    QPoint grid = pos - offset;
    int xIndex = (grid.x() - lastViewPort.x() + CACHE_WIDTH / 2) / GRID_SIZE;
    int yIndex = (grid.y() - lastViewPort.y() + CACHE_HEIGHT / 2) / GRID_SIZE;

    // 边界检查
    xIndex = qBound(0, xIndex, CACHE_COL - 1);
    yIndex = qBound(0, yIndex, CACHE_ROW - 1);

    return QPoint(xIndex, yIndex);
}

bool Map::setObstacle(int x, int y) {
    // 初始的一圈无障碍
    if (abs(x) < INIT_WIDHT && abs(y) < INIT_HEIGHT) {
        return false;
    }

    double noise = pn->noise(x * OBSTACLE_NOISE_SCALE,
                             y * OBSTACLE_NOISE_SCALE,
                             OBSTACLE_NOISE_SCALE);
    return noise < OBSTACLE_PROPORTION;
}

void Map::updateIntegrationField(const QPoint &targetPos) {
    for (int j = 0; j < CACHE_ROW; j++) {
        for (int i = 0; i < CACHE_COL; i++) {
            integrationField[j][i] = -1;
        }
    }

    QQueue<QPoint> queue;

    QPoint targetIndex = getIndex(targetPos);
    integrationField[targetIndex.y()][targetIndex.x()] = 0;

    queue.enqueue(targetPos);

    while (!queue.empty()) {
        QPoint curPos = queue.dequeue();
        QPoint curIndex = getIndex(curPos);
        int curCost = integrationField[curIndex.y()][curIndex.x()];

        for (auto &dir : fourDirections) {
            auto [dx, dy] = ~dir;

            QPoint neighborPos = curPos + QPoint(dx * GRID_SIZE, dy * GRID_SIZE);
            QPoint neighborIndex = getIndex(neighborPos);

            if (neighborIndex.x() < 0 || neighborIndex.x() >= CACHE_COL || neighborIndex.y() < 0
                || neighborIndex.y() >= CACHE_ROW) {
                continue;
            }

            if (isObstacle(neighborPos)
                || integrationField[neighborIndex.y()][neighborIndex.x()] != -1) {
                continue;
            }

            if (isObstaclePadding(neighborPos)) {
                integrationField[neighborIndex.y()][neighborIndex.x()] = INF;
                continue;
            }

            integrationField[neighborIndex.y()][neighborIndex.x()] = curCost + 1;
            queue.enqueue(neighborPos);
        }
    }
}

void Map::updateFlowField(const QPoint &targetPos) {
    updateIntegrationField(targetPos);

    for (int j = 0; j < CACHE_ROW; j++) {
        for (int i = 0; i < CACHE_COL; i++) {
            if (integrationField[j][i] == -1) {
                flowField[j][i] = Direction::Center;
                continue;
            }

            int minCost = INF;
            Direction optimalDir = Direction::Center;

            for (auto &dir : eightDirections) {
                auto [dx, dy] = ~dir;
                QPoint neighborIndex = QPoint(i + dx, j + dy);

                if (neighborIndex.x() < 0 || neighborIndex.x() >= CACHE_COL || neighborIndex.y() < 0
                    || neighborIndex.y() >= CACHE_ROW) {
                    continue;
                }

                int neighborCost = integrationField[neighborIndex.y()][neighborIndex.x()];
                if (neighborCost >= 0 && neighborCost < minCost) {
                    minCost = neighborCost;
                    optimalDir = dir;
                }
            }

            flowField[j][i] = optimalDir;
        }
    }
}

Direction Map::getFlow(const QPoint &pos) const {
    QPoint index = getIndex(pos);
    return flowField[index.y()][index.x()];
}

double Map::getFriction() const {
    return friction;
}

void Map::render(QPainter *painter, const QPoint &viewport) const {
    int gridX = viewport.x() - viewport.x() % GRID_SIZE - GRID_SIZE;
    int gridY = viewport.y() - viewport.y() % GRID_SIZE - GRID_SIZE;

    // 取 +1 以渲染右边缘和下边缘
    for (int j = 0; j <= MAP_HEIGHT / GRID_SIZE + 1; j++) {
        for (int i = 0; i <= MAP_WIDTH / GRID_SIZE + 1; i++) {
            QPoint index = getIndex(QPoint(gridX + i * GRID_SIZE, gridY + j * GRID_SIZE));
            QRect grid(gridX + i * GRID_SIZE, gridY + j * GRID_SIZE, GRID_SIZE, GRID_SIZE);

            int textureIndex = textureIndices[index.y()][index.x()];

            if (!obstacleCache[index.y()][index.x()]) {
                painter->drawPixmap(grid, groundTileTextures[textureIndex]);
            } else {
                painter->drawPixmap(grid, wallTileTextures[textureIndex]);
            }
        }
    }

    painter->drawPixmap(-160, 0, QPixmap(":/images/ui/hint"));
}

bool Map::isOutOfBoundry(const QPoint &pos) const {
    QRect validRange(lastViewPort.x() - CACHE_WIDTH / 2,
                     lastViewPort.y() - CACHE_HEIGHT / 2,
                     CACHE_WIDTH,
                     CACHE_HEIGHT);
    return !validRange.contains(pos);
}

bool Map::isObstacle(const QPoint &pos) const {
    QPoint index = getIndex(pos);
    return obstacleCache[index.y()][index.x()];
}

bool Map::isObstaclePadding(const QPoint &pos) const {
    QPoint index = getIndex(pos);
    return obstaclePaddingCache[index.y()][index.x()];
}

bool Map::isOnlyObstacle(const QPoint &pos) const {
    return isObstacle(pos) && !isObstaclePadding(pos);
}

bool Map::isOnlyPadding(const QPoint &pos) const {
    return !isObstacle(pos) && isObstaclePadding(pos);
}

void Map::updateObstacleAndTextureIndex(const QPoint &viewport) {
    int safeWidth = MAP_WIDTH * (CACHE_MAGNIFICATION - 2) / 2,
        safeHeight = MAP_HEIGHT * (CACHE_MAGNIFICATION - 2) / 2;
    QRect safeRange(lastViewPort.x() - safeWidth / 2,
                    lastViewPort.y() - safeHeight / 2,
                    safeWidth,
                    safeHeight);
    if (safeRange.contains(viewport)) {
        return;
    }

    // 清空 padding
    obstaclePaddingCache = QVector<QVector<bool>>(CACHE_ROW, QVector<bool>(CACHE_COL, false));

    lastViewPort = viewport;

    QPoint offset = getOffset(viewport);
    int startX = viewport.x() - offset.x() - CACHE_WIDTH / 2;
    int startY = viewport.y() - offset.y() - CACHE_HEIGHT / 2;

    for (int j = 0; j < CACHE_ROW; j++) {
        for (int i = 0; i < CACHE_COL; i++) {
            bool obstacle = setObstacle(startX + i * GRID_SIZE, startY + j * GRID_SIZE);
            obstacleCache[j][i] = obstacle;
            if (!obstacle) {
                textureIndices[j][i] = setTextureIndex(startX + i * GRID_SIZE,
                                                       startY + j * GRID_SIZE,
                                                       groundTextureTypes);
                continue;
            } else {
                textureIndices[j][i] = setTextureIndex(startX + i * GRID_SIZE,
                                                       startY + j * GRID_SIZE,
                                                       wallTextureTypes);
            }

            obstacleCache[j][i] = true;
            for (auto &dir : fourDirections) {
                auto [dx, dy] = ~dir;
                if (i + dx < 0 || i + dx >= CACHE_COL || j + dy < 0 || j + dy >= CACHE_ROW) {
                    continue;
                }
                obstaclePaddingCache[j + dy][i + dx] = true;
            }
        }
    }

    updateFlowField(viewport);
}

QPainterPath Map::getPartialPath(const QPoint start, const QPoint end) {
    QPainterPath path;

    QPoint startOffset = getOffset(start), endOffset = getOffset(end);
    QPoint startGrid = start - startOffset, endGrid = end - endOffset;

    int dx = std::abs(startGrid.x() - endGrid.x()), dy = std::abs(startGrid.y() - endGrid.y());
    int err = dx - dy;

    int stepX = (end - start).x() > 0 ? GRID_SIZE : -GRID_SIZE;
    int stepY = (end - start).y() > 0 ? GRID_SIZE : -GRID_SIZE;

    QPoint cur = startGrid;
    // 使用 Bresenham 算法计算两格之间是否有障碍物
    while (true) {
        if (isObstacle(cur)) {
            path.addRect(cur.x(), cur.y(), GRID_SIZE, GRID_SIZE);
        }

        if (cur == endGrid) {
            break;
        }

        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            cur.setX(cur.x() + stepX);
        }
        if (e2 < dx) {
            err += dx;
            cur.setY(cur.y() + stepY);
        }
    }

    return path;
}

FlowFieldWorker::FlowFieldWorker(Map *map, const QPoint &targetPos)
    : map(map), targetPos(targetPos) {
}

void FlowFieldWorker::updateFlowField() {
    map->updateFlowField(targetPos);
    emit flowFieldUpdated();
}
