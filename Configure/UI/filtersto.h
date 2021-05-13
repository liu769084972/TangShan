#ifndef FILTERSTO_H
#define FILTERSTO_H

#include <QGroupBox>
#include <QLineEdit>
#include <QFormLayout>
#include <QVBoxLayout>

class FilterSTO : public QGroupBox
{
    Q_OBJECT

public:
    FilterSTO(QWidget *parent = nullptr);

    QLineEdit *editComeCode1;
    QLineEdit *editComeCode2;

    QFormLayout *layoutComeCode;

    QVBoxLayout *layoutMain;

};

#endif // FILTERSTO_H
