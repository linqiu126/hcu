/*
 * dbisysperfm.c
 *
 *  Created on: 2016年1月19日
 *      Author: hitpony
 */

#include "../l0dbi/dbisysperfm.h"
#include "../l0service/trace.h"
#include "../l0dbi/dbicom.h"

//数据表格式
// HcuSyspmGlobaldDatainfo
/*
CREATE TABLE IF NOT EXISTS `hcusyspmglobaldatainfo` (
  `sid` int(4) NOT NULL AUTO_INCREMENT,
  `taskhcuvmec` int(4) NOT NULL,
  `taskhwinvec` int(4) NOT NULL,
  `tasksyspmec` int(4) NOT NULL,
  `taskmodbusec` int(4) NOT NULL,
  `taskcloudvelaec` int(4) NOT NULL,
  `taskavorionec` int(4) NOT NULL,
  `taskspsvirgoec` int(4) NOT NULL,
  `taskhsmmpec` int(4) NOT NULL,
  `taskemcec` int(4) NOT NULL,
  `taskpm25ec` int(4) NOT NULL,
  `taskwinddirec` int(4) NOT NULL,
  `taskwindspdec` int(4) NOT NULL,
  `tasktempec` int(4) NOT NULL,
  `taskhumidec` int(4) NOT NULL,
  `tasknoiseec` int(4) NOT NULL,
  `taskairprsec` int(4) NOT NULL,
  `taskco1ec` int(4) NOT NULL,
  `tasklightstrec` int(4) NOT NULL,
  `taskalcoholec` int(4) NOT NULL,
  `taskhchoec` int(4) NOT NULL,
  `tasktoxicgasec` int(4) NOT NULL,
  `restartcnt` int(4) NOT NULL,
  `cloudvelaconncnt` int(4) NOT NULL,
  `cloudveladisccnt` int(4) NOT NULL,
  `clouddatatimeoutcnt` int(4) NOT NULL,
  `timestamp` int(4) NOT NULL,
  PRIMARY KEY (`sid`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8;

INSERT INTO `hcusyspmglobaldatainfo` (`sid`, `taskhcuvmec`, `taskhwinvec`, `tasksyspmec`, `taskmodbusec`, `taskcloudvelaec`, `taskavorionec`,
`taskspsvirgoec`, `taskhsmmpec`, `taskemcec`, `taskpm25ec`, `taskwinddirec`, `taskwindspdec`, `tasktempec`, `taskhumidec`, `tasknoiseec`,
`taskairprsec`, `taskco1ec`, `tasklightstrec`, `taskalcoholec`, `taskhchoec`, `tasktoxicgasec`, `restartcnt`, `cloudvelaconncnt`, `cloudvelaconnfailcnt`, `cloudveladisccnt`, `clouddatatimeoutcnt`, `socketdisccnt`,
`cpuoccupy`, `memoccupy`, diskoccupy,`timestamp`) VALUES
(1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 111);
*/

//存储SysPerformance Global Counter数据，每一次存储，都是新增一条记录
//由于是本地数据库，这里不考虑网格问题，直接存储，简单处理
OPSTAT dbi_HcuSyspmGlobalDataInfo_save(void)
{
	MYSQL *sqlHandler;
    int result = 0;
    char strsql[DBI_MAX_SQL_INQUERY_STRING_LENGTH];

    //入参检查：不涉及到生死问题，参数也没啥大问题，故而不需要检查，都可以存入数据库表单中

	//建立连接
    sqlHandler = mysql_init(NULL);
    if(!sqlHandler)
    {
    	HcuErrorPrint("DBISP: MySQL init failed!\n");
        return FAILURE;
    }
    sqlHandler = mysql_real_connect(sqlHandler, zHcuSysEngPar.dbi.hcuDbHost, zHcuSysEngPar.dbi.hcuDbUser, zHcuSysEngPar.dbi.hcuDbPsw, zHcuSysEngPar.dbi.hcuDbName, zHcuSysEngPar.dbi.hcuDbPort, NULL, 0);  //unix_socket and clientflag not used.
    if (!sqlHandler){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBISP: MySQL connection failed!\n");
        return FAILURE;
    }

	//存入新的数据
    sprintf(strsql, "INSERT INTO `hcusyspmglobaldatainfo` (taskhcuvmec, taskhwinvec, tasksyspmec, taskmodbusec,\
    		taskcloudvelaec, taskavorionec, taskspsvirgoec, taskhsmmpec, \
			taskemcec, taskpm25ec, taskwinddirec, taskwindspdec, tasktempec, taskhumidec, tasknoiseec,\
    		taskairprsec, taskco1ec, tasklightstrec, taskalcoholec, taskhchoec, tasktoxicgasec,\
    		restartcnt, cloudvelaconncnt, cloudvelaconnfailcnt, cloudveladisccnt, clouddatatimeoutcnt, socketdisccnt, cpuoccupy, memoccupy, diskoccupy, timestamp) VALUES \
    		('%d', '%d','%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d')",\
    		zHcuGlobalCounter.errCnt[TASK_ID_HCUVM], zHcuGlobalCounter.errCnt[TASK_ID_HWINV], zHcuGlobalCounter.errCnt[TASK_ID_SYSPM],zHcuGlobalCounter.errCnt[TASK_ID_MODBUS],\
    		zHcuGlobalCounter.errCnt[TASK_ID_CLOUDVELA], zHcuGlobalCounter.errCnt[TASK_ID_AVORION], zHcuGlobalCounter.errCnt[TASK_ID_SPSVIRGO], zHcuGlobalCounter.errCnt[TASK_ID_HSMMP],\
			zHcuGlobalCounter.errCnt[TASK_ID_EMC], zHcuGlobalCounter.errCnt[TASK_ID_PM25], zHcuGlobalCounter.errCnt[TASK_ID_WINDDIR], zHcuGlobalCounter.errCnt[TASK_ID_WINDSPD], \
			zHcuGlobalCounter.errCnt[TASK_ID_TEMP], zHcuGlobalCounter.errCnt[TASK_ID_HUMID], zHcuGlobalCounter.errCnt[TASK_ID_NOISE],\
    		zHcuGlobalCounter.errCnt[TASK_ID_AIRPRS], zHcuGlobalCounter.errCnt[TASK_ID_CO1], zHcuGlobalCounter.errCnt[TASK_ID_LIGHTSTR], zHcuGlobalCounter.errCnt[TASK_ID_ALCOHOL], zHcuGlobalCounter.errCnt[TASK_ID_HCHO], zHcuGlobalCounter.errCnt[TASK_ID_TOXICGAS],\
    		zHcuGlobalCounter.restartCnt, zHcuGlobalCounter.cloudVelaConnCnt, zHcuGlobalCounter.cloudVelaConnFailCnt, zHcuGlobalCounter.cloudVelaDiscCnt, zHcuGlobalCounter.CloudDataTimeOutCnt, zHcuGlobalCounter.SocketDiscCnt, \
			zHcuGlobalCounter.cpu_occupy, zHcuGlobalCounter.mem_occupy, zHcuGlobalCounter.disk_occupy, (UINT32)time(NULL));
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBISP: INSERT data error: %s\n", mysql_error(sqlHandler));
        return FAILURE;
	}

	//释放记录集
    mysql_close(sqlHandler);
	if ((zHcuSysEngPar.debugMode & HCU_TRACE_DEBUG_NOR_ON) != FALSE){
		HcuDebugPrint("DBISP: System performance statistic data record save to DB!\n");
	}
    return SUCCESS;
}

//删除对应用户所有超过90天的数据
//缺省做成90天，如果参数错误，导致90天以内的数据强行删除，则不被认可
OPSTAT dbi_HcuSyspmGlobalDataInfo_delete_3monold(UINT32 days)
{
	MYSQL *sqlHandler;
    int result = 0;
    char strsql[DBI_MAX_SQL_INQUERY_STRING_LENGTH];
    UINT32 cursec = 0;

    //入参检查：不涉及到生死问题，参数也没啥大问题，故而不需要检查，都可以存入数据库表单中
    if (days < SP_GLOBAL_DATA_SAVE_DAYS_MIN) days = SP_GLOBAL_DATA_SAVE_DAYS_MIN;

	//建立连接
    sqlHandler = mysql_init(NULL);
    if(!sqlHandler)
    {
    	HcuErrorPrint("DBISP: MySQL init failed!\n");
        return FAILURE;
    }
    sqlHandler = mysql_real_connect(sqlHandler, zHcuSysEngPar.dbi.hcuDbHost, zHcuSysEngPar.dbi.hcuDbUser, zHcuSysEngPar.dbi.hcuDbPsw, zHcuSysEngPar.dbi.hcuDbName, zHcuSysEngPar.dbi.hcuDbPort, NULL, 0);  //unix_socket and clientflag not used.
    if (!sqlHandler){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBISP: MySQL connection failed!\n");
        return FAILURE;
    }

	//删除满足条件的数据
    cursec = time(NULL);
    days = days * 24 * 3600;
    sprintf(strsql, "DELETE FROM `hcusyspmglobaldatainfo` WHERE (%d - `timestamp` > '%d')", cursec, days);
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBISP: INSERT data error: %s\n", mysql_error(sqlHandler));
        return FAILURE;
	}

	//释放记录集
    mysql_close(sqlHandler);
    return SUCCESS;
}

/*
//存储SysPerformance ErrCnt数据，每一次存储，都是新增一条记录
//由于是本地数据库，这里不考虑网格问题，直接存储，简单处理
OPSTAT dbi_HcuSpErrcntDataInfo_save(void)
{
	MYSQL *sqlHandler;
    int result = 0;
    char strsql[DBI_MAX_SQL_INQUERY_STRING_LENGTH];

    //入参检查：不涉及到生死问题，参数也没啥大问题，故而不需要检查，都可以存入数据库表单中

	//建立连接
    sqlHandler = mysql_init(NULL);
    if(!sqlHandler)
    {
    	HcuErrorPrint("DBISP: MySQL init failed!\n");
        return FAILURE;
    }
    sqlHandler = mysql_real_connect(sqlHandler, zHcuSysEngPar.dbi.hcuDbHost, zHcuSysEngPar.dbi.hcuDbUser, zHcuSysEngPar.dbi.hcuDbPsw, zHcuSysEngPar.dbi.hcuDbName, zHcuSysEngPar.dbi.hcuDbPort, NULL, 0);  //unix_socket and clientflag not used.
    if (!sqlHandler){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBISP: MySQL connection failed!\n");
        return FAILURE;
    }

	//存入新的数据
    sprintf(strsql, "INSERT INTO `hcusperrcntdatainfo` (taskhcuvmec, taskhwinvec, taskmodbusec, taskcloudvelaec, taskavorionec, taskspsvirgoec, taskhsmmpec, \
			taskemcec, taskpm25ec, taskwinddirec, taskwindspdec, tasktempec, taskhumidec, tasknoiseec, timestamp) VALUES \
    		('%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d')", zHcuRunErrCnt[TASK_ID_HCUVM], zHcuRunErrCnt[TASK_ID_HWINV], \
			zHcuRunErrCnt[TASK_ID_MODBUS], zHcuRunErrCnt[TASK_ID_CLOUDVELA], zHcuRunErrCnt[TASK_ID_AVORION], zHcuRunErrCnt[TASK_ID_SPSVIRGO], \
			zHcuRunErrCnt[TASK_ID_HSMMP], zHcuRunErrCnt[TASK_ID_EMC], zHcuRunErrCnt[TASK_ID_PM25], zHcuRunErrCnt[TASK_ID_WINDDIR], zHcuRunErrCnt[TASK_ID_WINDSPD], \
			zHcuRunErrCnt[TASK_ID_TEMP], zHcuRunErrCnt[TASK_ID_HUMID], zHcuRunErrCnt[TASK_ID_NOISE], (UINT32)time(NULL));
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBISP: INSERT data error: %s\n", mysql_error(sqlHandler));
        return FAILURE;
	}

	//释放记录集
    mysql_close(sqlHandler);
	if ((zHcuSysEngPar.debugMode & TRACE_DEBUG_NOR_ON) != FALSE){
		HcuDebugPrint("DBISP: System Perfm ErrCnt data record save to DB!\n");
	}
    return SUCCESS;
}

//删除对应用户所有超过90天的数据
//缺省做成90天，如果参数错误，导致90天以内的数据强行删除，则不被认可
OPSTAT dbi_HcuSpErrcntDataInfo_delete_3monold(UINT32 days)
{
	MYSQL *sqlHandler;
    int result = 0;
    char strsql[DBI_MAX_SQL_INQUERY_STRING_LENGTH];
    UINT32 cursec = 0;

    //入参检查：不涉及到生死问题，参数也没啥大问题，故而不需要检查，都可以存入数据库表单中
    if (days <SP_ERRCNT_DATA_SAVE_DAYS_MIN) days = SP_ERRCNT_DATA_SAVE_DAYS_MIN;

	//建立连接
    sqlHandler = mysql_init(NULL);
    if(!sqlHandler)
    {
    	HcuErrorPrint("DBISP: MySQL init failed!\n");
        return FAILURE;
    }
    sqlHandler = mysql_real_connect(sqlHandler, zHcuSysEngPar.dbi.hcuDbHost, zHcuSysEngPar.dbi.hcuDbUser, zHcuSysEngPar.dbi.hcuDbPsw, zHcuSysEngPar.dbi.hcuDbName, zHcuSysEngPar.dbi.hcuDbPort, NULL, 0);  //unix_socket and clientflag not used.
    if (!sqlHandler){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBISP: MySQL connection failed!\n");
        return FAILURE;
    }

	//删除满足条件的数据
    cursec = time(NULL);
    days = days * 24 * 3600;
    sprintf(strsql, "DELETE FROM `hcusperrcntdatainfo` WHERE (%d - `timestamp` > '%d')", cursec, days);
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBISP: INSERT data error: %s\n", mysql_error(sqlHandler));
        return FAILURE;
	}

	//释放记录集
    mysql_close(sqlHandler);
    return SUCCESS;
}
*/
