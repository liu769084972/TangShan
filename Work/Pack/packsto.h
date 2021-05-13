#ifndef PACKSTO_H
#define PACKSTO_H

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QTcpServer>
#include <QThread>

#include "plc.h"
#include "log.h"

class PackSTO : public QObject
{
    Q_OBJECT
public:
    explicit PackSTO(QObject *parent = nullptr);

private:
    QString createJson(QSqlQuery query, QString containerNo, QString boxId,
                       QString effectiveType, QString userCode);
    QString calculateDigest(QString content);
    QString createUuid();
    bool createDbConnection();
    const QJsonObject createJsonObject(QString waybillNo, QString time,
                                       QString nextOrgCode, QString containerNo,
                                       QString effectiveType);
    QSqlQuery getData(int boxNum, qint64 start, qint64 end);
    void createServer();

signals:
    void appendAlarm(QString);
    void appendDetailLog(QString);
    void appendPackDataToGUI(QStringList expressIdList, QString containerNo,
                             QString boxId, QString status);

public slots:
    void init();
    void httpRequest(QSqlQuery query, QString containerNo, QString boxId,
                     QString effectiveType, QString userCode);
    void httpReply();
    void readServer();
    void work(QString boxNum, QString containerNo, QTcpSocket *socket, QByteArray CB);
    void readSocket();
    void setValid(bool _valid); //设置是否启用集包，由界面开始停止按钮设置

private:
    QNetworkAccessManager *manager;
    QSqlDatabase db;

    PLC *plc;

    QTcpServer *server; //把枪
    qint64 endTime[400]; //记录集包成功的截至时间，当扫描为空的时候使用，作为下次时间的开始时间，开机用当前时间初始化

    Log *log;

    bool isValid = false; //是否集包
};

#endif // PACKSTO_H
