/*
 * dbipm25.c
 *
 *  Created on: 2016年1月5日
 *      Author: hitpony
 */

#include "../l0dbi/dbipm25.h"
#include "../l0service/trace.h"
#include "../l0dbi/dbicom.h"
#include "../l1com/l1comdef.h" //by Shanchun

//PM25数据表格式
// HcuPm25DataInfo
/*
CREATE TABLE IF NOT EXISTS `hcupm25datainfo` (
  `sid` int(4) NOT NULL AUTO_INCREMENT,
  `deviceid` int(4) NOT NULL,
  `timestamp` int(4) NOT NULL,
  `dataformat` int(1) NOT NULL,
  `pm1d0value` int(4) NOT NULL,
  `pm2d5value` int(4) NOT NULL,
  `pm10dvalue` int(4) NOT NULL,
  `ew` char(1) NOT NULL,
  `gpsx` int(4) NOT NULL,
  `ns` char(1) NOT NULL,
  `gpsy` int(4) NOT NULL,
  `gpsz` int(4) NOT NULL,
  `onofflineflag` int(2) NOT NULL,
  PRIMARY KEY (`sid`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `hcupm25configdata` (
  `sid` int(4) NOT NULL AUTO_INCREMENT,
  `deviceid` int(4) NOT NULL,
  `poweronoff` int(1) NOT NULL,
  `workCycle` int(4) NOT NULL,
  `interSample` int(4) NOT NULL,
  `meausTimes` int(4) NOT NULL,
  PRIMARY KEY (`sid`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8;


INSERT INTO `hcudb`.`hcupm25datainfo` (
`sid` ,
`deviceid` ,
`timestamp` ,
`dataformat`,
`pm1d0value` ,
`pm2d5value` ,
`pm10dvalue` ,
`ew` ,
`gpsx` ,
`ns` ,
`gpsy` ,
`gpsz` ,
`onofflineflag`
)
VALUES (
'1', '3', '2', '1', '4', '44','55',  'e', '66', 'n', '6', '7', '8');

CREATE TABLE IF NOT EXISTS `hcupm25sharpdatainfo` (
  `sid` int(4) NOT NULL AUTO_INCREMENT,
  `deviceid` int(4) NOT NULL,
  `timestamp` int(4) NOT NULL,
  `dataformat` int(1) NOT NULL,
  `pm2d5value` int(4) NOT NULL,
  PRIMARY KEY (`sid`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8;

INSERT INTO `hcudb`.`hcupm25sharpdatainfo` (
`sid` ,
`deviceid` ,
`timestamp` ,
`dataformat`,
`pm2d5value`
)
VALUES (
'1', '3', '2', '1', '4');

CREATE TABLE IF NOT EXISTS `hcupm25bmpd300datainfo` (
  `sid` int(4) NOT NULL AUTO_INCREMENT,
  `deviceid` int(4) NOT NULL,
  `timestamp` int(4) NOT NULL,
  `dataformat` int(1) NOT NULL,
  `pm2d5value` int(4) NOT NULL,
  PRIMARY KEY (`sid`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8;

INSERT INTO `hcudb`.`hcupm25bmpd300datainfo` (
`sid` ,
`deviceid` ,
`timestamp` ,
`dataformat`,
`pm2d5value`
)
VALUES (
'1', '3', '2', '1', '4');

*/

//存储PM25数据，每一次存储，都是新增一条记录
//由于是本地数据库，这里不考虑网格问题，直接存储，简单处理
OPSTAT dbi_HcuPm25DataInfo_save(sensor_pm25_data_element_t *pm25Data)
{
	MYSQL *sqlHandler;
    int result = 0;
    char strsql[DBI_MAX_SQL_INQUERY_STRING_LENGTH];

    //入参检查：不涉及到生死问题，参数也没啥大问题，故而不需要检查，都可以存入数据库表单中
    if (pm25Data == NULL){
    	HcuErrorPrint("DBIPM25: Input parameter NULL pointer!\n");
        return FAILURE;
    }

	//建立连接
    sqlHandler = mysql_init(NULL);
    if(!sqlHandler)
    {
    	HcuErrorPrint("DBIPM25: MySQL init failed!\n");
        return FAILURE;
    }
    sqlHandler = mysql_real_connect(sqlHandler, zHcuSysEngPar.dbi.hcuDbHost, zHcuSysEngPar.dbi.hcuDbUser, zHcuSysEngPar.dbi.hcuDbPsw, zHcuSysEngPar.dbi.hcuDbName, zHcuSysEngPar.dbi.hcuDbPort, NULL, 0);  //unix_socket and clientflag not used.
    if (!sqlHandler){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIPM25: MySQL connection failed!\n");
        return FAILURE;
    }

	//存入新的数据
    sprintf(strsql, "INSERT INTO `hcupm25datainfo` (deviceid, timestamp, dataformat, pm1d0value, pm2d5value, pm10dvalue, ew, gpsx, ns, gpsy, gpsz, onofflineflag) VALUES \
    		('%d', '%d', '%d', '%d', '%d', '%d', '%c', '%d', '%c', '%d', '%d', '%d')", pm25Data->equipid, pm25Data->timeStamp, pm25Data->dataFormat, pm25Data->pm1d0Value, pm25Data->pm2d5Value, pm25Data->pm10Value, pm25Data->gps.ew, pm25Data->gps.gpsx, pm25Data->gps.ns, pm25Data->gps.gpsy, pm25Data->gps.gpsz, pm25Data->onOffLineFlag);
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIPM25: INSERT data error: %s\n", mysql_error(sqlHandler));
        return FAILURE;
	}

	//释放记录集
    mysql_close(sqlHandler);
	if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_NOR_ON) != FALSE){
		HcuDebugPrint("DBIPM25: PM25 data record save to DB!\n");
	}
    return SUCCESS;
}



//存储PM25 sensor config数据 by Shanchun

OPSTAT dbi_HcuPm25ConfigData_save(sensor_modbus_opertion_general_t *PM25ConfigData)
{
	MYSQL *sqlHandler;
    int result = 0;
    char strsql[DBI_MAX_SQL_INQUERY_STRING_LENGTH];
    UINT8 sensortype = MODBUS_PM25_RTU_EQUIPMENT_TYPE;

    //入参检查：不涉及到生死问题，参数也没啥大问题，故而不需要检查，都可以存入数据库表单中
    if (PM25ConfigData == NULL){
    	HcuErrorPrint("DBIPM25: Input parameter NULL pointer!\n");
        return FAILURE;
    }

	//建立连接
    sqlHandler = mysql_init(NULL);
    if(!sqlHandler)
    {
    	HcuErrorPrint("DBIPM25: MySQL init failed!\n");
        return FAILURE;
    }
    sqlHandler = mysql_real_connect(sqlHandler, zHcuSysEngPar.dbi.hcuDbHost, zHcuSysEngPar.dbi.hcuDbUser, zHcuSysEngPar.dbi.hcuDbPsw, zHcuSysEngPar.dbi.hcuDbName, zHcuSysEngPar.dbi.hcuDbPort, NULL, 0);  //unix_socket and clientflag not used.
    if (!sqlHandler){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIPM25: MySQL connection failed!\n");
        return FAILURE;
    }

	//存入新的数据
    sprintf(strsql, "INSERT INTO `hcupm25configdata` (deviceid, poweronoff, workcycle, intersample, meaustimes, sensortype) VALUES \
    		('%d', '%d', '%d', '%d', '%d', '%d')", PM25ConfigData->equId, PM25ConfigData->powerOnOff, PM25ConfigData->workCycle, PM25ConfigData->interSample, PM25ConfigData->meausTimes, sensortype);

    result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIPM25: INSERT data error: %s\n", mysql_error(sqlHandler));
        return FAILURE;
	}

	//释放记录集
    mysql_close(sqlHandler);
	if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_NOR_ON) != FALSE){
		HcuDebugPrint("DBIPM25: PM25 config data record save to DB!\n");
	}
    return SUCCESS;
}

//Update PM25 sensor config data by Shanchun
OPSTAT dbi_HcuPm25ConfigData_update(UINT8 optId, sensor_modbus_opertion_general_t *PM25ConfigData)
{
	MYSQL *sqlHandler;
    int result = 0;
    char strsql[DBI_MAX_SQL_INQUERY_STRING_LENGTH];
    UINT8 sensortype = MODBUS_PM25_RTU_EQUIPMENT_TYPE;

    //入参检查：不涉及到生死问题，参数也没啥大问题，故而不需要检查，都可以存入数据库表单中
    if (PM25ConfigData == NULL){
    	HcuErrorPrint("DBIPM25: Input parameter NULL pointer!\n");
        return FAILURE;
    }

	//建立连接
    sqlHandler = mysql_init(NULL);
    if(!sqlHandler)
    {
    	HcuErrorPrint("DBIPM25: MySQL init failed!\n");
        return FAILURE;
    }
    sqlHandler = mysql_real_connect(sqlHandler, zHcuSysEngPar.dbi.hcuDbHost, zHcuSysEngPar.dbi.hcuDbUser, zHcuSysEngPar.dbi.hcuDbPsw, zHcuSysEngPar.dbi.hcuDbName, zHcuSysEngPar.dbi.hcuDbPort, NULL, 0);  //unix_socket and clientflag not used.
    if (!sqlHandler){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIPM25: MySQL connection failed!\n");
        return FAILURE;
    }

	switch(optId)
	{
	    case L3PO_pm25_set_switch:
	    case L3PO_pm25_read_switch:
	    	sprintf(strsql, "UPDATE `hcupm25configdata` SET `poweronoff` = '%d' WHERE `hcupm25configdata`.`sensortype` = %d", PM25ConfigData->powerOnOff,sensortype);
	    	break;

	    case L3PO_pm25_set_modbus_address:
	    case L3PO_pm25_read_modbus_address:
	    	sprintf(strsql, "UPDATE `hcupm25configdata` SET `deviceid` = '%d' WHERE `hcupm25configdata`.`sensortype` = %d", PM25ConfigData->equId,sensortype);
		    break;

	    case L3PO_pm25_set_work_cycle:
	    case L3PO_pm25_read_work_cycle:
	    	sprintf(strsql, "UPDATE `hcupm25configdata` SET `workcycle` = '%d' WHERE `hcupm25configdata`.`sensortype` = %d", PM25ConfigData->workCycle,sensortype);
		    break;

	    case L3PO_pm25_set_sample_cycle:
	    case L3PO_pm25_read_sample_cycle:
	    	sprintf(strsql, "UPDATE `hcupm25configdata` SET `intersample` = '%d' WHERE `hcupm25configdata`.`sensortype` = %d", PM25ConfigData->interSample,sensortype);
		    break;

	    case L3PO_pm25_set_sample_number:
	    case L3PO_pm25_read_sample_number:
	        sprintf(strsql, "UPDATE `hcupm25configdata` SET `meausTimes` = '%d' WHERE (`hcupm25configdata`.`sensortype` = %d)", PM25ConfigData->meausTimes,sensortype);
	        //sprintf(strsql, "DELETE FROM `hcupm25datainfo` WHERE (%d - `timestamp` > '%d')", cursec, days);
	        //sprintf(strsql, "SELECT * FROM `hcupm25datainfo` WHERE (`deviceid` = '%d')", deviceid);
	        break;

	    default:
		    HcuErrorPrint("DBIPM25: Error operation code received!\n");
		    return FAILURE;
		    break;

	}

	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIPM25: UPDATE data error: %s\n", mysql_error(sqlHandler));
        return FAILURE;
	}

	//释放记录集
    mysql_close(sqlHandler);
	if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_NOR_ON) != FALSE){
		HcuDebugPrint("DBIPM25: PM25 config data record save to DB!\n");
	}
    return SUCCESS;
}

//查询满足条件的第一条记录
//deviceid是入参，指针为出参
OPSTAT dbi_HcuPm25DataInfo_inqury_1st_record(UINT32 deviceid, sensor_pm25_data_element_t *pm25Data)
{
	MYSQL *sqlHandler;
	MYSQL_RES *resPtr;
	MYSQL_ROW sqlRow;
    int result = 0;
    char strsql[DBI_MAX_SQL_INQUERY_STRING_LENGTH];

    //入参检查：不涉及到生死问题，参数也没啥大问题，故而不需要检查，都可以存入数据库表单中
    if (pm25Data == NULL){
    	HcuErrorPrint("DBIPM25: Input parameter NULL pointer!\n");
        return FAILURE;
    }

	//建立数据库连接
    sqlHandler = mysql_init(NULL);
    if(!sqlHandler)
    {
    	HcuErrorPrint("DBIPM25: MySQL init failed!\n");
        return FAILURE;
    }
    sqlHandler = mysql_real_connect(sqlHandler, zHcuSysEngPar.dbi.hcuDbHost, zHcuSysEngPar.dbi.hcuDbUser, zHcuSysEngPar.dbi.hcuDbPsw, zHcuSysEngPar.dbi.hcuDbName, zHcuSysEngPar.dbi.hcuDbPort, NULL, 0);  //unix_socket and clientflag not used.
    if (!sqlHandler){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIPM25: MySQL connection failed!\n");
        return FAILURE;
    }

	//获取数据
    sprintf(strsql, "SELECT * FROM `hcupm25datainfo` WHERE (`deviceid` = '%d')", deviceid);
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIPM25: INSET data error: %s\n", mysql_error(sqlHandler));
        return FAILURE;
	}

	//查具体的结果
	resPtr = mysql_use_result(sqlHandler);
	if (!resPtr){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIPM25: mysql_use_result error!\n");
        return FAILURE;
	}

	//只读取第一条记录
	if ((sqlRow = mysql_fetch_row(resPtr)) == NULL)
	{
		mysql_free_result(resPtr);
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIPM25: mysql_fetch_row NULL error!\n");
        return FAILURE;
	}
	else{
		UINT32 index = 2;
		if (sqlRow[index]) pm25Data->timeStamp = (UINT32)atol(sqlRow[index++]);
		if (sqlRow[index]) pm25Data->dataFormat = (UINT8)(atol(sqlRow[index++]) & 0xFF);
		if (sqlRow[index]) pm25Data->pm1d0Value = (UINT32)atol(sqlRow[index++]);
		if (sqlRow[index]) pm25Data->pm2d5Value = (UINT32)atol(sqlRow[index++]);
		if (sqlRow[index]) pm25Data->pm10Value = (UINT32)atol(sqlRow[index++]);
		if (sqlRow[index]) pm25Data->gps.ew = (UINT32)atol(sqlRow[index++]);
		if (sqlRow[index]) pm25Data->gps.gpsx = (UINT32)atol(sqlRow[index++]);
		if (sqlRow[index]) pm25Data->gps.ns = (UINT32)atol(sqlRow[index++]);
		if (sqlRow[index]) pm25Data->gps.gpsy = (UINT32)atol(sqlRow[index++]);
		if (sqlRow[index]) pm25Data->gps.gpsz = (UINT32)atol(sqlRow[index++]);
		if (sqlRow[index]) pm25Data->onOffLineFlag = (UINT8)(atol(sqlRow[index++]) & 0xFF);
	}

	//释放记录集
	mysql_free_result(resPtr);
    mysql_close(sqlHandler);
    return SUCCESS;
}

//删除对应用户所有超过90天的数据
//缺省做成90天，如果参数错误，导致90天以内的数据强行删除，则不被认可
OPSTAT dbi_HcuPm25DataInfo_delete_3monold(UINT32 days)
{
	MYSQL *sqlHandler;
    int result = 0;
    char strsql[DBI_MAX_SQL_INQUERY_STRING_LENGTH];
    UINT32 cursec = 0;

    //入参检查：不涉及到生死问题，参数也没啥大问题，故而不需要检查，都可以存入数据库表单中
    if (days <PM25_DATA_SAVE_DAYS_MIN) days = PM25_DATA_SAVE_DAYS_MIN;

	//建立连接
    sqlHandler = mysql_init(NULL);
    if(!sqlHandler)
    {
    	HcuErrorPrint("DBIPM25: MySQL init failed!\n");
        return FAILURE;
    }
    sqlHandler = mysql_real_connect(sqlHandler, zHcuSysEngPar.dbi.hcuDbHost, zHcuSysEngPar.dbi.hcuDbUser, zHcuSysEngPar.dbi.hcuDbPsw, zHcuSysEngPar.dbi.hcuDbName, zHcuSysEngPar.dbi.hcuDbPort, NULL, 0);  //unix_socket and clientflag not used.
    if (!sqlHandler){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIPM25: MySQL connection failed!\n");
        return FAILURE;
    }

	//删除满足条件的数据
    cursec = time(NULL);
    days = days * 24 * 3600;
    sprintf(strsql, "DELETE FROM `hcupm25datainfo` WHERE (%d - `timestamp` > '%d')", cursec, days);
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIPM25: INSET data error: %s\n", mysql_error(sqlHandler));
        return FAILURE;
	}

	//释放记录集
    mysql_close(sqlHandler);
    return SUCCESS;
}

//存储PM25 sharp数据，每一次存储，都是新增一条记录
//由于是本地数据库，这里不考虑网格问题，直接存储，简单处理
OPSTAT dbi_HcuPm25SharpDataInfo_save(sensor_pm25_sharp_data_element_t *pm25Data)
{
	MYSQL *sqlHandler;
    int result = 0;
    char strsql[DBI_MAX_SQL_INQUERY_STRING_LENGTH];

    //入参检查：不涉及到生死问题，参数也没啥大问题，故而不需要检查，都可以存入数据库表单中

     if (pm25Data == NULL){
    	HcuErrorPrint("DBIPM25: Input parameter NULL pointer!\n");
        return FAILURE;
    }

	//建立连接
    sqlHandler = mysql_init(NULL);
    if(!sqlHandler)
    {
    	HcuErrorPrint("DBIPM25: MySQL init failed!\n");
        return FAILURE;
    }
    sqlHandler = mysql_real_connect(sqlHandler, zHcuSysEngPar.dbi.hcuDbHost, zHcuSysEngPar.dbi.hcuDbUser, zHcuSysEngPar.dbi.hcuDbPsw, zHcuSysEngPar.dbi.hcuDbName, zHcuSysEngPar.dbi.hcuDbPort, NULL, 0);  //unix_socket and clientflag not used.
    if (!sqlHandler){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIPM25: MySQL connection failed!\n");
        return FAILURE;
    }

	//存入新的数据
    sprintf(strsql, "INSERT INTO `hcupm25sharpdatainfo` (deviceid, timestamp, dataformat, pm2d5value) VALUES \
    		('%d', '%d', '%d', '%d')", pm25Data->equipid, pm25Data->timeStamp, pm25Data->dataFormat, pm25Data->pm2d5Value);
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIPM25: INSERT data error: %s\n", mysql_error(sqlHandler));
        return FAILURE;
	}

	//释放记录集
    mysql_close(sqlHandler);
	if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_NOR_ON) != FALSE){
		HcuDebugPrint("DBIPM25: PM25 sharp data record save to DB!\n");
	}
    return SUCCESS;
}

//删除对应用户所有超过90天的数据
//缺省做成90天，如果参数错误，导致90天以内的数据强行删除，则不被认可
OPSTAT dbi_HcuPm25SharpDataInfo_delete_3monold(UINT32 days)
{
	MYSQL *sqlHandler;
    int result = 0;
    char strsql[DBI_MAX_SQL_INQUERY_STRING_LENGTH];
    UINT32 cursec = 0;

    //入参检查：不涉及到生死问题，参数也没啥大问题，故而不需要检查，都可以存入数据库表单中
    if (days < PM25SHARP_DATA_SAVE_DAYS_MIN) days = PM25SHARP_DATA_SAVE_DAYS_MIN;

	//建立连接
    sqlHandler = mysql_init(NULL);
    if(!sqlHandler)
    {
    	HcuErrorPrint("DBIPM25: MySQL init failed!\n");
        return FAILURE;
    }
    sqlHandler = mysql_real_connect(sqlHandler, zHcuSysEngPar.dbi.hcuDbHost, zHcuSysEngPar.dbi.hcuDbUser, zHcuSysEngPar.dbi.hcuDbPsw, zHcuSysEngPar.dbi.hcuDbName, zHcuSysEngPar.dbi.hcuDbPort, NULL, 0);  //unix_socket and clientflag not used.
    if (!sqlHandler){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIPM25: MySQL connection failed!\n");
        return FAILURE;
    }

	//删除满足条件的数据
    cursec = time(NULL);
    days = days * 24 * 3600;
    sprintf(strsql, "DELETE FROM `hcupm25sharpdatainfo` WHERE (%d - `timestamp` > '%d')", cursec, days);
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIPM25: INSET data error: %s\n", mysql_error(sqlHandler));
        return FAILURE;
	}

	//释放记录集
    mysql_close(sqlHandler);
    return SUCCESS;
}

//存储PM25 bmpd300数据，每一次存储，都是新增一条记录
//由于是本地数据库，这里不考虑网格问题，直接存储，简单处理
OPSTAT dbi_HcuPm25Bmpd300DataInfo_save(sensor_pm25_bmpd300_data_element_t *pm25Data)
{
	MYSQL *sqlHandler;
    int result = 0;
    char strsql[DBI_MAX_SQL_INQUERY_STRING_LENGTH];

    //入参检查：不涉及到生死问题，参数也没啥大问题，故而不需要检查，都可以存入数据库表单中

     if (pm25Data == NULL){
    	HcuErrorPrint("DBIPM25: Input parameter NULL pointer!\n");
        return FAILURE;
    }

	//建立连接
    sqlHandler = mysql_init(NULL);
    if(!sqlHandler)
    {
    	HcuErrorPrint("DBIPM25: MySQL init failed!\n");
        return FAILURE;
    }
    sqlHandler = mysql_real_connect(sqlHandler, zHcuSysEngPar.dbi.hcuDbHost, zHcuSysEngPar.dbi.hcuDbUser, zHcuSysEngPar.dbi.hcuDbPsw, zHcuSysEngPar.dbi.hcuDbName, zHcuSysEngPar.dbi.hcuDbPort, NULL, 0);  //unix_socket and clientflag not used.
    if (!sqlHandler){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIPM25: MySQL connection failed!\n");
        return FAILURE;
    }

	//存入新的数据
    sprintf(strsql, "INSERT INTO `hcupm25bmpd300datainfo` (deviceid, timestamp, dataformat, pm2d5value) VALUES \
    		('%d', '%d', '%d', '%d')", pm25Data->equipid, pm25Data->timeStamp, pm25Data->dataFormat, pm25Data->pm2d5Value);
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIPM25: INSERT data error: %s\n", mysql_error(sqlHandler));
        return FAILURE;
	}

	//释放记录集
    mysql_close(sqlHandler);
	if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_NOR_ON) != FALSE){
		HcuDebugPrint("DBIPM25: PM25 bmpd300 data record save to DB!\n");
	}
    return SUCCESS;
}

//删除对应用户所有超过90天的数据
//缺省做成90天，如果参数错误，导致90天以内的数据强行删除，则不被认可
OPSTAT dbi_HcuPm25Bmpd300DataInfo_delete_3monold(UINT32 days)
{
	MYSQL *sqlHandler;
    int result = 0;
    char strsql[DBI_MAX_SQL_INQUERY_STRING_LENGTH];
    UINT32 cursec = 0;

    //入参检查：不涉及到生死问题，参数也没啥大问题，故而不需要检查，都可以存入数据库表单中
    if (days < PM25SHARP_DATA_SAVE_DAYS_MIN) days = PM25SHARP_DATA_SAVE_DAYS_MIN;

	//建立连接
    sqlHandler = mysql_init(NULL);
    if(!sqlHandler)
    {
    	HcuErrorPrint("DBIPM25: MySQL init failed!\n");
        return FAILURE;
    }
    sqlHandler = mysql_real_connect(sqlHandler, zHcuSysEngPar.dbi.hcuDbHost, zHcuSysEngPar.dbi.hcuDbUser, zHcuSysEngPar.dbi.hcuDbPsw, zHcuSysEngPar.dbi.hcuDbName, zHcuSysEngPar.dbi.hcuDbPort, NULL, 0);  //unix_socket and clientflag not used.
    if (!sqlHandler){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIPM25: MySQL connection failed!\n");
        return FAILURE;
    }

	//删除满足条件的数据
    cursec = time(NULL);
    days = days * 24 * 3600;
    sprintf(strsql, "DELETE FROM `hcupm25bmpd300datainfo` WHERE (%d - `timestamp` > '%d')", cursec, days);
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIPM25: INSET data error: %s\n", mysql_error(sqlHandler));
        return FAILURE;
	}

	//释放记录集
    mysql_close(sqlHandler);
    return SUCCESS;
}



OPSTAT dbi_HcuPm25DataInfo_GetMin(UINT32 dur, HcuSysMsgIeL3aqycContextStaElement_t *PM10data)
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
    	HcuErrorPrint("DBIPM25: MySQL init failed!\n");
        return FAILURE;
    }
    sqlHandler = mysql_real_connect(sqlHandler, zHcuSysEngPar.dbi.hcuDbHost, zHcuSysEngPar.dbi.hcuDbUser, zHcuSysEngPar.dbi.hcuDbPsw, zHcuSysEngPar.dbi.hcuDbName, zHcuSysEngPar.dbi.hcuDbPort, NULL, 0);  //unix_socket and clientflag not used.
    if (!sqlHandler){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIPM25: MySQL connection failed!\n");
        return FAILURE;
    }

    cursec = time(NULL);
    duration = dur;

    //Get the minimum data
    sprintf(strsql, "SELECT MIN(pm10dvalue) FROM `hcupm25datainfo` WHERE (%d - `timestamp` < '%d')", cursec, duration);
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIPM25: Get Minimum data error: %s\n", mysql_error(sqlHandler));
        return FAILURE;
	}

	resPtr = mysql_use_result(sqlHandler);
	if (!resPtr){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIPM25: mysql_use_result error!\n");
        return FAILURE;
	}


	if ((sqlRow = mysql_fetch_row(resPtr)) == NULL)
	{
		mysql_free_result(resPtr);
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIPM25: mysql_fetch_row NULL error!\n");
        return FAILURE;
	}
	else{

		if (sqlRow[index]) PM10data->a34001_Min = (UINT32)atol(sqlRow[index]);
		HCU_DEBUG_PRINT_INF("DBIPM25: min=%d\n\n", PM10data->a34001_Min);
	}

	//释放记录集
	mysql_free_result(resPtr);
    mysql_close(sqlHandler);
    return SUCCESS;
}

OPSTAT dbi_HcuPm25DataInfo_GetMax(UINT32 dur, HcuSysMsgIeL3aqycContextStaElement_t *PM10data)
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
    	HcuErrorPrint("DBIPM25: MySQL init failed!\n");
        return FAILURE;
    }
    sqlHandler = mysql_real_connect(sqlHandler, zHcuSysEngPar.dbi.hcuDbHost, zHcuSysEngPar.dbi.hcuDbUser, zHcuSysEngPar.dbi.hcuDbPsw, zHcuSysEngPar.dbi.hcuDbName, zHcuSysEngPar.dbi.hcuDbPort, NULL, 0);  //unix_socket and clientflag not used.
    if (!sqlHandler){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIPM25: MySQL connection failed!\n");
        return FAILURE;
    }

    cursec = time(NULL);
    duration = dur;

    //Get the minimum data
    sprintf(strsql, "SELECT MAX(pm10dvalue) FROM `hcupm25datainfo` WHERE (%d - `timestamp` < '%d')", cursec, duration);
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIPM25: Get Maximum data error: %s\n", mysql_error(sqlHandler));
        return FAILURE;
	}

	resPtr = mysql_use_result(sqlHandler);
	if (!resPtr){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIPM25: mysql_use_result error!\n");
        return FAILURE;
	}


	if ((sqlRow = mysql_fetch_row(resPtr)) == NULL)
	{
		mysql_free_result(resPtr);
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIPM25: mysql_fetch_row NULL error!\n");
        return FAILURE;
	}
	else{

		if (sqlRow[index]) PM10data->a34001_Max = (UINT32)atol(sqlRow[index]);
		HCU_DEBUG_PRINT_INF("DBIPM25: max=%d\n\n", PM10data->a34001_Max);
	}

	//释放记录集
	mysql_free_result(resPtr);
    mysql_close(sqlHandler);
    return SUCCESS;
}

OPSTAT dbi_HcuPm25DataInfo_GetAvg(UINT32 dur, HcuSysMsgIeL3aqycContextStaElement_t *PM10data)
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
    	HcuErrorPrint("DBIPM25: MySQL init failed!\n");
        return FAILURE;
    }
    sqlHandler = mysql_real_connect(sqlHandler, zHcuSysEngPar.dbi.hcuDbHost, zHcuSysEngPar.dbi.hcuDbUser, zHcuSysEngPar.dbi.hcuDbPsw, zHcuSysEngPar.dbi.hcuDbName, zHcuSysEngPar.dbi.hcuDbPort, NULL, 0);  //unix_socket and clientflag not used.
    if (!sqlHandler){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIPM25: MySQL connection failed!\n");
        return FAILURE;
    }

    cursec = time(NULL);
    duration = dur;

    //Get the minimum data
    sprintf(strsql, "SELECT AVG(pm10dvalue) FROM `hcupm25datainfo` WHERE (%d - `timestamp` < '%d')", cursec, duration);
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIPM25: Get Avg data error: %s\n", mysql_error(sqlHandler));
        return FAILURE;
	}

	resPtr = mysql_use_result(sqlHandler);
	if (!resPtr){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIPM25: mysql_use_result error!\n");
        return FAILURE;
	}


	if ((sqlRow = mysql_fetch_row(resPtr)) == NULL)
	{
		mysql_free_result(resPtr);
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIPM25: mysql_fetch_row NULL error!\n");
        return FAILURE;
	}
	else{

		if (sqlRow[index]) PM10data->a34001_Avg = (UINT32)atol(sqlRow[index]);
		HCU_DEBUG_PRINT_INF("DBIPM25: avg=%d\n\n", PM10data->a34001_Avg);
	}

	//释放记录集
	mysql_free_result(resPtr);
    mysql_close(sqlHandler);
    return SUCCESS;
}

