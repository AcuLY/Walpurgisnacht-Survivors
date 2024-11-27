#include "gamelogic.h"

GameLogic::GameLogic(QObject* parent) : QObject{parent} {
    QWidget* window = qobject_cast<QWidget*>(parent);
    map = new Map(FRICTION, window);

    RemoteWeapon* weapon = new RemoteWeapon(50, 10, 3, 100, 1000, true, map);
    //MeleeWeapon* weapon = new MeleeWeapon(5, 200, 100, true, map);
    player = new MagicalGirl("lufuck", 20, 50, 1, 5, 0.1, 0.1, 0, weapon, map);

    connect(player, &Character::attackPerformed, this, &GameLogic::storeAttack);
}

GameLogic::~GameLogic() {
    delete map;
    delete player;
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

QSet<Bullet*>& GameLogic::getBullets() {
    return bullets;
}

QSet<Slash*>& GameLogic::getSlashes() {
    return slashes;
}

void GameLogic::startGame() {
}

void GameLogic::movePlayer(Direction dir) {
    player->moveActively(dir);
    player->applyFriction(map->getFriction());
}

void GameLogic::moveWitches() {
    for (auto witchIt = witches.begin(); witchIt != witches.end(); ++witchIt) {
        (*witchIt)->moveActively(player);
        (*witchIt)->applyFriction(map->getFriction());
    }
}

void GameLogic::moveBullets() {
    for (auto it = bullets.begin(); it != bullets.end(); ++it) {
        (*it)->moveActively();
    }
}

void GameLogic::handleCharacterCollision() {
    player->handleCollision(map);

    for (auto witchIt = witches.begin(); witchIt != witches.end(); ++witchIt) {
        player->handleCollision(*witchIt);
    }

    for (auto witchIt = witches.begin(); witchIt != witches.end(); ++witchIt) {
        for (auto witchIt2 = witches.begin(); witchIt2 != witches.end(); ++witchIt2) {
            if (witchIt != witchIt2) {
                (*witchIt)->handleCollision(*witchIt2);
            }
            (*witchIt)->handleCollision(player);
        }
    }
}

void GameLogic::addWitch(int viewportX, int viewPortY) {
    int ifAddWitch = QRandomGenerator::global()->generate() % 100;
    if (ifAddWitch < 198) {
        return;
    }

    RemoteWeapon* weapon = new RemoteWeapon(3, 10, 3, 3000, 800, false, map);
    //MeleeWeapon* weapon = new MeleeWeapon(5, 200, 100, false, map);
    auto newWitch = new Witch("witch", 30, 50, 5, 1, 1, 0.8, 1000, weapon, map);

    connect(newWitch, &Witch::attackPerformed, this, &GameLogic::storeAttack);

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

Witch* GameLogic::playerSelectTarget() {
    double minDistance = INF;
    MagicalGirl* player = getPlayer();
    AttackRange* range = player->getRange();

    Witch* target = nullptr;
    for (auto it = witches.begin(); it != witches.end(); it++) {
        QPointF playerPos(player->x(), player->y()), witchPos((*it)->x(), (*it)->y());
        double distance = MathUtils::euclideanDistance(playerPos, witchPos);

        if (distance < minDistance && range->contains(playerPos, (*it)->geometry())) {
            minDistance = distance;
            target = (*it);
        }
    }

    return target;
}

void GameLogic::playerAttack() {
    Witch* target = playerSelectTarget();
    player->performAttack(target);
}

void GameLogic::witchAttack() {
    for (auto it = witches.begin(); it != witches.end(); ++it) {
        (*it)->performAttack(player);
    }
}

void GameLogic::handleAttack() {
    for (auto bulletIt = bullets.begin(); bulletIt != bullets.end();) {
        if (!(*bulletIt)->getValidity()) {
            ++bulletIt;
            continue;
        }

        bool bulletHit = false;
        // player
        if (!(*bulletIt)->getPlayerSide() && (*bulletIt)->isHit(player->geometry())) {
            player->receiveDamage((*bulletIt)->getDamage());

            delete *bulletIt;
            bulletIt = bullets.erase(bulletIt);

            continue;
        }

        // witch
        for (auto witchIt = witches.begin(); witchIt != witches.end(); ++witchIt) {
            if ((*bulletIt)->getPlayerSide() && (*bulletIt)->isHit((*witchIt)->geometry())) {
                (*witchIt)->receiveDamage((*bulletIt)->getDamage());

                delete *bulletIt;
                bulletIt = bullets.erase(bulletIt);

                bulletHit = true;
                break;
            }
        }

        if (!bulletHit) {
            ++bulletIt;
        }
    }

    for (auto slashIt = slashes.begin(); slashIt != slashes.end(); ++slashIt) {
        if (!(*slashIt)->getValidity()) {
            continue;
        }

        // player
        if (!(*slashIt)->getPlayerSide() && (*slashIt)->isHit(player->geometry())) {
            player->receiveDamage((*slashIt)->getDamage());
        }

        // witch
        for (auto witchIt = witches.begin(); witchIt != witches.end(); ++witchIt) {
            if ((*slashIt)->getPlayerSide() && (*slashIt)->isHit((*witchIt)->geometry())) {
                (*witchIt)->receiveDamage((*slashIt)->getDamage());
            }
        }
    }
}

void GameLogic::handleDeadWitches() {
    for (auto witchIt = witches.begin(); witchIt != witches.end();) {
        if ((*witchIt)->getHealth() <= 0) {
            delete *witchIt;
            witchIt = witches.erase(witchIt);
        } else {
            ++witchIt;
        }
    }
}

void GameLogic::handleInvalidAttack() {
    for (auto bulletIt = bullets.begin(); bulletIt != bullets.end();) {
        if (!(*bulletIt)->getValidity()) {
            delete *bulletIt;
            bulletIt = bullets.erase(bulletIt);
        } else {
            ++bulletIt;
        }
    }

    for (auto slashIt = slashes.begin(); slashIt != slashes.end();) {
        if (!(*slashIt)->getValidity()) {
            delete *slashIt;
            slashIt = slashes.erase(slashIt);
        } else {
            ++slashIt;
        }
    }
}

void GameLogic::handleOutOfBoundObject(int viewportX, int viewportY) {
    QRect validRange(viewportX - MAP_WIDTH * (VALID_MAP_MAGNIFICATION - 1) / 2,
                     viewportY - MAP_HEIGHT * (VALID_MAP_MAGNIFICATION - 1) / 2,
                     MAP_WIDTH * VALID_MAP_MAGNIFICATION,
                     MAP_HEIGHT * VALID_MAP_MAGNIFICATION);

    for (auto witchIt = witches.begin(); witchIt != witches.end();) {
        if (!validRange.contains(QPoint((*witchIt)->x(), (*witchIt)->y()))) {
            delete *witchIt;
            witchIt = witches.erase(witchIt);
        } else {
            ++witchIt;
        }
    }

    for (auto bulletIt = bullets.begin(); bulletIt != bullets.end();) {
        if (!validRange.contains(QPoint((*bulletIt)->x(), (*bulletIt)->y()))) {
            delete *bulletIt;
            bulletIt = bullets.erase(bulletIt);
        } else {
            ++bulletIt;
        }
    }
}

void GameLogic::storeAttack(Attack* attack) {
    if (auto bullet = dynamic_cast<Bullet*>(attack)) {
        bullets.insert(bullet);
    } else if (auto slash = dynamic_cast<Slash*>(attack)) {
        slashes.insert(slash);
    }
}
