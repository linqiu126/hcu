/*
 * dbinoise.c
 *
 *  Created on: 2016年1月5日
 *      Author: hitpony
 */

#include "../l0dbi/dbinoise.h"
#include "../l0service/trace.h"
#include "../l0dbi/dbicom.h"

//NOISE数据表格式
// HcuNoiseDataInfo
/*
CREATE TABLE IF NOT EXISTS `hcunoisedatainfo` (
  `sid` int(4) NOT NULL AUTO_INCREMENT,
  `deviceid` int(4) NOT NULL,
  `timestamp` int(4) NOT NULL,
  `dataformat` int(1) NOT NULL,
  `noisevalue` int(4) NOT NULL,
  `ew` char(1) NOT NULL,
  `gpsx` int(4) NOT NULL,
  `ns` char(1) NOT NULL,
  `gpsy` int(4) NOT NULL,
  `gpsz` int(4) NOT NULL,
  `onofflineflag` int(2) NOT NULL,
  PRIMARY KEY (`sid`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8;

INSERT INTO `hcudb`.`hcunoisedatainfo` (
`sid` ,
`deviceid` ,
`timestamp` ,
`dataformat`,
`noisevalue` ,
`ew`,
`gpsx` ,
`ns`,
`gpsy` ,
`gpsz` ,
`onofflineflag`
)
VALUES (
'2', '3', '2', '2', '4', 'n', '5', 'w', '6', '7', '8'
);

*/

//存储NOISE数据，每一次存储，都是新增一条记录
//由于是本地数据库，这里不考虑网格问题，直接存储，简单处理
OPSTAT dbi_HcuNoiseDataInfo_save(sensor_noise_data_element_t *noiseData)
{
	MYSQL *sqlHandler;
    int result = 0;
    char strsql[DBI_MAX_SQL_INQUERY_STRING_LENGTH];

    //入参检查：不涉及到生死问题，参数也没啥大问题，故而不需要检查，都可以存入数据库表单中
    if (noiseData == NULL){
    	HcuErrorPrint("DBINOISE: Input parameter NULL pointer!\n");
        return FAILURE;
    }

	//建立连接
    sqlHandler = mysql_init(NULL);
    if(!sqlHandler)
    {
    	HcuErrorPrint("DBINOISE: MySQL init failed!\n");
        return FAILURE;
    }
    sqlHandler = mysql_real_connect(sqlHandler, zHcuSysEngPar.dbi.hcuDbHost, zHcuSysEngPar.dbi.hcuDbUser, zHcuSysEngPar.dbi.hcuDbPsw, zHcuSysEngPar.dbi.hcuDbName, zHcuSysEngPar.dbi.hcuDbPort, NULL, 0);  //unix_socket and clientflag not used.
    if (!sqlHandler){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBINOISE: MySQL connection failed!\n");
        return FAILURE;
    }

    HcuDebugPrint("hcunoisedatainfo: deviceid = %d, timestamp = %d, dataformat = %d, noisevalue = %d, ew = %c, gpsx = %d, ns = %c, gpsy = %d, gpsz = %d, onofflineflag = %d\n\n\n", noiseData->equipid, noiseData->timeStamp, noiseData->dataFormat, noiseData->noiseValue, noiseData->gps.ew, noiseData->gps.gpsx, noiseData->gps.ns, noiseData->gps.gpsy, noiseData->gps.gpsz, noiseData->onOffLineFlag);

	//存入新的数据
    sprintf(strsql, "INSERT INTO `hcunoisedatainfo` (deviceid, timestamp, dataformat, noisevalue, ew, gpsx, ns, gpsy, gpsz, onofflineflag) VALUES \
    		('%d', '%d', '%d', '%d', '%c', '%d', '%c', '%d', '%d', '%d')", noiseData->equipid, noiseData->timeStamp, noiseData->dataFormat, noiseData->noiseValue, noiseData->gps.ew, noiseData->gps.gpsx, noiseData->gps.ns, noiseData->gps.gpsy, noiseData->gps.gpsz, noiseData->onOffLineFlag);

    result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBINOISE: INSERT data error: %s\n", mysql_error(sqlHandler));
        return FAILURE;
	}

	//释放记录集
    mysql_close(sqlHandler);
    /*
	if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_NOR_ON) != FALSE){
		HcuDebugPrint("DBINOISE: NOISE data record save to DB!\n");
	}
	*/
    return SUCCESS;
}

//查询满足条件的第一条记录
//deviceid是入参，指针为出参
OPSTAT dbi_HcuNoiseDataInfo_inqury_1st_record(UINT32 deviceid, sensor_noise_data_element_t *noiseData)
{
	MYSQL *sqlHandler;
	MYSQL_RES *resPtr;
	MYSQL_ROW sqlRow;
    int result = 0;
    char strsql[DBI_MAX_SQL_INQUERY_STRING_LENGTH];

    //入参检查：不涉及到生死问题，参数也没啥大问题，故而不需要检查，都可以存入数据库表单中
    if (noiseData == NULL){
    	HcuErrorPrint("DBINOISE: Input parameter NULL pointer!\n");
        return FAILURE;
    }

	//建立数据库连接
    sqlHandler = mysql_init(NULL);
    if(!sqlHandler)
    {
    	HcuErrorPrint("DBINOISE: MySQL init failed!\n");
        return FAILURE;
    }
    sqlHandler = mysql_real_connect(sqlHandler, zHcuSysEngPar.dbi.hcuDbHost, zHcuSysEngPar.dbi.hcuDbUser, zHcuSysEngPar.dbi.hcuDbPsw, zHcuSysEngPar.dbi.hcuDbName, zHcuSysEngPar.dbi.hcuDbPort, NULL, 0);  //unix_socket and clientflag not used.
    if (!sqlHandler){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBINOISE: MySQL connection failed!\n");
        return FAILURE;
    }

	//获取数据
    sprintf(strsql, "SELECT * FROM `hcunoisedatainfo` WHERE (`deviceid` = '%d')", deviceid);
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBINOISE: INSET data error: %s\n", mysql_error(sqlHandler));
        return FAILURE;
	}

	//查具体的结果
	resPtr = mysql_use_result(sqlHandler);
	if (!resPtr){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBINOISE: mysql_use_result error!\n");
        return FAILURE;
	}

	//只读取第一条记录
	if ((sqlRow = mysql_fetch_row(resPtr)) == NULL)
	{
		mysql_free_result(resPtr);
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBINOISE: mysql_fetch_row NULL error!\n");
        return FAILURE;
	}
	else{
		UINT32 index = 2;
		if (sqlRow[index]) noiseData->timeStamp = (UINT32)atol(sqlRow[index++]);
		if (sqlRow[index]) noiseData->dataFormat = (UINT8)(atol(sqlRow[index++]) & 0xFF);
		if (sqlRow[index]) noiseData->noiseValue = (UINT32)atol(sqlRow[index++]);
		if (sqlRow[index]) noiseData->gps.ew = (UINT32)atol(sqlRow[index++]);
		if (sqlRow[index]) noiseData->gps.gpsx = (UINT32)atol(sqlRow[index++]);
		if (sqlRow[index]) noiseData->gps.ns = (UINT32)atol(sqlRow[index++]);
		if (sqlRow[index]) noiseData->gps.gpsy = (UINT32)atol(sqlRow[index++]);
		if (sqlRow[index]) noiseData->gps.gpsz = (UINT32)atol(sqlRow[index++]);
		if (sqlRow[index]) noiseData->onOffLineFlag = (UINT8)(atol(sqlRow[index++]) & 0xFF);
	}

	//释放记录集
	mysql_free_result(resPtr);
    mysql_close(sqlHandler);
    return SUCCESS;
}

//删除对应用户所有超过90天的数据
//缺省做成90天，如果参数错误，导致90天以内的数据强行删除，则不被认可
OPSTAT dbi_HcuNoiseDataInfo_delete_3monold(UINT32 days)
{
	MYSQL *sqlHandler;
    int result = 0;
    char strsql[DBI_MAX_SQL_INQUERY_STRING_LENGTH];
    UINT32 cursec = 0;

    //入参检查：不涉及到生死问题，参数也没啥大问题，故而不需要检查，都可以存入数据库表单中
    if (days <NOISE_DATA_SAVE_DAYS_MIN) days = NOISE_DATA_SAVE_DAYS_MIN;

	//建立连接
    sqlHandler = mysql_init(NULL);
    if(!sqlHandler)
    {
    	HcuErrorPrint("DBINOISE: MySQL init failed!\n");
        return FAILURE;
    }
    sqlHandler = mysql_real_connect(sqlHandler, zHcuSysEngPar.dbi.hcuDbHost, zHcuSysEngPar.dbi.hcuDbUser, zHcuSysEngPar.dbi.hcuDbPsw, zHcuSysEngPar.dbi.hcuDbName, zHcuSysEngPar.dbi.hcuDbPort, NULL, 0);  //unix_socket and clientflag not used.
    if (!sqlHandler){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBINOISE: MySQL connection failed!\n");
        return FAILURE;
    }

	//删除满足条件的数据
    cursec = time(NULL);
    days = days * 24 * 3600;
    sprintf(strsql, "DELETE FROM `hcunoisedatainfo` WHERE (%d - `timestamp` > '%d')", cursec, days);
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBINOISE: INSET data error: %s\n", mysql_error(sqlHandler));
        return FAILURE;
	}

	//释放记录集
    mysql_close(sqlHandler);
    return SUCCESS;
}





//Get Min Max Avg accoriding time duration by shanchun
OPSTAT dbi_HcuNoiseDataInfo_GetMin(UINT32 dur, HcuSysMsgIeL3aqycContextStaElement_t *Noisedata)
{
	MYSQL *sqlHandler;
    int result = 0;
	MYSQL_RES *resPtr;
    char strsql[DBI_MAX_SQL_INQUERY_STRING_LENGTH];
	MYSQL_ROW sqlRow;
    UINT32 cursec = 0;
    UINT32 duration =0;

	UINT32 index = 0;
	//UINT32 min;

	//建立连接
    sqlHandler = mysql_init(NULL);
    if(!sqlHandler)
    {
    	HcuErrorPrint("DBINOISE: MySQL init failed!\n");
        return FAILURE;
    }
    sqlHandler = mysql_real_connect(sqlHandler, zHcuSysEngPar.dbi.hcuDbHost, zHcuSysEngPar.dbi.hcuDbUser, zHcuSysEngPar.dbi.hcuDbPsw, zHcuSysEngPar.dbi.hcuDbName, zHcuSysEngPar.dbi.hcuDbPort, NULL, 0);  //unix_socket and clientflag not used.
    if (!sqlHandler){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBINOISE: MySQL connection failed!\n");
        return FAILURE;
    }

    cursec = time(NULL);
    duration = dur;

    //Get the minimum data
    sprintf(strsql, "SELECT MIN(noiseValue) FROM `hcunoisedatainfo` WHERE (%d - `timestamp` < '%d')", cursec, duration);
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBINOISE: Get Minimum data error: %s\n", mysql_error(sqlHandler));
        return FAILURE;
	}

	resPtr = mysql_use_result(sqlHandler);
	if (!resPtr){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBINOISE: mysql_use_result error!\n");
        return FAILURE;
	}


	if ((sqlRow = mysql_fetch_row(resPtr)) == NULL)
	{
		mysql_free_result(resPtr);
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBINOISE: mysql_fetch_row NULL error!\n");
        return FAILURE;
	}
	else{

		if (sqlRow[index]) Noisedata->a50001_Min = ((UINT32)atol(sqlRow[index]))*0.1;
		HCU_DEBUG_PRINT_INF("DBINOISE: min=%4.2f\n\n", Noisedata->a50001_Min);
	}

	//释放记录集
	mysql_free_result(resPtr);
    mysql_close(sqlHandler);
    return SUCCESS;
}

OPSTAT dbi_HcuNoiseDataInfo_GetMax(UINT32 dur, HcuSysMsgIeL3aqycContextStaElement_t *Noisedata)
{
	MYSQL *sqlHandler;
    int result = 0;
	MYSQL_RES *resPtr;
    char strsql[DBI_MAX_SQL_INQUERY_STRING_LENGTH];
	MYSQL_ROW sqlRow;
    UINT32 cursec = 0;
    UINT32 duration =0;

	UINT32 index = 0;
	//UINT32 max;

	//建立连接
    sqlHandler = mysql_init(NULL);
    if(!sqlHandler)
    {
    	HcuErrorPrint("DBINOISE: MySQL init failed!\n");
        return FAILURE;
    }
    sqlHandler = mysql_real_connect(sqlHandler, zHcuSysEngPar.dbi.hcuDbHost, zHcuSysEngPar.dbi.hcuDbUser, zHcuSysEngPar.dbi.hcuDbPsw, zHcuSysEngPar.dbi.hcuDbName, zHcuSysEngPar.dbi.hcuDbPort, NULL, 0);  //unix_socket and clientflag not used.
    if (!sqlHandler){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBINOISE: MySQL connection failed!\n");
        return FAILURE;
    }

    cursec = time(NULL);
    duration = dur;

    //Get the minimum data
    sprintf(strsql, "SELECT MAX(noiseValue) FROM `hcunoisedatainfo` WHERE (%d - `timestamp` < '%d')", cursec, duration);
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBINOISE: Get Maximum data error: %s\n", mysql_error(sqlHandler));
        return FAILURE;
	}

	resPtr = mysql_use_result(sqlHandler);
	if (!resPtr){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBINOISE: mysql_use_result error!\n");
        return FAILURE;
	}


	if ((sqlRow = mysql_fetch_row(resPtr)) == NULL)
	{
		mysql_free_result(resPtr);
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBINOISE: mysql_fetch_row NULL error!\n");
        return FAILURE;
	}
	else{

		if (sqlRow[index]) Noisedata->a50001_Max = ((UINT32)atol(sqlRow[index]))*DB_SENSOR_DATA_FOMAT_FLOAT_WITH_NF1;
		HCU_DEBUG_PRINT_INF("DBINOISE: max=%4.2f\n\n", Noisedata->a50001_Max);
	}

	//释放记录集
	mysql_free_result(resPtr);
    mysql_close(sqlHandler);
    return SUCCESS;
}

OPSTAT dbi_HcuNoiseDataInfo_GetAvg(UINT32 dur, HcuSysMsgIeL3aqycContextStaElement_t *Noisedata)
{
	MYSQL *sqlHandler;
    int result = 0;
	MYSQL_RES *resPtr;
    char strsql[DBI_MAX_SQL_INQUERY_STRING_LENGTH];
	MYSQL_ROW sqlRow;
    UINT32 cursec = 0;
    UINT32 duration =0;

	UINT32 index = 0;
	//UINT32 avg;

	//建立连接
    sqlHandler = mysql_init(NULL);
    if(!sqlHandler)
    {
    	HcuErrorPrint("DBINOISE: MySQL init failed!\n");
        return FAILURE;
    }
    sqlHandler = mysql_real_connect(sqlHandler, zHcuSysEngPar.dbi.hcuDbHost, zHcuSysEngPar.dbi.hcuDbUser, zHcuSysEngPar.dbi.hcuDbPsw, zHcuSysEngPar.dbi.hcuDbName, zHcuSysEngPar.dbi.hcuDbPort, NULL, 0);  //unix_socket and clientflag not used.
    if (!sqlHandler){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBINOISE: MySQL connection failed!\n");
        return FAILURE;
    }

    cursec = time(NULL);
    duration = dur;

    //Get the minimum data
    sprintf(strsql, "SELECT AVG(noiseValue) FROM `hcunoisedatainfo` WHERE (%d - `timestamp` < '%d')", cursec, duration);
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBINOISE: Get Avg data error: %s\n", mysql_error(sqlHandler));
        return FAILURE;
	}

	resPtr = mysql_use_result(sqlHandler);
	if (!resPtr){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBINOISE: mysql_use_result error!\n");
        return FAILURE;
	}


	if ((sqlRow = mysql_fetch_row(resPtr)) == NULL)
	{
		mysql_free_result(resPtr);
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBINOISE: mysql_fetch_row NULL error!\n");
        return FAILURE;
	}
	else{

		if (sqlRow[index]) Noisedata->a50001_Avg = ((UINT32)atol(sqlRow[index]))*DB_SENSOR_DATA_FOMAT_FLOAT_WITH_NF1;
		HCU_DEBUG_PRINT_INF("DBINOISE: avg=%4.2f\n\n", Noisedata->a50001_Avg);
	}

	//释放记录集
	mysql_free_result(resPtr);
    mysql_close(sqlHandler);
    return SUCCESS;
}


