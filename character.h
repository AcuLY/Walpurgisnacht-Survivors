#ifndef CHARACTER_H
#define CHARACTER_H

#include <QPainter>
#include <QString>
#include <QWidget>

#include "map.h"

class Character : public QWidget {
    Q_OBJECT

protected:
    QString name;

    int width;
    int height;

    double health;
    double damage;

    double maxVelocity;
    double accelerationFactor;
    double reboundFactor;

    double velocityX = 0.0, velocityY = 0.0;
    double accelerationX = 0.0, accelerationY = 0.0;

public:
    explicit Character(QString name,
                       int width,
                       int height,
                       double health,
                       double damage,
                       double maxVelocity,
                       double accelerationFactor,
                       double reboundFactor,
                       QWidget *parent = nullptr);

    QString getName() const;
    double getHealth() const;
    double getDamage() const;
    double getVelocity() const;

    void updateAcceleration(BiDirection moveX, BiDirection moveY);
    void updateVelocity();
    void updatePosition();
    void applyFriction(double friction);

    void moveActively(Direction dir);

    void handleCollision(Character *other);
};

#endif // CHARACTER_H
