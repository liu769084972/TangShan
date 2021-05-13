#ifndef TT_H
#define TT_H

#include <QObject>

class Express;

class TT : public QObject
{
    Q_OBJECT
public:
    explicit TT(bool _comeOrGo, Express *_express, QObject *parent = nullptr);

    void getBoxId(QString expressId);

signals:

public slots:

private:
    Express *express;

    bool comeOrGo;

};

#endif // TT_H
