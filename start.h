#ifndef START_H
#define START_H

#include <QDialog>

namespace Ui {
class Start;
}

class Start : public QDialog
{
    Q_OBJECT

public:
    explicit Start(QWidget *parent = nullptr);
    ~Start();

signals:
    void come();
    void go();

private:
    Ui::Start *ui;
};

#endif // START_H
