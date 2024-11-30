#ifndef GAMELOGIC_H
#define GAMELOGIC_H

#include <QList>
#include <QObject>
#include <QRandomGenerator>
#include <QSet>
#include <QTime>

#include "direction.h"
#include "magicalgirl.h"
#include "map.h"
#include "witch.h"

const double VALID_MAP_MAGNIFICATION = 5;

class GameLogic : public QObject {
    Q_OBJECT

private:
    Map *map;
    MagicalGirl *player;
    QSet<Witch *> witches;
    QSet<Bullet *> bullets;
    QSet<Slash *> slashes;

    bool isBlocked(QPoint pos1, QPoint pos2);

public:
    explicit GameLogic(QObject *parent = nullptr);
    ~GameLogic();

    void startGame();

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
    void handleOutOfBoundObject(QPoint &viewport);

signals:
    void gameOver();

private slots:
    void storeAttack(Attack *attack);
};

#endif // GAMELOGIC_H
