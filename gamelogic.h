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
#include "enhancementwindow.h"
#include "loot.h"
#include "magicalgirl.h"
#include "map.h"
#include "witch.h"

const double survivalTime = 600.0;

class GameLogic : public QObject {
    Q_OBJECT

private:
    MagicalGirl *player;
    QPoint lastPlayerPos; // 上次玩家位置，如果玩家两次在同一个格子里则不更新流场

    QSet<Witch *> witches;

    Map *map;

    QSet<Bullet *> bullets;
    QSet<Slash *> slashes;

    QSet<Loot *> loots;

    int level = 1;
    int currentExp = 0;
    int nextLevelExp = 100;
    EnhancementManager *enhancementManager;
    QVector<Enhancement *> randomEnhancements;

    double survivalTimeLeft = survivalTime;

    QThread *workerThread;

    bool isBlocked(QPoint pos1, QPoint pos2);

public:
    explicit GameLogic(MagicalGirlEnum playerSelection, QObject *parent = nullptr);
    ~GameLogic();

    void startGame();

    void updateSurvivalTime();

    int getLevel();
    int getCurrentExp();
    int getNextLevelExp();

    Map *getMap() const;
    MagicalGirl *getPlayer() const;
    QSet<Witch *> &getWitches();
    QSet<Bullet *> &getBullets();
    QSet<Slash *> &getSlashes();
    QSet<Loot *> &getLoots();

    void movePlayer(Direction dir);
    void moveWitches();
    void moveBullets();
    void moveLoots();

    void updateMapFlowField();

    void handleCharacterCollision();

    void handleInRangeLoots();

    void addWitch(QPoint &viewport);

    QVector<double> playerSelectTarget();
    void playerAttack();

    void witchAttack();

    void handleAttack();
    void handleBulletMapCollision();

    void handleDeadWitches();
    void handleInvalidAttack();
    void handleOutOfBoundryObject();

    void handlePlayerHealthRecover();
    void handlePlayerManaRecover();
    void checkIfPlayerDie();

    void updateExp(int exp);
    void handleLevelUp();

signals:
    void gameWin();
    void gameOver();

    void levelUp(QVector<Enhancement *> &enhancements);
    void levelUpFinish();

private slots:
    void storeAttack(Attack *attack);

public slots:
    void enhancementSelected(int index);
};

#endif // GAMELOGIC_H
