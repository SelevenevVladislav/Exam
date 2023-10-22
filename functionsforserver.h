#ifndef FUNCTIONSFORSERVER_H
#define FUNCTIONSFORSERVER_H
#include <QObject>
#include <QString>
#include <QTcpSocket>
#include <QRandomGenerator>
#include "dataBase.h"
#include <QStringList>

class user: public QObject {
public:
    QString desc;
    QTcpSocket *mTcpSocket;
    /**
 * \brief parsing - функция, которая по введенному названию
 * и параметрам функции возращает результат.
 * \param [in] data_from_client - строка, которая приходит от клиента
 * в формате....
 * \return Возвращает результат работы функции, которая была подана на вход.
 *
 * \example указать_файл.txt
 */
    user(QTcpSocket *mTcpSocket);

    ~user();

    QByteArray parsing (QString data_from_client);

    QByteArray auth(QString log, QString pass);

    QByteArray start(QString time);

    QByteArray finish(QString time);

    int hhmmssToSeconds(QString timeStr);

public slots:
    void slotClientDisconnected();
    void slotServerRead();
};


#endif
// FUNCTIONSFORSERVER_H
