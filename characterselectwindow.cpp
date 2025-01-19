#include "characterselectwindow.h"
#include "ui_characterselectwindow.h"

CharacterSelectWindow::CharacterSelectWindow(SoundManager *soundManager, QWidget *parent)
    : QWidget(parent), ui(new Ui::CharacterSelectWindow), soundManager(soundManager) {
    ui->setupUi(this);

    this->setFixedSize(parent->geometry().width(), parent->geometry().height());

    // 返回按钮
    connect(ui->exit, &QPushButton::clicked, this, [this] {
        this->hide();
        emit backToMenu();
    });

    // 动态连接所有按钮的音效
    QStringList buttonNames = {"exit", "madoka", "homura", "sayaka", "mami", "kyouko"};
    for (const QString &buttonName : buttonNames) {
        QPushButton *button = findChild<QPushButton *>(buttonName);
        if (button) {
            connect(button, &QPushButton::clicked, this, [this] {
                this->soundManager->playSfx("select");
            });
        }
    }

    // 选择角色事件
    // connect(ui->madoka, &QPushButton::clicked, this, [this] {
    //     onPlayerSelected(MagicalGirlEnum::Madoka);
    // });
    connect(ui->homura, &QPushButton::clicked, this, [this] {
        emit playerSelected(MagicalGirlEnum::Homura);
    });
    connect(ui->sayaka, &QPushButton::clicked, this, [this] {
        emit playerSelected(MagicalGirlEnum::Sayaka);
    });
    // connect(ui->mami, &QPushButton::clicked, this, [this] {
    //     onPlayerSelected(MagicalGirlEnum::Mami);
    // });
    // connect(ui->kyouko, &QPushButton::clicked, this, [this] {
    //     onPlayerSelected(MagicalGirlEnum::Kyouko);
    // });
}

CharacterSelectWindow::~CharacterSelectWindow() {
    delete ui;
}
