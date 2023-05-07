#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "dbmanager.h"

#include <QScreen>
#include <QPixmap>
#include <QFile>
#include <QCryptographicHash>

#include <QSqlQuery>
#include <QStandardItemModel>

#include <QtConcurrent>
#include <QtMath>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void makeScreenshot();
    void stopScreenhoting();
    void startTimer();
    QPixmap getLastScreenshot();

    //якщо функція не статична то вибиває помилка бо хочу передати назву функції як вказівник на функцію
    static float compareImagesInThread(const QPixmap &img1,const QPixmap &img2);

    void viewOfTable();
    void addRowToTable();

    void on_closebutton_clicked();

private:
    Ui::MainWindow *ui;
    DBManager*db;
    QPixmap pixmap;
    QByteArray data;
    QTimer *timer;
    QStandardItemModel *model=nullptr;
};
#endif // MAINWINDOW_H
