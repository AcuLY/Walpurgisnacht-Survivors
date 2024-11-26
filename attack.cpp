#include "attack.h"

Attack::Attack(QPointF initPos,
               double size,
               double damage,
               bool isPlayerSide,
               int validTime,
               QWidget *parent)
    : QWidget(parent), pos(initPos), size(size), damage(damage), isPlayerSide(isPlayerSide) {
    this->move(initPos.x() - size, initPos.y() - size);

    validityTimer.setInterval(validTime);
    validityTimer.setSingleShot(true);
    connect(&validityTimer, &QTimer::timeout, this, [this]() { isValid = false; });
}

Attack::~Attack() {
    delete range;
}

QPointF Attack::getPos() const {
    return pos;
}

AttackRange *Attack::getRange() const {
    return range;
}

double Attack::getDamage() const {
    return damage;
}

double Attack::getPlayerSide() const {
    return isPlayerSide;
}

void Attack::setPlayerSide() {
    isPlayerSide = !isPlayerSide;
}

double Attack::getValidity() const {
    return isValid;
}

QPainterPath Attack::createPath() const {
    return range->createPath(pos);
}

Bullet::Bullet(QPointF initPos,
               double size,
               double damage,
               bool isPlayerSide,
               int validTime,
               QWidget *parent)
    : Attack(initPos, size, damage, isPlayerSide, validTime, parent) {
    range = new CircleRange(size);

    prevPos.setX(initPos.x() - size);
    prevPos.setY(initPos.y() - size);
}

void Bullet::setAcceleration(QPointF a) {
    acceleration = a;
}

void Bullet::setVelocity(QPointF v) {
    velocity = v;
}

void Bullet::moveActively() {
    prevPos.setX(this->x());
    prevPos.setY(this->y());
    this->move(this->x() + velocity.x(), this->y() + velocity.y());
    pos.setX(this->x());
    pos.setY(this->y());

    velocity += acceleration;
}

bool Bullet::isHit(const QRectF &targetRect) {
    QPointF curPos(this->x(), this->y());
    QPainterPath track = range->createTrack(curPos, prevPos);
    return track.intersects(targetRect);
}
