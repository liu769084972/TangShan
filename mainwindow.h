#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "DataQueryStatistics/dataQueryStatistics.h"
#include "Test/UI/testui.h"
#include "Alarm/alarmui.h"
#include "Work/UI/sortarrivedsendbag.h"
#include "Configure/UI/configureui.h"
#include "Test/testdrop.h"

class Camera;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(bool _comeOrGo, QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void messageBoxAlarm(QString message);
    //设置视觉连接状态
    void setCameraStatus(int indexCamera, QString status);
    //设置数据到界面
    void showData(int cameraId, int indexCompany, QString expressId, QString carrierId, QString boxId, int scanNum);
    void alarm(QString text); //报警
    //到件上传标志
    bool arrivedFlag();
    //派件上传标志
    bool packSendFlag();
    //上传派件数据到界面
    void showArrivedData(QString express, QString company, QString status);
    //上传集包或发件数据到界面
    void showPackSendData(QStringList expressIdList, QString containerNoOrcompany, QString status);
    //添加报警信息
    void appendAlarm(QString content, QString level, QString component);

private slots:
    void setDefaultTabChild();
    void tabWidgetCurrentChanged(int index); //当前tab页改变
    void actionDropTrigger(); //测试-落格-triggered（触发）

signals:
    void containerNoSignal(QString _containerNo);
    void sendOrPackSignal(bool);
    void arrivedSignal(bool);

private:
    void setTabWidget(); //设置tabWiget，如icon，font
    void creatActions(); //创建菜单
    void createToolBars(); //创建工具条
    void connectSignalSlot();
    //定时刷新界面
    void timerEvent(QTimerEvent *event);

private:
    Ui::MainWindow *ui;

    SortArrivedSendBag *sortArrivedSendBag;
    DataQueryStatistics *dataQueryStatistics;
    TestUI *testUI;
    AlarmUI *alarmUI;
    ConfigureUI *confUI;

    Camera *camera;
    bool comeOrGo; //进港或出港

    //菜单
    QAction *actionConnectAllCamera;
    QAction *actionCloseAllCamera;
    QAction *actionRule;
    QAction *actionFilterCodeComeSto;
    QAction *actionFilterCodeComeYto; //圆通进港三段码过滤规则
    //工具条
    QToolBar *toolBar;
};

#endif // MAINWINDOW_H
