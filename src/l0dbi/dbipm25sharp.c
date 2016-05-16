/*
 * dbipm25sharp.c
 *
 *  Created on: 12 May 2016
 *      Author: pi
 */

#include "../l0dbi/dbipm25sharp.h"
#include "../l0service/trace.h"
#include "../l0dbi/dbicom.h"

//PM25SHARP数据表格式
// HcuPm25SharpDataInfo
/*
CREATE TABLE IF NOT EXISTS `hcupm25sharpdatainfo` (
  `sid` int(4) NOT NULL AUTO_INCREMENT,
  `pm25` int(4) NOT NULL,
  PRIMARY KEY (`sid`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8;

INSERT INTO `hcudb`.`hcupm25sharpdatainfo` (
`sid` ,
`pm25` ,
)
VALUES (
'1', '1'
);

*/



//存储PM25 sharp数据，每一次存储，都是新增一条记录
//由于是本地数据库，这里不考虑网格问题，直接存储，简单处理
OPSTAT dbi_HcuPM25SharpDataInfo_save(int PM25data)
{
	MYSQL *sqlHandler;
    int result = 0;
    char strsql[DBI_MAX_SQL_INQUERY_STRING_LENGTH];

    //入参检查：不涉及到生死问题，参数也没啥大问题，故而不需要检查，都可以存入数据库表单中
    /*
     if (PM25data == NULL){
    	HcuErrorPrint("DBIPM25sharp: Input parameter NULL pointer!\n");
        return FAILURE;
    }
    */

	//建立连接
    sqlHandler = mysql_init(NULL);
    if(!sqlHandler)
    {
    	HcuErrorPrint("DBIPM25sharp: MySQL init failed!\n");
        return FAILURE;
    }
    sqlHandler = mysql_real_connect(sqlHandler, zHcuSysEngPar.dbi.hcuDbHost, zHcuSysEngPar.dbi.hcuDbUser, zHcuSysEngPar.dbi.hcuDbPsw, zHcuSysEngPar.dbi.hcuDbName, zHcuSysEngPar.dbi.hcuDbPort, NULL, 0);  //unix_socket and clientflag not used.
    if (!sqlHandler){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIPM25sharp: MySQL connection failed!\n");
        return FAILURE;
    }

	//存入新的数据
    sprintf(strsql, "INSERT INTO `hcupm25sharpdatainfo` (pm25) VALUES \
    		('%d')", PM25data);
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIPM25sharp: INSERT data error: %s\n", mysql_error(sqlHandler));
        return FAILURE;
	}

	//释放记录集
    mysql_close(sqlHandler);
	if ((zHcuSysEngPar.debugMode & TRACE_DEBUG_NOR_ON) != FALSE){
		HcuDebugPrint("DBIPM25sharp: PM25 sharp data record save to DB!\n");
	}
    return SUCCESS;
}




/*

//存储HUMID数据，每一次存储，都是新增一条记录
//由于是本地数据库，这里不考虑网格问题，直接存储，简单处理
OPSTAT dbi_HcuHumidDataInfo_save(sensor_humid_data_element_t *humidData)
{
	MYSQL *sqlHandler;
    int result = 0;
    char strsql[DBI_MAX_SQL_INQUERY_STRING_LENGTH];

    //入参检查：不涉及到生死问题，参数也没啥大问题，故而不需要检查，都可以存入数据库表单中
    if (humidData == NULL){
    	HcuErrorPrint("DBIHUMID: Input parameter NULL pointer!\n");
        return FAILURE;
    }

	//建立连接
    sqlHandler = mysql_init(NULL);
    if(!sqlHandler)
    {
    	HcuErrorPrint("DBIHUMID: MySQL init failed!\n");
        return FAILURE;
    }
    sqlHandler = mysql_real_connect(sqlHandler, zHcuSysEngPar.dbi.hcuDbHost, zHcuSysEngPar.dbi.hcuDbUser, zHcuSysEngPar.dbi.hcuDbPsw, zHcuSysEngPar.dbi.hcuDbName, zHcuSysEngPar.dbi.hcuDbPort, NULL, 0);  //unix_socket and clientflag not used.
    if (!sqlHandler){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIHUMID: MySQL connection failed!\n");
        return FAILURE;
    }

	//存入新的数据
    sprintf(strsql, "INSERT INTO `hcuhumiddatainfo` (deviceid, timestamp, dataformat, humidvalue, ew, gpsx, ns, gpsy, gpsz, onofflineflag) VALUES \
    		('%d', '%d', '%d', '%d', '%c', '%d', '%c', '%d', '%d', '%d')", humidData->equipid, humidData->timeStamp, humidData->dataFormat, humidData->humidValue, humidData->gps.ew, humidData->gps.gpsx, humidData->gps.ns, humidData->gps.gpsy, humidData->gps.gpsz, humidData->onOffLineFlag);
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIHUMID: INSERT data error: %s\n", mysql_error(sqlHandler));
        return FAILURE;
	}

	//释放记录集
    mysql_close(sqlHandler);
	if ((zHcuSysEngPar.debugMode & TRACE_DEBUG_NOR_ON) != FALSE){
		HcuDebugPrint("DBIHUMID: HUMID data record save to DB!\n");
	}
    return SUCCESS;
}

//查询满足条件的第一条记录
//deviceid是入参，指针为出参
OPSTAT dbi_HcuHumidDataInfo_inqury_1st_record(UINT32 deviceid, sensor_humid_data_element_t *humidData)
{
	MYSQL *sqlHandler;
	MYSQL_RES *resPtr;
	MYSQL_ROW sqlRow;
    int result = 0;
    char strsql[DBI_MAX_SQL_INQUERY_STRING_LENGTH];

    //入参检查：不涉及到生死问题，参数也没啥大问题，故而不需要检查，都可以存入数据库表单中
    if (humidData == NULL){
    	HcuErrorPrint("DBIHUMID: Input parameter NULL pointer!\n");
        return FAILURE;
    }

	//建立数据库连接
    sqlHandler = mysql_init(NULL);
    if(!sqlHandler)
    {
    	HcuErrorPrint("DBIHUMID: MySQL init failed!\n");
        return FAILURE;
    }
    sqlHandler = mysql_real_connect(sqlHandler, zHcuSysEngPar.dbi.hcuDbHost, zHcuSysEngPar.dbi.hcuDbUser, zHcuSysEngPar.dbi.hcuDbPsw, zHcuSysEngPar.dbi.hcuDbName, zHcuSysEngPar.dbi.hcuDbPort, NULL, 0);  //unix_socket and clientflag not used.
    if (!sqlHandler){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIHUMID: MySQL connection failed!\n");
        return FAILURE;
    }

	//获取数据
    sprintf(strsql, "SELECT * FROM `hcuhumiddatainfo` WHERE (`deviceid` = '%d')", deviceid);
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIHUMID: INSET data error: %s\n", mysql_error(sqlHandler));
        return FAILURE;
	}

	//查具体的结果
	resPtr = mysql_use_result(sqlHandler);
	if (!resPtr){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIHUMID: mysql_use_result error!\n");
        return FAILURE;
	}

	//只读取第一条记录
	if ((sqlRow = mysql_fetch_row(resPtr)) == NULL)
	{
		mysql_free_result(resPtr);
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIHUMID: mysql_fetch_row NULL error!\n");
        return FAILURE;
	}
	else{
		UINT32 index = 2;
		if (sqlRow[index]) humidData->timeStamp = (UINT32)atol(sqlRow[index++]);
		if (sqlRow[index]) humidData->dataFormat = (UINT8)(atol(sqlRow[index++]) & 0xFF);
		if (sqlRow[index]) humidData->humidValue = (UINT32)atol(sqlRow[index++]);
		if (sqlRow[index]) humidData->gps.ew = (UINT32)atol(sqlRow[index++]);
		if (sqlRow[index]) humidData->gps.gpsx = (UINT32)atol(sqlRow[index++]);
		if (sqlRow[index]) humidData->gps.ns = (UINT32)atol(sqlRow[index++]);
		if (sqlRow[index]) humidData->gps.gpsy = (UINT32)atol(sqlRow[index++]);
		if (sqlRow[index]) humidData->gps.gpsz = (UINT32)atol(sqlRow[index++]);
		if (sqlRow[index]) humidData->onOffLineFlag = (UINT8)(atol(sqlRow[index++]) & 0xFF);
	}

	//释放记录集
	mysql_free_result(resPtr);
    mysql_close(sqlHandler);
    return SUCCESS;
}

//删除对应用户所有超过90天的数据
//缺省做成90天，如果参数错误，导致90天以内的数据强行删除，则不被认可
OPSTAT dbi_HcuHumidDataInfo_delete_3monold(UINT32 days)
{
	MYSQL *sqlHandler;
    int result = 0;
    char strsql[DBI_MAX_SQL_INQUERY_STRING_LENGTH];
    UINT32 cursec = 0;

    //入参检查：不涉及到生死问题，参数也没啥大问题，故而不需要检查，都可以存入数据库表单中
    if (days <HUMID_DATA_SAVE_DAYS_MIN) days = HUMID_DATA_SAVE_DAYS_MIN;

	//建立连接
    sqlHandler = mysql_init(NULL);
    if(!sqlHandler)
    {
    	HcuErrorPrint("DBIHUMID: MySQL init failed!\n");
        return FAILURE;
    }
    sqlHandler = mysql_real_connect(sqlHandler, zHcuSysEngPar.dbi.hcuDbHost, zHcuSysEngPar.dbi.hcuDbUser, zHcuSysEngPar.dbi.hcuDbPsw, zHcuSysEngPar.dbi.hcuDbName, zHcuSysEngPar.dbi.hcuDbPort, NULL, 0);  //unix_socket and clientflag not used.
    if (!sqlHandler){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIHUMID: MySQL connection failed!\n");
        return FAILURE;
    }

	//删除满足条件的数据
    cursec = time(NULL);
    days = days * 24 * 3600;
    sprintf(strsql, "DELETE FROM `hcuhumiddatainfo` WHERE (%d - `timestamp` > '%d')", cursec, days);
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIHUMID: INSET data error: %s\n", mysql_error(sqlHandler));
        return FAILURE;
	}

	//释放记录集
    mysql_close(sqlHandler);
    return SUCCESS;
}
*/

