#include "map.h"

Map::Map(double friction, QWidget *parent) : QWidget{parent}, friction(friction) {
    setFixedSize(MAP_WIDTH, MAP_HEIGHT);
    pn = new PerlinNoise(QDateTime::currentSecsSinceEpoch());

    obstacleCache = QVector<QVector<bool>>(CACHE_HEIGHT / GRID_SIZE,
                                           QVector<bool>(CACHE_WIDTH / GRID_SIZE));
}

Map::~Map() {
    delete pn;
}

QPoint Map::getOffset(const QPoint pos) const {
    int offsetX = pos.x() < 0 ? pos.x() % GRID_SIZE + GRID_SIZE : pos.x() % GRID_SIZE;
    int offsetY = pos.y() < 0 ? pos.y() % GRID_SIZE + GRID_SIZE : pos.y() % GRID_SIZE;
    return QPoint(offsetX, offsetY);
}

bool Map::setObstacle(int x, int y) {
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
            int xIndex = (gridX - lastViewPoint.x() + CACHE_WIDTH / 2) / GRID_SIZE + i;
            int yIndex = (gridY - lastViewPoint.y() + CACHE_HEIGHT / 2) / GRID_SIZE + j;

            if (!obstacleCache[yIndex][xIndex]) {
                continue;
            }
            QRect grid(gridX + i * GRID_SIZE, gridY + j * GRID_SIZE, GRID_SIZE, GRID_SIZE);
            painter->drawRect(grid);

            // QString coordinates
            //     = QString("(%1, %2)").arg(gridX + i * GRID_SIZE).arg(gridY + j * GRID_SIZE);

            // QFont smallFont = painter->font();
            // smallFont.setPointSize(6); // 设置字体大小为 6（可以根据需要调整）

            // painter->setFont(smallFont);
            // painter->drawText(grid, Qt::AlignCenter, coordinates);
        }
    }
}

bool Map::isObstacle(const QPoint &pos) {
    QPoint offset = getOffset(pos);
    QPoint grid = pos - offset;
    int xIndex = (grid.x() - lastViewPoint.x() + CACHE_WIDTH / 2) / GRID_SIZE;
    int yIndex = (grid.y() - lastViewPoint.y() + CACHE_HEIGHT / 2) / GRID_SIZE;
    return obstacleCache[yIndex][xIndex];
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

    lastViewPoint = viewpoint;

    QPoint offset = getOffset(viewpoint);
    int startX = viewpoint.x() - offset.x() - CACHE_WIDTH / 2;
    int startY = viewpoint.y() - offset.y() - CACHE_HEIGHT / 2;

    for (int i = 0; i < CACHE_WIDTH / GRID_SIZE; i++) {
        for (int j = 0; j < CACHE_HEIGHT / GRID_SIZE; j++) {
            obstacleCache[j][i] = setObstacle(startX + i * GRID_SIZE, startY + j * GRID_SIZE);
        }
    }
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
