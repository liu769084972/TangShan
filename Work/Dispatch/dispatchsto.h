#ifndef DISPATCHSTO_H
#define DISPATCHSTO_H

#include <QNetworkAccessManager>
#include <QSqlDatabase>
#include <QTimer>
#include <QJsonObject>

#include "define.h"
#include "log.h"

class DispatchSTO : public QObject
{
    Q_OBJECT
public:
    explicit DispatchSTO(QObject *parent = nullptr);

    QByteArray createContent(QString orgCode, QString userCode,
        QString waybillNo, QString nextOrgCode, QString opTime, QString pdaCode);

signals:
    void appendAlarm(QString, QString, QString);
    void appendDispatchLog(QString);
    void appendDispatchDataToGUI(QStringList expressId, QString company, QString status);

public slots:
    void init();
    void timerEventDispatch();
    void startOrStopTimer(bool isStart);

private:
    QString uuId();
    void createDBConnection();
    void setSql();
    void dispatch(QString expressId, QString orgCode, QString userCode,
                  QString nextOrgCode, int boxId, QString time, QString updateFlag,
                  QString updateTimeFlag, QString company, QString pdaCode);
    QJsonObject createJsonObject(QString waybillNo,
                                       QString time,
                                       QString effectiveType);
    void httpReqeust(QString content);
    QString calculateDigest(QString content);
    void httpRead();

private:
    QNetworkAccessManager *manager;
    QSqlDatabase db;
    QString sql;

    QTimer *timer;
    Log *log;
};

#endif // DISPATCHSTO_H
