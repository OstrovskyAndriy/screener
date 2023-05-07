#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    db=DBManager::getInstance();
    //db->connectToDataBase();

    timer=new QTimer(this);

    connect(ui->startAndStop,&QPushButton::clicked,this,&MainWindow::makeScreenshot);
    connect(ui->startAndStop,&QPushButton::clicked,this,&MainWindow::startTimer);
    connect(timer, &QTimer::timeout, this, &MainWindow::makeScreenshot);

    ui->tableView->setSelectionMode(QAbstractItemView::NoSelection);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    this->setFixedSize(this->geometry().width(),this->geometry().height());

    viewOfTable();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete timer;
    delete model;
}


void MainWindow::makeScreenshot()
{
    disconnect(ui->startAndStop,&QPushButton::clicked,this,&MainWindow::makeScreenshot);
    connect(ui->startAndStop,&QPushButton::clicked,this,&MainWindow::stopScreenhoting);

    QScreen *screen = QGuiApplication::primaryScreen();

    // зробити скрін і збереги його в QPixmap
    QPixmap screenshot = screen->grabWindow(0);
    screenshot.save("./screenshot.png");

    QString filePath = "./screenshot.png";
    QFile file(filePath);
    float percent;

    pixmap=getLastScreenshot();

    QFuture<float> otherThread=QtConcurrent::run(compareImagesInThread,screenshot,pixmap);

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

            percent=otherThread.result();

            db->insert(QString::number(percent,'f',1),bytes,hashStr);

            //коли таблиця новостворена або пуста, дані з неї не будуть виводитись без цього коду
            //Qpixpam не заповниться і не встановиться розмір стовпчика для зображення
            //тому дописаний цей код
            if(model->rowCount()==1){
                delete model;
                model=nullptr;
                this->viewOfTable();
            }
            else{
                this->addRowToTable();
            }

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
        // Встановлюємо інтервал на хвилину
        timer->setInterval(60000);
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
        data = query.value(0).toByteArray();
        lastImg.loadFromData(data);

    }
    return lastImg;
}



float MainWindow::compareImagesInThread(const QPixmap &image1, const QPixmap &image2)
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
    if(qIsNaN(similarityPercentage)){
        return 0.0;
    }
    return similarityPercentage;

}


void MainWindow::viewOfTable()
{
    if(!model){
        QSqlQuery query("SELECT percent, image FROM images ORDER BY id DESC");
        model = new QStandardItemModel;

        model->setColumnCount(2);
        model->setHeaderData(0, Qt::Horizontal, "%");
        model->setHeaderData(1, Qt::Horizontal, "Screenshot");

        // Обробити результат запиту
        while (query.next()) {
            QString percent = query.value(0).toString();
            data = query.value(1).toByteArray();

            pixmap.loadFromData(data);

            QStandardItem *percentItem = new QStandardItem(percent);
            QStandardItem *imageItem = new QStandardItem;
            // деструктори потім не потрібно викликати бо QStandardItemModel сам звільнить пам'ять

            // Підігнати розмір зображення під розмір tableView
            pixmap = pixmap.scaled(ui->tableView->width()-48, ui->tableView->height()-29,
                                   Qt::KeepAspectRatio, Qt::SmoothTransformation);

            imageItem->setData(QVariant(pixmap), Qt::DecorationRole);
            model->appendRow({ percentItem, imageItem });
        }

        ui->tableView->setModel(model);
        ui->tableView->verticalHeader()->setDefaultSectionSize(pixmap.height());
        ui->tableView->verticalHeader()->setVisible(false);
        ui->tableView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

        ui->tableView->setColumnWidth(0,45);
        ui->tableView->setColumnWidth(1,pixmap.width());

        qDebug()<<ui->tableView->width();
    }

    else{
        this->addRowToTable();
    }
}

void MainWindow::addRowToTable()
{
    QSqlQuery query("SELECT percent, image FROM images ORDER BY id DESC LIMIT 1");
    if (query.next()) {
        QString percent = query.value(0).toString();
        data = query.value(1).toByteArray();

        pixmap.loadFromData(data);

        // створюємо новий рядок з отриманими даними та додаємо його до моделі таблиці
        QStandardItem *percentItem = new QStandardItem(percent);
        QStandardItem *imageItem = new QStandardItem;

        pixmap = pixmap.scaled(ui->tableView->width()-ui->tableView->columnWidth(0), ui->tableView->height(),
                               Qt::KeepAspectRatio, Qt::SmoothTransformation);

        imageItem->setData(QVariant(pixmap), Qt::DecorationRole);
        model->insertRow(0,{ percentItem, imageItem });
    }
}

void MainWindow::on_closebutton_clicked()
{
    QApplication::exit();
}

