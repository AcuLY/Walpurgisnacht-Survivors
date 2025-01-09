#ifndef WEAPON_H
#define WEAPON_H

#include <QTimer>
#include <QWidget>
#include <QtMath>

#include "attack.h"
#include "utils.h"

class Weapon : public QWidget {
    Q_OBJECT

protected:
    double damage;
    double attackInterval;
    double rangeSize;
    AttackRange *range;

    bool isOnCooldown = false;
    QTimer cooldownTimer;

    bool isPlayerSide;

public:
    explicit Weapon(double damage,
                    double attackInterval,
                    double rangeSize,
                    bool isPlayerSide,
                    QWidget *parent = nullptr);

    enum class WeaponType { Remote, Melee };

    double getDamage();
    double getAttackInterval();
    AttackRange *getRange();
    bool isCooldownFinished();
    bool getSide();
    virtual WeaponType getType() = 0;

    void setDamage(double newDamage);

signals:
};

class RemoteWeapon : public Weapon {
protected:
    double bulletVelocity;
    double bulletSize;

    WeaponType getType() override;

public:
    explicit RemoteWeapon(double bulletVelociy,
                          double bulletSize,
                          double damage,
                          double attackSpeed,
                          double rangeSize,
                          bool isPlayerSide,
                          QWidget *parent = nullptr);

    double getBulletSize() const;

    Bullet *attack(QPoint pos, double degree);
};

class MeleeWeapon : public Weapon {
protected:
    int validTime = 16;

    WeaponType getType() override;

public:
    explicit MeleeWeapon(double damage,
                         double attackInterval,
                         double rangeSize,
                         bool isPlayerSide,
                         QWidget *parent = nullptr);

    Slash *attack(QPoint pos, double degree);
};

#endif // WEAPON_H
