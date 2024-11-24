#ifndef GAMELOGIC_H
#define GAMELOGIC_H

#include <QObject>
#include <QRandomGenerator>
#include <QSet>

#include "direction.h"
#include "magicalgirl.h"
#include "map.h"
#include "witch.h"

class GameLogic : public QObject {
    Q_OBJECT

private:
    Map *map;
    MagicalGirl *player;
    QSet<Witch *> witches;

public:
    explicit GameLogic(QObject *parent = nullptr);
    ~GameLogic();

    void startGame();

    void movePlayer(Direction dir);
    void moveWitches();

    void addWitch(int viewportX, int viewPortY);

    Map *getMap() const;
    MagicalGirl *getPlayer() const;
    QSet<Witch *> &getWitches();

signals:
    void gameOver();
};

#endif // GAMELOGIC_H
