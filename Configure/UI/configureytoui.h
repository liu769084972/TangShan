#ifndef CONFIGUREYTOUI_H
#define CONFIGUREYTOUI_H

#include <QGroupBox>
#include <QLineEdit>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QComboBox>

class ConfigureYTOUI : public QGroupBox
{
public:
    ConfigureYTOUI(QWidget *parent = nullptr);

    QHBoxLayout *layoutMain;

    //网关
    QFormLayout *layoutGateway;
    QLineEdit *editGatewayIP; //网关IP
    QLineEdit *editGatewayUploadPort; //扫描端口
    QLineEdit *editGatewayCodePort; //三段码端口

    //分拣
    QFormLayout *layoutSort;
    QLineEdit *editCreateUserCode; //系统操作员工编码

    //签名
    QFormLayout *layoutSignature;
    QLineEdit *editClientMacSignature; //客户端mac地址（签名）
    QLineEdit *editClientIpSignature; //客户端IP地址（签名）
    QLineEdit *editUserNameSignature; //登录用户名（签名）
    QLineEdit *editpasswordSignature; //登录用户密码（签名）
    QLineEdit *editCameraMacSignature; //相机MAC地址（签名）
    QLineEdit *editClientIdSignature; //客户端编号（签名）

    //到件
    QFormLayout *layoutArrived;
    QComboBox *comboSource; //到件来源
    QLineEdit *editNextOrgCode; //下一网点（拆包地）
    QLineEdit *editOrgCode; //操作网点-当前操作网点
    QLineEdit *editPreviousOrgCode; //上一网点
    QLineEdit *editSourceOrgCode; //始发网点-当前操作网点
    QLineEdit *editLine; //场地流水线

    //集包
    QLineEdit *editPadPort; //把枪集包端口
};

#endif // CONFIGUREYTOUI_H
