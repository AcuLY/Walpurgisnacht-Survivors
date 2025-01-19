#ifndef CHARACTER_H
#define CHARACTER_H

#include <QPainter>
#include <QString>
#include <QWidget>
#include <QtMath>

#include "map.h"
#include "weapon.h"

const int RECEIVE_DAMAGE_DISPLAY_INTERVAL = 128; // 角色受击后变红的时间

class Character : public QWidget { // 角色实体类
    Q_OBJECT

protected:
    QPixmap texture;     // 正常贴图
    QPixmap textureHurt; // 受击时贴图

    QString name;

    QPointF moveAccumulator; // 小数部分的位移的累加器，坐标只能是整数所以小数部分需要额外计算

    int width;
    int height;

    double currentHealth;
    double maxHealth;

    double maxVelocity;
    double accelerationFactor;    // 加速系数，为 acceleration / maxVelocity
    double attackMoveDecayFactor; // 在攻击时移动的速度衰减系数
    double reboundFactor;   // 碰撞后反向的速度与原速度的比值，体现角色的重量
    bool isBlocked = false; // 是否被挡住

    QPointF velocity = QPointF(0, 0);
    QPointF acceleration = QPointF(0, 0);
    double facingDegree = 0; // 当前移动的方向

    bool isAttacking = false;       // 是否在攻击
    bool isReceivingDamage = false; // 是否在受伤

    QTimer *receiveDamageTimer;

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
    virtual ~Character();

    void render(QPainter *painter);

    QString getName() const;
    double getCurrentHealth() const;
    double getMaxHealth() const;
    QPoint getPos() const; // 返回中心位置
    AttackRange *getRange() const;
    Weapon *getWeapon() const;
    Weapon::WeaponType getWeaponType() const;
    bool getIsReceivingDamage() const;

    void applyFriction(double friction); // 地图阻力
    void moveActively(Direction dir);    // 根据方向主动移动

    void handleCollision(Character *other); // 处理和另一个角色的碰撞
    void handleCollision(Map *map);         // 处理和地图的碰撞

    void performAttack(Character *target); // 对目标发起攻击

    void receiveDamage(double damage); // 受伤

protected:
    void updateAcceleration(BiDirection moveX, BiDirection moveY); // 根据移动方向改变加速度
    void updateVelocity();
    void updatePosition();
    double getComposedVelocity() const; // 计算总速度

    QPair<bool, bool> handleCollision(QRect &otherRect); // 处理和一个矩形目标的碰撞
    void rebound(bool x, bool y);                        // 碰撞后的反弹

    Attack *generateAttack(double degree); // 返回一个攻击实体，如子弹

signals:
    void attackPerformed(Attack *attack);
    void damageReceived();

public slots:
};

#endif // CHARACTER_H
