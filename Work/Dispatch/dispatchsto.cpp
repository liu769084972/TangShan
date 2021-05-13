#include "dispatchsto.h"
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

DispatchSTO::DispatchSTO(QObject *parent) : QObject(parent)
{

}

QByteArray DispatchSTO::createContent(QString orgCode, QString userCode,
              QString waybillNo, QString nextOrgCode, QString opTime, QString pdaCode)
{
    QJsonObject jsonObject;
    jsonObject.insert("pdaCode", pdaCode);
    jsonObject.insert("opTerminal", pdaCode);
    jsonObject.insert("clientProgramRole", STO_CLIENTPROGRAMROLE);
    jsonObject.insert("deviceType", STO_DEVICETYPE);
    jsonObject.insert("orgCode", orgCode); //------待办，不同公司不同，由配置文件传入
    jsonObject.insert("userCode", userCode); //

    QJsonArray jsonArray;
    QJsonObject jsonObject1;
    jsonObject1.insert("uuid", uuId()); //
    jsonObject1.insert("waybillNo", waybillNo); //
    jsonObject1.insert("expType", STO_EXPTYPE); //
    jsonObject1.insert("opCode", STO_DISPATCH_OPCODE); //
    jsonObject1.insert("nextOrgCode", nextOrgCode);
    jsonObject1.insert("opTime", opTime); //
    jsonArray.append(jsonObject1);

    jsonObject.insert("records", jsonArray);

    QJsonDocument doc(jsonObject);

    return doc.toJson(QJsonDocument::Compact);
}

void DispatchSTO::init()
{    
    log = Log::instance();
    connect(this, &DispatchSTO::appendDispatchLog, log, &Log::appendDispatchLog);
    setSql();
    createDBConnection();
    manager = new QNetworkAccessManager(this);
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &DispatchSTO::timerEventDispatch);
}

void DispatchSTO::timerEventDispatch()
{
    QSqlQuery query(sql, db);
    //emit appendDetailLog(QString("函数：%1，行数：%2，sql：%3").arg(__func__).arg(__LINE__).arg(sql));
    //QString text = query.lastError().text();

    while (query.next()) {
        QString boxIdStr = query.value("BOX_NUM").toString();
        short boxId = boxIdStr.toShort();
        QString expressId = query.value("EXPRESSFLOW_ID").toString();
        QString code2 = query.value("code2").toString();

        //发件
        bool isDispatch = query.value("isDispatch").toBool();
        qint64 create_int = query.value("Create_int").toLongLong();
        QDateTime dateTime = QDateTime::fromSecsSinceEpoch(create_int);
        QString timeDispatch = dateTime.toString("yyyy-MM-dd hh:mm:ss");
        qDebug() << "isDispacht" << isDispatch << "操作时间" << timeDispatch;
        if (!isDispatch) {
            QString orgCode = Configure::instance()->configureSTO->stoOrgCodeHash().value(boxId);
            QString userCode = Configure::instance()->configureSTO->stoDispatchUserCodeVector().value(boxId);
            QString nextOrgCode = Configure::instance()->configureSTO->stoDispatchNextOrgCodeHash().value(boxId);
            //特殊的口，根据二段码的不同发送不同的下一站网点，以后为了方便可以做成配置
            if (boxId == 1 && code2 == "WX01") {
                nextOrgCode = "063001";
            } else if (boxId == 1 && code2 == "WX10") {
                nextOrgCode = "063610";
            }
            QString pdaCode = query.value("pdaCode").toString();
            dispatch(expressId, orgCode, userCode, nextOrgCode, boxId,
                     timeDispatch, "isDispatch", "Create_int", "申通发件", pdaCode);
        }
    }
}

void DispatchSTO::startOrStopTimer(bool isStart)
{
    if (isStart) {
        int interval = Configure::instance()->configureSTO->intervalDispatch();
        timer->start(interval);
        emit appendDispatchLog("申通发件:发件开始");
    } else {
        timer->stop();
        emit appendDispatchLog("申通发件：发件停止");
    }
}

QString DispatchSTO::uuId()
{
    QUuid id = QUuid::createUuid();
    QString uuid = id.toString()
            .remove('{').remove('}').remove('-');
    return uuid;
}

void DispatchSTO::createDBConnection()
{
    if (QSqlDatabase::contains("StoDispatch")) {
        db = QSqlDatabase::addDatabase("StoDispatch");
    } else {
        db = QSqlDatabase::addDatabase("QMYSQL", "StoDispatch");
    }

    db.setHostName(Configure::instance()->nativeip());
    db.setUserName(NATIVE_DB_USERNAME);
    db.setPassword(NATIVE_DB_PASSWORD);
    db.setDatabaseName(NATIVE_DB_DBNAME);
    db.setConnectOptions("MYSQL_OPT_RECONNECT=1");

    if (!db.open()) {
        emit appendDispatchLog(QString("申通发件：数据库打开失败: %1").arg(db.lastError().text()));
        emit appendAlarm(QString("申通发件：数据库打开失败: %1").arg(db.lastError().text()), "严重", "本地数据库");
    } else {
        emit appendDispatchLog(QString("申通发件：数据库连接成功"));
    }
}

void DispatchSTO::setSql()
{
    QString exceptionBoxIdSql;
    QVector<StructCamera> &cameras = Configure::instance()->camerasVector();
    for (int i = 0; i < cameras.size(); ++i) {
        exceptionBoxIdSql += QString("'%1',").arg(QString::number(cameras[i].exceptionBoxId));
    }

    exceptionBoxIdSql.chop(1);
    sql = QString(
            "SELECT * FROM %1 "
            "WHERE isDispatch IS FALSE AND Create_int IS NOT NULL AND box_num NOT IN (%2)")
            .arg(NATIVE_DB_STO_COME).arg(exceptionBoxIdSql);
    emit appendDispatchLog("申通发件：sql:" + sql);
}

void DispatchSTO::dispatch(QString expressId, QString orgCode, QString userCode,
                           QString nextOrgCode, int boxId, QString time,
                           QString updateFlag, QString updateTimeFlag, QString company, QString pdaCode)
{
    QString content = createContent(orgCode, userCode, expressId, nextOrgCode, time, pdaCode);
    emit appendDispatchLog(content);

    httpReqeust(content);

    //更新标志位和时间
    QString update;
    if (updateFlag == "isDispatch") {
        update = QString("UPDATE %1 "
           "SET isDispatch = TRUE WHERE EXPRESSFLOW_ID = '%2'")
                .arg(NATIVE_DB_STO_COME).arg(expressId);
    }

    emit appendDispatchLog(QString("申通发件：更新回传标志%1为1: %2").arg(updateFlag).arg(update));

    QSqlQuery query1(update, db);
    if (query1.isActive() > 0) {
        emit appendDispatchLog(QString("申通发件：已更新回传数据大小%1").arg(query1.numRowsAffected()));
    } else {
        emit appendDispatchLog("申通发件：更新回传数据标志失败");
    }

    //发送到界面
    emit appendDispatchDataToGUI(QStringList() << expressId, company, "上传");
}

QJsonObject DispatchSTO::createJsonObject(QString waybillNo, QString time, QString effectiveType)
{

}

void DispatchSTO::httpReqeust(QString content)
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

    QNetworkRequest request(QUrl(STO_URL));

    QNetworkReply *reply = manager->post(request, multiPart);
    connect(reply, &QNetworkReply::finished,
            this, &DispatchSTO::httpRead);
}

QString DispatchSTO::calculateDigest(QString content)
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

void DispatchSTO::httpRead()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());

    if (reply->error() == QNetworkReply::NoError) {
        QByteArray ba = reply->readAll();
        QJsonDocument doc = QJsonDocument::fromJson(ba);
        emit appendDispatchLog(ba);
    } else {
        emit appendAlarm("申通发件：" + reply->errorString(), "警告", "申通接口");
        emit appendDispatchLog(reply->errorString());
    }

    reply->deleteLater();
}
