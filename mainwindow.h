#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "characterselectwindow.h"
#include "gamewindow.h"
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

    CharacterSelectWindow *characterSelectWindow;
    GameWindow *gameWindow;

    void onGameStart();

public slots:
    void onPlayerSelected(MagicalGirlEnum player);
    void onRestartGame();
};
#endif // MAINWINDOW_H
