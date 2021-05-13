#include "devicestatusui.h"

DeviceStatusUI::DeviceStatusUI(QWidget *parent)
    : QFrame (parent)
{
    labelPLC = new QLabel("PLC状态");
    nativeDB = new QLabel("本地数据库");

    layoutMain = new QVBoxLayout;
    layoutMain->addWidget(labelPLC);
    layoutMain->addWidget(nativeDB);

    setLayout(layoutMain);

    setFrameShape(QFrame::Box);
}
