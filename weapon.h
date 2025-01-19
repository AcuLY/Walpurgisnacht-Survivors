#ifndef WEAPON_H
#define WEAPON_H

#include <QTimer>
#include <QWidget>
#include <QtMath>

#include "attack.h"

class Weapon : public QWidget { // 武器抽象类
    Q_OBJECT

protected:
    double damage;         // 伤害
    double attackInterval; // 攻击冷却时间
    double rangeSize;      // 范围
    AttackRange *range;

    bool isMultiAttacking = false; // 是否正在连击
    bool isOnCooldown = false;     // 是否在冷却
    QTimer *cooldownTimer;

    bool isPlayerSide; // 是否是右方

public:
    explicit Weapon(double damage,
                    double attackInterval,
                    double rangeSize,
                    bool isPlayerSide,
                    QWidget *parent = nullptr);
    virtual ~Weapon();

    enum class WeaponType { Remote, Melee }; // 武器类型枚举

    double getDamage();
    AttackRange *getRange();
    bool isCooldownFinished();
    virtual WeaponType getType() = 0; // 武器类型

    void setMultiAttackMode(bool isMulti); // 切换连击模式

    void increaseDamage(double value);         // 提高伤害，加法
    void decreaseAttackInterval(double value); // 提高攻速，乘法
    void increaseRangeSize(double value);      // 扩大范围，乘法

signals:
};

class RemoteWeapon : public Weapon { // 远程武器类
protected:
    double bulletVelocity; // 子弹速度
    double bulletSize;     // 子弹大小

    WeaponType getType() override;

public:
    explicit RemoteWeapon(double bulletVelociy,
                          double bulletSize,
                          double damage,
                          double attackSpeed,
                          double rangeSize,
                          bool isPlayerSide,
                          QWidget *parent = nullptr);

    Bullet *attack(QPoint pos, double degree);

    void increaseBulletVelocity(double value); // 增加子弹大小，乘法
    void increaseBulletSize(double value);     // 增加子弹速度，乘法
};

class MeleeWeapon : public Weapon { // 近战武器类
protected:
    int validTime = 16; // 有效时间为一帧
    double spanAngle;   // 张角

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
