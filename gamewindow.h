#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <QKeyEvent>
#include <QPainter>
#include <QSet>
#include <QTimer>
#include <QWidget>

#include <cmath>

#include "gamelogic.h"
#include "pausewindow.h"

const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;

const int RENDER_FPS = 60;
const int RENDER_FRAME_TIME = 1000 / RENDER_FPS;

const int LOGIC_FPS = 60;
const int LOGIC_FRAME_TIME = 1000 / LOGIC_FPS;

class GameWindow : public QWidget {
    Q_OBJECT

private:
    QPoint viewport = QPoint(0, 0);

    GameLogic *gameLogic;

    QSet<int> pressedKeys;

    QTimer *logicTimer;
    QTimer *renderTimer;
    QTimer *survivalTimer;

    bool isGamePaused = false;
    PauseWindow *pauseWindow;

public:
    explicit GameWindow(MagicalGirlEnum playerSelection, QWidget *parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

    Direction getPlayerMovement();
    bool getPlayerAttack();

    void updateViewport();

private slots:
    void updateGameLogic();
    void renderFrame();

public slots:
    void onClosePauseWindow(bool isGameContinued);
    void onGameWin();
    void onGameOver();

signals:
    void startNewGame();
};

#endif // GAMEWINDOW_H
