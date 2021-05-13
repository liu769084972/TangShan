#ifndef ARRIVEDSENDBAG_H
#define ARRIVEDSENDBAG_H

#include <QFrame>
#include <QListWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>

class ArrivedSendBagUI : public QFrame
{
    Q_OBJECT
public:   
    ArrivedSendBagUI(bool _comerOrGo, QWidget *parent = nullptr);
    void addItemArrived(QString expressId, QString Company, QString status); //向到件列表添加一项
    void addItemSendOrBag(QStringList expressIdList, QString containerOrCompany, QString status); //向派件/集包列表添加一项

signals:
    void containerNoSignal(QString _containerNo);
    void isSendOrPackSignal(bool valid); //是否集包或者派件
    void isArrived(bool valid); //是否到件

public:
    QTableWidget *arrived = nullptr;
    QTableWidget *sendOrBag = nullptr;

    QLabel *labelArrived = nullptr;
    QPushButton *buttonContainerNo;
    QPushButton *buttonArrivedStart;
    QPushButton *buttonArrivedStop;

    QLabel *labelSendOrBag= nullptr;
    QPushButton *buttonSendOrBagStart;
    QPushButton *buttonSendOrBagStop;

    QHBoxLayout *layoutArrived = nullptr;
    QHBoxLayout *layoutSendOrBag = nullptr;

    QVBoxLayout *layoutArrivedMain = nullptr;
    QVBoxLayout *layoutSendOrBagMain = nullptr;
    QVBoxLayout *layoutMain = nullptr;

    int arrivedRows;
    int sendOrBagRows;

    bool arrivedFlag = false;
    bool sendOrBagFlag = false;

    QString containerNo; //车签，每天一输入

    bool comeOrGo;
};

#endif // ARRIVEDSENDBAG_H
