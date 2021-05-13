#ifndef ALARMUI_H
#define ALARMUI_H

#include <QWidget>
#include <QHBoxLayout>
#include <QTableWidget>

#include "Alarm/headerviewcombobox.h"

class AlarmUI : public QWidget
{
    Q_OBJECT
public:
    explicit AlarmUI(QWidget *parent = nullptr);

    void appendMessage(QString message, QString level, QString type);
signals:

public slots:


public:
    QHBoxLayout *layoutMain;

    QTableWidget *tableShowAlarm;

    HeaderViewComboBox *headerviewerComboBox;
};

#endif // ALARMUI_H
