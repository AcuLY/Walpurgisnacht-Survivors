#include "gamewindow.h"
#include <QDebug>

GameWindow::GameWindow(QWidget *parent) : QWidget{parent} {
    setFixedSize(1280, 720);

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

void GameWindow::updateViewport() {
    Character *player = gameLogic->getPlayer();
    int playerX = player->geometry().x() + player->geometry().width() / 2;
    int playerY = player->geometry().y() + player->geometry().height() / 2;

    viewportX = playerX - WINDOW_WIDTH / 2;
    viewportY = playerY - WINDOW_HEIGHT / 2;
}

void GameWindow::updateGameLogic() {
    gameLogic->addWitch(viewportX, viewportY);
    Direction dir = getPlayerMovement();
    gameLogic->movePlayer(dir);
    gameLogic->moveWitches();

    updateViewport();
}

void GameWindow::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);

    QPainter painter(this);

    painter.translate(-viewportX, -viewportY);

    Map *map = gameLogic->getMap();
    map->render(&painter, viewportX, viewportY);

    MagicalGirl *player = gameLogic->getPlayer();
    painter.fillRect(player->geometry(), Qt::white);

    auto witches = gameLogic->getWitches();
    for (auto it = witches.begin(); it != witches.end(); ++it) {
        painter.fillRect((*it)->geometry(), Qt::blue);
    }
}

void GameWindow::renderFrame() {
    update();
}
