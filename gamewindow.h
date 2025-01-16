#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <QKeyEvent>
#include <QPainter>
#include <QSet>
#include <QTimer>
#include <QWidget>

#include <cmath>

#include "gamelogic.h"
#include "global.h"
#include "pausewindow.h"
#include "soundmanager.h"

const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;

const int RENDER_FPS = 60;
const int RENDER_FRAME_TIME = 1000 / RENDER_FPS;

const int LOGIC_FPS = 60;
const int LOGIC_FRAME_TIME = 1000 / LOGIC_FPS;

const int HP_MP_BAR_X_OFFSET = 188;
const int HP_BAR_Y_OFFSET = 29;
const int MP_BAR_Y_OFFSET = 100;
const int HP_MP_BAR_WIDTH = 285;
const int HP_MP_BAR_HEIGHT = 35;
const int EXP_BAR_Y_OFFSET = 668;
const int EXP_BAR_WIDTH = 1280;
const int EXP_BAR_HEIGHT = 52;
const int SURVIVAL_TIME_RECT_X_OFFSET = 980;
const int SURVIVAL_ITME_RECT_Y_OFFSET = 18;
const int SURVIVAL_TIME_RECT_WIDTH = 280;
const int SURVIVAL_TIME_RECT_HEIGHT = 100;

class GameWindow : public QWidget {
    Q_OBJECT

private:
    SoundManager *soundManager;

    QPixmap bar = QPixmap(":/images/ui/bar");
    QRect hpBar;
    QRect mpBar;
    QRect expBar;
    QRect survivalTimeRect;

    QPixmap avatar;

    void updateStatusBarParams();

    QPoint viewport = QPoint(0, 0);

    Global *global;
    GameLogic *gameLogic;

    QSet<int> pressedKeys;

    QTimer *logicTimer;
    QTimer *renderTimer;
    QTimer *survivalTimer;

    bool isGamePaused = false;
    PauseWindow *pauseWindow;

    EnhancementWindow *enhancementWindow;

public:
    explicit GameWindow(Global *global,
                        SoundManager *soundManager,
                        MagicalGirlEnum playerSelection,
                        QWidget *parent = nullptr);

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

    void onLevelUp(QVector<Enhancement *> &enhancements);
    void onLevelUpFinished();

signals:
    void startNewGame();
};

#endif // GAMEWINDOW_H
