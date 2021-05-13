#ifndef CONFIGUREDATAQUERYSTATISTICS_H
#define CONFIGUREDATAQUERYSTATISTICS_H

#include <QSettings>
#include <QObject>

class ConfigureDataQueryStatistics : public QObject
{
    Q_OBJECT
public:
    explicit ConfigureDataQueryStatistics(QObject *parent = nullptr);

    static ConfigureDataQueryStatistics *instance();

    QString isPackSuccess() { return _isPackSuccess; }
    QString isPackFail() { return _isPackFail; }
    QStringList monthList() { return _monthList; }
    QStringList todayList() { return _todayList; }
    QStringList yesterdayList() { return _yesterdayList; }

private:
    void setConfigure(const QString key, const QVariant value);
    QVariant getConfigure(const QString key);

    QSettings *dqs;
    static ConfigureDataQueryStatistics *configure;

    QString _isPackSuccess;
    QString _isPackFail;
    QStringList _monthList;
    QStringList _todayList;
    QStringList _yesterdayList;

signals:

public slots:
    void getAllConfigure();
};

#endif // CONFIGUREDATAQUERYSTATISTICS_H
