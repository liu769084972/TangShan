#include "testui.h"
#include "define.h"

#include <QMessageBox>
#include <QFile>

TestUI::TestUI(QWidget *parent) : QWidget(parent)
{
    log = Log::instance();
    connect(this, &TestUI::appendDetailLog, log, &Log::appendDetailLog);

    layoutMain = new QVBoxLayout;

    toolUi = new ToolUI;
    layoutMain->addWidget(toolUi);
    setTestDrop();
    interDetectQuery = new InterfaceDetectQuery;
    layoutMain->addWidget(interDetectQuery);

    setLayout(layoutMain);

    //测试落格主体类
    testDrop = new TestDrop(this, this);

    //创建信号和槽连接
    createSlotSignal();
}

void TestUI::buttonAddExpressIdBoxIdClicked()
{
    //插入一行
    int row = tableShowExpressBoxId->rowCount();
    tableShowExpressBoxId->insertRow(row);
}

void TestUI::buttonDelExpressIdBoxIdClicked()
{
    //从expressIdBoxIdHash删除选中的行
    QList<QTableWidgetSelectionRange> selectedItem = tableShowExpressBoxId->selectedRanges();

    for (int i = 0; i < selectedItem.length(); ++i) {
        int topRow = selectedItem[i].topRow();
        int bottomRow = selectedItem[i].bottomRow();
        for (int row = bottomRow; row >= topRow; --row) {
            QTableWidgetItem *item0 = tableShowExpressBoxId->item(row, 0);
            if (item0) {
                testDrop->deleteOneExpressIdBoxId(item0->text());
            }
            tableShowExpressBoxId->removeRow(row);
        }
    }
}

void TestUI::tableShowExpressBoxIdCellChanged(int row, int /*column*/)
{
    //获取对应行的快递号和格口号
    QTableWidgetItem *item0 = tableShowExpressBoxId->item(row, 0);
    QTableWidgetItem *item1 = tableShowExpressBoxId->item(row, 1);
    if (item0 && item1) {
        if (!isComoCarrierIdSelected) {
            QString expressId = item0->text();
            QString boxId = item1->text();
            if (!expressId.isEmpty() && !boxId.isEmpty()) {
                testDrop->addOneExpressIdBoxId(expressId, boxId.toShort());
            }
        }
    }
}

void TestUI::showData(QString expressId, QString carrierId, QString boxId, QString camera)
{
    editExpressId->setText(expressId);
    editCarrierId->setText(carrierId);
    editBoxId->setText(boxId);
    editCamera->setText(camera);
}

void TestUI::createSlotSignal()
{
    connect(buttonAddExpressIdBoxId, &QPushButton::clicked,
            this, &TestUI::buttonAddExpressIdBoxIdClicked);
    connect(tableShowExpressBoxId, &QTableWidget::cellChanged,
            this, &TestUI::tableShowExpressBoxIdCellChanged);
    //删除选中行
    connect(buttonDelExpressIdBoxId, &QPushButton::clicked,
            this, &TestUI::buttonDelExpressIdBoxIdClicked);

    connect(buttonLoad, &QPushButton::clicked, this, &TestUI::loadFileToTable);
    connect(buttonSave, &QPushButton::clicked, this, &TestUI::saveDataToFile);

    connect(buttonConnect, &QPushButton::clicked, testDrop, &TestDrop::openCamera);
    connect(buttonClsoe, &QPushButton::clicked, testDrop, &TestDrop::closeCamera);
}

void TestUI::setTestDrop()
{
    layoutGroupTestDrop = new QHBoxLayout; //群组框布局-水平布局

    groupTestDrop = new QGroupBox;

    editExpressId = new QLineEdit;
    editCarrierId = new QLineEdit;
    editBoxId = new QLineEdit;
    labelShowStatus = new QLabel;
    editCamera = new QLineEdit;
    buttonConnect = new QPushButton("连接");

    buttonClsoe = new QPushButton("关闭");

    layoutExpressId = new QFormLayout;
    layoutExpressId->addRow("快递编号：", editExpressId);

    layoutCarrierId = new QFormLayout;
    layoutCarrierId->addRow("小车编号：", editCarrierId);

    layoutBoxId = new QFormLayout;
    layoutBoxId->addRow("格口编号：", editBoxId);

    layoutCamera = new QFormLayout;
    layoutCamera->addRow("相机名称：", editCamera);

    layoutButton = new QHBoxLayout;
    layoutButton->addWidget(buttonConnect);
    layoutButton->addWidget(buttonClsoe);
    layoutButton->addStretch();

    layoutShowData = new QVBoxLayout; //展示数据区的布局
    layoutShowData->addLayout(layoutExpressId);
    layoutShowData->addLayout(layoutCarrierId);
    layoutShowData->addLayout(layoutBoxId);
    layoutShowData->addLayout(layoutCamera);
    layoutShowData->addStretch();
    layoutShowData->addWidget(labelShowStatus);
    layoutShowData->addLayout(layoutButton);

    buttonAddExpressIdBoxId = new QPushButton("添加"); //添加TestDrop::expressIdBoxIdHash添加格口和快递号
    buttonEditExpressIdBoxId = new QPushButton("编辑"); //编辑TestDrop::expressIdBoxIdHash添加格口和快递号
    buttonDelExpressIdBoxId = new QPushButton("删除"); //删除TestDrop::expressIdBoxIdHash格口和快递号
    buttonClearExpressIdBoxId = new QPushButton("清空"); //清空
    buttonLoad = new QPushButton("加载");
    buttonSave = new QPushButton("保存");

    //按钮加入到布局
    layoutButtonExpressBoxId = new QVBoxLayout;
    layoutButtonExpressBoxId->addWidget(buttonAddExpressIdBoxId);
    layoutButtonExpressBoxId->addWidget(buttonEditExpressIdBoxId);
    layoutButtonExpressBoxId->addWidget(buttonDelExpressIdBoxId);
    layoutButtonExpressBoxId->addWidget(buttonClearExpressIdBoxId);
    layoutButtonExpressBoxId->addWidget(buttonSave);
    layoutButtonExpressBoxId->addWidget(buttonLoad);
    layoutButtonExpressBoxId->addStretch();

    tableShowExpressBoxId = new QTableWidget(0, 2); //显示已添加的快递和格口表格
    tableShowExpressBoxId->setHorizontalHeaderLabels(QStringList() << "快递编号" << "格口编号");

    layoutTableShowExpressBoxId = new QHBoxLayout;
    layoutTableShowExpressBoxId->addWidget(tableShowExpressBoxId);
    layoutTableShowExpressBoxId->addLayout(layoutButtonExpressBoxId);

    //加入到群组框布局
    layoutGroupTestDrop->addLayout(layoutShowData);
    layoutGroupTestDrop->addSpacing(30);
    layoutGroupTestDrop->addLayout(layoutTableShowExpressBoxId);
    layoutGroupTestDrop->addStretch();
    //设置为群组框布局
    groupTestDrop->setLayout(layoutGroupTestDrop);
    //将群组框加入到界面主布局器
    layoutMain->addWidget(groupTestDrop);
}

void TestUI::saveDataToFile()
{
    QFile file(FILE_TEST);
    file.close(); //先清空

    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(nullptr, "警告", "文件打开失败");
        return;
    }
    QTextStream write(&file);
    int rows = tableShowExpressBoxId->rowCount();
    for (int row = 0; row < rows; ++row) {
        QTableWidgetItem *item0 = tableShowExpressBoxId->item(row, 0);
        QTableWidgetItem *item1 = tableShowExpressBoxId->item(row, 1);

        if (item0 && item1) {
            QString expressId = item0->text().trimmed();
            QString boxId = item1->text().trimmed();

            write << QString("%1,%2").arg(expressId, boxId) << "\r\n";
        }
    }
}

void TestUI::loadFileToTable()
{
    QFile file(FILE_TEST);

    if (!file.open(QIODevice::ReadOnly)) {
        emit appendDetailLog("测试文件打开失败");
        return;
    }

    while (!file.atEnd()) {
        int rows = tableShowExpressBoxId->rowCount();
        tableShowExpressBoxId->insertRow(rows);
        QString line = file.readLine();
        QString expressId = line.section(',', 0, 0).trimmed();
        QString boxId = line.section(',', 1, 1).trimmed();
        QTableWidgetItem *item0 = new QTableWidgetItem(expressId);
        QTableWidgetItem *item1 = new QTableWidgetItem(boxId);
        tableShowExpressBoxId->setItem( rows, 0,item0);
        tableShowExpressBoxId->setItem( rows, 1,item1);

        testDrop->addOneExpressIdBoxId(expressId, boxId.toShort());
    }
}


