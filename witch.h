#ifndef WITCH_H
#define WITCH_H

#include <QRandomGenerator>
#include <QWidget>

#include "character.h"
#include "direction.h"
#include "utils.h"

const double griefSeedFragmentPossibility = 0.1;
const int griefSeedFragmentMana = 2;

const double spawnProbLow = 0.2;     // 只生成 witch1 的进度
const double spawnProbMidLow = 0.5;  // 进度在20%到50%之间时，生成怪物的概率
const double spawnProbMidHigh = 0.8; // 进度在50%到80%之间时，生成怪物的概率
const double spawnProbHigh = 1.0;  // 进度大于80%时，生成怪物的概率

const double weakMonsterProb = 0.3;   // 生成弱怪的概率
const double midMonsterProb = 0.4;    // 生成中等怪的概率
const double strongMonsterProb = 0.8; // 生成强怪的概率

const double thresholdWeakMid = 0.75; // 用于分隔弱怪和中等怪的阈值

enum class WitchEnum { witch1, witch2, witch3, witch4, noWitch = -1 };

class Witch : public Character {
    Q_OBJECT

public:
    explicit Witch(QString name,
                   QString texturePath,
                   QString texturePathHurt,
                   int width,
                   int height,
                   double maxHealth,
                   double maxVelocity,
                   double accelerationFactor,
                   double attackMoveDecayFactor,
                   double reboundFactor,
                   int exp,
                   int attackWaitTime,
                   bool isPlayerDirected,
                   Weapon *weapon,
                   QWidget *parent = nullptr);

    static WitchEnum chooseWitch(double progress);
    static bool ifDropGriefSeedFragment();
    static Witch *loadWitchFromJson(WitchEnum witchIndex, Map *map);

    int getExp();
    bool getValidity();
    int getAttackWaitTime();

    void setValidity();

    void moveActively(Direction dir);
    void blocked();

    void performAttack(Character *player);

protected:
    int exp;

    Direction prevDir = Direction::Center;

    bool isValid = true;

    int attackWaitTime;

    bool isPlayerDirected;

signals:
};

#endif // WITCH_H
