#ifndef YUNDA_H
#define YUNDA_H

#include <QObject>

class Express;

class YunDa : public QObject
{
    Q_OBJECT
public:
    explicit YunDa(bool _comeOrGo, Express *_express, QObject *parent = nullptr);

    void getBoxId(QString expressId);

signals:

public slots:

private:
    Express *express;
    bool comeOrGo;
};

#endif // YUNDA_H
