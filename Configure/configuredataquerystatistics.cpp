#include "configuredataquerystatistics.h"
#include "define.h"
#include <QFile>
#include <QTextCodec>

class ConfigureDataQueryStatistics * ConfigureDataQueryStatistics::configure = nullptr;
ConfigureDataQueryStatistics::ConfigureDataQueryStatistics(QObject *parent) : QObject(parent)
{
    dqs = new QSettings(FILE_CONFIGUREDATAQUERYSTATISTICS, QSettings::IniFormat);

    dqs->setIniCodec(QTextCodec::codecForName("UTF-8")); //防止中文乱码

    getAllConfigure();
}

ConfigureDataQueryStatistics *ConfigureDataQueryStatistics::instance()
{
    if (configure != nullptr)
        return configure;
    else {
        configure = new ConfigureDataQueryStatistics(nullptr);
        return configure;
    }
}

void ConfigureDataQueryStatistics::setConfigure(const QString key, const QVariant value)
{
    dqs->setValue(key, value);
}

QVariant ConfigureDataQueryStatistics::getConfigure(const QString key)
{
    return dqs->value(key);
}

void ConfigureDataQueryStatistics::getAllConfigure()
{
    _isPackSuccess = getConfigure("ArrDelDispBag/packSuccess").toString();
    _isPackFail = getConfigure("ArrDelDispBag/packFail").toString();
    QStringList _monthList = getConfigure("Time/month").toStringList();
    QStringList _todayList = getConfigure("Time/today").toStringList();
    QStringList _yesterdayList = getConfigure("Time/yesterday").toStringList();
}
