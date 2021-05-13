#ifndef CONFIGUREUI_H
#define CONFIGUREUI_H

#include <QWidget>

#include "Configure/UI/configureglobal.h"
#include "Configure/UI/configurestoui.h"
#include "Configure/UI/configureytoui.h"

class ConfigureUI : public QWidget
{
    Q_OBJECT
public:
    explicit ConfigureUI(QWidget *parent = nullptr);

signals:

public slots:

private:
    void setGlobalUi();
    void setStoUi(); //设置申通UI
    void setYtoUi(); //设置圆通UI


private:
    QVBoxLayout *layoutMain;
    //设备
    ConfigureGlobal *configureGlobal;
    //申通相关
    ConfigureSTOUI *configureStoUi;
    //圆通相关
    ConfigureYTOUI *configureYtoUi;
};

#endif // CONFIGUREUI_H
