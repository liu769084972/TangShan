#ifndef EXPRESS_H
#define EXPRESS_H

#include <QObject>
#include <QBitArray>
#include <QQueue>

#include "../Company/sto.h"
#include "../Company/zto.h"
#include "../Company/yto.h"
#include "../Company/best.h"
#include "../Company/tt.h"
#include "../Company/yunda.h"
#include "../Company/youzheng.h"
#include "define.h"
#include "mainwindow.h"
#include "plc.h"
#include "nativedatabase.h"
#include "log.h"
#include "Work/Arrived/arrived.h"
#include "Work/Pack/pack.h"
#include "Plc/plclog.h"

typedef struct {
    QString expressId;
    CompanyIndex indexCompany = UnKnown; //公司索引
    short boxId = -1; //最终格口
    short exceptionBoxId = -1; //异常口
    int indexPLC = -1; //
    int scanCount = 0; //过相机次数
    qint64 timestamp = -1; //获取快递编号的时间戳
    short allCompanyBoxId[COMPANYNUM] = {0}; //保存是否异常口
    int cameraId = -1;
    QString pdaCode;
    QString cameraName;
} CarrierExpress;

class Express : public QObject
{
    Q_OBJECT

public:
    explicit Express(bool _comeOrGo, MainWindow *_m, QObject *parent = nullptr);

    void getBoxId(QString expressId);
    /*
     * 目的：保存单号到数组
     */
    void saveExpressTocarrierExpressVector(QString expressId, QString pdaCode,
              int cameraIndex, short exceptionBoxId, int cameraId, int carrierNum, QString cameraName);
    void saveData(QString expressId, int carrierId, int plcIndex,
                  short exceptionBoxId, int cameraId, QString pdaCode, QString cameraName);

    MainWindow *getMainWindow();

private:
    //判断字符串是否只包含数字和字母
    inline bool isLetterOrNumberString(QString str)
    {
        str = str.trimmed();
        QString::ConstIterator iterator = str.begin();
        while (iterator != str.end()) {
            if (!iterator->isLetterOrNumber()) {
                return false;
            }
            iterator++;
        }
        return true;
    }

    //根据快递号找到小车号索引（小车号减1）
    QPair<int, int> indexCarrierIdFromExpressId(QString expressId);

    void doWork(short boxId, QString expressId, int indexCarrierId,
               CompanyIndex indexCompany, int cameraId, bool isTimeOut,
                QString Code1 = QString(), QString Code2 = QString(),
                QString Code3 = QString(), QString Dest_Org = QString());

signals:
    void appendAlarm(QString message, QString level, QString component);
    void appendRunLog(QString message);
    void appendDetailLog(QString message);
    void sig_exceptionLog(QString mes);

public slots:
    //正常格口处理
    void noramlBoxIdHandle(CompanyIndex companyIndex, QString expressId,
                     QList<short> boxIdList, QString Code1 = QString(), QString Code2 = QString(),
                           QString Code3 = QString(), QString Dest_Org = QString());
    //所有的异常口在此处理
    void exceptionBoxIdHandle(CompanyIndex indexCompany, QString expressId, bool isTimeOut = false,
                              QString Code1 = QString(), QString Code2 = QString(),
                                                         QString Code3 = QString());

private:
    STO *sto;
    YTO *yto;
    ZTO *zto;
    BEST *best;
    TT *tt;
    YunDa *yunda;
    YouZheng *youzheng;

    MainWindow *m;
    PLC *plc;
    NativeDatabase *nativeDatabase;
    Log *log;
    QVector<CarrierExpress> carrierExpressVector[8];

    bool comeOrGo = true;

    //到件
    Arrived *arrived;
    Pack *pack;

    QVector<bool> dropVector; //是否掉落，作为平均分配的依据
    short carrierId[8] = {0};

    PlcLog *plclog;
    QThread *threadPlcLog;
};

#endif // EXPRESS_H
