#include "youzheng.h"

YouZheng::YouZheng(bool _comeOrGo, Express *_express, QObject *parent)
    : QObject(parent),
      express(_express),
      comeOrGo(_comeOrGo)
{

}

void YouZheng::getBoxId(QString expressId)
{

}
