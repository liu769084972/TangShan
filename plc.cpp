#include "plc.h"
#include "Configure/configure.h"
#include <QThread>
PLC::PLC(QObject *parent) : QObject(parent)
{
    log = Log::instance();
    connect(this, &PLC::appendDetailLog, log, &Log::appendDetailLog);
}

PLC::~PLC()
{
    for (int i = 0; i < 8; i++) {
        AdsSyncWriteReq(
                    pAddr, ADSIGRP_SYM_RELEASEHND, carrierIdHandle[i], 0x0, nullptr);
        AdsSyncWriteReq(
                    pAddr, ADSIGRP_SYM_RELEASEHND, boxIdHandle[i], 0x0, nullptr);
        AdsSyncWriteReq(
                    pAddr, ADSIGRP_SYM_RELEASEHND, expressIdHandle[i], 0x0, nullptr);
        AdsSyncWriteReq(
                    pAddr, ADSIGRP_SYM_RELEASEHND, tableNameHandle[i], 0x0, nullptr);
    }

    AdsSyncWriteReq(
              pAddr, ADSIGRP_SYM_RELEASEHND, startTimeHandle, 0x0, nullptr);
    AdsSyncWriteReq(
                pAddr, ADSIGRP_SYM_RELEASEHND, endTimeHandle, 0x0, nullptr);

    AdsPortClose();
}

long PLC::writeBoxId(QString expressId, short carrierId, short boxId, int index)
{
//    char *expressIdChar = expressId.toLatin1().data();
    emit appendDetailLog(QString("快递号%1，小车号：%2，格口：%3，索引：%4")
         .arg(expressId).arg(carrierId).arg(boxId).arg(index));
    QByteArray ba = expressId.toLocal8Bit();
    char *expressIdChar = ba.data();
    qDebug() << QString("写入PLC：快递号：%1，小车号：%2，格口号%3，plc位置：%4\r\n")
                .arg(expressId).arg(carrierId).arg(boxId).arg(index);

    long lErr1 = AdsSyncWriteReq(
                pAddr, ADSIGRP_SYM_VALBYHND,
                boxIdHandle[index], sizeof(boxId), &boxId);
    qDebug() << "写入格口:" << boxId << " 返回"<< lErr1 << "\r\n";
    long lErr2 = AdsSyncWriteReq(
                pAddr, ADSIGRP_SYM_VALBYHND,
                expressIdHandle[index], qstrlen(expressIdChar) + 1UL, expressIdChar);
    qDebug() << "写入快递号:" << expressIdChar << " 返回"<< lErr2 << "\r\n";
    //直线机小车号必须最后写
    long lErr3 = AdsSyncWriteReq(
                pAddr, ADSIGRP_SYM_VALBYHND,
                carrierIdHandle[index], sizeof(carrierId), &carrierId);
    qDebug() << "写入小车号:" << carrierId << " 返回"<< lErr3 << "\r\n";

//    unsigned long sleepTime = Configure::instance()->StrackInWait();
//    QThread::msleep(sleepTime);
//    long lErr0 = AdsSyncWriteReq(
//                pAddr, ADSIGRP_SYM_VALBYHND,
//                StrackInHandle[index], sizeof(true), &strackIn);

    return lErr1 + lErr2 + lErr3;
}

long PLC::writeTableName(QString tableName)
{
    QByteArray ba = tableName.toLocal8Bit();
    char *tableChar = ba.data();
    unsigned long length = qstrlen(tableChar) + 1UL;

    long lErr = 0;
    for (int i=0;i<8;i++) {
        lErr += AdsSyncWriteReq(
                    pAddr, ADSIGRP_SYM_VALBYHND,
                    tableNameHandle[i], length, tableChar);
    }

    return lErr;
}

const int* PLC::plcGetStartTime()
{
    AdsSyncReadReq(pAddr,
        ADSIGRP_SYM_VALBYHND,
        startTimeHandle,
        sizeof(startTime),
        startTime);

    return startTime[0];
}

const int* PLC::plcGetEndTime()
{
    AdsSyncReadReq(pAddr,
        ADSIGRP_SYM_VALBYHND,
        endTimeHandle,
        sizeof(endTime),
        endTime);

    return endTime[0];
}

pbAlarmArray PLC::plcGetAlarmArray()
{
    AdsSyncReadReq(pAddr,
        ADSIGRP_SYM_VALBYHND,
        bAlarmArrayHandle,
        sizeof(alarmArray),
        alarmArray);
    return alarmArray;
}

pExpressIDArray PLC::plcGetExpressIdArray()
{
    AdsSyncReadReq(pAddr,
        ADSIGRP_SYM_VALBYHND,
        ExpressIDArrayHandle,
        sizeof(ExpressIdArray),
        ExpressIdArray);
    return ExpressIdArray;
}

bool static EmptyAlarm = false;
char static EmptyExpressId[plcExpressIdLen] = {0};
long PLC::clearAlarmAndExpressId(int railway, int boxId)
{    
    long lErr1 = AdsSyncWriteReq(
                pAddr, ADSIGRP_SYM_VALBYHND,
                bAlarmHandle[railway][boxId], sizeof (EmptyAlarm), &EmptyAlarm);

    long lErr2 = AdsSyncWriteReq(
                pAddr, ADSIGRP_SYM_VALBYHND,
                ExpressIDHandle[railway][boxId], sizeof(EmptyExpressId), EmptyExpressId);
    return lErr1 + lErr2;
}

long PLC::testAlarmAndExpressId()
{
    bool value1 = false;
    long lErr1 = AdsSyncReadReq(
        pAddr,
        ADSIGRP_SYM_VALBYHND,
        bAlarmHandle[0][0],
        sizeof(value1),
        &value1);
    char EmptyExpressId[plcExpressIdLen] = {0};
    long lErr2 = AdsSyncReadReq(
        pAddr,
        ADSIGRP_SYM_VALBYHND,
        ExpressIDHandle[0][0],
        sizeof(EmptyExpressId),
        EmptyExpressId);
    qDebug() << value1 << EmptyExpressId << "\n";


    return lErr1 + lErr2;
}

long PLC::initAddress()
{
    pAddr = &Addr;
    long lPort = AdsPortOpen();
    long lErr = AdsGetLocalAddress(pAddr);

    AmsNetId id;
    unsigned char *pNetId = Configure::instance()->netId();
    for (int i = 0; i < 6; ++i) {
        id.b[i] = pNetId[i];
    }

    pAddr->netId = id;
    pAddr->port = 801;

    return lErr;
}

long PLC::initHandle()
{
    char carrierIdPLCVarName[8][25] =
    {
        ".gbo_CarrierNumber[1]",
        ".gbo_CarrierNumber[2]",
        ".gbo_CarrierNumber[3]",
        ".gbo_CarrierNumber[4]",
        ".gbo_CarrierNumber[5]",
        ".gbo_CarrierNumber[6]",
        ".gbo_CarrierNumber[7]",
        ".gbo_CarrierNumber[8]",
    };

    char boxIdPLCVarName[8][20] =
    {
        ".gbo_BoxNumber[1]",
        ".gbo_BoxNumber[2]",
        ".gbo_BoxNumber[3]",
        ".gbo_BoxNumber[4]",
        ".gbo_BoxNumber[5]",
        ".gbo_BoxNumber[6]",
        ".gbo_BoxNumber[7]",
        ".gbo_BoxNumber[8]",
    };

    char expressIdPLCVarName[8][20] =
    {
        ".gbo_Identifer[1]",
        ".gbo_Identifer[2]",
        ".gbo_Identifer[3]",
        ".gbo_Identifer[4]",
        ".gbo_Identifer[5]",
        ".gbo_Identifer[6]",
        ".gbo_Identifer[7]",
        ".gbo_Identifer[8]",
    };

    char tableNamePLCVarName[8][20] =
    {
        ".gbo_DbTable[1]",
        ".gbo_DbTable[2]",
        ".gbo_DbTable[3]",
        ".gbo_DbTable[4]",
        ".gbo_DbTable[5]",
        ".gbo_DbTable[6]",
        ".gbo_DbTable[7]",
        ".gbo_DbTable[8]",
    };

    char startTimeVarName[] = ".gbo_StartTime";
    char endTimeVarName[] = ".gbo_EndTime";

    char StrackInVarName[4][20] =
    {
        ".StrackIn1",
        ".StrackIn2",
        ".StrackIn3",
        ".StrackIn4"
    };

    long lErr = 0;
    for(int i = 0; i < 8; ++i) {
        lErr = AdsSyncReadWriteReq(
                    pAddr, ADSIGRP_SYM_HNDBYNAME, 0x0,
                    sizeof(carrierIdHandle[i]), &carrierIdHandle[i],
                    sizeof(carrierIdPLCVarName[i]), carrierIdPLCVarName[i]);
        qDebug() << carrierIdPLCVarName[i] << i << lErr;
        lErr = AdsSyncReadWriteReq(
                    pAddr, ADSIGRP_SYM_HNDBYNAME, 0x0,
                    sizeof(boxIdHandle[i]), &boxIdHandle[i],
                    sizeof(boxIdPLCVarName[i]), boxIdPLCVarName[i]);
        qDebug() << boxIdPLCVarName[i] << i << lErr;
        lErr = AdsSyncReadWriteReq(
                    pAddr, ADSIGRP_SYM_HNDBYNAME, 0x0,
                    sizeof(expressIdHandle[i]), &expressIdHandle[i],
                    sizeof(expressIdPLCVarName[i]), expressIdPLCVarName[i]);
        qDebug() << expressIdPLCVarName[i] << i << lErr;
        lErr = AdsSyncReadWriteReq(
                    pAddr, ADSIGRP_SYM_HNDBYNAME, 0x0,
                    sizeof(tableNameHandle[i]), &tableNameHandle[i],
                    sizeof(tableNamePLCVarName[i]), tableNamePLCVarName[i]);
        qDebug() << tableNamePLCVarName[i] << i << lErr;
        if (i < 4) {
            lErr = AdsSyncReadWriteReq(
                        pAddr, ADSIGRP_SYM_HNDBYNAME, 0x0,
                        sizeof(StrackInHandle[i]), &StrackInHandle[i],
                        sizeof(StrackInVarName[i]), StrackInVarName[i]);
            qDebug() << StrackInVarName[i] << i << lErr;
        }
    }

    lErr = AdsSyncReadWriteReq(
              pAddr, ADSIGRP_SYM_HNDBYNAME, //ADS-indexGroup-handle-by-name
              0x0, sizeof(startTimeHandle), &startTimeHandle,
              sizeof(startTimeVarName), startTimeVarName);
    qDebug() << startTimeVarName << lErr;

    lErr = AdsSyncReadWriteReq(
              pAddr, ADSIGRP_SYM_HNDBYNAME, //ADS-indexGroup-handle-by-name
              0x0, sizeof(endTimeHandle), &endTimeHandle,
              sizeof(endTimeVarName), endTimeVarName);
    qDebug() << endTimeVarName << lErr;

    //超出区间报警bool变量-数组
    char bAlarmVarName[10] = ".bAlarm";
    lErr = AdsSyncReadWriteReq(
              pAddr, ADSIGRP_SYM_HNDBYNAME, //ADS-indexGroup-handle-by-name
              0x0, sizeof(bAlarmArrayHandle), &bAlarmArrayHandle,
              sizeof(bAlarmVarName), bAlarmVarName);
    //超出区间报警bool变量-单个值
    for (int i=1;i<=railWayNum;++i) {
        for (int j=0;j<boxIdNum;++j) {
            char varname[20] = {0};
            sprintf_s(varname, 20, ".bAlarm[%d,%d]", i, j);
            unsigned long len = sizeof (varname);

            lErr = AdsSyncReadWriteReq(
                      pAddr, ADSIGRP_SYM_HNDBYNAME, //ADS-indexGroup-handle-by-name
                      0x0, sizeof(bAlarmHandle[i-1][j]), &bAlarmHandle[i-1][j],
                      len, varname);
            qDebug() << varname << len << i << j << lErr;
        }
    }
//                char varname1[15] = ".bAlarm[1,0]";
//                unsigned long len1 = sizeof (varname1);
//                long lErr1 = AdsSyncReadWriteReq(
//                          pAddr, ADSIGRP_SYM_HNDBYNAME, //ADS-indexGroup-handle-by-name
//                          0x0, sizeof(bAlarmHandle[0][0]), &bAlarmHandle[0][0],
//                          len1, varname1);
    //超出区间报警快递号-数组
    char ExpressIDVarName[15] = ".ExpressID";
    lErr = AdsSyncReadWriteReq(
              pAddr, ADSIGRP_SYM_HNDBYNAME, //ADS-indexGroup-handle-by-name
              0x0, sizeof(ExpressIDArrayHandle), &ExpressIDArrayHandle,
              sizeof(ExpressIDVarName), ExpressIDVarName);

    //超出区间报警快递号-单个值
    for (int i=1;i<=railWayNum;++i) {
        for (int j=0;j<boxIdNum;++j) {
            char varname[20] = {0};
            sprintf_s(varname, 20, ".ExpressID[%d,%d]", i, j);
            unsigned long len = sizeof (varname);

            lErr = AdsSyncReadWriteReq(
                      pAddr, ADSIGRP_SYM_HNDBYNAME, //ADS-indexGroup-handle-by-name
                      0x0, sizeof(ExpressIDHandle[i-1][j]), &ExpressIDHandle[i-1][j],
                      sizeof(varname), varname);
            qDebug() << varname << len << i << j << lErr;
        }
    }
//                char varname2[20] = ".ExpressID[1,0]";
//                unsigned long len2 = sizeof(varname2);
//                long lErr2 = AdsSyncReadWriteReq(
//                          pAddr, ADSIGRP_SYM_HNDBYNAME, //ADS-indexGroup-handle-by-name
//                          0x0, sizeof(ExpressIDHandle[0][0]), &ExpressIDHandle[0][0],
//                          len2, varname2);

    return lErr;
}
