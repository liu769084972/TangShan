#include "sortui.h"

SortUI::SortUI(bool _comeOrGo, QWidget *parent)
    : QFrame (parent)
{
    setFrameShape(QFrame::Box);
    calSortData = new CalculateSortDataUI(_comeOrGo);
    sortData = new SortDataShowUI;

    layoutMain = new QVBoxLayout;
    layoutMain->addWidget(calSortData);
    layoutMain->addWidget(sortData);

    setLayout(layoutMain);
}
