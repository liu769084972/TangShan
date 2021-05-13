#ifndef ARRIVED_H
#define ARRIVED_H

#include <QObject>

#include "Work/Arrived/arrivedsto.h"
#include "Work/Arrived/arrivedyto.h"

class Arrived : public QObject
{
    Q_OBJECT
public:
    explicit Arrived(QObject *parent = nullptr);

signals:

public slots:

private:
    ArrivedSTO *arrivedSto;
    ArrivedYTO *arrivedYto;

};

#endif // ARRIVED_H
