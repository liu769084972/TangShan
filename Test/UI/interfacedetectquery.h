#ifndef INTERFACEDETECTQUERY_H
#define INTERFACEDETECTQUERY_H

#include <QGroupBox>
#include <QTextEdit>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>

class InterfaceDetectQuery : public QGroupBox
{
    Q_OBJECT
public:
    explicit InterfaceDetectQuery(QWidget *parent = nullptr);

signals:

public slots:
    void querySTO(QString expressId);
    void queryYTO(QString expressId);

private:
    short getBestBoxId(QString code);

private:
    QTextEdit *textEdit;
    QLineEdit *editExpressId;
    QComboBox *comboCompany;
    QComboBox *comboComeOrGo;
    QPushButton *button;
    QPushButton *buttonClear;
    QVBoxLayout *layout1;
    QHBoxLayout *layout2;
    QFormLayout *layout3;
};

#endif // INTERFACEDETECTQUERY_H
