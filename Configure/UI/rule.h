#ifndef RULE_H
#define RULE_H

#include <QWidget>
#include <QTableWidget>
#include <QComboBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>

#include "define.h"

class Rule : public QWidget
{
    Q_OBJECT
public:
    explicit Rule(QWidget *parent = nullptr);

signals:

public slots:
    void buttonLoadClicked();
    void buttonSaveClicked();
    void buttonAddClicked();
    void buttonEditClicked();
    void buttonDelClicked();
    void tableRuleCellChanged(int row, int column);
    void loadFileToTable(int company, bool comeOrGo); //从文件加载数据到表格-保持一致性
    void saveFileToTable(int company, bool comeOrGo); //从文件加载数据到表格-保持一致性
    //void comoboCompanyCurrentTextChanged(const QString &text);
    //void comoboComeOrGoCurrentTextChanged(const QString &text);

private:
    void connectSignalSlot();
    void closeEvent(QCloseEvent *) override;

private:
    QTableWidget *tableRule;
    QComboBox *comboCompany;
    QComboBox *comboComeOrGo;
    QPushButton *buttonLoad;
    QPushButton *buttonSave;
    QPushButton *buttonAdd; //添加
    QPushButton *buttonEdit; //编辑
    QPushButton *buttonDel; //删除

    QHBoxLayout *layoutCombo;
    QHBoxLayout *layoutTableButton;
    QVBoxLayout *layoutButton;
    QVBoxLayout *layoutMain;

    QVector<QString> fileRuleComeVector;
    QVector<QString> fileRuleGoVector;
    QString fileRule[2][COMPANYNUM];

    bool isDirty = false;
};

#endif // RULE_H
