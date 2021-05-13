#ifndef TESTUI_H
#define TESTUI_H

#include <QObject>
#include <QWidget>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QFormLayout>
#include <QTableWidget>
#include <QRadioButton>

#include "Test/testdrop.h"
#include "Test/UI/toolui.h"
#include "Test/UI/interfacedetectquery.h"
#include "log.h"

class TestUI : public QWidget
{
    Q_OBJECT
public:
    explicit TestUI(QWidget *parent = nullptr);

signals:
    void appendDetailLog(QString);

public slots:
    void buttonAddExpressIdBoxIdClicked(); //添加快递和格口编号
    void buttonDelExpressIdBoxIdClicked(); //删除快递和格口编号
    void tableShowExpressBoxIdCellChanged(int row, int); //项数据改变
    void showData(QString expressId, QString carrierId, QString boxId, QString camera);

    void createSlotSignal();
    void setTestDrop();

    void saveDataToFile();
    void loadFileToTable();

public:
    QGroupBox *groupTestDrop;
    QLineEdit *editExpressId;
    QLineEdit *editCarrierId;
    QLineEdit *editBoxId;
    QLabel *labelShowStatus;
    QLineEdit *editCamera;
    QPushButton *buttonConnect; //连接视觉按钮
    QPushButton *buttonClsoe; //关闭视觉按钮
    QPushButton *buttonAddExpressIdBoxId; //添加TestDrop::expressIdBoxIdHash添加格口和快递号
    QPushButton *buttonEditExpressIdBoxId; //编辑TestDrop::expressIdBoxIdHash添加格口和快递号
    QPushButton *buttonDelExpressIdBoxId; //删除TestDrop::expressIdBoxIdHash格口和快递号
    QPushButton *buttonClearExpressIdBoxId; //清空TestDrop::expressIdBoxIdHash格口和快递号
    QPushButton *buttonLoad; //加载保存至文件的数据
    QPushButton *buttonSave; //保存当前格口的数据
    QTableWidget *tableShowExpressBoxId; //显示已添加的快递和格口

    //布局
    QVBoxLayout *layoutMain;
    QHBoxLayout *layoutGroupTestDrop; //群组框布局-水平布局
    QVBoxLayout *layoutShowData; //展示数据区的布局
    QFormLayout *layoutExpressId;
    QFormLayout *layoutCarrierId;
    QFormLayout *layoutBoxId;
    QFormLayout *layoutCamera;
    QHBoxLayout *layoutButton;
    QHBoxLayout *layoutTableShowExpressBoxId;
    QVBoxLayout *layoutButtonExpressBoxId;

    //单选按钮
    QRadioButton *radioExpressId;
    QRadioButton *radioCarrierId;

    //测试落格主体类
    TestDrop *testDrop;

    //指定小车和不指定小车
    bool isComoCarrierIdSelected = false;


    //工具栏
    ToolUI *toolUi;
    //接口检测栏
    InterfaceDetectQuery *interDetectQuery;

    Log *log;
};

#endif // TESTUI_H
