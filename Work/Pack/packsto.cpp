#include "packsto.h"
#include "Configure/configure.h"
#include "define.h"

#include <QSettings>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUuid>
#include <QSqlError>
#include <QMessageBox>
#include <QHttpMultiPart>
#include <QHttpPart>
#include <QTextCodec>
#include <QMessageAuthenticationCode>

PackSTO::PackSTO(QObject *parent) : QObject(parent)
{

}

void PackSTO::init()
{
    log = Log::instance();
    connect(this, &PackSTO::appendDetailLog, log, &Log::appendDetailLog);

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

    for (size_t i = 0; i < sizeof(endTime) / sizeof (qint64); ++i) {
        endTime[i] = QDateTime::currentSecsSinceEpoch();
    }

    manager = new QNetworkAccessManager();

    //数据库
    createDbConnection();

    //把枪
    createServer();
}

void PackSTO::httpRequest(QSqlQuery query, QString containerNo, QString boxId,
                          QString effectiveType, QString userCode)
{
    QString content = createJson(query, containerNo, boxId, effectiveType, userCode); //具体的请求参数报文
    appendDetailLog(content);
    QString data_digest = calculateDigest(content); //报文签名，网关会统一校验，下文有详细说明 //安全Key

    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    QHttpPart api_namePart;
    api_namePart.setHeader(QNetworkRequest::ContentDispositionHeader,
                           QVariant("form-data; name=\"api_name\""));
    api_namePart.setBody(STO_API_NAME);

    QHttpPart from_appkeyPart;
    from_appkeyPart.setHeader(QNetworkRequest::ContentDispositionHeader,
                              QVariant("form-data; name=\"from_appkey\""));
    from_appkeyPart.setBody(STO_FROM_APPKEY);

    QHttpPart from_codePart;
    from_codePart.setHeader(QNetworkRequest::ContentDispositionHeader,
                            QVariant("form-data; name=\"from_code\""));
    from_codePart.setBody(STO_FROM_CODE);

    QHttpPart to_appkeyPart;
    to_appkeyPart.setHeader(QNetworkRequest::ContentDispositionHeader,
                            QVariant("form-data; name=\"to_appkey\""));
    to_appkeyPart.setBody(STO_TO_APPKEY);

    QHttpPart to_codePart;
    to_codePart.setHeader(QNetworkRequest::ContentDispositionHeader,
                          QVariant("form-data; name=\"to_code\""));
    to_codePart.setBody(STO_TO_CODE);

    QHttpPart data_digestPart;
    data_digestPart.setHeader(QNetworkRequest::ContentDispositionHeader,
                              QVariant("form-data; name=\"data_digest\""));
    data_digestPart.setBody(data_digest.toUtf8());

    QHttpPart contentPart;
    contentPart.setHeader(QNetworkRequest::ContentDispositionHeader,
                          QVariant("form-data; name=\"content\""));
    contentPart.setBody(content.toUtf8());

    multiPart->append(api_namePart);
    multiPart->append(from_appkeyPart);
    multiPart->append(from_codePart);
    multiPart->append(to_appkeyPart);
    multiPart->append(to_codePart);
    multiPart->append(data_digestPart);
    multiPart->append(contentPart);

    QNetworkRequest request(QUrl(STO_URL));

    QNetworkReply *reply = manager->post(request, multiPart);
    connect(reply, &QNetworkReply::finished, this, &PackSTO::httpReply);
}

void PackSTO::httpReply()
{
    QNetworkReply *reply =  qobject_cast<QNetworkReply*>(sender());
    if (!reply) {
        emit appendAlarm("圆通集包reply为空");
        emit appendDetailLog("圆通集包reply为空");
        return;
    }

    if (reply->error() == QNetworkReply::NoError) {
        QByteArray ba = reply->readAll();
        emit appendDetailLog(ba);
        QJsonDocument doc = QJsonDocument::fromJson(ba);
        QJsonObject obj = doc.object();

        //对部分成功进行处理
        if (obj["errorCode"] == "000") {
            QJsonArray arry = obj["data"].toArray();
            for (int i=0; i < arry.size(); ++i) {
                QJsonObject obj1 = arry[i].toObject();
                QString waybillNo = obj1["waybillNo"].toString();
                QString errorDescription = obj1["errorDescription"].toString();
                QString sql(QString("UPDATE %1 SET failReason = '%2' WHERE EXPRESSFLOW_ID = '%3'")
                            .arg(NATIVE_DB_TOTAL_GO).arg(errorDescription).arg(waybillNo));
                QSqlQuery(sql, db);
            }
        }

    } else {
        emit appendAlarm(reply->errorString());
        emit appendDetailLog(reply->errorString());
    }

    reply->deleteLater();
}
//读取把枪
void PackSTO::readServer()
{
    QTcpSocket *socket = server->nextPendingConnection();
    if (!socket) {
        emit appendAlarm("申通集包：socket为空");
        return;
    }
    connect(socket, &QIODevice::readyRead, this, &PackSTO::readSocket);
    connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);
    connect(socket, &QTcpSocket::disconnected, this, [=](){
        emit this->appendAlarm("申通集包：断开连接");
    });
    emit appendDetailLog("申通集包：把枪连接:" + socket->peerAddress().toString());
}

void PackSTO::work(QString boxNum, QString containerNo, QTcpSocket* socket, QByteArray CB)
{
    const int *start = plc->plcGetStartTime();
    const int *end = plc->plcGetEndTime();

    int i_boxNum = boxNum.toInt();
    if (i_boxNum < 1 || i_boxNum > 320) {
        emit appendAlarm(QString("格口%1超出范围").arg(i_boxNum));
        socket->write(CB + ":02"); //02表示格口超出范围
        socket->flush();
        emit appendAlarm("回复把枪" + CB + ":02");
        return;
    }

    emit appendDetailLog(QString("开始时间：%1，结束时间：%2，格口：%3，包牌：%4")
         .arg(start[i_boxNum-1]).arg(end[i_boxNum-1]).arg(boxNum).arg(containerNo));

    {
        QSqlQuery query = getData(i_boxNum, start[i_boxNum-1], end[i_boxNum-1]);

        //根据结果给把枪返回不同结果
        if (query.size() > 0) {
            socket->write(CB + ":00");
            socket->flush();
            emit appendDetailLog(CB + ":00");
        } else {
            //失败补救一次
            int currentTime = static_cast<int>(QDateTime::currentSecsSinceEpoch());
            emit appendDetailLog(QString("失败补救一次：开始时间：%1，结束时间：%2，格口：%3，包牌：%4")
                    .arg(endTime[i_boxNum-1]+1).arg(currentTime).arg(boxNum).arg(containerNo));
            query = getData(i_boxNum, endTime[i_boxNum-1]+1, currentTime);
            if (query.size() > 0) {
                socket->write(CB + ":00");
                socket->flush();
                emit appendDetailLog(CB + ":00");
                endTime[i_boxNum-1] = currentTime; //成功结束时间保留
            } else {
                socket->write(CB + ":01"); //01表示空
                socket->flush();
                emit appendAlarm(CB + ":01");
                endTime[i_boxNum-1] = currentTime; //不成功保留当前时间
            }
        }

        httpRequest(query, containerNo, boxNum,
                    Configure::instance()->configureSTO->effectiveTypeVector().value(i_boxNum),
                    Configure::instance()->configureSTO->stoPackUserCodeVector().value(i_boxNum));
    }
}

void PackSTO::readSocket()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
    if (!isValid) { //没有开启
        socket->write("CB-1:集包功能未开启"); //03获取的数据格式不对
        socket->flush();
        emit appendAlarm(QString("申通集包：回复把枪:%1").arg("CB-1:03"));
        return;
    }
    if (!socket) {
        emit appendAlarm("socket读为空");
        return;
    }
    //把枪数据：CB-1:888555566666333,119
    QByteArray str = socket->readAll();
    if (str.length() < 10) {
        emit appendAlarm("从把枪获取的数据不正常");
        socket->write("CB-1:03"); //03获取的数据格式不对
        socket->flush();
        emit appendAlarm(QString("回复把枪:%1").arg("CB-1:03"));
    }
    str.remove(0,1).chop(1); //去掉开头的02H和结尾03H
    QList<QByteArray> list =  str.split(',');
    if (list.size() < 2) {
        emit appendAlarm("从把枪获取的数据不正常");
        emit appendDetailLog("从把枪获取的数据不正常");
        socket->write("CB-1:03"); //03获取的数据格式不对
        socket->flush();
        emit appendAlarm(QString("回复把枪:%1").arg("CB-1:03"));
        emit appendDetailLog(QString("回复把枪:%1").arg("CB-1:03"));
        return;
    }
    QList<QByteArray> list1 = list[0].split(':');
    if (list1.size() < 2) {
        emit appendAlarm("圆通集包：从把枪获取的数据不正常");
        emit appendDetailLog("圆通集包：从把枪获取的数据不正常");
        socket->write("CB-1:03"); //03获取的数据格式不对
        socket->flush();
        emit appendAlarm(QString("回复把枪:%1").arg("CB-1:03"));
        emit appendDetailLog(QString("回复把枪:%1").arg("CB-1:03"));
        return;
    }

    work(list[1], list1[1], socket, QByteArray("ACK-" + list1[0]));
}

void PackSTO::setValid(bool _valid)
{
    isValid = _valid;
}

QString PackSTO::createJson(QSqlQuery query, QString containerNo, QString boxId,
                            QString effectiveType, QString userCode)
{
     emit appendDetailLog(QString("获取数据大小%1").arg(query.size()));

     QJsonObject obj;
     obj.insert("pdaCode", Configure::instance()->configureSTO->pdaCode());
     obj.insert("opTerminal", Configure::instance()->configureSTO->opTerminal());
     obj.insert("clientProgramRole",  "0");
     obj.insert("deviceType", "ZDFJ");
     obj.insert("orgCode", Configure::instance()->configureSTO->orgCode());
     obj.insert("userCode", userCode);
     QJsonArray arry;
     QStringList expressIdList;
     while (query.next()) {
         QString expressId = query.value("EXPRESSFLOW_ID").toString();
         QString dateTime = QDateTime::fromSecsSinceEpoch(query.value("Create_int").toLongLong())
                 .toString("yyyy-MM-dd hh:mm:ss");
/*         if (boxIdForSpeciallyNextOrgCode.contains(query.value("BOX_NUM").toString())) {
             QJsonObject obj1 = createJsonObject(
                    expressId,
                    dateTime,
                    speciallyNextOrgCode, containerNo, effectiveType);
             arry.append(obj1);
         } else */{
             QJsonObject obj1 = createJsonObject(
                    expressId,
                    dateTime,
                    Configure::instance()->configureSTO->nextOrgCode(), containerNo, effectiveType);
             arry.append(obj1);
         }
         expressIdList.append(expressId);
     }
     obj.insert("records", arry);
     QJsonDocument doc(obj);

     //更新集包标志为1
     if (!expressIdList.isEmpty()) {
         QString expressIds = "'" + expressIdList.join("','") + "'";
         QString sql = QString("UPDATE %1 SET isJiBao=1, containerNo='%2' "
                               "WHERE EXPRESSFLOW_ID IN(%3)")
                 .arg(NATIVE_DB_STO_GO).arg(containerNo).arg(expressIds);
         emit appendDetailLog(sql);

         QSqlQuery update(sql);
         if (update.isActive()) {
             emit appendDetailLog(QString("更新大小%1").arg(update.numRowsAffected()));
         } else {
             emit appendAlarm("申通集包：更新失败");
             emit appendDetailLog("申通集包：更新失败");
         }
     }

     //发送到界面
     emit appendPackDataToGUI(expressIdList, containerNo, boxId, "上传");

     return doc.toJson(QJsonDocument::Compact);
}

/**
* 计算签名值（MD5 + Base64）
*
* @param content 报文体
* @param secretkey 应用上配置的私钥
* @return
**/
QString PackSTO::calculateDigest(QString content)
{
    QString text = content + STO_SECRETKEY;
    QByteArray ba;
    QCryptographicHash md(QCryptographicHash::Md5);
    //QCryptographicHash md(QCryptographicHash::Sha256); //错误
    ba.append(text);
    md.addData(ba);
    //QByteArray bb = md.result().toHex(); //32位md5加密，错误
    QByteArray bb = md.result();
    QString encryptStr = bb.toBase64().replace("\r\n", ""); //base64加密，去除回车换行
    //qDebug() << "加密字符串" << encryptStr << "\r\n";

    return encryptStr;
}

QString PackSTO::createUuid()
{
    QUuid id = QUuid::createUuid();
    QString uuid = id.toString()
            .remove('{').remove('}').remove('-');
    return uuid;
}

bool PackSTO::createDbConnection()
{
    db = QSqlDatabase::addDatabase("QMYSQL", "stoPack");
    db.setHostName(Configure::instance()->nativeip());
    db.setUserName(NATIVE_DB_USERNAME);
    db.setPassword(NATIVE_DB_PASSWORD);
    db.setDatabaseName(NATIVE_DB_DBNAME);
    db.setConnectOptions("MYSQL_OPT_RECONNECT=1");

    if (!db.open()) {
        emit appendAlarm(QString("申通集包：不能打开本地数据库: %1")
                .arg(db.lastError().text()));
        emit appendDetailLog(QString("申通集包：不能打开本地数据库: %1")
                             .arg(db.lastError().text()));
        return false;
    }
    return true;
}

const QJsonObject PackSTO::createJsonObject(QString waybillNo, QString time,
                                               QString nextOrgCode, QString containerNo,
                                               QString effectiveType)
{
    QJsonObject objContent;
    objContent.insert("uuid", createUuid());
    objContent.insert("waybillNo", waybillNo);
    objContent.insert("expType", "10");
    objContent.insert("opCode", "221");
    objContent.insert("effectiveType", effectiveType);
    objContent.insert("nextOrgCode", nextOrgCode);
    objContent.insert("containerNo", containerNo); //
    //objContent.insert("weight", "0"); //非必填
    objContent.insert("opTime", time);
    return objContent;
}

//从数据库获取数据
QSqlQuery PackSTO::getData(int boxNum, qint64 start, qint64 end)
{
#if STO_TEST
    QString sql = QString("SELECT * FROM %1 "
            "WHERE BOX_NUM = '%2' AND Create_int >= %3 and Create_int <= %4")
            .arg(NATIVE_DB_STO_GO).arg(boxNum).arg(50).arg(100);
#else
    QString sql = QString("SELECT * FROM %1 "
            "WHERE BOX_NUM = '%2' and Create_int >= %3 AND Create_int <= %4")
            .arg(NATIVE_DB_STO_GO).arg(boxNum).arg(start).arg(end);

#endif

    emit appendDetailLog("申通集包：" + sql);
    return QSqlQuery(sql, db);
}

void PackSTO::createServer()
{
    server = new QTcpServer(this);
    quint16 port = Configure::instance()->configureSTO->pdaPort();
    if (!server->listen(QHostAddress::Any, port)) {
        emit appendAlarm(QString("申通集包：把枪服务创建失败：%1").arg(server->errorString()));
        emit appendDetailLog(QString("申通集包：把枪服务创建失败：%1").arg(server->errorString()));
        return;
    }
    connect(server, &QTcpServer::newConnection, this, &PackSTO::readServer);
}

