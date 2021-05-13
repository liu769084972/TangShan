#include "alarmui.h"

#include <QDateTime>

AlarmUI::AlarmUI(QWidget *parent) : QWidget(parent)
{
    tableShowAlarm = new QTableWidget;
    tableShowAlarm->setColumnCount(4);
    headerviewerComboBox = new HeaderViewComboBox(Qt::Horizontal);
    tableShowAlarm->setHorizontalHeader(headerviewerComboBox);
    tableShowAlarm->setHorizontalHeaderItem(0, new QTableWidgetItem("时间"));
    tableShowAlarm->setHorizontalHeaderItem(1, new QTableWidgetItem("内容"));
    layoutMain = new QHBoxLayout;
    layoutMain->addWidget(tableShowAlarm);

    setLayout(layoutMain);
}

void AlarmUI::appendMessage(QString message, QString level, QString type)
{
    int row = tableShowAlarm->rowCount();
    //插入一行
    tableShowAlarm->insertRow(row);

    QTableWidgetItem *time = new QTableWidgetItem(
        QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    tableShowAlarm->setItem(row, 0, time);

    QTableWidgetItem *itemMessage = new QTableWidgetItem(message);
    tableShowAlarm->setItem(row, 1, itemMessage);

    QTableWidgetItem *itemLevel = new QTableWidgetItem(level);
    tableShowAlarm->setItem(row, 2, itemLevel);

    QTableWidgetItem *itemType = new QTableWidgetItem(type);
    tableShowAlarm->setItem(row, 3, itemType);
}
