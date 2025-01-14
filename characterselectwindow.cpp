#include "characterselectwindow.h"
#include "ui_characterselectwindow.h"

CharacterSelectWindow::CharacterSelectWindow(QWidget *parent)
    : QWidget(parent), ui(new Ui::CharacterSelectWindow) {
    qDebug() << "create select window";
    ui->setupUi(this);

    this->setFixedSize(parent->geometry().width(), parent->geometry().height());

    // 返回
    connect(ui->exit, &QPushButton::clicked, this, [this] {
        this->hide();
        emit backToMenu();
    });

    // 选择角色事件
    connect(ui->madoka, &QPushButton::clicked, this, [this] {
        onPlayerSelected(MagicalGirlEnum::Madoka);
    });
    connect(ui->homura, &QPushButton::clicked, this, [this] {
        onPlayerSelected(MagicalGirlEnum::Homura);
    });
    connect(ui->sayaka, &QPushButton::clicked, this, [this] {
        onPlayerSelected(MagicalGirlEnum::Sayaka);
    });
    connect(ui->mami, &QPushButton::clicked, this, [this] {
        onPlayerSelected(MagicalGirlEnum::Mami);
    });
    connect(ui->kyouko, &QPushButton::clicked, this, [this] {
        onPlayerSelected(MagicalGirlEnum::Kyouko);
    });
}

CharacterSelectWindow::~CharacterSelectWindow() {
    delete ui;
}

void CharacterSelectWindow::onPlayerSelected(MagicalGirlEnum playerSelection) { // 传递角色编号
    emit playerSelectedSignal(playerSelection);
}
