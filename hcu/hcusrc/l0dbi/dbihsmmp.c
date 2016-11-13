/*
 * dbihsmmp.c
 *
 *  Created on: 2016年1月5日
 *      Author: hitpony
 */

#include "../l0dbi/dbihsmmp.h"
#include "../l0service/trace.h"
#include "../l0dbi/dbicom.h"

//HSMMP数据表格式
// HcuHsmmpDataInfo
/*
CREATE TABLE IF NOT EXISTS `hcuhsmmpdatainfo` (
  `sid` int(4) NOT NULL AUTO_INCREMENT,
  `deviceid` int(4) NOT NULL,
  `timestamp` int(4) NOT NULL,
  `hsmmpfdir` char(100) NOT NULL,
  `hsmmpfname` char(100) NOT NULL,
  `hsmmplink` char(100) NOT NULL,
  `ew` char(1) NOT NULL,
  `gpsx` int(4) NOT NULL,
  `ns` char(1) NOT NULL,
  `gpsy` int(4) NOT NULL,
  `gpsz` int(4) NOT NULL,
  `onofflineflag` int(2) NOT NULL,
  PRIMARY KEY (`sid`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8;

INSERT INTO `hcuhsmmpdatainfo` (`sid`, `deviceid`, `timestamp`, `hsmmpfdir`, `hsmmpfname`, `hsmmplink`, `ew`, `gpsx`, `ns`, `gpsy`, `gpsz`, `onofflineflag`) VALUES
(1, 221, 1452836531, '/home/hitpony/workspace/hcu/X86_dev/log/201601/avorion201601142142.h264', 'avorion201601142142.h264', 'http://192.168.1.232/avorion/avorion201601142142.h264', 'n', 0, 'w', 0, 0, 1),
(2, 221, 1452836943, '/home/hitpony/workspace/hcu/X86_dev/log/201601/avorion201601142149.h264', 'avorion201601142149.h264', 'http://192.168.1.232/avorion/avorion201601142149.h264', 'n', 0, 'w', 0, 0, 1),
(3, 221, 1452837443, '/home/hitpony/hcu/X86_dev/log/201601/avorion201601142157.h264', 'avorion201601142157.h264', 'http://192.168.1.232/avorion/avorion201601142157.h264', 'n', 0, 'w', 0, 0, 1);

*/

//存储HSMMP数据，每一次存储，都是新增一条记录
//由于是本地数据库，这里不考虑网格问题，直接存储，简单处理
OPSTAT dbi_HcuHsmmpDataInfo_save(sensor_hsmmp_data_element_t *hsmmpData)
{
	MYSQL *sqlHandler;
    int result = 0;
    char strsql[DBI_MAX_SQL_INQUERY_STRING_LENGTH];

    //入参检查：不涉及到生死问题，参数也没啥大问题，故而不需要检查，都可以存入数据库表单中
    if (hsmmpData == NULL){
    	HcuErrorPrint("DBIHSMMP: Input parameter NULL pointer!\n");
        return FAILURE;
    }

	//建立连接
    sqlHandler = mysql_init(NULL);
    if(!sqlHandler)
    {
    	HcuErrorPrint("DBIHSMMP: MySQL init failed!\n");
        return FAILURE;
    }
    sqlHandler = mysql_real_connect(sqlHandler, zHcuSysEngPar.dbi.hcuDbHost, zHcuSysEngPar.dbi.hcuDbUser, zHcuSysEngPar.dbi.hcuDbPsw, \
    		zHcuSysEngPar.dbi.hcuDbName, zHcuSysEngPar.dbi.hcuDbPort, NULL, 0);  //unix_socket and clientflag not used.
    if (!sqlHandler){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIHSMMP: MySQL connection failed!\n");
        return FAILURE;
    }
	//存入新的数据
    sprintf(strsql, "INSERT INTO `hcuhsmmpdatainfo` (deviceid, timestamp, hsmmpfdir, hsmmpfname, hsmmplink, ew, gpsx, ns, gpsy, gpsz, onofflineflag) VALUES \
    		('%d', '%d', '%s', '%s', '%s', '%c', '%d', '%c', '%d', '%d', '%d')", hsmmpData->equipid, hsmmpData->timeStamp, hsmmpData->hsmmpFdir, \
			hsmmpData->hsmmpFname, hsmmpData->hsmmpLink, hsmmpData->gps.ew, hsmmpData->gps.gpsx, hsmmpData->gps.ns, hsmmpData->gps.gpsy, hsmmpData->gps.gpsz, hsmmpData->onOffLineFlag);
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIHSMMP: INSERT data error: %s\n", mysql_error(sqlHandler));
        return FAILURE;
	}
	//释放记录集
    mysql_close(sqlHandler);
	if ((zHcuSysEngPar.debugMode & HCU_TRACE_DEBUG_NOR_ON) != FALSE){
		HcuDebugPrint("DBIHSMMP: HSMMP data record save to DB!\n");
	}
    return SUCCESS;
}

//查询满足条件的第一条记录
//deviceid是入参，指针为出参
OPSTAT dbi_HcuHsmmpDataInfo_inqury_1st_record(UINT32 deviceid, sensor_hsmmp_data_element_t *hsmmpData)
{
	MYSQL *sqlHandler;
	MYSQL_RES *resPtr;
	MYSQL_ROW sqlRow;
    int result = 0;
    char strsql[DBI_MAX_SQL_INQUERY_STRING_LENGTH];

    //入参检查：不涉及到生死问题，参数也没啥大问题，故而不需要检查，都可以存入数据库表单中
    if (hsmmpData == NULL){
    	HcuErrorPrint("DBIHSMMP: Input parameter NULL pointer!\n");
        return FAILURE;
    }

	//建立数据库连接
    sqlHandler = mysql_init(NULL);
    if(!sqlHandler)
    {
    	HcuErrorPrint("DBIHSMMP: MySQL init failed!\n");
        return FAILURE;
    }
    sqlHandler = mysql_real_connect(sqlHandler, zHcuSysEngPar.dbi.hcuDbHost, zHcuSysEngPar.dbi.hcuDbUser, zHcuSysEngPar.dbi.hcuDbPsw, zHcuSysEngPar.dbi.hcuDbName, zHcuSysEngPar.dbi.hcuDbPort, NULL, 0);  //unix_socket and clientflag not used.
    if (!sqlHandler){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIHSMMP: MySQL connection failed!\n");
        return FAILURE;
    }

	//获取数据
    sprintf(strsql, "SELECT * FROM `hcuhsmmpdatainfo` WHERE (`deviceid` = '%d')", deviceid);
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIHSMMP: INSET data error: %s\n", mysql_error(sqlHandler));
        return FAILURE;
	}

	//查具体的结果
	resPtr = mysql_use_result(sqlHandler);
	if (!resPtr){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIHSMMP: mysql_use_result error!\n");
        return FAILURE;
	}

	//只读取第一条记录
	if ((sqlRow = mysql_fetch_row(resPtr)) == NULL)
	{
		mysql_free_result(resPtr);
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIHSMMP: mysql_fetch_row NULL error!\n");
        return FAILURE;
	}
	else{
		UINT32 index = 2;
		if (sqlRow[index]) hsmmpData->timeStamp = (UINT32)atol(sqlRow[index++]);
		if (sqlRow[index]) strncpy(hsmmpData->hsmmpFdir, sqlRow[index++], HCU_FILE_NAME_LENGTH_MAX-1);
		if (sqlRow[index]) strncpy(hsmmpData->hsmmpFname, sqlRow[index++], HCU_FILE_NAME_LENGTH_MAX-1);
		if (sqlRow[index]) strncpy(hsmmpData->hsmmpLink, sqlRow[index++], HCU_FILE_NAME_LENGTH_MAX-1);
		if (sqlRow[index]) hsmmpData->gps.ew = (UINT32)atol(sqlRow[index++]);
		if (sqlRow[index]) hsmmpData->gps.gpsx = (UINT32)atol(sqlRow[index++]);
		if (sqlRow[index]) hsmmpData->gps.ns = (UINT32)atol(sqlRow[index++]);
		if (sqlRow[index]) hsmmpData->gps.gpsy = (UINT32)atol(sqlRow[index++]);
		if (sqlRow[index]) hsmmpData->gps.gpsz = (UINT32)atol(sqlRow[index++]);
		if (sqlRow[index]) hsmmpData->onOffLineFlag = (UINT8)(atol(sqlRow[index++]) & 0xFF);
	}

	//释放记录集
	mysql_free_result(resPtr);
    mysql_close(sqlHandler);
    return SUCCESS;
}

//删除对应用户所有超过90天的数据
//缺省做成90天，如果参数错误，导致90天以内的数据强行删除，则不被认可
OPSTAT dbi_HcuHsmmpDataInfo_delete_3monold(UINT32 days)
{
	MYSQL *sqlHandler;
    int result = 0;
    char strsql[DBI_MAX_SQL_INQUERY_STRING_LENGTH];
    UINT32 cursec = 0;

    //入参检查：不涉及到生死问题，参数也没啥大问题，故而不需要检查，都可以存入数据库表单中
    if (days <HSMMP_DATA_SAVE_DAYS_MIN) days = HSMMP_DATA_SAVE_DAYS_MIN;

	//建立连接
    sqlHandler = mysql_init(NULL);
    if(!sqlHandler)
    {
    	HcuErrorPrint("DBIHSMMP: MySQL init failed!\n");
        return FAILURE;
    }
    sqlHandler = mysql_real_connect(sqlHandler, zHcuSysEngPar.dbi.hcuDbHost, zHcuSysEngPar.dbi.hcuDbUser, zHcuSysEngPar.dbi.hcuDbPsw, zHcuSysEngPar.dbi.hcuDbName, zHcuSysEngPar.dbi.hcuDbPort, NULL, 0);  //unix_socket and clientflag not used.
    if (!sqlHandler){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIHSMMP: MySQL connection failed!\n");
        return FAILURE;
    }

	//删除满足条件的数据
    cursec = time(NULL);
    days = days * 24 * 3600;
    sprintf(strsql, "DELETE FROM `hcuhsmmpdatainfo` WHERE (%d - `timestamp` > '%d')", cursec, days);
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIHSMMP: INSET data error: %s\n", mysql_error(sqlHandler));
        return FAILURE;
	}

	//释放记录集
    mysql_close(sqlHandler);
    return SUCCESS;
}

