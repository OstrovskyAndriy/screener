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
    void on_startButton_clicked();

    void on_pushButton_clicked();

    void viewOfTable();

private:
    Ui::MainWindow *ui;
    DBManager*db;
    QSqlQueryModel*queryModel;
    QPixmap pixmap;
};
#endif // MAINWINDOW_H
