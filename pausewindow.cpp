#include "pausewindow.h"
#include "ui_pausewindow.h"

PauseWindow::PauseWindow(QWidget *parent) : QWidget(parent), ui(new Ui::PauseWindow) {
    ui->setupUi(this);

    this->setAutoFillBackground(true); // 防止新窗口和旧窗口重叠

    connect(ui->continueGame, &QPushButton::clicked, this, [this] { emit closePauseWindow(true); });
    connect(ui->backToMenu, &QPushButton::clicked, this, [this] { emit closePauseWindow(false); });
}

PauseWindow::~PauseWindow() {
    this->hide();
    delete ui;
}

void PauseWindow::setStatus(PauseStatus status) {
    this->status = status;

    // 根据游戏状态调整按钮
    if (status == PauseStatus::GamePause) {
        ui->label->setText("游戏暂停");
        ui->continueGame->setText("继续游戏");
    }
    if (status == PauseStatus::GameLose) {
        ui->label->setText("游戏失败");
        ui->continueGame->setText("重新游戏");
    }
    if (status == PauseStatus::GameWin) {
        ui->label->setText("游戏胜利");
        ui->continueGame->setText("重新游戏");
    }
}

PauseStatus PauseWindow::getStatus() const {
    return status;
}
