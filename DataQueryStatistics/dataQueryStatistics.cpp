#include "dataQueryStatistics.h"
#include "ui_dataquerystatistics.h"
#include "define.h"
#include "Configure/configure.h"
#include "Configure/configuredataquerystatistics.h"
#include <QTextCodec>
#include <QMessageBox>
#include <QSqlQuery>
#include <QDebug>
#include <QSqlError>
#include <QSettings>

DataQueryStatistics::DataQueryStatistics(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DataQueryStatistics_Ui)
{
    ui->setupUi(this);
    btnGroup = new QButtonGroup(this);
    btnGroup->addButton(ui->buttonSearch, 0);
    btnGroup->addButton(ui->buttonTotal, 1);
    btnGroup->addButton(ui->buttonStatisticsByBoxId, 2);

    QPalette pe;
    pe.setColor(QPalette::WindowText, Qt::red);
    ui->labelTotal->setPalette(pe);

    //QLabel自动换行
    ui->labelSql->adjustSize();
    ui->labelSql->setGeometry(QRect(328, 240, 329, 27*4));
    ui->labelSql->setWordWrap(true);
    ui->labelSql->setAlignment(Qt::AlignTop);
    ui->labelSql->setTextInteractionFlags(Qt::TextSelectableByMouse);
    isCheckedEditExpressId = false;

    //安装事件过滤器
    ui->groupBox->installEventFilter(this);
    ui->comboBoxMonth->installEventFilter(this);
    ui->radioToday->installEventFilter(this);
    ui->radioYesterday->installEventFilter(this);
    ui->groupBox_2->installEventFilter(this);
    ui->labelStart->installEventFilter(this);
    ui->labelEnd->installEventFilter(this);
    ui->dateTimeStart->installEventFilter(this);
    ui->dateTimeEnd->installEventFilter(this);
    ui->groupBox_3->installEventFilter(this);
    ui->editExpressId->installEventFilter(this);

    ui->dateTimeStart->setDateTime(QDateTime::currentDateTime());
    ui->dateTimeEnd->setDateTime(QDateTime::currentDateTime());

    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers); //不可编辑

    sqlMode = new QSqlQueryModel(this);
    proxyModel = new QSortFilterProxyModel(this);

    if (QDate::currentDate().year() % 100 != 0) {
        if (QDate::currentDate().year() % 4 == 0) {
            month[1] = 29;
        }
        else {
            month[1] = 28;
        }
    } else {
        if (QDate::currentDate().year() % 400 == 0) {
            month[1] = 29;
        }
        else {
            month[1] = 28;
        }
    }

    createDatabaseConnection();
    connectSignalAndSlot();

    sqlList.insert(DML, "SELECT");
    sqlList.insert(FIELD, "*");
    tableList.resize(10);
}

DataQueryStatistics::~DataQueryStatistics()
{
    delete ui;
}

void DataQueryStatistics::createDatabaseConnection()
{
    db = new QSqlDatabase(QSqlDatabase::addDatabase("QMYSQL"));
    db->setHostName(Configure::instance()->nativeip());
    db->setUserName(NATIVE_DB_USERNAME);
    db->setPassword(NATIVE_DB_PASSWORD);
    db->setDatabaseName(NATIVE_DB_DBNAME);
    if (!db->open()) {
       QMessageBox::warning(this, "统计查询", "数据库连接失败！" + db->lastError().text());
    }
    qDebug() << db->tables();
}

QString DataQueryStatistics::getCompany()
{
    switch (ui->comboCompany->currentIndex()) {
    case 0:
        if (ui->comboBoxComeGo->currentText() == "进港")
            company = NATIVE_DB_STO_COME;
        if (ui->comboBoxComeGo->currentText() == "出港")
            company = NATIVE_DB_STO_GO;
        if (!db->tables().contains(company)) {
            QMessageBox::critical(this, "表格", "对应表不存在");
        }
        break;
    case 1:
        if (ui->comboBoxComeGo->currentText() == "进港")
            company = NATIVE_DB_ZTO_COME;
        if (ui->comboBoxComeGo->currentText() == "出港")
            company = NATIVE_DB_ZTO_GO;
        if (!db->tables().contains(company)) {
            QMessageBox::critical(this, "表格", "对应表不存在");
        }
        break;
    case 2:
        if (ui->comboBoxComeGo->currentText() == "进港")
            company = NATIVE_DB_YTO_COME;
        if (ui->comboBoxComeGo->currentText() == "出港")
            company = NATIVE_DB_YTO_GO;
        if (!db->tables().contains(company)) {
            QMessageBox::critical(this, "表格", "对应表不存在");
        }
        break;
     case 3:
        if (ui->comboBoxComeGo->currentText() == "进港")
            company = NATIVE_DB_BS_COME;
        if (ui->comboBoxComeGo->currentText() == "出港")
            company = NATIVE_DB_BS_GO;
        if (!db->tables().contains(company)) {
            QMessageBox::critical(this, "表格", "对应表不存在");
        }
        break;
     case 4:
        if (ui->comboBoxComeGo->currentText() == "进港")
            company = NATIVE_DB_TT_COME;
        if (ui->comboBoxComeGo->currentText() == "出港")
            company = NATIVE_DB_TT_GO;
        if (!db->tables().contains(company)) {
            QMessageBox::critical(this, "表格", "对应表不存在");
        }
       break;
     case 5:
        if (ui->comboBoxComeGo->currentText() == "进港")
            company = NATIVE_DB_YUNDA_COME;
        if (ui->comboBoxComeGo->currentText() == "出港")
            company = NATIVE_DB_YUNDA_GO;
        if (!db->tables().contains(company)) {
            QMessageBox::critical(this, "表格", "对应表不存在");
        }
    break; case 6:
        if (ui->comboBoxComeGo->currentText() == "进港")
            company = NATIVE_DB_YOUZHENG_COME;
        if (ui->comboBoxComeGo->currentText() == "出港")
            company = NATIVE_DB_YOUZHENG_GO;
        if (!db->tables().contains(company)) {
            QMessageBox::critical(this, "表格", "对应表不存在");
        }
    break; case 7:
        if (ui->comboBoxComeGo->currentText() == "进港")
            company = NATIVE_DB_EXCEPTION_COME;
        if (ui->comboBoxComeGo->currentText() == "出港")
            company = NATIVE_DB_EXCEPTION_GO;
        if (!db->tables().contains(company)) {
            QMessageBox::critical(this, "表格", "对应表不存在");
        }
    break; case 8:
        if (ui->comboBoxComeGo->currentText() == "进港") {
            company = "all";
            tableList[ST] = NATIVE_DB_STO_COME;
            tableList[ZT] = NATIVE_DB_ZTO_COME;
            tableList[YT] = NATIVE_DB_YTO_COME;
            tableList[BS] = NATIVE_DB_BS_COME;
            tableList[TT] = NATIVE_DB_TT_COME;
            tableList[YD] = NATIVE_DB_YUNDA_COME;
            tableList[YZ] = NATIVE_DB_YOUZHENG_COME;
            tableList[EX] = NATIVE_DB_EXCEPTION_COME;
        }
        if (ui->comboBoxComeGo->currentText() == "出港") {
            company = "all";
            tableList[ST] = NATIVE_DB_STO_GO;
            tableList[ZT] = NATIVE_DB_ZTO_GO;
            tableList[YT] = NATIVE_DB_YTO_GO;
            tableList[BS] = NATIVE_DB_BS_GO;
            tableList[TT] = NATIVE_DB_TT_GO;
            tableList[YD] = NATIVE_DB_YUNDA_GO;
            tableList[YZ] = NATIVE_DB_YOUZHENG_GO;
            tableList[EX] = NATIVE_DB_EXCEPTION_GO;
        }
    }
    return company;
}

QString DataQueryStatistics::getTime()
{
    if (ui->comboBoxTime->currentText() == "创建时间") {
        start1 = start;
        end1 = end;
        return QString("CREATE_TIME > '%1' AND CREATE_TIME < '%2'").arg(start1).arg(end1);
    } else if (ui->comboBoxTime->currentText() == "掉落时间") {
        qDebug() << start << end;
        start1 = start;
        end1 = end;
        start1 = "DT#" + start1.replace(QChar(' '), QChar('-'));
        end1 = "DT#"  + end1.replace(QChar(' '), QChar('-'));
        return QString("Drop_Time > '%1' AND Drop_Time < '%2'").arg(start1).arg(end1);;
    }
    return "";
}

void DataQueryStatistics::setTime(QStringList timeList, QDateTime _start, QDateTime _end)
{   
    qDebug() << timeList << timeList.value(0) << timeList.value(1);
    if (timeList.size() < 2)
        return;

    if (timeList[0].startsWith("-")) {
        QDateTime dateStart1 = _start.addDays(-1);
        QString monthList0 = timeList[0].remove(0, 1);
        QTime time = QTime::fromString(monthList0, "hh:mm:ss");
        int secs = time.msecsSinceStartOfDay() / 1000;
        QDateTime dateStart2 = dateStart1.addSecs(secs);
        start = dateStart2.toString("yyyy-MM-dd hh:mm:ss");
        qDebug() << __LINE__ << dateStart1 <<  monthList0 << time << secs << dateStart2 <<start;
    } else if (timeList[0].startsWith("+")) {
        start = _start.addDays(+1).addSecs(
              QTime::fromString(timeList[0].remove(0, 1), "hh:mm:ss")
                .msecsSinceStartOfDay() / 1000).toString("yyyy-MM-dd hh:mm:ss");
        qDebug() << __LINE__ << start;
    } else {
        start = _start.addSecs(
              QTime::fromString(timeList[0], "hh:mm:ss")
                .msecsSinceStartOfDay() / 1000).toString("yyyy-MM-dd hh:mm:ss");
        qDebug() << __LINE__ << start;
    }

    if (timeList[1].startsWith("-")) {
        end = _end.addDays(-1).addSecs(
              QTime::fromString(timeList[1].remove(0, 1), "hh:mm:ss")
                .msecsSinceStartOfDay() / 1000).toString("yyyy-MM-dd hh:mm:ss");
        qDebug() << __LINE__ << end;
    } else if (timeList[1].startsWith("+")) {
        QDateTime dateEnd1 = _end.addDays(+1);
        QString monthList1 = timeList[1].remove(0, 1);
        QTime time = QTime::fromString(monthList1, "hh:mm:ss");
        int secs = time.msecsSinceStartOfDay() / 1000;
        QDateTime dateEnd2 = dateEnd1.addSecs(secs);
        end = dateEnd2.toString("yyyy-MM-dd hh:mm:ss");
        qDebug() << __LINE__ << dateEnd1 <<  monthList1 << time << secs << dateEnd2 <<end;
    } else {
        end = _end.addSecs(
              QTime::fromString(timeList[1], "hh:mm:ss")
                .msecsSinceStartOfDay() / 1000).toString("yyyy-MM-dd hh:mm:ss");
        qDebug() << __LINE__ << end;
    }
}

void DataQueryStatistics::comboBoxMonthCurrentIndexChanged(int index)
{
    if (!index)
        return;

    int year = QDate::currentDate().year();
    QDate d1 = QDate(year, index, 1);
    QTime t1 = QTime(00, 00, 00);
    QDateTime dateStart(d1, t1);
    QDate d2 = QDate(year, index, month[index-1]);
    QTime t2 = QTime(00, 00, 00);
    QDateTime dateEnd(d2, t2);
    QStringList timeList = ConfigureDataQueryStatistics::instance()->monthList();
    setTime(timeList, dateStart, dateEnd);
}

void DataQueryStatistics::connectSignalAndSlot()
{

    connect(btnGroup, QOverload<int>::of(&QButtonGroup::buttonClicked),
            this, &DataQueryStatistics::slot_btnGroupClicked);
    connect(ui->editExpressId, &QLineEdit::textChanged,
            this, &DataQueryStatistics::editExpressIdTextChanged);

    connect(ui->dateTimeStart, &QDateTimeEdit::dateTimeChanged,
            this, &DataQueryStatistics::dateTimeTextChanged);
    connect(ui->dateTimeEnd, &QDateTimeEdit::dateTimeChanged,
            this, &DataQueryStatistics::dateTimeTextChanged);

    connect(ui->comboBoxMonth, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &DataQueryStatistics::comboBoxMonthCurrentIndexChanged);
//    connect(ui->radioToday, &QRadioButton::toggled, this,
//            &MainWindow::radioButtonToggled);
//    connect(ui->radioYesterday, &QRadioButton::toggled, this,
//            &MainWindow::radioButtonToggled);
}

bool DataQueryStatistics::eventFilter(QObject *target, QEvent *event)
{
    if (event->type() != QEvent::FocusIn) {
        return false;
    }

    qDebug() << target;
    //今天
    if (target == ui->radioToday) {

        QDateTime _start(QDate::currentDate(), QTime::fromString("00:00:00", "hh:mm:ss"));
        QDateTime _end(QDate::currentDate(), QTime::fromString("00:00:00", "hh:mm:ss"));
        QStringList timeList = ConfigureDataQueryStatistics::instance()->todayList();
        setTime(timeList, _start, _end);
        qDebug() << "当天统计-焦点";
        isCheckedEditExpressId = false;
        return true;
    }
    //昨天
    if (target == ui->radioYesterday) {
        QDateTime _start(QDate::currentDate(), QTime::fromString("00:00:00", "hh:mm:ss"));
        QDateTime _end(QDate::currentDate(), QTime::fromString("00:00:00", "hh:mm:ss"));
        QStringList timeList = ConfigureDataQueryStatistics::instance()->yesterdayList();
        setTime(timeList, _start.addDays(-1), _end.addDays(-1));

        qDebug() << "昨天统计-焦点";
        isCheckedEditExpressId = false;
        return true;
    }

    if (target == ui->dateTimeStart ||
            target == ui->dateTimeEnd) {
        qDebug() << "时间段统计-焦点";
        //时间段
        start = ui->dateTimeStart->text();
        end = ui->dateTimeEnd->text();
        isCheckedEditExpressId = false;
        return true;
    }

    if (target == ui->editExpressId) {
        isCheckedEditExpressId = true;
        qDebug() << "条码查询-焦点" << isCheckedEditExpressId;
        return true;
    }

    return QWidget::eventFilter(target, event);
}

QString DataQueryStatistics::getArrDelDispBag()
{
    QStringList addbList;
    if (ui->comboPDPDstatus->currentIndex() == 0) {
        if (ui->checkBoxPack->isChecked()) {
            QString field = ConfigureDataQueryStatistics::instance()->isPackFail();
            addbList << field;
        }
    } else if (ui->comboPDPDstatus->currentIndex() == 1) {
        if (ui->checkBoxPack->isChecked()) {
            QString field = ConfigureDataQueryStatistics::instance()->isPackSuccess();
            addbList << field;
        }
    }
    if (addbList.isEmpty())
        return QString();
    else {
        QString sql = "AND " + addbList.join(" AND ");
        return sql;
    }
}

QString DataQueryStatistics::getBoxId()
{
    QString txt = ui->editBoxId->text().trimmed();
    if (txt.isEmpty())
        return txt;
    else {
        QStringList boxIdList = txt.split(',');
        QString sql = QString("AND Box_Num IN(%1)").arg("'" + boxIdList.join("','") + "'");
        return sql;
    }
}

QString DataQueryStatistics::getExpressId()
{
    QString txt = ui->editExpressId->text().trimmed();
    if (txt.isEmpty()) {
        return QString();
    } else {
        QStringList expressIdList = txt.split(',');
        QString expressIds = "'" + expressIdList.join("','") + "'";
        QString sql = "Expressflow_id IN(" + expressIds + ")";
        return sql;
    }
}

QString DataQueryStatistics::getSql(QString dml, QString field, QString table, QString time,
                           QString boxId, QString arrDelDisBag, QString groupBy, QString expressIds)
{
    QString sql;
    if (db->tables().contains(table)) {
        if (expressIds.isEmpty())
            sql = dml + " " + field + " " + "FROM" + " " + table + " " + "WHERE" +
                " " + time + " " + arrDelDisBag + " " + boxId + " " + groupBy;
        else
            sql = dml + " " + "*" + " " + "FROM" + " " + table + " " + "WHERE" +
                " " + expressIds;
    }

    return sql;
}

void DataQueryStatistics::editExpressIdTextChanged()
{
    isCheckedEditExpressId = true;
    qDebug() << "条码查询-改变" << isCheckedEditExpressId;
}

void DataQueryStatistics::dateTimeTextChanged()
{
    start = ui->dateTimeStart->text().trimmed();
    end = ui->dateTimeEnd->text().trimmed();
    qDebug() << "时间段查询-改变";
}

void DataQueryStatistics::radioButtonToggled(bool /*checked*/)
{
//    //今天
//    if (ui->radioToday->isChecked()) {

//        start = QDate::currentDate().toString("yyyy-MM-dd 00:00:00");
//        end = QDate::currentDate().toString("yyyy-MM-dd 23:59:59");
//        qDebug() << "当天统计-切换";

//        return;
//    }
//    //昨天
//    if (ui->radioYesterday->isChecked()) {
//        qDebug() << "昨天统计-切换";
//        start = QDate::currentDate().addDays(-1).toString("yyyy-MM-dd 00:00:00");
//        end = QDate::currentDate().addDays(-1).toString("yyyy-MM-dd 23:59:59");
//        return;
    //    }
}

void DataQueryStatistics::slot_btnGroupClicked(int id)
{   
    QString company = getCompany();
    QString time = getTime();        
    QString boxId = getBoxId();
    QString arrDelDisBag = getArrDelDispBag();
    QString expressIds = getExpressId();
    QString field;
    QString groupBy;
    if (id == 0) {
        field = "*";
    } else if (id == 1) {
        field = "COUNT(*)";
    } else if (id == 2) {
        field = "COUNT(*)";
        groupBy = "GROUP BY Box_Num";
    }

    QString sql;
    if (company.compare("all")) { //单个表
        sql = getSql("SELECT", field, company, time, boxId, arrDelDisBag, groupBy, expressIds);
    } else { //所有表，使用
        QStringList sqlList;
        field = "Expressflow_Id, Box_Num, Create_time, Drop_time, Create_int";
        for(int i = 0; i < tableList.size(); ++i) {
            if (db->tables().contains(tableList.value(i))) {
                sqlList <<  getSql("SELECT", field, tableList.value(i), time, boxId, arrDelDisBag, groupBy, expressIds);
            }
        }
        if (id == 0) {
            sql = sqlList.join(" UNION ALL ");
        } else if (id == 1) {
            sql = QString("SELECT COUNT(*) AS 总量 FROM (") + sqlList.join(" UNION ALL ") + QString(")");
        } else if (id == 2) {
            sql = sqlList.join(" UNION ALL ");
        }
    }


    qDebug() << sql;
    //查询
    ui->labelSql->setText(sql);
    sqlMode->setQuery(sql); //直接设置
    proxyModel->setSourceModel(sqlMode);
    ui->tableView->setModel(proxyModel);
    ui->labelTotal->setText("行数:" + QString::number(sqlMode->rowCount()));

    //排序
    ui->tableView->setSortingEnabled(true);
    QHeaderView *header = ui->tableView->horizontalHeader();
    header->setSortIndicator(0, Qt::DescendingOrder);
    header->setSortIndicatorShown(true);
    header->setSectionsClickable(true);
    connect(header, SIGNAL(sectionClicked(int)),
            ui->tableView, SLOT(sortByColumn(int)));
}

void DataQueryStatistics::on_checkBoxPack_stateChanged(int arg1)
{
    if (arg1 == Qt::Checked) {

    }
}
