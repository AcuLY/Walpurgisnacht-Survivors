#include <QApplication>
#include <mainwindow.h>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    MainWindow window;
    window.setWindowTitle("Walpurgisnacht Survivors");
    window.show();

    return app.exec();
}
