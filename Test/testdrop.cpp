#include "testdrop.h"
#include "Test/UI/testui.h"
#include "define.h"
#include <QDir>

TestDrop::TestDrop(TestUI *testUi, QObject *parent)
    : QObject (parent), testUI(testUi)
{
    deviceState.resize(12);
    initCamera();
}

void TestDrop::addOneExpressIdBoxId(QString expressId, short boxId)
{
    expressIdBoxIdHash.insert(expressId, boxId);
}

void TestDrop::clearExpressIdBoxIdHash()
{
    expressIdBoxIdHash.clear();
}

void TestDrop::deleteOneExpressIdBoxId(QString expressId)
{
    expressIdBoxIdHash.remove(expressId);
}

void TestDrop::initCamera()
{
    cameras  = Configure::instance()->camerasVector();

    for (int i = 0; i < cameras.size(); ++i) {
        cameras[i].tcpSocekt = new QTcpSocket(this);
        connect(cameras[i].tcpSocekt, &QAbstractSocket::connected, this, [&](){
            auto socket = qobject_cast<QTcpSocket *>(sender());
            for (int i = 0; i < cameras.size(); ++i) {
                if (cameras[i].tcpSocekt == socket) {
                    deviceState[i+3] = QString("<font color='green'>%1连接成功</font>").arg(cameras[i].cameraName);
                    testUI->labelShowStatus->setText(deviceState.toList().join("<br>").trimmed());
                    return;
                }
            }
        });

        connect(cameras[i].tcpSocekt, QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error), this, [&]()
        {
            auto socket = qobject_cast<QTcpSocket *>(sender());
            for (int i = 0; i < cameras.size(); ++i) {
                if (cameras[i].tcpSocekt == socket) {
                    deviceState[i+3] = QString("<font color='red'>%1连接失败:%2</font>")
                            .arg(cameras[i].cameraName).arg(socket->errorString());
                    testUI->labelShowStatus->setText(deviceState.toList().join("<br>").trimmed());
                    return;
                }
            }
        });

        connect(cameras[i].tcpSocekt, SIGNAL(readyRead()), this, SLOT(readCamera()));       
    }
}

void TestDrop::openCamera()
{
    plc = new PLC(this);
    long lErr1 = plc->initAddress();
    if (lErr1) {
        deviceState[0] = QString("<font color=red>测试：PLC初始化地址失败，错误代号%1</font>").arg(lErr1);
        testUI->labelShowStatus->setText(deviceState.toList().join("</br>").trimmed());
    }
    long lErr2 = plc->initHandle();
    if (lErr2) {
        deviceState[1] = QString("<font color='red'>测试：PLC初始化句柄失败，错误代号%1</font>").arg(lErr2);
        testUI->labelShowStatus->setText(deviceState.toList().join("<br>").trimmed());
    }
    testUI->labelShowStatus->clear();

    for (int i = 0; i < cameras.size(); ++i) {
        cameras[i].tcpSocekt->connectToHost(cameras[i].ip, cameras[i].port);
    }
}

void TestDrop::closeCamera()
{
    delete plc;
    testUI->labelShowStatus->clear();
    for (int i = 0; i < cameras.size(); ++i) {
        cameras[i].tcpSocekt->close();
        deviceState[i+3] = QString("<font color='blue'>%1关闭</font>").arg(cameras[i].cameraName);
        testUI->labelShowStatus->setText(deviceState.toList().join("<br>").trimmed());
    }
}

void TestDrop::saveDataToFile()
{

}

//建立文件
QString TestDrop::mkMultiDir(const QString path)
{
    QDir dir(path);
    if (dir.exists()) {
        return path;
    }

    QString parentDir = mkMultiDir(path.mid(0, path.lastIndexOf('/')));
    QString dirname = path.mid(path.lastIndexOf('/')+1);
    QDir parentPath(parentDir);
    if (!dirname.isEmpty()) {
        parentPath.mkpath(dirname);
    }
    return parentDir + "/" + dirname;
}

bool TestDrop::isLetterOrNumberString(QString str)
{
    str = str.trimmed();
    if (str.isEmpty())
        return false;
    QString::ConstIterator iterator = str.begin();
    while (iterator != str.end()) {
        if (!iterator->isLetterOrNumber()) {
            return false;
        }
        iterator++;
    }
    return true;
}

void TestDrop::readCamera()
{
    //读取内容
    QTcpSocket *tcpSocket = qobject_cast<QTcpSocket *>(sender());
    QString cameraData = tcpSocket->readAll();
    qDebug() << "测试：相机原始数据" << cameraData << "\r\n";

    //找到相机位置
    int cameraId = -1;
    int cameraIndex = -1; //默认异常口
    QString camera;
    int carrierNum = 0;
    CameraType cameraType = DaHua;
    for (int i = 0; i < cameras.size(); ++i) {
        if (tcpSocket == cameras[i].tcpSocekt) {
            cameraIndex = cameras[i].plcIndex;
            camera = cameras[i].cameraName;
            carrierNum = cameras[i].carrierNum;
            cameraType = cameras[i].type;
            cameraId = cameras[i].cameraId;
            break;
        }
    }

    //获取快递号和小车号
    QString expressId;
    if (cameraType == DaHua) { //大华
        expressId = cameraData.trimmed();
    } else if (cameraType == HaiKang) { //海康
        //排除Noread和空车
        if (cameraData.contains("NoRead", Qt::CaseInsensitive) ||
                cameraData.length() < 3 || !cameraData.startsWith('[')) {
            return;
        }

        //去掉]后面的连包（也可以做连包处理）
        cameraData = cameraData.section(']', 0, 0);
        cameraData.remove("[");
        expressId = cameraData.trimmed();
    }

    if (!isLetterOrNumberString(expressId))
        return;

    short boxId = expressIdBoxIdHash.value(expressId);
    carrierId[cameraId]++;
    if (carrierId[cameraId] > carrierNum)
        carrierId[cameraId] = 1;
    qDebug() << "测试：解析数据" << QString("相机%1，快递号%2，小车号%3，格口号%4")
                .arg(cameraId).arg(expressId).arg(carrierId[cameraId]).arg(boxId) << "\r\n";
    //写入PLC
    plc->writeBoxId(expressId, carrierId[cameraId], boxId, cameraIndex);
    //写入数据库
    //emit appendDetailLog(QString("函数：%1，行数：%2").arg(__func__).arg(__LINE__));
    QString insertSql = QString(
          "INSERT INTO %1"
          "(EXPRESSFLOW_ID, STATUS, EXPRESS_SN, BOX_NUM, DELIVER_TYPE, CREATE_TIME, User_id) "
          "VALUES('%2', 0, '%2', '%3', 0, '%4', 'DbTest') "
          "ON DUPLICATE KEY "
          "UPDATE BOX_NUM = '%3'")
          .arg(NATIVE_DB_STO_COME).arg(expressId).arg(boxId)
          .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    //emit appendDetailLog(QString("函数：%1，行数：%2").arg(__func__).arg(__LINE__));
    //emit appendDetailLog(insertSql);
    //将结果插入数据库
    //QSqlQuery query(insertSql, db);
    //emit appendDetailLog(QString("函数：%1，行数：%2").arg(__func__).arg(__LINE__));
    //return query.numRowsAffected();
    //显示到界面
    testUI->showData(expressId, QString::number(carrierId[cameraId]),
                     QString::number(boxId), camera);
}

