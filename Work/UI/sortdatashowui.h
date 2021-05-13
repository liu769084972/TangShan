#ifndef SORTDATASHOWUI_H
#define SORTDATASHOWUI_H

#include <QFrame>
#include <QLabel>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>

typedef struct {
    QLineEdit *editExpressId = nullptr;
    QLineEdit *editCarrierId  = nullptr;
    QLineEdit *editBoxId  = nullptr;

    QLabel *status  = nullptr;
    QPushButton *buttonConnect = nullptr;
    QPushButton *buttonClose = nullptr;

} SortDataWidget;

class SortDataShowUI : public QFrame
{
    Q_OBJECT

public:
    SortDataShowUI(QWidget *parent = nullptr);

    void setUI();

    QVector<SortDataWidget *> sortDataWidgetVector;

    void showData(int cameraId, QString expressId, QString carrierId, QString boxId); //显示分拣数据

signals:
    void buttonCameraClicked(int indexCamera, bool isOpen);
};

#endif // SORTDATASHOWUI_H
