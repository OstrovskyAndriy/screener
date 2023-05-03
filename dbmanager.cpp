#include "dbmanager.h"

DBManager* DBManager::instance = nullptr;

DBManager::DBManager(){
    this->connectToDataBase();
}

DBManager* DBManager::getInstance()
{
    if(instance == nullptr){
        instance = new DBManager();
    }
    return instance;
}


void DBManager::connectToDataBase()
{

    if(QFile("screenshotDB.sqlite").exists()){
        this->openDataBase();
    } else {
        this->restoreDataBase();
    }
}

QSqlDatabase DBManager::getDB()
{
    return db;
}


bool DBManager::restoreDataBase()
{
    if(this->openDataBase()){
        if(!this->createTables()){
            return false;
        } else {
            return true;
        }
    } else {
        qDebug() << "Не вдалось відновити базу даних";
        return false;
    }
}


bool DBManager::openDataBase()
{
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("screenshotDB.sqlite");
    if(db.open()){
        return true;
    } else
        return false;
}


void DBManager::closeDataBase()
{
    db.close();
}

bool DBManager::createTables()
{
    QSqlQuery query;

    if(!query.exec("CREATE TABLE images ("
                   "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                   "percent INTEGER,"
                   "image BLOB,"
                   "hash TEXT);"))
    {
        qDebug()<<"users error creating";
        qDebug() << query.lastError().text();
        return false;
    }

        return true;
}

bool DBManager::insert(int percent,const QByteArray& screen, const QString &hash) const
{
    QSqlQuery query;

    query.prepare("INSERT INTO images("
                       "percent, "
                       "image,"
                       "hash)"
                       "VALUES(?,?,?);");

    query.addBindValue(percent);
    query.addBindValue(screen);
    query.addBindValue(hash);

    if(!query.exec()){
        qDebug() << query.lastError().text();
        qDebug() << query.lastQuery();

        return false;
    } else
        return true;
}


QString DBManager::getAudioTableName()
{
    return "audioList";
}

QString DBManager::getUsersTableName()
{
    return "users";
}

