#ifndef CHARACTERSELECTWINDOW_H
#define CHARACTERSELECTWINDOW_H

#include <QKeyEvent>
#include <QWidget>

#include "magicalgirl.h"
#include "soundmanager.h"

namespace Ui {
class CharacterSelectWindow;
}

class CharacterSelectWindow : public QWidget { // 角色选择窗口
    Q_OBJECT

public:
    explicit CharacterSelectWindow(SoundManager *soundManager, QWidget *parent = nullptr);
    ~CharacterSelectWindow();

signals:
    void backToMenu();
    void playerSelected(MagicalGirlEnum player);

private:
    Ui::CharacterSelectWindow *ui;

    SoundManager *soundManager;
};

#endif // CHARACTERSELECTWINDOW_H
