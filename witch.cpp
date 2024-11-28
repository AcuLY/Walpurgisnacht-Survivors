#include "witch.h"

Witch::Witch(QString name,
             int width,
             int height,
             double health,
             double maxVelocity,
             double accelerationFactor,
             double reboundFactor,
             int attackWaitTime,
             Weapon *weapon,
             QWidget *parent)
    : Character(name,
                width,
                height,
                health,
                maxVelocity,
                accelerationFactor,
                reboundFactor,
                weapon,
                parent),
      attackWaitTime(attackWaitTime) {};

bool Witch::getValidity() {
    return isValid;
}

int Witch::getAttackWaitTime() {
    return attackWaitTime;
}

void Witch::setValidity() {
    isValid = !isValid;
}

Direction Witch::chooseDirection(Character *player) {
    Direction directions[]
        = {West, South, North, East, Center, NorthEast, NorthWest, SouthEast, SouthWest};

    double minDistance = INF;
    Direction optimalDirection;

    for (auto dir : directions) {
        auto [moveX, moveY] = ~dir;
        int newX = moveX * maxVelocity + this->x(), newY = moveY * maxVelocity + this->y();
        QPointF playerPos(player->x(), player->y()), newPos(newX, newY);
        double distance = MathUtils::euclideanDistance(playerPos, newPos);

        if (distance < minDistance) {
            minDistance = distance;
            optimalDirection = dir;
        }
    }

    return optimalDirection;
}

void Witch::moveActively(Character *player) {
    Direction dir = chooseDirection(player);
    auto [moveX, moveY] = ~dir;
    updateAcceleration(moveX, moveY);
    updateVelocity();
    updatePosition();
}

void Witch::performAttack(Character *player) {
    if (!weapon->isCooldownFinished() || isAttacking || !isValid) {
        return;
    }

    AttackRange *range = weapon->getRange();
    QPointF witchPos = this->getPos(), playerPos = player->getPos();
    if (!range->contains(witchPos, player->geometry())) {
        return;
    }

    double degree = MathUtils::calculateDegree(witchPos, playerPos);
    isAttacking = true;

    QTimer::singleShot(attackWaitTime, this, [this, degree]() {
        if (weapon->getType() == Weapon::WeaponType::Remote) {
            Bullet *bullet = (Bullet *) this->regularAttack(degree);
            emit attackPerformed(bullet);
        } else {
            Slash *slash = (Slash *) this->regularAttack(degree);
            emit attackPerformed(slash);
        }

        isAttacking = false;
    });
}
