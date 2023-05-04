#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    db=DBManager::getInstance();
    //db->connectToDataBase();

    queryModel=new QSqlQueryModel;
    timer=new QTimer(this);

    connect(ui->startAndStop,&QPushButton::clicked,this,&MainWindow::makeScreenshot);
    connect(ui->startAndStop,&QPushButton::clicked,this,&MainWindow::startTimer);
    connect(timer, &QTimer::timeout, this, &MainWindow::makeScreenshot);

    viewOfTable();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete timer;
}


void MainWindow::makeScreenshot()
{
    disconnect(ui->startAndStop,&QPushButton::clicked,this,&MainWindow::makeScreenshot);
    connect(ui->startAndStop,&QPushButton::clicked,this,&MainWindow::stopScreenhoting);

    QScreen *screen = QGuiApplication::primaryScreen();

    // зробити скрін і збереги його в QPixmap
    QPixmap screenshot = screen->grabWindow(0);

    // зберегти скріншот як тимчасовий файл
    screenshot.save("./screenshot.png");

    QString filePath = "./screenshot.png";
    QFile file(filePath);

    if (file.open(QIODevice::ReadOnly)) {
        //клас містить набір функцій хешування, включаючи MD5, SHA-1, SHA-256 та інші
        QCryptographicHash hash(QCryptographicHash::Sha256);

        if (hash.addData(&file)) {
            QByteArray result = hash.result();
            QString hashStr = QString(result.toHex());
            qDebug() << "Hash of" << filePath << "is" << hashStr;

            QByteArray bytes;
            QBuffer buffer(&bytes);
            buffer.open(QIODevice::WriteOnly);
            screenshot.save(&buffer, "PNG");

            db->insert(1,bytes,hashStr);

            this->viewOfTable();

        }
        else {
            qWarning() << "Failed to add data to hash.";
        }
        file.close();
    }
    else {
        qWarning() << "Failed to open file" << filePath;
    }

    ui->startAndStop->setText("stop");
}

void MainWindow::stopScreenhoting()
{
    disconnect(ui->startAndStop,&QPushButton::clicked,this,&MainWindow::stopScreenhoting);
    connect(ui->startAndStop,&QPushButton::clicked,this,&MainWindow::makeScreenshot);


    ui->startAndStop->setText("start");
}

void MainWindow::startTimer()
{
    if (!timer->isActive()) {
        // Встановлюємо інтервал на 5 секунд
        timer->setInterval(5000);
        timer->start();

        qDebug() << "Timer started";
    }
    else {
        // Якщо таймер активний, то зупиняємо його
        timer->stop();
        qDebug() << "Timer stopped";
    }
}


void MainWindow::viewOfTable()
{
    QSqlQuery query("SELECT image FROM images ORDER BY id DESC");
    QStandardItemModel *model = new QStandardItemModel;

    // Додати заголовки стовпців
    model->setHorizontalHeaderLabels(QStringList() << "Image");

    // Обробити результат запиту
    QPixmap pixmap;
    QPixmap scaledPixmap;
    while (query.next()) {
        QByteArray data = query.value(0).toByteArray();
        pixmap.loadFromData(data);
        QStandardItem *item = new QStandardItem;

        // Підігнати розмір зображення під розмір tableView
        scaledPixmap = pixmap.scaled(ui->tableView->width(), ui->tableView->height(),
                                     Qt::KeepAspectRatio, Qt::SmoothTransformation);

        item->setData(QVariant(scaledPixmap), Qt::DecorationRole);
        model->appendRow(item);
    }

    // Відобразити дані у QTableView
    ui->tableView->setModel(model);
    ui->tableView->setColumnWidth(0,scaledPixmap.width());
    ui->tableView->verticalHeader()->setDefaultSectionSize(scaledPixmap.height());
    ui->tableView->verticalHeader()->setVisible(false);
    ui->tableView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

}
