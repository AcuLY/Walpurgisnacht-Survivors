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

    if (playerSelection == MagicalGirlEnum::Homura) {
        avatar = QPixmap(":/images/character_avatar/homura_avatar");
    } else {
        avatar = QPixmap(":/images/character_avatar/sayaka_avatar");
    }

    gameLogic = new GameLogic(global, soundManager, playerSelection);
    connect(gameLogic, &GameLogic::gameWin, this, &GameWindow::onGameWin);
    connect(gameLogic, &GameLogic::gameOver, this, &GameWindow::onGameOver);
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

    pauseWindow = new PauseWindow(this);
    pauseWindow->move((WINDOW_WIDTH - pauseWindow->geometry().width()) / 2,
                      (WINDOW_HEIGHT - pauseWindow->geometry().height()) / 2);
    pauseWindow->hide();
    connect(pauseWindow, &PauseWindow::closePauseWindow, this, &GameWindow::onClosePauseWindow);

    enhancementWindow = new EnhancementWindow(soundManager, this);
    connect(enhancementWindow,
            &EnhancementWindow::selectEnhancement,
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
                             viewport.y() + SURVIVAL_ITME_RECT_Y_OFFSET,
                             SURVIVAL_TIME_RECT_WIDTH,
                             SURVIVAL_TIME_RECT_HEIGHT);
}

void GameWindow::keyPressEvent(QKeyEvent *event) {
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

    QElapsedTimer timer;

    gameLogic->addWitch(viewport);

    gameLogic->movePlayer(getPlayerMovement());

    gameLogic->handleInRangeLoots();

    gameLogic->updateMapFlowField();

    gameLogic->moveWitches();

    gameLogic->moveBullets();

    gameLogic->moveLoots();

    gameLogic->handleCharacterCollision();

    gameLogic->handleAttack();

    gameLogic->checkIfPlayerDie();

    gameLogic->handlePlayerHealthRecover();

    gameLogic->handleBulletMapCollision();

    gameLogic->handleDeadWitches();

    gameLogic->handleInvalidAttack();

    gameLogic->handleOutOfBoundryObject();

    timer.start();
    if (getPlayerAttack()) {
        gameLogic->playerAttack();
    }

    gameLogic->handleDeadWitches();

    gameLogic->witchAttack();

    updateViewport();
}

void GameWindow::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);

    QPainter painter(this);

    painter.translate(-viewport);

    Map *map = gameLogic->getMap();
    map->updateObstacleAndTextureIndex(viewport);
    map->render(&painter, viewport);

    painter.drawPixmap(-160, 0, QPixmap(":/images/ui/hint"));

    auto loots = gameLogic->getLoots();
    for (auto it = loots.begin(); it != loots.end(); ++it) {
        (*it)->render(&painter);
    }

    MagicalGirl *player = gameLogic->getPlayer();
    player->render(&painter);

    auto witches = gameLogic->getWitches();
    for (auto it = witches.begin(); it != witches.end(); ++it) {
        (*it)->render(&painter);
    }

    auto bullets = gameLogic->getBullets();
    for (auto it = bullets.begin(); it != bullets.end(); ++it) {
        (*it)->render(&painter);
    }

    auto slashes = gameLogic->getSlashes();
    for (auto it = slashes.begin(); it != slashes.end(); ++it) {
        painter.fillPath((*it)->createPath(), Qt::red);
    }

    updateStatusBarParams();

    painter.setPen(QColor(0, 0, 0, 0));
    painter.setBrush(QColor(236, 0, 45, 180));
    painter.drawRect(hpBar);

    painter.setBrush(QColor(109, 157, 211, 180));
    painter.drawRect(mpBar);

    painter.setBrush(QColor(255, 255, 255, 180));
    painter.drawRect(expBar);

    QString survivalTime = gameLogic->getSurvivalTimeString();
    painter.setFont(QFont("汉仪像素入侵 U", 64));
    painter.setBrush(QColor(255, 255, 255, 255));
    painter.setPen(QColor(255, 255, 255, 255));
    painter.drawText(survivalTimeRect, Qt::AlignCenter, survivalTime);

    painter.setFont(QFont("汉仪像素入侵 U", 16));
    painter.drawText(QRect(viewport.x() + HP_MP_BAR_X_OFFSET,
                           viewport.y() + HP_BAR_Y_OFFSET,
                           HP_MP_BAR_WIDTH,
                           HP_MP_BAR_HEIGHT),
                     Qt::AlignCenter,
                     gameLogic->getHpText());
    painter.drawText(QRect(viewport.x() + HP_MP_BAR_X_OFFSET,
                           viewport.y() + MP_BAR_Y_OFFSET,
                           HP_MP_BAR_WIDTH,
                           HP_MP_BAR_HEIGHT),
                     Qt::AlignCenter,
                     gameLogic->getMpText());
    painter.setFont(QFont("汉仪像素入侵 U", 24));
    painter.setPen(QColor(0, 0, 0));
    painter.drawText(QRect(viewport.x(),
                           viewport.y() + EXP_BAR_Y_OFFSET,
                           EXP_BAR_WIDTH,
                           EXP_BAR_HEIGHT),
                     Qt::AlignCenter,
                     gameLogic->getExpText());

    painter.drawPixmap(viewport.x(), viewport.y(), bar);

    painter.drawPixmap(viewport.x() + 20, viewport.y() + 12, avatar);

    if (gameLogic->isPlayerReceivingDamage()) {
        painter.setBrush(QColor(255, 0, 0, 32));
        painter.drawRect(viewport.x(), viewport.y(), WINDOW_WIDTH, WINDOW_HEIGHT);
    }
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

        PauseStatus status = pauseWindow->getStatus();

        if (status == PauseStatus::GamePause) {
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
