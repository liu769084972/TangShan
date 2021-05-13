#include "configureui.h"
#include "Configure/configure.h"
#include "define.h"

ConfigureUI::ConfigureUI(QWidget *parent) : QWidget(parent)
{    
    layoutMain = new QVBoxLayout;
    //以进港为主，决定UI（因为进港一般可以多个，而出港只有一个   
    setGlobalUi();
    QBitArray ba = Configure::instance()->companySwitchCome();
    if (ba[Sto])
        setStoUi();
    if (ba[Yto])
        setYtoUi();

    setLayout(layoutMain);
}

void ConfigureUI::setGlobalUi()
{
    configureGlobal = new ConfigureGlobal;
    layoutMain->addWidget(configureGlobal);
}

void ConfigureUI::setStoUi()
{
    configureStoUi = new ConfigureSTOUI;
    layoutMain->addWidget(configureStoUi);
}

void ConfigureUI::setYtoUi()
{
    configureYtoUi = new ConfigureYTOUI;
    layoutMain->addWidget(configureYtoUi);
}
