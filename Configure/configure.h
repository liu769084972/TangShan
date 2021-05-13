#ifndef CONFIGURE_H
#define CONFIGURE_H

#include <QObject>
#include <QBitArray>
#include <QJsonObject>
#include <QHash>
#include <QTcpSocket>
#include <QSettings>
#include <QMutex>
#include "define.h"
#include "Configure/configureyto.h"
#include "Configure/configuresto.h"

typedef struct {
    int cameraId = -1; //视觉编号，-1表示不存在
    QString cameraName; //视觉名称
    CameraType type = HaiKang; //视觉类型 大华，海康
    QTcpSocket *tcpSocekt = nullptr;
    QString ip; //视觉IP地址
    quint16 port = 0; //端口
    int plcIndex = -1; //对应的PLC索引
    short exceptionBoxId = -1; //异常口
    int carrierNum; //小车数量
    QString pdaCode;
} StructCamera;

class Configure : public QObject
{
    Q_OBJECT

private:
    explicit Configure(QObject *parent = nullptr);

    ~Configure();

public:
    static Configure *instance();

    QBitArray companySwitch();

    QHash<QString, short> &inRuleBest();
    QHash<QString, short> &outRuleBest();
    QHash<QString, short> &inRuleTt();
    QHash<QString, short> &outRuleTt();

    QBitArray &companySwitchCome(){ return _companySwitchCome; }
    QBitArray &companySwitchGo(){ return _companySwitchGo; }

    unsigned char* netId() { return _netId; }

    QString &nativeip() { return _nativeip; } //本地数据库IP
    int cameraType() { return _cameraType; } //相机类型
    int carrierNum() { return _carrierNum; } //小车数量
    int maxBoxId() { return _maxBoxId; } //最大格口编号
    qint64 interfaceMaxDelay() { return _interfaceMaxDelay; }
    QVector<StructCamera>& camerasVector() { return _camerasVector; } //返回所有视觉信息
    unsigned long StrackInWait() { return _StrackInWait; }
    //QStringList goTableName() { return _goTableName; } //进港表

    //GUI
    int arrivedRow() { return _arrivedRow; }
    int sendOrBagRow() { return _sendOrBagRow; }   
    QString appName() { return _appName; }
    int intervalPlcAlarm() { return _intervalPlcAlarm; }
    void getAllConfigure();
    void setConfigure(const QString key, const QVariant value);
    QVariant getConfigure(const QString key);


    void readRuleBest();
    void readRuleTt();


signals:

public slots:

private:
    static Configure *configure;

    QSettings *global;
    QMutex mutex;

    QBitArray _companySwitchCome; //设备使用快递公司-进港
    QBitArray _companySwitchGo; //设备使用快递公司-进港
    //QStringList _comeTableName; //进港表
    //QStringList _goTableName; //进港表
    unsigned char _netId[6]; //PLC NetId

    int _cameraType; //相机类型
    int _carrierNum; //小车数量
    int _maxBoxId; //最大格口编号

    QHash<QString, short> _inRuleBest; //百世进港规则
    QHash<QString, short> _outRuleBest; //百世出港规则
    QHash<QString, short> _inRuleTt; //天天进港规则
    QHash<QString, short> _outRuleTt; //天天出港规则

    QString _nativeip; //本地数据库

    //[ALARM]
    //接口最大延迟，超过，会产生一条报警，单位毫秒
    qint64 _interfaceMaxDelay = 200;

    QVector<StructCamera> _camerasVector;

    //界面设置
    //[GUI]
    //到件显示列表的行数
    int _arrivedRow;
    //派件或集包显示列表的行数
    int _sendOrBagRow;
    QString _appName;

    unsigned long _StrackInWait;

    int _intervalPlcAlarm;

public:
    ConfigureYTO *configureYTO;
    ConfigureSTO *configureSTO;
};

#endif // CONFIGURE_H
