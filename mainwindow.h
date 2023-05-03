#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "dbmanager.h"

#include <QScreen>
#include <QPixmap>
#include <QFile>
#include <QCryptographicHash>

#include <qsqlquery.h>

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
    void on_startButton_clicked();

    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    DBManager*db;
};
#endif // MAINWINDOW_H
