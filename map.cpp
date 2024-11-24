#include "map.h"

Map::Map(double friction, QWidget *parent) : QWidget{parent}, friction(friction) {
    setFixedSize(MAP_WIDTH, MAP_HEIGHT);
};

double Map::getFriction() const {
    return friction;
}

void Map::render(QPainter *painter, int viewportX, int viewportY) const {
    painter->setPen(Qt::black);
    painter->setBrush(Qt::gray);

    int startX = viewportX - viewportX % GRID_SIZE - GRID_SIZE;
    int endX = startX + MAP_WIDTH + GRID_SIZE;
    int startY = viewportY - viewportY % GRID_SIZE - GRID_SIZE;
    int endY = startY + MAP_HEIGHT + GRID_SIZE;

    for (int x = startX; x <= endX; x += GRID_SIZE) {
        for (int y = startY; y <= endY; y += GRID_SIZE) {
            QRect grid(x, y, GRID_SIZE, GRID_SIZE);
            painter->drawRect(grid);
        }
    }
}
