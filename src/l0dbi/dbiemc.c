/*
 * dbiemc.c
 *
 *  Created on: 2016年1月5日
 *      Author: test
 */

#include "../l0dbi/dbiemc.h"
#include "../l0service/trace.h"
#include "../l0dbi/dbicom.h"

//EMC数据表格式
// HcuEmcDataInfo
/*
CREATE TABLE IF NOT EXISTS `hcuemcdatainfo` (
  `sid` int(4) NOT NULL AUTO_INCREMENT,
  `deviceid` int(4) NOT NULL,
  `timestamp` int(4) NOT NULL,
  `dataformat` int(1) NOT NULL,
  `emcvalue` int(4) NOT NULL,
  `ew` char(1) NOT NULL,
  `gpsx` int(4) NOT NULL,
  `ns` char(1) NOT NULL,
  `gpsy` int(4) NOT NULL,
  `gpsz` int(4) NOT NULL,
  `onofflineflag` int(2) NOT NULL,
  PRIMARY KEY (`sid`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8;

INSERT INTO `hcudb`.`hcuemcdatainfo` (
`sid` ,
`deviceid` ,
`timestamp` ,
`dataformat`,
`emcvalue` ,
`ew` ,
`gpsx` ,
`ns` ,
`gpsy` ,
`gpsz` ,
`onofflineflag`
)
VALUES (
'4', '3', '2', '1', '4', 'n', '5', 'w', '6', '7', '8'
);

*/

//存储EMC数据，每一次存储，都是新增一条记录
//由于是本地数据库，这里不考虑网格问题，直接存储，简单处理
OPSTAT dbi_HcuEmcDataInfo_save(sensor_emc_data_element_t *emcData)
{
	MYSQL *sqlHandler;
    int result = 0;
    char strsql[DBI_MAX_SQL_INQUERY_STRING_LENGTH];

    //入参检查：不涉及到生死问题，参数也没啥大问题，故而不需要检查，都可以存入数据库表单中
    if (emcData == NULL){
    	HcuErrorPrint("DBIEMC: Input parameter NULL pointer!\n");
        return FAILURE;
    }

	//建立连接
    sqlHandler = mysql_init(NULL);
    if(!sqlHandler)
    {
    	HcuErrorPrint("DBIEMC: MySQL init failed!\n");
        return FAILURE;
    }
    sqlHandler = mysql_real_connect(sqlHandler, zHcuSysEngPar.dbi.hcuDbHost, zHcuSysEngPar.dbi.hcuDbUser, zHcuSysEngPar.dbi.hcuDbPsw, zHcuSysEngPar.dbi.hcuDbName, zHcuSysEngPar.dbi.hcuDbPort, NULL, 0);  //unix_socket and clientflag not used.
    if (!sqlHandler){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIEMC: MySQL connection failed!\n");
        return FAILURE;
    }


	HcuDebugPrint("hcuemcdatainfo: deviceid = %d, timestamp = %d, dataformat = %d, emcvalue = %d, ew = %c, gpsx = %d, ns = %c, gpsy = %d, gpsz = %d, onofflineflag = %d\n", emcData->equipid, emcData->timeStamp, emcData->dataFormat, emcData->emcValue, emcData->gps.ew, emcData->gps.gpsx, emcData->gps.ns, emcData->gps.gpsy, emcData->gps.gpsz, emcData->onOffLineFlag);


	//存入新的数据
    sprintf(strsql, "INSERT INTO `hcuemcdatainfo` (deviceid, timestamp, dataformat, emcvalue, ew, gpsx, ns, gpsy, gpsz, onofflineflag) VALUES \
    		('%d', '%d', '%d', '%d', '%c', '%d', '%c', '%d', '%d', '%d')", emcData->equipid, emcData->timeStamp, emcData->dataFormat, emcData->emcValue, emcData->gps.ew, emcData->gps.gpsx, emcData->gps.ns, emcData->gps.gpsy, emcData->gps.gpsz, emcData->onOffLineFlag);
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIEMC: INSERT data error: %s\n", mysql_error(sqlHandler));
        return FAILURE;
	}

	//释放记录集
    mysql_close(sqlHandler);
	if ((zHcuSysEngPar.debugMode & TRACE_DEBUG_NOR_ON) != FALSE){
		HcuDebugPrint("DBIEMC: EMC data record save to DB!\n");
	}
    return SUCCESS;
}

//查询满足条件的第一条记录
//deviceid是入参，指针为出参
OPSTAT dbi_HcuEmcDataInfo_inqury_1st_record(UINT32 deviceid, sensor_emc_data_element_t *emcData)
{
	MYSQL *sqlHandler;
	MYSQL_RES *resPtr;
	MYSQL_ROW sqlRow;
    int result = 0;
    char strsql[DBI_MAX_SQL_INQUERY_STRING_LENGTH];

    //入参检查：不涉及到生死问题，参数也没啥大问题，故而不需要检查，都可以存入数据库表单中
    if (emcData == NULL){
    	HcuErrorPrint("DBIEMC: Input parameter NULL pointer!\n");
        return FAILURE;
    }

	//建立数据库连接
    sqlHandler = mysql_init(NULL);
    if(!sqlHandler)
    {
    	HcuErrorPrint("DBIEMC: MySQL init failed!\n");
        return FAILURE;
    }
    sqlHandler = mysql_real_connect(sqlHandler, zHcuSysEngPar.dbi.hcuDbHost, zHcuSysEngPar.dbi.hcuDbUser, zHcuSysEngPar.dbi.hcuDbPsw, zHcuSysEngPar.dbi.hcuDbName, zHcuSysEngPar.dbi.hcuDbPort, NULL, 0);  //unix_socket and clientflag not used.
    if (!sqlHandler){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIEMC: MySQL connection failed!\n");
        return FAILURE;
    }

	//获取数据
    sprintf(strsql, "SELECT * FROM `hcuemcdatainfo` WHERE (`deviceid` = '%d')", deviceid);
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIEMC: INSET data error: %s\n", mysql_error(sqlHandler));
        return FAILURE;
	}

	//查具体的结果
	resPtr = mysql_use_result(sqlHandler);
	if (!resPtr){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIEMC: mysql_use_result error!\n");
        return FAILURE;
	}

	//只读取第一条记录
	if ((sqlRow = mysql_fetch_row(resPtr)) == NULL)
	{
		mysql_free_result(resPtr);
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIEMC: mysql_fetch_row NULL error!\n");
        return FAILURE;
	}
	else{
		UINT32 index = 2;
		if (sqlRow[index]) emcData->timeStamp = (UINT32)atol(sqlRow[index++]);
		if (sqlRow[index]) emcData->dataFormat = (UINT8)(atol(sqlRow[index++]) & 0xFF);
		if (sqlRow[index]) emcData->emcValue = (UINT32)atol(sqlRow[index++]);
		if (sqlRow[index]) emcData->gps.ew = (UINT32)atol(sqlRow[index++]);
		if (sqlRow[index]) emcData->gps.gpsx = (UINT32)atol(sqlRow[index++]);
		if (sqlRow[index]) emcData->gps.ns = (UINT32)atol(sqlRow[index++]);
		if (sqlRow[index]) emcData->gps.gpsy = (UINT32)atol(sqlRow[index++]);
		if (sqlRow[index]) emcData->gps.gpsz = (UINT32)atol(sqlRow[index++]);
		if (sqlRow[index]) emcData->onOffLineFlag = (UINT8)(atol(sqlRow[index++]) & 0xFF);
	}

	//释放记录集
	mysql_free_result(resPtr);
    mysql_close(sqlHandler);
    return SUCCESS;
}

//删除对应用户所有超过90天的数据
//缺省做成90天，如果参数错误，导致90天以内的数据强行删除，则不被认可
OPSTAT dbi_HcuEmcDataInfo_delete_3monold(UINT32 days)
{
	MYSQL *sqlHandler;
    int result = 0;
    char strsql[DBI_MAX_SQL_INQUERY_STRING_LENGTH];
    UINT32 cursec = 0;

    //入参检查：不涉及到生死问题，参数也没啥大问题，故而不需要检查，都可以存入数据库表单中
    if (days <EMC_DATA_SAVE_DAYS_MIN) days = EMC_DATA_SAVE_DAYS_MIN;

	//建立连接
    sqlHandler = mysql_init(NULL);
    if(!sqlHandler)
    {
    	HcuErrorPrint("DBIEMC: MySQL init failed!\n");
        return FAILURE;
    }
    sqlHandler = mysql_real_connect(sqlHandler, zHcuSysEngPar.dbi.hcuDbHost, zHcuSysEngPar.dbi.hcuDbUser, zHcuSysEngPar.dbi.hcuDbPsw, zHcuSysEngPar.dbi.hcuDbName, zHcuSysEngPar.dbi.hcuDbPort, NULL, 0);  //unix_socket and clientflag not used.
    if (!sqlHandler){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIEMC: MySQL connection failed!\n");
        return FAILURE;
    }

	//删除满足条件的数据
    cursec = time(NULL);
    days = days * 24 * 3600;
    sprintf(strsql, "DELETE FROM `hcuemcdatainfo` WHERE (%d - `timestamp` > '%d')", cursec, days);
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIEMC: INSET data error: %s\n", mysql_error(sqlHandler));
        return FAILURE;
	}

	//释放记录集
    mysql_close(sqlHandler);
    return SUCCESS;
}


/*
MYSQL *mysql_init(MYSQL *);
//这里称之为载入函数吧，返回的MYSQL指针要用到后续的函数中

int mysql_options(MYSQL *connection, enum option_to_set,const char *argument);
//设置MYSQL*的一些属性，比如超时时间等

MYSQL *mysql_real_connect(MYSQL *connection,
                const char *server_host,
                const char *sql_user_name,
                const char *sql_password,
                const char *db_name,
                unsigned int port_number,//置0连接默认端口，一般为3306
                const char *unix_socket_name,//NULL
                unsigned int flags);//无另外属性时置0
//连接函数

void mysql_close(MYSQL *connection);
//关闭连接

unsigned int mysql_errno(MYSQL *connection);
//返回错误代码

char *mysql_error(MYSQL *connection);

//返回错误信息

int mysql_query(MYSQL *connection, const char *query);
//执行sql语句

my_ulonglong mysql_affected_rows(MYSQL *connection);
//返回执行语句过后受影响的行数

MYSQL_RES *mysql_store_result(MYSQL *connection);
//返回执行结果，适用于数据量较小时

my_ulonglong mysql_num_rows(MYSQL_RES *result);
//返回上面函数返回结果的行数

MYSQL_ROW mysql_fetch_row(MYSQL_RES *result);
//抽取一条记录，返回NULL时表示抽取完记录或者错误

void mysql_data_seek(MYSQL_RES *result, my_ulonglong offset);
//调整数据位置，offset为0时，下次调用mysql_fetch_row将返回result第一条记录

MYSQL_ROW_OFFSET mysql_row_tell(MYSQL_RES *result);
//返回当前的位置

MYSQL_ROW_OFFSET mysql_row_seek(MYSQL_RES *result, MYSQL_ROW_OFFSET offset);
//移动数据位置，并返回先前的位置，可以和上一个函数结合使用

void mysql_free_result(MYSQL_RES *result);
//释放result空间

MYSQL_RES *mysql_use_result(MYSQL *connection);
//返回执行结果，适用于数据量较大时

unsigned int mysql_field_count(MYSQL *connection);
//返回查询结果中的列数（column数）

MYSQL_FIELD *mysql_fetch_field(MYSQL_RES *result);
//获得查询结果中的列名等信息（表头信息）
*/

//只是一个操作数据库不完善的例子，放在这里纯粹作为参考，未来可以删掉
OPSTAT dbi_HcuEmcDataInfo_save_example(sensor_emc_data_element_t *emcData)
{
	MYSQL *sqlHandler;
	MYSQL_RES *resPtr, *resPtrChild;
	MYSQL_ROW sqlRow;
    int result = 0;
    //int first_row = 1;
    UINT32 sid = 0;

    //入参检查

	//建立连接
    sqlHandler = mysql_init(NULL);
    if(!sqlHandler)
    {
    	HcuErrorPrint("DBIEMC: MySQL init failed!\n");
        return FAILURE;
    }
    sqlHandler = mysql_real_connect(sqlHandler, zHcuSysEngPar.dbi.hcuDbHost, zHcuSysEngPar.dbi.hcuDbUser, zHcuSysEngPar.dbi.hcuDbPsw, zHcuSysEngPar.dbi.hcuDbName, zHcuSysEngPar.dbi.hcuDbPort, NULL, 0);  //unix_socket and clientflag not used.
    if (!sqlHandler){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIEMC: MySQL connection failed!\n");
        return FAILURE;
    }

    //读取SID=0
    result = mysql_query(sqlHandler, "SELECT * FROM `hcuemcdatainfo` WHERE `sid` = '0'");
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIEMC: SELECT SID=0 error: %s\n", mysql_error(sqlHandler));
        return FAILURE;
	}
	resPtr = mysql_use_result(sqlHandler);
	if (!resPtr){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIEMC: mysql_use_result error!\n");
        return FAILURE;
	}

	//只取结果的第一行，原则上不能有SID相同的，按照表单本身的定义，相同的，是不能存成功的，如果有相同的，则需要维护人员的人工干预进行消除，程序不是万能的
	//SID=0的行号不存在
	//插入第一行SID=0，当然先遍历并找到最大的SID，然后将SID+1赋值
	if ((sqlRow = mysql_fetch_row(resPtr)) == NULL)
	{
		result = mysql_query(sqlHandler, "SELECT `sid` FROM `hcuemcdatainfo` WHERE 1");
		//如果是空的数据库，则SID直接赋值1
		if(result)
		{
			sid = 1;
		}
		//只要有元素，这个就不应该为1
		else
		{
			resPtrChild = mysql_use_result(sqlHandler);
			if (!resPtr)
			{
		    	mysql_close(sqlHandler);
		    	HcuErrorPrint("DBIEMC: mysql_use_result error!\n");
		        return FAILURE;
			}
			//循环找出所有结果中最大的
			//列中第二个数据域，必须为数字，强制判定保护，待完善
			sid = 0;
			while((sqlRow=mysql_fetch_row(resPtrChild)) != NULL)
			{
				if ((UINT32)sqlRow[1] > sid) sid = (UINT32)sqlRow[1];
			}
			sid = sid + 1;
			mysql_free_result(resPtrChild);
		}//if(result)
		//插入一个SID=0的记录
		result = mysql_query(sqlHandler, "INSERT INTO `hcuemcdatainfo` (sid, deviceid, timestamp, emcvalue, gpsx, gpsy, gpsz,\
				onofflineflag) VALUES (0, 'sid', '', '', '','', '','')");
		if(result)
		{
	    	mysql_close(sqlHandler);
	    	HcuErrorPrint("DBIEMC: INSET SID=0 error: %s\n", mysql_error(sqlHandler));
	        return FAILURE;
		}
	}//if ((sqlRow = mysql_fetch_row(resPtr)) == NULL)

	//SID=0的行号存在
	//获取SID的值，准备插入数值
	else
	{
		//列中第二个数据域，必须为数字，强制判定保护，待完善
		sid = (UINT32)sqlRow[1];
		HcuDebugPrint("DBIEMC: sid = %d", sid);
	}//if ((sqlRow = mysql_fetch_row(resPtr)) == NULL)

	//存入新的数据
	result = mysql_query(sqlHandler, "INSERT INTO `hcuemcdatainfo` (deviceid, timestamp, emcvalue, gpsx, gpsy, gpsz,\
			onofflineflag) VALUES ('deviceid', 'timestamp', 'emc', 'gpsx', 'gpsy', 'gpsz', 'onofflineflag')");
	if(result)
	{
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIEMC: INSET data error: %s\n", mysql_error(sqlHandler));
        return FAILURE;
	}

	//更新SID=0的记录
	sid = sid+1;
	result = mysql_query(sqlHandler, "UPDATE `hcuemcdatainfo` SET `deviceid` = sid WHERE (`sid` = 0)");
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIEMC: UPDATE SID error: %s\n", mysql_error(sqlHandler));
        return FAILURE;
	}

	//释放记录集
	mysql_free_result(resPtr);
    mysql_close(sqlHandler);
    return SUCCESS;

	//从后台CLOUD@PHP程序中拷贝而来，待修改
	//入参检查
	//建立连接
	/*$mysqli=new mysqli(WX_DBHOST, WX_DBUSER, WX_DBPSW, WX_DBNAME, WX_DBPORT);
	if (!$mysqli)
	{
		die('Could not connect: ' . mysqli_error($mysqli));
	}
	//找到数据库中已有序号最大的，也许会出现序号(6 BYTE)用满的情况，这时应该考虑更新该算法，短期内不需要考虑这么复杂的情况
	//数据库SID=0的记录保留作为特殊用途，对应的longitude字段保存当前最大可用sid
	$result = $mysqli->query("SELECT * FROM `hcuemcdatainfo` WHERE `sid` = '0'");
	if ($result->num_rows>0)
	{
		$row = $result->fetch_array();
		$sid = intval($row['longitude']); //记录中存储着最大的SID
	}
	else //如果没有sid＝0记录项,找到当前最大sid并插入一条sid＝0记录项，其longitude字段存入sid＋1
	{
		$result = $mysqli->query("SELECT `sid` FROM `emcdatainfo` WHERE 1");
		$sid =0;
		while($row = $result->fetch_array())
		{
			if ($row['sid'] > $sid)
			{
				$sid = $row['sid'];
			}
		}
		$sid = $sid+1;
		$result = $mysqli->query("INSERT INTO `emcdatainfo` (sid, wxuser, deviceid, date, hourminindex, emcvalue, latitude, longitude)
	  VALUES ('0', '', '', '', '','', '','$sid')");
	}

	//存储新记录，如果发现是已经存在的数据，则覆盖，否则新增
	$date = intval(date("ymd", $timestamp));
	$stamp = getdate($timestamp);
	$hourminindex = intval(($stamp["hours"] * 60 + floor($stamp["minutes"]/TIME_GRID_SIZE)));

	$result = $mysqli->query("SELECT `sid` FROM `emcdatainfo` WHERE ((`wxuser` = '$user' AND `deviceid` =
	  '$deviceid') AND (`date` = '$date' AND `hourminindex` = '$hourminindex'))");
	if (($result->num_rows)>0)   //重复，则覆盖
	{
		$result=$mysqli->query("UPDATE `emcdatainfo` SET `emcvalue` = '$emc'
		  WHERE ((`wxuser` = '$user' AND `deviceid` = '$deviceid') AND (`date` = '$date' AND `hourminindex` = '$hourminindex'))");
	}
	else   //不存在，新增
	{
		$res1=$mysqli->query("INSERT INTO `emcdatainfo` (sid, wxuser, deviceid, date, hourminindex, emcvalue, latitude, longitude)
	  VALUES ('$sid', '$user', '$deviceid', '$date', '$hourminindex','$emc', '', '')");
		//更新最大可用的sid到数据库SID=0的记录项
		$sid = $sid + 1;
		$res2=$mysqli->query("UPDATE `emcdatainfo` SET `longitude` = '$sid' WHERE (`sid` = '0')");
		$result = $res1 AND $res2;
	}
	$mysqli->close();
	return $result;*/
}

//SQLite3函数例子
static int sqlite3_dbi_HcuEmcDataInfo_save_callback(void *NotUsed, int argc, char **argv, char **azColName){
   int i;
   for(i=0; i < argc; i++){
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
}

int sqlite3_dbi_HcuEmcDataInfo_save(sensor_emc_data_element_t *emcData)
{
   sqlite3 *db;
   char *zErrMsg = 0;
   int rc;
   char strsql[DBI_MAX_SQL_INQUERY_STRING_LENGTH];

   /* Open database */
   rc = sqlite3_open(HCU_DB_SQLITE3_LOC_AND_FILE, &db);
   if( rc ){
      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
      exit(0);
   }else{
      fprintf(stderr, "Opened database successfully\n");
   }

   /* Create SQL statement */
   sprintf(strsql, "INSERT INTO `hcuemcdatainfo` (deviceid, timestamp, dataformat, emcvalue, gpsx, gpsy, gpsz, onofflineflag) VALUES \
   		('%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d')", emcData->equipid, emcData->timeStamp, emcData->dataFormat, emcData->emcValue, emcData->gps.gpsx, emcData->gps.gpsy, emcData->gps.gpsz, emcData->onOffLineFlag);

   /* Execute SQL statement */
   rc = sqlite3_exec(db, strsql, sqlite3_dbi_HcuEmcDataInfo_save_callback, 0, &zErrMsg);
   if( rc != SQLITE_OK ){
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   }else{
      fprintf(stdout, "Records created successfully\n");
   }
   sqlite3_close(db);
   return 0;
}

