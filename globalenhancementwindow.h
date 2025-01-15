#ifndef GLOBALENHANCEMENTWINDOW_H
#define GLOBALENHANCEMENTWINDOW_H

#include <QWidget>

#include "enhancement.h"
#include "global.h"
#include "qpushbutton.h"

namespace Ui {
class GlobalEnhancementWindow;
}

class GlobalEnhancementWindow : public QWidget {
    Q_OBJECT

public:
    explicit GlobalEnhancementWindow(Global *global, QWidget *parent = nullptr);
    ~GlobalEnhancementWindow();

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

    QVector<Enhancement *> globalEnhancements;
    QVector<QVector<int>> prices;

    void updateButtonText(QPushButton *button, int index);
    void purchaseGlobalEnhancement(QPushButton *button, int index);

signals:
    void backToMenu();
};

#endif // GLOBALENHANCEMENTWINDOW_H
