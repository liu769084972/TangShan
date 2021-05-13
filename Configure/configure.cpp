#include "configure.h"
#include "define.h"

#include <QJsonDocument>
#include <QFile>
#include <QTextCodec>
#include <QMutexLocker>

class Configure * Configure::configure = nullptr;

Configure::Configure(QObject *parent)
    : QObject(parent)
{
    global = new QSettings(FILE_GLOBAL_PATH, QSettings::IniFormat);
    QString fileName = global->fileName();

    global->setIniCodec(QTextCodec::codecForName("UTF-8")); //防止中文乱码

    getAllConfigure();
}

Configure::~Configure()
{

}

class Configure *Configure::instance()
{
    if (configure != nullptr)
        return configure;
    else {
        configure = new Configure(nullptr);
        return configure;
    }
}

QBitArray Configure::companySwitch()
{
    QBitArray ba;
    ba.resize(COMPANYNUM);
    for (int i = 0; i < COMPANYNUM; ++i) {
        ba[i] = _companySwitchGo[i] | _companySwitchCome[i];
    }

    return ba;
}

QHash<QString, short> &Configure::inRuleBest()
{
    return _inRuleBest;
}

QHash<QString, short> &Configure::outRuleBest()
{
    return _outRuleBest;
}

QHash<QString, short> &Configure::inRuleTt()
{
    return _inRuleTt;
}

QHash<QString, short> &Configure::outRuleTt()
{
    return _outRuleTt;
}

void Configure::getAllConfigure()
{
    //分拣机相关
    _carrierNum = global->value("Track/carrierNum").toInt();
    _maxBoxId = global->value("Track/boxNum").toInt();

    //公司相关
    QString sto = global->value("COMPANY/sto").toString();
    QStringList stoList = sto.split("|");
    QString zto = global->value("COMPANY/zto").toString();
    QStringList ztoList = zto.split("|");
    QString yto = global->value("COMPANY/yto").toString();
    QStringList ytoList = yto.split("|");
    QString best = global->value("COMPANY/best").toString();
    QStringList bestList = best.split("|");
    QString tt = global->value("COMPANY/tt").toString();
    QStringList ttList = tt.split("|");
    QString yunda = global->value("COMPANY/yunda").toString();
    QStringList yundaList = yunda.split("|");
    QString youzheng = global->value("COMPANY/youzheng").toString();
    QStringList youzhengList = youzheng.split("|");

    //是否在用公司

    _companySwitchCome.clear();
    _companySwitchCome.resize(COMPANYNUM);
    _companySwitchCome[Sto] = stoList.value(0).toInt();
    _companySwitchCome[Zto] = ztoList.value(0).toInt();
    _companySwitchCome[Yto] = ytoList.value(0).toInt();
    _companySwitchCome[Best] = bestList.value(0).toInt();
    _companySwitchCome[Tt] = ttList.value(0).toInt();
    _companySwitchCome[Yunda] = yundaList.value(0).toInt();
    _companySwitchCome[Youzheng] = youzhengList.value(0).toInt();

    _companySwitchGo.clear();
    _companySwitchGo.resize(COMPANYNUM);
    _companySwitchGo[Sto] = stoList.value(1).toInt();
    _companySwitchGo[Zto] = ztoList.value(1).toInt();
    _companySwitchGo[Yto] = ytoList.value(1).toInt();
    _companySwitchGo[Best] = bestList.value(1).toInt();
    _companySwitchGo[Tt] = ttList.value(1).toInt();
    _companySwitchGo[Yunda] = yundaList.value(1).toInt();
    _companySwitchGo[Youzheng] = youzhengList.value(1).toInt();


    //PLC
    QStringList netIdList = global->value("PLC/AmsNetId").toString().split('.');
    for(int i = 0; i < netIdList.size(); ++i) {
        _netId[i] = static_cast<unsigned char>(netIdList[i].toUShort());
    }
    _intervalPlcAlarm = global->value("PLC/intervalPlcAlarm").toInt();

    //本地数据库相关NATIVEDB
    _nativeip = global->value("NATIVEDB/ip").toString();

    //厂家接口最大报警时限，超过，将报警
    _interfaceMaxDelay = global->value("ALARM/interfaceMaxDelay").toLongLong();

    //所有相机
    _cameraType = global->value("CAMERA/cameraType").toInt();

    _camerasVector.clear();
    QString sCameras = global->value("CAMERA/cameraArg").toString(); //相机参数
    qDebug() << QString("相机参数：%1").arg(sCameras);
//    QTextCodec *codec = QTextCodec::codecForName("GB2312");
//    sCameras = codec->fromUnicode(sCameras.unicode());
    QStringList camerasList = sCameras.split("-", QString::SkipEmptyParts);
    for(int i = 0; i < camerasList.size(); ++i) {
        QStringList cameraPart = camerasList[i].split("|");
        StructCamera structCamera;
        structCamera.cameraId = i;
        structCamera.cameraName = cameraPart.value(0);
        if (cameraPart.value(1) == "大华") {
            structCamera.type = DaHua;
        } else if (cameraPart.value(1) == "海康") {
            structCamera.type = HaiKang;
        } else {
            structCamera.type = HaiKang; //默认
        }
        structCamera.ip = cameraPart.value(2);
        structCamera.port = cameraPart.value(3).toUShort();
        structCamera.plcIndex = cameraPart.value(4).toInt();
        structCamera.exceptionBoxId = cameraPart.value(5).toShort();
        structCamera.carrierNum = cameraPart.value(6).toInt();
        structCamera.pdaCode = cameraPart.value(7);
        _camerasVector.append(structCamera);
    }

    //GUI
    _arrivedRow = global->value("GUI/arrivedRow").toInt();
    _sendOrBagRow = global->value("GUI/sendOrBagRow").toInt();
    _appName = global->value("GUI/appName").toString();

    if (_companySwitchGo[Sto] || _companySwitchCome[Sto]) {
        configureSTO = new ConfigureSTO(this);
    }

    if (_companySwitchGo[Yto] || _companySwitchCome[Yto]) {
        configureYTO = new ConfigureYTO(this);
    }

    _StrackInWait = global->value("PLC/StrackInWait").toUInt();
}

void Configure::setConfigure(const QString key, const QVariant value)
{
    QMutexLocker locker(&mutex);
    global->setValue(key, value);
}

QVariant Configure::getConfigure(const QString key)
{
    return global->value(key);
}

void Configure::readRuleBest()
{
    //进港
    QFile fileRuleIn("Configure/inRuleBest.csv");
    if (!fileRuleIn.open(QIODevice::ReadOnly | QIODevice::Text)) {
        //todo
    } else {
        _inRuleBest.clear();
        while (!fileRuleIn.atEnd()) {
            QString csv = fileRuleIn.readLine();
            QString code3 = csv.section(',', 0, 0);
            QString boxId = csv.section(',', 1, 1);
            _inRuleBest.insert(code3, boxId.toShort());
        }
        //todo "<font style=color:green;font-size:12px>进港规则读取成功<br/></font>";
    }

    //出港
    QFile fileRuleOut("Configure/outRuleBest.csv");
    if (!fileRuleOut.open(QIODevice::ReadOnly | QIODevice::Text)) {
        //todo
    } else {
        _outRuleBest.clear();
        while (!fileRuleOut.atEnd()) {
            QString csv = fileRuleOut.readLine();
            QString code1 = csv.section(',', 0, 0);
            QString boxId = csv.section(',', 1, 1);
            _outRuleBest.insert(code1, boxId.toShort());
        }
        fileRuleOut.close();
        //todo "<font style=color:green;font-size:12px>出港规则读取成功</font>";
    }
}

void Configure::readRuleTt()
{
    //进港
    QFile fileRuleIn("Configure/inRuleTt.csv");
    if (!fileRuleIn.open(QIODevice::ReadOnly | QIODevice::Text)) {
        //todo
    } else {
        _inRuleTt.clear();
        while (!fileRuleIn.atEnd()) {
            QString csv = fileRuleIn.readLine();
            QString code3 = csv.section(',', 0, 0);
            QString boxId = csv.section(',', 1, 1);
            _inRuleTt.insert(code3, boxId.toShort());
        }
        //todo "<font style=color:green;font-size:12px>进港规则读取成功<br/></font>";
    }

    //出港
    QFile fileRuleOut("Configure/outRuleTt.csv");
    if (!fileRuleOut.open(QIODevice::ReadOnly | QIODevice::Text)) {
        //todo
    } else {
        _outRuleTt.clear();
        while (!fileRuleOut.atEnd()) {
            QString csv = fileRuleOut.readLine();
            QString code1 = csv.section(',', 0, 0);
            QString boxId = csv.section(',', 1, 1);
            _outRuleTt.insert(code1, boxId.toShort());
        }
        fileRuleOut.close();
        //todo "<font style=color:green;font-size:12px>出港规则读取成功</font>";
    }
}


