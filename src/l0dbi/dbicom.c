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


//SYSENGPAR数据表格式
// HcuSysEngPar
/*

-- --------------------------------------------------------

--
-- Table structure for table `hcusysengpar`
--

CREATE TABLE IF NOT EXISTS `hcusysengpar` (
  `prjname` char(100) NOT NULL,
  `commbackhawlcon` int(1) NOT NULL,
  `commhwboardethernet` int(1) NOT NULL,
  `commhwboardusbnet` int(1) NOT NULL,
  `commhwboardwifi` int(1) NOT NULL,
  `commhwboard3g4g` int(1) NOT NULL,
  `commhwboardgps` int(1) NOT NULL,
  `commhwboardlcd` int(1) NOT NULL,
  `commhwboardled` int(1) NOT NULL,
  `commhwboardzeegbe` int(1) NOT NULL,
  `commhwboardflash` int(1) NOT NULL,
  `commframemodbus` int(1) NOT NULL,
  `commframespsvirgo` int(1) NOT NULL,
  `commframeavorion` int(1) NOT NULL,
  `commframecloudvela` int(1) NOT NULL,
  `commframei2cbuslibra` int(1) NOT NULL,
  `commframespibusaries` int(1) NOT NULL,
  `commframenbiotcj188` int(1) NOT NULL,
  `commframenbiotqg376` int(1) NOT NULL,
  `commfrontsps485` int(1) NOT NULL,
  `commfrontsps232` int(1) NOT NULL,
  `commfrontmicrophone` int(1) NOT NULL,
  `commfrontcamera` int(1) NOT NULL,
  `commfrontble` int(1) NOT NULL,
  `commfrontgpio` int(1) NOT NULL,
  `commfronti2c` int(1) NOT NULL,
  `commfrontspi` int(1) NOT NULL,
  `commfrontpwm` int(1) NOT NULL,
  `commfrontadc` int(1) NOT NULL,
  `commfrontswitch` int(1) NOT NULL,
  `commfrontrelay` int(1) NOT NULL,
  `commfrontmotor` int(1) NOT NULL,
  `commfrontsensoremc` int(1) NOT NULL,
  `commfrontsensorpm25` int(1) NOT NULL,
  `commfrontsensortemp` int(1) NOT NULL,
  `commfrontsensorhumid` int(1) NOT NULL,
  `commfrontsensorwinddir` int(1) NOT NULL,
  `commfrontsensorwindspd` int(1) NOT NULL,
  `commfrontsensornoise` int(1) NOT NULL,
  `commfrontsensorhsmmp` int(1) NOT NULL,
  `commfrontsensorairprs` int(1) NOT NULL,
  `commfrontsensorco1` int(1) NOT NULL,
  `commfrontsensorlightstr` int(1) NOT NULL,
  `commfrontsensoralcohol` int(1) NOT NULL,
  `commfrontsensorhcho` int(1) NOT NULL,
  `commfrontsensortoxicgas` int(1) NOT NULL,
  `commfrontsensoriwm` int(1) NOT NULL,
  `commfrontsensorihm` int(1) NOT NULL,
  `commfrontsensorigm` int(1) NOT NULL,
  `commfrontsensoripm` int(1) NOT NULL,
  `commfrontsensorpm25sharp` int(1) NOT NULL,
  `hcudbhost` char(20) NOT NULL,
  `hcudbuser` char(20) NOT NULL,
  `hcudbpsw` char(20) NOT NULL,
  `hcudbname` char(20) NOT NULL,
  `hcudbport` int(1) NOT NULL,
  `emcreqtimer` int(1) NOT NULL,
  `emcreqtimerfb` int(1) NOT NULL,
  `humidreqtimer` int(1) NOT NULL,
  `humidreqtimerfb` int(1) NOT NULL,
  `noisereqtimer` int(1) NOT NULL,
  `noisereqtimerfb` int(1) NOT NULL,
  `pm25reqtimer` int(1) NOT NULL,
  `pm25reqtimerfb` int(1) NOT NULL,
  `tempreqtimer` int(1) NOT NULL,
  `tempreqtimerfb` int(1) NOT NULL,
  `winddirreqtimer` int(1) NOT NULL,
  `winddirreqtimerfb` int(1) NOT NULL,
  `windspdreqtimer` int(1) NOT NULL,
  `windspdreqtimerfb` int(1) NOT NULL,
  `cloudvelahbtimer` int(1) NOT NULL,
  `cloudvelahbbacktimer` int(1) NOT NULL,
  `nbiotcj188hbtimer` int(1) NOT NULL,
  `nbiotcj188hbbacktimer` int(1) NOT NULL,
  `nbiotqg376hbtimer` int(1) NOT NULL,
  `nbiotqg376hbbacktimer` int(1) NOT NULL,
  `cmdcontrollongtimer` int(1) NOT NULL,
  `cmdcontrolshorttimer` int(1) NOT NULL,
  `hsmmpreqtimer` int(1) NOT NULL,
  `hsmmpcapduration` int(1) NOT NULL,
  `hsmmpcapdurationfb` int(11) NOT NULL,
  `hsmmprefrate` int(1) NOT NULL,
  `airprsreqtimer` int(1) NOT NULL,
  `co1reqtimer` int(1) NOT NULL,
  `lightstrreqtimer` int(1) NOT NULL,
  `alcoholreqtimer` int(1) NOT NULL,
  `hchoreqtimer` int(1) NOT NULL,
  `toxicgasreqtimer` int(1) NOT NULL,
  `iwmreqtimer` int(1) NOT NULL,
  `ihmreqtimer` int(1) NOT NULL,
  `igmreqtimer` int(1) NOT NULL,
  `ipmreqtimer` int(1) NOT NULL,
  `pm25sharpreqtimer` int(1) NOT NULL,
  `syspmworkingtimer` int(1) NOT NULL,
  `seriesportformodbus` int(1) NOT NULL,
  `seriesportforgps` int(1) NOT NULL,
  `seriesportforpm25sharp` int(1) NOT NULL,
  `cloudhttpaddlocal` char(200) NOT NULL,
  `cloudhttpaddtest` char(200) NOT NULL,
  `cloudhttpaddsae` char(200) NOT NULL,
  `cloudhttpaddjd` char(200) NOT NULL,
  `cloudhttpaddwechat` char(200) NOT NULL,
  `cloudbhservername` char(12) NOT NULL,
  `cloudbhhcuname` char(12) NOT NULL,
  `cloudbhitfframestd` int(1) NOT NULL,
  `cloudftpadd` char(64) NOT NULL,
  `cloudftpuser` char(12) NOT NULL,
  `cloudftppwd` char(12) NOT NULL,
  `cloudftppwdvideo` char(12) NOT NULL,
  `cloudftpuservideo` char(12) NOT NULL,
  `hcuswdownloaddir` char(64) NOT NULL,
  `hcuswactivedir` char(64) NOT NULL,
  `hcuswbackupdir` char(64) NOT NULL,
  `hcuvideoserverdir` char(64) NOT NULL,
  `hcuvideoserverhttp` char(64) NOT NULL,
  `debugmode` int(1) NOT NULL,
  `tracemode` int(1) NOT NULL,
  `browselautostartupflag` int(1) NOT NULL,
  `browselprog` char(20) NOT NULL,
  `browselstartupaddress` char(128) NOT NULL,
  `browselworkingoption` char(128) NOT NULL,
  PRIMARY KEY (`prjname`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Dumping data for table `hcusysengpar`
--

INSERT INTO `hcusysengpar` (`prjname`, `commbackhawlcon`, `commhwboardethernet`, `commhwboardusbnet`, `commhwboardwifi`, `commhwboard3g4g`, `commhwboardgps`, `commhwboardlcd`, `commhwboardled`, `commhwboardzeegbe`, `commhwboardflash`, `commframemodbus`, `commframespsvirgo`, `commframeavorion`, `commframecloudvela`, `commframei2cbuslibra`, `commframespibusaries`, `commframenbiotcj188`, `commframenbiotqg376`, `commfrontsps485`, `commfrontsps232`, `commfrontmicrophone`, `commfrontcamera`, `commfrontble`, `commfrontgpio`, `commfronti2c`, `commfrontspi`, `commfrontpwm`, `commfrontadc`, `commfrontswitch`, `commfrontrelay`, `commfrontmotor`, `commfrontsensoremc`, `commfrontsensorpm25`, `commfrontsensortemp`, `commfrontsensorhumid`, `commfrontsensorwinddir`, `commfrontsensorwindspd`, `commfrontsensornoise`, `commfrontsensorhsmmp`, `commfrontsensorairprs`, `commfrontsensorco1`, `commfrontsensorlightstr`, `commfrontsensoralcohol`, `commfrontsensorhcho`, `commfrontsensortoxicgas`, `commfrontsensoriwm`, `commfrontsensorihm`, `commfrontsensorigm`, `commfrontsensoripm`, `commfrontsensorpm25sharp`, `hcudbhost`, `hcudbuser`, `hcudbpsw`, `hcudbname`, `hcudbport`, `emcreqtimer`, `emcreqtimerfb`, `humidreqtimer`, `humidreqtimerfb`, `noisereqtimer`, `noisereqtimerfb`, `pm25reqtimer`, `pm25reqtimerfb`, `tempreqtimer`, `tempreqtimerfb`, `winddirreqtimer`, `winddirreqtimerfb`, `windspdreqtimer`, `windspdreqtimerfb`, `cloudvelahbtimer`, `cloudvelahbbacktimer`, `nbiotcj188hbtimer`, `nbiotcj188hbbacktimer`, `nbiotqg376hbtimer`, `nbiotqg376hbbacktimer`, `cmdcontrollongtimer`, `cmdcontrolshorttimer`, `hsmmpreqtimer`, `hsmmpcapduration`, `hsmmpcapdurationfb`, `hsmmprefrate`, `airprsreqtimer`, `co1reqtimer`, `lightstrreqtimer`, `alcoholreqtimer`, `hchoreqtimer`, `toxicgasreqtimer`, `iwmreqtimer`, `ihmreqtimer`, `igmreqtimer`, `ipmreqtimer`, `pm25sharpreqtimer`, `syspmworkingtimer`, `seriesportformodbus`, `seriesportforgps`, `seriesportforpm25sharp`, `cloudhttpaddlocal`, `cloudhttpaddtest`, `cloudhttpaddsae`, `cloudhttpaddjd`, `cloudhttpaddwechat`, `cloudbhservername`, `cloudbhhcuname`, `cloudbhitfframestd`, `cloudftpadd`, `cloudftpuser`, `cloudftppwd`, `cloudftppwdvideo`, `cloudftpuservideo`, `hcuswdownloaddir`, `hcuswactivedir`, `hcuswbackupdir`, `hcuvideoserverdir`, `hcuvideoserverhttp`, `debugmode`, `tracemode`, `browselautostartupflag`, `browselprog`, `browselstartupaddress`, `browselworkingoption`) VALUES
('HCU_PRJ_AQYC', 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 'localhost', 'root', '123456', 'hcudb', 3306, 60, 10, 600, 10, 600, 10, 600, 10, 600, 10, 600, 10, 600, 10, 600, 10, 600, 10, 600, 10, 70, 5, 200, 2, 12, 20, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 3600, 3, 1, 0, 'http://127.0.0.1/test.php', 'http://aaa.bbb/', 'http://121.40.185.177/xhzn/mfunhcu/l1mainentry/cloud_callback_hcu.php', 'http://mfunhcu.sinaapp.com/jd/cloud_callback.php', 'http://mfunhcu.sinaapp.com/l1mainentry/cloud_callback_wechat.php', 'AQ_HCU', 'HCU_SH_0304', 1, 'ftp://121.40.185.177', 'forlinx', 'Forlinx321', 'avorion', 'Avorion321', '/home/pi/hcu_sw_download/', '/home/pi/hcu_sw_active/', '/home/pi/hcu_sw_backup/', '/usr/local/apache_arm/htdocs/avorion/', 'http://192.168.1.232:8000/avorion/', 31, 4, 0, 'firefox', 'http://localhost/yii2basic/web/index.php', '-'),
('HCU_PRJ_NBIOT_CJ188', 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 'localhost', 'root', '123456', 'hcudb', 3306, 60, 10, 600, 10, 600, 10, 600, 10, 600, 10, 600, 10, 600, 10, 600, 10, 600, 10, 600, 10, 70, 5, 200, 2, 12, 20, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 3600, 3, 1, 0, 'http://127.0.0.1/test.php', 'http://aaa.bbb/', 'http://121.40.185.177/xhzn/mfunhcu/l1mainentry/cloud_callback_hcu.php', 'http://mfunhcu.sinaapp.com/jd/cloud_callback.php', 'http://mfunhcu.sinaapp.com/l1mainentry/cloud_callback_wechat.php', 'AQ_HCU', 'HCU_SH_0304', 1, 'ftp://121.40.185.177', 'forlinx', 'Forlinx321', 'avorion', 'Avorion321', '/home/pi/hcu_sw_download/', '/home/pi/hcu_sw_active/', '/home/pi/hcu_sw_backup/', '/usr/local/apache_arm/htdocs/avorion/', 'http://192.168.1.232:8000/avorion/', 31, 4, 0, 'firefox', 'http://localhost/yii2basic/web/index.php', '-'),
('HCU_PRJ_NBIOT_QG376', 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 1, 'localhost', 'root', '123456', 'hcudb', 3306, 60, 10, 600, 10, 600, 10, 600, 10, 600, 10, 600, 10, 600, 10, 600, 10, 600, 10, 600, 10, 70, 5, 200, 2, 12, 20, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 3600, 3, 1, 0, 'http://127.0.0.1/test.php', 'http://aaa.bbb/', 'http://121.40.185.177/xhzn/mfunhcu/l1mainentry/cloud_callback_hcu.php', 'http://mfunhcu.sinaapp.com/jd/cloud_callback.php', 'http://mfunhcu.sinaapp.com/l1mainentry/cloud_callback_wechat.php', 'AQ_HCU', 'HCU_SH_0304', 1, 'ftp://121.40.185.177', 'forlinx', 'Forlinx321', 'avorion', 'Avorion321', '/home/pi/hcu_sw_download/', '/home/pi/hcu_sw_active/', '/home/pi/hcu_sw_backup/', '/usr/local/apache_arm/htdocs/avorion/', 'http://192.168.1.232:8000/avorion/', 31, 4, 0, 'firefox', 'http://localhost/yii2basic/web/index.php', '-'),
('HCU_PRJ_TBSWR', 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 'localhost', 'root', '123456', 'hcudb', 3306, 60, 10, 600, 10, 600, 10, 600, 10, 600, 10, 600, 10, 600, 10, 600, 10, 600, 10, 600, 10, 70, 5, 200, 2, 12, 20, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 3600, 3, 1, 0, 'http://127.0.0.1/test.php', 'http://aaa.bbb/', 'http://121.40.185.177/xhzn/mfunhcu/l1mainentry/cloud_callback_hcu.php', 'http://mfunhcu.sinaapp.com/jd/cloud_callback.php', 'http://mfunhcu.sinaapp.com/l1mainentry/cloud_callback_wechat.php', 'AQ_HCU', 'HCU_SH_0304', 1, 'ftp://121.40.185.177', 'forlinx', 'Forlinx321', 'avorion', 'Avorion321', '/home/pi/hcu_sw_download/', '/home/pi/hcu_sw_active/', '/home/pi/hcu_sw_backup/', '/usr/local/apache_arm/htdocs/avorion/', 'http://192.168.1.232:8000/avorion/', 31, 4, 0, 'firefox', 'http://localhost/yii2basic/web/index.php', '-'),
('HCU_PRJ_TESTMODE', 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 'localhost', 'root', '123456', 'hcudb', 3306, 60, 10, 600, 10, 600, 10, 600, 10, 600, 10, 600, 10, 600, 10, 600, 10, 600, 10, 600, 10, 70, 5, 200, 2, 12, 20, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 600, 3600, 3, 1, 0, 'http://127.0.0.1/test.php', 'http://aaa.bbb/', 'http://121.40.185.177/xhzn/mfunhcu/l1mainentry/cloud_callback_hcu.php', 'http://mfunhcu.sinaapp.com/jd/cloud_callback.php', 'http://mfunhcu.sinaapp.com/l1mainentry/cloud_callback_wechat.php', 'AQ_HCU', 'HCU_SH_0304', 1, 'ftp://121.40.185.177', 'forlinx', 'Forlinx321', 'avorion', 'Avorion321', '/home/pi/hcu_sw_download/', '/home/pi/hcu_sw_active/', '/home/pi/hcu_sw_backup/', '/usr/local/apache_arm/htdocs/avorion/', 'http://192.168.1.232:8000/avorion/', 31, 4, 0, 'firefox', 'http://localhost/yii2basic/web/index.php', '-');



//HcuTraceModuleCtr数据表格式
-- --------------------------------------------------------

--
-- Table structure for table `hcutracemodulectr`
--

CREATE TABLE IF NOT EXISTS `hcutracemodulectr` (
  `moduleid` int(2) NOT NULL,
  `modulename` char(12) NOT NULL,
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
(0, 'MIN', 1, 1, 1, 1, 1),
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
(15, 'GRPS', 1, 1, 1, 1, 1),
(16, 'SPS232', 1, 1, 1, 1, 1),
(17, 'SPS485', 1, 1, 1, 1, 1),
(18, 'BLE', 1, 1, 1, 1, 1),
(19, 'ETHERNET', 1, 1, 1, 1, 1),
(20, 'WIFI', 1, 1, 1, 1, 1),
(21, 'USBNET', 1, 1, 1, 1, 1),
(22, '3G4G', 1, 1, 1, 1, 1),
(23, 'HARDDISK', 1, 1, 1, 1, 1),
(24, 'CAMERA', 1, 1, 1, 1, 1),
(25, 'MICROPHONE', 1, 1, 1, 1, 1),
(26, 'FLASH', 1, 1, 1, 1, 1),
(27, 'GPS', 1, 1, 1, 1, 1),
(28, 'LCD', 1, 1, 1, 1, 1),
(29, 'LED', 1, 1, 1, 1, 1),
(30, 'HWINV', 1, 1, 1, 1, 1),
(31, 'SPSVIRGO', 1, 1, 1, 1, 1),
(32, 'CLOUDVELA', 1, 1, 1, 1, 1),
(33, 'MODBUS', 1, 1, 1, 1, 1),
(34, 'AVORION', 1, 1, 1, 1, 1),
(35, 'I2CBUSLIBRA', 1, 1, 1, 1, 1),
(36, 'SPIBUSARIES', 1, 1, 1, 1, 1),
(37, 'NBIOTCJ188', 1, 1, 1, 1, 1),
(38, 'NBIOTQG376', 1, 1, 1, 1, 1),
(39, 'HSMMP', 1, 1, 1, 1, 1),
(40, 'EMC', 1, 1, 1, 1, 1),
(41, 'HUMID', 1, 1, 1, 1, 1),
(42, 'PM25', 1, 1, 1, 1, 1),
(43, 'TEMP', 1, 1, 1, 1, 1),
(44, 'WINDDIR', 1, 1, 1, 1, 1),
(45, 'WINDSPD', 1, 1, 1, 1, 1),
(46, 'NOISE', 1, 1, 1, 1, 1),
(47, 'AIRPRS', 1, 1, 1, 1, 1),
(48, 'CO1', 1, 1, 1, 1, 1),
(49, 'LIGHTSTR', 1, 1, 1, 1, 1),
(50, 'ALCOHOL', 1, 1, 1, 1, 1),
(51, 'HCHO', 1, 1, 1, 1, 1),
(52, 'TOXICGAS', 1, 1, 1, 1, 1),
(53, 'IWM', 1, 1, 1, 1, 1),
(54, 'IHM', 1, 1, 1, 1, 1),
(55, 'IGM', 1, 1, 1, 1, 1),
(56, 'IPM', 1, 1, 1, 1, 1),
(57, 'SVRCON', 1, 1, 1, 1, 1),
(58, 'SYSPM', 1, 1, 1, 1, 1),
(59, 'PM25SHARP', 1, 1, 1, 1, 1),
(60, 'MAX', 1, 1, 1, 1, 1);

//HcuTraceMsgCtr数据表格式

 如果MSG_NAME_MAX_LENGTH改变，这里也需要改变
CREATE TABLE IF NOT EXISTS `hcutracemsgctr` (
  `msgid` int(2) NOT NULL,
  `msgname` char(70) NOT NULL,
  `msgctrflag` int(1) NOT NULL,
  `msgallow` int(1) NOT NULL,
  `msgrestrict` int(1) NOT NULL,
  PRIMARY KEY (`msgid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;


//MESSAGE的消息数量有任何改动，需要更新这里，否则会出错！
INSERT INTO `hcutracemsgctr` (`msgid`, `msgname`, `msgctrflag`, `msgallow`, `msgrestrict`) VALUES
(  0, 'MSG_ID_COM_MIN', 1, 1, 1),
(  1, 'MSG_ID_COM_INIT', 1, 1, 1),
(  2, 'MSG_ID_COM_INIT_FEEDBACK', 1, 1, 1),
(  3, 'MSG_ID_COM_RESTART', 1, 1, 1),
(  4, 'MSG_ID_COM_STOP', 1, 1, 1),
(  5, 'MSG_ID_COM_COMPLETE', 1, 1, 1),
(  6, 'MSG_ID_COM_TIME_OUT', 1, 1, 1),
(  7, 'MSG_ID_COM_DEBUG_TEST', 1, 1, 1),
(  8, 'MSG_ID_COM_HEART_BEAT', 1, 1, 1),
(  9, 'MSG_ID_COM_HEART_BEAT_FB', 1, 1, 1),
( 10, 'MSG_ID_COM_PROCESS_REBOOT', 1, 1, 1),
( 11, 'MSG_ID_ETHERNET_CLOUDVELA_DATA_RX', 1, 1, 1),
( 12, 'MSG_ID_ETHERNET_NBIOTCJ188_DATA_RX', 1, 1, 1),
( 13, 'MSG_ID_ETHERNET_NBIOTQG376_DATA_RX', 1, 1, 1),
( 14, 'MSG_ID_WIFI_CLOUDVELA_DATA_RX', 1, 1, 1),
( 15, 'MSG_ID_USBNET_CLOUDVELA_DATA_RX', 1, 1, 1),
( 16, 'MSG_ID_3G4G_CLOUDVELA_DATA_RX', 1, 1, 1),
( 17, 'MSG_ID_SPS232_MODBUS_DATA_RX', 1, 1, 1),
( 18, 'MSG_ID_SPS485_MODBUS_DATA_RX', 1, 1, 1),
( 19, 'MSG_ID_SPSVIRGO_HSMMP_DATA_RX', 1, 1, 1),
( 20, 'MSG_ID_SPSVIRGO_NOISE_DATA_REPORT', 1, 1, 1),
( 21, 'MSG_ID_SPSVIRGO_NOISE_CONTROL_FB', 1, 1, 1),
( 22, 'MSG_ID_AVORION_HSMMP_DATA_READ_FB', 1, 1, 1),
( 23, 'MSG_ID_AVORION_HSMMP_DATA_RX', 1, 1, 1),
( 24, 'MSG_ID_BLE_HSMMP_DATA_RX', 1, 1, 1),
( 25, 'MSG_ID_BLE_MODBUS_DATA_RX', 1, 1, 1),
( 26, 'MSG_ID_LCD_AVORION_DATA_RX', 1, 1, 1),
( 27, 'MSG_ID_CAMERA_AVORION_DATA_RX', 1, 1, 1),
( 28, 'MSG_ID_MICROPHONE_AVORION_DATA_RX', 1, 1, 1),
( 29, 'MSG_ID_HWINV_CLOUDVELA_PHY_STATUS_CHG', 1, 1, 1),
( 30, 'MSG_ID_CLOUDVELA_EMC_DATA_REQ', 1, 1, 1),
( 31, 'MSG_ID_CLOUDVELA_EMC_CONTROL_CMD', 1, 1, 1),
( 32, 'MSG_ID_CLOUDVELA_PM25_DATA_REQ', 1, 1, 1),
( 33, 'MSG_ID_CLOUDVELA_PM25_CONTROL_CMD', 1, 1, 1),
( 34, 'MSG_ID_CLOUDVELA_WINDDIR_DATA_REQ', 1, 1, 1),
( 35, 'MSG_ID_CLOUDVELA_WINDDIR_CONTROL_CMD', 1, 1, 1),
( 36, 'MSG_ID_CLOUDVELA_WINDSPD_DATA_REQ', 1, 1, 1),
( 37, 'MSG_ID_CLOUDVELA_WINDSPD_CONTROL_CMD', 1, 1, 1),
( 38, 'MSG_ID_CLOUDVELA_TEMP_DATA_REQ', 1, 1, 1),
( 39, 'MSG_ID_CLOUDVELA_TEMP_CONTROL_CMD', 1, 1, 1),
( 40, 'MSG_ID_CLOUDVELA_HUMID_DATA_REQ', 1, 1, 1),
( 41, 'MSG_ID_CLOUDVELA_HUMID_CONTROL_CMD', 1, 1, 1),
( 42, 'MSG_ID_CLOUDVELA_HSMMP_CONTROL_CMD', 1, 1, 1),
( 43, 'MSG_ID_CLOUDVELA_NOISE_DATA_REQ', 1, 1, 1),
( 44, 'MSG_ID_CLOUDVELA_NOISE_CONTROL_CMD', 1, 1, 1),
( 45, 'MSG_ID_NBIOTCJ188_IWM_DATA_REQ', 1, 1, 1),
( 46, 'MSG_ID_NBIOTCJ188_IWM_CONTROL_CMD', 1, 1, 1),
( 47, 'MSG_ID_NBIOTCJ188_IHM_DATA_REQ', 1, 1, 1),
( 48, 'MSG_ID_NBIOTCJ188_IHM_CONTROL_CMD', 1, 1, 1),
( 49, 'MSG_ID_NBIOTCJ188_IGM_DATA_REQ', 1, 1, 1),
( 50, 'MSG_ID_NBIOTCJ188_IGM_CONTROL_CMD', 1, 1, 1),
( 51, 'MSG_ID_NBIOTCJ188_IPM_DATA_REQ', 1, 1, 1),
( 52, 'MSG_ID_NBIOTCJ188_IPM_CONTROL_CMD', 1, 1, 1),
( 53, 'MSG_ID_NBIOTQG376_IPM_DATA_REQ', 1, 1, 1),
( 54, 'MSG_ID_NBIOTQG376_IPM_CONTROL_CMD', 1, 1, 1),
( 55, 'MSG_ID_MODBUS_EMC_DATA_REPORT', 1, 1, 1),
( 56, 'MSG_ID_MODBUS_EMC_CONTROL_FB', 1, 1, 1),
( 57, 'MSG_ID_MODBUS_PM25_DATA_REPORT', 1, 1, 1),
( 58, 'MSG_ID_MODBUS_PM25_CONTROL_FB', 1, 1, 1),
( 59, 'MSG_ID_MODBUS_WINDDIR_DATA_REPORT', 1, 1, 1),
( 60, 'MSG_ID_MODBUS_WINDDIR_CONTROL_FB', 1, 1, 1),
( 61, 'MSG_ID_MODBUS_WINDSPD_DATA_REPORT', 1, 1, 1),
( 62, 'MSG_ID_MODBUS_WINDSPD_CONTROL_FB', 1, 1, 1),
( 63, 'MSG_ID_MODBUS_TEMP_DATA_REPORT', 1, 1, 1),
( 64, 'MSG_ID_MODBUS_TEMP_CONTROL_FB', 1, 1, 1),
( 65, 'MSG_ID_MODBUS_HUMID_DATA_REPORT', 1, 1, 1),
( 66, 'MSG_ID_MODBUS_HUMID_CONTROL_FB', 1, 1, 1),
( 67, 'MSG_ID_MODBUS_NOISE_DATA_REPORT', 1, 1, 1),
( 68, 'MSG_ID_MODBUS_NOISE_CONTROL_FB', 1, 1, 1),
( 69, 'MSG_ID_EMC_MODBUS_DATA_READ', 1, 1, 1),
( 70, 'MSG_ID_EMC_CLOUDVELA_DATA_RESP', 1, 1, 1),
( 71, 'MSG_ID_EMC_MODBUS_CONTROL_CMD', 1, 1, 1),
( 72, 'MSG_ID_EMC_CLOUDVELA_CONTROL_FB', 1, 1, 1),
( 73, 'MSG_ID_PM25_MODBUS_DATA_READ', 1, 1, 1),
( 74, 'MSG_ID_PM25_CLOUDVELA_DATA_RESP', 1, 1, 1),
( 75, 'MSG_ID_PM25_MODBUS_CONTROL_CMD', 1, 1, 1),
( 76, 'MSG_ID_PM25_CLOUDVELA_CONTROL_FB', 1, 1, 1),
( 77, 'MSG_ID_WINDDIR_MODBUS_DATA_READ', 1, 1, 1),
( 78, 'MSG_ID_WINDDIR_CLOUDVELA_DATA_RESP', 1, 1, 1),
( 79, 'MSG_ID_WINDDIR_MODBUS_CONTROL_CMD', 1, 1, 1),
( 80, 'MSG_ID_WINDDIR_CLOUDVELA_CONTROL_FB', 1, 1, 1),
( 81, 'MSG_ID_WINDSPD_MODBUS_DATA_READ', 1, 1, 1),
( 82, 'MSG_ID_WINDSPD_CLOUDVELA_DATA_RESP', 1, 1, 1),
( 83, 'MSG_ID_WINDSPD_MODBUS_CONTROL_CMD', 1, 1, 1),
( 84, 'MSG_ID_WINDSPD_CLOUDVELA_CONTROL_FB', 1, 1, 1),
( 85, 'MSG_ID_TEMP_MODBUS_DATA_READ', 1, 1, 1),
( 86, 'MSG_ID_TEMP_SPIBUSARIES_DATA_READ', 1, 1, 1),
( 87, 'MSG_ID_TEMP_CLOUDVELA_DATA_RESP', 1, 1, 1),
( 88, 'MSG_ID_TEMP_MODBUS_CONTROL_CMD', 1, 1, 1),
( 89, 'MSG_ID_TEMP_SPIBUSARIES_CONTROL_CMD', 1, 1, 1),
( 90, 'MSG_ID_TEMP_CLOUDVELA_CONTROL_FB', 1, 1, 1),
( 91, 'MSG_ID_HUMID_MODBUS_DATA_READ', 1, 1, 1),
( 92, 'MSG_ID_HUMID_CLOUDVELA_DATA_RESP', 1, 1, 1),
( 93, 'MSG_ID_HUMID_MODBUS_CONTROL_CMD', 1, 1, 1),
( 94, 'MSG_ID_HUMID_CLOUDVELA_CONTROL_FB', 1, 1, 1),
( 95, 'MSG_ID_HSMMP_AVORION_DATA_READ', 1, 1, 1),
( 96, 'MSG_ID_HSMMP_AVORION_STOP', 1, 1, 1),
( 97, 'MSG_ID_HSMMP_CLOUDVELA_DATA_RESP', 1, 1, 1),
( 98, 'MSG_ID_HSMMP_CLOUDVELA_CONTROL_FB', 1, 1, 1),
( 99, 'MSG_ID_HSMMP_CLOUDVELA_DATA_LINK_RESP', 1, 1, 1),
( 100, 'MSG_ID_NOISE_SPSVIRGO_DATA_READ', 1, 1, 1),
( 101, 'MSG_ID_NOISE_SPSVIRGO_CONTROL_CMD', 1, 1, 1),
( 102, 'MSG_ID_NOISE_SPSVIRGO_STOP', 1, 1, 1),
( 103, 'MSG_ID_NOISE_CLOUDVELA_DATA_RESP', 1, 1, 1),
( 104, 'MSG_ID_NOISE_CLOUDVELA_CONTROL_FB', 1, 1, 1),
( 105, 'MSG_ID_NOISE_MODBUS_DATA_READ', 1, 1, 1),
( 106, 'MSG_ID_NOISE_MODBUS_CONTROL_CMD', 1, 1, 1),
(107, 'MSG_ID_IWM_NBIOTCJ188_DATA_RESP', 1, 1, 1),
(108, 'MSG_ID_IWM_NBIOTCJ188_CONTROL_FB', 1, 1, 1),
(109, 'MSG_ID_IHM_NBIOTCJ188_DATA_RESP', 1, 1, 1),
(110, 'MSG_ID_IHM_NBIOTCJ188_CONTROL_FB', 1, 1, 1),
(111, 'MSG_ID_IGM_NBIOTCJ188_DATA_RESP', 1, 1, 1),
(112, 'MSG_ID_IGM_NBIOTCJ188_CONTROL_FB', 1, 1, 1),
(113, 'MSG_ID_IPM_NBIOTCJ188_DATA_RESP', 1, 1, 1),
(114, 'MSG_ID_IPM_NBIOTCJ188_CONTROL_FB', 1, 1, 1),
(115, 'MSG_ID_IPM_NBIOTQG376_DATA_RESP', 1, 1, 1),
(116, 'MSG_ID_IPM_NBIOTQG376_CONTROL_FB', 1, 1, 1),
(117, 'MSG_ID_AIRSYNC_INIT', 1, 1, 1),
(118, 'MSG_ID_AIRSYNC_BLE_CONNNECTD', 1, 1, 1),
(119, 'MSG_ID_AIRSYNC_AUTH_REQ', 1, 1, 1),
(120, 'MSG_ID_AIRSYNC_AUTH_RESP', 1, 1, 1),
(121, 'MSG_ID_AIRSYNC_INIT_REQ', 1, 1, 1),
(122, 'MSG_ID_AIRSYNC_INIT_RESP', 1, 1, 1),
(123, 'MSG_ID_AIRSYNC_CONNECT_READY', 1, 1, 1),
(124, 'MSG_ID_AIRSYNC_DATA_PUSH', 1, 1, 1),
(125, 'MSG_ID_AIRSYNC_DATA_REQ', 1, 1, 1),
(126, 'MSG_ID_AIRSYNC_DATA_RESP', 1, 1, 1),
(127, 'MSG_ID_AIRSYNC_AUTH_WAIT_TIMER', 1, 1, 1),
(128, 'MSG_ID_AIRSYNC_DISCONNECT_TIGGER_L3', 1, 1, 1),
(129, 'MSG_ID_AIRSYNC_SEND_BLE_DATA_TIMER', 1, 1, 1),
(130, 'MSG_ID_WXAPP_3MIN_TIMEOUT', 1, 1, 1),
(131, 'MSG_ID_WXAPP_5SECOND_TIMEOUT', 1, 1, 1),
(132, 'MSG_ID_WXAPP_DATA_REQ_EMC', 1, 1, 1),
(133, 'MSG_ID_WXAPP_PERIOD_REPORT_EMC_TRIGGER', 1, 1, 1),
(134, 'MSG_ID_WXAPP_DATA_REQ_TIME_SYNC', 1, 1, 1),
(135, 'MSG_ID_WXAPP_EQUIPMENT_INFO_SYNC', 1, 1, 1),
(136, 'MSG_ID_WXAPP_MODBUS_CONNECT_READY', 1, 1, 1),
(137, 'MSG_ID_WXAPP_MODBUS_DATA_PUSH', 1, 1, 1),
(138, 'MSG_ID_WXAPP_MODBUS_DISCONNECT', 1, 1, 1),
(139, 'MSG_ID_MODBUS_10MIN_TIMEOUT', 1, 1, 1),
(140, 'MSG_ID_MODBUS_5SECOND_TIMEOUT', 1, 1, 1),
(141, 'MSG_ID_MODBUS_PERIOD_REPORT_PM1025_TRIGGER', 1, 1, 1),
(142, 'MSG_ID_MODBUS_PERIOD_REPORT_WIND_SPEED_TRIGGER', 1, 1, 1),
(143, 'MSG_ID_MODBUS_PERIOD_REPORT_WIND_DIR_TRIGGER', 1, 1, 1),
(144, 'MSG_ID_MODBUS_PERIOD_REPORT_TEMPERATURE_TRIGGER', 1, 1, 1),
(145, 'MSG_ID_MODBUS_PERIOD_REPORT_HUMIDITY_TRIGGER', 1, 1, 1),
(146, 'MSG_ID_MODBUS_DATA_REQ_PM1025_REPORT', 1, 1, 1),
(147, 'MSG_ID_MODBUS_DATA_REQ_WIND_SPEED_REPORT', 1, 1, 1),
(148, 'MSG_ID_MODBUS_DATA_REQ_WIND_DIR_REPORT', 1, 1, 1),
(149, 'MSG_ID_MODBUS_DATA_REQ_TEMPERATURE_REPORT', 1, 1, 1),
(150, 'MSG_ID_MODBUS_DATA_REQ_HUMIDITY_REPORT', 1, 1, 1),
(151, 'MSG_ID_MODBUS_UART1_FRAME_TIMEOUT', 1, 1, 1),
(152, 'MSG_ID_COM_MAX', 1, 1, 1);


*/


//查询满足条件的第一条记录
//deviceid是入参，指针为出参
OPSTAT dbi_HcuSysEngPar_inqury(HcuSysEngParTablet_t *engPar, char *prjname)
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
    sqlHandler = mysql_real_connect(sqlHandler, HCU_DB_HOST_DEFAULT, HCU_DB_USER_DEFAULT, HCU_DB_PSW_DEFAULT, HCU_DB_NAME_DEFAULT, HCU_DB_PORT_DEFAULT, NULL, 0);  //unix_socket and clientflag not used.
    if (!sqlHandler){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBICOM: MySQL connection failed!\n");
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
		/*for (index = 0; index < 43; index++){
			printf("Field[%d]=%s\n", index, sqlRow[index]);
		}*/
		index = 0;
		//项目名称
		if(sqlRow[index]) strncpy(engPar->prjname, sqlRow[index++], SYS_ENG_PAR_ELEMENT_DB_LEN-1);
		//通信部分
		if(sqlRow[index]) engPar->comm.commBackHawlCon = (UINT8)(atol(sqlRow[index++]) & 0xFF);
		//printf("engPar->comm.commBackHawlCon = %d, index = %d\n", engPar->comm.commBackHawlCon, index);
		if(sqlRow[index]) engPar->comm.commHwBoardEthernet = (UINT8)(atol(sqlRow[index++]) & 0xFF);
		if(sqlRow[index]) engPar->comm.commHwBoardUsbnet = (UINT8)(atol(sqlRow[index++]) & 0xFF);
		if(sqlRow[index]) engPar->comm.commHwBoardWifi = (UINT8)(atol(sqlRow[index++]) & 0xFF);
		if(sqlRow[index]) engPar->comm.commHwBoard3g4g = (UINT8)(atol(sqlRow[index++]) & 0xFF);
		if(sqlRow[index]) engPar->comm.commHwBoardGps = (UINT8)(atol(sqlRow[index++]) & 0xFF);
		if(sqlRow[index]) engPar->comm.commHwBoardLcd = (UINT8)(atol(sqlRow[index++]) & 0xFF);
		if(sqlRow[index]) engPar->comm.commHwBoardLed = (UINT8)(atol(sqlRow[index++]) & 0xFF);
		if(sqlRow[index]) engPar->comm.commHwBoardZeegbe = (UINT8)(atol(sqlRow[index++]) & 0xFF);
		if(sqlRow[index]) engPar->comm.commHwBoardFlash = (UINT8)(atol(sqlRow[index++]) & 0xFF);
		if(sqlRow[index]) engPar->comm.commFrameModbus = (UINT8)(atol(sqlRow[index++]) & 0xFF);
		if(sqlRow[index]) engPar->comm.commFrameSpsvirgo = (UINT8)(atol(sqlRow[index++]) & 0xFF);
		if(sqlRow[index]) engPar->comm.commFrameAvorion = (UINT8)(atol(sqlRow[index++]) & 0xFF);
		if(sqlRow[index]) engPar->comm.commFrameCloudvela = (UINT8)(atol(sqlRow[index++]) & 0xFF);
		if(sqlRow[index]) engPar->comm.commFrameI2cbuslibra = (UINT8)(atol(sqlRow[index++]) & 0xFF);
		if(sqlRow[index]) engPar->comm.commFrameSpibusaries = (UINT8)(atol(sqlRow[index++]) & 0xFF);
		if(sqlRow[index]) engPar->comm.commFrameNbiotcj188 = (UINT8)(atol(sqlRow[index++]) & 0xFF);
		if(sqlRow[index]) engPar->comm.commFrameNbiotqg376 = (UINT8)(atol(sqlRow[index++]) & 0xFF);
		//printf("engPar->comm.commFrameCloudvela = %d\n", engPar->comm.commFrameCloudvela);
		if(sqlRow[index]) engPar->comm.commFrontSps485 = (UINT8)(atol(sqlRow[index++]) & 0xFF);
		if(sqlRow[index]) engPar->comm.commFrontSps232 = (UINT8)(atol(sqlRow[index++]) & 0xFF);
		if(sqlRow[index]) engPar->comm.commFrontMicrophone = (UINT8)(atol(sqlRow[index++]) & 0xFF);
		if(sqlRow[index]) engPar->comm.commFrontCamera = (UINT8)(atol(sqlRow[index++]) & 0xFF);
		if(sqlRow[index]) engPar->comm.commFrontBle = (UINT8)(atol(sqlRow[index++]) & 0xFF);
		if(sqlRow[index]) engPar->comm.commFrontGpio = (UINT8)(atol(sqlRow[index++]) & 0xFF);
		if(sqlRow[index]) engPar->comm.commFrontI2c = (UINT8)(atol(sqlRow[index++]) & 0xFF);
		if(sqlRow[index]) engPar->comm.commFrontSpi = (UINT8)(atol(sqlRow[index++]) & 0xFF);
		if(sqlRow[index]) engPar->comm.commFrontPwm = (UINT8)(atol(sqlRow[index++]) & 0xFF);
		if(sqlRow[index]) engPar->comm.commFrontAdc = (UINT8)(atol(sqlRow[index++]) & 0xFF);
		if(sqlRow[index]) engPar->comm.commFrontSwitch = (UINT8)(atol(sqlRow[index++]) & 0xFF);
		if(sqlRow[index]) engPar->comm.commFrontRelay = (UINT8)(atol(sqlRow[index++]) & 0xFF);
		if(sqlRow[index]) engPar->comm.commFrontMotor = (UINT8)(atol(sqlRow[index++]) & 0xFF);
		if(sqlRow[index]) engPar->comm.commFrontSensorEmc = (UINT8)(atol(sqlRow[index++]) & 0xFF);
		if(sqlRow[index]) engPar->comm.commFrontSensorPm25 = (UINT8)(atol(sqlRow[index++]) & 0xFF);
		if(sqlRow[index]) engPar->comm.commFrontSensorTemp = (UINT8)(atol(sqlRow[index++]) & 0xFF);
		if(sqlRow[index]) engPar->comm.commFrontSensorHumid = (UINT8)(atol(sqlRow[index++]) & 0xFF);
		if(sqlRow[index]) engPar->comm.commFrontSensorWinddir = (UINT8)(atol(sqlRow[index++]) & 0xFF);
		if(sqlRow[index]) engPar->comm.commFrontSensorWindspd = (UINT8)(atol(sqlRow[index++]) & 0xFF);
		if(sqlRow[index]) engPar->comm.commFrontSensorNoise = (UINT8)(atol(sqlRow[index++]) & 0xFF);
		if(sqlRow[index]) engPar->comm.commFrontSensorHsmmp = (UINT8)(atol(sqlRow[index++]) & 0xFF);
		if(sqlRow[index]) engPar->comm.commFrontSensorAirprs = (UINT8)(atol(sqlRow[index++]) & 0xFF);
		if(sqlRow[index]) engPar->comm.commFrontSensorCo1 = (UINT8)(atol(sqlRow[index++]) & 0xFF);
		if(sqlRow[index]) engPar->comm.commFrontSensorLightstr = (UINT8)(atol(sqlRow[index++]) & 0xFF);
		if(sqlRow[index]) engPar->comm.commFrontSensorAlcohol = (UINT8)(atol(sqlRow[index++]) & 0xFF);
		if(sqlRow[index]) engPar->comm.commFrontSensorHcho = (UINT8)(atol(sqlRow[index++]) & 0xFF);
		if(sqlRow[index]) engPar->comm.commFrontSensorToxicgas = (UINT8)(atol(sqlRow[index++]) & 0xFF);
		if(sqlRow[index]) engPar->comm.commFrontSensorIwm = (UINT8)(atol(sqlRow[index++]) & 0xFF);
		if(sqlRow[index]) engPar->comm.commFrontSensorIhm = (UINT8)(atol(sqlRow[index++]) & 0xFF);
		if(sqlRow[index]) engPar->comm.commFrontSensorIgm = (UINT8)(atol(sqlRow[index++]) & 0xFF);
		if(sqlRow[index]) engPar->comm.commFrontSensorIpm = (UINT8)(atol(sqlRow[index++]) & 0xFF);
		if(sqlRow[index]) engPar->comm.commFrontSensorPm25Sharp = (UINT8)(atol(sqlRow[index++]) & 0xFF);

		//数据库部分
		if(sqlRow[index]) strncpy(engPar->dbi.hcuDbHost, sqlRow[index++], SYS_ENG_PAR_ELEMENT_DB_LEN-1);
		if(sqlRow[index]) strncpy(engPar->dbi.hcuDbUser, sqlRow[index++], SYS_ENG_PAR_ELEMENT_DB_LEN-1);
		if(sqlRow[index]) strncpy(engPar->dbi.hcuDbPsw, sqlRow[index++], SYS_ENG_PAR_ELEMENT_DB_LEN-1);
		if(sqlRow[index]) strncpy(engPar->dbi.hcuDbName, sqlRow[index++], SYS_ENG_PAR_ELEMENT_DB_LEN-1);
		if(sqlRow[index]) engPar->dbi.hcuDbPort = (UINT8)(atol(sqlRow[index++]) & 0xFF);

		//Timer setting by Shanchun
		if(sqlRow[index]) engPar->timer.emcReqTimer = (UINT32)(atol(sqlRow[index++]) & 0xFFFFFFFF);
		if(sqlRow[index]) engPar->timer.emcReqTimerFB = (UINT32)(atol(sqlRow[index++]) & 0xFFFFFFFF);
		if(sqlRow[index]) engPar->timer.humidReqTimer = (UINT32)(atol(sqlRow[index++]) & 0xFFFFFFFF);
		if(sqlRow[index]) engPar->timer.humidReqTimerFB = (UINT32)(atol(sqlRow[index++]) & 0xFFFFFFFF);
		if(sqlRow[index]) engPar->timer.noiseReqTimer = (UINT32)(atol(sqlRow[index++]) & 0xFFFFFFFF);
		if(sqlRow[index]) engPar->timer.noiseReqTimerFB = (UINT32)(atol(sqlRow[index++]) & 0xFFFFFFFF);
		if(sqlRow[index]) engPar->timer.pm25ReqTimer = (UINT32)(atol(sqlRow[index++]) & 0xFFFFFFFF);
		if(sqlRow[index]) engPar->timer.pm25ReqTimerFB = (UINT32)(atol(sqlRow[index++]) & 0xFFFFFFFF);
		if(sqlRow[index]) engPar->timer.tempReqTimer = (UINT32)(atol(sqlRow[index++]) & 0xFFFFFFFF);
		if(sqlRow[index]) engPar->timer.tempReqTimerFB = (UINT32)(atol(sqlRow[index++]) & 0xFFFFFFFF);
		if(sqlRow[index]) engPar->timer.winddirReqTimer = (UINT32)(atol(sqlRow[index++]) & 0xFFFFFFFF);
		if(sqlRow[index]) engPar->timer.winddirReqTimerFB = (UINT32)(atol(sqlRow[index++]) & 0xFFFFFFFF);
		if(sqlRow[index]) engPar->timer.windspdReqTimer = (UINT32)(atol(sqlRow[index++]) & 0xFFFFFFFF);
		if(sqlRow[index]) engPar->timer.windspdReqTimerFB = (UINT32)(atol(sqlRow[index++]) & 0xFFFFFFFF);
		if(sqlRow[index]) engPar->timer.cloudvelaHbTimer = (UINT32)(atol(sqlRow[index++]) & 0xFFFFFFFF);
		if(sqlRow[index]) engPar->timer.cloudvelaHbBackTimer = (UINT32)(atol(sqlRow[index++]) & 0xFFFFFFFF);
		if(sqlRow[index]) engPar->timer.nbiotcj188HbTimer = (UINT32)(atol(sqlRow[index++]) & 0xFFFFFFFF);
		if(sqlRow[index]) engPar->timer.nbiotcj188HbBackTimer = (UINT32)(atol(sqlRow[index++]) & 0xFFFFFFFF);
		if(sqlRow[index]) engPar->timer.nbiotqg376HbTimer = (UINT32)(atol(sqlRow[index++]) & 0xFFFFFFFF);
		if(sqlRow[index]) engPar->timer.nbiotqg376HbBackTimer = (UINT32)(atol(sqlRow[index++]) & 0xFFFFFFFF);
		if(sqlRow[index]) engPar->timer.cmdcontrolLongTimer = (UINT32)(atol(sqlRow[index++]) & 0xFFFFFFFF);
		if(sqlRow[index]) engPar->timer.cmdcontrolShortTimer = (UINT32)(atol(sqlRow[index++]) & 0xFFFFFFFF);
		if(sqlRow[index]) engPar->timer.hsmmpReqTimer = (UINT32)(atol(sqlRow[index++]) & 0xFFFFFFFF);
		if(sqlRow[index]) engPar->timer.hsmmpCapDuration = (UINT32)(atol(sqlRow[index++]) & 0xFFFFFFFF);
		if(sqlRow[index]) engPar->timer.hsmmpCapDurationFB = (UINT32)(atol(sqlRow[index++]) & 0xFFFFFFFF);
		if(sqlRow[index]) engPar->timer.hsmmpRefRate = (UINT32)(atol(sqlRow[index++]) & 0xFFFFFFFF);
		if(sqlRow[index]) engPar->timer.airprsReqTimer = (UINT32)(atol(sqlRow[index++]) & 0xFFFFFFFF);
		if(sqlRow[index]) engPar->timer.co1ReqTimer = (UINT32)(atol(sqlRow[index++]) & 0xFFFFFFFF);
		if(sqlRow[index]) engPar->timer.lightstrReqTimer = (UINT32)(atol(sqlRow[index++]) & 0xFFFFFFFF);
		if(sqlRow[index]) engPar->timer.alcoholReqTimer = (UINT32)(atol(sqlRow[index++]) & 0xFFFFFFFF);
		if(sqlRow[index]) engPar->timer.hchoReqTimer = (UINT32)(atol(sqlRow[index++]) & 0xFFFFFFFF);
		if(sqlRow[index]) engPar->timer.toxicgasReqTimer = (UINT32)(atol(sqlRow[index++]) & 0xFFFFFFFF);
		if(sqlRow[index]) engPar->timer.iwmReqTimer = (UINT32)(atol(sqlRow[index++]) & 0xFFFFFFFF);
		if(sqlRow[index]) engPar->timer.ihmReqTimer = (UINT32)(atol(sqlRow[index++]) & 0xFFFFFFFF);
		if(sqlRow[index]) engPar->timer.igmReqTimer = (UINT32)(atol(sqlRow[index++]) & 0xFFFFFFFF);
		if(sqlRow[index]) engPar->timer.ipmReqTimer = (UINT32)(atol(sqlRow[index++]) & 0xFFFFFFFF);
		if(sqlRow[index]) engPar->timer.pm25sharpReqTimer = (UINT32)(atol(sqlRow[index++]) & 0xFFFFFFFF);
		if(sqlRow[index]) engPar->timer.syspmWorkingTimer = (UINT32)(atol(sqlRow[index++]) & 0xFFFFFFFF);

		//Series Port config by Shanchun
		if(sqlRow[index]) engPar->serialport.SeriesPortForModbus = (UINT8)(atol(sqlRow[index++]) & 0xFF);
		//if(sqlRow[index]) engPar->serialport.BautRateForModbusPort = (UINT8)(atol(sqlRow[index++]) & 0xFF);
		if(sqlRow[index]) engPar->serialport.SeriesPortForGPS = (UINT8)(atol(sqlRow[index++]) & 0xFF);
		if(sqlRow[index]) engPar->serialport.SeriesPortForPm25Sharp = (UINT8)(atol(sqlRow[index++]) & 0xFF);
		//if(sqlRow[index]) engPar->serialport.BautRate = (UINT8)(atol(sqlRow[index++]) & 0xFF);

		//后台链接部分
		if(sqlRow[index]) strncpy(engPar->cloud.cloudHttpAddLocal, sqlRow[index++], SYS_ENG_PAR_ELEMENT_CLOUDVELA_LEN-1);
		//printf("engPar->cloud.cloudHttpAddLocal = %s\n", engPar->cloud.cloudHttpAddLocal);
		if(sqlRow[index]) strncpy(engPar->cloud.cloudHttpAddTest, sqlRow[index++], SYS_ENG_PAR_ELEMENT_CLOUDVELA_LEN-1);
		if(sqlRow[index]) strncpy(engPar->cloud.cloudHttpAddSae, sqlRow[index++], SYS_ENG_PAR_ELEMENT_CLOUDVELA_LEN-1);
		if(sqlRow[index]) strncpy(engPar->cloud.cloudHttpAddJd, sqlRow[index++], SYS_ENG_PAR_ELEMENT_CLOUDVELA_LEN-1);
		if(sqlRow[index]) strncpy(engPar->cloud.cloudHttpAddWechat, sqlRow[index++], SYS_ENG_PAR_ELEMENT_CLOUDVELA_LEN-1);
		if(sqlRow[index]) strncpy(engPar->cloud.cloudBhServerName, sqlRow[index++], SYS_ENG_PAR_ELEMENT_CLOUDVELA_NAME-1);
		if(sqlRow[index]) strncpy(engPar->cloud.cloudBhHcuName, sqlRow[index++], SYS_ENG_PAR_ELEMENT_CLOUDVELA_NAME-1);
		if(sqlRow[index]) engPar->cloud.cloudBhItfFrameStd = (UINT8)(atol(sqlRow[index++]) & 0xFF);

		//For HCU SW download
		if(sqlRow[index]) strncpy(engPar->cloud.cloudFtpAdd, sqlRow[index++], SYS_ENG_PAR_ELEMENT_CLOUDVELA_LEN-1);
		if(sqlRow[index]) strncpy(engPar->cloud.cloudFtpUser, sqlRow[index++], SYS_ENG_PAR_ELEMENT_CLOUDVELA_NAME-1);
		if(sqlRow[index]) strncpy(engPar->cloud.cloudFtpPwd, sqlRow[index++], SYS_ENG_PAR_ELEMENT_CLOUDVELA_NAME-1);

		//For HCU video upload
		if(sqlRow[index]) strncpy(engPar->cloud.cloudFtpUserVideo, sqlRow[index++], SYS_ENG_PAR_ELEMENT_CLOUDVELA_NAME-1);
		if(sqlRow[index]) strncpy(engPar->cloud.cloudFtpPwdVideo, sqlRow[index++], SYS_ENG_PAR_ELEMENT_CLOUDVELA_NAME-1);


		if(sqlRow[index]) strncpy(engPar->swDownload.hcuSwDownloadDir, sqlRow[index++], SYS_ENG_PAR_ELEMENT_SWDOWNLOAD_LEN-1);
		if(sqlRow[index]) strncpy(engPar->swDownload.hcuSwActiveDir, sqlRow[index++], SYS_ENG_PAR_ELEMENT_SWDOWNLOAD_LEN-1);
		if(sqlRow[index]) strncpy(engPar->swDownload.hcuSwBackupDir, sqlRow[index++], SYS_ENG_PAR_ELEMENT_SWDOWNLOAD_LEN-1);


		//视频服务器部分
		if(sqlRow[index]) strncpy(engPar->videoSev.hcuVideoServerDir, sqlRow[index++], SYS_ENG_PAR_ELEMENT_VIDEO_SERVER_LEN-1);
		if(sqlRow[index]) strncpy(engPar->videoSev.hcuVideoServerHttp, sqlRow[index++], SYS_ENG_PAR_ELEMENT_VIDEO_SERVER_LEN-1);
		//printf ("DBICOM: index = %d, hcuVideoServerHttp = %s\n", index, engPar->videoSev.hcuVideoServerHttp);
		//调试部分
		if(sqlRow[index]) engPar->debugMode = (UINT8)(atol(sqlRow[index++]) & 0xFF);
		//TRACE部分
		if(sqlRow[index]) engPar->traceMode = (UINT8)(atol(sqlRow[index++]) & 0xFF);
		//LocalUI本地界面启动部分
		if(sqlRow[index]) engPar->localUI.browselAutoStartUpFlag = (UINT8)(atol(sqlRow[index++]) & 0xFF);
		if(sqlRow[index]) strncpy(engPar->localUI.browselProg, sqlRow[index++], SYS_ENG_PAR_ELEMENT_LOCAL_UI_PROG_LEN-1);
		if(sqlRow[index]) strncpy(engPar->localUI.browselStartUpAddress, sqlRow[index++], SYS_ENG_PAR_ELEMENT_LOCAL_UI_ADD_LEN-1);
		//Check segment position is right or not: [http://localhost/yii2basic/web/index.php]
		//HcuDebugPrint("DBICOM: Pure test to cross-check whether it is the right position of segment, StartupAddress = [%s]!\n", engPar->localUI.browselStartUpAddress);
		if(sqlRow[index]) strncpy(engPar->localUI.browselWorkingOption, sqlRow[index++], SYS_ENG_PAR_ELEMENT_LOCAL_UI_OPTION_LEN-1);
	}

	//释放记录集
	mysql_free_result(resPtr);
    mysql_close(sqlHandler);
    return SUCCESS;
}


//查询所有基于模块控制的TRACE记录，并存入全局控制变量
OPSTAT dbi_HcuTraceModuleCtr_inqury(HcuSysEngParTablet_t *engPar)
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
    sqlHandler = mysql_real_connect(sqlHandler, HCU_DB_HOST_DEFAULT, HCU_DB_USER_DEFAULT, HCU_DB_PSW_DEFAULT, HCU_DB_NAME_DEFAULT, HCU_DB_PORT_DEFAULT, NULL, 0);  //unix_socket and clientflag not used.
    if (!sqlHandler){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBICOM: MySQL connection failed!\n");
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
		UINT16 moduleId = 0;
		int index = 0;
		if(sqlRow[index]) moduleId = (UINT8)(atol(sqlRow[index++]) & 0xFF);
		if (moduleId >= MAX_TASK_NUM_IN_ONE_HCU){
			HcuErrorPrint("DBICOM: Error ModuleID populated, very dangerous to crash whole system! ModueId = %d\n", moduleId);
			mysql_free_result(resPtr);
		    mysql_close(sqlHandler);
		    return FAILURE;
		}
		engPar->traceList.mod[moduleId].moduleId = moduleId;
		if(sqlRow[index]) strncpy(engPar->traceList.mod[moduleId].moduleName, sqlRow[index++], TASK_NAME_MAX_LENGTH-1);
		if (strcmp(engPar->traceList.mod[moduleId].moduleName, zHcuTaskNameList[moduleId])){
			if ((zHcuSysEngPar.debugMode & HCU_TRACE_DEBUG_CRT_ON) != FALSE){
				HcuDebugPrint("DBICOM: Error Module name populated!\n");
			}
		}
		if(sqlRow[index]) engPar->traceList.mod[moduleId].moduleCtrFlag = (UINT8)(atol(sqlRow[index++]) & 0xFF);
		if(sqlRow[index]) engPar->traceList.mod[moduleId].moduleToAllow = (UINT8)(atol(sqlRow[index++]) & 0xFF);
		if(sqlRow[index]) engPar->traceList.mod[moduleId].moduleToRestrict = (UINT8)(atol(sqlRow[index++]) & 0xFF);
		if(sqlRow[index]) engPar->traceList.mod[moduleId].moduleFromAllow = (UINT8)(atol(sqlRow[index++]) & 0xFF);
		if(sqlRow[index]) engPar->traceList.mod[moduleId].moduleFromRestrict = (UINT8)(atol(sqlRow[index++]) & 0xFF);
	}

	//释放记录集
	mysql_free_result(resPtr);
    mysql_close(sqlHandler);
    return SUCCESS;
}

//查询所有基于模块控制的TRACE记录，并存入全局控制变量
OPSTAT dbi_HcuTraceMsgCtr_inqury(HcuSysEngParTablet_t *engPar)
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
    sqlHandler = mysql_real_connect(sqlHandler, HCU_DB_HOST_DEFAULT, HCU_DB_USER_DEFAULT, HCU_DB_PSW_DEFAULT, HCU_DB_NAME_DEFAULT, HCU_DB_PORT_DEFAULT, NULL, 0);  //unix_socket and clientflag not used.
    if (!sqlHandler){
    	mysql_close(sqlHandler);
    	HcuErrorPrint("DBICOM: MySQL connection failed!\n");
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
		UINT16 msgId = 0;
		int index = 0;
		if(sqlRow[index]) msgId = (UINT8)(atol(sqlRow[index++]) & 0xFF);
		if (msgId >= MAX_MSGID_NUM_IN_ONE_TASK){
			HcuErrorPrint("DBICOM: Error MsgID populated, very dangerous to crash whole system! MsgID = %d\n", msgId);
			mysql_free_result(resPtr);
		    mysql_close(sqlHandler);
		    return FAILURE;
		}
		engPar->traceList.msg[msgId].msgId = msgId;
		if(sqlRow[index]) strncpy(engPar->traceList.msg[msgId].msgName, sqlRow[index++], MSG_NAME_MAX_LENGTH-1);
		if (strcmp(engPar->traceList.msg[msgId].msgName, zHcuMsgNameList[msgId])){
			if ((zHcuSysEngPar.debugMode & HCU_TRACE_DEBUG_CRT_ON) != FALSE){
				HcuDebugPrint("DBICOM: Error Message name populated, MsgId = %d, engPar->traceList.msg = [%s], zHcuMsgNameList = [%s]!\n", msgId, engPar->traceList.msg[msgId].msgName, zHcuMsgNameList[msgId]);
			}
		}
		if(sqlRow[index]) engPar->traceList.msg[msgId].msgCtrFlag = (UINT8)(atol(sqlRow[index++]) & 0xFF);
		if(sqlRow[index]) engPar->traceList.msg[msgId].msgAllow = (UINT8)(atol(sqlRow[index++]) & 0xFF);
		if(sqlRow[index]) engPar->traceList.msg[msgId].msgRestrict = (UINT8)(atol(sqlRow[index++]) & 0xFF);
	}

	//释放记录集
	mysql_free_result(resPtr);
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



