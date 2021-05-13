#ifndef PLC_H
#define PLC_H

#define MAXSIZE 400  //最大格口数

#include <QObject>

#include "C:\\TwinCAT\\AdsApi\\TcAdsDll\\Include\\TcAdsDef.h"
#include "C:\\TwinCAT\\AdsApi\\TcAdsDll\\Include\\TcAdsAPI.h"
#include "log.h"
#include "define.h"

class PLC : public QObject
{
    Q_OBJECT
public:
    explicit PLC(QObject *parent = nullptr);
    ~PLC();

    long initAddress(); //初始化PLC地址
    long initHandle(); //初始化PLC句柄

    //格口写入下位机PLC
    long writeBoxId(QString expressId, short carrierId, short boxId, int index);
    long writeTableName(QString tableName); //将表名写给PLC

    const int* plcGetStartTime();
    const int* plcGetEndTime();
    pbAlarmArray plcGetAlarmArray();
    pExpressIDArray plcGetExpressIdArray();
    long clearAlarmAndExpressId(int railway, int boxId);
    long testAlarmAndExpressId();

signals:
    void appendDetailLog(QString message);


public slots:


private:
    AmsAddr Addr;
    PAmsAddr pAddr;

    //句柄，将被映射为PLC变量的地址
    unsigned long tableNameHandle[8];
    unsigned long carrierIdHandle[8];
    unsigned long boxIdHandle[8];
    unsigned long expressIdHandle[8];

    unsigned long startTimeHandle;
    unsigned long endTimeHandle;

    unsigned long StrackInHandle[4];
    unsigned long bAlarmArrayHandle;
    unsigned long bAlarmHandle[railWayNum][boxIdNum];
    unsigned long ExpressIDArrayHandle;
    unsigned long ExpressIDHandle[railWayNum][boxIdNum];

    int startTime[1][MAXSIZE];
    int endTime[1][MAXSIZE];
    bool alarmArray[railWayNum][boxIdNum];
    char ExpressIdArray[railWayNum][boxIdNum][31];

    //char EmptyExpressId[plcExpressIdLen] = {0};
    //bool EmptyAlarm = false;
    //日志
    Log *log;
};

#endif // PLC_H
