#include "sto.h"
#include "Configure/configure.h"
#include "define.h"
#include "Work/Sort/express.h"
#include <QSqlError>
#include <QSqlQuery>
#include <QHash>
#include <QVariant>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QEventLoop>

STO::STO(bool _comeOrGo, Express *_express, QObject *parent)
    : QObject(parent),
      comeOrGo(_comeOrGo),
      express(_express)
{
    log = Log::instance();
    connect(this, &STO::appendRunLog, log, &Log::appendRunLog);
    connect(this, &STO::appendDetailLog, log, &Log::appendDetailLog);
    connect(this, &STO::sig_timeOutPlcLog, log, &Log::slot_timeOutPlcLog);
    connect(this, &STO::sig_InterceptorLog, log, &Log::slot_InterceptorLog);
    connect(this, &STO::appendAlarm, express->getMainWindow(), &MainWindow::appendAlarm);

    delay = Configure::instance()->interfaceMaxDelay();
    if (delay <= 0)
        delay = 500;

    //http方式
    manager = new QNetworkAccessManager(this);

    //connectDatabase();

    QBitArray baCome = Configure::instance()->companySwitchCome();   
    if (comeOrGo && baCome[Sto]) {
        //到件
        arrivedSto = new ArrivedSTO();
        threadArrivd = new QThread(this);
        arrivedSto->moveToThread(threadArrivd);
        connect(arrivedSto, &ArrivedSTO::appendArrivedDataToGUI,
                express->getMainWindow(), &MainWindow::showArrivedData);
        connect(arrivedSto, &ArrivedSTO::appendAlarm,
                express->getMainWindow(), &MainWindow::appendAlarm);
        connect(express->getMainWindow(), &MainWindow::arrivedSignal,
                arrivedSto, &ArrivedSTO::startOrStopTimer);
        connect(this, &STO::arrivedSignal, arrivedSto, &ArrivedSTO::arrived);

        //发件
        dispatchSto = new DispatchSTO();
        dispatchSto->moveToThread(threadArrivd);
        connect(dispatchSto, &DispatchSTO::appendDispatchDataToGUI,
                express->getMainWindow(), &MainWindow::showPackSendData);
        connect(dispatchSto, &DispatchSTO::appendAlarm,
                express->getMainWindow(), &MainWindow::appendAlarm);
        connect(express->getMainWindow(), &MainWindow::sendOrPackSignal,
                dispatchSto, &DispatchSTO::startOrStopTimer);

        threadArrivd->start();
        QMetaObject::invokeMethod(arrivedSto, &ArrivedSTO::init, Qt::QueuedConnection);
        QMetaObject::invokeMethod(dispatchSto, &DispatchSTO::init, Qt::QueuedConnection);
    }

    //集包
    QBitArray ba = Configure::instance()->companySwitchGo();
    if (!comeOrGo && ba[Sto]) {
        packSto = new PackSTO;
        threadPack = new QThread(this);
        packSto->moveToThread(threadPack);
        connect(packSto, &PackSTO::appendPackDataToGUI,
                express->getMainWindow(), &MainWindow::showPackSendData);
        connect(express->getMainWindow(), &MainWindow::sendOrPackSignal,
                packSto, &PackSTO::setValid);
        threadPack->start();
        QMetaObject::invokeMethod(packSto, &PackSTO::init, Qt::QueuedConnection);
    }
}

void STO::getBoxIdDB(QString expressId)
{
    if (comeOrGo) {
        getBoxIdCome(expressId);
    } else {
        getBoxIdGo(expressId);
    }
}

short STO::getBoxId(QString expressId)
{
    QString url = Configure::instance()->configureSTO->stoInterfaceUrl() + expressId;
    QNetworkReply *reply = manager->get(QNetworkRequest(url));
    QTimer timer;
    timer.setSingleShot(true);

    QEventLoop loop;
    connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    timer.start((int)delay);
    loop.exec();

    if (timer.isActive()) {
        timer.stop();
        getBoxIdComeOrGo(reply);
        return 1;
    } else {
        disconnect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
        reply->abort();
        express->exceptionBoxIdHandle(Sto, expressId, true);
        return -2;
    }
}

void STO::getBoxIdComeOrGo(QNetworkReply *reply)
{
    QList<short> boxIdList;

    if (reply && reply->error() == QNetworkReply::NoError) {
        QByteArray text = reply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(text);
        //解析
        QString expressId = doc["waybillNo"].toString();
        if (expressId.isEmpty()) {
            emit appendDetailLog("申通分拣：快递单号为空");
        }
        QString code1 = doc["code1"].toString();
        QString code2 = doc["code2"].toString();
        QString code3 = doc["code3"].toString();
        QString oderTypeEx = doc["oderTypeEx"].toString();
        emit appendDetailLog("申通分拣实际：" + code1 + "," + code2 + "," + code3);
        QHash<QStringList, QList<short> > rule;
        if (comeOrGo) {//进港
            rule = Configure::instance()->configureSTO->ruleStoCome();

            //找到对应的格口
            QHashIterator<QStringList, QList<short> > iterator(rule);
            while (iterator.hasNext()) {
                iterator.next();
                QStringList codeList = iterator.key();
                QString _Code1 = codeList.value(0);
                QString _Code2 = codeList.value(1);
                QString _Code3 = codeList.value(2);


                if (((!_Code2.isEmpty()) && (_Code2 != code2))) { //空的时候表示任意，即一定满足，所以不用比较
                    continue;
                }
                if (((!_Code3.isEmpty()) && (_Code3 != code3))) { //空的时候表示任意，即一定满足，所以不用比较
                    continue;
                }
                if (((!_Code1.isEmpty()) && (_Code1 != code1))) { //空的时候表示任意，即一定满足，所以不用比较
                    continue;
                }

                boxIdList = iterator.value();
                break;
            }
            if (boxIdList.size() > 0) { //不等于0，正常;
                express->noramlBoxIdHandle(Sto, expressId, boxIdList, code1, code2, code3);
            } else { //异常
                emit appendDetailLog(QString("申通分拣：申通异常"));
                express->exceptionBoxIdHandle(Sto, expressId, false, code1, code2, code3);
            }

            //拦截件保存到单独的日志
            if (code1.isEmpty() && code2.isEmpty() && code3.isEmpty() && oderTypeEx == "Interceptor") {
                emit sig_InterceptorLog(expressId);
            }
        } else { //出港
            QVector<QPair<QStringList, QList<short> > >boxIdVector;
            //出港
            rule = Configure::instance()->configureSTO->ruleStoGo();

            //先找符合所有的，没有，救找

            //找到所有符合条件的格口
            QHashIterator<QStringList, QList<short> > iterator(rule);
            while (iterator.hasNext()) {
                iterator.next();
                QStringList codeList = iterator.key();
                QString _Code1 = codeList.value(0);
                QString _Code2 = codeList.value(1);
                QString _Code3 = codeList.value(2);
                if (((!_Code1.isEmpty()) && (_Code1 != code1))) { //空的时候表示任意，即一定满足，所以不用比较
                    continue;
                }
                if (((!_Code2.isEmpty()) && (_Code2 != code2))) { //空的时候表示任意，即一定满足，所以不用比较
                    continue;
                }
                if (((!_Code3.isEmpty()) && (_Code3 != code3))) { //空的时候表示任意，即一定满足，所以不用比较
                    continue;
                }
                QPair<QStringList, QList<short> > pair(codeList, iterator.value());
                boxIdVector.append(pair);
                emit appendDetailLog("申通分拣-：" + codeList.join(','));
            }

            if (boxIdVector.size() == 1) {
                boxIdList = boxIdVector[0].second;
            } else if (boxIdVector.size() > 1) { //不止一种情况，哪个更完全，更正确
                emit appendDetailLog("申通分拣过滤" + QString::number(boxIdVector.size()));
                for (int i = 0; i < boxIdVector.size(); ++i) {
                    QStringList list = boxIdVector[i].first;
                    emit appendDetailLog("申通分拣过滤" + list[1]);
                    if (!list[1].isEmpty()) {
                        emit appendDetailLog("申通分拣过滤" + list[0]);
                        boxIdList = boxIdVector[i].second;
                        break;
                    }
                }
            }

            if (boxIdList.size() > 0) {
                //根据快递号获取小车号和相机索引，并设置小车快递数组
                express->noramlBoxIdHandle(Sto, expressId, boxIdList);
            } else {
                express->exceptionBoxIdHandle(Sto, expressId);
            }
        }
    }

    reply->deleteLater();
}


void STO::connectDatabase()
{
    if (QSqlDatabase::contains("ALY")) {
        db = QSqlDatabase::database("ALY");
    } else {
        db = QSqlDatabase::addDatabase("QMYSQL", "ALY");
    }

    QString ip = Configure::instance()->configureSTO->alyIP();
    QString userName = Configure::instance()->configureSTO->alyUserName();
    QString password = Configure::instance()->configureSTO->alyPassword();
    db.setHostName(ip);
    db.setUserName(userName);
    db.setPassword(password);
    db.setDatabaseName(ALY_DB_DBNAME);
    db.setConnectOptions("MYSQL_OPT_RECONNECT=1"); //断开自动重连

    if (!db.open()) {        
        express->getMainWindow()->alarm(
                    QString("<font style='color:red'>阿里云数据库连接失败：%1</font>").arg(db.lastError().text()));
        emit appendDetailLog(QString("<font style='color:red'>%1</font>").arg(db.lastError().text()));
    } else {
        emit appendDetailLog(QString("<font style='color:green'>阿里云数据库连接成功！</font>"));
    }
}

void STO::getBoxIdCome(QString expressId)
{
    QString sql = QString(
        "SELECT Code1, Code2, Code3 FROM %1 WHERE WAYBILLNO = '%2' ORDER BY SysRecevieTime")
            .arg(ALY_DB_TABLENAME).arg(expressId);
    emit appendDetailLog(sql);

    QSqlQuery query(sql, db);

    //开始处理
    QVector<QList<short> > normal; //正常格口存放数组
    QString _Code1, _Code2, _Code3;
    int priority = -1; //优先级
    QList<short> boxId;
    QJsonObject &OrderTypeEx = Configure::instance()->configureSTO->OrderTypeEx();
    QHash<QStringList, QList<short> > &inRule = Configure::instance()->configureSTO->ruleStoCome();

    while (query.next()) {
        //根据规则获取格口，正常口加入数组
        QString Code1 = query.value("Code1").toString();
        QString Code2 = query.value("Code2").toString();
        QString Code3 = query.value("Code3").toString();
        emit appendDetailLog(QString("一段码：%1，二段码：%2，三段码：%3").arg(Code1).arg(Code2).arg(Code3));

        QStringList& code1List = Configure::instance()->configureSTO->code1ListContain();
        QStringList& code2List = Configure::instance()->configureSTO->code2ListContain();


        if ((code1List.contains(Code1)) &&
                (code2List.contains(Code2)) &&
                boxId.size() > 0) {
            QString orderTypeEx = query.value("OrderTypeEx").toString();
            int _priority = OrderTypeEx[orderTypeEx].toInt();
            if (_priority >= priority) {
                normal.append(boxId);
                priority = _priority;
                _Code1 = Code1;
                _Code2 = Code2;
                _Code3 = Code3;
            }

        } else {
            QString orderTypeEx = query.value("OrderTypeEx").toString();
            int _priority = OrderTypeEx[orderTypeEx].toInt();
            if (_priority >= priority) {
                priority = _priority;
                _Code1 = Code1;
                _Code2 = Code2;
                _Code3 = Code3;
            }
        }
    }

    if (normal.isEmpty()) { //异常
        emit appendDetailLog(QString("申通异常"));
        express->exceptionBoxIdHandle(Sto, expressId);
    } else { //正常口
        boxId = normal.last();
        //获取到数据后，最后处理
//        QString boxIds;
//        for (int i = 0; i < normal.size(); ++i) {
//            boxIds += QString("格口：%1 ").arg(normal.value(i));
//        }
//        emit appendDetailLog(QString("申通正常：%1").arg(boxIds));
        express->noramlBoxIdHandle(Sto, expressId, boxId);
        //上传
//        if (express->getMainWindow()->arrivedFlag() &&
//                Configure::instance()->configureSTO->arrivedSource() == 0) {
////            arrivedSto->arrived(expressId, boxId,
////                                QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
//            QString orgCode = Configure::instance()->configureSTO->orgCode();
//            QString userCode = Configure::instance()->configureSTO->userCodeVector().value(boxId);
//            emit arrivedSignal(expressId, orgCode, userCode, boxId,
//              QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"), "isArrived", "");
//        }
    }
}

void STO::getBoxIdGo(QString expressId)
{
    QList<short> boxId;
    //先查拦截件
    if (Configure::instance()->configureSTO->interceptExpress().contains(expressId))
        boxId.append(Configure::instance()->configureSTO->interceptBoxId());

    //查阿里云
    QString sql = QString("SELECT * FROM orderautocategory.orderrecevie WHERE WAYBILLNO = '%1' ORDER BY SysRecevieTime").arg(expressId);
    QSqlQuery query(sql);

    //开始处理
    QVector<QList<short> > normal; //存放正常口
    QString _Code1, _Code2, _Code3;
    int priority = -1; //优先级
    QJsonObject &OrderTypeEx = Configure::instance()->configureSTO->OrderTypeEx();
    QHash<QStringList, QList<short> > &outRule = Configure::instance()->configureSTO->ruleStoGo();

    while (query.next()) {
        QString Code1 = query.value("Code1").toString();
        QString Code2 = query.value("Code2").toString();
        QString Code3 = query.value("Code3").toString();

        //根据规则获取格口
        if (boxId.size() > 0) { //正常口
            QString orderTypeEx = query.value("OrderTypeEx").toString();
            int _priority = OrderTypeEx[orderTypeEx].toInt();
            if (_priority >= priority) {
                normal.append(boxId);
                priority = _priority;
                _Code1 = Code1;
                _Code2 = Code2;
                _Code3 = Code3;
            }
        } else { //异常口
            QString orderTypeEx = query.value("OrderTypeEx").toString();
            int _priority = OrderTypeEx[orderTypeEx].toInt();
            if (_priority >= priority) {
                priority = _priority;
                _Code1 = Code1;
                _Code2 = Code2;
                _Code3 = Code3;
            }
        }
    }

    if (normal.isEmpty()) { //异常
        express->exceptionBoxIdHandle(Sto, expressId);
    } else { //正常口
        //根据快递号获取小车号和相机索引，并设置小车快递数组
        express->noramlBoxIdHandle(Sto, expressId, normal.last());
    }
}

void STO::getBoxIdComeOrGoAsynchronous()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    QList<short> boxIdList;

    if (reply && reply->error() == QNetworkReply::NoError) {
        QByteArray text = reply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(text);
        //解析
        QString expressId = doc["waybillNo"].toString();
        if (expressId.isEmpty()) {
            emit appendDetailLog("申通分拣：快递单号为空");
        }
        QString code1 = doc["code1"].toString();
        QString code2 = doc["code2"].toString();
        QString code3 = doc["code3"].toString();
        emit appendDetailLog("申通分拣实际：" + code1 + "," + code2 + "," + code3);
        QHash<QStringList, QList<short> > rule;
        if (comeOrGo) {//进港
            rule = Configure::instance()->configureSTO->ruleStoCome();

            //找到对应的格口
            QHashIterator<QStringList, QList<short> > iterator(rule);
            while (iterator.hasNext()) {
                iterator.next();
                QStringList codeList = iterator.key();
                QString _Code1 = codeList.value(0);
                QString _Code2 = codeList.value(1);
                QString _Code3 = codeList.value(2);

                if (((!_Code2.isEmpty()) && (_Code2 != code2))) { //空的时候表示任意，即一定满足，所以不用比较
                    continue;
                }
                if (((!_Code3.isEmpty()) && (_Code3 != code3))) { //空的时候表示任意，即一定满足，所以不用比较
                    continue;
                }
                if (((!_Code1.isEmpty()) && (_Code1 != code1))) { //空的时候表示任意，即一定满足，所以不用比较
                    continue;
                }

                boxIdList = iterator.value();
                break;
            }
            if (boxIdList.size() > 0) { //不等于0，正常;
                express->noramlBoxIdHandle(Sto, expressId, boxIdList);
                //上传
        //        if (express->getMainWindow()->arrivedFlag() &&
        //                Configure::instance()->configureSTO->arrivedSource() == 0) {
        //            arrivedSto->arrived(expressId, boxId,
        //                                QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
        //            QString orgCode = Configure::instance()->configureSTO->orgCode();
        //            QString userCode = Configure::instance()->configureSTO->userCodeVector().value(boxId);
        //            emit arrivedSignal(expressId, orgCode, userCode, boxId,
        //              QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"), "isArrived", "");
        //        }

            } else { //异常
                emit appendDetailLog(QString("申通分拣：申通异常"));
                express->exceptionBoxIdHandle(Sto, expressId, 0);
            }
        } else { //出港
            QVector<QPair<QStringList, QList<short> > >boxIdVector;
            //出港
            rule = Configure::instance()->configureSTO->ruleStoGo();

            //先找符合所有的，没有，救找

            //找到所有符合条件的格口
            QHashIterator<QStringList, QList<short> > iterator(rule);
            while (iterator.hasNext()) {
                iterator.next();
                QStringList codeList = iterator.key();
                QString _Code1 = codeList.value(0);
                QString _Code2 = codeList.value(1);
                QString _Code3 = codeList.value(2);
                if (((!_Code1.isEmpty()) && (_Code1 != code1))) { //空的时候表示任意，即一定满足，所以不用比较
                    continue;
                }
                if (((!_Code2.isEmpty()) && (_Code2 != code2))) { //空的时候表示任意，即一定满足，所以不用比较
                    continue;
                }
                if (((!_Code3.isEmpty()) && (_Code3 != code3))) { //空的时候表示任意，即一定满足，所以不用比较
                    continue;
                }
                QPair<QStringList, QList<short> > pair(codeList, iterator.value());
                boxIdVector.append(pair);
                emit appendDetailLog("申通分拣-：" + codeList.join(','));
            }

            if (boxIdVector.size() == 1) {
                boxIdList = boxIdVector[0].second;
            } else if (boxIdVector.size() > 1) { //不止一种情况，哪个更完全，更正确
                emit appendDetailLog("申通分拣过滤" + QString::number(boxIdVector.size()));
                for (int i = 0; i < boxIdVector.size(); ++i) {
                    QStringList list = boxIdVector[i].first;
                    emit appendDetailLog("申通分拣过滤" + list[1]);
                    if (!list[1].isEmpty()) {
                        emit appendDetailLog("申通分拣过滤" + list[0]);
                        boxIdList = boxIdVector[i].second;
                        break;
                    }
                }
            }

            if (boxIdList.size() > 0) {
                //根据快递号获取小车号和相机索引，并设置小车快递数组
                express->noramlBoxIdHandle(Sto, expressId, boxIdList);
            } else {
                express->exceptionBoxIdHandle(Sto, expressId, 0);
            }
        }
    }

    reply->deleteLater();
}

void STO::getBoxIdAsynchronous(QString expressId)
{
    QString url = Configure::instance()->configureSTO->stoInterfaceUrl() + expressId;
    QNetworkReply *reply = manager->get(QNetworkRequest(url));
    connect(reply, &QNetworkReply::finished, this, &STO::getBoxIdComeOrGoAsynchronous);
}
