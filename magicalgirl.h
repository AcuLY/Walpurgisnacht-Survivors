#ifndef MAGICALGIRL_H
#define MAGICALGIRL_H

#include <QTimer>
#include <QWidget>

#include "character.h"
#include "utils.h"
#include "weapon.h"

enum class MagicalGirlEnum { Madoka, Homura, Sayaka, Mami, Kyouko }; // 角色枚举

const int DEFAULT_OUT_ATTACK_INTERVAL = 5000;  // 脱战时间
const int DEFAULT_I_FRAME_INTERVAL = 1000;     // 无敌帧时间
const int DEFAULT_RECOVER_INTERVAL = 3000;     // 回血的间隔
const int DEFAULT_RECOVER_MANA_COST = 3;       // 回血消耗的蓝
const int DEFAULT_PICK_RANGE_SIZE = 100;       // 拾取范围
const int DEFAULT_MULTI_ATTACK_INTERVAL = 128; // 连击间隔
const int DEFAULT_MULTI_ATTACK_NUM = 2;        // 默认连击数

const int MIN_RECOVER_HEALTH_MANA = 5; // 自动回血的最小蓝量，小于该值就不回血

class MagicalGirl : public Character { // 魔法少女，即玩家类
    Q_OBJECT

protected:
    int maxMana;     // 最大蓝量
    int currentMana; // 当前蓝量

    int outAttackInterval = DEFAULT_OUT_ATTACK_INTERVAL; // 脱战所需时间
    int recoverInterval = DEFAULT_RECOVER_INTERVAL;      // 回血间隔时间

    bool isInAttack = false; // 是否在战斗中
    QTimer *inAttackTimer;   // 计时器

    bool isInvincible = false;                              // 是否处于无敌状态
    int invincibleFrameInterval = DEFAULT_I_FRAME_INTERVAL; // 无敌时间
    QTimer *invincibleTimer;

    bool isOnRecoverCooldown = false; // 是否还在回血的间隔
    QTimer *recoverTimer;

    int recoverManaCost = DEFAULT_RECOVER_MANA_COST; // 回血需要的蓝耗
    double recoverRate;                              // 每次回血量与总血量的比值

    int manaRecoverBonus = 0; // 额外回蓝量
    int experienceBonus = 0;  // 额外经验量

    QVector<double> targetDegrees; // 能攻击到的所有目标的角度
    QVector<double>::Iterator targetDegreesIt;
    bool targetDegreeUpdated = false;                        // 角度列表是否被更新
    double currentTargetDegree = INF;                        // 当前锁定的目标的角度
    int attackTimeLeft;                                      // 还要连击几次
    int multiAttackInterval = DEFAULT_MULTI_ATTACK_INTERVAL; // 连击间隔
    int multiAttackTime = DEFAULT_MULTI_ATTACK_NUM; // 连击次数
    QTimer *multiAttackTimer;

    int pickRangeSize = DEFAULT_PICK_RANGE_SIZE; // 拾取范围
    CircleRange *pickRange = new CircleRange(DEFAULT_PICK_RANGE_SIZE);

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
    ~MagicalGirl();

    static MagicalGirl *loadMagicalGirlFromJson(MagicalGirlEnum playerSelection, Map *map);

    void render(QPainter *painter);

    int getCurrentMana() const;
    int getMaxMana() const;
    bool getIsReadyToRecover() const;
    int getExperienceBonus() const;
    CircleRange *getPickRange() const;

    void initHealthAndMana();

    void performAttack(QVector<double> targetWitchDegrees); // 朝目标攻击

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
    void performSingleAttack(double targetDegree); // 若干次连击中的一次攻击
    void multiAttack();                            // 发起连击

signals:
    void healed();
};

#endif // MAGICALGIRL_H
