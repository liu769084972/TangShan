#include "pack.h"
#include "Configure/configure.h"

Pack::Pack(QObject *parent) : QObject(parent)
{
    packSto = new PackSTO(this);
    packYto = new PackYTO(this);
}
