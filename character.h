#ifndef CHARACTER_H
#define CHARACTER_H

#include <QPainter>
#include <QString>
#include <QWidget>
#include <QtMath>

#include "map.h"
#include "weapon.h"

const int REBOUND_PADDING = 1;

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

    bool isAttacking = false;

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
    AttackRange *getRange() const;
    double getFacingDegree() const;
    bool getAttacking() const;

    void setAttacking();

    void updateAcceleration(BiDirection moveX, BiDirection moveY);
    void updateVelocity();
    void updatePosition();
    void applyFriction(double friction);

    void moveActively(Direction dir);

    QPair<bool, bool> handleCollision(QRect &otherRect);
    void handleCollision(Character *other);
    void handleCollision(Map *map);
    void rebound(bool x, bool y);

    void performAttack(Character *target);

    void receiveDamage(double damage);

protected:
    Attack *regularAttack(double degree);

signals:
    void attackPerformed(Attack *attack);

public slots:
};

#endif // CHARACTER_H
