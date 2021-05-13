#ifndef CALCULATESORTDATAUI_H
#define CALCULATESORTDATAUI_H

#include <QFrame>
#include <QLabel>
#include <QHBoxLayout>
#include <QBitArray>

class CalculateSortDataUI : public QFrame
{
    Q_OBJECT

public:
    CalculateSortDataUI(bool _comeOrGo, QWidget *parent = nullptr);

    //根据视觉数量设置标签数量
    void setcalculateLabel();

    void calculate(int scanNum, int indexCompany);


    //保存Label，以备后面调用
    QVector<QLabel *> labelVector;

    QLabel *labelTotalNum; //显示数量
    QHBoxLayout *mainLayout; //主布局器

private:
    bool comeOrGo;
    QBitArray baCompany;
    int total = 0;
};

#endif // CALCULATESORTDATAUI_H
