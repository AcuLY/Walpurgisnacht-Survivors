#include "map.h"

Map::Map(int width, int height, double friction, QWidget *parent)
    : QWidget{parent}, width(width), height(height), friction(friction) {
    setFixedSize(width, height);
};

std::pair<int, int> Map::getSize() const {
    return {width, height};
}

int Map::getWidth() const {
    return width;
}

int Map::getHeight() const {
    return height;
}

double Map::getFriction() const {
    return friction;
}

Direction Map::isOutOfBoundry(const QRect &characterBounds) {
    if (characterBounds.left() < this->x()) {
        return Direction::West;
    }
    if (characterBounds.right() > this->x() + width) {
        return Direction::East;
    }
    if (characterBounds.top() < this->y()) {
        return Direction::North;
    }
    if (characterBounds.bottom() > this->y() + height) {
        return Direction::South;
    }
    return Direction::Center;
}

void Map::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);

    painter.setPen(Qt::black);
    painter.setBrush(Qt::yellow);

    painter.drawRect(this->geometry());
}
