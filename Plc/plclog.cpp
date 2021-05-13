#include "plclog.h"
#include "define.h"
#include "log.h"
#include "Configure/configure.h"
#include "mainwindow.h"
#include <QDateTime>
PlcLog::PlcLog(MainWindow *m, PLC *plc, QObject *parent) :
    QObject(parent), plc(plc), m(m)
{

}

void PlcLog::timerEvent(QTimerEvent */*e*/)
{
    //读取数组
    pbAlarmArray bAlarm = plc->plcGetAlarmArray();
    for (int i=0;i<railWayNum;++i) {
        for (int j=0;j<boxIdNum;++j) {
            bool value = bAlarm[i][j];
            if (value) {
                pExpressIDArray expressIdArray = plc->plcGetExpressIdArray();
                QString expressId = QString(expressIdArray[i][j]);
                QString dt = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss  ");
                QString mes = QString("%1,快递号:%2,轨道:%3,格口:%4").arg(dt).arg(expressId).arg(i+1).arg(j+1);
                emit sig_rangeOutPlcLog(mes);
                //清除数据
                plc->clearAlarmAndExpressId(i, j);
            }
        }
    }
}

void PlcLog::init()
{
    connect(this, &PlcLog::sig_rangeOutPlcLog, Log::instance(), &Log::slot_rangeOutPlcLog);
    int interval = Configure::instance()->intervalPlcAlarm();
    interval > 0 ? startTimer(interval) : startTimer(200);
}
