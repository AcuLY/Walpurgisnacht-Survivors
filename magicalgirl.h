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
const int defaultRecoverInterval = 3000;   // 回血的间隔
const int defaultRecoverManaCost = 3;      // 回血消耗的蓝
const int defaultPickRangeSize = 100;      // 拾取范围
const int defaultMultiAttackInterval = 128; // 连击间隔

const int minRecoverHealthMana = 5; // 自动回血的最小蓝量，小于该值就不回血

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

    int manaRecoverBonus = 0;
    int experienceBonus = 0;

    QVector<double> targetDegrees;
    QVector<double>::Iterator targetDegreesIt;
    bool targetDegreeUpdated = false;
    double currentTargetDegree = INF;
    int attackTimeLeft;
    int multiAttackInterval = defaultMultiAttackInterval;
    int multiAttackTime = 8; // 连击次数
    QTimer *multiAttackTimer;

    int pickRangeSize = defaultPickRangeSize;
    CircleRange *pickRange = new CircleRange(defaultPickRangeSize);

public:
    explicit MagicalGirl(QString name,
                         QString texturePath,
                         QString texturePathHurt,
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
    int getMaxMana() const;
    double getRecoverRate() const;
    bool getIsReadyToRecover() const;
    int getExperienceBonus() const;

    CircleRange *getPickRange() const;

    void initHealthAndMana();

    void performSingleAttack(double targetDegree);
    void performAttack(QVector<double> targetWitchDegrees);

    void receiveDamage(double damage);

    void recoverHealth();
    void recoverMana(int mana);

    void increaseAttackSpeed(double value);        // 增加攻速，乘法
    void increaseMultiAttackTime(int value);       // 增加连击次数，加法
    void increaseMaxHealth(int value);             // 增加最大生命，加法
    void decreaseRecoverInterval(double value);    // 增加回血频率，乘法
    void increaseRecoverRate(double value);        // 增加每次回血量，加法
    void decreaseRecoverManaCost(int value);       // 减少回血耗蓝，减法
    void decreaseOutAttackInterval(double value);  // 减少脱战时间，乘法
    void increaseInvincibleInterval(double value); // 增加无敌时间，乘法
    void increaseManaRecoverBonus(int value);      // 增加回蓝量，加法
    void increaseMaxMana(int value);               // 增加最大蓝量，加法
    void increaseMaxVelocity(int value);           // 增加最大移速，加法
    void decreaseAttackDecay(double value);        // 减少攻击移速衰减，加法
    void increasePickRange(double value);          // 增加拾取范围，乘法
    void increaseExperienceBonus(int value);       // 增加经验获取，加法

private:
    void multiAttack();
};

#endif // MAGICALGIRL_H
