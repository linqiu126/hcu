/*
 * dbi_bfdf.c
 *
 *  Created on: 2016年7月20日
 *      Author: hitpony
 */

#include "../l0dbi/dbi_bfdf.h"
#include "../l0service/trace.h"
#include "../l0dbi/dbicom.h"


/*

-- --------------------------------------------------------

--
-- Table structure for table `hcubfdfstadatainfo`
--

CREATE TABLE IF NOT EXISTS `hcubfdfstadatainfo` (
  `sid` int(4) NOT NULL AUTO_INCREMENT PRIMARY KEY,
  `statype` char(20) DEFAULT NULL,
  `configid` int(4) DEFAULT NULL,
  `timestamp` int(4) DEFAULT NULL,
  `wsincmatcnt` int(4) DEFAULT NULL,
  `wsincmatwgt` double(18,2) DEFAULT NULL,
  `wscombtimes` int(4) DEFAULT NULL,
  `wsttttimes` int(4) DEFAULT NULL,
  `wstgvtimes` int(4) DEFAULT NULL,
  `wstttmatcnt` int(4) DEFAULT NULL,
  `wstgvmatcnt` int(4) DEFAULT NULL,
  `wstttmatwgt` double(18,2) DEFAULT NULL,
  `wstgvmatwgt` double(18,2) DEFAULT NULL,
  `wsavgttttimes` int(4) DEFAULT NULL,
  `wsavgtttmatcnt` int(4) DEFAULT NULL,
  `wsavgtttmatwgt` double(18,2) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8 AUTO_INCREMENT=1;

--
-- Dumping data for table `hcubfdfstadatainfo`
--

INSERT INTO `hcubfdfstadatainfo` (`sid`, `statype`, `configid`, `timestamp`, `wsincmatcnt`, `wsincmatwgt`, `wscombtimes`, `wsttttimes`, `wstgvtimes`, `wstttmatcnt`, `wstgvmatcnt`, `wstttmatwgt`, `wstgvmatwgt`, `wsavgttttimes`, `wsavgtttmatcnt`, `wsavgtttmatwgt`) VALUES
(1, 'BFDF_STA_24_HOUR', 1, 1501126545, 0, 0.00, 0, 0, 0, 0, 0, 0.00, 0.00, 0, 0, 0.00),
(2, 'BFDF_STA_LOCAL_UI', 0, 1501641455, 0, 0.00, 0, 0, 0, 0, 0, 0.00, 0.00, 0, 0, 0.00),
(3, 'BFDF_STA_LOCAL_UI', 1, 1501641455, 0, 0.00, 0, 0, 0, 0, 0, 0.00, 0.00, 0, 0, 0.00),
(4, 'BFDF_STA_UP_2_NOW', 1, 1501641451, 0, 0.00, 0, 0, 0, 0, 0, 0.00, 0.00, 0, 0, 0.00);

-- --------------------------------------------------------

--
-- Table structure for table `hcubfdfcallcell`
--

CREATE TABLE IF NOT EXISTS `hcubfdfcallcell` (
  `sid` int(4) NOT NULL AUTO_INCREMENT PRIMARY KEY,
  `configid` int(4) DEFAULT NULL,
  `timestamp` int(4) DEFAULT NULL,
  `operator` char(20) DEFAULT NULL,
  `groupid` int(1) DEFAULT NULL,
  `grouprangehigh` int(4) DEFAULT NULL,
  `grouprangelow` int(4) DEFAULT NULL,
  `targetweight` int(4) DEFAULT NULL,
  `uplimitweight` int(4) DEFAULT NULL,
  `actualweight` int(4) DEFAULT NULL,
  `streamline` int(1) DEFAULT NULL,
  `hooperid` int(1) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8 AUTO_INCREMENT=1;

--
-- Dumping data for table `hcubfdfcallcell`
--

INSERT INTO `hcubfdfcallcell` (`sid`, `configid`, `timestamp`, `operator`, `groupid`, `grouprangehigh`, `grouprangelow`, `targetweight`, `uplimitweight`, `actualweight`, `streamline`, `hooperid`) VALUES
(1, 1, 3245662, 'TEST', 1, 100000, 120000, 5000000, 100000, 5032500, 0, 6);


*/


//根据VM初始化数据，写入数据库表单中初始化值，方便任务模块的增删，降低研发工作复杂度和工作量
OPSTAT dbi_HcuBfdf_StaDatainfo_save(char *StaType, UINT16 configId, HcuSysMsgIeL3bfdfContextStaElement_t *StaDatainfo)
{
	MYSQL *sqlHandler;
    int result = 0;
    char strsql[DBI_MAX_SQL_INQUERY_STRING_LENGTH];
	//MYSQL_RES *resPtr;
	//MYSQL_ROW sqlRow;

    //入参检查：不涉及到生死问题，参数也没啥大问题，故而不需要检查，都可以存入数据库表单中
    if (configId == 0){
    	HcuErrorPrint("DBIBFDF: Function dbi_HcuBfsc_StaDatainfo_save() ConfigId=0, no processing!\n");
    	return SUCCESS;
    }

    //正式启动
    char s[20];
    memset(s, 0, sizeof(s));
    strncpy(s, StaType, strlen(StaType)<sizeof(s)?strlen(StaType):sizeof(s));
    UINT32 tmp = time(0);

	//建立数据库连接
    HCU_L0DBICOM_INIT_DB_CONN();

	//DELETE旧的数据
    sprintf(strsql, "DELETE FROM `hcubfdfstadatainfo` WHERE (`statype` = '%s' AND `configid` = '%d')", s, configId);
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIBFDF: DELETE data error, statype = %s, configId=%d, err cause = %s\n", StaType, configId, mysql_error(sqlHandler));
        return FAILURE;
	}

	//REPLACE新的数据
    sprintf(strsql, "REPLACE INTO `hcubfdfstadatainfo` (statype, configid, timestamp, wsincmatcnt, wsincmatwgt, wscombtimes, wsttttimes, wstgvtimes, wstttmatcnt, wstgvmatcnt, wstttmatwgt, wstgvmatwgt, wsavgttttimes, wsavgtttmatcnt, wsavgtttmatwgt) VALUES \
    		('%s', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d')", s, configId, tmp, StaDatainfo->wsIncMatCnt, StaDatainfo->wsIncMatWgt, \
			StaDatainfo->wsCombTimes, StaDatainfo->wsTttTimes, StaDatainfo->wsTgvTimes, StaDatainfo->wsTttMatCnt, StaDatainfo->wsTgvMatCnt, StaDatainfo->wsTttMatWgt, \
			StaDatainfo->wsTgvMatWgt, StaDatainfo->wsAvgTttTimes, StaDatainfo->wsAvgTttMatCnt, StaDatainfo->wsAvgTttMatWgt);
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIBFDF: REPLACE data error, statype = %s, configId=%d, err cause = %s\n", StaType, configId, mysql_error(sqlHandler));
        return FAILURE;
	}

	//释放记录集
    //mysql_free_result(resPtr);
    mysql_close(sqlHandler);
    return SUCCESS;
}

//获取最大的SID号
UINT32 dbi_HcuBfdf_CallCellMaxIdGet(void)
{
	MYSQL *sqlHandler;
	MYSQL_RES *resPtr;
	MYSQL_ROW sqlRow;
	int result = 0;
	char strsql[DBI_MAX_SQL_INQUERY_STRING_LENGTH];
	UINT32 locSid=0;

	//建立数据库连接
    HCU_L0DBICOM_INIT_DB_CONN();

	//获取数据
	sprintf(strsql, "SELECT * FROM `hcubfdfcallcell` WHERE MAX(`sid`)");
	result = mysql_query(sqlHandler, strsql);
	if(result){  //成功返回0
		mysql_close(sqlHandler);
		return 0;
	}
	//查具体的结果
	resPtr = mysql_use_result(sqlHandler);
	if (!resPtr){
		mysql_close(sqlHandler);
		HcuErrorPrint("DBIBFDF: mysql_use_result error!\n");
		return 0;
	}

	//只读取第一条记录
	if ((sqlRow = mysql_fetch_row(resPtr)) == NULL)
	{
		mysql_free_result(resPtr);
		mysql_close(sqlHandler);
		HcuErrorPrint("DBIBFDF: mysql_fetch_row NULL error! strsql = %s\n", strsql);
		return 0;
	}
	else{
		if (sqlRow[0]) locSid = (UINT32)atol(sqlRow[0]);
	}

	//释放记录集
	mysql_free_result(resPtr);
	mysql_close(sqlHandler);
	return locSid;
}


OPSTAT dbi_HcuBfdf_callcell_save(HcuSysMsgIeL3bfdfCallcellElement_t *input)
{
	MYSQL *sqlHandler;
    int result = 0;
    char strsql[DBI_MAX_SQL_INQUERY_STRING_LENGTH];
	//MYSQL_RES *resPtr;
	//MYSQL_ROW sqlRow;

    //建立数据库连接
    HCU_L0DBICOM_INIT_DB_CONN();

	//DELETE旧的数据
    sprintf(strsql, "DELETE FROM `hcubfdfcallcell` WHERE (`sid` = '%d')", input->sid);
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIBFDF: DELETE data error, sid=%d, err cause = %s\n", input->sid, mysql_error(sqlHandler));
        return FAILURE;
	}

	//REPLACE新的数据
    sprintf(strsql, "REPLACE INTO `hcubfdfcallcell` (sid, configid, timestamp, operator, groupid, grouprangehigh, grouprangelow, targetweight, uplimitweight, actualweight, streamline, hooperid) VALUES \
    		('%d', '%d', '%d', '%s', '%d', '%d', '%d', '%d', '%d', '%d', '%d', '%d')", input->sid, input->configId, input->timestamp, input->operator, input->groupId, \
			input->groupRangeHigh, input->groupRangeLow, input->targetWeight, input->upLimitWeight, input->actualWeight, input->streamLine, input->hopperId);
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIBFDF: REPLACE data error, configId=%d, err cause = %s\n", input->sid, mysql_error(sqlHandler));
        return FAILURE;
	}

	//释放记录集
    //mysql_free_result(resPtr);
    mysql_close(sqlHandler);
    return SUCCESS;
}







