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

void Witch::moveActively(Direction dir) {
    // 防卡死
    if (dir == Direction::Center) {
        dir = prevDir;
    } else {
        prevDir = dir;
    }

    auto [moveX, moveY] = ~dir;
    updateAcceleration(moveX, moveY);
    updateVelocity();
    updatePosition();
}
