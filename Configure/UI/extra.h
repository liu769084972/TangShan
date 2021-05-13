#ifndef EXTRA_H
#define EXTRA_H

#include <QWidget>
#include <QFormLayout>
#include <QLineEdit>
#include <QTableWidget>
#include <QComboBox>

class Extra : public QWidget
{
    Q_OBJECT
public:
    explicit Extra(QWidget *parent = nullptr);

signals:

public slots:

private:
    QLineEdit *editUrban;
    QTableWidget *table;
    QFormLayout *layout;
    QComboBox *combo;
};

#endif // EXTRA_H
