#include "best.h"
#include "Work/Sort/express.h"

BEST::BEST(bool _comeOrGo, Express *_express, QObject *parent)
    : QObject(parent),
      express(_express),
      comeOrGo(_comeOrGo)
{

}

void BEST::getBoxId(QString expressId)
{

}
