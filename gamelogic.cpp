#include "gamelogic.h"
#include <queue>

GameLogic::GameLogic(Global* global,
                     SoundManager* soundManager,
                     MagicalGirlEnum playerSelection,
                     QObject* parent)
    : QObject{parent}, global(global), soundManager(soundManager) {
    QWidget* window = qobject_cast<QWidget*>(parent);

    workerThread = new QThread;

    map = new Map(FRICTION, window);

    player = MagicalGirl::loadMagicalGirlFromJson(playerSelection, map);
    connect(player, &Character::attackPerformed, this, &GameLogic::storeAttack); // 存放角色的子弹等
    connect(player, &Character::damageReceived, this, [this] {
        this->soundManager->playSfx("hit");
    });
    connect(player, &Character::attackPerformed, this, [this] {
        if (player->getWeaponType() == Weapon::WeaponType::Remote) {
            this->soundManager->playSfx("shoot");
        } else {
            this->soundManager->playSfx("slash");
        }
    });
    connect(player, &MagicalGirl::healed, this, [this] { this->soundManager->playSfx("health"); });

    // 设置初始中心位置
    QPoint initViewport(player->geometry().width() / 2, player->geometry().height() / 2);
    map->updateObstacleAndTextureIndex(initViewport);

    // 局内强化系统
    enhancementManager = new EnhancementManager(player, window);
    // 施加全局强化
    for (int i = 0; i < 6; i++) {
        auto globalEnhancements = enhancementManager->getGlobalEnhancements();
        enhancementManager->applyEnhancement(globalEnhancements[i],
                                             global->getGlobalEnhancementLevel(i));
    }
    player->initHealthAndMana();
}

GameLogic::~GameLogic() {
    delete map;
    delete player;
    delete enhancementManager;
}

int GameLogic::getLevel() {
    return level;
}

int GameLogic::getCurrentExp() {
    return currentExp;
}

int GameLogic::getNextLevelExp() {
    return nextLevelExp;
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

QSet<Loot*>& GameLogic::getLoots() {
    return loots;
}

double GameLogic::getHpPercent() const {
    return double(player->getHealth()) / player->getMaxHealth();
}

double GameLogic::getMpPercent() const {
    return double(player->getCurrentMana()) / player->getMaxMana();
}

double GameLogic::getExpPercent() const {
    return double(currentExp) / nextLevelExp;
}

QString GameLogic::getHpText() const {
    return QString::number(int(player->getHealth())) + " / "
           + QString::number(int(player->getMaxHealth()));
}

QString GameLogic::getMpText() const {
    return QString::number(player->getCurrentMana()) + " / "
           + QString::number(player->getMaxMana());
}

QString GameLogic::getExpText() const {
    return QString::number(currentExp) + " / " + QString::number(nextLevelExp);
}

bool GameLogic::isPlayerReceivingDamage() const {
    return player->getIsReceivingDamage();
}

QString GameLogic::getSurvivalTimeString() const {
    int minutes = survivalTimeLeft / 60;
    int seconds = int(survivalTimeLeft) % 60;
    return QString("%1:%2").arg(minutes, 2, 10, QChar('0')).arg(seconds, 2, 10, QChar('0'));
}

void GameLogic::updateSurvivalTime() {
    survivalTimeLeft -= 1;

    if (survivalTimeLeft < 0) {
        handleRewards();

        emit gameWin();
    }
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

void GameLogic::moveLoots() {
    for (auto it = loots.begin(); it != loots.end();) {
        (*it)->moveToPlayer(player->getPos());

        if ((*it)->getIsPicked()) {
            delete *it;
            it = loots.erase(it);
        } else {
            ++it;
        }
    }
}

void GameLogic::updateMapFlowField() {
    // 如果玩家两次在一个格子就不更新流场
    if (map->getGridCornerPos(player->getPos()) == lastPlayerPos) {
        return;
    }

    static QTime lastCallTime = QTime::currentTime();
    QTime currentTime = QTime::currentTime();

    int interval = 200;

    if (lastCallTime.msecsTo(currentTime) >= interval) {
        lastCallTime = currentTime;
    } else {
        return;
    }

    if (!workerThread || !workerThread->isRunning()) {
        if (workerThread) {
            workerThread->quit();
            workerThread->wait();
            delete workerThread;
        }

        workerThread = new QThread;
        FlowFieldWorker* worker = new FlowFieldWorker(map, player->getPos());
        worker->moveToThread(workerThread);

        connect(workerThread, &QThread::started, worker, &FlowFieldWorker::updateFlowField);
        connect(worker, &FlowFieldWorker::flowFieldUpdated, workerThread, &QThread::quit);
        connect(workerThread, &QThread::finished, worker, &FlowFieldWorker::deleteLater);

        workerThread->start();
    }

    lastPlayerPos = map->getGridCornerPos(player->getPos());
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

void GameLogic::handleInRangeLoots() {
    for (auto it = loots.begin(); it != loots.end(); ++it) {
        CircleRange* pickRange = player->getPickRange();

        if (pickRange->contains(player->getPos(), (*it)->geometry())) {
            (*it)->inPickRange();
        }
    }
}

void GameLogic::addWitch(QPoint& viewport) {
    double progress = (survivalTime - survivalTimeLeft) / survivalTime;
    WitchEnum witchIndex = Witch::chooseWitch(progress);

    if (witchIndex == WitchEnum::noWitch) {
        return;
    }

    Witch* newWitch = Witch::loadWitchFromJson(witchIndex, map);
    connect(newWitch, &Character::attackPerformed, this, &GameLogic::storeAttack);
    connect(newWitch, &Character::damageReceived, this, [this] {
        this->soundManager->playSfx("hit");
    });
    connect(newWitch, &Character::attackPerformed, this, [this, newWitch] {
        if (newWitch->getWeaponType() == Weapon::WeaponType::Remote) {
            this->soundManager->playSfx("shoot");
        } else {
            this->soundManager->playSfx("slash");
        }
    });

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

QVector<double> GameLogic::playerSelectTarget() {
    MagicalGirl* player = getPlayer();
    AttackRange* range = player->getRange();

    std::priority_queue<QPair<double, Witch*>,
                        std::vector<QPair<double, Witch*>>,
                        std::greater<QPair<double, Witch*>>>
        distances;
    for (auto it = witches.begin(); it != witches.end(); it++) {
        if (player->getWeaponType() == Weapon::WeaponType::Remote
            && isBlocked(player->getPos(), (*it)->getPos())) {
            continue;
        }

        QPointF playerPos(player->x(), player->y()), witchPos((*it)->x(), (*it)->y());
        double distance = MathUtils::euclideanDistance(playerPos, witchPos);

        if (!range->contains(playerPos, (*it)->geometry())) {
            continue;
        }

        distances.push(qMakePair(distance, (*it)));
    }

    QVector<double> targetWitchDegrees;
    while (!distances.empty()) {
        targetWitchDegrees.append(
            MathUtils::calculateDegree(player->getPos(), distances.top().second->getPos()));
        distances.pop();
    }

    return targetWitchDegrees;
}

void GameLogic::playerAttack() {
    QVector<double> targetWitchDegrees = playerSelectTarget();
    player->performAttack(targetWitchDegrees);
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
            // 经验掉落
            int witchExp = (*witchIt)->getExp();
            Experience* exp = new Experience(witchExp, (*witchIt)->getPos(), map);
            connect(exp, &Experience::experiencePicked, this, &GameLogic::updateExp);
            connect(exp, &Experience::experiencePicked, this, [this] {
                soundManager->playSfx("experience");
            });
            loots.insert(exp);

            // 悲叹之种碎片掉落
            if (Witch::ifDropGriefSeedFragment()) {
                GriefSeedFragment* gsf = new GriefSeedFragment((*witchIt)->getPos() + QPoint(5, 5),
                                                               map); // 偏移一点，防止和经验重合
                connect(gsf,
                        &GriefSeedFragment::griefSeedFragmentPicked,
                        this,
                        &GameLogic::handlePlayerManaRecover);
                connect(gsf, &GriefSeedFragment::griefSeedFragmentPicked, this, [this] {
                    soundManager->playSfx("griefseed");
                });
                loots.insert(gsf);
            }

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

void GameLogic::handleOutOfBoundryObject() {
    for (auto witchIt = witches.begin(); witchIt != witches.end();) {
        if (map->isOutOfBoundry((*witchIt)->getPos())) {
            delete *witchIt;
            witchIt = witches.erase(witchIt);
        } else {
            ++witchIt;
        }
    }

    for (auto bulletIt = bullets.begin(); bulletIt != bullets.end();) {
        if (map->isOutOfBoundry((*bulletIt)->getPos())) {
            delete *bulletIt;
            bulletIt = bullets.erase(bulletIt);
        } else {
            ++bulletIt;
        }
    }

    for (auto it = loots.begin(); it != loots.end();) {
        QPoint pos((*it)->x(), (*it)->y());
        if (map->isOutOfBoundry(pos)) {
            delete *it;
            it = loots.erase(it);
        } else {
            ++it;
        }
    }
}

void GameLogic::handlePlayerHealthRecover() {
    if (player->getIsReadyToRecover()) {
        player->recoverHealth();
    }
}

void GameLogic::handlePlayerManaRecover() {
    if (player->getCurrentMana() < minRecoverHealthMana) {
        return;
    }

    player->recoverMana(griefSeedFragmentMana);
}

void GameLogic::checkIfPlayerDie() {
    int manaLeft = player->getCurrentMana();

    if (manaLeft <= 0) {
        handleRewards();

        emit gameOver();
    }
}

void GameLogic::updateExp(int exp) {
    currentExp += exp + player->getExperienceBonus();

    if (currentExp >= nextLevelExp) {
        currentExp %= nextLevelExp;
        nextLevelExp *= nextLevelExpFactor;

        handleLevelUp();
    }
}

void GameLogic::handleLevelUp() {
    level++;

    soundManager->playSfx("levelup");

    randomEnhancements = enhancementManager->generateEnhancement(player);
    emit levelUp(randomEnhancements);
}

void GameLogic::handleRewards() {
    global->addMoney(survivalTime - survivalTimeLeft);
}

void GameLogic::storeAttack(Attack* attack) {
    if (auto bullet = dynamic_cast<Bullet*>(attack)) {
        bullets.insert(bullet);
    } else if (auto slash = dynamic_cast<Slash*>(attack)) {
        slashes.insert(slash);
    }
}

void GameLogic::enhancementSelected(int index) {
    enhancementManager->applyEnhancement(randomEnhancements[index], 0);

    emit levelUpFinish();
}
