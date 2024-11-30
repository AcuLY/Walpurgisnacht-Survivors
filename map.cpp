#include "map.h"
#include "utils.h"

Map::Map(double friction, QWidget *parent) : QWidget{parent}, friction(friction) {
    setFixedSize(MAP_WIDTH, MAP_HEIGHT);
    pn = new PerlinNoise(QDateTime::currentSecsSinceEpoch());

    obstacleCache = QVector<QVector<bool>>(CACHE_ROW, QVector<bool>(CACHE_COL));
    obstaclePaddingCache = QVector<QVector<bool>>(CACHE_ROW, QVector<bool>(CACHE_COL));

    integrationField = QVector<QVector<int>>(CACHE_ROW, QVector<int>(CACHE_COL));
    flowField = QVector<QVector<Direction>>(CACHE_ROW,
                                            QVector<Direction>(CACHE_COL, Direction::Center));
}

Map::~Map() {
    delete pn;
}

QPoint Map::getOffset(const QPoint &pos) const {
    int offsetX = (pos.x() % GRID_SIZE + GRID_SIZE) % GRID_SIZE;
    int offsetY = (pos.y() % GRID_SIZE + GRID_SIZE) % GRID_SIZE;
    return QPoint(offsetX, offsetY);
}

QPoint Map::getIndex(const QPoint &pos) const {
    QPoint offset = getOffset(pos);
    QPoint grid = pos - offset;
    int xIndex = (grid.x() - lastViewPoint.x() + CACHE_WIDTH / 2) / GRID_SIZE;
    int yIndex = (grid.y() - lastViewPoint.y() + CACHE_HEIGHT / 2) / GRID_SIZE;

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

    double noise = pn->noise(x * NOISE_SCALE, y * NOISE_SCALE, NOISE_SCALE);
    return noise < OBSTACLE_PROPORTION;
}

void Map::updateIntegrationField(const QPoint &targetPos) {
    for (int i = 0; i < CACHE_COL; i++) {
        for (int j = 0; j < CACHE_ROW; j++) {
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

    for (int i = 0; i < CACHE_COL; i++) {
        for (int j = 0; j < CACHE_ROW; j++) {
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

void Map::render(QPainter *painter, const QPoint &viewpoint) const {
    painter->setPen(Qt::black);
    painter->setBrush(Qt::green);

    int gridX = viewpoint.x() - viewpoint.x() % GRID_SIZE - GRID_SIZE;
    int gridY = viewpoint.y() - viewpoint.y() % GRID_SIZE - GRID_SIZE;

    painter->drawRect(QRect(gridX, gridY, MAP_WIDTH + GRID_SIZE * 10, MAP_HEIGHT * 10));
    painter->setBrush(Qt::gray);

    // 取 +1 以渲染右边缘和下边缘
    for (int i = 0; i <= MAP_WIDTH / GRID_SIZE + 1; i++) {
        for (int j = 0; j <= MAP_HEIGHT / GRID_SIZE + 1; j++) {
            QPoint index = getIndex(QPoint(gridX + i * GRID_SIZE, gridY + j * GRID_SIZE));
            if (!obstacleCache[index.y()][index.x()]) {
                continue;
            }

            QRect grid(gridX + i * GRID_SIZE, gridY + j * GRID_SIZE, GRID_SIZE, GRID_SIZE);
            painter->drawRect(grid);

            // QString coordinates
            //     = QString("(%1, %2)").arg(gridX + i * GRID_SIZE).arg(gridY + j * GRID_SIZE);

            // QFont smallFont = painter->font();
            // smallFont.setPointSize(6); // 设置字体大小为 6（可以根据需要调整）

            // painter->setFont(smallFont);

            // QPoint Findex = getIndex(QPoint(gridX + i * GRID_SIZE, gridY + j * GRID_SIZE));
            // QString coordinates = QString("");
            // if (flowField[Findex.y()][Findex.x()] == Direction::North) {
            //     coordinates = QString("↑");
            // }
            // if (flowField[Findex.y()][Findex.x()] == Direction::NorthEast) {
            //     coordinates = QString("↗");
            // }
            // if (flowField[Findex.y()][Findex.x()] == Direction::East) {
            //     coordinates = QString("→");
            // }
            // if (flowField[Findex.y()][Findex.x()] == Direction::SouthEast) {
            //     coordinates = QString("↘");
            // }
            // if (flowField[Findex.y()][Findex.x()] == Direction::South) {
            //     coordinates = QString("↓");
            // }
            // if (flowField[Findex.y()][Findex.x()] == Direction::SouthWest) {
            //     coordinates = QString("↙");
            // }
            // if (flowField[Findex.y()][Findex.x()] == Direction::West) {
            //     coordinates = QString("←");
            // }
            // if (flowField[Findex.y()][Findex.x()] == Direction::NorthWest) {
            //     coordinates = QString("↖");
            // }
            // painter->drawText(grid, Qt::AlignCenter, coordinates);
        }
    }
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

void Map::updateObstacle(const QPoint &viewpoint) {
    int safeWidth = MAP_WIDTH * (CACHE_MAGNIFICATION - 2) / 2,
        safeHeight = MAP_HEIGHT * (CACHE_MAGNIFICATION - 2) / 2;
    QRect safeRange(lastViewPoint.x() - safeWidth / 2,
                    lastViewPoint.y() - safeHeight / 2,
                    safeWidth,
                    safeHeight);
    if (safeRange.contains(viewpoint)) {
        return;
    }

    // 清空 padding
    obstaclePaddingCache = QVector<QVector<bool>>(CACHE_ROW, QVector<bool>(CACHE_COL, false));

    lastViewPoint = viewpoint;

    QPoint offset = getOffset(viewpoint);
    int startX = viewpoint.x() - offset.x() - CACHE_WIDTH / 2;
    int startY = viewpoint.y() - offset.y() - CACHE_HEIGHT / 2;

    for (int i = 0; i < CACHE_COL; i++) {
        for (int j = 0; j < CACHE_ROW; j++) {
            bool obstacle = setObstacle(startX + i * GRID_SIZE, startY + j * GRID_SIZE);
            obstacleCache[j][i] = obstacle;
            if (!obstacle) {
                continue;
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

    updateFlowField(viewpoint);
}

QPainterPath Map::getPartialPath(const QPoint begin, const QPoint end) {
    QPainterPath path;

    QPoint beginOffset = getOffset(begin), endOffset = getOffset(end);

    int stepX = (end - begin).x() > 0 ? GRID_SIZE : -GRID_SIZE;
    int stepY = (end - begin).y() > 0 ? GRID_SIZE : -GRID_SIZE;

    int endX = (end - endOffset).x();
    int endY = (end - endOffset).y();

    for (int x = (begin - beginOffset).x(); (stepX > 0 ? x <= endX : x >= endX); x += stepX) {
        for (int y = (begin - beginOffset).y(); (stepY > 0 ? y <= endY : y >= endY); y += stepY) {
            if (isObstacle(QPoint(x, y))) {
                path.addRect(x, y, GRID_SIZE, GRID_SIZE);
            }
        }
    }

    return path;
}
