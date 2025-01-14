#include "enhancementwindow.h"
#include "ui_enhancementwindow.h"

EnhancementWindow::EnhancementWindow(QWidget *parent)
    : QWidget(parent), ui(new Ui::EnhancementWindow) {
    ui->setupUi(this);

    connect(ui->enhancement1, &QPushButton::clicked, this, [this] { emit selectEnhancement(0); });
    connect(ui->enhancement2, &QPushButton::clicked, this, [this] { emit selectEnhancement(1); });
    connect(ui->enhancement3, &QPushButton::clicked, this, [this] { emit selectEnhancement(2); });
}

EnhancementWindow::~EnhancementWindow() {
    delete ui;
}

void EnhancementWindow::setDescriptions(QVector<Enhancement *> enhancements) {
    ui->enhancement1->setText(enhancements[0]->getDescription());
    ui->enhancement2->setText(enhancements[1]->getDescription());
    ui->enhancement3->setText(enhancements[2]->getDescription());
}
