#include "gamelogic.h"

GameLogic::GameLogic(QObject* parent) : QObject{parent} {
    QWidget* window = qobject_cast<QWidget*>(parent);
    map = new Map(FRICTION, window);

    player = new MagicalGirl("lufuck", 20, 50, 100, 1, 20, 0.4, 0.1, 0, map);
}

GameLogic::~GameLogic() {
    delete map;
    delete player;
    for (auto it = witches.begin(); it != witches.end(); ++it) {
        delete *it;
    }
}

Map* GameLogic::getMap() const {
    return map;
}

MagicalGirl* GameLogic::getPlayer() const {
    return player;
}

QSet<Witch*>& GameLogic::getWitches() {
    return witches;
}

void GameLogic::startGame() {
}

void GameLogic::movePlayer(Direction dir) {
    player->moveActively(dir);
    player->applyFriction(map->getFriction());

    for (auto it = witches.begin(); it != witches.end(); ++it) {
        player->handleCollision(*it);
    }
}

void GameLogic::moveWitches() {
    for (auto it = witches.begin(); it != witches.end(); ++it) {
        (*it)->moveActively(player);

        for (auto it2 = witches.begin(); it2 != witches.end(); ++it2) {
            if (it2 != it) {
                (*it)->handleCollision(*it2);
            }
        }
    }
}

void GameLogic::addWitch(int viewportX, int viewPortY) {
    int ifAddWitch = QRandomGenerator::global()->generate() % 100;
    if (ifAddWitch < 99) {
        return;
    }

    Witch* newWitch = new Witch("lufuck", 20, 50, 100, 1, 30, 0.1, 1.8, map);

    int edge = QRandomGenerator::global()->bounded(0, 4);
    int x = 0, y = 0;
    switch (edge) {
        case 0: // 上边界
            x = QRandomGenerator::global()->bounded(0, MAP_WIDTH) + viewportX;
            y = viewPortY - newWitch->geometry().height();
            break;

        case 1: // 右边界
            x = viewportX + MAP_WIDTH;
            y = QRandomGenerator::global()->bounded(0, MAP_HEIGHT) + viewPortY;
            break;

        case 2: // 下边界
            x = QRandomGenerator::global()->bounded(0, MAP_WIDTH) + viewportX;
            y = viewPortY + MAP_HEIGHT;
            break;

        case 3: // 左边界
            x = viewportX - newWitch->geometry().width();
            y = QRandomGenerator::global()->bounded(0, MAP_HEIGHT) + viewPortY;
            break;
    }
    newWitch->move(x, y);

    witches.insert(newWitch);
}
