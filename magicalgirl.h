#ifndef MAGICALGIRL_H
#define MAGICALGIRL_H

#include <QTimer>
#include <QWidget>

#include "character.h"
#include "utils.h"
#include "weapon.h"

enum class MagicalGirlEnum { Madoka, Homura, Sayaka, Mami, Kyouko };

const int defaultOutAttackInterval = 5000;
const int defaultRecoverInterval = 1000;
const int defaultRecoverManaCost = 3;

class MagicalGirl : public Character {
    Q_OBJECT

protected:
    int maxMana;
    int currentMana;

    int outAttackInterval = defaultOutAttackInterval;
    int recoverInterval = defaultRecoverInterval;

    bool isInAttack = false;
    QTimer *inAttackTimer;

    bool isOnRecoverCooldown = false;
    QTimer *recoverTimer;

    int recoverManaCost = defaultRecoverManaCost;
    double recoverRate;

    Weapon *weapon;

public:
    explicit MagicalGirl(QString name,
                         int width,
                         int height,
                         double maxHealth,
                         double maxVelocity,
                         double accelerationFactor,
                         double attackMoveDecayFactor,
                         double reboundFactor,
                         int maxMana,
                         double recoverRate,
                         Weapon *weapon,
                         QWidget *parent = nullptr);

    static MagicalGirl *loadMagicalGirlFromJson(MagicalGirlEnum playerSelection, Map *map);

    int getCurrentMana() const;
    double getRecoverRate() const;
    bool getIsReadyToRecover() const;

    void receiveDamage(double damage);

    void recoverHealth();

signals:
};

#endif // MAGICALGIRL_H
