#include "magicalgirl.h"

MagicalGirl::MagicalGirl(QString name,
                         int width,
                         int height,
                         double health,
                         double maxVelocity,
                         double accelerationFactor,
                         double reboundFactor,
                         int mana,
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
      mana(mana) {
}

void MagicalGirl::setMaxHealth(double newMaxHealth) {
    maxHealth = newMaxHealth;
};
