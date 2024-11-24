#include "magicalgirl.h"

MagicalGirl::MagicalGirl(QString name,
                         int width,
                         int height,
                         double health,
                         double damage,
                         double maxVelocity,
                         double accelerationFactor,
                         double reboundFactor,
                         int mana,
                         QWidget *parent)
    : Character(name,
                width,
                height,
                health,
                damage,
                maxVelocity,
                accelerationFactor,
                reboundFactor,
                parent),
      mana(mana) {};
