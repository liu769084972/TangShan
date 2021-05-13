#ifndef OTHERSTO_H
#define OTHERSTO_H

#include <QWidget>
#include <QTableWidget>
#include <QComboBox>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>

class OtherSto : public QWidget
{
    Q_OBJECT
public:
    explicit OtherSto(QWidget *parent = nullptr);

signals:

public slots:

private:
    QTableWidget *table;
    QComboBox *combo;
    QPushButton *buttonLoad;
    QPushButton *buttonSave;
    QPushButton *buttonAdd;
    QPushButton *buttonDel;
    QHBoxLayout *layoutOperate;
    QVBoxLayout *layoutMain;
};

#endif // OTHERSTO_H
