#include "toolui.h"

#include <QCoreApplication>
#include <QProcess>

ToolUI::ToolUI(QWidget *parent)
    : QGroupBox (parent)
{
    layoutMain = new QHBoxLayout;

    buttonTcpIpHelper = new QPushButton("打开网络调试助手");
    buttonMySqlCommand = new QPushButton("打开mysql命令行工具");
    buttonCMD = new QPushButton("打开CMD");
    buttonRemoteDesktop = new QPushButton("打开远程桌面");
    connect(buttonTcpIpHelper, &QPushButton::clicked, this, [=]()
    {
        QString appDirPath = QCoreApplication::applicationDirPath();
        QString fileName = QString("%1\\tools\\NetAssist.exe").arg(appDirPath);
        QProcess::execute("explorer " + fileName.replace('/', '\\'));
    });

    connect(buttonMySqlCommand, &QPushButton::clicked, this, [=]()
    {
        QString program("C:\\Program Files\\MySQL\\MySQL Server 5.5\\bin\\mysql.exe");
        QStringList arguments;
        arguments << "-hlocalhost" << "-uroot" << "-p";
        QProcess *myProcess = new QProcess(this);
        myProcess->execute(program, arguments);
//        QString str = "C:\\Windows\\System32\\cmd.exe " + program.replace('/', "\\") + " -hlocalhost" + " -uroot" + " -p";
//        QProcess::execute(str);
    });

    connect(buttonCMD, &QPushButton::clicked, this, [=]()
    {
        QString program("C:\\Windows\\System32\\cmd.exe");
        QString appDirPath = QCoreApplication::applicationDirPath();
        QString fileName = QString("%1\\tools\\NetAssist.exe").arg(appDirPath);
        QProcess::execute("explorer " + program.replace('/', '\\'));
    });

    connect(buttonRemoteDesktop, &QPushButton::clicked, this, [=]()
    {
        QString program("C:\\Windows\\System32\\mstsc.exe");
        QString appDirPath = QCoreApplication::applicationDirPath();
        QString fileName = QString("%1\\tools\\NetAssist.exe").arg(appDirPath);
        QProcess::execute("explorer " + program.replace('/', '\\'));
    });

    layoutMain->addWidget(buttonTcpIpHelper);
    layoutMain->addWidget(buttonMySqlCommand);
    layoutMain->addWidget(buttonCMD);
    layoutMain->addWidget(buttonRemoteDesktop);

    setLayout(layoutMain);
}
