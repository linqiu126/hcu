-- phpMyAdmin SQL Dump
-- version 4.2.12deb2+deb8u1
-- http://www.phpmyadmin.net
--
-- Host: localhost
-- Generation Time: Jun 04, 2017 at 04:14 PM
-- Server version: 5.5.44-0+deb8u1
-- PHP Version: 5.6.20-0+deb8u1

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;

--
-- Database: `hcudb`
--

-- --------------------------------------------------------

--
-- Table structure for table `hcuairprsaltitudebmp180datainfo`
--

CREATE TABLE IF NOT EXISTS `hcuairprsaltitudebmp180datainfo` (
`sid` int(4) NOT NULL,
  `deviceid` int(4) NOT NULL,
  `timestamp` int(4) NOT NULL,
  `dataformat` int(1) NOT NULL,
  `altitudevalue` int(4) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Table structure for table `hcuairprsbmp180datainfo`
--

CREATE TABLE IF NOT EXISTS `hcuairprsbmp180datainfo` (
`sid` int(4) NOT NULL,
  `deviceid` int(4) NOT NULL,
  `timestamp` int(4) NOT NULL,
  `dataformat` int(1) NOT NULL,
  `airprsvalue` int(4) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Table structure for table `hcuairprsdatainfo`
--

CREATE TABLE IF NOT EXISTS `hcuairprsdatainfo` (
`sid` int(4) NOT NULL,
  `deviceid` int(4) NOT NULL,
  `timestamp` int(4) NOT NULL,
  `dataformat` int(1) NOT NULL,
  `airprsvalue` int(4) NOT NULL,
  `ew` char(1) NOT NULL,
  `gpsx` int(4) NOT NULL,
  `ns` char(1) NOT NULL,
  `gpsy` int(4) NOT NULL,
  `gpsz` int(4) NOT NULL,
  `onofflineflag` int(2) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Table structure for table `hcualcoholdatainfo`
--

CREATE TABLE IF NOT EXISTS `hcualcoholdatainfo` (
`sid` int(4) NOT NULL,
  `deviceid` int(4) NOT NULL,
  `timestamp` int(4) NOT NULL,
  `dataformat` int(1) NOT NULL,
  `alcoholvalue` int(4) NOT NULL,
  `ew` char(1) NOT NULL,
  `gpsx` int(4) NOT NULL,
  `ns` char(1) NOT NULL,
  `gpsy` int(4) NOT NULL,
  `gpsz` int(4) NOT NULL,
  `onofflineflag` int(2) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Table structure for table `hcualcoholmq3alcodatainfo`
--

CREATE TABLE IF NOT EXISTS `hcualcoholmq3alcodatainfo` (
`sid` int(4) NOT NULL,
  `deviceid` int(4) NOT NULL,
  `timestamp` int(4) NOT NULL,
  `dataformat` int(1) NOT NULL,
  `alcoholvalue` int(4) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Table structure for table `hcuco1datainfo`
--

CREATE TABLE IF NOT EXISTS `hcuco1datainfo` (
`sid` int(4) NOT NULL,
  `deviceid` int(4) NOT NULL,
  `timestamp` int(4) NOT NULL,
  `dataformat` int(1) NOT NULL,
  `co1value` int(4) NOT NULL,
  `ew` char(1) NOT NULL,
  `gpsx` int(4) NOT NULL,
  `ns` char(1) NOT NULL,
  `gpsy` int(4) NOT NULL,
  `gpsz` int(4) NOT NULL,
  `onofflineflag` int(2) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Table structure for table `hcuemcdatainfo`
--

CREATE TABLE IF NOT EXISTS `hcuemcdatainfo` (
`sid` int(4) NOT NULL,
  `deviceid` int(4) NOT NULL,
  `timestamp` int(4) NOT NULL,
  `dataformat` int(1) NOT NULL,
  `emcvalue` int(4) NOT NULL,
  `EW` char(1) NOT NULL,
  `gpsx` int(4) NOT NULL,
  `NS` char(1) NOT NULL,
  `gpsy` int(4) NOT NULL,
  `gpsz` int(4) NOT NULL,
  `onofflineflag` int(2) NOT NULL
) ENGINE=InnoDB AUTO_INCREMENT=2726 DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Table structure for table `hcuhchodatainfo`
--

CREATE TABLE IF NOT EXISTS `hcuhchodatainfo` (
`sid` int(4) NOT NULL,
  `deviceid` int(4) NOT NULL,
  `timestamp` int(4) NOT NULL,
  `dataformat` int(1) NOT NULL,
  `hchovalue` int(4) NOT NULL,
  `ew` char(1) NOT NULL,
  `gpsx` int(4) NOT NULL,
  `ns` char(1) NOT NULL,
  `gpsy` int(4) NOT NULL,
  `gpsz` int(4) NOT NULL,
  `onofflineflag` int(2) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Table structure for table `hcuhchoze08ch2odatainfo`
--

CREATE TABLE IF NOT EXISTS `hcuhchoze08ch2odatainfo` (
`sid` int(4) NOT NULL,
  `deviceid` int(4) NOT NULL,
  `timestamp` int(4) NOT NULL,
  `dataformat` int(1) NOT NULL,
  `hchovalue` int(4) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Table structure for table `hcuhsmmpdatainfo`
--

CREATE TABLE IF NOT EXISTS `hcuhsmmpdatainfo` (
`sid` int(4) NOT NULL,
  `deviceid` int(4) NOT NULL,
  `timestamp` int(4) NOT NULL,
  `hsmmpfdir` char(100) NOT NULL,
  `hsmmpfname` char(100) NOT NULL,
  `hsmmplink` char(100) NOT NULL,
  `EW` char(1) NOT NULL,
  `gpsx` int(4) NOT NULL,
  `NS` char(1) NOT NULL,
  `gpsy` int(4) NOT NULL,
  `gpsz` int(4) NOT NULL,
  `onofflineflag` int(2) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Table structure for table `hcuhumiddatainfo`
--

CREATE TABLE IF NOT EXISTS `hcuhumiddatainfo` (
`sid` int(4) NOT NULL,
  `deviceid` int(4) NOT NULL,
  `timestamp` int(4) NOT NULL,
  `dataformat` int(1) NOT NULL,
  `humidvalue` int(4) NOT NULL,
  `EW` char(1) NOT NULL,
  `gpsx` int(4) NOT NULL,
  `NS` char(1) NOT NULL,
  `gpsy` int(4) NOT NULL,
  `gpsz` int(4) NOT NULL,
  `onofflineflag` int(2) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Table structure for table `hcuhumiddht11datainfo`
--

CREATE TABLE IF NOT EXISTS `hcuhumiddht11datainfo` (
`sid` int(4) NOT NULL,
  `deviceid` int(4) NOT NULL,
  `timestamp` int(4) NOT NULL,
  `dataformat` int(1) NOT NULL,
  `humidvalue` int(4) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Table structure for table `hcuhumidmth01datainfo`
--

CREATE TABLE IF NOT EXISTS `hcuhumidmth01datainfo` (
`sid` int(4) NOT NULL,
  `deviceid` int(4) NOT NULL,
  `timestamp` int(4) NOT NULL,
  `dataformat` int(1) NOT NULL,
  `humidvalue` int(4) NOT NULL
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=utf8;

--
-- Dumping data for table `hcuhumidmth01datainfo`
--

INSERT INTO `hcuhumidmth01datainfo` (`sid`, `deviceid`, `timestamp`, `dataformat`, `humidvalue`) VALUES
(1, 3, 2, 1, 4);

-- --------------------------------------------------------

--
-- Table structure for table `hcuhumidrht03datainfo`
--

CREATE TABLE IF NOT EXISTS `hcuhumidrht03datainfo` (
`sid` int(4) NOT NULL,
  `deviceid` int(4) NOT NULL,
  `timestamp` int(4) NOT NULL,
  `dataformat` int(1) NOT NULL,
  `humidvalue` int(4) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Table structure for table `hcuhumidsht20datainfo`
--

CREATE TABLE IF NOT EXISTS `hcuhumidsht20datainfo` (
`sid` int(4) NOT NULL,
  `deviceid` int(4) NOT NULL,
  `timestamp` int(4) NOT NULL,
  `dataformat` int(1) NOT NULL,
  `humidvalue` int(4) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Table structure for table `hcuigmcj188datainfo`
--

CREATE TABLE IF NOT EXISTS `hcuigmcj188datainfo` (
`sid` int(4) NOT NULL,
  `cj188address` char(14) NOT NULL,
  `timestamp` int(4) NOT NULL,
  `equtype` int(1) NOT NULL,
  `billtodayaccuvolume` float(8,2) NOT NULL,
  `billtodayaccuvolumeunit` int(1) NOT NULL,
  `currentaccuvolume` float(8,2) NOT NULL,
  `currentaccuvolumeunit` int(1) NOT NULL,
  `flowvolume` float(8,2) NOT NULL,
  `flowvolumeunit` int(1) NOT NULL,
  `lastmonth` int(1) NOT NULL,
  `accumuworktime` int(3) NOT NULL,
  `supplywatertemp` float(6,2) NOT NULL,
  `backwatertemp` float(6,2) NOT NULL,
  `realtime` char(14) NOT NULL,
  `st` char(4) NOT NULL,
  `billtodaydate` int(1) NOT NULL,
  `readamountcurdate` int(1) NOT NULL,
  `startdate` int(1) NOT NULL,
  `key` int(8) NOT NULL,
  `price1` float(6,2) NOT NULL,
  `volume1` int(3) NOT NULL,
  `price2` float(6,2) NOT NULL,
  `volume2` int(3) NOT NULL,
  `price3` float(6,2) NOT NULL,
  `buycode` int(1) NOT NULL,
  `thisamount` float(8,2) NOT NULL,
  `accuamount` float(8,2) NOT NULL,
  `remainamount` float(8,2) NOT NULL,
  `keyver` int(1) NOT NULL,
  `switchctrl` int(1) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Table structure for table `hcuihmcj188datainfo`
--

CREATE TABLE IF NOT EXISTS `hcuihmcj188datainfo` (
`sid` int(4) NOT NULL,
  `cj188address` char(14) NOT NULL,
  `timestamp` int(4) NOT NULL,
  `equtype` int(1) NOT NULL,
  `heatpower` float(8,2) NOT NULL,
  `heatpowerunit` int(1) NOT NULL,
  `currentheat` float(8,2) NOT NULL,
  `currentheatunit` int(1) NOT NULL,
  `billtodayheat` float(8,2) NOT NULL,
  `billtodayheatunit` int(1) NOT NULL,
  `currentaccuvolume` float(8,2) NOT NULL,
  `currentaccuvolumeunit` int(1) NOT NULL,
  `flowvolume` float(8,2) NOT NULL,
  `flowvolumeunit` int(1) NOT NULL,
  `lastmonth` int(1) NOT NULL,
  `accumuworktime` int(3) NOT NULL,
  `supplywatertemp` float(6,2) NOT NULL,
  `backwatertemp` float(6,2) NOT NULL,
  `realtime` char(14) NOT NULL,
  `st` char(4) NOT NULL,
  `billtodaydate` int(1) NOT NULL,
  `readamountcurdate` int(1) NOT NULL,
  `startdate` int(1) NOT NULL,
  `key` int(8) NOT NULL,
  `price1` float(6,2) NOT NULL,
  `volume1` int(3) NOT NULL,
  `price2` float(6,2) NOT NULL,
  `volume2` int(3) NOT NULL,
  `price3` float(6,2) NOT NULL,
  `buycode` int(1) NOT NULL,
  `thisamount` float(8,2) NOT NULL,
  `accuamount` float(8,2) NOT NULL,
  `remainamount` float(8,2) NOT NULL,
  `keyver` int(1) NOT NULL,
  `switchctrl` int(1) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Table structure for table `hcuipmcj188datainfo`
--

CREATE TABLE IF NOT EXISTS `hcuipmcj188datainfo` (
`sid` int(4) NOT NULL,
  `cj188address` char(14) NOT NULL,
  `timestamp` int(4) NOT NULL,
  `equtype` int(1) NOT NULL,
  `billtodayaccuvolume` float(8,2) NOT NULL,
  `billtodayaccuvolumeunit` int(1) NOT NULL,
  `currentaccuvolume` float(8,2) NOT NULL,
  `currentaccuvolumeunit` int(1) NOT NULL,
  `flowvolume` float(8,2) NOT NULL,
  `flowvolumeunit` int(1) NOT NULL,
  `lastmonth` int(1) NOT NULL,
  `accumuworktime` int(3) NOT NULL,
  `supplywatertemp` float(6,2) NOT NULL,
  `backwatertemp` float(6,2) NOT NULL,
  `realtime` char(14) NOT NULL,
  `st` char(4) NOT NULL,
  `billtodaydate` int(1) NOT NULL,
  `readamountcurdate` int(1) NOT NULL,
  `startdate` int(1) NOT NULL,
  `key` int(8) NOT NULL,
  `price1` float(6,2) NOT NULL,
  `volume1` int(3) NOT NULL,
  `price2` float(6,2) NOT NULL,
  `volume2` int(3) NOT NULL,
  `price3` float(6,2) NOT NULL,
  `buycode` int(1) NOT NULL,
  `thisamount` float(8,2) NOT NULL,
  `accuamount` float(8,2) NOT NULL,
  `remainamount` float(8,2) NOT NULL,
  `keyver` int(1) NOT NULL,
  `switchctrl` int(1) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Table structure for table `hcuiwmcj188datainfo`
--

CREATE TABLE IF NOT EXISTS `hcuiwmcj188datainfo` (
`sid` int(4) NOT NULL,
  `cj188address` char(14) NOT NULL,
  `timestamp` int(4) NOT NULL,
  `equtype` int(1) NOT NULL,
  `billtodayaccuvolume` float(8,2) NOT NULL,
  `billtodayaccuvolumeunit` int(1) NOT NULL,
  `currentaccuvolume` float(8,2) NOT NULL,
  `currentaccuvolumeunit` int(1) NOT NULL,
  `flowvolume` float(8,2) NOT NULL,
  `flowvolumeunit` int(1) NOT NULL,
  `lastmonth` int(1) NOT NULL,
  `accumuworktime` int(3) NOT NULL,
  `supplywatertemp` float(6,2) NOT NULL,
  `backwatertemp` float(6,2) NOT NULL,
  `realtime` char(14) NOT NULL,
  `st` char(4) NOT NULL,
  `billtodaydate` int(1) NOT NULL,
  `readamountcurdate` int(1) NOT NULL,
  `startdate` int(1) NOT NULL,
  `key` int(8) NOT NULL,
  `price1` float(6,2) NOT NULL,
  `volume1` int(3) NOT NULL,
  `price2` float(6,2) NOT NULL,
  `volume2` int(3) NOT NULL,
  `price3` float(6,2) NOT NULL,
  `buycode` int(1) NOT NULL,
  `thisamount` float(8,2) NOT NULL,
  `accuamount` float(8,2) NOT NULL,
  `remainamount` float(8,2) NOT NULL,
  `keyver` int(1) NOT NULL,
  `switchctrl` int(1) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Table structure for table `hculightstrbh1750datainfo`
--

CREATE TABLE IF NOT EXISTS `hculightstrbh1750datainfo` (
`sid` int(4) NOT NULL,
  `deviceid` int(4) NOT NULL,
  `timestamp` int(4) NOT NULL,
  `dataformat` int(1) NOT NULL,
  `lightstrvalue` int(4) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Table structure for table `hculightstrdatainfo`
--

CREATE TABLE IF NOT EXISTS `hculightstrdatainfo` (
`sid` int(4) NOT NULL,
  `deviceid` int(4) NOT NULL,
  `timestamp` int(4) NOT NULL,
  `dataformat` int(1) NOT NULL,
  `lightstrvalue` int(4) NOT NULL,
  `ew` char(1) NOT NULL,
  `gpsx` int(4) NOT NULL,
  `ns` char(1) NOT NULL,
  `gpsy` int(4) NOT NULL,
  `gpsz` int(4) NOT NULL,
  `onofflineflag` int(2) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Table structure for table `hcunoisedatainfo`
--

CREATE TABLE IF NOT EXISTS `hcunoisedatainfo` (
`sid` int(4) NOT NULL,
  `deviceid` int(4) NOT NULL,
  `timestamp` int(4) NOT NULL,
  `dataformat` int(1) NOT NULL,
  `noisevalue` int(4) NOT NULL,
  `EW` char(1) NOT NULL,
  `gpsx` int(4) NOT NULL,
  `NS` char(1) NOT NULL,
  `gpsy` int(4) NOT NULL,
  `gpsz` int(4) NOT NULL,
  `onofflineflag` int(2) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Table structure for table `hcupm25bmpd300datainfo`
--

CREATE TABLE IF NOT EXISTS `hcupm25bmpd300datainfo` (
`sid` int(4) NOT NULL,
  `deviceid` int(4) NOT NULL,
  `timestamp` int(4) NOT NULL,
  `dataformat` int(1) NOT NULL,
  `pm2d5value` int(4) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Table structure for table `hcupm25datainfo`
--

CREATE TABLE IF NOT EXISTS `hcupm25datainfo` (
`sid` int(4) NOT NULL,
  `deviceid` int(4) NOT NULL,
  `timestamp` int(4) NOT NULL,
  `dataformat` int(1) NOT NULL,
  `pm1d0value` int(4) NOT NULL,
  `pm2d5value` int(4) NOT NULL,
  `pm10dvalue` int(4) NOT NULL,
  `EW` char(1) NOT NULL,
  `gpsx` int(4) NOT NULL,
  `NS` char(1) NOT NULL,
  `gpsy` int(4) NOT NULL,
  `gpsz` int(4) NOT NULL,
  `onofflineflag` int(2) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Table structure for table `hcupm25sharpdatainfo`
--

CREATE TABLE IF NOT EXISTS `hcupm25sharpdatainfo` (
`sid` int(4) NOT NULL,
  `deviceid` int(4) NOT NULL,
  `timestamp` int(4) NOT NULL,
  `dataformat` int(1) NOT NULL,
  `pm2d5value` int(4) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Table structure for table `hcuswdb`
--

CREATE TABLE IF NOT EXISTS `hcuswdb` (
  `dbver` int(4) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Dumping data for table `hcuswdb`
--

INSERT INTO `hcuswdb` (`dbver`) VALUES
(193);

-- --------------------------------------------------------

--
-- Table structure for table `hcusysalarminfo`
--

CREATE TABLE IF NOT EXISTS `hcusysalarminfo` (
`sid` int(4) NOT NULL,
  `eqpid` int(4) NOT NULL,
  `alarmtype` int(4) NOT NULL,
  `alarmcontent` int(4) NOT NULL,
  `alarmseverity` int(4) NOT NULL,
  `alarmclearflag` int(4) NOT NULL,
  `alarmdesc` char(100) NOT NULL,
  `timestamp` int(4) NOT NULL
) ENGINE=InnoDB AUTO_INCREMENT=71002 DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Table structure for table `hcusysengpar`
--

CREATE TABLE IF NOT EXISTS `hcusysengpar` (
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
  `svraddhttplocal` char(200) NOT NULL,
  `svraddsocketipdefault` char(200) NOT NULL,
  `svraddhttpdefault` char(200) NOT NULL,
  `svrnamedefault` char(12) NOT NULL,
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
  `browselworkingoption` char(128) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Dumping data for table `hcusysengpar`
--

INSERT INTO `hcusysengpar` (`prjname`, `commbackhawlcon`, `hcudbhost`, `hcudbuser`, `hcudbpsw`, `hcudbname`, `hcudbport`, `seriesportformodbus`, `seriesportforgps`, `seriesportforpm25sharp`, `svraddhttplocal`, `svraddsocketipdefault`, `svraddhttpdefault`, `svrnamedefault`, `cloudftpadd`, `cloudftpuser`, `cloudftppwd`, `cloudftppwdvideo`, `cloudftpuservideo`, `hcuswdownloaddir`, `hcuswactivedir`, `hcuswbackupdir`, `hcuvideoserverdir`, `hcuvideoserverhttp`, `debugmode`, `tracemode`, `browselautostartupflag`, `browselprog`, `browselstartupaddress`, `browselworkingoption`) VALUES
('HCU_PRJ_AQYCG10_335D', 1, 'localhost', 'root', '123456', 'hcudb', 3306, 0, 4, 2, 'http://127.0.0.1/test.php', 'http://aaa.bbb/', 'http://www.hkrob.com/mfunhcu/l1mainentry/cloud_callback_hcu.php', 'AQ_HCU', 'ftp://121.40.185.177/', 'forlinx', 'Forlinx321', 'avorion', 'Avorion321', '/var/hcu/hcu_sw_download/', '/var/hcu/hcu_sw_active/', '/var/hcu/hcu_sw_backup/', '/usr/local/apache_arm/htdocs/avorion/', 'http://192.168.1.232:8000/avorion/', 31, 4, 0, 'firefox', 'http://localhost/yii2basic/web/index.php', '-'),
('HCU_PRJ_AQYCG20_RASBERRY', 1, 'localhost', 'root', '123456', 'hcudb', 3306, 0, 4, 1, 'http://127.0.0.1/test.php', '121.40.185.177', 'http://www.hkrob.com/mfunhcu/l1mainentry/cloud_callback_wechat.php', 'AQYC_HCU', 'ftp://121.40.118.33/', 'forlinx', 'Forlinx321', 'avorion', 'Avorion321', '/var/hcu/hcu_sw_download/', '/var/hcu/hcu_sw_active/', '/var/hcu/hcu_sw_backup/', '121.40.118.33', 'http://192.168.1.232:8000/avorion/', 31, 4, 0, 'firefox', 'http://localhost/yii2basic/web/index.php', '-'),
('HCU_PRJ_AQYC_OBSOLETE', 1, 'localhost', 'root', '123456', 'hcudb', 3306, 0, 4, 2, 'http://127.0.0.1/test.php', 'http://aaa.bbb/', 'http://www.hkrob.com/mfunhcu/l1mainentry/cloud_callback_hcu.php', 'AQ_HCU', 'ftp://121.40.185.177/', 'forlinx', 'Forlinx321', 'avorion', 'Avorion321', '/var/hcu/hcu_sw_download/', '/var/hcu/hcu_sw_active/', '/var/hcu/hcu_sw_backup/', '/usr/local/apache_arm/htdocs/avorion/', 'http://192.168.1.232:8000/avorion/', 31, 4, 0, 'firefox', 'http://localhost/yii2basic/web/index.php', '-'),
('HCU_PRJ_BFSC_CBU', 1, 'localhost', 'root', '123456', 'hcudb', 3306, 0, 4, 2, 'http://127.0.0.1/test.php', '121.40.118.33', 'http://www.hkrob.com/mfunhcu/l1mainentry/cloud_callback_hcu.php', 'XHZN_HCU', 'ftp://121.40.185.177/', 'forlinx', 'Forlinx321', 'avorion', 'Avorion321', '/var/hcu/hcu_sw_download/', '/var/hcu/hcu_sw_active/', '/var/hcu/hcu_sw_backup/', '121.40.118.33', 'http://192.168.1.232:8000/avorion/', 28, 4, 0, 'firefox', 'http://localhost/yii2basic/web/index.php', '-'),
('HCU_PRJ_CXGLACM', 1, 'localhost', 'root', '123456', 'hcudb', 3306, 0, 4, 2, 'http://127.0.0.1/test.php', 'http://aaa.bbb/', 'http://www.hkrob.com/mfunhcu/l1mainentry/cloud_callback_hcu.php', 'AQ_HCU', 'ftp://121.40.185.177/', 'forlinx', 'Forlinx321', 'avorion', 'Avorion321', '/var/hcu/hcu_sw_download/', '/var/hcu/hcu_sw_active/', '/var/hcu/hcu_sw_backup/', '/usr/local/apache_arm/htdocs/avorion/', 'http://192.168.1.232:8000/avorion/', 31, 4, 0, 'firefox', 'http://localhost/yii2basic/web/index.php', '-'),
('HCU_PRJ_CXILC', 1, 'localhost', 'root', '123456', 'hcudb', 3306, 0, 4, 2, 'http://127.0.0.1/test.php', 'http://aaa.bbb/', 'http://www.hkrob.com/mfunhcu/l1mainentry/cloud_callback_hcu.php', 'AQ_HCU', 'ftp://121.40.185.177/', 'forlinx', 'Forlinx321', 'avorion', 'Avorion321', '/var/hcu/hcu_sw_download/', '/var/hcu/hcu_sw_active/', '/var/hcu/hcu_sw_backup/', '/usr/local/apache_arm/htdocs/avorion/', 'http://192.168.1.232:8000/avorion/', 31, 4, 0, 'firefox', 'http://localhost/yii2basic/web/index.php', '-'),
('HCU_PRJ_GQYBG40', 1, 'localhost', 'root', '123456', 'hcudb', 3306, 0, 4, 2, 'http://127.0.0.1/test.php', 'http://aaa.bbb/', 'http://www.hkrob.com/mfunhcu/l1mainentry/cloud_callback_hcu.php', 'AQ_HCU', 'ftp://121.40.185.177/', 'forlinx', 'Forlinx321', 'avorion', 'Avorion321', '/var/hcu/hcu_sw_download/', '/var/hcu/hcu_sw_active/', '/var/hcu/hcu_sw_backup/', '/usr/local/apache_arm/htdocs/avorion/', 'http://192.168.1.232:8000/avorion/', 31, 4, 0, 'firefox', 'http://localhost/yii2basic/web/index.php', '-'),
('HCU_PRJ_NBIOT_HPM_QG', 1, 'localhost', 'root', '123456', 'hcudb', 3306, 3, 1, 0, 'http://127.0.0.1/test.php', 'http://aaa.bbb/', 'http://121.40.185.177/xhzn/mfunhcu/l1mainentry/cloud_callback_hcu.php', 'AQ_HCU', 'ftp://121.40.185.177', 'forlinx', 'Forlinx321', 'avorion', 'Avorion321', '/var/hcu/hcu_sw_download/', '/var/hcu/hcu_sw_active/', '/var/hcu/hcu_sw_backup/', '/usr/local/apache_arm/htdocs/avorion/', 'http://192.168.1.232:8000/avorion/', 31, 4, 0, 'firefox', 'http://localhost/yii2basic/web/index.php', '-'),
('HCU_PRJ_NBIOT_LPM_CJ', 1, 'localhost', 'root', '123456', 'hcudb', 3306, 3, 1, 0, 'http://127.0.0.1/test.php', 'http://aaa.bbb/', 'http://121.40.185.177/xhzn/mfunhcu/l1mainentry/cloud_callback_hcu.php', 'AQ_HCU', 'ftp://121.40.185.177', 'forlinx', 'Forlinx321', 'avorion', 'Avorion321', '/var/hcu/hcu_sw_download/', '/var/hcu/hcu_sw_active/', '/var/hcu/hcu_sw_backup/', '/usr/local/apache_arm/htdocs/avorion/', 'http://192.168.1.232:8000/avorion/', 31, 4, 0, 'firefox', 'http://localhost/yii2basic/web/index.php', '-'),
('HCU_PRJ_OPWL_OTDR', 1, 'localhost', 'root', '123456', 'hcudb', 3306, 0, 4, 2, 'http://127.0.0.1/test.php', 'http://aaa.bbb/', 'http://www.hkrob.com/mfunhcu/l1mainentry/cloud_callback_hcu.php', 'AQ_HCU', 'ftp://121.40.185.177/', 'forlinx', 'Forlinx321', 'avorion', 'Avorion321', '/var/hcu/hcu_sw_download/', '/var/hcu/hcu_sw_active/', '/var/hcu/hcu_sw_backup/', '/usr/local/apache_arm/htdocs/avorion/', 'http://192.168.1.232:8000/avorion/', 31, 4, 0, 'firefox', 'http://localhost/yii2basic/web/index.php', '-'),
('HCU_PRJ_TBSWRG30', 1, 'localhost', 'root', '123456', 'hcudb', 3306, 3, 1, 0, 'http://127.0.0.1/test.php', 'http://aaa.bbb/', 'http://121.40.185.177/xhzn/mfunhcu/l1mainentry/cloud_callback_hcu.php', 'AQ_HCU', 'ftp://121.40.185.177', 'forlinx', 'Forlinx321', 'avorion', 'Avorion321', '/var/hcu/hcu_sw_download/', '/var/hcu/hcu_sw_active/', '/var/hcu/hcu_sw_backup/', '/usr/local/apache_arm/htdocs/avorion/', 'http://192.168.1.232:8000/avorion/', 31, 4, 0, 'firefox', 'http://localhost/yii2basic/web/index.php', '-'),
('HCU_PRJ_TEST_MODE', 1, 'localhost', 'root', '123456', 'hcudb', 3306, 3, 1, 0, 'http://127.0.0.1/test.php', 'http://aaa.bbb/', 'http://121.40.185.177/xhzn/mfunhcu/l1mainentry/cloud_callback_hcu.php', 'AQ_HCU', 'ftp://121.40.185.177', 'forlinx', 'Forlinx321', 'avorion', 'Avorion321', '/var/hcu/hcu_sw_download/', '/var/hcu/hcu_sw_active/', '/var/hcu/hcu_sw_backup/', '/usr/local/apache_arm/htdocs/avorion/', 'http://192.168.1.232:8000/avorion/', 31, 4, 0, 'firefox', 'http://localhost/yii2basic/web/index.php', '-');

-- --------------------------------------------------------

--
-- Table structure for table `hcusysengtimer`
--

CREATE TABLE IF NOT EXISTS `hcusysengtimer` (
  `timerid` int(2) NOT NULL,
  `timername` char(40) NOT NULL,
  `granularity` int(1) NOT NULL,
  `duration` int(1) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Dumping data for table `hcusysengtimer`
--

INSERT INTO `hcusysengtimer` (`timerid`, `timername`, `granularity`, `duration`) VALUES
(0, 'TID_MIN', 1, 0),
(1, 'TID_1S_MIN', 1, 0),
(2, 'TID_1S_SVRCON_INIT_FB', 1, 10),
(3, 'TID_1S_LED_GALOWAG_SCAN', 1, 1),
(4, 'TID_1S_EMC_PERIOD_READ', 1, 60),
(5, 'TID_1S_EMC_MODBUS_FB', 1, 10),
(6, 'TID_1S_PM25_PERIOD_READ', 1, 31),
(7, 'TID_1S_PM25_MODBUS_FB', 1, 10),
(8, 'TID_1S_WINDDIR_PERIOD_READ', 1, 13),
(9, 'TID_1S_WINDDIR_MODBUS_FB', 1, 10),
(10, 'TID_1S_WINDSPD_PERIOD_READ', 1, 15),
(11, 'TID_1S_WINDSPD_MODBUS_FB', 1, 10),
(12, 'TID_1S_TEMP_PERIOD_READ', 1, 17),
(13, 'TID_1S_TEMP_FB', 1, 10),
(14, 'TID_1S_HUMID_PERIOD_READ', 1, 19),
(15, 'TID_1S_HUMID_MODBUS_FB', 1, 10),
(16, 'TID_1S_NOISE_PERIOD_READ', 1, 7),
(17, 'TID_1S_NOISE_MODBUS_FB', 1, 5),
(18, 'TID_1S_NOISE_SPSVIRGO_FB', 1, 10),
(19, 'TID_1S_HSMMP_PERIOD_AVORION_READ', 1, 120),
(20, 'TID_1S_HSMMP_AVORION_FB', 1, 10),
(21, 'TID_1S_CLOUDVELA_PERIOD_LINK_HBT', 1, 10),
(22, 'TID_1S_NBIOTCJ188_PERIOD_LINK_HBT', 1, 600),
(23, 'TID_1S_NBIOTCJ188_SEND_DATA_BACK', 1, 10),
(24, 'TID_1S_NBIOTQG376_PERIOD_LINK_HBT', 1, 600),
(25, 'TID_1S_NBIOTQG376_SEND_DATA_BACK', 1, 10),
(26, 'TID_1S_LLCZHB_LINK_CTRL', 1, 10),
(27, 'TID_1S_AIRPRS_PERIOD_READ', 1, 600),
(28, 'TID_1S_CO1_PERIOD_READ', 1, 600),
(29, 'TID_1S_LIGHTSTR_PERIOD_READ', 1, 600),
(30, 'TID_1S_ALCOHOL_PERIOD_READ', 1, 600),
(31, 'TID_1S_HCHO_PERIOD_READ', 1, 600),
(32, 'TID_1S_TOXICGAS_PERIOD_READ', 1, 600),
(33, 'TID_1S_PM25SHARP_PERIOD_READ', 1, 600),
(34, 'TID_1S_IWM_PERIOD_READ', 1, 600),
(35, 'TID_1S_IHM_PERIOD_READ', 1, 600),
(36, 'TID_1S_IGM_PERIOD_READ', 1, 600),
(37, 'TID_1S_IPM_PERIOD_READ', 1, 600),
(38, 'TID_1S_SYSPM_PERIOD_WORKING', 1, 60),
(39, 'TID_1S_SYSSWM_PERIOD_WORKING', 1, 115),
(40, 'TID_1S_CANITFLEO_WORKING_SCAN', 1, 10),
(41, 'TID_1S_L3BFSC_SYS_CFG_WAIT_FB', 1, 15),
(42, 'TID_1S_L3BFSC_SYS_START_WAIT_FB', 1, 15),
(43, 'TID_1S_L3BFSC_SYS_STOP_WAIT_FB', 1, 15),
(44, 'TID_1S_L3BFSC_TTT_WAIT_FB', 1, 15),
(45, 'TID_1S_L3BFSC_TGU_WAIT_FB', 1, 15),
(46, 'TID_1S_L3BFSC_PERIOD_ERROR_SCAN', 1, 60),
(47, 'TID_1S_BFSCUICOMM_PERIOD_READ', 1, 5),
(48, 'TID_1S_L3AQYCG10_PERIOD_READ', 1, 600),
(49, 'TID_1S_L3AQYCG20_PERIOD_READ', 1, 600),
(57, 'TID_1S_L3TBSWRG30_PERIOD_READ', 1, 600),
(58, 'TID_1S_L3GQYBG40_PERIOD_READ', 1, 600),
(59, 'TID_1S_L3CXGLACM_PERIOD_READ', 1, 600),
(60, 'TID_1S_L3CXILC_PERIOD_READ', 1, 600),
(61, 'TID_1S_L3NBHPM_PERIOD_READ', 1, 600),
(62, 'TID_1S_L3NBLPM_PERIOD_READ', 1, 600),
(63, 'TID_1S_L3OPWLOTDR_PERIOD_READ', 1, 600),
(64, 'TID_1S_MAX', 1, 0),
(65, 'TID_10MS_MIN', 2, 0),
(66, 'TID_10MS_SVRCON_TEST', 2, 10000),
(67, 'TID_10MS_L3BFSC_PERIOD_STA_SCAN', 2, 50),
(68, 'TID_10MS_CANITFLEO_SIMULATION_DATA', 2, 20),
(69, 'TID_10MS_MAX', 2, 0),
(70, 'TID_1MS_MIN', 3, 0),
(71, 'TID_1MS_SVRCON_TEST', 3, 100),
(72, 'TID_1MS_MAX', 3, 0),
(73, 'TID_MAX', 1, 0);

-- --------------------------------------------------------

--
-- Table structure for table `hcusyspmglobaldatainfo`
--

CREATE TABLE IF NOT EXISTS `hcusyspmglobaldatainfo` (
`sid` int(4) NOT NULL,
  `taskhcuvmec` int(4) NOT NULL,
  `taskhwinvec` int(4) NOT NULL,
  `tasksyspmec` int(4) NOT NULL,
  `taskmodbusec` int(4) NOT NULL,
  `taskcloudvelaec` int(4) NOT NULL,
  `taskavorionec` int(4) NOT NULL,
  `taskspsvirgoec` int(4) NOT NULL,
  `taskhsmmpec` int(4) NOT NULL,
  `taskemcec` int(4) NOT NULL,
  `taskpm25ec` int(4) NOT NULL,
  `taskwinddirec` int(4) NOT NULL,
  `taskwindspdec` int(4) NOT NULL,
  `tasktempec` int(4) NOT NULL,
  `taskhumidec` int(4) NOT NULL,
  `tasknoiseec` int(4) NOT NULL,
  `taskairprsec` int(4) NOT NULL,
  `taskco1ec` int(4) NOT NULL,
  `tasklightstrec` int(4) NOT NULL,
  `taskalcoholec` int(4) NOT NULL,
  `taskhchoec` int(4) NOT NULL,
  `tasktoxicgasec` int(4) NOT NULL,
  `taskpm25sharp` int(4) NOT NULL,
  `restartcnt` int(4) NOT NULL,
  `cloudvelaconncnt` int(4) NOT NULL,
  `cloudvelaconnfailcnt` int(4) NOT NULL,
  `cloudveladisccnt` int(4) NOT NULL,
  `clouddatatimeoutcnt` int(4) NOT NULL,
  `socketdisccnt` int(4) NOT NULL,
  `cpuoccupy` int(4) NOT NULL,
  `memoccupy` int(4) NOT NULL,
  `diskoccupy` int(4) NOT NULL,
  `cputemp` int(4) NOT NULL,
  `timestamp` int(4) NOT NULL
) ENGINE=InnoDB AUTO_INCREMENT=1492 DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Table structure for table `hcutempbmp180datainfo`
--

CREATE TABLE IF NOT EXISTS `hcutempbmp180datainfo` (
`sid` int(4) NOT NULL,
  `deviceid` int(4) NOT NULL,
  `timestamp` int(4) NOT NULL,
  `dataformat` int(1) NOT NULL,
  `tempvalue` int(4) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Table structure for table `hcutempdatainfo`
--

CREATE TABLE IF NOT EXISTS `hcutempdatainfo` (
`sid` int(4) NOT NULL,
  `deviceid` int(4) NOT NULL,
  `timestamp` int(4) NOT NULL,
  `dataformat` int(1) NOT NULL,
  `tempvalue` int(4) NOT NULL,
  `EW` char(1) NOT NULL,
  `gpsx` int(4) NOT NULL,
  `NS` char(1) NOT NULL,
  `gpsy` int(4) NOT NULL,
  `gpsz` int(4) NOT NULL,
  `onofflineflag` int(2) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Table structure for table `hcutempdht11datainfo`
--

CREATE TABLE IF NOT EXISTS `hcutempdht11datainfo` (
`sid` int(4) NOT NULL,
  `deviceid` int(4) NOT NULL,
  `timestamp` int(4) NOT NULL,
  `dataformat` int(1) NOT NULL,
  `tempvalue` int(4) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Table structure for table `hcutempmth01datainfo`
--

CREATE TABLE IF NOT EXISTS `hcutempmth01datainfo` (
`sid` int(4) NOT NULL,
  `deviceid` int(4) NOT NULL,
  `timestamp` int(4) NOT NULL,
  `dataformat` int(1) NOT NULL,
  `tempvalue` int(4) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Table structure for table `hcutemprht03datainfo`
--

CREATE TABLE IF NOT EXISTS `hcutemprht03datainfo` (
`sid` int(4) NOT NULL,
  `deviceid` int(4) NOT NULL,
  `timestamp` int(4) NOT NULL,
  `dataformat` int(1) NOT NULL,
  `tempvalue` int(4) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Table structure for table `hcutempsht20datainfo`
--

CREATE TABLE IF NOT EXISTS `hcutempsht20datainfo` (
`sid` int(4) NOT NULL,
  `deviceid` int(4) NOT NULL,
  `timestamp` int(4) NOT NULL,
  `dataformat` int(1) NOT NULL,
  `tempvalue` int(4) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Table structure for table `hcutoxicgasdatainfo`
--

CREATE TABLE IF NOT EXISTS `hcutoxicgasdatainfo` (
`sid` int(4) NOT NULL,
  `deviceid` int(4) NOT NULL,
  `timestamp` int(4) NOT NULL,
  `dataformat` int(1) NOT NULL,
  `toxicgasvalue` int(4) NOT NULL,
  `ew` char(1) NOT NULL,
  `gpsx` int(4) NOT NULL,
  `ns` char(1) NOT NULL,
  `gpsy` int(4) NOT NULL,
  `gpsz` int(4) NOT NULL,
  `onofflineflag` int(2) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Table structure for table `hcutoxicgasmq135datainfo`
--

CREATE TABLE IF NOT EXISTS `hcutoxicgasmq135datainfo` (
`sid` int(4) NOT NULL,
  `deviceid` int(4) NOT NULL,
  `timestamp` int(4) NOT NULL,
  `dataformat` int(1) NOT NULL,
  `toxicgasvalue` int(4) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Table structure for table `hcutoxicgaszp01vocdatainfo`
--

CREATE TABLE IF NOT EXISTS `hcutoxicgaszp01vocdatainfo` (
`sid` int(4) NOT NULL,
  `deviceid` int(4) NOT NULL,
  `timestamp` int(4) NOT NULL,
  `dataformat` int(1) NOT NULL,
  `toxicgasvalue` int(4) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

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
  `modulefromrestrict` int(1) NOT NULL
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
(40, 'LLCZHB', 1, 1, 1, 1, 1),
(41, 'HSMMP', 1, 1, 1, 1, 1),
(42, 'EMC', 1, 1, 1, 1, 1),
(43, 'HUMID', 1, 1, 1, 1, 1),
(44, 'PM25', 1, 1, 1, 1, 1),
(45, 'TEMP', 1, 1, 1, 1, 1),
(46, 'WINDDIR', 1, 1, 1, 1, 1),
(47, 'WINDSPD', 1, 1, 1, 1, 1),
(48, 'NOISE', 1, 1, 1, 1, 1),
(49, 'AIRPRS', 1, 1, 1, 1, 1),
(50, 'CO1', 1, 1, 1, 1, 1),
(51, 'LIGHTSTR', 1, 1, 1, 1, 1),
(52, 'ALCOHOL', 1, 1, 1, 1, 1),
(53, 'HCHO', 1, 1, 1, 1, 1),
(54, 'TOXICGAS', 1, 1, 1, 1, 1),
(55, 'IWM', 1, 1, 1, 1, 1),
(56, 'IHM', 1, 1, 1, 1, 1),
(57, 'IGM', 1, 1, 1, 1, 1),
(58, 'IPM', 1, 1, 1, 1, 1),
(59, 'SVRCON', 1, 1, 1, 1, 1),
(60, 'SYSPM', 1, 1, 1, 1, 1),
(61, 'SYSSWM', 1, 1, 1, 1, 1),
(62, 'PM25SHARP', 1, 1, 1, 1, 1),
(63, 'CANITFLEO', 1, 1, 1, 1, 1),
(64, 'L3AQYCG10', 1, 1, 1, 1, 1),
(65, 'L3AQYCG20', 1, 1, 1, 1, 1),
(66, 'L3TBSWRG30', 1, 1, 1, 1, 1),
(67, 'L3GQYBG40', 1, 1, 1, 1, 1),
(68, 'L3CXILC', 1, 1, 1, 1, 1),
(69, 'L3CXGLACM', 1, 1, 1, 1, 1),
(70, 'L3NBLPM', 1, 1, 1, 1, 1),
(71, 'L3NBHPM', 1, 1, 1, 1, 1),
(72, 'L3BFSC', 1, 1, 1, 1, 1),
(73, 'BFSCUICOMM', 1, 1, 1, 1, 1),
(74, 'L3OPWLOTDR', 1, 1, 1, 1, 1),
(75, 'TASKMAX', 1, 1, 1, 1, 1);

-- --------------------------------------------------------

--
-- Table structure for table `hcutracemsgctr`
--

CREATE TABLE IF NOT EXISTS `hcutracemsgctr` (
  `msgid` int(2) NOT NULL,
  `msgname` char(70) NOT NULL,
  `msgctrflag` int(1) NOT NULL,
  `msgallow` int(1) NOT NULL,
  `msgrestrict` int(1) NOT NULL
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
(25, 'MSG_ID_AVORION_HSMMP_DATA_READ_FB', 1, 1, 1),
(26, 'MSG_ID_AVORION_HSMMP_DATA_RX', 1, 1, 1),
(27, 'MSG_ID_BLE_HSMMP_DATA_RX', 1, 1, 1),
(28, 'MSG_ID_BLE_MODBUS_DATA_RX', 1, 1, 1),
(29, 'MSG_ID_LCD_AVORION_DATA_RX', 1, 1, 1),
(30, 'MSG_ID_CAMERA_AVORION_DATA_RX', 1, 1, 1),
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
(112, 'MSG_ID_CLOUDVELA_PM25_CTRL_REQ', 1, 1, 1),
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
(148, 'MSG_ID_CLOUDVELA_NOISE_CTRL_REQ', 1, 1, 1),
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
(187, 'MSG_ID_CLOUDVELA_L3BFSC_DATA_REQ', 1, 1, 1),
(188, 'MSG_ID_L3BFSC_CLOUDVELA_DATA_RESP', 1, 1, 1),
(189, 'MSG_ID_L3BFSC_CLOUDVELA_DATA_REPORT', 1, 1, 1),
(190, 'MSG_ID_CLOUDVELA_L3BFSC_DATA_CONFIRM', 1, 1, 1),
(191, 'MSG_ID_L3BFSC_CLOUDVELA_EVENT_REPORT', 1, 1, 1),
(192, 'MSG_ID_CLOUDVELA_L3BFSC_EVENT_CONFIRM', 1, 1, 1),
(193, 'MSG_ID_CLOUDVELA_L3BFSC_CTRL_REQ', 1, 1, 1),
(194, 'MSG_ID_L3BFSC_CLOUDVELA_CMD_RESP', 1, 1, 1),
(195, 'MSG_ID_L3BFSC_CLOUDVELA_STATISTIC_REPORT', 1, 1, 1),
(196, 'MSG_ID_CLOUDVELA_L3BFSC_STATISTIC_CONFIRM', 1, 1, 1),
(197, 'MSG_ID_L3BFSC_CAN_SYS_CFG_REQ', 1, 1, 1),
(198, 'MSG_ID_CAN_L3BFSC_SYS_CFG_RESP', 1, 1, 1),
(199, 'MSG_ID_L3BFSC_CAN_SYS_START_REQ', 1, 1, 1),
(200, 'MSG_ID_CAN_L3BFSC_SYS_START_RESP', 1, 1, 1),
(201, 'MSG_ID_L3BFSC_CAN_SYS_STOP_REQ', 1, 1, 1),
(202, 'MSG_ID_CAN_L3BFSC_SYS_STOP_RESP', 1, 1, 1),
(203, 'MSG_ID_CAN_L3BFSC_WS_NEW_READY_EVENT', 1, 1, 1),
(204, 'MSG_ID_L3BFSC_CAN_WS_COMB_OUT', 1, 1, 1),
(205, 'MSG_ID_CAN_L3BFSC_WS_COMB_OUT_FB', 1, 1, 1),
(206, 'MSG_ID_L3BFSC_CAN_WS_GIVE_UP', 1, 1, 1),
(207, 'MSG_ID_CAN_L3BFSC_WS_GIVE_UP_FB', 1, 1, 1),
(208, 'MSG_ID_L3BFSC_CAN_ERROR_INQ_CMD_REQ', 1, 1, 1),
(209, 'MSG_ID_CAN_L3BFSC_ERROR_INQ_CMD_RESP', 1, 1, 1),
(210, 'MSG_ID_USBCAN_L2FRAME_RCV', 1, 1, 1),
(211, 'MSG_ID_UICOMM_L3BFSC_CMD_REQ', 1, 1, 1),
(212, 'MSG_ID_L3BFSC_UICOMM_CMD_RESP', 1, 1, 1),
(213, 'MSG_ID_UICOMM_L3BFSC_CFG_REQ', 1, 1, 1),
(214, 'MSG_ID_L3BFSC_UICOMM_CFG_RESP', 1, 1, 1),
(215, 'MSG_ID_UICOMM_CAN_TEST_CMD_REQ', 1, 1, 1),
(216, 'MSG_ID_CAN_UICOMM_TEST_CMD_RESP', 1, 1, 1),
(233, 'MSG_ID_L3AQYC_EXG_CTRL_REQ', 1, 1, 1),
(234, 'MSG_ID_L3AQYC_EXG_CTRL_RESP', 1, 1, 1),
(235, 'MSG_ID_L3AQYC_EXG_DATA_REPORT', 1, 1, 1),
(236, 'MSG_ID_CLOUDVELA_LLCZHB_FRAME_REQ', 1, 1, 1),
(237, 'MSG_ID_LLCZHB_CLOUDVELA_FRAME_RESP', 1, 1, 1),
(238, 'MSG_ID_LLCZHB_L3MOD_CTRL_REQ', 1, 1, 1),
(239, 'MSG_ID_L3MOD_LLCZHB_CTRL_RESP', 1, 1, 1),
(240, 'MSG_ID_L3MOD_LLCZHB_DATA_REPORT', 1, 1, 1),
(241, 'MSG_ID_ZHBL3MOD_EXG_CTRL_REQ', 1, 1, 1),
(242, 'MSG_ID_ZHBL3MOD_EXG_CTRL_RESP', 1, 1, 1),
(243, 'MSG_ID_ZHBL3MOD_EXG_DATA_REPORT', 1, 1, 1),
(244, 'MSG_ID_COM_MAX', 0, 0, 0);

-- --------------------------------------------------------

--
-- Table structure for table `hcuwinddirdatainfo`
--

CREATE TABLE IF NOT EXISTS `hcuwinddirdatainfo` (
`sid` int(4) NOT NULL,
  `deviceid` int(4) NOT NULL,
  `timestamp` int(4) NOT NULL,
  `dataformat` int(1) NOT NULL,
  `winddirvalue` int(4) NOT NULL,
  `EW` char(1) NOT NULL,
  `gpsx` int(4) NOT NULL,
  `NS` char(1) NOT NULL,
  `gpsy` int(4) NOT NULL,
  `gpsz` int(4) NOT NULL,
  `onofflineflag` int(2) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Table structure for table `hcuwindspddatainfo`
--

CREATE TABLE IF NOT EXISTS `hcuwindspddatainfo` (
`sid` int(4) NOT NULL,
  `deviceid` int(4) NOT NULL,
  `timestamp` int(4) NOT NULL,
  `dataformat` int(1) NOT NULL,
  `windspdvalue` int(4) NOT NULL,
  `EW` char(1) NOT NULL,
  `gpsx` int(4) NOT NULL,
  `NS` char(1) NOT NULL,
  `gpsy` int(4) NOT NULL,
  `gpsz` int(4) NOT NULL,
  `onofflineflag` int(2) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Indexes for dumped tables
--

--
-- Indexes for table `hcuairprsaltitudebmp180datainfo`
--
ALTER TABLE `hcuairprsaltitudebmp180datainfo`
 ADD PRIMARY KEY (`sid`);

--
-- Indexes for table `hcuairprsbmp180datainfo`
--
ALTER TABLE `hcuairprsbmp180datainfo`
 ADD PRIMARY KEY (`sid`);

--
-- Indexes for table `hcuairprsdatainfo`
--
ALTER TABLE `hcuairprsdatainfo`
 ADD PRIMARY KEY (`sid`);

--
-- Indexes for table `hcualcoholdatainfo`
--
ALTER TABLE `hcualcoholdatainfo`
 ADD PRIMARY KEY (`sid`);

--
-- Indexes for table `hcualcoholmq3alcodatainfo`
--
ALTER TABLE `hcualcoholmq3alcodatainfo`
 ADD PRIMARY KEY (`sid`);

--
-- Indexes for table `hcuco1datainfo`
--
ALTER TABLE `hcuco1datainfo`
 ADD PRIMARY KEY (`sid`);

--
-- Indexes for table `hcuemcdatainfo`
--
ALTER TABLE `hcuemcdatainfo`
 ADD PRIMARY KEY (`sid`);

--
-- Indexes for table `hcuhchodatainfo`
--
ALTER TABLE `hcuhchodatainfo`
 ADD PRIMARY KEY (`sid`);

--
-- Indexes for table `hcuhchoze08ch2odatainfo`
--
ALTER TABLE `hcuhchoze08ch2odatainfo`
 ADD PRIMARY KEY (`sid`);

--
-- Indexes for table `hcuhsmmpdatainfo`
--
ALTER TABLE `hcuhsmmpdatainfo`
 ADD PRIMARY KEY (`sid`);

--
-- Indexes for table `hcuhumiddatainfo`
--
ALTER TABLE `hcuhumiddatainfo`
 ADD PRIMARY KEY (`sid`);

--
-- Indexes for table `hcuhumiddht11datainfo`
--
ALTER TABLE `hcuhumiddht11datainfo`
 ADD PRIMARY KEY (`sid`);

--
-- Indexes for table `hcuhumidmth01datainfo`
--
ALTER TABLE `hcuhumidmth01datainfo`
 ADD PRIMARY KEY (`sid`);

--
-- Indexes for table `hcuhumidrht03datainfo`
--
ALTER TABLE `hcuhumidrht03datainfo`
 ADD PRIMARY KEY (`sid`);

--
-- Indexes for table `hcuhumidsht20datainfo`
--
ALTER TABLE `hcuhumidsht20datainfo`
 ADD PRIMARY KEY (`sid`);

--
-- Indexes for table `hcuigmcj188datainfo`
--
ALTER TABLE `hcuigmcj188datainfo`
 ADD PRIMARY KEY (`sid`);

--
-- Indexes for table `hcuihmcj188datainfo`
--
ALTER TABLE `hcuihmcj188datainfo`
 ADD PRIMARY KEY (`sid`);

--
-- Indexes for table `hcuipmcj188datainfo`
--
ALTER TABLE `hcuipmcj188datainfo`
 ADD PRIMARY KEY (`sid`);

--
-- Indexes for table `hcuiwmcj188datainfo`
--
ALTER TABLE `hcuiwmcj188datainfo`
 ADD PRIMARY KEY (`sid`);

--
-- Indexes for table `hculightstrbh1750datainfo`
--
ALTER TABLE `hculightstrbh1750datainfo`
 ADD PRIMARY KEY (`sid`);

--
-- Indexes for table `hculightstrdatainfo`
--
ALTER TABLE `hculightstrdatainfo`
 ADD PRIMARY KEY (`sid`);

--
-- Indexes for table `hcunoisedatainfo`
--
ALTER TABLE `hcunoisedatainfo`
 ADD PRIMARY KEY (`sid`);

--
-- Indexes for table `hcupm25bmpd300datainfo`
--
ALTER TABLE `hcupm25bmpd300datainfo`
 ADD PRIMARY KEY (`sid`);

--
-- Indexes for table `hcupm25datainfo`
--
ALTER TABLE `hcupm25datainfo`
 ADD PRIMARY KEY (`sid`);

--
-- Indexes for table `hcupm25sharpdatainfo`
--
ALTER TABLE `hcupm25sharpdatainfo`
 ADD PRIMARY KEY (`sid`);

--
-- Indexes for table `hcuswdb`
--
ALTER TABLE `hcuswdb`
 ADD PRIMARY KEY (`dbver`);

--
-- Indexes for table `hcusysalarminfo`
--
ALTER TABLE `hcusysalarminfo`
 ADD PRIMARY KEY (`sid`);

--
-- Indexes for table `hcusysengpar`
--
ALTER TABLE `hcusysengpar`
 ADD PRIMARY KEY (`prjname`);

--
-- Indexes for table `hcusysengtimer`
--
ALTER TABLE `hcusysengtimer`
 ADD PRIMARY KEY (`timerid`);

--
-- Indexes for table `hcusyspmglobaldatainfo`
--
ALTER TABLE `hcusyspmglobaldatainfo`
 ADD PRIMARY KEY (`sid`);

--
-- Indexes for table `hcutempbmp180datainfo`
--
ALTER TABLE `hcutempbmp180datainfo`
 ADD PRIMARY KEY (`sid`);

--
-- Indexes for table `hcutempdatainfo`
--
ALTER TABLE `hcutempdatainfo`
 ADD PRIMARY KEY (`sid`);

--
-- Indexes for table `hcutempdht11datainfo`
--
ALTER TABLE `hcutempdht11datainfo`
 ADD PRIMARY KEY (`sid`);

--
-- Indexes for table `hcutempmth01datainfo`
--
ALTER TABLE `hcutempmth01datainfo`
 ADD PRIMARY KEY (`sid`);

--
-- Indexes for table `hcutemprht03datainfo`
--
ALTER TABLE `hcutemprht03datainfo`
 ADD PRIMARY KEY (`sid`);

--
-- Indexes for table `hcutempsht20datainfo`
--
ALTER TABLE `hcutempsht20datainfo`
 ADD PRIMARY KEY (`sid`);

--
-- Indexes for table `hcutoxicgasdatainfo`
--
ALTER TABLE `hcutoxicgasdatainfo`
 ADD PRIMARY KEY (`sid`);

--
-- Indexes for table `hcutoxicgasmq135datainfo`
--
ALTER TABLE `hcutoxicgasmq135datainfo`
 ADD PRIMARY KEY (`sid`);

--
-- Indexes for table `hcutoxicgaszp01vocdatainfo`
--
ALTER TABLE `hcutoxicgaszp01vocdatainfo`
 ADD PRIMARY KEY (`sid`);

--
-- Indexes for table `hcutracemodulectr`
--
ALTER TABLE `hcutracemodulectr`
 ADD PRIMARY KEY (`moduleid`);

--
-- Indexes for table `hcutracemsgctr`
--
ALTER TABLE `hcutracemsgctr`
 ADD PRIMARY KEY (`msgid`);

--
-- Indexes for table `hcuwinddirdatainfo`
--
ALTER TABLE `hcuwinddirdatainfo`
 ADD PRIMARY KEY (`sid`);

--
-- Indexes for table `hcuwindspddatainfo`
--
ALTER TABLE `hcuwindspddatainfo`
 ADD PRIMARY KEY (`sid`);

--
-- AUTO_INCREMENT for dumped tables
--

--
-- AUTO_INCREMENT for table `hcuairprsaltitudebmp180datainfo`
--
ALTER TABLE `hcuairprsaltitudebmp180datainfo`
MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT;
--
-- AUTO_INCREMENT for table `hcuairprsbmp180datainfo`
--
ALTER TABLE `hcuairprsbmp180datainfo`
MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT;
--
-- AUTO_INCREMENT for table `hcuairprsdatainfo`
--
ALTER TABLE `hcuairprsdatainfo`
MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT;
--
-- AUTO_INCREMENT for table `hcualcoholdatainfo`
--
ALTER TABLE `hcualcoholdatainfo`
MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT;
--
-- AUTO_INCREMENT for table `hcualcoholmq3alcodatainfo`
--
ALTER TABLE `hcualcoholmq3alcodatainfo`
MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT;
--
-- AUTO_INCREMENT for table `hcuco1datainfo`
--
ALTER TABLE `hcuco1datainfo`
MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT;
--
-- AUTO_INCREMENT for table `hcuemcdatainfo`
--
ALTER TABLE `hcuemcdatainfo`
MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT,AUTO_INCREMENT=2726;
--
-- AUTO_INCREMENT for table `hcuhchodatainfo`
--
ALTER TABLE `hcuhchodatainfo`
MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT;
--
-- AUTO_INCREMENT for table `hcuhchoze08ch2odatainfo`
--
ALTER TABLE `hcuhchoze08ch2odatainfo`
MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT;
--
-- AUTO_INCREMENT for table `hcuhsmmpdatainfo`
--
ALTER TABLE `hcuhsmmpdatainfo`
MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT;
--
-- AUTO_INCREMENT for table `hcuhumiddatainfo`
--
ALTER TABLE `hcuhumiddatainfo`
MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT;
--
-- AUTO_INCREMENT for table `hcuhumiddht11datainfo`
--
ALTER TABLE `hcuhumiddht11datainfo`
MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT;
--
-- AUTO_INCREMENT for table `hcuhumidmth01datainfo`
--
ALTER TABLE `hcuhumidmth01datainfo`
MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT,AUTO_INCREMENT=2;
--
-- AUTO_INCREMENT for table `hcuhumidrht03datainfo`
--
ALTER TABLE `hcuhumidrht03datainfo`
MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT;
--
-- AUTO_INCREMENT for table `hcuhumidsht20datainfo`
--
ALTER TABLE `hcuhumidsht20datainfo`
MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT;
--
-- AUTO_INCREMENT for table `hcuigmcj188datainfo`
--
ALTER TABLE `hcuigmcj188datainfo`
MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT;
--
-- AUTO_INCREMENT for table `hcuihmcj188datainfo`
--
ALTER TABLE `hcuihmcj188datainfo`
MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT;
--
-- AUTO_INCREMENT for table `hcuipmcj188datainfo`
--
ALTER TABLE `hcuipmcj188datainfo`
MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT;
--
-- AUTO_INCREMENT for table `hcuiwmcj188datainfo`
--
ALTER TABLE `hcuiwmcj188datainfo`
MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT;
--
-- AUTO_INCREMENT for table `hculightstrbh1750datainfo`
--
ALTER TABLE `hculightstrbh1750datainfo`
MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT;
--
-- AUTO_INCREMENT for table `hculightstrdatainfo`
--
ALTER TABLE `hculightstrdatainfo`
MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT;
--
-- AUTO_INCREMENT for table `hcunoisedatainfo`
--
ALTER TABLE `hcunoisedatainfo`
MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT;
--
-- AUTO_INCREMENT for table `hcupm25bmpd300datainfo`
--
ALTER TABLE `hcupm25bmpd300datainfo`
MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT;
--
-- AUTO_INCREMENT for table `hcupm25datainfo`
--
ALTER TABLE `hcupm25datainfo`
MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT;
--
-- AUTO_INCREMENT for table `hcupm25sharpdatainfo`
--
ALTER TABLE `hcupm25sharpdatainfo`
MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT;
--
-- AUTO_INCREMENT for table `hcusysalarminfo`
--
ALTER TABLE `hcusysalarminfo`
MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT,AUTO_INCREMENT=71002;
--
-- AUTO_INCREMENT for table `hcusyspmglobaldatainfo`
--
ALTER TABLE `hcusyspmglobaldatainfo`
MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT,AUTO_INCREMENT=1492;
--
-- AUTO_INCREMENT for table `hcutempbmp180datainfo`
--
ALTER TABLE `hcutempbmp180datainfo`
MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT;
--
-- AUTO_INCREMENT for table `hcutempdatainfo`
--
ALTER TABLE `hcutempdatainfo`
MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT;
--
-- AUTO_INCREMENT for table `hcutempdht11datainfo`
--
ALTER TABLE `hcutempdht11datainfo`
MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT;
--
-- AUTO_INCREMENT for table `hcutempmth01datainfo`
--
ALTER TABLE `hcutempmth01datainfo`
MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT;
--
-- AUTO_INCREMENT for table `hcutemprht03datainfo`
--
ALTER TABLE `hcutemprht03datainfo`
MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT;
--
-- AUTO_INCREMENT for table `hcutempsht20datainfo`
--
ALTER TABLE `hcutempsht20datainfo`
MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT;
--
-- AUTO_INCREMENT for table `hcutoxicgasdatainfo`
--
ALTER TABLE `hcutoxicgasdatainfo`
MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT;
--
-- AUTO_INCREMENT for table `hcutoxicgasmq135datainfo`
--
ALTER TABLE `hcutoxicgasmq135datainfo`
MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT;
--
-- AUTO_INCREMENT for table `hcutoxicgaszp01vocdatainfo`
--
ALTER TABLE `hcutoxicgaszp01vocdatainfo`
MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT;
--
-- AUTO_INCREMENT for table `hcuwinddirdatainfo`
--
ALTER TABLE `hcuwinddirdatainfo`
MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT;
--
-- AUTO_INCREMENT for table `hcuwindspddatainfo`
--
ALTER TABLE `hcuwindspddatainfo`
MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
