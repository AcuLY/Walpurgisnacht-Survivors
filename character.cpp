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

QPoint Character::getPos() const {
    return QPoint(this->x() + width / 2, this->y() + height / 2);
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

    if (velocity.x() || velocity.y()) {
        facingDegree = qAtan2(-velocity.y(), velocity.x());
    }
}

void Character::updatePosition() {
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
    updatePosition();
}

void Character::stop() {
    velocity = QPointF(0, 0);
    acceleration = QPointF(0, 0);
}

QPair<bool, bool> Character::handleCollision(QRect &otherRect) {
    QRect thisRect(this->x(), this->y(), width + 1, height + 1);

    bool moveX = false, moveY = false;

    if (thisRect.intersects(otherRect)) {
        QRect intersection = thisRect.intersected(otherRect);

        if (intersection.width() > intersection.height()) {
            if (thisRect.top() < otherRect.top()) {
                this->move(thisRect.left(), otherRect.top() - thisRect.height() - REBOUND_PADDING);
            } else {
                this->move(thisRect.left(), otherRect.bottom() + REBOUND_PADDING);
            }
            moveY = true;
        } else {
            if (thisRect.left() < otherRect.left()) {
                this->move(otherRect.left() - thisRect.width() - REBOUND_PADDING, thisRect.top());
            } else {
                this->move(otherRect.right() + REBOUND_PADDING, thisRect.top());
            }
            moveX = true;
        }
    }

    this->rebound(moveX, moveY);

    return qMakePair(moveX, moveY);
}

void Character::handleCollision(Character *other) {
    QRect otherRect = other->geometry();

    auto [moveX, moveY] = handleCollision(otherRect);

    other->rebound(moveX, moveY);
}

void Character::handleCollision(Map *map) {
    int offsetX = this->x() < 0 ? this->x() % GRID_SIZE + GRID_SIZE : this->x() % GRID_SIZE;
    int offsetY = this->y() < 0 ? this->y() % GRID_SIZE + GRID_SIZE : this->y() % GRID_SIZE;
    int startX = this->x() - offsetX;
    int startY = this->y() - offsetY;

    int widthGridsNum = width / GRID_SIZE;
    int heightGridsNum = height / GRID_SIZE;

    // 边界情况
    if (this->x() + width > startX + GRID_SIZE * (widthGridsNum + 1)) {
        widthGridsNum++;
    }
    if (this->y() + height > startY + GRID_SIZE * (heightGridsNum + 1)) {
        heightGridsNum++;
    }

    int endX = startX + GRID_SIZE * widthGridsNum;
    int endY = startY + GRID_SIZE * heightGridsNum;

    for (int x = startX; x <= endX; x += GRID_SIZE) {
        QPoint p1(x, startY), p2(x, endY);
        if (map->isObstacle(p1)) {
            QRect obstacle(x, startY, GRID_SIZE, GRID_SIZE);
            this->handleCollision(obstacle);
        }
        if (map->isObstacle(p2)) {
            QRect obstacle(x, endY, GRID_SIZE, GRID_SIZE);
            this->handleCollision(obstacle);
        }
    }

    for (int y = startY; y <= endY; y += GRID_SIZE) {
        QPoint p1(startX, y), p2(endX, y);
        if (map->isObstacle(p1)) {
            QRect obstacle(startX, y, GRID_SIZE, GRID_SIZE);
            this->handleCollision(obstacle);
        }
        if (map->isObstacle(p2)) {
            QRect obstacle(endX, y, GRID_SIZE, GRID_SIZE);
            this->handleCollision(obstacle);
        }
    }
}

void Character::rebound(bool x, bool y) {
    if (x) {
        velocity.setX(-velocity.x() * reboundFactor);
        acceleration.setX(-acceleration.x() * reboundFactor);
    }
    if (y) {
        velocity.setY(-velocity.y() * reboundFactor);
        acceleration.setY(-acceleration.y() * reboundFactor);
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
