#include "functionsforserver.h"
#include <QTcpSocket>
#include <QTime>

user::user(QTcpSocket *mTcpSocket){
    connect(mTcpSocket, &QTcpSocket::readyRead,
            this, &user::slotServerRead);
    connect(mTcpSocket, &QTcpSocket::disconnected,
            this, &user::slotClientDisconnected);
    desc = QString::number(QRandomGenerator::system()->generate64());
    this->mTcpSocket = mTcpSocket;
}

user::~user(){
    delete[] mTcpSocket;
}

void user::slotServerRead(){
    QString res = "";
    while(mTcpSocket->bytesAvailable()>0)
    {
        QByteArray array =mTcpSocket->readAll();
        res.append(array);
    }
    qDebug()<<res;
    mTcpSocket->write(parsing(res) + "\n");
}

void user::slotClientDisconnected(){
    qDebug() << "Closing connection";
    mTcpSocket->close();
}

QByteArray user::parsing(QString data_from_client)
 {
     /* data_from_client = NameOfFunc&Param1&Param2&Param3
      * Цель парсинга:
      * return NameOfFunc(Param1,Param2,Param3)
      */
     QStringList data_from_client_list = data_from_client.split(QLatin1Char('\\'));
     // data_from_client_list  = "NameOfFunc" "Param1" "Param2" Param3"
     QString nameOfFunc = data_from_client_list.front();
     data_from_client_list.pop_front();
     if (nameOfFunc == "auth")
         return auth(data_from_client_list.at(0),data_from_client_list.at(1));
     if (nameOfFunc == "start")
         return start(data_from_client_list.at(0));
     if (nameOfFunc == "finish")
        return finish(data_from_client_list.at(0));
     else
         return "error";
 }


 QByteArray user::auth(QString log, QString pass)
{
    QString queryStr = QString("SELECT desc FROM users WHERE login='%1' AND password='%2'").arg(log).arg(pass);
    QSqlQuery query = DataBase::sendQuery(queryStr);
    if (query.isActive() && query.next()) {
        int status = query.value(0).toInt();
        if (status == 0) {
            QString updateStr = QString("UPDATE users SET desc='%1' WHERE login='%2'").arg(desc, log);
            DataBase::sendQuery(updateStr);
            return "auth_ok";
        } else {
            return "auth_err";
        }
    } else {
        return "auth_false";
    }
}

QByteArray user::start(QString time)
{
    QString queryStr = QString("SELECT time, stat FROM users WHERE desc='%2'").arg(desc);
    QSqlQuery query = DataBase::sendQuery(queryStr);
    if (query.isActive() && query.next()) {
        int dbttime = query.value(0).toInt();
        int usertime = hhmmssToSeconds(time);
        if (dbttime == 0) {
            QString updateStr = QString("UPDATE users SET time='%1' WHERE desc='%2'").arg(QString::number(usertime), desc);
            DataBase::sendQuery(updateStr);
            return "start_ok";
        } else {
            int stattime = query.value(1).toInt();
            stattime = stattime + usertime - dbttime - 3600;
            QString updateStr = QString("UPDATE users SET time='%1' WHERE desc='%2'").arg(QString::number(usertime), desc);
            DataBase::sendQuery(updateStr);
            updateStr = QString("UPDATE users SET stat='%1' WHERE desc='%2'").arg(QString::number(stattime), desc);
            DataBase::sendQuery(updateStr);
            return "start_reok";
        }
    } else {
        return "start_err";
    }
}

QByteArray user::finish(QString time)
{
    QString queryStr = QString("SELECT time, stat FROM users WHERE desc='%2'").arg(desc);
    QSqlQuery query = DataBase::sendQuery(queryStr);
    if (query.isActive() && query.next()) {
        int dbttime = query.value(0).toInt();
        int usertime = hhmmssToSeconds(time);
        if (dbttime == 0) {
            return "finish_reerr";
        } else {
            int stattime = query.value(1).toInt();
            stattime = stattime + usertime - dbttime - 3600;
            QString updateStr = QString("UPDATE users SET time='%1' WHERE desc='%2'").arg(QString::number(usertime), desc);
            DataBase::sendQuery(updateStr);
            updateStr = QString("UPDATE users SET stat='%1' WHERE desc='%2'").arg(QString::number(stattime), desc);
            DataBase::sendQuery(updateStr);
            return "finish_ok";
        }
    } else {
        return "finish_err";
    }
}

int user::hhmmssToSeconds(QString timeStr){
    QTime time = QTime::fromString(timeStr, "hh_mm_ss");
    return time.hour() * 3600 + time.minute() * 60 + time.second();
}
