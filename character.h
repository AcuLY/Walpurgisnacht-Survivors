#ifndef CHARACTER_H
#define CHARACTER_H

#include <QPainter>
#include <QString>
#include <QWidget>
#include <QtMath>

#include "map.h"
#include "weapon.h"

const int REBOUND_PADDING = 0;

const int RECEIVE_DAMAGE_DISPLAY_INTERVAL = 128;

class Character : public QWidget {
    Q_OBJECT

protected:
    QPixmap texture;
    QPixmap textureHurt;

    QString name;

    QPointF moveAccumulator; // 计算小数部分的位移

    int width;
    int height;

    double currentHealth;
    double maxHealth;

    double maxVelocity;
    double accelerationFactor;
    double attackMoveDecayFactor;
    double reboundFactor;
    bool isBlocked = false;

    QPointF velocity = QPointF(0, 0);
    QPointF acceleration = QPointF(0, 0);
    double facingDegree = 0;

    bool isAttacking = false;
    bool isReceivingDamage = false;

    QTimer *receiveDamageReceiveTimer;

    Weapon *weapon;

public:
    explicit Character(QString name,
                       QString texturePath,
                       QString texturePathHurt,
                       int width,
                       int height,
                       double maxHealth,
                       double maxVelocity,
                       double accelerationFactor,
                       double attackMoveDecayFactor,
                       double reboundFactor,
                       Weapon *weapon,
                       QWidget *parent = nullptr);
    ~Character();

    void render(QPainter *painter);

    QString getName() const;
    double getHealth() const;
    double getMaxHealth() const;
    QPoint getPos() const;
    double getComposedVelocity() const;
    double getMaxVelocity() const;
    AttackRange *getRange() const;
    double getFacingDegree() const;
    bool getAttacking() const;
    Weapon *getWeapon() const;
    Weapon::WeaponType getWeaponType() const;
    bool getIsReceivingDamage() const;

    void setAttacking();

    void updateAcceleration(BiDirection moveX, BiDirection moveY);
    void updateVelocity();
    void updatePosition();
    void applyFriction(double friction);

    void moveActively(Direction dir);
    void stop();

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
    void damageReceived();

public slots:
};

#endif // CHARACTER_H
