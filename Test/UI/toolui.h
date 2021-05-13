#ifndef TOOLUI_H
#define TOOLUI_H

#include <QGroupBox>
#include <QPushButton>
#include <QHBoxLayout>

class ToolUI : public QGroupBox
{
public:
    ToolUI(QWidget *parent = nullptr);

    QHBoxLayout *layoutMain;

    QPushButton *buttonTcpIpHelper;
    QPushButton *buttonMySqlCommand;
    QPushButton *buttonCMD;
    QPushButton *buttonRemoteDesktop;
};

#endif // TOOLUI_H
