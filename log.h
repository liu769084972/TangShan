#ifndef LOG_H
#define LOG_H

#include <QObject>
#include <QFile>
#include <QTextStream>

class Log : public QObject
{
    Q_OBJECT
private:
    explicit Log(QObject *parent = nullptr);

public:
    static Log* instance();

signals:
   void appendAlarmWindow(QString message);

public slots:
    void appendRunLog(QString mes); //运行日志
    void appendDetailLog(QString mes); //详细日志
    void appendDispatchLog(QString mes); //发件日志
    void appendArriveLog(QString mes); //到件日志
    void slot_timeOutPlcLog(QString mes); //发送超时
    void slot_rangeOutPlcLog(QString mes); //超过区间
    void slot_InterceptorLog(QString mes); //拦截件日志
    void slot_ExceptionLog(QString mes); //异常件日志
private:
    void openFile();

private:
    QFile *runLog;
    QTextStream *runLogWrite;
    QFile *detailLog;
    QTextStream *detailLogWrite;
    QFile *arriveLog;
    QTextStream *arriveWrite;
    QFile *dispatchLog;
    QTextStream *dispatchWrite;
    QFile *timeOutPlcLog;
    QTextStream *timeOutPlcWrite;
    QFile *rangeOutPlcLog;
    QTextStream *rangeOutPlcWrite;
    QFile *InterceptorLog;
    QTextStream *InterceptorWrite;
    QFile *ExceptionLog;
    QTextStream *ExceptionWrite;

    static Log *log;
    static QThread *thread;

};

#endif // LOG_H
