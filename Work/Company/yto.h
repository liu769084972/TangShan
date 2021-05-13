#ifndef YTO_H
#define YTO_H

#include <QObject>
#include <QTcpSocket>
#include <QThread>

#include "log.h"
#include "Work/Arrived/arrivedyto.h"
#include "Work/Pack/packyto.h"

class Express;

class YTO : public QObject
{
    Q_OBJECT
public:
    explicit YTO(bool _comeOrGo, Express *_express, QObject *parent = nullptr);

    void getBoxId(QString expressId);

private:
    void createYTOServerConnection(); //连接圆通服务器
    void getSignature(); //获取签名
    void getBoxIdCome(QString code1, QString code2, QString code3, QString expressId, QString Dest_Org); //进港
    void getBoxIdGo(QString code1, QString code2, QString code3, QString expressId, QString Dest_Org); //出港

    QTcpSocket *tcpSocket = nullptr;
    QString signature;

signals:
    void arrivedSignal(QString expressId, int boxId, QString time);
    void appendRunLog(QString message);
    void appendDetailLog(QString message);
    void appendAlarm(QString content, QString level, QString component);

public slots:
    void readTcpSocket();
    void tcpSocketError();
    void tcpSocektSuccess();
    void autoConnectServer();

private:
    Express *express;

    bool comeOrGo;

    //日志
    Log *log;

    ArrivedYTO *arrivedYto;
    QThread *threadArrivd;
    PackYTO *packYto;
    QThread *threadPack;
};

#endif // YTO_H
