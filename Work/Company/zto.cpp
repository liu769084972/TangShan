#include "zto.h"

ZTO::ZTO(bool _comeOrGo, Express *_express, QObject *parent)
    : QObject(parent),
      express(_express),
      comeOrGo(_comeOrGo)
{

}

void ZTO::getBoxId(QString expressId)
{

}
