#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "characterselectwindow.h"
#include "gamewindow.h"
#include "global.h"
#include "globalenhancementwindow.h"
#include "magicalgirl.h"
#include "soundmanager.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow { // 主窗口类
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    SoundManager *soundManager; // 音频管理

    Global *global;                        // 全局设置
    bool isSettingKeyboardMapping = false; // 是否在改键位
    GameKey keyToSet;                      // 要改的键位

    MagicalGirlEnum lastPlayerSelection; // 记录上次选择的角色，用于直接重开

    CharacterSelectWindow *characterSelectWindow;     // 角色选择窗口
    GlobalEnhancementWindow *globalEnhancementWindow; // 全局强化窗口
    GameWindow *gameWindow;                           // 游戏内窗口

    void onGameStart();
    void onGlobalEnhancement();

    void hideWidgets(); // 隐藏当前组件，防止和新窗口重叠
    void showWidgets();

    void updateKeyMappingText(); // 更新键位显示信息

protected:
    void keyPressEvent(QKeyEvent *event) override;

public slots:
    void onPlayerSelected(MagicalGirlEnum player);
    void onRestartGame();
private slots:
    void on_back_2_clicked();
    void on_back_clicked();
    void on_setKeyboardMapping_clicked();
    void on_setting_clicked();
    void on_backgroundMusic_clicked();
    void on_soundEffect_clicked();
    void on_voiceEffect_clicked();
    void on_up_clicked();
    void on_down_clicked();
    void on_left_clicked();
    void on_right_clicked();
    void on_attack_clicked();
    void on_skill_clicked();
    void on_dodge_clicked();
};
#endif // MAINWINDOW_H
