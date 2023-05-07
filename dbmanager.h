#ifndef DBMANAGER_H
#define DBMANAGER_H


#include <QSqlDatabase> // бібліотека бази даних
#include <QDebug> //бібліотека для відладки програми
#include <QSqlQuery> // бібліотека для запиту до баз даних
#include <QtSql>

#include <QScreen>
#include <QPixmap>

class DBManager
{
    QSqlDatabase db;
    QSqlQuery *query;

    static DBManager* instance;

    DBManager();

    bool openDataBase();
    bool restoreDataBase();
    void closeDataBase();
    bool createTables();

public:
    static DBManager* getInstance();

    void connectToDataBase();
    QSqlDatabase getDB();
    bool insert(const QString& percent,const QByteArray& screen,const QString &hash) const;

};

#endif // DBMANAGER_H
