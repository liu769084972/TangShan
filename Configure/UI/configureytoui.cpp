#include "configureytoui.h"
#include "Configure/configure.h"

ConfigureYTOUI::ConfigureYTOUI(QWidget *parent)
    : QGroupBox ("圆通", parent)
{
    setCheckable(true);
    setChecked(false);

    layoutMain = new QHBoxLayout;

    //网关
    layoutGateway = new QFormLayout;

    editGatewayIP = new QLineEdit; //网关IP
    editGatewayIP->setText(Configure::instance()->configureYTO->ip());
    connect(editGatewayIP, &QLineEdit::textChanged, this, [=]()
    {
        QString text = editGatewayIP->text();
        Configure::instance()->configureYTO->setConfigure("YTSERVER/ip", text);
    });

    editGatewayUploadPort = new QLineEdit; //扫描端口
    editGatewayUploadPort->setText(QString::number(Configure::instance()->configureYTO->portUpload()));
    connect(editGatewayUploadPort, &QLineEdit::textChanged, this, [=]()
    {
        QString text = editGatewayUploadPort->text();
        Configure::instance()->configureYTO->setConfigure("YTSERVER/portUpload", text.toUShort());
    });

    editGatewayCodePort = new QLineEdit; //三段码端口
    editGatewayCodePort->setText(QString::number(Configure::instance()->configureYTO->portCode()));
    connect(editGatewayCodePort, &QLineEdit::textChanged, this, [=]()
    {
        QString text = editGatewayCodePort->text();
        Configure::instance()->configureYTO->setConfigure("YTSERVER/portCode", text.toUShort());
    });

    layoutGateway->addRow("网关IP", editGatewayIP);
    layoutGateway->addRow("扫描端口", editGatewayUploadPort);
    layoutGateway->addRow("三段码端口", editGatewayCodePort);

    layoutMain->addLayout(layoutGateway);

    //签名
    layoutSignature = new QFormLayout;
    editClientMacSignature = new QLineEdit; //客户端mac地址（签名）
    editClientMacSignature->setText(Configure::instance()->configureYTO->signatureArg().value("clientMac"));
    connect(editClientMacSignature, &QLineEdit::textChanged, this, [=]()
    {
        QString text = editClientMacSignature->text();
        Configure::instance()->configureYTO->setConfigure("SIGNATURE/clientMac", text);
    });

    editClientIpSignature = new QLineEdit; //客户端IP地址（签名）
    editClientIpSignature->setText(Configure::instance()->configureYTO->signatureArg().value("clientIp"));
    connect(editClientIpSignature, &QLineEdit::textChanged, this, [=]()
    {
        QString text = editClientIpSignature->text();
        Configure::instance()->configureYTO->setConfigure("SIGNATURE/clientIp", text);
    });

    editUserNameSignature = new QLineEdit; //登录用户名（签名）
    editUserNameSignature->setText(Configure::instance()->configureYTO->signatureArg().value("userName"));
    connect(editUserNameSignature, &QLineEdit::textChanged, this, [=]()
    {
        QString text = editUserNameSignature->text();
        Configure::instance()->configureYTO->setConfigure("SIGNATURE/userName", text);
    });

    editpasswordSignature = new QLineEdit; //登录用户密码（签名）
    editpasswordSignature->setText(Configure::instance()->configureYTO->signatureArg().value("password"));
    connect(editpasswordSignature, &QLineEdit::textChanged, this, [=]()
    {
        QString text = editpasswordSignature->text();
        Configure::instance()->configureYTO->setConfigure("IGNATURE/password", text);
    });

    editCameraMacSignature = new QLineEdit; //相机MAC地址（签名）
    editCameraMacSignature->setText(Configure::instance()->configureYTO->signatureArg().value("cameraMac"));
    connect(editCameraMacSignature, &QLineEdit::textChanged, this, [=]()
    {
        QString text = editCameraMacSignature->text();
        Configure::instance()->configureYTO->setConfigure("SIGNATURE/clientMac", text);
    });

    editClientIdSignature = new QLineEdit; //客户端编号（签名）
    editClientIdSignature->setText(Configure::instance()->configureYTO->signatureArg().value("clientId"));
    connect(editClientIdSignature, &QLineEdit::textChanged, this, [=]()
    {
        QString text = editClientIdSignature->text();
        Configure::instance()->configureYTO->setConfigure("", text);
    });

    layoutSignature->addRow("客户端mac地址（签名）", editClientMacSignature);
    layoutSignature->addRow("客户端IP地址（签名）", editClientIpSignature);
    layoutSignature->addRow("登录用户名（签名）", editUserNameSignature);
    layoutSignature->addRow("登录用户密码（签名）", editpasswordSignature);
    layoutSignature->addRow("客户端编号（签名）", editClientIdSignature);

    layoutMain->addLayout(layoutSignature);

    //分拣
    layoutSort = new QFormLayout;
    editCreateUserCode = new QLineEdit; //系统操作员工编码
    editCreateUserCode->setText(Configure::instance()->configureYTO->createUserCode());
    connect(editCreateUserCode, &QLineEdit::textChanged, this, [=]()
    {
        QString text = editCreateUserCode->text();
        Configure::instance()->configureYTO->setConfigure("PUBLIC/createUserName", text);
    });
    layoutSignature->addRow("系统操作员工编码", editCreateUserCode);

    layoutMain->addLayout(layoutSort);

    //到件
    layoutArrived = new QFormLayout;

    comboSource = new QComboBox;
    comboSource->addItem("相机");
    comboSource->addItem("数据库");
    comboSource->setCurrentIndex(
       Configure::instance()->configureYTO->arrivedSource());
    connect(comboSource, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=](int index)
    {
        Configure::instance()->configureYTO->setConfigure("ARRIVED/source", index);
    });
    layoutArrived->addRow("到件数据来源:", comboSource);

    editNextOrgCode = new QLineEdit; //下一网点（拆包地）
    editNextOrgCode->setText(Configure::instance()->configureYTO->nextOrgCode());
    connect(editNextOrgCode, &QLineEdit::textChanged, this, [=]()
    {
        QString text = editNextOrgCode->text();
        Configure::instance()->configureYTO->setConfigure("ARRIVED/nextOrgCode", text);
    });

    editOrgCode = new QLineEdit; //操作网点-当前操作网点
    editOrgCode->setText(Configure::instance()->configureYTO->orgCode());
    connect(editOrgCode, &QLineEdit::textChanged, this, [=]()
    {
        QString text = editOrgCode->text();
        Configure::instance()->configureYTO->setConfigure("ARRIVED/orgCode", text);
    });

    editPreviousOrgCode = new QLineEdit; //上一网点
    editPreviousOrgCode->setText(Configure::instance()->configureYTO->previousOrgCode());
    connect(editPreviousOrgCode, &QLineEdit::textChanged, this, [=]()
    {
        QString text = editPreviousOrgCode->text();
        Configure::instance()->configureYTO->setConfigure("ARRIVED/previousOrgCode", text);
    });

    editSourceOrgCode = new QLineEdit; //始发网点-当前操作网点
    editSourceOrgCode->setText(Configure::instance()->configureYTO->sourceOrgCode());
    connect(editSourceOrgCode, &QLineEdit::textChanged, this, [=]()
    {
        QString text = editSourceOrgCode->text();
        Configure::instance()->configureYTO->setConfigure("ARRIVED/sourceOrgCode", text);
    });

    editLine = new QLineEdit; //场地流水线
    editLine->setText(Configure::instance()->configureYTO->line());
    connect(editLine, &QLineEdit::textChanged, this, [=]()
    {
        QString text = editLine->text();
        Configure::instance()->configureYTO->setConfigure("ARRIVED/line", text);
    });

    editPadPort = new QLineEdit;
    editPadPort->setText(QString::number(Configure::instance()->configureYTO->padPort()));
    connect(editPadPort, &QLineEdit::textChanged, this, [=]()
    {
        QString text = editPadPort->text();
        Configure::instance()->configureYTO->setConfigure("PACK/padPort", text);
    });

    layoutArrived->addRow("下一网点（拆包地）", editNextOrgCode);
    layoutArrived->addRow("当前操作网点", editOrgCode);
    layoutArrived->addRow("上一网点", editPreviousOrgCode);
    layoutArrived->addRow("始发网点", editSourceOrgCode);
    layoutArrived->addRow("场地流水线号", editLine);
    layoutArrived->addRow("集包端口:", editPadPort);

    layoutMain->addLayout(layoutArrived);

    setLayout(layoutMain);
}
