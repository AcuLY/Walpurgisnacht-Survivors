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

void Character::updateVelocity() {
    velocityX += accelerationX;
    velocityY += accelerationY;

    if (getVelocity() > maxVelocity) {
        double currentVelocity = getVelocity();
        velocityX = maxVelocity * velocityX / currentVelocity;
        velocityY = maxVelocity * velocityY / currentVelocity;
    }
}

void Character::updatePosition(Map &map) {
    this->move(this->x() + velocityX, this->y() + velocityY);

    Direction boundCheck = map.isOutOfBoundry(this->geometry());
    if (boundCheck == Direction::North) {
        this->move(this->x(), map.y());
    }
    if (boundCheck == Direction::South) {
        this->move(this->x(), map.y() + map.getHeight() - height);
    }
    if (boundCheck == Direction::West) {
        this->move(map.x(), this->y());
    }
    if (boundCheck == Direction::East) {
        this->move(map.x() + map.getWidth() - width, this->y());
    }

    if (boundCheck == Direction::West || boundCheck == Direction::East) {
        velocityX = -velocityX * reboundFactor;
        accelerationX = 0;
    }
    if (boundCheck == Direction::North || boundCheck == Direction::South) {
        velocityY = -velocityY * reboundFactor;
        accelerationY = 0;
    }
}

void Character::applyAcceleration(double aX, double aY) {
    accelerationX += aX;
    accelerationY += aY;
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

void Character::moveActively(BiDirection moveX, BiDirection moveY, Map &map) {
    accelerationX = moveX ? moveX * maxVelocity * accelerationFactor : 0;
    accelerationY = moveY ? moveY * maxVelocity * accelerationFactor : 0;

    if (moveX && moveY) {
        accelerationX *= DIAGONAL_FACTOR;
        accelerationY *= DIAGONAL_FACTOR;
    }

    updateVelocity();
    updatePosition(map);
}

void Character::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);

    painter.setPen(Qt::black);
    painter.setBrush(Qt::blue);

    painter.drawRect(this->rect());

    painter.setPen(Qt::white);
    painter.drawText(rect(), Qt::AlignCenter, name);
}
