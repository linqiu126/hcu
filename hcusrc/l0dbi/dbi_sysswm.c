/*
 * dbi_sysswm.c
 *
 *  Created on: 2016年7月20日
 *      Author: hitpony
 */

#include "../l0dbi/dbi_sysswm.h"
#include "../l0service/trace.h"
#include "../l0dbi/dbicom.h"


/*

-- --------------------------------------------------------
--
-- Table structure for table `hcusysswm_swpkg`
--

CREATE TABLE `hcusysswm_swpkg` (
  `sid` int(4) NOT NULL AUTO_INCREMENT,
  `equentry` int(1) NOT NULL DEFAULT '1',
  `hwtype` int(2) NOT NULL DEFAULT '0',
  `hwpem` int(2) NOT NULL DEFAULT '0',
  `swrel` int(2) NOT NULL DEFAULT '0',
  `swver` int(2) NOT NULL DEFAULT '0',
  `dbver` int(2) NOT NULL DEFAULT '0',
  `upgradeflag` int(1) NOT NULL DEFAULT '4',
  `swtotallen` int(4) NOT NULL DEFAULT '0',
  `swchecksum` int(2) NOT NULL DEFAULT '0',
  `dbtotallen` int(4) NOT NULL DEFAULT '0',
  `dbchecksum` int(2) NOT NULL DEFAULT '0',
  `filename` varchar(60) DEFAULT NULL,
  `dbname` varchar(60) DEFAULT NULL,
  `currentactive` char(10) NOT NULL,
  `updatetime` int(4) NOT NULL DEFAULT '0',
  PRIMARY KEY (`sid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;



-- --------------------------------------------------------
--
-- Table structure for table `hcusysswm_swdl`
--
CREATE TABLE `hcusysswm_swdl` (
  `sid` int(4) NOT NULL AUTO_INCREMENT,
  `equentry` int(1) NOT NULL DEFAULT '1',
  `hwtype` int(2) NOT NULL DEFAULT '0',
  `hwpem` int(2) NOT NULL DEFAULT '0',
  `swrel` int(2) NOT NULL DEFAULT '0',
  `swver` int(2) NOT NULL DEFAULT '0',
  `upgradeflag` int(1) NOT NULL DEFAULT '4',
  `totallen` int(4) NOT NULL DEFAULT '0',
  `checksum` int(2) NOT NULL DEFAULT '0',
  `nodeid` int(1) NOT NULL DEFAULT '0',
  `segtotal` int(2) NOT NULL DEFAULT '0',
  `segindex` int(2) NOT NULL DEFAULT '0',
  `segsplitlen` int(2) NOT NULL DEFAULT '0',
  `segvalidlen` int(2) NOT NULL DEFAULT '0',
  `segcksum` int(2) NOT NULL DEFAULT '0',
  `filename` varchar(60) DEFAULT NULL,
  `dltime` int(4) NOT NULL DEFAULT '0',
  PRIMARY KEY (`sid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;



*/

//
OPSTAT dbi_HcuSysSwm_SwPkg_save(HcuSysMsgIeL3SysSwmSwPkgElement_t *ptrSwPkg)
{
	MYSQL *sqlHandler;
	//MYSQL_RES *resPtr;
	//MYSQL_ROW sqlRow;
    int result = 0;
    char strsql[DBI_MAX_SQL_INQUERY_STRING_LENGTH];

    //入参检查：不涉及到生死问题，参数也没啥大问题，故而不需要检查，都可以存入数据库表单中
    if (ptrSwPkg == NULL){
    	HcuErrorPrint("DBISYSSWM: Null input parameter\n");
        return FAILURE;
    }

	//建立数据库连接
    sqlHandler = mysql_init(NULL);
    if(!sqlHandler)
    {
    	HcuErrorPrint("DBICOM: MySQL init failed!\n");
        return FAILURE;
    }
    sqlHandler = mysql_real_connect(sqlHandler, HCU_SYSCFG_LOCAL_DB_HOST_DEFAULT, HCU_SYSCFG_LOCAL_DB_USER_DEFAULT, HCU_SYSCFG_LOCAL_DB_PSW_DEFAULT, HCU_SYSCFG_LOCAL_DB_NAME_DEFAULT, HCU_SYSCFG_LOCAL_DB_PORT_DEFAULT, NULL, 0);  //unix_socket and clientflag not used.
    if (!sqlHandler){
    	HcuErrorPrint("DBISYSSWM: MySQL connection failed, Err Code = %s!\n", mysql_error(sqlHandler));
    	mysql_close(sqlHandler);
        return FAILURE;
    }

    //先删除旧数据
	sprintf(strsql, "DELETE from `hcusysswm_swpkg` WHERE (`equentry` = '%d' AND `hwtype` = '%d' AND `hwpem` = '%d' AND `swrel` = '%d' AND `swver` = '%d' AND `upgradeflag` = '%d')",\
			ptrSwPkg->equEntry, ptrSwPkg->hwType, ptrSwPkg->hwPem, ptrSwPkg->swRel, ptrSwPkg->swVer, ptrSwPkg->upgradeFlag);
	result = mysql_query(sqlHandler, strsql);
	if(result){
	 	mysql_close(sqlHandler);
	 	HcuErrorPrint("DBISYSSWM: DELETE data error, err cause = %s\n", mysql_error(sqlHandler));
	    return FAILURE;
	}

	//插入INSERT新的数据，确保记录是唯一的
    sprintf(strsql, "REPLACE INTO `hcusysswm_swpkg` (equentry, hwtype, hwpem, swrel, swver, dbver, upgradeflag, swtotallen, swchecksum, dbtotallen, dbchecksum, filename, dbname, currentactive, updatetime) VALUES \
    		('%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%s', '%s', '%s', '%d')", ptrSwPkg->equEntry, ptrSwPkg->hwType, ptrSwPkg->hwPem, ptrSwPkg->swRel, ptrSwPkg->swVer, ptrSwPkg->dbVer, \
			ptrSwPkg->upgradeFlag, ptrSwPkg->swTotalLen, ptrSwPkg->swCksum, ptrSwPkg->dbTotalLen, ptrSwPkg->dbCksum, ptrSwPkg->fileName, ptrSwPkg->dbName, ptrSwPkg->currentActive, ptrSwPkg->updateTime);
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBISYSSWM: REPLACE data error, err cause = %s\n", mysql_error(sqlHandler));
        return FAILURE;
	}

/*	//获取数据
    sprintf(strsql, "SELECT * FROM `hcusysswm_swpkg` WHERE (`equentry` = '%d' AND `hwtype` = '%d' AND `hwpem` = '%d' AND `swrel` = '%d' AND `swver` = '%d' AND `upgradeflag` = '%d')",\
    		ptrSwPkg->equEntry, ptrSwPkg->hwType, ptrSwPkg->hwPem, ptrSwPkg->swRel, ptrSwPkg->swVer, ptrSwPkg->upgradeFlag);
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBISYSSWM: Inquery hcusysswm_swpkg error: %s\n", mysql_error(sqlHandler));
        return FAILURE;
	}

	//查具体的结果
	resPtr = mysql_use_result(sqlHandler);
	if (!resPtr){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBISYSSWM: mysql_use_result error!\n");
        return FAILURE;
	}

	//只读取第一条记录
	if ((sqlRow = mysql_fetch_row(resPtr)) == NULL)
	{
		//插入INSERT新的数据，确保记录是唯一的
	    sprintf(strsql, "INSERT INTO `hcusysswm_swpkg` (equentry, hwtype, hwpem, swrel, swver, dbver, upgradeflag, swtotallen, swchecksum, dbtotallen, dbhecksum, filename, dbname, currentactive, updatetime) VALUES \
	    		('%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%s', '%s', ''%s', '%d')", ptrSwPkg->equEntry, ptrSwPkg->hwType, ptrSwPkg->hwPem, ptrSwPkg->swRel, ptrSwPkg->swVer, ptrSwPkg->dbVer, \
				ptrSwPkg->upgradeFlag, ptrSwPkg->swTotalLen, ptrSwPkg->swCksum, ptrSwPkg->dbTotalLen, ptrSwPkg->dbCksum, ptrSwPkg->fileName, ptrSwPkg->dbName, ptrSwPkg->currentActive, ptrSwPkg->updateTime);
		result = mysql_query(sqlHandler, strsql);
		if(result){
			mysql_free_result(resPtr);
	    	mysql_close(sqlHandler);
	    	HcuErrorPrint("DBISYSSWM: INSERT data error, err cause = %s\n", mysql_error(sqlHandler));
	        return FAILURE;
		}
	}else{
		//UPDATE旧的数据
		sprintf(strsql, "UPDATE `hcusysswm_swpkg` SET swtotallen = '%d', swchecksum = '%d', dbtotallen = '%d', dbchecksum = '%d', filename = '%s', dbname = '%s', currentactive = '%s' , updatetime = '%d' WHERE (`equentry` = '%d' AND `hwtype` = '%d' AND `hwpem` = '%d' AND `swrel` = '%d' AND `swver` = '%d' AND `upgradeflag` = '%d')",\
				ptrSwPkg->swTotalLen, ptrSwPkg->swCksum, ptrSwPkg->dbTotalLen, ptrSwPkg->dbCksum, ptrSwPkg->fileName, ptrSwPkg->dbName, ptrSwPkg->currentActive, ptrSwPkg->updateTime, ptrSwPkg->equEntry, ptrSwPkg->hwType, ptrSwPkg->hwPem, ptrSwPkg->swRel, ptrSwPkg->swVer, ptrSwPkg->upgradeFlag);
		result = mysql_query(sqlHandler, strsql);
		if(result){
			mysql_free_result(resPtr);
		 	mysql_close(sqlHandler);
		 	HcuErrorPrint("DBISYSSWM: UPDATE data error, err cause = %s\n", mysql_error(sqlHandler));
		    return FAILURE;
		}
	}*/

	//	//DELETE所有旧的数据
	//    sprintf(strsql, "DELETE FROM `hcusysswm_swpkg` WHERE (`equentry` = '%d' AND `hwtype` = '%d' AND `hwpem` = '%d' AND `swrel` = '%d' AND `swver` = '%d' AND `upgradeflag` = '%d')", ptrSwPkg->equEntry, ptrSwPkg->hwType, ptrSwPkg->hwPem, ptrSwPkg->swRel, ptrSwPkg->swVer, ptrSwPkg->upgradeFlag);
	//	result = mysql_query(sqlHandler, strsql);
	//	if(result){
	//		mysql_free_result(resPtr);
	//    	mysql_close(sqlHandler);
	//    	HcuErrorPrint("DBISYSSWM: DELETE data error, err cause = %s\n", mysql_error(sqlHandler));
	//        return FAILURE;
	//	}

	//释放记录集：不再需要Free resPtr
    mysql_close(sqlHandler);
    return SUCCESS;
}

OPSTAT dbi_HcuSysSwm_SwPkg_inquery_to_judge_existance(UINT8 equEntry, UINT16 hwType, UINT16 hwPem, UINT16 swRel, UINT16 swVer, UINT8 upgradeFlag, INT32 *sid)
{
	MYSQL *sqlHandler;
	MYSQL_RES *resPtr;
	MYSQL_ROW sqlRow;
    int result = 0;
    char strsql[DBI_MAX_SQL_INQUERY_STRING_LENGTH];

    //入参检查：不涉及到生死问题，参数也没啥大问题，故而不需要检查，都可以存入数据库表单中
    *sid = -1;

	//建立数据库连接
    sqlHandler = mysql_init(NULL);
    if(!sqlHandler)
    {
    	HcuErrorPrint("DBICOM: MySQL init failed!\n");
        return FAILURE;
    }
    sqlHandler = mysql_real_connect(sqlHandler, HCU_SYSCFG_LOCAL_DB_HOST_DEFAULT, HCU_SYSCFG_LOCAL_DB_USER_DEFAULT, HCU_SYSCFG_LOCAL_DB_PSW_DEFAULT, HCU_SYSCFG_LOCAL_DB_NAME_DEFAULT, HCU_SYSCFG_LOCAL_DB_PORT_DEFAULT, NULL, 0);  //unix_socket and clientflag not used.
    if (!sqlHandler){
    	HcuErrorPrint("DBISYSSWM: MySQL connection failed, Err Code = %s!\n", mysql_error(sqlHandler));
    	mysql_close(sqlHandler);
        return FAILURE;
    }

	//获取数据
    sprintf(strsql, "SELECT * FROM `hcusysswm_swpkg` WHERE (`equentry` = '%d' AND `hwtype` = '%d' AND `hwpem` = '%d' AND `swrel` = '%d' AND `swver` = '%d' AND `upgradeflag` = '%d')",\
    		equEntry, hwType, hwPem, swRel, swVer, upgradeFlag);
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBISYSSWM: Inquery hcusysswm_swpkg error: %s\n", mysql_error(sqlHandler));
        return FAILURE;
	}

	//查具体的结果
	resPtr = mysql_use_result(sqlHandler);
	if (!resPtr){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBISYSSWM: mysql_use_result error!\n");
        return FAILURE;
	}

	//只读取第一条记录
	if ((sqlRow = mysql_fetch_row(resPtr)) != NULL)
	{
		if(sqlRow[0]) *sid = (UINT32)(atol(sqlRow[0]) & 0xFFFFFFFF);
	}
	mysql_free_result(resPtr);

	//释放记录集
    mysql_close(sqlHandler);
    return SUCCESS;
}

OPSTAT dbi_HcuSysSwm_SwPkg_inquery_whole_record(UINT8 equEntry, UINT16 hwType, UINT16 hwPem, UINT16 swRel, UINT16 swVer, UINT8 upgradeFlag, HcuSysMsgIeL3SysSwmSwPkgElement_t *ptrSwPkg)
{
	MYSQL *sqlHandler;
	MYSQL_RES *resPtr;
	MYSQL_ROW sqlRow;
    int result = 0;
    char strsql[DBI_MAX_SQL_INQUERY_STRING_LENGTH];

    //入参检查：不涉及到生死问题，参数也没啥大问题，故而不需要检查，都可以存入数据库表单中
    if (ptrSwPkg == NULL){
    	HcuErrorPrint("DBISYSSWM: Null input parameter\n");
        return FAILURE;
    }

	//建立数据库连接
    sqlHandler = mysql_init(NULL);
    if(!sqlHandler)
    {
    	HcuErrorPrint("DBICOM: MySQL init failed!\n");
        return FAILURE;
    }
    sqlHandler = mysql_real_connect(sqlHandler, HCU_SYSCFG_LOCAL_DB_HOST_DEFAULT, HCU_SYSCFG_LOCAL_DB_USER_DEFAULT, HCU_SYSCFG_LOCAL_DB_PSW_DEFAULT, HCU_SYSCFG_LOCAL_DB_NAME_DEFAULT, HCU_SYSCFG_LOCAL_DB_PORT_DEFAULT, NULL, 0);  //unix_socket and clientflag not used.
    if (!sqlHandler){
    	HcuErrorPrint("DBISYSSWM: MySQL connection failed, Err Code = %s!\n", mysql_error(sqlHandler));
    	mysql_close(sqlHandler);
        return FAILURE;
    }

	//获取数据
    sprintf(strsql, "SELECT * FROM `hcusysswm_swpkg` WHERE (`equentry` = '%d' AND `hwtype` = '%d' AND `hwpem` = '%d' AND `swrel` = '%d' AND `swver` = '%d' AND `upgradeflag` = '%d')",\
    		equEntry, hwType, hwPem, swRel, swVer, upgradeFlag);
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBISYSSWM: Inquery hcusysswm_swpkg error: %s\n", mysql_error(sqlHandler));
        return FAILURE;
	}

	//查具体的结果
	resPtr = mysql_use_result(sqlHandler);
	if (!resPtr){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBISYSSWM: mysql_use_result error!\n");
        return FAILURE;
	}

	//只读取第一条记录
	if ((sqlRow = mysql_fetch_row(resPtr)) != NULL)
	{
		int index;  //SID是INDEX=0的主键
		index = 1;
		if(sqlRow[index]) ptrSwPkg->equEntry = (UINT8)(atol(sqlRow[index++]) & 0xFF);
		if(sqlRow[index]) ptrSwPkg->hwType = (UINT16)(atol(sqlRow[index++]) & 0xFFFF);
		if(sqlRow[index]) ptrSwPkg->hwPem = (UINT16)(atol(sqlRow[index++]) & 0xFFFF);
		if(sqlRow[index]) ptrSwPkg->swRel = (UINT16)(atol(sqlRow[index++]) & 0xFFFF);
		if(sqlRow[index]) ptrSwPkg->swVer = (UINT16)(atol(sqlRow[index++]) & 0xFFFF);
		if(sqlRow[index]) ptrSwPkg->dbVer = (UINT16)(atol(sqlRow[index++]) & 0xFFFF);
		if(sqlRow[index]) ptrSwPkg->upgradeFlag = (UINT8)(atol(sqlRow[index++]) & 0xFF);
		if(sqlRow[index]) ptrSwPkg->swTotalLen = (UINT16)(atol(sqlRow[index++]) & 0xFFFF);
		if(sqlRow[index]) ptrSwPkg->swCksum = (UINT16)(atol(sqlRow[index++]) & 0xFFFF);
		if(sqlRow[index]) ptrSwPkg->dbTotalLen = (UINT16)(atol(sqlRow[index++]) & 0xFFFF);
		if(sqlRow[index]) ptrSwPkg->dbCksum = (UINT16)(atol(sqlRow[index++]) & 0xFFFF);
		if(sqlRow[index]) strncpy(ptrSwPkg->fileName, sqlRow[index++], HCU_SYSMSG_SYSSWM_SW_PKG_FILE_NAME_MAX_LEN-1);
		if(sqlRow[index]) strncpy(ptrSwPkg->dbName, sqlRow[index++], HCU_SYSMSG_SYSSWM_SW_PKG_FILE_NAME_MAX_LEN-1);
		if(sqlRow[index]) strncpy(ptrSwPkg->currentActive, sqlRow[index++], 1);
		if(sqlRow[index]) ptrSwPkg->updateTime = (UINT32)(atol(sqlRow[index++]) & 0xFFFFFFFF);
	}
	mysql_free_result(resPtr);

	//释放记录集
    mysql_close(sqlHandler);
    return SUCCESS;
}

OPSTAT dbi_HcuSysSwm_SwPkg_inquery_max_sw_ver(UINT8 equEntry, UINT16 hwType, UINT16 hwPem, UINT8 upgradeFlag, HcuSysMsgIeL3SysSwmSwPkgElement_t *ptrSwPkg)
{
	MYSQL *sqlHandler;
	MYSQL_RES *resPtr;
	MYSQL_ROW sqlRow;
    int result = 0;
    char strsql[DBI_MAX_SQL_INQUERY_STRING_LENGTH];

    //入参检查：不涉及到生死问题，参数也没啥大问题，故而不需要检查，都可以存入数据库表单中
    if (ptrSwPkg == NULL){
    	HcuErrorPrint("DBISYSSWM: Null input parameter\n");
        return FAILURE;
    }

    //初始化
    memset(ptrSwPkg, 0, sizeof(HcuSysMsgIeL3SysSwmSwPkgElement_t));
    ptrSwPkg->equEntry = equEntry;
    ptrSwPkg->hwType = hwType;
    ptrSwPkg->hwPem = hwPem;
    ptrSwPkg->upgradeFlag = upgradeFlag;

	//建立数据库连接
    sqlHandler = mysql_init(NULL);
    if(!sqlHandler)
    {
    	HcuErrorPrint("DBICOM: MySQL init failed!\n");
        return FAILURE;
    }
    sqlHandler = mysql_real_connect(sqlHandler, HCU_SYSCFG_LOCAL_DB_HOST_DEFAULT, HCU_SYSCFG_LOCAL_DB_USER_DEFAULT, HCU_SYSCFG_LOCAL_DB_PSW_DEFAULT, HCU_SYSCFG_LOCAL_DB_NAME_DEFAULT, HCU_SYSCFG_LOCAL_DB_PORT_DEFAULT, NULL, 0);  //unix_socket and clientflag not used.
    if (!sqlHandler){
    	HcuErrorPrint("DBISYSSWM: MySQL connection failed, Err Code = %s!\n", mysql_error(sqlHandler));
    	mysql_close(sqlHandler);
        return FAILURE;
    }

	//获取数据
    sprintf(strsql, "SELECT * FROM `hcusysswm_swpkg` WHERE (`equentry` = '%d' AND `hwtype` = '%d' AND `upgradeflag` = '%d')",\
    		equEntry, hwType, upgradeFlag);
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBISYSSWM: Inquery hcusysswm_swpkg error: %s\n", mysql_error(sqlHandler));
        return FAILURE;
	}

	//查具体的结果
	resPtr = mysql_use_result(sqlHandler);
	if (!resPtr){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBISYSSWM: mysql_use_result error!\n");
        return FAILURE;
	}

	int index;  //SID是INDEX=0的主键
	UINT16 pemId = 0;
	UINT16 swRel = 0;
	UINT16 swVer = 0;

	//循环查找
	while ((sqlRow = mysql_fetch_row(resPtr)) != NULL)
	{
		pemId = 0; swRel = 0; swVer = 0;
		if(sqlRow[3]) pemId = (UINT16)(atol(sqlRow[3]) & 0xFFFF);
		if(sqlRow[4]) swRel = (UINT16)(atol(sqlRow[4]) & 0xFFFF);
		if(sqlRow[5]) swVer = (UINT16)(atol(sqlRow[5]) & 0xFFFF);
		if (((pemId >= ptrSwPkg->hwPem) && (swRel > ptrSwPkg->swRel)) || ((pemId >= ptrSwPkg->hwPem) && (swRel == ptrSwPkg->swRel) && (swVer > ptrSwPkg->swVer))){
			index = 1;
			if(sqlRow[index]) ptrSwPkg->equEntry = (UINT8)(atol(sqlRow[index++]) & 0xFF);
			if(sqlRow[index]) ptrSwPkg->hwType = (UINT16)(atol(sqlRow[index++]) & 0xFFFF);
			if(sqlRow[index]) ptrSwPkg->hwPem = (UINT16)(atol(sqlRow[index++]) & 0xFFFF);
			if(sqlRow[index]) ptrSwPkg->swRel = (UINT16)(atol(sqlRow[index++]) & 0xFFFF);
			if(sqlRow[index]) ptrSwPkg->swVer = (UINT16)(atol(sqlRow[index++]) & 0xFFFF);
			if(sqlRow[index]) ptrSwPkg->dbVer = (UINT16)(atol(sqlRow[index++]) & 0xFFFF);
			if(sqlRow[index]) ptrSwPkg->upgradeFlag = (UINT8)(atol(sqlRow[index++]) & 0xFF);
			if(sqlRow[index]) ptrSwPkg->swTotalLen = (UINT16)(atol(sqlRow[index++]) & 0xFFFF);
			if(sqlRow[index]) ptrSwPkg->swCksum = (UINT16)(atol(sqlRow[index++]) & 0xFFFF);
			if(sqlRow[index]) ptrSwPkg->dbTotalLen = (UINT16)(atol(sqlRow[index++]) & 0xFFFF);
			if(sqlRow[index]) ptrSwPkg->dbCksum = (UINT16)(atol(sqlRow[index++]) & 0xFFFF);
			if(sqlRow[index]) strncpy(ptrSwPkg->fileName, sqlRow[index++], HCU_SYSMSG_SYSSWM_SW_PKG_FILE_NAME_MAX_LEN-1);
			if(sqlRow[index]) strncpy(ptrSwPkg->dbName, sqlRow[index++], HCU_SYSMSG_SYSSWM_SW_PKG_FILE_NAME_MAX_LEN-1);
			if(sqlRow[index]) strncpy(ptrSwPkg->currentActive, sqlRow[index++], 1);
			if(sqlRow[index]) ptrSwPkg->updateTime = (UINT32)(atol(sqlRow[index++]) & 0xFFFFFFFF);
		}
	}
	mysql_free_result(resPtr);

	//释放记录集
    mysql_close(sqlHandler);
    return SUCCESS;
}

OPSTAT dbi_HcuSysSwm_SwPkg_orphane_file_delete(void)
{
	MYSQL *sqlHandler;
	MYSQL_RES *resPtr;
	MYSQL_ROW sqlRow;
    int result = 0;
    char strsql[DBI_MAX_SQL_INQUERY_STRING_LENGTH];
	DIR *dir;
	struct dirent *ptr;
	char fopr[200];

    //入参检查：不涉及到生死问题，参数也没啥大问题，故而不需要检查，都可以存入数据库表单中
	//zHcuSysEngPar.swm.hcuSwActiveDir
	//遍历文件目录下的所有文件，获取文件名字
	if ((dir=opendir(zHcuSysEngPar.swm.hcuSwActiveDir)) == NULL){
    	HcuErrorPrint("DBISYSSWM: Open dir error! zHcuSysEngPar.swm.hcuSwActiveDir = %s\n", zHcuSysEngPar.swm.hcuSwActiveDir);
        return FAILURE;
	}

	//建立数据库连接
    sqlHandler = mysql_init(NULL);
    if(!sqlHandler)
    {
    	HcuErrorPrint("DBISYSSWM: MySQL init failed!\n");
        return FAILURE;
    }
    sqlHandler = mysql_real_connect(sqlHandler, HCU_SYSCFG_LOCAL_DB_HOST_DEFAULT, HCU_SYSCFG_LOCAL_DB_USER_DEFAULT, HCU_SYSCFG_LOCAL_DB_PSW_DEFAULT, HCU_SYSCFG_LOCAL_DB_NAME_DEFAULT, HCU_SYSCFG_LOCAL_DB_PORT_DEFAULT, NULL, 0);  //unix_socket and clientflag not used.
    if (!sqlHandler){
    	HcuErrorPrint("DBISYSSWM: MySQL connection failed, Err Code = %s!\n", mysql_error(sqlHandler));
    	mysql_close(sqlHandler);
        return FAILURE;
    }

    //循环遍历
	while ((ptr=readdir(dir)) != NULL)
	{
	    if(strcmp(ptr->d_name,".")==0 || strcmp(ptr->d_name,"..")==0)    ///current dir OR parrent dir
	        continue;
	    else if ((ptr->d_type == 8) || (ptr->d_type == 10)){    ///File or Link file
	    	//STEP1: 搜索软件体
	    	//搜索文件名字是否出现在数据库中，不是则删掉
	        sprintf(strsql, "SELECT * FROM `hcusysswm_swpkg` WHERE (`filename` = '%s')", ptr->d_name);
	    	result = mysql_query(sqlHandler, strsql);
	    	if(result){
	    		closedir(dir);
	        	mysql_close(sqlHandler);
	        	HcuErrorPrint("DBISYSSWM: Inquery hcusysswm_swpkg error: %s\n", mysql_error(sqlHandler));
	            return FAILURE;
	    	}

	    	//查具体的结果
	    	resPtr = mysql_use_result(sqlHandler);
	    	if (!resPtr){
	    		closedir(dir);
	        	mysql_close(sqlHandler);
	        	HcuErrorPrint("DBISYSSWM: mysql_use_result error!\n");
	            return FAILURE;
	    	}

	    	//读取不为空
	    	if ((sqlRow = mysql_fetch_row(resPtr)) != NULL){
		    	mysql_free_result(resPtr);
		    	continue;
	    	}

	    	//STEP2: 搜索数据库表单体
	    	//搜索文件名字是否出现在数据库中，不是则删掉
	        sprintf(strsql, "SELECT * FROM `hcusysswm_swpkg` WHERE (`dbname` = '%s')", ptr->d_name);
	    	result = mysql_query(sqlHandler, strsql);
	    	if(result){
	    		closedir(dir);
	        	mysql_close(sqlHandler);
	        	HcuErrorPrint("DBISYSSWM: Inquery hcusysswm_swpkg error: %s\n", mysql_error(sqlHandler));
	            return FAILURE;
	    	}

	    	//查具体的结果
	    	resPtr = mysql_use_result(sqlHandler);
	    	if (!resPtr){
	    		closedir(dir);
	        	mysql_close(sqlHandler);
	        	HcuErrorPrint("DBISYSSWM: mysql_use_result error!\n");
	            return FAILURE;
	    	}

	    	//读取不为空
	    	if((sqlRow = mysql_fetch_row(resPtr)) != NULL){
		    	mysql_free_result(resPtr);
	    		continue;
	    	}

	    	//STEP3: 孤儿文件，所以干掉
			memset(fopr, 0, sizeof(fopr));
			sprintf(fopr, "rm %s%s", zHcuSysEngPar.swm.hcuSwActiveDir, ptr->d_name);
			//HCU_DEBUG_PRINT_FAT("DBISYSSWM: Cmd = %s\n", fopr);
			if (access(&fopr[3], F_OK) == 0) system(fopr);
	    }

	    else if(ptr->d_type == 4)    //dir
	    {
	    }
	}

	//释放记录集
	closedir(dir);
    mysql_close(sqlHandler);
    return SUCCESS;
}


OPSTAT dbi_HcuSysSwm_SwPkg_download_incomplete_file_and_table_delete(void)
{
	MYSQL *sqlHandler;
	MYSQL_RES *resPtr;
	MYSQL_ROW sqlRow;
    int result = 0;
    char strsql[DBI_MAX_SQL_INQUERY_STRING_LENGTH];
    UINT32 selTab[DBI_SYSSWM_SELETION_RESULT_MAX];

    //入参检查：不涉及到生死问题，参数也没啥大问题，故而不需要检查，都可以存入数据库表单中

	//建立数据库连接
    sqlHandler = mysql_init(NULL);
    if(!sqlHandler)
    {
    	HcuErrorPrint("DBICOM: MySQL init failed!\n");
        return FAILURE;
    }
    sqlHandler = mysql_real_connect(sqlHandler, HCU_SYSCFG_LOCAL_DB_HOST_DEFAULT, HCU_SYSCFG_LOCAL_DB_USER_DEFAULT, HCU_SYSCFG_LOCAL_DB_PSW_DEFAULT, HCU_SYSCFG_LOCAL_DB_NAME_DEFAULT, HCU_SYSCFG_LOCAL_DB_PORT_DEFAULT, NULL, 0);  //unix_socket and clientflag not used.
    if (!sqlHandler){
    	HcuErrorPrint("DBISYSSWM: MySQL connection failed, Err Code = %s!\n", mysql_error(sqlHandler));
    	mysql_close(sqlHandler);
        return FAILURE;
    }

    //STEP1: START文件
	//获取数据
    sprintf(strsql, "SELECT * FROM `hcusysswm_swpkg` WHERE (`currentactive` = '%s')", HCU_SYSMSG_SYSSWM_CUR_ACTIVE_START);
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBISYSSWM: Inquery hcusysswm_swpkg error: %s\n", mysql_error(sqlHandler));
        return FAILURE;
	}

	//查具体的结果
	resPtr = mysql_use_result(sqlHandler);
	if (!resPtr){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBISYSSWM: mysql_use_result error!\n");
        return FAILURE;
	}

	char strOpr[200];

	//循环查找
	while ((sqlRow = mysql_fetch_row(resPtr)) != NULL)
	{
		memset(strOpr, 0, sizeof(strOpr));
		if(sqlRow[12]) sprintf(strOpr, "rm %s%s", zHcuSysEngPar.swm.hcuSwActiveDir, sqlRow[12]);
		if (access(&strOpr[3], F_OK) == 0) system(strOpr);
		if(sqlRow[13]) sprintf(strOpr, "rm %s%s", zHcuSysEngPar.swm.hcuSwActiveDir, sqlRow[13]);
		if (access(&strOpr[3], F_OK) == 0) system(strOpr);
	}
	mysql_free_result(resPtr);

	//STEP2: HALF_COMPL文件
	//获取数据
    sprintf(strsql, "SELECT * FROM `hcusysswm_swpkg` WHERE (`currentactive` = '%s')", HCU_SYSMSG_SYSSWM_CUR_ACTIVE_HALF_COMP);
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBISYSSWM: Inquery hcusysswm_swpkg error: %s\n", mysql_error(sqlHandler));
        return FAILURE;
	}

	//查具体的结果
	resPtr = mysql_use_result(sqlHandler);
	if (!resPtr){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBISYSSWM: mysql_use_result error!\n");
        return FAILURE;
	}

	//循环查找
	while ((sqlRow = mysql_fetch_row(resPtr)) != NULL)
	{
		memset(strOpr, 0, sizeof(strOpr));
		if(sqlRow[12]) sprintf(strOpr, "rm %s%s", zHcuSysEngPar.swm.hcuSwActiveDir, sqlRow[12]);
		if (access(&strOpr[3], F_OK) == 0) system(strOpr);
		if(sqlRow[13]) sprintf(strOpr, "rm %s%s", zHcuSysEngPar.swm.hcuSwActiveDir, sqlRow[13]);
		if (access(&strOpr[3], F_OK) == 0) system(strOpr);
	}
	mysql_free_result(resPtr);

	//STEP3: START/HALF_COMPL记录
	//再删去所有的
    sprintf(strsql, "DELETE FROM `hcusysswm_swpkg` WHERE (`currentactive` = '%s')", HCU_SYSMSG_SYSSWM_CUR_ACTIVE_START);
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBISYSSWM: Inquery hcusysswm_swpkg error: %s\n", mysql_error(sqlHandler));
        return FAILURE;
	}
    sprintf(strsql, "DELETE FROM `hcusysswm_swpkg` WHERE (`currentactive` = '%s')", HCU_SYSMSG_SYSSWM_CUR_ACTIVE_HALF_COMP);
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBISYSSWM: Inquery hcusysswm_swpkg error: %s\n", mysql_error(sqlHandler));
        return FAILURE;
	}

	//STEP4: 所有文件不存在的COMP记录
    sprintf(strsql, "SELECT * FROM `hcusysswm_swpkg` WHERE (`currentactive` = '%s')", HCU_SYSMSG_SYSSWM_CUR_ACTIVE_COMPLETE);
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBISYSSWM: Inquery hcusysswm_swpkg error: %s\n", mysql_error(sqlHandler));
        return FAILURE;
	}

	//STEP5: 所有无效的记录
    sprintf(strsql, "DELETE FROM `hcusysswm_swpkg` WHERE (`filename` = '')");
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBISYSSWM: Inquery hcusysswm_swpkg error: %s\n", mysql_error(sqlHandler));
        return FAILURE;
	}

    sprintf(strsql, "DELETE FROM `hcusysswm_swpkg` WHERE (`dbname` = '')");
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBISYSSWM: Inquery hcusysswm_swpkg error: %s\n", mysql_error(sqlHandler));
        return FAILURE;
	}

	//查具体的结果
	resPtr = mysql_use_result(sqlHandler);
	if (!resPtr){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBISYSSWM: mysql_use_result error!\n");
        return FAILURE;
	}

	//循环查找
	memset(selTab, 0, DBI_SYSSWM_SELETION_RESULT_MAX*sizeof(UINT32));
	int index = 0;
	while ((sqlRow = mysql_fetch_row(resPtr)) != NULL)
	{
		memset(strOpr, 0, sizeof(strOpr));
		UINT32 sid = 0;
		//软件体积不存在：下一次再清理数据库文件，不然还需要分为HCU/IHU，导致更为复杂的情况
		if(sqlRow[12]) sprintf(strOpr, "%s%s", zHcuSysEngPar.swm.hcuSwActiveDir, sqlRow[12]);
		//不存在，删去记录
		if (access(strOpr, F_OK) != 0){
			if(sqlRow[0]) sid = (UINT32)(atol(sqlRow[0]) & 0xFFFFFFFF);
			//HCU_DEBUG_PRINT_FAT("SYSSWM: strOpr = %s, sid = %d, access result=%d\n", strOpr, sid, access(strOpr, F_OK));
			if (index < DBI_SYSSWM_SELETION_RESULT_MAX) selTab[index++] = sid;
		}
	}
	mysql_free_result(resPtr);
	int i = 0;
	for (i=0; i<index; i++){
	    sprintf(strsql, "DELETE FROM `hcusysswm_swpkg` WHERE (`sid` = '%d')", selTab[i]);
		result = mysql_query(sqlHandler, strsql);
		if(result){
	    	mysql_close(sqlHandler);
	    	HcuErrorPrint("DBISYSSWM: Inquery hcusysswm_swpkg error: %s\n", mysql_error(sqlHandler));
	        return FAILURE;
		}
	}

	//释放记录集
    mysql_close(sqlHandler);
    return SUCCESS;
}


//下载过的最新记录的处理过程，暂时没有。处理起来比较复杂。
//不执行3Month的自动删除，这跟硬件是否下载的记录重复相关，不能贸然去掉
OPSTAT dbi_HcuSysSwm_SwDownLoad_save(HcuSysMsgIeL3SysSwmSwDlElement_t *prtSwDl)
{
	MYSQL *sqlHandler;
    int result = 0;
    char strsql[DBI_MAX_SQL_INQUERY_STRING_LENGTH];

    //入参检查：不涉及到生死问题，参数也没啥大问题，故而不需要检查，都可以存入数据库表单中

	//建立数据库连接
    sqlHandler = mysql_init(NULL);
    if(!sqlHandler)
    {
    	HcuErrorPrint("DBICOM: MySQL init failed!\n");
        return FAILURE;
    }
    sqlHandler = mysql_real_connect(sqlHandler, HCU_SYSCFG_LOCAL_DB_HOST_DEFAULT, HCU_SYSCFG_LOCAL_DB_USER_DEFAULT, HCU_SYSCFG_LOCAL_DB_PSW_DEFAULT, HCU_SYSCFG_LOCAL_DB_NAME_DEFAULT, HCU_SYSCFG_LOCAL_DB_PORT_DEFAULT, NULL, 0);  //unix_socket and clientflag not used.
    if (!sqlHandler){
    	HcuErrorPrint("DBISYSSWM: MySQL connection failed, Err Code = %s!\n", mysql_error(sqlHandler));
    	mysql_close(sqlHandler);
        return FAILURE;
    }

	//DELETE旧的数据
	sprintf(strsql, "DELETE from `hcusysswm_swdl` WHERE (`equentry` = '%d' AND `hwtype` = '%d' AND `hwpem` = '%d' AND `swrel` = '%d' AND `swver` = '%d' AND `upgradeflag` = '%d')",\
			prtSwDl->equEntry, prtSwDl->hwType, prtSwDl->hwPem, prtSwDl->swRel, prtSwDl->verId, prtSwDl->upgradeFlag);
	result = mysql_query(sqlHandler, strsql);
	if(result){
	 	mysql_close(sqlHandler);
	 	HcuErrorPrint("DBISYSSWM: DELETE data error, err cause = %s\n", mysql_error(sqlHandler));
	    return FAILURE;
	}

	//REPLACE的数据
    sprintf(strsql, "REPLACE INTO `hcusysswm_swdl` (equentry, hwtype, hwpem, swrel, swver, upgradeflag, totallen, checksum, nodeid, segtotal, segindex, segsplitlen, segvalidlen, segcksum, filename, dltime) VALUES \
    		('%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%s', '%d')", prtSwDl->equEntry, prtSwDl->hwType, prtSwDl->hwPem, prtSwDl->swRel, prtSwDl->verId, \
			prtSwDl->upgradeFlag, prtSwDl->totalLen, prtSwDl->checksum, prtSwDl->nodeId, prtSwDl->segTotal, prtSwDl->segIndex, prtSwDl->segSplitLen, prtSwDl->segValidLen, prtSwDl->segCkSum, prtSwDl->fileName, prtSwDl->dlTime);
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBISYSSWM: REPLACE data error, err cause = %s\n", mysql_error(sqlHandler));
        return FAILURE;
	}

	//释放记录集：不再需要free resPtr
    mysql_close(sqlHandler);
    return SUCCESS;
}



/*	//获取数据
    sprintf(strsql, "SELECT * FROM `hcusysswm_swdl` WHERE (`equentry` = '%d' AND `hwtype` = '%d' AND `hwpem` = '%d' AND `swrel` = '%d' AND `swver` = '%d' AND `upgradeflag` = '%d')",\
    		prtSwDl->equEntry, prtSwDl->hwType, prtSwDl->hwPem, prtSwDl->swRel, prtSwDl->verId, prtSwDl->upgradeFlag);
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBISYSSWM: Inquery hcusysswm_swdl error: %s\n", mysql_error(sqlHandler));
        return FAILURE;
	}

	//查具体的结果
	resPtr = mysql_use_result(sqlHandler);
	if (!resPtr){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBISYSSWM: mysql_use_result error!\n");
        return FAILURE;
	}

	//只读取第一条记录
	if ((sqlRow = mysql_fetch_row(resPtr)) == NULL)
	{
		//插入INSERT新的数据，确保记录是唯一的
	    sprintf(strsql, "INSERT INTO `hcusysswm_swdl` (equentry, hwtype, hwpem, swrel, swver, upgradeflag, totallen, checksum, nodeid, segtotal, segindex, segsplitlen, segvalidlen, segcksum, filename, dltime) VALUES \
	    		('%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%s', '%d')", prtSwDl->equEntry, prtSwDl->hwType, prtSwDl->hwPem, prtSwDl->swRel, prtSwDl->verId, \
				prtSwDl->upgradeFlag, prtSwDl->totalLen, prtSwDl->checksum, prtSwDl->nodeId, prtSwDl->segTotal, prtSwDl->segIndex, prtSwDl->segSplitLen, prtSwDl->segValidLen, prtSwDl->segCkSum, prtSwDl->fileName, prtSwDl->dlTime);
		result = mysql_query(sqlHandler, strsql);
		if(result){
			mysql_free_result(resPtr);
	    	mysql_close(sqlHandler);
	    	HcuErrorPrint("DBISYSSWM: INSERT data error, err cause = %s\n", mysql_error(sqlHandler));
	        return FAILURE;
		}
	}

	else{
		//UPDATE旧的数据
		sprintf(strsql, "UPDATE `hcusysswm_swdl` SET totallen = '%d', checksum = '%d', nodeid = '%d', segtotal = '%d', segindex = '%d', segsplitlen = '%d', segvalidlen = '%d', segcksum = '%d', filename = '%s', dltime = '%d' WHERE (`equentry` = '%d' AND `hwtype` = '%d' AND `hwpem` = '%d' AND `swrel` = '%d' AND `swver` = '%d' AND `upgradeflag` = '%d')",\
				prtSwDl->totalLen, prtSwDl->checksum, prtSwDl->nodeId, prtSwDl->segTotal, prtSwDl->segIndex, prtSwDl->segSplitLen, prtSwDl->segValidLen, prtSwDl->segCkSum, prtSwDl->fileName, prtSwDl->dlTime, \
				prtSwDl->equEntry, prtSwDl->hwType, prtSwDl->hwPem, prtSwDl->swRel, prtSwDl->verId, prtSwDl->upgradeFlag);
		result = mysql_query(sqlHandler, strsql);
		if(result){
			mysql_free_result(resPtr);
		 	mysql_close(sqlHandler);
		 	HcuErrorPrint("DBISYSSWM: UPDATE data error, err cause = %s\n", mysql_error(sqlHandler));
		    return FAILURE;
		}
	}*/

