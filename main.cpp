#include "mainwindow.h"
#include <QApplication>

#include "start.h"
#include "Configure/configure.h"
#include "plc.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("杭州迅工科技有限公司");
    QCoreApplication::setApplicationName("杭州迅工分拣系统");

    qApp->setApplicationDisplayName(Configure::instance()->appName());

    QIcon icon(":/images/logo.png");
    qApp->setWindowIcon(icon);
    Start *start = new Start();
    QObject::connect(start, &Start::come, start, [&](){
        new MainWindow(true);
        start->close();
        //qApp->setQuitOnLastWindowClosed(true);
    });
    QObject::connect(start, &Start::go, start, [&](){
        new MainWindow(false);
        start->close();
        //qApp->setQuitOnLastWindowClosed(true);
    });
    start->show();
//    PLC plc;
//    long lErr1 = plc.initAddress();
//    long lErr2 = plc.initHandle();
//    plc.testAlarmAndExpressId();


    return a.exec();
}
