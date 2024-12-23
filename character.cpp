#include "character.h"
#include <QDebug>

Character::Character(QString name,
                     int width,
                     int height,
                     double maxHealth,
                     double maxVelocity,
                     double accelerationFactor,
                     double reboundFactor,
                     Weapon *weapon,
                     QWidget *parent)
    : QWidget(parent), name(name), width(width), height(height), maxHealth(maxHealth),
      maxVelocity(maxVelocity), accelerationFactor(accelerationFactor),
      reboundFactor(reboundFactor), weapon(weapon) {
    setFixedSize(width, height);
    currentHealth = maxHealth;
}

Character::~Character() {
    delete weapon;
};

QString Character::getName() const {
    return name;
}

double Character::getHealth() const {
    return currentHealth;
}

double Character::getMaxHealth() const {
    return maxHealth;
}

QPoint Character::getPos() const {
    return QPoint(this->x() + width / 2, this->y() + height / 2);
}

double Character::getComposedVelocity() const {
    return std::hypot(velocity.x(), velocity.y());
}

double Character::getMaxVelocity() const {
    return maxVelocity;
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

Weapon *Character::getWeapon() const {
    return weapon;
}

Weapon::WeaponType Character::getWeaponType() const {
    return weapon->getType();
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
    if (getComposedVelocity() > maxVelocity) {
        if (acceleration.x() * velocity.x() > 0) {
            acceleration.setX(0);
        }
        if (acceleration.y() * velocity.y() > 0) {
            acceleration.setY(0);
        }
    }

    if (moveX || moveY) {
        facingDegree = qAtan2((double) -moveY, (double) moveX);
    }
}

void Character::updateVelocity() {
    double prevVelocity = getComposedVelocity();

    velocity.setX(velocity.x() + acceleration.x());
    velocity.setY(velocity.y() + acceleration.y());
    double curVelocity = getComposedVelocity();

    if (prevVelocity <= maxVelocity && curVelocity > maxVelocity) {
        velocity.setX(velocity.x() * maxVelocity / curVelocity);
        velocity.setY(velocity.y() * maxVelocity / curVelocity);
    }
}

void Character::updatePosition() {
    // move 方法只能处理整型, 需要使用 accumulator 对浮点部分进行累加
    moveAccumulator.setX(moveAccumulator.x() + modf(velocity.x(), nullptr));
    moveAccumulator.setY(moveAccumulator.y() + modf(velocity.y(), nullptr));

    int moveX = static_cast<int>(velocity.x()) + static_cast<int>(moveAccumulator.x());
    int moveY = static_cast<int>(velocity.y()) + static_cast<int>(moveAccumulator.y());

    this->move(this->x() + moveX, this->y() + moveY);

    moveAccumulator.setX(modf(moveAccumulator.x(), nullptr));
    moveAccumulator.setY(modf(moveAccumulator.y(), nullptr));
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
    currentHealth -= damage;
}
