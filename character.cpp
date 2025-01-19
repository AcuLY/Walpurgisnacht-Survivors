#include "character.h"
#include <QDebug>
#include "utils.h"

Character::Character(QString name,
                     QString texturePath,
                     QString texturePathHurt,
                     int width,
                     int height,
                     double maxHealth,
                     double maxVelocity,
                     double accelerationFactor,
                     double attackMoveDecayFactor,
                     double reboundFactor,
                     Weapon *weapon,
                     QWidget *parent)
    : QWidget(parent), name(name), width(width), height(height), currentHealth(maxHealth),
      maxHealth(maxHealth), maxVelocity(maxVelocity), accelerationFactor(accelerationFactor),
      attackMoveDecayFactor(attackMoveDecayFactor), reboundFactor(reboundFactor), weapon(weapon) {
    texture = QPixmap(texturePath);
    textureHurt = QPixmap(texturePathHurt);

    setFixedSize(width, height);

    // 受伤显示计时器
    receiveDamageTimer = new QTimer(this);
    receiveDamageTimer->setInterval(RECEIVE_DAMAGE_DISPLAY_INTERVAL);
    receiveDamageTimer->setSingleShot(true);
    connect(receiveDamageTimer, &QTimer::timeout, this, [this] { isReceivingDamage = false; });
}

Character::~Character() {
    delete weapon;
    delete receiveDamageTimer;
}

void Character::render(QPainter *painter) {
    // 根据是否受伤渲染贴图
    if (isReceivingDamage) {
        painter->drawPixmap(this->x() + width / 2 - textureHurt.width() / 2,
                            this->y() + height / 2 - textureHurt.height() / 2,
                            textureHurt);
        return;
    }

    painter->drawPixmap(this->x() + width / 2 - texture.width() / 2,
                        this->y() + height / 2 - texture.height() / 2,
                        texture);
};

QString Character::getName() const {
    return name;
}

double Character::getCurrentHealth() const {
    return currentHealth;
}

double Character::getMaxHealth() const {
    return maxHealth;
}

QPoint Character::getPos() const {
    // 计算中心位置
    return QPoint(this->x() + width / 2, this->y() + height / 2);
}

double Character::getComposedVelocity() const {
    // 根据 xy 方向的速度计算总速度
    return std::hypot(velocity.x(), velocity.y());
}

AttackRange *Character::getRange() const {
    return weapon->getRange();
}

Weapon *Character::getWeapon() const {
    return weapon;
}

Weapon::WeaponType Character::getWeaponType() const {
    return weapon->getType();
}

bool Character::getIsReceivingDamage() const {
    return isReceivingDamage;
}

void Character::updateAcceleration(BiDirection moveX, BiDirection moveY) {
    // 初始加速度
    acceleration.setX(moveX ? moveX * maxVelocity * accelerationFactor : 0);
    acceleration.setY(moveY ? moveY * maxVelocity * accelerationFactor : 0);

    // 如果斜向移动则对 x 和 y 方向速度除根号二
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

    // 更新移动方向
    if (moveX || moveY) {
        facingDegree = qAtan2((double) -moveY, (double) moveX);
    }
}

void Character::updateVelocity() {
    double prevVelocity = getComposedVelocity();

    velocity.setX(velocity.x() + acceleration.x());
    velocity.setY(velocity.y() + acceleration.y());
    double curVelocity = getComposedVelocity();

    // 防止加速超过最大速度
    if (prevVelocity <= maxVelocity && curVelocity > maxVelocity) {
        velocity.setX(velocity.x() * maxVelocity / curVelocity);
        velocity.setY(velocity.y() * maxVelocity / curVelocity);
    }

    // 攻击时移动衰减
    if (!weapon->isCooldownFinished() || isAttacking) {
        velocity.setX(velocity.x() * attackMoveDecayFactor);
        velocity.setY(velocity.y() * attackMoveDecayFactor);
    }
}

void Character::updatePosition() {
    // move 方法只能处理整型, 需要使用 accumulator 对浮点部分进行累加
    moveAccumulator.setX(moveAccumulator.x() + modf(velocity.x(), nullptr));
    moveAccumulator.setY(moveAccumulator.y() + modf(velocity.y(), nullptr));

    int moveX = static_cast<int>(velocity.x()) + static_cast<int>(moveAccumulator.x());
    int moveY = static_cast<int>(velocity.y()) + static_cast<int>(moveAccumulator.y());

    this->move(this->x() + moveX, this->y() + moveY);

    // 去除整数部分
    moveAccumulator.setX(modf(moveAccumulator.x(), nullptr));
    moveAccumulator.setY(modf(moveAccumulator.y(), nullptr));
}

void Character::applyFriction(double friction) {
    friction = velocity.x() && velocity.y() ? friction * DIAGONAL_FACTOR
                                            : friction; // 如果斜向移动阻力也是斜向的

    // 根据速度方向施加阻力
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

QPair<bool, bool> Character::handleCollision(QRect &otherRect) {
    QRect thisRect(this->x(), this->y(), width + 1, height + 1); // 当前角色的矩形范围

    bool moveX = false, moveY = false;

    if (thisRect.intersects(otherRect)) { // 判断是否碰撞
        isBlocked = true;

        QRect intersection = thisRect.intersected(otherRect); // 两个矩形的重合部分

        // 根据重合部分的形状和两个角色的位置判断碰撞的方向，并解除重合
        if (intersection.width() > intersection.height()) {
            if (thisRect.top() < otherRect.top()) {
                this->move(thisRect.left(), otherRect.top() - thisRect.height());
            } else {
                this->move(thisRect.left(), otherRect.bottom());
            }
            moveY = true;
        } else {
            if (thisRect.left() < otherRect.left()) {
                this->move(otherRect.left() - thisRect.width(), thisRect.top());
            } else {
                this->move(otherRect.right(), thisRect.top());
            }
            moveX = true;
        }
    }

    this->rebound(moveX, moveY);

    // 返回当前角色的运动反向，让另一个角色根据此反弹
    return qMakePair(moveX, moveY);
}

void Character::handleCollision(Character *other) {
    QRect otherRect = other->geometry();

    auto [moveX, moveY] = handleCollision(otherRect);

    other->rebound(moveX, moveY);
}

void Character::handleCollision(Map *map) {
    // 计算当前角色碰撞矩形的左上角位于地图上的格子的左上角坐标
    int offsetX = this->x() < 0 ? this->x() % GRID_SIZE + GRID_SIZE : this->x() % GRID_SIZE,
        offsetY = this->y() < 0 ? this->y() % GRID_SIZE + GRID_SIZE : this->y() % GRID_SIZE,
        startX = this->x() - offsetX, startY = this->y() - offsetY;

    int widthGridsNum = width / GRID_SIZE,
        heightGridsNum = height / GRID_SIZE; // 当前角色的矩形覆盖的格子数

    // 边界情况
    if (this->x() + width > startX + GRID_SIZE * (widthGridsNum + 1)) {
        widthGridsNum++;
    }
    if (this->y() + height > startY + GRID_SIZE * (heightGridsNum + 1)) {
        heightGridsNum++;
    }

    // 右下角格子的左上角坐标
    int endX = startX + GRID_SIZE * widthGridsNum, endY = startY + GRID_SIZE * heightGridsNum;

    // 遍历角色碰撞矩形覆盖的地图格子，并进行碰撞检测
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
    // 反弹
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
    // 检查普通攻击是否冷却结束，或者仍在攻击中
    if (!weapon->isCooldownFinished() || isAttacking) {
        return;
    }

    // 如果有目标就朝目标攻击，否则朝移动方向攻击
    double degree = facingDegree;
    if (target) {
        degree = MathUtils::calculateDegree(getPos(), target->getPos());
    }

    // 发送攻击实体
    emit attackPerformed(generateAttack(degree));
}

Attack *Character::generateAttack(double degree) {
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
    currentHealth = fmax(currentHealth, 0); // 生命值不会减到负数

    // 更新受伤显示状态
    isReceivingDamage = true;
    receiveDamageTimer->start();

    emit damageReceived();
}
