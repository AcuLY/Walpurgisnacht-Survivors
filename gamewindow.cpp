#include "gamewindow.h"
#include <QDebug>

GameWindow::GameWindow(QWidget *parent) : QWidget{parent} {
    setFixedSize(600, 600);

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

std::pair<BiDirection, BiDirection> GameWindow::getPlayerMovement() {
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
    return {moveX, moveY};
}

void GameWindow::updateGameLogic() {
    auto [moveX, moveY] = getPlayerMovement();
    gameLogic->updatePlayerPosition(moveX, moveY);
}

void GameWindow::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);

    QPainter painter(this);

    painter.fillRect(rect(), Qt::black);

    Map *map = gameLogic->getMap();
    painter.fillRect(map->geometry(), Qt::green);

    Character *player = gameLogic->getPlayer();
    painter.fillRect(player->geometry(), Qt::white);
}

void GameWindow::renderFrame() {
    update();
}
