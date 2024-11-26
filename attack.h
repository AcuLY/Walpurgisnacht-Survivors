#ifndef ATTACK_H
#define ATTACK_H

#include <QTimer>
#include <QWidget>

#include "attackrange.h"

class Attack : public QWidget {
    Q_OBJECT

protected:
    QPointF pos;
    double size;
    AttackRange *range;
    double damage;
    bool isPlayerSide;
    bool isValid = true;
    QTimer validityTimer;

public:
    explicit Attack(QPointF initPos,
                    double size,
                    double damage,
                    bool isPlayerSide,
                    int validTime,
                    QWidget *parent = nullptr);
    ~Attack();

    QPointF getPos() const;
    AttackRange *getRange() const;
    double getDamage() const;
    double getPlayerSide() const;
    void setPlayerSide();
    double getValidity() const;

    QPainterPath createPath() const;
};

class Bullet : public Attack {
    Q_OBJECT

protected:
    QPointF velocity;
    QPointF acceleration = QPointF(0, 0);

    QPointF prevPos;

public:
    explicit Bullet(QPointF initPos,
                    double size,
                    double damage,
                    bool isPlayerSide,
                    int validTime,
                    QWidget *parent = nullptr);

    void setAcceleration(QPointF a);
    void setVelocity(QPointF v);

    void moveActively();

    bool isHit(const QRectF &targetRect);

signals:
};

#endif // ATTACK_H
