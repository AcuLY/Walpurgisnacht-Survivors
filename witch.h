#ifndef WITCH_H
#define WITCH_H

#include <QRandomGenerator>
#include <QWidget>

#include "character.h"
#include "direction.h"

const double GRIEF_SEED_FRAGMENT_POSSIBILITY = 0.1; // 悲叹之种爆率
const int GRIEF_SEED_FRAGMENT_MANA = 3;             // 悲叹之种蓝量

const double PROGRESS_1_THRES = 0.2; // 刚开始只生成 witch1 的进度
const double PROGRESS_2_THRES = 0.5; // 进度在 20% 到 50% 之间时只生成前两种怪物
const double PROGRESS_3_THRES = 0.8; // 进度在 50% 到 80% 之间时生成前三种怪物
const double PROGRESS_4_THRES = 1.0; // 进度大于 80% 生成所有怪物

const double WITCH_1_PROB = 0.4; // witch1 的概率为 0.4
const double WITCH_2_PROB = 0.1; // witch2 的概率为 0.1
const double WITCH_3_PROB = 0.3; // witch3 的概率为 0.3
const double WITCH_4_PROB = 0.2; // witch4 的概率为 0.2

enum class WitchEnum { witch1, witch2, witch3, witch4, noWitch = -1 }; // 怪物类型枚举

class Witch : public Character { // 使魔，即敌人类
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

    static WitchEnum chooseWitch(double progress); // 选择是否生成怪物，并选择生成的类型
    static bool ifDropGriefSeedFragment();         // 是否掉悲叹之种
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
