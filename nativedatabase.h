#ifndef NATIVEDATABASE_H
#define NATIVEDATABASE_H

#include <QObject>
#include <QSqlDatabase>

#include "define.h"
#include "log.h"

class NativeDatabase : public QObject
{
    Q_OBJECT
public:
    explicit NativeDatabase(QObject *parent = nullptr);

    void connection();
    void insertCome(CompanyIndex company, QString expressId, short boxId, QString pdaCode,
                    QString Code1 = QString() , QString Code2 = QString(),
                    QString Code3 = QString(), QString Dest_Org = QString());
    void insertGo(CompanyIndex company, QString expressId, short boxId,
                  QString Code1 = QString() , QString Code2 = QString(),
                  QString Code3 = QString(),QString Dest_Org = QString());

    int insertComeSto(QString expressId, short boxId, QString pdaCode,
                      QString Code1=QString(), QString Code2=QString(), QString Code3=QString());
    int insertGoSto(QString expressId, short boxId);

    int insertComeYto(QString expressId, short boxId, QString Dest_Org = QString());
    int insertGoYto(QString expressId, short boxId, QString Dest_Org = QString());

    int insertComeZto(QString expressId, short boxId);
    int insertGoZto(QString expressId, short boxId);

    int insertComeBest(QString expressId, short boxId);
    int insertGoBest(QString expressId, short boxId);
    int insertComeTt(QString expressId, short boxId);
    int insertGoTt(QString expressId, short boxId);
    int insertComeYunda(QString expressId, short boxId);
    int insertGoYunda(QString expressId, short boxId);
    int insertComeYouzheng(QString expressId, short boxId);
    int insertGoYouzheng(QString expressId, short boxId);

    int insertComeException(QString expressId, short boxId);
    int insertGoException(QString expressId, short boxId);

    int insertTotalCome(CompanyIndex company, QString expressId, short boxId, QString Dest_Org = QString());
    int insertTotalGo(CompanyIndex company, QString expressId, short boxId, QString Dest_Org = QString());

signals:
    void appendDetailLog(QString);

public slots:

private:
    QSqlDatabase db;
    QVector<QString> companyVector;

    Log *log;
};

#endif // NATIVEDATABASE_H
