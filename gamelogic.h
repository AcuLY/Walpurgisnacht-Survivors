#ifndef GAMELOGIC_H
#define GAMELOGIC_H

#include <QList>
#include <QObject>
#include <QRandomGenerator>
#include <QSet>

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

public:
    explicit GameLogic(QObject *parent = nullptr);
    ~GameLogic();

    void startGame();

    Map *getMap() const;
    MagicalGirl *getPlayer() const;
    QSet<Witch *> &getWitches();
    QSet<Bullet *> &getBullets();

    void movePlayer(Direction dir);
    void moveWitches();
    void moveBullets();

    void handleCharacterCollision();

    void addWitch(int viewportX, int viewPortY);

    Witch *playerSelectTarget();
    void playerAttack();

    void handleBulletCollision();

    void handleOutOfBoundObject(int viewportX, int viewportY);

signals:
    void gameOver();
};

#endif // GAMELOGIC_H
