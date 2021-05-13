#ifndef CAMERA_H
#define CAMERA_H

#include <QObject>
#include <QTcpSocket>

#include "express.h"
#include "plc.h"
#include "log.h"
#include "mainwindow.h"

class Camera : public QObject
{
    Q_OBJECT

public:
    explicit Camera(bool _comeOrGo, MainWindow *m1, QObject *parent = nullptr);

signals:
    void appendDetailLog(QString);
    void appendAlarm(QString content, QString level, QString component);

public slots:
    void openCamera(int cameraId, bool isOpen);

private slots:
    void readCamera();
    void cameraConnectSuccess();
    void cameraError();

private:
    void initCamera();
    /*
     *  目的：根据camera地址找到cameraId
     */
    int cameraIdFromCamera(QTcpSocket *tcpSocket);

    inline bool isLetterOrNumberString(QString str);

private:
    //视觉
    Express *express;
    PLC *plc;
    Log *log;
    MainWindow *m;
    bool comeOrGo = true;
};

#endif // CAMERA_H
