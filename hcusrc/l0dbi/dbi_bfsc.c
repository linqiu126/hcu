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
  `sid` int(4) NOT NULL AUTO_INCREMENT,
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
-- Dumping data for table `hcubfscstadatainfo`
--

INSERT INTO `hcubfscstadatainfo` (`sid`, `statype`, `configid`, `timestamp`, `wsincmatcnt`, `wsincmatwgt`, `wscombtimes`, `wsttttimes`, `wstgvtimes`, `wstttmatcnt`, `wstgvmatcnt`, `wstttmatwgt`, `wstgvmatwgt`, `wsavgttttimes`, `wsavgtttmatcnt`, `wsavgtttmatwgt`) VALUES
(1, 'BFSC_STA_24_HOUR', 1, 1501126545, 0, 0.00, 0, 0, 0, 0, 0, 0.00, 0.00, 0, 0, 0.00),
(2, 'BFSC_STA_LOCAL_UI', 0, 1501641455, 0, 0.00, 0, 0, 0, 0, 0, 0.00, 0.00, 0, 0, 0.00),
(3, 'BFSC_STA_LOCAL_UI', 1, 1501641455, 0, 0.00, 0, 0, 0, 0, 0, 0.00, 0.00, 0, 0, 0.00),
(4, 'BFSC_STA_UP_2_NOW', 1, 1501641451, 0, 0.00, 0, 0, 0, 0, 0, 0.00, 0.00, 0, 0, 0.00);


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

CREATE TABLE `hcubfsccurrentinfo` (
  `timestamp` int(4) NOT NULL PRIMARY KEY,
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

INSERT INTO `hcubfsccurrentinfo` (`timestamp`, `status_00`, `value_00`, `status_01`, `value_01`, `status_02`, `value_02`, `status_03`, `value_03`, `status_04`, `value_04`, `status_05`, `value_05`, `status_06`, `value_06`, `status_07`, `value_07`, `status_08`, `value_08`, `status_09`, `value_09`, `status_10`, `value_10`, `status_11`, `value_11`, `status_12`, `value_12`, `status_13`, `value_13`, `status_14`, `value_14`, `status_15`, `value_15`, `status_16`, `value_16`, `curcomwgt`) VALUES
(1501215098, 0, 35483, 0, 36906, 0, 36906, 0, 33144, 0, 36520, 0, 15508, 0, 28983, 1, 0, 1, 0, 1, 0, 0, 0, 0, 111, 0, 121, 0, 131, 0, 141, 0, 151, 0, 161, 128566);

-- --------------------------------------------------------
--
-- Table structure for table `hcubfscstaticpara`
--
CREATE TABLE `hcubfscstaticpara` (
  `sid` int(4) NOT NULL AUTO_INCREMENT PRIMARY KEY,
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

INSERT INTO `hcubfscstaticpara` (`sid`, `calmaxallowedwgt`, `calfullwgt`, `caladcgain`, `caladcwordrate`, `snrstaticzerovalue`, `snrtailorvalue`, `snrdynzerothread`, `snrdynzerohysteresis`) VALUES
(1, 1000000, 100000, 6, 3, 0, 0, 499, 5000);


-- --------------------------------------------------------
--
-- Table structure for table `hcubfsccalibration`
--

CREATE TABLE `hcubfsccalibration` (
  `sid` int(4) NOT NULL AUTO_INCREMENT PRIMARY KEY,
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

INSERT INTO `hcubfsccalibration` (`sid`, `zeroadc_01`, `fulladc_01`, `fullwgt_01`, `zeroadc_02`, `fulladc_02`, `fullwgt_02`, `zeroadc_03`, `fulladc_03`, `fullwgt_03`, `zeroadc_04`, `fulladc_04`, `fullwgt_04`, `zeroadc_05`, `fulladc_05`, `fullwgt_05`, `zeroadc_06`, `fulladc_06`, `fullwgt_06`, `zeroadc_07`, `fulladc_07`, `fullwgt_07`, `zeroadc_08`, `fulladc_08`, `fullwgt_08`, `zeroadc_09`, `fulladc_09`, `fullwgt_09`, `zeroadc_10`, `fulladc_10`, `fullwgt_10`, `zeroadc_11`, `fulladc_11`, `fullwgt_11`, `zeroadc_12`, `fulladc_12`, `fullwgt_12`, `zeroadc_13`, `fulladc_13`, `fullwgt_13`, `zeroadc_14`, `fulladc_14`, `fullwgt_14`, `zeroadc_15`, `fulladc_15`, `fullwgt_15`, `zeroadc_16`, `fulladc_16`, `fullwgt_16`) VALUES
(1, 434221, 506206, 100000, 474360, 515157, 100000, 455673, 508063, 100000, 447451, 519359, 100000, 432639, 504921, 100000, 442397, 514862, 100000, 443020, 515286, 100000, 468387, 543587, 100000, 438766, 510644, 100000, 450173, 520188, 100000, 0, 0, 100000, 0, 0, 100000, 0, 0, 100000, 0, 0, 100000, 0, 0, 100000, 0, 0, 100000);


*/



//根据VM初始化数据，写入数据库表单中初始化值，方便任务模块的增删，降低研发工作复杂度和工作量
OPSTAT dbi_HcuBfsc_StaDatainfo_save(char *StaType, UINT16 configId, HcuSysMsgIeL3bfscContextStaElement_t *StaDatainfo)
{
	MYSQL *sqlHandler;
    int result = 0;
    char strsql[DBI_MAX_SQL_INQUERY_STRING_LENGTH];
	//MYSQL_RES *resPtr;
	//MYSQL_ROW sqlRow;

    //入参检查：不涉及到生死问题，参数也没啥大问题，故而不需要检查，都可以存入数据库表单中
    char s[20];
    memset(s, 0, sizeof(s));
    strncpy(s, StaType, strlen(StaType)<sizeof(s)?strlen(StaType):sizeof(s));
    UINT32 tmp = time(0);

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

	//DELETE旧的数据
    sprintf(strsql, "DELETE FROM `hcubfscstadatainfo` WHERE (`statype` = '%s' AND `configid` = '%d')", s, configId);
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIBFSC: DELETE data error, statype = %s, configId=%d, err cause = %s\n", StaType, configId, mysql_error(sqlHandler));
        return FAILURE;
	}

	//REPLACE新的数据
    sprintf(strsql, "REPLACE INTO `hcubfscstadatainfo` (statype, configid, timestamp, wsincmatcnt, wsincmatwgt, wscombtimes, wsttttimes, wstgvtimes, wstttmatcnt, wstgvmatcnt, wstttmatwgt, wstgvmatwgt, wsavgttttimes, wsavgtttmatcnt, wsavgtttmatwgt) VALUES \
    		('%s', '%d', '%d', '%d', '%f', '%d', '%d', '%d', '%d', '%d', '%f', '%f', '%d', '%d', '%f')", s, configId, tmp, StaDatainfo->wsIncMatCnt, StaDatainfo->wsIncMatWgt, \
			StaDatainfo->wsCombTimes, StaDatainfo->wsTttTimes, StaDatainfo->wsTgvTimes, StaDatainfo->wsTttMatCnt, StaDatainfo->wsTgvMatCnt, StaDatainfo->wsTttMatWgt, \
			StaDatainfo->wsTgvMatWgt, StaDatainfo->wsAvgTttTimes, StaDatainfo->wsAvgTttMatCnt, StaDatainfo->wsAvgTttMatWgt);
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIBFSC: REPLACE data error, statype = %s, configId=%d, err cause = %s\n", StaType, configId, mysql_error(sqlHandler));
        return FAILURE;
	}

/*    //获取数据
	configId = 3;
    sprintf(strsql, "SELECT sid FROM `hcubfscstadatainfo` WHERE (`statype` = '%s' AND `configid` = '%d')", s, configId);
    HCU_DEBUG_PRINT_FAT("StrSql = %s\n", strsql);
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIBFSC: SELECT data error, StaType = %s, ConfigId=%d,　Err Cause = %s\n", s, configId, mysql_error(sqlHandler));
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
		//插入INSERT新的数据，确保记录是唯一的
    	//INSERT新的数据
        sprintf(strsql, "INSERT INTO `hcubfscstadatainfo` (statype, configid, timestamp, wsincmatcnt, wsincmatwgt, wscombtimes, wsttttimes, wstgvtimes, wstttmatcnt, wstgvmatcnt, wstttmatwgt, wstgvmatwgt, wsavgttttimes, wsavgtttmatcnt, wsavgtttmatwgt) VALUES \
        		('%s', '%d', %d', '%d', '%f', '%d', '%d', '%d', '%d', '%d', '%f', '%f', '%d', '%d', '%f')", s, configId, tmp, StaDatainfo->wsIncMatCnt, StaDatainfo->wsIncMatWgt, \
    			StaDatainfo->wsCombTimes, StaDatainfo->wsTttTimes, StaDatainfo->wsTgvTimes, StaDatainfo->wsTttMatCnt, StaDatainfo->wsTgvMatCnt, StaDatainfo->wsTttMatWgt, \
    			StaDatainfo->wsTgvMatWgt, StaDatainfo->wsAvgTttTimes, StaDatainfo->wsAvgTttMatCnt, StaDatainfo->wsAvgTttMatWgt);
    	result = mysql_query(sqlHandler, strsql);
    	if(result){
    		mysql_free_result(resPtr);
        	mysql_close(sqlHandler);
        	HcuErrorPrint("DBIBFSC: INSERT data error, staType = %s, configId=%d, err cause = %s\n", StaType, configId, mysql_error(sqlHandler));
            return FAILURE;
    	}
	}
	else
	{
		//UPDATE新的数据
		sprintf(strsql, "UPDATE `hcubfscstadatainfo` SET timestamp='%d', \
		wsincmatcnt='%d', \
		wsincmatwgt='%f', \
		wscombtimes='%d', \
		wsttttimes='%d', \
		wstgvtimes='%d', \
		wstttmatcnt='%d', \
		wstgvmatcnt='%d', \
		wstttmatwgt='%f', \
		wstgvmatwgt='%f', \
		wsavgttttimes='%d', \
		wsavgtttmatcnt='%d', \
		wsavgtttmatwgt='%f' \
		WHERE (`statype` = '%s' AND `configid` = '%d')", \
		(int)tmp, \
		StaDatainfo->wsIncMatCnt, \
		StaDatainfo->wsIncMatWgt, \
		StaDatainfo->wsCombTimes, \
		StaDatainfo->wsTttTimes, \
		StaDatainfo->wsTgvTimes, \
		StaDatainfo->wsTttMatCnt, \
		StaDatainfo->wsTgvMatCnt, \
		StaDatainfo->wsTttMatWgt, \
		StaDatainfo->wsTgvMatWgt, \
		StaDatainfo->wsAvgTttTimes, \
		StaDatainfo->wsAvgTttMatCnt, \
		StaDatainfo->wsAvgTttMatWgt, \
		s, configId);

	    HCU_DEBUG_PRINT_FAT("StrSql = %s\n", strsql);
		result = mysql_query(sqlHandler, strsql);
		if(result){
			mysql_free_result(resPtr);
	    	mysql_close(sqlHandler);
	    	HcuErrorPrint("DBIBFSC: UPDATE data error, staType = %s, configId=%d, err cause = %s\n", StaType, configId, mysql_error(sqlHandler));
	        return FAILURE;
		}
	}*/

	//释放记录集
    //mysql_free_result(resPtr);
    mysql_close(sqlHandler);
    //HCU_DEBUG_PRINT_CRT("DBIBFSC: MYSQL release result = %s\n", mysql_error(sqlHandler));
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
    	HcuErrorPrint("DBIBFSC: MySQL init failed!\n");
        return FAILURE;
    }
    sqlHandler = mysql_real_connect(sqlHandler, HCU_SYSCFG_LOCAL_DB_HOST_DEFAULT, HCU_SYSCFG_LOCAL_DB_USER_DEFAULT, HCU_SYSCFG_LOCAL_DB_PSW_DEFAULT, HCU_SYSCFG_LOCAL_DB_NAME_DEFAULT, HCU_SYSCFG_LOCAL_DB_PORT_DEFAULT, NULL, 0);  //unix_socket and clientflag not used.
    if (!sqlHandler){
    	HcuErrorPrint("DBIBFSC: MySQL connection failed, Err Code = %s!\n", mysql_error(sqlHandler));
    	mysql_close(sqlHandler);
    	return FAILURE;
    }

	//REPLACE新的数据
    //UINT32 tmp = time(0);
    sprintf(strsql, "REPLACE INTO `hcubfscfb2ui` (cmdtype, validflag, fbinfo) VALUES ('%d', '%d', '%s')", cmdType, validFlag, info);
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIBFSC: REPLACE data error: %s\n", mysql_error(sqlHandler));
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
    	HcuErrorPrint("DBIBFSC: Input nodeId too big than HCU_SYSCFG_BFSC_SNR_WS_NBR_MAX = %d\n", HCU_SYSCFG_BFSC_SNR_WS_NBR_MAX);
        return FAILURE;
    }

    char s[20];
    memset(s, 0, sizeof(s));

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

	//UPDATE新的数据
    timestamp = time(NULL);
    if( (0xFFFFFFFF == wmc_id) || (0xFFFFFFFF == wmc_weight_value) ){
    	HcuErrorPrint("DBIBFSC: Input data error!\n");
        mysql_close(sqlHandler);
        return SUCCESS;
    }
    else{
		sprintf(strsql, "UPDATE `hcubfsccurrentinfo` SET timestamp = '%d', status_%02d = '%d', value_%02d = '%d' WHERE (1)", \
				timestamp, wmc_id, wmc_status, wmc_id, wmc_weight_value);
    }

    result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIBFSC: UPDATE database error: %s\n", mysql_error(sqlHandler));
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
    	HcuErrorPrint("DBIBFSC: MySQL init failed!\n");
        return FAILURE;
    }
    sqlHandler = mysql_real_connect(sqlHandler, HCU_SYSCFG_LOCAL_DB_HOST_DEFAULT, HCU_SYSCFG_LOCAL_DB_USER_DEFAULT, HCU_SYSCFG_LOCAL_DB_PSW_DEFAULT, HCU_SYSCFG_LOCAL_DB_NAME_DEFAULT, HCU_SYSCFG_LOCAL_DB_PORT_DEFAULT, NULL, 0);  //unix_socket and clientflag not used.
    if (!sqlHandler){
    	HcuErrorPrint("DBIBFSC: MySQL connection failed, Err Code = %s!\n", mysql_error(sqlHandler));
    	mysql_close(sqlHandler);
        return FAILURE;
    }

	//UPDATE新的数据
    sprintf(strsql, "UPDATE `hcubfsccurrentinfo` SET curcomwgt = '%d' WHERE (1)", wgt);

    result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIBFSC: UPDATE data error: %s\n", mysql_error(sqlHandler));
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
    	HcuErrorPrint("DBIBFSC: MySQL init failed!\n");
        return FAILURE;
    }
    sqlHandler = mysql_real_connect(sqlHandler, HCU_SYSCFG_LOCAL_DB_HOST_DEFAULT, HCU_SYSCFG_LOCAL_DB_USER_DEFAULT, HCU_SYSCFG_LOCAL_DB_PSW_DEFAULT, HCU_SYSCFG_LOCAL_DB_NAME_DEFAULT, HCU_SYSCFG_LOCAL_DB_PORT_DEFAULT, NULL, 0);  //unix_socket and clientflag not used.
    if (!sqlHandler){
    	HcuErrorPrint("DBIBFSC: MySQL connection failed, Err Code = %s!\n", mysql_error(sqlHandler));
    	mysql_close(sqlHandler);
        return FAILURE;
    }

	//REPLACE新的数据
    timestamp = time(NULL);
    for(wmc_id = 1; wmc_id <= 16; wmc_id++)
    {
		sprintf(strsql, "UPDATE `hcubfsccurrentinfo` SET timestamp = '%d', status_%02d = '%d' WHERE (1)", timestamp, wmc_id, DBI_BFSC_SNESOR_STATUS_INVALID);
		result = mysql_query(sqlHandler, strsql);
		if(result){
	    	mysql_close(sqlHandler);
	    	HcuErrorPrint("DBIBFSC: UPDATE data error: %s\n", mysql_error(sqlHandler));
	        return FAILURE;
		}
    }

	//释放记录集
    mysql_close(sqlHandler);
    return SUCCESS;
}

OPSTAT dbi_HcuBfsc_CalibrationDataUpdate_adczero(UINT32  adcvalue, UINT32 fullweight, UINT8  sensorid)
{
	MYSQL *sqlHandler;
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

    HcuDebugPrint("DBIBFSC: Receive calibration date: sensorid = %02d, adcvalue = %d, weight = %d!\n", sensorid,adcvalue,fullweight);
    //零值校准数据
    sprintf(strsql, "UPDATE `hcubfsccalibration` SET zeroadc_%02d = '%d' WHERE (1)", sensorid, adcvalue);
	result = mysql_query(sqlHandler, strsql);

    if(result){
    	    	mysql_close(sqlHandler);
    	    	HcuErrorPrint("DBIBFSC: UPDATE data error [func=dbi_HcuBfsc_CalibrationDataUpdate]: %s\n", mysql_error(sqlHandler));
    	        return FAILURE;
    		}

	//释放记录集
    mysql_close(sqlHandler);
    return SUCCESS;
}

OPSTAT dbi_HcuBfsc_CalibrationDataUpdate_adcfull(UINT32  adcvalue, UINT32 fullweight, UINT8  sensorid)
{
	MYSQL *sqlHandler;
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

    HcuDebugPrint("DBIBFSC: Receive calibration date: sensorid = %02d, adcvalue = %d, weight = %d!\n", sensorid,adcvalue,fullweight);
    //满值校准数据
    sprintf(strsql, "UPDATE `hcubfsccalibration` SET fulladc_%02d = '%d', fullwgt_%02d='%d'  WHERE (1)", sensorid, adcvalue, sensorid, fullweight);
	result = mysql_query(sqlHandler, strsql);

    if(result){
    	    	mysql_close(sqlHandler);
    	    	HcuErrorPrint("DBIBFSC: UPDATE data error [func=dbi_HcuBfsc_CalibrationDataUpdate]: %s\n", mysql_error(sqlHandler));
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
    sprintf(strsql, "SELECT * FROM `hcubfsccalibration` WHERE (1)");
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
	    sprintf(strsql, "SELECT * FROM `hcubfscstaticpara` WHERE (1)");
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

OPSTAT dbi_HcuBfsc_TestCmdRespUpdate(UINT8 cmdid, UINT8 validFlag, char strInput[DBI_MAX_FBINFO_STRING_LENGTH])
{
	MYSQL *sqlHandler;
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
	//save data
	sprintf(strsql, "UPDATE `hcubfscfb2ui` SET validflag = '%d', fbinfo = '%s' WHERE (sid = (SELECT MAX(sid) FROM `hcubfscfb2ui` WHERE cmdtype = %d))", validFlag, strInput, cmdid);
	result = mysql_query(sqlHandler, strsql);
	if(result){  //成功返回0
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBIBFSC: UPDATE data error: %s, strsql = %s\n", mysql_error(sqlHandler), strsql);
        return FAILURE;
	}

	//释放记录集
    mysql_close(sqlHandler);
    return SUCCESS;
}
