#ifndef FILTERYTO_H
#define FILTERYTO_H

#include <QGroupBox>
#include <QLineEdit>
#include <QFormLayout>
#include <QVBoxLayout>

class FilterYTO : public QGroupBox
{
    Q_OBJECT
public:
    explicit FilterYTO(QWidget *parent = nullptr);

signals:

public slots:

private:
    QLineEdit *editComeCode1;
    QLineEdit *editComeCode2;

    QFormLayout *layoutComeCode;

    QVBoxLayout *layoutMain;
};

#endif // FILTERYTO_H
