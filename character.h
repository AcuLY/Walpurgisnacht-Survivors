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

    void updateVelocity();
    void updatePosition(Map &map);
    void applyAcceleration(double aX, double aY);
    void applyFriction(double friction);

    void moveActively(BiDirection moveX, BiDirection moveY, Map &map);

protected:
    void paintEvent(QPaintEvent *event) override;
};

#endif // CHARACTER_H
