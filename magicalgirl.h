#ifndef MAGICALGIRL_H
#define MAGICALGIRL_H

#include <QWidget>

#include "character.h"
#include "weapon.h"

class MagicalGirl : public Character {
    Q_OBJECT

protected:
    int mana;

    Weapon *weapon;

public:
    explicit MagicalGirl(QString name,
                         int width,
                         int height,
                         double health,
                         double maxVelocity,
                         double accelerationFactor,
                         double reboundFactor,
                         int mana,
                         Weapon *weapon,
                         QWidget *parent = nullptr);

signals:
};

#endif // MAGICALGIRL_H
