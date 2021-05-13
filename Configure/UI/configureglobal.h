#ifndef CONFIGUREGLOBAL_H
#define CONFIGUREGLOBAL_H

#include <QGroupBox>
#include <QVBoxLayout>
#include <QTableWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QFormLayout>
#include <QLabel>

class ConfigureGlobal : public QGroupBox
{
    Q_OBJECT
public:
    ConfigureGlobal(QWidget *parent = nullptr);

public slots:
    void buttonAddClicked();
    void buttonEditClicked();
    void buttonDelClicked();
    void tableCameraCellChanged(int row, int column);
    void editNetIpTextChanged(QString text);
    void editNativeIpTextChanged(QString text);
    void editMaxCarrierIdTextChanged(QString text);

private:
    void loadFileToTableCamera(); //从文件加载数据到表格-保持一致性

private:
    QLabel *labelTableTip; //提示表格内容格式
    QTableWidget *tableCamera;
    QPushButton *buttonAdd;
    QPushButton *buttonEdit;
    QPushButton *buttonDel;
    QVBoxLayout *layoutTableLabel;
    QHBoxLayout *layoutTableButton;
    QHBoxLayout *layoutButton;
    QHBoxLayout *layoutGroupDevice;
    QStringList cameraArgList; //代表表格每行的数据，如果对应的项不存在，或者文本为空，则指为""

    QLineEdit *editNetIp;
    QLineEdit *editNativeIp;
    QLineEdit *editMaxCarrierId;
    QLineEdit *editMaxBoxId;
    QFormLayout *layoutDeviceEdit;
};

#endif // CONFIGUREGLOBAL_H
