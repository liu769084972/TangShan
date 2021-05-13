#ifndef DATAQUERYSTATISTICS_H
#define DATAQUERYSTATISTICS_H

#include <QWidget>
#include <QSqlDatabase>
#include <QSettings>
#include <QSqlQueryModel>
#include <QSortFilterProxyModel>
#include <QDateTime>
#include <QButtonGroup>

namespace Ui {
class DataQueryStatistics_Ui;
}

class DataQueryStatistics : public QWidget
{
    Q_OBJECT

public:
    explicit DataQueryStatistics(QWidget *parent = nullptr);
    ~DataQueryStatistics() override;

private:
    void search(QString sql);

    void createDatabaseConnection();

    void connectSignalAndSlot();

    bool eventFilter(QObject *target, QEvent *event) Q_DECL_OVERRIDE; //焦点过滤器
    QString getCompany();
    QString getTime();
    QString getArrDelDispBag(); //获取到件派件发件和集包的标志sql
    QString getBoxId();
    QString getExpressId();
    QString getSql(QString dml, QString field, QString table, QString time,
                   QString boxId, QString arrDelDisBag, QString groupBy, QString expressIds);

private slots:
    void editExpressIdTextChanged(); //条码槽文本改变
    void dateTimeTextChanged(); //时间段槽文本改变
    void radioButtonToggled(bool); //快捷查询切换

    void slot_btnGroupClicked(int id);
    void setTime(QStringList timeList, QDateTime _start, QDateTime _end);
    void comboBoxMonthCurrentIndexChanged(int index);

    void on_checkBoxPack_stateChanged(int arg1);

private:
    Ui::DataQueryStatistics_Ui *ui;
    QButtonGroup *btnGroup;
    QSqlDatabase *db;

    QSqlQueryModel *sqlMode;
    QSortFilterProxyModel *proxyModel;

    int order = -1; //-1原始顺序，0升序，1降序

    bool isCheckedEditExpressId;

    QString company;
    QString start;
    QString end;
    QString start1;
    QString end1;

    int month[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    //QSettings *cfg;
    enum {DML, FIELD, TABLE, CONDITION};
    QStringList sqlList;
    enum {ST, ZT, YT, BS, TT, YD, YZ, EX, TOTAL};
    QVector<QString> tableList;
};

#endif // DATAQUERYSTATISTICS_H
