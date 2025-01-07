#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    connect(ui->startGame, &QPushButton::clicked, this, &MainWindow::onGameStart); // 开始游戏按钮
    connect(ui->quitGame, &QPushButton::clicked, this, &QApplication::quit); // 退出游戏按钮

    characterSelectWindow = new CharacterSelectWindow(this);
    characterSelectWindow->hide();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::onGameStart() {
    characterSelectWindow->show();

    connect(characterSelectWindow,
            &CharacterSelectWindow::playerSelectedSignal,
            this,
            &MainWindow::onPlayerSelected);
}

void MainWindow::onPlayerSelected(MagicalGirlEnum player) {
    gameWindow = new GameWindow(this);
    connect(gameWindow, &GameWindow::startNewGame, this, &MainWindow::onRestartGame); // 重开
    gameWindow->show();

    characterSelectWindow->hide();
}

void MainWindow::onRestartGame() {
    gameWindow = new GameWindow(this);
    connect(gameWindow, &GameWindow::startNewGame, this, &MainWindow::onRestartGame); // 重开
    gameWindow->show();
}
