/*
 * dbicom.c
 *
 *  Created on: 2016年1月5日
 *      Author: test
 */

#include "../l0dbi/dbicom.h"
#include "../l0service/trace.h"
#include "../l0comvm/sysengpar.h"

//安装MYSQL方法
//先参考MYC的LAMP安装大全，然后安装开发库，如下
//sudo apt-get install libmysqlclient-dev

/*
//SYSENGPAR数据表格式
// HcuSysEngPar

-- --------------------------------------------------------

--
-- Database: `hcudb`
--

-- --------------------------------------------------------

--
-- 表的结构 `hcusysengpar`
--

CREATE TABLE `hcusysengpar` (
  `prjname` char(100) NOT NULL,
  `commbackhawlcon` int(1) NOT NULL,
  `hcudbhost` char(20) NOT NULL,
  `hcudbuser` char(20) NOT NULL,
  `hcudbpsw` char(20) NOT NULL,
  `hcudbname` char(20) NOT NULL,
  `hcudbport` int(1) NOT NULL,
  `seriesportformodbus` int(1) NOT NULL,
  `seriesportforgps` int(1) NOT NULL,
  `seriesportforpm25sharp` int(1) NOT NULL,
  `seriesportforqrprinter` int(1) NOT NULL,
  `svraddhttplocal` char(200) NOT NULL,
  `svraddsocketipdefault` char(200) NOT NULL,
  `svraddhttpdefault` char(200) NOT NULL,
  `svrnamedefault` char(12) NOT NULL,
  `cloudftppwdvideo` char(12) NOT NULL,
  `cloudftpuservideo` char(12) NOT NULL,
  `hcuvideoserverdir` char(64) NOT NULL,
  `hcuvideoserverhttp` char(64) NOT NULL,
  `debugmode` int(1) NOT NULL,
  `tracemode` int(1) NOT NULL,
  `browselautostartupflag` int(1) NOT NULL,
  `browselprog` char(20) NOT NULL,
  `browselstartupaddress` char(128) NOT NULL,
  `browselworkingoption` char(128) NOT NULL,
  `windircalibration` int(1) NOT NULL,
  `swdlduration` int(4) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- 转存表中的数据 `hcusysengpar`
--

INSERT INTO `hcusysengpar` (`prjname`, `commbackhawlcon`, `hcudbhost`, `hcudbuser`, `hcudbpsw`, `hcudbname`, `hcudbport`, `seriesportformodbus`, `seriesportforgps`, `seriesportforpm25sharp`, `seriesportforqrprinter`, `svraddhttplocal`, `svraddsocketipdefault`, `svraddhttpdefault`, `svrnamedefault`, `cloudftppwdvideo`, `cloudftpuservideo`, `hcuvideoserverdir`, `hcuvideoserverhttp`, `debugmode`, `tracemode`, `browselautostartupflag`, `browselprog`, `browselstartupaddress`, `browselworkingoption`, `windircalibration`, `swdlduration`) VALUES
('HCU_PRJ_AQYCG10_335D', 1, 'localhost', 'root', '123456', 'hcudb', 3306, 0, 4, 2, 1, 'http://127.0.0.1/test.php', 'http://aaa.bbb/', 'http://www.hkrob.com/mfunhcu/l1mainentry/cloud_callback_hcu.php', 'AQ_HCU', 'avorion', 'Avorion321', '/usr/local/apache_arm/htdocs/avorion/', 'http://192.168.1.232:8000/avorion/', 31, 4, 0, 'firefox', 'http://localhost/yii2basic/web/index.php', '-', 0, 4),
('HCU_PRJ_AQYCG20_RASBERRY', 1, 'localhost', 'root', '123456', 'hcudb', 3306, 0, 4, 1, 1, 'http://127.0.0.1/test.php', '112.64.16.27', 'http://www.hkrob.com/mfunhcu/l1mainentry/cloud_callback_wechat.php', 'XHZN_HCU', 'avorion', 'Avorion321', '/usr/local/apache_arm/htdocs/avorion/', 'http://192.168.1.232:8000/avorion/', 31, 4, 0, 'firefox', 'http://localhost/yii2basic/web/index.php', '-kiosk', 0, 4),
('HCU_PRJ_AQYC_OBSOLETE', 1, 'localhost', 'root', '123456', 'hcudb', 3306, 0, 4, 2, 1, 'http://127.0.0.1/test.php', 'http://aaa.bbb/', 'http://www.hkrob.com/mfunhcu/l1mainentry/cloud_callback_hcu.php', 'AQ_HCU', 'avorion', 'Avorion321', '/usr/local/apache_arm/htdocs/avorion/', 'http://192.168.1.232:8000/avorion/', 31, 4, 0, 'firefox', 'http://localhost/yii2basic/web/index.php', '-', 0, 4),
('HCU_PRJ_BFSC_CBU', 1, 'localhost', 'root', '123456', 'hcudb', 3306, 0, 4, 2, 1, 'http://127.0.0.1/test.php', '121.40.118.33', 'http://www.hkrob.com/mfunhcu/l1mainentry/cloud_callback_hcu.php', 'XHZN_HCU', 'avorion', 'Avorion321', '/usr/local/apache_arm/htdocs/avorion/', 'http://192.168.1.232:8000/avorion/', 28, 4, 0, 'chromium-browser', '--app=http://localhost/bfscui/index.html', '--no-sandbox --kiosk --password-store --start-fullscreen', 0, 4),
('HCU_PRJ_CXGLACM', 1, 'localhost', 'root', '123456', 'hcudb', 3306, 0, 4, 2, 1, 'http://127.0.0.1/test.php', 'http://aaa.bbb/', 'http://www.hkrob.com/mfunhcu/l1mainentry/cloud_callback_hcu.php', 'AQ_HCU', 'avorion', 'Avorion321', '/usr/local/apache_arm/htdocs/avorion/', 'http://192.168.1.232:8000/avorion/', 31, 4, 0, 'firefox', 'http://localhost/yii2basic/web/index.php', '-', 0, 4),
('HCU_PRJ_CXILC', 1, 'localhost', 'root', '123456', 'hcudb', 3306, 0, 4, 2, 1, 'http://127.0.0.1/test.php', 'http://aaa.bbb/', 'http://www.hkrob.com/mfunhcu/l1mainentry/cloud_callback_hcu.php', 'AQ_HCU', 'avorion', 'Avorion321', '/usr/local/apache_arm/htdocs/avorion/', 'http://192.168.1.232:8000/avorion/', 31, 4, 0, 'firefox', 'http://localhost/yii2basic/web/index.php', '-', 0, 4),
('HCU_PRJ_GQYBG40', 1, 'localhost', 'root', '123456', 'hcudb', 3306, 0, 4, 2, 1, 'http://127.0.0.1/test.php', 'http://aaa.bbb/', 'http://www.hkrob.com/mfunhcu/l1mainentry/cloud_callback_hcu.php', 'AQ_HCU', 'avorion', 'Avorion321', '/usr/local/apache_arm/htdocs/avorion/', 'http://192.168.1.232:8000/avorion/', 31, 4, 0, 'firefox', 'http://localhost/yii2basic/web/index.php', '-', 0, 4),
('HCU_PRJ_NBIOT_HPM_QG', 1, 'localhost', 'root', '123456', 'hcudb', 3306, 3, 1, 0, 1, 'http://127.0.0.1/test.php', 'http://aaa.bbb/', 'http://121.40.185.177/xhzn/mfunhcu/l1mainentry/cloud_callback_hcu.php', 'AQ_HCU', 'avorion', 'Avorion321', '/usr/local/apache_arm/htdocs/avorion/', 'http://192.168.1.232:8000/avorion/', 31, 4, 0, 'firefox', 'http://localhost/yii2basic/web/index.php', '-', 0, 4),
('HCU_PRJ_NBIOT_LPM_CJ', 1, 'localhost', 'root', '123456', 'hcudb', 3306, 3, 1, 0, 1, 'http://127.0.0.1/test.php', 'http://aaa.bbb/', 'http://121.40.185.177/xhzn/mfunhcu/l1mainentry/cloud_callback_hcu.php', 'AQ_HCU', 'avorion', 'Avorion321', '/usr/local/apache_arm/htdocs/avorion/', 'http://192.168.1.232:8000/avorion/', 31, 4, 0, 'firefox', 'http://localhost/yii2basic/web/index.php', '-', 0, 4),
('HCU_PRJ_OPWL_OTDR', 1, 'localhost', 'root', '123456', 'hcudb', 3306, 0, 4, 2, 1, 'http://127.0.0.1/test.php', 'http://aaa.bbb/', 'http://www.hkrob.com/mfunhcu/l1mainentry/cloud_callback_hcu.php', 'AQ_HCU', 'avorion', 'Avorion321', '/usr/local/apache_arm/htdocs/avorion/', 'http://192.168.1.232:8000/avorion/', 31, 4, 0, 'firefox', 'http://localhost/yii2basic/web/index.php', '-', 0, 4),
('HCU_PRJ_TBSWRG30', 1, 'localhost', 'root', '123456', 'hcudb', 3306, 3, 1, 0, 1, 'http://127.0.0.1/test.php', 'http://aaa.bbb/', 'http://121.40.185.177/xhzn/mfunhcu/l1mainentry/cloud_callback_hcu.php', 'AQ_HCU', 'avorion', 'Avorion321', '/usr/local/apache_arm/htdocs/avorion/', 'http://192.168.1.232:8000/avorion/', 31, 4, 0, 'firefox', 'http://localhost/yii2basic/web/index.php', '-', 0, 4),
('HCU_PRJ_TEST_MODE', 1, 'localhost', 'root', '123456', 'hcudb', 3306, 3, 1, 0, 1, 'http://127.0.0.1/test.php', 'http://aaa.bbb/', 'http://121.40.185.177/xhzn/mfunhcu/l1mainentry/cloud_callback_hcu.php', 'AQ_HCU', 'avorion', 'Avorion321', '/usr/local/apache_arm/htdocs/avorion/', 'http://192.168.1.232:8000/avorion/', 31, 4, 0, 'firefox', 'http://localhost/yii2basic/web/index.php', '-', 0, 4),
('HCU_PRJ_BFDF_CBU', 1, 'localhost', 'root', '123456', 'hcudb', 3306, 0, 4, 2, 1, 'http://127.0.0.1/test.php', '121.40.118.33', 'http://www.hkrob.com/mfunhcu/l1mainentry/cloud_callback_hcu.php', 'XHZN_HCU', 'avorion', 'Avorion321', '/usr/local/apache_arm/htdocs/avorion/', 'http://192.168.1.232:8000/avorion/', 28, 4, 0, 'chromium-browser', '--app=http://localhost/bfscui/index.html', '--no-sandbox --kiosk --password-store --start-fullscreen', 0, 4),
('HCU_PRJ_BFHS_CBU', 1, 'localhost', 'root', '123456', 'hcudb', 3306, 0, 4, 2, 1, 'http://127.0.0.1/test.php', '121.40.118.33', 'http://www.hkrob.com/mfunhcu/l1mainentry/cloud_callback_hcu.php', 'XHZN_HCU', 'avorion', 'Avorion321', '/usr/local/apache_arm/htdocs/avorion/', 'http://192.168.1.232:8000/avorion/', 28, 4, 0, 'chromium-browser', '--app=http://localhost/bfscui/index.html', '--no-sandbox --kiosk --password-store --start-fullscreen', 0, 4);


//HcuTraceModuleCtr数据表格式
//该数据表单将支持智能初始化
-- --------------------------------------------------------

--
-- Table structure for table `hcutracemodulectr`
--

CREATE TABLE IF NOT EXISTS `hcutracemodulectr` (
  `moduleid` int(2) NOT NULL,
  `modulename` char(15) NOT NULL,
  `modulectrflag` int(1) NOT NULL,
  `moduletoallow` int(1) NOT NULL,
  `moduletorestrict` int(1) NOT NULL,
  `modulefromallow` int(1) NOT NULL,
  `modulefromrestrict` int(1) NOT NULL,
  PRIMARY KEY (`moduleid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Dumping data for table `hcutracemodulectr`
--

INSERT INTO `hcutracemodulectr` (`moduleid`, `modulename`, `modulectrflag`, `moduletoallow`, `moduletorestrict`, `modulefromallow`, `modulefromrestrict`) VALUES
(0, 'TASKMIN', 1, 1, 1, 1, 1),
(1, 'HCUMAIN', 1, 1, 1, 1, 1),
(2, 'HCUVM', 1, 1, 1, 1, 1),
(3, 'TRACE', 1, 1, 1, 1, 1),
(4, 'CONFIG', 1, 1, 1, 1, 1),
(5, 'TIMER', 1, 1, 1, 1, 1),
(6, 'MMC', 1, 1, 1, 1, 1),
(7, 'GPIO', 1, 1, 1, 1, 1),
(8, 'I2C', 1, 1, 1, 1, 1),
(9, 'SPI', 1, 1, 1, 1, 1),
(10, 'PWM', 1, 1, 1, 1, 1),
(11, 'ADC', 1, 1, 1, 1, 1),
(12, 'SWITCH', 1, 1, 1, 1, 1),
(13, 'RELAY', 1, 1, 1, 1, 1),
(14, 'MOTOR', 1, 1, 1, 1, 1),
(15, 'ZEEGBE', 1, 1, 1, 1, 1),
(16, 'GPRS', 1, 1, 1, 1, 1),
(17, 'SPS232', 1, 1, 1, 1, 1),
(18, 'SPS485', 1, 1, 1, 1, 1),
(19, 'BLE', 1, 1, 1, 1, 1),
(20, 'ETHERNET', 1, 1, 1, 1, 1),
(21, 'WIFI', 1, 1, 1, 1, 1),
(22, 'USBNET', 1, 1, 1, 1, 1),
(23, '3G4G', 1, 1, 1, 1, 1),
(24, 'HARDDISK', 1, 1, 1, 1, 1),
(25, 'CAMERA', 1, 1, 1, 1, 1),
(26, 'MICROPHONE', 1, 1, 1, 1, 1),
(27, 'FLASH', 1, 1, 1, 1, 1),
(28, 'GPS', 1, 1, 1, 1, 1),
(29, 'LCD', 1, 1, 1, 1, 1),
(30, 'LED', 1, 1, 1, 1, 1),
(31, 'HWINV', 1, 1, 1, 1, 1),
(32, 'SPSVIRGO', 1, 1, 1, 1, 1),
(33, 'CLOUDVELA', 1, 1, 1, 1, 1),
(34, 'MODBUS', 1, 1, 1, 1, 1),
(35, 'AVORION', 1, 1, 1, 1, 1),
(36, 'I2CBUSLIBRA', 1, 1, 1, 1, 1),
(37, 'SPIBUSARIES', 1, 1, 1, 1, 1),
(38, 'NBIOTCJ188', 1, 1, 1, 1, 1),
(39, 'NBIOTQG376', 1, 1, 1, 1, 1),
(40, 'HSMMP', 1, 1, 1, 1, 1),
(41, 'EMC', 1, 1, 1, 1, 1),
(42, 'HUMID', 1, 1, 1, 1, 1),
(43, 'PM25', 1, 1, 1, 1, 1),
(44, 'TEMP', 1, 1, 1, 1, 1),
(45, 'WINDDIR', 1, 1, 1, 1, 1),
(46, 'WINDSPD', 1, 1, 1, 1, 1),
(47, 'NOISE', 1, 1, 1, 1, 1),
(48, 'AIRPRS', 1, 1, 1, 1, 1),
(49, 'CO1', 1, 1, 1, 1, 1),
(50, 'LIGHTSTR', 1, 1, 1, 1, 1),
(51, 'ALCOHOL', 1, 1, 1, 1, 1),
(52, 'HCHO', 1, 1, 1, 1, 1),
(53, 'TOXICGAS', 1, 1, 1, 1, 1),
(54, 'IWM', 1, 1, 1, 1, 1),
(55, 'IHM', 1, 1, 1, 1, 1),
(56, 'IGM', 1, 1, 1, 1, 1),
(57, 'IPM', 1, 1, 1, 1, 1),
(58, 'SVRCON', 1, 1, 1, 1, 1),
(59, 'SYSPM', 1, 1, 1, 1, 1),
(60, 'SYSSWM', 1, 1, 1, 1, 1),
(61, 'PM25SHARP', 1, 1, 1, 1, 1),
(62, 'CANITFLEO', 1, 1, 1, 1, 1),
(63, 'L3AQYCG10', 1, 1, 1, 1, 1),
(64, 'L3AQYCG20', 1, 1, 1, 1, 1),
(65, 'L3TBSWRG30', 1, 1, 1, 1, 1),
(66, 'L3GQYBG40', 1, 1, 1, 1, 1),
(67, 'L3CXILC', 1, 1, 1, 1, 1),
(68, 'L3CXGLACM', 1, 1, 1, 1, 1),
(69, 'L3NBLPM', 1, 1, 1, 1, 1),
(70, 'L3NBHPM', 1, 1, 1, 1, 1),
(71, 'L3BFSC', 1, 1, 1, 1, 1),
(72, 'BFSCUICOMM', 1, 1, 1, 1, 1),
(73, 'L3OPWLOTDR', 1, 1, 1, 1, 1),
(74, 'TASKMAX', 1, 1, 1, 1, 1);

//HcuTraceMsgCtr数据表格式
//该数据表单将支持智能初始化

--
-- Table structure for table `hcutracemsgctr`
--

CREATE TABLE IF NOT EXISTS `hcutracemsgctr` (
  `msgid` int(2) NOT NULL,
  `msgname` char(70) NOT NULL,
  `msgctrflag` int(1) NOT NULL,
  `msgallow` int(1) NOT NULL,
  `msgrestrict` int(1) NOT NULL,
  PRIMARY KEY (`msgid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Dumping data for table `hcutracemsgctr`
--

INSERT INTO `hcutracemsgctr` (`msgid`, `msgname`, `msgctrflag`, `msgallow`, `msgrestrict`) VALUES
(0, 'MSG_ID_COM_MIN', 0, 0, 0),
(1, 'MSG_ID_COM_INIT', 1, 1, 1),
(2, 'MSG_ID_COM_INIT_FEEDBACK', 1, 1, 1),
(3, 'MSG_ID_COM_RESTART', 1, 1, 1),
(4, 'MSG_ID_COM_STOP', 1, 1, 1),
(5, 'MSG_ID_COM_COMPLETE', 1, 1, 1),
(6, 'MSG_ID_COM_TIME_OUT', 1, 1, 1),
(7, 'MSG_ID_COM_DEBUG_TEST', 1, 1, 1),
(8, 'MSG_ID_COM_HEART_BEAT', 1, 1, 1),
(9, 'MSG_ID_COM_HEART_BEAT_FB', 1, 1, 1),
(10, 'MSG_ID_COM_PROCESS_REBOOT', 1, 1, 1),
(11, 'MSG_ID_COM_ALARM_REPORT', 1, 1, 1),
(12, 'MSG_ID_COM_PM_REPORT', 1, 1, 1),
(13, 'MSG_ID_ETHERNET_CLOUDVELA_DATA_RX', 1, 1, 1),
(14, 'MSG_ID_ETHERNET_NBIOTCJ188_DATA_RX', 1, 1, 1),
(15, 'MSG_ID_ETHERNET_NBIOTQG376_DATA_RX', 1, 1, 1),
(16, 'MSG_ID_ETHERNET_CLOUDVELA_SOCKET_DATA_RX', 1, 1, 1),
(17, 'MSG_ID_WIFI_CLOUDVELA_DATA_RX', 1, 1, 1),
(18, 'MSG_ID_USBNET_CLOUDVELA_DATA_RX', 1, 1, 1),
(19, 'MSG_ID_3G4G_CLOUDVELA_DATA_RX', 1, 1, 1),
(20, 'MSG_ID_SPS232_MODBUS_DATA_RX', 1, 1, 1),
(21, 'MSG_ID_SPS485_MODBUS_DATA_RX', 1, 1, 1),
(22, 'MSG_ID_SPSVIRGO_HSMMP_DATA_RX', 1, 1, 1),
(23, 'MSG_ID_SPSVIRGO_NOISE_DATA_REPORT', 1, 1, 1),
(24, 'MSG_ID_SPSVIRGO_NOISE_CONTROL_FB', 1, 1, 1),
(25, 'MSG_ID_AVORION_HSMMP_DATA_READ_FB',  1, 1, 1),
(26, 'MSG_ID_AVORION_HSMMP_DATA_RX', 1, 1, 1),
(27, 'MSG_ID_BLE_HSMMP_DATA_RX', 1, 1, 1),
(28, 'MSG_ID_BLE_MODBUS_DATA_RX', 1, 1, 1),
(29, 'MSG_ID_LCD_AVORION_DATA_RX',1, 1, 1),
(30,  'MSG_ID_CAMERA_AVORION_DATA_RX',  1, 1, 1),
(31, 'MSG_ID_MICROPHONE_AVORION_DATA_RX', 1, 1, 1),
(32, 'MSG_ID_HWINV_CLOUDVELA_PHY_STATUS_CHG', 1, 1, 1),
(33, 'MSG_ID_NBIOTCJ188_IWM_DATA_REQ', 1, 1, 1),
(34, 'MSG_ID_IWM_NBIOTCJ188_DATA_RESP', 1, 1, 1),
(35, 'MSG_ID_NBIOTCJ188_IWM_CTRL_REQ', 1, 1, 1),
(36, 'MSG_ID_IWM_NBIOTCJ188_CTRL_RESP', 1, 1, 1),
(37, 'MSG_ID_NBIOTCJ188_IHM_DATA_REQ', 1, 1, 1),
(38, 'MSG_ID_IHM_NBIOTCJ188_DATA_RESP', 1, 1, 1),
(39, 'MSG_ID_NBIOTCJ188_IHM_CTRL_REQ', 1, 1, 1),
(40, 'MSG_ID_IHM_NBIOTCJ188_CTRL_RESP', 1, 1, 1),
(41, 'MSG_ID_NBIOTCJ188_IGM_DATA_REQ', 1, 1, 1),
(42, 'MSG_ID_IGM_NBIOTCJ188_DATA_RESP', 1, 1, 1),
(43, 'MSG_ID_NBIOTCJ188_IGM_CTRL_REQ', 1, 1, 1),
(44, 'MSG_ID_IGM_NBIOTCJ188_CTRL_RESP', 1, 1, 1),
(45, 'MSG_ID_NBIOTCJ188_IPM_DATA_REQ', 1, 1, 1),
(46, 'MSG_ID_IPM_NBIOTCJ188_DATA_RESP', 1, 1, 1),
(47, 'MSG_ID_NBIOTCJ188_IPM_CTRL_REQ', 1, 1, 1),
(48, 'MSG_ID_IPM_NBIOTCJ188_CTRL_RESP', 1, 1, 1),
(49, 'MSG_ID_NBIOTQG376_IPM_DATA_REQ', 1, 1, 1),
(50, 'MSG_ID_IPM_NBIOTQG376_DATA_RESP', 1, 1, 1),
(51, 'MSG_ID_NBIOTQG376_IPM_CTRL_REQ', 1, 1, 1),
(52, 'MSG_ID_IPM_NBIOTQG376_CTRL_RESP', 1, 1, 1),
(53, 'MSG_ID_MODBUS_EMC_DATA_REPORT', 1, 1, 1),
(54, 'MSG_ID_MODBUS_EMC_CONTROL_FB', 1, 1, 1),
(55, 'MSG_ID_MODBUS_PM25_DATA_REPORT', 1, 1, 1),
(56, 'MSG_ID_MODBUS_PM25_CONTROL_FB', 1, 1, 1),
(57, 'MSG_ID_MODBUS_WINDDIR_DATA_REPORT', 1, 1, 1),
(58, 'MSG_ID_MODBUS_WINDDIR_CONTROL_FB', 1, 1, 1),
(59, 'MSG_ID_MODBUS_WINDSPD_DATA_REPORT', 1, 1, 1),
(60, 'MSG_ID_MODBUS_WINDSPD_CONTROL_FB', 1, 1, 1),
(61, 'MSG_ID_MODBUS_TEMP_DATA_REPORT', 1, 1, 1),
(62, 'MSG_ID_MODBUS_TEMP_CONTROL_FB', 1, 1, 1),
(63, 'MSG_ID_MODBUS_HUMID_DATA_REPORT', 1, 1, 1),
(64, 'MSG_ID_MODBUS_HUMID_CONTROL_FB', 1, 1, 1),
(65, 'MSG_ID_MODBUS_NOISE_DATA_REPORT', 1, 1, 1),
(66, 'MSG_ID_MODBUS_NOISE_CONTROL_FB', 1, 1, 1),
(67, 'MSG_ID_EMC_MODBUS_DATA_READ', 1, 1, 1),
(68, 'MSG_ID_EMC_MODBUS_CONTROL_CMD', 1, 1, 1),
(69, 'MSG_ID_PM25_MODBUS_DATA_READ', 1, 1, 1),
(70, 'MSG_ID_PM25_MODBUS_CONTROL_CMD', 1, 1, 1),
(71, 'MSG_ID_WINDDIR_MODBUS_DATA_READ', 1, 1, 1),
(72, 'MSG_ID_WINDDIR_MODBUS_CONTROL_CMD', 1, 1, 1),
(73, 'MSG_ID_WINDSPD_MODBUS_DATA_READ', 1, 1, 1),
(74, 'MSG_ID_WINDSPD_MODBUS_CONTROL_CMD', 1, 1, 1),
(75, 'MSG_ID_TEMP_MODBUS_DATA_READ', 1, 1, 1),
(76, 'MSG_ID_TEMP_SPIBUSARIES_DATA_READ', 1, 1, 1),
(77, 'MSG_ID_TEMP_MODBUS_CONTROL_CMD', 1, 1, 1),
(78, 'MSG_ID_TEMP_SPIBUSARIES_CONTROL_CMD', 1, 1, 1),
(79, 'MSG_ID_HUMID_MODBUS_DATA_READ', 1, 1, 1),
(80, 'MSG_ID_HUMID_MODBUS_CONTROL_CMD', 1, 1, 1),
(81, 'MSG_ID_HSMMP_AVORION_DATA_READ', 1, 1, 1),
(82, 'MSG_ID_HSMMP_AVORION_STOP', 1, 1, 1),
(83, 'MSG_ID_NOISE_SPSVIRGO_DATA_READ', 1, 1, 1),
(84, 'MSG_ID_NOISE_SPSVIRGO_CONTROL_CMD', 1, 1, 1),
(85, 'MSG_ID_NOISE_SPSVIRGO_STOP', 1, 1, 1),
(86, 'MSG_ID_NOISE_MODBUS_DATA_READ', 1, 1, 1),
(87, 'MSG_ID_NOISE_MODBUS_CONTROL_CMD', 1, 1, 1),
(88, 'MSG_ID_CLOUDVELA_SYSPM_ALARM_REQ', 1, 1, 1),
(89, 'MSG_ID_SYSPM_CLOUDVELA_ALARM_RESP', 1, 1, 1),
(90, 'MSG_ID_SYSPM_CLOUDVELA_ALARM_REPORT', 1, 1, 1),
(91, 'MSG_ID_CLOUDVELA_SYSPM_ALARM_CONFIRM', 1, 1, 1),
(92, 'MSG_ID_CLOUDVELA_SYSPM_PERFM_REQ', 1, 1, 1),
(93, 'MSG_ID_SYSPM_CLOUDVELA_PERFM_RESP', 1, 1, 1),
(94, 'MSG_ID_SYSPM_CLOUDVELA_PERFM_REPORT', 1, 1, 1),
(95, 'MSG_ID_CLOUDVELA_SYSPM_PERFM_CONFIRM', 1, 1, 1),
(96, 'MSG_ID_CLOUDVELA_SYSSWM_INVENTORY_REQ', 1, 1, 1),
(97, 'MSG_ID_SYSSWM_CLOUDVELA_INVENTORY_RESP', 1, 1, 1),
(98, 'MSG_ID_SYSSWM_CLOUDVELA_INVENTORY_REPORT', 1, 1, 1),
(99, 'MSG_ID_CLOUDVELA_SYSSWM_INVENTORY_CONFIRM', 1, 1, 1),
(100, 'MSG_ID_CLOUDVELA_SYSSWM_SW_PACKAGE_REQ', 1, 1, 1),
(101, 'MSG_ID_SYSSWM_CLOUDVELA_SW_PACKAGE_RESP', 1, 1, 1),
(102, 'MSG_ID_SYSSWM_CLOUDVELA_SW_PACKAGE_REPORT', 1, 1, 1),
(103, 'MSG_ID_CLOUDVELA_SYSSWM_SW_PACKAGE_CONFIRM', 1, 1, 1),
(104, 'MSG_ID_CLOUDVELA_EMC_DATA_REQ', 1, 1, 1),
(105, 'MSG_ID_EMC_CLOUDVELA_DATA_RESP', 1, 1, 1),
(106, 'MSG_ID_CLOUDVELA_EMC_CTRL_REQ', 1, 1, 1),
(107, 'MSG_ID_EMC_CLOUDVELA_CTRL_RESP', 1, 1, 1),
(108, 'MSG_ID_EMC_CLOUDVELA_DATA_REPORT', 1, 1, 1),
(109, 'MSG_ID_CLOUDVELA_EMC_DATA_CONFIRM', 1, 1, 1),
(110, 'MSG_ID_CLOUDVELA_PM25_DATA_REQ', 1, 1, 1),
(111, 'MSG_ID_PM25_CLOUDVELA_DATA_RESP', 1, 1, 1),
(112, 'MSG_ID_CLOUDVELA_PM25_CTRL_REQ',1, 1, 1),
(113, 'MSG_ID_PM25_CLOUDVELA_CTRL_RESP', 1, 1, 1),
(114, 'MSG_ID_PM25_CLOUDVELA_DATA_REPORT', 1, 1, 1),
(115, 'MSG_ID_CLOUDVELA_PM25_DATA_CONFIRM', 1, 1, 1),
(116, 'MSG_ID_CLOUDVELA_TEMP_DATA_REQ', 1, 1, 1),
(117, 'MSG_ID_TEMP_CLOUDVELA_DATA_RESP', 1, 1, 1),
(118, 'MSG_ID_CLOUDVELA_TEMP_CTRL_REQ', 1, 1, 1),
(119, 'MSG_ID_TEMP_CLOUDVELA_CTRL_RESP', 1, 1, 1),
(120, 'MSG_ID_TEMP_CLOUDVELA_DATA_REPORT', 1, 1, 1),
(121, 'MSG_ID_CLOUDVELA_TEMP_DATA_CONFIRM', 1, 1, 1),
(122, 'MSG_ID_CLOUDVELA_HUMID_DATA_REQ', 1, 1, 1),
(123, 'MSG_ID_HUMID_CLOUDVELA_DATA_RESP', 1, 1, 1),
(124, 'MSG_ID_CLOUDVELA_HUMID_CTRL_REQ', 1, 1, 1),
(125, 'MSG_ID_HUMID_CLOUDVELA_CTRL_RESP', 1, 1, 1),
(126, 'MSG_ID_HUMID_CLOUDVELA_DATA_REPORT', 1, 1, 1),
(127, 'MSG_ID_CLOUDVELA_HUMID_DATA_CONFIRM', 1, 1, 1),
(128, 'MSG_ID_CLOUDVELA_WINDDIR_DATA_REQ', 1, 1, 1),
(129, 'MSG_ID_WINDDIR_CLOUDVELA_DATA_RESP', 1, 1, 1),
(130, 'MSG_ID_CLOUDVELA_WINDDIR_CTRL_REQ', 1, 1, 1),
(131, 'MSG_ID_WINDDIR_CLOUDVELA_CTRL_RESP', 1, 1, 1),
(132, 'MSG_ID_WINDDIR_CLOUDVELA_DATA_REPORT', 1, 1, 1),
(133, 'MSG_ID_CLOUDVELA_WINDDIR_DATA_CONFIRM', 1, 1, 1),
(134, 'MSG_ID_CLOUDVELA_WINDSPD_DATA_REQ', 1, 1, 1),
(135, 'MSG_ID_WINDSPD_CLOUDVELA_DATA_RESP', 1, 1, 1),
(136, 'MSG_ID_CLOUDVELA_WINDSPD_CTRL_REQ', 1, 1, 1),
(137, 'MSG_ID_WINDSPD_CLOUDVELA_CTRL_RESP', 1, 1, 1),
(138, 'MSG_ID_WINDSPD_CLOUDVELA_DATA_REPORT', 1, 1, 1),
(139, 'MSG_ID_CLOUDVELA_WINDSPD_DATA_CONFIRM', 1, 1, 1),
(140, 'MSG_ID_CLOUDVELA_HSMMP_DATA_REQ', 1, 1, 1),
(141, 'MSG_ID_HSMMP_CLOUDVELA_DATA_RESP', 1, 1, 1),
(142, 'MSG_ID_CLOUDVELA_HSMMP_CTRL_REQ', 1, 1, 1),
(143, 'MSG_ID_HSMMP_CLOUDVELA_CTRL_RESP', 1, 1, 1),
(144, 'MSG_ID_HSMMP_CLOUDVELA_DATA_REPORT', 1, 1, 1),
(145, 'MSG_ID_CLOUDVELA_HSMMP_DATA_CONFIRM', 1, 1, 1),
(146, 'MSG_ID_CLOUDVELA_NOISE_DATA_REQ', 1, 1, 1),
(147, 'MSG_ID_NOISE_CLOUDVELA_DATA_RESP', 1, 1, 1),
(148, 'MSG_ID_CLOUDVELA_NOISE_CTRL_REQ',  1, 1, 1),
(149, 'MSG_ID_NOISE_CLOUDVELA_CTRL_RESP', 1, 1, 1),
(150, 'MSG_ID_NOISE_CLOUDVELA_DATA_REPORT', 1, 1, 1),
(151, 'MSG_ID_CLOUDVELA_NOISE_DATA_CONFIRM', 1, 1, 1),
(152, 'MSG_ID_CLOUDVELA_YCJK_DATA_REQ', 1, 1, 1),
(153, 'MSG_ID_YCJK_CLOUDVELA_DATA_RESP', 1, 1, 1),
(154, 'MSG_ID_CLOUDVELA_YCJK_CTRL_REQ', 1, 1, 1),
(155, 'MSG_ID_YCJK_CLOUDVELA_CTRL_RESP', 1, 1, 1),
(156, 'MSG_ID_YCJK_CLOUDVELA_DATA_REPORT', 1, 1, 1),
(157, 'MSG_ID_CLOUDVELA_YCJK_DATA_CONFIRM', 1, 1, 1),
(158, 'MSG_ID_CLOUDVELA_AIRPRS_DATA_REQ', 1, 1, 1),
(159, 'MSG_ID_AIRPRS_CLOUDVELA_DATA_RESP', 1, 1, 1),
(160, 'MSG_ID_AIRPRS_CLOUDVELA_DATA_REPORT', 1, 1, 1),
(161, 'MSG_ID_CLOUDVELA_AIRPRS_DATA_CONFIRM', 1, 1, 1),
(162, 'MSG_ID_CLOUDVELA_ALCOHOL_DATA_REQ', 1, 1, 1),
(163, 'MSG_ID_ALCOHOL_CLOUDVELA_DATA_RESP', 1, 1, 1),
(164, 'MSG_ID_ALCOHOL_CLOUDVELA_DATA_REPORT', 1, 1, 1),
(165, 'MSG_ID_CLOUDVELA_ALCOHOL_DATA_CONFIRM', 1, 1, 1),
(166, 'MSG_ID_CLOUDVELA_CO1_DATA_REQ', 1, 1, 1),
(167, 'MSG_ID_CO1_CLOUDVELA_DATA_RESP', 1, 1, 1),
(168, 'MSG_ID_CO1_CLOUDVELA_DATA_REPORT', 1, 1, 1),
(169, 'MSG_ID_CLOUDVELA_CO1_DATA_CONFIRM', 1, 1, 1),
(170, 'MSG_ID_CLOUDVELA_HCHO_DATA_REQ', 1, 1, 1),
(171, 'MSG_ID_HCHO_CLOUDVELA_DATA_RESP', 1, 1, 1),
(172, 'MSG_ID_HCHO_CLOUDVELA_DATA_REPORT', 1, 1, 1),
(173, 'MSG_ID_CLOUDVELA_HCHO_DATA_CONFIRM', 1, 1, 1),
(174, 'MSG_ID_CLOUDVELA_LIGHTSTR_DATA_REQ', 1, 1, 1),
(175, 'MSG_ID_LIGHTSTR_CLOUDVELA_DATA_RESP', 1, 1, 1),
(176, 'MSG_ID_LIGHTSTR_CLOUDVELA_DATA_REPORT', 1, 1, 1),
(177, 'MSG_ID_CLOUDVELA_LIGHTSTR_DATA_CONFIRM', 1, 1, 1),
(178, 'MSG_ID_CLOUDVELA_PM25SP_DATA_REQ', 1, 1, 1),
(179, 'MSG_ID_PM25SP_CLOUDVELA_DATA_RESP', 1, 1, 1),
(180, 'MSG_ID_PM25SP_CLOUDVELA_DATA_REPORT', 1, 1, 1),
(181, 'MSG_ID_CLOUDVELA_PM25SP_DATA_CONFIRM', 1, 1, 1),
(182, 'MSG_ID_CLOUDVELA_TOXICGAS_DATA_REQ', 1, 1, 1),
(183, 'MSG_ID_TOXICGAS_CLOUDVELA_DATA_RESP', 1, 1, 1),
(184, 'MSG_ID_TOXICGAS_CLOUDVELA_DATA_REPORT', 1, 1, 1),
(185, 'MSG_ID_CLOUDVELA_TOXICGAS_DATA_CONFIRM', 1, 1, 1),
(186, 'MSG_ID_CANITFLEO_DATA_REPORT', 1, 1, 1),
(187, 'MSG_ID_CANITFLEO_SYSSWM_INVENTORY_REPORT', 1, 1, 1),
(188, 'MSG_ID_SYSSWM_CANITFLEO_INVENTORY_CONFIRM', 1, 1, 1),
(189, 'MSG_ID_CANITFLEO_SYSSWM_SW_PACKAGE_REPORT', 1, 1, 1),
(190, 'MSG_ID_SYSSWM_CANITFLEO_SW_PACKAGE_CONFIRM', 1, 1, 1),
(191, 'MSG_ID_CLOUDVELA_L3BFSC_DATA_REQ', 1, 1, 1),
(192, 'MSG_ID_L3BFSC_CLOUDVELA_DATA_RESP', 1, 1, 1),
(193, 'MSG_ID_L3BFSC_CLOUDVELA_DATA_REPORT', 1, 1, 1),
(194, 'MSG_ID_CLOUDVELA_L3BFSC_DATA_CONFIRM', 1, 1, 1),
(195, 'MSG_ID_L3BFSC_CLOUDVELA_EVENT_REPORT', 1, 1, 1),
(196, 'MSG_ID_CLOUDVELA_L3BFSC_EVENT_CONFIRM', 1, 1, 1),
(197, 'MSG_ID_CLOUDVELA_L3BFSC_CTRL_REQ', 1, 1, 1),
(198, 'MSG_ID_L3BFSC_CLOUDVELA_CTRL_RESP', 1, 1, 1),
(199, 'MSG_ID_L3BFSC_CLOUDVELA_STATISTIC_REPORT', 1, 1, 1),
(200, 'MSG_ID_CLOUDVELA_L3BFSC_STATISTIC_CONFIRM', 1, 1, 1),
(201, 'MSG_ID_L3BFSC_CAN_SYS_CFG_REQ', 1, 1, 1),
(202, 'MSG_ID_CAN_L3BFSC_SYS_CFG_RESP', 1, 1, 1),
(203, 'MSG_ID_L3BFSC_CAN_SYS_START_REQ', 1, 1, 1),
(204, 'MSG_ID_CAN_L3BFSC_SYS_START_RESP', 1, 1, 1),
(205, 'MSG_ID_L3BFSC_CAN_SYS_STOP_REQ', 1, 1, 1),
(206, 'MSG_ID_CAN_L3BFSC_SYS_STOP_RESP', 1, 1, 1),
(207, 'MSG_ID_CAN_L3BFSC_WS_NEW_READY_EVENT', 1, 1, 1),
(208, 'MSG_ID_L3BFSC_CAN_WS_COMB_OUT', 1, 1, 1),
(209, 'MSG_ID_CAN_L3BFSC_WS_COMB_OUT_FB', 1, 1, 1),
(210, 'MSG_ID_L3BFSC_CAN_WS_GIVE_UP', 1, 1, 1),
(211, 'MSG_ID_CAN_L3BFSC_WS_GIVE_UP_FB', 1, 1, 1),
(212, 'MSG_ID_L3BFSC_CAN_ERROR_INQ_CMD_REQ', 1, 1, 1),
(213, 'MSG_ID_CAN_L3BFSC_ERROR_INQ_CMD_RESP', 1, 1, 1),
(214, 'MSG_ID_USBCAN_L2FRAME_RCV', 1, 1, 1),
(215, 'MSG_ID_INOTIFY_UICOMM_FILE_CHANGE_IND', 1, 1, 1),
(216, 'MSG_ID_UICOMM_L3BFSC_CMD_REQ', 1, 1, 1),
(217, 'MSG_ID_L3BFSC_UICOMM_CMD_RESP', 1, 1, 1),
(218, 'MSG_ID_UICOMM_L3BFSC_CFG_REQ', 1, 1, 1),
(219, 'MSG_ID_L3BFSC_UICOMM_CFG_RESP', 1, 1, 1),
(220, 'MSG_ID_UICOMM_CAN_TEST_CMD_REQ', 1, 1, 1),
(221, 'MSG_ID_CAN_UICOMM_TEST_CMD_RESP', 1, 1, 1),
(222, 'MSG_ID_L3AQYC_EXG_CTRL_REQ', 1, 1, 1),
(223, 'MSG_ID_L3AQYC_EXG_CTRL_RESP', 1, 1, 1),
(224, 'MSG_ID_L3AQYC_EXG_DATA_REPORT', 1, 1, 1),
(225, 'MSG_ID_CLOUDVELA_LLCZHB_FRAME_REQ', 1, 1, 1),
(226, 'MSG_ID_LLCZHB_CLOUDVELA_FRAME_RESP', 1, 1, 1),
(227, 'MSG_ID_LLCZHB_L3MOD_CTRL_REQ', 1, 1, 1),
(228, 'MSG_ID_L3MOD_LLCZHB_CTRL_RESP', 1, 1, 1),
(229, 'MSG_ID_L3MOD_LLCZHB_DATA_REPORT', 1, 1, 1),
(230, 'MSG_ID_ZHBL3MOD_EXG_CTRL_REQ', 1, 1, 1),
(231, 'MSG_ID_ZHBL3MOD_EXG_CTRL_RESP', 1, 1, 1),
(232, 'MSG_ID_ZHBL3MOD_EXG_DATA_REPORT', 1, 1, 1),
(233, 'MSG_ID_COM_MAX', 0, 0, 0);


//hcusysengtimer数据表格式
//该数据表单将支持智能初始化
-- --------------------------------------------------------

-- --------------------------------------------------------

--
-- Table structure for table `hcusysengtimer`
--

CREATE TABLE IF NOT EXISTS `hcusysengtimer` (
  `timerid` int(2) NOT NULL,
  `timername` char(40) NOT NULL,
  `granularity` int(1) NOT NULL,
  `duration` int(1) NOT NULL,
  PRIMARY KEY (`timerid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Table structure for table `hcusysengtimer`
--

INSERT INTO `hcusysengtimer` (`timerid`, `timername`, `granularity`, `duration`) VALUES
(0, 'TID_MIN', 1, 0),
(1, 'TID_1S_MIN', 1, 0),
(2, 'TID_1S_SVRCON_INIT_FB', 1, 10),
(3, 'TID_1S_LED_GALOWAG_SCAN', 1, 1),
(4, 'TID_1S_EMC_PERIOD_READ', 1, 100),
(5, 'TID_1S_EMC_MODBUS_FB', 1, 10),
(6, 'TID_1S_PM25_PERIOD_READ', 1, 600),
(7, 'TID_1S_PM25_MODBUS_FB', 1, 10),
(8, 'TID_1S_WINDDIR_PERIOD_READ', 1, 600),
(9, 'TID_1S_WINDDIR_MODBUS_FB', 1, 10),
(10, 'TID_1S_WINDSPD_PERIOD_READ', 1, 600),
(11, 'TID_1S_WINDSPD_MODBUS_FB', 1, 10),
(12, 'TID_1S_TEMP_PERIOD_READ', 1, 600),
(13, 'TID_1S_TEMP_FB', 1, 10),
(14, 'TID_1S_HUMID_PERIOD_READ', 1, 600),
(15, 'TID_1S_HUMID_MODBUS_FB', 1, 10),
(16, 'TID_1S_NOISE_PERIOD_READ', 1, 600),
(17, 'TID_1S_NOISE_MODBUS_FB', 1, 10),
(18, 'TID_1S_NOISE_SPSVIRGO_FB', 1, 10),
(19, 'TID_1S_HSMMP_PERIOD_AVORION_READ', 1, 600),
(20, 'TID_1S_HSMMP_AVORION_FB', 1, 10),
(21, 'TID_1S_CLOUDVELA_PERIOD_LINK_HBT', 1, 60),
(22, 'TID_1S_CLOUDVELA_SEND_DATA_BACK', 1, 10),
(23, 'TID_1S_CLOUDVELA_PRD_SWDB_VER_REP', 1, 900),
(24, 'TID_1S_NBIOTCJ188_PERIOD_LINK_HBT', 1, 600),
(25, 'TID_1S_NBIOTCJ188_SEND_DATA_BACK', 1, 10),
(26, 'TID_1S_NBIOTQG376_PERIOD_LINK_HBT', 1, 600),
(27, 'TID_1S_NBIOTQG376_SEND_DATA_BACK', 1, 10),
(28, 'TID_1S_AIRPRS_PERIOD_READ', 1, 600),
(29, 'TID_1S_CO1_PERIOD_READ', 1, 600),
(30, 'TID_1S_LIGHTSTR_PERIOD_READ', 1, 600),
(31, 'TID_1S_ALCOHOL_PERIOD_READ', 1, 600),
(32, 'TID_1S_HCHO_PERIOD_READ', 1, 600),
(33, 'TID_1S_TOXICGAS_PERIOD_READ', 1, 600),
(34, 'TID_1S_PM25SHARP_PERIOD_READ', 1, 600),
(35, 'TID_1S_IWM_PERIOD_READ', 1, 600),
(36, 'TID_1S_IHM_PERIOD_READ', 1, 600),
(37, 'TID_1S_IGM_PERIOD_READ', 1, 600),
(38, 'TID_1S_IPM_PERIOD_READ', 1, 600),
(39, 'TID_1S_SYSPM_PERIOD_WORKING', 1, 3600),
(40, 'TID_1S_CANITFLEO_WORKING_SCAN', 1, 10),
(41, 'TID_1S_L3BFSC_PERIOD_READ', 1, 10),
(42, 'TID_1S_L3BFSC_INIT_FB_WAIT', 1, 15),
(43, 'TID_1S_L3BFSC_TTT_WAIT_FB', 1, 15),
(44, 'TID_1S_L3BFSC_TGU_WAIT_FB', 1, 15),
(45, 'TID_1S_L3BFSC_PERIOD_ERROR_SCAN', 1, 60),
(46, 'TID_1S_BFSCUICOMM_PERIOD_READ', 1, 600),
(47, 'TID_1S_L3AQYCG10_PERIOD_READ', 1, 600),
(48, 'TID_1S_L3AQYCG20_PERIOD_READ', 1, 600),
(49, 'TID_1S_L3TBSWRG30_PERIOD_READ', 1, 600),
(50, 'TID_1S_L3GQYBG40_PERIOD_READ', 1, 600),
(51, 'TID_1S_L3CXGLACM_PERIOD_READ', 1, 600),
(52, 'TID_1S_L3CXILC_PERIOD_READ', 1, 600),
(53, 'TID_1S_L3NBHPM_PERIOD_READ', 1, 600),
(54, 'TID_1S_L3NBLPM_PERIOD_READ', 1, 600),
(55, 'TID_1S_L3OPWLOTDR_PERIOD_READ', 1, 600),
(56, 'TID_1S_MAX', 1, 0),
(57, 'TID_10MS_MIN', 2, 0),
(58, 'TID_10MS_SVRCON_TEST', 2, 10000),
(59, 'TID_10MS_CANITFLEO_SIMULATION_DATA', 2, 200),
(60, 'TID_10MS_MAX', 2, 0),
(61, 'TID_1MS_MIN', 3, 0),
(62, 'TID_1MS_SVRCON_TEST', 3, 100),
(63, 'TID_1MS_MAX', 3, 0),
(64, 'TID_MAX', 1, 0);


*/


//查询满足条件的第一条记录
//deviceid是入参，指针为出参
OPSTAT dbi_HcuSysEngPar_inqury(HcuSysEngParTab_t *engPar, char *prjname)
{
	MYSQL *sqlHandler;
	MYSQL_RES *resPtr;
	MYSQL_ROW sqlRow;
    int result = 0;
    char strsql[DBI_MAX_SQL_INQUERY_STRING_LENGTH];

    //入参检查：不涉及到生死问题，参数也没啥大问题，故而不需要检查，都可以存入数据库表单中
    if ((engPar == NULL) || (prjname == NULL)){
    	HcuErrorPrint("DBICOM: Input parameter NULL pointer!\n");
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
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBICOM: MySQL connection failed: %s\n", mysql_error(sqlHandler));

        return FAILURE;
    }

	//获取数据
    sprintf(strsql, "SELECT * FROM `hcusysengpar` WHERE (`prjname` = '%s')", prjname);
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBICOM: Inqury data error: %s\n", mysql_error(sqlHandler));
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
		mysql_free_result(resPtr);
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBICOM: mysql_fetch_row NULL error!\n");
        return FAILURE;
	}
	else{
		int index;  //.PRJNAME是INDEX=0的主键
		index = 0;
		//项目名称
		if(sqlRow[index]) strncpy(engPar->prjname, sqlRow[index++], HCU_SYSENG_PAR_ELEMENT_DB_LEN-1);
		//通信部分=》模块和通信机制的选择，留给了VM任务启动初始化静态配置部分，不再这里进行工参干预，以提高研发效率
		if(sqlRow[index]) engPar->comm.commBackHawlCon = (UINT8)(atol(sqlRow[index++]) & 0xFF);

		//数据库部分
		if(sqlRow[index]) strncpy(engPar->dbi.hcuDbHost, sqlRow[index++], HCU_SYSENG_PAR_ELEMENT_DB_LEN-1);
		if(sqlRow[index]) strncpy(engPar->dbi.hcuDbUser, sqlRow[index++], HCU_SYSENG_PAR_ELEMENT_DB_LEN-1);
		if(sqlRow[index]) strncpy(engPar->dbi.hcuDbPsw, sqlRow[index++], HCU_SYSENG_PAR_ELEMENT_DB_LEN-1);
		if(sqlRow[index]) strncpy(engPar->dbi.hcuDbName, sqlRow[index++], HCU_SYSENG_PAR_ELEMENT_DB_LEN-1);
		if(sqlRow[index]) engPar->dbi.hcuDbPort = (UINT8)(atol(sqlRow[index++]) & 0xFF);

		//Series Port
		if(sqlRow[index]) engPar->serialport.SeriesPortForModbus = (UINT8)(atol(sqlRow[index++]) & 0xFF);
		if(sqlRow[index]) engPar->serialport.SeriesPortForGPS = (UINT8)(atol(sqlRow[index++]) & 0xFF);
		if(sqlRow[index]) engPar->serialport.SeriesPortForPm25Sharp = (UINT8)(atol(sqlRow[index++]) & 0xFF);
		if(sqlRow[index]) engPar->serialport.SeriesPortForQrPrinter = (UINT8)(atol(sqlRow[index++]) & 0xFF);

		//后台链接部分
		if(sqlRow[index]) strncpy(engPar->cloud.svrAddrHttpLocal, sqlRow[index++], HCU_SYSENG_PAR_CLOUD_ADDR_LEN_MAX-1);
		if(sqlRow[index]) strncpy(engPar->cloud.svrAddrSocketipDefault, sqlRow[index++], HCU_SYSENG_PAR_CLOUD_ADDR_LEN_MAX-1);
		if(sqlRow[index]) strncpy(engPar->cloud.svrAddrHttpHome, sqlRow[index++], HCU_SYSENG_PAR_CLOUD_ADDR_LEN_MAX-1);
		if(sqlRow[index]) strncpy(engPar->cloud.svrNameDefault, sqlRow[index++], HCU_SYSENG_PAR_CLOUD_NAME_LEN_MAX-1);

		//For HCU SW download
//		if(sqlRow[index]) strncpy(engPar->cloud.cloudFtpAdd, sqlRow[index++], HCU_SYSENG_PAR_CLOUD_ADDR_LEN_MAX-1);
//		if(sqlRow[index]) strncpy(engPar->cloud.cloudFtpUser, sqlRow[index++], HCU_SYSENG_PAR_CLOUD_NAME_LEN_MAX-1);
//		if(sqlRow[index]) strncpy(engPar->cloud.cloudFtpPwd, sqlRow[index++], HCU_SYSENG_PAR_CLOUD_NAME_LEN_MAX-1);

		//For HCU video upload
		if(sqlRow[index]) strncpy(engPar->cloud.cloudFtpUserVideo, sqlRow[index++], HCU_SYSENG_PAR_CLOUD_NAME_LEN_MAX-1);
		if(sqlRow[index]) strncpy(engPar->cloud.cloudFtpPwdVideo, sqlRow[index++], HCU_SYSENG_PAR_CLOUD_NAME_LEN_MAX-1);
//		if(sqlRow[index]) strncpy(engPar->swm.hcuSwDownloadDir, sqlRow[index++], HCU_SYSENG_PAR_ELEMENT_SWDOWNLOAD_LEN-1);
//		if(sqlRow[index]) strncpy(engPar->swm.hcuSwActiveDir, sqlRow[index++], HCU_SYSENG_PAR_ELEMENT_SWDOWNLOAD_LEN-1);
//		if(sqlRow[index]) strncpy(engPar->swm.hcuSwBackupDir, sqlRow[index++], HCU_SYSENG_PAR_ELEMENT_SWDOWNLOAD_LEN-1);

		//视频服务器部分
		if(sqlRow[index]) strncpy(engPar->videoSev.hcuVideoServerDir, sqlRow[index++], HCU_SYSENG_PAR_ELEMENT_VIDEO_SERVER_LEN-1);
		if(sqlRow[index]) strncpy(engPar->videoSev.hcuVideoServerHttp, sqlRow[index++], HCU_SYSENG_PAR_ELEMENT_VIDEO_SERVER_LEN-1);
		//printf ("DBICOM: index = %d, hcuVideoServerHttp = %s\n", index, engPar->videoSev.hcuVideoServerHttp);
		//调试部分
		if(sqlRow[index]) engPar->debugMode = (UINT8)(atol(sqlRow[index++]) & 0xFF);
		//TRACE部分
		if(sqlRow[index]) engPar->traceMode = (UINT8)(atol(sqlRow[index++]) & 0xFF);
		//HcuDebugPrint("DBICOM: Sysengpar read in, Index=%d, TraceMode=%d\n", index-1, engPar->traceMode);
		//LocalUI本地界面启动部分
		if(sqlRow[index]) engPar->localUI.browselAutoStartUpFlag = (UINT8)(atol(sqlRow[index++]) & 0xFF);
		//HcuDebugPrint("DBICOM: Sysengpar read in, Index=%d, browselAutoStartUpFlag=%d\n", index-1, engPar->localUI.browselAutoStartUpFlag);
		if(sqlRow[index]) strncpy(engPar->localUI.browselProg, sqlRow[index++], HCU_SYSENG_PAR_ELEMENT_LOCAL_UI_PROG_LEN-1);
		if(sqlRow[index]) strncpy(engPar->localUI.browselStartUpAddress, sqlRow[index++], HCU_SYSENG_PAR_ELEMENT_LOCAL_UI_ADD_LEN-1);
		if(sqlRow[index]) strncpy(engPar->localUI.browselWorkingOption, sqlRow[index++], HCU_SYSENG_PAR_ELEMENT_LOCAL_UI_OPTION_LEN-1);

		//风向角度校准值
		if(sqlRow[index]) engPar->winddircalibration = (UINT8)(atol(sqlRow[index++]) & 0xFF);

		//软件下载的时间周期长度设置
		if(sqlRow[index]) engPar->swm.swDlDuration = (UINT32)(atol(sqlRow[index++]) & 0xFFFFFFFF);
	}

	//释放记录集
	mysql_free_result(resPtr);
    mysql_close(sqlHandler);
    return SUCCESS;
}


//查询所有基于模块控制的TRACE记录，并存入全局控制变量
OPSTAT dbi_HcuTraceModuleCtr_inqury(HcuSysEngParTab_t *engPar)
{
	MYSQL *sqlHandler;
	MYSQL_RES *resPtr;
	MYSQL_ROW sqlRow;
    int result = 0;
    char strsql[DBI_MAX_SQL_INQUERY_STRING_LENGTH];

    //入参检查：不涉及到生死问题，参数也没啥大问题，故而不需要检查，都可以存入数据库表单中
    if (engPar == NULL){
    	HcuErrorPrint("DBICOM: Input parameter NULL pointer!\n");
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
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBICOM: MySQL connection failed: %s\n", mysql_error(sqlHandler));
        return FAILURE;
    }

	//获取数据
    sprintf(strsql, "SELECT * FROM `hcutracemodulectr` WHERE 1");
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBICOM: Inqury data error: %s\n", mysql_error(sqlHandler));
        return FAILURE;
	}

	//查具体的结果
	resPtr = mysql_use_result(sqlHandler);
	if (!resPtr){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBICOM: mysql_use_result error!\n");
        return FAILURE;
	}

	//循环读取所有的记录
	while ((sqlRow = mysql_fetch_row(resPtr)) != NULL)
	{
		UINT16 taskid = 0;
		int index = 0;
		int tableRow = 0;
		if(sqlRow[index]) taskid = (UINT16)(atol(sqlRow[index++]) & 0xFFFF);
		if (taskid >= HCU_SYSDIM_TASK_NBR_MAX){
			HcuErrorPrint("DBICOM: Error ModuleID populated, very dangerous to crash whole system! ModueId = %d\n", taskid);
			mysql_free_result(resPtr);
		    mysql_close(sqlHandler);
		    return FAILURE;
		}

		engPar->traceList.mod[taskid].moduleId = taskid;
		tableRow = hcu_vm_search_task_static_cfg_table_of_row(taskid);
		if(sqlRow[index]) strncpy(engPar->traceList.mod[taskid].moduleName, sqlRow[index++], HCU_SYSDIM_TASK_NAME_LEN_MAX-1);
		//不能再做比较，这个比较必须到后期再做
		if (strcmp(engPar->traceList.mod[taskid].moduleName, zHcuVmCtrTaskStaticCfg[tableRow].taskInputName)){
			HCU_DEBUG_PRINT_CRT("DBICOM: Error Module name populated, taskid=%d, Input/sysEngPar=[%s], StaticTable=[%s], tableRow=%d.\n", taskid, engPar->traceList.mod[taskid].moduleName, zHcuVmCtrTab.task[tableRow].taskName, tableRow);
		}
		if(sqlRow[index]) engPar->traceList.mod[taskid].moduleCtrFlag = (UINT8)(atol(sqlRow[index++]) & 0xFF);
		if(sqlRow[index]) engPar->traceList.mod[taskid].moduleToAllow = (UINT8)(atol(sqlRow[index++]) & 0xFF);
		if(sqlRow[index]) engPar->traceList.mod[taskid].moduleToRestrict = (UINT8)(atol(sqlRow[index++]) & 0xFF);
		if(sqlRow[index]) engPar->traceList.mod[taskid].moduleFromAllow = (UINT8)(atol(sqlRow[index++]) & 0xFF);
		if(sqlRow[index]) engPar->traceList.mod[taskid].moduleFromRestrict = (UINT8)(atol(sqlRow[index++]) & 0xFF);

	}

	//释放记录集
	mysql_free_result(resPtr);
    mysql_close(sqlHandler);
    return SUCCESS;
}

//根据VM初始化数据，写入数据库表单中初始化值，方便任务模块的增删，降低研发工作复杂度和工作量
OPSTAT dbi_HcuTraceModuleCtr_engpar_intelligence_init(void)
{
	MYSQL *sqlHandler;
    int result = 0, item = 0;
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
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBICOM: MySQL connection failed: %s\n", mysql_error(sqlHandler));
        return FAILURE;
    }

	//清除表格已有数据
    sprintf(strsql, "TRUNCATE table `hcutracemodulectr`");
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBICOM: Delete data error: %s, result = %d\n", mysql_error(sqlHandler), result);
        return FAILURE;
	}

	//读取VM中模块初始化表单内容
	//扫描输入表单，起始必须是TASK_ID_MIN条目
	if (zHcuVmCtrTaskStaticCfg[0].taskInputId != TASK_ID_MIN){
		HcuErrorPrint("DBICOM: Initialize HCU-VM failure, task input configuration error!\n");
		return FAILURE;
	}
	//扫描输入表单，以TASK_ID_MAX为终止条目：2倍配置项最多
	for(item=1; item < (2*HCU_SYSDIM_TASK_NBR_MAX); item++){
		if(zHcuVmCtrTaskStaticCfg[item].taskInputId == TASK_ID_MAX){
			break;
		}
		if ((zHcuVmCtrTaskStaticCfg[item].taskInputId <= TASK_ID_MIN) || (zHcuVmCtrTaskStaticCfg[item].taskInputId > TASK_ID_MAX)){
			HcuErrorPrint("DBICOM: Initialize HCU-VM failure, task input configuration error!\n");
			return FAILURE;
		}
	}

	//从任务配置输入区域读取参数到系统任务表，一旦遇到TASK_ID_MAX就终止
	item = 0;
	while(zHcuVmCtrTaskStaticCfg[item].taskInputId != TASK_ID_MAX){
		//REPLACE新的数据
	    sprintf(strsql, "REPLACE INTO `hcutracemodulectr` (moduleid, modulename, modulectrflag, moduletoallow, moduletorestrict, modulefromallow, modulefromrestrict) VALUES \
	    		('%d', '%s', '%d', '%d', '%d', '%d', '%d')", zHcuVmCtrTaskStaticCfg[item].taskInputId, zHcuVmCtrTaskStaticCfg[item].taskInputName, \
				zHcuVmCtrTaskStaticCfg[item].traceCtrFlag, zHcuVmCtrTaskStaticCfg[item].traceModToAllowFlag, zHcuVmCtrTaskStaticCfg[item].traceModToRestrictFlag, \
				zHcuVmCtrTaskStaticCfg[item].traceModFromAllowFlag, zHcuVmCtrTaskStaticCfg[item].traceModFromRestrictFlag);
		result = mysql_query(sqlHandler, strsql);
		if(result){
	    	mysql_close(sqlHandler);
	    	HcuErrorPrint("DBICOM: REPLACE data error: %s\n", mysql_error(sqlHandler));
	        return FAILURE;
		}
		item++;
	}
	//最后一项必定是TASK_ID_MAX
    sprintf(strsql, "REPLACE INTO `hcutracemodulectr` (moduleid, modulename, modulectrflag, moduletoallow, moduletorestrict, modulefromallow, modulefromrestrict) VALUES \
    		('%d', '%s', '%d', '%d', '%d', '%d', '%d')", zHcuVmCtrTaskStaticCfg[item].taskInputId, zHcuVmCtrTaskStaticCfg[item].taskInputName, \
			zHcuVmCtrTaskStaticCfg[item].traceCtrFlag, zHcuVmCtrTaskStaticCfg[item].traceModToAllowFlag, zHcuVmCtrTaskStaticCfg[item].traceModToRestrictFlag, \
			zHcuVmCtrTaskStaticCfg[item].traceModFromAllowFlag, zHcuVmCtrTaskStaticCfg[item].traceModFromRestrictFlag);
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBICOM: INSERT data error: %s\n", mysql_error(sqlHandler));
        return FAILURE;
	}

	//释放记录集
    mysql_close(sqlHandler);
    return SUCCESS;
}


//查询所有基于模块控制的TRACE记录，并存入全局控制变量
OPSTAT dbi_HcuTraceMsgCtr_inqury(HcuSysEngParTab_t *engPar)
{
	MYSQL *sqlHandler;
	MYSQL_RES *resPtr;
	MYSQL_ROW sqlRow;
    int result = 0;
    char strsql[DBI_MAX_SQL_INQUERY_STRING_LENGTH];

    //入参检查：不涉及到生死问题，参数也没啥大问题，故而不需要检查，都可以存入数据库表单中
    if (engPar == NULL){
    	HcuErrorPrint("DBICOM: Input parameter NULL pointer!\n");
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
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBICOM: MySQL connection failed: %s\n", mysql_error(sqlHandler));
        return FAILURE;
    }

	//获取数据
    sprintf(strsql, "SELECT * FROM `hcutracemsgctr` WHERE 1");
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBICOM: Inqury data error: %s\n", mysql_error(sqlHandler));
        return FAILURE;
	}

	//查具体的结果
	resPtr = mysql_use_result(sqlHandler);
	if (!resPtr){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBICOM: mysql_use_result error!\n");
        return FAILURE;
	}

	//循环读取所有的记录
	while ((sqlRow = mysql_fetch_row(resPtr)) != NULL)
	{
		UINT16 mid = 0;
		int index = 0;
		int tableRow = 0;
		if(sqlRow[index]) mid = (UINT16)(atol(sqlRow[index++]) & 0xFFFF);
		if (mid >= HCU_SYSDIM_MSGID_NBR_MAX){
			HcuErrorPrint("DBICOM: Error MsgID populated, very dangerous to crash whole system! MsgID = %d\n", mid);
			mysql_free_result(resPtr);
		    mysql_close(sqlHandler);
		    return FAILURE;
		}

		engPar->traceList.msg[mid].msgId = mid;
		tableRow = hcu_vm_search_msg_static_cfg_table_of_row(mid);
		if(sqlRow[index]) strncpy(engPar->traceList.msg[mid].msgName, sqlRow[index++], HCU_SYSDIM_MSGID_NAME_LEN_MAX-1);
		if (strcmp(engPar->traceList.msg[mid].msgName, zHcuSysEngTrcMsgCtrStaticCfg[tableRow].msgName)){
			if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_CRT_ON) != FALSE){
				HcuDebugPrint("DBICOM: Error Message name populated, MsgId = %d, engPar->traceList.msg = [%s], zHcuMsgNameList = [%s]!\n", mid, engPar->traceList.msg[mid].msgName, zHcuSysEngTrcMsgCtrStaticCfg[tableRow].msgName);
			}
		}
		if(sqlRow[index]) engPar->traceList.msg[mid].msgCtrFlag = (UINT8)(atol(sqlRow[index++]) & 0xFF);
		if(sqlRow[index]) engPar->traceList.msg[mid].msgAllow = (UINT8)(atol(sqlRow[index++]) & 0xFF);
		if(sqlRow[index]) engPar->traceList.msg[mid].msgRestrict = (UINT8)(atol(sqlRow[index++]) & 0xFF);
	}

	//释放记录集
	mysql_free_result(resPtr);
    mysql_close(sqlHandler);
    return SUCCESS;
}

//根据VM初始化数据，写入数据库表单中初始化值，方便任务模块的增删，降低研发工作复杂度和工作量
OPSTAT dbi_HcuTraceMsgCtr_engpar_intelligence_init(void)
{
	MYSQL *sqlHandler;
	int result = 0, item = 0;
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
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBICOM: MySQL connection failed: %s\n", mysql_error(sqlHandler));
        return FAILURE;
    }

	//清除表格已有数据
    sprintf(strsql, "TRUNCATE table `hcutracemsgctr`");
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBICOM: Delete data error: %s, result = %d\n", mysql_error(sqlHandler), result);
        return FAILURE;
	}

	//读取VM中模块初始化表单内容
	//扫描输入表单，起始必须是MSG_ID_COM_MIN条目
	if (zHcuSysEngTrcMsgCtrStaticCfg[0].msgId != MSG_ID_COM_MIN){
		HcuErrorPrint("DBICOM: Initialize HCU-VM failure, message input configuration error!\n");
		return FAILURE;
	}
	//扫描输入表单，以MSG_ID_COM_MAX为终止条目：2倍配置项最多
	for(item=1; item < (2*HCU_SYSDIM_MSGID_NBR_MAX); item++){
		if(zHcuSysEngTrcMsgCtrStaticCfg[item].msgId == MSG_ID_COM_MAX){
			break;
		}
		if ((zHcuSysEngTrcMsgCtrStaticCfg[item].msgId <= MSG_ID_COM_MIN) || (zHcuSysEngTrcMsgCtrStaticCfg[item].msgId > MSG_ID_COM_MAX)){
			HcuErrorPrint("DBICOM: Initialize HCU-VM failure, message input configuration error!\n");
			return FAILURE;
		}
	}

	//从任务配置输入区域读取参数到系统任务表，一旦遇到MSG_ID_COM_MAX就终止
	item = 0;
	while(zHcuSysEngTrcMsgCtrStaticCfg[item].msgId != MSG_ID_COM_MAX){
		//REPLACE新的数据
	    sprintf(strsql, "REPLACE INTO `hcutracemsgctr` (msgid, msgname, msgctrflag, msgallow, msgrestrict) VALUES \
	    		('%d', '%s', '%d', '%d', '%d')", zHcuSysEngTrcMsgCtrStaticCfg[item].msgId, zHcuSysEngTrcMsgCtrStaticCfg[item].msgName, \
				zHcuSysEngTrcMsgCtrStaticCfg[item].traceCtrFlag, zHcuSysEngTrcMsgCtrStaticCfg[item].traceMsgAllowFlag, zHcuSysEngTrcMsgCtrStaticCfg[item].traceMsgRestrictFlag);
		result = mysql_query(sqlHandler, strsql);
		if(result){
	    	mysql_close(sqlHandler);
	    	HcuErrorPrint("DBICOM: REPLACE data error: %s\n", mysql_error(sqlHandler));
	        return FAILURE;
		}
		item++;
	}
	//最后一项必定是MSG_ID_COM_MAX
    sprintf(strsql, "REPLACE INTO `hcutracemsgctr` (msgid, msgname, msgctrflag, msgallow, msgrestrict) VALUES \
    		('%d', '%s', '%d', '%d', '%d')", zHcuSysEngTrcMsgCtrStaticCfg[item].msgId, zHcuSysEngTrcMsgCtrStaticCfg[item].msgName, \
			zHcuSysEngTrcMsgCtrStaticCfg[item].traceCtrFlag, zHcuSysEngTrcMsgCtrStaticCfg[item].traceMsgAllowFlag, zHcuSysEngTrcMsgCtrStaticCfg[item].traceMsgRestrictFlag);	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBICOM: INSERT data error: %s\n", mysql_error(sqlHandler));
        return FAILURE;
	}


	//释放记录集
    mysql_close(sqlHandler);
    return SUCCESS;
}


//公共处理函数
void dbi_display_header(MYSQL_RES *resPtr)
{
    MYSQL_FIELD *field_ptr;
    int max1 = 0;
    HcuDebugPrint("DBICOM: Column details:\n");
    while((field_ptr = mysql_fetch_field(resPtr))!=NULL)
    {
    	max1++;
    	if (max1>100) break; //做最大行数的保护，不让该函数无限超长显示
    	HcuDebugPrint("\t Name: %s\n",field_ptr->name);
    	HcuDebugPrint("\t Type: ");
        if(IS_NUM(field_ptr->type))
        {
        	HcuDebugPrint("Numeric filed\n");
        }
        else
        {
            switch(field_ptr->type)
            {
            case FIELD_TYPE_VAR_STRING:
            	HcuDebugPrint("VARCHAR\n");
                break;
            case FIELD_TYPE_LONG:
            	HcuDebugPrint("LONG\n");
                break;
            default:
            	HcuDebugPrint("Type is %d,check in mysql_com.h\n",field_ptr->type);
            }
        }
        HcuDebugPrint("\t MAX width %ld\n",field_ptr->length);
        if(field_ptr->flags&AUTO_INCREMENT_FLAG)
        	HcuDebugPrint("\t Auto increments\n");
        HcuDebugPrint("\n");
    }
}

//公共处理函数
void dbi_display_row(MYSQL *sqlHandler, MYSQL_ROW sqlRow)
{
    unsigned int field_count;

    field_count = 0;
    int max2 = 0;
    while(field_count < mysql_field_count(sqlHandler))
    {
    	max2++;
    	if (max2 > 20) break; //对域做最大限制，不想让这个显示无限循环，故而做一个保护
        if(sqlRow[field_count]) HcuDebugPrint("%s ", sqlRow[field_count]);
        else HcuDebugPrint("NULL");
        field_count++;
    }
    HcuDebugPrint("\n");
}

//查询当前HCU使用的数据库版本号
/*OPSTAT dbi_HcuDbVersion_inqury(SysEngParElementHwBurnPhyIdAddr_t *hcuInv)
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
    	HcuErrorPrint("DBICOM: MySQL init failed!\n");
        return FAILURE;
    }
    sqlHandler = mysql_real_connect(sqlHandler, HCU_SYSCFG_LOCAL_DB_HOST_DEFAULT, HCU_SYSCFG_LOCAL_DB_USER_DEFAULT, HCU_SYSCFG_LOCAL_DB_PSW_DEFAULT, HCU_SYSCFG_LOCAL_DB_NAME_DEFAULT, HCU_SYSCFG_LOCAL_DB_PORT_DEFAULT, NULL, 0);  //unix_socket and clientflag not used.
    if (!sqlHandler){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBICOM: MySQL connection failed: %s\n", mysql_error(sqlHandler));
        return FAILURE;
    }

	//获取数据
    sprintf(strsql, "SELECT * FROM `hcuswdb` WHERE 1");
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBICOM: Inqury data error: %s\n", mysql_error(sqlHandler));
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
		mysql_free_result(resPtr);
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBICOM: mysql_fetch_row NULL error!\n");
        return FAILURE;
	}
	else{
		int index = 0;
		if(sqlRow[index]) hcuInv->dbVerId = (UINT16)(atol(sqlRow[index++]) & 0xFFFF);
	}

	//释放记录集
	mysql_free_result(resPtr);
    mysql_close(sqlHandler);
    return SUCCESS;
}*/

//查询所有基于模块控制的TRACE记录，并存入全局控制变量
OPSTAT dbi_HcuSysEngTimer_inqury(HcuSysEngParTab_t *engPar)
{
	MYSQL *sqlHandler;
	MYSQL_RES *resPtr;
	MYSQL_ROW sqlRow;
    int result = 0;
    char strsql[DBI_MAX_SQL_INQUERY_STRING_LENGTH];

    //入参检查：不涉及到生死问题，参数也没啥大问题，故而不需要检查，都可以存入数据库表单中
    if (engPar == NULL){
    	HcuErrorPrint("DBICOM: Input parameter NULL pointer!\n");
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
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBICOM: MySQL connection failed: %s\n", mysql_error(sqlHandler));
        return FAILURE;
    }

	//获取数据
    sprintf(strsql, "SELECT * FROM `hcusysengtimer` WHERE 1");
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBICOM: Inqury data error: %s\n", mysql_error(sqlHandler));
        return FAILURE;
	}

	//查具体的结果
	resPtr = mysql_use_result(sqlHandler);
	if (!resPtr){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBICOM: mysql_use_result error!\n");
        return FAILURE;
	}

	//循环读取所有的记录
	while ((sqlRow = mysql_fetch_row(resPtr)) != NULL)
	{
		UINT16 tid = 0;
		int index = 0;
		int tableRow = 0;
		if(sqlRow[index]) tid = (UINT16)(atol(sqlRow[index++]) & 0xFFFF);
		if (tid >= HCU_SYSDIM_TIMERID_NBR_MAX){
			HcuErrorPrint("DBICOM: Error TimerId populated, very dangerous to crash whole system! TimerId = %d\n", tid);
			mysql_free_result(resPtr);
		    mysql_close(sqlHandler);
		    return FAILURE;
		}

		engPar->timer.array[tid].id = tid;
		tableRow = hcu_vm_search_timer_static_cfg_table_of_row(tid);
		if(sqlRow[index]) strncpy(engPar->timer.array[tid].name, sqlRow[index++], HCU_SYSDIM_TIMERID_NAME_LEN_MAX-1);
		if (strcmp(engPar->timer.array[tid].name, zHcuSysEngTimerStaticCfg[tableRow].timerName)){
			if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_CRT_ON) != FALSE){
				HcuDebugPrint("DBICOM: Error Timer name populated, TimerId = %d, engPar->traceList.msg = [%s], Input Table or DB = [%s]!\n",\
						tid, engPar->timer.array[tid].name, zHcuSysEngTimerStaticCfg[tableRow].timerName);
			}
		}
		if(sqlRow[index]) engPar->timer.array[tid].gradunarity = (UINT8)(atol(sqlRow[index++]) & 0xFF);
		if(sqlRow[index]) engPar->timer.array[tid].dur = (UINT32)(atol(sqlRow[index++]) & 0xFFFFFFFF);
	}

	//释放记录集
	mysql_free_result(resPtr);
    mysql_close(sqlHandler);
    return SUCCESS;
}

OPSTAT dbi_HcuSysEngTimer_engpar_intelligence_init(void)
{
	MYSQL *sqlHandler;
	int result = 0, item = 0;
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
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBICOM: MySQL connection failed: %s\n", mysql_error(sqlHandler));
        return FAILURE;
    }

	//清除表格已有数据
    sprintf(strsql, "TRUNCATE table `hcusysengtimer`");
	result = mysql_query(sqlHandler, strsql);
	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBICOM: Delete data error: %s, result = %d\n", mysql_error(sqlHandler), result);
        return FAILURE;
	}

	//读取VM中模块初始化表单内容
	//扫描输入表单，起始必须是TIMER_ID_MIN条目
	if (zHcuSysEngTimerStaticCfg[0].timerId != TIMER_ID_MIN){
		HcuErrorPrint("DBICOM: Initialize HCU-VM failure, timer input configuration error!\n");
		return FAILURE;
	}
	//扫描输入表单，以TIMER_ID_MAX为终止条目：2倍配置项最多
	for(item=1; item < (2*HCU_SYSDIM_TIMERID_NBR_MAX); item++){
		if(zHcuSysEngTimerStaticCfg[item].timerId == TIMER_ID_MAX){
			break;
		}
		if ((zHcuSysEngTimerStaticCfg[item].timerId <= TIMER_ID_MIN) || (zHcuSysEngTimerStaticCfg[item].timerId > TIMER_ID_MAX)){
			HcuErrorPrint("DBICOM: Initialize HCU-VM failure, timer input configuration error!\n");
			return FAILURE;
		}
	}

	//从任务配置输入区域读取参数到系统任务表，一旦遇到TIMER_ID_MAX就终止
	item = 0;
	while(zHcuSysEngTimerStaticCfg[item].timerId != TIMER_ID_MAX){
		//REPLACE新的数据
	    sprintf(strsql, "REPLACE INTO `hcusysengtimer` (timerid, timername, granularity, duration) VALUES \
	    		('%d', '%s', '%d', '%d')", zHcuSysEngTimerStaticCfg[item].timerId, zHcuSysEngTimerStaticCfg[item].timerName, \
				zHcuSysEngTimerStaticCfg[item].timerGranularity, zHcuSysEngTimerStaticCfg[item].timerDur);
		result = mysql_query(sqlHandler, strsql);
		if(result){
	    	mysql_close(sqlHandler);
	    	HcuErrorPrint("DBICOM: REPLACE data error: %s\n", mysql_error(sqlHandler));
	        return FAILURE;
		}
		item++;
	}
	//最后一项必定是TIMER_ID_MAX
    sprintf(strsql, "REPLACE INTO `hcusysengtimer` (timerid, timername, granularity, duration) VALUES \
    		('%d', '%s', '%d', '%d')", zHcuSysEngTimerStaticCfg[item].timerId, zHcuSysEngTimerStaticCfg[item].timerName, \
			zHcuSysEngTimerStaticCfg[item].timerGranularity, zHcuSysEngTimerStaticCfg[item].timerDur);
    	result = mysql_query(sqlHandler, strsql);
    	if(result){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBICOM: INSERT data error: %s\n", mysql_error(sqlHandler));
        return FAILURE;
	}

	//释放记录集
    mysql_close(sqlHandler);
    return SUCCESS;
}






/*///////////////////////////////////////UPDATE的用法，还未验证好/////////////////////////////////
//更新数据 UPDATE Person SET Address = 'Zhongshan 23', City = 'Nanjing' WHERE LastName = 'Wilson'
sprintf(strsql, "UPDATE `hcutracemodulectr` SET `modulectrflag` = '%d', `moduletoallow` = '%d', `moduletorestrict` = '%d', `modulefromallow` = '%d', modulefromrestrict = '%d' (WHERE `hcutracemodulectr`.`moduleid` = '%d')", \
		zHcuGlobalTaskInputConfig[item].traceCtrFlag, zHcuGlobalTaskInputConfig[item].traceModToAllowFlag, zHcuGlobalTaskInputConfig[item].traceModToRestrictFlag, \
		zHcuGlobalTaskInputConfig[item].traceModFromAllowFlag, zHcuGlobalTaskInputConfig[item].traceModFromRestrictFlag, zHcuGlobalTaskInputConfig[item].taskInputId);
result = mysql_query(sqlHandler, strsql);
if(result){
	mysql_close(sqlHandler);
	HcuErrorPrint("DBICOM: UPDATE data error: Result = %d, %s, Input=[%s], Index = %d, taskid = %d, taskName=%s\n", result, mysql_error(sqlHandler), strsql, item, zHcuGlobalTaskInputConfig[item].taskInputId, zHcuGlobalTaskInputConfig[item].taskInputName);
    return FAILURE;
}
*///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////








