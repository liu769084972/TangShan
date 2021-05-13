#include "arrivedyto.h"
#include "Configure/configure.h"
#include "define.h"

#include <QSettings>
#include <QSqlError>
#include <QSqlQuery>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QTextCodec>
#include <QDateTime>
#include <QFile>

ArrivedYTO::ArrivedYTO(QObject *parent) : QObject(parent)
{

}

void ArrivedYTO::arrived(QString expressId, int boxId, QString time)
{
    QJsonObject obj = createJsonObject(expressId, time,
        Configure::instance()->configureYTO->nextOrgCode(), boxId);
    //创建消息实体
    QByteArray message = createMessage(createOpRecord(obj));
    //发送消息
    sendMessage(message);
    //更新标志位
    QString update = QString("UPDATE %1 SET isArrived = 1 WHERE EXPRESSFLOW_ID ='%2'")
            .arg(NATIVE_DB_YTO_COME).arg(expressId);
    emit appendDetailLog(update);
    QSqlQuery queryUpdate(update, db);

    QString str = QString("更新标志条数：%1").arg(queryUpdate.numRowsAffected());
    emit appendDetailLog(str);
    //发送到界面
    emit appendArrivedDataToGUI(expressId, "圆通", "上传");
}

void ArrivedYTO::init()
{
    log = Log::instance();
    connect(this, &ArrivedYTO::appendDetailLog, log, &Log::appendDetailLog);

    sql = QString("SELECT * FROM %1 WHERE isArrived IS NULL")
            .arg(NATIVE_DB_YTO_COME);
    emit appendDetailLog(QString("圆通到件：%1").arg(sql));

    createDbConnection();

    createSocketConnection();
#if STO_SIGNATURE
    getSignature();
#endif

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &ArrivedYTO::getData);

}

QString ArrivedYTO::createDbConnection()
{
    if (QSqlDatabase::contains("ArrivedYto")) {
        db = QSqlDatabase::database("ArrivedYto");
    } else {
        db = QSqlDatabase::addDatabase("QMYSQL", "ArrivedYto");
    }

    db.setHostName(Configure::instance()->nativeip());
    db.setUserName(NATIVE_DB_USERNAME);
    db.setPassword(NATIVE_DB_PASSWORD);
    db.setDatabaseName(NATIVE_DB_DBNAME);
    db.setConnectOptions("MYSQL_OPT_RECONNECT=1");
    if (db.open()) {
        emit appendDetailLog("圆通到件：本地数据库连接成功");
        return "圆通到件：本地数据库连接成功";
    } else {
        emit appendDetailLog(QString("圆通到件：本地数据库打开失败：%1").arg(db.lastError().text()));
        return QString("圆通到件：本地数据库打开失败：%1").arg(db.lastError().text());
    }
}

void ArrivedYTO::getSignature()
{
    //组装json
    QJsonObject obj;
    obj.insert("mty", YTO_MTY_SIGNATURE);
    obj.insert("clientMac", Configure::instance()->configureYTO->signatureArg()["clientMac"]); //00-50-56-C0-00-01
    obj.insert("clientIp", Configure::instance()->configureYTO->signatureArg()["clientIp"]); //172.31.76.101
    obj.insert("devType", "1");
    obj.insert("userName", Configure::instance()->configureYTO->signatureArg()["userName"]);
    obj.insert("password", Configure::instance()->configureYTO->signatureArg()["password"]);
    obj.insert("cameraMac", Configure::instance()->configureYTO->signatureArg()["cameraMac"]); //macViewer1
    obj.insert("clientId", "1");
    obj.insert("extend1", "");
    obj.insert("extend2", "");
    obj.insert("extend3", "");
    QByteArray json = QJsonDocument(obj).toJson(QJsonDocument::Compact) + "\r\n";
    int length = json.length() - 2; //json长度
    QByteArray str = (QString("%1").arg(length, 8, 10, QLatin1Char('0')) + json).toLatin1();
    //qDebug() << "请求签名消息：" << qPrintable(str) << "\r\n";
    //socket请求和获取
    QTcpSocket socket;
    socket.connectToHost(Configure::instance()->configureYTO->ip(),
                          Configure::instance()->configureYTO->portUpload());
    socket.waitForConnected();
    socket.write(str);
    socket.flush();
    socket.waitForReadyRead();
    QByteArray result = socket.readAll();
    QTextCodec *codec = QTextCodec::codecForName("GBK");
    QString strUnicode = codec->toUnicode(result);
    //qDebug() << "签名返回消息strUnicode" << strUnicode << "\r\n";
    //解析获取
    QByteArray resultHandle = result.remove(0, 8).trimmed();
    QTextCodec *codecResult = QTextCodec::codecForName("GBK");
    QString resultHandleUnicode = codecResult->toUnicode(resultHandle);
    //qDebug() << "截取后的字符串："<< resultHandleUnicode;
    QJsonDocument doc = QJsonDocument::fromJson(resultHandleUnicode.toUtf8());
    QJsonObject dataObj = doc["data"].toObject();
    //qDebug() << "dataObj" << dataObj;
    signature = dataObj["signature"].toString();
    //qDebug() << "signature" << signature << "\r\n";
    emit appendDetailLog(QString("圆通到件：签名：%1").arg(signature));
}

QByteArray ArrivedYTO::createOpRecord(QJsonObject opRecord)
{
    QJsonObject obj;
    obj.insert("aviType", "0"); //航空在线离线标识-传默认值0
    obj.insert("trace", "0"); //流水号-传默认值0
    obj.insert("isFirstTransfer", "1"); //实物类型-传默认值1
    obj.insert("mty", YTO_MTY_ARRIVED); //PDA接口识别码:建车签时统一使用0140（备注：可能需要一天一变）
    obj.insert("signature", signature); //数字签名-调用设备登录返回签名接口获取数字签名
    obj.insert("opRecord", opRecord); //

    //非必填
    obj.insert("opRecords","");
    obj.insert("opRecordsOffLine","");
    obj.insert("respcode","");
    obj.insert("response","");

    QJsonDocument doc(obj);
    return doc.toJson(QJsonDocument::Compact);
}

QByteArray ArrivedYTO::createMessage(QByteArray opRecord)
{
    QTextCodec *gbk = QTextCodec::codecForName("GBK");
    QByteArray opRecordGBK = gbk->fromUnicode(QString(opRecord).unicode());
    //QString length = QString("%1").arg(opRecordGBK.length());
    //QByteArray lengthGBK = gbk->fromUnicode(length.unicode());
    QString message = QString("%1%2\r\n")
            .arg(opRecordGBK.length(), 8, 10, QChar('0')).arg(QString(opRecord));
    QByteArray messageGBK = gbk->fromUnicode(message.unicode());
    return messageGBK;
}

void ArrivedYTO::getData()
{
    //emit appendDetailLog(QString("函数：%1，行数：%2").arg(__func__).arg(__LINE__));
    QSqlQuery query(sql, db);
    emit appendDetailLog(QString("函数：%1，行数：%2，sql：%3").arg(__func__).arg(__LINE__).arg(sql));

    QJsonObject obj;
    QString expressIds;

    while (query.next()) {
        QString expressId = query.value("EXPRESSFLOW_ID").toString();
        QString createTime = query.value("CREATE_TIME").toString()+".000";
        //QString nextOrgCode = query.value("DEST_ORG").toString();
        int boxId = query.value("Box_Num").toInt();
        arrived(expressId, boxId, createTime);
//        obj = createJsonObject(expressId, createTime, Configure::instance()->configureYTO->nextOrgCode(), boxId.toInt());
//        //创建消息实体
//        QByteArray message = createMessage(createOpRecord(obj));
//        //发送消息
//        sendMessage(message);
//        expressIds += '\'' + expressId + "',";
    }

//    //更新标志位
//    expressIds.chop(1); //去掉最后一位的逗号
//    QString update = QString("UPDATE %1 SET isArrived = 1 WHERE EXPRESSFLOW_ID IN(%2)")
//            .arg(Configure::instance()->goTableName()[Yto])
//            .arg(expressIds);
//    emit appendDetailLog(update);
//    QSqlQuery queryUpdate(update);
//    QString str = QString("更新标志条数：%1").arg(queryUpdate.numRowsAffected());
    //    emit appendDetailLog(str);
}

void ArrivedYTO::startOrStopTimer(bool isStart)
{
    if (isStart) {
        if (Configure::instance()->configureYTO->arrivedSource() == 1) {
            timer->start(Configure::instance()->configureYTO->intervalArrived());
            emit appendDetailLog("圆通到件开始");
         }

    } else {
        timer->stop();
        emit appendDetailLog("圆通到件停止");
    }
}

QJsonObject ArrivedYTO::createJsonObject(QString waybillNo, QString createTime, QString nextOrgCode, int boxId)
{
    QJsonObject obj;
    obj.insert("auxOpCode", "NEW"); //三个状态：NEW UPDATE DELETE
    obj.insert("createOrgCode", Configure::instance()->configureYTO->createOrgCode()); //操作网点
    obj.insert("createTerminal", Configure::instance()->configureYTO->createTerminal()); //服务器IP
    obj.insert("createTime", createTime); //服务器IP
    obj.insert("createUserCode", Configure::instance()->configureYTO->userCodeUserName().value(boxId).first); //操作员编码
    obj.insert("createUserName", Configure::instance()->configureYTO->userCodeUserName().value(boxId).second); //操作员姓名
    obj.insert("deviceType", "PDA"); //标记的是所用的设备是PDA还是PC端（备注：统一PDA）
    obj.insert("effectiveTypeCode", Configure::instance()->configureYTO->effectiveTypeCode()); //T001：早航班(次日下午达)T002：晚航班(次晨达)T003：正常航班T004：汽运T005：第三方运输T006：标准T007：同城当天T008:区域当天
    obj.insert("expType", Configure::instance()->configureYTO->expType()); //实物类型-10:运单20:包签30:笼签40:车签
    obj.insert("expressContentCode", Configure::instance()->configureYTO->expressContentCode()); //判断重量0.15以下为文件DOC，0.15以上是PKG
    obj.insert("feeFlag", Configure::instance()->configureYTO->feeFlag()); //计费标识-0：无效1：有效8：有效--已冻结9：有效--已完成，结算完成
    obj.insert("frequencyNo", Configure::instance()->configureYTO->frequencyNo()); //频次收入1.FC100901012.FC100901023.FC100901034.FC100901095.FC100901106.FC100901507.FC10090151
    obj.insert("waybillNo", waybillNo); //运单号-存放车签
    obj.insert("ioType", "01"); //收发类型-存放车签
    obj.insert("nextOrgCode", nextOrgCode); //下一网点（拆包地）
    obj.insert("orgCode", Configure::instance()->configureYTO->orgCode()); //操作网点-当前操作网点
    obj.insert("previousOrgCode", Configure::instance()->configureYTO->previousOrgCode()); //上一网点
    obj.insert("remark", "10010001"); //备注-建车签统一默认传值：“10020007”,建包统一默认传值：“10010001”
    obj.insert("sourceOrgCode", Configure::instance()->configureYTO->sourceOrgCode()); //始发网点-当前操作网点
    obj.insert("weighWeight", "0.0"); //称入重KG
    obj.insert("pkgWidth", "0.0"); // 宽
    obj.insert("inputWeight", "0.0"); //输入重
    obj.insert("pkgLength", "0.0"); //长
    obj.insert("pkgHeight", "0.0"); //高
    obj.insert("volumeWeight", "0.0"); //体积重
    obj.insert("opCode", YTO_OPCODE_ARRIVERD); //操作码-识别各个扫描模块的编码（车签是：170）
    obj.insert("pkgQty", "0"); //包裹数量-扫车签号时默认赋值0，扫运单号的时候赋值1
    obj.insert("routeCode", "0"); //路由检查代码
    obj.insert("type", "0"); //类型-（自动化分拣）标注是否是补码，0为不补码，1补码
    obj.insert("line", Configure::instance()->configureYTO->line()); //自动分拣线-（自动化分拣）场地流水线(1或2)

//    //非必填
//    {
        obj.insert("auxRouteCode", ""); //路由检查辅助代码
    //对于运单号小车，必填
        obj.insert("containerNo", containerNo); //容器号"CQ12345678"
        //destOrgCode可由数据库获取
        obj.insert("desOrgCode", ""); //拆包地在运单号之前输入，先输入包签号后输入拆包地
        obj.insert("feeAmt", ""); //计费金额
        obj.insert("id", ""); //对应数据库id号
        obj.insert("lineNo", ""); //路由线路编码
        obj.insert("modifyOrgCode", ""); //发生数据修改的操作网点
        obj.insert("modifyTerminal", ""); //发生数据修改修改的终端
        obj.insert("modifyTime", ""); //发生数据修改的实际时间yyyy-MM-dd
        obj.insert("modifyUserCode", ""); //发生数据修改的用户编码，例如：00003529（侯建国：此为静安操作网点员工，只可在静安网点使用）
        obj.insert("modifyUserName", ""); //发生数据修改的用户编码，例如：侯建国（侯建国：此为静安操作网点员工，只可在静安网点使用）
        obj.insert("uploadStatu", ""); //更新状态
        obj.insert("getStatus", ""); //状态
        obj.insert("resMessage", ""); //返回消息
        obj.insert("deStationNo", ""); //目的代号
        obj.insert("vehiclePlateNo", ""); //vehiclePlateNo
        obj.insert("uploadTime", ""); //上传时间
        obj.insert("transferStatus", ""); //1：汽转航:0：航转汽，主要用汽航转换等操作，揽收可不予理会
        obj.insert("status", ""); //状态
        obj.insert("refId", ""); //关联id
        obj.insert("opRecords", ""); //记录服务端实体类
        obj.insert("opRecordsOffLine", ""); //离线操作记录-基本上不会用到，可为null
        obj.insert("respcode", ""); //响应记录
        obj.insert("response", ""); //响应请求
        obj.insert("repairCode", ""); //目的网点
        obj.insert("pdaNo", ""); //Pda号
        obj.insert("transportTypeCode", ""); //车签下车没有
        //运单号小车必填
        obj.insert("needArrTruck", "1"); //需要到车
//    }

    return obj;
}

void ArrivedYTO::sendMessage(QByteArray message)
{
    socket->write(message);
    QTextCodec *codec = QTextCodec::codecForName("GBK");
    QString message1 = codec->toUnicode(message);

    emit appendDetailLog("发送到件消息：" + message1);
}

void ArrivedYTO::readSocket()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    QByteArray result = socket->readAll();
    QTextCodec *codec = QTextCodec::codecForName("GBK");
    QString strUnicode = codec->toUnicode(result);

    emit appendDetailLog(QString("圆通到件：返回到件消息：%1").arg(strUnicode));
}

void ArrivedYTO::setContainerNo(QString _containerNo)
{
    containerNo = _containerNo;
}

void ArrivedYTO::socektSuccess()
{
    emit appendDetailLog(QString("圆通到件：网关%1连接成功").arg(socket->peerPort()));
}

void ArrivedYTO::socketError()
{
    emit appendAlarm(QString("圆通到件：网关%1:%2").arg(socket->peerPort()).arg(socket->errorString()));
    emit appendDetailLog(QString("圆通到件：网关%1:%2").arg(socket->peerPort()).arg(socket->errorString()));
}

void ArrivedYTO::createSocketConnection()
{
    socket = new QTcpSocket(this);
    connect(socket, &QTcpSocket::disconnected, this, &ArrivedYTO::autoConnection);
    connect(socket, SIGNAL(readyRead()), this, SLOT(readSocket()));
    connect(socket, SIGNAL(connected()), this, SLOT(socektSuccess()));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socketError()));
    socket->setSocketOption(QAbstractSocket::KeepAliveOption, 1);
    socket->connectToHost(Configure::instance()->configureYTO->ip(),
                          Configure::instance()->configureYTO->portUpload());
}

void ArrivedYTO::autoConnection()
{
    emit appendDetailLog(QString("函数：%1，行数：%2").arg(__func__).arg(__LINE__));
    socket->connectToHost(Configure::instance()->configureYTO->ip(),
                          Configure::instance()->configureYTO->portUpload());
}

