#include "interfacedetectquery.h"
#include "Configure/configure.h"
#include "define.h"

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QTextCodec>

InterfaceDetectQuery::InterfaceDetectQuery(QWidget *parent)
    : QGroupBox(parent)
{
    layout1 = new QVBoxLayout;
    layout2 = new QHBoxLayout;
    layout3 = new QFormLayout;

    textEdit = new QTextEdit;
    editExpressId = new QLineEdit;
    layout3->addRow("快递号:", editExpressId);
    comboCompany = new QComboBox;
    comboCompany->addItem("全部");
    comboCompany->setItemData(0, COMPANYNUM + 1, Qt::UserRole + 1);
    comboCompany->addItem("申通");
    comboCompany->setItemData(1, Sto, Qt::UserRole + 1);
    comboCompany->addItem("圆通");
    comboCompany->setItemData(2, Yto, Qt::UserRole + 1);

    comboComeOrGo = new QComboBox;
    comboComeOrGo->addItem("进港");
    comboComeOrGo->addItem("出港");

    button = new QPushButton("查询检测");
    connect(button, &QPushButton::clicked, this, [=]()
    {
        switch (comboCompany->currentData(Qt::UserRole + 1).toInt()) {
        case Sto:
            this->querySTO(editExpressId->text().trimmed());
            break;
        case Yto:
            this->queryYTO(editExpressId->text().trimmed());
            break;
        case COMPANYNUM + 1:
            this->querySTO(editExpressId->text().trimmed());
            this->queryYTO(editExpressId->text().trimmed());
        }
    });

    buttonClear = new QPushButton("清空内容");
    connect(buttonClear, &QPushButton::clicked, textEdit, &QTextEdit::clear);

    layout1->addWidget(comboComeOrGo);
    layout1->addWidget(comboCompany);
    layout1->addLayout(layout3);
    layout1->addWidget(button);
    layout1->addWidget(buttonClear);

    layout2->addWidget(textEdit);
    layout2->addLayout(layout1);
    //layout2->addStretch();

    setLayout(layout2);
}

void InterfaceDetectQuery::querySTO(QString expressId)
{
    textEdit->append("-----------------------申通------------------------------");
    //连接阿里云
    QSqlDatabase db;
    if (QSqlDatabase::contains()) {
        db = QSqlDatabase::database();
    } else {
        db = QSqlDatabase::addDatabase("QMYSQL", "ALYTest");
    }

    db.setHostName(Configure::instance()->configureSTO->alyIP());
    db.setUserName(Configure::instance()->configureSTO->alyUserName());
    db.setPassword(Configure::instance()->configureSTO->alyPassword());
    db.setConnectOptions("MYSQL_OPT_RECONNECT=1"); //断开自动重连

    if (!db.open()) {
        textEdit->append(QString("<font style='color:red'>阿里云：%1</font>").arg(db.lastError().text()));
        return;
    }

    //查询
    QString sql = QString("SELECT Code1, Code2, Code3 FROM orderautocategory.orderrecevie "
                          "WHERE WAYBILLNO = '%1' ORDER BY SysRecevieTime").arg(expressId);
    textEdit->append(sql);

    QSqlQuery query(sql, db);
    QVector<QList<short> > normal; //正常格口存放数组
    QString _Code1, _Code2, _Code3;
    int priority = -1; //优先级
    QList<short> boxIdList;
    QJsonObject &OrderTypeEx = Configure::instance()->configureSTO->OrderTypeEx();
    QHash<QStringList, QList<short> > rule;

    if (comboComeOrGo->currentText() == "进港") { //进港
        rule = Configure::instance()->configureSTO->ruleStoCome();
        //开始处理
        while (query.next()) {
            //根据规则获取格口，正常口加入数组
            QString Code1 = query.value("Code1").toString();
            QString Code2 = query.value("Code2").toString();
            QString Code3 = query.value("Code3").toString();
            textEdit->append(QString("一段码：%1，二段码：%2，三段码：%3").arg(Code1).arg(Code2).arg(Code3));

//            QStringList& code1List = Configure::instance()->configureSTO->code1ListContain();
//            QStringList& code2List = Configure::instance()->configureSTO->code2ListContain();
            //找到对应的格口
            QHashIterator<QStringList, QList<short> > iterator(rule);
            while (iterator.hasNext()) {
                iterator.next();
                QStringList codeList = iterator.key();
                QString _Code1 = codeList.value(0);
                QString _Code2 = codeList.value(1);
                QString _Code3 = codeList.value(2);
                if ((_Code3 == Code3) && (_Code2 == Code2) && (_Code1 == Code1)) {
                    boxIdList = iterator.value();
                }
            }

            if (boxIdList.size() > 0) {
                QString orderTypeEx = query.value("OrderTypeEx").toString();
                int _priority = OrderTypeEx[orderTypeEx].toInt();
                textEdit->append(QString("优先级：%1-%2").arg(orderTypeEx).arg(_priority));
                if (_priority >= priority) {
                    normal.append(boxIdList);
                    priority = _priority;
                    _Code1 = Code1;
                    _Code2 = Code2;
                    _Code3 = Code3;
                }
            } else {
                QString orderTypeEx = query.value("OrderTypeEx").toString();
                int _priority = OrderTypeEx[orderTypeEx].toInt();
                if (_priority >= priority) {
                    priority = _priority;
                    _Code1 = Code1;
                    _Code2 = Code2;
                    _Code3 = Code3;
                }
            }
        }

        if (normal.isEmpty()) { //异常
            textEdit->append(QString("申通获取为异常"));
        } else { //正常口
            boxIdList = normal.last();
            QString boxIdStr;
            foreach(short boxId, boxIdList) {
                boxIdStr += QString::number(boxId) + ',';
            }
            textEdit->append(QString("申通获取为正常，格口%1").arg(boxIdStr));
        }
    } else { //出港
            //先查拦截件
            if (Configure::instance()->configureSTO->interceptExpress().contains(expressId))
                boxIdList.append(Configure::instance()->configureSTO->interceptBoxId());

            //开始处理
            rule = Configure::instance()->configureSTO->ruleStoGo();
            while (query.next()) {
                QString Code1 = query.value("Code1").toString();
                QString Code2 = query.value("Code2").toString();
                QString Code3 = query.value("Code3").toString();

                //找到对应的格口
                QHashIterator<QStringList, QList<short> > iterator(rule);
                while (iterator.hasNext()) {
                    iterator.next();
                    QStringList codeList = iterator.key();
                    QString _Code1 = codeList.value(0);
                    QString _Code2 = codeList.value(1);
                    QString _Code3 = codeList.value(2);
                    bool bCode1 = false;
                    bool bCode2 = false;
                    bool bCode3 = false;
                    if (_Code1.isEmpty() || ((!_Code1.isEmpty()) && (_Code1 == Code1))) { //空的时候表示任意，即一定满足，所以不用比较
                        bCode1 = true;
                    }
                    if (_Code2.isEmpty() || ((!_Code2.isEmpty()) && (_Code2 == Code2))) { //空的时候表示任意，即一定满足，所以不用比较
                        bCode1 = true;
                    }
                    if (_Code3.isEmpty() || ((!_Code3.isEmpty()) && (_Code3 == Code3))) { //空的时候表示任意，即一定满足，所以不用比较
                        bCode1 = true;
                    }

                    if (bCode1 && bCode2 && bCode3) {
                        boxIdList = iterator.value();
                        break;
                    }
                }
                //根据规则获取格口
                if (boxIdList.size() > 0) { //正常口
                    QString orderTypeEx = query.value("OrderTypeEx").toString();
                    int _priority = OrderTypeEx[orderTypeEx].toInt();
                    if (_priority >= priority) {
                        normal.append(boxIdList);
                        priority = _priority;
                        _Code1 = Code1;
                        _Code2 = Code2;
                        _Code3 = Code3;
                    }
                } else { //异常口
                    QString orderTypeEx = query.value("OrderTypeEx").toString();
                    int _priority = OrderTypeEx[orderTypeEx].toInt();
                    if (_priority >= priority) {
                        priority = _priority;
                        _Code1 = Code1;
                        _Code2 = Code2;
                        _Code3 = Code3;
                    }
                }
            }

            if (normal.isEmpty()) { //异常
                textEdit->append(QString("申通获取为异常"));
            } else { //正常口
                boxIdList = normal.last();
                QString boxIdStr;
                foreach(short boxId, boxIdList) {
                    boxIdStr += QString::number(boxId) + ',';
                }
                textEdit->append(QString("申通获取为正常，格口%1").arg(boxIdStr));
            }
    }
}

void InterfaceDetectQuery::queryYTO(QString expressId)
{
    textEdit->append("-----------------------圆通------------------------------");
    //连接网关10001
    QTcpSocket tcpSocket;
    QString ip = Configure::instance()->configureYTO->ip();
    quint16 portCode = Configure::instance()->configureYTO->portCode();
    tcpSocket.connectToHost(ip, portCode);
    //写入
    QJsonObject obj;
    obj.insert("aviType", "0");
    obj.insert("trace", "0");
    obj.insert("isFirstTransfer", "1");
    obj.insert("mty", YTO_MTY_SORT);
    obj.insert("respcode", QJsonValue());
    obj.insert("signature", QJsonValue()); //获取签名，需要代码获取，没有就QJsonValue()
    QJsonObject objChild;
    objChild.insert("createTime", QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz"));
    objChild.insert("createUserCode", Configure::instance()->configureYTO->createUserCode());
    objChild.insert("id", QJsonValue());
    objChild.insert("waybillNo", expressId);
    objChild.insert("opCode", YTO_OPCODE_SORT);
    obj.insert("opRecord", objChild);
    QString json = QJsonDocument(obj).toJson(QJsonDocument::Compact);
    int length = json.length();
    QByteArray str = QString("%1%2\r\n").arg(length, 8, 10, QLatin1Char('0')).arg(json).toLatin1();
    textEdit->append(str);
    tcpSocket.write(str);
    bool timeOut = tcpSocket.waitForReadyRead();
    if (!timeOut) {
        textEdit->append("查询超时");
        return;
    }
    //读取返回
    QByteArray result = tcpSocket.readAll();
    QTextCodec *codec = QTextCodec::codecForName("GBK");
    QString strUnicode = codec->toUnicode(result);
    textEdit->append(strUnicode.prepend("圆通返回："));
    //解析获取
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(result.remove(0, 8).trimmed(), &error);
    if (error.error != QJsonParseError::NoError) {
        textEdit->append("读取圆通socket的时候json解析错误");
        return;
    }
    if (doc.isEmpty()) {
        textEdit->append("读取圆通socket的时候json为空");
        return;
    }
    if (!doc.isObject()) {
        textEdit->append("读取圆通socket的时候doc不是一个json对象");
        return;
    }
    QJsonValue value = doc["WAYBILL_NO"];
    if (!value.isString()) {
        textEdit->append("读取圆通socket的时候doc的快递单号不是一个字符串");
        return;
    }
    QString expressId1 = doc["WAYBILL_NO"].toString();
    if (expressId1.isEmpty()) {
        textEdit->append("读取圆通socket的时候doc的快递单号为空");
        return;
    }
    QJsonValue value1 = doc["DA_TOU_BI"];
    if (!value.isString()) {
       textEdit->append("读取圆通socket的时候doc的三段码不是一个字符串");
        return;
    }

    QString code = doc["DA_TOU_BI"].toString();
    QString code1 = code.section('-', 0, 0);
    QString code2 = code.section('-', 1, 1);
    QString code3 = code.right(3);
    textEdit->append(QString("一段码：%1，二段码：%2，三段码：%3").arg(code1).arg(code2).arg(code3));

    if (comboComeOrGo->currentText() == "进港") {
        short boxId = 0;
        if (Configure::instance()->configureYTO->ytoCode1().contains(code1) &&
                Configure::instance()->configureYTO->ytoCode2().contains(code2)) {
            boxId = Configure::instance()->configureYTO->ruleYtoCome().value(code3);
            if (boxId != 0) {
                textEdit->append(QString("圆通获取为正常，格口%1").arg(boxId));
            } else {
                textEdit->append(QString("圆通获取为异常"));
            }
        }
    } else {
        short boxId = Configure::instance()->configureYTO->ruleYtoGo().value(code1);
        if (boxId != 0) {
            textEdit->append(QString("圆通获取为正常，格口%1").arg(boxId));
        } else {
            textEdit->append(QString("圆通获取为异常"));
        }
    }
}

short InterfaceDetectQuery::getBestBoxId(QString code)
{
    QHash<QStringList, QList<short> > rule;
    if (comboComeOrGo->currentText() == "进港") {
        rule = Configure::instance()->configureSTO->ruleStoCome();
    } else {
        rule = Configure::instance()->configureSTO->ruleStoGo();
    }
}
