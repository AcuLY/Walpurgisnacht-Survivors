#ifndef ATTACK_H
#define ATTACK_H

#include <QPainter>
#include <QTimer>
#include <QWidget>

#include "attackrange.h"

class Attack : public QWidget {
    Q_OBJECT

protected:
    QPoint pos;
    double size;
    AttackRange *range;
    double damage;
    bool isPlayerSide;
    bool isValid = true;
    QTimer validityTimer;

public:
    explicit Attack(QPoint initPos,
                    double size,
                    double damage,
                    bool isPlayerSide,
                    int validTime,
                    QWidget *parent = nullptr);
    ~Attack();

    QPoint getPos() const;
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

    QPoint prevPos;

public:
    explicit Bullet(QPoint initPos,
                    double size,
                    double damage,
                    bool isPlayerSide,
                    int validTime,
                    QWidget *parent = nullptr);

    QPoint &getPrevPos();

    void setAcceleration(QPointF a);
    void setVelocity(QPointF v);

    void moveActively();

    bool isHit(const QRectF &targetRect);
    bool isHit(const QPainterPath &path);
};

class Slash : public Attack {
public:
    explicit Slash(QPoint pos,
                   double size,
                   double startAngle,
                   double spanAngle,
                   double damage,
                   bool isPlayerSide,
                   int validTime,
                   QWidget *parent = nullptr);

    bool isHit(const QRectF &targetRect);
};

#endif // ATTACK_H
