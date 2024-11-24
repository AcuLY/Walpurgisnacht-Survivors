#include "character.h"
#include <QDebug>

Character::Character(QString name,
                     int width,
                     int height,
                     double health,
                     double damage,
                     double maxVelocity,
                     double accelerationFactor,
                     double reboundFactor,
                     QWidget *parent)
    : QWidget(parent), name(name), width(width), height(height), health(health), damage(damage),
      maxVelocity(maxVelocity), accelerationFactor(accelerationFactor),
      reboundFactor(reboundFactor) {
    setFixedSize(width, height);
};

QString Character::getName() const {
    return name;
}

double Character::getHealth() const {
    return health;
}

double Character::getDamage() const {
    return damage;
}

double Character::getVelocity() const {
    return std::hypot(velocityX, velocityY);
}

void Character::updateAcceleration(BiDirection moveX, BiDirection moveY) {
    accelerationX = moveX ? moveX * maxVelocity * accelerationFactor : 0;
    accelerationY = moveY ? moveY * maxVelocity * accelerationFactor : 0;

    if (moveX && moveY) {
        accelerationX *= DIAGONAL_FACTOR;
        accelerationY *= DIAGONAL_FACTOR;
    }

    // 当速度大于角色自身最大时只能做减速
    if (getVelocity() >= maxVelocity) {
        if (accelerationX * velocityX > 0) {
            accelerationX = 0;
        }
        if (accelerationY * velocityY > 0) {
            accelerationY = 0;
        }
    }
}

void Character::updateVelocity() {
    velocityX += accelerationX;
    velocityY += accelerationY;
}

void Character::updatePosition() {
    this->move(this->x() + velocityX, this->y() + velocityY);
}

void Character::applyFriction(double friction) {
    friction = velocityX && velocityY ? friction * DIAGONAL_FACTOR : friction;

    if (velocityX > 0) {
        velocityX = qMax(velocityX - friction, 0.0);
    } else if (velocityX < 0) {
        velocityX = qMin(velocityX + friction, 0.0);
    }

    if (velocityY > 0) {
        velocityY = qMax(velocityY - friction, 0.0);
    } else if (velocityY < 0) {
        velocityY = qMin(velocityY + friction, 0.0);
    }
}

void Character::moveActively(Direction dir) {
    auto [moveX, moveY] = ~dir;
    updateAcceleration(moveX, moveY);
    updateVelocity();
    updatePosition();
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

            velocityY = -velocityY * reboundFactor;
            accelerationY = 0;
            other->velocityY = -other->velocityY * other->reboundFactor;
            other->accelerationY = 0;
        } else {
            if (thisRect.left() < otherRect.left()) {
                this->move(otherRect.left() - thisRect.width(), thisRect.top());
            } else {
                this->move(otherRect.right(), thisRect.top());
            }

            velocityX = -velocityX * reboundFactor;
            accelerationX = 0;
            other->velocityX = -other->velocityX * other->reboundFactor;
            other->accelerationX = 0;
        }
    }
}
