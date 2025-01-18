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

    bool isMultiAttacking = false;
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

    void setMultiAttackMode(bool isMulti);

    void increaseDamage(double value);         // 提高伤害，加法
    void decreaseAttackInterval(double value); // 提高攻速，乘法
    void increaseRangeSize(double value);      // 扩大范围，乘法

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

    void increaseBulletVelocity(double value); // 增加子弹大小，乘法
    void increaseBulletSize(double value);     // 增加子弹速度，乘法
};

class MeleeWeapon : public Weapon {
protected:
    int validTime = 16;
    double spanAngle;

    WeaponType getType() override;

public:
    explicit MeleeWeapon(double damage,
                         double attackInterval,
                         double rangeSize,
                         double spanAngle,
                         bool isPlayerSide,
                         QWidget *parent = nullptr);

    Slash *attack(QPoint pos, double degree);

    void increaseSpanAngle(double value); // 增大斩击张角，乘法
};

#endif // WEAPON_H
