#ifndef CHARACTER_H
#define CHARACTER_H

#include <QPainter>
#include <QString>
#include <QWidget>
#include <QtMath>

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

    QPointF velocity = QPointF(0, 0);
    QPointF acceleration = QPointF(0, 0);
    double facingDegree = 0;

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
    QPointF getPos() const;
    double getVelocity() const;
    Weapon *getWeapon() const;
    double getFacingDegree() const;

    void updateAcceleration(BiDirection moveX, BiDirection moveY);
    void updateVelocity();
    void updateQPointF();
    void applyFriction(double friction);

    void moveActively(Direction dir);
    void handleCollision(Character *other);

    Attack *regularAttack(double degree);

    void receiveDamage(double damage);
};

#endif // CHARACTER_H
