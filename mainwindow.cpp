#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(0);

    global = new Global();

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
    hideWidgets();

    connect(characterSelectWindow,
            &CharacterSelectWindow::playerSelectedSignal,
            this,
            &MainWindow::onPlayerSelected,
            Qt::UniqueConnection); // 使用 uniqueconnnection，不然会重复绑定
    connect(characterSelectWindow,
            &CharacterSelectWindow::backToMenu,
            this,
            &MainWindow::showWidgets);
}

void MainWindow::hideWidgets() {
    ui->label->hide();
    ui->startGame->hide();
    ui->enhancement->hide();
    ui->quitGame->hide();
    ui->setting->hide();
}

void MainWindow::showWidgets() {
    ui->label->show();
    ui->startGame->show();
    ui->enhancement->show();
    ui->quitGame->show();
    ui->setting->show();
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    if (isSettingKeyboardMapping) {
        global->setKeyboardMapping(keyToSet, event->key());
        QString keyName = QKeySequence(event->key()).toString();

        if (keyToSet == GameKey::up) {
            ui->up->setText("上移动：" + keyName);
        }
        if (keyToSet == GameKey::down) {
            ui->down->setText("下移动：" + keyName);
        }
        if (keyToSet == GameKey::left) {
            ui->left->setText("左移动：" + keyName);
        }
        if (keyToSet == GameKey::right) {
            ui->right->setText("右移动：" + keyName);
        }
        if (keyToSet == GameKey::attack) {
            ui->attack->setText("攻击：" + keyName);
        }
        if (keyToSet == GameKey::skill) {
            ui->skill->setText("技能：" + keyName);
        }
        if (keyToSet == GameKey::dodge) {
            ui->dodge->setText("闪避：" + keyName);
        }

        isSettingKeyboardMapping = false;
    }
}

void MainWindow::onPlayerSelected(MagicalGirlEnum playerSelection) {
    lastPlayerSelection = playerSelection;

    gameWindow = new GameWindow(global, playerSelection, this);
    connect(gameWindow, &GameWindow::startNewGame, this, &MainWindow::onRestartGame); // 重开
    gameWindow->show();

    characterSelectWindow->hide();
    showWidgets();
}

void MainWindow::onRestartGame() {
    gameWindow = new GameWindow(global, lastPlayerSelection, this);
    connect(gameWindow, &GameWindow::startNewGame, this, &MainWindow::onRestartGame); // 重开
    gameWindow->show();
}

void MainWindow::on_back_2_clicked() {
    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::on_back_clicked() {
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::on_setKeyboardMapping_clicked() {
    isSettingKeyboardMapping = false;
    ui->stackedWidget->setCurrentIndex(2);
}

void MainWindow::on_setting_clicked() {
    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::on_backgroundMusic_clicked() {
    global->setBackgroundMusic();

    if (global->isBackgroundMusicOn()) {
        ui->backgroundMusic->setText("音乐：开");
    } else {
        ui->backgroundMusic->setText("音乐：关");
    }
}

void MainWindow::on_soundEffect_clicked() {
    global->setSoundEffect();

    if (global->isSoundEffectOn()) {
        ui->soundEffect->setText("音效：开");
    } else {
        ui->soundEffect->setText("音效：关");
    }
}

void MainWindow::on_voiceEffect_clicked() {
    global->setVoiceEffect();

    if (global->isVoiceEffectOn()) {
        ui->voiceEffect->setText("语音：开");
    } else {
        ui->voiceEffect->setText("语音：关");
    }
}

void MainWindow::on_up_clicked() {
    isSettingKeyboardMapping = true;
    keyToSet = GameKey::up;
}

void MainWindow::on_down_clicked() {
    isSettingKeyboardMapping = true;
    keyToSet = GameKey::down;
}

void MainWindow::on_left_clicked() {
    isSettingKeyboardMapping = true;
    keyToSet = GameKey::left;
}

void MainWindow::on_right_clicked() {
    isSettingKeyboardMapping = true;
    keyToSet = GameKey::right;
}

void MainWindow::on_attack_clicked() {
    isSettingKeyboardMapping = true;
    keyToSet = GameKey::attack;
}

void MainWindow::on_skill_clicked() {
    isSettingKeyboardMapping = true;
    keyToSet = GameKey::skill;
}

void MainWindow::on_dodge_clicked() {
    isSettingKeyboardMapping = true;
    keyToSet = GameKey::dodge;
}
