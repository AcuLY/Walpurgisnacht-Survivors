#ifndef GLOBALENHANCEMENTWINDOW_H
#define GLOBALENHANCEMENTWINDOW_H

#include <QWidget>

#include "enhancement.h"
#include "global.h"
#include "qpushbutton.h"
#include "soundmanager.h"

namespace Ui {
class GlobalEnhancementWindow;
}

class GlobalEnhancementWindow : public QWidget { // 全局强化窗口类
    Q_OBJECT

public:
    explicit GlobalEnhancementWindow(Global *global,
                                     SoundManager *soundManager,
                                     QWidget *parent = nullptr);
    ~GlobalEnhancementWindow();

    void updateInfo();

private slots:
    void on_back_clicked();
    void on_globalEnhancement1_clicked();
    void on_globalEnhancement2_clicked();
    void on_globalEnhancement3_clicked();
    void on_globalEnhancement4_clicked();
    void on_globalEnhancement5_clicked();
    void on_globalEnhancement6_clicked();

private:
    Ui::GlobalEnhancementWindow *ui;

    Global *global;
    SoundManager *soundManager;

    QVector<Enhancement *> globalEnhancements; // 全局强化词条
    QVector<QVector<int>> prices;              // 价格

    void updateButtonText(QPushButton *button, int index); // 更新全局强化的显示信息
    void purchaseGlobalEnhancement(QPushButton *button, int index); // 升级

signals:
    void backToMenu();
};

#endif // GLOBALENHANCEMENTWINDOW_H
