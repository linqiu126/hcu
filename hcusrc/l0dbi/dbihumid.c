/*
 * dbihumid.c
 *
 *  Created on: 2016年1月5日
 *      Author: hitpony
 */

#include "../l0dbi/dbihumid.h"
#include "../l0service/trace.h"
#include "../l0dbi/dbicom.h"

//HUMID数据表格式
// HcuHumidDataInfo
/*
CREATE TABLE IF NOT EXISTS `hcuhumiddatainfo` (
  `sid` int(4) NOT NULL AUTO_INCREMENT,
  `deviceid` int(4) NOT NULL,
  `timestamp` int(4) NOT NULL,
  `dataformat` int(1) NOT NULL,
  `humidvalue` int(4) NOT NULL,
  `ew` char(1) NOT NULL,
  `gpsx` int(4) NOT NULL,
  `ns` char(1) NOT NULL,
  `gpsy` int(4) NOT NULL,
  `gpsz` int(4) NOT NULL,
  `onofflineflag` int(2) NOT NULL,
  PRIMARY KEY (`sid`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8;

INSERT INTO `hcudb`.`hcuhumiddatainfo` (
`sid` ,
`deviceid` ,
`timestamp` ,
`dataformat` ,
`humidvalue` ,
`ew` ,
`gpsx` ,
`ns` ,
`gpsy` ,
`gpsz` ,
`onofflineflag`
)
VALUES (
'1', '3', '2', '1', '4', 'n', '5', 'w', '6', '7', '8'
);

CREATE TABLE IF NOT EXISTS `hcuhumiddht11datainfo` (
  `sid` int(4) NOT NULL AUTO_INCREMENT,
  `deviceid` int(4) NOT NULL,
  `timestamp` int(4) NOT NULL,
  `dataformat` int(1) NOT NULL,
  `humidvalue` int(4) NOT NULL,
  PRIMARY KEY (`sid`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8;

INSERT INTO `hcudb`.`hcuhumiddht11datainfo` (
`sid` ,
`deviceid` ,
`timestamp` ,
`dataformat`,
`humidvalue`
)
VALUES (
'1', '3', '2', '1', '4');

CREATE TABLE IF NOT EXISTS `hcuhumidsht20datainfo` (
  `sid` int(4) NOT NULL AUTO_INCREMENT,
  `deviceid` int(4) NOT NULL,
  `timestamp` int(4) NOT NULL,
  `dataformat` int(1) NOT NULL,
  `humidvalue` int(4) NOT NULL,
  PRIMARY KEY (`sid`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8;

INSERT INTO `hcudb`.`hcuhumidsht20datainfo` (
`sid` ,
`deviceid` ,
`timestamp` ,
`dataformat`,
`humidvalue`
)
VALUES (
'1', '3', '2', '1', '4');

CREATE TABLE IF NOT EXISTS `hcuhumidrht03datainfo` (
  `sid` int(4) NOT NULL AUTO_INCREMENT,
  `deviceid` int(4) NOT NULL,
  `timestamp` int(4) NOT NULL,
  `dataformat` int(1) NOT NULL,
  `humidvalue` int(4) NOT NULL,
  PRIMARY KEY (`sid`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8;

INSERT INTO `hcudb`.`hcuhumidrht03datainfo` (
`sid` ,
`deviceid` ,
`timestamp` ,
`dataformat`,
`humidvalue`
)
VALUES (
'1', '3', '2', '1', '4');

CREATE TABLE IF NOT EXISTS `hcuhumidmth01datainfo` (
  `sid` int(4) NOT NULL AUTO_INCREMENT,
  `deviceid` int(4) NOT NULL,
  `timestamp` int(4) NOT NULL,
  `dataformat` int(1) NOT NULL,
  `humidvalue` int(4) NOT NULL,
  PRIMARY KEY (`sid`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8;

INSERT INTO `hcudb`.`hcuhumidmth01datainfo` (
`sid` ,
`deviceid` ,
`timestamp` ,
`dataformat`,
`humidvalue`
)
VALUES (
'1', '3', '2', '1', '4');


*/

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
    	HcuErrorPrint("DBIHUMID: MySQL connection failed: %s\n", mysql_error(sqlHandler));
        return FAILURE;
    }

    HcuDebugPrint("hcuhumiddatainfo: deviceid = %d, timestamp = %d, dataformat = %d, humidValue = %d, ew = %c, gpsx = %d, ns = %c, gpsy = %d, gpsz = %d, onofflineflag = %d\n\n\n", humidData->equipid, humidData->timeStamp, humidData->dataFormat, humidData->humidValue, humidData->gps.ew, humidData->gps.gpsx, humidData->gps.ns, humidData->gps.gpsy, humidData->gps.gpsz, humidData->onOffLineFlag);


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
	if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_NOR_ON) != FALSE){
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

//存储HUMID数据，每一次存储，都是新增一条记录
//由于是本地数据库，这里不考虑网格问题，直接存储，简单处理
OPSTAT dbi_HcuHumidDht11DataInfo_save(sensor_humid_dht11_data_element_t *humidData)
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
    sprintf(strsql, "INSERT INTO `hcuhumiddht11datainfo` (deviceid, timestamp, dataformat, humidvalue) VALUES \
    		('%d', '%d', '%d', '%d')", humidData->equipid, humidData->timeStamp, humidData->dataFormat, humidData->humidValue);
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIHUMID: INSERT data error: %s\n", mysql_error(sqlHandler));
        return FAILURE;
	}

	//释放记录集
    mysql_close(sqlHandler);
	if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_NOR_ON) != FALSE){
		HcuDebugPrint("DBIHUMID: HUMID DHT11 data record save to DB!\n");
	}
    return SUCCESS;
}

//删除对应用户所有超过90天的数据
//缺省做成90天，如果参数错误，导致90天以内的数据强行删除，则不被认可
OPSTAT dbi_HcuHumidDht11DataInfo_delete_3monold(UINT32 days)
{
	MYSQL *sqlHandler;
    int result = 0;
    char strsql[DBI_MAX_SQL_INQUERY_STRING_LENGTH];
    UINT32 cursec = 0;

    //入参检查：不涉及到生死问题，参数也没啥大问题，故而不需要检查，都可以存入数据库表单中
    if (days < HUMID_DATA_SAVE_DAYS_MIN) days = HUMID_DATA_SAVE_DAYS_MIN;

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
    sprintf(strsql, "DELETE FROM `hcuhumiddht11datainfo` WHERE (%d - `timestamp` > '%d')", cursec, days);
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

//存储HUMID数据，每一次存储，都是新增一条记录
//由于是本地数据库，这里不考虑网格问题，直接存储，简单处理
OPSTAT dbi_HcuHumidSht20DataInfo_save(sensor_humid_sht20_data_element_t *humidData)
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
    sprintf(strsql, "INSERT INTO `hcuhumidsht20datainfo` (deviceid, timestamp, dataformat, humidvalue) VALUES \
    		('%d', '%d', '%d', '%d')", humidData->equipid, humidData->timeStamp, humidData->dataFormat, humidData->humidValue);
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIHUMID: INSERT data error: %s\n", mysql_error(sqlHandler));
        return FAILURE;
	}

	//释放记录集
    mysql_close(sqlHandler);
	if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_NOR_ON) != FALSE){
		HcuDebugPrint("DBIHUMID: HUMID SHT20 data record save to DB!\n");
	}
    return SUCCESS;
}

//删除对应用户所有超过90天的数据
//缺省做成90天，如果参数错误，导致90天以内的数据强行删除，则不被认可
OPSTAT dbi_HcuHumidSht20DataInfo_delete_3monold(UINT32 days)
{
	MYSQL *sqlHandler;
    int result = 0;
    char strsql[DBI_MAX_SQL_INQUERY_STRING_LENGTH];
    UINT32 cursec = 0;

    //入参检查：不涉及到生死问题，参数也没啥大问题，故而不需要检查，都可以存入数据库表单中
    if (days < HUMID_DATA_SAVE_DAYS_MIN) days = HUMID_DATA_SAVE_DAYS_MIN;

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
    sprintf(strsql, "DELETE FROM `hcuhumidsht20datainfo` WHERE (%d - `timestamp` > '%d')", cursec, days);
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

//存储HUMID数据，每一次存储，都是新增一条记录
//由于是本地数据库，这里不考虑网格问题，直接存储，简单处理
OPSTAT dbi_HcuHumidRht03DataInfo_save(sensor_humid_rht03_data_element_t *humidData)
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
    sprintf(strsql, "INSERT INTO `hcuhumidrht03datainfo` (deviceid, timestamp, dataformat, humidvalue) VALUES \
    		('%d', '%d', '%d', '%d')", humidData->equipid, humidData->timeStamp, humidData->dataFormat, humidData->humidValue);
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIHUMID: INSERT data error: %s\n", mysql_error(sqlHandler));
        return FAILURE;
	}

	//释放记录集
    mysql_close(sqlHandler);
	if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_NOR_ON) != FALSE){
		HcuDebugPrint("DBIHUMID: HUMID RHT03 data record save to DB!\n");
	}
    return SUCCESS;
}

//删除对应用户所有超过90天的数据
//缺省做成90天，如果参数错误，导致90天以内的数据强行删除，则不被认可
OPSTAT dbi_HcuHumidRht03DataInfo_delete_3monold(UINT32 days)
{
	MYSQL *sqlHandler;
    int result = 0;
    char strsql[DBI_MAX_SQL_INQUERY_STRING_LENGTH];
    UINT32 cursec = 0;

    //入参检查：不涉及到生死问题，参数也没啥大问题，故而不需要检查，都可以存入数据库表单中
    if (days < HUMID_DATA_SAVE_DAYS_MIN) days = HUMID_DATA_SAVE_DAYS_MIN;

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
    sprintf(strsql, "DELETE FROM `hcuhumidrht03datainfo` WHERE (%d - `timestamp` > '%d')", cursec, days);
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

//存储HUMID数据，每一次存储，都是新增一条记录
//由于是本地数据库，这里不考虑网格问题，直接存储，简单处理
OPSTAT dbi_HcuHumidMth01DataInfo_save(sensor_humid_mth01_data_element_t *humidData)
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
    sprintf(strsql, "INSERT INTO `hcuhumidmth01datainfo` (deviceid, timestamp, dataformat, humidvalue) VALUES \
    		('%d', '%d', '%d', '%d')", humidData->equipid, humidData->timeStamp, humidData->dataFormat, humidData->humidValue);
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIHUMID: INSERT data error: %s\n", mysql_error(sqlHandler));
        return FAILURE;
	}

	//释放记录集
    mysql_close(sqlHandler);
	if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_NOR_ON) != FALSE){
		HcuDebugPrint("DBIHUMID: HUMID MTH01 data record save to DB!\n");
	}
    return SUCCESS;
}

//删除对应用户所有超过90天的数据
//缺省做成90天，如果参数错误，导致90天以内的数据强行删除，则不被认可
OPSTAT dbi_HcuHumidMth01DataInfo_delete_3monold(UINT32 days)
{
	MYSQL *sqlHandler;
    int result = 0;
    char strsql[DBI_MAX_SQL_INQUERY_STRING_LENGTH];
    UINT32 cursec = 0;

    //入参检查：不涉及到生死问题，参数也没啥大问题，故而不需要检查，都可以存入数据库表单中
    if (days < HUMID_DATA_SAVE_DAYS_MIN) days = HUMID_DATA_SAVE_DAYS_MIN;

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
    sprintf(strsql, "DELETE FROM `hcuhumidmth01datainfo` WHERE (%d - `timestamp` > '%d')", cursec, days);
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



//Get Min Max Avg accoriding time duration by shanchun
OPSTAT dbi_HcuHumidDataInfo_GetMin(UINT32 dur, HcuSysMsgIeL3aqycContextStaElement_t *Humiddata)
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
    	HcuErrorPrint("DBIHUMID: MySQL init failed!\n");
        return FAILURE;
    }
    sqlHandler = mysql_real_connect(sqlHandler, zHcuSysEngPar.dbi.hcuDbHost, zHcuSysEngPar.dbi.hcuDbUser, zHcuSysEngPar.dbi.hcuDbPsw, zHcuSysEngPar.dbi.hcuDbName, zHcuSysEngPar.dbi.hcuDbPort, NULL, 0);  //unix_socket and clientflag not used.
    if (!sqlHandler){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIHUMID: MySQL connection failed!\n");
        return FAILURE;
    }

    cursec = time(NULL);
    duration = dur;

    //Get the minimum data
    sprintf(strsql, "SELECT MIN(humidvalue) FROM `hcuhumiddatainfo` WHERE (%d - `timestamp` < '%d')", cursec, duration);
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIHUMID: Get Minimum data error: %s\n", mysql_error(sqlHandler));
        return FAILURE;
	}

	resPtr = mysql_use_result(sqlHandler);
	if (!resPtr){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIHUMID: mysql_use_result error!\n");
        return FAILURE;
	}


	if ((sqlRow = mysql_fetch_row(resPtr)) == NULL)
	{
		mysql_free_result(resPtr);
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIHUMID: mysql_fetch_row NULL error!\n");
        return FAILURE;
	}
	else{

		if (sqlRow[index]) Humiddata->a01002_Min = ((UINT32)atol(sqlRow[index]))*DB_SENSOR_DATA_FOMAT_FLOAT_WITH_NF1;
		//HCU_DEBUG_PRINT_INF("DBIHUMID: min=%4.3f\n", Humiddata->a01002_Min);
	}

	//释放记录集
	mysql_free_result(resPtr);
    mysql_close(sqlHandler);
    return SUCCESS;
}

OPSTAT dbi_HcuHumidDataInfo_GetMax(UINT32 dur, HcuSysMsgIeL3aqycContextStaElement_t *Humiddata)
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
    	HcuErrorPrint("DBIHUMID: MySQL init failed!\n");
        return FAILURE;
    }
    sqlHandler = mysql_real_connect(sqlHandler, zHcuSysEngPar.dbi.hcuDbHost, zHcuSysEngPar.dbi.hcuDbUser, zHcuSysEngPar.dbi.hcuDbPsw, zHcuSysEngPar.dbi.hcuDbName, zHcuSysEngPar.dbi.hcuDbPort, NULL, 0);  //unix_socket and clientflag not used.
    if (!sqlHandler){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIHUMID: MySQL connection failed!\n");
        return FAILURE;
    }

    cursec = time(NULL);
    duration = dur;

    //Get the minimum data
    sprintf(strsql, "SELECT MAX(humidvalue) FROM `hcuhumiddatainfo` WHERE (%d - `timestamp` < '%d')", cursec, duration);
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIHUMID: Get Maximum data error: %s\n", mysql_error(sqlHandler));
        return FAILURE;
	}

	resPtr = mysql_use_result(sqlHandler);
	if (!resPtr){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIHUMID: mysql_use_result error!\n");
        return FAILURE;
	}


	if ((sqlRow = mysql_fetch_row(resPtr)) == NULL)
	{
		mysql_free_result(resPtr);
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIHUMID: mysql_fetch_row NULL error!\n");
        return FAILURE;
	}
	else{

		if (sqlRow[index]) Humiddata->a01002_Max = ((UINT32)atol(sqlRow[index]))*DB_SENSOR_DATA_FOMAT_FLOAT_WITH_NF1;
		//HCU_DEBUG_PRINT_INF("DBIHUMID: max=%4.3f\n", Humiddata->a01002_Max);
	}

	//释放记录集
	mysql_free_result(resPtr);
    mysql_close(sqlHandler);
    return SUCCESS;
}

OPSTAT dbi_HcuHumidDataInfo_GetAvg(UINT32 dur, HcuSysMsgIeL3aqycContextStaElement_t *Humiddata)
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
    	HcuErrorPrint("DBIHUMID: MySQL init failed!\n");
        return FAILURE;
    }
    sqlHandler = mysql_real_connect(sqlHandler, zHcuSysEngPar.dbi.hcuDbHost, zHcuSysEngPar.dbi.hcuDbUser, zHcuSysEngPar.dbi.hcuDbPsw, zHcuSysEngPar.dbi.hcuDbName, zHcuSysEngPar.dbi.hcuDbPort, NULL, 0);  //unix_socket and clientflag not used.
    if (!sqlHandler){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIHUMID: MySQL connection failed!\n");
        return FAILURE;
    }

    cursec = time(NULL);
    duration = dur;

    //Get the minimum data
    sprintf(strsql, "SELECT AVG(humidvalue) FROM `hcuhumiddatainfo` WHERE (%d - `timestamp` < '%d')", cursec, duration);
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIHUMID: Get Avg data error: %s\n", mysql_error(sqlHandler));
        return FAILURE;
	}

	resPtr = mysql_use_result(sqlHandler);
	if (!resPtr){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIHUMID: mysql_use_result error!\n");
        return FAILURE;
	}


	if ((sqlRow = mysql_fetch_row(resPtr)) == NULL)
	{
		mysql_free_result(resPtr);
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIHUMID: mysql_fetch_row NULL error!\n");
        return FAILURE;
	}
	else{

		if (sqlRow[index]) Humiddata->a01002_Avg = ((UINT32)atol(sqlRow[index]))*DB_SENSOR_DATA_FOMAT_FLOAT_WITH_NF1;
		//HCU_DEBUG_PRINT_INF("DBIHUMID: avg=%4.3f\n", Humiddata->a01002_Avg);
	}

	//释放记录集
	mysql_free_result(resPtr);
    mysql_close(sqlHandler);
    return SUCCESS;
}
