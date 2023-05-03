#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    db=DBManager::getInstance();
    //db->connectToDataBase();
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_startButton_clicked()
{
    QScreen *screen = QGuiApplication::primaryScreen();

    // Take a screenshot of the screen and save it to a QPixmap object
    QPixmap screenshot = screen->grabWindow(0);

    // Save the screenshot to a file
    screenshot.save("./screenshot.png");

    QString filePath = "./screenshot.png";
    QFile file(filePath);

    if (file.open(QIODevice::ReadOnly)) {
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


        } else {
            qWarning() << "Failed to add data to hash.";
        }
        file.close();
    } else {
        qWarning() << "Failed to open file" << filePath;
    }
}


void MainWindow::on_pushButton_clicked()
{
    QSqlQuery q(db->getDB());

    QString query = "SELECT image FROM images WHERE id = :id";
    q.prepare(query);
    q.bindValue(":id", 1);

    if (q.exec() && q.first()) {
        // Отримати масив байтів з бази даних
        QByteArray data = q.value("image").toByteArray();

        // Створити QPixmap з масиву байтів
        QPixmap pixmap;
        pixmap.loadFromData(data);
        ui->label->setPixmap(pixmap);
        ui->label->setScaledContents(true);

        // Використовуйте pixmap у вашому коді
    }
}

