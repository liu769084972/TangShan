#include "configurestoui.h"
#include "Configure/configure.h"
#include "Configure/UI/filtersto.h"
#include "Configure/UI/rule.h"
#include "Configure/UI/othersto.h"

ConfigureSTOUI::ConfigureSTOUI(QWidget *parent)
    : QGroupBox ("申通", parent)
{
    setCheckable(true);
    setChecked(false);

    layoutMain = new QHBoxLayout;

    //阿里云
    layout1 = new QFormLayout;

    editAlyIP = new QLineEdit;
    editAlyIP->setText(Configure::instance()->configureSTO->alyIP());
    connect(editAlyIP, &QLineEdit::textChanged, this, [=]()
    {
        QString text = editAlyIP->text().trimmed();
        Configure::instance()->configureSTO->setConfigure("ALY/ip", text);
    });
    layout1->addRow("阿里云IP:", editAlyIP);

    editAlyUserName = new QLineEdit;
    editAlyUserName->setText(Configure::instance()->configureSTO->alyUserName());
    connect(editAlyUserName, &QLineEdit::textChanged, this, [=]()
    {
        QString text = editAlyUserName->text().trimmed();
        Configure::instance()->configureSTO->setConfigure("ALY/userName", text);
    });
    layout1->addRow("阿里云用户名:", editAlyUserName);

    editAlyPassword = new QLineEdit;
    editAlyPassword->setText(Configure::instance()->configureSTO->alyPassword());
    editAlyPassword->setEchoMode(QLineEdit::Password);
    connect(editAlyPassword, &QLineEdit::textChanged, this, [=]()
    {
        QString text = editAlyPassword->text().trimmed();
        Configure::instance()->configureSTO->setConfigure("ALY/password", text);
    });
    layout1->addRow("阿里云密码:", editAlyPassword);

    //layoutMain->addLayout(layout1);

    //公共部分
    layout2 = new QFormLayout;

    editPdaCode = new QLineEdit;
    layout2->addRow("终端设备CODE:", editPdaCode);
    editPdaCode->setText(Configure::instance()->configureSTO->pdaCode());
    connect(editPdaCode, &QLineEdit::textChanged, this, [=]()
    {
        QString text = editPdaCode->text().trimmed();
        Configure::instance()->configureSTO->setConfigure("PUBLIC/pdaCode", text);
    });

    editOpTerminal = new QLineEdit;
    layout2->addRow("设备唯一码:", editOpTerminal);
    editOpTerminal->setText(Configure::instance()->configureSTO->opTerminal());
    connect(editOpTerminal, &QLineEdit::textChanged, this, [=]()
    {
        QString text = editOpTerminal->text().trimmed();
        Configure::instance()->configureSTO->setConfigure("PUBLIC/opTerminal", text);
    });

    editOrgCode = new QLineEdit; //操作组织编码
    //layout2->addRow("操作组织编码：", editOrgCode);
    editOrgCode->setText(Configure::instance()->configureSTO->orgCode());
    connect(editOrgCode, &QLineEdit::textChanged, this, [=]()
    {
        QString text = editOrgCode->text().trimmed();
        Configure::instance()->configureSTO->setConfigure("PUBLIC/orgCode", text);
    });

    layoutMain->addLayout(layout2);

    //editUserCode = new QLineEdit; //操作员工编码
    //layoutPublic->addRow("操作员工编码：", editUserCode);
    //editUserCode->setText(Configure::instance()->configureSTO->userCode());
//    connect(editUserCode, &QLineEdit::textChanged, this, [=]()
//    {
//        QString text = editUserCode->text().trimmed();
//        Configure::instance()->configureSTO->setConfigure("PUBLIC/userCode", text);
//    });

    //私有部分
    layout3 = new QFormLayout;

    comboArrivedSource = new QComboBox;
    //comboArrivedSource->addItem("相机");
    comboArrivedSource->addItem("数据库");
//    comboArrivedSource->setCurrentIndex(
//       Configure::instance()->configureSTO->arrivedSource());
    connect(comboArrivedSource, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [=](int index)
    {
         Configure::instance()->configureSTO->setConfigure("ARRIVED/source", index);
    });
    layout3->addRow("到件数据来源:", comboArrivedSource);

    editNextOrgCode = new QLineEdit;
    //layout3->addRow("下一站编码:", editNextOrgCode);
    editNextOrgCode->setText(Configure::instance()->configureSTO->nextOrgCode());
    connect(editNextOrgCode, &QLineEdit::textChanged, this, [=]()
    {
        QString text = editNextOrgCode->text().trimmed();
        Configure::instance()->configureSTO->setConfigure("PACK/nextOrgCode", text);
    });

    editPadPort = new QLineEdit;
    layout3->addRow("集包网关端口:", editPadPort);
    editPadPort->setText(QString::number(Configure::instance()->configureSTO->pdaPort()));
    connect(editPadPort, &QLineEdit::textChanged, this, [=]()
    {
        QString text = editPadPort->text().trimmed();
        Configure::instance()->configureSTO->setConfigure("PAD/port", text);
    });

    layoutMain->addLayout(layout3);

    layout4 = new QVBoxLayout;
    buttonRule = new QPushButton("规则");
    connect(buttonRule, &QPushButton::clicked, this, [=]()
    {
        Rule *rule = new Rule;
        rule->setAttribute(Qt::WA_DeleteOnClose);
        rule->setWindowModality(Qt::ApplicationModal);
        rule->show();
    });
    layout4->addWidget(buttonRule);
    buttonCode = new QPushButton("三段码过滤");
    connect(buttonCode, &QPushButton::clicked, this, [=]()
    {
        FilterSTO *fs = new FilterSTO;
        fs->setAttribute(Qt::WA_DeleteOnClose);
        fs->setWindowModality(Qt::ApplicationModal);
        fs->show();
    });
    layout4->addWidget(buttonCode);

    buttonOther = new QPushButton("其他设置");
    connect(buttonOther, &QPushButton::clicked, this, [=]()
    {
        OtherSto *fs = new OtherSto;
        fs->setAttribute(Qt::WA_DeleteOnClose);
        fs->setWindowModality(Qt::ApplicationModal);
        fs->show();
    });
    layout4->addWidget(buttonOther);
    layout4->addStretch();

    layoutMain->addLayout(layout4);


    setLayout(layoutMain);

    connectSignalSlot();
}

void ConfigureSTOUI::connectSignalSlot()
{





}
