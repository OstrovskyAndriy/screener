#include "mainwindow.h"

#include <QApplication>
#include <QMenu>
#include <QSystemTrayIcon>
#include <QAction>
#include<QKeyEvent>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    QSystemTrayIcon *trayIcon = new QSystemTrayIcon(QIcon(":/resources/img/screenshot_icon.png"));
    trayIcon->setVisible(true);

    // контекстне меню в треї
    QMenu *menu = new QMenu;
    QAction *openAction = new QAction("Open", menu);
    QAction *quitAction = new QAction("Quit", menu);
    menu->addAction(openAction);
    menu->addAction(quitAction);
    trayIcon->setContextMenu(menu);
    trayIcon->show();

    //не виключати програму якщо закрито
    a.setQuitOnLastWindowClosed(false);

    QObject::connect(openAction, &QAction::triggered, &w, &MainWindow::show);

    QObject::connect(quitAction, &QAction::triggered, &a, &QApplication::quit);

    w.show();
    return a.exec();
}
