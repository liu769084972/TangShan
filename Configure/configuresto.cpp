#include "configuresto.h"
#include "define.h"

#include <QJsonDocument>
#include <QFile>
#include <QMessageBox>

ConfigureSTO::ConfigureSTO(QObject *parent) : QObject(parent)
{
    getAllConfigure();
}

void ConfigureSTO::getAllConfigure()
{
    //读取规则
    readRuleSto();
    //读取扫描员等
    readSto();

    //申通接口数据库
    stoConfigure = new QSettings(FILE_STO_PATH, QSettings::IniFormat);

    _alyIP = stoConfigure->value("ALY/ip").toString();
    _alyUserName = stoConfigure->value("ALY/userName").toString();
    _alyPassword = stoConfigure->value("ALY/password").toString();


    //三段码优先级
    QJsonDocument doc = QJsonDocument::fromJson(
                stoConfigure->value("FILTER/OrderTypeEx").toByteArray());
    _OrderTypeEx = doc.object();
    _code1ListContain = stoConfigure->value("FILTER/code1ListContain").toStringList();
    _code2ListContain = stoConfigure->value("FILTER/code2ListContain").toStringList();
    //拦截格口
    _interceptBoxId = stoConfigure->value("FILTER/interceptBoxId").toString().toShort();
    _otherCompanyBoxId = stoConfigure->value("FILTER/otherCompanyBoxId").toStringList();
    _urbanBoxId = stoConfigure->value("FILTER/urbanBoxId ").toStringList();

    //把枪
    _pdaPort = stoConfigure->value("PAD/port").toString().toUShort();

    //间隔时间
    _intervalArrived = stoConfigure->value("ARRIVED/intervalArrived").toInt();
    _intervalSend = stoConfigure->value("SEND/intervalSend").toInt();
    _startSendTime = stoConfigure->value("SEND/startSendTime").toTime();
    _noSendBoxIdList = stoConfigure->value("CONF/noSendBoxIdList").toStringList();

    //公共
    _pdaCode = stoConfigure->value("PUBLIC/pdaCode").toString();
    _opTerminal = stoConfigure->value("PUBLIC/opTerminal").toString();
    _orgCode = stoConfigure->value("PUBLIC/orgCode").toString();
    //_userCode = stoConfigure->value("PUBLIC/userCode").toString();
    _urbanOrgCode = stoConfigure->value("PUBLIC/urbanOrgCode").toString();; //城区公司组织代码

    //集包
    _nextOrgCode = stoConfigure->value("PACK/nextOrgCode").toString();
    _intervalPack =stoConfigure->value("PACK/intervalPack").toInt();

    //到件
    _arrivedSource = stoConfigure->value("ARRIVED/source").toInt();

    //发件
    _intervalDispatch = stoConfigure->value("DISPATCH/intervalDispatch").toInt();

    //时间间隔(秒，配置文件分钟）
    _intervalZhuJiDispatchToUrbanArrived = stoConfigure->value("DISPATCH/intervalZhuJiDispatchToUrbanArrived").toInt() * 60; //诸暨发件到城区到件的时间
    _intervalUrbanArrivedToDispatch = stoConfigure->value("ARRIVED/intervalUrbanArrivedToDispatch").toInt() * 60; //城区到件到发件的时间
    _intervalUrbanDispatchToCellArrived = stoConfigure->value("DISPATCH/intervalUrbanDispatchToCellArrived").toInt() * 60; //城区发件到网点到件的时间

    QString ip = stoConfigure->value("INTERFACE/ip").toString();
    _stoInterfaceUrl = QString(STO_CODE_URL).arg(ip); //申通接口url
}

void ConfigureSTO::readRuleSto()
{
    //进港
    QFile fileRuleIn(FILE_STO_COME_RULE);
    if (!fileRuleIn.open(QIODevice::ReadOnly | QIODevice::Text)) {        
        QMessageBox::critical(nullptr, "报警", QString("%1打开失败！").arg(FILE_STO_COME_RULE));
    } else {
        _RuleStoCome.clear();
        //跳过第一行
        fileRuleIn.readLine();
        while (!fileRuleIn.atEnd()) {
            QString csv = fileRuleIn.readLine();

            QStringList list = csv.split(',');
            //3段码，空代表任意，必须存进去
            QStringList code;
            QString code1 = list[0].trimmed();
            QString code2 = list[1].trimmed();
            QString code3 = list[2].trimmed();
            code.append(code1);
            code.append(code2);
            code.append(code3);

            //格口
            QList<short> boxIdList;
            for (int i = 3; i < list.size(); ++i) {
                if (list[i].toShort() > 0)
                    boxIdList.append(list[i].toShort()); //只有正常的格口才会添加到列表
            }
            if (boxIdList.size() > 0)
                _RuleStoCome.insert(code, boxIdList); //只有有格口才会添加到规则
        }
        //todo "<font style=color:green;font-size:12px>进港规则读取成功<br/></font>";
    }

    //出港
    QFile fileRuleOut(FILE_STO_GO_RULE);
    if (!fileRuleOut.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(nullptr, "报警", QString("%1打开失败！").arg(FILE_STO_GO_RULE));
    } else {
        _RuleStoGo.clear();
        while (!fileRuleOut.atEnd()) {
            QString csv = fileRuleOut.readLine();

            QStringList list = csv.split(',');
            //3段码，空代表任意，必须存进去
            QStringList code;
            QString code1 = list[0].trimmed();
            QString code2 = list[1].trimmed();
            QString code3 = list[2].trimmed();
            code.append(code1);
            code.append(code2);
            code.append(code3);

            //格口
            QList<short> boxIdList;
            for (int i = 1; i < list.size(); ++i) {
                if (list[i].toShort() > 0)
                    boxIdList.append(list[i].toShort());
            }
            if (boxIdList.size() > 0)
                _RuleStoGo.insert(code, boxIdList);
        }
        fileRuleOut.close();
        //todo "<font style=color:green;font-size:12px>出港规则读取成功</font>";
    }
}

void ConfigureSTO::readSto()
{
    //快件类型
    QFile file(FILE_STO);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(nullptr, "报警", QString("%1打开失败！")
                              .arg(FILE_STO));
       // emit appendAlarm(QString("读取快件类型失败: %1").arg(file.errorString()));
    } else {

        _stoOrgCodeHash.clear(); //组织代码
        _stoArrivedUserCodeVector.clear(); //到件扫描员
        _stoArrivedUserCodeVector.resize(400);
        _stoDispatchUserCodeVector.clear(); //发件扫描员
        _stoDispatchUserCodeVector.resize(400);
        _stoDispatchNextOrgCodeHash.clear(); //发件下一站编码
        _stoPackUserCodeVector.clear(); //集包扫描员
        _stoPackUserCodeVector.resize(400);
        _stoPackNextOrgCodeHash.clear(); //集包下一站编码
        _empCodeVector.clear(); //派件员
        _empCodeVector.resize(400);
        _effectiveTypeVector.clear(); //快件类型
        _effectiveTypeVector.resize(400);

        file.readLine();
        while (!file.atEnd()) {
            QString line = file.readLine();
            QStringList list = line.split(',');

            short boxId = list[0].toShort();
            QString orgCode = list[1];
            QString arrivedUserCode = list[2];
            QString dispatchUserCode = list[3];
            QString dispatchNextOrgCode = list[4];
            QString packUserCode = list[5];
            QString packNextOrgCode = list[6];
            QString empCode = list[7];
            QString effectiveType = list[8];
            if (boxId >= 0 && boxId < 400) {
                _stoOrgCodeHash.insert(boxId, orgCode);
                _stoArrivedUserCodeVector[boxId] = arrivedUserCode;
                _stoDispatchUserCodeVector[boxId] = dispatchUserCode;
                _stoDispatchNextOrgCodeHash.insert(boxId, dispatchNextOrgCode);
                _stoPackUserCodeVector[boxId] = packUserCode;
                _stoPackNextOrgCodeHash.insert(boxId, packNextOrgCode);
                _empCodeVector[boxId] = empCode;
                _effectiveTypeVector[boxId] = effectiveType;
            }
        }
    }
}

void ConfigureSTO::setConfigure(const QString key, const QVariant value)
{
    stoConfigure->setValue(key, value);
}

QVariant ConfigureSTO::getConfigure(const QString key)
{
    return stoConfigure->value(key);
}
