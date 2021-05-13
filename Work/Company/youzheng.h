#ifndef YOUZHENG_H
#define YOUZHENG_H

#include <QObject>

class Express;

class YouZheng : public QObject
{
    Q_OBJECT
public:
    explicit YouZheng(bool _comeOrGo, Express *_express, QObject *parent = nullptr);

    void getBoxId(QString expressId);

signals:

public slots:

private:
    Express *express;
    bool comeOrGo;
};

#endif // YOUZHENG_H
