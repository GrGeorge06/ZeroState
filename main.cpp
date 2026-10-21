#include <QApplication>
#include "browserwindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("BraveLite");

    BrowserWindow window(false);
    window.show();

    return app.exec();
}
