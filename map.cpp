#include "map.h"
#include "qpainterpath.h"

Map::Map(double friction, QWidget *parent) : QWidget{parent}, friction(friction) {
    setFixedSize(MAP_WIDTH, MAP_HEIGHT);
    pn = new PerlinNoise(QDateTime::currentSecsSinceEpoch());

    obstacleCache = QVector<QVector<bool>>(CACHE_HEIGHT / GRID_SIZE,
                                           QVector<bool>(CACHE_WIDTH / GRID_SIZE));
}

Map::~Map() {
    delete pn;
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

void Map::render(QPainter *painter, QPoint &viewpoint) const {
    painter->setPen(Qt::black);
    painter->setBrush(Qt::gray);

    int gridX = viewpoint.x() - viewpoint.x() % GRID_SIZE - GRID_SIZE;
    int gridY = viewpoint.y() - viewpoint.y() % GRID_SIZE - GRID_SIZE;

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
            QString coordinates
                = QString("(%1, %2)").arg(gridX + i * GRID_SIZE).arg(gridY + j * GRID_SIZE);

            // 设置文本对齐方式，绘制坐标
            // QFont smallFont = painter->font();
            // smallFont.setPointSize(6); // 设置字体大小为 6（可以根据需要调整）

            // painter->setFont(smallFont);
            // painter->drawText(grid, Qt::AlignCenter, coordinates);
        }
    }
}

bool Map::isObstacle(QPoint &pos) {
    int offsetX = this->x() < 0 ? this->x() % GRID_SIZE + GRID_SIZE : this->x() % GRID_SIZE;
    int offsetY = this->y() < 0 ? this->y() % GRID_SIZE + GRID_SIZE : this->y() % GRID_SIZE;
    int gridX = pos.x() - offsetX;
    int gridY = pos.y() - offsetY;
    int xIndex = (gridX - lastViewPoint.x() + CACHE_WIDTH / 2) / GRID_SIZE;
    int yIndex = (gridY - lastViewPoint.y() + CACHE_HEIGHT / 2) / GRID_SIZE;
    return obstacleCache[yIndex][xIndex];
}

void Map::updateObstacle(QPoint &viewpoint) {
    int safeWidth = MAP_WIDTH * (CACHE_MAGNIFICATION - 2) / 2,
        safeHeight = MAP_HEIGHT * (CACHE_MAGNIFICATION - 2) / 2;
    QRect safeRange(lastViewPoint.x() - safeWidth / 2,
                    lastViewPoint.y() - safeHeight / 2,
                    safeWidth,
                    safeHeight);
    if (safeRange.contains(viewpoint)) {
        return;
    }
    qDebug() << "update";
    lastViewPoint = viewpoint;

    int offsetX = viewpoint.x() < 0 ? viewpoint.x() % GRID_SIZE + GRID_SIZE
                                    : viewpoint.x() % GRID_SIZE;
    int offsetY = viewpoint.y() < 0 ? viewpoint.y() % GRID_SIZE + GRID_SIZE
                                    : viewpoint.y() % GRID_SIZE;
    int startX = viewpoint.x() - offsetX - CACHE_WIDTH / 2;
    int startY = viewpoint.y() - offsetY - CACHE_HEIGHT / 2;
    qDebug() << viewpoint << startX << startY;

    for (int i = 0; i < CACHE_WIDTH / GRID_SIZE; i++) {
        for (int j = 0; j < CACHE_HEIGHT / GRID_SIZE; j++) {
            obstacleCache[j][i] = setObstacle(startX + i * GRID_SIZE, startY + j * GRID_SIZE);
        }
    }
}

QPainterPath Map::getWholePath() {
    QPainterPath path;

    int offsetX = this->x() < 0 ? this->x() % GRID_SIZE + GRID_SIZE : this->x() % GRID_SIZE;
    int offsetY = this->y() < 0 ? this->y() % GRID_SIZE + GRID_SIZE : this->y() % GRID_SIZE;
    int startX = lastViewPoint.x() - offsetX - CACHE_WIDTH / 2;
    int startY = lastViewPoint.y() - offsetY - CACHE_HEIGHT / 2;

    for (int i = 0; i < CACHE_WIDTH / GRID_SIZE; i++) {
        for (int j = 0; j < CACHE_HEIGHT / GRID_SIZE; j++) {
            QPoint pos(startX + i * GRID_SIZE, startY + j * GRID_SIZE);
            if (isObstacle(pos)) {
                path.addRect(pos.x(), pos.y(), GRID_SIZE, GRID_SIZE);
            }
        }
    }

    return path;
}
