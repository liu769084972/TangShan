#ifndef CONFIGURESTOUI_H
#define CONFIGURESTOUI_H

#include <QGroupBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QComboBox>
#include <QPushButton>

class ConfigureSTOUI : public QGroupBox
{
public:
    ConfigureSTOUI(QWidget *parent = nullptr);

    QHBoxLayout *layoutMain;

    QFormLayout *layout1;
    QLineEdit *editAlyIP;
    QLineEdit *editAlyUserName;
    QLineEdit *editAlyPassword;

    QFormLayout *layout2; //编辑框布局
    QLineEdit *editPdaCode; //终端设备CODE
    QLineEdit *editOpTerminal; //设备唯一码
    QLineEdit *editOrgCode; //操作组织编码
    QLineEdit *editUserCode; //操作员工编码

    QFormLayout *layout3; //编辑框布局
    QComboBox *comboArrivedSource; //到件数据来源
    QLineEdit *editPadPort; //网关端口
    QLineEdit *editNextOrgCode; //下一站编码

    //规则相关
    QVBoxLayout *layout4;
    QPushButton *buttonRule;
    QPushButton *buttonCode;
    QPushButton *buttonOther;

private:
    void connectSignalSlot();
};

#endif // CONFIGURESTOUI_H
