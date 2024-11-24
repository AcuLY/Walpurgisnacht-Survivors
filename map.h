#ifndef MAP_H
#define MAP_H

#include <QPainter>
#include <QRandomGenerator>
#include <QWidget>

#include "direction.h"

const int GRID_SIZE = 40;

const int MAP_WIDTH = 1280;
const int MAP_HEIGHT = 720;

const double FRICTION = 1;

class Map : public QWidget {
    Q_OBJECT

private:
    double friction;

public:
    explicit Map(double friction, QWidget *parent = nullptr);

    double getFriction() const;

    void render(QPainter *painter, int viewportX, int viewportY) const;
};

#endif // MAP_H
