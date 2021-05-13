#ifndef PACK_H
#define PACK_H

#include <QObject>
#include <QTcpServer>
#include <QNetworkAccessManager>

#include "plc.h"
#include "Work/Pack/packsto.h"
#include "Work/Pack/packyto.h"

class Pack : public QObject
{
    Q_OBJECT
public:
    explicit Pack(QObject *parent = nullptr);

private:


signals:
    void appendPackWindow(QString message);
    void appendDetailLog(QString message);
    void appendAlarmWindow(QString message);

public slots:


private:
    PackSTO *packSto;
    PackYTO *packYto;

};

#endif // PACK_H
