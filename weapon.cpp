#include "weapon.h"
#include "attackrange.h"
#include "utils.h"

Weapon::Weapon(double damage,
               double attackInterval,
               double rangeSize,
               bool isPlayerSide,
               QWidget *parent)
    : QWidget{parent}, damage(damage), attackInterval(attackInterval), rangeSize(rangeSize),
      isPlayerSide(isPlayerSide) {
    cooldownTimer = new QTimer;
    cooldownTimer->setInterval(attackInterval);
    cooldownTimer->setSingleShot(true);
    connect(cooldownTimer, &QTimer::timeout, this, [this]() { isOnCooldown = false; });
}

Weapon::~Weapon() {
    delete cooldownTimer;
}

double Weapon::getDamage() {
    return damage;
}

AttackRange *Weapon::getRange() {
    return range;
}

bool Weapon::isCooldownFinished() {
    return !isOnCooldown;
}

void Weapon::setMultiAttackMode(bool isMulti) {
    isMultiAttacking = isMulti;
}

void Weapon::increaseDamage(double value) {
    damage += value;
}

void Weapon::decreaseAttackInterval(double value) {
    attackInterval *= value;
    cooldownTimer->setInterval(attackInterval);
}

void Weapon::increaseRangeSize(double value) {
    rangeSize *= value;
    delete range;
    range = new CircleRange(rangeSize);
}

RemoteWeapon::RemoteWeapon(double bulletVelocity,
                           double bulletSize,
                           double damage,
                           double attackInterval,
                           double rangeSize,
                           bool isPlayerSide,
                           QWidget *parent)
    : Weapon(damage, attackInterval, rangeSize, isPlayerSide, parent),
      bulletVelocity(bulletVelocity), bulletSize(bulletSize) {
    range = new CircleRange(rangeSize);
}

Weapon::WeaponType RemoteWeapon::getType() {
    return Weapon::WeaponType::Remote;
}

Bullet *RemoteWeapon::attack(QPoint pos, double degree) {
    // 如果在连击模式则忽视冷却，否则进入冷却
    if (!isMultiAttacking) {
        if (isOnCooldown) {
            return nullptr;
        }

        isOnCooldown = true;
        cooldownTimer->start();
    }

    int validTime = rangeSize / bulletVelocity * 16; // 子弹类的有效时间是 范围 / 速度 * 帧长度
    auto newBullet = new Bullet(pos, bulletSize, damage, isPlayerSide, validTime);

    QPointF v = MathUtils::velocityDecomQPointF(bulletVelocity, degree); // 由角度分解速度
    newBullet->setVelocity(v);

    return newBullet;
}

void RemoteWeapon::increaseBulletVelocity(double value) {
    bulletVelocity *= value;
}

void RemoteWeapon::increaseBulletSize(double value) {
    bulletSize *= value;
}

Weapon::WeaponType MeleeWeapon::getType() {
    return Weapon::WeaponType::Melee;
}

MeleeWeapon::MeleeWeapon(double damage,
                         double attackInterval,
                         double rangeSize,
                         double spanAngle,
                         bool isPlayerSide,
                         QWidget *parent)
    : Weapon(damage, attackInterval, rangeSize, isPlayerSide, parent), spanAngle(spanAngle) {
    range = new CircleRange(rangeSize);
}

Slash *MeleeWeapon::attack(QPoint pos, double degree) {
    if (!isMultiAttacking) {
        if (isOnCooldown) {
            return nullptr;
        }

        isOnCooldown = true;
        cooldownTimer->start();
    }

    Slash *slash = new Slash(pos,
                             rangeSize,
                             degree,
                             qDegreesToRadians(spanAngle),
                             damage,
                             isPlayerSide,
                             validTime);

    return slash;
}

void MeleeWeapon::increaseSpanAngle(double value) {
    spanAngle *= value;
    spanAngle = qMin(360, (int) spanAngle);
}
