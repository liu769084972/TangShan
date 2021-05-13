#include "tt.h"

TT::TT(bool _comeOrGo, Express *_express, QObject *parent)
    : QObject(parent),
      express(_express),
      comeOrGo(_comeOrGo)

{

}

void TT::getBoxId(QString expressId)
{

}
