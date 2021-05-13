#ifndef TESTDROP_H
#define TESTDROP_H

#include <QHash>
#include <QObject>
#include <QTcpSocket>

#include "plc.h"
#include "Configure/configure.h"

class TestUI;
class TestDrop : public QObject
{
    Q_OBJECT

public:
    explicit TestDrop(TestUI *testUi, QObject *parent = nullptr);

signals:

public slots:
    void addOneExpressIdBoxId(QString expressId, short boxId);
    void clearExpressIdBoxIdHash();
    void deleteOneExpressIdBoxId(QString expressId);
    void readCamera();
    void initCamera();
    void openCamera();
    void closeCamera();
    void saveDataToFile();

private:
    QString mkMultiDir(const QString path);
    //判断字符串是否只包含数字和字母
    inline bool isLetterOrNumberString(QString str);

    QHash<QString, short> expressIdBoxIdHash; //存放测试用的快递单号和格口

    //视觉
    QVector<StructCamera> cameras;

    //界面类
    TestUI *testUI;

    PLC *plc;

    //设备状态
    QVector<QString> deviceState; //0为plc，其他相机编号+1

    short carrierId[8] = {0};
};

#endif // TESTDROP_H
