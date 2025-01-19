#include "gamelogic.h"
#include <queue>

GameLogic::GameLogic(Global* global,
                     SoundManager* soundManager,
                     MagicalGirlEnum playerSelection,
                     QObject* parent)
    : QObject{parent}, global(global), soundManager(soundManager),
      flowFieldUpdateThread(new QThread()) {
    QWidget* gameWindow = qobject_cast<QWidget*>(parent);

    map = new Map(FRICTION, gameWindow);

    player = MagicalGirl::loadMagicalGirlFromJson(playerSelection, map);
    connect(player, &Character::attackPerformed, this, &GameLogic::storeAttack); // 存放玩家的攻击实体
    // 绑定音效
    connect(player, &Character::damageReceived, this, [this] {
        this->soundManager->playSfx("hurt");
    });
    connect(player, &Character::attackPerformed, this, [this] {
        if (player->getWeaponType() == Weapon::WeaponType::Remote) {
            this->soundManager->playSfx("shoot");
        } else {
            this->soundManager->playSfx("slash");
        }
    });
    connect(player, &MagicalGirl::healed, this, [this] { this->soundManager->playSfx("health"); });

    QPoint initViewport(player->geometry().width() / 2,
                        player->geometry().height() / 2); // 设置初始视点

    map->updateObstacleAndTextureIndex(initViewport); // 初始化地图障碍

    enhancementManager = new EnhancementManager(player, gameWindow); // 初始化局内强化系统
    // 施加全局强化
    for (int i = 0; i < 6; i++) {
        auto globalEnhancements = enhancementManager->getGlobalEnhancements();
        enhancementManager->applyEnhancement(globalEnhancements[i],
                                             global->getGlobalEnhancementLevel(i));
    }

    player->initHealthAndMana(); // 根据全局强化更新玩家初始值
}

GameLogic::~GameLogic() {
    delete map;
    delete player;
    delete enhancementManager;

    if (flowFieldUpdateThread) {
        flowFieldUpdateThread->quit();
        flowFieldUpdateThread->wait();
        delete flowFieldUpdateThread;
    }

    for (auto it = witches.begin(); it != witches.end(); ++it) {
        delete *it;
    }
    for (auto it = bullets.begin(); it != bullets.end(); ++it) {
        delete *it;
    }
    for (auto it = slashes.begin(); it != slashes.end(); ++it) {
        delete *it;
    }
    for (auto it = loots.begin(); it != loots.end(); ++it) {
        delete *it;
    }
}

Map* GameLogic::getMap() const {
    return map;
}

MagicalGirl* GameLogic::getPlayer() const {
    return player;
}

const QSet<Witch*>& GameLogic::getWitches() const {
    return witches;
}

const QSet<Bullet*>& GameLogic::getBullets() const {
    return bullets;
}

const QSet<Slash*>& GameLogic::getSlashes() const {
    return slashes;
}

const QSet<Loot*>& GameLogic::getLoots() const {
    return loots;
}

double GameLogic::getHpPercent() const {
    return double(player->getCurrentHealth()) / player->getMaxHealth();
}

double GameLogic::getMpPercent() const {
    return double(player->getCurrentMana()) / player->getMaxMana();
}

double GameLogic::getExpPercent() const {
    return double(currentExp) / nextLevelExp;
}

QString GameLogic::getHpText() const {
    return QString::number(int(player->getCurrentHealth())) + " / "
           + QString::number(int(player->getMaxHealth()));
}

QString GameLogic::getMpText() const {
    return QString::number(player->getCurrentMana()) + " / "
           + QString::number(player->getMaxMana());
}

QString GameLogic::getExpText() const {
    return QString::number(currentExp) + " / " + QString::number(nextLevelExp);
}

QString GameLogic::getLevelText() const {
    return "Level:" + QString::number(level);
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

    // 游戏胜利条件
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
    for (auto it = witches.begin(); it != witches.end(); ++it) {
        Direction dir = map->getFlow((*it)->getPos());
        (*it)->moveActively(dir);
    }
}

void GameLogic::moveBullets() {
    for (auto it = bullets.begin(); it != bullets.end(); ++it) {
        (*it)->moveActively();
    }
}

void GameLogic::moveLoots() {
    for (auto it = loots.begin(); it != loots.end();) {
        (*it)->moveToPlayer(player->getPos()); // 向玩家持续移动

        // 是否被拾取
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

    static QTime lastCallTime = QTime::currentTime(); // 上次更新的时间
    QTime currentTime = QTime::currentTime();

    int interval = 200; // 流场更新最小时间间隔

    // 如果时间没到则不更新流场
    if (lastCallTime.msecsTo(currentTime) >= interval) {
        lastCallTime = currentTime;
    } else {
        return;
    }

    if (!flowFieldUpdateThread || !flowFieldUpdateThread->isRunning()) {
        // 检查线程状态
        if (flowFieldUpdateThread) {
            flowFieldUpdateThread->quit();
            flowFieldUpdateThread->wait();
            delete flowFieldUpdateThread;
        }

        // 创建子线程用于更新流场
        flowFieldUpdateThread = new QThread;
        FlowFieldWorker* worker = new FlowFieldWorker(map, player->getPos());
        worker->moveToThread(flowFieldUpdateThread);

        connect(flowFieldUpdateThread, &QThread::started, worker, &FlowFieldWorker::updateFlowField);
        connect(worker, &FlowFieldWorker::flowFieldUpdated, flowFieldUpdateThread, &QThread::quit);
        connect(flowFieldUpdateThread, &QThread::finished, worker, &FlowFieldWorker::deleteLater);

        flowFieldUpdateThread->start();
    }

    lastPlayerPos = map->getGridCornerPos(player->getPos());
}

void GameLogic::handleCharacterCollision() {
    // 玩家与地图，玩家与敌人
    player->handleCollision(map);
    for (auto witchIt = witches.begin(); witchIt != witches.end(); ++witchIt) {
        player->handleCollision(*witchIt);
    }
    player->handleCollision(map);

    // 敌人与地图，敌人与玩家，敌人与敌人
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

        // 判断是否吸引到了掉落物
        if (pickRange->contains(player->getPos(), (*it)->geometry())) {
            (*it)->inPickRange();
        }
    }
}

void GameLogic::addWitch(QPoint& viewport) {
    double progress = (SURVIVAL_TIME - survivalTimeLeft) / SURVIVAL_TIME; // 当前游戏进度
    WitchEnum witchIndex = Witch::chooseWitch(progress);                  // 敌人编号

    if (witchIndex == WitchEnum::noWitch) { // 当前不生成敌人
        return;
    }

    // witch 初始化
    Witch* newWitch = Witch::loadWitchFromJson(witchIndex, map);
    connect(newWitch, &Character::attackPerformed, this, &GameLogic::storeAttack);
    connect(newWitch, &Character::damageReceived, this, [this] {
        this->soundManager->playSfx("hit");
    });
    connect(newWitch, &Character::attackPerformed, this, [this, newWitch] {
        if (newWitch->getWeaponType() == Weapon::WeaponType::Remote) {
            this->soundManager->playSfx("enemy_shoot");
        } else {
            this->soundManager->playSfx("slash");
        }
    });

    int edge = QRandomGenerator::global()->bounded(0, 4); // 选择一个边界
    int x = 0, y = 0;                                     // 初始坐标
    Direction dir = Direction::Center;                    // 边界方向

    // 根据边界初始化值
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

    // 判断初始位置是否卡在地图里，如果是则移动位置
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
    // 判断两点间是否有障碍物
    QPainterPath partialPath = map->getPartialPath(pos1, pos2);
    return !partialPath.isEmpty();
}

QVector<double> GameLogic::playerSelectTarget() {
    MagicalGirl* player = getPlayer();
    AttackRange* range = player->getRange();

    // 将能攻击到的敌人按距离排序
    std::priority_queue<QPair<double, Witch*>,
                        std::vector<QPair<double, Witch*>>,
                        std::greater<QPair<double, Witch*>>>
        distances;
    for (auto it = witches.begin(); it != witches.end(); it++) {
        if (player->getWeaponType() == Weapon::WeaponType::Remote
            && isBlocked(player->getPos(), (*it)->getPos())) { // 被障碍物阻挡
            continue;
        }

        QPointF playerPos(player->x(), player->y()), witchPos((*it)->x(), (*it)->y());
        double distance = MathUtils::euclideanDistance(playerPos, witchPos);

        if (!range->contains(playerPos, (*it)->geometry())) { // 距离外
            continue;
        }

        distances.push(qMakePair(distance, (*it)));
    }

    // 将敌人换算成相对于玩家的角度
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
            && isBlocked(player->getPos(), (*it)->getPos())) { // 被挡住或者打不到
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
        // 玩家是否受伤
        if (!(*bulletIt)->getPlayerSide() && (*bulletIt)->isHit(player->geometry())) {
            player->receiveDamage((*bulletIt)->getDamage());

            delete *bulletIt;
            bulletIt = bullets.erase(bulletIt);

            continue;
        }

        // 敌人是否受伤
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

        // 玩家
        if (!(*slashIt)->getPlayerSide() && (*slashIt)->isHit(player->geometry())) {
            player->receiveDamage((*slashIt)->getDamage());
        }

        // 敌人
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
        if ((*witchIt)->getCurrentHealth() <= 0) {
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
                GriefSeedFragment* gsf = new GriefSeedFragment((*witchIt)->getPos() + QPoint(3, 3),
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
    player->recoverMana(GRIEF_SEED_FRAGMENT_MANA);
}

void GameLogic::checkIfPlayerDie() {
    int manaLeft = player->getCurrentMana();

    if (manaLeft <= 0) { // 没蓝了就死了
        soundManager->stopBackgroundMusic();
        handleRewards();

        emit gameLose();
    }
}

void GameLogic::updateExp(int exp) {
    currentExp += exp + player->getExperienceBonus();

    if (currentExp >= nextLevelExp) {
        currentExp %= nextLevelExp;
        nextLevelExp *= NEXT_LEVEL_EXP_INCREASE_FACTOR;

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
    global->addMoney(SURVIVAL_TIME - survivalTimeLeft); // 金币奖励为存活的时间
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
