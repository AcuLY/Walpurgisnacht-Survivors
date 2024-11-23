#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <QKeyEvent>
#include <QPainter>
#include <QSet>
#include <QTimer>
#include <QWidget>

#include <cmath>

#include "gamelogic.h"

const int RENDER_FPS = 120;
const int RENDER_FRAME_TIME = 1000 / RENDER_FPS;

const int LOGIC_FPS = 60;
const int LOGIC_FRAME_TIME = 1000 / LOGIC_FPS;

class GameWindow : public QWidget {
    Q_OBJECT

private:
    GameLogic *gameLogic;

    QSet<int> pressedKeys;

    QTimer *logicTimer;
    QTimer *renderTimer;

public:
    explicit GameWindow(QWidget *parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

    std::pair<BiDirection, BiDirection> getPlayerMovement();

private slots:
    void updateGameLogic();
    void renderFrame();
};

#endif // GAMEWINDOW_H
