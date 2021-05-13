#include "configureyto.h"
#include "define.h"

#include <QTextCodec>
#include <QFile>
#include <QMessageBox>
#include <QDir>
#include <QDebug>

ConfigureYTO::ConfigureYTO(QObject *parent) : QObject(parent)
{
    getAllConfigure();
}

void ConfigureYTO::getAllConfigure()
{
    readRuleYto();
    //qDebug() << QDir::currentPath();
    ytoConfigure = new QSettings(FILE_YTO_PATH, QSettings::IniFormat);
    ytoConfigure->setIniCodec(QTextCodec::codecForName("UTF-8")); //读取中文

    //圆通服务器
    _ip = ytoConfigure->value("YTSERVER/ip").toString();
    _portUpload = static_cast<quint16>(ytoConfigure->value("YTSERVER/portUpload").toUInt());
    _portCode = static_cast<quint16>(ytoConfigure->value("YTSERVER/portCode").toUInt());

    //签名参数
    _userName = ytoConfigure->value("SIGNATURE/userName").toString();
    _password = ytoConfigure->value("SIGNATURE/password").toString();
    _clientMac = ytoConfigure->value("SIGNATURE/clientMac").toString();
    _clientIp = ytoConfigure->value("SIGNATURE/clientIp").toString();
    _cameraMac = ytoConfigure->value("SIGNATURE/cameraMac").toString();
    _signatureArg.clear();
    _signatureArg.insert("userName", _userName);
    _signatureArg.insert("password", _password);
    _signatureArg.insert("clientMac", _clientMac);
    _signatureArg.insert("clientIp", _clientIp);
    _signatureArg.insert("cameraMac", _cameraMac);

    _createUserName = ytoConfigure->value("PUBLIC/createUserName").toString();


    //过滤规则
    _ytoCode1Come = ytoConfigure->value("FILTER/ytoCode1Come").toStringList();
    _ytoCode2Come = ytoConfigure->value("FILTER/ytoCode2Come").toStringList();

    //到件
    _createOrgCode = ytoConfigure->value("ARRIVED/createOrgCode").toString();
    _createTerminal = ytoConfigure->value("ARRIVED/createTerminal").toString();
    _createUserCode = ytoConfigure->value("ARRIVED/createUserCode").toString();
    _createUserName = ytoConfigure->value("ARRIVED/createUserName").toString();

    _effectiveTypeCode = ytoConfigure->value("ARRIVED/effectiveTypeCode").toString();
    _expType = ytoConfigure->value("ARRIVED/expType").toString();
    _expressContentCode = ytoConfigure->value("ARRIVED/expressContentCode").toString();
    _feeFlag = ytoConfigure->value("ARRIVED/feeFlag").toString();
    _frequencyNo = ytoConfigure->value("ARRIVED/frequencyNo").toString();

    _nextOrgCode = ytoConfigure->value("ARRIVED/nextOrgCode").toString();
    _orgCode = ytoConfigure->value("ARRIVED/orgCode").toString();
    _previousOrgCode = ytoConfigure->value("ARRIVED/previousOrgCode").toString();
    _sourceOrgCode = ytoConfigure->value("ARRIVED/sourceOrgCode").toString();

    _line = ytoConfigure->value("ARRIVED/line").toString();
    _remark = ytoConfigure->value("ARRIVED/remark").toString();


    //到件接口
    readYtoUserCodeUserName();
    readYtoDesOrgCodes();

    //集包
    _padPort = static_cast<quint16>(ytoConfigure->value("PACK/padPort").toUInt());

    //到件
    _arrivedSource = ytoConfigure->value("ARRIVED/source").toInt();
    _intervalArrived = ytoConfigure->value("ARRIVED/intervalArrived").toInt();
    _intervalPack = ytoConfigure->value("PACK/intervalPack").toInt();
}

void ConfigureYTO::readYtoUserCodeUserName()
{
    _userCodeUserNameVector.clear();
    _userCodeUserNameVector.resize(400);

    QTextCodec *codec = QTextCodec::codecForName("GBK");
    QFile file(FILE_YTO_USERCODEUSERNAME);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(nullptr, "报警", QString("%1打开失败！").arg(FILE_YTO_USERCODEUSERNAME));
        //exit(-1);
    }

    while (!file.atEnd()) {
        QByteArray line = file.readLine();
        QPair<QString, QString> pair(line.split(',')[1].trimmed(),
                codec->toUnicode(line.split(',')[2].trimmed()));
        _userCodeUserNameVector[line.split(',')[0].trimmed().toInt()] = pair;
    }
}

void ConfigureYTO::readRuleYto()
{
    //进港
    QFile fileRuleIn(FILE_YTO_COME_RULE);
    if (!fileRuleIn.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(nullptr, "报警", "inRuleYto.csv打开失败！");
    } else {
        _ruleYtoCome.clear();
        while (!fileRuleIn.atEnd()) {
            QString csv = fileRuleIn.readLine();
            QString code3 = csv.section(',', 0, 0).trimmed();
            QString boxId = csv.section(',', 1, 1).trimmed();
            _ruleYtoCome.insert(code3, boxId.toShort());
        }
        //todo "<font style=color:green;font-size:12px>进港规则读取成功<br/></font>";
    }

    //出港
    QFile fileRuleOut(FILE_YTO_GO_RULE);
    if (!fileRuleOut.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(nullptr, "报警", "outRuleYto.csv打开失败！");
    } else {
        _ruleYtoGo.clear();
        while (!fileRuleOut.atEnd()) {
            QString csv = fileRuleOut.readLine();
            QString code1 = csv.section(',', 0, 0).trimmed();
            QString boxId = csv.section(',', 1, 1).trimmed();
            _ruleYtoGo.insert(code1, boxId.toShort());
        }
        fileRuleOut.close();
        //todo "<font style=color:green;font-size:12px>出港规则读取成功</font>";
    }
}

void ConfigureYTO::readYtoDesOrgCodes()
{
    QFile file(FILE_YTO_DESTORGCODE);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(nullptr, "报警", QString("%1打开失败！").arg(FILE_YTO_DESTORGCODE));
    } else {
        _desOrgCode.clear();
        _desOrgCode.resize(400);
        while (!file.atEnd()) {
            QString csv = file.readLine();
            int boxId = csv.section(',', 0, 0).trimmed().toInt();
            QString desOrgCode = csv.section(',', 1, 1).trimmed();
            _desOrgCode[boxId] = desOrgCode;
        }
    }
}

void ConfigureYTO::setConfigure(const QString key, const QVariant value)
{
    ytoConfigure->setValue(key, value);
}

QVariant ConfigureYTO::getConfigure(const QString key)
{
    return ytoConfigure->value(key);
}
