/*
 * dbibfsc.c
 *
 *  Created on: 2016年7月20日
 *      Author: hitpony
 */

#include "../l0dbi/dbi_bfsc.h"
#include "../l0service/trace.h"
#include "../l0dbi/dbicom.h"

/*

--
-- 表的结构 `hcubfsccfgpar`
--

-- --------------------------------------------------------

--
-- Table structure for table `hcubfsccfgpar`
--

CREATE TABLE IF NOT EXISTS `hcubfsccfgpar` (
  `sid` int(4) NOT NULL AUTO_INCREMENT,
  `MinScaleNumberCombination` int(4) NOT NULL,
  `MaxScaleNumberCombination` int(4) NOT NULL,
  `MinScaleNumberStartCombination` int(4) NOT NULL,
  `TargetCombinationWeight` int(4) NOT NULL,
  `TargetCombinationUpperWeight` int(4) NOT NULL,
  `IsPriorityScaleEnabled` int(4) NOT NULL,
  `IsProximitCombinationMode` int(4) NOT NULL,
  `CombinationBias` int(4) NOT NULL,
  `IsRemainDetectionEnable` int(4) NOT NULL,
  `RemainDetectionTimeSec` int(4) NOT NULL,
  `RemainScaleTreatment` int(4) NOT NULL,
  `CombinationSpeedMode` int(4) NOT NULL,
  `CombinationAutoMode` int(4) NOT NULL,
  `MovingAvrageSpeedCount` int(4) NOT NULL,
  `AlgSpare1` int(4) NOT NULL,
  `AlgSpare2` int(4) NOT NULL,
  `AlgSpare3` int(4) NOT NULL,
  `AlgSpare4` int(4) NOT NULL,
  `WeightSensorAdcParameter` int(4) NOT NULL,
  `WeightSensorFilterMode` int(4) NOT NULL,
  `filer_parameter1` int(4) NOT NULL,
  `filer_parameter2` int(4) NOT NULL,
  `filer_parameter3` int(4) NOT NULL,
  `filer_parameter4` int(4) NOT NULL,
  `WeightSensorAutoZeroThread` int(4) NOT NULL,
  `WeightSensorFixCompesation` int(4) NOT NULL,
  `WeightSensorLoadDetectionTimeMs` int(4) NOT NULL,
  `WeightSensorLoadThread` int(4) NOT NULL,
  `WeightSensorEmptyThread` int(4) NOT NULL,
  `WeightSensorEmptyDetectionTimeMs` int(4) NOT NULL,
  `WeightSensorPickupThread` int(4) NOT NULL,
  `WeightSensorPickupDetectionTimeMs` int(4) NOT NULL,
  `StardardReadyTimeMs` int(4) NOT NULL,
  `MaxAllowedWeight` int(4) NOT NULL,
  `WeightSpare1` int(4) NOT NULL,
  `WeightSpare2` int(4) NOT NULL,
  `WeightSpare3` int(4) NOT NULL,
  `WeightSpare4` int(4) NOT NULL,
  `MotorSpeed` int(4) NOT NULL,
  `MotorDirection` int(4) NOT NULL,
  `MotorRollingStartMs` int(4) NOT NULL,
  `MotorRollingStopMs` int(4) NOT NULL,
  `MotorRollingInveralMs` int(4) NOT NULL,
  `MotorFailureDetectionVaration` int(4) NOT NULL,
  `MotorFailureDetectionTimeMs` int(4) NOT NULL,
  `MotorSpare1` int(4) NOT NULL,
  `MotorSpare2` int(4) NOT NULL,
  `MotorSpare3` int(4) NOT NULL,
  `MotorSpare4` int(4) NOT NULL,
  PRIMARY KEY (`sid`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 AUTO_INCREMENT=2 ;

--
-- Dumping data for table `hcubfsccfgpar`
--

INSERT INTO `hcubfsccfgpar` (`sid`, `MinScaleNumberCombination`, `MaxScaleNumberCombination`, `MinScaleNumberStartCombination`, `TargetCombinationWeight`, `TargetCombinationUpperWeight`, `IsPriorityScaleEnabled`, `IsProximitCombinationMode`, `CombinationBias`, `IsRemainDetectionEnable`, `RemainDetectionTimeSec`, `RemainScaleTreatment`, `CombinationSpeedMode`, `CombinationAutoMode`, `MovingAvrageSpeedCount`, `AlgSpare1`, `AlgSpare2`, `AlgSpare3`, `AlgSpare4`, `WeightSensorAdcParameter`, `WeightSensorFilterMode`, `filer_parameter1`, `filer_parameter2`, `filer_parameter3`, `filer_parameter4`, `WeightSensorAutoZeroThread`, `WeightSensorFixCompesation`, `WeightSensorLoadDetectionTimeMs`, `WeightSensorLoadThread`, `WeightSensorEmptyThread`, `WeightSensorEmptyDetectionTimeMs`, `WeightSensorPickupThread`, `WeightSensorPickupDetectionTimeMs`, `StardardReadyTimeMs`, `MaxAllowedWeight`, `WeightSpare1`, `WeightSpare2`, `WeightSpare3`, `WeightSpare4`, `MotorSpeed`, `MotorDirection`, `MotorRollingStartMs`, `MotorRollingStopMs`, `MotorRollingInveralMs`, `MotorFailureDetectionVaration`, `MotorFailureDetectionTimeMs`, `MotorSpare1`, `MotorSpare2`, `MotorSpare3`, `MotorSpare4`) VALUES
(1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1);



-- --------------------------------------------------------

--
-- Table structure for table `hcubfscstadatainfo`
--

CREATE TABLE IF NOT EXISTS `hcubfscstadatainfo` (
  `StaType` char(20) NOT NULL,
  `timestamp` int(4) NOT NULL,
  `wsIncMatCnt` int(4) NOT NULL,
  `wsIncMatWgt` float(8,2) NOT NULL,
  `wsCombTimes` int(4) NOT NULL,
  `wsTttTimes` int(4) NOT NULL,
  `wsTgvTimes` int(4) NOT NULL,
  `wsTttMatCnt` int(4) NOT NULL,
  `wsTgvMatCnt` int(4) NOT NULL,
  `wsTttMatWgt` float(8,2) NOT NULL,
  `wsTgvMatWgt` float(8,2) NOT NULL,
  `wsAvgTttTimes` int(4) NOT NULL,
  `wsAvgTttMatCnt` int(4) NOT NULL,
  `wsAvgTttMatWgt` float(8,2) NOT NULL,
  PRIMARY KEY (`StaType`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Dumping data for table `hcubfscstadatainfo`
--

INSERT INTO `hcubfscstadatainfo` (`StaType`, `timestamp`, `wsIncMatCnt`, `wsIncMatWgt`, `wsCombTimes`, `wsTttTimes`, `wsTgvTimes`, `wsTttMatCnt`, `wsTgvMatCnt`, `wsTttMatWgt`, `wsTgvMatWgt`, `wsAvgTttTimes`, `wsAvgTttMatCnt`, `wsAvgTttMatWgt`) VALUES
('BFSC_STA_24_HOUR', 1487855761, 749, 392529.00, 162, 162, 0, 648, 0, 340166.00, 0.00, 46656, 186624, 1000000.00),
('BFSC_STA_LOCAL_UI', 1487855819, 147, 76719.00, 35, 35, 0, 142, 0, 74453.00, 0.00, 35, 142, 74453.00),
('BFSC_STA_UP_2_NOW', 1487855816, 887, 464713.00, 195, 195, 0, 780, 0, 409172.00, 0.00, 195, 780, 409172.00);


--
-- 表的结构 `hcubfscctrlcmdexg`
--

-- --------------------------------------------------------

--
-- Table structure for table `hcubfscctrlcmdexg`
--

CREATE TABLE IF NOT EXISTS `hcubfscctrlcmdexg` (
  `sid` int(4) NOT NULL AUTO_INCREMENT,
  `startexecuteflag` int(1) NOT NULL,
  PRIMARY KEY (`sid`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 AUTO_INCREMENT=2 ;

--
-- Dumping data for table `hcubfscctrlcmdexg`
--

INSERT INTO `hcubfscctrlcmdexg` (`sid`, `startexecuteflag`) VALUES
(1, 0);









*/



//查询满足条件的第一条记录
OPSTAT dbi_HcuBfsc_Cfgpar_read_into_syseng(UINT32 sid, HcuSysEngBfscCfgpar_t *bfscCfgpar)
{
	MYSQL *sqlHandler;
	MYSQL_RES *resPtr;
	MYSQL_ROW sqlRow;
    int result = 0;
    char strsql[DBI_MAX_SQL_INQUERY_STRING_LENGTH];

    //入参检查：不涉及到生死问题，参数也没啥大问题，故而不需要检查，都可以存入数据库表单中
    if (bfscCfgpar == NULL){
    	HcuErrorPrint("DBIBFSC: Input parameter NULL pointer!\n");
        return FAILURE;
    }

	//建立数据库连接
    sqlHandler = mysql_init(NULL);
    if(!sqlHandler)
    {
    	HcuErrorPrint("DBIBFSC: MySQL init failed!\n");
        return FAILURE;
    }
    sqlHandler = mysql_real_connect(sqlHandler, zHcuSysEngPar.dbi.hcuDbHost, zHcuSysEngPar.dbi.hcuDbUser, zHcuSysEngPar.dbi.hcuDbPsw, zHcuSysEngPar.dbi.hcuDbName, zHcuSysEngPar.dbi.hcuDbPort, NULL, 0);  //unix_socket and clientflag not used.
    if (!sqlHandler){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIBFSC: MySQL connection failed!\n");
        return FAILURE;
    }

	//获取数据
    sprintf(strsql, "SELECT * FROM `hcubfsccfgpar` WHERE (`sid` = '%d')", sid);
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIBFSC: SELECT data error: %s\n", mysql_error(sqlHandler));
        return FAILURE;
	}

	//查具体的结果
	resPtr = mysql_use_result(sqlHandler);
	if (!resPtr){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIBFSC: mysql_use_result error!\n");
        return FAILURE;
	}

	//只读取第一条记录
	if ((sqlRow = mysql_fetch_row(resPtr)) == NULL)
	{
		mysql_free_result(resPtr);
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIBFSC: mysql_fetch_row NULL error!\n");
        return FAILURE;
	}
	else{
		UINT32 index = 0;
		if (sqlRow[index]) bfscCfgpar->sid = (UINT32)(atol(sqlRow[index++]) & 0xFFFFFFFF);
		if (sqlRow[index]) bfscCfgpar->combAlg.MinScaleNumberCombination = (UINT32)(atol(sqlRow[index++]) & 0xFFFFFFFF);
		if (sqlRow[index]) bfscCfgpar->combAlg.MaxScaleNumberCombination = (UINT32)(atol(sqlRow[index++]) & 0xFFFFFFFF);
		if (sqlRow[index]) bfscCfgpar->combAlg.MinScaleNumberStartCombination = (UINT32)(atol(sqlRow[index++]) & 0xFFFFFFFF);
		if (sqlRow[index]) bfscCfgpar->combAlg.TargetCombinationWeight = (UINT32)(atol(sqlRow[index++]) & 0xFFFFFFFF);
		if (sqlRow[index]) bfscCfgpar->combAlg.TargetCombinationUpperWeight = (UINT32)(atol(sqlRow[index++]) & 0xFFFFFFFF);
		if (sqlRow[index]) bfscCfgpar->combAlg.IsPriorityScaleEnabled = (UINT32)(atol(sqlRow[index++]) & 0xFFFFFFFF);
		if (sqlRow[index]) bfscCfgpar->combAlg.IsProximitCombinationMode = (UINT32)(atol(sqlRow[index++]) & 0xFFFFFFFF);
		if (sqlRow[index]) bfscCfgpar->combAlg.CombinationBias = (UINT32)(atol(sqlRow[index++]) & 0xFFFFFFFF);
		if (sqlRow[index]) bfscCfgpar->combAlg.IsRemainDetectionEnable = (UINT32)(atol(sqlRow[index++]) & 0xFFFFFFFF);
		if (sqlRow[index]) bfscCfgpar->combAlg.RemainDetectionTimeSec = (UINT32)(atol(sqlRow[index++]) & 0xFFFFFFFF);
		if (sqlRow[index]) bfscCfgpar->combAlg.RemainScaleTreatment = (UINT32)(atol(sqlRow[index++]) & 0xFFFFFFFF);
		if (sqlRow[index]) bfscCfgpar->combAlg.CombinationSpeedMode = (UINT32)(atol(sqlRow[index++]) & 0xFFFFFFFF);
		if (sqlRow[index]) bfscCfgpar->combAlg.CombinationAutoMode = (UINT32)(atol(sqlRow[index++]) & 0xFFFFFFFF);
		if (sqlRow[index]) bfscCfgpar->combAlg.MovingAvrageSpeedCount = (UINT32)(atol(sqlRow[index++]) & 0xFFFFFFFF);
		if (sqlRow[index]) bfscCfgpar->combAlg.AlgSpare1 = (UINT32)(atol(sqlRow[index++]) & 0xFFFFFFFF);
		if (sqlRow[index]) bfscCfgpar->combAlg.AlgSpare2 = (UINT32)(atol(sqlRow[index++]) & 0xFFFFFFFF);
		if (sqlRow[index]) bfscCfgpar->combAlg.AlgSpare3 = (UINT32)(atol(sqlRow[index++]) & 0xFFFFFFFF);
		if (sqlRow[index]) bfscCfgpar->combAlg.AlgSpare4 = (UINT32)(atol(sqlRow[index++]) & 0xFFFFFFFF);
		if (sqlRow[index]) bfscCfgpar->wsPar.WeightSensorAdcParameter = (UINT32)(atol(sqlRow[index++]) & 0xFFFFFFFF);
		if (sqlRow[index]) bfscCfgpar->wsPar.WeightSensorFilterMode = (UINT32)(atol(sqlRow[index++]) & 0xFFFFFFFF);
		if (sqlRow[index]) bfscCfgpar->wsPar.FilterParam.filer_parameter1 = (UINT32)(atol(sqlRow[index++]) & 0xFFFFFFFF);
		if (sqlRow[index]) bfscCfgpar->wsPar.FilterParam.filer_parameter2 = (UINT32)(atol(sqlRow[index++]) & 0xFFFFFFFF);
		if (sqlRow[index]) bfscCfgpar->wsPar.FilterParam.filer_parameter3 = (UINT32)(atol(sqlRow[index++]) & 0xFFFFFFFF);
		if (sqlRow[index]) bfscCfgpar->wsPar.FilterParam.filer_parameter4 = (UINT32)(atol(sqlRow[index++]) & 0xFFFFFFFF);
		if (sqlRow[index]) bfscCfgpar->wsPar.WeightSensorAutoZeroThread = (UINT32)(atol(sqlRow[index++]) & 0xFFFFFFFF);
		if (sqlRow[index]) bfscCfgpar->wsPar.WeightSensorFixCompesation = (UINT32)(atol(sqlRow[index++]) & 0xFFFFFFFF);
		if (sqlRow[index]) bfscCfgpar->wsPar.WeightSensorLoadDetectionTimeMs = (UINT32)(atol(sqlRow[index++]) & 0xFFFFFFFF);
		if (sqlRow[index]) bfscCfgpar->wsPar.WeightSensorLoadThread = (UINT32)(atol(sqlRow[index++]) & 0xFFFFFFFF);
		if (sqlRow[index]) bfscCfgpar->wsPar.WeightSensorEmptyThread = (UINT32)(atol(sqlRow[index++]) & 0xFFFFFFFF);
		if (sqlRow[index]) bfscCfgpar->wsPar.WeightSensorEmptyDetectionTimeMs = (UINT32)(atol(sqlRow[index++]) & 0xFFFFFFFF);
		if (sqlRow[index]) bfscCfgpar->wsPar.WeightSensorPickupThread = (UINT32)(atol(sqlRow[index++]) & 0xFFFFFFFF);
		if (sqlRow[index]) bfscCfgpar->wsPar.WeightSensorPickupDetectionTimeMs = (UINT32)(atol(sqlRow[index++]) & 0xFFFFFFFF);
		if (sqlRow[index]) bfscCfgpar->wsPar.StardardReadyTimeMs = (UINT32)(atol(sqlRow[index++]) & 0xFFFFFFFF);
		if (sqlRow[index]) bfscCfgpar->wsPar.MaxAllowedWeight = (UINT32)(atol(sqlRow[index++]) & 0xFFFFFFFF);
		if (sqlRow[index]) bfscCfgpar->wsPar.WeightSpare1 = (UINT32)(atol(sqlRow[index++]) & 0xFFFFFFFF);
		if (sqlRow[index]) bfscCfgpar->wsPar.WeightSpare2 = (UINT32)(atol(sqlRow[index++]) & 0xFFFFFFFF);
		if (sqlRow[index]) bfscCfgpar->wsPar.WeightSpare3 = (UINT32)(atol(sqlRow[index++]) & 0xFFFFFFFF);
		if (sqlRow[index]) bfscCfgpar->wsPar.WeightSpare4 = (UINT32)(atol(sqlRow[index++]) & 0xFFFFFFFF);
		if (sqlRow[index]) bfscCfgpar->motoPar.MotorSpeed = (UINT32)(atol(sqlRow[index++]) & 0xFFFFFFFF);
		if (sqlRow[index]) bfscCfgpar->motoPar.MotorDirection = (UINT32)(atol(sqlRow[index++]) & 0xFFFFFFFF);
		if (sqlRow[index]) bfscCfgpar->motoPar.MotorRollingStartMs = (UINT32)(atol(sqlRow[index++]) & 0xFFFFFFFF);
		if (sqlRow[index]) bfscCfgpar->motoPar.MotorRollingStopMs = (UINT32)(atol(sqlRow[index++]) & 0xFFFFFFFF);
		if (sqlRow[index]) bfscCfgpar->motoPar.MotorRollingInveralMs = (UINT32)(atol(sqlRow[index++]) & 0xFFFFFFFF);
		if (sqlRow[index]) bfscCfgpar->motoPar.MotorFailureDetectionVaration = (UINT32)(atol(sqlRow[index++]) & 0xFFFFFFFF);
		if (sqlRow[index]) bfscCfgpar->motoPar.MotorFailureDetectionTimeMs = (UINT32)(atol(sqlRow[index++]) & 0xFFFFFFFF);
		if (sqlRow[index]) bfscCfgpar->motoPar.MotorSpare1 = (UINT32)(atol(sqlRow[index++]) & 0xFFFFFFFF);
		if (sqlRow[index]) bfscCfgpar->motoPar.MotorSpare2 = (UINT32)(atol(sqlRow[index++]) & 0xFFFFFFFF);
		if (sqlRow[index]) bfscCfgpar->motoPar.MotorSpare3 = (UINT32)(atol(sqlRow[index++]) & 0xFFFFFFFF);
		if (sqlRow[index]) bfscCfgpar->motoPar.MotorSpare4 = (UINT32)(atol(sqlRow[index++]) & 0xFFFFFFFF);
	}

	//释放记录集
	mysql_free_result(resPtr);
    mysql_close(sqlHandler);
    return SUCCESS;
}


//根据VM初始化数据，写入数据库表单中初始化值，方便任务模块的增删，降低研发工作复杂度和工作量
OPSTAT dbi_HcuBfsc_StaDatainfo_save(char *StaType, HcuSysMsgIeL3bfscContextStaElement_t *StaDatainfo)
{
	MYSQL *sqlHandler;
    int result = 0;
    char strsql[DBI_MAX_SQL_INQUERY_STRING_LENGTH];

    //入参检查：不涉及到生死问题，参数也没啥大问题，故而不需要检查，都可以存入数据库表单中
    char s[20];
    memset(s, 0, sizeof(s));
    strncpy(s, StaType, strlen(StaType)<sizeof(s)?strlen(StaType):sizeof(s));

	//建立数据库连接
    sqlHandler = mysql_init(NULL);
    if(!sqlHandler)
    {
    	HcuErrorPrint("DBICOM: MySQL init failed!\n");
        return FAILURE;
    }
    sqlHandler = mysql_real_connect(sqlHandler, HCU_SYSCFG_LOCAL_DB_HOST_DEFAULT, HCU_SYSCFG_LOCAL_DB_USER_DEFAULT, HCU_SYSCFG_LOCAL_DB_PSW_DEFAULT, HCU_SYSCFG_LOCAL_DB_NAME_DEFAULT, HCU_SYSCFG_LOCAL_DB_PORT_DEFAULT, NULL, 0);  //unix_socket and clientflag not used.
    if (!sqlHandler){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBICOM: MySQL connection failed!\n");
        return FAILURE;
    }

	//REPLACE新的数据
    UINT32 tmp = time(0);
    sprintf(strsql, "REPLACE INTO `hcubfscstadatainfo` (StaType, timestamp, wsIncMatCnt, wsIncMatWgt, wsCombTimes, wsTttTimes, wsTgvTimes, wsTttMatCnt, wsTgvMatCnt, wsTttMatWgt, wsTgvMatWgt, wsAvgTttTimes, wsAvgTttMatCnt, wsAvgTttMatWgt) VALUES \
    		('%s', '%d', '%d', '%f', '%d', '%d', '%d', '%d', '%d', '%f', '%f', '%d', '%d', '%f')", s, tmp, StaDatainfo->wsIncMatCnt, StaDatainfo->wsIncMatWgt, \
			StaDatainfo->wsCombTimes, StaDatainfo->wsTttTimes, StaDatainfo->wsTgvTimes, StaDatainfo->wsTttMatCnt, StaDatainfo->wsTgvMatCnt, StaDatainfo->wsTttMatWgt, \
			StaDatainfo->wsTgvMatWgt, StaDatainfo->wsAvgTttTimes, StaDatainfo->wsAvgTttMatCnt, StaDatainfo->wsAvgTttMatWgt);
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBICOM: REPLACE data error: %s\n", mysql_error(sqlHandler));
        return FAILURE;
	}

	//释放记录集
    mysql_close(sqlHandler);
    return SUCCESS;
}

//只读取第一条控制字段
OPSTAT dbi_HcuBfsc_ui_ctrl_exg_read(UINT32 *output)
{
	MYSQL *sqlHandler;
	MYSQL_RES *resPtr;
	MYSQL_ROW sqlRow;
    int result = 0;
    char strsql[DBI_MAX_SQL_INQUERY_STRING_LENGTH];

    //入参检查：不涉及到生死问题，参数也没啥大问题，故而不需要检查，都可以存入数据库表单中

	//建立数据库连接
    sqlHandler = mysql_init(NULL);
    if(!sqlHandler)
    {
    	HcuErrorPrint("DBIBFSC: MySQL init failed!\n");
        return FAILURE;
    }
    sqlHandler = mysql_real_connect(sqlHandler, zHcuSysEngPar.dbi.hcuDbHost, zHcuSysEngPar.dbi.hcuDbUser, zHcuSysEngPar.dbi.hcuDbPsw, zHcuSysEngPar.dbi.hcuDbName, zHcuSysEngPar.dbi.hcuDbPort, NULL, 0);  //unix_socket and clientflag not used.
    if (!sqlHandler){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIBFSC: MySQL connection failed!\n");
        return FAILURE;
    }

	//获取数据
    sprintf(strsql, "SELECT * FROM `hcubfscctrlcmdexg` WHERE (1)");
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIBFSC: SELECT data error: %s\n", mysql_error(sqlHandler));
        return FAILURE;
	}

	//查具体的结果
	resPtr = mysql_use_result(sqlHandler);
	if (!resPtr){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIBFSC: mysql_use_result error!\n");
        return FAILURE;
	}

	//只读取第一条记录
	if ((sqlRow = mysql_fetch_row(resPtr)) == NULL)
	{
		mysql_free_result(resPtr);
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIBFSC: mysql_fetch_row NULL error!\n");
        return FAILURE;
	}
	else{
		UINT32 index = 1;
		if (sqlRow[index]) *output = (UINT32)(atol(sqlRow[index++]) & 0xFFFFFFFF);
	}

	//释放记录集
	mysql_free_result(resPtr);
    mysql_close(sqlHandler);
    return SUCCESS;
}









