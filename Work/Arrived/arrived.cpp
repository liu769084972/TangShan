#include "arrived.h"

Arrived::Arrived(QObject *parent) : QObject(parent)
{
    arrivedSto = new ArrivedSTO(this);
    arrivedYto = new ArrivedYTO(this);
}
