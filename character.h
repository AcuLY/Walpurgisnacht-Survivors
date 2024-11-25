#ifndef CHARACTER_H
#define CHARACTER_H

#include <QPainter>
#include <QString>
#include <QWidget>

#include "map.h"
#include "weapon.h"

class Character : public QWidget {
    Q_OBJECT

protected:
    QString name;

    int width;
    int height;

    double health;

    double maxVelocity;
    double accelerationFactor;
    double reboundFactor;

    double velocityX = 0.0, velocityY = 0.0;
    double accelerationX = 0.0, accelerationY = 0.0;

    Weapon *weapon;

public:
    explicit Character(QString name,
                       int width,
                       int height,
                       double health,
                       double maxVelocity,
                       double accelerationFactor,
                       double reboundFactor,
                       Weapon *weapon,
                       QWidget *parent = nullptr);

    QString getName() const;
    double getHealth() const;
    double getDamage() const;
    double getVelocity() const;
    Weapon *getWeapon() const;

    void updateAcceleration(BiDirection moveX, BiDirection moveY);
    void updateVelocity();
    void updateQPointF();
    void applyFriction(double friction);

    void moveActively(Direction dir);
    void handleCollision(Character *other);

    Bullet *regularAttack(double degree);
};

#endif // CHARACTER_H
