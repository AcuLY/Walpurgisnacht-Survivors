#ifndef PAUSEWINDOW_H
#define PAUSEWINDOW_H

#include <QWidget>

enum class PauseStatus { GamePause, GameWin, GameLose };

namespace Ui {
class PauseWindow;
}

class PauseWindow : public QWidget {
    Q_OBJECT

public:
    explicit PauseWindow(QWidget *parent = nullptr);
    ~PauseWindow();

    void setStatus(PauseStatus status);
    PauseStatus getStatus() const;

private:
    Ui::PauseWindow *ui;

    PauseStatus status = PauseStatus::GamePause;

signals:
    void closePauseWindow(bool isGameContinued);
};

#endif // PAUSEWINDOW_H
