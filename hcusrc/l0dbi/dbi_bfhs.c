/*
 * dbibfhs.c
 *
 *  Created on: 2016年7月20日
 *      Author: hitpony
 */

#include "../l0dbi/dbi_bfhs.h"
#include "../l0service/trace.h"
#include "../l0dbi/dbicom.h"

/*

-- --------------------------------------------------------

--
-- Table structure for table `hcubfhsstadatainfo`
--

CREATE TABLE IF NOT EXISTS `hcubfhsstadatainfo` (
  `sid` int(4) NOT NULL AUTO_INCREMENT PRIMARY KEY,
  `statype` char(20) DEFAULT NULL,
  `configid` int(4) DEFAULT NULL,
  `timestamp` int(4) DEFAULT NULL,
  `wsincmatwgt` int(4) DEFAULT NULL,
  `wsincmatcnt` int(4) DEFAULT NULL,
  `wsnormalcnt` int(4) DEFAULT NULL,
  `wsnormalwgt` int(4) DEFAULT NULL,
  `wsovercnt` int(4) DEFAULT NULL,
  `wsoverwgt` int(4) DEFAULT NULL,
  `wsundertotalcnt` int(4) DEFAULT NULL,
  `wsundertotalwgt` int(4) DEFAULT NULL,
  `wsundertu1cnt` int(4) DEFAULT NULL,
  `wsundertu1wgt` int(4) DEFAULT NULL,
  `wsundertu2cnt` int(4) DEFAULT NULL,
  `wsundertu2wgt` int(4) DEFAULT NULL,
  `wsunspecificcnt` int(4) DEFAULT NULL,
  `wsunspecificwgt` int(4) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8 AUTO_INCREMENT=1;


--
-- Dumping data for table `hcubfhsstadatainfo`
--

INSERT INTO `hcubfhsstadatainfo` (`sid`, `statype`, `configid`, `timestamp`, `wsincmatwgt`, `wsincmatcnt`, `wsnormalcnt`, `wsnormalwgt`, `wsovercnt`, `wsoverwgt`, `wsundertotalcnt`, `wsundertotalwgt`, `wsundertu1cnt`, `wsundertu1wgt`, `wsundertu2cnt`, `wsundertu2wgt`, `wsunspecificcnt`, `wsunspecificwgt`) VALUES
(1, 'BFHS_STA_UP_2_NOW', 1, 1231241, 3232, 2, 424, 222, 12, 323, 321, 3123213, 22, 31231, 33, 1232321, 12, 3213);

-- --------------------------------------------------------

--
-- Table structure for table `hcubfhscallcell`
--

CREATE TABLE IF NOT EXISTS `hcubfhscallcell` (
  `sid` int(4) NOT NULL AUTO_INCREMENT PRIMARY KEY,
  `configid` int(4) DEFAULT NULL,
  `timestamp` int(4) DEFAULT NULL,
  `operator` char(20) DEFAULT NULL,
  `targetweight` int(4) DEFAULT NULL,
  `tu1` int(4) DEFAULT NULL,
  `tu2` int(4) DEFAULT NULL,
  `actualweight` int(4) DEFAULT NULL,
  `state` int(1) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8 AUTO_INCREMENT=1;

--
-- Dumping data for table `hcubfhscallcell`
--

INSERT INTO `hcubfhscallcell` (`sid`, `configid`, `timestamp`, `operator`, `targetweight`, `tu1`, `tu2`, `actualweight`, `state`) VALUES
(1, 1, 124124214, 'TEST', 940000, 5000, 8000, 938000, 1);


*/


//根据VM初始化数据，写入数据库表单中初始化值，方便任务模块的增删，降低研发工作复杂度和工作量
OPSTAT dbi_HcuBfhs_StaDatainfo_save(char *StaType, UINT16 configId, HcuSysMsgIeL3bfhsContextStaElement_t *StaDatainfo)
{
	MYSQL *sqlHandler;
    int result = 0;
    char strsql[DBI_MAX_SQL_INQUERY_STRING_LENGTH];
	//MYSQL_RES *resPtr;
	//MYSQL_ROW sqlRow;

    //入参检查：不涉及到生死问题，参数也没啥大问题，故而不需要检查，都可以存入数据库表单中
    if (configId == 0){
    	HcuErrorPrint("DBIBFHS: Function dbi_HcuBfhs_StaDatainfo_save() ConfigId=0, no processing!\n");
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
    sprintf(strsql, "DELETE FROM `hcubfhsstadatainfo` WHERE (`statype` = '%s' AND `configid` = '%d')", s, configId);
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIBFHS: DELETE data error, statype = %s, configId=%d, err cause = %s\n", StaType, configId, mysql_error(sqlHandler));
        return FAILURE;
	}


	//REPLACE新的数据
    sprintf(strsql, "REPLACE INTO `hcubfhsstadatainfo` (statype, configid, timestamp, wsincmatwgt, wsincmatcnt, wsnormalcnt, wsnormalwgt, wsovercnt, wsoverwgt, wsundertotalcnt, wsundertotalwgt, wsundertu1cnt, wsundertu1wgt, wsundertu2cnt, wsundertu2wgt, wsunspecificcnt, wsunspecificwgt) VALUES \
    		('%s', '%d', '%d', '%lu', '%d', '%d', '%lu', '%d', '%lu', '%d', '%lu', '%d', '%lu', '%d', '%lu', '%d', '%lu')", s, configId, tmp, StaDatainfo->wsIncMatWgt, StaDatainfo->wsIncMatCnt,\
			StaDatainfo->wsNormalCnt, StaDatainfo->wsNormalWgt, StaDatainfo->wsOverCnt, StaDatainfo->wsOverWgt, StaDatainfo->wsUnderTotalCnt, StaDatainfo->wsUnderTotalWgt,\
			StaDatainfo->wsUnderTu1Cnt, StaDatainfo->wsUnderTu1Wgt, StaDatainfo->wsUnderTu2Cnt, StaDatainfo->wsUnderTu2Wgt, StaDatainfo->wsUnspecificCnt, StaDatainfo->wsUnspecificWgt);
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIBFHS: REPLACE data error, statype = %s, configId=%d, err cause = %s\n", StaType, configId, mysql_error(sqlHandler));
        return FAILURE;
	}

	//释放记录集
    //mysql_free_result(resPtr);
    mysql_close(sqlHandler);
    return SUCCESS;
}

//获取最大的SID号
UINT32 dbi_HcuBfhs_CallCellMaxIdGet(void)
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
	sprintf(strsql, "SELECT * FROM `hcubfhscallcell` WHERE MAX(`sid`)");
	result = mysql_query(sqlHandler, strsql);
	if(result){  //成功返回0
		mysql_close(sqlHandler);
		return 0;
	}
	//查具体的结果
	resPtr = mysql_use_result(sqlHandler);
	if (!resPtr){
		mysql_close(sqlHandler);
		HcuErrorPrint("DBIBFHS: mysql_use_result error!\n");
		return 0;
	}

	//只读取第一条记录
	if ((sqlRow = mysql_fetch_row(resPtr)) == NULL)
	{
		mysql_free_result(resPtr);
		mysql_close(sqlHandler);
		HcuErrorPrint("DBIBFHS: mysql_fetch_row NULL error! strsql = %s\n", strsql);
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


OPSTAT dbi_HcuBfhs_callcell_save(HcuSysMsgIeL3bfhsCallcellElement_t *input)
{
	MYSQL *sqlHandler;
    int result = 0;
    char strsql[DBI_MAX_SQL_INQUERY_STRING_LENGTH];
	//MYSQL_RES *resPtr;
	//MYSQL_ROW sqlRow;

    //建立数据库连接
    HCU_L0DBICOM_INIT_DB_CONN();

	//DELETE旧的数据
    sprintf(strsql, "DELETE FROM `hcubfhscallcell` WHERE (`sid` = '%d')", input->sid);
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIBFHS: DELETE data error, sid=%d, err cause = %s\n", input->sid, mysql_error(sqlHandler));
        return FAILURE;
	}

	//REPLACE新的数据
    sprintf(strsql, "REPLACE INTO `hcubfhscallcell` (sid, configid, timestamp, operator, targetweight, tu1, tu2, actualweight, state) VALUES \
    		('%d', '%d', '%d', '%s', '%d', '%d', '%d', '%d', '%d')", input->sid, input->configId, input->timestamp, input->operator, input->targetWeight, \
			input->tu1, input->tu2, input->actualWeight, input->state);
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIBFHS: REPLACE data error, configId=%d, err cause = %s\n", input->sid, mysql_error(sqlHandler));
        return FAILURE;
	}

	//释放记录集
    //mysql_free_result(resPtr);
    mysql_close(sqlHandler);
    return SUCCESS;
}

//删除对应用户所有超过90天的数据
//缺省做成90天，如果参数错误，导致90天以内的数据强行删除，则不被认可
OPSTAT dbi_HcuBfhs_callcell_delete_3monold(UINT32 days)
{
	MYSQL *sqlHandler;
    int result = 0;
    char strsql[DBI_MAX_SQL_INQUERY_STRING_LENGTH];
    UINT32 cursec = 0;

    //入参检查：不涉及到生死问题，参数也没啥大问题，故而不需要检查，都可以存入数据库表单中
    if (days <DBI_BFHS_DATA_SAVE_DAYS_MIN) days = DBI_BFHS_DATA_SAVE_DAYS_MIN;

    //建立数据库连接
    HCU_L0DBICOM_INIT_DB_CONN();

	//删除满足条件的数据
    cursec = time(NULL);
    days = days * 24 * 3600;
    sprintf(strsql, "DELETE FROM `hcubfhscallcell` WHERE (%d - `timestamp` > '%d')", cursec, days);
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIBFHS: INSET data error: %s\n", mysql_error(sqlHandler));
        return FAILURE;
	}

	//释放记录集
    mysql_close(sqlHandler);
    return SUCCESS;
}

OPSTAT dbi_HcuBfhs_sysConfigData_read(UINT32 sysConfigData[HCU_SYSCFG_BFHS_DB_COLUMN_NUM_MAX])
	{
		MYSQL *sqlHandler;
		MYSQL_RES *resPtr;
		MYSQL_ROW sqlRow;
	    int result = 0;
	    char strsql[DBI_MAX_SQL_INQUERY_STRING_LENGTH];

	    //建立数据库连接
	    HCU_L0DBICOM_INIT_DB_CONN();

		//获取数据
	    sprintf(strsql, "SELECT * FROM `hcubfhssystempara` WHERE (1)");
		result = mysql_query(sqlHandler, strsql);
		if(result){  //成功返回0
	    	mysql_close(sqlHandler);
	    	HcuErrorPrint("DBIBFHS: SELECT data from hcubfhssystempara error: %s\n", mysql_error(sqlHandler));
	        return FAILURE;
		}
		//查具体的结果
		resPtr = mysql_use_result(sqlHandler);
		if (!resPtr){
	    	mysql_close(sqlHandler);
	    	HcuErrorPrint("DBIBFHS: mysql_use_result error!\n");
	        return FAILURE;
		}
		//只读取第一条记录
		if ((sqlRow = mysql_fetch_row(resPtr)) == NULL){
			mysql_free_result(resPtr);
			mysql_close(sqlHandler);
			HcuErrorPrint("DBIBFHS: mysql_fetch_row NULL error! strsql = %s\n", strsql);
			return FAILURE;
		}
		else{
			UINT8  index;
			for (index =0; index < resPtr->field_count; index++){
				if (sqlRow[index] && index<HCU_SYSCFG_BFHS_DB_COLUMN_NUM_MAX)  sysConfigData[index] = (UINT32)atol(sqlRow[index]);
			}
		}

		//释放记录集
		mysql_free_result(resPtr);
	    mysql_close(sqlHandler);
	    return SUCCESS;
	}

OPSTAT dbi_HcuBfhs_productConfigData_read(UINT16 configId, UINT32 productConfigData[HCU_PRODUCTCFG_BFHS_DB_COLUMN_NUM_MAX])
	{
		MYSQL *sqlHandler;
		MYSQL_RES *resPtr;
		MYSQL_ROW sqlRow;
	    int result = 0;
	    char strsql[DBI_MAX_SQL_INQUERY_STRING_LENGTH];

	    //建立数据库连接
	    HCU_L0DBICOM_INIT_DB_CONN();

		//获取数据
	    sprintf(strsql, "SELECT * FROM `hcubfhsproductpara` WHERE (`configid` = '%d')" , configId);
		result = mysql_query(sqlHandler, strsql);
		if(result){  //成功返回0
	    	mysql_close(sqlHandler);
	    	HcuErrorPrint("DBIBFHS: SELECT data from hcubfhsproductpara error: %s\n", mysql_error(sqlHandler));
	        return FAILURE;
		}
		//查具体的结果
		resPtr = mysql_use_result(sqlHandler);
		if (!resPtr){
	    	mysql_close(sqlHandler);
	    	HcuErrorPrint("DBIBFHS: mysql_use_result error!\n");
	        return FAILURE;
		}
		//只读取第一条记录
		if ((sqlRow = mysql_fetch_row(resPtr)) == NULL){
			mysql_free_result(resPtr);
			mysql_close(sqlHandler);
			HcuErrorPrint("DBIBFHS: mysql_fetch_row NULL error! strsql = %s\n", strsql);
			return FAILURE;
		}
		else{
			UINT8  index;
			for (index =0; index < resPtr->field_count; index++){
				if (sqlRow[index] && index<HCU_PRODUCTCFG_BFHS_DB_COLUMN_NUM_MAX)  productConfigData[index] = (UINT32)atol(sqlRow[index]);
			}
		}

		//释放记录集
		mysql_free_result(resPtr);
	    mysql_close(sqlHandler);
	    return SUCCESS;
	}

OPSTAT dbi_HcuBfhs_calData_save(StrMsgIe_WeightSensorBfhsCalibrationFullRespParamaters_t *input)
{
	MYSQL *sqlHandler;
	    int result = 0;
	    char strsql[DBI_MAX_SQL_INQUERY_STRING_LENGTH];
		//MYSQL_RES *resPtr;
		//MYSQL_ROW sqlRow;

	    //建立数据库连接
	    HCU_L0DBICOM_INIT_DB_CONN();

		//UPDATE新的数据
	    sprintf(strsql, "UPDATE `hcubfhssystempara` SET snrtimegrid='%d', autozeroswitch='%d', snrautotaringtime ='%d', snrautozerorange ='%d', snrfiltercutoffreq ='%d',snrmeasrangeno='%d', snrpreloadvalueformat='%d', snrpreloadvalue='%d', snrringbuftime ='%d', snrstandstilltime='%d', snrstandstilltimeout='%d', snrstandstillrange='%d' WHERE (1)", \
				input->WeightSensorTimeGrid, input->WeightSensorAutoZero, input->WeightSensorAutoZeroAutotaringTimeMs,input->WeightSensorAutoZeroCaptureRangeGrams,input->WeightSensorFilterCutOffFreqHz,\
				input->WeightSensorMeasurementRangeNo, input->WeightSensorPreloadComPensationPlacesAfterDecimalPoint, input->WeightSensorPreloadComPensationValuePercent, input->WeightSensorRingBufTimeMs,\
				input->WeightSensorStandstillTime,input->WeightSensorStandstillTimeoutMs,input->WeightSensorStandstillRangeGrams);

	    printf ("BFHS_DBI: calData_save strsql = %s\n\n", strsql);
		result = mysql_query(sqlHandler, strsql);
		if(result){
	    	mysql_close(sqlHandler);
	    	HcuErrorPrint("DBIBFHS: UPDATE data error, err cause = %s\n", mysql_error(sqlHandler));
	        return FAILURE;
		}

		//释放记录集
	    //mysql_free_result(resPtr);
	    mysql_close(sqlHandler);
	    return SUCCESS;
}


