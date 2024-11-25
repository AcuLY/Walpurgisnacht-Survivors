#include "weapon.h"

Weapon::Weapon(double damage, double attackInterval, double range, QWidget *parent)
    : QWidget{parent}, damage(damage), attackInterval(attackInterval), range(range) {
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

double Weapon::getRange() {
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
                           double range,
                           QWidget *parent)
    : Weapon(damage, attackInterval, range, parent), bulletVelocity(bulletVelocity),
      bulletSize(bulletSize) {};

Weapon::WeaponType RemoteWeapon::getType() {
    return Weapon::WeaponType::Remote;
}

Bullet *RemoteWeapon::attack(int x, int y, double degree) {
    if (isOnCooldown) {
        return nullptr;
    }

    isOnCooldown = true;
    cooldownTimer.start();

    auto newBullet = new Bullet(x, y, bulletSize, damage, isPlayerSide);

    auto [vX, vY] = MathUtils::velocityDecomQPointF(bulletVelocity, degree);
    newBullet->setVelocity(vX, vY);

    return newBullet;
}
