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

-- --------------------------------------------------------

--
-- Table structure for table `hcubfscconfigpara`
--

CREATE TABLE `hcubfscconfigpara` (
  `sid` int(4) NOT NULL,
  `confname` varchar(50) DEFAULT NULL,
  `currentconf` char(1) NOT NULL DEFAULT 'N',
  `baseconf` char(1) NOT NULL DEFAULT 'N',
  `confowner` varchar(10) DEFAULT NULL,
  `conficon` varchar(20) DEFAULT NULL,
  `confdescription` varchar(100) DEFAULT NULL,
  `minscalenum` int(4) NOT NULL DEFAULT '1',
  `maxscalenum` int(4) NOT NULL DEFAULT '16',
  `minscalenumstart` int(4) NOT NULL DEFAULT '1',
  `targetweight` int(4) NOT NULL DEFAULT '100000',
  `upperweightlimit` int(4) NOT NULL DEFAULT '100200',
  `proximitmode` int(4) NOT NULL DEFAULT '0',
  `combinationbias` int(4) NOT NULL DEFAULT '1',
  `remaindetectind` int(4) NOT NULL DEFAULT '0',
  `remaindetecttime` int(4) NOT NULL DEFAULT '100',
  `remainobjtreat` int(4) NOT NULL DEFAULT '0',
  `prioritymode` int(4) NOT NULL DEFAULT '0',
  `automode` int(4) NOT NULL DEFAULT '0',
  `averagenum` int(4) NOT NULL DEFAULT '1',
  `loaddetecttime` int(4) NOT NULL DEFAULT '500',
  `loaddetectthread` int(4) NOT NULL DEFAULT '50',
  `emptydetecttime` int(4) NOT NULL DEFAULT '500',
  `emptydetectthread` int(4) NOT NULL DEFAULT '10',
  `standardreadytime` int(4) NOT NULL DEFAULT '500',
  `motorspeed` int(4) NOT NULL DEFAULT '500',
  `motordirection` int(4) NOT NULL DEFAULT '0',
  `rollingstart` int(4) NOT NULL DEFAULT '500',
  `rollingstop` int(4) NOT NULL DEFAULT '500',
  `rollinginterval` int(4) NOT NULL DEFAULT '500',
  `failuredetectvaration` int(4) NOT NULL DEFAULT '500',
  `failuredetecttime` int(4) NOT NULL DEFAULT '500'
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Dumping data for table `hcubfscconfigpara`
--

INSERT INTO `hcubfscconfigpara` (`sid`, `confname`, `currentconf`, `baseconf`, `confowner`, `conficon`, `confdescription`, `minscalenum`, `maxscalenum`, `minscalenumstart`, `targetweight`, `upperweightlimit`, `proximitmode`, `combinationbias`, `remaindetectind`, `remaindetecttime`, `remainobjtreat`, `prioritymode`, `automode`, `averagenum`, `loaddetecttime`, `loaddetectthread`, `emptydetecttime`, `emptydetectthread`, `standardreadytime`, `motorspeed`, `motordirection`, `rollingstart`, `rollingstop`, `rollinginterval`, `failuredetectvaration`, `failuredetecttime`) VALUES
(1, 'apple', 'N', 'Y', 'System', 'apple60.svg', 'standard setting - apple', 1, 16, 1, 100000, 100200, 0, 1, 0, 100, 0, 0, 0, 1, 500, 50, 500, 10, 500, 500, 0, 500, 500, 500, 500, 500),
(2, 'tomato', 'N', 'Y', 'System', 'tomato4.svg', 'Standard setting - tomato', 1, 16, 1, 100000, 100200, 0, 1, 0, 100, 0, 0, 0, 1, 500, 50, 500, 10, 500, 500, 0, 500, 500, 500, 500, 500),
(4, 'tomato-1', 'Y', 'N', 'Bofeng', 'tomato1.svg', 'tomato 1Kg', 1, 16, 1, 15000, 5000, 0, 1, 0, 2, 0, 0, 0, 1, 400, 100, 400, 500, 0, 150, 0, 5000, 0, 0, 0, 0),
(6, 'apple-1', 'N', 'N', 'Bofeng', 'apple60.svg', 'apple 1Kg', 1, 16, 1, 100000, 100200, 0, 1, 0, 100, 0, 0, 0, 1, 500, 50, 500, 10, 500, 500, 0, 500, 500, 500, 500, 500);




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



-- --------------------------------------------------------

--
-- Table structure for table `hcubfsccurrentinfo`
--

CREATE TABLE IF NOT EXISTS `hcubfsccurrentinfo` (
  `deviceid` varchar(20) NOT NULL,
  `timestamp` int(4) NOT NULL,
  `status_00` int(4) DEFAULT NULL,
  `value_00` int(4) DEFAULT NULL,
  `status_01` int(4) DEFAULT NULL,
  `value_01` int(4) DEFAULT NULL,
  `status_02` int(4) DEFAULT NULL,
  `value_02` int(4) DEFAULT NULL,
  `status_03` int(4) DEFAULT NULL,
  `value_03` int(4) DEFAULT NULL,
  `status_04` int(4) DEFAULT NULL,
  `value_04` int(4) DEFAULT NULL,
  `status_05` int(4) DEFAULT NULL,
  `value_05` int(4) DEFAULT NULL,
  `status_06` int(4) DEFAULT NULL,
  `value_06` int(4) DEFAULT NULL,
  `status_07` int(4) DEFAULT NULL,
  `value_07` int(4) DEFAULT NULL,
  `status_08` int(4) DEFAULT NULL,
  `value_08` int(4) DEFAULT NULL,
  `status_09` int(4) DEFAULT NULL,
  `value_09` int(4) DEFAULT NULL,
  `status_10` int(4) DEFAULT NULL,
  `value_10` int(4) DEFAULT NULL,
  `status_11` int(4) DEFAULT NULL,
  `value_11` int(4) DEFAULT NULL,
  `status_12` int(4) DEFAULT NULL,
  `value_12` int(4) DEFAULT NULL,
  `status_13` int(4) DEFAULT NULL,
  `value_13` int(4) DEFAULT NULL,
  `status_14` int(4) DEFAULT NULL,
  `value_14` int(4) DEFAULT NULL,
  `status_15` int(4) DEFAULT NULL,
  `value_15` int(4) DEFAULT NULL,
  `status_16` int(4) DEFAULT NULL,
  `value_16` int(4) DEFAULT NULL,
  `curcomwgt` int(4) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Dumping data for table `hcubfsccurrentinfo`
--

INSERT INTO `hcubfsccurrentinfo` (`deviceid`, `timestamp`, `status_00`, `value_00`, `status_01`, `value_01`, `status_02`, `value_02`, `status_03`, `value_03`, `status_04`, `value_04`, `status_05`, `value_05`, `status_06`, `value_06`, `status_07`, `value_07`, `status_08`, `value_08`, `status_09`, `value_09`, `status_10`, `value_10`, `status_11`, `value_11`, `status_12`, `value_12`, `status_13`, `value_13`, `status_14`, `value_14`, `status_15`, `value_15`, `status_16`, `value_16`, `curcomwgt`) VALUES
('HCU_G301_BFSC_P0001', 20170518, 0, 35483, 0, 36906, 0, 36906, 0, 33144, 0, 36520, 0, 15508, 0, 28983, 0, 26959, 0, 39988, 0, 36037, 0, 39413, 0, 111, 0, 121, 0, 131, 0, 141, 0, 151, 0, 161, 128566);



-- --------------------------------------------------------

--
-- Table structure for table `hcubfscstaticpara`
--

CREATE TABLE `hcubfscstaticpara` (
  `deviceid` varchar(20) NOT NULL,
  `calmaxallowedwgt` int(4) NOT NULL DEFAULT '0',
  `calfullwgt` int(4) NOT NULL DEFAULT '0',
  `caladcgain` int(4) NOT NULL DEFAULT '0',
  `caladcwordrate` int(4) NOT NULL DEFAULT '0',
  `snrstaticzerovalue` int(4) NOT NULL DEFAULT '0',
  `snrtailorvalue` int(4) NOT NULL DEFAULT '0',
  `snrdynzerothread` int(4) NOT NULL DEFAULT '0',
  `snrdynzerohysteresis` int(4) NOT NULL DEFAULT '0'
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COMMENT='calibration and sensor configuration table';

--
-- Dumping data for table `hcubfscstaticpara`
--

INSERT INTO `hcubfscstaticpara` (`deviceid`, `calmaxallowedwgt`, `calfullwgt`, `caladcgain`, `caladcwordrate`, `snrstaticzerovalue`, `snrtailorvalue`, `snrdynzerothread`, `snrdynzerohysteresis`) VALUES
('HCU_G301_BFSC_P0001', 1000000, 100000, 6, 3, 0, 0, 499, 5000);


-- --------------------------------------------------------

--
-- Table structure for table `hcubfsccalibration`
--

CREATE TABLE `hcubfsccalibration` (
  `deviceid` varchar(20) NOT NULL,
  `zeroadc_01` int(4) NOT NULL DEFAULT '0',
  `fulladc_01` int(4) NOT NULL DEFAULT '0',
  `fullwgt_01` int(4) NOT NULL DEFAULT '100000',
  `zeroadc_02` int(4) NOT NULL DEFAULT '0',
  `fulladc_02` int(4) NOT NULL DEFAULT '0',
  `fullwgt_02` int(4) NOT NULL DEFAULT '100000',
  `zeroadc_03` int(4) NOT NULL DEFAULT '0',
  `fulladc_03` int(4) NOT NULL DEFAULT '0',
  `fullwgt_03` int(4) NOT NULL DEFAULT '100000',
  `zeroadc_04` int(4) NOT NULL DEFAULT '0',
  `fulladc_04` int(4) NOT NULL DEFAULT '0',
  `fullwgt_04` int(4) NOT NULL DEFAULT '100000',
  `zeroadc_05` int(4) NOT NULL DEFAULT '0',
  `fulladc_05` int(4) NOT NULL DEFAULT '0',
  `fullwgt_05` int(4) NOT NULL DEFAULT '100000',
  `zeroadc_06` int(4) NOT NULL DEFAULT '0',
  `fulladc_06` int(4) NOT NULL DEFAULT '0',
  `fullwgt_06` int(4) NOT NULL DEFAULT '100000',
  `zeroadc_07` int(4) NOT NULL DEFAULT '0',
  `fulladc_07` int(4) NOT NULL DEFAULT '0',
  `fullwgt_07` int(4) NOT NULL DEFAULT '100000',
  `zeroadc_08` int(4) NOT NULL DEFAULT '0',
  `fulladc_08` int(4) NOT NULL DEFAULT '0',
  `fullwgt_08` int(4) NOT NULL DEFAULT '100000',
  `zeroadc_09` int(4) NOT NULL DEFAULT '0',
  `fulladc_09` int(4) NOT NULL DEFAULT '0',
  `fullwgt_09` int(4) NOT NULL DEFAULT '100000',
  `zeroadc_10` int(4) NOT NULL DEFAULT '0',
  `fulladc_10` int(4) NOT NULL DEFAULT '0',
  `fullwgt_10` int(4) NOT NULL DEFAULT '100000',
  `zeroadc_11` int(4) NOT NULL DEFAULT '0',
  `fulladc_11` int(4) NOT NULL DEFAULT '0',
  `fullwgt_11` int(4) NOT NULL DEFAULT '100000',
  `zeroadc_12` int(4) NOT NULL DEFAULT '0',
  `fulladc_12` int(4) NOT NULL DEFAULT '0',
  `fullwgt_12` int(4) NOT NULL DEFAULT '100000',
  `zeroadc_13` int(4) NOT NULL DEFAULT '0',
  `fulladc_13` int(4) NOT NULL DEFAULT '0',
  `fullwgt_13` int(4) NOT NULL DEFAULT '100000',
  `zeroadc_14` int(4) NOT NULL DEFAULT '0',
  `fulladc_14` int(4) NOT NULL DEFAULT '0',
  `fullwgt_14` int(4) NOT NULL DEFAULT '100000',
  `zeroadc_15` int(4) NOT NULL DEFAULT '0',
  `fulladc_15` int(4) NOT NULL DEFAULT '0',
  `fullwgt_15` int(4) NOT NULL DEFAULT '100000',
  `zeroadc_16` int(4) NOT NULL DEFAULT '0',
  `fulladc_16` int(4) NOT NULL DEFAULT '0',
  `fullwgt_16` int(4) NOT NULL DEFAULT '100000'
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Dumping data for table `hcubfsccalibration`
--

INSERT INTO `hcubfsccalibration` (`deviceid`, `zeroadc_01`, `fulladc_01`, `fullwgt_01`, `zeroadc_02`, `fulladc_02`, `fullwgt_02`, `zeroadc_03`, `fulladc_03`, `fullwgt_03`, `zeroadc_04`, `fulladc_04`, `fullwgt_04`, `zeroadc_05`, `fulladc_05`, `fullwgt_05`, `zeroadc_06`, `fulladc_06`, `fullwgt_06`, `zeroadc_07`, `fulladc_07`, `fullwgt_07`, `zeroadc_08`, `fulladc_08`, `fullwgt_08`, `zeroadc_09`, `fulladc_09`, `fullwgt_09`, `zeroadc_10`, `fulladc_10`, `fullwgt_10`, `zeroadc_11`, `fulladc_11`, `fullwgt_11`, `zeroadc_12`, `fulladc_12`, `fullwgt_12`, `zeroadc_13`, `fulladc_13`, `fullwgt_13`, `zeroadc_14`, `fulladc_14`, `fullwgt_14`, `zeroadc_15`, `fulladc_15`, `fullwgt_15`, `zeroadc_16`, `fulladc_16`, `fullwgt_16`) VALUES
('HCU_G301_BFSC_P0001', 434221, 506206, 100000, 474360, 515157, 100000, 455673, 508063, 100000, 447451, 519359, 100000, 432639, 504921, 100000, 442397, 514862, 100000, 443020, 515286, 100000, 468387, 543587, 100000, 438766, 510644, 100000, 450173, 520188, 100000, 0, 0, 100000, 0, 0, 100000, 0, 0, 100000, 0, 0, 100000, 0, 0, 100000, 0, 0, 100000);




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
    if (wmc_id >= HCU_SYSCFG_BFSC_SNR_WS_NBR_MAX){
    	HcuErrorPrint("DBICOM: Input nodeId too big than HCU_SYSCFG_BFSC_SNR_WS_NBR_MAX = %d\n", HCU_SYSCFG_BFSC_SNR_WS_NBR_MAX);
        return FAILURE;
    }

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
    if( (0xFFFFFFFF == wmc_id) || (0xFFFFFFFF == wmc_weight_value) ){
    	HcuErrorPrint("DBICOM: Input data error!\n");
        mysql_close(sqlHandler);
        return SUCCESS;
    }
    else{
		sprintf(strsql, "UPDATE `hcubfsccurrentinfo` SET timestamp = '%d', status_%02d = '%d', value_%02d = '%d' WHERE (deviceid = '%s')", \
				timestamp, wmc_id, wmc_status, wmc_id, wmc_weight_value, zHcuSysEngPar.hwBurnId.equLable);
    }

    result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBICOM: REPLACE/UPDATE database error: %s\n", mysql_error(sqlHandler));
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
	mysql_free_result(resPtr);
    mysql_close(sqlHandler);
    return SUCCESS;
}

OPSTAT dbi_HcuBfsc_DynamicConfigDataGet(UINT32 config_index, UINT32  dynamicdata[HCU_SYSCFG_BFSC_DB_COLUMN_NUM_MAX])
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
	    sprintf(strsql, "SELECT * FROM `hcubfscconfigpara` WHERE (`currentconf` = 'Y' && `sid` = '%d')" , config_index);
		result = mysql_query(sqlHandler, strsql);
		if(result){  //成功返回0
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
		    	HcuErrorPrint("DBIBFSC: mysql_fetch_row NULL error! strsql = %s\n", strsql);
		        return FAILURE;
			}
			else{
				UINT8  index;
				for (index =0; index < resPtr->field_count; index++){
					if (sqlRow[index] && index<HCU_SYSCFG_BFSC_DB_COLUMN_NUM_MAX)  dynamicdata[index] = (UINT32)atol(sqlRow[index]);
				}
			}

		//释放记录集
		mysql_free_result(resPtr);
	    mysql_close(sqlHandler);
	    return SUCCESS;
	}

OPSTAT dbi_HcuBfsc_StaticConfigDataGet(UINT32  staticdata[HCU_SYSCFG_BFSC_DB_COLUMN_NUM_MAX])
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
	    sprintf(strsql, "SELECT * FROM `hcubfscstaticpara` WHERE (`deviceid` = '%s')", zHcuSysEngPar.hwBurnId.equLable);
		result = mysql_query(sqlHandler, strsql);
		if(result){  //成功返回0
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
		    	HcuErrorPrint("DBIBFSC: mysql_fetch_row NULL error! strsql = %s\n", strsql);
		        return FAILURE;
			}
			else{
				UINT8  index;
				for (index =0; index < resPtr->field_count; index++){
					if (sqlRow[index] && index<HCU_SYSCFG_BFSC_DB_COLUMN_NUM_MAX)  staticdata[index] = (UINT32)atol(sqlRow[index]);
				}
			}

		//释放记录集
		mysql_free_result(resPtr);
	    mysql_close(sqlHandler);
	    return SUCCESS;
	}
