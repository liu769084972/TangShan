#ifndef SORTARRIVEDSENDBAG_H
#define SORTARRIVEDSENDBAG_H

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "Work/UI/arrivedsendbagui.h"
#include "Work/UI/devicestatusui.h"
#include "Work/UI/sortui.h"

class SortArrivedSendBag : public QWidget
{
    Q_OBJECT
public:
    explicit SortArrivedSendBag(bool _comeOrGo, QWidget *parent = nullptr);

signals:

public slots:

public:
    SortUI *sort; //分拣
    ArrivedSendBagUI *arrSndBag; //到派集包
    DeviceStatusUI *devStatus;

private:
    QHBoxLayout *layoutMain; //主布局器
};

#endif // SORTARRIVEDSENDBAG_H
