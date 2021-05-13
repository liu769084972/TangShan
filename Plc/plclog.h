#ifndef PLCLOG_H
#define PLCLOG_H

#include <QObject>
#include "plc.h"

class MainWindow;
class PlcLog : public QObject
{
    Q_OBJECT
public:
    explicit PlcLog(MainWindow *m, PLC *plc, QObject *parent = nullptr);

private:
    void timerEvent(QTimerEvent *);
    PLC *plc;
    MainWindow *m;

signals:
    void sig_rangeOutPlcLog(QString mes);

public slots:
    void init();
};

#endif // PLCLOG_H
