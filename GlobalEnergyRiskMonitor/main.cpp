#include <QApplication>
#include <QTimer>
#include "mainwindow.h"
#include "splashscreen.h"
#include "stylemanager.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName("Global Energy Risk Monitor");
    app.setOrganizationName("GERM");
    app.setApplicationVersion("2.0");

    // Apply global stylesheet
    app.setStyleSheet(StyleManager::instance().getApplicationStyleSheet());

    // Show splash screen
    SplashScreen *splash = new SplashScreen();
    MainWindow *mainWindow = new MainWindow();

    QObject::connect(splash, &SplashScreen::finished, [splash, mainWindow]() {
        splash->close();
        splash->deleteLater();
        mainWindow->show();
    });

    splash->startAnimation();

    return app.exec();
}
