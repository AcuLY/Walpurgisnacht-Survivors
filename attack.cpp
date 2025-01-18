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
    return QPoint(pos.x(), pos.y());
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
    texture = isPlayerSide ? QPixmap(":/images/item/bullet")
                           : QPixmap(":/images/item/bullet_enemy");

    range = new CircleRange(size);

    prevPos = initPos;
}

void Bullet::render(QPainter *painter) {
    QRect paintRect(pos.x() - size, pos.y() - size, size * 2, size * 2);
    painter->drawPixmap(paintRect, texture);
}

QPoint Bullet::getPrevPos() {
    return QPoint(prevPos.x(), prevPos.y());
}

void Bullet::setAcceleration(QPointF a) {
    acceleration = a;
}

void Bullet::setVelocity(QPointF v) {
    velocity = v;
}

void Bullet::moveActively() {
    prevPos = pos;

    this->move(pos.x() + velocity.x() - size / 2,
               pos.y() + velocity.y()
                   - size / 2); // 这里必须用 pos 来计算，不然会出现负数舍入错误的情况

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
    : Attack(pos, size, damage, isPlayerSide, validTime, parent), startAngle(startAngle),
      spanAngle(spanAngle) {
    range = new SectorRange(size, startAngle, spanAngle);

    texture = isPlayerSide ? QPixmap(":/images/item/slash") : QPixmap(":/images/item/slash_enemy");
};

bool Slash::isHit(const QRectF &targetRect) {
    QPainterPath path = range->createPath(pos);
    return path.intersects(targetRect);
}

void Slash::render(QPainter *painter) {
    painter->save(); // 保存当前画笔状态

    painter->setClipPath(range->createPath(pos));

    // 缩放贴图
    QPixmap scaledTexture = texture.scaled(size * 2,
                                           size * 2,
                                           Qt::IgnoreAspectRatio,
                                           Qt::SmoothTransformation);

    // 绘制贴图，中心对齐
    QRect paintRect(pos.x() - size, pos.y() - size, size * 2, size * 2);

    // 使用正确的 drawPixmap 重载函数
    painter->drawPixmap(paintRect, scaledTexture);

    painter->restore(); // 恢复画笔状态
}
