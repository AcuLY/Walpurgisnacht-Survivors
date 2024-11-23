#ifndef MAP_H
#define MAP_H

#include <QPainter>
#include <QWidget>

#include "direction.h"

const int MAP_X = 100;
const int MAP_Y = 100;

const int MAP_WIDTH = 400;
const int MAP_HEIGHT = 400;

const double FRICTION = 1;

class Map : public QWidget {
    Q_OBJECT

private:
    int width, height;
    double friction;

public:
    explicit Map(int width, int height, double friction, QWidget *parent = nullptr);

    std::pair<int, int> getSize() const;
    int getWidth() const;
    int getHeight() const;
    double getFriction() const;

    Direction isOutOfBoundry(const QRect &characterBounds);

protected:
    void paintEvent(QPaintEvent *event) override;
};

#endif // MAP_H
