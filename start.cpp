#include "start.h"
#include "ui_start.h"
#include "Configure/configure.h"

Start::Start(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Start)
{
    ui->setupUi(this);
    connect(ui->buttonCome, &QPushButton::clicked, this, &Start::come);
    connect(ui->buttonGo, &QPushButton::clicked, this, &Start::go);
    setAttribute(Qt::WA_DeleteOnClose); //关闭窗体后，删除窗体对象
    //setWindowTitle(Configure::instance()->appName());
    //QIcon icon(":/images/logo.png");
    //setWindowIcon(icon);
}

Start::~Start()
{
    delete ui;
}
