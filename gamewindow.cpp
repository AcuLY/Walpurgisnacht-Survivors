#include "gamewindow.h"
#include <QDebug>

GameWindow::GameWindow(QWidget *parent) : QWidget{parent} {
    setFixedSize(MAP_WIDTH, MAP_HEIGHT);

    setFocusPolicy(Qt::StrongFocus);

    gameLogic = new GameLogic();

    logicTimer = new QTimer(this);
    connect(logicTimer, &QTimer::timeout, this, &GameWindow::updateGameLogic);
    logicTimer->start(LOGIC_FRAME_TIME);

    renderTimer = new QTimer(this);
    connect(renderTimer, &QTimer::timeout, this, &GameWindow::renderFrame);
    renderTimer->start(RENDER_FRAME_TIME);
}

void GameWindow::keyPressEvent(QKeyEvent *event) {
    pressedKeys.insert(event->key());
}

void GameWindow::keyReleaseEvent(QKeyEvent *event) {
    pressedKeys.remove(event->key());
}

Direction GameWindow::getPlayerMovement() {
    BiDirection moveX = BiDirection::Neutral, moveY = BiDirection::Neutral;
    if (pressedKeys.contains(Qt::Key_W)) {
        moveY = BiDirection::Negative;
    }
    if (pressedKeys.contains(Qt::Key_S)) {
        moveY = moveY ? BiDirection::Neutral : BiDirection::Positive;
    }
    if (pressedKeys.contains(Qt::Key_A)) {
        moveX = BiDirection::Negative;
    }
    if (pressedKeys.contains(Qt::Key_D)) {
        moveX = moveX ? BiDirection::Neutral : BiDirection::Positive;
    }

    return pairBiDirection(moveX, moveY);
}

bool GameWindow::getPlayerAttack() {
    return pressedKeys.contains(Qt::Key_Space);
}

void GameWindow::updateViewport() {
    MagicalGirl *player = gameLogic->getPlayer();
    int playerX = player->geometry().x() + player->geometry().width() / 2;
    int playerY = player->geometry().y() + player->geometry().height() / 2;

    viewport.setX(playerX - WINDOW_WIDTH / 2);
    viewport.setY(playerY - WINDOW_HEIGHT / 2);
}

void GameWindow::updateGameLogic() {
    static qint64 totalAddWitchTime = 0;
    static qint64 totalMovePlayerTime = 0;
    static qint64 totalUpdateFlowTime = 0;
    static qint64 totalMoveWitchesTime = 0;
    static qint64 totalMoveBulletsTime = 0;
    static qint64 totalCharaColliTime = 0;
    static qint64 totalAttackTime = 0;
    static qint64 totalBulletMapColliTime = 0;
    static qint64 totalDeadWitchesTime = 0;
    static qint64 totalInvalidAttackTime = 0;
    static qint64 totalOobTime = 0;
    static qint64 totalAttackActionTime = 0;

    QElapsedTimer timer;

    // 逻辑操作
    timer.start();
    gameLogic->addWitch(viewport);
    totalAddWitchTime += timer.elapsed();

    timer.start();
    gameLogic->movePlayer(getPlayerMovement());
    totalMovePlayerTime += timer.elapsed();

    timer.start();
    gameLogic->updateMapFlowField();
    totalUpdateFlowTime += timer.elapsed();

    timer.start();
    gameLogic->moveWitches();
    totalMoveWitchesTime += timer.elapsed();

    timer.start();
    gameLogic->moveBullets();
    totalMoveBulletsTime += timer.elapsed();

    timer.start();
    gameLogic->handleCharacterCollision();
    totalCharaColliTime += timer.elapsed();

    timer.start();
    gameLogic->handleAttack();
    totalAttackTime += timer.elapsed();

    timer.start();
    gameLogic->handleBulletMapCollision();
    totalBulletMapColliTime += timer.elapsed();

    timer.start();
    gameLogic->handleDeadWitches();
    totalDeadWitchesTime += timer.elapsed();

    timer.start();
    gameLogic->handleInvalidAttack();
    totalInvalidAttackTime += timer.elapsed();

    timer.start();
    gameLogic->handleOutOfBoundryObject();
    totalOobTime += timer.elapsed();

    timer.start();
    if (getPlayerAttack()) {
        gameLogic->playerAttack();
    }
    gameLogic->witchAttack();
    totalAttackActionTime += timer.elapsed();

    updateViewport();

    // 输出时间
    // qDebug() << "add witch:" << totalAddWitchTime;
    // qDebug() << "move player:" << totalMovePlayerTime;
    // qDebug() << "update flow:" << totalUpdateFlowTime;
    // qDebug() << "move witch:" << totalMoveWitchesTime;
    // qDebug() << "move bullets:" << totalMoveBulletsTime;
    // qDebug() << "chara colli:" << totalCharaColliTime;
    // qDebug() << "attack:" << totalAttackTime;
    // qDebug() << "bullet map colli:" << totalBulletMapColliTime;
    // qDebug() << "dead witches:" << totalDeadWitchesTime;
    // qDebug() << "invalid attack:" << totalInvalidAttackTime;
    // qDebug() << "out of bound object:" << totalOobTime;
    // qDebug() << "attack action:" << totalAttackActionTime;
}

void GameWindow::paintEvent(QPaintEvent *event) {
    QElapsedTimer timer;
    timer.start();
    Q_UNUSED(event);

    QPainter painter(this);

    painter.translate(-viewport);

    Map *map = gameLogic->getMap();
    map->updateObstacle(viewport);
    map->render(&painter, viewport);
    qint64 renderMapTime = timer.elapsed();
    timer.start();

    MagicalGirl *player = gameLogic->getPlayer();
    painter.fillRect(player->geometry(), Qt::white);
    qint64 renderPlayerTime = timer.elapsed();
    timer.start();

    auto witches = gameLogic->getWitches();
    for (auto it = witches.begin(); it != witches.end(); ++it) {
        painter.fillRect((*it)->geometry(), Qt::blue);
    }
    qint64 renderWitchesTime = timer.elapsed();
    timer.start();

    auto bullets = gameLogic->getBullets();
    for (auto it = bullets.begin(); it != bullets.end(); ++it) {
        painter.fillPath((*it)->createPath(), Qt::red);
    }
    qint64 renderBulletsTime = timer.elapsed();
    timer.start();

    auto slashes = gameLogic->getSlashes();
    for (auto it = slashes.begin(); it != slashes.end(); ++it) {
        painter.fillPath((*it)->createPath(), Qt::red);
    }
    qint64 renderSlashesTime = timer.elapsed();

    // 最后统一输出所有时间
    // qDebug() << "render map:" << renderMapTime;
    // qDebug() << "render player:" << renderPlayerTime;
    // qDebug() << "render witches:" << renderWitchesTime;
    // qDebug() << "render bullets:" << renderBulletsTime;
    // qDebug() << "render slashes:" << renderSlashesTime;
}

void GameWindow::renderFrame() {
    update();
}
