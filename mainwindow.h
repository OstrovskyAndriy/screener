#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "dbmanager.h"

#include <QScreen>
#include <QPixmap>
#include <QFile>
#include <QCryptographicHash>

#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QStandardItemModel>
#include <QItemDelegate>


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

    float compareImages(const QPixmap &img1,const QPixmap &img2);

    void viewOfTable();

private:
    Ui::MainWindow *ui;
    DBManager*db;
    QSqlQueryModel*queryModel;
    QPixmap pixmap;
    QTimer *timer;
};
#endif // MAINWINDOW_H
