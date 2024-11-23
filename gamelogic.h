#ifndef GAMELOGIC_H
#define GAMELOGIC_H

#include <QObject>

#include "character.h"
#include "direction.h"
#include "map.h"

class GameLogic : public QObject {
    Q_OBJECT

private:
    Map *map;
    Character *luca;

public:
    explicit GameLogic(QObject *parent = nullptr);
    ~GameLogic();

    void startGame();

    void updatePlayerPosition(BiDirection moveX, BiDirection moveY);

    Map *getMap() const;
    Character *getPlayer() const;

signals:
    void gameOver();
};

#endif // GAMELOGIC_H
