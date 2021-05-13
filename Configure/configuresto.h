#ifndef CONFIGURESTO_H
#define CONFIGURESTO_H

#include <QObject>
#include <QHash>
#include <QJsonObject>
#include <QTime>
#include <QSettings>

class ConfigureSTO : public QObject
{
    Q_OBJECT
public:
    explicit ConfigureSTO(QObject *parent = nullptr);
    void getAllConfigure();

    void readRuleSto(); //读取规则文件
    void readSto(); //读取快件类型，派件员和扫描员

    QString& alyIP() { return _alyIP; }
    QString& alyUserName() { return _alyUserName; }
    QString& alyPassword() { return _alyPassword; }

    //过滤
    QJsonObject& OrderTypeEx() { return _OrderTypeEx; }
    QHash<QStringList, QList<short> > &ruleStoCome() { return _RuleStoCome; }
    QHash<QStringList, QList<short> > &ruleStoGo() { return _RuleStoGo; }
    QStringList& code1ListContain() { return _code1ListContain; }
    QStringList& code2ListContain() { return _code2ListContain; }
    short interceptBoxId() { return _interceptBoxId; }
    QStringList &interceptExpress() { return _interceptExpress; }
    QStringList &otherCompanyBoxId() { return _otherCompanyBoxId; }
    QStringList &urbanBoxId() { return _urbanBoxId; }

    quint16 pdaPort() { return _pdaPort; } //申通把枪端口

    QHash<short, QString>& stoOrgCodeHash() { return _stoOrgCodeHash; } //当前网点组织代码
    QVector<QString>& stoArrivedUserCodeVector() { return _stoArrivedUserCodeVector; } //到件扫描员
    QVector<QString>& stoDispatchUserCodeVector() { return _stoDispatchUserCodeVector; } //发件扫描员
    QHash<short, QString>& stoDispatchNextOrgCodeHash() { return _stoDispatchNextOrgCodeHash; } //发件下一站编码
    QVector<QString>& stoPackUserCodeVector() { return _stoPackUserCodeVector; } //集包扫描员
    QHash<short, QString>& stoPackNextOrgCodeHash() { return _stoPackNextOrgCodeHash; } //集包下一站编码
    QVector<QString>& effectiveTypeVector() { return _effectiveTypeVector; } //快件类型
    QVector<QString>& empCodeVector() { return _empCodeVector; } //派件员

    int intervalArrived() { return _intervalArrived; }
    int intervalSend() { return _intervalSend; }
    QTime& startSendTime() { return _startSendTime; } //开始派件的时间
    QStringList& noSendBoxIdList() { return _noSendBoxIdList; } //不派件的格口
    QString& pdaCode() { return _pdaCode; }
    QString& opTerminal() { return _opTerminal; }
    QString& orgCode() { return _orgCode; }
    QString& urbanOrgCode() { return _urbanOrgCode; }
    //QString& userCode() { return _userCode; }
    QString& nextOrgCode() { return _nextOrgCode; }
    int arrivedSource() { return _arrivedSource; }
    int intervalPack() { return _intervalPack; }
    int intervalDispatch() { return _intervalDispatch; }
    int intervalZhuJiDispatchToUrbanArrived() { return _intervalZhuJiDispatchToUrbanArrived; } //诸暨发件到城区到件的时间
    int intervalUrbanArrivedToDispatch() { return _intervalUrbanArrivedToDispatch; } //城区到件到发件的时间
    int intervalUrbanDispatchToCellArrived() { return _intervalUrbanDispatchToCellArrived; } //城区发件到网点到件的时间
    //接口地址url
    QString& stoInterfaceUrl() { return _stoInterfaceUrl; }


signals:
    void appendAlarm(QString);

public slots:
    void setConfigure(const QString key, const QVariant value);
    QVariant getConfigure(const QString key);

private:
    QSettings *stoConfigure;

    QString _alyIP;
    QString _alyUserName;
    QString _alyPassword;

    QHash<QStringList, QList<short> > _RuleStoCome; //申通进港规则
    QHash<QStringList, QList<short> > _RuleStoGo; //申通出港规则
    QJsonObject _OrderTypeEx; //申通三段码优先级
    QStringList _code1ListContain;
    QStringList _code2ListContain;
    QStringList _interceptExpress; //申通拦截件列表
    short _interceptBoxId; //拦截件格口
    //其他公司所对应得格口-没有到件及后续操作
    QStringList _otherCompanyBoxId;
    QStringList _urbanBoxId;

    quint16 _pdaPort; //申通把枪端口

    QVector<QString> _stoArrivedUserCodeVector; //到件扫描员
    QVector<QString> _stoDispatchUserCodeVector; //发件扫描员
    QHash<short, QString> _stoOrgCodeHash; //当前组织代码
    QHash<short, QString> _stoDispatchNextOrgCodeHash; //发件下一站编码
    QVector<QString> _stoPackUserCodeVector; //集包扫描员
    QHash<short, QString> _stoPackNextOrgCodeHash; //集包下一站编码
    QVector<QString> _empCodeVector; //派件员
    QVector<QString> _effectiveTypeVector; //快件类型
    //到件
    //[ARRIVERD]
    //从数据库获取数据的时间间隔
    int _intervalArrived;

    //发件
    int _intervalDispatch;

    //派件
    //[SEND]
    //从数据库获取数据的时间间隔
    int _intervalSend;
    QTime _startSendTime; //开始派件的时间
    QStringList _noSendBoxIdList;

    QString _pdaCode;
    QString _opTerminal;
    QString _orgCode; //诸暨公司组织代码
    QString _urbanOrgCode; //城区公司组织代码

    //QString _userCode;
    QString _nextOrgCode;

    int _arrivedSource; //达件数据来源
    int _intervalPack;

    //时间间隔
    int _intervalZhuJiDispatchToUrbanArrived; //诸暨发件到城区到件的时间
    int _intervalUrbanArrivedToDispatch; //城区到件到发件的时间
    int _intervalUrbanDispatchToCellArrived; //城区发件到网点到件的时间

    //接口地址url
    QString _stoInterfaceUrl;
};

#endif // CONFIGURESTO_H
