#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <QKeyEvent>
#include <QPainter>
#include <QSet>
#include <QTimer>
#include <QWidget>

#include <cmath>

#include "enhancementwindow.h"
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
const int SURVIVAL_TIME_RECT_Y_OFFSET = 18;
const int SURVIVAL_TIME_RECT_WIDTH = 280;
const int SURVIVAL_TIME_RECT_HEIGHT = 100;

class GameWindow : public QWidget { // 游戏窗口类
    Q_OBJECT

private:
    SoundManager *soundManager;

    QPixmap bar = QPixmap(":/images/ui/bar"); // 游戏内 ui 的贴图
    QRect hpBar;                              // 血条矩形
    QRect mpBar;                              // 蓝条矩形
    QRect expBar;                             // 经验条矩形
    QRect survivalTimeRect;                   // 剩余时间的位置矩形
    QPixmap avatar;                           // 角色头像

    void updateStatusBarParams(); // 更新状态条的数据

    QPoint viewport = QPoint(0, 0); // 窗口左上角的坐标

    Global *global;
    GameLogic *gameLogic;

    QSet<int> pressedKeys; // 记录当前按下的按钮

    QTimer *logicTimer;  // 逻辑帧时间
    QTimer *renderTimer; // 渲染帧时间

    QTimer *survivalTimer; // 剩余游戏时间

    bool isGamePaused = false;
    PauseWindow *pauseWindow; // 游戏暂停窗口

    EnhancementWindow *enhancementWindow; // 局内强化窗口

public:
    explicit GameWindow(Global *global,
                        SoundManager *soundManager,
                        MagicalGirlEnum playerSelection,
                        QWidget *parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

    Direction getPlayerMovement(); // 获取玩家移动方向
    bool getPlayerAttack();        // 获取攻击状态

    void updateViewport(); // 根据玩家移动更新左上角坐标

    void updateAndRenderUI(QPainter *painter); // 更新并渲染 UI

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
