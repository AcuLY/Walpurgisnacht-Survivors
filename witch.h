#ifndef WITCH_H
#define WITCH_H

#include <QWidget>

#include "character.h"
#include "direction.h"
#include "utils.h"

class Witch : public Character {
    Q_OBJECT

public:
    explicit Witch(QString name,
                   int width,
                   int height,
                   double health,
                   double maxVelocity,
                   double accelerationFactor,
                   double reboundFactor,
                   int attackWaitTime,
                   Weapon *weapon,
                   QWidget *parent = nullptr);

    bool getValidity();
    int getAttackWaitTime();

    void setValidity();

    void moveActively(Character *player);

    void performAttack(Character *player);

protected:
    bool isValid = true;
    int attackWaitTime;

    Direction chooseDirection(Character *player);

signals:
};

#endif // WITCH_H
