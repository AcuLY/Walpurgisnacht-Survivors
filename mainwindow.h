#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "characterselectwindow.h"
#include "gamewindow.h"
#include "global.h"
#include "magicalgirl.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    Global *global;
    bool isSettingKeyboardMapping = false;
    GameKey keyToSet;

    MagicalGirlEnum lastPlayerSelection; // 记录上次选择的角色，用于直接重开

    CharacterSelectWindow *characterSelectWindow;
    GameWindow *gameWindow;

    void onGameStart();

    void hideWidgets();
    void showWidgets();

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
