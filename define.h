#ifndef DEFINE_H
#define DEFINE_H

#define LOCAL 0
#define PLC_ON 1
#define DB_ON 1
#define STO_TEST 0
#define TEST 0
#define STO_SIGNATURE 0

#if LOCAL
    #define FILE_GLOBAL_PATH "H:/ZhuJi-ST/ExpressSort/Configure/global.ini"       //configure/global.ini    //

    #define FILE_STO_PATH "H:/ZhuJi-ST/ExpressSort/Configure/stoConfigure.ini"    //configure/stoConfigure.ini //
    #define FILE_STO_COME_RULE "H:/ZhuJi-ST/ExpressSort/Configure/stoInRule.csv"   // //configure/inRuleSto.csv
    #define FILE_STO_GO_RULE "H:/ZhuJi-ST/ExpressSort/Configure/stoOutRule.csv"   //configure/outRuleSto.csv //

    #define FILE_STO_CELLORGCODE "H:/ZhuJi-ST/ExpressSort/Configure/stoCellOrgCodes.csv" //网点公司组织代码
    #define FILE_STO_USERCODE "H:/ZhuJi-ST/ExpressSort/Configure/stoUserCodes.csv" //诸暨扫描员
    #define FILE_STO_URBANUSERCODE "H:/ZhuJi-ST/ExpressSort/Configure/stoUrbanUserCodes.csv" //城区扫描员
    #define FILE_STO_CELLUSERCODE "H:/ZhuJi-ST/ExpressSort/Configure/stoCellUserCodes.csv" //网点公司扫描员
    #define FILE_STO_EMPCODE "H:/ZhuJi-ST/ExpressSort/Configure/stoEmpCodes.csv" //派件员
    #define FILE_STO_EFFECTIVE "H:/ZhuJi-ST/ExpressSort/Configure/stoEffectiveTypes.csv" //快件类型
    #define FILE_STO_NEXTORGCODE "H:/ZhuJi-ST/ExpressSort/Configure/stoNextOrgCodes.csv"

    #define FILE_YTO_PATH "H:/ZhuJi-ST/ExpressSort/Configure/ytoConfigure.ini"   //configure/ytoConfigure.ini //
    #define FILE_YTO_COME_RULE "H:/ZhuJi-ST/ExpressSort/Configure/ytoInRule.csv"   //configure/inRuleYto.csv //
    #define FILE_YTO_GO_RULE "H:/ZhuJi-ST/ExpressSort/Configure/ytoOutRule.csv"   //configure/outRuleYto.csv //

    #define FILE_YTO_USERCODEUSERNAME "H:/ZhuJi-ST/ExpressSort/Configure/ytoUserCodeUserNames.csv" //扫描员和扫描员姓名
    #define FILE_YTO_DESTORGCODE "H:/ZhuJi-ST/ExpressSort/Configure/ytoDesOrgCodes.csv"


    #define FILE_TEST "H:/ZhuJi-ST/ExpressSort/Configure/test.txt" //保存测试文件的路径

#else
    #define FILE_GLOBAL_PATH "configure/global.ini"       //    //H:/ExpressSort/Configure/global.ini

    #define FILE_STO_PATH "configure/stoConfigure.ini"    // //H:/ExpressSort/Configure/stoConfigure.ini
    #define FILE_STO_COME_RULE "configure/stoInRule.csv"   // //H:/ExpressSort/Configure/inRuleSto.csv
    #define FILE_STO_GO_RULE "configure/stoOutRule.csv"   // //H:/ExpressSort/Configure/outRuleSto.csv

    #define FILE_STO "configure/sto.csv"

    #define FILE_YTO_PATH "configure/ytoConfigure.ini"   // //H:/ExpressSort/Configure/ytoConfigure.ini
    #define FILE_YTO_COME_RULE "configure/inRuleYto.csv"   // //H:/ExpressSort/Configure/inRuleYto.csv
    #define FILE_YTO_GO_RULE "configure/outRuleYto.csv"   // //H:/ExpressSort/Configure/outRuleYto.csv

    #define FILE_YTO_USERCODEUSERNAME "configure/ytoUserCodeUserNames.csv" //扫描员和扫描员姓名
    #define FILE_YTO_DESTORGCODE "configure/ytoDesOrgCodes.csv"

    #define FILE_TEST "configure/test.txt" //保存测试文件的路径
    #define FILE_CONFIGUREDATAQUERYSTATISTICS "configure/dataQueryStatistics.ini" //查询统计

#endif

#define COMPANYNUM 7 //公司数量

//本地数据库
#define NATIVE_DB_USERNAME "remoteuser"
#define NATIVE_DB_PASSWORD "express"
#define NATIVE_DB_DBNAME "express_plc"

#define NATIVE_DB_TOTAL_COME "express_inflow" //进港总表
#define NATIVE_DB_TOTAL_GO "express_outflow" //出港总表

#define NATIVE_DB_STO_COME "express_inflow_st" //申通进港表
#define NATIVE_DB_STO_GO "express_outflow_st" //申通出港表

#define NATIVE_DB_YTO_COME "express_inflow_yt" //圆通进港表
#define NATIVE_DB_YTO_GO "express_outflow_yt" //圆通出港表

#define NATIVE_DB_ZTO_COME "express_inflow_zt" //中通进港表
#define NATIVE_DB_ZTO_GO "express_outflow_zt" //中通出港表

#define NATIVE_DB_BS_COME "express_inflow_bs" //百世进港表
#define NATIVE_DB_BS_GO "express_outflow_bs" //百世出港表

#define NATIVE_DB_TT_COME "express_inflow_tt" //天天进港表
#define NATIVE_DB_TT_GO "express_outflow_tt" //天天出港表

#define NATIVE_DB_YUNDA_COME "express_inflow_yd" //韵达进港表
#define NATIVE_DB_YUNDA_GO "express_outflow_yd" //韵达出港表

#define NATIVE_DB_YOUZHENG_COME "express_inflow_yz" //邮政进港表
#define NATIVE_DB_YOUZHENG_GO "express_outflow_yz" //邮政出港表

#define NATIVE_DB_EXCEPTION_COME "express_inflow_exception" //异常进港表
#define NATIVE_DB_EXCEPTION_GO "express_outflow_exception" //异常出港表

//阿里云
#define ALY_DB_DBNAME "orderautocategory"
#define ALY_DB_TABLENAME "orderautocategory.orderrecevie"

//申通
#define STO_CODE_URL "http://%1:9010/selectWayBillNo?wayBillNo="
#define ORDERTYPEEX "{\"Interceptor\":100,\"Rookie\":90,\"PDD\":90,\"bigcustome\":80,\"callcenter\":80,\"taobaoonline\":30,\"taobaooffline\":30}"

#if STO_TEST
    #define STO_URL "http://cloudinter-linkgatewaytest.sto.cn/gateway/link.do"
#else
    #define STO_URL "http://cloudinter-linkgatewayonline.sto.cn/gateway/link.do"
#endif

#define STO_API_NAME  "GALAXY_RECEIVE_SCAN_MESSAGE" //
#define STO_FROM_APPKEY  "CAKTOteSvjtadmZ" //订阅方appkey
#define STO_FROM_CODE  "CAKTOteSvjtadmZ" //订阅方appkey 授权的资源 code
#define STO_TO_APPKEY  "galaxy_receive" //注册方appkey
#define STO_TO_CODE  "galaxy_receive" //注册方appkey 授权的资源 code
#define STO_SECRETKEY "ZK1YcNTW2O3AHzXgWcnQbaLiudi2v95b"

#define STO_ARRIVERD_OPCODE "520" //到件操作码
#define STO_SEND_OPCODE "710" //派件操作码
#define STO_PACK_OPCODE "221" //集包操作码
#define STO_DISPATCH_OPCODE "210" //发件
#define STO_EXPTYPE "10" //单号类型：10-运单号
#define STO_DEVICETYPE "ZDFJ" //设备类型：PDA-巴枪 ZDFJ-自动分拣设备
#define STO_CLIENTPROGRAMROLE "0" // 0-网点 1-中心 2-航空
#define STO_GOODSTYPE "0001" //物品类别：0001-货物 0002-非货样

//圆通
#define YTO_OPCODE_SORT "182"
#define YTO_OPCODE_ARRIVERD "171"
#define YTO_OPCODE_PACK "111"
#define YTO_MTY_SIGNATURE "0900" //签名mty
#define YTO_MTY_SORT "0140" //三段码mty
#define YTO_MTY_ARRIVED "0140" //到件mty
#define YTO_MTY_PACK "0606" //集包mty

enum CameraType{DaHua, HaiKang};
enum CompanyIndex{Sto, Zto, Yto, Best, Tt, Yunda, Youzheng, Exception, UnKnown};
enum StoInterfaceIndex{Ip, UserName, Password, Database, TableName};
enum TabTitleIndex{SortArrivedSendBagTabPage, ConfigureTabPage, QueryStatisticsTabPage, TestTabPage, AlarmTabPage};
//分拣设备状态标签索引
enum SortDeviceStatusLabelIndex{YTOSocketLabel, NativeDatabaseLabel, CameraLabel, PLCLabel};

//读plc报警
const int railWayNum = 2;
const int boxIdNum = 25;
const int plcExpressIdLen = 31;
typedef bool(*pbAlarmArray)[boxIdNum];
typedef char(*pExpressIDArray)[boxIdNum][plcExpressIdLen];
#endif // DEFINE_H
