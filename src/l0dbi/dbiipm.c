/*
 * dbiipm.c
 *
 *  Created on: 2016年7月20日
 *      Author: hitpony
 */

#include "../l0dbi/dbiipm.h"
#include "../l0service/trace.h"
#include "../l0dbi/dbicom.h"

/*

--
-- 表的结构 `hcuipmcj188datainfo`
--

CREATE TABLE IF NOT EXISTS `hcuipmcj188datainfo` (
  `sid` int(4) NOT NULL AUTO_INCREMENT,
  `cj188address` char(14) NOT NULL,
  `timestamp` int(4) NOT NULL,
  `equtype` int(1) NOT NULL,
  `todayaccuvolume` float(8,2) NOT NULL,
  `todayaccuvolumeuint` int(1) NOT NULL,
  `currentaccuvolume` float(8,2) NOT NULL,
  `currentaccuvolumeuint` int(1) NOT NULL,
  `flowvolume` float(8,2) NOT NULL,
  `flowvolumeuint` int(1) NOT NULL,
  `lastmonth` int(1) NOT NULL,
  `accumuworktime` int(3) NOT NULL,
  `supplywatertemp` float(6,2) NOT NULL,
  `backwatertemp` float(6,2) NOT NULL,
  `realtime` char(14) NOT NULL,
  `st` char(4) NOT NULL,
  `billdate` int(1) NOT NULL,
  `readdate` int(1) NOT NULL,
  `key` int(8) NOT NULL,
  `price1` float(6,2) NOT NULL,
  `volume1` int(3) NOT NULL,
  `price2` float(6,2) NOT NULL,
  `volume2` int(3) NOT NULL,
  `price3` float(6,2) NOT NULL,
  `buycode` int(1) NOT NULL,
  `thisamount` float(8,2) NOT NULL,
  `accuamount` float(8,2) NOT NULL,
  `remainamount` float(8,2) NOT NULL,
  `keyver` int(1) NOT NULL,
  PRIMARY KEY (`sid`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 AUTO_INCREMENT=1 ;


*/

//存储数据，每一次存储，都是新增一条记录
//由于是本地数据库，这里不考虑网格问题，直接存储，简单处理
OPSTAT dbi_HcuIpmCj188DataInfo_save(sensor_ipm_cj188_data_element_t *ipmData)
{
	MYSQL *sqlHandler;
    int result = 0;
    char strsql[DBI_MAX_SQL_INQUERY_STRING_LENGTH];

    //入参检查：不涉及到生死问题，参数也没啥大问题，故而不需要检查，都可以存入数据库表单中
    if (ipmData == NULL){
    	HcuErrorPrint("DBIIPM: Input parameter NULL pointer!\n");
        return FAILURE;
    }

	//建立连接
    sqlHandler = mysql_init(NULL);
    if(!sqlHandler)
    {
    	HcuErrorPrint("DBIIPM: MySQL init failed!\n");
        return FAILURE;
    }
    sqlHandler = mysql_real_connect(sqlHandler, zHcuSysEngPar.dbi.hcuDbHost, zHcuSysEngPar.dbi.hcuDbUser, zHcuSysEngPar.dbi.hcuDbPsw, zHcuSysEngPar.dbi.hcuDbName, zHcuSysEngPar.dbi.hcuDbPort, NULL, 0);  //unix_socket and clientflag not used.
    if (!sqlHandler){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIIPM: MySQL connection failed!\n");
        return FAILURE;
    }

	//存入新的数据
    sprintf(strsql, "INSERT INTO `hcuipmcj188datainfo` (cj188address, timestamp, equtype, todayaccuvolume, todayaccuvolumeuint, currentaccuvolume, currentaccuvolumeuint,\
    		flowvolume, flowvolumeuint, lastmonth, \
    		accumuworktime, supplywatertemp, backwatertemp, realtime, st, billdate, readdate, key, price1, volume1,\
    		price2, volume2, price3, buycode, thisamount, accuamount, remainamount, keyver) VALUES \
    		('%s', '%d', '%d', '%f', '%d', '%f', '%d', '%f', '%d', '%d', '%d', '%f', '%f', '%s', '%s', '%d', '%d', '%ld', '%f', '%d', '%f', '%d', '%f', '%d', '%f', '%f', '%f', '%d')",
			ipmData->cj188address, ipmData->timestamp, ipmData->equtype, ipmData->todayaccuvolume, ipmData->todayaccuvolumeuint, ipmData->ipm.currentaccuvolume,
			ipmData->ipm.currentaccuvolumeuint, ipmData->ipm.flowvolume, ipmData->ipm.flowvolumeunit, ipmData->ipm.lastmonth, ipmData->ipm.accumuworktime,
			ipmData->ipm.supplywatertemp, ipmData->ipm.backwatertemp, ipmData->ipm.realtime, ipmData->ipm.st, ipmData->ipm.billdate,
			ipmData->ipm.readdate, ipmData->ipm.key, ipmData->ipm.price1, ipmData->ipm.volume1, ipmData->ipm.price2, ipmData->ipm.volume2, ipmData->ipm.price3, ipmData->ipm.buycode,
			ipmData->ipm.thisamount, ipmData->ipm.accuamount, ipmData->ipm.remainamount, ipmData->ipm.keyver);
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIIPM: INSERT data error: %s\n", mysql_error(sqlHandler));
        return FAILURE;
	}

	//释放记录集
    mysql_close(sqlHandler);
	if ((zHcuSysEngPar.debugMode & HCU_TRACE_DEBUG_NOR_ON) != FALSE){
		HcuDebugPrint("DBIIPM: CO1 data record save to DB!\n");
	}
    return SUCCESS;
}

//查询满足条件的第一条记录
//addr是入参，指针为出参
OPSTAT dbi_HcuIpmCj188DataInfo_inqury_1st_record(char *addr, sensor_ipm_cj188_data_element_t *ipmData)
{
	MYSQL *sqlHandler;
	MYSQL_RES *resPtr;
	MYSQL_ROW sqlRow;
    int result = 0;
    char strsql[DBI_MAX_SQL_INQUERY_STRING_LENGTH];

    //入参检查：不涉及到生死问题，参数也没啥大问题，故而不需要检查，都可以存入数据库表单中
    if (ipmData == NULL){
    	HcuErrorPrint("DBIIPM: Input parameter NULL pointer!\n");
        return FAILURE;
    }

	//建立数据库连接
    sqlHandler = mysql_init(NULL);
    if(!sqlHandler)
    {
    	HcuErrorPrint("DBIIPM: MySQL init failed!\n");
        return FAILURE;
    }
    sqlHandler = mysql_real_connect(sqlHandler, zHcuSysEngPar.dbi.hcuDbHost, zHcuSysEngPar.dbi.hcuDbUser, zHcuSysEngPar.dbi.hcuDbPsw, zHcuSysEngPar.dbi.hcuDbName, zHcuSysEngPar.dbi.hcuDbPort, NULL, 0);  //unix_socket and clientflag not used.
    if (!sqlHandler){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIIPM: MySQL connection failed!\n");
        return FAILURE;
    }

	//获取数据
    sprintf(strsql, "SELECT * FROM `hcuipmcj188datainfo` WHERE (`addr` = '%s')", addr);
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIIPM: SELECT data error: %s\n", mysql_error(sqlHandler));
        return FAILURE;
	}

	//查具体的结果
	resPtr = mysql_use_result(sqlHandler);
	if (!resPtr){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIIPM: mysql_use_result error!\n");
        return FAILURE;
	}

	//只读取第一条记录
	if ((sqlRow = mysql_fetch_row(resPtr)) == NULL)
	{
		mysql_free_result(resPtr);
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIIPM: mysql_fetch_row NULL error!\n");
        return FAILURE;
	}
	else{
		UINT32 index = 1;
		if (sqlRow[index]) strncpy(ipmData->cj188address, sqlRow[index++], 14);
		if (sqlRow[index]) ipmData->timestamp = (UINT32)atol(sqlRow[index++]);
		if (sqlRow[index]) ipmData->equtype = (UINT8)(atol(sqlRow[index++]) & 0xFF);
		if (sqlRow[index]) ipmData->todayaccuvolume = (float)atof(sqlRow[index++]);
		if (sqlRow[index]) ipmData->todayaccuvolumeuint = (UINT8)(atol(sqlRow[index++]) & 0xFF);
		if (sqlRow[index]) ipmData->ipm.currentaccuvolume = (float)atof(sqlRow[index++]);
		if (sqlRow[index]) ipmData->ipm.currentaccuvolumeuint = (UINT8)(atol(sqlRow[index++]) & 0xFF);
		if (sqlRow[index]) ipmData->ipm.flowvolume = (float)atof(sqlRow[index++]);
		if (sqlRow[index]) ipmData->ipm.flowvolumeunit = (UINT8)(atol(sqlRow[index++]) & 0xFF);
		if (sqlRow[index]) ipmData->ipm.lastmonth = (UINT8)(atol(sqlRow[index++]) & 0xFF);
		if (sqlRow[index]) ipmData->ipm.accumuworktime = (INT32)atol(sqlRow[index++]);
		if (sqlRow[index]) ipmData->ipm.supplywatertemp = (float)atof(sqlRow[index++]);
		if (sqlRow[index]) ipmData->ipm.backwatertemp = (float)atof(sqlRow[index++]);
		if (sqlRow[index]) strncpy(ipmData->ipm.realtime, sqlRow[index++], 14);
		if (sqlRow[index]) strncpy(ipmData->ipm.st, sqlRow[index++], 4);
		if (sqlRow[index]) ipmData->ipm.billdate = (INT8)(atol(sqlRow[index++]) & 0xFF);
		if (sqlRow[index]) ipmData->ipm.readdate = (INT8)(atol(sqlRow[index++]) & 0xFF);
		if (sqlRow[index]) ipmData->ipm.key = (INT64)atol(sqlRow[index++]);
		if (sqlRow[index]) ipmData->ipm.price1 = (float)atof(sqlRow[index++]);
		if (sqlRow[index]) ipmData->ipm.volume1 = (INT32)atol(sqlRow[index++]);
		if (sqlRow[index]) ipmData->ipm.price2 = (float)atof(sqlRow[index++]);
		if (sqlRow[index]) ipmData->ipm.volume2 = (INT32)atol(sqlRow[index++]);
		if (sqlRow[index]) ipmData->ipm.price3 = (float)atof(sqlRow[index++]);
		if (sqlRow[index]) ipmData->ipm.buycode = (UINT8)(atol(sqlRow[index++]) & 0xFF);
		if (sqlRow[index]) ipmData->ipm.thisamount = (float)atof(sqlRow[index++]);
		if (sqlRow[index]) ipmData->ipm.accuamount = (float)atof(sqlRow[index++]);
		if (sqlRow[index]) ipmData->ipm.remainamount = (float)atof(sqlRow[index++]);
		if (sqlRow[index]) ipmData->ipm.keyver = (UINT8)(atol(sqlRow[index++]) & 0xFF);
	}

	//释放记录集
	mysql_free_result(resPtr);
    mysql_close(sqlHandler);
    return SUCCESS;
}

//删除对应用户所有超过90天的数据
//缺省做成90天，如果参数错误，导致90天以内的数据强行删除，则不被认可
OPSTAT dbi_HcuIpmCj188DataInfo_delete_3monold(UINT32 days)
{
	MYSQL *sqlHandler;
    int result = 0;
    char strsql[DBI_MAX_SQL_INQUERY_STRING_LENGTH];
    UINT32 cursec = 0;

    //入参检查：不涉及到生死问题，参数也没啥大问题，故而不需要检查，都可以存入数据库表单中
    if (days <IPM_DATA_SAVE_DAYS_MIN) days = IPM_DATA_SAVE_DAYS_MIN;

	//建立连接
    sqlHandler = mysql_init(NULL);
    if(!sqlHandler)
    {
    	HcuErrorPrint("DBIIPM: MySQL init failed!\n");
        return FAILURE;
    }
    sqlHandler = mysql_real_connect(sqlHandler, zHcuSysEngPar.dbi.hcuDbHost, zHcuSysEngPar.dbi.hcuDbUser, zHcuSysEngPar.dbi.hcuDbPsw, zHcuSysEngPar.dbi.hcuDbName, zHcuSysEngPar.dbi.hcuDbPort, NULL, 0);  //unix_socket and clientflag not used.
    if (!sqlHandler){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIIPM: MySQL connection failed!\n");
        return FAILURE;
    }

	//删除满足条件的数据
    cursec = time(NULL);
    days = days * 24 * 3600;
    sprintf(strsql, "DELETE FROM `hcuipmcj188datainfo` WHERE (%d - `timestamp` > '%d')", cursec, days);
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIIPM: DELETE data error: %s\n", mysql_error(sqlHandler));
        return FAILURE;
	}

	//释放记录集
    mysql_close(sqlHandler);
    return SUCCESS;
}

OPSTAT dbi_HcuIpmQg376DataInfo_save(sensor_ipm_qg376_data_element_t *ipmData)
{
    return SUCCESS;
}

OPSTAT dbi_HcuIpmQg376DataInfo_inqury_1st_record(char *addr, sensor_ipm_qg376_data_element_t *ipmData)
{
    return SUCCESS;
}

OPSTAT dbi_HcuIpmQg376DataInfo_delete_3monold(UINT32 days)
{
    return SUCCESS;
}
