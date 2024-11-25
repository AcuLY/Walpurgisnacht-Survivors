#include "bullet.h"

Bullet::Bullet(int x, int y, double size, double damage, bool isPlayerSide, QWidget *parent)
    : QWidget{parent}, size(size), damage(damage), isPlayerSide(isPlayerSide) {
    this->move(x, y);
    prevPos.setX(x);
    prevPos.setY(y);
}

double Bullet::getSize() const {
    return size;
}

bool Bullet::getSide() const {
    return isPlayerSide;
}

bool Bullet::getValidity() const {
    return isValid;
}

void Bullet::setValidity() {
    isValid = !isValid;
}

void Bullet::setAcceleration(double aX, double aY) {
    accelerationX = aX;
    accelerationY = aY;
}

void Bullet::setVelocity(double vX, double vY) {
    velocityX = vX;
    velocityY = vY;
}

void Bullet::moveActively() {
    prevPos.setX(this->x());
    prevPos.setY(this->y());
    this->move(this->x() + velocityX, this->y() + velocityY);

    velocityX += accelerationX;
    velocityY += accelerationY;
}

bool Bullet::isHit(const QRectF &targetRect) {
    QLineF bulletPath(prevPos, QPointF(this->x() + size, this->y() + size));

    QLineF top(targetRect.topLeft(), targetRect.topRight());
    QLineF left(targetRect.topLeft(), targetRect.bottomLeft());
    QLineF bottom(targetRect.bottomLeft(), targetRect.bottomRight());
    QLineF right(targetRect.topRight(), targetRect.bottomRight());

    bulletPath.translate(0, -size);
    for (int i = 0; i < 3; ++i) {
        bulletPath.translate(0, size);

        if (bulletPath.intersects(top) == QLineF::BoundedIntersection
            || bulletPath.intersects(left) == QLineF::BoundedIntersection
            || bulletPath.intersects(bottom) == QLineF::BoundedIntersection
            || bulletPath.intersects(right) == QLineF::BoundedIntersection) {
            return true;
        }
    }

    return false;
}
