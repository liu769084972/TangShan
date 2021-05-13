#include "arrivedsendbagui.h"
#include "Configure/configure.h"
#include "define.h"

#include <QInputDialog>

ArrivedSendBagUI::ArrivedSendBagUI(bool _comerOrGo, QWidget *parent)
    : QFrame (parent), comeOrGo(_comerOrGo)
{
    setFrameShape(QFrame::Box);

    labelArrived = new QLabel("<font color='red'>到件停止</font>");
    if (comeOrGo) {
        labelSendOrBag= new QLabel("<font color='red'>发件/派件停止</font>");
    } else {
        labelSendOrBag= new QLabel("<font color='red'>集包停止</font>");
    }

    arrivedFlag = false;
    sendOrBagFlag = false;
    arrivedRows = Configure::instance()->arrivedRow();
    sendOrBagRows = Configure::instance()->sendOrBagRow();

    arrived = new QTableWidget(0, 4);
    arrived->setHorizontalHeaderLabels(
        QStringList() << "时间" << "快递编号" << "厂家" << "状态");
    sendOrBag = new QTableWidget(0, 4);
    if (comeOrGo) {
        sendOrBag->setHorizontalHeaderLabels(
            QStringList() << "时间" << "快递编号" << "厂家" << "状态");
    } else {
        sendOrBag->setHorizontalHeaderLabels(
            QStringList() << "时间" << "快递编号" << "包号" << "状态");
    }

    QBitArray ba = Configure::instance()->companySwitch();
    buttonContainerNo = new QPushButton("车签");
    connect(buttonContainerNo, &QPushButton::clicked, this, [&]()
    {
        containerNo = QInputDialog::getText(this, "车签", "输入车签");
        labelArrived->setText(QString("<font color='%1'>到件%2，圆通车签%3</font>")
            .arg(arrivedFlag ? "green" : "red").arg(arrivedFlag ? "开始" : "停止").arg(containerNo));
        emit containerNoSignal(containerNo);
    });

    buttonArrivedStart = new QPushButton("开始");
    connect(buttonArrivedStart, &QPushButton::clicked, this, [&]()
    {
        arrivedFlag = true;
        emit isArrived(true);
        QBitArray ba = Configure::instance()->companySwitch();
        if (ba[Yto]) {
            labelArrived->setText(
                QString("<font color='green'>到件开始，圆通车签%1</font>")
                .arg(containerNo));
        } else {
            labelArrived->setText(
                QString("<font color='green'>到件开始</font>"));
        }
    });

    buttonArrivedStop = new QPushButton("停止");
    connect(buttonArrivedStop, &QPushButton::clicked, this, [&]()
    {
        arrivedFlag = false;
        emit isArrived(false);
        QBitArray ba = Configure::instance()->companySwitch();
        if (ba[Yto]) {
            labelArrived->setText(
                 QString("<font color='red'>到件停止，圆通车签%1</font>")
                .arg(containerNo));
        } else {
            labelArrived->setText(
                 QString("<font color='red'>到件停止</font>"));
        }
    });

    buttonSendOrBagStart = new QPushButton("开始");
    connect(buttonSendOrBagStart, &QPushButton::clicked, this, [&]()
    {
        sendOrBagFlag = true;
        emit isSendOrPackSignal(true);
        if (comeOrGo) {
            labelSendOrBag->setText("<font color='green'>发件/派件开始</font>");
        } else {
            labelSendOrBag->setText("<font color='green'>集包开始</font>");
        }
    });

    buttonSendOrBagStop = new QPushButton("停止");
    connect(buttonSendOrBagStop, &QPushButton::clicked, this, [&]()
    {
        sendOrBagFlag = false;
        emit isSendOrPackSignal(false);
        if (comeOrGo) {
            labelSendOrBag->setText("<font color='red'>发件/派件停止</font>");
        } else {
            labelSendOrBag->setText("<font color='red'>集包停止</font>");
        }
    });

    layoutArrived = new QHBoxLayout;
    layoutSendOrBag = new QHBoxLayout;
    layoutArrivedMain = new QVBoxLayout;
    layoutSendOrBagMain = new QVBoxLayout;

    layoutArrived->addWidget(labelArrived);
    if (ba[Yto]) {
        layoutArrived->addWidget(buttonContainerNo);
    }

    layoutArrived->addWidget(buttonArrivedStart);
    layoutArrived->addWidget(buttonArrivedStop);

    layoutSendOrBag->addWidget(labelSendOrBag);
    layoutSendOrBag->addWidget(buttonSendOrBagStart);
    layoutSendOrBag->addWidget(buttonSendOrBagStop);

    layoutArrivedMain->addLayout(layoutArrived);
    layoutArrivedMain->addWidget(arrived);

    layoutSendOrBagMain->addLayout(layoutSendOrBag);
    layoutSendOrBagMain->addWidget(sendOrBag);

    layoutMain = new QVBoxLayout;
    layoutMain->addLayout(layoutArrivedMain);
    layoutMain->addLayout(layoutSendOrBagMain);

    setLayout(layoutMain);
}

//向派件/集包列表添加一项
void ArrivedSendBagUI::addItemSendOrBag(QStringList expressIdList,
        QString containerOrCompany, QString status)
{
    if (comeOrGo) {
        for (int i = 0; i < expressIdList.size(); ++i) {
            if(sendOrBag->rowCount() >= arrivedRows) {
                sendOrBag->removeRow(0); //删除第一行
            }
            int row = sendOrBag->rowCount();
            sendOrBag->insertRow(row);

            sendOrBag->setItem(row, 0, new QTableWidgetItem(
                 QDateTime::currentDateTime().toString("yyyy/MM/dd hh:mm:ss")));
            sendOrBag->setItem(row, 1, new QTableWidgetItem(expressIdList.value(i)));
            sendOrBag->setItem(row, 2, new QTableWidgetItem(containerOrCompany));
            sendOrBag->setItem(row, 3, new QTableWidgetItem(status));
        }
    } else { //集包
        //删除所有行
        for(int row = 0; row < sendOrBag->rowCount(); ++row) {
            sendOrBag->removeRow(row);
        }

        for (int i = 0; i < expressIdList.size(); ++i) {
            int row = sendOrBag->rowCount();
            sendOrBag->insertRow(row);
            sendOrBag->setItem(row, 0, new QTableWidgetItem(
                 QDateTime::currentDateTime().toString("yyyy/MM/dd hh:mm:ss")));
            sendOrBag->setItem(row, 1, new QTableWidgetItem(expressIdList.value(i)));
            sendOrBag->setItem(row, 2, new QTableWidgetItem(containerOrCompany));
            sendOrBag->setItem(row, 3, new QTableWidgetItem(status));
        }
    }

}

void ArrivedSendBagUI::addItemArrived(QString expressId, QString company, QString status)
{
    //删除第一列
    if(arrived->rowCount() >= arrivedRows) {
        arrived->removeRow(0); //删除第一行
    }
    int row = arrived->rowCount();
    arrived->insertRow(row);

    arrived->setItem(row, 0, new QTableWidgetItem(
        QDateTime::currentDateTime().toString("yyyy/MM/dd hh:mm:ss")));
    arrived->setItem(row, 1, new QTableWidgetItem(expressId));
    arrived->setItem(row, 2, new QTableWidgetItem(company));
    arrived->setItem(row, 3, new QTableWidgetItem(status));
}
