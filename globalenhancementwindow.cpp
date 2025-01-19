#include "globalenhancementwindow.h"
#include "ui_globalenhancementwindow.h"

GlobalEnhancementWindow::GlobalEnhancementWindow(Global *global,
                                                 SoundManager *soundManager,
                                                 QWidget *parent)
    : QWidget(parent), ui(new Ui::GlobalEnhancementWindow), global(global),
      soundManager(soundManager) {
    ui->setupUi(this);

    // 读取全局强化词条
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

    // 动态连接所有按钮的音效
    QStringList buttonNames = {"back",
                               "globalEnhancement1",
                               "globalEnhancement2",
                               "globalEnhancement3",
                               "globalEnhancement4",
                               "globalEnhancement5",
                               "globalEnhancement6"};
    for (const QString &buttonName : buttonNames) {
        QPushButton *button = findChild<QPushButton *>(buttonName);
        if (button) {
            connect(button, &QPushButton::clicked, this, [this] {
                this->soundManager->playSfx("select");
            });
        }
    }

    updateInfo();
}

GlobalEnhancementWindow::~GlobalEnhancementWindow() {
    delete ui;
    for (auto it = globalEnhancements.begin(); it != globalEnhancements.end(); ++it) {
        delete *it;
    }
}

void GlobalEnhancementWindow::updateInfo() {
    ui->money->setText("金币：" + QString::number(global->getMoney()));
    updateButtonText(ui->globalEnhancement1, 0);
    updateButtonText(ui->globalEnhancement2, 1);
    updateButtonText(ui->globalEnhancement3, 2);
    updateButtonText(ui->globalEnhancement4, 3);
    updateButtonText(ui->globalEnhancement5, 4);
    updateButtonText(ui->globalEnhancement6, 5);
}

void GlobalEnhancementWindow::on_back_clicked() {
    this->hide();
    emit backToMenu();
}

void GlobalEnhancementWindow::updateButtonText(QPushButton *button, int index) {
    int level = global->getGlobalEnhancementLevel(index);

    QString enhancementDescription = globalEnhancements[index]->getDescription() + "\n";
    QString levelDescription = "等级：" + QString::number(level) + "\n";
    // 如果满级则不显示下一级的价格
    QString priceDescription = level == 5 ? "已满级"
                                          : "价格：" + QString::number(prices[index][level]);
    button->setText(enhancementDescription + levelDescription + priceDescription);
}

void GlobalEnhancementWindow::purchaseGlobalEnhancement(QPushButton *button, int index) {
    // 如果已经满级就无效
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
