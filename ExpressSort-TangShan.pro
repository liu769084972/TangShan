#-------------------------------------------------
#
# Project created by QtCreator 2020-06-11T09:13:02
#
#-------------------------------------------------

QT       += core gui sql network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ExpressSort-20210323
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11 console

SOURCES += \
        Alarm/alarm.cpp \
        Alarm/alarmui.cpp \
        Alarm/headerviewcombobox.cpp \
        Configure/UI/configureglobal.cpp \
        Configure/UI/configurestoui.cpp \
        Configure/UI/configureui.cpp \
        Configure/UI/configureytoui.cpp \
        Configure/UI/extra.cpp \
        Configure/UI/filtersto.cpp \
        Configure/UI/filteryto.cpp \
        Configure/UI/othersto.cpp \
        Configure/UI/rule.cpp \
        Configure/configuredataquerystatistics.cpp \
        DataQueryStatistics/dataquerystatistics.cpp \
    Plc/plclog.cpp \
        Test/UI/interfacedetectquery.cpp \
        Test/UI/testui.cpp \
        Test/UI/toolui.cpp \
        Test/interfacetestui.cpp \
        Work/Arrived/arrived.cpp \
        Work/Arrived/arrivedsto.cpp \
        Work/Arrived/arrivedyto.cpp \
        Work/Company/best.cpp \
        Work/Company/sto.cpp \
        Work/Company/tt.cpp \
        Work/Company/youzheng.cpp \
        Work/Company/yto.cpp \
        Work/Company/zto.cpp \
        Configure/configure.cpp \
        Configure/configuresto.cpp \
        Configure/configureyto.cpp \
        Work/Pack/pack.cpp \
        Work/Pack/packsto.cpp \
        Work/Pack/packyto.cpp \
        Work/Send/send.cpp \
        Work/UI/arrivedsendbagui.cpp \
        Work/UI/calculatesortdataui.cpp \
        Work/UI/devicestatusui.cpp \
        Work/UI/sortarrivedsendbag.cpp \
        Work/UI/sortdatashowui.cpp \
        Test/test.cpp \
        Test/testdrop.cpp \
        Work/Sort/camera.cpp \
        Work/Sort/express.cpp \
        Work/UI/sortui.cpp \
        log.cpp \
        main.cpp \
        mainwindow.cpp \
        nativedatabase.cpp \
        plc.cpp \
        Work/Company/yunda.cpp \
        start.cpp \
        Work/Dispatch/dispatchsto.cpp \

HEADERS += \
        Alarm/alarm.h \
        Alarm/alarmui.h \
        Alarm/headerviewcombobox.h \
        Configure/UI/configureglobal.h \
        Configure/UI/configurestoui.h \
        Configure/UI/configureui.h \
        Configure/UI/configureytoui.h \
    Configure/UI/extra.h \
        Configure/UI/filtersto.h \
        Configure/UI/filteryto.h \
    Configure/UI/othersto.h \
        Configure/UI/rule.h \
    Configure/configuredataquerystatistics.h \
        DataQueryStatistics/dataquerystatistics.h \
    Plc/plclog.h \
        Test/UI/interfacedetectquery.h \
        Test/UI/testui.h \
        Test/UI/toolui.h \
        Test/interfacetestui.h \
        Work/Arrived/arrived.h \
        Work/Arrived/arrivedsto.h \
        Work/Arrived/arrivedyto.h \
        Work/Company/best.h \
        Work/Company/sto.h \
        Work/Company/tt.h \
        Work/Company/youzheng.h \
        Work/Company/yto.h \
        Work/Company/zto.h \
        Configure/configure.h \
        Configure/configuresto.h \
        Configure/configureyto.h \
        Work/Pack/pack.h \
        Work/Pack/packsto.h \
        Work/Pack/packyto.h \
        Work/Send/send.h \
        Work/UI/arrivedsendbagui.h \
        Work/UI/calculatesortdataui.h \
        Work/UI/devicestatusui.h \
        Work/UI/sortarrivedsendbag.h \
        Work/UI/sortdatashowui.h \
        Test/test.h \
        Test/testdrop.h \
        Work/Sort/camera.h \
        Work/UI/sortui.h \
        define.h \
        Work/Sort/express.h \
        log.h \
        mainwindow.h \
        nativedatabase.h \
        plc.h \
        Work/Company/yunda.h \
        start.h \
        Work/Dispatch/dispatchsto.h \

FORMS += \
        Alarm/alarm.ui \
        mainwindow.ui \
        start.ui \
        Test/test.ui \
        Test/testdrop.ui \
        DataQueryStatistics/dataquerystatistics.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    Configure/global.ini \
    Configure/inRuleST.csv \
    Configure/interceptExpressId.txt \
    Configure/stoCellOrgCodes.csv \
    Configure/stoCellUserCodes.csv \
    Configure/stoConfigure.ini \
    Configure/stoEffectiveTypes.csv \
    Configure/stoEmpCodes.csv \
    Configure/stoInRule.csv \
    Configure/stoUrbanNextOrgCode.csv \
    Configure/stoUrbanUserCodes.csv \
    Configure/stoZhuJiNextOrgCode.csv \
    Configure/stoZhuJiUserCodes.csv \
    Configure/ytoConfigure.ini \
    Configure/ytoDesOrgCodes.csv \
    Configure/ytoUserCodeUserNames.csv

win32: LIBS += -LC:/TwinCAT/AdsApi/TcAdsDll/Lib/ -lTcAdsDll

INCLUDEPATH += C:/TwinCAT/AdsApi/TcAdsDll/Lib
DEPENDPATH += C:/TwinCAT/AdsApi/TcAdsDll/Lib
# 注意twincat库的位数

RESOURCES += \
    images/images.qrc
