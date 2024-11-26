#include "character.h"
#include <QDebug>

Character::Character(QString name,
                     int width,
                     int height,
                     double health,
                     double maxVelocity,
                     double accelerationFactor,
                     double reboundFactor,
                     Weapon *weapon,
                     QWidget *parent)
    : QWidget(parent), name(name), width(width), height(height), health(health),
      maxVelocity(maxVelocity), accelerationFactor(accelerationFactor),
      reboundFactor(reboundFactor), weapon(weapon) {
    setFixedSize(width, height);
};

QString Character::getName() const {
    return name;
}

double Character::getHealth() const {
    return health;
}

QPointF Character::getPos() const {
    return QPointF(this->x() + width / 2, this->y() + height / 2);
}

double Character::getVelocity() const {
    return std::hypot(velocity.x(), velocity.y());
}

Weapon *Character::getWeapon() const {
    return weapon;
}

void Character::updateAcceleration(BiDirection moveX, BiDirection moveY) {
    acceleration.setX(moveX ? moveX * maxVelocity * accelerationFactor : 0);
    acceleration.setY(moveY ? moveY * maxVelocity * accelerationFactor : 0);

    if (moveX && moveY) {
        acceleration.setX(acceleration.x() * DIAGONAL_FACTOR);
        acceleration.setY(acceleration.y() * DIAGONAL_FACTOR);
    }

    // 当速度大于角色自身最大时只能做减速
    if (getVelocity() >= maxVelocity) {
        if (acceleration.x() * velocity.x() > 0) {
            acceleration.setX(0);
        }
        if (acceleration.y() * velocity.y() > 0) {
            acceleration.setY(0);
        }
    }
}

void Character::updateVelocity() {
    velocity.setX(velocity.x() + acceleration.x());
    velocity.setY(velocity.y() + acceleration.y());
}

void Character::updateQPointF() {
    this->move(this->x() + velocity.x(), this->y() + velocity.y());
}

void Character::applyFriction(double friction) {
    friction = velocity.x() && velocity.y() ? friction * DIAGONAL_FACTOR : friction;

    if (velocity.x() > 0) {
        velocity.setX(qMax(velocity.x() - friction, 0.0));
    } else if (velocity.x() < 0) {
        velocity.setX(qMin(velocity.x() + friction, 0.0));
    }

    if (velocity.y() > 0) {
        velocity.setY(qMax(velocity.y() - friction, 0.0));
    } else if (velocity.y() < 0) {
        velocity.setY(qMin(velocity.y() + friction, 0.0));
    }
}

void Character::moveActively(Direction dir) {
    auto [moveX, moveY] = ~dir;
    updateAcceleration(moveX, moveY);
    updateVelocity();
    updateQPointF();
}

void Character::handleCollision(Character *other) {
    QRect thisRect = geometry();
    QRect otherRect = other->geometry();

    if (thisRect.intersects(otherRect)) {
        QRect intersection = thisRect.intersected(otherRect);

        if (intersection.width() > intersection.height()) {
            if (thisRect.top() < otherRect.top()) {
                this->move(thisRect.left(), otherRect.top() - thisRect.height());
            } else {
                this->move(thisRect.left(), otherRect.bottom());
            }

            velocity.setY(-velocity.y() * reboundFactor);
            acceleration.setY(0);
            other->velocity.setY(-other->velocity.y() * other->reboundFactor);
            other->acceleration.setY(0);
        } else {
            if (thisRect.left() < otherRect.left()) {
                this->move(otherRect.left() - thisRect.width(), thisRect.top());
            } else {
                this->move(otherRect.right(), thisRect.top());
            }

            velocity.setX(-velocity.x() * reboundFactor);
            acceleration.setX(0);
            other->velocity.setX(-other->velocity.x() * other->reboundFactor);
            other->acceleration.setX(0);
        }
    }
}

Bullet *Character::regularAttack(double degree) {
    if (weapon->getType() == Weapon::WeaponType::Remote) {
        Bullet *bullet = ((RemoteWeapon *) weapon)->attack(this->getPos(), degree);
        return bullet;
    }
    return nullptr;
}

void Character::receiveDamage(double damage) {
    health -= damage;
}
