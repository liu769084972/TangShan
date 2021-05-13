#include "calculatesortdataui.h"
#include "define.h"
#include "Configure/configure.h"

#include <QFormLayout>

CalculateSortDataUI::CalculateSortDataUI(bool _comeOrGo, QWidget *parent)
    :QFrame (parent)
{
    comeOrGo = _comeOrGo;
    if (comeOrGo) {
        baCompany = Configure::instance()->companySwitchCome();
    } else {
        baCompany = Configure::instance()->companySwitchGo();
    }
    //设置存放标签数组的大小
    labelVector.resize(COMPANYNUM + 1); //8个公司

    labelTotalNum = new QLabel("数量"); //显示数量

    setcalculateLabel();
}

void CalculateSortDataUI::setcalculateLabel()
{  
    mainLayout = new QHBoxLayout();
    if (baCompany[Sto]) {
        labelVector[Sto] = new QLabel("数量");
        QFormLayout *layout = new QFormLayout();
        layout->addRow("申通：", labelVector[Sto]);
        mainLayout->addLayout(layout);
    }
    if (baCompany[Zto]) {
        labelVector[Zto] = new QLabel("数量");
        QFormLayout *layout = new QFormLayout();
        layout->addRow("中通：", labelVector[Zto]);
        mainLayout->addLayout(layout);
    }
    if (baCompany[Yto]) {
        labelVector[Yto] = new QLabel("数量");
        QFormLayout *layout = new QFormLayout();
        layout->addRow("圆通：", labelVector[Yto]);
        mainLayout->addLayout(layout);
    }
    if (baCompany[Best]) {
        labelVector[Best] = new QLabel("数量");
        QFormLayout *layout = new QFormLayout();
        layout->addRow("百世：", labelVector[Best]);
        mainLayout->addLayout(layout);
    }
    if (baCompany[Tt]) {
        labelVector[Tt] = new QLabel("数量");
        QFormLayout *layout = new QFormLayout();
        layout->addRow("天天：", labelVector[Tt]);
        mainLayout->addLayout(layout);
    }
    if (baCompany[Yunda]) {
        labelVector[Yunda] = new QLabel("数量");
        QFormLayout *layout = new QFormLayout();
        layout->addRow("韵达：", labelVector[Yunda]);
        mainLayout->addLayout(layout);
    }
    if (baCompany[Youzheng]) {
        labelVector[Youzheng] = new QLabel("数量");
        QFormLayout *layout = new QFormLayout();
        layout->addRow("邮政：", labelVector[Youzheng]);
        mainLayout->addLayout(layout);
    }

    //添加总量和异常
    labelVector[Exception] = new QLabel("数量");
    QFormLayout *layoutException = new QFormLayout();
    layoutException->addRow("异常：", labelVector[Exception]);
    mainLayout->addLayout(layoutException);

    QFormLayout *layoutTotal = new QFormLayout();
    layoutTotal->addRow("总量：", labelTotalNum);
    mainLayout->addLayout(layoutTotal);

    setLayout(mainLayout);
}

void CalculateSortDataUI::calculate(int scanNum, int indexCompany)
{
    if (scanNum == 1) {
        int num = labelVector.value(indexCompany)->text().toInt();
        labelVector.value(indexCompany)->setText(QString::number(++num));

        labelTotalNum->setText(QString::number(++total));
    }
}
