#include "packyto.h"
#include "Configure/configure.h"
#include "define.h"

#include <QSettings>
#include <QSqlError>
#include <QSqlQuery>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonArray>
#include <QTextCodec>
#include <QDateTime>
#include <QFile>
#include <QUuid>

PackYTO::PackYTO(QObject *parent) : QObject(parent)
{

}

QString PackYTO::createUuid()
{
    QUuid id = QUuid::createUuid();
    QString uuid = id.toString()
            .remove('{').remove('}');
    return uuid;
}

void PackYTO::setVaild(bool valid)
{
    isValid = valid;
}

void PackYTO::createDbConnection()
{
    if (QSqlDatabase::contains("ytoPack")) {
        db = QSqlDatabase::database("ytoPack");
    } else {
        db = QSqlDatabase::addDatabase("QMYSQL", "ytoPack");
    }

    db.setHostName(Configure::instance()->nativeip());
    db.setUserName(NATIVE_DB_USERNAME);
    db.setPassword(NATIVE_DB_PASSWORD);
    db.setDatabaseName(NATIVE_DB_DBNAME);
    db.setConnectOptions("MYSQL_OPT_RECONNECT=1");

    if (!db.open()) {
        emit appendDetailLog(db.lastError().text() + "\r\n");
    }
    emit appendDetailLog("圆通集包：数据库连接成功\r\n");

    dbWeight = QSqlDatabase::addDatabase("QMYSQL", "Weight");

    dbWeight.setHostName("192.168.10.210");
    dbWeight.setUserName("root");
    dbWeight.setPassword("YTO_admin");
    dbWeight.setDatabaseName("Ytodb");
    dbWeight.setConnectOptions("MYSQL_OPT_RECONNECT=1");

    if (!dbWeight.open()) {
        qDebug() << dbWeight.lastError().text() << "\r\n";
        emit appendAlarmWindow(QString("数据库连接失败！: %1")
                              .arg(dbWeight.lastError().text()));
    }
    emit appendDetailLog("圆通获取重量数据库连接成功\r\n");
}

void PackYTO::getSignature()
{
    //组装json
    QJsonObject obj;
    obj.insert("mty", YTO_MTY_SIGNATURE);
    obj.insert("clientMac", Configure::instance()->configureYTO->signatureArg().value("clientMac")); //00-50-56-C0-00-01
    obj.insert("clientIp", Configure::instance()->configureYTO->signatureArg().value("clientIp")); //172.31.76.101
    obj.insert("devType", "1");
    obj.insert("userName", Configure::instance()->configureYTO->signatureArg().value("userName"));
    obj.insert("password", Configure::instance()->configureYTO->signatureArg().value("password"));
    obj.insert("cameraMac", Configure::instance()->configureYTO->signatureArg().value("cameraMac"));
    obj.insert("clientId", "1");
    obj.insert("extend1", "");
    obj.insert("extend2", "");
    obj.insert("extend3", "");
    QByteArray json = QJsonDocument(obj).toJson(QJsonDocument::Compact) + "\r\n";
    int length = json.length() - 2; //json长度
    QByteArray str = (QString("%1").arg(length, 8, 10, QLatin1Char('0')) + json).toLatin1();
    emit appendDetailLog("请求签名消息：" + str + "\r\n");
    //socket请求和获取
    QTcpSocket socket;
    socket.connectToHost(Configure::instance()->configureYTO->ip(), Configure::instance()->configureYTO->portUpload());
    socket.waitForConnected();
    socket.write(str);
    socket.waitForReadyRead();
    QByteArray result = socket.readAll();
    QTextCodec *codec = QTextCodec::codecForName("GBK");
    QString strUnicode = codec->toUnicode(result);
    emit appendDetailLog("签名返回消息strUnicode" + strUnicode + "\r\n");
    //解析获取
    QByteArray resultHandle = result.remove(0, 8).trimmed();
    QTextCodec *codecResult = QTextCodec::codecForName("GBK");
    QString resultHandleUnicode = codecResult->toUnicode(resultHandle);
    qDebug() << "截取后的字符串："<< resultHandleUnicode;
    QJsonDocument doc = QJsonDocument::fromJson(resultHandleUnicode.toUtf8());
    QJsonObject dataObj = doc["data"].toObject();
    qDebug() << "dataObj" << dataObj;
    signature = dataObj["signature"].toString();
    emit appendDetailLog("signature:" + signature + "\r\n");
}

QByteArray PackYTO::createMessage(QString waybillNo, QString drop_time,
       QString containerNo, QString chipNo, QString desOrgCode, QString _extend5, QString weight)
{
    QJsonObject obj;

    obj.insert("mty", YTO_MTY_PACK); //PDA接口识别码
    obj.insert("signature", signature); //数字签名-调用设备登录返回签名接口获取数字签名
    obj.insert("chipNo", chipNo);

    QJsonObject objopRecord;
    objopRecord.insert("id", createUuid()); //--------
    objopRecord.insert("containerNo", containerNo); //包签
    objopRecord.insert("desOrgCode", desOrgCode); //拆包地在运单号之前输入，先输入包签号后输入拆包地
    objopRecord.insert("effectiveTypeCode", Configure::instance()->configureYTO->effectiveTypeCode()); //C001：即日达(无效，可不使用)C002：test1C003：国内次日达C004：隔日达testC005：72小时件testC006：同城当天件C007：区域当天件C008：专机件专
    objopRecord.insert("expType", Configure::instance()->configureYTO->expType()); //实物类型-10:运单20:包签30:笼签40:车签
    objopRecord.insert("expressContentCode", Configure::instance()->configureYTO->expressContentCode()); //判断重量0.15以下为文件DOC，0.15以上是PKG
    objopRecord.insert("frequencyNo", Configure::instance()->configureYTO->frequencyNo()); //频次收入1.FC100901012.FC100901023.FC100901034.FC100901095.FC100901106.FC100901507.FC10090151
    objopRecord.insert("inOutDiff", "1");
    objopRecord.insert("waybillNo", waybillNo); //运单号-存放车签
    objopRecord.insert("vehiclePlateNo", Configure::instance()->configureYTO->vehiclePlateNo()); //vehiclePlateNo---"vehiclePlateNo":"T004"
    objopRecord.insert("status", "0"); //状态"status":"0",
    objopRecord.insert("weighWeight", weight); //称入重KG，称入重和输入重必须有一个
    objopRecord.insert("inputWeight", weight); //输入重
    objopRecord.insert("volumeWeight", "0"); //体积重
    objopRecord.insert("createTime", drop_time); //
    objopRecord.insert("opCode", Configure::instance()->configureYTO->opCode()); //操作码-识别各个扫描模块的编码（113）

    objopRecord.insert("extend1", ""); //
    objopRecord.insert("extend2", ""); //
    objopRecord.insert("extend3", ""); //
    objopRecord.insert("extend4", "1"); //
    objopRecord.insert("extend5", _extend5); //
    objopRecord.insert("extend6", ""); //

    objopRecord.insert("number1", ""); //
    objopRecord.insert("number2", ""); //

    obj.insert("opRecord", objopRecord);

    QJsonDocument doc(obj);
    QByteArray json = doc.toJson(QJsonDocument::Compact); //不加QJsonDocument::Compact影响长度的计算
    int length = json.length(); //json长度
    QByteArray jsonLengthStr = QString("%1").arg(length, 8, 10, QLatin1Char('0')).toLatin1();
    QByteArray message = jsonLengthStr + json + "\r\n";

    return message;
}

//从数据库获取数据
void PackYTO::getData(int boxNum, int start, int end, QString containerNo, QTcpSocket* socketPad, QString chipNo)
{
    QString sql = QString(
            "SELECT * FROM %1 "
            "WHERE BOX_NUM = '%2' and Create_int >= %3 and Create_int <= %4 and Company = 'yto'")
            .arg(NATIVE_DB_TOTAL_GO).arg(boxNum).arg(start).arg(end);
    emit appendDetailLog(sql);
    QSqlQuery query(sql, db);

    QStringList expressIdList;

    if (query.size() > 0) {
        //内场pad
        padResponse(socketPad, containerNo, Configure::instance()->configureYTO->nextOrgCode(), chipNo);

        endTime[boxNum-1] = end;
        while (query.next()) {
            QString boxId = query.value("Box_Num").toString();
            QString expressId = query.value("EXpressflow_Id").toString();
            QString drop_time = query.value("Drop_Time").toString().mid(3).replace('-', 10, ' ')  + ".000";
            QByteArray ba = createMessage(expressId, drop_time,
                   containerNo, chipNo, destOrgCodeVector.value(boxId.toInt()),
                   destOrgCodeVector.value(boxId.toInt()), getWeight(expressId));
            socket->write(ba);
            emit appendDetailLog(ba);

            update(expressId, containerNo);

            expressIdList.append(expressId);           
        }
        emit appendDataToGUI(expressIdList, containerNo, QString::number(boxNum), "上传");
    } else {
        //失败补救一次
        int currentTime = static_cast<int>(QDateTime::currentSecsSinceEpoch());
        emit appendDetailLog(QString("失败补救一次：开始时间：%1，结束时间：%2，格口：%3，包牌：%4")
                             .arg(endTime[boxNum-1]).arg(currentTime).arg(boxNum).arg(containerNo));
        QString sql = QString(
                "select * from %1 "
                "where BOX_NUM='%2' and Create_int > %3 and Create_int <= %4 and Company = 'yto'")
                .arg(NATIVE_DB_TOTAL_GO)
                .arg(boxNum).arg(endTime[boxNum-1]).arg(currentTime);
        emit appendDetailLog(sql);

        QSqlQuery query(sql, db);
        if (query.size() > 0) {
            //内场pda
            padResponse(socketPad, containerNo, Configure::instance()->configureYTO->nextOrgCode(), chipNo);
            endTime[boxNum-1] = currentTime;
            while (query.next() > 0) {
                QString boxId = query.value("Box_Num").toString();
                QString expressId = query.value("EXpressflow_Id").toString();
                QString drop_time = query.value("Drop_Time").toString().mid(3).replace('-', 10, ' ') + ".000";


                QByteArray ba = createMessage(expressId, drop_time, containerNo,
                       chipNo, destOrgCodeVector.value(boxId.toInt()),
                       destOrgCodeVector.value(boxId.toInt()), getWeight(expressId));
                socket->write(ba);
                emit appendDetailLog(qPrintable(ba));
                update(expressId, containerNo);
                expressIdList.append(expressId);               
            }
            emit appendDataToGUI(expressIdList, containerNo, QString::number(boxNum), "上传");

        } else { //数据为空
            endTime[boxNum-1] = currentTime; //不成功保留当前时间
        }
    }
}

void PackYTO::padResponse(QTcpSocket *socketPad, QString wayBillNo, QString desOrgCode, QString chipNo)
{
    //00000082{"wayBillNo":"DWB366870813","desOrgCode":"210901","opCode":"188","chipNo":"1321546","extend1":"","extend2":"","extend3":""}
    QJsonObject obj;
    obj.insert("wayBillNo", wayBillNo);
    obj.insert("desOrgCode", desOrgCode);
    obj.insert("opCode", "188");
    obj.insert("chipNo", chipNo);
    obj.insert("extend1", "null");
    obj.insert("extend2", "");
    obj.insert("extend3", "");
    QString json = QJsonDocument(obj).toJson(QJsonDocument::Compact);
    QString length = QString("%1").arg(json.length(), 8, 10, QChar('0'));
    socketPad->write(QString("%1%2\r\n").arg(length, json).toLatin1());
}

void PackYTO::update(QString expressId, QString containerNo)
{
    QString sql = QString(
            "UPDATE %1 SET containerNo = '%2' WHERE Expressflow_Id = '%3'")
            .arg(NATIVE_DB_TOTAL_GO)
            .arg(containerNo).arg(expressId);
    QSqlQuery query(sql, db);

    if (query.isActive())
        emit appendDetailLog(QString("更新数量%1").arg(query.numRowsAffected()));
    else {
        emit appendDetailLog(QString("更新失败！"));
    }
}

QHash<QString, QString> PackYTO::getWeight(QStringList expressList)
{
    QString expresses = QString("%1%2%3").arg("'", expressList.join("','"), "'");

    QString sql = QString
    ("SELECT WAYBILL_NO, WEIGHT FROM Ytodb.`t_exp_waybill_check_0` WHERE  WAYBILL_NO IN (%1) UNION "
    "SELECT WAYBILL_NO, WEIGHT FROM Ytodb.`t_exp_waybill_check_1` WHERE  WAYBILL_NO IN (%1) UNION "
    "SELECT WAYBILL_NO, WEIGHT FROM Ytodb.`t_exp_waybill_check_2` WHERE  WAYBILL_NO IN (%1) UNION "
    "SELECT WAYBILL_NO, WEIGHT FROM Ytodb.`t_exp_waybill_check_3` WHERE  WAYBILL_NO IN (%1) UNION "
    "SELECT WAYBILL_NO, WEIGHT FROM Ytodb.`t_exp_waybill_check_4` WHERE  WAYBILL_NO IN (%1) UNION "
    "SELECT WAYBILL_NO, WEIGHT FROM Ytodb.`t_exp_waybill_check_5` WHERE  WAYBILL_NO IN (%1) UNION "
    "SELECT WAYBILL_NO, WEIGHT FROM Ytodb.`t_exp_waybill_check_6` WHERE  WAYBILL_NO IN (%1) UNION "
    "SELECT WAYBILL_NO, WEIGHT FROM Ytodb.`t_exp_waybill_check_7` WHERE  WAYBILL_NO IN (%1) UNION "
    "SELECT WAYBILL_NO, WEIGHT FROM Ytodb.`t_exp_waybill_check_8` WHERE  WAYBILL_NO IN (%1) UNION "
    "SELECT WAYBILL_NO, WEIGHT FROM Ytodb.`t_exp_waybill_check_9` WHERE  WAYBILL_NO IN (%1)").arg(expresses);
    qDebug() << QString("查询重量语句：%1").arg(sql);

    QSqlQuery query(sql, dbWeight);

    QHash<QString, QString> weightHash;
    while (query.next()) {
        weightHash.insert(query.value(0).toString(), query.value(1).toString());
    }
    return weightHash;
}

QString PackYTO::getWeight(QString expressId)
{
    QString sql = QString("SELECT WEIGHT FROM Ytodb.`t_exp_waybill_check_%1` WHERE  WAYBILL_NO = '%2'")
            .arg(expressId.right(1)).arg(expressId);
    emit appendDetailLog("获取圆通重量：" + sql);
    QSqlQuery query(sql, dbWeight);
    while (query.next()) {
        double weight = query.value(0).toDouble();
        if (weight > 0) {
            return QString::number(weight);
        }
    }
    return QString("0");
}

void PackYTO::init()
{
    log = Log::instance();
    connect(this, &PackYTO::appendDetailLog, log, &Log::appendDetailLog);

    for (size_t i = 0; i < sizeof(endTime) / sizeof (qint64); ++i) {
        endTime[i] = QDateTime::currentSecsSinceEpoch();
    }

    destOrgCodeVector = Configure::instance()->configureYTO->desOrgCode();


    plc = new PLC(this);
#if PLC_ON
    long lErr1 = plc->initAddress();
    if (lErr1) {
        emit appendDetailLog(QString("<font color=red>圆通集包：PLC初始化地址失败，错误代号%1</font>").arg(lErr1));
    }
    long lErr2 = plc->initHandle();
    if (lErr2) {
        emit appendDetailLog(QString("<font color=red>圆通集包：PLC初始化句柄失败，错误代号%1</font>").arg(lErr2));
    }
#endif
    createDbConnection();

    createSocketConnection();

#if STO_SIGNATURE
    getSignature();
#endif

    initServerPda();
}

void PackYTO::initServerPda()
{
    //连接网关
    server = new QTcpServer();
    if (!server->listen(QHostAddress::Any, Configure::instance()->configureYTO->padPort())) {
        emit appendDetailLog("圆通集包：服务监听失败");
    }
    emit appendDetailLog("圆通集包：开始接收网关81003的连接...");
    connect(server, &QTcpServer::newConnection, this, &PackYTO::readServer);
}

void PackYTO::socektSuccess()
{
    errorLabel = false;
    emit appendDetailLog("圆通集包：圆通网关10002连接成功！\r\n");
}

void PackYTO::socketError()
{
    emit appendDetailLog("圆通集包：圆通网关10002连接失败：" +  socket->errorString() + "\r\n");
    if (errorLabel) {
    }
}

void PackYTO::readTcpSocket()
{
    //emit appendDetailLog(QString("函数：%1，行数：%2").arg(__func__).arg(__LINE__));
    QTextCodec *codec = QTextCodec::codecForName("GBK");
    QByteArray ba =  socket->readAll();
    //emit appendDetailLog(ba);
    QString str = codec->toUnicode(ba);

    emit appendDetailLog(str);
}

void PackYTO::createSocketConnection()
{
    socket = new QTcpSocket();
    connect(socket, &QTcpSocket::disconnected, this, &PackYTO::autoConnection);
    connect(socket, SIGNAL(readyRead()), this, SLOT(readTcpSocket()));
    connect(socket, SIGNAL(connected()), this, SLOT(socektSuccess()));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socketError()));
    socket->setSocketOption(QAbstractSocket::KeepAliveOption, 1);
    socket->connectToHost(Configure::instance()->configureYTO->ip(),
                          Configure::instance()->configureYTO->portUpload());
}

void PackYTO::autoConnection()
{
    errorLabel = true;
    socket->connectToHost(Configure::instance()->configureYTO->ip(),
                          Configure::instance()->configureYTO->portUpload());
    emit appendDetailLog("圆通集包：连接断开，网关10002正在自动连接...\r\n");
}

void PackYTO::readServer()
{
    QTcpSocket *socketPad = server->nextPendingConnection();
    emit appendDetailLog(QString("圆通集包：网关8103，%1").arg(socketPad->peerAddress().toString()));

    connect(socketPad, &QAbstractSocket::disconnected,
            socketPad, &QObject::deleteLater);
    connect(socketPad, &QIODevice::readyRead, this, [=]()
    {
        if (!isValid) {
            return ;
        }
        //申通内场pad
        //00000109{"Waybill_NO":"WB2000300426","Lattice":"01","Time":"2017-05-19 10:27:25","Line":"1","Employee_NO":"00003529","chipNo":"46556"}
        QTcpSocket *socketPad = qobject_cast<QTcpSocket *>(sender());

        if (socketPad == nullptr)
            return ;
        QByteArray str = socketPad->readAll().trimmed();
        emit appendDetailLog(QString("收到网关8103数据：%1").arg(QString(str)));
        str.remove(0,8);
        QJsonParseError error;
        QJsonDocument doc = QJsonDocument::fromJson(str, &error);
        if (error.error != QJsonParseError::NoError) {
            emit this->appendDetailLog(error.errorString());
            emit this->appendAlarmWindow(error.errorString());
            return;
        }
        QJsonValue Waybill_NO = doc["Waybill_NO"];
        if (!Waybill_NO.isString()) {
            qDebug() << Waybill_NO;
            return;
        }

        QJsonValue Lattice = doc["Lattice"];
        if (!Lattice.isString()) {
            qDebug() << Lattice;
            return;
        }
        int boxId = Lattice.toString().toInt();
        if (boxId > 400 || boxId <= 0) {
            qDebug() << "格口超出正常范围:" << boxId;
            return ;
        }

        work(boxId, Waybill_NO.toString(), doc["chipNo"].toString(), socketPad);

    });
}

void PackYTO::work(int boxNum, QString containerNo, QString chipNo, QTcpSocket* socketPad)
{
    const int *start = plc->plcGetStartTime();
    const int *end = plc->plcGetEndTime();

    emit appendDetailLog(QString("圆通集包：开始时间：%1，结束时间：%2，格口：%3，包签：%4")
         .arg(start[boxNum-1]).arg(end[boxNum-1]).arg(boxNum).arg(containerNo));

    //getData(boxNum, 100, 105, containerNo, socketPad, chipNo);
    getData(boxNum, start[boxNum-1], end[boxNum-1], containerNo, socketPad, chipNo);
}


