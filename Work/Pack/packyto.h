#ifndef PACKYTO_H
#define PACKYTO_H

#include <QSqlDatabase>
#include <QTcpSocket>
#include <QTcpServer>

#include "plc.h"
#include "log.h"

class PackYTO : public QObject
{
    Q_OBJECT
public:
    explicit PackYTO(QObject *parent = nullptr);

private:
    void createDbConnection(); //创建数据库连接
    void createSocketConnection();
    void autoConnection();
    void getSignature();
    QByteArray createMessage(QString waybillNo, QString drop_time, QString containerNo, QString chipNo, QString desOrgCode, QString extend5, QString weight);
    void initServerPda();
    void readServer();
    void work(int boxNum, QString containerNo, QString chipNo, QTcpSocket* socketPad);
    void getData(int boxNum, int start, int end, QString containerNo, QTcpSocket *socket, QString chipNo);
    void padResponse(QTcpSocket *socket, QString wayBillNo, QString desOrgCode, QString chipNo);
    void update(QString expressId, QString containerNo);
    QHash<QString, QString> getWeight(QStringList expressList); //获取重量

    QString getWeight(QString expressId);

signals:
    void appendPackWindow(QString message);
    void appendDetailLog(QString message);
    void appendAlarmWindow(QString message);
    void appendDataToGUI(QStringList expressIdList, QString containerNo, QString boxId, QString status);

public slots:
    void init();
    void socektSuccess();
    void socketError();
    void readTcpSocket();
    QString createUuid();
    void setVaild(bool valid);

private:
    QSqlDatabase db;
    //获取重量的数据库
    QSqlDatabase dbWeight;

    QString mty;

    QTcpSocket *socket;
    bool errorLabel; //是否将错误显示到标签

    //把枪
    QTcpServer *server = nullptr;

    PLC *plc;

    qint64 endTime[400]; //记录集包成功的截至时间，当扫描为空的时候使用，作为下次时间的开始时间，开机用当前时间初始化

    QVector<QString> destOrgCodeVector;

    QString signature;

    Log *log;

    bool isValid = false;
};

#endif // PACKYTO_H
