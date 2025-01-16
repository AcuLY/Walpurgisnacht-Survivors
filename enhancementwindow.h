#ifndef ENHANCEMENTWINDOW_H
#define ENHANCEMENTWINDOW_H

#include <QVector>
#include <QWidget>

#include "enhancement.h"
#include "soundmanager.h"

namespace Ui {
class EnhancementWindow;
}

class EnhancementWindow : public QWidget {
    Q_OBJECT

public:
    explicit EnhancementWindow(SoundManager *soundManager, QWidget *parent = nullptr);
    ~EnhancementWindow();

    void setDescriptions(QVector<Enhancement *> enhancements);

private:
    Ui::EnhancementWindow *ui;

    SoundManager *soundManager;

signals:
    void selectEnhancement(int index);
};

#endif // ENHANCEMENTWINDOW_H
