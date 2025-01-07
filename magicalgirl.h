#ifndef MAGICALGIRL_H
#define MAGICALGIRL_H

#include <QWidget>

#include "character.h"
#include "weapon.h"

enum class MagicalGirlEnum { Madoka, Homura, Sayaka, Mami, Kyouko };

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

    void setMaxHealth(double newMaxHealth);

signals:
};

#endif // MAGICALGIRL_H
