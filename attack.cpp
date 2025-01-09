#include "attack.h"

Attack::Attack(QPoint initPos,
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
    validityTimer.start();
}

Attack::~Attack() {
    delete range;
}

QPoint Attack::getPos() const {
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

Bullet::Bullet(QPoint initPos,
               double size,
               double damage,
               bool isPlayerSide,
               int validTime,
               QWidget *parent)
    : Attack(initPos, size, damage, isPlayerSide, validTime, parent) {
    range = new CircleRange(size);

    prevPos.setX(initPos.x());
    prevPos.setY(initPos.y());
}

QPoint &Bullet::getPrevPos() {
    return prevPos;
}

void Bullet::setAcceleration(QPointF a) {
    acceleration = a;
}

void Bullet::setVelocity(QPointF v) {
    velocity = v;
}

void Bullet::moveActively() {
    prevPos.setX(pos.x());
    prevPos.setY(pos.y());
    this->move(this->x() + velocity.x(), this->y() + velocity.y());
    pos.setX(pos.x() + velocity.x());
    pos.setY(pos.y() + velocity.y());

    velocity += acceleration;
}

bool Bullet::isHit(const QRectF &targetRect) {
    QPointF curPos = pos;
    QPainterPath track = range->createTrack(curPos, prevPos);
    return track.intersects(targetRect);
}

bool Bullet::isHit(const QPainterPath &path) {
    QPointF curPos = pos;
    QPainterPath track = range->createTrack(curPos, prevPos);

    return track.intersects(path);
}

Slash::Slash(QPoint pos,
             double size,
             double startAngle,
             double spanAngle,
             double damage,
             bool isPlayerSide,
             int validTime,
             QWidget *parent)
    : Attack(pos, size, damage, isPlayerSide, validTime, parent) {
    range = new SectorRange(size, startAngle, spanAngle);
};

bool Slash::isHit(const QRectF &targetRect) {
    QPainterPath path = range->createPath(pos);
    return path.intersects(targetRect);
}
