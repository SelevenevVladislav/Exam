#ifndef DATABASE_H
#define DATABASE_H

#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>


class DataBase;

class DataBaseDestroyer
{
    private:
        DataBase * dataBasePointer;
    public:
        ~DataBaseDestroyer() {
            delete dataBasePointer;
        }
        void initialize(DataBase * p){dataBasePointer = p;}
};

class DataBase
{
    private:
        static DataBase * p_instance;
        static DataBaseDestroyer destroyer;
        static QSqlDatabase db;
    protected:
        DataBase(){
            p_instance->db = QSqlDatabase::addDatabase("QSQLITE");
            p_instance->db.setDatabaseName("Test.db");

            if(!p_instance->db.open())
                qDebug()<<p_instance->db.lastError().text();


        }
        DataBase(const DataBase& ) = delete;
        DataBase& operator = (DataBase &) = delete;
        ~DataBase() {db.close();}

        friend class DataBaseDestroyer;
    public:
        static DataBase* getInstance(){
            if (!p_instance)
            {
                p_instance = new DataBase();

                destroyer.initialize(p_instance);
            }
            return p_instance;
        }
        ///здесь пишем функцию сделать запрос к БД
        static QSqlQuery sendQuery(QString str){
            QSqlQuery query(db);
            query.prepare(str);
            query.exec();
            return query;
        }
};
/*
DataBase * DataBase::p_instance;
DataBaseDestroyer DataBase::destroyer;
QSqlDatabase DataBase::db;
*/
#endif // DATABASE_H
