#include "sortarrivedsendbag.h"


SortArrivedSendBag::SortArrivedSendBag(bool _comeOrGo, QWidget *parent) : QWidget(parent)
{
    sort = new SortUI(_comeOrGo);
    arrSndBag = new ArrivedSendBagUI(_comeOrGo);
    devStatus = new DeviceStatusUI;

    layoutMain = new QHBoxLayout;
    layoutMain->addWidget(sort);
    layoutMain->addWidget(arrSndBag);
    //layoutMain->addWidget(devStatus);

    setLayout(layoutMain);
}
