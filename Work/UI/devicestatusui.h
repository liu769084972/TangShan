#ifndef DEVICESTATUSUI_H
#define DEVICESTATUSUI_H

#include <QFrame>
#include <QLabel>
#include <QVBoxLayout>

class DeviceStatusUI : public QFrame
{
public:
    DeviceStatusUI(QWidget *parent = nullptr);


    QLabel *labelPLC;
    QLabel *nativeDB;

    QVBoxLayout *layoutMain;

};

#endif // DEVICESTATUSUI_H
