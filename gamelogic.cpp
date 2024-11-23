#include "gamelogic.h"

GameLogic::GameLogic(QObject* parent) : QObject{parent} {
    QWidget* window = qobject_cast<QWidget*>(parent);
    map = new Map(MAP_WIDTH, MAP_HEIGHT, FRICTION, window);
    map->move(MAP_X, MAP_Y);

    luca = new Character("lufuck", 20, 50, 100, 1, 30, 0.1, 0.5, map);
    luca->move(MAP_X, MAP_Y);
}

GameLogic::~GameLogic() {
    delete map;
    delete luca;
}

void GameLogic::startGame() {
}

void GameLogic::updatePlayerPosition(BiDirection moveX, BiDirection moveY) {
    luca->moveActively(moveX, moveY, *map);
    luca->applyFriction(map->getFriction());
}

Map* GameLogic::getMap() const {
    return map;
}

Character* GameLogic::getPlayer() const {
    return luca;
}
