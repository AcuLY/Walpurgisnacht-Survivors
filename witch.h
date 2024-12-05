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
                   double maxHealth,
                   double maxVelocity,
                   double accelerationFactor,
                   double reboundFactor,
                   int exp,
                   int attackWaitTime,
                   Weapon *weapon,
                   QWidget *parent = nullptr);

    int getExp();
    bool getValidity();
    int getAttackWaitTime();

    void setValidity();

    void moveActively(Direction dir);

    void performAttack(Character *player);

protected:
    int exp;
    Direction prevDir;
    bool isValid = true;
    int attackWaitTime;

signals:
};

#endif // WITCH_H
