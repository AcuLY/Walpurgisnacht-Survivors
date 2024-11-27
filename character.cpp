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

AttackRange *Character::getRange() const {
    return weapon->getRange();
}

double Character::getFacingDegree() const {
    return facingDegree;
}

bool Character::getAttacking() const {
    return isAttacking;
}

void Character::setAttacking() {
    isAttacking = !isAttacking;
}

void Character::updateAcceleration(BiDirection moveX, BiDirection moveY) {
    acceleration.setX(moveX ? moveX * maxVelocity * accelerationFactor : 0);
    acceleration.setY(moveY ? moveY * maxVelocity * accelerationFactor : 0);

    if (moveX && moveY) {
        acceleration.setX(acceleration.x() * DIAGONAL_FACTOR);
        acceleration.setY(acceleration.y() * DIAGONAL_FACTOR);
    }

    // 当速度大于角色自身最大时只能做减速
    if (getVelocity() > maxVelocity) {
        if (acceleration.x() * velocity.x() > 0) {
            acceleration.setX(0);
        }
        if (acceleration.y() * velocity.y() > 0) {
            acceleration.setY(0);
        }
    }

    // 如果一个方向达到最大速度时往另一个方向加速, 则原方向减速
    if (abs(velocity.x()) >= maxVelocity && moveY) {
        acceleration.setX(0);
        acceleration.setY(moveY * maxVelocity * accelerationFactor);
    }
    if (abs(velocity.y()) >= maxVelocity && moveX) {
        acceleration.setY(0);
        acceleration.setX(moveX * maxVelocity * accelerationFactor);
    }
}

void Character::updateVelocity() {
    velocity.setX(velocity.x() + acceleration.x());
    velocity.setY(velocity.y() + acceleration.y());

    facingDegree = qAtan2(-velocity.y(), velocity.x());
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

void Character::handleCollision(Map *map) {
    if (!map->getObstacle(QPoint(this->x(), this->y()))) {
        return;
    }

    int gridX = this->x() - this->x() % GRID_SIZE, gridY = this->y() - this->y() % GRID_SIZE;
    QRect otherRect(gridX, gridY, GRID_SIZE, GRID_SIZE);

    QRect thisRect = geometry();

    QRect intersection = thisRect.intersected(otherRect);
    if (intersection.width() > intersection.height()) {
        if (thisRect.top() < otherRect.top()) {
            this->move(thisRect.left(), otherRect.top() - thisRect.height());
        } else {
            this->move(thisRect.left(), otherRect.bottom());
        }

        velocity.setY(-velocity.y() * reboundFactor);
        acceleration.setY(0);
    } else {
        if (thisRect.left() < otherRect.left()) {
            this->move(otherRect.left() - thisRect.width(), thisRect.top());
        } else {
            this->move(otherRect.right(), thisRect.top());
        }

        velocity.setX(-velocity.x() * reboundFactor);
        acceleration.setX(0);
    }
}

void Character::performAttack(Character *target) {
    if (!weapon->isCooldownFinished() || isAttacking) {
        return;
    }

    double degree = facingDegree;
    if (target) {
        degree = MathUtils::calculateDegree(this->getPos(), target->getPos());
    }

    if (weapon->getType() == Weapon::WeaponType::Remote) {
        Bullet *bullet = (Bullet *) this->regularAttack(degree);
        emit attackPerformed(bullet);
    } else {
        Slash *slash = (Slash *) this->regularAttack(degree);
        emit attackPerformed(slash);
    }
}

Attack *Character::regularAttack(double degree) {
    if (weapon->getType() == Weapon::WeaponType::Remote) {
        Bullet *bullet = ((RemoteWeapon *) weapon)->attack(this->getPos(), degree);
        return bullet;
    } else {
        Slash *slash = ((MeleeWeapon *) weapon)->attack(this->getPos(), degree);
        return slash;
    }
}

void Character::receiveDamage(double damage) {
    health -= damage;
}
