#include "mainwindow.h"
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(0);

    global = new Global();

    soundManager = new SoundManager(global, this);
    soundManager->playMenuMusic();

    updateKeyMappingText();

    connect(ui->startGame, &QPushButton::clicked, this, &MainWindow::onGameStart); // 开始游戏按钮
    connect(ui->enhancement, &QPushButton::clicked, this, &MainWindow::onGlobalEnhancement);
    connect(ui->quitGame, &QPushButton::clicked, this, &QApplication::quit); // 退出游戏按钮

    characterSelectWindow = new CharacterSelectWindow(soundManager, this);
    characterSelectWindow->hide();

    globalEnhancementWindow = new GlobalEnhancementWindow(global, soundManager, this);
    globalEnhancementWindow->hide();

    // 动态连接所有按钮
    QStringList buttonNames = {"startGame",
                               "enhancement",
                               "setting",
                               "quitGame",
                               "soundEffect",
                               "attack",
                               "back",
                               "back_2",
                               "backgroundMusic",
                               "dodge",
                               "down",
                               "left",
                               "right",
                               "setKeyboardMapping",
                               "skill",
                               "up",
                               "voiceEffect"};
    for (const QString &buttonName : buttonNames) {
        QPushButton *button = findChild<QPushButton *>(buttonName);
        if (button) {
            connect(button, &QPushButton::clicked, this, [this] {
                soundManager->playSfx("select");
            });
        }
    }
}

MainWindow::~MainWindow() {
    delete ui;

    // 更新 save.json 文件
    QJsonObject settings;
    settings["backgroundMusic"] = global->isBackgroundMusicOn();
    settings["soundEffect"] = global->isSoundEffectOn();
    settings["voiceEffect"] = global->isVoiceEffectOn();

    QJsonArray keyboardMapping;
    auto mapping = global->getKeyboardMapping();
    keyboardMapping.append(mapping[GameKey::up]);
    keyboardMapping.append(mapping[GameKey::down]);
    keyboardMapping.append(mapping[GameKey::left]);
    keyboardMapping.append(mapping[GameKey::right]);
    keyboardMapping.append(mapping[GameKey::attack]);
    keyboardMapping.append(mapping[GameKey::skill]);
    keyboardMapping.append(mapping[GameKey::dodge]);
    settings["keyboardMapping"] = keyboardMapping;

    QJsonArray globalEnhancementLevels;
    for (int i = 0; i < 6; i++) {
        globalEnhancementLevels.append(global->getGlobalEnhancementLevel(i));
    }

    QJsonObject rootObject;
    rootObject["settings"] = settings;
    rootObject["money"] = global->getMoney();
    rootObject["globalEnhancementLevels"] = globalEnhancementLevels;

    QJsonArray outerArray;
    outerArray.append(rootObject);

    QJsonDocument doc(outerArray);

    QFile file(saveFilePath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        file.write(doc.toJson());
        file.close();
    } else {
        qWarning() << "Failed to open file for writing:" << file.errorString();
    }
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

void MainWindow::onGlobalEnhancement() {
    globalEnhancementWindow->updateInfo();
    globalEnhancementWindow->show();
    hideWidgets();

    connect(globalEnhancementWindow,
            &GlobalEnhancementWindow::backToMenu,
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

void MainWindow::updateKeyMappingText() {
    auto mapping = global->getKeyboardMapping();
    ui->up->setText("上移动：" + QKeySequence(mapping[GameKey::up]).toString());
    ui->down->setText("下移动：" + QKeySequence(mapping[GameKey::down]).toString());
    ui->left->setText("左移动：" + QKeySequence(mapping[GameKey::left]).toString());
    ui->right->setText("右移动：" + QKeySequence(mapping[GameKey::right]).toString());
    ui->attack->setText("攻击：" + QKeySequence(mapping[GameKey::attack]).toString());
    ui->skill->setText("技能：" + QKeySequence(mapping[GameKey::skill]).toString());
    ui->dodge->setText("闪避：" + QKeySequence(mapping[GameKey::dodge]).toString());
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

    soundManager->stopBackgroundMusic();
    soundManager->playGameMusic();

    gameWindow = new GameWindow(global, soundManager, playerSelection, this);
    connect(gameWindow, &GameWindow::startNewGame, this, &MainWindow::onRestartGame); // 重开
    gameWindow->show();

    characterSelectWindow->hide();
    showWidgets();
}

void MainWindow::onRestartGame() {
    soundManager->stopBackgroundMusic();
    soundManager->playGameMusic();

    gameWindow = new GameWindow(global, soundManager, lastPlayerSelection, this);
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
        soundManager->playMenuMusic();
    } else {
        ui->backgroundMusic->setText("音乐：关");
        soundManager->stopBackgroundMusic();
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
