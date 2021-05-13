#include "yunda.h"

YunDa::YunDa(bool _comeOrGo, Express *_express, QObject *parent)
    : QObject(parent),
      express(_express),
      comeOrGo(_comeOrGo)
{

}

void YunDa::getBoxId(QString expressId)
{

}
