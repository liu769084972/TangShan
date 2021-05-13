#ifndef ARRIVEDSTO_H
#define ARRIVEDSTO_H

#include <QNetworkAccessManager>
#include <QSqlDatabase>
#include <QTimer>
#include <QJsonObject>

#include "define.h"
#include "log.h"

class ArrivedSTO : public QObject
{
    Q_OBJECT
public:
    explicit ArrivedSTO(QObject *parent = nullptr);

public slots:
    void init();
    void arrived(QString expressId, QString orgCode, QString userCode,  QString effectiveType,
                 int boxId, QString time, QString updateFlag, QString updateTime, QString company, QString pdaCode); //到件
    void setIsDb(bool value);
    void startOrStopTimer(bool isStart);

signals:
    void appendAlarm(QString, QString, QString);
    void appendArriveLog(QString);
    void appendArrivedDataToGUI(QString expressId, QString company, QString status);

private:
    void createDBConnection();
    void timerEventArrived();
    QJsonArray getArray(QSqlQuery query);
    QHttpMultiPart *createMultiPart(QString content);
    void httpRequestArrived(QHttpMultiPart *multiPart);
    QString createJson(QJsonArray records, QString orgCode, QString userCode, QString pdaCode);
    const QJsonObject createJsonObject(QString waybillNo,
                                       QString time,
                                       QString effectiveType);
    const QJsonObject createJsonObject(QString waybillNo,
                                       QString time,
                                       QString effectiveType,
                                       QString goodsType,
                                       QString empCodeType);
    QString calculateDigest(QString content);
    inline QString createUuid();
    void httpReplyArrived();
    void setSql();

private:
    QNetworkAccessManager *manager;
    QSqlDatabase db;
    QString sql;

    QTimer *timerArrived;

    Log *log;

    bool isDb = false; //数据来源是否数据库
};

#endif // ARRIVEDSTO_H
