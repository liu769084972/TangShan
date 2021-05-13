#ifndef STO_H
#define STO_H

#include <QObject>
#include <QSqlDatabase>
#include <QNetworkAccessManager>

#include "define.h"
#include "Work/Arrived/arrivedsto.h"
#include "Work/Pack/packsto.h"
#include "Work/Dispatch/dispatchsto.h"
#include "log.h"

class Express;

class STO : public QObject
{
    Q_OBJECT
public:
    explicit STO(bool _comeOrGo, Express *_express, QObject *parent = nullptr);

    void getBoxIdDB(QString expressId);
    void getBoxIdAsynchronous(QString expressId); //异步
    short getBoxId(QString expressId); //同步

private slots:
    void connectDatabase();
    void getBoxIdCome(QString expressId); //进港
    void getBoxIdGo(QString expressId); //出港
    void getBoxIdComeOrGo(QNetworkReply *reply); //同步
    void getBoxIdComeOrGoAsynchronous(); //异步
signals:
    void arrivedSignal(QString expressId, QString orgCode, QString userCode,
                       QString effectiveType, int boxId, QString time, QString updateFlag,
                       QString updateFlagTime, QString company, QString pdaCode);
    void appendRunLog(QString message);
    void appendDetailLog(QString message);
    void appendAlarm(QString content, QString level, QString component);
    void sig_timeOutPlcLog(QString mes);
    void sig_InterceptorLog(QString mes);

public slots:

private:
    QSqlDatabase db;

    //接口方式（http）
    QNetworkAccessManager *manager;

    bool comeOrGo;
    Express *express;

    //到件
    ArrivedSTO *arrivedSto;
    QThread *threadArrivd;
    //集包
    PackSTO *packSto;
    QThread *threadPack;
    //发件
    DispatchSTO *dispatchSto;

    //日志
    Log *log;

    qint64 delay = 500;
};

#endif // STO_H
