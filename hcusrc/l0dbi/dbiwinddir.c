/*
 * dbiwinddir.c
 *
 *  Created on: 2016年1月5日
 *      Author: hitpony
 */

#include "../l0dbi/dbiwinddir.h"
#include "../l0service/trace.h"
#include "../l0dbi/dbicom.h"

//WINDDIR数据表格式
// HcuWinddirDataInfo
/*
CREATE TABLE IF NOT EXISTS `hcuwinddirdatainfo` (
  `sid` int(4) NOT NULL AUTO_INCREMENT,
  `deviceid` int(4) NOT NULL,
  `timestamp` int(4) NOT NULL,
  `dataformat` int(1) NOT NULL,
  `winddirvalue` int(4) NOT NULL,
  `ew` char(1) NOT NULL,
  `gpsx` int(4) NOT NULL,
  `ns` char(1) NOT NULL,
  `gpsy` int(4) NOT NULL,
  `gpsz` int(4) NOT NULL,
  `onofflineflag` int(2) NOT NULL,
  PRIMARY KEY (`sid`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8;

INSERT INTO `hcudb`.`hcuwinddirdatainfo` (
`sid` ,
`deviceid` ,
`timestamp` ,
`dataformat`,
`winddirvalue` ,
`ew` ,
`gpsx` ,
`ns` ,
`gpsy` ,
`gpsz` ,
`onofflineflag`
)
VALUES (
'1', '3', '2', '1', '4', 'e', '5', 'n', '6', '7', '8'
);

*/

//存储WINDDIR数据，每一次存储，都是新增一条记录
//由于是本地数据库，这里不考虑网格问题，直接存储，简单处理
OPSTAT dbi_HcuWinddirDataInfo_save(sensor_winddir_data_element_t *winddirData)
{
	MYSQL *sqlHandler;
    int result = 0;
    char strsql[DBI_MAX_SQL_INQUERY_STRING_LENGTH];

    //入参检查：不涉及到生死问题，参数也没啥大问题，故而不需要检查，都可以存入数据库表单中
    if (winddirData == NULL){
    	HcuErrorPrint("DBIWINDDIR: Input parameter NULL pointer!\n");
        return FAILURE;
    }

	//建立连接
    sqlHandler = mysql_init(NULL);
    if(!sqlHandler)
    {
    	HcuErrorPrint("DBIWINDDIR: MySQL init failed!\n");
        return FAILURE;
    }
    sqlHandler = mysql_real_connect(sqlHandler, zHcuSysEngPar.dbi.hcuDbHost, zHcuSysEngPar.dbi.hcuDbUser, zHcuSysEngPar.dbi.hcuDbPsw, zHcuSysEngPar.dbi.hcuDbName, zHcuSysEngPar.dbi.hcuDbPort, NULL, 0);  //unix_socket and clientflag not used.
    if (!sqlHandler){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIWINDDIR: MySQL connection failed!\n");
        return FAILURE;
    }


    HcuDebugPrint("hcuwinddirdatainfo: deviceid = %d, timestamp = %d, dataformat = %d, winddirvalue = %d, ew = %c, gpsx = %d, ns = %c, gpsy = %d, gpsz = %d, onofflineflag = %d\n\n\n", winddirData->equipid, winddirData->timeStamp, winddirData->dataFormat, winddirData->winddirValue, winddirData->gps.ew, winddirData->gps.gpsx, winddirData->gps.ns, winddirData->gps.gpsy, winddirData->gps.gpsz, winddirData->onOffLineFlag);


	//存入新的数据
    sprintf(strsql, "INSERT INTO `hcuwinddirdatainfo` (deviceid, timestamp, dataformat, winddirvalue, ew, gpsx, ns, gpsy, gpsz, onofflineflag) VALUES \
    		('%d', '%d', '%d', '%d', '%c', '%d', '%c', '%d', '%d', '%d')", winddirData->equipid, winddirData->timeStamp, winddirData->dataFormat, winddirData->winddirValue, winddirData->gps.ew, winddirData->gps.gpsx, winddirData->gps.ns,winddirData->gps.gpsy, winddirData->gps.gpsz, winddirData->onOffLineFlag);
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIWINDDIR: INSERT data error: %s\n", mysql_error(sqlHandler));
        return FAILURE;
	}

	//释放记录集
    mysql_close(sqlHandler);
	if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_NOR_ON) != FALSE){
		HcuDebugPrint("DBIWINDDIR: WINDDIR data record save to DB!\n");
	}
    return SUCCESS;
}

//查询满足条件的第一条记录
//deviceid是入参，指针为出参
OPSTAT dbi_HcuWinddirDataInfo_inqury_1st_record(UINT32 deviceid, sensor_winddir_data_element_t *winddirData)
{
	MYSQL *sqlHandler;
	MYSQL_RES *resPtr;
	MYSQL_ROW sqlRow;
    int result = 0;
    char strsql[DBI_MAX_SQL_INQUERY_STRING_LENGTH];

    //入参检查：不涉及到生死问题，参数也没啥大问题，故而不需要检查，都可以存入数据库表单中
    if (winddirData == NULL){
    	HcuErrorPrint("DBIWINDDIR: Input parameter NULL pointer!\n");
        return FAILURE;
    }

	//建立数据库连接
    sqlHandler = mysql_init(NULL);
    if(!sqlHandler)
    {
    	HcuErrorPrint("DBIWINDDIR: MySQL init failed!\n");
        return FAILURE;
    }
    sqlHandler = mysql_real_connect(sqlHandler, zHcuSysEngPar.dbi.hcuDbHost, zHcuSysEngPar.dbi.hcuDbUser, zHcuSysEngPar.dbi.hcuDbPsw, zHcuSysEngPar.dbi.hcuDbName, zHcuSysEngPar.dbi.hcuDbPort, NULL, 0);  //unix_socket and clientflag not used.
    if (!sqlHandler){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIWINDDIR: MySQL connection failed!\n");
        return FAILURE;
    }

	//获取数据
    sprintf(strsql, "SELECT * FROM `hcuwinddirdatainfo` WHERE (`deviceid` = '%d')", deviceid);
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIWINDDIR: INSET data error: %s\n", mysql_error(sqlHandler));
        return FAILURE;
	}

	//查具体的结果
	resPtr = mysql_use_result(sqlHandler);
	if (!resPtr){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIWINDDIR: mysql_use_result error!\n");
        return FAILURE;
	}

	//只读取第一条记录
	if ((sqlRow = mysql_fetch_row(resPtr)) == NULL)
	{
		mysql_free_result(resPtr);
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIWINDDIR: mysql_fetch_row NULL error!\n");
        return FAILURE;
	}
	else{
		UINT32 index = 2;
		if (sqlRow[index]) winddirData->timeStamp = (UINT32)atol(sqlRow[index++]);
		if (sqlRow[index]) winddirData->dataFormat = (UINT8)(atol(sqlRow[index++]) & 0xFF);
		if (sqlRow[index]) winddirData->winddirValue = (UINT32)atol(sqlRow[index++]);
		if (sqlRow[index]) winddirData->gps.ew = (UINT32)atol(sqlRow[index++]);
		if (sqlRow[index]) winddirData->gps.gpsx = (UINT32)atol(sqlRow[index++]);
		if (sqlRow[index]) winddirData->gps.ns = (UINT32)atol(sqlRow[index++]);
		if (sqlRow[index]) winddirData->gps.gpsy = (UINT32)atol(sqlRow[index++]);
		if (sqlRow[index]) winddirData->gps.gpsz = (UINT32)atol(sqlRow[index++]);
		if (sqlRow[index]) winddirData->onOffLineFlag = (UINT8)(atol(sqlRow[index++]) & 0xFF);
	}

	//释放记录集
	mysql_free_result(resPtr);
    mysql_close(sqlHandler);
    return SUCCESS;
}

//删除对应用户所有超过90天的数据
//缺省做成90天，如果参数错误，导致90天以内的数据强行删除，则不被认可
OPSTAT dbi_HcuWinddirDataInfo_delete_3monold(UINT32 days)
{
	MYSQL *sqlHandler;
    int result = 0;
    char strsql[DBI_MAX_SQL_INQUERY_STRING_LENGTH];
    UINT32 cursec = 0;

    //入参检查：不涉及到生死问题，参数也没啥大问题，故而不需要检查，都可以存入数据库表单中
    if (days <WINDDIR_DATA_SAVE_DAYS_MIN) days = WINDDIR_DATA_SAVE_DAYS_MIN;

	//建立连接
    sqlHandler = mysql_init(NULL);
    if(!sqlHandler)
    {
    	HcuErrorPrint("DBIWINDDIR: MySQL init failed!\n");
        return FAILURE;
    }
    sqlHandler = mysql_real_connect(sqlHandler, zHcuSysEngPar.dbi.hcuDbHost, zHcuSysEngPar.dbi.hcuDbUser, zHcuSysEngPar.dbi.hcuDbPsw, zHcuSysEngPar.dbi.hcuDbName, zHcuSysEngPar.dbi.hcuDbPort, NULL, 0);  //unix_socket and clientflag not used.
    if (!sqlHandler){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIWINDDIR: MySQL connection failed!\n");
        return FAILURE;
    }

	//删除满足条件的数据
    cursec = time(NULL);
    days = days * 24 * 3600;
    sprintf(strsql, "DELETE FROM `hcuwinddirdatainfo` WHERE (%d - `timestamp` > '%d')", cursec, days);
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIWINDDIR: INSET data error: %s\n", mysql_error(sqlHandler));
        return FAILURE;
	}

	//释放记录集
    mysql_close(sqlHandler);
    return SUCCESS;
}


//Get Min Max Avg accoriding time duration by shanchun
OPSTAT dbi_HcuWinddirDataInfo_GetMin(UINT32 dur, HcuSysMsgIeL3aqycContextStaElement_t *winddirData)
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
    	HcuErrorPrint("DBIWINDDIR: MySQL init failed!\n");
        return FAILURE;
    }
    sqlHandler = mysql_real_connect(sqlHandler, zHcuSysEngPar.dbi.hcuDbHost, zHcuSysEngPar.dbi.hcuDbUser, zHcuSysEngPar.dbi.hcuDbPsw, zHcuSysEngPar.dbi.hcuDbName, zHcuSysEngPar.dbi.hcuDbPort, NULL, 0);  //unix_socket and clientflag not used.
    if (!sqlHandler){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIWINDDIR: MySQL connection failed!\n");
        return FAILURE;
    }

    cursec = time(NULL);
    duration = dur;

    //Get the minimum data
    sprintf(strsql, "SELECT MIN(winddirvalue) FROM `hcuwinddirdatainfo` WHERE (%d - `timestamp` < '%d')", cursec, duration);
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIWINDDIR: Get Minimum data error: %s\n", mysql_error(sqlHandler));
        return FAILURE;
	}

	resPtr = mysql_use_result(sqlHandler);
	if (!resPtr){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIWINDDIR: mysql_use_result error!\n");
        return FAILURE;
	}


	if ((sqlRow = mysql_fetch_row(resPtr)) == NULL)
	{
		mysql_free_result(resPtr);
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIWINDDIR: mysql_fetch_row NULL error!\n");
        return FAILURE;
	}
	else{

		if (sqlRow[index]) winddirData->a01008_Min = ((UINT32)atol(sqlRow[index]))*DB_SENSOR_DATA_FOMAT_FLOAT_WITH_NF1;
		//HCU_DEBUG_PRINT_INF("DBIWINDDIR: min=%4.3f\n", winddirData->a01008_Min);
	}

	//释放记录集
	mysql_free_result(resPtr);
    mysql_close(sqlHandler);
    return SUCCESS;
}

OPSTAT dbi_HcuWinddirDataInfo_GetMax(UINT32 dur, HcuSysMsgIeL3aqycContextStaElement_t *winddirData)
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
    	HcuErrorPrint("DBIWINDDIR: MySQL init failed!\n");
        return FAILURE;
    }
    sqlHandler = mysql_real_connect(sqlHandler, zHcuSysEngPar.dbi.hcuDbHost, zHcuSysEngPar.dbi.hcuDbUser, zHcuSysEngPar.dbi.hcuDbPsw, zHcuSysEngPar.dbi.hcuDbName, zHcuSysEngPar.dbi.hcuDbPort, NULL, 0);  //unix_socket and clientflag not used.
    if (!sqlHandler){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIWINDDIR: MySQL connection failed!\n");
        return FAILURE;
    }

    cursec = time(NULL);
    duration = dur;

    //Get the minimum data
    sprintf(strsql, "SELECT MAX(winddirvalue) FROM `hcuwinddirdatainfo` WHERE (%d - `timestamp` < '%d')", cursec, duration);
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIWINDDIR: Get Maximum data error: %s\n", mysql_error(sqlHandler));
        return FAILURE;
	}

	resPtr = mysql_use_result(sqlHandler);
	if (!resPtr){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIWINDDIR: mysql_use_result error!\n");
        return FAILURE;
	}


	if ((sqlRow = mysql_fetch_row(resPtr)) == NULL)
	{
		mysql_free_result(resPtr);
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIWINDDIR: mysql_fetch_row NULL error!\n");
        return FAILURE;
	}
	else{

		if (sqlRow[index]) winddirData->a01008_Max = ((UINT32)atol(sqlRow[index]))*DB_SENSOR_DATA_FOMAT_FLOAT_WITH_NF1;
		//HCU_DEBUG_PRINT_INF("DBIWINDDIR: max=%4.3f\n", winddirData->a01008_Max);
	}

	//释放记录集
	mysql_free_result(resPtr);
    mysql_close(sqlHandler);
    return SUCCESS;
}

OPSTAT dbi_HcuWinddirDataInfo_GetAvg(UINT32 dur, HcuSysMsgIeL3aqycContextStaElement_t *winddirData)
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
    	HcuErrorPrint("DBIWINDDIR: MySQL init failed!\n");
        return FAILURE;
    }
    sqlHandler = mysql_real_connect(sqlHandler, zHcuSysEngPar.dbi.hcuDbHost, zHcuSysEngPar.dbi.hcuDbUser, zHcuSysEngPar.dbi.hcuDbPsw, zHcuSysEngPar.dbi.hcuDbName, zHcuSysEngPar.dbi.hcuDbPort, NULL, 0);  //unix_socket and clientflag not used.
    if (!sqlHandler){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIWINDDIR: MySQL connection failed!\n");
        return FAILURE;
    }

    cursec = time(NULL);
    duration = dur;

    //Get the minimum data
    sprintf(strsql, "SELECT AVG(winddirvalue) FROM `hcuwinddirdatainfo` WHERE (%d - `timestamp` < '%d')", cursec, duration);
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIWINDDIR: Get Avg data error: %s\n", mysql_error(sqlHandler));
        return FAILURE;
	}

	resPtr = mysql_use_result(sqlHandler);
	if (!resPtr){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIWINDDIR: mysql_use_result error!\n");
        return FAILURE;
	}


	if ((sqlRow = mysql_fetch_row(resPtr)) == NULL)
	{
		mysql_free_result(resPtr);
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIWINDDIR: mysql_fetch_row NULL error!\n");
        return FAILURE;
	}
	else{

		if (sqlRow[index]) winddirData->a01008_Avg = ((UINT32)atol(sqlRow[index]))*DB_SENSOR_DATA_FOMAT_FLOAT_WITH_NF1;
		//HCU_DEBUG_PRINT_INF("DBIWINDDIR: avg=%4.3f\n", winddirData->a01008_Avg);
	}

	//释放记录集
	mysql_free_result(resPtr);
    mysql_close(sqlHandler);
    return SUCCESS;
}
