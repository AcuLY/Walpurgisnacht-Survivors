#include "gamewindow.h"
#include <QDebug>

GameWindow::GameWindow(Global *global,
                       SoundManager *soundManager,
                       MagicalGirlEnum playerSelection,
                       QWidget *parent)
    : QWidget{parent}, soundManager(soundManager), global(global) {
    this->setFixedSize(parent->geometry().width(), parent->geometry().height());

    setFocusPolicy(Qt::StrongFocus);
    setFocus(); // 获取焦点

    // 选择头像
    if (playerSelection == MagicalGirlEnum::Homura) {
        avatar = QPixmap(":/images/character_avatar/homura_avatar");
    } else {
        avatar = QPixmap(":/images/character_avatar/sayaka_avatar");
    }

    // 逻辑成员初始化
    gameLogic = new GameLogic(global, soundManager, playerSelection);
    connect(gameLogic, &GameLogic::gameWin, this, &GameWindow::onGameWin);
    connect(gameLogic, &GameLogic::gameLose, this, &GameWindow::onGameOver);
    connect(gameLogic, &GameLogic::levelUp, this, &GameWindow::onLevelUp);
    connect(gameLogic, &GameLogic::levelUpFinish, this, &GameWindow::onLevelUpFinished);

    logicTimer = new QTimer(this);
    connect(logicTimer, &QTimer::timeout, this, &GameWindow::updateGameLogic);
    logicTimer->start(LOGIC_FRAME_TIME);

    renderTimer = new QTimer(this);
    connect(renderTimer, &QTimer::timeout, this, &GameWindow::renderFrame);
    renderTimer->start(RENDER_FRAME_TIME);

    survivalTimer = new QTimer(this);
    connect(survivalTimer, &QTimer::timeout, this, [this] {
        if (!isGamePaused) {
            gameLogic->updateSurvivalTime();
        }
    });
    survivalTimer->start(1000);

    // 窗口初始化
    pauseWindow = new PauseWindow(this);
    pauseWindow->move((WINDOW_WIDTH - pauseWindow->geometry().width()) / 2,
                      (WINDOW_HEIGHT - pauseWindow->geometry().height()) / 2);
    pauseWindow->hide();
    connect(pauseWindow, &PauseWindow::closePauseWindow, this, &GameWindow::onClosePauseWindow);

    enhancementWindow = new EnhancementWindow(soundManager, this);
    connect(enhancementWindow,
            &EnhancementWindow::enhancementSelected,
            gameLogic,
            &GameLogic::enhancementSelected);
    enhancementWindow->hide();
}

void GameWindow::updateStatusBarParams() {
    hpBar = QRect(viewport.x() + HP_MP_BAR_X_OFFSET,
                  viewport.y() + HP_BAR_Y_OFFSET,
                  gameLogic->getHpPercent() * HP_MP_BAR_WIDTH,
                  HP_MP_BAR_HEIGHT);
    mpBar = QRect(viewport.x() + HP_MP_BAR_X_OFFSET,
                  viewport.y() + MP_BAR_Y_OFFSET,
                  gameLogic->getMpPercent() * HP_MP_BAR_WIDTH,
                  HP_MP_BAR_HEIGHT);
    expBar = QRect(viewport.x(),
                   viewport.y() + EXP_BAR_Y_OFFSET,
                   gameLogic->getExpPercent() * EXP_BAR_WIDTH,
                   EXP_BAR_HEIGHT);
    survivalTimeRect = QRect(viewport.x() + SURVIVAL_TIME_RECT_X_OFFSET,
                             viewport.y() + SURVIVAL_TIME_RECT_Y_OFFSET,
                             SURVIVAL_TIME_RECT_WIDTH,
                             SURVIVAL_TIME_RECT_HEIGHT);
}

void GameWindow::keyPressEvent(QKeyEvent *event) {
    // 存入当前按下的按键，如果是 esc 则暂停
    if (event->key() == Qt::Key_Escape) {
        pauseWindow->setStatus(PauseStatus::GamePause);
        soundManager->pauseBackgroundMusic();
        pauseWindow->show();
        isGamePaused = true;
    }

    pressedKeys.insert(event->key());
}

void GameWindow::keyReleaseEvent(QKeyEvent *event) {
    pressedKeys.remove(event->key());
}

Direction GameWindow::getPlayerMovement() {
    // 根据按下的按键获取玩家移动方向
    BiDirection moveX = BiDirection::Neutral, moveY = BiDirection::Neutral;
    if (pressedKeys.contains(global->getKeyboardMapping()[GameKey::up])) {
        moveY = BiDirection::Negative;
    }
    if (pressedKeys.contains(global->getKeyboardMapping()[GameKey::down])) {
        moveY = moveY ? BiDirection::Neutral : BiDirection::Positive;
    }
    if (pressedKeys.contains(global->getKeyboardMapping()[GameKey::left])) {
        moveX = BiDirection::Negative;
    }
    if (pressedKeys.contains(global->getKeyboardMapping()[GameKey::right])) {
        moveX = moveX ? BiDirection::Neutral : BiDirection::Positive;
    }

    return pairBiDirection(moveX, moveY);
}

bool GameWindow::getPlayerAttack() {
    return pressedKeys.contains(global->getKeyboardMapping()[GameKey::attack]);
}

void GameWindow::updateViewport() {
    MagicalGirl *player = gameLogic->getPlayer();
    int playerX = player->geometry().x() + player->geometry().width() / 2;
    int playerY = player->geometry().y() + player->geometry().height() / 2;

    viewport.setX(playerX - WINDOW_WIDTH / 2);
    viewport.setY(playerY - WINDOW_HEIGHT / 2);
}

void GameWindow::updateGameLogic() {
    if (isGamePaused) {
        return;
    }

    // 处理定时事件
    gameLogic->updateMapFlowField();
    gameLogic->handlePlayerHealthRecover();
    gameLogic->addWitch(viewport);

    // 移动实体
    gameLogic->movePlayer(getPlayerMovement());
    gameLogic->moveWitches();
    gameLogic->moveBullets();
    gameLogic->moveLoots();

    // 处理碰撞事件
    gameLogic->handleCharacterCollision();
    gameLogic->handleAttack();
    gameLogic->handleInRangeLoots();
    gameLogic->handleBulletMapCollision();

    // 处理失效实体
    gameLogic->handleDeadWitches();
    gameLogic->handleInvalidAttack();
    gameLogic->handleOutOfBoundryObject();

    // 处理攻击
    if (getPlayerAttack()) {
        gameLogic->playerAttack();
    }
    gameLogic->witchAttack();
    gameLogic->handleDeadWitches();

    // 判断游戏状态
    gameLogic->checkIfPlayerDie();

    updateViewport();
}

void GameWindow::updateAndRenderUI(QPainter *painter) {
    updateStatusBarParams(); // 获取最新数据

    // 血条矩形
    painter->setPen(QColor(0, 0, 0, 0));
    painter->setBrush(QColor(236, 0, 45, 180));
    painter->drawRect(hpBar);
    // 蓝条矩形
    painter->setBrush(QColor(109, 157, 211, 180));
    painter->drawRect(mpBar);
    // 经验条矩形
    painter->setBrush(QColor(255, 255, 255, 180));
    painter->drawRect(expBar);
    // 剩余时间
    QString survivalTimeLeft = gameLogic->getSurvivalTimeString();
    painter->setFont(QFont("汉仪像素入侵 U", 64));
    painter->setBrush(QColor(255, 255, 255, 255));
    painter->setPen(QColor(255, 255, 255, 255));
    painter->drawText(survivalTimeRect, Qt::AlignCenter, survivalTimeLeft);
    // 血条和蓝条的数字
    painter->setFont(QFont("汉仪像素入侵 U", 16));
    painter->drawText(QRect(viewport.x() + HP_MP_BAR_X_OFFSET,
                            viewport.y() + HP_BAR_Y_OFFSET,
                            HP_MP_BAR_WIDTH,
                            HP_MP_BAR_HEIGHT),
                      Qt::AlignCenter,
                      gameLogic->getHpText());
    painter->drawText(QRect(viewport.x() + HP_MP_BAR_X_OFFSET,
                            viewport.y() + MP_BAR_Y_OFFSET,
                            HP_MP_BAR_WIDTH,
                            HP_MP_BAR_HEIGHT),
                      Qt::AlignCenter,
                      gameLogic->getMpText());
    // 等级和经验条的数字
    painter->setFont(QFont("汉仪像素入侵 U", 24));
    painter->drawText(viewport.x() + 560, viewport.y() + 650, gameLogic->getLevelText());

    painter->setPen(QColor(0, 0, 0));
    painter->drawText(QRect(viewport.x(),
                            viewport.y() + EXP_BAR_Y_OFFSET,
                            EXP_BAR_WIDTH,
                            EXP_BAR_HEIGHT),
                      Qt::AlignCenter,
                      gameLogic->getExpText());
    // UI 的框架
    painter->drawPixmap(viewport.x(), viewport.y(), bar);
    // 头像
    painter->drawPixmap(viewport.x() + 20, viewport.y() + 12, avatar);

    // 如果玩家当前正在受伤，则画面闪一下红色
    if (gameLogic->isPlayerReceivingDamage()) {
        painter->setBrush(QColor(255, 0, 0, 32));
        painter->drawRect(viewport.x(), viewport.y(), WINDOW_WIDTH, WINDOW_HEIGHT);
    }
}

void GameWindow::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);

    QPainter painter(this);

    painter.translate(-viewport); // 将 painter 移动到视点

    // 渲染地图
    Map *map = gameLogic->getMap();
    map->updateObstacleAndTextureIndex(viewport);
    map->render(&painter, viewport);

    // 按层次顺序渲染实体
    auto loots = gameLogic->getLoots();
    for (auto it = loots.begin(); it != loots.end(); ++it) {
        (*it)->render(&painter);
    }

    auto witches = gameLogic->getWitches();
    for (auto it = witches.begin(); it != witches.end(); ++it) {
        (*it)->render(&painter);
    }

    auto player = gameLogic->getPlayer();
    player->render(&painter);

    auto bullets = gameLogic->getBullets();
    for (auto it = bullets.begin(); it != bullets.end(); ++it) {
        (*it)->render(&painter);
    }

    auto slashes = gameLogic->getSlashes();
    for (auto it = slashes.begin(); it != slashes.end(); ++it) {
        (*it)->render(&painter);
    }

    // 渲染 ui
    updateAndRenderUI(&painter);
}

void GameWindow::renderFrame() {
    if (isGamePaused) {
        return;
    }

    update();
}

void GameWindow::onClosePauseWindow(bool isGameContinued) {
    if (isGameContinued) {
        pauseWindow->hide();
        this->setFocus();

        PauseStatus status = pauseWindow->getStatus(); // 获取是暂停还是胜利还是结束

        if (status == PauseStatus::GamePause) { // 如果是暂停则继续，否则退出
            soundManager->playGameMusic();
            isGamePaused = false;
        } else {
            this->deleteLater();
            emit startNewGame();
        }
    } else {
        soundManager->stopBackgroundMusic();
        soundManager->playMenuMusic();
        this->deleteLater();
    }
}

void GameWindow::onGameWin() {
    pauseWindow->setStatus(PauseStatus::GameWin);
    pauseWindow->show();

    isGamePaused = true;
}

void GameWindow::onGameOver() {
    pauseWindow->setStatus(PauseStatus::GameLose);
    pauseWindow->show();

    isGamePaused = true;
}

void GameWindow::onLevelUp(QVector<Enhancement *> &enhancements) {
    enhancementWindow->setDescriptions(enhancements);
    enhancementWindow->show();

    isGamePaused = true;
}

void GameWindow::onLevelUpFinished() {
    enhancementWindow->hide();
    this->setFocus();

    isGamePaused = false;
}
