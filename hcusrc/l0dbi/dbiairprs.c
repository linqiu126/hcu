/*
 * dbiairprs.c
 *
 *  Created on: 2016年5月10日
 *      Author: hitpony
 */

#include "../l0dbi/dbiairprs.h"
#include "../l0service/trace.h"
#include "../l0dbi/dbicom.h"

//AIRPRS数据表格式
// HcuAirprsDataInfo
/*
CREATE TABLE IF NOT EXISTS `hcuairprsdatainfo` (
  `sid` int(4) NOT NULL AUTO_INCREMENT,
  `deviceid` int(4) NOT NULL,
  `timestamp` int(4) NOT NULL,
  `dataformat` int(1) NOT NULL,
  `airprsvalue` int(4) NOT NULL,
  `ew` char(1) NOT NULL,
  `gpsx` int(4) NOT NULL,
  `ns` char(1) NOT NULL,
  `gpsy` int(4) NOT NULL,
  `gpsz` int(4) NOT NULL,
  `onofflineflag` int(2) NOT NULL,
  PRIMARY KEY (`sid`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8;

INSERT INTO `hcudb`.`hcuairprsdatainfo` (
`sid` ,
`deviceid` ,
`timestamp` ,
`dataformat`,
`airprsvalue` ,
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

CREATE TABLE IF NOT EXISTS `hcuairprsbmp180datainfo` (
  `sid` int(4) NOT NULL AUTO_INCREMENT,
  `deviceid` int(4) NOT NULL,
  `timestamp` int(4) NOT NULL,
  `dataformat` int(1) NOT NULL,
  `airprsvalue` int(4) NOT NULL,
  PRIMARY KEY (`sid`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8;

INSERT INTO `hcudb`.`hcuairprsbmp180datainfo` (
`sid` ,
`deviceid` ,
`timestamp` ,
`dataformat`,
`airprsvalue`
)
VALUES (
'1', '3', '2', '1', '4');

CREATE TABLE IF NOT EXISTS `hcuairprsaltitudebmp180datainfo` (
  `sid` int(4) NOT NULL AUTO_INCREMENT,
  `deviceid` int(4) NOT NULL,
  `timestamp` int(4) NOT NULL,
  `dataformat` int(1) NOT NULL,
  `altitudevalue` int(4) NOT NULL,
  PRIMARY KEY (`sid`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8;

INSERT INTO `hcudb`.`hcuairprsaltitudebmp180datainfo` (
`sid` ,
`deviceid` ,
`timestamp` ,
`dataformat`,
`altitudevalue`
)
VALUES (
'1', '3', '2', '1', '4');



*/

//存储AIRPRS数据，每一次存储，都是新增一条记录
//由于是本地数据库，这里不考虑网格问题，直接存储，简单处理
OPSTAT dbi_HcuAirprsDataInfo_save(sensor_airprs_data_element_t *airprsData)
{
	MYSQL *sqlHandler;
    int result = 0;
    char strsql[DBI_MAX_SQL_INQUERY_STRING_LENGTH];

    //入参检查：不涉及到生死问题，参数也没啥大问题，故而不需要检查，都可以存入数据库表单中
    if (airprsData == NULL){
    	HcuErrorPrint("DBIAIRPRS: Input parameter NULL pointer!\n");
        return FAILURE;
    }

	//建立连接
//    sqlHandler = mysql_init(NULL);
//    if(!sqlHandler)
//    {
//    	HcuErrorPrint("DBIAIRPRS: MySQL init failed!\n");
//        return FAILURE;
//    }
//    sqlHandler = mysql_real_connect(sqlHandler, zHcuSysEngPar.dbi.hcuDbHost, zHcuSysEngPar.dbi.hcuDbUser, zHcuSysEngPar.dbi.hcuDbPsw, zHcuSysEngPar.dbi.hcuDbName, zHcuSysEngPar.dbi.hcuDbPort, NULL, 0);  //unix_socket and clientflag not used.
//    if (!sqlHandler){
//    	mysql_close(sqlHandler);
//    	HcuErrorPrint("DBIAIRPRS: MySQL connection failed!\n");
//        return FAILURE;
//    }
    HCU_L0DBICOM_INIT_DB_CONN();

	//存入新的数据
    sprintf(strsql, "INSERT INTO `hcuairprsdatainfo` (deviceid, timestamp, dataformat, airprsvalue, ew, gpsx, ns, gpsy, gpsz, onofflineflag) VALUES \
    		('%d', '%d', '%d', '%d', '%c', '%d', '%c', '%d', '%d', '%d')", airprsData->equipid, airprsData->timeStamp, airprsData->dataFormat, airprsData->airprsValue, airprsData->gps.ew, airprsData->gps.gpsx, airprsData->gps.ns, airprsData->gps.gpsy, airprsData->gps.gpsz, airprsData->onOffLineFlag);
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIAIRPRS: INSERT data error: %s\n", mysql_error(sqlHandler));
        return FAILURE;
	}

	//释放记录集
    mysql_close(sqlHandler);
	if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_NOR_ON) != FALSE){
		HcuDebugPrint("DBIAIRPRS: AIRPRS data record save to DB!\n");
	}
    return SUCCESS;
}

//查询满足条件的第一条记录
//deviceid是入参，指针为出参
OPSTAT dbi_HcuAirprsDataInfo_inqury_1st_record(UINT32 deviceid, sensor_airprs_data_element_t *airprsData)
{
	MYSQL *sqlHandler;
	MYSQL_RES *resPtr;
	MYSQL_ROW sqlRow;
    int result = 0;
    char strsql[DBI_MAX_SQL_INQUERY_STRING_LENGTH];

    //入参检查：不涉及到生死问题，参数也没啥大问题，故而不需要检查，都可以存入数据库表单中
    if (airprsData == NULL){
    	HcuErrorPrint("DBIAIRPRS: Input parameter NULL pointer!\n");
        return FAILURE;
    }

	//建立数据库连接
//    sqlHandler = mysql_init(NULL);
//    if(!sqlHandler)
//    {
//    	HcuErrorPrint("DBIAIRPRS: MySQL init failed!\n");
//        return FAILURE;
//    }
//    sqlHandler = mysql_real_connect(sqlHandler, zHcuSysEngPar.dbi.hcuDbHost, zHcuSysEngPar.dbi.hcuDbUser, zHcuSysEngPar.dbi.hcuDbPsw, zHcuSysEngPar.dbi.hcuDbName, zHcuSysEngPar.dbi.hcuDbPort, NULL, 0);  //unix_socket and clientflag not used.
//    if (!sqlHandler){
//    	mysql_close(sqlHandler);
//    	HcuErrorPrint("DBIAIRPRS: MySQL connection failed!\n");
//        return FAILURE;
//    }
    HCU_L0DBICOM_INIT_DB_CONN();

	//获取数据
    sprintf(strsql, "SELECT * FROM `hcuairprsdatainfo` WHERE (`deviceid` = '%d')", deviceid);
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIAIRPRS: INSET data error: %s\n", mysql_error(sqlHandler));
        return FAILURE;
	}

	//查具体的结果
	resPtr = mysql_use_result(sqlHandler);
	if (!resPtr){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIAIRPRS: mysql_use_result error!\n");
        return FAILURE;
	}

	//只读取第一条记录
	if ((sqlRow = mysql_fetch_row(resPtr)) == NULL)
	{
		mysql_free_result(resPtr);
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIAIRPRS: mysql_fetch_row NULL error!\n");
        return FAILURE;
	}
	else{
		UINT32 index = 2;
		if (sqlRow[index]) airprsData->timeStamp = (UINT32)atol(sqlRow[index++]);
		if (sqlRow[index]) airprsData->dataFormat = (UINT8)(atol(sqlRow[index++]) & 0xFF);
		if (sqlRow[index]) airprsData->airprsValue = (UINT32)atol(sqlRow[index++]);
		if (sqlRow[index]) airprsData->gps.ew = (UINT32)atol(sqlRow[index++]);
		if (sqlRow[index]) airprsData->gps.gpsx = (UINT32)atol(sqlRow[index++]);
		if (sqlRow[index]) airprsData->gps.ns = (UINT32)atol(sqlRow[index++]);
		if (sqlRow[index]) airprsData->gps.gpsy = (UINT32)atol(sqlRow[index++]);
		if (sqlRow[index]) airprsData->gps.gpsz = (UINT32)atol(sqlRow[index++]);
		if (sqlRow[index]) airprsData->onOffLineFlag = (UINT8)(atol(sqlRow[index++]) & 0xFF);
	}

	//释放记录集
	mysql_free_result(resPtr);
    mysql_close(sqlHandler);
    return SUCCESS;
}

//删除对应用户所有超过90天的数据
//缺省做成90天，如果参数错误，导致90天以内的数据强行删除，则不被认可
OPSTAT dbi_HcuAirprsDataInfo_delete_3monold(UINT32 days)
{
	MYSQL *sqlHandler;
    int result = 0;
    char strsql[DBI_MAX_SQL_INQUERY_STRING_LENGTH];
    UINT32 cursec = 0;

    //入参检查：不涉及到生死问题，参数也没啥大问题，故而不需要检查，都可以存入数据库表单中
    if (days < AIRPRS_DATA_SAVE_DAYS_MIN) days = AIRPRS_DATA_SAVE_DAYS_MIN;

	//建立连接
//    sqlHandler = mysql_init(NULL);
//    if(!sqlHandler)
//    {
//    	HcuErrorPrint("DBIAIRPRS: MySQL init failed!\n");
//        return FAILURE;
//    }
//    sqlHandler = mysql_real_connect(sqlHandler, zHcuSysEngPar.dbi.hcuDbHost, zHcuSysEngPar.dbi.hcuDbUser, zHcuSysEngPar.dbi.hcuDbPsw, zHcuSysEngPar.dbi.hcuDbName, zHcuSysEngPar.dbi.hcuDbPort, NULL, 0);  //unix_socket and clientflag not used.
//    if (!sqlHandler){
//    	mysql_close(sqlHandler);
//    	HcuErrorPrint("DBIAIRPRS: MySQL connection failed!\n");
//        return FAILURE;
//    }
    HCU_L0DBICOM_INIT_DB_CONN();

	//删除满足条件的数据
    cursec = time(NULL);
    days = days * 24 * 3600;
    sprintf(strsql, "DELETE FROM `hcuairprsdatainfo` WHERE (%d - `timestamp` > '%d')", cursec, days);
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIAIRPRS: INSET data error: %s\n", mysql_error(sqlHandler));
        return FAILURE;
	}

	//释放记录集
    mysql_close(sqlHandler);
    return SUCCESS;
}

//存储AIRPRS数据，每一次存储，都是新增一条记录
//由于是本地数据库，这里不考虑网格问题，直接存储，简单处理
OPSTAT dbi_HcuAirprsBmp180DataInfo_save(sensor_airprs_bmp180_data_element_t *airprsData)
{
	MYSQL *sqlHandler;
    int result = 0;
    char strsql[DBI_MAX_SQL_INQUERY_STRING_LENGTH];

    //入参检查：不涉及到生死问题，参数也没啥大问题，故而不需要检查，都可以存入数据库表单中
    if (airprsData == NULL){
    	HcuErrorPrint("DBIAIRPRS: Input parameter NULL pointer!\n");
        return FAILURE;
    }

	//建立连接
//    sqlHandler = mysql_init(NULL);
//    if(!sqlHandler)
//    {
//    	HcuErrorPrint("DBIAIRPRS: MySQL init failed!\n");
//        return FAILURE;
//    }
//    sqlHandler = mysql_real_connect(sqlHandler, zHcuSysEngPar.dbi.hcuDbHost, zHcuSysEngPar.dbi.hcuDbUser, zHcuSysEngPar.dbi.hcuDbPsw, zHcuSysEngPar.dbi.hcuDbName, zHcuSysEngPar.dbi.hcuDbPort, NULL, 0);  //unix_socket and clientflag not used.
//    if (!sqlHandler){
//    	mysql_close(sqlHandler);
//    	HcuErrorPrint("DBIAIRPRS: MySQL connection failed!\n");
//        return FAILURE;
//    }
    HCU_L0DBICOM_INIT_DB_CONN();

	//存入新的数据
    sprintf(strsql, "INSERT INTO `hcuairprsbmp180datainfo` (deviceid, timestamp, dataformat, airprsvalue) VALUES \
    		('%d', '%d', '%d', '%d')", airprsData->equipid, airprsData->timeStamp, airprsData->dataFormat, airprsData->airprsValue);
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIAIRPRS: INSERT data error: %s\n", mysql_error(sqlHandler));
        return FAILURE;
	}

	//释放记录集
    mysql_close(sqlHandler);
	if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_NOR_ON) != FALSE){
		HcuDebugPrint("DBIAIRPRS: AIRPRS BMP180 data record save to DB!\n");
	}
    return SUCCESS;
}

//删除对应用户所有超过90天的数据
//缺省做成90天，如果参数错误，导致90天以内的数据强行删除，则不被认可
OPSTAT dbi_HcuAirprsBmp180DataInfo_delete_3monold(UINT32 days)
{
	MYSQL *sqlHandler;
    int result = 0;
    char strsql[DBI_MAX_SQL_INQUERY_STRING_LENGTH];
    UINT32 cursec = 0;

    //入参检查：不涉及到生死问题，参数也没啥大问题，故而不需要检查，都可以存入数据库表单中
    if (days < AIRPRS_DATA_SAVE_DAYS_MIN) days = AIRPRS_DATA_SAVE_DAYS_MIN;

	//建立连接
//    sqlHandler = mysql_init(NULL);
//    if(!sqlHandler)
//    {
//    	HcuErrorPrint("DBIAIRPRS: MySQL init failed!\n");
//        return FAILURE;
//    }
//    sqlHandler = mysql_real_connect(sqlHandler, zHcuSysEngPar.dbi.hcuDbHost, zHcuSysEngPar.dbi.hcuDbUser, zHcuSysEngPar.dbi.hcuDbPsw, zHcuSysEngPar.dbi.hcuDbName, zHcuSysEngPar.dbi.hcuDbPort, NULL, 0);  //unix_socket and clientflag not used.
//    if (!sqlHandler){
//    	mysql_close(sqlHandler);
//    	HcuErrorPrint("DBIAIRPRS: MySQL connection failed!\n");
//        return FAILURE;
//    }
    HCU_L0DBICOM_INIT_DB_CONN();

	//删除满足条件的数据
    cursec = time(NULL);
    days = days * 24 * 3600;
    sprintf(strsql, "DELETE FROM `hcuairprsbmp180datainfo` WHERE (%d - `timestamp` > '%d')", cursec, days);
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIAIRPRS: INSET data error: %s\n", mysql_error(sqlHandler));
        return FAILURE;
	}

	//释放记录集
    mysql_close(sqlHandler);
    return SUCCESS;
}

//存储AIRPRS数据，每一次存储，都是新增一条记录
//由于是本地数据库，这里不考虑网格问题，直接存储，简单处理
OPSTAT dbi_HcuAirprsAltitudeBmp180DataInfo_save(sensor_airprs_altitude_bmp180_data_element_t *altitudeData)
{
	MYSQL *sqlHandler;
    int result = 0;
    char strsql[DBI_MAX_SQL_INQUERY_STRING_LENGTH];

    //入参检查：不涉及到生死问题，参数也没啥大问题，故而不需要检查，都可以存入数据库表单中
    if (altitudeData == NULL){
    	HcuErrorPrint("DBIAIRPRS: Input parameter NULL pointer!\n");
        return FAILURE;
    }

	//建立连接
//    sqlHandler = mysql_init(NULL);
//    if(!sqlHandler)
//    {
//    	HcuErrorPrint("DBIAIRPRS: MySQL init failed!\n");
//        return FAILURE;
//    }
//    sqlHandler = mysql_real_connect(sqlHandler, zHcuSysEngPar.dbi.hcuDbHost, zHcuSysEngPar.dbi.hcuDbUser, zHcuSysEngPar.dbi.hcuDbPsw, zHcuSysEngPar.dbi.hcuDbName, zHcuSysEngPar.dbi.hcuDbPort, NULL, 0);  //unix_socket and clientflag not used.
//    if (!sqlHandler){
//    	mysql_close(sqlHandler);
//    	HcuErrorPrint("DBIAIRPRS: MySQL connection failed!\n");
//        return FAILURE;
//    }
    HCU_L0DBICOM_INIT_DB_CONN();

	//存入新的数据
    sprintf(strsql, "INSERT INTO `hcuairprsaltitudebmp180datainfo` (deviceid, timestamp, dataformat, altitudevalue) VALUES \
    		('%d', '%d', '%d', '%d')", altitudeData->equipid, altitudeData->timeStamp, altitudeData->dataFormat, altitudeData->altitudeValue);
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIAIRPRS: INSERT data error: %s\n", mysql_error(sqlHandler));
        return FAILURE;
	}

	//释放记录集
    mysql_close(sqlHandler);
	if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_NOR_ON) != FALSE){
		HcuDebugPrint("DBIAIRPRS: AIRPRS BMP180 data record save to DB!\n");
	}
    return SUCCESS;
}

//删除对应用户所有超过90天的数据
//缺省做成90天，如果参数错误，导致90天以内的数据强行删除，则不被认可
OPSTAT dbi_HcuAirprsAltitudeBmp180DataInfo_delete_3monold(UINT32 days)
{
	MYSQL *sqlHandler;
    int result = 0;
    char strsql[DBI_MAX_SQL_INQUERY_STRING_LENGTH];
    UINT32 cursec = 0;

    //入参检查：不涉及到生死问题，参数也没啥大问题，故而不需要检查，都可以存入数据库表单中
    if (days < AIRPRS_DATA_SAVE_DAYS_MIN) days = AIRPRS_DATA_SAVE_DAYS_MIN;

	//建立连接
//    sqlHandler = mysql_init(NULL);
//    if(!sqlHandler)
//    {
//    	HcuErrorPrint("DBIAIRPRS: MySQL init failed!\n");
//        return FAILURE;
//    }
//    sqlHandler = mysql_real_connect(sqlHandler, zHcuSysEngPar.dbi.hcuDbHost, zHcuSysEngPar.dbi.hcuDbUser, zHcuSysEngPar.dbi.hcuDbPsw, zHcuSysEngPar.dbi.hcuDbName, zHcuSysEngPar.dbi.hcuDbPort, NULL, 0);  //unix_socket and clientflag not used.
//    if (!sqlHandler){
//    	mysql_close(sqlHandler);
//    	HcuErrorPrint("DBIAIRPRS: MySQL connection failed!\n");
//        return FAILURE;
//    }
    HCU_L0DBICOM_INIT_DB_CONN();

	//删除满足条件的数据
    cursec = time(NULL);
    days = days * 24 * 3600;
    sprintf(strsql, "DELETE FROM `hcuairprsaltitudebmp180datainfo` WHERE (%d - `timestamp` > '%d')", cursec, days);
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIAIRPRS: INSET data error: %s\n", mysql_error(sqlHandler));
        return FAILURE;
	}

	//释放记录集
    mysql_close(sqlHandler);
    return SUCCESS;
}
