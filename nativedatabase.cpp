#include "nativedatabase.h"
#include "Configure/configure.h"
#include "define.h"

#include <QSqlError>
#include <QDateTime>
#include <QSqlQuery>
#include <QThread>

NativeDatabase::NativeDatabase(QObject *parent) : QObject(parent)
{
    log = Log::instance();
    connect(this, &NativeDatabase::appendDetailLog, log, &Log::appendDetailLog);

    companyVector.resize(COMPANYNUM + 1);
    companyVector[Sto] = "sto";
    companyVector[Zto] = "zto";
    companyVector[Yto] = "yto";
    companyVector[Best] = "best";
    companyVector[Tt] = "tt";
    companyVector[Yunda] = "yunda";
    companyVector[Youzheng] = "youzheng";
    companyVector[Exception] = "exception";

    connection();
}

void NativeDatabase::connection()
{
    if (QSqlDatabase::contains("nativeDB")) {
        db = QSqlDatabase::database("nativeDB");
    } else {
        db = QSqlDatabase::addDatabase("QMYSQL", "nativeDB");
    }

    db.setHostName(Configure::instance()->nativeip());
    db.setUserName(NATIVE_DB_USERNAME);
    db.setPassword(NATIVE_DB_PASSWORD);
    db.setDatabaseName(NATIVE_DB_DBNAME);
    if (db.open())
        emit appendDetailLog("本地数据库：本地数据库连接成功！");
    else
        emit appendDetailLog("本地数据库：" + db.lastError().text());
}

void NativeDatabase::insertCome(CompanyIndex company, QString expressId, short boxId, QString pdaCode,
                                QString Code1, QString Code2, QString Code3, QString Dest_Org)
{
    //emit appendDetailLog(QString("函数：%1，行数：%2").arg(__func__).arg(__LINE__));
    switch (company) {
    case Sto:
        insertComeSto(expressId, boxId, pdaCode, Code1, Code2, Code3);
        break;
    case Zto:
        insertComeZto(expressId, boxId);
        break;
    case Yto:
        insertComeYto(expressId, boxId/*, Dest_Org*/);
        break;
    case Best:
        insertComeBest(expressId, boxId);
        break;
    case Tt:
        insertComeTt(expressId, boxId);
        break;
    case Yunda:
        insertComeYunda(expressId, boxId);
        break;
    case Youzheng:
        insertComeYouzheng(expressId, boxId);
        break;
    case Exception:
        insertComeException(expressId, boxId);
        break;
    default:
        ;
    }
}

void NativeDatabase::insertGo(CompanyIndex company, QString expressId, short boxId,
                              QString Code1, QString Code2, QString Code3, QString Dest_Org)
{
    switch (company) {
    case Sto:
        insertGoSto(expressId, boxId);
        break;
    case Zto:
        insertGoZto(expressId, boxId);
        break;
    case Yto:
        insertGoYto(expressId, boxId/*, Dest_Org*/);
        break;
    case Best:
        insertGoBest(expressId, boxId);
        break;
    case Tt:
        insertGoTt(expressId, boxId);
        break;
    case Yunda:
        insertGoYunda(expressId, boxId);
        break;
    case Youzheng:
        insertGoYouzheng(expressId, boxId);
        break;
    case Exception:
        insertGoException(expressId, boxId);
        break;
    default:
        ;
    }
}

int NativeDatabase::insertComeSto(QString expressId, short boxId, QString pdaCode,
                                  QString Code1, QString Code2, QString Code3)
{
    //emit appendDetailLog(QString("函数：%1，行数：%2").arg(__func__).arg(__LINE__));
    QString insertSql = QString(
          "INSERT INTO %1"
          "(EXPRESSFLOW_ID, STATUS, EXPRESS_SN, BOX_NUM, DELIVERY_TYPE, CREATE_TIME, User_id, Code1, Code2, Code3, pdaCode) "
          "VALUES('%2', 0, '%2', '%3', 0, '%4', 'DbTest', '%5', '%6', '%7', '%8') "
          "ON DUPLICATE KEY "
          "UPDATE BOX_NUM = '%3'")
          .arg(NATIVE_DB_STO_COME).arg(expressId).arg(boxId)
          .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
          .arg(Code1).arg(Code2).arg(Code3).arg(pdaCode);
    //emit appendDetailLog(QString("函数：%1，行数：%2").arg(__func__).arg(__LINE__));
    emit appendDetailLog(insertSql);
    //将结果插入数据库
    QSqlQuery query(insertSql, db);
    //emit appendDetailLog(QString("函数：%1，行数：%2").arg(__func__).arg(__LINE__));
    return query.numRowsAffected();
}

int NativeDatabase::insertGoSto(QString expressId, short boxId)
{
    QString insertSql = QString(
          "INSERT INTO %1"
          "(EXPRESSFLOW_ID, STATUS, EXPRESS_SN, BOX_NUM, DELIVERY_TYPE, CREATE_TIME, User_id) "
          "VALUES('%2', 0, '%2', '%3', 0, '%4', 'DbTest') "
          "ON DUPLICATE KEY "
          "UPDATE BOX_NUM = '%3'")
          .arg(NATIVE_DB_STO_GO).arg(expressId).arg(boxId)
          .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    emit appendDetailLog(insertSql);
    //将结果插入数据库
    QSqlQuery query(insertSql, db);

    return query.numRowsAffected();
}

int NativeDatabase::insertComeYto(QString expressId, short boxId, QString Dest_Org)
{
    //emit appendDetailLog(QString("函数：%1，行数：%2").arg(__func__).arg(__LINE__));
    QString insertSql = QString(
          "INSERT INTO %1"
          "(EXPRESSFLOW_ID, STATUS, EXPRESS_SN, BOX_NUM, DELIVERY_TYPE, CREATE_TIME, User_id) "
          "VALUES('%2', 0, '%2', '%3', 0, '%4', 'DbTest') "
          "ON DUPLICATE KEY "
          "UPDATE BOX_NUM = '%3'")
          .arg(NATIVE_DB_YTO_COME).arg(expressId).arg(boxId)
          .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
          /*.arg(Dest_Org)*/;
    emit appendDetailLog(insertSql);

    //将结果插入数据库
    QSqlQuery query(insertSql, db);
    //emit appendDetailLog(QString("函数：%1，行数：%2").arg(__func__).arg(__LINE__));
    return query.numRowsAffected();
}

int NativeDatabase::insertGoYto(QString expressId, short boxId, QString Dest_Org)
{
    QString insertSql = QString(
          "INSERT INTO %1"
          "(EXPRESSFLOW_ID, STATUS, EXPRESS_SN, BOX_NUM, DELIVERY_TYPE, CREATE_TIME, User_id) "
          "VALUES('%2', 0, '%2', '%3', 0, '%4', 'DbTest') "
          "ON DUPLICATE KEY "
          "UPDATE BOX_NUM = '%3'")
          .arg(NATIVE_DB_YTO_GO).arg(expressId).arg(boxId)
          .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
          /*.arg(Dest_Org)*/;
    emit appendDetailLog(insertSql);

    //将结果插入数据库
    QSqlQuery query(insertSql, db);

    return query.numRowsAffected();
}

int NativeDatabase::insertComeZto(QString expressId, short boxId)
{
    return 0;
}

int NativeDatabase::insertGoZto(QString expressId, short boxId)
{
    return 0;
}

int NativeDatabase::insertComeBest(QString expressId, short boxId)
{
    return 0;
}

int NativeDatabase::insertGoBest(QString expressId, short boxId)
{
    return 0;
}

int NativeDatabase::insertComeTt(QString expressId, short boxId)
{
    return 0;
}

int NativeDatabase::insertGoTt(QString expressId, short boxId)
{
    return 0;
}

int NativeDatabase::insertComeYunda(QString expressId, short boxId)
{
    return 0;
}

int NativeDatabase::insertGoYunda(QString expressId, short boxId)
{
    return 0;
}

int NativeDatabase::insertComeYouzheng(QString expressId, short boxId)
{
    return 0;
}

int NativeDatabase::insertGoYouzheng(QString expressId, short boxId)
{
    return 0;
}

int NativeDatabase::insertComeException(QString expressId, short boxId)
{
    //emit appendDetailLog(QString("函数：%1，行数：%2").arg(__func__).arg(__LINE__));
    QString insertSql = QString(
          "INSERT INTO %1"
          "(EXPRESSFLOW_ID, STATUS, EXPRESS_SN, BOX_NUM, DELIVERY_TYPE, CREATE_TIME, User_id) "
          "VALUES('%2', 0, '%2', '%3', 0, '%4', 'DbTest') "
          "ON DUPLICATE KEY "
          "UPDATE BOX_NUM = '%3'")
          .arg(NATIVE_DB_EXCEPTION_COME).arg(expressId).arg(boxId)
          .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    //emit appendDetailLog(QString("函数：%1，行数：%2").arg(__func__).arg(__LINE__));
    emit appendDetailLog(insertSql);
    //将结果插入数据库
    QSqlQuery query(insertSql, db);
    //emit appendDetailLog(QString("函数：%1，行数：%2").arg(__func__).arg(__LINE__));
    return query.numRowsAffected();
}

int NativeDatabase::insertGoException(QString expressId, short boxId)
{
    //emit appendDetailLog(QString("函数：%1，行数：%2").arg(__func__).arg(__LINE__));
    QString insertSql = QString(
          "INSERT INTO %1"
          "(EXPRESSFLOW_ID, STATUS, EXPRESS_SN, BOX_NUM, DELIVERY_TYPE, CREATE_TIME, User_id) "
          "VALUES('%2', 0, '%2', '%3', 0, '%4', 'DbTest') "
          "ON DUPLICATE KEY "
          "UPDATE BOX_NUM = '%3'")
          .arg(NATIVE_DB_EXCEPTION_GO).arg(expressId).arg(boxId)
          .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    //emit appendDetailLog(QString("函数：%1，行数：%2").arg(__func__).arg(__LINE__));
    emit appendDetailLog(insertSql);
    //将结果插入数据库
    QSqlQuery query(insertSql, db);
    //emit appendDetailLog(QString("函数：%1，行数：%2").arg(__func__).arg(__LINE__));
    return query.numRowsAffected();
}

int NativeDatabase::insertTotalCome(CompanyIndex company, QString expressId, short boxId, QString Dest_Org)
{
    //emit appendDetailLog(QString("函数：%1，行数：%2").arg(__func__).arg(__LINE__));
    QString insertSql = QString(
       "INSERT INTO %1"
       "(EXPRESSFLOW_ID, STATUS, EXPRESS_SN, BOX_NUM, DELIVERY_TYPE, CREATE_TIME, User_id, Dest_Org, Company) "
       "VALUES('%2', 0, '%2', '%3', 0, '%4', 'DbTest', '%5', '%6') "
       "ON DUPLICATE KEY "
       "UPDATE BOX_NUM = '%3', Company = '%6'")
       .arg(NATIVE_DB_TOTAL_COME).arg(expressId).arg(boxId)
       .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
       .arg(Dest_Org).arg(companyVector.value(company));
    emit appendDetailLog(insertSql);

    //将结果插入数据库
    QSqlQuery query(insertSql, db);

    return query.numRowsAffected();
}

int NativeDatabase::insertTotalGo(CompanyIndex company, QString expressId, short boxId, QString Dest_Org)
{
    //emit appendDetailLog(QString("函数：%1，行数：%2").arg(__func__).arg(__LINE__));
    QString insertSql = QString(
       "INSERT INTO %1"
       "(EXPRESSFLOW_ID, STATUS, EXPRESS_SN, BOX_NUM, DELIVERY_TYPE, CREATE_TIME, User_id, Dest_Org, Company) "
       "VALUES('%2', 0, '%2', '%3', 0, '%4', 'DbTest', '%5', '%6') "
       "ON DUPLICATE KEY "
       "UPDATE BOX_NUM = '%3', Company = '%6'")
       .arg(NATIVE_DB_TOTAL_GO).arg(expressId).arg(boxId)
       .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
       .arg(Dest_Org).arg(companyVector.value(company));
    emit appendDetailLog(insertSql);

    //将结果插入数据库
    QSqlQuery query(insertSql, db);

    return query.numRowsAffected();
}
