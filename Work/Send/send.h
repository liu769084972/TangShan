#ifndef SEND_H
#define SEND_H

#include <QObject>

class Send : public QObject
{
    Q_OBJECT
public:
    explicit Send(QObject *parent = nullptr);

signals:

public slots:
};

#endif // SEND_H
