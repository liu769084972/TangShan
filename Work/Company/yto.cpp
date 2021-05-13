#include "yto.h"
#include "Configure/configure.h"
#include "Work/Sort/express.h"
#include "define.h"

#include <QJsonObject>
#include <QJsonDocument>
#include <QDebug>
#include <QTextCodec>
#include <QDateTime>

YTO::YTO(bool _comeOrGo, Express *_express, QObject *parent)
    : QObject(parent),
      express(_express),
      comeOrGo(_comeOrGo)
{
    log = Log::instance();
    connect(this, &YTO::appendRunLog, log, &Log::appendRunLog);
    connect(this, &YTO::appendDetailLog, log, &Log::appendDetailLog);
    connect(this, &YTO::appendAlarm, express->getMainWindow(), &MainWindow::appendAlarm);

    createYTOServerConnection();

#if STO_SIGNATURE
    getSignature();
#endif

    //到件
    QBitArray baCome = Configure::instance()->companySwitchCome();
    if (comeOrGo && baCome[Yto]) {
        arrivedYto = new ArrivedYTO();
        threadArrivd = new QThread(this);
        arrivedYto->moveToThread(threadArrivd);
        connect(arrivedYto, &ArrivedYTO::appendArrivedDataToGUI,
                express->getMainWindow(), &MainWindow::showArrivedData);
        connect(express->getMainWindow(), &MainWindow::containerNoSignal,
                arrivedYto, &ArrivedYTO::setContainerNo);
        connect(express->getMainWindow(), &MainWindow::arrivedSignal,
                arrivedYto, &ArrivedYTO::startOrStopTimer);
        connect(this, &YTO::arrivedSignal, arrivedYto, &ArrivedYTO::arrived);
        threadArrivd->start();
        QMetaObject::invokeMethod(arrivedYto, &ArrivedYTO::init, Qt::QueuedConnection);
    }

    //集包
    QBitArray ba = Configure::instance()->companySwitchGo();
    if (!comeOrGo && ba[Yto]) {
        threadPack = new QThread(this);
        packYto = new PackYTO(nullptr);
        packYto->moveToThread(threadPack);
        connect(packYto, &PackYTO::appendDataToGUI,
                express->getMainWindow(), &MainWindow::showPackSendData);
        connect(express->getMainWindow(), &MainWindow::sendOrPackSignal,
                packYto, &PackYTO::setVaild);

        threadPack->start();
        QMetaObject::invokeMethod(packYto, &PackYTO::init, Qt::QueuedConnection);
    }
}

void YTO::getBoxId(QString expressId)
{
    QJsonObject obj;
    obj.insert("aviType", "0");
    obj.insert("trace", "0");
    obj.insert("isFirstTransfer", "1");
    obj.insert("mty", YTO_MTY_SORT);
    obj.insert("respcode", QJsonValue());
    obj.insert("signature", signature); //获取签名，需要代码获取，没有就QJsonValue()
    QJsonObject objChild;
    objChild.insert("createTime", QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz"));
    objChild.insert("createUserCode", Configure::instance()->configureYTO->createUserCode());
    objChild.insert("id", QJsonValue());
    objChild.insert("waybillNo", expressId);
    objChild.insert("opCode", YTO_OPCODE_SORT);
    obj.insert("opRecord", objChild);
    QString json = QJsonDocument(obj).toJson(QJsonDocument::Compact);
    int length = json.length();
    QByteArray str = QString("%1%2\r\n").arg(length, 8, 10, QLatin1Char('0')).arg(json).toLatin1();
    emit appendDetailLog(str);

    //socket请求和获取
    if (!tcpSocket) {
        emit appendDetailLog("获取圆通格口时socket为空\r\n");
        return;
    }
    tcpSocket->write(str);
}

void YTO::createYTOServerConnection()
{
    if (!tcpSocket) {
        tcpSocket = new QTcpSocket();
        connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(readTcpSocket()));
        connect(tcpSocket, SIGNAL(connected()), this, SLOT(tcpSocektSuccess()));
        connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
                this, SLOT(tcpSocketError()));
        connect(tcpSocket, SIGNAL(disconnected()), this, SLOT(autoConnectServer()));
        tcpSocket->setSocketOption(QAbstractSocket::KeepAliveOption, 1);
    }

    QString ip = Configure::instance()->configureYTO->ip();
    quint16 portCode = Configure::instance()->configureYTO->portCode();
    tcpSocket->connectToHost(ip, portCode);
}

void YTO::getSignature()
{
    QHash<QString, QString> arg = Configure::instance()->configureYTO->signatureArg();

    QJsonObject obj;
    obj.insert("mty", YTO_MTY_SIGNATURE);
    obj.insert("clientMac", arg.value("clientMac"));
    obj.insert("clientIp", arg.value("clientIp"));
    obj.insert("devType", "1");
    obj.insert("userName", arg.value("userName"));
    obj.insert("password", arg.value("password"));
    obj.insert("cameraMac", arg.value("cameraMac"));
    obj.insert("clientId", "1");
    obj.insert("extend1", "");
    obj.insert("extend2", "");
    obj.insert("extend3", "");

    QByteArray json = QJsonDocument(obj).toJson(QJsonDocument::Compact);
    int length = json.length();
    QByteArray str = (QString("%1").arg(length, 8, 10, QLatin1Char('0')) + json + "\r\n").toLatin1();   
    emit appendDetailLog(str);
    //socket请求和获取
    QTcpSocket tcpSocket;
    QString ip = Configure::instance()->configureYTO->ip();
    quint16 portCode = Configure::instance()->configureYTO->portCode();
    tcpSocket.connectToHost(ip, portCode);
    tcpSocket.write(str);
    tcpSocket.waitForReadyRead();
    //阻止发射信号
    //todo

    QByteArray result = tcpSocket.readAll();
    QTextCodec *codec = QTextCodec::codecForName("GBK");
    QString strUnicode = codec->toUnicode(result);
    emit appendDetailLog(strUnicode);
    //解析获取
    QString resultHandle = strUnicode.remove(0, 8).trimmed();
    QJsonDocument doc = QJsonDocument::fromJson(resultHandle.toUtf8());
    QJsonObject dataObj = doc["data"].toObject();
    signature = dataObj["signature"].toString();
}

void YTO::getBoxIdCome(QString code1, QString code2, QString code3, QString expressId, QString Dest_Org)
{
//    short boxId = 0;
//    if (Configure::instance()->configureYTO->ytoCode1().contains(code1) &&
//            Configure::instance()->configureYTO->ytoCode2().contains(code2)) {
//        boxId = Configure::instance()->configureYTO->ruleYtoCome().value(code3);
//        if (boxId != 0) {
//            express->noramlBoxIdHandle(Yto, expressId, boxId, Dest_Org);
//            //到件
//            if (express->getMainWindow()->arrivedFlag() &&
//                    Configure::instance()->configureYTO->arrivedSource() == 0) {
//                //arrivedYto->arriverd(expressId, boxId);
//                emit arrivedSignal(expressId, boxId,
//                   QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz"));
//            }
//            return;
//        }
//    }

//    express->exceptionBoxIdHandle(Yto, expressId);
}

void YTO::getBoxIdGo(QString code1, QString code2, QString code3, QString expressId, QString Dest_Org)
{
//    short boxId = Configure::instance()->configureYTO->ruleYtoGo().value(code1);
//    //正常口
//    if (boxId != 0) {
//        express->noramlBoxIdHandle(Yto, expressId, boxId, Dest_Org);
//        return;
//    }
//    //异常口
//    express->exceptionBoxIdHandle(Yto, expressId);
}

void YTO::readTcpSocket()
{
    if (tcpSocket == nullptr) {
        emit appendDetailLog("读取圆通socket的时候为空\r\n");
        return;
    }

    QByteArray result = tcpSocket->readAll();
    emit appendDetailLog(result);
    QTextCodec *codec = QTextCodec::codecForName("GBK");
    QString strUnicode = codec->toUnicode(result);

    //解析获取
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(result.remove(0, 8).trimmed(), &error);
    if (error.error != QJsonParseError::NoError) {
        emit appendDetailLog("读取圆通socket的时候json解析错误");
        return;
    }
    if (doc.isEmpty()) {
        emit appendDetailLog("读取圆通socket的时候json为空");
        return;
    }
    if (!doc.isObject()) {
        emit appendDetailLog("读取圆通socket的时候doc不是一个json对象");
        return;
    }
    QJsonValue value = doc["WAYBILL_NO"];
    if (!value.isString()) {
        emit appendDetailLog("读取圆通socket的时候doc的快递单号不是一个字符串");
        return;
    }
    QString expressId = doc["WAYBILL_NO"].toString();
    if (expressId.isEmpty()) {
        emit appendDetailLog("读取圆通socket的时候doc的快递单号为空");
        return;
    }
    QJsonValue value1 = doc["DA_TOU_BI"];
    if (!value.isString()) {
        emit appendDetailLog("读取圆通socket的时候doc的三段码不是一个字符串");
        return;
    }

    QString code = doc["DA_TOU_BI"].toString();
    QString code1 = code.section('-', 0, 0);
    QString code2 = code.section('-', 1, 1);
    QString code3 = code.right(3);
    QString Dest_Org = doc["DEST_ORG"].toString();

    if (comeOrGo) {
        getBoxIdCome(code1, code2, code3, expressId, Dest_Org);
    } else {
        getBoxIdGo(code1, code2, code3, expressId, Dest_Org);
    }
}

void YTO::tcpSocketError()
{
    if (tcpSocket == nullptr) {
        return;
    }
    QString str = tcpSocket->errorString();
    emit appendDetailLog(QString("圆通分拣：圆通网关%1连接错误：%2").arg(tcpSocket->peerPort()).arg(str));
}

void YTO::tcpSocektSuccess()
{
    emit appendDetailLog(QString("圆通分拣：圆通网关%1连接成功").arg(tcpSocket->peerPort()));
}

//自动重连
void YTO::autoConnectServer()
{
    createYTOServerConnection();
}
