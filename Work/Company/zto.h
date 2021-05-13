#ifndef ZTO_H
#define ZTO_H

#include <QObject>

class Express;

class ZTO : public QObject
{
    Q_OBJECT
public:
    explicit ZTO(bool _comeOrGo, Express *_express, QObject *parent = nullptr);

    void getBoxId(QString expressId);

signals:

public slots:

private:
    Express *express;

    bool comeOrGo;
};

#endif // ZTO_H
