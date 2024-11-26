#include "weapon.h"

Weapon::Weapon(double damage,
               double attackInterval,
               double rangeSize,
               bool isPlayerSide,
               QWidget *parent)
    : QWidget{parent}, damage(damage), attackInterval(attackInterval), rangeSize(rangeSize),
      isPlayerSide(isPlayerSide) {
    cooldownTimer.setInterval(attackInterval);
    cooldownTimer.setSingleShot(true);
    connect(&cooldownTimer, &QTimer::timeout, this, [this]() { isOnCooldown = false; });
}

double Weapon::getDamage() {
    return damage;
}

double Weapon::getAttackInterval() {
    return attackInterval;
}

AttackRange *Weapon::getRange() {
    return range;
}

bool Weapon::isCooldownFinished() {
    return !isOnCooldown;
}

bool Weapon::getSide() {
    return isPlayerSide;
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
};

Weapon::WeaponType RemoteWeapon::getType() {
    return Weapon::WeaponType::Remote;
}

Bullet *RemoteWeapon::attack(QPointF pos, double degree) {
    if (isOnCooldown) {
        return nullptr;
    }

    isOnCooldown = true;
    cooldownTimer.start();

    int validTime = rangeSize / bulletVelocity;
    auto newBullet = new Bullet(pos, bulletSize, damage, isPlayerSide, validTime);

    QPointF v = MathUtils::velocityDecomQPointF(bulletVelocity, degree);
    newBullet->setVelocity(v);

    return newBullet;
}
