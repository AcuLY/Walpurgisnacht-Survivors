#ifndef MAGICALGIRL_H
#define MAGICALGIRL_H

#include <QTimer>
#include <QWidget>

#include "character.h"
#include "utils.h"
#include "weapon.h"
#include "witch.h"

enum class MagicalGirlEnum { Madoka, Homura, Sayaka, Mami, Kyouko };

const int defaultOutAttackInterval = 5000; // 脱战时间
const int defaultIFrameInterval = 2000;    // 无敌帧时间
const int defaultRecoverInterval = 1000;   // 回血的间隔
const int defaultRecoverManaCost = 3;      // 回血消耗的蓝
const int defaultPickRangeSize = 300;      // 拾取范围
const int multiAttackInterval = 32;        // 连击间隔

class MagicalGirl : public Character {
    Q_OBJECT

protected:
    int maxMana;
    int currentMana;

    int outAttackInterval = defaultOutAttackInterval;
    int recoverInterval = defaultRecoverInterval;

    bool isInAttack = false;
    QTimer *inAttackTimer;

    bool isInvincible = false;
    int invincibleFrameInterval = defaultIFrameInterval;
    QTimer *invincibleTimer;

    bool isOnRecoverCooldown = false;
    QTimer *recoverTimer;

    int recoverManaCost = defaultRecoverManaCost;
    double recoverRate;

    QVector<double> targetDegrees;
    QVector<double>::Iterator targetDegreesIt;
    double currentTargetDegree = INF;
    int attackTimeLeft;
    int multiAttackTime = 3; // 连击次数
    QTimer *multiAttackTimer;

    Weapon *weapon;

    int pickRangeSize = defaultPickRangeSize;
    CircleRange *pickRange = new CircleRange(defaultPickRangeSize);

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

    CircleRange *getPickRange() const;

    void performSingleAttack(double targetDegree);
    void performAttack(QVector<double> targetWitchDegrees);

    void receiveDamage(double damage);

    void recoverHealth();
    void recoverMana(int mana);

signals:
};

#endif // MAGICALGIRL_H
