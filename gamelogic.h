#ifndef GAMELOGIC_H
#define GAMELOGIC_H

#include <QList>
#include <QObject>
#include <QRandomGenerator>
#include <QSet>
#include <QThread>
#include <QTime>

#include "direction.h"
#include "enhancement.h"
#include "global.h"
#include "loot.h"
#include "magicalgirl.h"
#include "map.h"
#include "soundmanager.h"
#include "witch.h"

const double SURVIVAL_TIME = 600.0; // 获胜所需时间

const int INIT_NEXT_LEVEL_EXP = 30;                // 升到第二级需要的经验
const double NEXT_LEVEL_EXP_INCREASE_FACTOR = 1.1; // 升下一级需要的经验系数

class GameLogic : public QObject { // 游戏逻辑类
    Q_OBJECT

private:
    Global *global;
    SoundManager *soundManager;

    MagicalGirl *player;
    QPoint lastPlayerPos; // 上次玩家位置，如果玩家两次在同一个格子里则不更新流场

    QSet<Witch *> witches;

    Map *map;

    QSet<Bullet *> bullets;
    QSet<Slash *> slashes;

    QSet<Loot *> loots;

    int level = 1;
    int currentExp = 0;
    int nextLevelExp = INIT_NEXT_LEVEL_EXP;
    EnhancementManager *enhancementManager;
    QVector<Enhancement *> randomEnhancements; // 存储当前的三个局内强化

    double survivalTimeLeft = SURVIVAL_TIME; // 剩余生存时间

    QThread *flowFieldUpdateThread; // 更新流场用的线程

    QVector<double> playerSelectTarget();     // 玩家索敌
    bool isBlocked(QPoint pos1, QPoint pos2); // 两个点是否被障碍物阻挡

public:
    explicit GameLogic(Global *global,
                       SoundManager *soundManager,
                       MagicalGirlEnum playerSelection,
                       QObject *parent = nullptr);
    ~GameLogic();

    void updateSurvivalTime();
    QString getSurvivalTimeString() const; // 返回 MM:SS 的字符串

    Map *getMap() const;
    MagicalGirl *getPlayer() const;
    const QSet<Witch *> &getWitches() const;
    const QSet<Bullet *> &getBullets() const;
    const QSet<Slash *> &getSlashes() const;
    const QSet<Loot *> &getLoots() const;

    // 获取百分比
    double getHpPercent() const;
    double getMpPercent() const;
    double getExpPercent() const;

    // 获取用于渲染的字符串
    QString getHpText() const;
    QString getMpText() const;
    QString getExpText() const;
    QString getLevelText() const;

    bool isPlayerReceivingDamage() const; // 玩家是否在受伤

    void movePlayer(Direction dir); // 玩家移动
    void moveWitches();
    void moveBullets();
    void moveLoots();

    void updateMapFlowField(); // 更新地图流场

    void handleCharacterCollision(); // 处理角色碰撞

    void handleInRangeLoots(); // 处理掉落书拾取

    void addWitch(QPoint &viewport); // 生成敌人

    void playerAttack();
    void witchAttack();

    void handleAttack();             // 处理攻击实体
    void handleBulletMapCollision(); // 处理子弹地图碰撞

    void handleDeadWitches();        // 处理敌人死亡
    void handleInvalidAttack();      // 处理失效的攻击实体
    void handleOutOfBoundryObject(); // 处理超出游戏最大边界的实体

    void handlePlayerHealthRecover(); // 玩家回血
    void handlePlayerManaRecover();   // 玩家回蓝
    void checkIfPlayerDie();          // 检查游戏结束

    void updateExp(int exp);
    void handleLevelUp();

    void handleRewards(); // 金币奖励

signals:
    void gameWin();
    void gameLose();

    void levelUp(QVector<Enhancement *> &enhancements); // 开始选择局内强化
    void levelUpFinish();                               // 结束选择局内强化

private slots:
    void storeAttack(Attack *attack); // 将攻击实体存入

public slots:
    void enhancementSelected(int index); // 已选择强化
};

#endif // GAMELOGIC_H
