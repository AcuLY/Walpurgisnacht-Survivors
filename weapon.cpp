#include "weapon.h"
#include "attackrange.h"

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

void Weapon::setDamage(double newDamage) {
    damage = newDamage;
}

RemoteWeapon::RemoteWeapon(double bulletVelocity,
                           double bulletSize,
                           double damage,
                           double attackInterval,
                           double rangeSize,
                           bool isPlayerSide,
                           bool penetrability,
                           QWidget *parent)
    : Weapon(damage, attackInterval, rangeSize, isPlayerSide, parent),
      bulletVelocity(bulletVelocity), bulletSize(bulletSize), penetrability(penetrability) {
    range = new CircleRange(rangeSize);
}

double RemoteWeapon::getBulletSize() const {
    return bulletSize;
};

Weapon::WeaponType RemoteWeapon::getType() {
    return Weapon::WeaponType::Remote;
}

Bullet *RemoteWeapon::attack(QPoint pos, double degree) {
    if (isOnCooldown) {
        return nullptr;
    }

    isOnCooldown = true;
    cooldownTimer.start();

    int validTime = rangeSize / bulletVelocity * 16;
    auto newBullet = new Bullet(pos, bulletSize, damage, isPlayerSide, validTime, penetrability);

    QPointF v = MathUtils::velocityDecomQPointF(bulletVelocity, degree);
    newBullet->setVelocity(v);

    return newBullet;
}

Weapon::WeaponType MeleeWeapon::getType() {
    return Weapon::WeaponType::Melee;
}

MeleeWeapon::MeleeWeapon(double damage,
                         double attackInterval,
                         double rangeSize,
                         bool isPlayerSide,
                         QWidget *parent)
    : Weapon(damage, attackInterval, rangeSize, isPlayerSide, parent) {
    range = new CircleRange(rangeSize);
}

Slash *MeleeWeapon::attack(QPoint pos, double degree) {
    if (isOnCooldown) {
        return nullptr;
    }

    isOnCooldown = true;
    cooldownTimer.start();

    Slash *slash
        = new Slash(pos, rangeSize, degree, qDegreesToRadians(60), damage, isPlayerSide, validTime);

    return slash;
}
