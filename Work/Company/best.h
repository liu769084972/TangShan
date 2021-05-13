#ifndef BEST_H
#define BEST_H

#include <QObject>

class Express;

class BEST : public QObject
{
    Q_OBJECT
public:
    explicit BEST(bool _comeOrGo, Express *_express, QObject *parent = nullptr);

    void getBoxId(QString expressId);

signals:

public slots:

private:
    Express *express;

    bool comeOrGo;
};

#endif // BEST_H
