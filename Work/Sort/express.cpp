#include "express.h"
#include "Configure/configure.h"
#include "define.h"

#include <QDateTime>

Express::Express(bool _comeOrGo, MainWindow *_m, QObject *parent)
    : QObject(parent),
      m(_m), comeOrGo(_comeOrGo)
{
    log = Log::instance();
    connect(this, &Express::appendRunLog, log, &Log::appendRunLog);
    connect(this, &Express::appendDetailLog, log, &Log::appendDetailLog);
    connect(this, &Express::sig_exceptionLog, log, &Log::slot_ExceptionLog);
    connect(this, &Express::appendAlarm, m, &MainWindow::appendAlarm);
    //按照所配置的小车数量调整小车快递数组大小
    QVector<StructCamera> cameras = Configure::instance()->camerasVector();
    for (int i=0; i<cameras.size(); ++i) {
        int carrierNum = cameras[i].carrierNum;
        carrierExpressVector[i].resize(carrierNum);
    }

    dropVector.resize(400);
    dropVector.fill(false, 400);
    //初始化
    QHash<QStringList, QList<short> > rule;
    if (comeOrGo) {
        rule = Configure::instance()->configureSTO->ruleStoCome();
    } else {
        rule = Configure::instance()->configureSTO->ruleStoGo();
    }
    QHashIterator<QStringList, QList<short> > iterator(rule);
    while (iterator.hasNext()) {
        iterator.next();
        QList<short> boxIdList = iterator.value();
        //初始化为第一个开，为true
        if (boxIdList.size() >= 0)
            dropVector[boxIdList[0]] = true;
    }

    if (comeOrGo) {
        const QBitArray &baCome = Configure::instance()->companySwitchCome();
        if (baCome[Sto])
            sto = new STO(comeOrGo, this, this);
        if (baCome[Zto])
            zto = new ZTO(comeOrGo, this, this);
        if (baCome[Yto])
            yto = new YTO(comeOrGo, this, this);
        if (baCome[Best])
            best = new BEST(comeOrGo, this, this);
        if (baCome[Yunda])
            yunda = new YunDa(comeOrGo, this, this);
        if (baCome[Tt])
            tt = new TT(comeOrGo, this, this);
        if (baCome[Youzheng])
            youzheng = new YouZheng(comeOrGo, this, this);
    } else {
        const QBitArray &baGo= Configure::instance()->companySwitchGo();
        if (baGo[Sto])
            sto = new STO(comeOrGo, this, this);
        if (baGo[Zto])
            zto = new ZTO(comeOrGo, this, this);
        if (baGo[Yto])
            yto = new YTO(comeOrGo, this, this);
        if (baGo[Best])
            best = new BEST(comeOrGo, this, this);
        if (baGo[Yunda])
            yunda = new YunDa(comeOrGo, this, this);
        if (baGo[Tt])
            tt = new TT(comeOrGo, this, this);
        if (baGo[Youzheng])
            youzheng = new YouZheng(comeOrGo, this, this);
    }


    plc = new PLC(this);

    long lErr1 = plc->initAddress();
    if (lErr1) {
        m->alarm(QString("<font color=red>分拣：PLC初始化地址失败，错误代号%1</font>").arg(lErr1));
        emit appendAlarm(QString("<font color='red'>分拣：PLC初始化地址失败，错误代号%1</font>").arg(lErr1),
                         "严重", "PLC");
    }
    long lErr2 = plc->initHandle();
    if (lErr2) {
        m->alarm(QString("<font color='red'>分拣：PLC初始化句柄失败，错误代号%1</font>").arg(lErr2));
        emit appendAlarm(QString("<font color='red'>分拣：PLC初始化句柄失败，错误代号%1</font>").arg(lErr1),
                         "严重", "PLC");
    }

    long lErr = -1L;
    int num = 10;
    while (lErr && num) {
        if (comeOrGo) {
            lErr = plc->writeTableName(NATIVE_DB_STO_COME);
        } else {
            lErr = plc->writeTableName(NATIVE_DB_STO_GO);
        }
        num--;
    }
    if (lErr) {
        m->alarm("<font color='red'>PLC写表名错误，PLC将无法写入掉落时间</font>");
    }

    nativeDatabase = new NativeDatabase(this);

    //plc报警
    plclog = new PlcLog(m, plc);
    threadPlcLog = new QThread(this);
    plclog->moveToThread(threadPlcLog);
    threadPlcLog->start();
    QMetaObject::invokeMethod(plclog, &PlcLog::init, Qt::QueuedConnection);
}

//所有的快递公司接口请求-异步
void Express::getBoxId(QString expressId)
{
    if (comeOrGo) {
        const QBitArray &baCome = Configure::instance()->companySwitchCome();
        if (baCome[Sto])
            sto->getBoxId(expressId);
        if (baCome[Zto])
            zto->getBoxId(expressId);
        if (baCome[Yto])
            yto->getBoxId(expressId);
        if (baCome[Best])
            best->getBoxId(expressId);
        if (baCome[Tt])
            tt->getBoxId(expressId);
        if (baCome[Yunda])
            yunda->getBoxId(expressId);
        if (baCome[Youzheng])
            youzheng->getBoxId(expressId);
    } else { //出港
        const QBitArray &baGo = Configure::instance()->companySwitchGo();
        if (baGo[Sto])
            sto->getBoxId(expressId);
        if (baGo[Zto])
            zto->getBoxId(expressId);
        if (baGo[Yto])
            yto->getBoxId(expressId);
        if (baGo[Best])
            best->getBoxId(expressId);
        if (baGo[Tt])
            tt->getBoxId(expressId);
        if (baGo[Yunda])
            yunda->getBoxId(expressId);
        if (baGo[Youzheng])
            youzheng->getBoxId(expressId);
    }

}

void Express::saveExpressTocarrierExpressVector(QString expressId, QString pdaCode,
      int plcIndex, short exceptionBoxId, int cameraId, int carrierNum, QString cameraName)
{
    carrierId[cameraId]++;
    if (carrierId[cameraId] >= carrierNum)
        carrierId[cameraId] = 1;

    saveData(expressId, carrierId[cameraId], plcIndex,
             exceptionBoxId, cameraId, pdaCode, cameraName);

    //格口请求
    //getBoxId(expressId);
    //同步方法
    short value = sto->getBoxId(expressId);
    if (value == -2) {
        emit appendDetailLog(QString("快递单号%1超时").arg(expressId));
    }
}

void Express::saveData(QString expressId, int carrierId, int plcIndex,
                       short exceptionBoxId, int cameraId, QString pdaCode, QString cameraName)
{
    //先清掉之前存在的数据
    QPair<int, int> pair = indexCarrierIdFromExpressId(expressId);
    int _cameraId = pair.first;
    int _carrierId = pair.second;
    if (_cameraId >= 0 && _carrierId >= 0) {
        carrierExpressVector[_cameraId][_carrierId].expressId = QString();
        carrierExpressVector[_cameraId][_carrierId].boxId = -1;
        carrierExpressVector[_cameraId][_carrierId].indexPLC = -1;
        carrierExpressVector[_cameraId][_carrierId].scanCount = 0;
        carrierExpressVector[_cameraId][_carrierId].timestamp = -1;
        carrierExpressVector[_cameraId][_carrierId].exceptionBoxId = -1;
        carrierExpressVector[_cameraId][_carrierId].cameraId = -1;
        carrierExpressVector[_cameraId][_carrierId].indexCompany = UnKnown;
        carrierExpressVector[_cameraId][_carrierId].pdaCode = QString();
        carrierExpressVector[_cameraId][_carrierId].cameraName = cameraName;
        for (int i = 0; i < COMPANYNUM; ++i) { //不管新旧，重新来一遍，异常口都得重新初始化
            carrierExpressVector[_cameraId][_carrierId].allCompanyBoxId[i] = -1;
        }
    }

    //存入数组
    carrierExpressVector[cameraId][carrierId - 1].expressId = expressId;
    carrierExpressVector[cameraId][carrierId - 1].boxId = -1;
    carrierExpressVector[cameraId][carrierId - 1].indexPLC = plcIndex;
    carrierExpressVector[cameraId][carrierId - 1].scanCount = 1;
    carrierExpressVector[cameraId][carrierId - 1].timestamp = QDateTime::currentMSecsSinceEpoch();
    carrierExpressVector[cameraId][carrierId - 1].exceptionBoxId = exceptionBoxId;
    carrierExpressVector[cameraId][carrierId - 1].cameraId = cameraId;
    carrierExpressVector[cameraId][carrierId - 1].indexCompany = UnKnown;
    carrierExpressVector[cameraId][carrierId - 1].pdaCode = pdaCode;
    for (int i = 0; i < COMPANYNUM; ++i) { //不管新旧，重新来一遍，异常口都得重新初始化
        carrierExpressVector[cameraId][carrierId - 1].allCompanyBoxId[i] = -1;
    }
}

MainWindow *Express::getMainWindow()
{
    return m;
}

QPair<int, int> Express::indexCarrierIdFromExpressId(QString expressId)
{
    /*
     * 注意：这里为了处理掉了的件又拿上来分拣，可以进行复杂的处理
     * 1）不能删掉已经掉落的件，因为需要计数（如果不需要计数，删掉是最简单的方法）
     * 2）在不能删掉的情况下，可以找到并且最终格口是-1的
     * 3）如果都找不到，就取最近的时间戳的小车快递
     */
    //这里什么情况都不考虑，只考虑最简单的
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < carrierExpressVector[i].size(); ++j) {
            if (expressId == carrierExpressVector[i][j].expressId) {
                QPair<int, int> pair(i, j);
                return pair;
            }
        }
    }

    return QPair<int, int>(-1, -1);
}

void Express::doWork(short boxId, QString expressId, int indexCarrierId,
                     CompanyIndex indexCompany, int cameraId, bool isTimeOut,
                     QString Code1, QString Code2, QString Code3, QString Dest_Org)
{
    emit appendDetailLog(QString("格口：%1，快递：%2，小车索引：%3，公司索引：%4").arg(boxId)
                         .arg(expressId).arg(indexCarrierId).arg(indexCompany));
    //保存最终格口
    carrierExpressVector[cameraId][indexCarrierId].boxId = boxId;
    //保存所属快递公司
    carrierExpressVector[cameraId][indexCarrierId].indexCompany = indexCompany;
    QString pdaCode = carrierExpressVector[cameraId][indexCarrierId].pdaCode;
    //emit appendDetailLog(QString("函数：%1，行数：%2").arg(__func__).arg(__LINE__));
    int plcIndex = carrierExpressVector[cameraId][indexCarrierId].indexPLC;
    //写入plc
    plc->writeBoxId(expressId, indexCarrierId + 1, boxId, plcIndex);

    //写入数据库
    if (comeOrGo) {
        nativeDatabase->insertCome(indexCompany, expressId, boxId, pdaCode, Code1, Code2, Code3);
        //插入总表
        nativeDatabase->insertTotalCome(indexCompany, expressId, boxId, Dest_Org);
    } else {
        nativeDatabase->insertGo(indexCompany, expressId, boxId, Code1, Code2, Code3);
        //插入总表
        nativeDatabase->insertTotalGo(indexCompany, expressId, boxId);
    }
    //计算接口延时
    qint64 delay = QDateTime::currentMSecsSinceEpoch() - carrierExpressVector[cameraId][indexCarrierId].timestamp;
    if (delay > Configure::instance()->interfaceMaxDelay()) {
        emit appendAlarm(QString("快递单号：%1，时间：%2，").arg(expressId).arg(delay), "警告", "服务器返回延迟较大");
    }
    //显示到界面
    QString strTimeOut;
    if (isTimeOut)
        strTimeOut = "-超时";
    QString boxId1 = QString("%1|延时%2%3").arg(boxId).arg(delay).arg(strTimeOut);
    m->showData(cameraId, indexCompany, expressId, QString::number(indexCarrierId + 1),
       boxId1, carrierExpressVector[cameraId][indexCarrierId].scanCount);
    //写入日志
    emit appendRunLog(QString("时间：%1, 快递：%2, 小车：%3, 格口：%4, 公司：%5，扫描次数：%6，时延：%7，相机：%8")
                      .arg(QDateTime::currentDateTime().toString("yyyy/MM/dd hh:mm:ss")).arg(expressId)
                      .arg(indexCarrierId + 1).arg(boxId).arg(indexCompany)
                      .arg(carrierExpressVector[cameraId][indexCarrierId].scanCount).arg(delay)
                      .arg(carrierExpressVector[cameraId][indexCarrierId].cameraName));
    //发送异常件到异常日志
    if (carrierExpressVector[cameraId][indexCarrierId].exceptionBoxId == boxId)
        emit sig_exceptionLog(expressId);

}

void Express::exceptionBoxIdHandle(CompanyIndex indexCompany, QString expressId, bool isTimeOut,
                                   QString Code1, QString Code2, QString Code3)
{
    //emit appendDetailLog(QString("函数：%1，行数：%2").arg(__func__).arg(__LINE__));
    /*
     * 注意：这里为了处理掉了的件又拿上来分拣，可以进行复杂的处理
     * 1）不能删掉已经掉落的件，因为需要计数（如果不需要计数，删掉是最简单的方法）
     * 2）在不能删掉的情况下，可以找到并且最终格口是-1的
     * 3）如果都找不到，就取最近的时间戳的小车快递
     */

    //这里什么情况都不考虑，只考虑最简单的
    if (comeOrGo) {
        const QBitArray &ba = Configure::instance()->companySwitchCome();
        QPair<int, int> pair = indexCarrierIdFromExpressId(expressId);
        int cameraId = pair.first;
        int carrierId = pair.second;
        if (cameraId >= 0 && carrierId >= 0) {
            //给对应的格口赋值
            carrierExpressVector[cameraId][carrierId].allCompanyBoxId[indexCompany]
                    = carrierExpressVector[cameraId][carrierId].exceptionBoxId;

            //判断是否全为异常口
            for (int i = 0; i < COMPANYNUM; ++i) {
                if(ba[i]) {//如果对应的公司为开启，需要计算
                    //如果需要计算的公司的格口不是异常口，则要么是正常口，要么是还没到来
                    if (carrierExpressVector[cameraId][carrierId].allCompanyBoxId[i]
                            != carrierExpressVector[cameraId][carrierId].exceptionBoxId)
                        return;
                }
            }

            //没有return，则说明都是等于，就说明肯定异常口
            doWork(carrierExpressVector[cameraId][carrierId].exceptionBoxId,
                   expressId, carrierId, Exception, cameraId, isTimeOut);
        }
        //emit appendDetailLog(QString("函数：%1，行数：%2").arg(__func__).arg(__LINE__));
    } else { //出港
        //直接断定异常口
        QPair<int, int> pair = indexCarrierIdFromExpressId(expressId);
        int cameraId = pair.first;
        int carrierId = pair.second;
        if (cameraId >= 0 && carrierId >= 0) {
            //给对应的格口赋值
            carrierExpressVector[carrierId][cameraId].allCompanyBoxId[indexCompany]
                    = carrierExpressVector[carrierId][cameraId].exceptionBoxId;

            doWork(carrierExpressVector[carrierId][cameraId].exceptionBoxId,
                   expressId, carrierId, Exception, cameraId, isTimeOut);
        }
        //emit appendDetailLog(QString("函数：%1，行数：%2").arg(__func__).arg(__LINE__));
    }
}

void Express::noramlBoxIdHandle(CompanyIndex companyIndex, QString expressId,
                                QList<short> boxIdList, QString Code1, QString Code2,
                                QString Code3, QString Dest_Org)
{
    //QString str;
//    foreach(short boxId, boxIdList) {
//        str += QString("%1,").arg(boxId);
//    }
//    str.chop(1);
    //emit appendDetailLog(QString("函数：%1, 格口%2").arg(__func__).arg(str));
    QPair<int, int> pair = indexCarrierIdFromExpressId(expressId);
    int cameraId = pair.first;
    int carrierId = pair.second;
    //找到正确的格口
    if (boxIdList.size() == 1) {
        if (boxIdList.value(0) > 0)
            doWork(boxIdList.value(0), expressId, carrierId, companyIndex, cameraId, false,
                   Code1, Code2, Code3, Dest_Org);
        else
            exceptionBoxIdHandle(Exception, expressId, false, Code1, Code2, Code3);

    } else if (boxIdList.size() > 1){ //平均掉落
        int size = boxIdList.size(); //预排格口的数量
        //保证每次都只有一个是false，表示
        short boxId = boxIdList[0]; //默认取第一个

        for (int i = 0; i < size; ++i) { //找到格口为true开的格口
            if (dropVector[boxIdList[i]]) {
                boxId = boxIdList[i];
                //本格口关闭
                dropVector[boxIdList[i]] = false;
                //后面一个格口开启
                i++;
                if (i >= size) {
                    //开启第一个
                    dropVector[boxIdList[0]] = true;
                } else {
                    dropVector[boxIdList[i]] = true;
                }
            }
        }
        doWork(boxId, expressId, carrierId, companyIndex, cameraId, false, Code1, Code2, Code3, Dest_Org);

        //short startBoxId = carrierExpressVector[indexCarrierId].startBoxId;
        //short endBoxId = carrierExpressVector[indexCarrierId].endBoxId;
//        for (int i = 0; i < boxIdList.size(); ++i) { //就近原则
//            short boxId = boxIdList[i];
//            if (boxId <= 0)
//                continue;
//            if (boxId >= startBoxId && boxId <= endBoxId) {
//                doWork(boxId, expressId, indexCarrierId, companyIndex);
//                return;
//            }
//        }
        //如果执行到这一步，就找默认找第一个，也可以复杂的实行平均分配
        //doWork(boxIdList.value(0), expressId, indexCarrierId, companyIndex);
    }
    //qDebug() << __func__ << __LINE__;
}
