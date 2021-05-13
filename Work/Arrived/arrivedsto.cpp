#include "arrivedsto.h"
#include "Configure/configure.h"

#include <QSettings>
#include <QFile>
#include <QMessageBox>
#include <QSqlError>
#include <QSqlQuery>
#include <QHttpMultiPart>
#include <QHttpPart>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUuid>
#include <QNetworkReply>

ArrivedSTO::ArrivedSTO(QObject *parent) : QObject(parent)
{

}

//发送到件，一件一件发送
void ArrivedSTO::arrived(QString expressId, QString orgCode, QString userCode, QString effectiveType,
                         int boxId, QString time, QString updateFlag,
                         QString updateTime, QString company, QString pdaCode)
{
    //emit appendDetailLog(QString("函数：%1，行数：%2").arg(__func__).arg(__LINE__));
    QJsonObject obj = createJsonObject(expressId, time, effectiveType);
    QString content = createJson(QJsonArray() << obj, orgCode, userCode, pdaCode);
    emit appendArriveLog(content);
    QHttpMultiPart *multiPart = createMultiPart(content);

    //发送
    httpRequestArrived(multiPart);

    //更新已经回传的数据标志为1和时间
    //expressIds.chop(1);
    QString update;
    if (updateFlag == "isArrived") {
        update = QString(
           "UPDATE %1 SET %2 = TRUE WHERE EXPRESSFLOW_ID = '%3'")
                .arg(NATIVE_DB_STO_COME)
                .arg(updateFlag)
                .arg(expressId);
    } else {
        qint64 unixTimeStamp = QDateTime::fromString(time, "yyyy-MM-dd hh:mm:ss").toMSecsSinceEpoch();
        update = QString(
            "UPDATE %1 SET %2 = TRUE, %3 = %4 WHERE EXPRESSFLOW_ID = '%5'")
                .arg(NATIVE_DB_STO_COME)
                .arg(updateFlag).arg(updateTime)
                .arg(unixTimeStamp)
                .arg(expressId);
    }
    emit appendArriveLog("申通到件：更新回传数据标志为1sql: " + update);

    QSqlQuery query1(update, db);
    if (query1.size() > 0) {
        emit appendArriveLog(QString("申通到件：已更新回传标志%1：%2")
             .arg(updateFlag).arg(query1.numRowsAffected()));
    } else {
        emit appendArriveLog(QString("申通到件：更新回传数据标志%1失败").arg(updateFlag));
    }

    //发送到界面
    emit appendArrivedDataToGUI(expressId, company, "上传");
}

void ArrivedSTO::init()
{
    log = Log::instance();
    connect(this, &ArrivedSTO::appendArriveLog, log, &Log::appendArriveLog);

    setSql();
    manager = new QNetworkAccessManager(this);
    createDBConnection();

    timerArrived = new QTimer;
    connect(timerArrived, &QTimer::timeout, this, &ArrivedSTO::timerEventArrived);
    //emit appendDetailLog(QString("函数：%1，行数：%2").arg(__func__).arg(__LINE__));
}

QJsonArray ArrivedSTO::getArray(QSqlQuery query)
{
    if (query.exec()) {
        QJsonArray arry; //每个对象一条快递信息
        while (query.next()) {
            QJsonObject obj1 = createJsonObject(
                    query.value("EXPRESSFLOW_ID").toString(),
                    QDateTime::fromSecsSinceEpoch(query.value("Create_int").toLongLong())
                       .toString("yyyy-MM-dd hh:mm:ss"),
                    Configure::instance()->configureSTO->effectiveTypeVector()[query.value("BOX_NUM").toInt()],
                    STO_GOODSTYPE,
                    Configure::instance()->configureSTO->empCodeVector()[query.value("BOX_NUM").toInt()]);
            arry.append(obj1);
        }
        return arry;

    } else {
        return QJsonArray();
    }
}

QString ArrivedSTO::createUuid()
{
    QUuid id = QUuid::createUuid();
    QString uuid = id.toString()
            .remove('{').remove('}').remove('-');
    return uuid;
}

void ArrivedSTO::httpReplyArrived()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());

    if (reply->error() == QNetworkReply::NoError) {
        QByteArray ba = reply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(ba);
        emit appendArriveLog(ba);
    } else {
        emit appendAlarm("申通到件：" + reply->errorString(), "警告", "申通接口");
        emit appendArriveLog(reply->errorString());
    }

    reply->deleteLater();
}

void ArrivedSTO::setSql()
{
    QString exceptionBoxIdSql;
    QVector<StructCamera> &cameras = Configure::instance()->camerasVector();
    for (int i = 0; i < cameras.size(); ++i) {
        exceptionBoxIdSql += QString("'%1',").arg(QString::number(cameras[i].exceptionBoxId));
    }
    exceptionBoxIdSql.chop(1);
    sql = QString(
            "SELECT * FROM express_inflow_st "
            "WHERE isArrived IS NULL OR isUrbanArrived = FALSE OR isCellArrived is FALSE "
            "AND box_num NOT IN (%1)")
            .arg(exceptionBoxIdSql); //加上异常件可以适应异常口没有分表的情况
    emit appendArriveLog("申通到件：获取未到件快递的sql:" + sql);
}

//到件
const QJsonObject ArrivedSTO::createJsonObject(QString waybillNo,
                                               QString time,
                                               QString effectiveType)
{
    QJsonObject objContent;
    objContent.insert("uuid", createUuid());
    objContent.insert("waybillNo", waybillNo);
    objContent.insert("expType", "10");
    objContent.insert("opCode", "520");
    objContent.insert("effectiveType", effectiveType);
    objContent.insert("goodsType", STO_GOODSTYPE); //
    objContent.insert("opTime", time);
    return objContent;
}

//派件
const QJsonObject ArrivedSTO::createJsonObject(QString waybillNo,
                                               QString time,
                                               QString effectiveType,
                                               QString goodsType,
                                               QString empCode)
{
    QJsonObject objContent;
    objContent.insert("uuid", createUuid());
    objContent.insert("waybillNo", waybillNo);
    objContent.insert("expType", "10");
    objContent.insert("opCode", "710");
    objContent.insert("effectiveType", effectiveType);
    objContent.insert("goodsType", goodsType); //
    objContent.insert("empCode", empCode);
    objContent.insert("opTime", time);
    return objContent;
}

QString ArrivedSTO::createJson(QJsonArray records, QString orgCode, QString userCode, QString pdaCode)
{
     QJsonObject obj;
     obj.insert("pdaCode", pdaCode); //申通ZJHZLX310006625 //测试171100020110237
     obj.insert("opTerminal", pdaCode); //申通ZJHZLX310006625 //测试171100020110237
     obj.insert("clientProgramRole", STO_CLIENTPROGRAMROLE);
     obj.insert("deviceType", STO_DEVICETYPE);
     obj.insert("orgCode", orgCode); //申通310006 //测试900000
     obj.insert("userCode", userCode); //申通3100060212 //测试900000100
     obj.insert("records", records);

     QJsonDocument doc(obj);
     return doc.toJson(QJsonDocument::Compact);
}

QString ArrivedSTO::calculateDigest(QString content)
{
    QString text = content + STO_SECRETKEY;
    QByteArray ba;
    QCryptographicHash md(QCryptographicHash::Md5);
    ba.append(text);
    md.addData(ba);
    QByteArray bb = md.result();
    QString encryptStr = bb.toBase64().replace("\r\n", ""); //base64加密，去除回车换行

    return encryptStr;
}

QHttpMultiPart* ArrivedSTO::createMultiPart(QString content)
{
    //报文签名，网关会统一校验，下文有详细说明 //安全Key
    QString data_digest = calculateDigest(content);

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

    return multiPart;
}

void ArrivedSTO::httpRequestArrived(QHttpMultiPart *multiPart1)
{
    QNetworkRequest request(QUrl(STO_URL));

    QNetworkReply *reply = manager->post(request, multiPart1);
    connect(reply, &QNetworkReply::finished,
            this, &ArrivedSTO::httpReplyArrived);
}

void ArrivedSTO::timerEventArrived()
{
    QSqlQuery query(sql, db);
    //emit appendDetailLog(QString("函数：%1，行数：%2，sql：%3")
        //.arg(__func__).arg(__LINE__).arg(sql));
        //QHash<QString, QJsonArray> arryHash; //每个对象一条快递信息
        //QJsonArray array;
        //QHash<QString, QString> expressIdHash; //
        //QString expressIds;

        while (query.next()) {
            QString boxIdStr = query.value("BOX_NUM").toString();
            int boxId = boxIdStr.toInt();
            QString expressId = query.value("EXPRESSFLOW_ID").toString();
            QString pdaCode = query.value("pdaCode").toString();
            QString code2 = query.value("code2").toString();
            //到件
            bool isArrived = query.value("isArrived").toBool();
            QString timeZhuJiArrived = query.value("CREATE_TIME").toString();
            if (!isArrived) {
                QString orgCode = Configure::instance()->configureSTO->orgCode();
                QString userCode = Configure::instance()->configureSTO->stoArrivedUserCodeVector().value(boxId);
                QString effectiveType = Configure::instance()->configureSTO->effectiveTypeVector().value(boxId);
                arrived(expressId, orgCode, userCode, effectiveType, boxId,
                        timeZhuJiArrived, "isArrived", "", "唐山到件", pdaCode);
            }

            //QString Code2 = query.value("Code2").toString();
            //qDebug() << "Code2" << Code2 << "\r\n";
            //arryHash[Code2].append(obj);
            //array.append(obj);
            //qDebug() << "arry" << arry;
//            expressIdHash[Code2]
//                    .append("'" + query.value("EXPRESSFLOW_ID").toString() + "',");
            //expressIds += "'" + query.value("EXPRESSFLOW_ID").toString() + "',";

            //发送到件，一件一件发送

            //更新已经回传的数据标志为1
            //expressIds.chop(1);
        }



        //准备好各自的arry后，开始独立发送
//        for (int i=0; i<code2ToOrgCodeKeys.size(); ++i) {
//            QString key = code2ToOrgCodeKeys[i];
//            qDebug() << "key" << key << "\r\n";
//            if (arryHash[key].isEmpty()) {
//                continue;
//            }

//            //发送到件
//            QString content = createJson(arryHash[key],
//                                         code2ToOrgCodeObj[key].toString(),
//                                         code2ToUserCodeObj[key].toString());
//            ui->textEditArrived->append(content);
//            QHttpMultiPart *multiPart = createMultiPart(content);
//            httpRequestArrived(multiPart);
//            //更新已经回传的数据标志为1
//            expressIdHash[key].chop(1);
//            QString sql1 = QString("update express_plc.express_inflow_st "
//               "set isArrived = true where EXPRESSFLOW_ID IN(%1)").arg(expressIdHash[key]);
//            ui->textEditArrived->append("更新回传数据标志为1: " + sql1);
//            QSqlQuery query1(db);
//            if (query1.exec(sql1)) {
//                //ui->textEditArrived->append(QString("已更新回传数据大小%1").arg(query1.numRowsAffected()));
//            } else {
//                ui->textEditArrived->append("更新回传数据标志失败");
//            }
//        }

}

void ArrivedSTO::setIsDb(bool value)
{
    isDb = value;
}

void ArrivedSTO::startOrStopTimer(bool isStart)
{
    if (isStart) {
//        if (Configure::instance()->configureSTO->arrivedSource() == 1) {
            timerArrived->start(Configure::instance()->configureSTO->intervalArrived());
            emit appendArriveLog("申通到件：开始到件");
//        }
    } else {
        timerArrived->stop();
        emit appendArriveLog("申通到件：停止到件");
    }
}

void ArrivedSTO::createDBConnection()
{
    db = QSqlDatabase::addDatabase("QMYSQL", "ArrivedSto");

    db.setHostName(Configure::instance()->nativeip());
    db.setUserName(NATIVE_DB_USERNAME);
    db.setPassword(NATIVE_DB_PASSWORD);
    db.setDatabaseName(NATIVE_DB_DBNAME);
    db.setConnectOptions("MYSQL_OPT_RECONNECT=1");

    if (!db.open()) {
        emit appendArriveLog(QString("申通到件：数据库打开失败: %1").arg(db.lastError().text()));
        emit appendAlarm(QString("申通到件：数据库打开失败: %1").arg(db.lastError().text()), "严重", "本地数据库");
    } else {
        emit appendArriveLog(QString("申通到件：数据库连接成功"));
    }
}

