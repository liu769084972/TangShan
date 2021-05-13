#include "sortdatashowui.h"
#include "Configure/configure.h"

#include <QFormLayout>
#include <QGroupBox>
#include <QGridLayout>

SortDataShowUI::SortDataShowUI(QWidget *parent)
    : QFrame (parent)
{
    int num = Configure::instance()->camerasVector().length();
    sortDataWidgetVector.resize(num);

    setUI();
}

void SortDataShowUI::setUI()
{
    QVector<StructCamera> & cameras = Configure::instance()->camerasVector();
    QGridLayout *layoutMain = new QGridLayout;

    for (int i = 0; i < cameras.size(); ++i) {
        QGroupBox *groupBox = new QGroupBox();
        groupBox->setTitle(cameras[i].cameraName); //群组框标题
        QVBoxLayout *layout = new QVBoxLayout(); //群组框布局

        SortDataWidget *structWidget = new SortDataWidget;

        //快递
        QFormLayout *layoutExpressId = new QFormLayout();
        structWidget->editExpressId = new QLineEdit;
        layoutExpressId->addRow("快递编号：", structWidget->editExpressId);
        layout->addLayout(layoutExpressId);

        //小车
        QFormLayout *layoutCarrierId = new QFormLayout();
        structWidget->editCarrierId = new QLineEdit;
        layoutCarrierId->addRow("小车编号：", structWidget->editCarrierId);
        layout->addLayout(layoutCarrierId);

        //格口
        QFormLayout *layoutBoxId = new QFormLayout();
        structWidget->editBoxId = new QLineEdit;
        layoutBoxId->addRow("格口编号：", structWidget->editBoxId);
        layout->addLayout(layoutBoxId);

        //伸展器
        layout->addStretch();

        //视觉连接状态标签
        QFormLayout *layoutCameraStatus = new QFormLayout();
        structWidget->status = new QLabel;
        layoutBoxId->addRow("相机状态：", structWidget->status);
        layout->addLayout(layoutCameraStatus);

        //连接视觉按钮
        structWidget->buttonConnect = new QPushButton("连接");
        structWidget->buttonClose = new QPushButton("关闭");
        connect(structWidget->buttonConnect, &QPushButton::clicked,
                this, [=]() { emit buttonCameraClicked(cameras[i].cameraId, true);});
        connect(structWidget->buttonClose, &QPushButton::clicked,
                this, [=]() { emit buttonCameraClicked(cameras[i].cameraId, false);});
        QHBoxLayout *layoutButton = new QHBoxLayout;
        layoutButton->addWidget(structWidget->buttonConnect);
        layoutButton->addWidget(structWidget->buttonClose);
        layoutButton->addStretch();
        layout->addLayout(layoutButton);

        //给群组框设置布局
        groupBox->setLayout(layout);

        //将相关控件加入数组，以备后面使用
        sortDataWidgetVector[cameras[i].cameraId] = structWidget;

        //加到到主布局器，每行两个
        layoutMain->addWidget(groupBox, i / 2, i % 2);
    }

    setLayout(layoutMain);
}

void SortDataShowUI::showData(int cameraId, QString expressId, QString carrierId, QString boxId)
{
    sortDataWidgetVector.value(cameraId)->editExpressId->setText(expressId);
    sortDataWidgetVector.value(cameraId)->editCarrierId->setText(carrierId);
    sortDataWidgetVector.value(cameraId)->editBoxId->setText(boxId);
}
