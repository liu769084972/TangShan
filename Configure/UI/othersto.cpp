#include "othersto.h"

OtherSto::OtherSto(QWidget *parent) : QWidget(parent)
{
    table = new QTableWidget(0, 2);
    table->setHorizontalHeaderLabels(QStringList() << "格口" << "值");
    combo = new QComboBox;
    combo->addItem("扫描员");
    combo->addItem("派件员");
    combo->addItem("快递类型");
    buttonLoad = new QPushButton("加载");
    buttonSave = new QPushButton("保存");
    buttonAdd = new QPushButton("添加");
    buttonDel = new QPushButton("删除");
    layoutOperate = new QHBoxLayout;
    layoutOperate->addWidget(combo);
    layoutOperate->addWidget(buttonLoad);
    layoutOperate->addWidget(buttonSave);
    layoutOperate->addWidget(buttonAdd);
    layoutOperate->addWidget(buttonDel);
    layoutMain = new QVBoxLayout;
    layoutMain->addLayout(layoutOperate);
    layoutMain->addWidget(table);
    setLayout(layoutMain);
}
