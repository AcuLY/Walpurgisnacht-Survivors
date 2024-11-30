#include "gamelogic.h"

GameLogic::GameLogic(QObject* parent) : QObject{parent} {
    QWidget* window = qobject_cast<QWidget*>(parent);
    map = new Map(FRICTION, window);
    QPoint initViewpoint(10, 25);
    map->updateObstacle(initViewpoint);

    //RemoteWeapon* weapon = new RemoteWeapon(50, 10, 3, 10, 1000, true, map);
    MeleeWeapon* weapon = new MeleeWeapon(5, 200, 100, true, map);
    player = new MagicalGirl("lufuck", 5, 10, 1, 5, 0.5, 0.1, 0, weapon, map);

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
        Direction dir = map->getFlow((*witchIt)->getPos());
        (*witchIt)->moveActively(dir);
    }
}

void GameLogic::moveBullets() {
    for (auto it = bullets.begin(); it != bullets.end(); ++it) {
        (*it)->moveActively();
    }
}

void GameLogic::updateMapFlowField() {
    static QTime lastCallTime = QTime::currentTime();
    QTime currentTime = QTime::currentTime();

    int interval = 100;

    if (lastCallTime.msecsTo(currentTime) >= interval) {
        lastCallTime = currentTime;
        map->updateFlowField(player->getPos());
    }
}

void GameLogic::handleCharacterCollision() {
    player->handleCollision(map);
    for (auto witchIt = witches.begin(); witchIt != witches.end(); ++witchIt) {
        player->handleCollision(*witchIt);
    }
    player->handleCollision(map);

    for (auto witchIt = witches.begin(); witchIt != witches.end(); ++witchIt) {
        (*witchIt)->handleCollision(map);
        (*witchIt)->handleCollision(player);
        for (auto witchIt2 = witches.begin(); witchIt2 != witches.end(); ++witchIt2) {
            if (witchIt != witchIt2) {
                (*witchIt)->handleCollision(*witchIt2);
            }
        }
        (*witchIt)->handleCollision(map);
    }
}

void GameLogic::addWitch(QPoint& viewport) {
    int ifAddWitch = QRandomGenerator::global()->generate() % 100;
    if (ifAddWitch < 80) {
        return;
    }

    RemoteWeapon* weapon = new RemoteWeapon(3, 10, 3, 3000, 800, false, map);
    //MeleeWeapon* weapon = new MeleeWeapon(5, 200, 10, false, map);
    auto newWitch = new Witch("witch", 10, 20, 1, 1, 1, 0.5, 1000, weapon, map);

    connect(newWitch, &Witch::attackPerformed, this, &GameLogic::storeAttack);

    int edge = QRandomGenerator::global()->bounded(0, 4);
    int x = 0, y = 0;
    Direction dir = Direction::Center;

    switch (edge) {
        case 0: // 上边界
            x = QRandomGenerator::global()->bounded(0, MAP_WIDTH) + viewport.x();
            y = viewport.y() - newWitch->geometry().height();
            dir = Direction::North;
            break;

        case 1: // 右边界
            x = viewport.x() + MAP_WIDTH;
            y = QRandomGenerator::global()->bounded(0, MAP_HEIGHT) + viewport.y();
            dir = Direction::East;
            break;

        case 2: // 下边界
            x = QRandomGenerator::global()->bounded(0, MAP_WIDTH) + viewport.x();
            y = viewport.y() + MAP_HEIGHT;
            dir = Direction::South;
            break;

        case 3: // 左边界
            x = viewport.x() - newWitch->geometry().width();
            y = QRandomGenerator::global()->bounded(0, MAP_HEIGHT) + viewport.y();
            dir = Direction::West;
            break;
    }

    newWitch->move(x, y);
    int width = newWitch->geometry().width(), height = newWitch->geometry().height();
    QPainterPath partialPath = map->getPartialPath(QPoint(x, y), QPoint(x + width, y + height));
    auto [moveX, moveY] = ~dir;
    while (partialPath.intersects(newWitch->geometry())) {
        x += moveX * GRID_SIZE;
        y += moveY * GRID_SIZE;
        newWitch->move(x, y);
        partialPath = map->getPartialPath(QPoint(x, y), QPoint(x + width, y + height));
    }

    witches.insert(newWitch);
}

bool GameLogic::isBlocked(QPoint pos1, QPoint pos2) {
    QPainterPath partialPath = map->getPartialPath(pos1, pos2);
    return !partialPath.isEmpty();
}

Witch* GameLogic::playerSelectTarget() {
    double minDistance = INF;
    MagicalGirl* player = getPlayer();
    AttackRange* range = player->getRange();

    Witch* target = nullptr;
    for (auto it = witches.begin(); it != witches.end(); it++) {
        if (player->getWeaponType() == Weapon::WeaponType::Remote
            && isBlocked(player->getPos(), (*it)->getPos())) {
            continue;
        }

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
        if ((*it)->getWeaponType() == Weapon::WeaponType::Remote
            && isBlocked(player->getPos(), (*it)->getPos())) {
            continue;
        }

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

void GameLogic::handleBulletMapCollision() {
    for (auto it = bullets.begin(); it != bullets.end();) {
        QPoint curPos = (*it)->getPos(), prevPos = (*it)->getPrevPos();
        QPainterPath partialPath = map->getPartialPath(prevPos, curPos);

        if ((*it)->isHit(partialPath)) {
            delete *it;
            it = bullets.erase(it);
        } else {
            ++it;
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

void GameLogic::handleOutOfBoundObject(QPoint& viewport) {
    QRect validRange(viewport.x() - MAP_WIDTH * (VALID_MAP_MAGNIFICATION - 1) / 2,
                     viewport.y() - MAP_HEIGHT * (VALID_MAP_MAGNIFICATION - 1) / 2,
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
