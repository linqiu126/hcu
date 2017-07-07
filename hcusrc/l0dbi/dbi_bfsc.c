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
  `wsIncMatWgt` double(18,2) NOT NULL,
  `wsCombTimes` int(4) NOT NULL,
  `wsTttTimes` int(4) NOT NULL,
  `wsTgvTimes` int(4) NOT NULL,
  `wsTttMatCnt` int(4) NOT NULL,
  `wsTgvMatCnt` int(4) NOT NULL,
  `wsTttMatWgt` double(18,2) NOT NULL,
  `wsTgvMatWgt` double(18,2) NOT NULL,
  `wsAvgTttTimes` int(4) NOT NULL,
  `wsAvgTttMatCnt` int(4) NOT NULL,
  `wsAvgTttMatWgt` double(18,2) NOT NULL,
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
-- 表的结构 `hcubfscfb2ui`
--

-- --------------------------------------------------------

--
-- Table structure for table `hcubfscfb2ui`
--

CREATE TABLE IF NOT EXISTS `hcubfscfb2ui` (
  `sid` int(4) NOT NULL AUTO_INCREMENT,
  `cmdtype` int(1) NOT NULL,
  `validflag` int(1) NOT NULL,
  `fbinfo` char(80) NOT NULL,
  PRIMARY KEY (`sid`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 AUTO_INCREMENT=2 ;

--
-- Dumping data for table `hcubfscfb2ui`
--

INSERT INTO `hcubfscfb2ui` (`sid`, `cmdtype`, `validflag`, `fbinfo`) VALUES
(1, 1, 1, 'Configuration success'),
(2, 2, 0, 'Start failure due to WS not reachable'),
(3, 3, 0, 'Stop failure due to WS not reachable');



--
-- Database: `hcudb`
--

-- --------------------------------------------------------

--
-- Table structure for table `hcubfsccurrentinfo`
--

CREATE TABLE `hcubfsccurrentinfo` (
  `deviceid` varchar(20) NOT NULL,
  `timestamp` int(4) NOT NULL,
  `status_01` int(1) DEFAULT NULL,
  `value_01` int(2) DEFAULT NULL,
  `status_02` int(1) DEFAULT NULL,
  `value_02` int(2) DEFAULT NULL,
  `status_03` int(1) DEFAULT NULL,
  `value_03` int(2) DEFAULT NULL,
  `status_04` int(1) DEFAULT NULL,
  `value_04` int(2) DEFAULT NULL,
  `status_05` int(1) DEFAULT NULL,
  `value_05` int(2) DEFAULT NULL,
  `status_06` int(1) DEFAULT NULL,
  `value_06` int(2) DEFAULT NULL,
  `status_07` int(1) DEFAULT NULL,
  `value_07` int(2) DEFAULT NULL,
  `status_08` int(1) DEFAULT NULL,
  `value_08` int(2) DEFAULT NULL,
  `status_09` int(1) DEFAULT NULL,
  `value_09` int(2) DEFAULT NULL,
  `status_10` int(1) DEFAULT NULL,
  `value_10` int(2) DEFAULT NULL,
  `status_11` int(1) DEFAULT NULL,
  `value_11` int(2) DEFAULT NULL,
  `status_12` int(1) DEFAULT NULL,
  `value_12` int(2) DEFAULT NULL,
  `status_13` int(1) DEFAULT NULL,
  `value_13` int(2) DEFAULT NULL,
  `status_14` int(1) DEFAULT NULL,
  `value_14` int(2) DEFAULT NULL,
  `status_15` int(1) DEFAULT NULL,
  `value_15` int(2) DEFAULT NULL,
  `status_16` int(1) DEFAULT NULL,
  `value_16` int(2) DEFAULT NULL,
  `curcomwgt` int(4) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Dumping data for table `hcubfsccurrentinfo`
--

INSERT INTO `hcubfsccurrentinfo` (`deviceid`, `timestamp`, `status_01`, `value_01`, `status_02`, `value_02`, `status_03`, `value_03`, `status_04`, `value_04`, `status_05`, `value_05`, `status_06`, `value_06`, `status_07`, `value_07`, `status_08`, `value_08`, `status_09`, `value_09`, `status_10`, `value_10`, `status_11`, `value_11`, `status_12`, `value_12`, `status_13`, `value_13`, `status_14`, `value_14`, `status_15`, `value_15`, `status_16`, `value_16`, `curcomwgt`) VALUES
('HCU_G301_BFSC_P0001', 20170518, 1, 27639, 1, 23373, 1, 37718, 1, 26290, 1, 17153, 1, 20731, 1, 30501, 1, 23568, 1, 39125, 1, 22317, 0, 111, 0, 121, 0, 131, 0, 141, 0, 151, 0, 161, 0);





*/



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
    	HcuErrorPrint("DBICOM: MySQL connection failed, Err Code = %s!\n", mysql_error(sqlHandler));
    	mysql_close(sqlHandler);
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
    	HcuErrorPrint("DBICOM: REPLACE data error, staType = %s, err cause = %s\n", StaType, mysql_error(sqlHandler));
        return FAILURE;
	}

	//释放记录集
    mysql_close(sqlHandler);
    //HCU_DEBUG_PRINT_CRT("DBICOM: MYSQL release result = %s\n", mysql_error(sqlHandler));
    return SUCCESS;
}


//将执行结果存入hcubfscfb2ui
OPSTAT dbi_HcuBfsc_Fb2Ui_save(UINT32 cmdType, UINT32 validFlag, char *info)
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
    	HcuErrorPrint("DBICOM: MySQL connection failed, Err Code = %s!\n", mysql_error(sqlHandler));
    	mysql_close(sqlHandler);
    	return FAILURE;
    }

	//REPLACE新的数据
    //UINT32 tmp = time(0);
    sprintf(strsql, "REPLACE INTO `hcubfscfb2ui` (cmdtype, validflag, fbinfo) VALUES ('%d', '%d', '%s')", cmdType, validFlag, info);
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


//
OPSTAT dbi_HcuBfsc_WmcStatusUpdate(uint32_t aws_id, uint32_t wmc_id, uint32_t wmc_status, uint32_t wmc_weight_value)
{
	MYSQL *sqlHandler;
    int result = 0;
    UINT32 timestamp;
    char strsql[DBI_MAX_SQL_INQUERY_STRING_LENGTH];

    //入参检查：不涉及到生死问题，参数也没啥大问题，故而不需要检查，都可以存入数据库表单中
    char s[20];
    memset(s, 0, sizeof(s));

	//建立数据库连接
    sqlHandler = mysql_init(NULL);
    if(!sqlHandler)
    {
    	HcuErrorPrint("DBICOM: MySQL init failed!\n");
        return FAILURE;
    }
    sqlHandler = mysql_real_connect(sqlHandler, HCU_SYSCFG_LOCAL_DB_HOST_DEFAULT, HCU_SYSCFG_LOCAL_DB_USER_DEFAULT, HCU_SYSCFG_LOCAL_DB_PSW_DEFAULT, HCU_SYSCFG_LOCAL_DB_NAME_DEFAULT, HCU_SYSCFG_LOCAL_DB_PORT_DEFAULT, NULL, 0);  //unix_socket and clientflag not used.
    if (!sqlHandler){
    	HcuErrorPrint("DBICOM: MySQL connection failed, Err Code = %s!\n", mysql_error(sqlHandler));
    	mysql_close(sqlHandler);
        return FAILURE;
    }

	//UPDATE新的数据
    timestamp = time(NULL);
    if( (0xFFFFFFFF == wmc_id) && (0xFFFFFFFF == wmc_weight_value) )
    {
        mysql_close(sqlHandler);
        return SUCCESS;
    }
    else if( (0xFFFFFFFF == wmc_id) && (0xFFFFFFFF != wmc_weight_value) )
    {
        sprintf(strsql, "UPDATE `hcubfsccurrentinfo` SET timestamp = '%d', value_%02d = '%d' WHERE (deviceid = '%s')", \
        		timestamp, wmc_id, wmc_weight_value, zHcuSysEngPar.hwBurnId.equLable);
    }
    else if( (0xFFFFFFFF != wmc_id) && (0xFFFFFFFF == wmc_weight_value) )
    {
		sprintf(strsql, "UPDATE `hcubfsccurrentinfo` SET timestamp = '%d', status_%02d = '%d' WHERE (deviceid = '%s')", \
				timestamp, wmc_id, wmc_status, wmc_id, wmc_weight_value,zHcuSysEngPar.hwBurnId.equLable);
    }
    else
    {
		sprintf(strsql, "UPDATE `hcubfsccurrentinfo` SET timestamp = '%d', status_%02d = '%d', value_%02d = '%d' WHERE (deviceid = '%s')", \
				timestamp, wmc_id, wmc_status, wmc_id, wmc_weight_value, zHcuSysEngPar.hwBurnId.equLable);
    }

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


OPSTAT dbi_HcuBfsc_WmcCurComWgtUpdate(uint32_t wgt)
{
	MYSQL *sqlHandler;
    int result = 0;
    char strsql[DBI_MAX_SQL_INQUERY_STRING_LENGTH];

    //入参检查：不涉及到生死问题，参数也没啥大问题，故而不需要检查，都可以存入数据库表单中
    char s[20];
    memset(s, 0, sizeof(s));

	//建立数据库连接
    sqlHandler = mysql_init(NULL);
    if(!sqlHandler)
    {
    	HcuErrorPrint("DBICOM: MySQL init failed!\n");
        return FAILURE;
    }
    sqlHandler = mysql_real_connect(sqlHandler, HCU_SYSCFG_LOCAL_DB_HOST_DEFAULT, HCU_SYSCFG_LOCAL_DB_USER_DEFAULT, HCU_SYSCFG_LOCAL_DB_PSW_DEFAULT, HCU_SYSCFG_LOCAL_DB_NAME_DEFAULT, HCU_SYSCFG_LOCAL_DB_PORT_DEFAULT, NULL, 0);  //unix_socket and clientflag not used.
    if (!sqlHandler){
    	HcuErrorPrint("DBICOM: MySQL connection failed, Err Code = %s!\n", mysql_error(sqlHandler));
    	mysql_close(sqlHandler);
        return FAILURE;
    }

	//UPDATE新的数据
    sprintf(strsql, "UPDATE `hcubfsccurrentinfo` SET curcomwgt = '%d' WHERE (deviceid = '%s')", \
		wgt, zHcuSysEngPar.hwBurnId.equLable);

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

//
OPSTAT dbi_HcuBfsc_WmcStatusForceInvalid(uint32_t aws_id)
{
	MYSQL *sqlHandler;
	UINT32 timestamp;
    int result = 0;
    char strsql[DBI_MAX_SQL_INQUERY_STRING_LENGTH];
    uint32_t wmc_id =0 ;

    //入参检查：不涉及到生死问题，参数也没啥大问题，故而不需要检查，都可以存入数据库表单中
    char s[20];
    memset(s, 0, sizeof(s));

	//建立数据库连接
    sqlHandler = mysql_init(NULL);
    if(!sqlHandler)
    {
    	HcuErrorPrint("DBICOM: MySQL init failed!\n");
        return FAILURE;
    }
    sqlHandler = mysql_real_connect(sqlHandler, HCU_SYSCFG_LOCAL_DB_HOST_DEFAULT, HCU_SYSCFG_LOCAL_DB_USER_DEFAULT, HCU_SYSCFG_LOCAL_DB_PSW_DEFAULT, HCU_SYSCFG_LOCAL_DB_NAME_DEFAULT, HCU_SYSCFG_LOCAL_DB_PORT_DEFAULT, NULL, 0);  //unix_socket and clientflag not used.
    if (!sqlHandler){
    	HcuErrorPrint("DBICOM: MySQL connection failed, Err Code = %s!\n", mysql_error(sqlHandler));
    	mysql_close(sqlHandler);
        return FAILURE;
    }

	//REPLACE新的数据
    timestamp = time(NULL);
    for(wmc_id = 1; wmc_id <= 16; wmc_id++)
    {
		sprintf(strsql, "UPDATE `hcubfsccurrentinfo` SET timestamp = '%d', status_%02d = '%d' WHERE (deviceid = '%s')", \
				timestamp, wmc_id, 0, zHcuSysEngPar.hwBurnId.equLable);
		result = mysql_query(sqlHandler, strsql);
		if(result){
	    	mysql_close(sqlHandler);
	    	HcuErrorPrint("DBICOM: REPLACE data error: %s\n", mysql_error(sqlHandler));
	        return FAILURE;
		}
    }

	//释放记录集
    mysql_close(sqlHandler);
    return SUCCESS;
}

OPSTAT dbi_HcuBfsc_CalibrationDataUpdate(UINT8 cmdid, UINT32  adcvalue, UINT32 fullweight, UINT8  sensorid)
{
	MYSQL *sqlHandler;
    int result = 0;
    char strsql[DBI_MAX_SQL_INQUERY_STRING_LENGTH];

	//建立数据库连接
    sqlHandler = mysql_init(NULL);
    if(!sqlHandler)
    {
    	HcuErrorPrint("DBICOM: MySQL init failed!\n");
        return FAILURE;
    }
    sqlHandler = mysql_real_connect(sqlHandler, HCU_SYSCFG_LOCAL_DB_HOST_DEFAULT, HCU_SYSCFG_LOCAL_DB_USER_DEFAULT, HCU_SYSCFG_LOCAL_DB_PSW_DEFAULT, HCU_SYSCFG_LOCAL_DB_NAME_DEFAULT, HCU_SYSCFG_LOCAL_DB_PORT_DEFAULT, NULL, 0);  //unix_socket and clientflag not used.
    if (!sqlHandler){
    	HcuErrorPrint("DBICOM: MySQL connection failed, Err Code = %s!\n", mysql_error(sqlHandler));
    	mysql_close(sqlHandler);
        return FAILURE;
    }

    HcuDebugPrint("DBIBFSC: Receive calibration date: sensorid = %02d, adcvalue = %d, weight = %d!\n", sensorid,adcvalue,fullweight);
    //零值校准数据
    if (cmdid == CMDID_SENSOR_COMMAND_CALIBRATION_ZERO){
		sprintf(strsql, "UPDATE `hcubfsccalibration` SET zeroadc_%02d = '%d' WHERE (deviceid = '%s')", sensorid, adcvalue, zHcuSysEngPar.hwBurnId.equLable);
		result = mysql_query(sqlHandler, strsql);
    }
    //满值校准数据
    else if (cmdid == CMDID_SENSOR_COMMAND_CALIBRATION_FULL){
    	sprintf(strsql, "UPDATE `hcubfsccalibration` SET fulladc_%02d = '%d', fullwgt_%02d='%d'  WHERE (deviceid = '%s')", sensorid, adcvalue, sensorid, fullweight, zHcuSysEngPar.hwBurnId.equLable);
		result = mysql_query(sqlHandler, strsql);
    }

    if(result){
    	    	mysql_close(sqlHandler);
    	    	HcuErrorPrint("DBICOM: REPLACE data error [func=dbi_HcuBfsc_CalibrationDataUpdate]: %s\n", mysql_error(sqlHandler));
    	        return FAILURE;
    		}

	//释放记录集
    mysql_close(sqlHandler);
    return SUCCESS;
}

//获取校准ADC值并初始化到全局表 gTaskL3bfscContext
OPSTAT dbi_HcuBfsc_CalibrationDataGet( UINT32 data[(HCU_SYSCFG_BFSC_SNR_WS_NBR_MAX-1)*3] )
{
	MYSQL *sqlHandler;
	MYSQL_RES *resPtr;
	MYSQL_ROW sqlRow;
    int result = 0;
    char strsql[DBI_MAX_SQL_INQUERY_STRING_LENGTH];

	//建立数据库连接
    sqlHandler = mysql_init(NULL);
    if(!sqlHandler)
    {
    	HcuErrorPrint("DBIBFSC: MySQL init failed!\n");
        return FAILURE;
    }
    sqlHandler = mysql_real_connect(sqlHandler, HCU_SYSCFG_LOCAL_DB_HOST_DEFAULT, HCU_SYSCFG_LOCAL_DB_USER_DEFAULT, HCU_SYSCFG_LOCAL_DB_PSW_DEFAULT, HCU_SYSCFG_LOCAL_DB_NAME_DEFAULT, HCU_SYSCFG_LOCAL_DB_PORT_DEFAULT, NULL, 0);  //unix_socket and clientflag not used.
    if (!sqlHandler){
    	HcuErrorPrint("DBIBFSC: MySQL connection failed, Err Code = %s!\n", mysql_error(sqlHandler));
    	mysql_close(sqlHandler);
        return FAILURE;
    }

	//获取数据
    sprintf(strsql, "SELECT * FROM `hcubfsccalibration` WHERE (`deviceid` = '%s')", zHcuSysEngPar.hwBurnId.equLable);
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
		UINT8  index;
		for (index = 1; index <= 3*(HCU_SYSCFG_BFSC_SNR_WS_NBR_MAX-1); index++){
			if (sqlRow[index])  data[index-1] = (UINT32)atol(sqlRow[index]);
		}
	}

	//释放记录集
    mysql_close(sqlHandler);
    return SUCCESS;
}



