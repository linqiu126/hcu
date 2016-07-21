/*
 * dbinbiot.c
 *
 *  Created on: 2016年7月20日
 *      Author: hitpony
 */
#include "../l0dbi/dbinbiot.h"
#include "../l0service/trace.h"
#include "../l0comvm/sysengpar.h"

/*

--
-- 表的结构 `hcu_nbiot_qg376_context`
--

CREATE TABLE IF NOT EXISTS `hcu_nbiot_qg376_context` (
  `sid` int(4) NOT NULL AUTO_INCREMENT,
  `ipmaddress` int(4) NOT NULL,
  `cntpfc` int(1) NOT NULL,
  `deviceflag` int(1) NOT NULL,
  PRIMARY KEY (`sid`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 AUTO_INCREMENT=1 ;

-- 转存表中的数据 `hcu_nbiot_qg376_context`
--

INSERT INTO `hcu_nbiot_qg376_context` (`sid`, `ipmaddress`, `cntpfc`, `deviceflag`) VALUES (1, 111, 17, 1);


--
-- 表的结构 `hcu_nbiot_cj188_context`
--

CREATE TABLE IF NOT EXISTS `hcu_nbiot_cj188_context` (
  `sid` int(4) NOT NULL AUTO_INCREMENT,
  `cj188address` char(14) NOT NULL,
  `cntser` int(1) NOT NULL,
  `deviceflag` int(1) NOT NULL,
  PRIMARY KEY (`sid`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 AUTO_INCREMENT=1 ;

-- 转存表中的数据 `hcu_nbiot_cj188_context`
--

INSERT INTO `hcu_nbiot_cj188_context` (`sid`, `cj188address`, `cntser`, `deviceflag`) VALUES (1, 111, 17, 1);


*/

//存入CJ188的CNT/SER数据结果
OPSTAT dbi_NbiotCJ188CntSerContext_save(char *add, INT8 cntser, INT8 flag)
{
	MYSQL *sqlHandler;
	MYSQL_RES *resPtr;
	MYSQL_ROW sqlRow;
    int result = 0;
    char strsql[DBI_MAX_SQL_INQUERY_STRING_LENGTH];

    //入参检查：不涉及到生死问题，参数也没啥大问题，故而不需要检查，都可以存入数据库表单中
    if (add == NULL){
    	HcuErrorPrint("DBICOM: Input parameter NULL pointer!\n");
        return FAILURE;
    }

	//建立连接
    sqlHandler = mysql_init(NULL);
    if(!sqlHandler)
    {
    	HcuErrorPrint("DBICOM: MySQL init failed!\n");
        return FAILURE;
    }
    sqlHandler = mysql_real_connect(sqlHandler, zHcuSysEngPar.dbi.hcuDbHost, zHcuSysEngPar.dbi.hcuDbUser, zHcuSysEngPar.dbi.hcuDbPsw, zHcuSysEngPar.dbi.hcuDbName, zHcuSysEngPar.dbi.hcuDbPort, NULL, 0);  //unix_socket and clientflag not used.
    if (!sqlHandler){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBICOM: MySQL connection failed!\n");
        return FAILURE;
    }

    //查询地址是否存在
    sprintf(strsql, "SELECT * FROM `hcu_nbiot_cj188_context` WHERE (`cj188address` = '%s')", add);
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBICOM: SELECT data error: %s\n", mysql_error(sqlHandler));
        return FAILURE;
	}

	//查具体的结果
	resPtr = mysql_use_result(sqlHandler);
	if (!resPtr){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBICOM: mysql_use_result error!\n");
        return FAILURE;
	}

	//只读取第一条记录
	if ((sqlRow = mysql_fetch_row(resPtr)) == NULL)
	{
		//Insert
		sprintf(strsql, "INSERT INTO `hcu_nbiot_cj188_context` (cj188address, cntser, deviceflag) VALUES ('%s','%d','%d')", add, cntser, flag);
		result = mysql_query(sqlHandler, strsql);
		if(result){HcuErrorPrint("DBICOM: INSERT data error: %s\n", mysql_error(sqlHandler));}
	}
	else{
		//Update
		sprintf(strsql, "UPDATE `hcu_nbiot_cj188_context` SET  `cntser` = '%d',`deviceflag` = '%d' WHERE (`cj188address` = '%s')", cntser, flag, add);
		result = mysql_query(sqlHandler, strsql);
		if(result){HcuErrorPrint("DBICOM: UPDATE data error: %s\n", mysql_error(sqlHandler)); }
	}

	//释放记录集
	mysql_free_result(resPtr);
    mysql_close(sqlHandler);
    return SUCCESS;
}


//先只放一个返回值
OPSTAT dbi_NbiotCJ188CntSerContext_ser_inqury(char *add, INT8 cntser)
{
	MYSQL *sqlHandler;
	MYSQL_RES *resPtr;
	MYSQL_ROW sqlRow;
    int result = 0;
    char strsql[DBI_MAX_SQL_INQUERY_STRING_LENGTH];

    //入参检查：不涉及到生死问题，参数也没啥大问题，故而不需要检查，都可以存入数据库表单中
    if (add == NULL){
    	HcuErrorPrint("DBICOM: Input parameter NULL pointer!\n");
        return FAILURE;
    }

	//建立连接
    sqlHandler = mysql_init(NULL);
    if(!sqlHandler)
    {
    	HcuErrorPrint("DBICOM: MySQL init failed!\n");
        return FAILURE;
    }
    sqlHandler = mysql_real_connect(sqlHandler, zHcuSysEngPar.dbi.hcuDbHost, zHcuSysEngPar.dbi.hcuDbUser, zHcuSysEngPar.dbi.hcuDbPsw, zHcuSysEngPar.dbi.hcuDbName, zHcuSysEngPar.dbi.hcuDbPort, NULL, 0);  //unix_socket and clientflag not used.
    if (!sqlHandler){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBICOM: MySQL connection failed!\n");
        return FAILURE;
    }

    //查询地址是否存在
    sprintf(strsql, "SELECT * FROM `hcu_nbiot_cj188_context` WHERE (`cj188address` = '%s')", add);
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBICOM: SELECT data error: %s\n", mysql_error(sqlHandler));
        return FAILURE;
	}

	//查具体的结果
	resPtr = mysql_use_result(sqlHandler);
	if (!resPtr){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBICOM: mysql_use_result error!\n");
        return FAILURE;
	}

	//只读取第一条记录
	if ((sqlRow = mysql_fetch_row(resPtr)) == NULL)
	{
    	HcuErrorPrint("DBICOM: mysql_fetch_row NULL error!\n");
	}
	else{
		//fetch
		if (sqlRow[2]) cntser = (INT8)atol(sqlRow[2]);
	}

	//释放记录集
	mysql_free_result(resPtr);
    mysql_close(sqlHandler);
    return SUCCESS;
}


//先COUNTER增加1
OPSTAT dbi_NbiotCJ188CntSerContext_ser_increase(char *add)
{
	MYSQL *sqlHandler;
	MYSQL_RES *resPtr;
	MYSQL_ROW sqlRow;
    int result = 0;
    char strsql[DBI_MAX_SQL_INQUERY_STRING_LENGTH];

    //入参检查：不涉及到生死问题，参数也没啥大问题，故而不需要检查，都可以存入数据库表单中
    if (add == NULL){
    	HcuErrorPrint("DBICOM: Input parameter NULL pointer!\n");
        return FAILURE;
    }

	//建立连接
    sqlHandler = mysql_init(NULL);
    if(!sqlHandler)
    {
    	HcuErrorPrint("DBICOM: MySQL init failed!\n");
        return FAILURE;
    }
    sqlHandler = mysql_real_connect(sqlHandler, zHcuSysEngPar.dbi.hcuDbHost, zHcuSysEngPar.dbi.hcuDbUser, zHcuSysEngPar.dbi.hcuDbPsw, zHcuSysEngPar.dbi.hcuDbName, zHcuSysEngPar.dbi.hcuDbPort, NULL, 0);  //unix_socket and clientflag not used.
    if (!sqlHandler){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBICOM: MySQL connection failed!\n");
        return FAILURE;
    }

    //查询地址是否存在
    sprintf(strsql, "SELECT * FROM `hcu_nbiot_cj188_context` WHERE (`cj188address` = '%s')", add);
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBICOM: SELECT data error: %s\n", mysql_error(sqlHandler));
        return FAILURE;
	}

	//查具体的结果
	resPtr = mysql_use_result(sqlHandler);
	if (!resPtr){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBICOM: mysql_use_result error!\n");
        return FAILURE;
	}

	//只读取第一条记录
	if ((sqlRow = mysql_fetch_row(resPtr)) == NULL)
	{
		//Insert
		sprintf(strsql, "INSERT INTO `hcu_nbiot_cj188_context` (cj188address, cntser) VALUES ('%s', 0)", add);
		result = mysql_query(sqlHandler, strsql);
		if(result){HcuErrorPrint("DBICOM: INSERT data error: %s\n", mysql_error(sqlHandler)); }
	}
	else{
		//fetch
		INT8 cntser = 0;
		if (sqlRow[2]) cntser = (INT8)atol(sqlRow[2]);
		cntser = (cntser + 1) % 256;
		//Update
		sprintf(strsql, "UPDATE `hcu_nbiot_cj188_context` SET  `cntser` = '%d' WHERE (`cj188address` = '%s')", cntser, add);
		result = mysql_query(sqlHandler, strsql);
		if(result){HcuErrorPrint("DBICOM: UPDATE data error: %s\n", mysql_error(sqlHandler)); }
	}

	//释放记录集
	mysql_free_result(resPtr);
    mysql_close(sqlHandler);
    return SUCCESS;
}

//存入QG376的CNT/PCF数据结果
OPSTAT dbi_NbiotQG376CntPcfContext_save(char *add, INT8 cntpcf, INT8 flag)
{
	MYSQL *sqlHandler;
	MYSQL_RES *resPtr;
	MYSQL_ROW sqlRow;
    int result = 0;
    char strsql[DBI_MAX_SQL_INQUERY_STRING_LENGTH];

    //入参检查：不涉及到生死问题，参数也没啥大问题，故而不需要检查，都可以存入数据库表单中
    if (add == NULL){
    	HcuErrorPrint("DBICOM: Input parameter NULL pointer!\n");
        return FAILURE;
    }

	//建立连接
    sqlHandler = mysql_init(NULL);
    if(!sqlHandler)
    {
    	HcuErrorPrint("DBICOM: MySQL init failed!\n");
        return FAILURE;
    }
    sqlHandler = mysql_real_connect(sqlHandler, zHcuSysEngPar.dbi.hcuDbHost, zHcuSysEngPar.dbi.hcuDbUser, zHcuSysEngPar.dbi.hcuDbPsw, zHcuSysEngPar.dbi.hcuDbName, zHcuSysEngPar.dbi.hcuDbPort, NULL, 0);  //unix_socket and clientflag not used.
    if (!sqlHandler){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBICOM: MySQL connection failed!\n");
        return FAILURE;
    }

    //查询地址是否存在
    sprintf(strsql, "SELECT * FROM `hcu_nbiot_qg376_context` WHERE (`ipmaddress` = '%s')", add);
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBICOM: SELECT data error: %s\n", mysql_error(sqlHandler));
        return FAILURE;
	}

	//查具体的结果
	resPtr = mysql_use_result(sqlHandler);
	if (!resPtr){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBICOM: mysql_use_result error!\n");
        return FAILURE;
	}

	//只读取第一条记录
	if ((sqlRow = mysql_fetch_row(resPtr)) == NULL)
	{
		//Insert
		sprintf(strsql, "INSERT INTO `hcu_nbiot_qg376_context` (ipmaddress, cntpcf, deviceflag) VALUES ('%s','%d','%d')", add, cntpcf, flag);
		result = mysql_query(sqlHandler, strsql);
		if(result){HcuErrorPrint("DBICOM: INSERT data error: %s\n", mysql_error(sqlHandler));}
	}
	else{
		//Update
		sprintf(strsql, "UPDATE `hcu_nbiot_qg376_context` SET  `cntpcf` = '%d',`deviceflag` = '%d' WHERE (`ipmaddress` = '%s')", cntpcf, flag, add);
		result = mysql_query(sqlHandler, strsql);
		if(result){HcuErrorPrint("DBICOM: UPDATE data error: %s\n", mysql_error(sqlHandler)); }
	}

	//释放记录集
	mysql_free_result(resPtr);
    mysql_close(sqlHandler);
    return SUCCESS;
}


//先只放一个返回值
OPSTAT dbi_NbiotQG376CntPcfContext_ser_inqury(char *add, INT8 cntpcf)
{
	MYSQL *sqlHandler;
	MYSQL_RES *resPtr;
	MYSQL_ROW sqlRow;
    int result = 0;
    char strsql[DBI_MAX_SQL_INQUERY_STRING_LENGTH];

    //入参检查：不涉及到生死问题，参数也没啥大问题，故而不需要检查，都可以存入数据库表单中
    if (add == NULL){
    	HcuErrorPrint("DBICOM: Input parameter NULL pointer!\n");
        return FAILURE;
    }

	//建立连接
    sqlHandler = mysql_init(NULL);
    if(!sqlHandler)
    {
    	HcuErrorPrint("DBICOM: MySQL init failed!\n");
        return FAILURE;
    }
    sqlHandler = mysql_real_connect(sqlHandler, zHcuSysEngPar.dbi.hcuDbHost, zHcuSysEngPar.dbi.hcuDbUser, zHcuSysEngPar.dbi.hcuDbPsw, zHcuSysEngPar.dbi.hcuDbName, zHcuSysEngPar.dbi.hcuDbPort, NULL, 0);  //unix_socket and clientflag not used.
    if (!sqlHandler){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBICOM: MySQL connection failed!\n");
        return FAILURE;
    }

    //查询地址是否存在
    sprintf(strsql, "SELECT * FROM `hcu_nbiot_qg376_context` WHERE (`ipmaddress` = '%s')", add);
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBICOM: SELECT data error: %s\n", mysql_error(sqlHandler));
        return FAILURE;
	}

	//查具体的结果
	resPtr = mysql_use_result(sqlHandler);
	if (!resPtr){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBICOM: mysql_use_result error!\n");
        return FAILURE;
	}

	//只读取第一条记录
	if ((sqlRow = mysql_fetch_row(resPtr)) == NULL)
	{
    	HcuErrorPrint("DBICOM: mysql_fetch_row NULL error!\n");
	}
	else{
		//fetch
		if (sqlRow[2]) cntpcf = (INT8)atol(sqlRow[2]);
	}

	//释放记录集
	mysql_free_result(resPtr);
    mysql_close(sqlHandler);
    return SUCCESS;
}


//先COUNTER增加1
OPSTAT dbi_NbiotQG376CntPcfContext_ser_increase(char *add)
{
	MYSQL *sqlHandler;
	MYSQL_RES *resPtr;
	MYSQL_ROW sqlRow;
    int result = 0;
    char strsql[DBI_MAX_SQL_INQUERY_STRING_LENGTH];

    //入参检查：不涉及到生死问题，参数也没啥大问题，故而不需要检查，都可以存入数据库表单中
    if (add == NULL){
    	HcuErrorPrint("DBICOM: Input parameter NULL pointer!\n");
        return FAILURE;
    }

	//建立连接
    sqlHandler = mysql_init(NULL);
    if(!sqlHandler)
    {
    	HcuErrorPrint("DBICOM: MySQL init failed!\n");
        return FAILURE;
    }
    sqlHandler = mysql_real_connect(sqlHandler, zHcuSysEngPar.dbi.hcuDbHost, zHcuSysEngPar.dbi.hcuDbUser, zHcuSysEngPar.dbi.hcuDbPsw, zHcuSysEngPar.dbi.hcuDbName, zHcuSysEngPar.dbi.hcuDbPort, NULL, 0);  //unix_socket and clientflag not used.
    if (!sqlHandler){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBICOM: MySQL connection failed!\n");
        return FAILURE;
    }

    //查询地址是否存在
    sprintf(strsql, "SELECT * FROM `hcu_nbiot_qg376_context` WHERE (`ipmaddress` = '%s')", add);
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBICOM: SELECT data error: %s\n", mysql_error(sqlHandler));
        return FAILURE;
	}

	//查具体的结果
	resPtr = mysql_use_result(sqlHandler);
	if (!resPtr){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBICOM: mysql_use_result error!\n");
        return FAILURE;
	}

	//只读取第一条记录
	if ((sqlRow = mysql_fetch_row(resPtr)) == NULL)
	{
		//Insert
		sprintf(strsql, "INSERT INTO `hcu_nbiot_qg376_context` (ipmaddress, cntpcf) VALUES ('%s', 0)", add);
		result = mysql_query(sqlHandler, strsql);
		if(result){HcuErrorPrint("DBICOM: INSERT data error: %s\n", mysql_error(sqlHandler)); }
	}
	else{
		//fetch
		INT8 cntpcf = 0;
		if (sqlRow[2]) cntpcf = (INT8)atol(sqlRow[2]);
		cntpcf = (cntpcf + 1) % 256;
		//Update
		sprintf(strsql, "UPDATE `hcu_nbiot_qg376_context` SET  `cntpcf` = '%d' WHERE (`ipmaddress` = '%s')", cntpcf, add);
		result = mysql_query(sqlHandler, strsql);
		if(result){HcuErrorPrint("DBICOM: UPDATE data error: %s\n", mysql_error(sqlHandler)); }
	}

	//释放记录集
	mysql_free_result(resPtr);
    mysql_close(sqlHandler);
    return SUCCESS;
}
