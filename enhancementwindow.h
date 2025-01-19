#ifndef ENHANCEMENTWINDOW_H
#define ENHANCEMENTWINDOW_H

#include <QVector>
#include <QWidget>

#include "enhancement.h"
#include "soundmanager.h"

namespace Ui {
class EnhancementWindow;
}

class EnhancementWindow : public QWidget { // 局内强化选择窗口
    Q_OBJECT

public:
    explicit EnhancementWindow(SoundManager *soundManager, QWidget *parent = nullptr);
    ~EnhancementWindow();

    void setDescriptions(QVector<Enhancement *> enhancements); // 生成强化的描述

private:
    Ui::EnhancementWindow *ui;

    SoundManager *soundManager;

signals:
    void enhancementSelected(int index);
};

#endif // ENHANCEMENTWINDOW_H
