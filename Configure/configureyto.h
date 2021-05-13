#ifndef CONFIGUREYTO_H
#define CONFIGUREYTO_H

#include <QSettings>
#include <QHash>


class ConfigureYTO : public QObject
{
    Q_OBJECT
public:
    explicit ConfigureYTO(QObject *parent = nullptr);

    void getAllConfigure();

    QHash<QString, QString> signatureArg() { return _signatureArg; }
    QString ip() { return _ip; }
    quint16 portUpload() { return _portUpload; }
    quint16 portCode() { return _portCode; }

    QStringList& ytoCode1() { return _ytoCode1Come; }
    QStringList& ytoCode2() { return _ytoCode2Come; }
    QHash<QString, short> &ruleYtoCome() { return _ruleYtoCome; }
    QHash<QString, short> &ruleYtoGo() { return _ruleYtoGo; }

    QVector<QPair<QString, QString> >& userCodeUserName(){ return _userCodeUserNameVector; }
    QVector<QString>& desOrgCode() { return _desOrgCode; }
    quint16 padPort() { return _padPort; }
    QString createOrgCode() { return _createOrgCode; }
    QString createTerminal() { return _createTerminal; }
    QString createUserCode() { return _createUserCode; }
    QString createUserName() { return _createUserName; }

    QString effectiveTypeCode() { return _effectiveTypeCode; }
    QString expType() { return _expType; }
    QString expressContentCode() { return _expressContentCode; }
    QString feeFlag() { return _feeFlag; }
    QString frequencyNo() { return _frequencyNo; }

    QString nextOrgCode() { return _nextOrgCode; }
    QString orgCode() { return _orgCode; }
    QString previousOrgCode() { return _previousOrgCode; }
    QString sourceOrgCode() { return _sourceOrgCode; }
    QString opCode() { return _opCode; }
    QString line() { return _line; }
    QString remark() { return _remark; }
    QString extend5() { return _extend5; }

    QString vehiclePlateNo() { return _vehiclePlateNo; }
    int arrivedSource() { return _arrivedSource; }
    int intervalArrived() { return _intervalArrived; }
    int intervalPack() { return _intervalPack; }

    //读取文件
    void readYtoUserCodeUserName();
    void readRuleYto();
    void readYtoDesOrgCodes();

signals:

public slots:
    void setConfigure(const QString key, const QVariant value);
    QVariant getConfigure(const QString key);

private:
    QSettings *ytoConfigure;

    //服务器
    QString _ip;
    quint16 _portUpload;
    quint16 _portCode;

    //[SIGNATURE]
    //用户名和密码一个月一改
    QString _userName;
    QString _password;
    //客户端MAC地址
    QString _clientMac;
    //客户端IP地址
    QString _clientIp;
    //视觉MAC地址（随便选一个）
    QString _cameraMac;
    QHash<QString, QString> _signatureArg; //签名参数

    //进港一段码
    QStringList _ytoCode1Come;
    //进港二段码
    QStringList _ytoCode2Come;
    QHash<QString, short> _ruleYtoCome; //圆通进港规则
    QHash<QString, short> _ruleYtoGo; //圆通出港规则

    //扫描员代码和扫描员姓名-与格口对应
    QVector<QPair<QString, QString> > _userCodeUserNameVector;
    //拆包地
    QVector<QString> _desOrgCode;

    QString _createOrgCode;
    QString _createTerminal;
    QString _createUserCode;
    QString _createUserName;

    QString _effectiveTypeCode;
    QString _expType;
    QString _expressContentCode;
    QString _feeFlag;
    QString _frequencyNo;

    QString _nextOrgCode;
    QString _orgCode;
    QString _previousOrgCode;
    QString _sourceOrgCode;
    QString _opCode;
    QString _line;
    QString _remark;
    QString _extend5;

    QString _vehiclePlateNo;
    //把枪
    quint16 _padPort;

    int _arrivedSource; //到件数据来源

    int _intervalPack;
    int _intervalArrived; //
};

#endif // CONFIGUREYTO_H
