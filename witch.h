#ifndef WITCH_H
#define WITCH_H

#include <QRandomGenerator>
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
                   double attackMoveDecayFactor,
                   double reboundFactor,
                   int exp,
                   int attackWaitTime,
                   Weapon *weapon,
                   QWidget *parent = nullptr);

    static int chooseWitch(double progress);
    static Witch *loadWitchFromJson(int typeIndex, Map *map);

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
