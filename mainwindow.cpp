#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "define.h"
#include "start.h"
#include "Configure/UI/rule.h"
#include "Work/Sort/camera.h"
#include "Configure/configure.h"
#include "Configure/UI/filtersto.h"
#include "Configure/UI/filteryto.h"

#include <QMessageBox>
#include <QProcess>

MainWindow::MainWindow(bool _comeOrGo, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow), 
    comeOrGo(_comeOrGo)

{
    ui->setupUi(this);
    setCentralWidget(ui->tabWidget);
    QString title = QString("%1").arg(comeOrGo ? "进港" : "出港");
    setWindowTitle(title);
    //设置菜单
    creatActions();
    createToolBars();
    //初始化各tab默认页
    sortArrivedSendBag = new SortArrivedSendBag(_comeOrGo);
    connect(sortArrivedSendBag->arrSndBag, &ArrivedSendBagUI::containerNoSignal,
            this, &MainWindow::containerNoSignal);
    connect(sortArrivedSendBag->arrSndBag, &ArrivedSendBagUI::isSendOrPackSignal,
            this, &MainWindow::sendOrPackSignal);
    connect(sortArrivedSendBag->arrSndBag, &ArrivedSendBagUI::isArrived,
            this, &MainWindow::arrivedSignal);
    alarmUI = new AlarmUI;
    testUI = new TestUI;
    confUI = new ConfigureUI;
    dataQueryStatistics = new DataQueryStatistics;
    //设置tabWidget样式
    setTabWidget();
    //设置tab父页的默认子页
    setDefaultTabChild();

    show();
    camera = new Camera(comeOrGo, this, this);

    connectSignalSlot();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setCameraStatus(int indexCamera, QString status)
{
    sortArrivedSendBag->sort->sortData->sortDataWidgetVector[indexCamera]->status->setText(status);
}

void MainWindow::showData(int cameraId, int indexCompany,
        QString expressId, QString carrierId, QString boxId, int scanNum)
{
    sortArrivedSendBag->sort->sortData->showData(cameraId, expressId, carrierId, boxId);
    sortArrivedSendBag->sort->calSortData->calculate(scanNum, indexCompany);
}

void MainWindow::messageBoxAlarm(QString message)
{
    QMessageBox::critical(this, "错误", message);
}

void MainWindow::setDefaultTabChild()
{

}

void MainWindow::tabWidgetCurrentChanged(int index)
{
    //使能相应菜单
    switch (index) {
    case 0:
    {

    }

    }
}

void MainWindow::actionDropTrigger()
{
    //设置测试tab页为testDrop
    //tabPages[Test].first = testDrop; 错误，地址虽然可以指向具体的内容，但是地址变量本身是按值传进去的，所以tab页保存的是以前的地址
    //如果要改变tab页保存指针的内容
    QHBoxLayout *layout = new QHBoxLayout();
}

void MainWindow::alarm(QString text)
{
    QMessageBox::critical(this, "报警", text);
}

bool MainWindow::arrivedFlag()
{
    return sortArrivedSendBag->arrSndBag->arrivedFlag;
}

bool MainWindow::packSendFlag()
{
    return sortArrivedSendBag->arrSndBag->sendOrBagFlag;
}

void MainWindow::showArrivedData(QString express, QString company, QString status)
{
    sortArrivedSendBag->arrSndBag->addItemArrived(express, company, status);
}

void MainWindow::showPackSendData(QStringList expressIdList,
            QString containerNoOrcompany, QString status)
{
    sortArrivedSendBag->arrSndBag->addItemSendOrBag(
                expressIdList, containerNoOrcompany, status);
}

void MainWindow::appendAlarm(QString content, QString level, QString component)
{
    alarmUI->appendMessage(content, level, component);
}

void MainWindow::setTabWidget()
{
    ui->tabWidget->setTabPosition(QTabWidget::South);

    ui->tabWidget->insertTab(SortArrivedSendBagTabPage, sortArrivedSendBag, "分拣-到件-派件-集包");
    ui->tabWidget->insertTab(QueryStatisticsTabPage, dataQueryStatistics, "查询统计");
    ui->tabWidget->insertTab(ConfigureTabPage, confUI, "配置");
    ui->tabWidget->insertTab(AlarmTabPage, alarmUI, "报警");
    ui->tabWidget->insertTab(TestTabPage, testUI, "测试");
    QTabBar * bars = ui->tabWidget->tabBar();

    bars->setFont(QFont("", 20));
}

void MainWindow::creatActions()
{
    connect(ui->actionDrop, &QAction::triggered, this, &MainWindow::actionDropTrigger);
    connect(ui->actionRunLog, &QAction::triggered, this, [=]()
    {
        QString appDirPath = QCoreApplication::applicationDirPath();
        QString fileName = QString("%1/logs/runLog%2.log").arg(appDirPath)
                .arg(QDateTime::currentDateTime().toString("yyyyMMdd"));
        QProcess::execute("explorer " + fileName.replace('/', '\\'));
    });
    connect(ui->actionDetailLog, &QAction::triggered, this, [=]()
    {
        QString appDirPath = QCoreApplication::applicationDirPath();
        QString fileName = QString("%1/logs/detailLog%2.log").arg(appDirPath)
                .arg(QDateTime::currentDateTime().toString("yyyyMMdd"));
        QProcess::execute("explorer " + fileName.replace('/', '\\'));
    });

    //一键连接
    actionConnectAllCamera = new QAction("一键连接", this);
    actionConnectAllCamera->setIcon(QIcon(""));
    //actionConnectAllCamera->setShortcut();
    actionConnectAllCamera->setStatusTip("连接所有的相机");
    connect(actionConnectAllCamera, &QAction::triggered, this, [=]()
    {
        auto vector = sortArrivedSendBag->sort->sortData->sortDataWidgetVector;
        foreach(auto i, vector) {
            i->buttonConnect->click();
        }
    });

    //一键关闭
    actionCloseAllCamera = new QAction("一键关闭", this);
    actionCloseAllCamera->setIcon(QIcon(""));
    //actionCloseAllCamera->setShortcut();
    actionCloseAllCamera->setStatusTip("关闭所有的相机");
    connect(actionCloseAllCamera, &QAction::triggered, this, [=]()
    {
        auto vector = sortArrivedSendBag->sort->sortData->sortDataWidgetVector;
        foreach(auto i, vector) {
            i->buttonClose->click();
        }
    });

    //规则
    actionRule = new QAction("规则", this);
    actionRule->setIcon(QIcon(""));
    //actionConnectAllCamera->setShortcut();
    actionRule->setStatusTip("设置格口和三段码对应关系");
    connect(actionRule, &QAction::triggered, this, [=]()
    {
        Rule *rule = new Rule;
        rule->setAttribute(Qt::WA_DeleteOnClose);
        rule->setWindowModality(Qt::ApplicationModal);
        rule->show();
    });

    //申通三段码过滤
    actionFilterCodeComeSto = new QAction("申通三段码过滤", this);
    actionFilterCodeComeSto->setIcon(QIcon(""));
    //actionConnectAllCamera->setShortcut();
    actionFilterCodeComeSto->setStatusTip("设置申通三段码过滤");
    connect(actionFilterCodeComeSto, &QAction::triggered, this, [=]()
    {
        FilterSTO *fs = new FilterSTO;
        fs->setAttribute(Qt::WA_DeleteOnClose);
        fs->setWindowModality(Qt::ApplicationModal);
        fs->show();
    });

    //圆通三段码过滤
    actionFilterCodeComeYto = new QAction("圆通三段码过滤", this);
    actionFilterCodeComeYto->setIcon(QIcon(""));
    //actionConnectAllCamera->setShortcut();
    actionFilterCodeComeYto->setStatusTip("设置圆通三段码过滤");
    connect(actionFilterCodeComeYto, &QAction::triggered, this, [=]()
    {
        FilterYTO *ys = new FilterYTO;
        ys->setAttribute(Qt::WA_DeleteOnClose);
        ys->setWindowModality(Qt::ApplicationModal);
        ys->show();
    });
}

void MainWindow::createToolBars()
{
    QBitArray ba = Configure::instance()->companySwitch();

    toolBar = addToolBar("");
    toolBar->addAction(actionConnectAllCamera);
    toolBar->addAction(actionCloseAllCamera);
    //toolBar->addAction(actionRule);
//    if (ba[Sto])
//        toolBar->addAction(actionFilterCodeComeSto);
//    if (ba[Yto])
//        toolBar->addAction(actionFilterCodeComeYto);
}

void MainWindow::connectSignalSlot()
{
    connect(sortArrivedSendBag->sort->sortData, &SortDataShowUI::buttonCameraClicked,
            camera, &Camera::openCamera);
}

void MainWindow::timerEvent(QTimerEvent *event)
{
    updateGeometry();
}
