#ifndef CHARACTERSELECTWINDOW_H
#define CHARACTERSELECTWINDOW_H

#include <QKeyEvent>
#include <QWidget>

#include "magicalgirl.h"
#include "soundmanager.h"

namespace Ui {
class CharacterSelectWindow;
}

class CharacterSelectWindow : public QWidget {
    Q_OBJECT

public:
    explicit CharacterSelectWindow(SoundManager *soundManager, QWidget *parent = nullptr);
    ~CharacterSelectWindow();

signals:
    void backToMenu();
    void playerSelectedSignal(MagicalGirlEnum player);

private:
    Ui::CharacterSelectWindow *ui;

    SoundManager *soundManager;

    void onPlayerSelected(MagicalGirlEnum player);
};

#endif // CHARACTERSELECTWINDOW_H
