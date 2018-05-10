/*
 * dbiigm.c
 *
 *  Created on: 2016年7月20日
 *      Author: hitpony
 */

#include "../l0dbi/dbiigm.h"
#include "../l0service/trace.h"
#include "../l0dbi/dbicom.h"

/*

--
-- 表的结构 `hcuigmcj188datainfo`
--

CREATE TABLE IF NOT EXISTS `hcuigmcj188datainfo` (
  `sid` int(4) NOT NULL AUTO_INCREMENT,
  `cj188address` char(14) NOT NULL,
  `timestamp` int(4) NOT NULL,
  `equtype` int(1) NOT NULL,
  `billtodayaccuvolume` float(8,2) NOT NULL,
  `billtodayaccuvolumeunit` int(1) NOT NULL,
  `currentaccuvolume` float(8,2) NOT NULL,
  `currentaccuvolumeunit` int(1) NOT NULL,
  `flowvolume` float(8,2) NOT NULL,
  `flowvolumeunit` int(1) NOT NULL,
  `lastmonth` int(1) NOT NULL,
  `accumuworktime` int(3) NOT NULL,
  `supplywatertemp` float(6,2) NOT NULL,
  `backwatertemp` float(6,2) NOT NULL,
  `realtime` char(14) NOT NULL,
  `st` char(4) NOT NULL,
  `billtodaydate` int(1) NOT NULL,
  `readamountcurdate` int(1) NOT NULL,
  `startdate` int(1) NOT NULL,
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
  `switchctrl` int(1) NOT NULL,
  PRIMARY KEY (`sid`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 AUTO_INCREMENT=1 ;


*/

//存储数据，每一次存储，都是新增一条记录
//由于是本地数据库，这里不考虑网格问题，直接存储，简单处理
OPSTAT dbi_HcuIgmCj188DataInfo_save(sensor_igm_cj188_data_element_t *igmData)
{
	MYSQL *sqlHandler;
    int result = 0;
    char strsql[DBI_MAX_SQL_INQUERY_STRING_LENGTH];

    //入参检查：不涉及到生死问题，参数也没啥大问题，故而不需要检查，都可以存入数据库表单中
    if (igmData == NULL){
    	HcuErrorPrint("DBIIGM: Input parameter NULL pointer!\n");
        return FAILURE;
    }

	//建立连接
//    sqlHandler = mysql_init(NULL);
//    if(!sqlHandler)
//    {
//    	HcuErrorPrint("DBIIGM: MySQL init failed!\n");
//        return FAILURE;
//    }
//    sqlHandler = mysql_real_connect(sqlHandler, zHcuSysEngPar.dbi.hcuDbHost, zHcuSysEngPar.dbi.hcuDbUser, zHcuSysEngPar.dbi.hcuDbPsw, zHcuSysEngPar.dbi.hcuDbName, zHcuSysEngPar.dbi.hcuDbPort, NULL, 0);  //unix_socket and clientflag not used.
//    if (!sqlHandler){
//    	mysql_close(sqlHandler);
//    	HcuErrorPrint("DBIIGM: MySQL connection failed!\n");
//        return FAILURE;
//    }
    HCU_L0DBICOM_INIT_DB_CONN();

	//存入新的数据
    sprintf(strsql, "INSERT INTO `hcuigmcj188datainfo` (cj188address, timestamp, equtype, billtodayaccuvolume, billtodayaccuvolumeunit, currentaccuvolume, currentaccuvolumeunit,\
    		flowvolume, flowvolumeunit, lastmonth, \
    		accumuworktime, supplywatertemp, backwatertemp, realtime, st, billtodaydate, readamountcurdate, startdate, key, price1, volume1,\
    		price2, volume2, price3, buycode, thisamount, accuamount, remainamount, keyver, switchctrl) VALUES \
    		('%s', '%d', '%d', '%f', '%d', '%f', '%d', '%f', '%d', '%d', '%d', '%f', '%f', '%s', '%s', '%d', '%d', '%d', '%ld', '%f', '%d', '%f', '%d', '%f', '%d', '%f', '%f', '%f', '%d', '%d')",
			igmData->cj188address, igmData->timestamp, igmData->equtype, igmData->billtodayaccuvolume, igmData->billtodayaccuvolumeunit, igmData->igm.currentaccuvolume,
			igmData->igm.currentaccuvolumeunit, igmData->igm.flowvolume, igmData->igm.flowvolumeunit, igmData->igm.lastmonth, igmData->igm.accumuworktime,
			igmData->igm.supplywatertemp, igmData->igm.backwatertemp, igmData->igm.realtime, igmData->igm.st, igmData->igm.billtodaydate,
			igmData->igm.readamountcurdate, igmData->igm.startdate, igmData->igm.key, igmData->igm.price1, igmData->igm.volume1, igmData->igm.price2, igmData->igm.volume2, igmData->igm.price3, igmData->igm.buycode,
			igmData->igm.thisamount, igmData->igm.accuamount, igmData->igm.remainamount, igmData->igm.keyver, igmData->igm.switchctrl);
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIIGM: INSERT data error: %s\n", mysql_error(sqlHandler));
        return FAILURE;
	}

	//释放记录集
    mysql_close(sqlHandler);
	if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_NOR_ON) != FALSE){
		HcuDebugPrint("DBIIGM: CO1 data record save to DB!\n");
	}
    return SUCCESS;
}

//查询满足条件的第一条记录
//addr是入参，指针为出参
OPSTAT dbi_HcuIgmCj188DataInfo_inqury_1st_record(char *addr, sensor_igm_cj188_data_element_t *igmData)
{
	MYSQL *sqlHandler;
	MYSQL_RES *resPtr;
	MYSQL_ROW sqlRow;
    int result = 0;
    char strsql[DBI_MAX_SQL_INQUERY_STRING_LENGTH];

    //入参检查：不涉及到生死问题，参数也没啥大问题，故而不需要检查，都可以存入数据库表单中
    if (igmData == NULL){
    	HcuErrorPrint("DBIIGM: Input parameter NULL pointer!\n");
        return FAILURE;
    }

	//建立数据库连接
//    sqlHandler = mysql_init(NULL);
//    if(!sqlHandler)
//    {
//    	HcuErrorPrint("DBIIGM: MySQL init failed!\n");
//        return FAILURE;
//    }
//    sqlHandler = mysql_real_connect(sqlHandler, zHcuSysEngPar.dbi.hcuDbHost, zHcuSysEngPar.dbi.hcuDbUser, zHcuSysEngPar.dbi.hcuDbPsw, zHcuSysEngPar.dbi.hcuDbName, zHcuSysEngPar.dbi.hcuDbPort, NULL, 0);  //unix_socket and clientflag not used.
//    if (!sqlHandler){
//    	mysql_close(sqlHandler);
//    	HcuErrorPrint("DBIIGM: MySQL connection failed!\n");
//        return FAILURE;
//    }
    HCU_L0DBICOM_INIT_DB_CONN();

	//获取数据
    sprintf(strsql, "SELECT * FROM `hcuigmcj188datainfo` WHERE (`addr` = '%s')", addr);
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIIGM: SELECT data error: %s\n", mysql_error(sqlHandler));
        return FAILURE;
	}

	//查具体的结果
	resPtr = mysql_use_result(sqlHandler);
	if (!resPtr){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIIGM: mysql_use_result error!\n");
        return FAILURE;
	}

	//只读取第一条记录
	if ((sqlRow = mysql_fetch_row(resPtr)) == NULL)
	{
		mysql_free_result(resPtr);
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIIGM: mysql_fetch_row NULL error!\n");
        return FAILURE;
	}
	else{
		UINT32 index = 1;
		if (sqlRow[index]) strncpy(igmData->cj188address, sqlRow[index++], 14);
		if (sqlRow[index]) igmData->timestamp = (UINT32)atol(sqlRow[index++]);
		if (sqlRow[index]) igmData->equtype = (UINT8)(atol(sqlRow[index++]) & 0xFF);
		if (sqlRow[index]) igmData->billtodayaccuvolume = (float)atof(sqlRow[index++]);
		if (sqlRow[index]) igmData->billtodayaccuvolumeunit = (UINT8)(atol(sqlRow[index++]) & 0xFF);
		if (sqlRow[index]) igmData->igm.currentaccuvolume = (float)atof(sqlRow[index++]);
		if (sqlRow[index]) igmData->igm.currentaccuvolumeunit = (UINT8)(atol(sqlRow[index++]) & 0xFF);
		if (sqlRow[index]) igmData->igm.flowvolume = (float)atof(sqlRow[index++]);
		if (sqlRow[index]) igmData->igm.flowvolumeunit = (UINT8)(atol(sqlRow[index++]) & 0xFF);
		if (sqlRow[index]) igmData->igm.lastmonth = (UINT8)(atol(sqlRow[index++]) & 0xFF);
		if (sqlRow[index]) igmData->igm.accumuworktime = (INT32)atol(sqlRow[index++]);
		if (sqlRow[index]) igmData->igm.supplywatertemp = (float)atof(sqlRow[index++]);
		if (sqlRow[index]) igmData->igm.backwatertemp = (float)atof(sqlRow[index++]);
		if (sqlRow[index]) strncpy(igmData->igm.realtime, sqlRow[index++], 14);
		if (sqlRow[index]) strncpy(igmData->igm.st, sqlRow[index++], 4);
		if (sqlRow[index]) igmData->igm.billtodaydate = (INT8)(atol(sqlRow[index++]) & 0xFF);
		if (sqlRow[index]) igmData->igm.readamountcurdate = (INT8)(atol(sqlRow[index++]) & 0xFF);
		if (sqlRow[index]) igmData->igm.startdate = (INT8)(atol(sqlRow[index++]) & 0xFF);
		if (sqlRow[index]) igmData->igm.key = (INT64)atol(sqlRow[index++]);
		if (sqlRow[index]) igmData->igm.price1 = (float)atof(sqlRow[index++]);
		if (sqlRow[index]) igmData->igm.volume1 = (INT32)atol(sqlRow[index++]);
		if (sqlRow[index]) igmData->igm.price2 = (float)atof(sqlRow[index++]);
		if (sqlRow[index]) igmData->igm.volume2 = (INT32)atol(sqlRow[index++]);
		if (sqlRow[index]) igmData->igm.price3 = (float)atof(sqlRow[index++]);
		if (sqlRow[index]) igmData->igm.buycode = (UINT8)(atol(sqlRow[index++]) & 0xFF);
		if (sqlRow[index]) igmData->igm.thisamount = (float)atof(sqlRow[index++]);
		if (sqlRow[index]) igmData->igm.accuamount = (float)atof(sqlRow[index++]);
		if (sqlRow[index]) igmData->igm.remainamount = (float)atof(sqlRow[index++]);
		if (sqlRow[index]) igmData->igm.keyver = (UINT8)(atol(sqlRow[index++]) & 0xFF);
		if (sqlRow[index]) igmData->igm.switchctrl = (UINT8)(atol(sqlRow[index++]) & 0xFF);
	}

	//释放记录集
	mysql_free_result(resPtr);
    mysql_close(sqlHandler);
    return SUCCESS;
}

//删除对应用户所有超过90天的数据
//缺省做成90天，如果参数错误，导致90天以内的数据强行删除，则不被认可
OPSTAT dbi_HcuIgmCj188DataInfo_delete_3monold(UINT32 days)
{
	MYSQL *sqlHandler;
    int result = 0;
    char strsql[DBI_MAX_SQL_INQUERY_STRING_LENGTH];
    UINT32 cursec = 0;

    //入参检查：不涉及到生死问题，参数也没啥大问题，故而不需要检查，都可以存入数据库表单中
    if (days <IGM_DATA_SAVE_DAYS_MIN) days = IGM_DATA_SAVE_DAYS_MIN;

	//建立连接
//    sqlHandler = mysql_init(NULL);
//    if(!sqlHandler)
//    {
//    	HcuErrorPrint("DBIIGM: MySQL init failed!\n");
//        return FAILURE;
//    }
//    sqlHandler = mysql_real_connect(sqlHandler, zHcuSysEngPar.dbi.hcuDbHost, zHcuSysEngPar.dbi.hcuDbUser, zHcuSysEngPar.dbi.hcuDbPsw, zHcuSysEngPar.dbi.hcuDbName, zHcuSysEngPar.dbi.hcuDbPort, NULL, 0);  //unix_socket and clientflag not used.
//    if (!sqlHandler){
//    	mysql_close(sqlHandler);
//    	HcuErrorPrint("DBIIGM: MySQL connection failed!\n");
//        return FAILURE;
//    }
    HCU_L0DBICOM_INIT_DB_CONN();

	//删除满足条件的数据
    cursec = time(NULL);
    days = days * 24 * 3600;
    sprintf(strsql, "DELETE FROM `hcuigmcj188datainfo` WHERE (%d - `timestamp` > '%d')", cursec, days);
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIIGM: DELETE data error: %s\n", mysql_error(sqlHandler));
        return FAILURE;
	}

	//释放记录集
    mysql_close(sqlHandler);
    return SUCCESS;
}






