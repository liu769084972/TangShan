#ifndef ARRIVEDYT_H
#define ARRIVEDYT_H

#include <QObject>
#include <QSqlDatabase>
#include <QTcpSocket>
#include <QTimer>

#include "log.h"

class ArrivedYTO : public QObject
{
    Q_OBJECT
public:
    explicit ArrivedYTO(QObject *parent = nullptr);

signals:
    void appendAlarm(QString);
    void appendDetailLog(QString);
    void appendArrivedDataToGUI(QString expressId, QString company, QString status);

public slots:
    void init();
    void arrived(QString expressId, int boxId, QString time);
    void socektSuccess();
    void socketError();
    void readSocket();
    void setContainerNo(QString _containerNo); //车签，每天一输入
    void getData();
    void startOrStopTimer(bool isStart);

private:
    QString createDbConnection(); //创建数据库连接
    void createSocketConnection();
    void autoConnection();
    void getSignature();
    QByteArray createOpRecord(QJsonObject opRecord);
    QByteArray createMessage(QByteArray opRecord);

    QJsonObject createJsonObject(QString waybillNo, QString createTime, QString nextOrgCode, int boxId);
    void sendMessage(QByteArray message);

private:
    QSqlDatabase db;
    QString sql;
    QString signature;
    QString containerNo; //车签，每天一输入

    QTcpSocket *socket;

    Log *log;

    QTimer *timer;
};

#endif // ARRIVEDYT_H
