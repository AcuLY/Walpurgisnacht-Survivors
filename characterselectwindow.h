#ifndef CHARACTERSELECTWINDOW_H
#define CHARACTERSELECTWINDOW_H

#include <QKeyEvent>
#include <QWidget>

#include "magicalgirl.h"

namespace Ui {
class CharacterSelectWindow;
}

class CharacterSelectWindow : public QWidget {
    Q_OBJECT

public:
    explicit CharacterSelectWindow(QWidget *parent = nullptr);
    ~CharacterSelectWindow();

signals:
    void playerSelectedSignal(MagicalGirlEnum player);

private:
    Ui::CharacterSelectWindow *ui;

    void onPlayerSelected(MagicalGirlEnum player);
};

#endif // CHARACTERSELECTWINDOW_H
