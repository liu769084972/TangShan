#include "rule.h"
#include "Configure/configure.h"

#include <QHeaderView>
#include <QMessageBox>
#include <QFile>

Rule::Rule(QWidget *parent) : QWidget(parent)
{
    fileRuleComeVector.resize(COMPANYNUM);
    fileRuleComeVector[Sto] = FILE_STO_COME_RULE;
    fileRuleComeVector[Zto] = "";
    fileRuleComeVector[Yto] = FILE_YTO_COME_RULE;
    fileRuleComeVector[Best] = "";
    fileRuleComeVector[Tt] = "";
    fileRuleComeVector[Yunda] = "";
    fileRuleComeVector[Youzheng] = "";

    fileRuleGoVector.resize(COMPANYNUM);
    fileRuleGoVector[Sto] = FILE_STO_GO_RULE;
    fileRuleGoVector[Zto] = "";
    fileRuleGoVector[Yto] = FILE_YTO_GO_RULE;
    fileRuleGoVector[Best] = "";
    fileRuleGoVector[Tt] = "";
    fileRuleGoVector[Yunda] = "";
    fileRuleGoVector[Youzheng] = "";

    fileRule[true][Sto] = FILE_STO_COME_RULE;
    fileRule[true][Yto] = FILE_YTO_COME_RULE;
    fileRule[false][Sto] = FILE_STO_GO_RULE;
    fileRule[false][Yto] = FILE_YTO_GO_RULE;

    layoutMain = new QVBoxLayout;

     tableRule = new QTableWidget(0, 4);
     tableRule->horizontalHeader()->setStretchLastSection(true);
     tableRule->setHorizontalHeaderLabels(QStringList() << "一段码" << "二段码" << "三段码" << "格口");
     comboCompany = new QComboBox;
     QBitArray ba = Configure::instance()->companySwitchCome();
     QStringList listCompany;
     listCompany << "申通" << "中通" << "圆通" << "百世" << "天天" << "韵达" << "邮政";
     for (int i = 0; i < ba.size(); ++i) {
         if (ba[i]) {
             comboCompany->addItem(listCompany[i]);
         }
     }

     comboComeOrGo = new QComboBox();
     comboComeOrGo->addItem("进港");
     comboComeOrGo->addItem("出港");
     buttonLoad = new QPushButton("加载");
     connect(buttonLoad, &QPushButton::clicked, this, &Rule::buttonLoadClicked);
     buttonSave = new QPushButton("保存");
     connect(buttonLoad, &QPushButton::clicked, this, &Rule::buttonSaveClicked);

     layoutCombo = new QHBoxLayout;
     layoutCombo->addWidget(comboComeOrGo);
     layoutCombo->addWidget(comboCompany);

     layoutCombo->addStretch();

     buttonAdd = new QPushButton("添加");
     buttonEdit = new QPushButton("修改");
     buttonDel = new QPushButton("删除");

     layoutButton = new QVBoxLayout; //表格操作按钮布局
     layoutButton->addWidget(buttonLoad);
     layoutButton->addWidget(buttonSave);
     layoutButton->addWidget(buttonAdd);
     //layoutButton->addWidget(buttonEdit);
     layoutButton->addWidget(buttonDel);
     layoutButton->addStretch();

     layoutTableButton = new QHBoxLayout;

     layoutTableButton->addWidget(tableRule);
     layoutTableButton->addLayout(layoutButton);

     layoutMain->addLayout(layoutCombo);
     layoutMain->addLayout(layoutTableButton);

     setLayout(layoutMain);
}

void Rule::buttonLoadClicked()
{
    QString comeOrGo = comboComeOrGo->currentText();
    QString company = comboCompany->currentText();
    QStringList listCompany;
    listCompany << "申通" << "中通" << "圆通" << "百世" << "天天" << "韵达" << "邮政";
    int indexCompany = listCompany.indexOf(company);
    bool value = (comeOrGo == QString("进港") ? true : false);
    loadFileToTable(indexCompany, value);
}

void Rule::buttonSaveClicked()
{
    int rows = tableRule->rowCount();
    if (rows == 0) {
        QMessageBox::critical(nullptr, "报警", "表格为空，不能保存！");
        return;
    }
    int ret = QMessageBox::warning(nullptr, "警告", QString("行数：%1，请确认行数是否正确").arg(rows),
                         QMessageBox::Ok, QMessageBox::No);
    if (ret == QMessageBox::Ok) {
        //保存
        //清空文件
        QString comeOrGo = comboComeOrGo->currentText();
        QString company = comboCompany->currentText();
        QStringList listCompany;
        listCompany << "申通" << "中通" << "圆通" << "百世" << "天天" << "韵达" << "邮政";
        int indexCompany = listCompany.indexOf(company);
        bool value = (comeOrGo == QString("进港") ? true : false);

        saveFileToTable(indexCompany, value);
    }
}

void Rule::buttonAddClicked()
{
    int row = tableRule->rowCount();
    tableRule->insertRow(row);
}

void Rule::buttonEditClicked()
{

}

void Rule::buttonDelClicked()
{
    //获取选中项
    QList<QTableWidgetSelectionRange> select = tableRule->QTableWidget::selectedRanges();
    //获取选中数据
    for (int i = select.size() - 1; i >= 0; --i) {
        //获取行和列
        int topRow = select[i].topRow();
        int bottomRow = select[i].bottomRow();
        //删除前先获取文本
        for(int row = bottomRow; row >= topRow; --row) {
            tableRule->removeRow(row);
        }
    }
}

void Rule::tableRuleCellChanged(int row, int column)
{
    isDirty = true;
}

void Rule::loadFileToTable(int company, bool comeOrGo)
{
    //为了保持文件和表格数据的一致性，全部删除，全部加载
    int rows = tableRule->rowCount(); //清空表格，连同行列
    for (int i = 0; i < rows; ++i) {
        tableRule->removeRow(i);
    }
    //获取配置文件规则数据
    QHash<QStringList, QList<short> > rule;
    if (comeOrGo) { //进港

        switch (company) {
        case Sto:
            rule = Configure::instance()->configureSTO->ruleStoCome(); //申通进港
            break;
        case Zto:
            //rule = Configure::instance()->configureZTO->ruleZtoCome(); //申通进港
            break;
        case Yto:
            //rule = Configure::instance()->configureYTO->ruleYtoCome(); //圆通进港
            break;
        case Best:
            //rule = Configure::instance()->configureBest->ruleBestCome(); //申通进港
            break;
        case Tt:
            //rule = Configure::instance()->configureTT->ruleTtCome(); //申通进港
            break;
        case Yunda:
            //rule = Configure::instance()->configureSTO->ruleStoCome(); //申通进港
            break;
        case Youzheng:
            //rule = Configure::instance()->configureSTO->ruleStoCome(); //申通进港
            break;
        default:
            ;
        }

    } else { //出港
        switch (company) {
        case Sto:
            rule = Configure::instance()->configureSTO->ruleStoGo(); //申通出港
            break;
        case Zto:
            //rule = Configure::instance()->configureZTO->ruleZtoCome(); //申通进港
            break;
        case Yto:
            //rule = Configure::instance()->configureYTO->ruleYtoGo(); //圆通出港
            break;
        case Best:
            //rule = Configure::instance()->configureBest->ruleBestCome(); //申通进港
            break;
        case Tt:
            //rule = Configure::instance()->configureTT->ruleTtCome(); //申通进港
            break;
        case Yunda:
            //rule = Configure::instance()->configureSTO->ruleStoCome(); //申通进港
            break;
        case Youzheng:
            //rule = Configure::instance()->configureSTO->ruleStoCome(); //申通进港
            break;
        default:
            ;
        }
    }

    //加载到表格
    QHashIterator<QStringList, QList<short> > iterator(rule);
    while (iterator.hasNext()) {
        iterator.next();
        int row = tableRule->rowCount();
        tableRule->insertRow(row);

        QStringList codeList = iterator.key();
        QString code1 = codeList.value(0);
        QString code2 = codeList.value(1);
        QString code3 = codeList.value(2);

        //一段码
        QTableWidgetItem *item0 = new QTableWidgetItem(code1);
        tableRule->setItem(row, 0, item0);
        //二段码
        QTableWidgetItem *item1 = new QTableWidgetItem(code2);
        tableRule->setItem(row, 1, item1);
        //三段码
        QTableWidgetItem *item2 = new QTableWidgetItem(code3);
        tableRule->setItem(row, 2, item2);

        QList<short> boxIdList = iterator.value();
        QString text;
        for (int i = 0; i < boxIdList.size(); ++i) {
            text += QString::number(boxIdList.value(i)) + ',';
        }
        text.chop(1);
        QTableWidgetItem *item3 = new QTableWidgetItem(text);
        tableRule->setItem(row, 3, item3);
    }
}

void Rule::saveFileToTable(int company, bool comeOrGo)
{
    QString fileName;
    if (comeOrGo) {
        fileName = fileRuleComeVector[company];
    } else {
        fileName = fileRuleGoVector[company];
    }

    QFile file(fileRule[comeOrGo][company]);

    //file.close(); //先清空

    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(nullptr, "警告", QString("文件%1打开失败").arg(fileRule[comeOrGo][company]));
        return;
    }

    QTextStream write(&file);
    int rows = tableRule->rowCount();
    for (int row = 0; row < rows; ++row) {
        QTableWidgetItem *item0 = tableRule->item(row, 0);
        QTableWidgetItem *item1 = tableRule->item(row, 1);

        if (item0 && item1) {
            QString code = item0->text().trimmed();
            QString boxId = item1->text().trimmed();

            write << QString("%1,%2").arg(code, boxId) << "\r\n";
        }
    }
}

void Rule::connectSignalSlot()
{
    connect(buttonLoad, &QPushButton::clicked, this, &Rule::buttonLoadClicked);
    connect(buttonSave, &QPushButton::clicked, this, &Rule::buttonSaveClicked);
    connect(buttonAdd, &QPushButton::clicked, this, &Rule::buttonAddClicked);
    connect(buttonEdit, &QPushButton::clicked, this, &Rule::buttonEditClicked);
    connect(buttonDel, &QPushButton::clicked, this, &Rule::buttonDelClicked);
}

void Rule::closeEvent(QCloseEvent *)
{

}
