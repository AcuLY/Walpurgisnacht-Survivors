#ifndef GAMELOGIC_H
#define GAMELOGIC_H

#include <QList>
#include <QObject>
#include <QRandomGenerator>
#include <QSet>
#include <QTime>

#include "direction.h"
#include "enhancement.h"
#include "magicalgirl.h"
#include "map.h"
#include "witch.h"

class GameLogic : public QObject {
    Q_OBJECT

private:
    MagicalGirl *player;
    QSet<Witch *> witches;

    Map *map;

    QSet<Bullet *> bullets;
    QSet<Slash *> slashes;

    int level = 1;
    int currentExp = 0;
    int nextLevelExp = 100;
    EnhancementManager *enhancementManager;

    bool isBlocked(QPoint pos1, QPoint pos2);

public:
    explicit GameLogic(QObject *parent = nullptr);
    ~GameLogic();

    void startGame();

    int getLevel();
    int getCurrentExp();
    int getNextLevelExp();

    Map *getMap() const;
    MagicalGirl *getPlayer() const;
    QSet<Witch *> &getWitches();
    QSet<Bullet *> &getBullets();
    QSet<Slash *> &getSlashes();

    void movePlayer(Direction dir);
    void moveWitches();
    void moveBullets();
    void updateMapFlowField();

    void handleCharacterCollision();

    void addWitch(QPoint &viewport);

    Witch *playerSelectTarget();
    void playerAttack();

    void witchAttack();

    void handleAttack();
    void handleBulletMapCollision();

    void handleDeadWitches();
    void handleInvalidAttack();
    void handleOutOfBoundryObject();

    void updateExp(int exp);
    void handleLevelUp();

signals:
    void gameOver();

private slots:
    void storeAttack(Attack *attack);
};

#endif // GAMELOGIC_H
