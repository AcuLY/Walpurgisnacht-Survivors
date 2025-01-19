#include "enhancementwindow.h"
#include "ui_enhancementwindow.h"

EnhancementWindow::EnhancementWindow(SoundManager *soundManager, QWidget *parent)
    : QWidget(parent), ui(new Ui::EnhancementWindow), soundManager(soundManager) {
    ui->setupUi(this);

    // 动态连接所有按钮的音效
    QStringList buttonNames = {"enhancement1", "enhancement2", "enhancement3"};
    for (const QString &buttonName : buttonNames) {
        QPushButton *button = findChild<QPushButton *>(buttonName);
        if (button) {
            connect(button, &QPushButton::clicked, this, [this] {
                this->soundManager->playSfx("select");
            });
        }
    }

    connect(ui->enhancement1, &QPushButton::clicked, this, [this] { emit enhancementSelected(0); });
    connect(ui->enhancement2, &QPushButton::clicked, this, [this] { emit enhancementSelected(1); });
    connect(ui->enhancement3, &QPushButton::clicked, this, [this] { emit enhancementSelected(2); });
}

EnhancementWindow::~EnhancementWindow() {
    delete ui;
}

void EnhancementWindow::setDescriptions(QVector<Enhancement *> enhancements) {
    ui->enhancement1->setText(enhancements[0]->getDescription());
    ui->enhancement2->setText(enhancements[1]->getDescription());
    ui->enhancement3->setText(enhancements[2]->getDescription());
}
