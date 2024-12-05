#include "magicalgirl.h"

MagicalGirl::MagicalGirl(QString name,
                         int width,
                         int height,
                         double maxHealth,
                         double maxVelocity,
                         double accelerationFactor,
                         double reboundFactor,
                         int mana,
                         Weapon *weapon,
                         QWidget *parent)
    : Character(name,
                width,
                height,
                maxHealth,
                maxVelocity,
                accelerationFactor,
                reboundFactor,
                weapon,
                parent),
      mana(mana) {
}

void MagicalGirl::setMaxHealth(double newMaxHealth) {
    maxHealth = newMaxHealth;
};

Madoka::Madoka(QString name,
               int width,
               int height,
               double maxHealth,
               double maxVelocity,
               double accelerationFactor,
               double reboundFactor,
               int mana,
               Weapon *weapon,
               QWidget *parent)
    : MagicalGirl(name,
                  width,
                  height,
                  maxHealth,
                  maxVelocity,
                  accelerationFactor,
                  reboundFactor,
                  mana,
                  weapon,
                  parent) {
}

AttackRange *Madoka::useDodgeSkill() {
    if (dodgeSkill->getIsOnCooldown()) {
        return nullptr;
    }
    if (mana < dodgeSkill->getManaCost()) {
        return nullptr;
    }

    QVector<double> effectParameters = dodgeSkill->getEffectParameters();
    double velocityBoost = effectParameters[0];
    double backwardAcceleration = effectParameters[1];

    velocity.setX(velocity.x() + velocityBoost * cos(facingDegree));
    velocity.setY(velocity.y() + velocityBoost * sin(facingDegree));

    acceleration.setX(-backwardAcceleration * cos(facingDegree));
    acceleration.setY(-backwardAcceleration * sin(facingDegree));

    return nullptr;
}
