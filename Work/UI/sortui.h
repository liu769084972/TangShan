#ifndef SORTUI_H
#define SORTUI_H

#include <QFrame>
#include <QVBoxLayout>

#include "Work/UI/calculatesortdataui.h"
#include "Work/UI/sortdatashowui.h"

class SortUI : public QFrame
{
public:
    SortUI(bool _comeOrGo, QWidget *parent = nullptr);

    CalculateSortDataUI *calSortData; //计数
    SortDataShowUI *sortData; //分拣数据

private:
    QVBoxLayout *layoutMain;
};

#endif // SORTUI_H
