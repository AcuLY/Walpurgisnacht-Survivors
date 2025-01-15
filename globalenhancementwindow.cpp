#include "globalenhancementwindow.h"
#include "ui_globalenhancementwindow.h"

GlobalEnhancementWindow::GlobalEnhancementWindow(Global *global, QWidget *parent)
    : QWidget(parent), ui(new Ui::GlobalEnhancementWindow), global(global) {
    ui->setupUi(this);

    QJsonArray globalEnhancementJsons = FileUtils::loadJsonFile(
        ":/data/enhancement/global_enhancement_jsons");
    for (const QJsonValue &value : globalEnhancementJsons) {
        QJsonObject json = value.toObject();
        Enhancement *e = new Enhancement(json);
        globalEnhancements.append(e);

        QVector<int> currentPrices;
        QJsonArray priceArray = json["price"].toArray();
        for (const QJsonValue &value : priceArray) {
            currentPrices.append(value.toDouble());
        }
        prices.append(currentPrices);
    }

    ui->money->setText("金币：" + QString::number(global->getMoney()));
    updateButtonText(ui->globalEnhancement1, 0);
    updateButtonText(ui->globalEnhancement2, 1);
    updateButtonText(ui->globalEnhancement3, 2);
    updateButtonText(ui->globalEnhancement4, 3);
    updateButtonText(ui->globalEnhancement5, 4);
    updateButtonText(ui->globalEnhancement6, 5);
}

GlobalEnhancementWindow::~GlobalEnhancementWindow() {
    delete ui;
}

void GlobalEnhancementWindow::on_back_clicked() {
    this->hide();
    emit backToMenu();
}

void GlobalEnhancementWindow::updateButtonText(QPushButton *button, int index) {
    int level = global->getGlobalEnhancementLevel(index);

    QString enhancementDescription = globalEnhancements[index]->getDescription() + "\n";
    QString levelDescription = "等级：" + QString::number(level) + "\n";
    QString priceDescription = level == 5 ? "已满级"
                                          : "价格：" + QString::number(prices[index][level]);
    button->setText(enhancementDescription + levelDescription + priceDescription);
}

void GlobalEnhancementWindow::purchaseGlobalEnhancement(QPushButton *button, int index) {
    int level = global->getGlobalEnhancementLevel(index);
    if (level == 5 || global->getMoney() < prices[index][level]) {
        return;
    }

    global->useMoney(prices[index][level]);
    global->addGlobalEnhancementLevel(index);

    updateButtonText(button, index);

    ui->money->setText("金币：" + QString::number(global->getMoney()));
}

void GlobalEnhancementWindow::on_globalEnhancement1_clicked() {
    purchaseGlobalEnhancement(ui->globalEnhancement1, 0);
}

void GlobalEnhancementWindow::on_globalEnhancement2_clicked() {
    purchaseGlobalEnhancement(ui->globalEnhancement2, 1);
}

void GlobalEnhancementWindow::on_globalEnhancement3_clicked() {
    purchaseGlobalEnhancement(ui->globalEnhancement3, 2);
}

void GlobalEnhancementWindow::on_globalEnhancement4_clicked() {
    purchaseGlobalEnhancement(ui->globalEnhancement4, 3);
}

void GlobalEnhancementWindow::on_globalEnhancement5_clicked() {
    purchaseGlobalEnhancement(ui->globalEnhancement5, 4);
}

void GlobalEnhancementWindow::on_globalEnhancement6_clicked() {
    purchaseGlobalEnhancement(ui->globalEnhancement6, 5);
}
