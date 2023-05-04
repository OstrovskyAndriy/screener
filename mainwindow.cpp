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

    //    ui->tableView->setSelectionMode(QAbstractItemView::NoSelection);
    //    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    this->setFixedSize(this->geometry().width(),this->geometry().height());

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


    QPixmap lastScreen;
    QString filePath = "./screenshot.png";
    QFile file(filePath);
    float percent;

    lastScreen=getLastScreenshot();
    percent=compareImages(screenshot,lastScreen);

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

            db->insert(QString::number(percent,'f',2),bytes,hashStr);

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

QPixmap MainWindow::getLastScreenshot()
{
    QSqlQuery query("SELECT image FROM images ORDER BY id DESC LIMIT 1");

    // Обробити результат запиту
    QPixmap lastImg;

    if(query.next()){
        QByteArray data = query.value(0).toByteArray();
        lastImg.loadFromData(data);

    }
    return lastImg;
}


float MainWindow::compareImages(const QPixmap &image1, const QPixmap &image2)
{
    QImage img1 = image1.scaled(image2.size()).toImage();
    QImage img2 = image2.toImage();

    int differentPixels = 0;

    for (int y = 0; y < img1.height(); y++) {
        for (int x = 0; x < img1.width(); x++) {
            if (img1.pixel(x, y) != img2.pixel(x, y)) {
                differentPixels++;
            }
        }
    }

    float similarityPercentage = 100.0 - (differentPixels / (double)(img1.width() * img1.height())) * 100.0;

    return similarityPercentage;
}


void MainWindow::viewOfTable()
{

    QSqlQuery query("SELECT percent, image FROM images ORDER BY id DESC");
    QStandardItemModel *model = new QStandardItemModel;

    model->setColumnCount(2);
    model->setHeaderData(0, Qt::Horizontal, "%");
    model->setHeaderData(1, Qt::Horizontal, "Screenshot");

    // Обробити результат запиту
    QPixmap pixmap;
    while (query.next()) {
        QString percent = query.value(0).toString();
        QByteArray data = query.value(1).toByteArray();

        pixmap.loadFromData(data);
        QStandardItem *percentItem = new QStandardItem(percent);
        QStandardItem *imageItem = new QStandardItem;

        // Підігнати розмір зображення під розмір tableView
        pixmap = pixmap.scaled(ui->tableView->width()-43, ui->tableView->height()-26,
                                     Qt::KeepAspectRatio, Qt::SmoothTransformation);

        imageItem->setData(QVariant(pixmap), Qt::DecorationRole);
        model->appendRow({ percentItem, imageItem });
    }

    ui->tableView->setModel(model);
    ui->tableView->verticalHeader()->setDefaultSectionSize(pixmap.height());
    ui->tableView->verticalHeader()->setVisible(false);
    ui->tableView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    ui->tableView->setColumnWidth(0,40);
    ui->tableView->setColumnWidth(1,pixmap.width());

    qDebug()<<ui->tableView->width();
}
