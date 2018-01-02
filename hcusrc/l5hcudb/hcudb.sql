-- phpMyAdmin SQL Dump
-- version 4.6.6deb4
-- https://www.phpmyadmin.net/
--
-- Host: localhost:3306
-- Generation Time: 2018-01-02 09:39:09
-- 服务器版本： 5.7.19-0ubuntu0.17.04.1
-- PHP Version: 7.0.22-0ubuntu0.17.04.1

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;

--
-- Database: `hcudb`
--

-- --------------------------------------------------------

--
-- 表的结构 `hcuairprsaltitudebmp180datainfo`
--

CREATE TABLE `hcuairprsaltitudebmp180datainfo` (
  `sid` int(4) NOT NULL,
  `deviceid` int(4) NOT NULL,
  `timestamp` int(4) NOT NULL,
  `dataformat` int(1) NOT NULL,
  `altitudevalue` int(4) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- 表的结构 `hcuairprsbmp180datainfo`
--

CREATE TABLE `hcuairprsbmp180datainfo` (
  `sid` int(4) NOT NULL,
  `deviceid` int(4) NOT NULL,
  `timestamp` int(4) NOT NULL,
  `dataformat` int(1) NOT NULL,
  `airprsvalue` int(4) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- 表的结构 `hcuairprsdatainfo`
--

CREATE TABLE `hcuairprsdatainfo` (
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
-- 表的结构 `hcualcoholdatainfo`
--

CREATE TABLE `hcualcoholdatainfo` (
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
-- 表的结构 `hcualcoholmq3alcodatainfo`
--

CREATE TABLE `hcualcoholmq3alcodatainfo` (
  `sid` int(4) NOT NULL,
  `deviceid` int(4) NOT NULL,
  `timestamp` int(4) NOT NULL,
  `dataformat` int(1) NOT NULL,
  `alcoholvalue` int(4) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- 表的结构 `hcubfhsproductpara`
--

CREATE TABLE `hcubfhsproductpara` (
  `sid` int(4) NOT NULL,
  `confname` varchar(50) CHARACTER SET utf8 NOT NULL,
  `currentconf` char(1) CHARACTER SET utf8 NOT NULL DEFAULT 'N',
  `baseconf` char(1) CHARACTER SET utf8 NOT NULL DEFAULT 'N',
  `confowner` varchar(20) CHARACTER SET utf8 NOT NULL DEFAULT 'System',
  `conficon` varchar(20) CHARACTER SET utf8 NOT NULL,
  `confdescription` varchar(100) CHARACTER SET utf8 DEFAULT NULL,
  `targetweight` int(5) NOT NULL,
  `tareweight` int(5) NOT NULL,
  `targetspeed` int(4) NOT NULL,
  `productlength` int(4) NOT NULL,
  `armrollinginterval` int(4) NOT NULL,
  `algorithmoption` int(4) NOT NULL,
  `upperlimit` int(4) NOT NULL,
  `tu1limit` int(4) NOT NULL,
  `tu2limit` int(4) NOT NULL,
  `maxtu1` float NOT NULL,
  `rejectoroption` int(4) NOT NULL,
  `statisticsswitch` int(4) NOT NULL DEFAULT '2',
  `targetthroughput` int(6) NOT NULL DEFAULT '10000',
  `snrautozerorange` int(4) NOT NULL,
  `snrstandstillrange` int(4) NOT NULL,
  `snrfiltercutofffreq` int(4) NOT NULL,
  `snrringbuftime` int(2) NOT NULL,
  `snrautotaringtime` int(2) NOT NULL,
  `snrpreloadvalue` int(2) NOT NULL,
  `snrpreloadvalueformat` int(2) NOT NULL,
  `snrstandstilltimeout` int(2) NOT NULL,
  `snrstandstilltime` int(2) NOT NULL,
  `snrmeasurementrange` int(1) NOT NULL,
  `snrtimegrid` int(1) NOT NULL,
  `motorspeed` int(4) NOT NULL,
  `motordirection` int(4) NOT NULL,
  `armrollingstart` int(4) NOT NULL,
  `armrollingstop` int(4) NOT NULL,
  `armfailvaration` int(4) NOT NULL,
  `armfailtime` int(4) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- 转存表中的数据 `hcubfhsproductpara`
--

INSERT INTO `hcubfhsproductpara` (`sid`, `confname`, `currentconf`, `baseconf`, `confowner`, `conficon`, `confdescription`, `targetweight`, `tareweight`, `targetspeed`, `productlength`, `armrollinginterval`, `algorithmoption`, `upperlimit`, `tu1limit`, `tu2limit`, `maxtu1`, `rejectoroption`, `statisticsswitch`, `targetthroughput`, `snrautozerorange`, `snrstandstillrange`, `snrfiltercutofffreq`, `snrringbuftime`, `snrautotaringtime`, `snrpreloadvalue`, `snrpreloadvalueformat`, `snrstandstilltimeout`, `snrstandstilltime`, `snrmeasurementrange`, `snrtimegrid`, `motorspeed`, `motordirection`, `armrollingstart`, `armrollingstop`, `armfailvaration`, `armfailtime`) VALUES
(13, 'fish888', 'N', 'N', 'System', 'fish37.svg', '', 1000, 995, 990, 0, 2, 2, 800, 5, 25, 350, 10, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0),
(14, 'apple', 'Y', 'N', 'System', 'apple54.svg', '', 990, 995, 990, 0, 2, 2, 999, 5, 25, 350, 10, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0),
(15, 'qyyy', 'N', 'N', 'System', 'apple54.svg', '', 990, 995, 990, 0, 2, 2, 999, 5, 25, 350, 10, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0),
(16, 'apple', 'N', 'N', 'System', 'apple54.svg', '', 990, 995, 990, 0, 2, 2, 999, 5, 25, 350, 10, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0),
(17, 'apple', 'N', 'N', 'System', 'apple54.svg', '', 990, 995, 990, 0, 2, 2, 999, 5, 25, 350, 10, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0),
(18, 'apple', 'N', 'N', 'System', 'apple54.svg', '', 990, 995, 990, 0, 2, 2, 999, 5, 25, 350, 10, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0),
(19, 'fish888', 'N', 'N', 'System', 'fish37.svg', '', 1000, 995, 990, 0, 2, 2, 800, 5, 25, 350, 10, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

-- --------------------------------------------------------

--
-- 表的结构 `hcubfhssystempara`
--

CREATE TABLE `hcubfhssystempara` (
  `weighterlength` int(4) NOT NULL DEFAULT '350',
  `rejectorposition` int(4) NOT NULL,
  `maxallowedweight` int(4) NOT NULL,
  `minallowedweight` int(4) NOT NULL,
  `counterweight` int(4) NOT NULL,
  `autozerosignal` int(4) NOT NULL DEFAULT '500',
  `autozeroswitch` int(1) NOT NULL,
  `autozerolimit` int(1) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- 转存表中的数据 `hcubfhssystempara`
--

INSERT INTO `hcubfhssystempara` (`weighterlength`, `rejectorposition`, `maxallowedweight`, `minallowedweight`, `counterweight`, `autozerosignal`, `autozeroswitch`, `autozerolimit`) VALUES
(400, 500, 500, 5, 1000, 6, 100000, 3);

-- --------------------------------------------------------

--
-- 表的结构 `hcubfsccalibration`
--

CREATE TABLE `hcubfsccalibration` (
  `sid` int(4) NOT NULL,
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
-- 转存表中的数据 `hcubfsccalibration`
--

INSERT INTO `hcubfsccalibration` (`sid`, `zeroadc_01`, `fulladc_01`, `fullwgt_01`, `zeroadc_02`, `fulladc_02`, `fullwgt_02`, `zeroadc_03`, `fulladc_03`, `fullwgt_03`, `zeroadc_04`, `fulladc_04`, `fullwgt_04`, `zeroadc_05`, `fulladc_05`, `fullwgt_05`, `zeroadc_06`, `fulladc_06`, `fullwgt_06`, `zeroadc_07`, `fulladc_07`, `fullwgt_07`, `zeroadc_08`, `fulladc_08`, `fullwgt_08`, `zeroadc_09`, `fulladc_09`, `fullwgt_09`, `zeroadc_10`, `fulladc_10`, `fullwgt_10`, `zeroadc_11`, `fulladc_11`, `fullwgt_11`, `zeroadc_12`, `fulladc_12`, `fullwgt_12`, `zeroadc_13`, `fulladc_13`, `fullwgt_13`, `zeroadc_14`, `fulladc_14`, `fullwgt_14`, `zeroadc_15`, `fulladc_15`, `fullwgt_15`, `zeroadc_16`, `fulladc_16`, `fullwgt_16`) VALUES
(1, 402121, 475568, 100000, 397526, 469447, 100000, 398223, 471012, 100000, 412981, 487157, 100000, 402460, 475539, 100000, 400936, 473725, 100000, 404690, 477764, 100000, 397114, 469254, 100000, 398982, 471891, 100000, 409975, 482795, 100000, 448185, 519955, 100000, 433829, 506237, 100000, 433829, 506237, 100000, 433829, 506237, 100000, 433829, 506237, 100000, 433829, 506237, 100000);

-- --------------------------------------------------------

--
-- 表的结构 `hcubfscconfigpara`
--

CREATE TABLE `hcubfscconfigpara` (
  `sid` int(4) NOT NULL,
  `confname` varchar(50) NOT NULL,
  `currentconf` char(1) NOT NULL DEFAULT 'N',
  `baseconf` char(1) NOT NULL DEFAULT 'N',
  `confowner` varchar(20) NOT NULL DEFAULT 'System',
  `conficon` varchar(20) NOT NULL,
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
-- 转存表中的数据 `hcubfscconfigpara`
--

INSERT INTO `hcubfscconfigpara` (`sid`, `confname`, `currentconf`, `baseconf`, `confowner`, `conficon`, `confdescription`, `minscalenum`, `maxscalenum`, `minscalenumstart`, `targetweight`, `upperweightlimit`, `proximitmode`, `combinationbias`, `remaindetectind`, `remaindetecttime`, `remainobjtreat`, `prioritymode`, `automode`, `averagenum`, `loaddetecttime`, `loaddetectthread`, `emptydetecttime`, `emptydetectthread`, `standardreadytime`, `motorspeed`, `motordirection`, `rollingstart`, `rollingstop`, `rollinginterval`, `failuredetectvaration`, `failuredetecttime`) VALUES
(4, 'candy 1', 'Y', 'N', 'Bofeng', 'candy.svg', 'candy 200g', 1, 16, 1, 20000, 5000, 0, 0, 0, 0, 0, 0, 0, 0, 400, 100, 400, 500, 0, 400, 0, 1000, 0, 0, 0, 0),
(7, 'apple 1', 'N', 'N', 'Bofeng', 'apple12.svg', 'Apple 1Kg', 2, 16, 1, 100000, 5000, 0, 1, 0, 2, 0, 0, 0, 1, 400, 100, 400, 500, 0, 150, 0, 5000, 0, 0, 0, 0),
(8, '-apple', 'N', 'Y', 'System', 'apple54.svg', 'Apple standard config (1Kg) ', 1, 16, 1, 100000, 5000, 0, 1, 0, 2, 0, 0, 0, 1, 400, 100, 400, 500, 0, 150, 0, 5000, 0, 0, 0, 0),
(9, '-tomato', 'N', 'Y', 'System', 'tomato1.svg', 'Tomato standard config 1Kg', 1, 16, 1, 100000, 5000, 0, 1, 0, 2, 0, 0, 0, 1, 400, 100, 400, 500, 0, 150, 0, 5000, 0, 0, 0, 0),
(10, '-candy', 'N', 'Y', 'System', 'candy.svg', 'Candy standard config 500g', 1, 16, 1, 50000, 5000, 0, 1, 0, 2, 0, 0, 0, 1, 400, 100, 400, 500, 0, 150, 0, 5000, 0, 0, 0, 0),
(11, '-fish', 'N', 'Y', 'System', 'fish12.svg', 'fish 500g', 1, 16, 1, 50000, 5000, 0, 1, 0, 2, 0, 0, 0, 1, 400, 100, 400, 500, 0, 150, 0, 5000, 0, 0, 0, 0),
(12, 'fish 1', 'N', 'N', 'System', 'fish12.svg', 'fish 500g', 1, 16, 1, 50000, 5000, 0, 1, 0, 2, 0, 0, 0, 1, 400, 100, 400, 500, 0, 150, 0, 5000, 0, 0, 0, 0);

-- --------------------------------------------------------

--
-- 表的结构 `hcubfsccurrentinfo`
--

CREATE TABLE `hcubfsccurrentinfo` (
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
  `curcomwgt` int(4) DEFAULT NULL,
  `hcusw` char(100) DEFAULT NULL,
  `ihusw` char(100) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- 转存表中的数据 `hcubfsccurrentinfo`
--

INSERT INTO `hcubfsccurrentinfo` (`timestamp`, `status_00`, `value_00`, `status_01`, `value_01`, `status_02`, `value_02`, `status_03`, `value_03`, `status_04`, `value_04`, `status_05`, `value_05`, `status_06`, `value_06`, `status_07`, `value_07`, `status_08`, `value_08`, `status_09`, `value_09`, `status_10`, `value_10`, `status_11`, `value_11`, `status_12`, `value_12`, `status_13`, `value_13`, `status_14`, `value_14`, `status_15`, `value_15`, `status_16`, `value_16`, `curcomwgt`, `hcusw`, `ihusw`) VALUES
(1508486169, 6, 0, 6, 0, 6, 0, 6, 0, 6, 0, 6, 0, 6, 0, 6, 0, 6, 0, 6, 0, 6, 0, 0, 111, 0, 121, 0, 131, 0, 141, 0, 151, 0, 161, 0, 'NOW: Inventory confirm HCU-SW-R3.V265.DB200.PATCH!', 'NOW: Inventory confirm IHU-SW-R3.V214.DB0.PATCH!');

-- --------------------------------------------------------

--
-- 表的结构 `hcubfscfb2ui`
--

CREATE TABLE `hcubfscfb2ui` (
  `sid` int(4) NOT NULL,
  `cmdtype` int(1) NOT NULL,
  `validflag` int(1) NOT NULL,
  `fbinfo` char(80) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- 转存表中的数据 `hcubfscfb2ui`
--

INSERT INTO `hcubfscfb2ui` (`sid`, `cmdtype`, `validflag`, `fbinfo`) VALUES
(1, 1, 1, 'Configuration success'),
(2, 2, 0, 'Start failure due to WS not reachable'),
(3, 3, 0, 'Stop failure due to WS not reachable');

-- --------------------------------------------------------

--
-- 表的结构 `hcubfscflowsheet`
--

CREATE TABLE `hcubfscflowsheet` (
  `sid` int(4) NOT NULL,
  `configid` int(2) NOT NULL,
  `targetwgt` int(4) NOT NULL,
  `realwgt` int(4) NOT NULL,
  `pkgnum` int(4) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- 转存表中的数据 `hcubfscflowsheet`
--

INSERT INTO `hcubfscflowsheet` (`sid`, `configid`, `targetwgt`, `realwgt`, `pkgnum`) VALUES
(1, 4, 20000, 23215, 1),
(2, 4, 20000, 23302, 1),
(3, 4, 20000, 22658, 1),
(4, 4, 20000, 22596, 1),
(5, 4, 20000, 24155, 4);

-- --------------------------------------------------------

--
-- 表的结构 `hcubfsclanguagedict`
--

CREATE TABLE `hcubfsclanguagedict` (
  `sid` int(4) NOT NULL,
  `english` varchar(200) NOT NULL,
  `chinese` varchar(200) DEFAULT NULL,
  `french` varchar(200) DEFAULT NULL,
  `japanese` varchar(200) DEFAULT NULL,
  `german` varchar(200) DEFAULT NULL,
  `arabic` varchar(200) DEFAULT NULL,
  `thai` varchar(200) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- 转存表中的数据 `hcubfsclanguagedict`
--

INSERT INTO `hcubfsclanguagedict` (`sid`, `english`, `chinese`, `french`, `japanese`, `german`, `arabic`, `thai`) VALUES
(1, 'MaxAllowedWeight', '重量传感器校准参数', NULL, NULL, NULL, NULL, NULL),
(2, 'Command ID', '命令字', NULL, NULL, NULL, NULL, NULL),
(3, 'Max allowed weight upper limit (0.01 grams)', '最大允许称重上限值 (0.01g)', NULL, NULL, NULL, NULL, NULL),
(4, 'Please select test command', '请选择要测试的命令字', NULL, NULL, NULL, NULL, NULL),
(5, 'CalFullWeight', '校准满载重量', NULL, NULL, NULL, NULL, NULL),
(6, 'Sensor ID', '秤台编号', NULL, NULL, NULL, NULL, NULL),
(7, 'Weight sensor full weight when calibration(0.01 grams)', '校准时称重传感器满载值 (0.01g)', NULL, NULL, NULL, NULL, NULL),
(8, 'Please select the sensor you want to test', '请选择你想要测试的称台编号', NULL, NULL, NULL, NULL, NULL),
(9, 'CalibrationPara', '重量传感器校准参数', NULL, NULL, NULL, NULL, NULL),
(10, 'General Parameter', '通用参数', NULL, NULL, NULL, NULL, NULL),
(11, 'StaticZeroValue', '静态偏置重量', NULL, NULL, NULL, NULL, NULL),
(12, 'Motor speed parameter for motor test', '马达速度参数，马达测试命令时有效', NULL, NULL, NULL, NULL, NULL),
(13, 'Weight sensor static bias zero value (0.01 grams)', '称台的静态时的重量偏置值 (0.01g)', NULL, NULL, NULL, NULL, NULL),
(14, 'Test Command Setting', '测试命令设置', NULL, NULL, NULL, NULL, NULL),
(15, 'TailorValue', '皮重量', NULL, NULL, NULL, NULL, NULL),
(16, 'Weight sensor tailor value (0.01 grams)', '称台和被称物品的皮重量 (0.01g)', NULL, NULL, NULL, NULL, NULL),
(17, 'DynZeroThresholdValue', '动态归零阈值', NULL, NULL, NULL, NULL, NULL),
(18, 'Weight sensor threshold value to active dynamic zeroing (0.01 grams)', '激活动态归零功能的重量门限 (0.01g)', NULL, NULL, NULL, NULL, NULL),
(19, 'DynZeroHysteresis', '动态归零的迟滞', NULL, NULL, NULL, NULL, NULL),
(20, 'Weight sensor hysteresis time to active dynamic zeroing (ms)', '激活动态归零功能的保持时间 (毫秒)', NULL, NULL, NULL, NULL, NULL),
(21, 'WeightSensorPara', '重量传感器参数', NULL, NULL, NULL, NULL, NULL),
(22, 'MinScaleNum', '最小称台数', NULL, NULL, NULL, NULL, NULL),
(23, 'Min scale number for combination allowed', '每个组合允许的最小称台个数', NULL, NULL, NULL, NULL, NULL),
(24, 'MaxScaleNum', '最大称台数', NULL, NULL, NULL, NULL, NULL),
(25, 'Max scale number for combination allowed', '每个组合允许的最大称台个数', NULL, NULL, NULL, NULL, NULL),
(26, 'MinScaleNumStart', '搜索开始称台数', NULL, NULL, NULL, NULL, NULL),
(27, 'Min scale number for starting combination search', '开始组合搜索的最小称台个数', NULL, NULL, NULL, NULL, NULL),
(28, 'TargetWeight', '目标重量值', NULL, NULL, NULL, NULL, NULL),
(29, 'Target weight per package (0.01 grams)', '组合每包目标重量值 (0.01g)', NULL, NULL, NULL, NULL, NULL),
(30, 'UpperWeightLimit', '目标重量偏差上限', NULL, NULL, NULL, NULL, NULL),
(31, 'Upper delta weight of target (0.01 grams)', '组合重量允许偏离目标的上限量 (0.01g)', NULL, NULL, NULL, NULL, NULL),
(32, 'ProximityMode', '组合的模糊估计模式', NULL, NULL, NULL, NULL, NULL),
(33, 'Proximity mode of combination', '如果在重量范围之内找不到组合是否驱动模糊模式', NULL, NULL, NULL, NULL, NULL),
(34, 'RemainDetectTime', '未被组合检测时间', NULL, NULL, NULL, NULL, NULL),
(35, 'Detection timer of not being combined (ms)', '判断秤台超时未被组合的检测时间 (毫秒)', NULL, NULL, NULL, NULL, NULL),
(36, 'CombinationAlgoMode', '组合算法模式', NULL, NULL, NULL, NULL, NULL),
(37, 'Priority mode for combination algorithm', '组合算法优选搜索模式', NULL, NULL, NULL, NULL, NULL),
(38, 'AlgorithmPara', '组合算法参数', NULL, NULL, NULL, NULL, NULL),
(39, 'LoadDetectTime', '秤台稳定检测时间', NULL, NULL, NULL, NULL, NULL),
(40, 'Load stable detection time (ms)', '判断秤台重量稳定的检测时间 (毫秒)', NULL, NULL, NULL, NULL, NULL),
(41, 'LoadDetectThreshold', '秤台允许抖动阈值', NULL, NULL, NULL, NULL, NULL),
(42, 'Fluctuation limit of load stable detection (0.01 grams)', '判断秤台稳定的重量允许抖动阈值 (0.01g)', NULL, NULL, NULL, NULL, NULL),
(43, 'EmptyDetectTime', '空秤检测时间', NULL, NULL, NULL, NULL, NULL),
(44, 'Load empty detection time (ms)', '判断称台为空的检测时间 (毫秒)', NULL, NULL, NULL, NULL, NULL),
(45, 'EmptyDetectThreshold', '空秤允许抖动阈值', NULL, NULL, NULL, NULL, NULL),
(46, 'Fluctuation limit of load empty detection (0.01 grams)', '判断称台为空的重量阈值 (0.01g)', NULL, NULL, NULL, NULL, NULL),
(47, 'MotorSpeed', '马达速度', NULL, NULL, NULL, NULL, NULL),
(48, 'Motor speed (rpm)', '电机的转速 (每分钟转数)', NULL, NULL, NULL, NULL, NULL),
(49, 'MotorDirection', '马达方向', NULL, NULL, NULL, NULL, NULL),
(50, 'Motor direction: Clockwise or Counter-Clockwise', '电机的转动方向 (顺时针或者逆时针)', NULL, NULL, NULL, NULL, NULL),
(51, 'RollingLastTime', '马达启动延迟时间', NULL, NULL, NULL, NULL, NULL),
(52, 'The last time of motor rolling (ms)', '每次电机转动的最大保护时间 (毫秒)', NULL, NULL, NULL, NULL, NULL),
(53, 'MotorControlPara', '马达参数', NULL, NULL, NULL, NULL, NULL),
(54, 'OFFLINE', '离线', NULL, NULL, NULL, NULL, NULL),
(55, 'ProductName', '产品名称', NULL, NULL, NULL, NULL, NULL),
(56, 'Please select product name for data export', '请选择要数据导出的产品', NULL, NULL, NULL, NULL, NULL),
(57, 'StatDataType', '统计数据类型', NULL, NULL, NULL, NULL, NULL),
(58, 'Please select statistic data type you want to export', '请选择要导出的统计数据类型', NULL, NULL, NULL, NULL, NULL),
(59, 'Export Data Setting', '数据导出设置', NULL, NULL, NULL, NULL, NULL),
(60, 'Data export success', '数据导出成功', NULL, NULL, NULL, NULL, NULL),
(61, 'ONLINE', '在线', NULL, NULL, NULL, NULL, NULL),
(62, 'Data export failure', '数据导出失败', NULL, NULL, NULL, NULL, NULL),
(63, 'CONFIG OK', '配置成功', NULL, NULL, NULL, NULL, NULL),
(64, 'CONFIG NOK', '配置失败', NULL, NULL, NULL, NULL, NULL),
(65, 'START OK', '启动成功', NULL, NULL, NULL, NULL, NULL),
(66, 'STOP', '停止', NULL, NULL, NULL, NULL, NULL),
(67, 'FAULT', '设备异常', NULL, NULL, NULL, NULL, NULL),
(68, 'SW UPGRADE', '软件更新', NULL, NULL, NULL, NULL, NULL),
(69, 'INVALID', '无效', NULL, NULL, NULL, NULL, NULL),
(70, 'WARNING:System Suspend;Please click resume;', '告警:系统暂停;请点击继续;', NULL, NULL, NULL, NULL, NULL),
(71, 'SUSPEND', '暂停', NULL, NULL, NULL, NULL, NULL),
(72, 'SW Version Info', '软件版本信息', NULL, NULL, NULL, NULL, NULL),
(73, 'New Version available, please reboot whole system', '有新版本，请重启系统', NULL, NULL, NULL, NULL, NULL),
(74, 'SW Version Info', '软件版本信息', NULL, NULL, NULL, NULL, NULL),
(75, 'New Version available, please reboot whole system', '有新版本，请重启系统', NULL, NULL, NULL, NULL, NULL);

-- --------------------------------------------------------

--
-- 表的结构 `hcubfsclanguagelist`
--

CREATE TABLE `hcubfsclanguagelist` (
  `lang_name` varchar(20) NOT NULL,
  `lang_icon` varchar(20) NOT NULL,
  `lang_abbr` varchar(20) NOT NULL,
  `defaultflag` tinyint(1) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- 转存表中的数据 `hcubfsclanguagelist`
--

INSERT INTO `hcubfsclanguagelist` (`lang_name`, `lang_icon`, `lang_abbr`, `defaultflag`) VALUES
('ARABIC', 'Arabia.svg', 'ar', 0),
('CHINESE', 'China.svg', 'ch', 1),
('ENGLISH', 'UK.svg', 'en', 0),
('FRENCH', 'France.svg', 'fr', 0),
('GERMAN', 'Germany.svg', 'de', 0),
('JAPANESE', 'Japan.svg', 'jp', 0),
('THAI', 'Thailand.svg', 'th', 0);

-- --------------------------------------------------------

--
-- 表的结构 `hcubfscstadatainfo`
--

CREATE TABLE `hcubfscstadatainfo` (
  `sid` int(4) NOT NULL,
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
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- 转存表中的数据 `hcubfscstadatainfo`
--

INSERT INTO `hcubfscstadatainfo` (`sid`, `statype`, `configid`, `timestamp`, `wsincmatcnt`, `wsincmatwgt`, `wscombtimes`, `wsttttimes`, `wstgvtimes`, `wstttmatcnt`, `wstgvmatcnt`, `wstttmatwgt`, `wstgvmatwgt`, `wsavgttttimes`, `wsavgtttmatcnt`, `wsavgtttmatwgt`) VALUES
(1, 'BFSC_STA_24_HOUR', 1, 1501126545, 22, 0.00, 0, 0, 0, 0, 0, 0.00, 0.00, 0, 0, 0.00),
(3, 'BFSC_STA_LOCAL_UI', 1, 1502874213, 0, 0.00, 0, 0, 0, 0, 0, 0.00, 0.00, 0, 0, 0.00),
(4, 'BFSC_STA_UP_2_NOW', 1, 1501641451, 22, 0.00, 0, 0, 0, 0, 0, 0.00, 0.00, 0, 0, 0.00),
(21, 'BFSC_STA_LOCAL_UI', 0, 1503199385, 0, 0.00, 0, 0, 0, 0, 0, 0.00, 0.00, 0, 0, 0.00),
(11897, 'BFSC_STA_60_MIN', 4, 1508402309, 1276, 1790622.00, 2, 2, 0, 2, 0, 45254.00, 0.00, 2, 2, 45254.00),
(24843, 'BFSC_STA_24_HOUR', 4, 1508486339, 8, 11636.00, 0, 0, 0, 0, 0, 0.00, 0.00, 0, 0, 0.00),
(24854, 'BFSC_STA_LOCAL_UI', 4, 1508486344, 0, 0.00, 0, 0, 0, 0, 0, 0.00, 0.00, 0, 0, 0.00),
(24855, 'BFSC_STA_UP_2_NOW', 4, 1508486344, 8, 11636.00, 0, 0, 0, 0, 0, 0.00, 0.00, 0, 0, 0.00);

-- --------------------------------------------------------

--
-- 表的结构 `hcubfscstaticpara`
--

CREATE TABLE `hcubfscstaticpara` (
  `sid` int(4) NOT NULL,
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
-- 转存表中的数据 `hcubfscstaticpara`
--

INSERT INTO `hcubfscstaticpara` (`sid`, `calmaxallowedwgt`, `calfullwgt`, `caladcgain`, `caladcwordrate`, `snrstaticzerovalue`, `snrtailorvalue`, `snrdynzerothread`, `snrdynzerohysteresis`) VALUES
(1, 1000000, 100000, 6, 3, 0, 0, 499, 5000);

-- --------------------------------------------------------

--
-- 表的结构 `hcubfznaccount`
--

CREATE TABLE `hcubfznaccount` (
  `sid` int(4) NOT NULL,
  `username` varchar(20) CHARACTER SET utf8 NOT NULL,
  `password` varchar(50) CHARACTER SET utf8 NOT NULL,
  `memo` text CHARACTER SET utf8
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- 转存表中的数据 `hcubfznaccount`
--

INSERT INTO `hcubfznaccount` (`sid`, `username`, `password`, `memo`) VALUES
(1, 'admin', '0DPiKuNIrrVmD8IUCuw1hQxNqZc', '管理员'),
(4, 'bfzn', '123456', NULL),
(10, 'wxl', '123456', NULL),
(11, 'www', '123456', NULL);

-- --------------------------------------------------------

--
-- 表的结构 `hcuco1datainfo`
--

CREATE TABLE `hcuco1datainfo` (
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
-- 表的结构 `hcuemcdatainfo`
--

CREATE TABLE `hcuemcdatainfo` (
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
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- 转存表中的数据 `hcuemcdatainfo`
--

INSERT INTO `hcuemcdatainfo` (`sid`, `deviceid`, `timestamp`, `dataformat`, `emcvalue`, `EW`, `gpsx`, `NS`, `gpsy`, `gpsz`, `onofflineflag`) VALUES
(12162, 5, 1500786617, 1, 5152, 'E', 0, 'N', 0, 0, 0),
(12163, 5, 1500786717, 1, 5309, 'E', 0, 'N', 0, 0, 0),
(12164, 5, 1500786817, 1, 5081, 'E', 0, 'N', 0, 0, 0),
(12165, 5, 1500786917, 1, 5022, 'E', 0, 'N', 0, 0, 0),
(12166, 5, 1500787210, 1, 5152, 'E', 0, 'N', 0, 0, 0),
(12167, 5, 1500787310, 1, 5309, 'E', 0, 'N', 0, 0, 0),
(12168, 5, 1500787410, 1, 5081, 'E', 0, 'N', 0, 0, 0),
(12169, 5, 1500787510, 1, 5022, 'E', 0, 'N', 0, 0, 0),
(12170, 5, 1500787610, 1, 4687, 'E', 0, 'N', 0, 0, 0),
(12171, 5, 1500787879, 1, 5152, 'E', 0, 'N', 0, 0, 0),
(12172, 5, 1500787979, 1, 5309, 'E', 0, 'N', 0, 0, 0),
(12173, 5, 1500788079, 1, 5081, 'E', 0, 'N', 0, 0, 0),
(12174, 5, 1500788179, 1, 5022, 'E', 0, 'N', 0, 0, 0),
(12175, 5, 1500788279, 1, 4687, 'E', 0, 'N', 0, 0, 0),
(12176, 5, 1500788379, 1, 5350, 'E', 0, 'N', 0, 0, 0),
(12177, 5, 1500788479, 1, 4719, 'E', 0, 'N', 0, 0, 0),
(12178, 5, 1500788657, 1, 5152, 'E', 0, 'N', 0, 0, 0),
(12179, 5, 1500788757, 1, 5309, 'E', 0, 'N', 0, 0, 0),
(12180, 5, 1500788857, 1, 5081, 'E', 0, 'N', 0, 0, 0),
(12181, 5, 1500788957, 1, 5022, 'E', 0, 'N', 0, 0, 0),
(12182, 5, 1500789132, 1, 5152, 'E', 0, 'N', 0, 0, 0),
(12183, 5, 1500789232, 1, 5309, 'E', 0, 'N', 0, 0, 0),
(12184, 5, 1500789332, 1, 5081, 'E', 0, 'N', 0, 0, 0),
(12185, 5, 1500789432, 1, 5022, 'E', 0, 'N', 0, 0, 0),
(12186, 5, 1500789532, 1, 4687, 'E', 0, 'N', 0, 0, 0),
(12187, 5, 1500789632, 1, 5350, 'E', 0, 'N', 0, 0, 0),
(12188, 5, 1500789732, 1, 4719, 'E', 0, 'N', 0, 0, 0),
(12189, 5, 1500789832, 1, 5423, 'E', 0, 'N', 0, 0, 0),
(12190, 5, 1500789932, 1, 5586, 'E', 0, 'N', 0, 0, 0),
(12191, 5, 1500790564, 1, 5152, 'E', 0, 'N', 0, 0, 0),
(12192, 5, 1500791164, 1, 5309, 'E', 0, 'N', 0, 0, 0),
(12193, 5, 1500791764, 1, 5081, 'E', 0, 'N', 0, 0, 0),
(12194, 5, 1500792364, 1, 5022, 'E', 0, 'N', 0, 0, 0),
(12195, 5, 1500792964, 1, 4687, 'E', 0, 'N', 0, 0, 0),
(12196, 5, 1500793564, 1, 5350, 'E', 0, 'N', 0, 0, 0),
(12197, 5, 1500794164, 1, 4719, 'E', 0, 'N', 0, 0, 0),
(12198, 5, 1500794764, 1, 5423, 'E', 0, 'N', 0, 0, 0),
(12199, 5, 1500795364, 1, 5586, 'E', 0, 'N', 0, 0, 0),
(12200, 5, 1500795964, 1, 5200, 'E', 0, 'N', 0, 0, 0),
(12201, 5, 1500796564, 1, 5086, 'E', 0, 'N', 0, 0, 0),
(12202, 5, 1500797164, 1, 4832, 'E', 0, 'N', 0, 0, 0),
(12203, 5, 1500797764, 1, 5396, 'E', 0, 'N', 0, 0, 0),
(12204, 5, 1500798364, 1, 4995, 'E', 0, 'N', 0, 0, 0);

-- --------------------------------------------------------

--
-- 表的结构 `hcuhchodatainfo`
--

CREATE TABLE `hcuhchodatainfo` (
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
-- 表的结构 `hcuhchoze08ch2odatainfo`
--

CREATE TABLE `hcuhchoze08ch2odatainfo` (
  `sid` int(4) NOT NULL,
  `deviceid` int(4) NOT NULL,
  `timestamp` int(4) NOT NULL,
  `dataformat` int(1) NOT NULL,
  `hchovalue` int(4) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- 表的结构 `hcuhsmmpdatainfo`
--

CREATE TABLE `hcuhsmmpdatainfo` (
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
-- 表的结构 `hcuhumiddatainfo`
--

CREATE TABLE `hcuhumiddatainfo` (
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
-- 表的结构 `hcuhumiddht11datainfo`
--

CREATE TABLE `hcuhumiddht11datainfo` (
  `sid` int(4) NOT NULL,
  `deviceid` int(4) NOT NULL,
  `timestamp` int(4) NOT NULL,
  `dataformat` int(1) NOT NULL,
  `humidvalue` int(4) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- 表的结构 `hcuhumidmth01datainfo`
--

CREATE TABLE `hcuhumidmth01datainfo` (
  `sid` int(4) NOT NULL,
  `deviceid` int(4) NOT NULL,
  `timestamp` int(4) NOT NULL,
  `dataformat` int(1) NOT NULL,
  `humidvalue` int(4) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- 转存表中的数据 `hcuhumidmth01datainfo`
--

INSERT INTO `hcuhumidmth01datainfo` (`sid`, `deviceid`, `timestamp`, `dataformat`, `humidvalue`) VALUES
(1, 3, 2, 1, 4);

-- --------------------------------------------------------

--
-- 表的结构 `hcuhumidrht03datainfo`
--

CREATE TABLE `hcuhumidrht03datainfo` (
  `sid` int(4) NOT NULL,
  `deviceid` int(4) NOT NULL,
  `timestamp` int(4) NOT NULL,
  `dataformat` int(1) NOT NULL,
  `humidvalue` int(4) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- 表的结构 `hcuhumidsht20datainfo`
--

CREATE TABLE `hcuhumidsht20datainfo` (
  `sid` int(4) NOT NULL,
  `deviceid` int(4) NOT NULL,
  `timestamp` int(4) NOT NULL,
  `dataformat` int(1) NOT NULL,
  `humidvalue` int(4) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- 表的结构 `hcuigmcj188datainfo`
--

CREATE TABLE `hcuigmcj188datainfo` (
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
-- 表的结构 `hcuihmcj188datainfo`
--

CREATE TABLE `hcuihmcj188datainfo` (
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
-- 表的结构 `hcuipmcj188datainfo`
--

CREATE TABLE `hcuipmcj188datainfo` (
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
-- 表的结构 `hcuiwmcj188datainfo`
--

CREATE TABLE `hcuiwmcj188datainfo` (
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
-- 表的结构 `hculightstrbh1750datainfo`
--

CREATE TABLE `hculightstrbh1750datainfo` (
  `sid` int(4) NOT NULL,
  `deviceid` int(4) NOT NULL,
  `timestamp` int(4) NOT NULL,
  `dataformat` int(1) NOT NULL,
  `lightstrvalue` int(4) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- 表的结构 `hculightstrdatainfo`
--

CREATE TABLE `hculightstrdatainfo` (
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
-- 表的结构 `hcunoisedatainfo`
--

CREATE TABLE `hcunoisedatainfo` (
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
-- 表的结构 `hcupm25bmpd300datainfo`
--

CREATE TABLE `hcupm25bmpd300datainfo` (
  `sid` int(4) NOT NULL,
  `deviceid` int(4) NOT NULL,
  `timestamp` int(4) NOT NULL,
  `dataformat` int(1) NOT NULL,
  `pm2d5value` int(4) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- 表的结构 `hcupm25datainfo`
--

CREATE TABLE `hcupm25datainfo` (
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
-- 表的结构 `hcupm25sharpdatainfo`
--

CREATE TABLE `hcupm25sharpdatainfo` (
  `sid` int(4) NOT NULL,
  `deviceid` int(4) NOT NULL,
  `timestamp` int(4) NOT NULL,
  `dataformat` int(1) NOT NULL,
  `pm2d5value` int(4) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- 表的结构 `hcusysalarminfo`
--

CREATE TABLE `hcusysalarminfo` (
  `sid` int(4) NOT NULL,
  `eqpid` int(4) NOT NULL,
  `alarmtype` int(4) NOT NULL,
  `alarmcontent` int(4) NOT NULL,
  `alarmseverity` int(4) NOT NULL,
  `alarmclearflag` int(4) NOT NULL,
  `alarmdesc` char(100) NOT NULL,
  `timestamp` int(4) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- 转存表中的数据 `hcusysalarminfo`
--

INSERT INTO `hcusysalarminfo` (`sid`, `eqpid`, `alarmtype`, `alarmcontent`, `alarmseverity`, `alarmclearflag`, `alarmdesc`, `timestamp`) VALUES
(6, 7, 1, 6, 1, 0, '', 1500788382),
(7, 6, 1, 3, 1, 0, '', 1500788382),
(8, 35, 1, 4, 1, 0, '', 1500788389),
(9, 33, 1, 5, 1, 0, '', 1500788391),
(10, 6, 1, 2, 1, 0, '', 1500788391),
(11, 1, 1, 1, 1, 0, '', 1500788393),
(12, 6, 1, 2, 1, 0, '', 1500789635),
(13, 7, 1, 6, 1, 0, '', 1500789635),
(14, 35, 1, 4, 1, 0, '', 1500789642),
(15, 1, 1, 1, 1, 0, '', 1500789644),
(16, 33, 1, 5, 1, 0, '', 1500789644),
(17, 6, 1, 3, 1, 0, '', 1500789646),
(18, 7, 1, 6, 1, 0, '', 1500790010),
(19, 1, 1, 1, 1, 0, '', 1500790014),
(20, 35, 1, 4, 1, 0, '', 1500790021),
(21, 7, 1, 6, 1, 0, '', 1500790024),
(22, 6, 1, 3, 1, 0, '', 1500790026),
(23, 33, 1, 5, 1, 0, '', 1500790027),
(24, 6, 1, 2, 1, 0, '', 1500790031),
(25, 7, 1, 6, 1, 0, '', 1500790038),
(26, 35, 1, 4, 1, 0, '', 1500790044),
(27, 1, 1, 1, 1, 0, '', 1500790049),
(28, 33, 1, 5, 1, 0, '', 1500790052),
(29, 7, 1, 6, 1, 0, '', 1500790052),
(30, 6, 1, 3, 1, 0, '', 1500790055),
(31, 6, 1, 2, 1, 0, '', 1500790058),
(32, 7, 1, 6, 1, 0, '', 1500790066),
(33, 35, 1, 4, 1, 0, '', 1500790067),
(34, 33, 1, 5, 1, 0, '', 1500790077),
(35, 7, 1, 6, 1, 0, '', 1500790080),
(36, 1, 1, 1, 1, 0, '', 1500790084),
(37, 6, 1, 3, 1, 0, '', 1500790084),
(38, 6, 1, 2, 1, 0, '', 1500790085),
(39, 35, 1, 4, 1, 0, '', 1500790090),
(40, 7, 1, 6, 1, 0, '', 1500790094),
(41, 33, 1, 5, 1, 0, '', 1500790102),
(42, 7, 1, 6, 1, 0, '', 1500790108),
(43, 6, 1, 2, 1, 0, '', 1500790112),
(44, 6, 1, 3, 1, 0, '', 1500790113),
(45, 35, 1, 4, 1, 0, '', 1500790113),
(46, 1, 1, 1, 1, 0, '', 1500790119),
(47, 7, 1, 6, 1, 0, '', 1500790122),
(48, 33, 1, 5, 1, 0, '', 1500790127),
(49, 7, 1, 6, 1, 0, '', 1500790136),
(50, 35, 1, 4, 1, 0, '', 1500790136),
(51, 6, 1, 2, 1, 0, '', 1500790139),
(52, 6, 1, 3, 1, 0, '', 1500790142),
(53, 7, 1, 6, 1, 0, '', 1500790143),
(54, 33, 1, 5, 1, 0, '', 1500790152),
(55, 1, 1, 1, 1, 0, '', 1500790154),
(56, 7, 1, 6, 1, 0, '', 1500790164),
(57, 6, 1, 3, 1, 0, '', 1500790171),
(58, 7, 1, 6, 1, 0, '', 1500790178),
(59, 35, 1, 4, 1, 0, '', 1500790182),
(60, 1, 1, 1, 1, 0, '', 1500790189),
(61, 7, 1, 6, 1, 0, '', 1500790192),
(62, 7, 1, 6, 1, 0, '', 1500790199),
(63, 6, 1, 3, 1, 0, '', 1500790200),
(64, 7, 1, 6, 1, 0, '', 1500790213),
(65, 1, 1, 1, 1, 0, '', 1500790224),
(66, 7, 1, 6, 1, 0, '', 1500790234),
(67, 7, 1, 6, 1, 0, '', 1500790248),
(68, 7, 1, 6, 1, 0, '', 1500790255),
(69, 1, 1, 1, 1, 0, '', 1500790259),
(70, 7, 1, 6, 1, 0, '', 1500790269),
(71, 6, 1, 2, 1, 0, '', 1500790274),
(72, 35, 1, 4, 1, 0, '', 1500790274),
(73, 33, 1, 5, 1, 0, '', 1500790277),
(74, 7, 1, 6, 1, 0, '', 1500790290),
(75, 7, 1, 6, 1, 0, '', 1500790304),
(76, 7, 1, 6, 1, 0, '', 1500790325),
(77, 6, 1, 2, 1, 0, '', 1500790328),
(78, 1, 1, 1, 1, 0, '', 1500790329),
(79, 6, 1, 3, 1, 0, '', 1500790345),
(80, 7, 1, 6, 1, 0, '', 1500790346),
(81, 7, 1, 6, 1, 0, '', 1500790360),
(82, 35, 1, 4, 1, 0, '', 1500790366),
(83, 6, 1, 3, 1, 0, '', 1500790374),
(84, 33, 1, 5, 1, 0, '', 1500790377),
(85, 6, 1, 2, 1, 0, '', 1500790382),
(86, 7, 1, 6, 1, 0, '', 1500790395),
(87, 33, 1, 5, 1, 0, '', 1500790402),
(88, 6, 1, 3, 1, 0, '', 1500790403),
(89, 7, 1, 6, 1, 0, '', 1500790409),
(90, 6, 1, 2, 1, 0, '', 1500790409),
(91, 35, 1, 4, 1, 0, '', 1500790412),
(92, 7, 1, 6, 1, 0, '', 1500790423),
(93, 33, 1, 5, 1, 0, '', 1500790427),
(94, 6, 1, 3, 1, 0, '', 1500790432),
(95, 35, 1, 4, 1, 0, '', 1500790435),
(96, 6, 1, 2, 1, 0, '', 1500790436),
(97, 7, 1, 6, 1, 0, '', 1500790437),
(98, 7, 1, 6, 1, 0, '', 1500790451),
(99, 33, 1, 5, 1, 0, '', 1500790452),
(100, 35, 1, 4, 1, 0, '', 1500790458),
(101, 6, 1, 3, 1, 0, '', 1500790461),
(102, 6, 1, 2, 1, 0, '', 1500790463),
(103, 7, 1, 6, 1, 0, '', 1500790465),
(104, 33, 1, 5, 1, 0, '', 1500790477),
(105, 7, 1, 6, 1, 0, '', 1500790479),
(106, 35, 1, 4, 1, 0, '', 1500790481),
(107, 6, 1, 2, 1, 0, '', 1500790490),
(108, 6, 1, 3, 1, 0, '', 1500790490),
(109, 7, 1, 6, 1, 0, '', 1500790493),
(110, 33, 1, 5, 1, 0, '', 1500790502),
(111, 1, 1, 1, 1, 0, '', 1500790504),
(112, 35, 1, 4, 1, 0, '', 1500790504),
(113, 7, 1, 6, 1, 0, '', 1500790507),
(114, 6, 1, 2, 1, 0, '', 1500790517),
(115, 6, 1, 3, 1, 0, '', 1500790519),
(116, 7, 1, 6, 1, 0, '', 1500790521),
(117, 33, 1, 5, 1, 0, '', 1500790532),
(118, 35, 1, 4, 1, 0, '', 1500790533),
(119, 7, 1, 6, 1, 0, '', 1500790535),
(120, 1, 1, 1, 1, 0, '', 1500790539),
(121, 6, 1, 2, 1, 0, '', 1500790544),
(122, 6, 1, 3, 1, 0, '', 1500790548),
(123, 7, 1, 6, 1, 0, '', 1500790549),
(124, 35, 1, 4, 1, 0, '', 1500790550),
(125, 33, 1, 5, 1, 0, '', 1500790552),
(126, 7, 1, 6, 1, 0, '', 1500790563),
(127, 6, 1, 2, 1, 0, '', 1500790571),
(128, 35, 1, 4, 1, 0, '', 1500790573),
(129, 1, 1, 1, 1, 0, '', 1500790574),
(130, 33, 1, 5, 1, 0, '', 1500790577),
(131, 7, 1, 6, 1, 0, '', 1500790577),
(132, 6, 1, 3, 1, 0, '', 1500790577),
(133, 7, 1, 6, 1, 0, '', 1500790591),
(134, 35, 1, 4, 1, 0, '', 1500790596),
(135, 6, 1, 2, 1, 0, '', 1500790598),
(136, 33, 1, 5, 1, 0, '', 1500790602),
(137, 7, 1, 6, 1, 0, '', 1500790605),
(138, 6, 1, 3, 1, 0, '', 1500790606),
(139, 1, 1, 1, 1, 0, '', 1500790609),
(140, 7, 1, 6, 1, 0, '', 1500790619),
(141, 35, 1, 4, 1, 0, '', 1500790619),
(142, 6, 1, 2, 1, 0, '', 1500790625),
(143, 33, 1, 5, 1, 0, '', 1500790627),
(144, 7, 1, 6, 1, 0, '', 1500790633),
(145, 6, 1, 3, 1, 0, '', 1500790635),
(146, 35, 1, 4, 1, 0, '', 1500790642),
(147, 1, 1, 1, 1, 0, '', 1500790644),
(148, 7, 1, 6, 1, 0, '', 1500790647),
(149, 33, 1, 5, 1, 0, '', 1500790652),
(150, 6, 1, 2, 1, 0, '', 1500790652),
(151, 7, 1, 6, 1, 0, '', 1500790661),
(152, 6, 1, 3, 1, 0, '', 1500790664),
(153, 35, 1, 4, 1, 0, '', 1500790665),
(154, 7, 1, 6, 1, 0, '', 1500790675),
(155, 33, 1, 5, 1, 0, '', 1500790677),
(156, 1, 1, 1, 1, 0, '', 1500790679),
(157, 6, 1, 2, 1, 0, '', 1500790679),
(158, 35, 1, 4, 1, 0, '', 1500790688),
(159, 7, 1, 6, 1, 0, '', 1500790689),
(160, 6, 1, 3, 1, 0, '', 1500790693),
(161, 33, 1, 5, 1, 0, '', 1500790702),
(162, 7, 1, 6, 1, 0, '', 1500790703),
(163, 6, 1, 2, 1, 0, '', 1500790706),
(164, 35, 1, 4, 1, 0, '', 1500790711),
(165, 1, 1, 1, 1, 0, '', 1500790714),
(166, 7, 1, 6, 1, 0, '', 1500790717),
(167, 6, 1, 3, 1, 0, '', 1500790722),
(168, 33, 1, 5, 1, 0, '', 1500790727),
(169, 7, 1, 6, 1, 0, '', 1500790731),
(170, 6, 1, 2, 1, 0, '', 1500790733),
(171, 35, 1, 4, 1, 0, '', 1500790734),
(172, 7, 1, 6, 1, 0, '', 1500790738),
(173, 7, 1, 6, 1, 0, '', 1500790745),
(174, 1, 1, 1, 1, 0, '', 1500790749),
(175, 33, 1, 5, 1, 0, '', 1500790752),
(176, 35, 1, 4, 1, 0, '', 1500790757),
(177, 7, 1, 6, 1, 0, '', 1500790759),
(178, 6, 1, 2, 1, 0, '', 1500790760),
(179, 7, 1, 6, 1, 0, '', 1500790773),
(180, 33, 1, 5, 1, 0, '', 1500790777),
(181, 35, 1, 4, 1, 0, '', 1500790780),
(182, 1, 1, 1, 1, 0, '', 1500790784),
(183, 6, 1, 2, 1, 0, '', 1500790787),
(184, 7, 1, 6, 1, 0, '', 1500790787),
(185, 7, 1, 6, 1, 0, '', 1500790801),
(186, 33, 1, 5, 1, 0, '', 1500790802),
(187, 35, 1, 4, 1, 0, '', 1500790803),
(188, 6, 1, 2, 1, 0, '', 1500790814),
(189, 7, 1, 6, 1, 0, '', 1500790815),
(190, 1, 1, 1, 1, 0, '', 1500790819),
(191, 35, 1, 4, 1, 0, '', 1500790832),
(192, 33, 1, 5, 1, 0, '', 1500790832),
(193, 7, 1, 6, 1, 0, '', 1500790832),
(194, 6, 1, 2, 1, 0, '', 1500790841),
(195, 7, 1, 6, 1, 0, '', 1500790843),
(196, 35, 1, 4, 1, 0, '', 1500790849),
(197, 33, 1, 5, 1, 0, '', 1500790852),
(198, 1, 1, 1, 1, 0, '', 1500790854),
(199, 7, 1, 6, 1, 0, '', 1500790857),
(200, 6, 1, 3, 1, 0, '', 1500790867),
(201, 6, 1, 2, 1, 0, '', 1500790868),
(202, 7, 1, 6, 1, 0, '', 1500790871),
(203, 35, 1, 4, 1, 0, '', 1500790872),
(204, 33, 1, 5, 1, 0, '', 1500790877),
(205, 7, 1, 6, 1, 0, '', 1500790885),
(206, 1, 1, 1, 1, 0, '', 1500790889),
(207, 6, 1, 2, 1, 0, '', 1500790895),
(208, 35, 1, 4, 1, 0, '', 1500790895),
(209, 6, 1, 3, 1, 0, '', 1500790896),
(210, 7, 1, 6, 1, 0, '', 1500790899),
(211, 33, 1, 5, 1, 0, '', 1500790902),
(212, 7, 1, 6, 1, 0, '', 1500790913),
(213, 35, 1, 4, 1, 0, '', 1500790918),
(214, 6, 1, 2, 1, 0, '', 1500790922),
(215, 1, 1, 1, 1, 0, '', 1500790924),
(216, 6, 1, 3, 1, 0, '', 1500790925),
(217, 33, 1, 5, 1, 0, '', 1500790927),
(218, 7, 1, 6, 1, 0, '', 1500790927),
(219, 7, 1, 6, 1, 0, '', 1500790941),
(220, 35, 1, 4, 1, 0, '', 1500790941),
(221, 6, 1, 2, 1, 0, '', 1500790949),
(222, 33, 1, 5, 1, 0, '', 1500790952),
(223, 6, 1, 3, 1, 0, '', 1500790954),
(224, 7, 1, 6, 1, 0, '', 1500790955),
(225, 1, 1, 1, 1, 0, '', 1500790959),
(226, 35, 1, 4, 1, 0, '', 1500790964),
(227, 7, 1, 6, 1, 0, '', 1500790969),
(228, 6, 1, 2, 1, 0, '', 1500790976),
(229, 33, 1, 5, 1, 0, '', 1500790977),
(230, 6, 1, 3, 1, 0, '', 1500790983),
(231, 7, 1, 6, 1, 0, '', 1500790983),
(232, 35, 1, 4, 1, 0, '', 1500790987),
(233, 1, 1, 1, 1, 0, '', 1500790994),
(234, 7, 1, 6, 1, 0, '', 1500790997),
(235, 33, 1, 5, 1, 0, '', 1500791002),
(236, 6, 1, 2, 1, 0, '', 1500791003),
(237, 35, 1, 4, 1, 0, '', 1500791010),
(238, 7, 1, 6, 1, 0, '', 1500791011),
(239, 6, 1, 3, 1, 0, '', 1500791012),
(240, 7, 1, 6, 1, 0, '', 1500791025),
(241, 33, 1, 5, 1, 0, '', 1500791027),
(242, 1, 1, 1, 1, 0, '', 1500791029),
(243, 6, 1, 2, 1, 0, '', 1500791030),
(244, 35, 1, 4, 1, 0, '', 1500791033),
(245, 7, 1, 6, 1, 0, '', 1500791039),
(246, 6, 1, 3, 1, 0, '', 1500791041),
(247, 33, 1, 5, 1, 0, '', 1500791052),
(248, 7, 1, 6, 1, 0, '', 1500791053),
(249, 35, 1, 4, 1, 0, '', 1500791056),
(250, 6, 1, 2, 1, 0, '', 1500791057),
(251, 1, 1, 1, 1, 0, '', 1500791064),
(252, 7, 1, 6, 1, 0, '', 1500791067),
(253, 6, 1, 3, 1, 0, '', 1500791070),
(254, 33, 1, 5, 1, 0, '', 1500791077),
(255, 35, 1, 4, 1, 0, '', 1500791079),
(256, 7, 1, 6, 1, 0, '', 1500791081),
(257, 6, 1, 2, 1, 0, '', 1500791084),
(258, 7, 1, 6, 1, 0, '', 1500791095),
(259, 1, 1, 1, 1, 0, '', 1500791099),
(260, 6, 1, 3, 1, 0, '', 1500791099),
(261, 33, 1, 5, 1, 0, '', 1500791102),
(262, 35, 1, 4, 1, 0, '', 1500791102),
(263, 7, 1, 6, 1, 0, '', 1500791109),
(264, 6, 1, 2, 1, 0, '', 1500791111),
(265, 7, 1, 6, 1, 0, '', 1500791132),
(266, 35, 1, 4, 1, 0, '', 1500791132),
(267, 33, 1, 5, 1, 0, '', 1500791132),
(268, 6, 1, 3, 1, 0, '', 1500791132),
(269, 1, 1, 1, 1, 0, '', 1500791134),
(270, 7, 1, 6, 1, 0, '', 1500791137),
(271, 6, 1, 2, 1, 0, '', 1500791138),
(272, 35, 1, 4, 1, 0, '', 1500791148),
(273, 7, 1, 6, 1, 0, '', 1500791151),
(274, 33, 1, 5, 1, 0, '', 1500791152),
(275, 6, 1, 3, 1, 0, '', 1500791157),
(276, 6, 1, 2, 1, 0, '', 1500791165),
(277, 7, 1, 6, 1, 0, '', 1500791165),
(278, 1, 1, 1, 1, 0, '', 1500791169),
(279, 35, 1, 4, 1, 0, '', 1500791171),
(280, 33, 1, 5, 1, 0, '', 1500791177),
(281, 7, 1, 6, 1, 0, '', 1500791179),
(282, 6, 1, 3, 1, 0, '', 1500791186),
(283, 6, 1, 2, 1, 0, '', 1500791192),
(284, 7, 1, 6, 1, 0, '', 1500791193),
(285, 35, 1, 4, 1, 0, '', 1500791194),
(286, 33, 1, 5, 1, 0, '', 1500791202),
(287, 1, 1, 1, 1, 0, '', 1500791204),
(288, 7, 1, 6, 1, 0, '', 1500791207),
(289, 6, 1, 3, 1, 0, '', 1500791215),
(290, 35, 1, 4, 1, 0, '', 1500791217),
(291, 6, 1, 2, 1, 0, '', 1500791219),
(292, 7, 1, 6, 1, 0, '', 1500791221),
(293, 33, 1, 5, 1, 0, '', 1500791227),
(294, 7, 1, 6, 1, 0, '', 1500791235),
(295, 1, 1, 1, 1, 0, '', 1500791239),
(296, 35, 1, 4, 1, 0, '', 1500791240),
(297, 6, 1, 3, 1, 0, '', 1500791244),
(298, 6, 1, 2, 1, 0, '', 1500791246),
(299, 7, 1, 6, 1, 0, '', 1500791249),
(300, 33, 1, 5, 1, 0, '', 1500791252),
(301, 7, 1, 6, 1, 0, '', 1500791263),
(302, 35, 1, 4, 1, 0, '', 1500791263),
(303, 6, 1, 2, 1, 0, '', 1500791273),
(304, 6, 1, 3, 1, 0, '', 1500791273),
(305, 1, 1, 1, 1, 0, '', 1500791274),
(306, 33, 1, 5, 1, 0, '', 1500791277),
(307, 7, 1, 6, 1, 0, '', 1500791277),
(308, 35, 1, 4, 1, 0, '', 1500791286),
(309, 7, 1, 6, 1, 0, '', 1500791291),
(310, 6, 1, 2, 1, 0, '', 1500791300),
(311, 33, 1, 5, 1, 0, '', 1500791302),
(312, 6, 1, 3, 1, 0, '', 1500791302),
(313, 7, 1, 6, 1, 0, '', 1500791305),
(314, 1, 1, 1, 1, 0, '', 1500791309),
(315, 35, 1, 4, 1, 0, '', 1500791309),
(316, 7, 1, 6, 1, 0, '', 1500791319),
(317, 33, 1, 5, 1, 0, '', 1500791327),
(318, 6, 1, 2, 1, 0, '', 1500791327),
(319, 6, 1, 3, 1, 0, '', 1500791331),
(320, 35, 1, 4, 1, 0, '', 1500791332),
(321, 7, 1, 6, 1, 0, '', 1500791333),
(322, 1, 1, 1, 1, 0, '', 1500791344),
(323, 7, 1, 6, 1, 0, '', 1500791347),
(324, 33, 1, 5, 1, 0, '', 1500791352),
(325, 6, 1, 2, 1, 0, '', 1500791354),
(326, 35, 1, 4, 1, 0, '', 1500791355),
(327, 6, 1, 3, 1, 0, '', 1500791360),
(328, 7, 1, 6, 1, 0, '', 1500791361),
(329, 7, 1, 6, 1, 0, '', 1500791375),
(330, 33, 1, 5, 1, 0, '', 1500791377),
(331, 35, 1, 4, 1, 0, '', 1500791378),
(332, 1, 1, 1, 1, 0, '', 1500791379),
(333, 6, 1, 2, 1, 0, '', 1500791381),
(334, 6, 1, 3, 1, 0, '', 1500791389),
(335, 7, 1, 6, 1, 0, '', 1500791389),
(336, 35, 1, 4, 1, 0, '', 1500791401),
(337, 33, 1, 5, 1, 0, '', 1500791402),
(338, 7, 1, 6, 1, 0, '', 1500791403),
(339, 6, 1, 2, 1, 0, '', 1500791408),
(340, 1, 1, 1, 1, 0, '', 1500791414),
(341, 7, 1, 6, 1, 0, '', 1500791417),
(342, 6, 1, 3, 1, 0, '', 1500791418),
(343, 35, 1, 4, 1, 0, '', 1500791432),
(344, 33, 1, 5, 1, 0, '', 1500791432),
(345, 7, 1, 6, 1, 0, '', 1500791432),
(346, 6, 1, 2, 1, 0, '', 1500791435),
(347, 7, 1, 6, 1, 0, '', 1500791445),
(348, 35, 1, 4, 1, 0, '', 1500791447),
(349, 6, 1, 3, 1, 0, '', 1500791447),
(350, 1, 1, 1, 1, 0, '', 1500791449),
(351, 33, 1, 5, 1, 0, '', 1500791452),
(352, 7, 1, 6, 1, 0, '', 1500791459),
(353, 6, 1, 2, 1, 0, '', 1500791462),
(354, 35, 1, 4, 1, 0, '', 1500791470),
(355, 7, 1, 6, 1, 0, '', 1500791473),
(356, 6, 1, 3, 1, 0, '', 1500791476),
(357, 33, 1, 5, 1, 0, '', 1500791477),
(358, 1, 1, 1, 1, 0, '', 1500791484),
(359, 7, 1, 6, 1, 0, '', 1500791487),
(360, 6, 1, 2, 1, 0, '', 1500791489),
(361, 35, 1, 4, 1, 0, '', 1500791493),
(362, 7, 1, 6, 1, 0, '', 1500791501),
(363, 33, 1, 5, 1, 0, '', 1500791502),
(364, 6, 1, 3, 1, 0, '', 1500791505),
(365, 7, 1, 6, 1, 0, '', 1500791515),
(366, 6, 1, 2, 1, 0, '', 1500791516),
(367, 35, 1, 4, 1, 0, '', 1500791516),
(368, 1, 1, 1, 1, 0, '', 1500791519),
(369, 33, 1, 5, 1, 0, '', 1500791527),
(370, 7, 1, 6, 1, 0, '', 1500791529),
(371, 6, 1, 3, 1, 0, '', 1500791534),
(372, 35, 1, 4, 1, 0, '', 1500791539),
(373, 6, 1, 2, 1, 0, '', 1500791543),
(374, 7, 1, 6, 1, 0, '', 1500791543),
(375, 33, 1, 5, 1, 0, '', 1500791552),
(376, 1, 1, 1, 1, 0, '', 1500791554),
(377, 7, 1, 6, 1, 0, '', 1500791557),
(378, 35, 1, 4, 1, 0, '', 1500791562),
(379, 6, 1, 3, 1, 0, '', 1500791563),
(380, 6, 1, 2, 1, 0, '', 1500791570),
(381, 7, 1, 6, 1, 0, '', 1500791571),
(382, 33, 1, 5, 1, 0, '', 1500791577),
(383, 35, 1, 4, 1, 0, '', 1500791585),
(384, 7, 1, 6, 1, 0, '', 1500791585),
(385, 1, 1, 1, 1, 0, '', 1500791589),
(386, 6, 1, 3, 1, 0, '', 1500791592),
(387, 6, 1, 2, 1, 0, '', 1500791597),
(388, 7, 1, 6, 1, 0, '', 1500791599),
(389, 33, 1, 5, 1, 0, '', 1500791602),
(390, 35, 1, 4, 1, 0, '', 1500791608),
(391, 7, 1, 6, 1, 0, '', 1500791613),
(392, 6, 1, 3, 1, 0, '', 1500791621),
(393, 1, 1, 1, 1, 0, '', 1500791624),
(394, 6, 1, 2, 1, 0, '', 1500791624),
(395, 33, 1, 5, 1, 0, '', 1500791627),
(396, 7, 1, 6, 1, 0, '', 1500791627),
(397, 35, 1, 4, 1, 0, '', 1500791631),
(398, 7, 1, 6, 1, 0, '', 1500791641),
(399, 6, 1, 3, 1, 0, '', 1500791650),
(400, 6, 1, 2, 1, 0, '', 1500791651),
(401, 33, 1, 5, 1, 0, '', 1500791652),
(402, 35, 1, 4, 1, 0, '', 1500791654),
(403, 7, 1, 6, 1, 0, '', 1500791655),
(404, 1, 1, 1, 1, 0, '', 1500791659),
(405, 7, 1, 6, 1, 0, '', 1500791669),
(406, 35, 1, 4, 1, 0, '', 1500791677),
(407, 33, 1, 5, 1, 0, '', 1500791677),
(408, 6, 1, 2, 1, 0, '', 1500791678),
(409, 6, 1, 3, 1, 0, '', 1500791679),
(410, 7, 1, 6, 1, 0, '', 1500791683),
(411, 1, 1, 1, 1, 0, '', 1500791694),
(412, 7, 1, 6, 1, 0, '', 1500791697),
(413, 35, 1, 4, 1, 0, '', 1500791700),
(414, 33, 1, 5, 1, 0, '', 1500791702),
(415, 6, 1, 2, 1, 0, '', 1500791705),
(416, 6, 1, 3, 1, 0, '', 1500791708),
(417, 7, 1, 6, 1, 0, '', 1500791711),
(418, 35, 1, 4, 1, 0, '', 1500791732),
(419, 7, 1, 6, 1, 0, '', 1500791732),
(420, 33, 1, 5, 1, 0, '', 1500791732),
(421, 1, 1, 1, 1, 0, '', 1500791732),
(422, 6, 1, 2, 1, 0, '', 1500791732),
(423, 6, 1, 3, 1, 0, '', 1500791737),
(424, 7, 1, 6, 1, 0, '', 1500791739),
(425, 35, 1, 4, 1, 0, '', 1500791746),
(426, 33, 1, 5, 1, 0, '', 1500791752),
(427, 7, 1, 6, 1, 0, '', 1500791753),
(428, 6, 1, 2, 1, 0, '', 1500791759),
(429, 1, 1, 1, 1, 0, '', 1500791764),
(430, 6, 1, 3, 1, 0, '', 1500791766),
(431, 7, 1, 6, 1, 0, '', 1500791767),
(432, 35, 1, 4, 1, 0, '', 1500791769),
(433, 33, 1, 5, 1, 0, '', 1500791777),
(434, 7, 1, 6, 1, 0, '', 1500791781),
(435, 6, 1, 2, 1, 0, '', 1500791786),
(436, 35, 1, 4, 1, 0, '', 1500791792),
(437, 6, 1, 3, 1, 0, '', 1500791795),
(438, 7, 1, 6, 1, 0, '', 1500791795),
(439, 1, 1, 1, 1, 0, '', 1500791799),
(440, 33, 1, 5, 1, 0, '', 1500791802),
(441, 7, 1, 6, 1, 0, '', 1500791809),
(442, 6, 1, 2, 1, 0, '', 1500791813),
(443, 35, 1, 4, 1, 0, '', 1500791815),
(444, 7, 1, 6, 1, 0, '', 1500791823),
(445, 6, 1, 3, 1, 0, '', 1500791824),
(446, 33, 1, 5, 1, 0, '', 1500791827),
(447, 1, 1, 1, 1, 0, '', 1500791834),
(448, 7, 1, 6, 1, 0, '', 1500791837),
(449, 35, 1, 4, 1, 0, '', 1500791838),
(450, 6, 1, 2, 1, 0, '', 1500791840),
(451, 7, 1, 6, 1, 0, '', 1500791851),
(452, 33, 1, 5, 1, 0, '', 1500791852),
(453, 6, 1, 3, 1, 0, '', 1500791853),
(454, 35, 1, 4, 1, 0, '', 1500791861),
(455, 7, 1, 6, 1, 0, '', 1500791865),
(456, 6, 1, 2, 1, 0, '', 1500791867),
(457, 1, 1, 1, 1, 0, '', 1500791869),
(458, 33, 1, 5, 1, 0, '', 1500791877),
(459, 7, 1, 6, 1, 0, '', 1500791879),
(460, 6, 1, 3, 1, 0, '', 1500791882),
(461, 35, 1, 4, 1, 0, '', 1500791884),
(462, 7, 1, 6, 1, 0, '', 1500791893),
(463, 6, 1, 2, 1, 0, '', 1500791894),
(464, 33, 1, 5, 1, 0, '', 1500791902),
(465, 1, 1, 1, 1, 0, '', 1500791904),
(466, 7, 1, 6, 1, 0, '', 1500791907),
(467, 35, 1, 4, 1, 0, '', 1500791907),
(468, 6, 1, 3, 1, 0, '', 1500791911),
(469, 6, 1, 2, 1, 0, '', 1500791921),
(470, 7, 1, 6, 1, 0, '', 1500791921),
(471, 33, 1, 5, 1, 0, '', 1500791927),
(472, 35, 1, 4, 1, 0, '', 1500791930),
(473, 7, 1, 6, 1, 0, '', 1500791935),
(474, 1, 1, 1, 1, 0, '', 1500791939),
(475, 6, 1, 3, 1, 0, '', 1500791940),
(476, 6, 1, 2, 1, 0, '', 1500791948),
(477, 7, 1, 6, 1, 0, '', 1500791949),
(478, 33, 1, 5, 1, 0, '', 1500791952),
(479, 35, 1, 4, 1, 0, '', 1500791953),
(480, 7, 1, 6, 1, 0, '', 1500791963),
(481, 6, 1, 3, 1, 0, '', 1500791969),
(482, 1, 1, 1, 1, 0, '', 1500791974),
(483, 6, 1, 2, 1, 0, '', 1500791975),
(484, 35, 1, 4, 1, 0, '', 1500791976),
(485, 33, 1, 5, 1, 0, '', 1500791977),
(486, 7, 1, 6, 1, 0, '', 1500791977),
(487, 7, 1, 6, 1, 0, '', 1500791991),
(488, 6, 1, 3, 1, 0, '', 1500791998),
(489, 35, 1, 4, 1, 0, '', 1500791999),
(490, 33, 1, 5, 1, 0, '', 1500792002),
(491, 6, 1, 2, 1, 0, '', 1500792002),
(492, 7, 1, 6, 1, 0, '', 1500792005),
(493, 1, 1, 1, 1, 0, '', 1500792009),
(494, 7, 1, 6, 1, 0, '', 1500792019),
(495, 35, 1, 4, 1, 0, '', 1500792032),
(496, 33, 1, 5, 1, 0, '', 1500792033),
(497, 6, 1, 3, 1, 0, '', 1500792033),
(498, 6, 1, 2, 1, 0, '', 1500792034),
(499, 7, 1, 6, 1, 0, '', 1500792034),
(500, 1, 1, 1, 1, 0, '', 1500792044),
(501, 35, 1, 4, 1, 0, '', 1500792045),
(502, 7, 1, 6, 1, 0, '', 1500792047),
(503, 33, 1, 5, 1, 0, '', 1500792052),
(504, 6, 1, 2, 1, 0, '', 1500792056),
(505, 6, 1, 3, 1, 0, '', 1500792056),
(506, 7, 1, 6, 1, 0, '', 1500792061),
(507, 35, 1, 4, 1, 0, '', 1500792068),
(508, 7, 1, 6, 1, 0, '', 1500792075),
(509, 33, 1, 5, 1, 0, '', 1500792077),
(510, 1, 1, 1, 1, 0, '', 1500792079),
(511, 6, 1, 2, 1, 0, '', 1500792083),
(512, 6, 1, 3, 1, 0, '', 1500792085),
(513, 7, 1, 6, 1, 0, '', 1500792089),
(514, 35, 1, 4, 1, 0, '', 1500792091),
(515, 33, 1, 5, 1, 0, '', 1500792102),
(516, 7, 1, 6, 1, 0, '', 1500792103),
(517, 6, 1, 2, 1, 0, '', 1500792110),
(518, 35, 1, 4, 1, 0, '', 1500792114),
(519, 6, 1, 3, 1, 0, '', 1500792114),
(520, 1, 1, 1, 1, 0, '', 1500792114),
(521, 7, 1, 6, 1, 0, '', 1500792117),
(522, 33, 1, 5, 1, 0, '', 1500792127),
(523, 7, 1, 6, 1, 0, '', 1500792131),
(524, 6, 1, 2, 1, 0, '', 1500792137),
(525, 35, 1, 4, 1, 0, '', 1500792137),
(526, 6, 1, 3, 1, 0, '', 1500792143),
(527, 7, 1, 6, 1, 0, '', 1500792145),
(528, 1, 1, 1, 1, 0, '', 1500792149),
(529, 33, 1, 5, 1, 0, '', 1500792152),
(530, 7, 1, 6, 1, 0, '', 1500792159),
(531, 35, 1, 4, 1, 0, '', 1500792160),
(532, 6, 1, 2, 1, 0, '', 1500792164),
(533, 6, 1, 3, 1, 0, '', 1500792172),
(534, 7, 1, 6, 1, 0, '', 1500792173),
(535, 33, 1, 5, 1, 0, '', 1500792177),
(536, 35, 1, 4, 1, 0, '', 1500792183),
(537, 1, 1, 1, 1, 0, '', 1500792184),
(538, 7, 1, 6, 1, 0, '', 1500792187),
(539, 6, 1, 2, 1, 0, '', 1500792191),
(540, 6, 1, 3, 1, 0, '', 1500792201),
(541, 7, 1, 6, 1, 0, '', 1500792201),
(542, 33, 1, 5, 1, 0, '', 1500792202),
(543, 35, 1, 4, 1, 0, '', 1500792206),
(544, 7, 1, 6, 1, 0, '', 1500792215),
(545, 6, 1, 2, 1, 0, '', 1500792218),
(546, 1, 1, 1, 1, 0, '', 1500792219),
(547, 33, 1, 5, 1, 0, '', 1500792227),
(548, 7, 1, 6, 1, 0, '', 1500792229),
(549, 35, 1, 4, 1, 0, '', 1500792229),
(550, 6, 1, 3, 1, 0, '', 1500792230),
(551, 7, 1, 6, 1, 0, '', 1500792243),
(552, 6, 1, 2, 1, 0, '', 1500792245),
(553, 33, 1, 5, 1, 0, '', 1500792252),
(554, 35, 1, 4, 1, 0, '', 1500792252),
(555, 1, 1, 1, 1, 0, '', 1500792254),
(556, 7, 1, 6, 1, 0, '', 1500792257),
(557, 6, 1, 3, 1, 0, '', 1500792259),
(558, 7, 1, 6, 1, 0, '', 1500792271),
(559, 6, 1, 2, 1, 0, '', 1500792272),
(560, 35, 1, 4, 1, 0, '', 1500792275),
(561, 33, 1, 5, 1, 0, '', 1500792277),
(562, 7, 1, 6, 1, 0, '', 1500792285),
(563, 6, 1, 3, 1, 0, '', 1500792288),
(564, 1, 1, 1, 1, 0, '', 1500792289),
(565, 35, 1, 4, 1, 0, '', 1500792298),
(566, 6, 1, 2, 1, 0, '', 1500792299),
(567, 7, 1, 6, 1, 0, '', 1500792299),
(568, 33, 1, 5, 1, 0, '', 1500792302),
(569, 7, 1, 6, 1, 0, '', 1500792313),
(570, 6, 1, 3, 1, 0, '', 1500792317),
(571, 35, 1, 4, 1, 0, '', 1500792321),
(572, 1, 1, 1, 1, 0, '', 1500792332),
(573, 6, 1, 2, 1, 0, '', 1500792332),
(574, 33, 1, 5, 1, 0, '', 1500792332),
(575, 7, 1, 6, 1, 0, '', 1500792332),
(576, 7, 1, 6, 1, 0, '', 1500792341),
(577, 35, 1, 4, 1, 0, '', 1500792344),
(578, 6, 1, 3, 1, 0, '', 1500792346),
(579, 33, 1, 5, 1, 0, '', 1500792352),
(580, 6, 1, 2, 1, 0, '', 1500792353),
(581, 7, 1, 6, 1, 0, '', 1500792355),
(582, 1, 1, 1, 1, 0, '', 1500792359),
(583, 35, 1, 4, 1, 0, '', 1500792367),
(584, 7, 1, 6, 1, 0, '', 1500792369),
(585, 6, 1, 3, 1, 0, '', 1500792375),
(586, 33, 1, 5, 1, 0, '', 1500792377),
(587, 6, 1, 2, 1, 0, '', 1500792380),
(588, 7, 1, 6, 1, 0, '', 1500792383),
(589, 35, 1, 4, 1, 0, '', 1500792390),
(590, 1, 1, 1, 1, 0, '', 1500792394),
(591, 7, 1, 6, 1, 0, '', 1500792397),
(592, 33, 1, 5, 1, 0, '', 1500792402),
(593, 6, 1, 3, 1, 0, '', 1500792404),
(594, 6, 1, 2, 1, 0, '', 1500792407),
(595, 7, 1, 6, 1, 0, '', 1500792411),
(596, 35, 1, 4, 1, 0, '', 1500792413),
(597, 7, 1, 6, 1, 0, '', 1500792425),
(598, 33, 1, 5, 1, 0, '', 1500792427),
(599, 1, 1, 1, 1, 0, '', 1500792429),
(600, 6, 1, 3, 1, 0, '', 1500792433),
(601, 6, 1, 2, 1, 0, '', 1500792434),
(602, 35, 1, 4, 1, 0, '', 1500792436),
(603, 7, 1, 6, 1, 0, '', 1500792439),
(604, 33, 1, 5, 1, 0, '', 1500792452),
(605, 7, 1, 6, 1, 0, '', 1500792453),
(606, 35, 1, 4, 1, 0, '', 1500792459),
(607, 6, 1, 2, 1, 0, '', 1500792461),
(608, 6, 1, 3, 1, 0, '', 1500792462),
(609, 1, 1, 1, 1, 0, '', 1500792464),
(610, 7, 1, 6, 1, 0, '', 1500792467),
(611, 33, 1, 5, 1, 0, '', 1500792477),
(612, 7, 1, 6, 1, 0, '', 1500792481),
(613, 35, 1, 4, 1, 0, '', 1500792482),
(614, 6, 1, 2, 1, 0, '', 1500792488),
(615, 6, 1, 3, 1, 0, '', 1500792491),
(616, 7, 1, 6, 1, 0, '', 1500792495),
(617, 1, 1, 1, 1, 0, '', 1500792499),
(618, 33, 1, 5, 1, 0, '', 1500792502),
(619, 35, 1, 4, 1, 0, '', 1500792505),
(620, 7, 1, 6, 1, 0, '', 1500792509),
(621, 6, 1, 2, 1, 0, '', 1500792515),
(622, 6, 1, 3, 1, 0, '', 1500792520),
(623, 7, 1, 6, 1, 0, '', 1500792523),
(624, 33, 1, 5, 1, 0, '', 1500792527),
(625, 35, 1, 4, 1, 0, '', 1500792528),
(626, 1, 1, 1, 1, 0, '', 1500792534),
(627, 7, 1, 6, 1, 0, '', 1500792537),
(628, 6, 1, 2, 1, 0, '', 1500792542),
(629, 6, 1, 3, 1, 0, '', 1500792549),
(630, 35, 1, 4, 1, 0, '', 1500792551),
(631, 7, 1, 6, 1, 0, '', 1500792551),
(632, 33, 1, 5, 1, 0, '', 1500792552),
(633, 7, 1, 6, 1, 0, '', 1500792565),
(634, 6, 1, 2, 1, 0, '', 1500792569),
(635, 1, 1, 1, 1, 0, '', 1500792569),
(636, 35, 1, 4, 1, 0, '', 1500792574),
(637, 33, 1, 5, 1, 0, '', 1500792577),
(638, 6, 1, 3, 1, 0, '', 1500792578),
(639, 7, 1, 6, 1, 0, '', 1500792579),
(640, 7, 1, 6, 1, 0, '', 1500792593),
(641, 6, 1, 2, 1, 0, '', 1500792596),
(642, 35, 1, 4, 1, 0, '', 1500792597),
(643, 33, 1, 5, 1, 0, '', 1500792602),
(644, 1, 1, 1, 1, 0, '', 1500792604),
(645, 6, 1, 3, 1, 0, '', 1500792607),
(646, 7, 1, 6, 1, 0, '', 1500792607),
(647, 35, 1, 4, 1, 0, '', 1500792620),
(648, 7, 1, 6, 1, 0, '', 1500792621),
(649, 6, 1, 2, 1, 0, '', 1500792632),
(650, 33, 1, 5, 1, 0, '', 1500792632),
(651, 7, 1, 6, 1, 0, '', 1500792635),
(652, 6, 1, 3, 1, 0, '', 1500792636),
(653, 1, 1, 1, 1, 0, '', 1500792639),
(654, 35, 1, 4, 1, 0, '', 1500792643),
(655, 7, 1, 6, 1, 0, '', 1500792649),
(656, 6, 1, 2, 1, 0, '', 1500792650),
(657, 33, 1, 5, 1, 0, '', 1500792652),
(658, 7, 1, 6, 1, 0, '', 1500792663),
(659, 6, 1, 3, 1, 0, '', 1500792665),
(660, 35, 1, 4, 1, 0, '', 1500792666),
(661, 1, 1, 1, 1, 0, '', 1500792674),
(662, 33, 1, 5, 1, 0, '', 1500792677),
(663, 7, 1, 6, 1, 0, '', 1500792677),
(664, 6, 1, 2, 1, 0, '', 1500792677),
(665, 35, 1, 4, 1, 0, '', 1500792689),
(666, 7, 1, 6, 1, 0, '', 1500792691),
(667, 6, 1, 3, 1, 0, '', 1500792694),
(668, 33, 1, 5, 1, 0, '', 1500792702),
(669, 6, 1, 2, 1, 0, '', 1500792704),
(670, 7, 1, 6, 1, 0, '', 1500792705),
(671, 1, 1, 1, 1, 0, '', 1500792709),
(672, 35, 1, 4, 1, 0, '', 1500792712),
(673, 7, 1, 6, 1, 0, '', 1500792719),
(674, 6, 1, 3, 1, 0, '', 1500792723),
(675, 33, 1, 5, 1, 0, '', 1500792727),
(676, 6, 1, 2, 1, 0, '', 1500792731),
(677, 7, 1, 6, 1, 0, '', 1500792733),
(678, 35, 1, 4, 1, 0, '', 1500792735),
(679, 1, 1, 1, 1, 0, '', 1500792744),
(680, 7, 1, 6, 1, 0, '', 1500792747),
(681, 33, 1, 5, 1, 0, '', 1500792752),
(682, 6, 1, 3, 1, 0, '', 1500792752),
(683, 6, 1, 2, 1, 0, '', 1500792758),
(684, 35, 1, 4, 1, 0, '', 1500792758),
(685, 7, 1, 6, 1, 0, '', 1500792761),
(686, 7, 1, 6, 1, 0, '', 1500792775),
(687, 33, 1, 5, 1, 0, '', 1500792777),
(688, 1, 1, 1, 1, 0, '', 1500792779),
(689, 35, 1, 4, 1, 0, '', 1500792781),
(690, 6, 1, 3, 1, 0, '', 1500792781),
(691, 6, 1, 2, 1, 0, '', 1500792785),
(692, 7, 1, 6, 1, 0, '', 1500792789),
(693, 33, 1, 5, 1, 0, '', 1500792802),
(694, 7, 1, 6, 1, 0, '', 1500792803),
(695, 35, 1, 4, 1, 0, '', 1500792804),
(696, 6, 1, 3, 1, 0, '', 1500792810),
(697, 6, 1, 2, 1, 0, '', 1500792812),
(698, 1, 1, 1, 1, 0, '', 1500792814),
(699, 7, 1, 6, 1, 0, '', 1500792817),
(700, 35, 1, 4, 1, 0, '', 1500792827),
(701, 33, 1, 5, 1, 0, '', 1500792827),
(702, 7, 1, 6, 1, 0, '', 1500792831),
(703, 6, 1, 2, 1, 0, '', 1500792839),
(704, 6, 1, 3, 1, 0, '', 1500792839),
(705, 7, 1, 6, 1, 0, '', 1500792845),
(706, 1, 1, 1, 1, 0, '', 1500792849),
(707, 35, 1, 4, 1, 0, '', 1500792850),
(708, 33, 1, 5, 1, 0, '', 1500792852),
(709, 7, 1, 6, 1, 0, '', 1500792859),
(710, 6, 1, 2, 1, 0, '', 1500792866),
(711, 6, 1, 3, 1, 0, '', 1500792868),
(712, 35, 1, 4, 1, 0, '', 1500792873),
(713, 7, 1, 6, 1, 0, '', 1500792873),
(714, 33, 1, 5, 1, 0, '', 1500792877),
(715, 1, 1, 1, 1, 0, '', 1500792884),
(716, 7, 1, 6, 1, 0, '', 1500792887),
(717, 6, 1, 2, 1, 0, '', 1500792893),
(718, 35, 1, 4, 1, 0, '', 1500792896),
(719, 6, 1, 3, 1, 0, '', 1500792897),
(720, 7, 1, 6, 1, 0, '', 1500792901),
(721, 33, 1, 5, 1, 0, '', 1500792902),
(722, 7, 1, 6, 1, 0, '', 1500792915),
(723, 1, 1, 1, 1, 0, '', 1500792919),
(724, 35, 1, 4, 1, 0, '', 1500792919),
(725, 6, 1, 2, 1, 0, '', 1500792920),
(726, 6, 1, 3, 1, 0, '', 1500792932),
(727, 33, 1, 5, 1, 0, '', 1500792932),
(728, 7, 1, 6, 1, 0, '', 1500792932),
(729, 35, 1, 4, 1, 0, '', 1500792942),
(730, 7, 1, 6, 1, 0, '', 1500792943),
(731, 6, 1, 2, 1, 0, '', 1500792947),
(732, 33, 1, 5, 1, 0, '', 1500792952),
(733, 1, 1, 1, 1, 0, '', 1500792954),
(734, 6, 1, 3, 1, 0, '', 1500792955),
(735, 7, 1, 6, 1, 0, '', 1500792957),
(736, 35, 1, 4, 1, 0, '', 1500792965),
(737, 7, 1, 6, 1, 0, '', 1500792971),
(738, 6, 1, 2, 1, 0, '', 1500792974),
(739, 33, 1, 5, 1, 0, '', 1500792977),
(740, 6, 1, 3, 1, 0, '', 1500792984),
(741, 7, 1, 6, 1, 0, '', 1500792985),
(742, 35, 1, 4, 1, 0, '', 1500792988),
(743, 1, 1, 1, 1, 0, '', 1500792989),
(744, 7, 1, 6, 1, 0, '', 1500792999),
(745, 6, 1, 2, 1, 0, '', 1500793001),
(746, 33, 1, 5, 1, 0, '', 1500793002),
(747, 35, 1, 4, 1, 0, '', 1500793011),
(748, 7, 1, 6, 1, 0, '', 1500793013),
(749, 6, 1, 3, 1, 0, '', 1500793013),
(750, 1, 1, 1, 1, 0, '', 1500793024),
(751, 33, 1, 5, 1, 0, '', 1500793027),
(752, 7, 1, 6, 1, 0, '', 1500793027),
(753, 6, 1, 2, 1, 0, '', 1500793028),
(754, 35, 1, 4, 1, 0, '', 1500793034),
(755, 7, 1, 6, 1, 0, '', 1500793041),
(756, 6, 1, 3, 1, 0, '', 1500793042),
(757, 33, 1, 5, 1, 0, '', 1500793052),
(758, 7, 1, 6, 1, 0, '', 1500793055),
(759, 6, 1, 2, 1, 0, '', 1500793055),
(760, 35, 1, 4, 1, 0, '', 1500793057),
(761, 1, 1, 1, 1, 0, '', 1500793059),
(762, 7, 1, 6, 1, 0, '', 1500793069),
(763, 6, 1, 3, 1, 0, '', 1500793071),
(764, 33, 1, 5, 1, 0, '', 1500793077),
(765, 35, 1, 4, 1, 0, '', 1500793080),
(766, 6, 1, 2, 1, 0, '', 1500793082),
(767, 7, 1, 6, 1, 0, '', 1500793083),
(768, 1, 1, 1, 1, 0, '', 1500793094),
(769, 7, 1, 6, 1, 0, '', 1500793097),
(770, 6, 1, 3, 1, 0, '', 1500793100),
(771, 33, 1, 5, 1, 0, '', 1500793102),
(772, 35, 1, 4, 1, 0, '', 1500793103),
(773, 6, 1, 2, 1, 0, '', 1500793109),
(774, 7, 1, 6, 1, 0, '', 1500793111),
(775, 7, 1, 6, 1, 0, '', 1500793125),
(776, 35, 1, 4, 1, 0, '', 1500793126),
(777, 33, 1, 5, 1, 0, '', 1500793127),
(778, 1, 1, 1, 1, 0, '', 1500793129),
(779, 6, 1, 3, 1, 0, '', 1500793129),
(780, 6, 1, 2, 1, 0, '', 1500793136),
(781, 7, 1, 6, 1, 0, '', 1500793139),
(782, 35, 1, 4, 1, 0, '', 1500793149),
(783, 33, 1, 5, 1, 0, '', 1500793152),
(784, 7, 1, 6, 1, 0, '', 1500793153),
(785, 6, 1, 3, 1, 0, '', 1500793158),
(786, 6, 1, 2, 1, 0, '', 1500793163),
(787, 1, 1, 1, 1, 0, '', 1500793164),
(788, 7, 1, 6, 1, 0, '', 1500793167),
(789, 35, 1, 4, 1, 0, '', 1500793172),
(790, 33, 1, 5, 1, 0, '', 1500793177),
(791, 7, 1, 6, 1, 0, '', 1500793181),
(792, 6, 1, 3, 1, 0, '', 1500793187),
(793, 6, 1, 2, 1, 0, '', 1500793190),
(794, 35, 1, 4, 1, 0, '', 1500793195),
(795, 7, 1, 6, 1, 0, '', 1500793195),
(796, 1, 1, 1, 1, 0, '', 1500793199),
(797, 33, 1, 5, 1, 0, '', 1500793202),
(798, 7, 1, 6, 1, 0, '', 1500793209),
(799, 6, 1, 3, 1, 0, '', 1500793216),
(800, 6, 1, 2, 1, 0, '', 1500793217),
(801, 35, 1, 4, 1, 0, '', 1500793218),
(802, 7, 1, 6, 1, 0, '', 1500793232),
(803, 33, 1, 5, 1, 0, '', 1500793232),
(804, 1, 1, 1, 1, 0, '', 1500793234),
(805, 7, 1, 6, 1, 0, '', 1500793237),
(806, 35, 1, 4, 1, 0, '', 1500793241),
(807, 6, 1, 2, 1, 0, '', 1500793244),
(808, 6, 1, 3, 1, 0, '', 1500793245),
(809, 7, 1, 6, 1, 0, '', 1500793251),
(810, 33, 1, 5, 1, 0, '', 1500793252),
(811, 35, 1, 4, 1, 0, '', 1500793264),
(812, 7, 1, 6, 1, 0, '', 1500793265),
(813, 1, 1, 1, 1, 0, '', 1500793269),
(814, 6, 1, 2, 1, 0, '', 1500793271),
(815, 6, 1, 3, 1, 0, '', 1500793274),
(816, 33, 1, 5, 1, 0, '', 1500793277),
(817, 7, 1, 6, 1, 0, '', 1500793279),
(818, 35, 1, 4, 1, 0, '', 1500793287),
(819, 7, 1, 6, 1, 0, '', 1500793293),
(820, 6, 1, 2, 1, 0, '', 1500793298),
(821, 33, 1, 5, 1, 0, '', 1500793302),
(822, 6, 1, 3, 1, 0, '', 1500793303),
(823, 1, 1, 1, 1, 0, '', 1500793304),
(824, 7, 1, 6, 1, 0, '', 1500793307),
(825, 35, 1, 4, 1, 0, '', 1500793310),
(826, 7, 1, 6, 1, 0, '', 1500793321),
(827, 6, 1, 2, 1, 0, '', 1500793325),
(828, 33, 1, 5, 1, 0, '', 1500793327),
(829, 6, 1, 3, 1, 0, '', 1500793332),
(830, 35, 1, 4, 1, 0, '', 1500793333),
(831, 7, 1, 6, 1, 0, '', 1500793335),
(832, 1, 1, 1, 1, 0, '', 1500793339),
(833, 7, 1, 6, 1, 0, '', 1500793349),
(834, 33, 1, 5, 1, 0, '', 1500793352),
(835, 6, 1, 2, 1, 0, '', 1500793352),
(836, 35, 1, 4, 1, 0, '', 1500793356),
(837, 6, 1, 3, 1, 0, '', 1500793361),
(838, 7, 1, 6, 1, 0, '', 1500793363),
(839, 1, 1, 1, 1, 0, '', 1500793374),
(840, 33, 1, 5, 1, 0, '', 1500793377),
(841, 7, 1, 6, 1, 0, '', 1500793377),
(842, 35, 1, 4, 1, 0, '', 1500793379),
(843, 6, 1, 2, 1, 0, '', 1500793379),
(844, 6, 1, 3, 1, 0, '', 1500793390),
(845, 7, 1, 6, 1, 0, '', 1500793391),
(846, 33, 1, 5, 1, 0, '', 1500793402),
(847, 35, 1, 4, 1, 0, '', 1500793402),
(848, 7, 1, 6, 1, 0, '', 1500793405),
(849, 6, 1, 2, 1, 0, '', 1500793406),
(850, 1, 1, 1, 1, 0, '', 1500793409),
(851, 6, 1, 3, 1, 0, '', 1500793419),
(852, 7, 1, 6, 1, 0, '', 1500793419),
(853, 35, 1, 4, 1, 0, '', 1500793425),
(854, 33, 1, 5, 1, 0, '', 1500793427),
(855, 6, 1, 2, 1, 0, '', 1500793433),
(856, 7, 1, 6, 1, 0, '', 1500793433),
(857, 1, 1, 1, 1, 0, '', 1500793444),
(858, 7, 1, 6, 1, 0, '', 1500793447),
(859, 35, 1, 4, 1, 0, '', 1500793448),
(860, 6, 1, 3, 1, 0, '', 1500793448),
(861, 33, 1, 5, 1, 0, '', 1500793452),
(862, 6, 1, 2, 1, 0, '', 1500793460),
(863, 7, 1, 6, 1, 0, '', 1500793461),
(864, 35, 1, 4, 1, 0, '', 1500793471),
(865, 7, 1, 6, 1, 0, '', 1500793475),
(866, 33, 1, 5, 1, 0, '', 1500793477),
(867, 6, 1, 3, 1, 0, '', 1500793477),
(868, 1, 1, 1, 1, 0, '', 1500793479),
(869, 6, 1, 2, 1, 0, '', 1500793487),
(870, 7, 1, 6, 1, 0, '', 1500793489),
(871, 35, 1, 4, 1, 0, '', 1500793494),
(872, 33, 1, 5, 1, 0, '', 1500793502),
(873, 7, 1, 6, 1, 0, '', 1500793503),
(874, 6, 1, 3, 1, 0, '', 1500793506),
(875, 1, 1, 1, 1, 0, '', 1500793514),
(876, 6, 1, 2, 1, 0, '', 1500793514),
(877, 35, 1, 4, 1, 0, '', 1500793517),
(878, 7, 1, 6, 1, 0, '', 1500793517),
(879, 33, 1, 5, 1, 0, '', 1500793532),
(880, 7, 1, 6, 1, 0, '', 1500793532),
(881, 6, 1, 3, 1, 0, '', 1500793535),
(882, 35, 1, 4, 1, 0, '', 1500793540),
(883, 6, 1, 2, 1, 0, '', 1500793541),
(884, 7, 1, 6, 1, 0, '', 1500793545),
(885, 1, 1, 1, 1, 0, '', 1500793549),
(886, 33, 1, 5, 1, 0, '', 1500793552),
(887, 7, 1, 6, 1, 0, '', 1500793559),
(888, 35, 1, 4, 1, 0, '', 1500793563),
(889, 6, 1, 3, 1, 0, '', 1500793564),
(890, 6, 1, 2, 1, 0, '', 1500793568),
(891, 7, 1, 6, 1, 0, '', 1500793573),
(892, 33, 1, 5, 1, 0, '', 1500793577),
(893, 1, 1, 1, 1, 0, '', 1500793584),
(894, 35, 1, 4, 1, 0, '', 1500793586),
(895, 7, 1, 6, 1, 0, '', 1500793587),
(896, 6, 1, 3, 1, 0, '', 1500793593),
(897, 6, 1, 2, 1, 0, '', 1500793595),
(898, 7, 1, 6, 1, 0, '', 1500793601),
(899, 33, 1, 5, 1, 0, '', 1500793602),
(900, 35, 1, 4, 1, 0, '', 1500793609),
(901, 7, 1, 6, 1, 0, '', 1500793615),
(902, 1, 1, 1, 1, 0, '', 1500793619),
(903, 6, 1, 2, 1, 0, '', 1500793622),
(904, 6, 1, 3, 1, 0, '', 1500793622),
(905, 33, 1, 5, 1, 0, '', 1500793627),
(906, 7, 1, 6, 1, 0, '', 1500793629),
(907, 35, 1, 4, 1, 0, '', 1500793632),
(908, 7, 1, 6, 1, 0, '', 1500793643),
(909, 6, 1, 2, 1, 0, '', 1500793649),
(910, 6, 1, 3, 1, 0, '', 1500793651),
(911, 33, 1, 5, 1, 0, '', 1500793652),
(912, 1, 1, 1, 1, 0, '', 1500793654),
(913, 35, 1, 4, 1, 0, '', 1500793655),
(914, 7, 1, 6, 1, 0, '', 1500793657),
(915, 7, 1, 6, 1, 0, '', 1500793671),
(916, 6, 1, 2, 1, 0, '', 1500793676),
(917, 33, 1, 5, 1, 0, '', 1500793677),
(918, 35, 1, 4, 1, 0, '', 1500793678),
(919, 6, 1, 3, 1, 0, '', 1500793680),
(920, 7, 1, 6, 1, 0, '', 1500793685),
(921, 1, 1, 1, 1, 0, '', 1500793689),
(922, 7, 1, 6, 1, 0, '', 1500793699),
(923, 35, 1, 4, 1, 0, '', 1500793701),
(924, 33, 1, 5, 1, 0, '', 1500793702),
(925, 6, 1, 2, 1, 0, '', 1500793703),
(926, 6, 1, 3, 1, 0, '', 1500793709),
(927, 7, 1, 6, 1, 0, '', 1500793713),
(928, 1, 1, 1, 1, 0, '', 1500793724),
(929, 35, 1, 4, 1, 0, '', 1500793724),
(930, 33, 1, 5, 1, 0, '', 1500793727),
(931, 7, 1, 6, 1, 0, '', 1500793727),
(932, 6, 1, 2, 1, 0, '', 1500793730),
(933, 6, 1, 3, 1, 0, '', 1500793738),
(934, 7, 1, 6, 1, 0, '', 1500793741),
(935, 35, 1, 4, 1, 0, '', 1500793747),
(936, 33, 1, 5, 1, 0, '', 1500793752),
(937, 7, 1, 6, 1, 0, '', 1500793755),
(938, 6, 1, 2, 1, 0, '', 1500793757),
(939, 1, 1, 1, 1, 0, '', 1500793759),
(940, 6, 1, 3, 1, 0, '', 1500793767),
(941, 7, 1, 6, 1, 0, '', 1500793769),
(942, 35, 1, 4, 1, 0, '', 1500793770),
(943, 33, 1, 5, 1, 0, '', 1500793777),
(944, 7, 1, 6, 1, 0, '', 1500793783),
(945, 6, 1, 2, 1, 0, '', 1500793784),
(946, 35, 1, 4, 1, 0, '', 1500793793),
(947, 1, 1, 1, 1, 0, '', 1500793794),
(948, 6, 1, 3, 1, 0, '', 1500793796),
(949, 7, 1, 6, 1, 0, '', 1500793797),
(950, 33, 1, 5, 1, 0, '', 1500793802),
(951, 6, 1, 2, 1, 0, '', 1500793811),
(952, 7, 1, 6, 1, 0, '', 1500793811),
(953, 35, 1, 4, 1, 0, '', 1500793816),
(954, 6, 1, 3, 1, 0, '', 1500793832),
(955, 7, 1, 6, 1, 0, '', 1500793832),
(956, 33, 1, 5, 1, 0, '', 1500793832),
(957, 1, 1, 1, 1, 0, '', 1500793832),
(958, 6, 1, 2, 1, 0, '', 1500793838),
(959, 35, 1, 4, 1, 0, '', 1500793839),
(960, 7, 1, 6, 1, 0, '', 1500793839),
(961, 33, 1, 5, 1, 0, '', 1500793852),
(962, 7, 1, 6, 1, 0, '', 1500793853),
(963, 6, 1, 3, 1, 0, '', 1500793854),
(964, 35, 1, 4, 1, 0, '', 1500793862),
(965, 1, 1, 1, 1, 0, '', 1500793864),
(966, 6, 1, 2, 1, 0, '', 1500793865),
(967, 7, 1, 6, 1, 0, '', 1500793867),
(968, 33, 1, 5, 1, 0, '', 1500793877),
(969, 7, 1, 6, 1, 0, '', 1500793881),
(970, 6, 1, 3, 1, 0, '', 1500793883),
(971, 35, 1, 4, 1, 0, '', 1500793885),
(972, 6, 1, 2, 1, 0, '', 1500793892),
(973, 7, 1, 6, 1, 0, '', 1500793895),
(974, 1, 1, 1, 1, 0, '', 1500793899),
(975, 33, 1, 5, 1, 0, '', 1500793902),
(976, 35, 1, 4, 1, 0, '', 1500793908),
(977, 7, 1, 6, 1, 0, '', 1500793909),
(978, 6, 1, 3, 1, 0, '', 1500793912),
(979, 6, 1, 2, 1, 0, '', 1500793919),
(980, 7, 1, 6, 1, 0, '', 1500793923),
(981, 33, 1, 5, 1, 0, '', 1500793927),
(982, 35, 1, 4, 1, 0, '', 1500793931),
(983, 1, 1, 1, 1, 0, '', 1500793934),
(984, 7, 1, 6, 1, 0, '', 1500793937),
(985, 6, 1, 3, 1, 0, '', 1500793941),
(986, 6, 1, 2, 1, 0, '', 1500793946),
(987, 7, 1, 6, 1, 0, '', 1500793951),
(988, 33, 1, 5, 1, 0, '', 1500793952),
(989, 35, 1, 4, 1, 0, '', 1500793954),
(990, 7, 1, 6, 1, 0, '', 1500793965),
(991, 1, 1, 1, 1, 0, '', 1500793969),
(992, 6, 1, 3, 1, 0, '', 1500793970),
(993, 6, 1, 2, 1, 0, '', 1500793973),
(994, 33, 1, 5, 1, 0, '', 1500793977),
(995, 35, 1, 4, 1, 0, '', 1500793977),
(996, 7, 1, 6, 1, 0, '', 1500793979),
(997, 7, 1, 6, 1, 0, '', 1500793993),
(998, 6, 1, 3, 1, 0, '', 1500793999),
(999, 6, 1, 2, 1, 0, '', 1500794000),
(1000, 35, 1, 4, 1, 0, '', 1500794000),
(1001, 33, 1, 5, 1, 0, '', 1500794002),
(1002, 1, 1, 1, 1, 0, '', 1500794004),
(1003, 7, 1, 6, 1, 0, '', 1500794007),
(1004, 7, 1, 6, 1, 0, '', 1500794021),
(1005, 35, 1, 4, 1, 0, '', 1500794023),
(1006, 33, 1, 5, 1, 0, '', 1500794027),
(1007, 6, 1, 2, 1, 0, '', 1500794027),
(1008, 6, 1, 3, 1, 0, '', 1500794028),
(1009, 7, 1, 6, 1, 0, '', 1500794035),
(1010, 1, 1, 1, 1, 0, '', 1500794039),
(1011, 35, 1, 4, 1, 0, '', 1500794046),
(1012, 7, 1, 6, 1, 0, '', 1500794049),
(1013, 33, 1, 5, 1, 0, '', 1500794052),
(1014, 6, 1, 2, 1, 0, '', 1500794054),
(1015, 6, 1, 3, 1, 0, '', 1500794057),
(1016, 7, 1, 6, 1, 0, '', 1500794063),
(1017, 35, 1, 4, 1, 0, '', 1500794069),
(1018, 1, 1, 1, 1, 0, '', 1500794074),
(1019, 33, 1, 5, 1, 0, '', 1500794077),
(1020, 7, 1, 6, 1, 0, '', 1500794077),
(1021, 6, 1, 2, 1, 0, '', 1500794081),
(1022, 6, 1, 3, 1, 0, '', 1500794086),
(1023, 7, 1, 6, 1, 0, '', 1500794091),
(1024, 35, 1, 4, 1, 0, '', 1500794092),
(1025, 33, 1, 5, 1, 0, '', 1500794102),
(1026, 7, 1, 6, 1, 0, '', 1500794105),
(1027, 6, 1, 2, 1, 0, '', 1500794108),
(1028, 1, 1, 1, 1, 0, '', 1500794109),
(1029, 35, 1, 4, 1, 0, '', 1500794115),
(1030, 6, 1, 3, 1, 0, '', 1500794115),
(1031, 7, 1, 6, 1, 0, '', 1500794119),
(1032, 33, 1, 5, 1, 0, '', 1500794132),
(1033, 7, 1, 6, 1, 0, '', 1500794133),
(1034, 6, 1, 2, 1, 0, '', 1500794135),
(1035, 35, 1, 4, 1, 0, '', 1500794138),
(1036, 1, 1, 1, 1, 0, '', 1500794144),
(1037, 6, 1, 3, 1, 0, '', 1500794144),
(1038, 7, 1, 6, 1, 0, '', 1500794147),
(1039, 33, 1, 5, 1, 0, '', 1500794152),
(1040, 35, 1, 4, 1, 0, '', 1500794161),
(1041, 7, 1, 6, 1, 0, '', 1500794161),
(1042, 6, 1, 2, 1, 0, '', 1500794162),
(1043, 6, 1, 3, 1, 0, '', 1500794173),
(1044, 7, 1, 6, 1, 0, '', 1500794175),
(1045, 33, 1, 5, 1, 0, '', 1500794177),
(1046, 1, 1, 1, 1, 0, '', 1500794179),
(1047, 35, 1, 4, 1, 0, '', 1500794184),
(1048, 7, 1, 6, 1, 0, '', 1500794189),
(1049, 6, 1, 2, 1, 0, '', 1500794189),
(1050, 33, 1, 5, 1, 0, '', 1500794202),
(1051, 6, 1, 3, 1, 0, '', 1500794202),
(1052, 7, 1, 6, 1, 0, '', 1500794203),
(1053, 35, 1, 4, 1, 0, '', 1500794207),
(1054, 1, 1, 1, 1, 0, '', 1500794214),
(1055, 6, 1, 2, 1, 0, '', 1500794216),
(1056, 7, 1, 6, 1, 0, '', 1500794217),
(1057, 33, 1, 5, 1, 0, '', 1500794227),
(1058, 35, 1, 4, 1, 0, '', 1500794230),
(1059, 6, 1, 3, 1, 0, '', 1500794231),
(1060, 7, 1, 6, 1, 0, '', 1500794231),
(1061, 6, 1, 2, 1, 0, '', 1500794243),
(1062, 7, 1, 6, 1, 0, '', 1500794245),
(1063, 1, 1, 1, 1, 0, '', 1500794249),
(1064, 33, 1, 5, 1, 0, '', 1500794252),
(1065, 35, 1, 4, 1, 0, '', 1500794253),
(1066, 7, 1, 6, 1, 0, '', 1500794259),
(1067, 6, 1, 3, 1, 0, '', 1500794260),
(1068, 6, 1, 2, 1, 0, '', 1500794270),
(1069, 7, 1, 6, 1, 0, '', 1500794273),
(1070, 35, 1, 4, 1, 0, '', 1500794276),
(1071, 33, 1, 5, 1, 0, '', 1500794277),
(1072, 1, 1, 1, 1, 0, '', 1500794284),
(1073, 7, 1, 6, 1, 0, '', 1500794287),
(1074, 6, 1, 3, 1, 0, '', 1500794289),
(1075, 6, 1, 2, 1, 0, '', 1500794297),
(1076, 35, 1, 4, 1, 0, '', 1500794299),
(1077, 7, 1, 6, 1, 0, '', 1500794301),
(1078, 33, 1, 5, 1, 0, '', 1500794302),
(1079, 7, 1, 6, 1, 0, '', 1500794315),
(1080, 6, 1, 3, 1, 0, '', 1500794318),
(1081, 1, 1, 1, 1, 0, '', 1500794319),
(1082, 35, 1, 4, 1, 0, '', 1500794322),
(1083, 6, 1, 2, 1, 0, '', 1500794324),
(1084, 33, 1, 5, 1, 0, '', 1500794327),
(1085, 7, 1, 6, 1, 0, '', 1500794329),
(1086, 7, 1, 6, 1, 0, '', 1500794343),
(1087, 35, 1, 4, 1, 0, '', 1500794345),
(1088, 6, 1, 3, 1, 0, '', 1500794347),
(1089, 6, 1, 2, 1, 0, '', 1500794351),
(1090, 33, 1, 5, 1, 0, '', 1500794352),
(1091, 1, 1, 1, 1, 0, '', 1500794354),
(1092, 7, 1, 6, 1, 0, '', 1500794357),
(1093, 35, 1, 4, 1, 0, '', 1500794368),
(1094, 7, 1, 6, 1, 0, '', 1500794371),
(1095, 6, 1, 3, 1, 0, '', 1500794376),
(1096, 33, 1, 5, 1, 0, '', 1500794377),
(1097, 6, 1, 2, 1, 0, '', 1500794378),
(1098, 7, 1, 6, 1, 0, '', 1500794385),
(1099, 1, 1, 1, 1, 0, '', 1500794389),
(1100, 35, 1, 4, 1, 0, '', 1500794391),
(1101, 7, 1, 6, 1, 0, '', 1500794399),
(1102, 33, 1, 5, 1, 0, '', 1500794402),
(1103, 6, 1, 2, 1, 0, '', 1500794405),
(1104, 6, 1, 3, 1, 0, '', 1500794405),
(1105, 7, 1, 6, 1, 0, '', 1500794413),
(1106, 35, 1, 4, 1, 0, '', 1500794414),
(1107, 1, 1, 1, 1, 0, '', 1500794432),
(1108, 7, 1, 6, 1, 0, '', 1500794432),
(1109, 33, 1, 5, 1, 0, '', 1500794432),
(1110, 6, 1, 2, 1, 0, '', 1500794432),
(1111, 6, 1, 3, 1, 0, '', 1500794434),
(1112, 35, 1, 4, 1, 0, '', 1500794437),
(1113, 7, 1, 6, 1, 0, '', 1500794441),
(1114, 33, 1, 5, 1, 0, '', 1500794452),
(1115, 7, 1, 6, 1, 0, '', 1500794455),
(1116, 1, 1, 1, 1, 0, '', 1500794459),
(1117, 6, 1, 2, 1, 0, '', 1500794459),
(1118, 35, 1, 4, 1, 0, '', 1500794460),
(1119, 6, 1, 3, 1, 0, '', 1500794463),
(1120, 7, 1, 6, 1, 0, '', 1500794469),
(1121, 33, 1, 5, 1, 0, '', 1500794477),
(1122, 35, 1, 4, 1, 0, '', 1500794483),
(1123, 7, 1, 6, 1, 0, '', 1500794483),
(1124, 6, 1, 2, 1, 0, '', 1500794486),
(1125, 6, 1, 3, 1, 0, '', 1500794492),
(1126, 1, 1, 1, 1, 0, '', 1500794494),
(1127, 7, 1, 6, 1, 0, '', 1500794497),
(1128, 33, 1, 5, 1, 0, '', 1500794502),
(1129, 35, 1, 4, 1, 0, '', 1500794506),
(1130, 7, 1, 6, 1, 0, '', 1500794511),
(1131, 6, 1, 2, 1, 0, '', 1500794513),
(1132, 6, 1, 3, 1, 0, '', 1500794521),
(1133, 7, 1, 6, 1, 0, '', 1500794525),
(1134, 33, 1, 5, 1, 0, '', 1500794527),
(1135, 1, 1, 1, 1, 0, '', 1500794529),
(1136, 35, 1, 4, 1, 0, '', 1500794529),
(1137, 7, 1, 6, 1, 0, '', 1500794539),
(1138, 6, 1, 2, 1, 0, '', 1500794540),
(1139, 6, 1, 3, 1, 0, '', 1500794550),
(1140, 35, 1, 4, 1, 0, '', 1500794552),
(1141, 33, 1, 5, 1, 0, '', 1500794552),
(1142, 7, 1, 6, 1, 0, '', 1500794553),
(1143, 1, 1, 1, 1, 0, '', 1500794564),
(1144, 6, 1, 2, 1, 0, '', 1500794567),
(1145, 7, 1, 6, 1, 0, '', 1500794567),
(1146, 35, 1, 4, 1, 0, '', 1500794575),
(1147, 33, 1, 5, 1, 0, '', 1500794577),
(1148, 6, 1, 3, 1, 0, '', 1500794579),
(1149, 7, 1, 6, 1, 0, '', 1500794581),
(1150, 6, 1, 2, 1, 0, '', 1500794594),
(1151, 7, 1, 6, 1, 0, '', 1500794595),
(1152, 35, 1, 4, 1, 0, '', 1500794598),
(1153, 1, 1, 1, 1, 0, '', 1500794599),
(1154, 33, 1, 5, 1, 0, '', 1500794602),
(1155, 6, 1, 3, 1, 0, '', 1500794608),
(1156, 7, 1, 6, 1, 0, '', 1500794609),
(1157, 35, 1, 4, 1, 0, '', 1500794621),
(1158, 6, 1, 2, 1, 0, '', 1500794621),
(1159, 7, 1, 6, 1, 0, '', 1500794623),
(1160, 33, 1, 5, 1, 0, '', 1500794627),
(1161, 1, 1, 1, 1, 0, '', 1500794634),
(1162, 6, 1, 3, 1, 0, '', 1500794637),
(1163, 7, 1, 6, 1, 0, '', 1500794637),
(1164, 35, 1, 4, 1, 0, '', 1500794644),
(1165, 6, 1, 2, 1, 0, '', 1500794648),
(1166, 7, 1, 6, 1, 0, '', 1500794651),
(1167, 33, 1, 5, 1, 0, '', 1500794652),
(1168, 7, 1, 6, 1, 0, '', 1500794665),
(1169, 6, 1, 3, 1, 0, '', 1500794666),
(1170, 35, 1, 4, 1, 0, '', 1500794667),
(1171, 1, 1, 1, 1, 0, '', 1500794669),
(1172, 6, 1, 2, 1, 0, '', 1500794675),
(1173, 33, 1, 5, 1, 0, '', 1500794677),
(1174, 7, 1, 6, 1, 0, '', 1500794679),
(1175, 35, 1, 4, 1, 0, '', 1500794690),
(1176, 7, 1, 6, 1, 0, '', 1500794693),
(1177, 6, 1, 3, 1, 0, '', 1500794695),
(1178, 6, 1, 2, 1, 0, '', 1500794702),
(1179, 33, 1, 5, 1, 0, '', 1500794702),
(1180, 1, 1, 1, 1, 0, '', 1500794704),
(1181, 7, 1, 6, 1, 0, '', 1500794707),
(1182, 35, 1, 4, 1, 0, '', 1500794713),
(1183, 7, 1, 6, 1, 0, '', 1500794721),
(1184, 6, 1, 3, 1, 0, '', 1500794732),
(1185, 33, 1, 5, 1, 0, '', 1500794732),
(1186, 6, 1, 2, 1, 0, '', 1500794732),
(1187, 7, 1, 6, 1, 0, '', 1500794735),
(1188, 35, 1, 4, 1, 0, '', 1500794736),
(1189, 1, 1, 1, 1, 0, '', 1500794739),
(1190, 7, 1, 6, 1, 0, '', 1500794749),
(1191, 33, 1, 5, 1, 0, '', 1500794752),
(1192, 6, 1, 3, 1, 0, '', 1500794753),
(1193, 6, 1, 2, 1, 0, '', 1500794756),
(1194, 35, 1, 4, 1, 0, '', 1500794759),
(1195, 7, 1, 6, 1, 0, '', 1500794763),
(1196, 1, 1, 1, 1, 0, '', 1500794774),
(1197, 33, 1, 5, 1, 0, '', 1500794777),
(1198, 7, 1, 6, 1, 0, '', 1500794777),
(1199, 35, 1, 4, 1, 0, '', 1500794782),
(1200, 6, 1, 3, 1, 0, '', 1500794782),
(1201, 6, 1, 2, 1, 0, '', 1500794783),
(1202, 7, 1, 6, 1, 0, '', 1500794791),
(1203, 33, 1, 5, 1, 0, '', 1500794802),
(1204, 35, 1, 4, 1, 0, '', 1500794805),
(1205, 7, 1, 6, 1, 0, '', 1500794805),
(1206, 1, 1, 1, 1, 0, '', 1500794809),
(1207, 6, 1, 2, 1, 0, '', 1500794810),
(1208, 6, 1, 3, 1, 0, '', 1500794811),
(1209, 7, 1, 6, 1, 0, '', 1500794819),
(1210, 33, 1, 5, 1, 0, '', 1500794827),
(1211, 35, 1, 4, 1, 0, '', 1500794828),
(1212, 7, 1, 6, 1, 0, '', 1500794833),
(1213, 6, 1, 2, 1, 0, '', 1500794837),
(1214, 6, 1, 3, 1, 0, '', 1500794840),
(1215, 1, 1, 1, 1, 0, '', 1500794844),
(1216, 7, 1, 6, 1, 0, '', 1500794847),
(1217, 35, 1, 4, 1, 0, '', 1500794851),
(1218, 33, 1, 5, 1, 0, '', 1500794852),
(1219, 7, 1, 6, 1, 0, '', 1500794861),
(1220, 6, 1, 2, 1, 0, '', 1500794864),
(1221, 6, 1, 3, 1, 0, '', 1500794869),
(1222, 35, 1, 4, 1, 0, '', 1500794874),
(1223, 7, 1, 6, 1, 0, '', 1500794875),
(1224, 33, 1, 5, 1, 0, '', 1500794877),
(1225, 1, 1, 1, 1, 0, '', 1500794879),
(1226, 7, 1, 6, 1, 0, '', 1500794889),
(1227, 6, 1, 2, 1, 0, '', 1500794891),
(1228, 35, 1, 4, 1, 0, '', 1500794897),
(1229, 6, 1, 3, 1, 0, '', 1500794898),
(1230, 33, 1, 5, 1, 0, '', 1500794902),
(1231, 7, 1, 6, 1, 0, '', 1500794903),
(1232, 1, 1, 1, 1, 0, '', 1500794914),
(1233, 7, 1, 6, 1, 0, '', 1500794917),
(1234, 6, 1, 2, 1, 0, '', 1500794918),
(1235, 35, 1, 4, 1, 0, '', 1500794920),
(1236, 33, 1, 5, 1, 0, '', 1500794927),
(1237, 6, 1, 3, 1, 0, '', 1500794927),
(1238, 7, 1, 6, 1, 0, '', 1500794931),
(1239, 35, 1, 4, 1, 0, '', 1500794943),
(1240, 7, 1, 6, 1, 0, '', 1500794945),
(1241, 6, 1, 2, 1, 0, '', 1500794945),
(1242, 1, 1, 1, 1, 0, '', 1500794949),
(1243, 33, 1, 5, 1, 0, '', 1500794952),
(1244, 6, 1, 3, 1, 0, '', 1500794956),
(1245, 7, 1, 6, 1, 0, '', 1500794959),
(1246, 35, 1, 4, 1, 0, '', 1500794966),
(1247, 6, 1, 2, 1, 0, '', 1500794972),
(1248, 7, 1, 6, 1, 0, '', 1500794973),
(1249, 33, 1, 5, 1, 0, '', 1500794977),
(1250, 1, 1, 1, 1, 0, '', 1500794984),
(1251, 6, 1, 3, 1, 0, '', 1500794985),
(1252, 7, 1, 6, 1, 0, '', 1500794987),
(1253, 35, 1, 4, 1, 0, '', 1500794989),
(1254, 6, 1, 2, 1, 0, '', 1500794999),
(1255, 7, 1, 6, 1, 0, '', 1500795001),
(1256, 33, 1, 5, 1, 0, '', 1500795002),
(1257, 35, 1, 4, 1, 0, '', 1500795012),
(1258, 6, 1, 3, 1, 0, '', 1500795014),
(1259, 7, 1, 6, 1, 0, '', 1500795015),
(1260, 1, 1, 1, 1, 0, '', 1500795019),
(1261, 6, 1, 2, 1, 0, '', 1500795032),
(1262, 33, 1, 5, 1, 0, '', 1500795032),
(1263, 7, 1, 6, 1, 0, '', 1500795032),
(1264, 35, 1, 4, 1, 0, '', 1500795035),
(1265, 6, 1, 3, 1, 0, '', 1500795043),
(1266, 7, 1, 6, 1, 0, '', 1500795043),
(1267, 33, 1, 5, 1, 0, '', 1500795052),
(1268, 6, 1, 2, 1, 0, '', 1500795053),
(1269, 1, 1, 1, 1, 0, '', 1500795054),
(1270, 7, 1, 6, 1, 0, '', 1500795057),
(1271, 35, 1, 4, 1, 0, '', 1500795058),
(1272, 7, 1, 6, 1, 0, '', 1500795071),
(1273, 6, 1, 3, 1, 0, '', 1500795072),
(1274, 33, 1, 5, 1, 0, '', 1500795077),
(1275, 6, 1, 2, 1, 0, '', 1500795080),
(1276, 35, 1, 4, 1, 0, '', 1500795081),
(1277, 7, 1, 6, 1, 0, '', 1500795085),
(1278, 1, 1, 1, 1, 0, '', 1500795089),
(1279, 7, 1, 6, 1, 0, '', 1500795099),
(1280, 6, 1, 3, 1, 0, '', 1500795101),
(1281, 33, 1, 5, 1, 0, '', 1500795102),
(1282, 35, 1, 4, 1, 0, '', 1500795104),
(1283, 6, 1, 2, 1, 0, '', 1500795107),
(1284, 7, 1, 6, 1, 0, '', 1500795113),
(1285, 1, 1, 1, 1, 0, '', 1500795124),
(1286, 35, 1, 4, 1, 0, '', 1500795127),
(1287, 33, 1, 5, 1, 0, '', 1500795127),
(1288, 7, 1, 6, 1, 0, '', 1500795127),
(1289, 6, 1, 3, 1, 0, '', 1500795130),
(1290, 6, 1, 2, 1, 0, '', 1500795134),
(1291, 7, 1, 6, 1, 0, '', 1500795141),
(1292, 35, 1, 4, 1, 0, '', 1500795150),
(1293, 33, 1, 5, 1, 0, '', 1500795152),
(1294, 7, 1, 6, 1, 0, '', 1500795155),
(1295, 1, 1, 1, 1, 0, '', 1500795159),
(1296, 6, 1, 3, 1, 0, '', 1500795159),
(1297, 6, 1, 2, 1, 0, '', 1500795161),
(1298, 7, 1, 6, 1, 0, '', 1500795169),
(1299, 35, 1, 4, 1, 0, '', 1500795173),
(1300, 33, 1, 5, 1, 0, '', 1500795177),
(1301, 7, 1, 6, 1, 0, '', 1500795183),
(1302, 6, 1, 3, 1, 0, '', 1500795188),
(1303, 6, 1, 2, 1, 0, '', 1500795188),
(1304, 1, 1, 1, 1, 0, '', 1500795194),
(1305, 35, 1, 4, 1, 0, '', 1500795196),
(1306, 7, 1, 6, 1, 0, '', 1500795197),
(1307, 33, 1, 5, 1, 0, '', 1500795202),
(1308, 7, 1, 6, 1, 0, '', 1500795211),
(1309, 6, 1, 2, 1, 0, '', 1500795215),
(1310, 6, 1, 3, 1, 0, '', 1500795217),
(1311, 35, 1, 4, 1, 0, '', 1500795219),
(1312, 7, 1, 6, 1, 0, '', 1500795225),
(1313, 33, 1, 5, 1, 0, '', 1500795227),
(1314, 1, 1, 1, 1, 0, '', 1500795229),
(1315, 7, 1, 6, 1, 0, '', 1500795239),
(1316, 6, 1, 2, 1, 0, '', 1500795242),
(1317, 35, 1, 4, 1, 0, '', 1500795242),
(1318, 6, 1, 3, 1, 0, '', 1500795246),
(1319, 33, 1, 5, 1, 0, '', 1500795252),
(1320, 7, 1, 6, 1, 0, '', 1500795253),
(1321, 1, 1, 1, 1, 0, '', 1500795264),
(1322, 35, 1, 4, 1, 0, '', 1500795265),
(1323, 7, 1, 6, 1, 0, '', 1500795267),
(1324, 6, 1, 2, 1, 0, '', 1500795269),
(1325, 6, 1, 3, 1, 0, '', 1500795275),
(1326, 33, 1, 5, 1, 0, '', 1500795277),
(1327, 7, 1, 6, 1, 0, '', 1500795281),
(1328, 35, 1, 4, 1, 0, '', 1500795288),
(1329, 7, 1, 6, 1, 0, '', 1500795295),
(1330, 6, 1, 2, 1, 0, '', 1500795296),
(1331, 1, 1, 1, 1, 0, '', 1500795299),
(1332, 33, 1, 5, 1, 0, '', 1500795302),
(1333, 6, 1, 3, 1, 0, '', 1500795304),
(1334, 7, 1, 6, 1, 0, '', 1500795309),
(1335, 35, 1, 4, 1, 0, '', 1500795311),
(1336, 6, 1, 2, 1, 0, '', 1500795332),
(1337, 7, 1, 6, 1, 0, '', 1500795332),
(1338, 33, 1, 5, 1, 0, '', 1500795332),
(1339, 6, 1, 3, 1, 0, '', 1500795333),
(1340, 1, 1, 1, 1, 0, '', 1500795334),
(1341, 35, 1, 4, 1, 0, '', 1500795334),
(1342, 7, 1, 6, 1, 0, '', 1500795337),
(1343, 6, 1, 2, 1, 0, '', 1500795350),
(1344, 7, 1, 6, 1, 0, '', 1500795351),
(1345, 33, 1, 5, 1, 0, '', 1500795352),
(1346, 35, 1, 4, 1, 0, '', 1500795357),
(1347, 6, 1, 3, 1, 0, '', 1500795362),
(1348, 7, 1, 6, 1, 0, '', 1500795365),
(1349, 1, 1, 1, 1, 0, '', 1500795369),
(1350, 6, 1, 2, 1, 0, '', 1500795377),
(1351, 33, 1, 5, 1, 0, '', 1500795377),
(1352, 7, 1, 6, 1, 0, '', 1500795379),
(1353, 35, 1, 4, 1, 0, '', 1500795380),
(1354, 6, 1, 3, 1, 0, '', 1500795391),
(1355, 7, 1, 6, 1, 0, '', 1500795393),
(1356, 33, 1, 5, 1, 0, '', 1500795402),
(1357, 35, 1, 4, 1, 0, '', 1500795403),
(1358, 1, 1, 1, 1, 0, '', 1500795404),
(1359, 6, 1, 2, 1, 0, '', 1500795404),
(1360, 7, 1, 6, 1, 0, '', 1500795407),
(1361, 6, 1, 3, 1, 0, '', 1500795420),
(1362, 7, 1, 6, 1, 0, '', 1500795421),
(1363, 35, 1, 4, 1, 0, '', 1500795426),
(1364, 33, 1, 5, 1, 0, '', 1500795427),
(1365, 6, 1, 2, 1, 0, '', 1500795431),
(1366, 7, 1, 6, 1, 0, '', 1500795435),
(1367, 1, 1, 1, 1, 0, '', 1500795439),
(1368, 6, 1, 3, 1, 0, '', 1500795449),
(1369, 7, 1, 6, 1, 0, '', 1500795449),
(1370, 35, 1, 4, 1, 0, '', 1500795449);
INSERT INTO `hcusysalarminfo` (`sid`, `eqpid`, `alarmtype`, `alarmcontent`, `alarmseverity`, `alarmclearflag`, `alarmdesc`, `timestamp`) VALUES
(1371, 33, 1, 5, 1, 0, '', 1500795452),
(1372, 6, 1, 2, 1, 0, '', 1500795458),
(1373, 7, 1, 6, 1, 0, '', 1500795463),
(1374, 35, 1, 4, 1, 0, '', 1500795472),
(1375, 1, 1, 1, 1, 0, '', 1500795474),
(1376, 33, 1, 5, 1, 0, '', 1500795477),
(1377, 7, 1, 6, 1, 0, '', 1500795477),
(1378, 6, 1, 3, 1, 0, '', 1500795478),
(1379, 6, 1, 2, 1, 0, '', 1500795485),
(1380, 7, 1, 6, 1, 0, '', 1500795491),
(1381, 35, 1, 4, 1, 0, '', 1500795495),
(1382, 33, 1, 5, 1, 0, '', 1500795502),
(1383, 7, 1, 6, 1, 0, '', 1500795505),
(1384, 6, 1, 3, 1, 0, '', 1500795507),
(1385, 1, 1, 1, 1, 0, '', 1500795509),
(1386, 6, 1, 2, 1, 0, '', 1500795512),
(1387, 35, 1, 4, 1, 0, '', 1500795518),
(1388, 7, 1, 6, 1, 0, '', 1500795519),
(1389, 33, 1, 5, 1, 0, '', 1500795527),
(1390, 7, 1, 6, 1, 0, '', 1500795533),
(1391, 6, 1, 3, 1, 0, '', 1500795536),
(1392, 6, 1, 2, 1, 0, '', 1500795539),
(1393, 35, 1, 4, 1, 0, '', 1500795541),
(1394, 1, 1, 1, 1, 0, '', 1500795544),
(1395, 7, 1, 6, 1, 0, '', 1500795547),
(1396, 33, 1, 5, 1, 0, '', 1500795552),
(1397, 7, 1, 6, 1, 0, '', 1500795561),
(1398, 35, 1, 4, 1, 0, '', 1500795564),
(1399, 6, 1, 3, 1, 0, '', 1500795565),
(1400, 6, 1, 2, 1, 0, '', 1500795566),
(1401, 7, 1, 6, 1, 0, '', 1500795575),
(1402, 33, 1, 5, 1, 0, '', 1500795577),
(1403, 1, 1, 1, 1, 0, '', 1500795579),
(1404, 35, 1, 4, 1, 0, '', 1500795587),
(1405, 7, 1, 6, 1, 0, '', 1500795589),
(1406, 6, 1, 2, 1, 0, '', 1500795593),
(1407, 6, 1, 3, 1, 0, '', 1500795594),
(1408, 33, 1, 5, 1, 0, '', 1500795602),
(1409, 7, 1, 6, 1, 0, '', 1500795603),
(1410, 35, 1, 4, 1, 0, '', 1500795610),
(1411, 1, 1, 1, 1, 0, '', 1500795614),
(1412, 7, 1, 6, 1, 0, '', 1500795617),
(1413, 6, 1, 2, 1, 0, '', 1500795620),
(1414, 6, 1, 3, 1, 0, '', 1500795632),
(1415, 33, 1, 5, 1, 0, '', 1500795632),
(1416, 7, 1, 6, 1, 0, '', 1500795632),
(1417, 35, 1, 4, 1, 0, '', 1500795633),
(1418, 7, 1, 6, 1, 0, '', 1500795645),
(1419, 6, 1, 2, 1, 0, '', 1500795647),
(1420, 1, 1, 1, 1, 0, '', 1500795649),
(1421, 33, 1, 5, 1, 0, '', 1500795652),
(1422, 6, 1, 3, 1, 0, '', 1500795652),
(1423, 35, 1, 4, 1, 0, '', 1500795656),
(1424, 7, 1, 6, 1, 0, '', 1500795659),
(1425, 7, 1, 6, 1, 0, '', 1500795673),
(1426, 6, 1, 2, 1, 0, '', 1500795674),
(1427, 33, 1, 5, 1, 0, '', 1500795677),
(1428, 35, 1, 4, 1, 0, '', 1500795679),
(1429, 6, 1, 3, 1, 0, '', 1500795681),
(1430, 1, 1, 1, 1, 0, '', 1500795684),
(1431, 7, 1, 6, 1, 0, '', 1500795687),
(1432, 6, 1, 2, 1, 0, '', 1500795701),
(1433, 7, 1, 6, 1, 0, '', 1500795701),
(1434, 33, 1, 5, 1, 0, '', 1500795702),
(1435, 35, 1, 4, 1, 0, '', 1500795702),
(1436, 6, 1, 3, 1, 0, '', 1500795710),
(1437, 7, 1, 6, 1, 0, '', 1500795715),
(1438, 1, 1, 1, 1, 0, '', 1500795719),
(1439, 35, 1, 4, 1, 0, '', 1500795725),
(1440, 33, 1, 5, 1, 0, '', 1500795727),
(1441, 6, 1, 2, 1, 0, '', 1500795728),
(1442, 7, 1, 6, 1, 0, '', 1500795729),
(1443, 6, 1, 3, 1, 0, '', 1500795739),
(1444, 7, 1, 6, 1, 0, '', 1500795743),
(1445, 35, 1, 4, 1, 0, '', 1500795748),
(1446, 33, 1, 5, 1, 0, '', 1500795752),
(1447, 1, 1, 1, 1, 0, '', 1500795754),
(1448, 6, 1, 2, 1, 0, '', 1500795755),
(1449, 7, 1, 6, 1, 0, '', 1500795757),
(1450, 6, 1, 3, 1, 0, '', 1500795768),
(1451, 35, 1, 4, 1, 0, '', 1500795771),
(1452, 7, 1, 6, 1, 0, '', 1500795771),
(1453, 33, 1, 5, 1, 0, '', 1500795777),
(1454, 6, 1, 2, 1, 0, '', 1500795782),
(1455, 7, 1, 6, 1, 0, '', 1500795785),
(1456, 1, 1, 1, 1, 0, '', 1500795789),
(1457, 35, 1, 4, 1, 0, '', 1500795794),
(1458, 6, 1, 3, 1, 0, '', 1500795797),
(1459, 7, 1, 6, 1, 0, '', 1500795799),
(1460, 33, 1, 5, 1, 0, '', 1500795802),
(1461, 6, 1, 2, 1, 0, '', 1500795809),
(1462, 7, 1, 6, 1, 0, '', 1500795813),
(1463, 35, 1, 4, 1, 0, '', 1500795817),
(1464, 1, 1, 1, 1, 0, '', 1500795824),
(1465, 6, 1, 3, 1, 0, '', 1500795826),
(1466, 33, 1, 5, 1, 0, '', 1500795827),
(1467, 7, 1, 6, 1, 0, '', 1500795827),
(1468, 6, 1, 2, 1, 0, '', 1500795836),
(1469, 35, 1, 4, 1, 0, '', 1500795840),
(1470, 7, 1, 6, 1, 0, '', 1500795841),
(1471, 33, 1, 5, 1, 0, '', 1500795852),
(1472, 6, 1, 3, 1, 0, '', 1500795855),
(1473, 7, 1, 6, 1, 0, '', 1500795855),
(1474, 1, 1, 1, 1, 0, '', 1500795859),
(1475, 6, 1, 2, 1, 0, '', 1500795863),
(1476, 35, 1, 4, 1, 0, '', 1500795863),
(1477, 7, 1, 6, 1, 0, '', 1500795869),
(1478, 33, 1, 5, 1, 0, '', 1500795877),
(1479, 7, 1, 6, 1, 0, '', 1500795883),
(1480, 6, 1, 3, 1, 0, '', 1500795884),
(1481, 35, 1, 4, 1, 0, '', 1500795886),
(1482, 6, 1, 2, 1, 0, '', 1500795890),
(1483, 1, 1, 1, 1, 0, '', 1500795894),
(1484, 7, 1, 6, 1, 0, '', 1500795897),
(1485, 33, 1, 5, 1, 0, '', 1500795902),
(1486, 35, 1, 4, 1, 0, '', 1500795909),
(1487, 7, 1, 6, 1, 0, '', 1500795911),
(1488, 6, 1, 3, 1, 0, '', 1500795913),
(1489, 6, 1, 2, 1, 0, '', 1500795917),
(1490, 7, 1, 6, 1, 0, '', 1500795932),
(1491, 33, 1, 5, 1, 0, '', 1500795932),
(1492, 1, 1, 1, 1, 0, '', 1500795932),
(1493, 35, 1, 4, 1, 0, '', 1500795932),
(1494, 7, 1, 6, 1, 0, '', 1500795939),
(1495, 6, 1, 3, 1, 0, '', 1500795942),
(1496, 6, 1, 2, 1, 0, '', 1500795944),
(1497, 33, 1, 5, 1, 0, '', 1500795952),
(1498, 7, 1, 6, 1, 0, '', 1500795953),
(1499, 35, 1, 4, 1, 0, '', 1500795955),
(1500, 1, 1, 1, 1, 0, '', 1500795964),
(1501, 7, 1, 6, 1, 0, '', 1500795967),
(1502, 6, 1, 2, 1, 0, '', 1500795971),
(1503, 6, 1, 3, 1, 0, '', 1500795971),
(1504, 33, 1, 5, 1, 0, '', 1500795977),
(1505, 35, 1, 4, 1, 0, '', 1500795978),
(1506, 7, 1, 6, 1, 0, '', 1500795981),
(1507, 7, 1, 6, 1, 0, '', 1500795995),
(1508, 6, 1, 2, 1, 0, '', 1500795998),
(1509, 1, 1, 1, 1, 0, '', 1500795999),
(1510, 6, 1, 3, 1, 0, '', 1500796000),
(1511, 35, 1, 4, 1, 0, '', 1500796001),
(1512, 33, 1, 5, 1, 0, '', 1500796002),
(1513, 7, 1, 6, 1, 0, '', 1500796009),
(1514, 7, 1, 6, 1, 0, '', 1500796023),
(1515, 35, 1, 4, 1, 0, '', 1500796024),
(1516, 6, 1, 2, 1, 0, '', 1500796025),
(1517, 33, 1, 5, 1, 0, '', 1500796027),
(1518, 6, 1, 3, 1, 0, '', 1500796029),
(1519, 1, 1, 1, 1, 0, '', 1500796034),
(1520, 7, 1, 6, 1, 0, '', 1500796037),
(1521, 35, 1, 4, 1, 0, '', 1500796047),
(1522, 7, 1, 6, 1, 0, '', 1500796051),
(1523, 33, 1, 5, 1, 0, '', 1500796052),
(1524, 6, 1, 2, 1, 0, '', 1500796052),
(1525, 6, 1, 3, 1, 0, '', 1500796058),
(1526, 7, 1, 6, 1, 0, '', 1500796065),
(1527, 1, 1, 1, 1, 0, '', 1500796069),
(1528, 35, 1, 4, 1, 0, '', 1500796070),
(1529, 33, 1, 5, 1, 0, '', 1500796077),
(1530, 6, 1, 2, 1, 0, '', 1500796079),
(1531, 7, 1, 6, 1, 0, '', 1500796079),
(1532, 6, 1, 3, 1, 0, '', 1500796087),
(1533, 35, 1, 4, 1, 0, '', 1500796093),
(1534, 7, 1, 6, 1, 0, '', 1500796093),
(1535, 33, 1, 5, 1, 0, '', 1500796102),
(1536, 1, 1, 1, 1, 0, '', 1500796104),
(1537, 6, 1, 2, 1, 0, '', 1500796106),
(1538, 7, 1, 6, 1, 0, '', 1500796107),
(1539, 6, 1, 3, 1, 0, '', 1500796116),
(1540, 35, 1, 4, 1, 0, '', 1500796116),
(1541, 7, 1, 6, 1, 0, '', 1500796121),
(1542, 33, 1, 5, 1, 0, '', 1500796127),
(1543, 6, 1, 2, 1, 0, '', 1500796133),
(1544, 7, 1, 6, 1, 0, '', 1500796135),
(1545, 1, 1, 1, 1, 0, '', 1500796139),
(1546, 35, 1, 4, 1, 0, '', 1500796139),
(1547, 6, 1, 3, 1, 0, '', 1500796145),
(1548, 7, 1, 6, 1, 0, '', 1500796149),
(1549, 33, 1, 5, 1, 0, '', 1500796152),
(1550, 6, 1, 2, 1, 0, '', 1500796160),
(1551, 35, 1, 4, 1, 0, '', 1500796162),
(1552, 7, 1, 6, 1, 0, '', 1500796163),
(1553, 1, 1, 1, 1, 0, '', 1500796174),
(1554, 6, 1, 3, 1, 0, '', 1500796174),
(1555, 7, 1, 6, 1, 0, '', 1500796177),
(1556, 33, 1, 5, 1, 0, '', 1500796177),
(1557, 35, 1, 4, 1, 0, '', 1500796185),
(1558, 6, 1, 2, 1, 0, '', 1500796187),
(1559, 7, 1, 6, 1, 0, '', 1500796191),
(1560, 33, 1, 5, 1, 0, '', 1500796202),
(1561, 6, 1, 3, 1, 0, '', 1500796203),
(1562, 7, 1, 6, 1, 0, '', 1500796205),
(1563, 35, 1, 4, 1, 0, '', 1500796208),
(1564, 1, 1, 1, 1, 0, '', 1500796209),
(1565, 6, 1, 2, 1, 0, '', 1500796214),
(1566, 7, 1, 6, 1, 0, '', 1500796219),
(1567, 33, 1, 5, 1, 0, '', 1500796232),
(1568, 35, 1, 4, 1, 0, '', 1500796232),
(1569, 6, 1, 3, 1, 0, '', 1500796232),
(1570, 7, 1, 6, 1, 0, '', 1500796233),
(1571, 6, 1, 2, 1, 0, '', 1500796241),
(1572, 1, 1, 1, 1, 0, '', 1500796244),
(1573, 7, 1, 6, 1, 0, '', 1500796247),
(1574, 33, 1, 5, 1, 0, '', 1500796252),
(1575, 35, 1, 4, 1, 0, '', 1500796254),
(1576, 6, 1, 3, 1, 0, '', 1500796261),
(1577, 7, 1, 6, 1, 0, '', 1500796261),
(1578, 6, 1, 2, 1, 0, '', 1500796268),
(1579, 7, 1, 6, 1, 0, '', 1500796275),
(1580, 33, 1, 5, 1, 0, '', 1500796277),
(1581, 35, 1, 4, 1, 0, '', 1500796277),
(1582, 1, 1, 1, 1, 0, '', 1500796279),
(1583, 7, 1, 6, 1, 0, '', 1500796289),
(1584, 6, 1, 3, 1, 0, '', 1500796290),
(1585, 6, 1, 2, 1, 0, '', 1500796295),
(1586, 35, 1, 4, 1, 0, '', 1500796300),
(1587, 33, 1, 5, 1, 0, '', 1500796302),
(1588, 7, 1, 6, 1, 0, '', 1500796303),
(1589, 1, 1, 1, 1, 0, '', 1500796314),
(1590, 7, 1, 6, 1, 0, '', 1500796317),
(1591, 6, 1, 3, 1, 0, '', 1500796319),
(1592, 6, 1, 2, 1, 0, '', 1500796322),
(1593, 35, 1, 4, 1, 0, '', 1500796323),
(1594, 33, 1, 5, 1, 0, '', 1500796327),
(1595, 7, 1, 6, 1, 0, '', 1500796331),
(1596, 7, 1, 6, 1, 0, '', 1500796345),
(1597, 35, 1, 4, 1, 0, '', 1500796346),
(1598, 6, 1, 3, 1, 0, '', 1500796348),
(1599, 1, 1, 1, 1, 0, '', 1500796349),
(1600, 6, 1, 2, 1, 0, '', 1500796349),
(1601, 33, 1, 5, 1, 0, '', 1500796352),
(1602, 7, 1, 6, 1, 0, '', 1500796359),
(1603, 35, 1, 4, 1, 0, '', 1500796369),
(1604, 7, 1, 6, 1, 0, '', 1500796373),
(1605, 6, 1, 2, 1, 0, '', 1500796376),
(1606, 33, 1, 5, 1, 0, '', 1500796377),
(1607, 6, 1, 3, 1, 0, '', 1500796377),
(1608, 1, 1, 1, 1, 0, '', 1500796384),
(1609, 7, 1, 6, 1, 0, '', 1500796387),
(1610, 35, 1, 4, 1, 0, '', 1500796392),
(1611, 7, 1, 6, 1, 0, '', 1500796401),
(1612, 33, 1, 5, 1, 0, '', 1500796402),
(1613, 6, 1, 2, 1, 0, '', 1500796403),
(1614, 6, 1, 3, 1, 0, '', 1500796406),
(1615, 35, 1, 4, 1, 0, '', 1500796415),
(1616, 7, 1, 6, 1, 0, '', 1500796415),
(1617, 1, 1, 1, 1, 0, '', 1500796419),
(1618, 33, 1, 5, 1, 0, '', 1500796427),
(1619, 7, 1, 6, 1, 0, '', 1500796429),
(1620, 6, 1, 2, 1, 0, '', 1500796430),
(1621, 6, 1, 3, 1, 0, '', 1500796435),
(1622, 35, 1, 4, 1, 0, '', 1500796438),
(1623, 7, 1, 6, 1, 0, '', 1500796443),
(1624, 33, 1, 5, 1, 0, '', 1500796452),
(1625, 1, 1, 1, 1, 0, '', 1500796454),
(1626, 6, 1, 2, 1, 0, '', 1500796457),
(1627, 7, 1, 6, 1, 0, '', 1500796457),
(1628, 35, 1, 4, 1, 0, '', 1500796461),
(1629, 6, 1, 3, 1, 0, '', 1500796464),
(1630, 7, 1, 6, 1, 0, '', 1500796471),
(1631, 33, 1, 5, 1, 0, '', 1500796477),
(1632, 6, 1, 2, 1, 0, '', 1500796484),
(1633, 35, 1, 4, 1, 0, '', 1500796484),
(1634, 7, 1, 6, 1, 0, '', 1500796485),
(1635, 1, 1, 1, 1, 0, '', 1500796489),
(1636, 6, 1, 3, 1, 0, '', 1500796493),
(1637, 7, 1, 6, 1, 0, '', 1500796499),
(1638, 33, 1, 5, 1, 0, '', 1500796502),
(1639, 35, 1, 4, 1, 0, '', 1500796507),
(1640, 6, 1, 2, 1, 0, '', 1500796511),
(1641, 7, 1, 6, 1, 0, '', 1500796513),
(1642, 6, 1, 3, 1, 0, '', 1500796532),
(1643, 1, 1, 1, 1, 0, '', 1500796532),
(1644, 33, 1, 5, 1, 0, '', 1500796532),
(1645, 7, 1, 6, 1, 0, '', 1500796532),
(1646, 35, 1, 4, 1, 0, '', 1500796532),
(1647, 6, 1, 2, 1, 0, '', 1500796538),
(1648, 7, 1, 6, 1, 0, '', 1500796541),
(1649, 6, 1, 3, 1, 0, '', 1500796551),
(1650, 33, 1, 5, 1, 0, '', 1500796552),
(1651, 35, 1, 4, 1, 0, '', 1500796553),
(1652, 7, 1, 6, 1, 0, '', 1500796555),
(1653, 1, 1, 1, 1, 0, '', 1500796559),
(1654, 6, 1, 2, 1, 0, '', 1500796565),
(1655, 7, 1, 6, 1, 0, '', 1500796569),
(1656, 35, 1, 4, 1, 0, '', 1500796576),
(1657, 33, 1, 5, 1, 0, '', 1500796577),
(1658, 6, 1, 3, 1, 0, '', 1500796580),
(1659, 7, 1, 6, 1, 0, '', 1500796583),
(1660, 6, 1, 2, 1, 0, '', 1500796592),
(1661, 1, 1, 1, 1, 0, '', 1500796594),
(1662, 7, 1, 6, 1, 0, '', 1500796597),
(1663, 35, 1, 4, 1, 0, '', 1500796599),
(1664, 33, 1, 5, 1, 0, '', 1500796602),
(1665, 6, 1, 3, 1, 0, '', 1500796609),
(1666, 7, 1, 6, 1, 0, '', 1500796611),
(1667, 6, 1, 2, 1, 0, '', 1500796619),
(1668, 35, 1, 4, 1, 0, '', 1500796622),
(1669, 7, 1, 6, 1, 0, '', 1500796625),
(1670, 33, 1, 5, 1, 0, '', 1500796627),
(1671, 1, 1, 1, 1, 0, '', 1500796629),
(1672, 6, 1, 3, 1, 0, '', 1500796638),
(1673, 7, 1, 6, 1, 0, '', 1500796639),
(1674, 35, 1, 4, 1, 0, '', 1500796645),
(1675, 6, 1, 2, 1, 0, '', 1500796646),
(1676, 33, 1, 5, 1, 0, '', 1500796652),
(1677, 7, 1, 6, 1, 0, '', 1500796653),
(1678, 1, 1, 1, 1, 0, '', 1500796664),
(1679, 6, 1, 3, 1, 0, '', 1500796667),
(1680, 7, 1, 6, 1, 0, '', 1500796667),
(1681, 35, 1, 4, 1, 0, '', 1500796668),
(1682, 6, 1, 2, 1, 0, '', 1500796673),
(1683, 33, 1, 5, 1, 0, '', 1500796677),
(1684, 7, 1, 6, 1, 0, '', 1500796681),
(1685, 35, 1, 4, 1, 0, '', 1500796691),
(1686, 7, 1, 6, 1, 0, '', 1500796695),
(1687, 6, 1, 3, 1, 0, '', 1500796696),
(1688, 1, 1, 1, 1, 0, '', 1500796699),
(1689, 6, 1, 2, 1, 0, '', 1500796700),
(1690, 33, 1, 5, 1, 0, '', 1500796702),
(1691, 7, 1, 6, 1, 0, '', 1500796709),
(1692, 35, 1, 4, 1, 0, '', 1500796714),
(1693, 7, 1, 6, 1, 0, '', 1500796723),
(1694, 6, 1, 3, 1, 0, '', 1500796725),
(1695, 6, 1, 2, 1, 0, '', 1500796727),
(1696, 33, 1, 5, 1, 0, '', 1500796727),
(1697, 1, 1, 1, 1, 0, '', 1500796734),
(1698, 35, 1, 4, 1, 0, '', 1500796737),
(1699, 7, 1, 6, 1, 0, '', 1500796737),
(1700, 7, 1, 6, 1, 0, '', 1500796751),
(1701, 33, 1, 5, 1, 0, '', 1500796752),
(1702, 6, 1, 2, 1, 0, '', 1500796754),
(1703, 6, 1, 3, 1, 0, '', 1500796754),
(1704, 35, 1, 4, 1, 0, '', 1500796760),
(1705, 7, 1, 6, 1, 0, '', 1500796765),
(1706, 1, 1, 1, 1, 0, '', 1500796769),
(1707, 33, 1, 5, 1, 0, '', 1500796777),
(1708, 7, 1, 6, 1, 0, '', 1500796779),
(1709, 6, 1, 2, 1, 0, '', 1500796781),
(1710, 6, 1, 3, 1, 0, '', 1500796783),
(1711, 35, 1, 4, 1, 0, '', 1500796783),
(1712, 7, 1, 6, 1, 0, '', 1500796793),
(1713, 33, 1, 5, 1, 0, '', 1500796802),
(1714, 1, 1, 1, 1, 0, '', 1500796804),
(1715, 35, 1, 4, 1, 0, '', 1500796806),
(1716, 7, 1, 6, 1, 0, '', 1500796807),
(1717, 6, 1, 2, 1, 0, '', 1500796808),
(1718, 6, 1, 3, 1, 0, '', 1500796812),
(1719, 7, 1, 6, 1, 0, '', 1500796821),
(1720, 33, 1, 5, 1, 0, '', 1500796832),
(1721, 35, 1, 4, 1, 0, '', 1500796832),
(1722, 6, 1, 2, 1, 0, '', 1500796835),
(1723, 7, 1, 6, 1, 0, '', 1500796835),
(1724, 1, 1, 1, 1, 0, '', 1500796839),
(1725, 6, 1, 3, 1, 0, '', 1500796841),
(1726, 7, 1, 6, 1, 0, '', 1500796849),
(1727, 33, 1, 5, 1, 0, '', 1500796852),
(1728, 35, 1, 4, 1, 0, '', 1500796852),
(1729, 6, 1, 2, 1, 0, '', 1500796862),
(1730, 7, 1, 6, 1, 0, '', 1500796863),
(1731, 6, 1, 3, 1, 0, '', 1500796870),
(1732, 1, 1, 1, 1, 0, '', 1500796874),
(1733, 35, 1, 4, 1, 0, '', 1500796875),
(1734, 33, 1, 5, 1, 0, '', 1500796877),
(1735, 7, 1, 6, 1, 0, '', 1500796877),
(1736, 6, 1, 2, 1, 0, '', 1500796889),
(1737, 7, 1, 6, 1, 0, '', 1500796891),
(1738, 35, 1, 4, 1, 0, '', 1500796898),
(1739, 6, 1, 3, 1, 0, '', 1500796899),
(1740, 33, 1, 5, 1, 0, '', 1500796902),
(1741, 7, 1, 6, 1, 0, '', 1500796905),
(1742, 1, 1, 1, 1, 0, '', 1500796909),
(1743, 6, 1, 2, 1, 0, '', 1500796916),
(1744, 7, 1, 6, 1, 0, '', 1500796919),
(1745, 35, 1, 4, 1, 0, '', 1500796921),
(1746, 33, 1, 5, 1, 0, '', 1500796927),
(1747, 6, 1, 3, 1, 0, '', 1500796928),
(1748, 7, 1, 6, 1, 0, '', 1500796933),
(1749, 6, 1, 2, 1, 0, '', 1500796943),
(1750, 1, 1, 1, 1, 0, '', 1500796944),
(1751, 35, 1, 4, 1, 0, '', 1500796944),
(1752, 7, 1, 6, 1, 0, '', 1500796947),
(1753, 33, 1, 5, 1, 0, '', 1500796952),
(1754, 6, 1, 3, 1, 0, '', 1500796957),
(1755, 7, 1, 6, 1, 0, '', 1500796961),
(1756, 35, 1, 4, 1, 0, '', 1500796967),
(1757, 6, 1, 2, 1, 0, '', 1500796970),
(1758, 7, 1, 6, 1, 0, '', 1500796975),
(1759, 33, 1, 5, 1, 0, '', 1500796977),
(1760, 1, 1, 1, 1, 0, '', 1500796979),
(1761, 6, 1, 3, 1, 0, '', 1500796986),
(1762, 7, 1, 6, 1, 0, '', 1500796989),
(1763, 35, 1, 4, 1, 0, '', 1500796990),
(1764, 6, 1, 2, 1, 0, '', 1500796997),
(1765, 33, 1, 5, 1, 0, '', 1500797002),
(1766, 7, 1, 6, 1, 0, '', 1500797003),
(1767, 35, 1, 4, 1, 0, '', 1500797013),
(1768, 1, 1, 1, 1, 0, '', 1500797014),
(1769, 6, 1, 3, 1, 0, '', 1500797015),
(1770, 7, 1, 6, 1, 0, '', 1500797017),
(1771, 6, 1, 2, 1, 0, '', 1500797024),
(1772, 33, 1, 5, 1, 0, '', 1500797027),
(1773, 7, 1, 6, 1, 0, '', 1500797031),
(1774, 35, 1, 4, 1, 0, '', 1500797036),
(1775, 6, 1, 3, 1, 0, '', 1500797044),
(1776, 7, 1, 6, 1, 0, '', 1500797045),
(1777, 1, 1, 1, 1, 0, '', 1500797049),
(1778, 6, 1, 2, 1, 0, '', 1500797051),
(1779, 33, 1, 5, 1, 0, '', 1500797052),
(1780, 7, 1, 6, 1, 0, '', 1500797059),
(1781, 35, 1, 4, 1, 0, '', 1500797059),
(1782, 6, 1, 3, 1, 0, '', 1500797073),
(1783, 7, 1, 6, 1, 0, '', 1500797073),
(1784, 33, 1, 5, 1, 0, '', 1500797077),
(1785, 6, 1, 2, 1, 0, '', 1500797078),
(1786, 35, 1, 4, 1, 0, '', 1500797082),
(1787, 1, 1, 1, 1, 0, '', 1500797084),
(1788, 7, 1, 6, 1, 0, '', 1500797087),
(1789, 7, 1, 6, 1, 0, '', 1500797101),
(1790, 33, 1, 5, 1, 0, '', 1500797102),
(1791, 6, 1, 3, 1, 0, '', 1500797102),
(1792, 35, 1, 4, 1, 0, '', 1500797105),
(1793, 6, 1, 2, 1, 0, '', 1500797105),
(1794, 7, 1, 6, 1, 0, '', 1500797115),
(1795, 1, 1, 1, 1, 0, '', 1500797119),
(1796, 33, 1, 5, 1, 0, '', 1500797132),
(1797, 35, 1, 4, 1, 0, '', 1500797132),
(1798, 7, 1, 6, 1, 0, '', 1500797132),
(1799, 6, 1, 3, 1, 0, '', 1500797132),
(1800, 6, 1, 2, 1, 0, '', 1500797132),
(1801, 7, 1, 6, 1, 0, '', 1500797143),
(1802, 35, 1, 4, 1, 0, '', 1500797151),
(1803, 33, 1, 5, 1, 0, '', 1500797152),
(1804, 1, 1, 1, 1, 0, '', 1500797154),
(1805, 7, 1, 6, 1, 0, '', 1500797157),
(1806, 6, 1, 2, 1, 0, '', 1500797159),
(1807, 6, 1, 3, 1, 0, '', 1500797160),
(1808, 7, 1, 6, 1, 0, '', 1500797171),
(1809, 35, 1, 4, 1, 0, '', 1500797174),
(1810, 33, 1, 5, 1, 0, '', 1500797177),
(1811, 7, 1, 6, 1, 0, '', 1500797185),
(1812, 6, 1, 2, 1, 0, '', 1500797186),
(1813, 1, 1, 1, 1, 0, '', 1500797189),
(1814, 6, 1, 3, 1, 0, '', 1500797189),
(1815, 35, 1, 4, 1, 0, '', 1500797197),
(1816, 7, 1, 6, 1, 0, '', 1500797199),
(1817, 33, 1, 5, 1, 0, '', 1500797202),
(1818, 7, 1, 6, 1, 0, '', 1500797213),
(1819, 6, 1, 2, 1, 0, '', 1500797213),
(1820, 6, 1, 3, 1, 0, '', 1500797218),
(1821, 35, 1, 4, 1, 0, '', 1500797220),
(1822, 1, 1, 1, 1, 0, '', 1500797224),
(1823, 33, 1, 5, 1, 0, '', 1500797227),
(1824, 7, 1, 6, 1, 0, '', 1500797227),
(1825, 6, 1, 2, 1, 0, '', 1500797240),
(1826, 7, 1, 6, 1, 0, '', 1500797241),
(1827, 35, 1, 4, 1, 0, '', 1500797243),
(1828, 6, 1, 3, 1, 0, '', 1500797247),
(1829, 33, 1, 5, 1, 0, '', 1500797252),
(1830, 7, 1, 6, 1, 0, '', 1500797255),
(1831, 1, 1, 1, 1, 0, '', 1500797259),
(1832, 35, 1, 4, 1, 0, '', 1500797266),
(1833, 6, 1, 2, 1, 0, '', 1500797267),
(1834, 7, 1, 6, 1, 0, '', 1500797269),
(1835, 6, 1, 3, 1, 0, '', 1500797276),
(1836, 33, 1, 5, 1, 0, '', 1500797277),
(1837, 7, 1, 6, 1, 0, '', 1500797283),
(1838, 35, 1, 4, 1, 0, '', 1500797289),
(1839, 1, 1, 1, 1, 0, '', 1500797294),
(1840, 6, 1, 2, 1, 0, '', 1500797294),
(1841, 7, 1, 6, 1, 0, '', 1500797297),
(1842, 33, 1, 5, 1, 0, '', 1500797302),
(1843, 6, 1, 3, 1, 0, '', 1500797305),
(1844, 7, 1, 6, 1, 0, '', 1500797311),
(1845, 35, 1, 4, 1, 0, '', 1500797312),
(1846, 6, 1, 2, 1, 0, '', 1500797321),
(1847, 7, 1, 6, 1, 0, '', 1500797325),
(1848, 33, 1, 5, 1, 0, '', 1500797327),
(1849, 1, 1, 1, 1, 0, '', 1500797329),
(1850, 6, 1, 3, 1, 0, '', 1500797334),
(1851, 35, 1, 4, 1, 0, '', 1500797335),
(1852, 7, 1, 6, 1, 0, '', 1500797339),
(1853, 6, 1, 2, 1, 0, '', 1500797348),
(1854, 33, 1, 5, 1, 0, '', 1500797352),
(1855, 7, 1, 6, 1, 0, '', 1500797353),
(1856, 35, 1, 4, 1, 0, '', 1500797358),
(1857, 6, 1, 3, 1, 0, '', 1500797363),
(1858, 1, 1, 1, 1, 0, '', 1500797364),
(1859, 7, 1, 6, 1, 0, '', 1500797367),
(1860, 6, 1, 2, 1, 0, '', 1500797375),
(1861, 33, 1, 5, 1, 0, '', 1500797377),
(1862, 35, 1, 4, 1, 0, '', 1500797381),
(1863, 7, 1, 6, 1, 0, '', 1500797381),
(1864, 6, 1, 3, 1, 0, '', 1500797392),
(1865, 7, 1, 6, 1, 0, '', 1500797395),
(1866, 1, 1, 1, 1, 0, '', 1500797399),
(1867, 33, 1, 5, 1, 0, '', 1500797402),
(1868, 6, 1, 2, 1, 0, '', 1500797402),
(1869, 35, 1, 4, 1, 0, '', 1500797404),
(1870, 7, 1, 6, 1, 0, '', 1500797409),
(1871, 6, 1, 3, 1, 0, '', 1500797421),
(1872, 7, 1, 6, 1, 0, '', 1500797432),
(1873, 33, 1, 5, 1, 0, '', 1500797432),
(1874, 35, 1, 4, 1, 0, '', 1500797432),
(1875, 6, 1, 2, 1, 0, '', 1500797432),
(1876, 1, 1, 1, 1, 0, '', 1500797434),
(1877, 7, 1, 6, 1, 0, '', 1500797437),
(1878, 35, 1, 4, 1, 0, '', 1500797450),
(1879, 6, 1, 3, 1, 0, '', 1500797450),
(1880, 7, 1, 6, 1, 0, '', 1500797451),
(1881, 33, 1, 5, 1, 0, '', 1500797452),
(1882, 6, 1, 2, 1, 0, '', 1500797456),
(1883, 7, 1, 6, 1, 0, '', 1500797465),
(1884, 1, 1, 1, 1, 0, '', 1500797469),
(1885, 35, 1, 4, 1, 0, '', 1500797473),
(1886, 33, 1, 5, 1, 0, '', 1500797477),
(1887, 6, 1, 3, 1, 0, '', 1500797479),
(1888, 7, 1, 6, 1, 0, '', 1500797479),
(1889, 6, 1, 2, 1, 0, '', 1500797483),
(1890, 7, 1, 6, 1, 0, '', 1500797493),
(1891, 35, 1, 4, 1, 0, '', 1500797496),
(1892, 33, 1, 5, 1, 0, '', 1500797502),
(1893, 1, 1, 1, 1, 0, '', 1500797504),
(1894, 7, 1, 6, 1, 0, '', 1500797507),
(1895, 6, 1, 3, 1, 0, '', 1500797508),
(1896, 6, 1, 2, 1, 0, '', 1500797510),
(1897, 35, 1, 4, 1, 0, '', 1500797519),
(1898, 7, 1, 6, 1, 0, '', 1500797521),
(1899, 33, 1, 5, 1, 0, '', 1500797527),
(1900, 7, 1, 6, 1, 0, '', 1500797535),
(1901, 6, 1, 2, 1, 0, '', 1500797537),
(1902, 6, 1, 3, 1, 0, '', 1500797537),
(1903, 1, 1, 1, 1, 0, '', 1500797539),
(1904, 35, 1, 4, 1, 0, '', 1500797542),
(1905, 7, 1, 6, 1, 0, '', 1500797549),
(1906, 33, 1, 5, 1, 0, '', 1500797552),
(1907, 7, 1, 6, 1, 0, '', 1500797563),
(1908, 6, 1, 2, 1, 0, '', 1500797564),
(1909, 35, 1, 4, 1, 0, '', 1500797565),
(1910, 6, 1, 3, 1, 0, '', 1500797566),
(1911, 1, 1, 1, 1, 0, '', 1500797574),
(1912, 7, 1, 6, 1, 0, '', 1500797577),
(1913, 33, 1, 5, 1, 0, '', 1500797577),
(1914, 35, 1, 4, 1, 0, '', 1500797588),
(1915, 6, 1, 2, 1, 0, '', 1500797591),
(1916, 7, 1, 6, 1, 0, '', 1500797591),
(1917, 6, 1, 3, 1, 0, '', 1500797595),
(1918, 33, 1, 5, 1, 0, '', 1500797602),
(1919, 7, 1, 6, 1, 0, '', 1500797605),
(1920, 1, 1, 1, 1, 0, '', 1500797609),
(1921, 35, 1, 4, 1, 0, '', 1500797611),
(1922, 6, 1, 2, 1, 0, '', 1500797618),
(1923, 7, 1, 6, 1, 0, '', 1500797619),
(1924, 6, 1, 3, 1, 0, '', 1500797624),
(1925, 33, 1, 5, 1, 0, '', 1500797627),
(1926, 7, 1, 6, 1, 0, '', 1500797633),
(1927, 35, 1, 4, 1, 0, '', 1500797634),
(1928, 1, 1, 1, 1, 0, '', 1500797644),
(1929, 6, 1, 2, 1, 0, '', 1500797645),
(1930, 7, 1, 6, 1, 0, '', 1500797647),
(1931, 33, 1, 5, 1, 0, '', 1500797652),
(1932, 6, 1, 3, 1, 0, '', 1500797653),
(1933, 35, 1, 4, 1, 0, '', 1500797657),
(1934, 7, 1, 6, 1, 0, '', 1500797661),
(1935, 6, 1, 2, 1, 0, '', 1500797672),
(1936, 7, 1, 6, 1, 0, '', 1500797675),
(1937, 33, 1, 5, 1, 0, '', 1500797677),
(1938, 1, 1, 1, 1, 0, '', 1500797679),
(1939, 35, 1, 4, 1, 0, '', 1500797680),
(1940, 6, 1, 3, 1, 0, '', 1500797682),
(1941, 7, 1, 6, 1, 0, '', 1500797689),
(1942, 6, 1, 2, 1, 0, '', 1500797699),
(1943, 33, 1, 5, 1, 0, '', 1500797702),
(1944, 35, 1, 4, 1, 0, '', 1500797703),
(1945, 7, 1, 6, 1, 0, '', 1500797703),
(1946, 6, 1, 3, 1, 0, '', 1500797711),
(1947, 1, 1, 1, 1, 0, '', 1500797714),
(1948, 7, 1, 6, 1, 0, '', 1500797717),
(1949, 35, 1, 4, 1, 0, '', 1500797732),
(1950, 6, 1, 2, 1, 0, '', 1500797732),
(1951, 33, 1, 5, 1, 0, '', 1500797732),
(1952, 7, 1, 6, 1, 0, '', 1500797733),
(1953, 6, 1, 3, 1, 0, '', 1500797740),
(1954, 7, 1, 6, 1, 0, '', 1500797745),
(1955, 1, 1, 1, 1, 0, '', 1500797749),
(1956, 35, 1, 4, 1, 0, '', 1500797749),
(1957, 33, 1, 5, 1, 0, '', 1500797752),
(1958, 6, 1, 2, 1, 0, '', 1500797753),
(1959, 7, 1, 6, 1, 0, '', 1500797759),
(1960, 6, 1, 3, 1, 0, '', 1500797769),
(1961, 35, 1, 4, 1, 0, '', 1500797772),
(1962, 7, 1, 6, 1, 0, '', 1500797773),
(1963, 33, 1, 5, 1, 0, '', 1500797777),
(1964, 6, 1, 2, 1, 0, '', 1500797780),
(1965, 1, 1, 1, 1, 0, '', 1500797784),
(1966, 7, 1, 6, 1, 0, '', 1500797787),
(1967, 35, 1, 4, 1, 0, '', 1500797795),
(1968, 6, 1, 3, 1, 0, '', 1500797798),
(1969, 7, 1, 6, 1, 0, '', 1500797801),
(1970, 33, 1, 5, 1, 0, '', 1500797802),
(1971, 6, 1, 2, 1, 0, '', 1500797807),
(1972, 7, 1, 6, 1, 0, '', 1500797815),
(1973, 35, 1, 4, 1, 0, '', 1500797818),
(1974, 1, 1, 1, 1, 0, '', 1500797819),
(1975, 6, 1, 3, 1, 0, '', 1500797827),
(1976, 33, 1, 5, 1, 0, '', 1500797827),
(1977, 7, 1, 6, 1, 0, '', 1500797829),
(1978, 6, 1, 2, 1, 0, '', 1500797834),
(1979, 35, 1, 4, 1, 0, '', 1500797841),
(1980, 7, 1, 6, 1, 0, '', 1500797843),
(1981, 33, 1, 5, 1, 0, '', 1500797852),
(1982, 1, 1, 1, 1, 0, '', 1500797854),
(1983, 6, 1, 3, 1, 0, '', 1500797856),
(1984, 7, 1, 6, 1, 0, '', 1500797857),
(1985, 6, 1, 2, 1, 0, '', 1500797861),
(1986, 35, 1, 4, 1, 0, '', 1500797864),
(1987, 7, 1, 6, 1, 0, '', 1500797871),
(1988, 33, 1, 5, 1, 0, '', 1500797877),
(1989, 6, 1, 3, 1, 0, '', 1500797885),
(1990, 7, 1, 6, 1, 0, '', 1500797885),
(1991, 35, 1, 4, 1, 0, '', 1500797887),
(1992, 6, 1, 2, 1, 0, '', 1500797888),
(1993, 1, 1, 1, 1, 0, '', 1500797889),
(1994, 7, 1, 6, 1, 0, '', 1500797899),
(1995, 33, 1, 5, 1, 0, '', 1500797902),
(1996, 35, 1, 4, 1, 0, '', 1500797910),
(1997, 7, 1, 6, 1, 0, '', 1500797913),
(1998, 6, 1, 3, 1, 0, '', 1500797914),
(1999, 6, 1, 2, 1, 0, '', 1500797915),
(2000, 1, 1, 1, 1, 0, '', 1500797924),
(2001, 33, 1, 5, 1, 0, '', 1500797927),
(2002, 7, 1, 6, 1, 0, '', 1500797927),
(2003, 35, 1, 4, 1, 0, '', 1500797933),
(2004, 7, 1, 6, 1, 0, '', 1500797941),
(2005, 6, 1, 2, 1, 0, '', 1500797942),
(2006, 6, 1, 3, 1, 0, '', 1500797943),
(2007, 33, 1, 5, 1, 0, '', 1500797952),
(2008, 7, 1, 6, 1, 0, '', 1500797955),
(2009, 35, 1, 4, 1, 0, '', 1500797956),
(2010, 1, 1, 1, 1, 0, '', 1500797959),
(2011, 6, 1, 2, 1, 0, '', 1500797969),
(2012, 7, 1, 6, 1, 0, '', 1500797969),
(2013, 6, 1, 3, 1, 0, '', 1500797972),
(2014, 33, 1, 5, 1, 0, '', 1500797977),
(2015, 35, 1, 4, 1, 0, '', 1500797979),
(2016, 7, 1, 6, 1, 0, '', 1500797983),
(2017, 1, 1, 1, 1, 0, '', 1500797994),
(2018, 6, 1, 2, 1, 0, '', 1500797996),
(2019, 7, 1, 6, 1, 0, '', 1500797997),
(2020, 6, 1, 3, 1, 0, '', 1500798001),
(2021, 33, 1, 5, 1, 0, '', 1500798002),
(2022, 35, 1, 4, 1, 0, '', 1500798002),
(2023, 7, 1, 6, 1, 0, '', 1500798011),
(2024, 6, 1, 2, 1, 0, '', 1500798032),
(2025, 7, 1, 6, 1, 0, '', 1500798032),
(2026, 35, 1, 4, 1, 0, '', 1500798032),
(2027, 33, 1, 5, 1, 0, '', 1500798032),
(2028, 1, 1, 1, 1, 0, '', 1500798032),
(2029, 6, 1, 3, 1, 0, '', 1500798032),
(2030, 7, 1, 6, 1, 0, '', 1500798039),
(2031, 35, 1, 4, 1, 0, '', 1500798048),
(2032, 6, 1, 2, 1, 0, '', 1500798050),
(2033, 33, 1, 5, 1, 0, '', 1500798052),
(2034, 7, 1, 6, 1, 0, '', 1500798053),
(2035, 6, 1, 3, 1, 0, '', 1500798059),
(2036, 1, 1, 1, 1, 0, '', 1500798064),
(2037, 7, 1, 6, 1, 0, '', 1500798067),
(2038, 35, 1, 4, 1, 0, '', 1500798071),
(2039, 33, 1, 5, 1, 0, '', 1500798077),
(2040, 6, 1, 2, 1, 0, '', 1500798077),
(2041, 7, 1, 6, 1, 0, '', 1500798081),
(2042, 6, 1, 3, 1, 0, '', 1500798088),
(2043, 35, 1, 4, 1, 0, '', 1500798094),
(2044, 7, 1, 6, 1, 0, '', 1500798095),
(2045, 1, 1, 1, 1, 0, '', 1500798099),
(2046, 33, 1, 5, 1, 0, '', 1500798102),
(2047, 6, 1, 2, 1, 0, '', 1500798104),
(2048, 7, 1, 6, 1, 0, '', 1500798109),
(2049, 6, 1, 3, 1, 0, '', 1500798117),
(2050, 35, 1, 4, 1, 0, '', 1500798117),
(2051, 7, 1, 6, 1, 0, '', 1500798123),
(2052, 33, 1, 5, 1, 0, '', 1500798127),
(2053, 6, 1, 2, 1, 0, '', 1500798131),
(2054, 1, 1, 1, 1, 0, '', 1500798134),
(2055, 7, 1, 6, 1, 0, '', 1500798137),
(2056, 35, 1, 4, 1, 0, '', 1500798140),
(2057, 6, 1, 3, 1, 0, '', 1500798146),
(2058, 7, 1, 6, 1, 0, '', 1500798151),
(2059, 33, 1, 5, 1, 0, '', 1500798152),
(2060, 6, 1, 2, 1, 0, '', 1500798158),
(2061, 35, 1, 4, 1, 0, '', 1500798163),
(2062, 7, 1, 6, 1, 0, '', 1500798165),
(2063, 1, 1, 1, 1, 0, '', 1500798169),
(2064, 6, 1, 3, 1, 0, '', 1500798175),
(2065, 33, 1, 5, 1, 0, '', 1500798177),
(2066, 7, 1, 6, 1, 0, '', 1500798179),
(2067, 6, 1, 2, 1, 0, '', 1500798185),
(2068, 35, 1, 4, 1, 0, '', 1500798186),
(2069, 7, 1, 6, 1, 0, '', 1500798193),
(2070, 33, 1, 5, 1, 0, '', 1500798202),
(2071, 1, 1, 1, 1, 0, '', 1500798204),
(2072, 6, 1, 3, 1, 0, '', 1500798204),
(2073, 7, 1, 6, 1, 0, '', 1500798207),
(2074, 35, 1, 4, 1, 0, '', 1500798209),
(2075, 6, 1, 2, 1, 0, '', 1500798212),
(2076, 7, 1, 6, 1, 0, '', 1500798221),
(2077, 33, 1, 5, 1, 0, '', 1500798227),
(2078, 35, 1, 4, 1, 0, '', 1500798232),
(2079, 6, 1, 3, 1, 0, '', 1500798233),
(2080, 7, 1, 6, 1, 0, '', 1500798235),
(2081, 1, 1, 1, 1, 0, '', 1500798239),
(2082, 6, 1, 2, 1, 0, '', 1500798239),
(2083, 7, 1, 6, 1, 0, '', 1500798249),
(2084, 33, 1, 5, 1, 0, '', 1500798252),
(2085, 35, 1, 4, 1, 0, '', 1500798255),
(2086, 6, 1, 3, 1, 0, '', 1500798262),
(2087, 7, 1, 6, 1, 0, '', 1500798263),
(2088, 6, 1, 2, 1, 0, '', 1500798266),
(2089, 1, 1, 1, 1, 0, '', 1500798274),
(2090, 33, 1, 5, 1, 0, '', 1500798277),
(2091, 7, 1, 6, 1, 0, '', 1500798277),
(2092, 35, 1, 4, 1, 0, '', 1500798278),
(2093, 6, 1, 3, 1, 0, '', 1500798291),
(2094, 7, 1, 6, 1, 0, '', 1500798291),
(2095, 6, 1, 2, 1, 0, '', 1500798293),
(2096, 35, 1, 4, 1, 0, '', 1500798301),
(2097, 33, 1, 5, 1, 0, '', 1500798302),
(2098, 7, 1, 6, 1, 0, '', 1500798305),
(2099, 1, 1, 1, 1, 0, '', 1500798309),
(2100, 7, 1, 6, 1, 0, '', 1500798319),
(2101, 6, 1, 2, 1, 0, '', 1500798320),
(2102, 6, 1, 3, 1, 0, '', 1500798320),
(2103, 35, 1, 4, 1, 0, '', 1500798332),
(2104, 33, 1, 5, 1, 0, '', 1500798332),
(2105, 7, 1, 6, 1, 0, '', 1500798333),
(2106, 1, 1, 1, 1, 0, '', 1500798344),
(2107, 6, 1, 2, 1, 0, '', 1500798347),
(2108, 35, 1, 4, 1, 0, '', 1500798347),
(2109, 6, 1, 3, 1, 0, '', 1500798349),
(2110, 33, 1, 5, 1, 0, '', 1500798352),
(2111, 35, 1, 4, 1, 0, '', 1500798370),
(2112, 6, 1, 2, 1, 0, '', 1500798374),
(2113, 33, 1, 5, 1, 0, '', 1500798377),
(2114, 6, 1, 3, 1, 0, '', 1500798378),
(2115, 1, 1, 1, 1, 0, '', 1500798379),
(2116, 7, 1, 6, 1, 0, '', 1500798382),
(2117, 35, 1, 4, 1, 0, '', 1500798393),
(2118, 7, 1, 6, 1, 0, '', 1500798396),
(2119, 6, 1, 2, 1, 0, '', 1500798401),
(2120, 33, 1, 5, 1, 0, '', 1500798402),
(2121, 6, 1, 3, 1, 0, '', 1500798407),
(2122, 7, 1, 6, 1, 0, '', 1500798410),
(2123, 1, 1, 1, 1, 0, '', 1500798414),
(2124, 35, 1, 4, 1, 0, '', 1500798416),
(2125, 7, 1, 6, 1, 0, '', 1500798424),
(2126, 33, 1, 5, 1, 0, '', 1500798427),
(2127, 6, 1, 2, 1, 0, '', 1500798428),
(2128, 6, 1, 3, 1, 0, '', 1500798436),
(2129, 7, 1, 6, 1, 0, '', 1500798438),
(2130, 35, 1, 4, 1, 0, '', 1500798439),
(2131, 1, 1, 1, 1, 0, '', 1500798449),
(2132, 7, 1, 6, 1, 0, '', 1500798452),
(2133, 33, 1, 5, 1, 0, '', 1500798452),
(2134, 6, 1, 2, 1, 0, '', 1500798455),
(2135, 35, 1, 4, 1, 0, '', 1500798462),
(2136, 6, 1, 3, 1, 0, '', 1500798465),
(2137, 7, 1, 6, 1, 0, '', 1500798466),
(2138, 33, 1, 5, 1, 0, '', 1500798477),
(2139, 7, 1, 6, 1, 0, '', 1500798480),
(2140, 6, 1, 2, 1, 0, '', 1500798482),
(2141, 1, 1, 1, 1, 0, '', 1500798484),
(2142, 35, 1, 4, 1, 0, '', 1500798485),
(2143, 6, 1, 3, 1, 0, '', 1500798494),
(2144, 7, 1, 6, 1, 0, '', 1500798494),
(2145, 33, 1, 5, 1, 0, '', 1500798502),
(2146, 7, 1, 6, 1, 0, '', 1500798508),
(2147, 35, 1, 4, 1, 0, '', 1500798508),
(2148, 6, 1, 2, 1, 0, '', 1500798509),
(2149, 1, 1, 1, 1, 0, '', 1500798519),
(2150, 7, 1, 6, 1, 0, '', 1500798522),
(2151, 6, 1, 3, 1, 0, '', 1500798523),
(2152, 33, 1, 5, 1, 0, '', 1500798527),
(2153, 35, 1, 4, 1, 0, '', 1500798531),
(2154, 7, 1, 6, 1, 0, '', 1500798536),
(2155, 6, 1, 2, 1, 0, '', 1500798536),
(2156, 6, 1, 3, 1, 0, '', 1501320742),
(2157, 1, 1, 1, 1, 0, '', 1501320745),
(2158, 33, 1, 5, 1, 0, '', 1501320752),
(2159, 7, 1, 6, 1, 0, '', 1501320754),
(2160, 35, 1, 4, 1, 0, '', 1501320754),
(2161, 6, 1, 2, 1, 0, '', 1501320756),
(2162, 6, 1, 3, 1, 0, '', 1501320833),
(2163, 7, 1, 6, 1, 0, '', 1501320833),
(2164, 35, 1, 4, 1, 0, '', 1501320839),
(2165, 33, 1, 5, 1, 0, '', 1501320839),
(2166, 6, 1, 2, 1, 0, '', 1501320845),
(2167, 7, 1, 6, 1, 0, '', 1501320847),
(2168, 1, 1, 1, 1, 0, '', 1501320848),
(2169, 35, 1, 4, 1, 0, '', 1501320852),
(2170, 6, 1, 3, 1, 0, '', 1501320852),
(2171, 33, 1, 5, 1, 0, '', 1501320854),
(2172, 7, 1, 6, 1, 0, '', 1501320861),
(2173, 6, 1, 2, 1, 0, '', 1501320862),
(2174, 35, 1, 4, 1, 0, '', 1501320865),
(2175, 33, 1, 5, 1, 0, '', 1501320869),
(2176, 6, 1, 3, 1, 0, '', 1501320871),
(2177, 7, 1, 6, 1, 0, '', 1501320875),
(2178, 35, 1, 4, 1, 0, '', 1501320878),
(2179, 1, 1, 1, 1, 0, '', 1501320879),
(2180, 6, 1, 2, 1, 0, '', 1501320879),
(2181, 33, 1, 5, 1, 0, '', 1501320892),
(2182, 7, 1, 6, 1, 0, '', 1501320892),
(2183, 6, 1, 3, 1, 0, '', 1501320892),
(2184, 35, 1, 4, 1, 0, '', 1501320892),
(2185, 6, 1, 2, 1, 0, '', 1501320896),
(2186, 33, 1, 5, 1, 0, '', 1501320899),
(2187, 7, 1, 6, 1, 0, '', 1501320903),
(2188, 35, 1, 4, 1, 0, '', 1501320904),
(2189, 6, 1, 3, 1, 0, '', 1501320909),
(2190, 1, 1, 1, 1, 0, '', 1501320910),
(2191, 6, 1, 2, 1, 0, '', 1501320913),
(2192, 33, 1, 5, 1, 0, '', 1501320914),
(2193, 35, 1, 4, 1, 0, '', 1501320917),
(2194, 7, 1, 6, 1, 0, '', 1501320917);

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
  `windircalibration` int(1) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- 转存表中的数据 `hcusysengpar`
--

INSERT INTO `hcusysengpar` (`prjname`, `commbackhawlcon`, `hcudbhost`, `hcudbuser`, `hcudbpsw`, `hcudbname`, `hcudbport`, `seriesportformodbus`, `seriesportforgps`, `seriesportforpm25sharp`, `seriesportforqrprinter`, `svraddhttplocal`, `svraddsocketipdefault`, `svraddhttpdefault`, `svrnamedefault`, `cloudftppwdvideo`, `cloudftpuservideo`, `hcuvideoserverdir`, `hcuvideoserverhttp`, `debugmode`, `tracemode`, `browselautostartupflag`, `browselprog`, `browselstartupaddress`, `browselworkingoption`, `windircalibration`) VALUES
('HCU_PRJ_AQYCG10_335D', 1, 'localhost', 'root', '123456', 'hcudb', 3306, 0, 4, 2, 1, 'http://127.0.0.1/test.php', 'http://aaa.bbb/', 'http://www.hkrob.com/mfunhcu/l1mainentry/cloud_callback_hcu.php', 'AQ_HCU', 'avorion', 'Avorion321', '/usr/local/apache_arm/htdocs/avorion/', 'http://192.168.1.232:8000/avorion/', 31, 4, 0, 'firefox', 'http://localhost/yii2basic/web/index.php', '-', 0),
('HCU_PRJ_AQYCG20_RASBERRY', 1, 'localhost', 'root', '123456', 'hcudb', 3306, 0, 4, 1, 1, 'http://127.0.0.1/test.php', '112.64.16.27', 'http://www.hkrob.com/mfunhcu/l1mainentry/cloud_callback_wechat.php', 'XHZN_HCU', 'avorion', 'Avorion321', '/usr/local/apache_arm/htdocs/avorion/', 'http://192.168.1.232:8000/avorion/', 31, 4, 0, 'firefox', 'http://localhost/yii2basic/web/index.php', '-kiosk', 0),
('HCU_PRJ_AQYC_OBSOLETE', 1, 'localhost', 'root', '123456', 'hcudb', 3306, 0, 4, 2, 1, 'http://127.0.0.1/test.php', 'http://aaa.bbb/', 'http://www.hkrob.com/mfunhcu/l1mainentry/cloud_callback_hcu.php', 'AQ_HCU', 'avorion', 'Avorion321', '/usr/local/apache_arm/htdocs/avorion/', 'http://192.168.1.232:8000/avorion/', 31, 4, 0, 'firefox', 'http://localhost/yii2basic/web/index.php', '-', 0),
('HCU_PRJ_BFSC_CBU', 1, 'localhost', 'root', '123456', 'hcudb', 3306, 0, 4, 2, 1, 'http://127.0.0.1/test.php', '121.40.118.33', 'http://www.hkrob.com/mfunhcu/l1mainentry/cloud_callback_hcu.php', 'XHZN_HCU', 'avorion', 'Avorion321', '/usr/local/apache_arm/htdocs/avorion/', 'http://192.168.1.232:8000/avorion/', 28, 4, 0, 'chromium-browser', '--app=http://localhost/bfscui/index.html', '--no-sandbox --kiosk --password-store --start-fullscreen', 0),
('HCU_PRJ_CXGLACM', 1, 'localhost', 'root', '123456', 'hcudb', 3306, 0, 4, 2, 1, 'http://127.0.0.1/test.php', 'http://aaa.bbb/', 'http://www.hkrob.com/mfunhcu/l1mainentry/cloud_callback_hcu.php', 'AQ_HCU', 'avorion', 'Avorion321', '/usr/local/apache_arm/htdocs/avorion/', 'http://192.168.1.232:8000/avorion/', 31, 4, 0, 'firefox', 'http://localhost/yii2basic/web/index.php', '-', 0),
('HCU_PRJ_CXILC', 1, 'localhost', 'root', '123456', 'hcudb', 3306, 0, 4, 2, 1, 'http://127.0.0.1/test.php', 'http://aaa.bbb/', 'http://www.hkrob.com/mfunhcu/l1mainentry/cloud_callback_hcu.php', 'AQ_HCU', 'avorion', 'Avorion321', '/usr/local/apache_arm/htdocs/avorion/', 'http://192.168.1.232:8000/avorion/', 31, 4, 0, 'firefox', 'http://localhost/yii2basic/web/index.php', '-', 0),
('HCU_PRJ_GQYBG40', 1, 'localhost', 'root', '123456', 'hcudb', 3306, 0, 4, 2, 1, 'http://127.0.0.1/test.php', 'http://aaa.bbb/', 'http://www.hkrob.com/mfunhcu/l1mainentry/cloud_callback_hcu.php', 'AQ_HCU', 'avorion', 'Avorion321', '/usr/local/apache_arm/htdocs/avorion/', 'http://192.168.1.232:8000/avorion/', 31, 4, 0, 'firefox', 'http://localhost/yii2basic/web/index.php', '-', 0),
('HCU_PRJ_NBIOT_HPM_QG', 1, 'localhost', 'root', '123456', 'hcudb', 3306, 3, 1, 0, 1, 'http://127.0.0.1/test.php', 'http://aaa.bbb/', 'http://121.40.185.177/xhzn/mfunhcu/l1mainentry/cloud_callback_hcu.php', 'AQ_HCU', 'avorion', 'Avorion321', '/usr/local/apache_arm/htdocs/avorion/', 'http://192.168.1.232:8000/avorion/', 31, 4, 0, 'firefox', 'http://localhost/yii2basic/web/index.php', '-', 0),
('HCU_PRJ_NBIOT_LPM_CJ', 1, 'localhost', 'root', '123456', 'hcudb', 3306, 3, 1, 0, 1, 'http://127.0.0.1/test.php', 'http://aaa.bbb/', 'http://121.40.185.177/xhzn/mfunhcu/l1mainentry/cloud_callback_hcu.php', 'AQ_HCU', 'avorion', 'Avorion321', '/usr/local/apache_arm/htdocs/avorion/', 'http://192.168.1.232:8000/avorion/', 31, 4, 0, 'firefox', 'http://localhost/yii2basic/web/index.php', '-', 0),
('HCU_PRJ_OPWL_OTDR', 1, 'localhost', 'root', '123456', 'hcudb', 3306, 0, 4, 2, 1, 'http://127.0.0.1/test.php', 'http://aaa.bbb/', 'http://www.hkrob.com/mfunhcu/l1mainentry/cloud_callback_hcu.php', 'AQ_HCU', 'avorion', 'Avorion321', '/usr/local/apache_arm/htdocs/avorion/', 'http://192.168.1.232:8000/avorion/', 31, 4, 0, 'firefox', 'http://localhost/yii2basic/web/index.php', '-', 0),
('HCU_PRJ_TBSWRG30', 1, 'localhost', 'root', '123456', 'hcudb', 3306, 3, 1, 0, 1, 'http://127.0.0.1/test.php', 'http://aaa.bbb/', 'http://121.40.185.177/xhzn/mfunhcu/l1mainentry/cloud_callback_hcu.php', 'AQ_HCU', 'avorion', 'Avorion321', '/usr/local/apache_arm/htdocs/avorion/', 'http://192.168.1.232:8000/avorion/', 31, 4, 0, 'firefox', 'http://localhost/yii2basic/web/index.php', '-', 0),
('HCU_PRJ_TEST_MODE', 1, 'localhost', 'root', '123456', 'hcudb', 3306, 3, 1, 0, 1, 'http://127.0.0.1/test.php', 'http://aaa.bbb/', 'http://121.40.185.177/xhzn/mfunhcu/l1mainentry/cloud_callback_hcu.php', 'AQ_HCU', 'avorion', 'Avorion321', '/usr/local/apache_arm/htdocs/avorion/', 'http://192.168.1.232:8000/avorion/', 31, 4, 0, 'firefox', 'http://localhost/yii2basic/web/index.php', '-', 0),
('HCU_PRJ_BFDF_CBU', 1, 'localhost', 'root', '123456', 'hcudb', 3306, 0, 4, 2, 1, 'http://127.0.0.1/test.php', '121.40.118.33', 'http://www.hkrob.com/mfunhcu/l1mainentry/cloud_callback_hcu.php', 'XHZN_HCU', 'avorion', 'Avorion321', '/usr/local/apache_arm/htdocs/avorion/', 'http://192.168.1.232:8000/avorion/', 28, 4, 0, 'chromium-browser', '--app=http://localhost/bfscui/index.html', '--no-sandbox --kiosk --password-store --start-fullscreen', 0),
('HCU_PRJ_BFHS_CBU', 1, 'localhost', 'root', '123456', 'hcudb', 3306, 0, 4, 2, 1, 'http://127.0.0.1/test.php', '121.40.118.33', 'http://www.hkrob.com/mfunhcu/l1mainentry/cloud_callback_hcu.php', 'XHZN_HCU', 'avorion', 'Avorion321', '/usr/local/apache_arm/htdocs/avorion/', 'http://192.168.1.232:8000/avorion/', 28, 4, 0, 'chromium-browser', '--app=http://localhost/bfscui/index.html', '--no-sandbox --kiosk --password-store --start-fullscreen', 0);

-- --------------------------------------------------------

--
-- 表的结构 `hcusysengtimer`
--

CREATE TABLE `hcusysengtimer` (
  `timerid` int(2) NOT NULL,
  `timername` char(40) NOT NULL,
  `granularity` int(1) NOT NULL,
  `duration` int(1) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- 转存表中的数据 `hcusysengtimer`
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
(21, 'TID_ID_1S_HSMMP_PERIOD_CURL_PICTURE', 1, 3600),
(22, 'TID_1S_CLOUDVELA_PERIOD_LINK_HBT', 1, 10),
(23, 'TID_1S_NBIOTCJ188_PERIOD_LINK_HBT', 1, 600),
(24, 'TID_1S_NBIOTCJ188_SEND_DATA_BACK', 1, 10),
(25, 'TID_1S_NBIOTQG376_PERIOD_LINK_HBT', 1, 600),
(26, 'TID_1S_NBIOTQG376_SEND_DATA_BACK', 1, 10),
(27, 'TID_1S_LLCZHB_LINK_CTRL', 1, 10),
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
(39, 'TID_1S_SYSPM_PERIOD_WORKING', 1, 311),
(40, 'TID_1S_SYSSWM_PERIOD_WORKING', 1, 900),
(41, 'TID_1S_SYSSWM_SEG_DL_WAIT', 1, 3),
(42, 'TID_1S_CANITFLEO_WORKING_SCAN', 1, 10),
(43, 'TID_1S_L3BFSC_SYS_CFG_WAIT_FB', 1, 15),
(44, 'TID_1S_L3BFSC_SYS_START_WAIT_FB', 1, 15),
(45, 'TID_1S_L3BFSC_SYS_STOP_WAIT_FB', 1, 15),
(46, 'TID_1S_L3BFSC_TTT_WAIT_FB', 1, 60),
(47, 'TID_1S_L3BFSC_TGU_WAIT_FB', 1, 60),
(48, 'TID_1S_L3BFSC_ERROR_INQ', 1, 15),
(49, 'TID_1S_BFSCUICOMM_PERIOD_READ', 1, 5),
(50, 'TID_1S_L3BFDF_CFG_START_WAIT_FB', 1, 15),
(51, 'TID_1S_L3BFDF_SUSPEND_WAIT_FB', 1, 15),
(52, 'TID_1S_L3BFDF_RESUME_WAIT_FB', 1, 15),
(53, 'TID_1S_L3BFDF_TTT_WAIT_FB', 1, 3),
(54, 'TID_1S_BFDFUICOMM_PERIOD_READ', 1, 600),
(55, 'TID_1S_L3BFHS_CFG_START_WAIT_FB', 1, 5),
(56, 'TID_1S_L3BFHS_SUSPEND_WAIT_FB', 1, 5),
(57, 'TID_1S_L3BFHS_RESUME_WAIT_FB', 1, 5),
(58, 'TID_1S_BFHSUICOMM_PERIOD_READ', 1, 600),
(59, 'TID_1S_L3AQYCG10_PERIOD_READ', 1, 600),
(60, 'TID_1S_L3AQYCG20_PERIOD_READ', 1, 600),
(61, 'TID_1S_L3AQYCG20_PERIOD_STA_SCAN', 1, 30),
(62, 'TID_1S_L3AQYCG20_START_MIN_REPORT', 1, 60),
(63, 'TID_1S_L3AQYCG20_STOP_MIN_REPORT', 1, 60),
(64, 'TID_1S_L3AQYCG20_START_HOUR_REPORT', 1, 60),
(65, 'TID_1S_L3AQYCG20_STOP_HOUR_REPORT', 1, 60),
(66, 'TID_1S_L3AQYCG20_START_DAY_REPORT', 1, 60),
(67, 'TID_1S_L3AQYCG20_STOP_DAY_REPORT', 1, 60),
(68, 'TID_1S_L3TBSWRG30_PERIOD_READ', 1, 600),
(69, 'TID_1S_L3GQYBG40_PERIOD_READ', 1, 600),
(70, 'TID_1S_L3CXGLACM_PERIOD_READ', 1, 600),
(71, 'TID_1S_L3CXILC_PERIOD_READ', 1, 600),
(72, 'TID_1S_L3NBHPM_PERIOD_READ', 1, 600),
(73, 'TID_1S_L3NBLPM_PERIOD_READ', 1, 600),
(74, 'TID_1S_L3OPWLOTDR_PERIOD_READ', 1, 600),
(75, 'TID_1S_L3HATE_WORK_CTRL_FB', 1, 5),
(76, 'TID_1S_MAX', 1, 0),
(77, 'TID_10MS_MIN', 2, 0),
(78, 'TID_10MS_SVRCON_TEST', 2, 10000),
(79, 'TID_10MS_L3BFSC_PERIOD_STA_SCAN', 2, 50),
(80, 'TID_10MS_CANITFLEO_SIMULATION_DATA', 2, 40),
(81, 'TID_10MS_L3BFDF_PERIOD_STA_SCAN', 2, 50),
(82, 'TID_10MS_L3BFHS_PERIOD_STA_SCAN', 2, 50),
(83, 'TID_10MS_MAX', 2, 0),
(84, 'TID_1MS_MIN', 3, 0),
(85, 'TID_1MS_SVRCON_TEST', 3, 100),
(86, 'TID_1MS_MAX', 3, 0),
(87, 'TID_MAX', 1, 0);

-- --------------------------------------------------------

--
-- 表的结构 `hcusyspmglobaldatainfo`
--

CREATE TABLE `hcusyspmglobaldatainfo` (
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
  `restartcnt` int(4) NOT NULL,
  `cloudvelaconncnt` int(4) NOT NULL,
  `cloudvelaconnfailcnt` int(4) NOT NULL,
  `cloudveladisccnt` int(4) NOT NULL,
  `clouddatatimeoutcnt` int(4) NOT NULL,
  `socketdisccnt` int(4) NOT NULL,
  `cpuoccupy` int(4) NOT NULL,
  `memoccupy` int(11) NOT NULL,
  `diskoccupy` int(11) NOT NULL,
  `cputemp` int(11) NOT NULL,
  `timestamp` int(4) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- 转存表中的数据 `hcusyspmglobaldatainfo`
--

INSERT INTO `hcusyspmglobaldatainfo` (`sid`, `taskhcuvmec`, `taskhwinvec`, `tasksyspmec`, `taskmodbusec`, `taskcloudvelaec`, `taskavorionec`, `taskspsvirgoec`, `taskhsmmpec`, `taskemcec`, `taskpm25ec`, `taskwinddirec`, `taskwindspdec`, `tasktempec`, `taskhumidec`, `tasknoiseec`, `taskairprsec`, `taskco1ec`, `tasklightstrec`, `taskalcoholec`, `taskhchoec`, `tasktoxicgasec`, `restartcnt`, `cloudvelaconncnt`, `cloudvelaconnfailcnt`, `cloudveladisccnt`, `clouddatatimeoutcnt`, `socketdisccnt`, `cpuoccupy`, `memoccupy`, `diskoccupy`, `cputemp`, `timestamp`) VALUES
(143, 0, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 12, 0, 0, 0, 43, 5, 61, 56, 1500786595),
(144, 0, 45, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 23, 0, 2, 0, 5, 3, 61, 55, 1500786706),
(145, 0, 69, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 34, 0, 4, 0, 54, 2, 61, 59, 1500786817),
(146, 0, 90, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 45, 0, 6, 0, 34, 3, 61, 59, 1500786928),
(147, 0, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 12, 0, 1, 0, 11, 2, 61, 55, 1500787189),
(148, 0, 45, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 23, 0, 3, 0, 24, 2, 61, 55, 1500787300),
(149, 0, 69, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 34, 0, 4, 0, 10, 3, 61, 54, 1500787411),
(150, 0, 90, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 45, 0, 7, 0, 4, 3, 61, 54, 1500787522),
(151, 0, 114, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 56, 0, 9, 0, 1, 3, 61, 54, 1500787633),
(152, 0, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 12, 0, 1, 0, 6, 4, 61, 53, 1500787857),
(153, 0, 45, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 23, 0, 3, 0, 2, 4, 61, 53, 1500787968),
(154, 0, 69, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 34, 0, 5, 0, 11, 5, 61, 54, 1500788079),
(155, 0, 90, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 45, 0, 7, 0, 24, 0, 61, 55, 1500788190),
(156, 0, 111, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 56, 0, 9, 0, 14, 1, 61, 55, 1500788301),
(157, 0, 135, 0, 6, 18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 0, 67, 0, 13, 0, 2, 3, 61, 54, 1500788412),
(158, 0, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 12, 0, 0, 0, 2, 3, 61, 54, 1500788635),
(159, 0, 45, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 23, 0, 2, 0, 31, 4, 61, 56, 1500788746),
(160, 0, 69, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 34, 0, 3, 0, 35, 5, 61, 56, 1500788857),
(161, 0, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 12, 0, 1, 0, 36, 3, 61, 60, 1500789110),
(162, 0, 45, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 23, 0, 1, 0, 32, 4, 61, 61, 1500789221),
(163, 0, 69, 0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 34, 0, 1, 0, 53, 1, 61, 61, 1500789332),
(164, 0, 90, 0, 0, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 45, 0, 2, 0, 37, 0, 61, 60, 1500789443),
(165, 0, 114, 0, 0, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 56, 0, 3, 0, 13, 0, 61, 59, 1500789554),
(166, 0, 135, 0, 6, 26, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 0, 67, 0, 8, 0, 36, 6, 61, 58, 1500789665),
(167, 0, 156, 0, 6, 28, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 78, 0, 11, 0, 29, 9, 61, 57, 1500789776),
(168, 0, 180, 0, 6, 28, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 89, 0, 14, 0, 70, 9, 61, 60, 1500789887),
(169, 0, 201, 0, 6, 30, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 0, 100, 0, 16, 0, 20, 11, 61, 57, 1500789998),
(170, 0, 24, 0, 12, 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 14, 0, 5, 0, 27, 12, 61, 57, 1500790211),
(171, 0, 27, 0, 13, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 16, 0, 7, 0, 28, 12, 61, 57, 1500790232),
(172, 0, 81, 0, 40, 43, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 35, 1, 26, 0, 27, 12, 61, 57, 1500790322),
(173, 0, 123, 0, 78, 81, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 0, 57, 1, 45, 0, 76, 8, 61, 60, 1500790532),
(174, 0, 183, 0, 143, 133, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 0, 86, 1, 89, 0, 0, 6, 61, 56, 1500790832),
(175, 0, 243, 0, 217, 193, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, 0, 116, 1, 137, 0, 23, 5, 61, 56, 1500791132),
(176, 0, 303, 0, 295, 269, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 11, 0, 146, 1, 182, 0, 17, 7, 61, 55, 1500791432),
(177, 0, 363, 0, 370, 325, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 14, 0, 176, 1, 232, 0, 14, 9, 61, 55, 1500791732),
(178, 0, 423, 0, 447, 381, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, 0, 206, 1, 286, 0, 9, 4, 61, 56, 1500792032),
(179, 0, 483, 0, 524, 451, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 18, 0, 236, 1, 330, 0, 7, 3, 61, 55, 1500792332),
(180, 0, 543, 0, 601, 509, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 20, 0, 266, 1, 382, 0, 21, 11, 61, 56, 1500792632),
(181, 0, 603, 0, 678, 567, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 23, 0, 296, 1, 434, 0, 49, 12, 61, 59, 1500792932),
(182, 0, 663, 0, 754, 629, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 25, 0, 326, 1, 485, 0, 25, 13, 61, 57, 1500793232),
(183, 0, 723, 0, 831, 689, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 27, 0, 356, 1, 536, 0, 25, 14, 61, 57, 1500793532),
(184, 0, 783, 0, 906, 745, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 0, 386, 1, 587, 0, 31, 14, 61, 59, 1500793832),
(185, 0, 843, 0, 984, 821, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 32, 0, 416, 1, 631, 0, 47, 15, 61, 59, 1500794132),
(186, 0, 903, 0, 1059, 885, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 34, 0, 446, 1, 679, 0, 62, 10, 61, 60, 1500794432),
(187, 0, 963, 0, 1136, 935, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 36, 0, 476, 1, 734, 0, 24, 11, 61, 58, 1500794732),
(188, 0, 1023, 0, 1213, 995, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 39, 0, 506, 1, 786, 0, 47, 5, 61, 59, 1500795032),
(189, 0, 1083, 0, 1288, 1057, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 41, 0, 536, 1, 833, 0, 35, 6, 61, 60, 1500795332),
(190, 0, 1143, 0, 1366, 1127, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 43, 0, 566, 1, 881, 0, 30, 7, 61, 60, 1500795632),
(191, 0, 1203, 0, 1442, 1183, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 46, 0, 596, 1, 933, 0, 31, 8, 61, 60, 1500795932),
(192, 0, 1263, 0, 1519, 1265, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 48, 0, 626, 1, 973, 0, 58, 9, 61, 60, 1500796232),
(193, 0, 1323, 0, 1594, 1325, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 50, 0, 656, 1, 1022, 0, 33, 10, 61, 60, 1500796532),
(194, 0, 1383, 0, 1672, 1395, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 52, 0, 686, 1, 1069, 0, 18, 8, 61, 60, 1500796832),
(195, 0, 1443, 0, 1748, 1465, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 55, 0, 716, 1, 1114, 0, 24, 9, 61, 57, 1500797132),
(196, 0, 1503, 0, 1824, 1519, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 57, 0, 746, 1, 1168, 0, 25, 10, 61, 59, 1500797432),
(197, 0, 1563, 0, 1901, 1583, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 59, 0, 776, 1, 1216, 0, 30, 11, 61, 59, 1500797732),
(198, 0, 1623, 0, 1976, 1651, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 62, 0, 806, 1, 1262, 0, 10, 10, 61, 57, 1500798032),
(199, 0, 1683, 0, 2055, 1721, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 64, 0, 836, 1, 1308, 0, 39, 7, 61, 59, 1500798332),
(200, 0, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 3, 6, 17, 35, 0, 1501120797),
(201, 0, 45, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 4, 9, 17, 35, 0, 1501120908),
(202, 0, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 7, 17, 35, 0, 1501121068),
(203, 0, 45, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 6, 5, 17, 35, 0, 1501121179),
(204, 0, 69, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 6, 26, 17, 35, 0, 1501121289),
(205, 0, 90, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 6, 87, 17, 35, 0, 1501121400),
(206, 0, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 32, 17, 35, 0, 1501121627),
(207, 0, 45, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 9, 17, 35, 0, 1501121737),
(208, 0, 69, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 2, 12, 17, 35, 0, 1501121848),
(209, 0, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 52, 18, 35, 0, 1501122272),
(210, 0, 45, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 2, 2, 18, 35, 0, 1501122383),
(211, 0, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 17, 18, 35, 0, 1501122586),
(212, 0, 45, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 2, 7, 18, 35, 0, 1501122698),
(213, 0, 69, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 4, 42, 18, 35, 0, 1501122809),
(214, 0, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 94, 18, 35, 0, 1501122952),
(215, 0, 45, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 3, 31, 18, 35, 0, 1501123063),
(216, 0, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 2, 1, 18, 35, 0, 1501123294),
(217, 0, 45, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 2, 2, 18, 35, 0, 1501123405),
(218, 0, 69, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 4, 2, 18, 35, 0, 1501123516),
(219, 0, 90, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 8, 43, 17, 35, 0, 1501123628),
(220, 0, 114, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 10, 30, 18, 35, 0, 1501123738),
(221, 0, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 2, 88, 18, 35, 0, 1501123928),
(222, 0, 45, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 6, 97, 18, 35, 0, 1501124039),
(223, 0, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 45, 18, 35, 0, 1501124181),
(224, 0, 45, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 2, 8, 18, 35, 0, 1501124292),
(225, 0, 69, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 5, 9, 18, 35, 0, 1501124403),
(226, 0, 90, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 6, 7, 18, 35, 0, 1501124514),
(227, 0, 114, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 7, 20, 18, 35, 0, 1501124626),
(228, 0, 135, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 10, 7, 18, 35, 0, 1501124737),
(229, 0, 156, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 11, 8, 18, 35, 0, 1501124847),
(230, 0, 180, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 11, 7, 18, 35, 0, 1501124958),
(231, 0, 201, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 11, 7, 18, 35, 0, 1501125070),
(232, 0, 225, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 12, 7, 18, 35, 0, 1501125180),
(233, 0, 246, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 13, 53, 18, 35, 0, 1501125291),
(234, 0, 267, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 13, 8, 18, 35, 0, 1501125403),
(235, 0, 291, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 14, 7, 18, 35, 0, 1501125514),
(236, 0, 312, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 14, 7, 18, 35, 0, 1501125625),
(237, 0, 336, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 15, 8, 18, 35, 0, 1501125735),
(238, 0, 357, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 16, 42, 18, 35, 0, 1501125847),
(239, 0, 378, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 20, 30, 18, 35, 0, 1501125958),
(240, 0, 402, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 5, 1, 0, 35, 7, 18, 35, 0, 1501126069),
(241, 0, 423, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 5, 1, 0, 36, 7, 18, 35, 0, 1501126180),
(242, 0, 447, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 5, 1, 0, 37, 6, 18, 35, 0, 1501126290),
(243, 0, 468, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 5, 1, 0, 38, 7, 18, 35, 0, 1501126402),
(244, 0, 489, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 5, 1, 0, 38, 7, 18, 35, 0, 1501126513),
(245, 0, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 2, 63, 18, 35, 0, 1501126681),
(246, 0, 45, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 4, 7, 18, 35, 0, 1501126791),
(247, 0, 69, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 5, 7, 18, 35, 0, 1501126902),
(248, 0, 90, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 6, 7, 18, 35, 0, 1501127013),
(249, 0, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 2, 9, 18, 35, 0, 1501127231),
(250, 0, 45, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 2, 8, 18, 35, 0, 1501127343),
(251, 0, 69, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 2, 7, 18, 35, 0, 1501127454),
(252, 0, 90, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 7, 7, 18, 35, 0, 1501127564),
(253, 0, 114, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 7, 7, 18, 35, 0, 1501127676),
(254, 0, 135, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 8, 88, 18, 35, 0, 1501127786),
(255, 0, 156, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 9, 75, 18, 35, 0, 1501127898),
(256, 0, 180, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 10, 85, 18, 35, 0, 1501128008),
(257, 0, 201, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 10, 95, 18, 35, 0, 1501128119),
(258, 0, 225, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 14, 49, 18, 35, 0, 1501128230),
(259, 0, 246, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 17, 16, 18, 35, 0, 1501128341),
(260, 0, 267, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 19, 16, 18, 35, 0, 1501128452),
(261, 0, 291, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 19, 12, 18, 35, 0, 1501128564),
(262, 0, 312, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 19, 7, 18, 35, 0, 1501128675),
(263, 0, 336, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 20, 7, 18, 35, 0, 1501128786),
(264, 0, 357, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 20, 8, 18, 35, 0, 1501128897),
(265, 0, 378, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 21, 7, 18, 35, 0, 1501129008),
(266, 0, 402, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 21, 7, 18, 35, 0, 1501129118),
(267, 0, 423, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 23, 7, 18, 35, 0, 1501129229),
(268, 0, 447, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 23, 7, 18, 35, 0, 1501129340),
(269, 0, 468, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 25, 8, 18, 35, 0, 1501129451),
(270, 0, 489, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 26, 7, 18, 35, 0, 1501129562),
(271, 0, 513, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 26, 7, 18, 35, 0, 1501129673),
(272, 0, 534, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 26, 6, 18, 35, 0, 1501129784),
(273, 0, 558, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 26, 6, 18, 35, 0, 1501129896),
(274, 0, 579, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 26, 7, 18, 35, 0, 1501130006),
(275, 0, 600, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 26, 7, 18, 35, 0, 1501130118),
(276, 0, 624, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 26, 6, 18, 35, 0, 1501130229),
(277, 0, 645, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 26, 7, 18, 35, 0, 1501130339),
(278, 0, 669, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 26, 7, 18, 35, 0, 1501130450),
(279, 0, 690, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 27, 7, 18, 35, 0, 1501130561),
(280, 0, 711, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 27, 6, 18, 35, 0, 1501130672),
(281, 0, 735, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 27, 7, 18, 35, 0, 1501130783),
(282, 0, 756, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 27, 9, 18, 35, 0, 1501130895),
(283, 0, 777, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 28, 7, 18, 35, 0, 1501131005),
(284, 0, 801, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 29, 8, 18, 35, 0, 1501131116),
(285, 0, 822, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 30, 6, 18, 35, 0, 1501131228),
(286, 0, 846, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 30, 7, 18, 35, 0, 1501131338),
(287, 0, 867, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 1, 0, 46, 7, 18, 35, 0, 1501131449),
(288, 0, 888, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 1, 0, 47, 7, 18, 35, 0, 1501131561),
(289, 0, 912, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 1, 0, 49, 7, 18, 35, 0, 1501131671),
(290, 0, 933, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 1, 0, 50, 7, 18, 35, 0, 1501131782),
(291, 0, 957, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 1, 0, 50, 7, 18, 35, 0, 1501131894),
(292, 0, 978, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 1, 0, 51, 6, 18, 35, 0, 1501132005),
(293, 0, 999, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 1, 0, 52, 7, 18, 35, 0, 1501132115),
(294, 0, 1023, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 1, 0, 52, 7, 18, 35, 0, 1501132226),
(295, 0, 1044, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 1, 0, 54, 9, 18, 35, 0, 1501132338),
(296, 0, 1068, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 1, 0, 54, 10, 18, 35, 0, 1501132448),
(297, 0, 1089, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 1, 0, 55, 7, 18, 35, 0, 1501132559),
(298, 0, 1110, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 1, 0, 55, 7, 18, 35, 0, 1501132671),
(299, 0, 1134, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 1, 0, 55, 6, 18, 35, 0, 1501132782),
(300, 0, 1155, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 1, 0, 56, 7, 18, 35, 0, 1501132893),
(301, 0, 1179, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 1, 0, 57, 6, 18, 35, 0, 1501133003),
(302, 0, 1200, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 1, 0, 59, 7, 18, 35, 0, 1501133115),
(303, 0, 1221, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 1, 0, 60, 7, 18, 35, 0, 1501133225),
(304, 0, 1245, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 1, 0, 60, 7, 18, 35, 0, 1501133336),
(305, 0, 1266, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 1, 0, 60, 6, 18, 35, 0, 1501133447),
(306, 0, 1290, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 1, 0, 61, 6, 18, 35, 0, 1501133559),
(307, 0, 1311, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 1, 0, 62, 7, 18, 35, 0, 1501133670),
(308, 0, 1332, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 1, 0, 62, 7, 18, 35, 0, 1501133781),
(309, 0, 1356, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 1, 0, 62, 9, 18, 35, 0, 1501133892),
(310, 0, 1377, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 1, 0, 62, 7, 18, 35, 0, 1501134003),
(311, 0, 1401, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 1, 0, 63, 9, 18, 35, 0, 1501134114),
(312, 0, 1422, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 1, 0, 63, 7, 18, 35, 0, 1501134225),
(313, 0, 1443, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 1, 0, 63, 7, 18, 35, 0, 1501134336),
(314, 0, 1467, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 1, 0, 64, 7, 18, 35, 0, 1501134446),
(315, 0, 1488, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 1, 0, 65, 7, 18, 35, 0, 1501134557),
(316, 0, 1512, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 1, 0, 66, 6, 18, 35, 0, 1501134668),
(317, 0, 1533, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 1, 0, 66, 7, 18, 35, 0, 1501134779),
(318, 0, 1554, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 1, 0, 67, 7, 18, 35, 0, 1501134890),
(319, 0, 1578, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 1, 0, 67, 7, 18, 35, 0, 1501135002),
(320, 0, 1599, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 1, 0, 67, 7, 18, 35, 0, 1501135112),
(321, 0, 1623, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 1, 0, 67, 6, 18, 35, 0, 1501135223),
(322, 0, 1644, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 1, 0, 67, 9, 18, 35, 0, 1501135334),
(323, 0, 1665, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 1, 0, 67, 7, 18, 35, 0, 1501135445),
(324, 0, 1689, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 1, 0, 67, 7, 18, 35, 0, 1501135556),
(325, 0, 1710, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 1, 0, 67, 7, 18, 35, 0, 1501135668),
(326, 0, 1734, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 1, 0, 67, 6, 18, 35, 0, 1501135778),
(327, 0, 1755, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 2, 2, 0, 89, 7, 18, 35, 0, 1501135889),
(328, 0, 1776, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 2, 2, 0, 89, 6, 18, 35, 0, 1501136001),
(329, 0, 1800, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 2, 2, 0, 89, 7, 18, 35, 0, 1501136111),
(330, 0, 1821, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 2, 2, 0, 89, 7, 18, 35, 0, 1501136223),
(331, 0, 1845, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 2, 2, 0, 89, 6, 18, 35, 0, 1501136333),
(332, 0, 1866, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 2, 2, 0, 89, 7, 18, 35, 0, 1501136445),
(333, 0, 1887, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 2, 2, 0, 89, 7, 18, 35, 0, 1501136555),
(334, 0, 1911, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 2, 2, 0, 89, 7, 18, 35, 0, 1501136667),
(335, 0, 1932, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 2, 2, 0, 89, 8, 18, 35, 0, 1501136778),
(336, 0, 1953, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 2, 2, 0, 89, 9, 18, 35, 0, 1501136888),
(337, 0, 1977, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 2, 2, 0, 89, 7, 18, 35, 0, 1501137000),
(338, 0, 1998, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 2, 2, 0, 90, 7, 18, 35, 0, 1501137111),
(339, 0, 2022, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 2, 2, 0, 91, 6, 18, 35, 0, 1501137222),
(340, 0, 2043, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 2, 2, 0, 91, 7, 18, 35, 0, 1501137332),
(341, 0, 2064, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 2, 2, 0, 91, 6, 18, 35, 0, 1501137443),
(342, 0, 2088, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 2, 2, 0, 91, 7, 18, 35, 0, 1501137554),
(343, 0, 2109, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 2, 2, 0, 91, 7, 18, 35, 0, 1501137665),
(344, 0, 2133, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 2, 2, 0, 92, 7, 18, 35, 0, 1501137776),
(345, 0, 2154, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 2, 2, 0, 92, 6, 18, 35, 0, 1501137887),
(346, 0, 2175, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 2, 2, 0, 92, 6, 18, 35, 0, 1501137998),
(347, 0, 2199, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 2, 2, 0, 92, 7, 18, 35, 0, 1501138110),
(348, 0, 2220, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 2, 2, 0, 94, 7, 18, 35, 0, 1501138221),
(349, 0, 2244, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 2, 2, 0, 95, 9, 18, 35, 0, 1501138331),
(350, 0, 2265, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 2, 2, 0, 95, 7, 18, 35, 0, 1501138442),
(351, 0, 2286, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 2, 2, 0, 95, 7, 18, 35, 0, 1501138553),
(352, 0, 2310, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 2, 2, 0, 95, 6, 18, 35, 0, 1501138665),
(353, 0, 2331, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 2, 2, 0, 95, 7, 18, 35, 0, 1501138776),
(354, 0, 2355, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 2, 2, 0, 96, 7, 18, 35, 0, 1501138886),
(355, 0, 2376, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 2, 2, 0, 96, 7, 18, 35, 0, 1501138997),
(356, 0, 2397, 0, 0, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 0, 114, 6, 18, 35, 0, 1501139108),
(357, 0, 2421, 0, 0, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 3, 3, 0, 114, 8, 18, 35, 0, 1501139219),
(358, 0, 2442, 0, 0, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 3, 3, 0, 114, 7, 18, 35, 0, 1501139330),
(359, 0, 2466, 0, 0, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 3, 3, 0, 115, 7, 18, 35, 0, 1501139441),
(360, 0, 2487, 0, 0, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 3, 3, 0, 116, 7, 18, 35, 0, 1501139552),
(361, 0, 2508, 0, 0, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 3, 3, 0, 116, 7, 18, 35, 0, 1501139664),
(362, 0, 2532, 0, 0, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 3, 3, 0, 116, 9, 18, 35, 0, 1501139774),
(363, 0, 2553, 0, 0, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 3, 3, 0, 116, 7, 18, 35, 0, 1501139886),
(364, 0, 2577, 0, 0, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 3, 3, 0, 116, 6, 18, 35, 0, 1501139997),
(365, 0, 2598, 0, 0, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 3, 3, 0, 116, 7, 18, 35, 0, 1501140108),
(366, 0, 2619, 0, 0, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 3, 3, 0, 116, 7, 18, 35, 0, 1501140218),
(367, 0, 2643, 0, 0, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 3, 3, 0, 117, 7, 18, 35, 0, 1501140329),
(368, 0, 2664, 0, 0, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 3, 3, 0, 117, 7, 18, 35, 0, 1501140440),
(369, 0, 2688, 0, 0, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 3, 3, 0, 118, 7, 18, 35, 0, 1501140552),
(370, 0, 2709, 0, 0, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 3, 3, 0, 118, 7, 18, 35, 0, 1501140663),
(371, 0, 2730, 0, 0, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 3, 3, 0, 120, 7, 18, 35, 0, 1501140774),
(372, 0, 2754, 0, 0, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 3, 3, 0, 120, 6, 18, 35, 0, 1501140884),
(373, 0, 2775, 0, 0, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 3, 3, 0, 121, 7, 18, 35, 0, 1501140995),
(374, 0, 2799, 0, 0, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 3, 3, 0, 121, 6, 18, 35, 0, 1501141106),
(375, 0, 2820, 0, 0, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 3, 3, 0, 122, 7, 18, 35, 0, 1501141218),
(376, 0, 2841, 0, 0, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 7, 4, 0, 167, 9, 18, 35, 0, 1501141328),
(377, 0, 2865, 0, 0, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 7, 4, 0, 168, 7, 18, 35, 0, 1501141440),
(378, 0, 2886, 0, 0, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 7, 4, 0, 169, 7, 18, 35, 0, 1501141551),
(379, 0, 2910, 0, 0, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 7, 4, 0, 170, 7, 18, 35, 0, 1501141661),
(380, 0, 2931, 0, 0, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 7, 4, 0, 170, 7, 18, 35, 0, 1501141772),
(381, 0, 2952, 0, 0, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 7, 4, 0, 170, 6, 18, 35, 0, 1501141884),
(382, 0, 2976, 0, 0, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 7, 4, 0, 171, 7, 18, 35, 0, 1501141994),
(383, 0, 2997, 0, 0, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 7, 4, 0, 171, 6, 18, 35, 0, 1501142105),
(384, 0, 3018, 0, 0, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 7, 4, 0, 171, 8, 18, 35, 0, 1501142217),
(385, 0, 3042, 0, 0, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 7, 4, 0, 171, 6, 18, 35, 0, 1501142327),
(386, 0, 3063, 0, 0, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 7, 4, 0, 171, 7, 18, 35, 0, 1501142439),
(387, 0, 3087, 0, 0, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 7, 4, 0, 171, 7, 18, 35, 0, 1501142550),
(388, 0, 3108, 0, 0, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 7, 4, 0, 171, 7, 18, 35, 0, 1501142660),
(389, 0, 3129, 0, 0, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 7, 4, 0, 171, 8, 18, 35, 0, 1501142772),
(390, 0, 3153, 0, 0, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 7, 4, 0, 171, 7, 18, 35, 0, 1501142882),
(391, 0, 3174, 0, 0, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 7, 4, 0, 171, 7, 18, 35, 0, 1501142993),
(392, 0, 3198, 0, 0, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 7, 4, 0, 172, 6, 18, 35, 0, 1501143105),
(393, 0, 3219, 0, 0, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 7, 4, 0, 173, 3, 18, 35, 0, 1501143215),
(394, 0, 3240, 0, 0, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 7, 4, 0, 174, 1, 18, 35, 0, 1501143326),
(395, 0, 3264, 0, 0, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 7, 4, 0, 174, 1, 18, 35, 0, 1501143437),
(396, 0, 3285, 0, 0, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 7, 4, 0, 175, 38, 18, 35, 0, 1501143548),
(397, 0, 3309, 0, 0, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 7, 4, 0, 175, 11, 18, 35, 0, 1501143659),
(398, 0, 3330, 0, 0, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 7, 4, 0, 176, 89, 18, 35, 0, 1501143770),
(399, 0, 3351, 0, 0, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 7, 4, 0, 177, 89, 18, 35, 0, 1501143881),
(400, 0, 3375, 0, 0, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 7, 4, 0, 178, 1, 18, 35, 0, 1501143993),
(401, 0, 3396, 0, 0, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 7, 4, 0, 180, 1, 18, 35, 0, 1501144104),
(402, 0, 3420, 0, 0, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 7, 4, 0, 181, 2, 18, 35, 0, 1501144214),
(403, 0, 3441, 0, 0, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 7, 4, 0, 181, 1, 18, 35, 0, 1501144325),
(404, 0, 3462, 0, 0, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 7, 4, 0, 181, 1, 18, 35, 0, 1501144436),
(405, 0, 3486, 0, 0, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 7, 4, 0, 181, 1, 18, 35, 0, 1501144548),
(406, 0, 3507, 0, 0, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 7, 4, 0, 182, 1, 18, 35, 0, 1501144658),
(407, 0, 3531, 0, 0, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 7, 4, 0, 182, 1, 18, 35, 0, 1501144769),
(408, 0, 3552, 0, 0, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 7, 4, 0, 183, 1, 18, 35, 0, 1501144880),
(409, 0, 3573, 0, 0, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 7, 4, 0, 185, 16, 18, 35, 0, 1501144992),
(410, 0, 3597, 0, 0, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 7, 4, 0, 185, 1, 18, 35, 0, 1501145102),
(411, 0, 3618, 0, 0, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 7, 4, 0, 186, 3, 18, 35, 0, 1501145213),
(412, 0, 3642, 0, 0, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 7, 4, 0, 187, 1, 18, 35, 0, 1501145325),
(413, 0, 3663, 0, 0, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 7, 4, 0, 187, 1, 18, 35, 0, 1501145436),
(414, 0, 3684, 0, 0, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 7, 4, 0, 188, 1, 18, 35, 0, 1501145546),
(415, 0, 3708, 0, 0, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 7, 4, 0, 188, 2, 18, 35, 0, 1501145658),
(416, 0, 3729, 0, 0, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 7, 4, 0, 189, 6, 18, 35, 0, 1501145769),
(417, 0, 3753, 0, 0, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 7, 4, 0, 191, 1, 18, 35, 0, 1501145880),
(418, 0, 3774, 0, 0, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 7, 4, 0, 194, 1, 18, 35, 0, 1501145990),
(419, 0, 3795, 0, 0, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 7, 4, 0, 194, 1, 18, 35, 0, 1501146101),
(420, 0, 3819, 0, 0, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 7, 4, 0, 195, 1, 18, 35, 0, 1501146212),
(421, 0, 3840, 0, 0, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 7, 4, 0, 195, 1, 18, 35, 0, 1501146324),
(422, 0, 3864, 0, 0, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 7, 4, 0, 195, 1, 18, 35, 0, 1501146434),
(423, 0, 3885, 0, 0, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 7, 4, 0, 195, 3, 18, 35, 0, 1501146545),
(424, 0, 3906, 0, 0, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 7, 4, 0, 195, 1, 18, 35, 0, 1501146657),
(425, 0, 3930, 0, 0, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 7, 4, 0, 196, 1, 18, 35, 0, 1501146768),
(426, 0, 3951, 0, 0, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 7, 4, 0, 196, 1, 18, 35, 0, 1501146878),
(427, 0, 3975, 0, 0, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 7, 4, 0, 197, 1, 18, 35, 0, 1501146989),
(428, 0, 3996, 0, 0, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 7, 4, 0, 197, 2, 18, 35, 0, 1501147100),
(429, 0, 4017, 0, 0, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 7, 4, 0, 197, 3, 18, 35, 0, 1501147211),
(430, 0, 4041, 0, 0, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 7, 4, 0, 199, 1, 18, 35, 0, 1501147322),
(431, 0, 4062, 0, 0, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 7, 4, 0, 199, 1, 18, 35, 0, 1501147433),
(432, 0, 4086, 0, 0, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 7, 4, 0, 201, 1, 18, 35, 0, 1501147544),
(433, 0, 4107, 0, 0, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 7, 4, 0, 201, 1, 18, 35, 0, 1501147656),
(434, 0, 4128, 0, 0, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 7, 4, 0, 203, 1, 18, 35, 0, 1501147766),
(435, 0, 312, 4, 12, 28, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 11, 0, 156, 0, 22, 0, 8, 18, 63, 53, 1501320463),
(436, 0, 333, 4, 12, 28, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 12, 0, 167, 0, 24, 0, 26, 15, 63, 54, 1501320574),
(437, 0, 354, 4, 12, 28, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 13, 0, 178, 0, 26, 0, 6, 15, 63, 53, 1501320685),
(438, 0, 24, 0, 19, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 12, 0, 14, 0, 4, 14, 63, 53, 1501320892),
(439, 0, 1371, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 2, 21, 89, 0, 1502803452),
(440, 0, 1401, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502803763),
(441, 0, 1431, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 18, 21, 89, 0, 1502804073),
(442, 0, 1464, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 36, 21, 89, 0, 1502804385),
(443, 0, 1494, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 21, 89, 0, 1502804696),
(444, 0, 1524, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502805007),
(445, 0, 1557, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502805318),
(446, 0, 1587, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502805629),
(447, 0, 1620, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502805940),
(448, 0, 1650, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502806251),
(449, 0, 1680, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502806562),
(450, 0, 1713, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502806873),
(451, 0, 1743, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502807184),
(452, 0, 1773, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502807495),
(453, 0, 1806, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502807806),
(454, 0, 1836, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502808117),
(455, 0, 1866, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502808428),
(456, 0, 1899, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502808739),
(457, 0, 1929, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502809050),
(458, 0, 1962, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502809361),
(459, 0, 1992, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502809672),
(460, 0, 2022, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502809983),
(461, 0, 2055, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502810294),
(462, 0, 2085, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502810605),
(463, 0, 2115, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502810916),
(464, 0, 2148, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502811227),
(465, 0, 2178, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502811538),
(466, 0, 2208, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502811849),
(467, 0, 2241, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502812160),
(468, 0, 2271, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502812471),
(469, 0, 2304, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502812782),
(470, 0, 2334, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502813093),
(471, 0, 2364, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502813404),
(472, 0, 2397, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502813715),
(473, 0, 2427, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502814026),
(474, 0, 2457, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502814337),
(475, 0, 2490, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502814648),
(476, 0, 2520, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502814959),
(477, 0, 2550, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502815270),
(478, 0, 2583, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502815580),
(479, 0, 2613, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502815891),
(480, 0, 2646, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502816202),
(481, 0, 2676, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502816513),
(482, 0, 2706, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502816824),
(483, 0, 2739, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502817135),
(484, 0, 2769, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502817446),
(485, 0, 2799, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502817757),
(486, 0, 2832, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502818068),
(487, 0, 2862, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502818379),
(488, 0, 2892, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502818690),
(489, 0, 2925, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502819001),
(490, 0, 2955, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502819313),
(491, 0, 2988, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502819623),
(492, 0, 3018, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502819934),
(493, 0, 3048, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502820245),
(494, 0, 3081, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502820556),
(495, 0, 3111, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502820867),
(496, 0, 3141, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502821178),
(497, 0, 3174, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502821489),
(498, 0, 3204, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502821800),
(499, 0, 3234, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502822111),
(500, 0, 3267, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502822422),
(501, 0, 3297, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502822733),
(502, 0, 3330, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502823044),
(503, 0, 3360, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502823355),
(504, 0, 3390, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502823666),
(505, 0, 3423, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502823977),
(506, 0, 3453, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502824288),
(507, 0, 3483, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502824599),
(508, 0, 3516, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502824910),
(509, 0, 3546, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502825221),
(510, 0, 3579, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502825532),
(511, 0, 3609, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502825843),
(512, 0, 3639, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502826154),
(513, 0, 3672, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502826465),
(514, 0, 3702, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502826776),
(515, 0, 3732, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502827087),
(516, 0, 3765, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502827399),
(517, 0, 3795, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502827709),
(518, 0, 3825, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502828020),
(519, 0, 3858, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502828331),
(520, 0, 3888, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502828642),
(521, 0, 3921, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502828953),
(522, 0, 3951, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502829265),
(523, 0, 3981, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502829575),
(524, 0, 4014, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502829886),
(525, 0, 4044, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502830197),
(526, 0, 4074, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502830508),
(527, 0, 4107, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502830819),
(528, 0, 4137, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502831131),
(529, 0, 4167, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502831441),
(530, 0, 4200, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502831752),
(531, 0, 4230, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502832063),
(532, 0, 4263, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502832374),
(533, 0, 4293, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502832685),
(534, 0, 4323, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502832996),
(535, 0, 4356, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502833307),
(536, 0, 4386, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502833618),
(537, 0, 4416, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502833929),
(538, 0, 4449, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502834240),
(539, 0, 4479, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502834551),
(540, 0, 4509, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502834862),
(541, 0, 4542, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502835173),
(542, 0, 4572, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502835484),
(543, 0, 4605, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502835795),
(544, 0, 4635, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502836106),
(545, 0, 4665, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502836417),
(546, 0, 4698, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502836728),
(547, 0, 4728, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502837039),
(548, 0, 4758, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502837350),
(549, 0, 4791, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 21, 89, 0, 1502837661),
(550, 0, 4821, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502837972),
(551, 0, 4851, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502838283),
(552, 0, 4884, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502838594),
(553, 0, 4914, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502838905),
(554, 0, 4947, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502839216),
(555, 0, 4977, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502839527),
(556, 0, 5007, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502839838),
(557, 0, 5040, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 21, 89, 0, 1502840149),
(558, 0, 5070, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 22, 89, 0, 1502840460),
(559, 0, 5100, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 22, 89, 0, 1502840771),
(560, 0, 5133, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 22, 89, 0, 1502841082),
(561, 0, 5163, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 22, 89, 0, 1502841393),
(562, 0, 5193, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 22, 89, 0, 1502841704),
(563, 0, 5226, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 22, 89, 0, 1502842015),
(564, 0, 5256, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 22, 89, 0, 1502842326),
(565, 0, 5289, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 22, 89, 0, 1502842638),
(566, 0, 5319, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 22, 89, 0, 1502842948);
INSERT INTO `hcusyspmglobaldatainfo` (`sid`, `taskhcuvmec`, `taskhwinvec`, `tasksyspmec`, `taskmodbusec`, `taskcloudvelaec`, `taskavorionec`, `taskspsvirgoec`, `taskhsmmpec`, `taskemcec`, `taskpm25ec`, `taskwinddirec`, `taskwindspdec`, `tasktempec`, `taskhumidec`, `tasknoiseec`, `taskairprsec`, `taskco1ec`, `tasklightstrec`, `taskalcoholec`, `taskhchoec`, `tasktoxicgasec`, `restartcnt`, `cloudvelaconncnt`, `cloudvelaconnfailcnt`, `cloudveladisccnt`, `clouddatatimeoutcnt`, `socketdisccnt`, `cpuoccupy`, `memoccupy`, `diskoccupy`, `cputemp`, `timestamp`) VALUES
(567, 0, 5349, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 22, 89, 0, 1502843259),
(568, 0, 5382, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 22, 89, 0, 1502843570),
(569, 0, 5412, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 22, 89, 0, 1502843881),
(570, 0, 5442, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 22, 89, 0, 1502844192),
(571, 0, 5475, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 22, 89, 0, 1502844503),
(572, 0, 5505, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 22, 89, 0, 1502844814),
(573, 0, 5538, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 22, 89, 0, 1502845125),
(574, 0, 5568, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 22, 89, 0, 1502845436),
(575, 0, 33, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 10, 22, 89, 0, 1502846068),
(576, 0, 33, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 7, 22, 89, 0, 1502846458),
(577, 0, 63, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 1, 0, 5, 3, 22, 89, 0, 1502846769),
(578, 0, 33, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 17, 22, 89, 0, 1502847204),
(579, 0, 33, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 13, 22, 89, 0, 1502848821),
(580, 0, 33, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 6, 22, 89, 0, 1502849530),
(581, 0, 33, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 23, 89, 0, 1502849947),
(582, 0, 63, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 23, 89, 0, 1502850258),
(583, 0, 96, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 23, 89, 0, 1502850569),
(584, 0, 126, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 23, 89, 0, 1502850880),
(585, 0, 156, 0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 12, 23, 89, 0, 1502851191),
(586, 0, 33, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 3, 0, 23, 89, 0, 1502851629),
(587, 0, 63, 0, 0, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 1, 0, 4, 10, 23, 89, 0, 1502851940),
(588, 0, 96, 0, 0, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 1, 0, 4, 21, 23, 89, 0, 1502852251),
(589, 0, 33, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 3, 23, 89, 0, 1502852583),
(590, 0, 33, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 23, 89, 0, 1502853010),
(591, 0, 33, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 23, 89, 0, 1502853593),
(592, 0, 33, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 15, 89, 0, 1502855089),
(593, 0, 63, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 15, 89, 0, 1502855400),
(594, 0, 96, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 15, 89, 0, 1502855711),
(595, 0, 126, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 15, 89, 0, 1502856022),
(596, 0, 156, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 23, 15, 89, 0, 1502856333),
(597, 0, 33, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 14, 89, 0, 1502856866),
(598, 0, 63, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 14, 89, 0, 1502857176),
(599, 0, 96, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 14, 89, 0, 1502857487),
(600, 0, 126, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 14, 89, 0, 1502857798),
(601, 0, 156, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 2, 14, 89, 0, 1502858109),
(602, 0, 189, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 14, 89, 0, 1502858420),
(603, 0, 219, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 15, 89, 0, 1502858731),
(604, 0, 249, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 15, 89, 0, 1502859042),
(605, 0, 282, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 15, 89, 0, 1502859353),
(606, 0, 312, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 15, 89, 0, 1502859664),
(607, 0, 345, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 15, 89, 0, 1502859975),
(608, 0, 375, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 15, 89, 0, 1502860286),
(609, 0, 405, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 15, 89, 0, 1502860598),
(610, 0, 33, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 15, 89, 0, 1502861027),
(611, 0, 63, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 4, 15, 89, 0, 1502861338),
(612, 0, 96, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 15, 89, 0, 1502861649),
(613, 0, 126, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 15, 89, 0, 1502861960),
(614, 0, 156, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 15, 89, 0, 1502862271),
(615, 0, 189, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 15, 89, 0, 1502862582),
(616, 0, 219, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 15, 89, 0, 1502862893),
(617, 0, 252, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 15, 89, 0, 1502863204),
(618, 0, 282, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 15, 89, 0, 1502863515),
(619, 0, 312, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 15, 89, 0, 1502863826),
(620, 0, 345, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 15, 89, 0, 1502864137),
(621, 0, 375, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 15, 89, 0, 1502864448),
(622, 0, 405, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 1, 0, 11, 0, 15, 89, 0, 1502864759),
(623, 0, 438, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 1, 0, 11, 0, 15, 89, 0, 1502865070),
(624, 0, 468, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 1, 0, 11, 0, 15, 89, 0, 1502865382),
(625, 0, 498, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 1, 0, 11, 2, 15, 89, 0, 1502865692),
(626, 0, 33, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 8, 15, 89, 0, 1502866241),
(627, 0, 33, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 3, 16, 90, 0, 1502875584),
(628, 0, 33, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 17, 90, 0, 1502876384),
(629, 0, 63, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 17, 90, 0, 1502876694),
(630, 0, 96, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 3, 17, 90, 0, 1502877006),
(631, 0, 126, 0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 2, 17, 90, 0, 1502877316),
(632, 0, 156, 0, 0, 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 17, 90, 0, 1502877627),
(633, 0, 189, 0, 0, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 17, 90, 0, 1502877938),
(634, 0, 219, 0, 0, 18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 7, 17, 90, 0, 1502878249),
(635, 0, 249, 0, 0, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 3, 17, 90, 0, 1502878560),
(636, 0, 282, 0, 0, 22, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 10, 17, 90, 0, 1502878871),
(637, 0, 312, 0, 0, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 17, 90, 0, 1502879182),
(638, 0, 33, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 4, 17, 90, 0, 1502879779),
(639, 0, 63, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 13, 17, 90, 0, 1502880089),
(640, 0, 33, 0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 6, 17, 90, 0, 1502880788),
(641, 0, 63, 0, 0, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 11, 17, 90, 0, 1502881099),
(642, 0, 33, 0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 4, 17, 90, 0, 1502881516),
(643, 0, 63, 0, 0, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 2, 17, 90, 0, 1502881826),
(644, 0, 96, 0, 0, 30, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 13, 17, 90, 0, 1502882137),
(645, 0, 63, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 23, 13, 82, 0, 1503199704),
(646, 0, 126, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 6, 13, 82, 0, 1503200016),
(647, 0, 189, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 7, 13, 82, 0, 1503200327),
(648, 0, 249, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 19, 18, 83, 0, 1503200638),
(649, 0, 312, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 3, 18, 83, 0, 1503200949),
(650, 0, 375, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 18, 18, 83, 0, 1503201260),
(651, 0, 438, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 10, 17, 83, 0, 1503201571),
(652, 0, 498, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 12, 17, 83, 0, 1503201882),
(653, 0, 561, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 2, 17, 83, 0, 1503202193),
(654, 0, 624, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 17, 83, 0, 1503202504),
(655, 0, 684, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 2, 17, 83, 0, 1503202815),
(656, 0, 747, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 2, 17, 83, 0, 1503203126),
(657, 0, 810, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 17, 83, 0, 1503203437),
(658, 0, 873, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 2, 17, 83, 0, 1503203748),
(659, 0, 933, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 2, 17, 83, 0, 1503204059),
(660, 0, 3732, 0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 33, 42, 16, 33, 0, 1503387842),
(661, 0, 3795, 0, 0, 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 34, 15, 15, 33, 0, 1503388154),
(662, 0, 3855, 0, 0, 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 35, 4, 15, 33, 0, 1503388464),
(663, 0, 3918, 0, 0, 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 35, 82, 16, 33, 0, 1503388775),
(664, 0, 3981, 0, 0, 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 35, 2, 16, 33, 0, 1503389086),
(665, 0, 4041, 0, 0, 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 38, 13, 16, 33, 0, 1503389397),
(666, 0, 264, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 1, 21, 0, 0, 2, 0, 7, 32, 0, 1504691507),
(667, 0, 327, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 1, 21, 0, 0, 3, 2, 7, 32, 0, 1504691818),
(668, 0, 390, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 1, 21, 0, 0, 4, 0, 7, 32, 0, 1504692129),
(669, 0, 453, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 1, 21, 0, 0, 5, 0, 7, 32, 0, 1504692440),
(670, 0, 513, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 1, 21, 0, 0, 6, 0, 7, 32, 0, 1504692751),
(671, 0, 576, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 1, 21, 0, 0, 7, 1, 7, 32, 0, 1504693062),
(672, 0, 639, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 1, 21, 0, 0, 8, 1, 7, 32, 0, 1504693373),
(673, 0, 699, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 1, 21, 0, 0, 9, 1, 7, 32, 0, 1504693684),
(674, 0, 762, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 1, 21, 0, 0, 12, 1, 7, 32, 0, 1504694209),
(675, 0, 825, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 1, 21, 0, 0, 12, 1, 7, 32, 0, 1504694521),
(676, 0, 888, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 1, 21, 0, 0, 12, 0, 7, 32, 0, 1504694831),
(677, 0, 948, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 1, 21, 0, 0, 15, 1, 7, 32, 0, 1504695856),
(678, 0, 1011, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 1, 21, 0, 0, 15, 1, 7, 32, 0, 1504696168),
(679, 0, 1074, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 1, 21, 0, 0, 15, 0, 7, 32, 0, 1504696479),
(680, 0, 1134, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 1, 21, 0, 0, 15, 0, 7, 32, 0, 1504696790),
(681, 0, 1197, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 1, 21, 0, 0, 15, 1, 7, 32, 0, 1504697100),
(682, 0, 1260, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 1, 21, 0, 0, 15, 1, 7, 32, 0, 1504697412),
(683, 0, 1323, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 1, 21, 0, 0, 15, 1, 7, 32, 0, 1504697723),
(684, 0, 1383, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 1, 21, 0, 0, 15, 0, 7, 32, 0, 1504698034),
(685, 0, 1446, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 2, 22, 1, 0, 25, 0, 7, 32, 0, 1504699361),
(686, 0, 1509, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 2, 22, 1, 0, 25, 0, 7, 32, 0, 1504699671),
(687, 0, 1569, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 2, 22, 1, 0, 25, 0, 7, 32, 0, 1504699983),
(688, 0, 1632, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 2, 22, 1, 0, 25, 15, 7, 32, 0, 1504700293),
(689, 0, 72, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 32, 0, 0, 0, 0, 7, 32, 0, 1504700870),
(690, 0, 135, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 1, 57, 0, 0, 1, 0, 7, 32, 0, 1504701180),
(691, 0, 198, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 1, 57, 0, 0, 1, 0, 7, 32, 0, 1504701491),
(692, 0, 258, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 1, 57, 0, 0, 1, 0, 7, 32, 0, 1504701802),
(693, 0, 321, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 1, 57, 0, 0, 1, 0, 7, 32, 0, 1504702114),
(694, 0, 384, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 1, 57, 0, 0, 1, 0, 7, 32, 0, 1504702425),
(695, 0, 447, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 1, 57, 0, 0, 1, 0, 7, 32, 0, 1504702736),
(696, 0, 507, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 1, 57, 0, 0, 1, 0, 7, 32, 0, 1504703047),
(697, 0, 570, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 1, 57, 0, 0, 1, 7, 7, 32, 0, 1504703357),
(698, 0, 72, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 32, 0, 0, 0, 0, 8, 32, 0, 1504704021),
(699, 0, 135, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 60, 0, 0, 1, 19, 8, 32, 0, 1504704331),
(700, 0, 72, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 32, 0, 0, 0, 10, 8, 32, 0, 1504704713),
(701, 0, 63, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 15, 8, 31, 0, 1506516670),
(702, 0, 63, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 8, 9, 26, 0, 1507696860),
(703, 0, 126, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 9, 11, 26, 0, 1507697171),
(704, 0, 189, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 9, 12, 26, 0, 1507697482),
(705, 0, 252, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 20, 13, 26, 0, 1507697793),
(706, 0, 312, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 43, 13, 26, 0, 1507698105),
(707, 0, 375, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 7, 14, 26, 0, 1507698415),
(708, 0, 438, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 9, 14, 26, 0, 1507698726),
(709, 0, 498, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 9, 15, 26, 0, 1507699037),
(710, 0, 561, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 90, 5, 26, 0, 1507699349),
(711, 0, 63, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 12, 12, 26, 0, 1507702464),
(712, 0, 63, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 7, 16, 26, 0, 1507702867),
(713, 0, 126, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 22, 16, 26, 0, 1507703178),
(714, 0, 189, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 6, 18, 26, 0, 1507703488),
(715, 0, 63, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 14, 8, 26, 0, 1507704281),
(716, 0, 126, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 6, 10, 26, 0, 1507704592),
(717, 0, 189, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 6, 13, 26, 0, 1507704903),
(718, 0, 252, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 7, 14, 26, 0, 1507705214),
(719, 0, 312, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 8, 15, 26, 0, 1507705525),
(720, 0, 375, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 7, 16, 26, 0, 1507705836),
(721, 0, 438, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 8, 17, 26, 0, 1507706147),
(722, 0, 63, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 8, 10, 26, 0, 1507710809),
(723, 0, 126, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 2, 7, 13, 26, 0, 1507711119),
(724, 0, 189, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 2, 7, 15, 26, 0, 1507711430),
(725, 0, 252, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 2, 7, 16, 26, 0, 1507711741),
(726, 0, 312, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 2, 8, 17, 26, 0, 1507712052),
(727, 0, 375, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 3, 7, 18, 26, 0, 1507712363),
(728, 0, 438, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 3, 8, 19, 26, 0, 1507712674),
(729, 0, 498, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 5, 7, 20, 26, 0, 1507712985),
(730, 0, 561, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 5, 9, 20, 26, 0, 1507713296),
(731, 0, 624, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 5, 8, 19, 26, 0, 1507713607),
(732, 0, 687, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 6, 8, 19, 26, 0, 1507713918),
(733, 0, 747, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 6, 8, 19, 26, 0, 1507714230),
(734, 0, 810, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 6, 7, 20, 26, 0, 1507714540),
(735, 0, 873, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 6, 51, 19, 26, 0, 1507714851),
(736, 0, 936, 0, 0, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 29, 1, 0, 78, 25, 18, 26, 0, 1507715162),
(737, 0, 996, 0, 0, 17, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 60, 1, 0, 182, 7, 18, 26, 0, 1507715473),
(738, 0, 1059, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 91, 1, 0, 286, 7, 18, 26, 0, 1507715784),
(739, 0, 1122, 0, 0, 31, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 122, 1, 0, 389, 7, 18, 26, 0, 1507716095),
(740, 0, 1182, 0, 0, 37, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 154, 1, 0, 493, 7, 18, 26, 0, 1507716406),
(741, 0, 1245, 0, 0, 45, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 185, 1, 0, 597, 7, 18, 26, 0, 1507716717),
(742, 0, 63, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 63, 8, 26, 0, 1507717683),
(743, 0, 63, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 8, 12, 26, 0, 1507718047),
(744, 0, 63, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 5, 15, 26, 0, 1507718666),
(745, 0, 126, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 21, 16, 26, 0, 1507718976),
(746, 0, 63, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 3, 8, 26, 0, 1507721876),
(747, 0, 63, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 47, 12, 26, 0, 1507722439),
(748, 0, 63, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 4, 13, 26, 0, 1507722959),
(749, 0, 126, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 4, 14, 26, 0, 1507723269),
(750, 0, 189, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 3, 14, 26, 0, 1507723580),
(751, 0, 252, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 3, 15, 26, 0, 1507723892),
(752, 0, 63, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 46, 0, 32, 0, 0, 0, 10, 7, 26, 0, 1507724573),
(753, 0, 126, 0, 0, 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 93, 0, 63, 0, 0, 0, 9, 8, 26, 0, 1507724884),
(754, 0, 189, 0, 0, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 139, 0, 94, 0, 0, 0, 9, 10, 26, 0, 1507725195),
(755, 0, 252, 0, 0, 26, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 186, 0, 125, 0, 0, 0, 10, 11, 26, 0, 1507725506),
(756, 0, 63, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 4, 8, 26, 0, 1507726407),
(757, 0, 63, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 4, 6, 26, 0, 1507727073),
(758, 0, 63, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 4, 12, 26, 0, 1507727788),
(759, 0, 126, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 4, 14, 26, 0, 1507728099),
(760, 0, 189, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 35, 17, 26, 0, 1507728411),
(761, 0, 63, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 12, 12, 26, 0, 1507730426),
(762, 0, 126, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 11, 14, 26, 0, 1507730737),
(763, 0, 189, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 11, 15, 26, 0, 1507731048),
(764, 0, 63, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 27, 7, 26, 0, 1507731694),
(765, 0, 63, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 46, 0, 32, 0, 0, 0, 12, 10, 24, 0, 1508218079),
(766, 0, 63, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 50, 13, 24, 0, 1508222122),
(767, 0, 126, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 4, 16, 24, 0, 1508222432),
(768, 0, 189, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 4, 17, 25, 0, 1508222743),
(769, 0, 63, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 4, 8, 24, 0, 1508227265),
(770, 0, 126, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 4, 10, 24, 0, 1508227576),
(771, 0, 189, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 4, 12, 24, 0, 1508227887),
(772, 0, 252, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 4, 14, 24, 0, 1508228198),
(773, 0, 312, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 3, 15, 24, 0, 1508228509),
(774, 0, 375, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 7, 15, 24, 0, 1508228821),
(775, 0, 438, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 4, 15, 24, 0, 1508229131),
(776, 0, 498, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 4, 16, 24, 0, 1508229442),
(777, 0, 561, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 4, 17, 24, 0, 1508229753),
(778, 0, 624, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 3, 17, 24, 0, 1508230064),
(779, 0, 687, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 4, 17, 24, 0, 1508230375),
(780, 0, 747, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 4, 18, 24, 0, 1508230686),
(781, 0, 810, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 4, 18, 24, 0, 1508230997),
(782, 0, 873, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 4, 18, 24, 0, 1508231308),
(783, 0, 936, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 4, 18, 24, 0, 1508231619),
(784, 0, 996, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 4, 18, 24, 0, 1508231930),
(785, 0, 1059, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 4, 18, 24, 0, 1508232241),
(786, 0, 1122, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 16, 1, 0, 58, 4, 18, 24, 0, 1508232552),
(787, 0, 1182, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 48, 1, 0, 162, 4, 18, 24, 0, 1508232863),
(788, 0, 1245, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 79, 1, 0, 265, 4, 18, 24, 0, 1508233174),
(789, 0, 1308, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 110, 1, 0, 369, 4, 18, 24, 0, 1508233485),
(790, 0, 1371, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 141, 1, 0, 473, 4, 18, 24, 0, 1508233796),
(791, 0, 1431, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 172, 1, 0, 576, 4, 18, 24, 0, 1508234107),
(792, 0, 1494, 0, 0, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 203, 1, 0, 680, 4, 19, 24, 0, 1508234418),
(793, 0, 1557, 0, 0, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 234, 1, 0, 783, 5, 19, 24, 0, 1508234729),
(794, 0, 1617, 0, 0, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 265, 1, 0, 868, 3, 18, 24, 0, 1508235040),
(795, 0, 1680, 0, 0, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 296, 1, 0, 1075, 4, 18, 24, 0, 1508235351),
(796, 0, 1743, 0, 0, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 327, 1, 0, 1179, 4, 18, 24, 0, 1508235662),
(797, 0, 1806, 0, 0, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 359, 1, 0, 1283, 4, 18, 24, 0, 1508235973),
(798, 0, 1866, 0, 0, 13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 390, 1, 0, 1386, 4, 18, 24, 0, 1508236284),
(799, 0, 1929, 0, 0, 13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 421, 1, 0, 1490, 4, 18, 24, 0, 1508236595),
(800, 0, 1992, 0, 0, 13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 452, 1, 0, 1594, 4, 18, 24, 0, 1508236906),
(801, 0, 2055, 0, 0, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 483, 1, 0, 1697, 4, 18, 24, 0, 1508237217),
(802, 0, 2115, 0, 0, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 514, 1, 0, 1801, 4, 18, 24, 0, 1508237528),
(803, 0, 2178, 0, 0, 17, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 545, 1, 0, 1905, 4, 18, 24, 0, 1508237839),
(804, 0, 2241, 0, 0, 17, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 576, 1, 0, 2008, 4, 18, 24, 0, 1508238150),
(805, 0, 2301, 0, 0, 17, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 607, 1, 0, 2112, 4, 18, 24, 0, 1508238461),
(806, 0, 2364, 0, 0, 19, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 638, 1, 0, 2215, 4, 18, 24, 0, 1508238772),
(807, 0, 2427, 0, 0, 19, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 670, 1, 0, 2319, 4, 18, 24, 0, 1508239083),
(808, 0, 2490, 0, 0, 19, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 701, 1, 0, 2423, 4, 18, 24, 0, 1508239394),
(809, 0, 2550, 0, 0, 21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 732, 1, 0, 2526, 4, 18, 24, 0, 1508239705),
(810, 0, 2613, 0, 0, 21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 763, 1, 0, 2630, 4, 18, 24, 0, 1508240016),
(811, 0, 2676, 0, 0, 21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 794, 1, 0, 2734, 4, 18, 24, 0, 1508240327),
(812, 0, 2739, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 825, 1, 0, 2837, 3, 18, 24, 0, 1508240638),
(813, 0, 2799, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 856, 1, 0, 2941, 4, 18, 24, 0, 1508240949),
(814, 0, 2862, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 887, 1, 0, 3045, 4, 18, 24, 0, 1508241260),
(815, 0, 2925, 0, 0, 25, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 918, 1, 0, 3148, 4, 18, 24, 0, 1508241571),
(816, 0, 2985, 0, 0, 25, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 949, 1, 0, 3252, 4, 18, 24, 0, 1508241882),
(817, 0, 3048, 0, 0, 25, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 981, 1, 0, 3356, 4, 18, 24, 0, 1508242193),
(818, 0, 3111, 0, 0, 27, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1012, 1, 0, 3459, 4, 18, 24, 0, 1508242504),
(819, 0, 3174, 0, 0, 27, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1043, 1, 0, 3563, 4, 18, 24, 0, 1508242815),
(820, 0, 3234, 0, 0, 27, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1074, 1, 0, 3666, 5, 18, 24, 0, 1508243126),
(821, 0, 3297, 0, 0, 29, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1105, 1, 0, 3770, 5, 18, 24, 0, 1508243437),
(822, 0, 3360, 0, 0, 29, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1136, 1, 0, 3874, 4, 18, 24, 0, 1508243748),
(823, 0, 3420, 0, 0, 29, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1167, 1, 0, 3977, 4, 18, 24, 0, 1508244059),
(824, 0, 3483, 0, 0, 31, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1198, 1, 0, 4081, 4, 18, 24, 0, 1508244370),
(825, 0, 3546, 0, 0, 31, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1229, 1, 0, 4185, 4, 18, 24, 0, 1508244681),
(826, 0, 3609, 0, 0, 31, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1260, 1, 0, 4288, 3, 18, 24, 0, 1508244992),
(827, 0, 3669, 0, 0, 33, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1292, 1, 0, 4392, 4, 18, 24, 0, 1508245303),
(828, 0, 3732, 0, 0, 33, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1323, 1, 0, 4496, 3, 18, 24, 0, 1508245614),
(829, 0, 3795, 0, 0, 33, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1354, 1, 0, 4599, 5, 18, 24, 0, 1508245925),
(830, 0, 3858, 0, 0, 35, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1385, 1, 0, 4703, 4, 18, 24, 0, 1508246236),
(831, 0, 3918, 0, 0, 35, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1416, 1, 0, 4807, 4, 18, 24, 0, 1508246547),
(832, 0, 3981, 0, 0, 37, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1447, 1, 0, 4910, 4, 18, 24, 0, 1508246858),
(833, 0, 4044, 0, 0, 37, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1478, 1, 0, 5014, 3, 18, 24, 0, 1508247169),
(834, 0, 4104, 0, 0, 37, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1509, 1, 0, 5118, 4, 18, 24, 0, 1508247480),
(835, 0, 4167, 0, 0, 39, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1540, 1, 0, 5221, 4, 18, 24, 0, 1508247791),
(836, 0, 4230, 0, 0, 39, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1571, 1, 0, 5325, 4, 18, 24, 0, 1508248102),
(837, 0, 4293, 0, 0, 39, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1603, 1, 0, 5428, 4, 18, 24, 0, 1508248413),
(838, 0, 4353, 0, 0, 41, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1634, 1, 0, 5532, 4, 18, 25, 0, 1508248724),
(839, 0, 4416, 0, 0, 41, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1665, 1, 0, 5636, 4, 18, 25, 0, 1508249035),
(840, 0, 4479, 0, 0, 41, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1696, 1, 0, 5739, 5, 18, 25, 0, 1508249346),
(841, 0, 4542, 0, 0, 43, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1727, 1, 0, 5843, 4, 18, 25, 0, 1508249657),
(842, 0, 4602, 0, 0, 43, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1758, 1, 0, 5947, 5, 18, 25, 0, 1508249968),
(843, 0, 4665, 0, 0, 43, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1789, 1, 0, 6050, 4, 18, 25, 0, 1508250279),
(844, 0, 4728, 0, 0, 45, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1820, 1, 0, 6154, 4, 18, 25, 0, 1508250590),
(845, 0, 4788, 0, 0, 45, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1851, 1, 0, 6258, 4, 18, 25, 0, 1508250901),
(846, 0, 4851, 0, 0, 45, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1882, 1, 0, 6361, 4, 18, 25, 0, 1508251212),
(847, 0, 4914, 0, 0, 47, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1914, 1, 0, 6465, 5, 17, 25, 0, 1508251523),
(848, 0, 4977, 0, 0, 47, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1945, 1, 0, 6569, 4, 17, 25, 0, 1508251834),
(849, 0, 5037, 0, 0, 47, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1976, 1, 0, 6672, 4, 17, 25, 0, 1508252145),
(850, 0, 5100, 0, 0, 49, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2007, 1, 0, 6776, 4, 17, 25, 0, 1508252456),
(851, 0, 5163, 0, 0, 49, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2038, 1, 0, 6879, 4, 17, 25, 0, 1508252767),
(852, 0, 5223, 0, 0, 49, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2069, 1, 0, 6983, 4, 17, 25, 0, 1508253078),
(853, 0, 5286, 0, 0, 51, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2100, 1, 0, 7087, 4, 17, 25, 0, 1508253389),
(854, 0, 5349, 0, 0, 51, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2131, 1, 0, 7190, 4, 17, 25, 0, 1508253700),
(855, 0, 5412, 0, 0, 51, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2162, 1, 0, 7294, 4, 17, 25, 0, 1508254011),
(856, 0, 5472, 0, 0, 53, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2193, 1, 0, 7398, 4, 17, 25, 0, 1508254322),
(857, 0, 5535, 0, 0, 53, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2225, 1, 0, 7501, 4, 17, 25, 0, 1508254633),
(858, 0, 5598, 0, 0, 55, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2256, 1, 0, 7605, 4, 17, 25, 0, 1508254944),
(859, 0, 5661, 0, 0, 55, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2287, 1, 0, 7709, 4, 17, 25, 0, 1508255255),
(860, 0, 5721, 0, 0, 55, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2318, 1, 0, 7812, 3, 17, 25, 0, 1508255566),
(861, 0, 5784, 0, 0, 57, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2349, 1, 0, 7916, 4, 17, 25, 0, 1508255877),
(862, 0, 5847, 0, 0, 57, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2380, 1, 0, 8020, 4, 17, 25, 0, 1508256188),
(863, 0, 5907, 0, 0, 57, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2411, 1, 0, 8123, 4, 17, 25, 0, 1508256499),
(864, 0, 5970, 0, 0, 59, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2442, 1, 0, 8227, 4, 17, 25, 0, 1508256810),
(865, 0, 6033, 0, 0, 59, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2473, 1, 0, 8331, 4, 17, 25, 0, 1508257121),
(866, 0, 6096, 0, 0, 59, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2504, 1, 0, 8434, 4, 17, 25, 0, 1508257432),
(867, 0, 6156, 0, 0, 61, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2536, 1, 0, 8538, 5, 17, 25, 0, 1508257743),
(868, 0, 6219, 0, 0, 61, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2567, 1, 0, 8641, 4, 17, 25, 0, 1508258054),
(869, 0, 6282, 0, 0, 61, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2598, 1, 0, 8745, 4, 17, 25, 0, 1508258365),
(870, 0, 6342, 0, 0, 63, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2629, 1, 0, 8849, 4, 17, 25, 0, 1508258676),
(871, 0, 6405, 0, 0, 63, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2660, 1, 0, 8952, 4, 17, 25, 0, 1508258987),
(872, 0, 6468, 0, 0, 63, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2691, 1, 0, 9056, 3, 17, 25, 0, 1508259298),
(873, 0, 6531, 0, 0, 65, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2722, 1, 0, 9160, 5, 17, 25, 0, 1508259609),
(874, 0, 6591, 0, 0, 65, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2753, 1, 0, 9263, 4, 17, 25, 0, 1508259920),
(875, 0, 6654, 0, 0, 65, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2784, 1, 0, 9367, 5, 17, 25, 0, 1508260231),
(876, 0, 6717, 0, 0, 67, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2815, 1, 0, 9471, 4, 17, 25, 0, 1508260542),
(877, 0, 6780, 0, 0, 67, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2847, 1, 0, 9574, 4, 17, 25, 0, 1508260853),
(878, 0, 6840, 0, 0, 67, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2878, 1, 0, 9678, 4, 17, 25, 0, 1508261164),
(879, 0, 6903, 0, 0, 69, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2909, 1, 0, 9782, 4, 17, 25, 0, 1508261475),
(880, 0, 6966, 0, 0, 69, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2940, 1, 0, 9885, 4, 17, 25, 0, 1508261786),
(881, 0, 7026, 0, 0, 69, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2971, 1, 0, 9989, 4, 17, 25, 0, 1508262097),
(882, 0, 7089, 0, 0, 71, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 3002, 1, 0, 10092, 4, 17, 25, 0, 1508262408),
(883, 0, 7152, 0, 0, 71, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 3033, 1, 0, 10196, 4, 17, 25, 0, 1508262720),
(884, 0, 7215, 0, 0, 71, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 3064, 1, 0, 10300, 4, 17, 25, 0, 1508263030),
(885, 0, 7275, 0, 0, 73, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 3095, 1, 0, 10403, 4, 17, 25, 0, 1508263341),
(886, 0, 7338, 0, 0, 73, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 3126, 1, 0, 10507, 4, 17, 25, 0, 1508263652),
(887, 0, 7401, 0, 0, 75, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 3158, 1, 0, 10611, 4, 17, 25, 0, 1508263963),
(888, 0, 7464, 0, 0, 75, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 3189, 1, 0, 10714, 5, 17, 25, 0, 1508264274),
(889, 0, 7524, 0, 0, 75, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 3220, 1, 0, 10818, 4, 17, 25, 0, 1508264585),
(890, 0, 7587, 0, 0, 77, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 3251, 1, 0, 10922, 4, 17, 25, 0, 1508264896),
(891, 0, 7650, 0, 0, 77, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 3282, 1, 0, 11025, 4, 17, 25, 0, 1508265207),
(892, 0, 7710, 0, 0, 77, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 3313, 1, 0, 11129, 4, 17, 25, 0, 1508265518),
(893, 0, 7773, 0, 0, 79, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 3344, 1, 0, 11233, 4, 17, 25, 0, 1508265829),
(894, 0, 7836, 0, 0, 79, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 3375, 1, 0, 11336, 4, 17, 25, 0, 1508266140),
(895, 0, 7899, 0, 0, 79, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 3406, 1, 0, 11440, 4, 17, 25, 0, 1508266452),
(896, 0, 7959, 0, 0, 81, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 3437, 1, 0, 11543, 4, 17, 25, 0, 1508266762),
(897, 0, 8022, 0, 0, 81, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 3469, 1, 0, 11647, 4, 17, 25, 0, 1508267073),
(898, 0, 8085, 0, 0, 81, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 3500, 1, 0, 11751, 4, 17, 25, 0, 1508267384),
(899, 0, 8145, 0, 0, 83, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 3531, 1, 0, 11854, 4, 17, 25, 0, 1508267695),
(900, 0, 8208, 0, 0, 83, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 3562, 1, 0, 11958, 4, 17, 25, 0, 1508268006),
(901, 0, 8271, 0, 0, 83, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 3593, 1, 0, 12062, 4, 17, 25, 0, 1508268317),
(902, 0, 8334, 0, 0, 85, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 3624, 1, 0, 12165, 4, 17, 25, 0, 1508268628),
(903, 0, 8394, 0, 0, 85, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 3655, 1, 0, 12269, 4, 17, 25, 0, 1508268939),
(904, 0, 8457, 0, 0, 85, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 3686, 1, 0, 12373, 4, 17, 25, 0, 1508269250),
(905, 0, 8520, 0, 0, 87, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 3717, 1, 0, 12476, 4, 17, 25, 0, 1508269561),
(906, 0, 8583, 0, 0, 87, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 3748, 1, 0, 12580, 4, 17, 25, 0, 1508269872),
(907, 0, 8643, 0, 0, 87, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 3780, 1, 0, 12684, 4, 17, 25, 0, 1508270183),
(908, 0, 8706, 0, 0, 89, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 3811, 1, 0, 12787, 4, 17, 25, 0, 1508270494),
(909, 0, 8769, 0, 0, 89, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 3842, 1, 0, 12891, 4, 17, 25, 0, 1508270805),
(910, 0, 8829, 0, 0, 89, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 3873, 1, 0, 12995, 4, 17, 25, 0, 1508271116),
(911, 0, 8892, 0, 0, 91, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 3904, 1, 0, 13098, 4, 16, 25, 0, 1508271427),
(912, 0, 8955, 0, 0, 91, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 3935, 1, 0, 13202, 3, 16, 25, 0, 1508271738),
(913, 0, 9018, 0, 0, 93, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 3966, 1, 0, 13305, 4, 16, 25, 0, 1508272049),
(914, 0, 9078, 0, 0, 93, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 3997, 1, 0, 13409, 4, 16, 25, 0, 1508272360),
(915, 0, 9141, 0, 0, 93, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 4028, 1, 0, 13513, 4, 16, 25, 0, 1508272671),
(916, 0, 9204, 0, 0, 95, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 4059, 1, 0, 13616, 4, 16, 25, 0, 1508272982),
(917, 0, 9267, 0, 0, 95, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 4091, 1, 0, 13720, 3, 16, 25, 0, 1508273293),
(918, 0, 9327, 0, 0, 95, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 4122, 1, 0, 13824, 4, 16, 25, 0, 1508273604),
(919, 0, 9390, 0, 0, 97, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 4153, 1, 0, 13927, 4, 16, 25, 0, 1508273915),
(920, 0, 9453, 0, 0, 97, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 4184, 1, 0, 14031, 3, 16, 25, 0, 1508274226),
(921, 0, 9513, 0, 0, 97, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 4215, 1, 0, 14135, 4, 16, 25, 0, 1508274537),
(922, 0, 9576, 0, 0, 99, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 4246, 1, 0, 14238, 4, 16, 25, 0, 1508274848),
(923, 0, 9639, 0, 0, 99, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 4277, 1, 0, 14342, 4, 16, 25, 0, 1508275159),
(924, 0, 9702, 0, 0, 99, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 4308, 1, 0, 14446, 4, 16, 25, 0, 1508275470),
(925, 0, 9762, 0, 0, 101, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 4339, 1, 0, 14549, 4, 16, 25, 0, 1508275781),
(926, 0, 9825, 0, 0, 101, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 4370, 1, 0, 14653, 4, 16, 25, 0, 1508276092),
(927, 0, 9888, 0, 0, 101, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 4402, 1, 0, 14756, 3, 16, 25, 0, 1508276403),
(928, 0, 9948, 0, 0, 103, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 4433, 1, 0, 14860, 4, 16, 25, 0, 1508276714),
(929, 0, 10011, 0, 0, 103, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 4464, 1, 0, 14964, 3, 16, 25, 0, 1508277025),
(930, 0, 10074, 0, 0, 103, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 4495, 1, 0, 15067, 3, 16, 25, 0, 1508277336),
(931, 0, 10137, 0, 0, 105, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 4526, 1, 0, 15171, 3, 16, 25, 0, 1508277647),
(932, 0, 10197, 0, 0, 105, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 4557, 1, 0, 15275, 3, 16, 25, 0, 1508277958),
(933, 0, 10260, 0, 0, 105, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 4588, 1, 0, 15378, 3, 16, 25, 0, 1508278269),
(934, 0, 10323, 0, 0, 107, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 4619, 1, 0, 15482, 3, 16, 25, 0, 1508278580),
(935, 0, 10386, 0, 0, 107, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 4650, 1, 0, 15586, 3, 16, 25, 0, 1508278891),
(936, 0, 10446, 0, 0, 107, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 4681, 1, 0, 15689, 3, 16, 25, 0, 1508279202),
(937, 0, 10509, 0, 0, 109, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 4713, 1, 0, 15793, 3, 16, 25, 0, 1508279513),
(938, 0, 10572, 0, 0, 109, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 4744, 1, 0, 15896, 3, 16, 25, 0, 1508279824),
(939, 0, 10632, 0, 0, 109, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 4775, 1, 0, 16000, 3, 16, 25, 0, 1508280135),
(940, 0, 10695, 0, 0, 111, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 4806, 1, 0, 16104, 3, 16, 25, 0, 1508280446),
(941, 0, 10758, 0, 0, 111, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 4837, 1, 0, 16207, 3, 16, 25, 0, 1508280757),
(942, 0, 10821, 0, 0, 113, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 4868, 1, 0, 16311, 3, 16, 25, 0, 1508281068),
(943, 0, 10881, 0, 0, 113, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 4899, 1, 0, 16415, 3, 16, 25, 0, 1508281379),
(944, 0, 10944, 0, 0, 113, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 4930, 1, 0, 16518, 3, 16, 25, 0, 1508281690),
(945, 0, 11007, 0, 0, 115, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 4961, 1, 0, 16622, 3, 16, 25, 0, 1508282001),
(946, 0, 11070, 0, 0, 115, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 4992, 1, 0, 16726, 4, 16, 25, 0, 1508282312),
(947, 0, 11130, 0, 0, 115, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 5024, 1, 0, 16829, 3, 16, 25, 0, 1508282623),
(948, 0, 11193, 0, 0, 117, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 5055, 1, 0, 16933, 4, 16, 25, 0, 1508282934),
(949, 0, 11256, 0, 0, 117, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 5086, 1, 0, 17037, 3, 16, 25, 0, 1508283245),
(950, 0, 11316, 0, 0, 117, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 5117, 1, 0, 17140, 3, 20, 24, 0, 1508283556),
(951, 0, 11379, 0, 0, 119, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 5148, 1, 0, 17244, 10, 17, 25, 0, 1508283867),
(952, 0, 11442, 0, 0, 119, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 5179, 1, 0, 17348, 3, 16, 24, 0, 1508284178),
(953, 0, 11505, 0, 0, 119, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 5210, 1, 0, 17451, 3, 16, 24, 0, 1508284489),
(954, 0, 11565, 0, 0, 121, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 5241, 1, 0, 17555, 3, 16, 24, 0, 1508284800),
(955, 0, 11628, 0, 0, 121, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 5260, 1, 0, 17588, 4, 16, 24, 0, 1508285111),
(956, 0, 11691, 0, 0, 121, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 5260, 1, 0, 17619, 3, 16, 24, 0, 1508285422),
(957, 0, 11754, 0, 0, 121, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 5260, 1, 0, 17651, 3, 16, 24, 0, 1508285733),
(958, 0, 11814, 0, 0, 121, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 5260, 1, 0, 17682, 3, 16, 24, 0, 1508286044),
(959, 0, 11877, 0, 0, 121, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 5260, 1, 0, 17713, 4, 16, 24, 0, 1508286355),
(960, 0, 11940, 0, 0, 121, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 5260, 1, 0, 17744, 3, 16, 24, 0, 1508286666),
(961, 0, 12000, 0, 0, 121, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 5260, 1, 0, 17775, 3, 16, 24, 0, 1508286977),
(962, 0, 12063, 0, 0, 121, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 5260, 1, 0, 17806, 3, 16, 24, 0, 1508287288),
(963, 0, 12126, 0, 0, 121, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 5260, 1, 0, 17836, 3, 16, 24, 0, 1508287599),
(964, 0, 12189, 0, 0, 121, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 5260, 1, 0, 17868, 3, 16, 24, 0, 1508287910),
(965, 0, 12249, 0, 0, 121, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 5260, 1, 0, 17899, 3, 16, 24, 0, 1508288221),
(966, 0, 12312, 0, 0, 121, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 5260, 1, 0, 17930, 3, 16, 24, 0, 1508288532),
(967, 0, 12375, 0, 0, 121, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 5260, 1, 0, 17961, 4, 16, 24, 0, 1508288843),
(968, 0, 12435, 0, 0, 121, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 5260, 1, 0, 17992, 3, 16, 24, 0, 1508289154),
(969, 0, 12498, 0, 0, 121, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 5260, 1, 0, 18023, 3, 16, 24, 0, 1508289465),
(970, 0, 12561, 0, 0, 121, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 5260, 1, 0, 18054, 3, 16, 24, 0, 1508289776),
(971, 0, 12624, 0, 0, 121, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 5260, 1, 0, 18084, 3, 16, 24, 0, 1508290087),
(972, 0, 12684, 0, 0, 121, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 5260, 1, 0, 18115, 3, 16, 24, 0, 1508290398),
(973, 0, 12747, 0, 0, 121, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 5260, 1, 0, 18146, 9, 16, 24, 0, 1508290709),
(974, 0, 12810, 0, 0, 121, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 5260, 1, 0, 18176, 3, 16, 24, 0, 1508291020),
(975, 0, 63, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 32, 3, 7, 24, 0, 1508291624),
(976, 0, 126, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 63, 4, 9, 24, 0, 1508291935),
(977, 0, 189, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 94, 3, 11, 24, 0, 1508292246),
(978, 0, 252, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 125, 3, 13, 24, 0, 1508292557),
(979, 0, 312, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 156, 3, 14, 24, 0, 1508292868),
(980, 0, 375, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 187, 4, 15, 24, 0, 1508293179),
(981, 0, 438, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 219, 3, 16, 24, 0, 1508293490),
(982, 0, 498, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 249, 4, 16, 24, 0, 1508293801),
(983, 0, 561, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 281, 3, 17, 24, 0, 1508294112),
(984, 0, 624, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 312, 5, 17, 24, 0, 1508294423);
INSERT INTO `hcusyspmglobaldatainfo` (`sid`, `taskhcuvmec`, `taskhwinvec`, `tasksyspmec`, `taskmodbusec`, `taskcloudvelaec`, `taskavorionec`, `taskspsvirgoec`, `taskhsmmpec`, `taskemcec`, `taskpm25ec`, `taskwinddirec`, `taskwindspdec`, `tasktempec`, `taskhumidec`, `tasknoiseec`, `taskairprsec`, `taskco1ec`, `tasklightstrec`, `taskalcoholec`, `taskhchoec`, `tasktoxicgasec`, `restartcnt`, `cloudvelaconncnt`, `cloudvelaconnfailcnt`, `cloudveladisccnt`, `clouddatatimeoutcnt`, `socketdisccnt`, `cpuoccupy`, `memoccupy`, `diskoccupy`, `cputemp`, `timestamp`) VALUES
(985, 0, 687, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 344, 6, 17, 24, 0, 1508294734),
(986, 0, 747, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 376, 5, 17, 24, 0, 1508295045),
(987, 0, 810, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 407, 6, 17, 24, 0, 1508295356),
(988, 0, 873, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 438, 6, 18, 24, 0, 1508295667),
(989, 0, 933, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 469, 6, 18, 24, 0, 1508295978),
(990, 0, 996, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 500, 6, 18, 24, 0, 1508296289),
(991, 0, 1059, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 532, 6, 18, 24, 0, 1508296600),
(992, 0, 1122, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 563, 5, 18, 24, 0, 1508296911),
(993, 0, 1182, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 594, 5, 18, 24, 0, 1508297222),
(994, 0, 1245, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 625, 6, 18, 24, 0, 1508297533),
(995, 0, 1308, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 657, 6, 18, 24, 0, 1508297844),
(996, 0, 1371, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 688, 6, 18, 24, 0, 1508298155),
(997, 0, 1431, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 718, 6, 18, 24, 0, 1508298466),
(998, 0, 1494, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 749, 6, 18, 24, 0, 1508298777),
(999, 0, 1557, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 780, 7, 18, 24, 0, 1508299088),
(1000, 0, 1617, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 811, 6, 18, 24, 0, 1508299399),
(1001, 0, 1680, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 843, 6, 18, 24, 0, 1508299710),
(1002, 0, 1743, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 874, 6, 18, 24, 0, 1508300021),
(1003, 0, 1806, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 905, 6, 18, 24, 0, 1508300332),
(1004, 0, 1866, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 935, 5, 18, 24, 0, 1508300643),
(1005, 0, 1929, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 967, 5, 18, 24, 0, 1508300954),
(1006, 0, 1992, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 998, 6, 14, 25, 0, 1508301265),
(1007, 0, 2055, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1028, 6, 14, 25, 0, 1508301576),
(1008, 0, 2115, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1059, 6, 14, 25, 0, 1508301887),
(1009, 0, 63, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 32, 5, 8, 24, 0, 1508303324),
(1010, 0, 126, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 63, 40, 8, 24, 0, 1508303635),
(1011, 0, 63, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 32, 8, 11, 24, 0, 1508304072),
(1012, 0, 126, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 61, 4, 12, 24, 0, 1508304383),
(1013, 0, 189, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 92, 5, 13, 24, 0, 1508304694),
(1014, 0, 252, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 123, 4, 14, 24, 0, 1508305005),
(1015, 0, 312, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 154, 4, 15, 24, 0, 1508305316),
(1016, 0, 375, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 186, 4, 15, 24, 0, 1508305627),
(1017, 0, 438, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 216, 4, 16, 24, 0, 1508305938),
(1018, 0, 498, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 248, 4, 17, 24, 0, 1508306249),
(1019, 0, 561, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 279, 4, 17, 24, 0, 1508306561),
(1020, 0, 624, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 310, 4, 17, 24, 0, 1508306871),
(1021, 0, 687, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 341, 4, 17, 24, 0, 1508307183),
(1022, 0, 747, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 372, 4, 18, 24, 0, 1508307493),
(1023, 0, 810, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 403, 4, 18, 24, 0, 1508307804),
(1024, 0, 873, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 434, 4, 18, 24, 0, 1508308115),
(1025, 0, 63, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 32, 5, 18, 24, 0, 1508308726),
(1026, 0, 126, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 63, 5, 18, 24, 0, 1508309037),
(1027, 0, 189, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 94, 5, 18, 24, 0, 1508309348),
(1028, 0, 63, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 28, 2, 18, 24, 0, 1508309775),
(1029, 0, 126, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 58, 1, 18, 24, 0, 1508310087),
(1030, 0, 189, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 89, 1, 18, 24, 0, 1508310397),
(1031, 0, 252, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 120, 2, 18, 24, 0, 1508310709),
(1032, 0, 312, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 145, 2, 18, 24, 0, 1508311019),
(1033, 0, 63, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 30, 2, 18, 24, 0, 1508311526),
(1034, 0, 126, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 58, 2, 18, 24, 0, 1508311837),
(1035, 0, 189, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 89, 1, 18, 24, 0, 1508312148),
(1036, 0, 252, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 120, 1, 18, 24, 0, 1508312459),
(1037, 0, 312, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 149, 2, 18, 24, 0, 1508312771),
(1038, 0, 375, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 179, 2, 18, 24, 0, 1508313081),
(1039, 0, 438, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 209, 1, 18, 24, 0, 1508313392),
(1040, 0, 498, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 236, 2, 18, 24, 0, 1508313703),
(1041, 0, 561, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 11, 1, 0, 255, 2, 18, 24, 0, 1508314014),
(1042, 0, 624, 0, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 11, 1, 0, 280, 2, 18, 24, 0, 1508314325),
(1043, 0, 687, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 22, 2, 0, 304, 1, 18, 24, 0, 1508314636),
(1044, 0, 747, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 22, 2, 0, 335, 2, 18, 24, 0, 1508314947),
(1045, 0, 810, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 22, 2, 0, 365, 2, 18, 24, 0, 1508315258),
(1046, 0, 873, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 22, 2, 0, 395, 1, 18, 24, 0, 1508315569),
(1047, 0, 933, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 22, 2, 0, 425, 2, 18, 24, 0, 1508315881),
(1048, 0, 996, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 22, 2, 0, 456, 2, 18, 24, 0, 1508316192),
(1049, 0, 1059, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 22, 2, 0, 483, 2, 18, 24, 0, 1508316503),
(1050, 0, 1122, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 22, 2, 0, 514, 1, 18, 24, 0, 1508316813),
(1051, 0, 1182, 0, 0, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 27, 3, 0, 581, 2, 18, 24, 0, 1508317124),
(1052, 0, 1245, 0, 0, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 27, 3, 0, 581, 2, 18, 24, 0, 1508317436),
(1053, 0, 1308, 0, 0, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 27, 3, 0, 581, 3, 17, 24, 0, 1508317746),
(1054, 0, 1371, 0, 0, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 56, 4, 0, 675, 2, 17, 24, 0, 1508318057),
(1055, 0, 1431, 0, 0, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 87, 4, 0, 779, 2, 17, 24, 0, 1508318368),
(1056, 0, 1494, 0, 0, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 118, 4, 0, 882, 1, 17, 24, 0, 1508318680),
(1057, 0, 1557, 0, 0, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 149, 4, 0, 986, 1, 17, 24, 0, 1508318991),
(1058, 0, 1617, 0, 0, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 180, 4, 0, 1090, 2, 17, 24, 0, 1508319302),
(1059, 0, 1680, 0, 0, 18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 211, 4, 0, 1193, 2, 17, 24, 0, 1508319612),
(1060, 0, 1743, 0, 0, 18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 242, 4, 0, 1297, 2, 17, 24, 0, 1508319923),
(1061, 0, 1806, 0, 0, 18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 274, 4, 0, 1516, 2, 17, 24, 0, 1508320234),
(1062, 0, 1866, 0, 0, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 305, 4, 0, 1711, 1, 17, 24, 0, 1508320546),
(1063, 0, 1929, 0, 0, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 336, 4, 0, 1815, 1, 17, 24, 0, 1508320857),
(1064, 0, 1992, 0, 0, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 367, 4, 0, 1918, 2, 17, 24, 0, 1508321167),
(1065, 0, 2052, 0, 0, 22, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 398, 4, 0, 2022, 2, 17, 24, 0, 1508321478),
(1066, 0, 2115, 0, 0, 22, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 429, 4, 0, 2126, 2, 17, 24, 0, 1508321789),
(1067, 0, 2178, 0, 0, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 460, 4, 0, 2229, 1, 17, 24, 0, 1508322101),
(1068, 0, 2241, 0, 0, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 491, 4, 0, 2333, 2, 17, 24, 0, 1508322412),
(1069, 0, 2301, 0, 0, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 522, 4, 0, 2436, 2, 17, 24, 0, 1508322723),
(1070, 0, 2364, 0, 0, 26, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 553, 4, 0, 2540, 2, 17, 24, 0, 1508323033),
(1071, 0, 2427, 0, 0, 26, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 585, 4, 0, 2644, 1, 17, 24, 0, 1508323344),
(1072, 0, 2490, 0, 0, 26, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 616, 4, 0, 2747, 2, 17, 24, 0, 1508323656),
(1073, 0, 2550, 0, 0, 28, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 647, 4, 0, 2851, 2, 17, 24, 0, 1508323967),
(1074, 0, 2613, 0, 0, 28, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 678, 4, 0, 2955, 2, 17, 24, 0, 1508324278),
(1075, 0, 2676, 0, 0, 28, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 709, 4, 0, 3058, 2, 17, 24, 0, 1508324589),
(1076, 0, 2736, 0, 0, 30, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 740, 4, 0, 3162, 2, 17, 24, 0, 1508324899),
(1077, 0, 2799, 0, 0, 30, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 771, 4, 0, 3266, 2, 17, 24, 0, 1508325210),
(1078, 0, 2862, 0, 0, 30, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 802, 4, 0, 3369, 2, 17, 24, 0, 1508325522),
(1079, 0, 2925, 0, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 833, 4, 0, 3473, 1, 17, 24, 0, 1508325833),
(1080, 0, 2985, 0, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 864, 4, 0, 3577, 2, 17, 24, 0, 1508326144),
(1081, 0, 3048, 0, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 896, 4, 0, 3680, 2, 17, 24, 0, 1508326454),
(1082, 0, 3111, 0, 0, 34, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 927, 4, 0, 3784, 2, 17, 24, 0, 1508326765),
(1083, 0, 3171, 0, 0, 34, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 958, 4, 0, 3887, 2, 17, 24, 0, 1508327077),
(1084, 0, 3234, 0, 0, 34, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 989, 4, 0, 3991, 1, 17, 24, 0, 1508327388),
(1085, 0, 3297, 0, 0, 36, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 1020, 4, 0, 4095, 2, 17, 24, 0, 1508327699),
(1086, 0, 3360, 0, 0, 36, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 1051, 4, 0, 4198, 2, 17, 24, 0, 1508328009),
(1087, 0, 3420, 0, 0, 36, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 1082, 4, 0, 4302, 2, 17, 24, 0, 1508328320),
(1088, 0, 3483, 0, 0, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 1113, 4, 0, 4406, 2, 17, 24, 0, 1508328631),
(1089, 0, 3546, 0, 0, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 1144, 4, 0, 4509, 2, 17, 24, 0, 1508328943),
(1090, 0, 3606, 0, 0, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 1175, 4, 0, 4613, 2, 17, 24, 0, 1508329254),
(1091, 0, 3669, 0, 0, 40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 1207, 4, 0, 4717, 2, 17, 24, 0, 1508329564),
(1092, 0, 3732, 0, 0, 40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 1238, 4, 0, 4820, 2, 17, 24, 0, 1508329875),
(1093, 0, 3795, 0, 0, 40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 1269, 4, 0, 4924, 2, 17, 24, 0, 1508330186),
(1094, 0, 3855, 0, 0, 42, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 1300, 4, 0, 5028, 2, 17, 24, 0, 1508330498),
(1095, 0, 3918, 0, 0, 42, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 1331, 4, 0, 5131, 2, 17, 24, 0, 1508330809),
(1096, 0, 3981, 0, 0, 44, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 1362, 4, 0, 5235, 1, 17, 24, 0, 1508331120),
(1097, 0, 4044, 0, 0, 44, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 1393, 4, 0, 5338, 2, 17, 24, 0, 1508331430),
(1098, 0, 4104, 0, 0, 44, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 1424, 4, 0, 5442, 12, 17, 24, 0, 1508331741),
(1099, 0, 4167, 0, 0, 46, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 1455, 4, 0, 5546, 2, 17, 24, 0, 1508332052),
(1100, 0, 4230, 0, 0, 46, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 1486, 4, 0, 5649, 2, 17, 24, 0, 1508332364),
(1101, 0, 4290, 0, 0, 46, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 1518, 4, 0, 5753, 2, 17, 24, 0, 1508332675),
(1102, 0, 4353, 0, 0, 48, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 1549, 4, 0, 5857, 2, 17, 24, 0, 1508332985),
(1103, 0, 4416, 0, 0, 48, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 1580, 4, 0, 5960, 2, 17, 24, 0, 1508333296),
(1104, 0, 4479, 0, 0, 48, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 1611, 4, 0, 6064, 1, 17, 24, 0, 1508333607),
(1105, 0, 4539, 0, 0, 50, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 1642, 4, 0, 6168, 2, 17, 24, 0, 1508333919),
(1106, 0, 4602, 0, 0, 50, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 1673, 4, 0, 6271, 2, 17, 24, 0, 1508334230),
(1107, 0, 4665, 0, 0, 50, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 1704, 4, 0, 6375, 2, 17, 24, 0, 1508334541),
(1108, 0, 4725, 0, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 1735, 4, 0, 6478, 2, 17, 24, 0, 1508334851),
(1109, 0, 4788, 0, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 1766, 4, 0, 6582, 2, 17, 24, 0, 1508335162),
(1110, 0, 4851, 0, 0, 52, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 1797, 4, 0, 6686, 2, 17, 24, 0, 1508335474),
(1111, 0, 4914, 0, 0, 54, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 1829, 4, 0, 6789, 2, 17, 24, 0, 1508335785),
(1112, 0, 4974, 0, 0, 54, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 1860, 4, 0, 6893, 2, 17, 24, 0, 1508336096),
(1113, 0, 5037, 0, 0, 54, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 1891, 4, 0, 6997, 2, 17, 24, 0, 1508336406),
(1114, 0, 5100, 0, 0, 56, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 1922, 4, 0, 7100, 1, 17, 24, 0, 1508336717),
(1115, 0, 5163, 0, 0, 56, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 1953, 4, 0, 7204, 2, 17, 24, 0, 1508337028),
(1116, 0, 5223, 0, 0, 56, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 1984, 4, 0, 7308, 2, 17, 24, 0, 1508337340),
(1117, 0, 5286, 0, 0, 58, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 2015, 4, 0, 7411, 2, 17, 24, 0, 1508337651),
(1118, 0, 5349, 0, 0, 58, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 2046, 4, 0, 7515, 1, 17, 24, 0, 1508337962),
(1119, 0, 5409, 0, 0, 58, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 2077, 4, 0, 7619, 2, 17, 24, 0, 1508338272),
(1120, 0, 5472, 0, 0, 60, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 2108, 4, 0, 7722, 2, 17, 24, 0, 1508338583),
(1121, 0, 5535, 0, 0, 60, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 2140, 4, 0, 7826, 2, 17, 24, 0, 1508338895),
(1122, 0, 5598, 0, 0, 62, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 2171, 4, 0, 7930, 2, 17, 24, 0, 1508339206),
(1123, 0, 5658, 0, 0, 62, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 2202, 4, 0, 8033, 2, 17, 24, 0, 1508339517),
(1124, 0, 5721, 0, 0, 62, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 2233, 4, 0, 8137, 2, 17, 24, 0, 1508339827),
(1125, 0, 5784, 0, 0, 64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 2264, 4, 0, 8240, 2, 17, 24, 0, 1508340138),
(1126, 0, 5844, 0, 0, 64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 2295, 4, 0, 8344, 2, 17, 24, 0, 1508340449),
(1127, 0, 5907, 0, 0, 64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 2326, 4, 0, 8448, 2, 17, 24, 0, 1508340761),
(1128, 0, 5970, 0, 0, 66, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 2357, 4, 0, 8551, 2, 17, 24, 0, 1508341072),
(1129, 0, 6033, 0, 0, 66, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 2388, 4, 0, 8655, 2, 17, 24, 0, 1508341382),
(1130, 0, 6093, 0, 0, 66, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 2419, 4, 0, 8759, 2, 17, 24, 0, 1508341693),
(1131, 0, 6156, 0, 0, 68, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 2451, 4, 0, 8862, 2, 17, 24, 0, 1508342004),
(1132, 0, 6219, 0, 0, 68, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 2482, 4, 0, 8966, 2, 17, 24, 0, 1508342316),
(1133, 0, 6279, 0, 0, 68, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 2513, 4, 0, 9070, 2, 17, 24, 0, 1508342627),
(1134, 0, 6342, 0, 0, 70, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 2544, 4, 0, 9173, 2, 17, 24, 0, 1508342938),
(1135, 0, 6405, 0, 0, 70, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 2575, 4, 0, 9277, 2, 17, 24, 0, 1508343248),
(1136, 0, 6468, 0, 0, 70, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 2606, 4, 0, 9381, 2, 17, 24, 0, 1508343559),
(1137, 0, 6528, 0, 0, 72, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 2637, 4, 0, 9484, 2, 16, 24, 0, 1508343871),
(1138, 0, 6591, 0, 0, 72, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 2668, 4, 0, 9588, 2, 16, 24, 0, 1508344182),
(1139, 0, 6654, 0, 0, 72, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 2699, 4, 0, 9691, 2, 16, 24, 0, 1508344492),
(1140, 0, 6717, 0, 0, 74, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 2730, 4, 0, 9795, 2, 16, 24, 0, 1508344803),
(1141, 0, 6777, 0, 0, 74, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 2762, 4, 0, 9899, 2, 16, 24, 0, 1508345114),
(1142, 0, 6840, 0, 0, 74, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 2793, 4, 0, 10002, 2, 16, 24, 0, 1508345425),
(1143, 0, 6903, 0, 0, 76, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 2824, 4, 0, 10106, 2, 16, 24, 0, 1508345737),
(1144, 0, 6963, 0, 0, 76, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 2855, 4, 0, 10210, 2, 16, 24, 0, 1508346048),
(1145, 0, 7026, 0, 0, 76, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 2886, 4, 0, 10313, 2, 16, 24, 0, 1508346359),
(1146, 0, 7089, 0, 0, 78, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 2917, 4, 0, 10417, 2, 16, 24, 0, 1508346669),
(1147, 0, 7152, 0, 0, 78, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 2948, 4, 0, 10521, 2, 16, 24, 0, 1508346980),
(1148, 0, 7212, 0, 0, 78, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 2979, 4, 0, 10624, 2, 16, 24, 0, 1508347292),
(1149, 0, 7275, 0, 0, 80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 3010, 4, 0, 10728, 2, 16, 24, 0, 1508347603),
(1150, 0, 7338, 0, 0, 80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 3041, 4, 0, 10832, 1, 16, 24, 0, 1508347914),
(1151, 0, 7398, 0, 0, 82, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 3073, 4, 0, 10935, 2, 16, 24, 0, 1508348224),
(1152, 0, 7461, 0, 0, 82, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 3104, 4, 0, 11039, 2, 16, 24, 0, 1508348535),
(1153, 0, 7524, 0, 0, 82, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 3135, 4, 0, 11143, 1, 16, 24, 0, 1508348846),
(1154, 0, 7587, 0, 0, 84, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 3166, 4, 0, 11246, 2, 16, 24, 0, 1508349158),
(1155, 0, 7647, 0, 0, 84, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 3197, 4, 0, 11350, 2, 16, 24, 0, 1508349469),
(1156, 0, 7710, 0, 0, 84, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 3228, 4, 0, 11453, 2, 16, 24, 0, 1508349779),
(1157, 0, 7773, 0, 0, 86, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 3259, 4, 0, 11557, 2, 16, 24, 0, 1508350090),
(1158, 0, 7833, 0, 0, 86, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 3290, 4, 0, 11661, 2, 16, 24, 0, 1508350401),
(1159, 0, 7896, 0, 0, 86, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 3321, 4, 0, 11764, 2, 16, 24, 0, 1508350713),
(1160, 0, 7959, 0, 0, 88, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 3352, 4, 0, 11868, 2, 14, 24, 0, 1508351024),
(1161, 0, 8022, 0, 0, 88, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 3384, 4, 0, 11972, 1, 14, 24, 0, 1508351335),
(1162, 0, 8082, 0, 0, 88, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 3415, 4, 0, 12075, 2, 14, 24, 0, 1508351645),
(1163, 0, 8145, 0, 0, 90, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 3446, 4, 0, 12179, 2, 14, 24, 0, 1508351956),
(1164, 0, 8208, 0, 0, 90, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 3477, 4, 0, 12283, 2, 14, 24, 0, 1508352267),
(1165, 0, 8271, 0, 0, 90, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 3508, 4, 0, 12386, 2, 14, 24, 0, 1508352579),
(1166, 0, 8331, 0, 0, 92, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 3539, 4, 0, 12490, 2, 14, 24, 0, 1508352890),
(1167, 0, 8394, 0, 0, 92, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 3570, 4, 0, 12594, 1, 14, 24, 0, 1508353201),
(1168, 0, 8457, 0, 0, 92, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 3601, 4, 0, 12697, 1, 14, 24, 0, 1508353511),
(1169, 0, 8517, 0, 0, 94, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 3632, 4, 0, 12801, 1, 14, 24, 0, 1508353822),
(1170, 0, 8580, 0, 0, 94, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 3663, 4, 0, 12905, 1, 14, 24, 0, 1508354134),
(1171, 0, 8643, 0, 0, 94, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 3695, 4, 0, 13008, 1, 14, 24, 0, 1508354445),
(1172, 0, 8706, 0, 0, 96, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 3726, 4, 0, 13112, 1, 14, 24, 0, 1508354756),
(1173, 0, 8766, 0, 0, 96, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 3757, 4, 0, 13216, 1, 14, 24, 0, 1508355066),
(1174, 0, 8829, 0, 0, 96, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 3788, 4, 0, 13319, 1, 14, 24, 0, 1508355377),
(1175, 0, 8892, 0, 0, 98, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 3819, 4, 0, 13423, 1, 14, 24, 0, 1508355689),
(1176, 0, 8952, 0, 0, 98, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 3850, 4, 0, 13526, 1, 14, 24, 0, 1508356000),
(1177, 0, 9015, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 3881, 4, 0, 13630, 1, 14, 24, 0, 1508356311),
(1178, 0, 9078, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 3912, 4, 0, 13734, 1, 14, 24, 0, 1508356622),
(1179, 0, 9141, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 3943, 4, 0, 13837, 1, 14, 24, 0, 1508356932),
(1180, 0, 9201, 0, 0, 102, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 3974, 4, 0, 13941, 1, 14, 24, 0, 1508357243),
(1181, 0, 9264, 0, 0, 102, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4006, 4, 0, 14045, 1, 14, 24, 0, 1508357555),
(1182, 0, 9327, 0, 0, 102, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4037, 4, 0, 14148, 1, 14, 24, 0, 1508357866),
(1183, 0, 9387, 0, 0, 104, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4068, 4, 0, 14252, 1, 14, 24, 0, 1508358177),
(1184, 0, 9450, 0, 0, 104, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4099, 4, 0, 14356, 1, 14, 24, 0, 1508358487),
(1185, 0, 9513, 0, 0, 104, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4130, 4, 0, 14459, 2, 14, 24, 0, 1508358798),
(1186, 0, 9576, 0, 0, 106, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4161, 4, 0, 14563, 1, 14, 24, 0, 1508359110),
(1187, 0, 9636, 0, 0, 106, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4192, 4, 0, 14666, 1, 14, 24, 0, 1508359421),
(1188, 0, 9699, 0, 0, 106, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4223, 4, 0, 14770, 1, 14, 24, 0, 1508359732),
(1189, 0, 9762, 0, 0, 108, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4254, 4, 0, 14874, 1, 14, 24, 0, 1508360042),
(1190, 0, 9825, 0, 0, 108, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4285, 4, 0, 14977, 1, 14, 24, 0, 1508360353),
(1191, 0, 9885, 0, 0, 108, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4317, 4, 0, 15081, 1, 14, 24, 0, 1508360664),
(1192, 0, 9948, 0, 0, 110, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4348, 4, 0, 15185, 1, 14, 24, 0, 1508360975),
(1193, 0, 10011, 0, 0, 110, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4379, 4, 0, 15288, 1, 14, 24, 0, 1508361287),
(1194, 0, 10071, 0, 0, 110, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4410, 4, 0, 15392, 1, 14, 24, 0, 1508361597),
(1195, 0, 10134, 0, 0, 112, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4441, 4, 0, 15496, 1, 14, 24, 0, 1508361908),
(1196, 0, 10197, 0, 0, 112, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4472, 4, 0, 15599, 1, 14, 24, 0, 1508362219),
(1197, 0, 10260, 0, 0, 112, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4503, 4, 0, 15703, 1, 14, 24, 0, 1508362531),
(1198, 0, 10320, 0, 0, 114, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4534, 4, 0, 15807, 1, 14, 24, 0, 1508362842),
(1199, 0, 10383, 0, 0, 114, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4565, 4, 0, 15910, 1, 14, 24, 0, 1508363153),
(1200, 0, 10446, 0, 0, 114, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4596, 4, 0, 16014, 1, 14, 24, 0, 1508363463),
(1201, 0, 10506, 0, 0, 116, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4628, 4, 0, 16118, 1, 14, 24, 0, 1508363774),
(1202, 0, 10569, 0, 0, 116, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4659, 4, 0, 16221, 1, 14, 24, 0, 1508364086),
(1203, 0, 10632, 0, 0, 116, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4690, 4, 0, 16325, 1, 14, 24, 0, 1508364397),
(1204, 0, 10695, 0, 0, 118, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4721, 4, 0, 16428, 1, 14, 24, 0, 1508364708),
(1205, 0, 10755, 0, 0, 118, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4752, 4, 0, 16532, 1, 14, 24, 0, 1508365018),
(1206, 0, 10818, 0, 0, 120, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4783, 4, 0, 16636, 1, 14, 24, 0, 1508365329),
(1207, 0, 10881, 0, 0, 120, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4814, 4, 0, 16739, 1, 14, 24, 0, 1508365640),
(1208, 0, 10941, 0, 0, 120, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4845, 4, 0, 16843, 1, 14, 24, 0, 1508365952),
(1209, 0, 11004, 0, 0, 122, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4876, 4, 0, 16947, 1, 14, 24, 0, 1508366263),
(1210, 0, 11067, 0, 0, 122, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4907, 4, 0, 17050, 1, 14, 24, 0, 1508366573),
(1211, 0, 11130, 0, 0, 122, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4939, 4, 0, 17154, 1, 14, 24, 0, 1508366884),
(1212, 0, 11190, 0, 0, 124, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4970, 4, 0, 17258, 2, 14, 24, 0, 1508367195),
(1213, 0, 11253, 0, 0, 124, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 5001, 4, 0, 17361, 1, 14, 24, 0, 1508367507),
(1214, 0, 11316, 0, 0, 124, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 5032, 4, 0, 17465, 1, 14, 24, 0, 1508367818),
(1215, 0, 11379, 0, 0, 126, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 5063, 4, 0, 17569, 1, 14, 24, 0, 1508368129),
(1216, 0, 11439, 0, 0, 126, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 5094, 4, 0, 17672, 1, 14, 24, 0, 1508368439),
(1217, 0, 11502, 0, 0, 126, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 5125, 4, 0, 17776, 1, 14, 24, 0, 1508368750),
(1218, 0, 11565, 0, 0, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 5156, 4, 0, 17879, 1, 14, 24, 0, 1508369061),
(1219, 0, 11625, 0, 0, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 5187, 4, 0, 17983, 1, 14, 24, 0, 1508369373),
(1220, 0, 11688, 0, 0, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 5218, 4, 0, 18087, 1, 14, 24, 0, 1508369684),
(1221, 0, 11751, 0, 0, 130, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 5250, 4, 0, 18190, 1, 17, 24, 0, 1508369994),
(1222, 0, 11814, 0, 0, 130, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 5281, 4, 0, 18294, 1, 17, 24, 0, 1508370305),
(1223, 0, 11874, 0, 0, 130, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 5312, 4, 0, 18362, 1, 17, 24, 0, 1508370616),
(1224, 0, 11937, 0, 0, 130, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5314, 4, 0, 18363, 1, 17, 24, 0, 1508370928),
(1225, 0, 12000, 0, 0, 130, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5314, 4, 0, 18363, 1, 17, 24, 0, 1508371239),
(1226, 0, 12060, 0, 0, 130, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5314, 4, 0, 18363, 1, 17, 24, 0, 1508371549),
(1227, 0, 12123, 0, 0, 130, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5314, 4, 0, 18363, 1, 17, 24, 0, 1508371860),
(1228, 0, 12186, 0, 0, 130, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5314, 4, 0, 18363, 1, 17, 24, 0, 1508372171),
(1229, 0, 12249, 0, 0, 130, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5314, 4, 0, 18363, 1, 17, 24, 0, 1508372482),
(1230, 0, 12309, 0, 0, 130, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5314, 4, 0, 18363, 1, 17, 24, 0, 1508372793),
(1231, 0, 12372, 0, 0, 130, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5314, 4, 0, 18363, 1, 17, 24, 0, 1508373105),
(1232, 0, 12435, 0, 0, 130, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5314, 4, 0, 18363, 1, 17, 24, 0, 1508373416),
(1233, 0, 12495, 0, 0, 130, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5314, 4, 0, 18363, 1, 17, 24, 0, 1508373727),
(1234, 0, 12558, 0, 0, 130, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5314, 4, 0, 18363, 1, 17, 24, 0, 1508374037),
(1235, 0, 12621, 0, 0, 130, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5314, 4, 0, 18363, 1, 17, 24, 0, 1508374348),
(1236, 0, 12684, 0, 0, 130, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5314, 4, 0, 18363, 1, 17, 24, 0, 1508374659),
(1237, 0, 12744, 0, 0, 130, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5314, 4, 0, 18363, 1, 17, 24, 0, 1508374970),
(1238, 0, 12807, 0, 0, 130, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5314, 4, 0, 18363, 1, 17, 24, 0, 1508375282),
(1239, 0, 12870, 0, 0, 130, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5314, 4, 0, 18363, 2, 17, 24, 0, 1508375592),
(1240, 0, 12930, 0, 0, 130, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5314, 4, 0, 18363, 1, 17, 24, 0, 1508375903),
(1241, 0, 12993, 0, 0, 130, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5314, 4, 0, 18363, 1, 17, 24, 0, 1508376214),
(1242, 0, 63, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 6, 12, 24, 0, 1508376839),
(1243, 0, 63, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 3, 7, 24, 0, 1508394543),
(1244, 0, 126, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 42, 8, 24, 0, 1508394854),
(1245, 0, 189, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 2, 8, 24, 0, 1508395165),
(1246, 0, 63, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 3, 7, 24, 0, 1508395688),
(1247, 0, 126, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 3, 9, 24, 0, 1508395999),
(1248, 0, 189, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 3, 10, 24, 0, 1508396310),
(1249, 0, 63, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 4, 7, 24, 0, 1508396947),
(1250, 0, 126, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 3, 8, 24, 0, 1508397258),
(1251, 0, 63, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 8, 12, 24, 0, 1508397910),
(1252, 0, 63, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 63, 7, 24, 0, 1508398535),
(1253, 0, 126, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 32, 9, 24, 0, 1508398846),
(1254, 0, 189, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 28, 10, 24, 0, 1508399157),
(1255, 0, 252, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 31, 12, 24, 0, 1508399468),
(1256, 0, 312, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 31, 13, 24, 0, 1508399779),
(1257, 0, 375, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 31, 14, 24, 0, 1508400091),
(1258, 0, 438, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 31, 15, 24, 0, 1508400401),
(1259, 0, 498, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 30, 16, 24, 0, 1508400712),
(1260, 0, 561, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 31, 16, 24, 0, 1508401023),
(1261, 0, 624, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 29, 17, 24, 0, 1508401334),
(1262, 0, 687, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 31, 18, 24, 0, 1508401645),
(1263, 0, 747, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 29, 18, 24, 0, 1508401956),
(1264, 0, 810, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 29, 18, 24, 0, 1508402267),
(1265, 0, 873, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 29, 19, 24, 0, 1508402578),
(1266, 0, 63, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 3, 12, 24, 0, 1508403039),
(1267, 0, 126, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 3, 14, 24, 0, 1508403350),
(1268, 0, 189, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 4, 15, 24, 0, 1508403661),
(1269, 0, 252, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 3, 15, 24, 0, 1508403972),
(1270, 0, 312, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 6, 1, 0, 3, 4, 16, 24, 0, 1508404283),
(1271, 0, 375, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 37, 1, 0, 106, 4, 16, 24, 0, 1508404594),
(1272, 0, 438, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 68, 1, 0, 210, 3, 17, 24, 0, 1508404905),
(1273, 0, 498, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 99, 1, 0, 314, 4, 17, 24, 0, 1508405216),
(1274, 0, 561, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 130, 1, 0, 417, 4, 18, 24, 0, 1508405527),
(1275, 0, 624, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 162, 1, 0, 521, 4, 18, 24, 0, 1508405838),
(1276, 0, 687, 0, 0, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 193, 1, 0, 625, 3, 19, 24, 0, 1508406149),
(1277, 0, 747, 0, 0, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 224, 1, 0, 728, 3, 19, 24, 0, 1508406460),
(1278, 0, 810, 0, 0, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 255, 1, 0, 832, 3, 19, 24, 0, 1508406771),
(1279, 0, 873, 0, 0, 9, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 286, 1, 0, 936, 3, 19, 24, 0, 1508407082),
(1280, 0, 933, 0, 0, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 317, 1, 0, 1039, 3, 19, 24, 0, 1508407393),
(1281, 0, 996, 0, 0, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 348, 1, 0, 1143, 3, 20, 24, 0, 1508407704),
(1282, 0, 1059, 0, 0, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 379, 1, 0, 1313, 4, 20, 24, 0, 1508408015),
(1283, 0, 1122, 0, 0, 13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 410, 1, 0, 1434, 3, 20, 24, 0, 1508408326),
(1284, 0, 1182, 0, 0, 13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 441, 1, 0, 1538, 3, 20, 24, 0, 1508408637),
(1285, 0, 1245, 0, 0, 13, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 473, 1, 0, 1641, 3, 20, 24, 0, 1508408948),
(1286, 0, 1308, 0, 0, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 504, 1, 0, 1745, 3, 20, 24, 0, 1508409259),
(1287, 0, 1371, 0, 0, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 535, 1, 0, 1849, 3, 20, 24, 0, 1508409570),
(1288, 0, 1431, 0, 0, 15, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 566, 1, 0, 1952, 3, 20, 24, 0, 1508409881),
(1289, 0, 1494, 0, 0, 17, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 597, 1, 0, 2056, 3, 20, 24, 0, 1508410192),
(1290, 0, 1557, 0, 0, 17, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 628, 1, 0, 2160, 3, 20, 24, 0, 1508410503),
(1291, 0, 1617, 0, 0, 17, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 659, 1, 0, 2263, 3, 20, 24, 0, 1508410814),
(1292, 0, 1680, 0, 0, 19, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 690, 1, 0, 2367, 3, 20, 24, 0, 1508411125),
(1293, 0, 1743, 0, 0, 19, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 721, 1, 0, 2471, 3, 20, 24, 0, 1508411436),
(1294, 0, 1806, 0, 0, 19, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 752, 1, 0, 2574, 4, 20, 24, 0, 1508411747),
(1295, 0, 1866, 0, 0, 21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 784, 1, 0, 2678, 4, 20, 24, 0, 1508412058),
(1296, 0, 1929, 0, 0, 21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 815, 1, 0, 2782, 4, 20, 24, 0, 1508412369),
(1297, 0, 1992, 0, 0, 21, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 846, 1, 0, 2885, 3, 20, 24, 0, 1508412680),
(1298, 0, 2055, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 877, 1, 0, 2989, 3, 20, 24, 0, 1508412991),
(1299, 0, 2115, 0, 0, 23, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 908, 1, 0, 3092, 4, 20, 24, 0, 1508413302),
(1300, 0, 2178, 0, 0, 25, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 939, 1, 0, 3196, 3, 20, 24, 0, 1508413613),
(1301, 0, 2241, 0, 0, 25, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 970, 1, 0, 3300, 3, 20, 24, 0, 1508413924),
(1302, 0, 2301, 0, 0, 25, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1001, 1, 0, 3403, 4, 20, 24, 0, 1508414235),
(1303, 0, 2364, 0, 0, 27, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1032, 1, 0, 3507, 3, 20, 24, 0, 1508414546),
(1304, 0, 2427, 0, 0, 27, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1063, 1, 0, 3611, 4, 20, 24, 0, 1508414857),
(1305, 0, 2490, 0, 0, 27, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1095, 1, 0, 3714, 3, 20, 24, 0, 1508415169),
(1306, 0, 2550, 0, 0, 29, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1126, 1, 0, 3818, 3, 20, 24, 0, 1508415479),
(1307, 0, 2613, 0, 0, 29, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1157, 1, 0, 3922, 3, 20, 24, 0, 1508415790),
(1308, 0, 2676, 0, 0, 29, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1188, 1, 0, 4025, 3, 20, 24, 0, 1508416101),
(1309, 0, 2736, 0, 0, 31, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1219, 1, 0, 4129, 4, 20, 24, 0, 1508416412),
(1310, 0, 2799, 0, 0, 31, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1250, 1, 0, 4233, 3, 20, 24, 0, 1508416723),
(1311, 0, 2862, 0, 0, 31, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1281, 1, 0, 4336, 4, 20, 24, 0, 1508417034),
(1312, 0, 2925, 0, 0, 33, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1312, 1, 0, 4440, 3, 20, 24, 0, 1508417345),
(1313, 0, 2985, 0, 0, 33, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1343, 1, 0, 4544, 3, 20, 24, 0, 1508417656),
(1314, 0, 3048, 0, 0, 33, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1374, 1, 0, 4647, 3, 20, 24, 0, 1508417967),
(1315, 0, 3111, 0, 0, 35, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1406, 1, 0, 4751, 4, 20, 24, 0, 1508418278),
(1316, 0, 3174, 0, 0, 35, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1437, 1, 0, 4855, 3, 20, 24, 0, 1508418589),
(1317, 0, 3234, 0, 0, 35, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1468, 1, 0, 4958, 3, 20, 24, 0, 1508418900),
(1318, 0, 3297, 0, 0, 37, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1499, 1, 0, 5062, 3, 20, 24, 0, 1508419211),
(1319, 0, 3360, 0, 0, 37, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1530, 1, 0, 5165, 3, 20, 24, 0, 1508419522),
(1320, 0, 3420, 0, 0, 37, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1561, 1, 0, 5269, 3, 20, 24, 0, 1508419833),
(1321, 0, 3483, 0, 0, 39, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1592, 1, 0, 5373, 3, 20, 24, 0, 1508420144),
(1322, 0, 3546, 0, 0, 39, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1623, 1, 0, 5476, 3, 20, 24, 0, 1508420455),
(1323, 0, 3609, 0, 0, 39, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1654, 1, 0, 5580, 3, 20, 24, 0, 1508420766),
(1324, 0, 3669, 0, 0, 41, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1685, 1, 0, 5684, 3, 20, 24, 0, 1508421077),
(1325, 0, 3732, 0, 0, 41, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1717, 1, 0, 5787, 3, 20, 24, 0, 1508421388),
(1326, 0, 3795, 0, 0, 41, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1748, 1, 0, 5891, 3, 20, 24, 0, 1508421699),
(1327, 0, 3858, 0, 0, 43, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1779, 1, 0, 5995, 4, 20, 24, 0, 1508422010),
(1328, 0, 3918, 0, 0, 43, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1810, 1, 0, 6098, 4, 20, 24, 0, 1508422321),
(1329, 0, 3981, 0, 0, 45, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1841, 1, 0, 6202, 3, 20, 24, 0, 1508422632),
(1330, 0, 4044, 0, 0, 45, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1872, 1, 0, 6306, 3, 20, 24, 0, 1508422943),
(1331, 0, 4104, 0, 0, 45, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1903, 1, 0, 6409, 3, 20, 24, 0, 1508423254),
(1332, 0, 4167, 0, 0, 47, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1934, 1, 0, 6513, 3, 20, 24, 0, 1508423565),
(1333, 0, 4230, 0, 0, 47, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1965, 1, 0, 6617, 3, 20, 24, 0, 1508423876),
(1334, 0, 4293, 0, 0, 47, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1996, 1, 0, 6720, 3, 20, 24, 0, 1508424187),
(1335, 0, 4353, 0, 0, 49, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2028, 1, 0, 6824, 3, 20, 24, 0, 1508424498),
(1336, 0, 4416, 0, 0, 49, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2059, 1, 0, 6928, 4, 20, 24, 0, 1508424809),
(1337, 0, 4479, 0, 0, 49, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2090, 1, 0, 7031, 3, 20, 24, 0, 1508425120),
(1338, 0, 4539, 0, 0, 51, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2121, 1, 0, 7135, 3, 20, 24, 0, 1508425431),
(1339, 0, 4602, 0, 0, 51, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2152, 1, 0, 7239, 5, 20, 24, 0, 1508425742),
(1340, 0, 4665, 0, 0, 51, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2183, 1, 0, 7342, 3, 20, 24, 0, 1508426053),
(1341, 0, 4728, 0, 0, 53, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2214, 1, 0, 7446, 3, 20, 24, 0, 1508426364),
(1342, 0, 4788, 0, 0, 53, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2245, 1, 0, 7549, 4, 20, 24, 0, 1508426675),
(1343, 0, 4851, 0, 0, 53, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2276, 1, 0, 7653, 3, 20, 24, 0, 1508426986),
(1344, 0, 4914, 0, 0, 55, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2307, 1, 0, 7757, 3, 20, 24, 0, 1508427297),
(1345, 0, 4977, 0, 0, 55, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2339, 1, 0, 7860, 3, 20, 24, 0, 1508427608),
(1346, 0, 5037, 0, 0, 55, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2370, 1, 0, 7964, 4, 20, 24, 0, 1508427919),
(1347, 0, 5100, 0, 0, 57, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2401, 1, 0, 8068, 4, 20, 24, 0, 1508428230),
(1348, 0, 5163, 0, 0, 57, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2432, 1, 0, 8171, 4, 20, 24, 0, 1508428541),
(1349, 0, 5223, 0, 0, 57, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2463, 1, 0, 8275, 4, 20, 24, 0, 1508428852),
(1350, 0, 5286, 0, 0, 59, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2494, 1, 0, 8379, 3, 20, 24, 0, 1508429163),
(1351, 0, 5349, 0, 0, 59, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2525, 1, 0, 8482, 3, 20, 24, 0, 1508429474),
(1352, 0, 5412, 0, 0, 59, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2556, 1, 0, 8586, 3, 20, 24, 0, 1508429785),
(1353, 0, 5472, 0, 0, 61, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2587, 1, 0, 8690, 3, 20, 24, 0, 1508430096),
(1354, 0, 5535, 0, 0, 61, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2618, 1, 0, 8793, 3, 20, 24, 0, 1508430407),
(1355, 0, 5598, 0, 0, 63, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2650, 1, 0, 8897, 4, 20, 24, 0, 1508430718),
(1356, 0, 5658, 0, 0, 63, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2681, 1, 0, 9001, 4, 20, 24, 0, 1508431029),
(1357, 0, 5721, 0, 0, 63, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2712, 1, 0, 9104, 3, 20, 24, 0, 1508431340),
(1358, 0, 5784, 0, 0, 65, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2743, 1, 0, 9208, 4, 20, 24, 0, 1508431651),
(1359, 0, 5847, 0, 0, 65, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2774, 1, 0, 9312, 3, 20, 24, 0, 1508431962),
(1360, 0, 5907, 0, 0, 65, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2805, 1, 0, 9415, 4, 20, 24, 0, 1508432273),
(1361, 0, 5970, 0, 0, 67, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2836, 1, 0, 9519, 4, 20, 24, 0, 1508432584),
(1362, 0, 6033, 0, 0, 67, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2867, 1, 0, 9623, 4, 20, 24, 0, 1508432895),
(1363, 0, 6096, 0, 0, 67, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2898, 1, 0, 9726, 3, 20, 24, 0, 1508433206),
(1364, 0, 6156, 0, 0, 69, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2929, 1, 0, 9830, 3, 20, 24, 0, 1508433517),
(1365, 0, 6219, 0, 0, 69, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2961, 1, 0, 9934, 3, 20, 24, 0, 1508433828),
(1366, 0, 6282, 0, 0, 69, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2992, 1, 0, 10037, 3, 19, 24, 0, 1508434139),
(1367, 0, 6342, 0, 0, 71, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 3023, 1, 0, 10141, 3, 19, 24, 0, 1508434450),
(1368, 0, 6405, 0, 0, 71, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 3054, 1, 0, 10244, 4, 19, 24, 0, 1508434761),
(1369, 0, 6468, 0, 0, 71, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 3085, 1, 0, 10348, 3, 19, 24, 0, 1508435072),
(1370, 0, 6531, 0, 0, 73, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 3116, 1, 0, 10452, 4, 19, 24, 0, 1508435383),
(1371, 0, 6591, 0, 0, 73, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 3147, 1, 0, 10555, 3, 19, 24, 0, 1508435694),
(1372, 0, 6654, 0, 0, 73, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 3178, 1, 0, 10659, 3, 19, 24, 0, 1508436005),
(1373, 0, 6717, 0, 0, 75, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 3209, 1, 0, 10763, 4, 19, 24, 0, 1508436316),
(1374, 0, 6780, 0, 0, 75, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 3240, 1, 0, 10866, 3, 19, 24, 0, 1508436627),
(1375, 0, 6840, 0, 0, 75, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 3272, 1, 0, 10970, 3, 19, 24, 0, 1508436938),
(1376, 0, 6903, 0, 0, 77, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 3303, 1, 0, 11074, 3, 19, 24, 0, 1508437249),
(1377, 0, 6966, 0, 0, 77, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 3334, 1, 0, 11177, 3, 19, 24, 0, 1508437560),
(1378, 0, 7026, 0, 0, 77, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 3365, 1, 0, 11281, 3, 19, 24, 0, 1508437871),
(1379, 0, 7089, 0, 0, 79, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 3396, 1, 0, 11385, 3, 19, 24, 0, 1508438182),
(1380, 0, 7152, 0, 0, 79, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 3427, 1, 0, 11488, 3, 19, 24, 0, 1508438493),
(1381, 0, 7215, 0, 0, 79, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 3458, 1, 0, 11592, 3, 19, 24, 0, 1508438804),
(1382, 0, 7275, 0, 0, 81, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 3489, 1, 0, 11696, 3, 19, 24, 0, 1508439115),
(1383, 0, 7338, 0, 0, 81, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 3520, 1, 0, 11799, 3, 19, 24, 0, 1508439426),
(1384, 0, 7401, 0, 0, 83, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 3551, 1, 0, 11903, 3, 19, 24, 0, 1508439737),
(1385, 0, 7461, 0, 0, 83, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 3583, 1, 0, 12007, 4, 19, 24, 0, 1508440048),
(1386, 0, 7524, 0, 0, 83, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 3614, 1, 0, 12110, 3, 19, 24, 0, 1508440359),
(1387, 0, 7587, 0, 0, 85, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 3645, 1, 0, 12214, 4, 19, 24, 0, 1508440670),
(1388, 0, 7650, 0, 0, 85, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 3676, 1, 0, 12317, 4, 19, 24, 0, 1508440981),
(1389, 0, 7710, 0, 0, 85, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 3707, 1, 0, 12421, 3, 19, 24, 0, 1508441292);
INSERT INTO `hcusyspmglobaldatainfo` (`sid`, `taskhcuvmec`, `taskhwinvec`, `tasksyspmec`, `taskmodbusec`, `taskcloudvelaec`, `taskavorionec`, `taskspsvirgoec`, `taskhsmmpec`, `taskemcec`, `taskpm25ec`, `taskwinddirec`, `taskwindspdec`, `tasktempec`, `taskhumidec`, `tasknoiseec`, `taskairprsec`, `taskco1ec`, `tasklightstrec`, `taskalcoholec`, `taskhchoec`, `tasktoxicgasec`, `restartcnt`, `cloudvelaconncnt`, `cloudvelaconnfailcnt`, `cloudveladisccnt`, `clouddatatimeoutcnt`, `socketdisccnt`, `cpuoccupy`, `memoccupy`, `diskoccupy`, `cputemp`, `timestamp`) VALUES
(1390, 0, 7773, 0, 0, 87, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 3738, 1, 0, 12525, 3, 19, 24, 0, 1508441603),
(1391, 0, 7836, 0, 0, 87, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 3769, 1, 0, 12628, 3, 19, 24, 0, 1508441914),
(1392, 0, 7899, 0, 0, 87, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 3800, 1, 0, 12732, 3, 19, 24, 0, 1508442225),
(1393, 0, 7959, 0, 0, 89, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 3831, 1, 0, 12836, 3, 19, 24, 0, 1508442536),
(1394, 0, 8022, 0, 0, 89, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 3862, 1, 0, 12939, 3, 19, 24, 0, 1508442847),
(1395, 0, 8085, 0, 0, 89, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 3894, 1, 0, 13043, 4, 19, 24, 0, 1508443158),
(1396, 0, 8145, 0, 0, 91, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 3925, 1, 0, 13147, 4, 19, 24, 0, 1508443469),
(1397, 0, 8208, 0, 0, 91, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 3956, 1, 0, 13250, 3, 19, 24, 0, 1508443780),
(1398, 0, 8271, 0, 0, 91, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 3987, 1, 0, 13354, 3, 19, 24, 0, 1508444091),
(1399, 0, 8334, 0, 0, 93, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 4018, 1, 0, 13458, 3, 19, 24, 0, 1508444402),
(1400, 0, 8394, 0, 0, 93, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 4049, 1, 0, 13561, 4, 19, 24, 0, 1508444713),
(1401, 0, 8457, 0, 0, 93, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 4080, 1, 0, 13665, 4, 19, 24, 0, 1508445024),
(1402, 0, 8520, 0, 0, 95, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 4111, 1, 0, 13769, 3, 19, 24, 0, 1508445335),
(1403, 0, 8583, 0, 0, 95, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 4142, 1, 0, 13872, 4, 19, 24, 0, 1508445646),
(1404, 0, 8643, 0, 0, 95, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 4173, 1, 0, 13976, 3, 19, 24, 0, 1508445957),
(1405, 0, 8706, 0, 0, 97, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 4205, 1, 0, 14080, 4, 19, 24, 0, 1508446268),
(1406, 0, 8769, 0, 0, 97, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 4236, 1, 0, 14183, 3, 19, 24, 0, 1508446579),
(1407, 0, 8829, 0, 0, 97, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 4267, 1, 0, 14287, 4, 19, 24, 0, 1508446890),
(1408, 0, 8892, 0, 0, 99, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 4298, 1, 0, 14391, 3, 19, 24, 0, 1508447201),
(1409, 0, 8955, 0, 0, 99, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 4329, 1, 0, 14494, 3, 19, 24, 0, 1508447512),
(1410, 0, 9018, 0, 0, 101, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 4360, 1, 0, 14598, 4, 19, 24, 0, 1508447823),
(1411, 0, 9078, 0, 0, 101, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 4391, 1, 0, 14701, 3, 19, 24, 0, 1508448135),
(1412, 0, 9141, 0, 0, 101, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 4422, 1, 0, 14805, 3, 19, 24, 0, 1508448445),
(1413, 0, 9204, 0, 0, 103, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 4453, 1, 0, 14909, 4, 19, 24, 0, 1508448756),
(1414, 0, 9267, 0, 0, 103, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 4484, 1, 0, 15012, 4, 19, 24, 0, 1508449067),
(1415, 0, 9327, 0, 0, 103, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 4516, 1, 0, 15116, 3, 19, 24, 0, 1508449378),
(1416, 0, 9390, 0, 0, 105, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 4547, 1, 0, 15220, 3, 19, 24, 0, 1508449689),
(1417, 0, 9453, 0, 0, 105, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 4578, 1, 0, 15323, 4, 19, 24, 0, 1508450000),
(1418, 0, 9513, 0, 0, 105, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 4609, 1, 0, 15427, 3, 19, 24, 0, 1508450311),
(1419, 0, 9576, 0, 0, 107, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 4640, 1, 0, 15531, 3, 19, 24, 0, 1508450622),
(1420, 0, 9639, 0, 0, 107, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 4671, 1, 0, 15634, 3, 19, 24, 0, 1508450933),
(1421, 0, 9702, 0, 0, 107, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 4702, 1, 0, 15738, 3, 19, 24, 0, 1508451244),
(1422, 0, 9762, 0, 0, 109, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 4733, 1, 0, 15842, 3, 19, 24, 0, 1508451555),
(1423, 0, 9825, 0, 0, 109, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 4764, 1, 0, 15945, 3, 19, 24, 0, 1508451866),
(1424, 0, 9888, 0, 0, 109, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 4795, 1, 0, 16049, 4, 19, 24, 0, 1508452177),
(1425, 0, 9948, 0, 0, 111, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 4827, 1, 0, 16153, 3, 19, 24, 0, 1508452488),
(1426, 0, 10011, 0, 0, 111, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 4858, 1, 0, 16256, 3, 19, 24, 0, 1508452799),
(1427, 0, 10074, 0, 0, 111, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 4889, 1, 0, 16360, 4, 19, 24, 0, 1508453110),
(1428, 0, 10137, 0, 0, 113, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 4920, 1, 0, 16464, 3, 19, 24, 0, 1508453421),
(1429, 0, 10197, 0, 0, 113, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 4951, 1, 0, 16567, 3, 19, 24, 0, 1508453732),
(1430, 0, 10260, 0, 0, 113, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 4982, 1, 0, 16671, 3, 19, 24, 0, 1508454043),
(1431, 0, 10323, 0, 0, 115, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 5013, 1, 0, 16774, 3, 19, 24, 0, 1508454354),
(1432, 0, 10386, 0, 0, 115, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 5044, 1, 0, 16878, 3, 19, 24, 0, 1508454665),
(1433, 0, 10446, 0, 0, 115, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 5075, 1, 0, 16982, 4, 19, 24, 0, 1508454976),
(1434, 0, 10509, 0, 0, 117, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 5106, 1, 0, 17085, 3, 19, 24, 0, 1508455287),
(1435, 0, 10572, 0, 0, 117, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 5138, 1, 0, 17189, 3, 19, 24, 0, 1508455598),
(1436, 0, 10632, 0, 0, 117, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 5169, 1, 0, 17293, 4, 19, 24, 0, 1508455909),
(1437, 0, 10695, 0, 0, 119, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 5200, 1, 0, 17396, 3, 23, 24, 0, 1508456220),
(1438, 0, 10758, 0, 0, 119, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 5231, 1, 0, 17500, 4, 23, 24, 0, 1508456531),
(1439, 0, 10821, 0, 0, 121, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 5262, 1, 0, 17604, 3, 23, 24, 0, 1508456842),
(1440, 0, 10881, 0, 0, 121, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 5293, 1, 0, 17707, 3, 23, 24, 0, 1508457153),
(1441, 0, 10944, 0, 0, 121, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 5318, 1, 0, 17748, 3, 23, 24, 0, 1508457464),
(1442, 0, 11007, 0, 0, 121, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 5318, 1, 0, 17748, 3, 23, 24, 0, 1508457775),
(1443, 0, 11070, 0, 0, 121, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 5318, 1, 0, 17748, 3, 23, 24, 0, 1508458086),
(1444, 0, 11130, 0, 0, 121, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 5318, 1, 0, 17748, 3, 23, 24, 0, 1508458397),
(1445, 0, 11193, 0, 0, 121, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 5318, 1, 0, 17748, 3, 23, 24, 0, 1508458708),
(1446, 0, 11256, 0, 0, 121, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 5318, 1, 0, 17748, 4, 23, 24, 0, 1508459019),
(1447, 0, 11316, 0, 0, 121, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 5318, 1, 0, 17748, 3, 23, 24, 0, 1508459330),
(1448, 0, 11379, 0, 0, 121, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 5318, 1, 0, 17748, 3, 23, 24, 0, 1508459641),
(1449, 0, 11442, 0, 0, 121, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 5318, 1, 0, 17748, 4, 21, 24, 0, 1508459952),
(1450, 0, 11505, 0, 0, 121, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 5318, 1, 0, 17748, 4, 21, 24, 0, 1508460263),
(1451, 0, 11565, 0, 0, 121, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 5318, 1, 0, 17748, 4, 21, 24, 0, 1508460574),
(1452, 0, 11628, 0, 0, 121, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 5318, 1, 0, 17748, 4, 21, 24, 0, 1508460885),
(1453, 0, 63, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 59, 6, 24, 0, 1508461504),
(1454, 0, 126, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 13, 7, 24, 0, 1508461815),
(1455, 0, 63, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 12, 7, 24, 0, 1508462735),
(1456, 0, 126, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 12, 8, 24, 0, 1508463047),
(1457, 0, 189, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 3, 10, 24, 0, 1508463357),
(1458, 0, 252, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 3, 11, 24, 0, 1508463668),
(1459, 0, 312, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 26, 12, 24, 0, 1508463979),
(1460, 0, 375, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 35, 14, 24, 0, 1508464291),
(1461, 0, 438, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 34, 14, 24, 0, 1508464601),
(1462, 0, 498, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 35, 15, 24, 0, 1508464913),
(1463, 0, 561, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 34, 15, 24, 0, 1508465223),
(1464, 0, 624, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 34, 16, 24, 0, 1508465535),
(1465, 0, 687, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 35, 17, 24, 0, 1508465845),
(1466, 0, 63, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 30, 6, 24, 0, 1508466504),
(1467, 0, 126, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 3, 7, 24, 0, 1508466815),
(1468, 0, 189, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 48, 8, 24, 0, 1508467127),
(1469, 0, 252, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 38, 9, 24, 0, 1508467437),
(1470, 0, 63, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 31, 7, 24, 0, 1508467906),
(1471, 0, 126, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 31, 9, 24, 0, 1508468217),
(1472, 0, 189, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 20, 10, 24, 0, 1508468528),
(1473, 0, 252, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 31, 12, 24, 0, 1508468840),
(1474, 0, 312, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 33, 14, 24, 0, 1508469150),
(1475, 0, 63, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 38, 7, 24, 0, 1508469828),
(1476, 0, 126, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 11, 8, 24, 0, 1508470139),
(1477, 0, 189, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 11, 9, 24, 0, 1508470450),
(1478, 0, 252, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 10, 10, 24, 0, 1508470761),
(1479, 0, 312, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 12, 11, 24, 0, 1508471072),
(1480, 0, 375, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 10, 13, 24, 0, 1508471383),
(1481, 0, 438, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 10, 14, 24, 0, 1508471694),
(1482, 0, 498, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 11, 14, 24, 0, 1508472005),
(1483, 0, 561, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 11, 15, 24, 0, 1508472316),
(1484, 0, 624, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 11, 15, 24, 0, 1508472627),
(1485, 0, 687, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 11, 16, 24, 0, 1508472938),
(1486, 0, 747, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 10, 16, 24, 0, 1508473249),
(1487, 0, 810, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 11, 17, 24, 0, 1508473560),
(1488, 0, 873, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 10, 17, 24, 0, 1508473871),
(1489, 0, 936, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 11, 18, 24, 0, 1508474182),
(1490, 0, 996, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 10, 18, 24, 0, 1508474493),
(1491, 0, 1059, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 12, 18, 24, 0, 1508474804),
(1492, 0, 1122, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 10, 18, 24, 0, 1508475115),
(1493, 0, 1182, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 11, 18, 24, 0, 1508475426),
(1494, 0, 1245, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 10, 19, 24, 0, 1508475737),
(1495, 0, 1308, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 11, 19, 24, 0, 1508476048),
(1496, 0, 1371, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 10, 19, 24, 0, 1508476359),
(1497, 0, 1431, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 11, 19, 24, 0, 1508476670),
(1498, 0, 1494, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 11, 19, 24, 0, 1508476981),
(1499, 0, 1557, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 11, 19, 24, 0, 1508477292),
(1500, 0, 1617, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 10, 19, 24, 0, 1508477603),
(1501, 0, 1680, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 10, 19, 24, 0, 1508477914),
(1502, 0, 1743, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 11, 19, 24, 0, 1508478225),
(1503, 0, 1806, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 11, 19, 24, 0, 1508478536),
(1504, 0, 63, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 11, 12, 24, 0, 1508479151),
(1505, 0, 126, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 18, 13, 24, 0, 1508479461),
(1506, 0, 63, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 4, 6, 25, 0, 1508480340),
(1507, 0, 126, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 4, 8, 25, 0, 1508480651),
(1508, 0, 63, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 5, 6, 24, 0, 1508481378),
(1509, 0, 126, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 13, 8, 24, 0, 1508481689),
(1510, 0, 63, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 24, 6, 24, 0, 1508482274),
(1511, 0, 63, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 30, 8, 24, 0, 1508482672),
(1512, 0, 126, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 13, 9, 24, 0, 1508482982),
(1513, 0, 189, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 11, 11, 24, 0, 1508483293),
(1514, 0, 252, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 5, 12, 25, 0, 1508483604),
(1515, 0, 312, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 4, 12, 25, 0, 1508483916),
(1516, 0, 375, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 4, 13, 25, 0, 1508484226),
(1517, 0, 438, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 5, 13, 25, 0, 1508484537),
(1518, 0, 63, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 13, 7, 25, 0, 1508485008),
(1519, 0, 63, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 5, 8, 25, 0, 1508485402),
(1520, 0, 63, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 33, 13, 25, 0, 1508486127);

-- --------------------------------------------------------

--
-- 表的结构 `hcusysswm_swdl`
--

CREATE TABLE `hcusysswm_swdl` (
  `sid` int(4) NOT NULL,
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
  `dltime` int(4) NOT NULL DEFAULT '0'
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- 表的结构 `hcusysswm_swpkg`
--

CREATE TABLE `hcusysswm_swpkg` (
  `sid` int(4) NOT NULL,
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
  `updatetime` int(4) NOT NULL DEFAULT '0'
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- 转存表中的数据 `hcusysswm_swpkg`
--

INSERT INTO `hcusysswm_swpkg` (`sid`, `equentry`, `hwtype`, `hwpem`, `swrel`, `swver`, `dbver`, `upgradeflag`, `swtotallen`, `swchecksum`, `dbtotallen`, `dbchecksum`, `filename`, `dbname`, `currentactive`, `updatetime`) VALUES
(2, 3, 8001, 3, 3, 180, 0, 3, 123448, 35465, 0, 0, 'IHU_HPT8001_PEM11_REL3_VER180_TRAIL.BIN', '', 'COMPLETE', 1508317570),
(6, 3, 8001, 3, 3, 210, 0, 4, 130764, 54904, 0, 0, 'IHU_HPT8001_PEM3_REL3_VER210_PATCH.BIN', '', 'COMPLETE', 1508372500),
(8, 1, 8001, 3, 3, 259, 199, 4, 1120491, 47719, 96, 8472, 'HCU_HPT8001_PEM3_REL3_VER259_PATCH.HEX', 'HCU_HPT8001_PEM3_REL3_VER199_MYSQL.SQL', 'COMPLETE', 1508373405),
(10, 1, 8001, 3, 3, 260, 199, 4, 1120491, 47719, 96, 8472, 'HCU_HPT8001_PEM3_REL3_VER260_PATCH.HEX', 'HCU_HPT8001_PEM3_REL3_VER199_MYSQL.SQL', 'COMPLETE', 1508396645),
(12, 3, 8001, 3, 3, 211, 0, 4, 130764, 54904, 0, 0, 'IHU_HPT8001_PEM3_REL3_VER211_PATCH.BIN', '', 'COMPLETE', 1508396687),
(14, 1, 8001, 3, 3, 261, 199, 4, 1120491, 47719, 96, 8472, 'HCU_HPT8001_PEM3_REL3_VER261_PATCH.HEX', 'HCU_HPT8001_PEM3_REL3_VER199_MYSQL.SQL', 'COMPLETE', 1508399181),
(16, 3, 8001, 3, 3, 212, 0, 4, 130764, 54904, 0, 0, 'IHU_HPT8001_PEM3_REL3_VER212_PATCH.BIN', '', 'COMPLETE', 1508401878),
(19, 1, 8001, 3, 3, 262, 200, 4, 4802856, 12098, 363, 36199, 'HCU_HPT8001_PEM3_REL3_VER262_PATCH.HEX', 'HCU_HPT8001_PEM3_REL3_VER200_MYSQL.SQL', 'COMPLETE', 1508457728),
(22, 3, 8001, 3, 3, 213, 0, 4, 130744, 55423, 0, 0, 'IHU_HPT8001_PEM3_REL3_VER213_PATCH.BIN', '', 'COMPLETE', 1508473170),
(26, 3, 8001, 3, 3, 180, 0, 2, 57912, 35465, 0, 0, 'IHU_HPT8001_PEM11_REL3_VER180_STABLE.BIN', '', 'COMPLETE', 1508478892),
(29, 1, 8001, 3, 3, 263, 200, 4, 4793489, 10835, 363, 36199, 'HCU_HPT8001_PEM3_REL3_VER263_PATCH.HEX', 'HCU_HPT8001_PEM3_REL3_VER200_MYSQL.SQL', 'COMPLETE', 1508479697),
(33, 1, 8001, 3, 3, 264, 200, 4, 1138006, 34797, 363, 36199, 'HCU_HPT8001_PEM3_REL3_VER264_PATCH.HEX', 'HCU_HPT8001_PEM3_REL3_VER200_MYSQL.SQL', 'COMPLETE', 1508481783),
(37, 1, 8001, 3, 3, 265, 200, 4, 1138284, 23327, 363, 36199, 'HCU_HPT8001_PEM3_REL3_VER265_PATCH.HEX', 'HCU_HPT8001_PEM3_REL3_VER200_MYSQL.SQL', 'COMPLETE', 1508485600),
(39, 3, 8001, 3, 3, 214, 0, 4, 130760, 56330, 0, 0, 'IHU_HPT8001_PEM3_REL3_VER214_PATCH.BIN', '', 'COMPLETE', 1508485642);

-- --------------------------------------------------------

--
-- 表的结构 `hcutempbmp180datainfo`
--

CREATE TABLE `hcutempbmp180datainfo` (
  `sid` int(4) NOT NULL,
  `deviceid` int(4) NOT NULL,
  `timestamp` int(4) NOT NULL,
  `dataformat` int(1) NOT NULL,
  `tempvalue` int(4) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- 表的结构 `hcutempdatainfo`
--

CREATE TABLE `hcutempdatainfo` (
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
-- 表的结构 `hcutempdht11datainfo`
--

CREATE TABLE `hcutempdht11datainfo` (
  `sid` int(4) NOT NULL,
  `deviceid` int(4) NOT NULL,
  `timestamp` int(4) NOT NULL,
  `dataformat` int(1) NOT NULL,
  `tempvalue` int(4) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- 表的结构 `hcutempmth01datainfo`
--

CREATE TABLE `hcutempmth01datainfo` (
  `sid` int(4) NOT NULL,
  `deviceid` int(4) NOT NULL,
  `timestamp` int(4) NOT NULL,
  `dataformat` int(1) NOT NULL,
  `tempvalue` int(4) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- 表的结构 `hcutemprht03datainfo`
--

CREATE TABLE `hcutemprht03datainfo` (
  `sid` int(4) NOT NULL,
  `deviceid` int(4) NOT NULL,
  `timestamp` int(4) NOT NULL,
  `dataformat` int(1) NOT NULL,
  `tempvalue` int(4) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- 表的结构 `hcutempsht20datainfo`
--

CREATE TABLE `hcutempsht20datainfo` (
  `sid` int(4) NOT NULL,
  `deviceid` int(4) NOT NULL,
  `timestamp` int(4) NOT NULL,
  `dataformat` int(1) NOT NULL,
  `tempvalue` int(4) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- 表的结构 `hcutoxicgasdatainfo`
--

CREATE TABLE `hcutoxicgasdatainfo` (
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
-- 表的结构 `hcutoxicgasmq135datainfo`
--

CREATE TABLE `hcutoxicgasmq135datainfo` (
  `sid` int(4) NOT NULL,
  `deviceid` int(4) NOT NULL,
  `timestamp` int(4) NOT NULL,
  `dataformat` int(1) NOT NULL,
  `toxicgasvalue` int(4) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- 表的结构 `hcutoxicgaszp01vocdatainfo`
--

CREATE TABLE `hcutoxicgaszp01vocdatainfo` (
  `sid` int(4) NOT NULL,
  `deviceid` int(4) NOT NULL,
  `timestamp` int(4) NOT NULL,
  `dataformat` int(1) NOT NULL,
  `toxicgasvalue` int(4) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- 表的结构 `hcutracemodulectr`
--

CREATE TABLE `hcutracemodulectr` (
  `moduleid` int(2) NOT NULL,
  `modulename` char(15) NOT NULL,
  `modulectrflag` int(1) NOT NULL,
  `moduletoallow` int(1) NOT NULL,
  `moduletorestrict` int(1) NOT NULL,
  `modulefromallow` int(1) NOT NULL,
  `modulefromrestrict` int(1) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- 转存表中的数据 `hcutracemodulectr`
--

INSERT INTO `hcutracemodulectr` (`moduleid`, `modulename`, `modulectrflag`, `moduletoallow`, `moduletorestrict`, `modulefromallow`, `modulefromrestrict`) VALUES
(0, 'TASKMIN', 1, 1, 1, 1, 1),
(1, 'HCUMAIN', 1, 1, 1, 1, 1),
(2, 'HCUVM', 1, 1, 1, 1, 1),
(3, 'TRACE', 1, 1, 1, 1, 1),
(4, 'CONFIG', 1, 1, 1, 1, 1),
(5, 'TIMER', 1, 1, 1, 1, 1),
(6, 'MMC', 1, 1, 1, 1, 1),
(7, 'MQTT', 1, 1, 1, 1, 1),
(8, 'GPIO', 1, 1, 1, 1, 1),
(9, 'I2C', 1, 1, 1, 1, 1),
(10, 'SPI', 1, 1, 1, 1, 1),
(11, 'PWM', 1, 1, 1, 1, 1),
(12, 'ADC', 1, 1, 1, 1, 1),
(13, 'SWITCH', 1, 1, 1, 1, 1),
(14, 'RELAY', 1, 1, 1, 1, 1),
(15, 'MOTOR', 1, 1, 1, 1, 1),
(16, 'ZEEGBE', 1, 1, 1, 1, 1),
(17, 'GPRS', 1, 1, 1, 1, 1),
(18, 'SPS232', 1, 1, 1, 1, 1),
(19, 'SPS485', 1, 1, 1, 1, 1),
(20, 'BLE', 1, 1, 1, 1, 1),
(21, 'ETHERNET', 1, 1, 1, 1, 1),
(22, 'WIFI', 1, 1, 1, 1, 1),
(23, 'USBNET', 1, 1, 1, 1, 1),
(24, '3G4G', 1, 1, 1, 1, 1),
(25, 'HARDDISK', 1, 1, 1, 1, 1),
(26, 'CAMERA', 1, 1, 1, 1, 1),
(27, 'MICROPHONE', 1, 1, 1, 1, 1),
(28, 'FLASH', 1, 1, 1, 1, 1),
(29, 'GPS', 1, 1, 1, 1, 1),
(30, 'LCD', 1, 1, 1, 1, 1),
(31, 'LED', 1, 1, 1, 1, 1),
(32, 'HWINV', 1, 1, 1, 1, 1),
(33, 'SPSVIRGO', 1, 1, 1, 1, 1),
(34, 'CLOUDVELA', 1, 1, 1, 1, 1),
(35, 'MODBUS', 1, 1, 1, 1, 1),
(36, 'AVORION', 1, 1, 1, 1, 1),
(37, 'I2CBUSLIBRA', 1, 1, 1, 1, 1),
(38, 'SPIBUSARIES', 1, 1, 1, 1, 1),
(39, 'NBIOTCJ188', 1, 1, 1, 1, 1),
(40, 'NBIOTQG376', 1, 1, 1, 1, 1),
(41, 'LLCZHB', 1, 1, 1, 1, 1),
(42, 'HSMMP', 1, 1, 1, 1, 1),
(43, 'EMC', 1, 1, 1, 1, 1),
(44, 'HUMID', 1, 1, 1, 1, 1),
(45, 'PM25', 1, 1, 1, 1, 1),
(46, 'TEMP', 1, 1, 1, 1, 1),
(47, 'WINDDIR', 1, 1, 1, 1, 1),
(48, 'WINDSPD', 1, 1, 1, 1, 1),
(49, 'NOISE', 1, 1, 1, 1, 1),
(50, 'AIRPRS', 1, 1, 1, 1, 1),
(51, 'CO1', 1, 1, 1, 1, 1),
(52, 'LIGHTSTR', 1, 1, 1, 1, 1),
(53, 'ALCOHOL', 1, 1, 1, 1, 1),
(54, 'HCHO', 1, 1, 1, 1, 1),
(55, 'TOXICGAS', 1, 1, 1, 1, 1),
(56, 'IWM', 1, 1, 1, 1, 1),
(57, 'IHM', 1, 1, 1, 1, 1),
(58, 'IGM', 1, 1, 1, 1, 1),
(59, 'IPM', 1, 1, 1, 1, 1),
(60, 'SVRCON', 1, 1, 1, 1, 1),
(61, 'SYSPM', 1, 1, 1, 1, 1),
(62, 'SYSSWM', 1, 1, 1, 1, 1),
(63, 'PM25SHARP', 1, 1, 1, 1, 1),
(64, 'CANITFLEO', 1, 1, 1, 1, 1),
(65, 'L3AQYCG10', 1, 1, 1, 1, 1),
(66, 'L3AQYCG20', 1, 1, 1, 1, 1),
(67, 'L3TBSWRG30', 1, 1, 1, 1, 1),
(68, 'L3GQYBG40', 1, 1, 1, 1, 1),
(69, 'L3CXILC', 1, 1, 1, 1, 1),
(70, 'L3CXGLACM', 1, 1, 1, 1, 1),
(71, 'L3NBLPM', 1, 1, 1, 1, 1),
(72, 'L3NBHPM', 1, 1, 1, 1, 1),
(73, 'L3BFSC', 1, 1, 1, 1, 1),
(74, 'BFSCUICOMM', 1, 1, 1, 1, 1),
(75, 'JSONINOTIFY', 1, 1, 1, 1, 1),
(76, 'BFDFUICOMM', 1, 1, 1, 1, 1),
(77, 'BFHSUICOMM', 1, 1, 1, 1, 1),
(78, 'L3OPWLOTDR', 1, 1, 1, 1, 1),
(79, 'L3BFDF', 1, 1, 1, 1, 1),
(80, 'L3BFHS', 1, 1, 1, 1, 1),
(81, 'L3HATE', 1, 1, 1, 1, 1),
(82, 'TASKMAX', 1, 1, 1, 1, 1);

-- --------------------------------------------------------

--
-- 表的结构 `hcutracemsgctr`
--

CREATE TABLE `hcutracemsgctr` (
  `msgid` int(2) NOT NULL,
  `msgname` char(70) NOT NULL,
  `msgctrflag` int(1) NOT NULL,
  `msgallow` int(1) NOT NULL,
  `msgrestrict` int(1) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- 转存表中的数据 `hcutracemsgctr`
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
(13, 'MSG_ID_COM_MQTT_SEND', 1, 1, 1),
(14, 'MSG_ID_COM_MQTT_RCV', 1, 1, 1),
(15, 'MSG_ID_ETHERNET_CLOUDVELA_DATA_RX', 1, 1, 1),
(16, 'MSG_ID_ETHERNET_NBIOTCJ188_DATA_RX', 1, 1, 1),
(17, 'MSG_ID_ETHERNET_NBIOTQG376_DATA_RX', 1, 1, 1),
(18, 'MSG_ID_ETHERNET_CLOUDVELA_SOCKET_DATA_RX', 1, 1, 1),
(19, 'MSG_ID_WIFI_CLOUDVELA_DATA_RX', 1, 1, 1),
(20, 'MSG_ID_USBNET_CLOUDVELA_DATA_RX', 1, 1, 1),
(21, 'MSG_ID_3G4G_CLOUDVELA_DATA_RX', 1, 1, 1),
(22, 'MSG_ID_SPS232_MODBUS_DATA_RX', 1, 1, 1),
(23, 'MSG_ID_SPS485_MODBUS_DATA_RX', 1, 1, 1),
(24, 'MSG_ID_SPSVIRGO_HSMMP_DATA_RX', 1, 1, 1),
(25, 'MSG_ID_SPSVIRGO_NOISE_DATA_REPORT', 1, 1, 1),
(26, 'MSG_ID_SPSVIRGO_NOISE_CONTROL_FB', 1, 1, 1),
(27, 'MSG_ID_AVORION_HSMMP_DATA_READ_FB', 1, 1, 1),
(28, 'MSG_ID_AVORION_HSMMP_DATA_RX', 1, 1, 1),
(29, 'MSG_ID_BLE_HSMMP_DATA_RX', 1, 1, 1),
(30, 'MSG_ID_BLE_MODBUS_DATA_RX', 1, 1, 1),
(31, 'MSG_ID_LCD_AVORION_DATA_RX', 1, 1, 1),
(32, 'MSG_ID_CAMERA_AVORION_DATA_RX', 1, 1, 1),
(33, 'MSG_ID_MICROPHONE_AVORION_DATA_RX', 1, 1, 1),
(34, 'MSG_ID_HWINV_CLOUDVELA_PHY_STATUS_CHG', 1, 1, 1),
(35, 'MSG_ID_NBIOTCJ188_IWM_DATA_REQ', 1, 1, 1),
(36, 'MSG_ID_IWM_NBIOTCJ188_DATA_RESP', 1, 1, 1),
(37, 'MSG_ID_NBIOTCJ188_IWM_CTRL_REQ', 1, 1, 1),
(38, 'MSG_ID_IWM_NBIOTCJ188_CTRL_RESP', 1, 1, 1),
(39, 'MSG_ID_NBIOTCJ188_IHM_DATA_REQ', 1, 1, 1),
(40, 'MSG_ID_IHM_NBIOTCJ188_DATA_RESP', 1, 1, 1),
(41, 'MSG_ID_NBIOTCJ188_IHM_CTRL_REQ', 1, 1, 1),
(42, 'MSG_ID_IHM_NBIOTCJ188_CTRL_RESP', 1, 1, 1),
(43, 'MSG_ID_NBIOTCJ188_IGM_DATA_REQ', 1, 1, 1),
(44, 'MSG_ID_IGM_NBIOTCJ188_DATA_RESP', 1, 1, 1),
(45, 'MSG_ID_NBIOTCJ188_IGM_CTRL_REQ', 1, 1, 1),
(46, 'MSG_ID_IGM_NBIOTCJ188_CTRL_RESP', 1, 1, 1),
(47, 'MSG_ID_NBIOTCJ188_IPM_DATA_REQ', 1, 1, 1),
(48, 'MSG_ID_IPM_NBIOTCJ188_DATA_RESP', 1, 1, 1),
(49, 'MSG_ID_NBIOTCJ188_IPM_CTRL_REQ', 1, 1, 1),
(50, 'MSG_ID_IPM_NBIOTCJ188_CTRL_RESP', 1, 1, 1),
(51, 'MSG_ID_NBIOTQG376_IPM_DATA_REQ', 1, 1, 1),
(52, 'MSG_ID_IPM_NBIOTQG376_DATA_RESP', 1, 1, 1),
(53, 'MSG_ID_NBIOTQG376_IPM_CTRL_REQ', 1, 1, 1),
(54, 'MSG_ID_IPM_NBIOTQG376_CTRL_RESP', 1, 1, 1),
(55, 'MSG_ID_MODBUS_EMC_DATA_REPORT', 1, 1, 1),
(56, 'MSG_ID_MODBUS_EMC_CONTROL_FB', 1, 1, 1),
(57, 'MSG_ID_MODBUS_PM25_DATA_REPORT', 1, 1, 1),
(58, 'MSG_ID_MODBUS_PM25_CONTROL_FB', 1, 1, 1),
(59, 'MSG_ID_MODBUS_WINDDIR_DATA_REPORT', 1, 1, 1),
(60, 'MSG_ID_MODBUS_WINDDIR_CONTROL_FB', 1, 1, 1),
(61, 'MSG_ID_MODBUS_WINDSPD_DATA_REPORT', 1, 1, 1),
(62, 'MSG_ID_MODBUS_WINDSPD_CONTROL_FB', 1, 1, 1),
(63, 'MSG_ID_MODBUS_TEMP_DATA_REPORT', 1, 1, 1),
(64, 'MSG_ID_MODBUS_TEMP_CONTROL_FB', 1, 1, 1),
(65, 'MSG_ID_MODBUS_HUMID_DATA_REPORT', 1, 1, 1),
(66, 'MSG_ID_MODBUS_HUMID_CONTROL_FB', 1, 1, 1),
(67, 'MSG_ID_MODBUS_NOISE_DATA_REPORT', 1, 1, 1),
(68, 'MSG_ID_MODBUS_NOISE_CONTROL_FB', 1, 1, 1),
(69, 'MSG_ID_EMC_MODBUS_DATA_READ', 1, 1, 1),
(70, 'MSG_ID_EMC_MODBUS_CONTROL_CMD', 1, 1, 1),
(71, 'MSG_ID_PM25_MODBUS_DATA_READ', 1, 1, 1),
(72, 'MSG_ID_PM25_MODBUS_CONTROL_CMD', 1, 1, 1),
(73, 'MSG_ID_WINDDIR_MODBUS_DATA_READ', 1, 1, 1),
(74, 'MSG_ID_WINDDIR_MODBUS_CONTROL_CMD', 1, 1, 1),
(75, 'MSG_ID_WINDSPD_MODBUS_DATA_READ', 1, 1, 1),
(76, 'MSG_ID_WINDSPD_MODBUS_CONTROL_CMD', 1, 1, 1),
(77, 'MSG_ID_TEMP_MODBUS_DATA_READ', 1, 1, 1),
(78, 'MSG_ID_TEMP_SPIBUSARIES_DATA_READ', 1, 1, 1),
(79, 'MSG_ID_TEMP_MODBUS_CONTROL_CMD', 1, 1, 1),
(80, 'MSG_ID_TEMP_SPIBUSARIES_CONTROL_CMD', 1, 1, 1),
(81, 'MSG_ID_HUMID_MODBUS_DATA_READ', 1, 1, 1),
(82, 'MSG_ID_HUMID_MODBUS_CONTROL_CMD', 1, 1, 1),
(83, 'MSG_ID_HSMMP_AVORION_DATA_READ', 1, 1, 1),
(84, 'MSG_ID_HSMMP_AVORION_STOP', 1, 1, 1),
(85, 'MSG_ID_NOISE_SPSVIRGO_DATA_READ', 1, 1, 1),
(86, 'MSG_ID_NOISE_SPSVIRGO_CONTROL_CMD', 1, 1, 1),
(87, 'MSG_ID_NOISE_SPSVIRGO_STOP', 1, 1, 1),
(88, 'MSG_ID_NOISE_MODBUS_DATA_READ', 1, 1, 1),
(89, 'MSG_ID_NOISE_MODBUS_CONTROL_CMD', 1, 1, 1),
(90, 'MSG_ID_CLOUDVELA_SYSPM_ALARM_REQ', 1, 1, 1),
(91, 'MSG_ID_SYSPM_CLOUDVELA_ALARM_RESP', 1, 1, 1),
(92, 'MSG_ID_SYSPM_CLOUDVELA_ALARM_REPORT', 1, 1, 1),
(93, 'MSG_ID_CLOUDVELA_SYSPM_ALARM_CONFIRM', 1, 1, 1),
(94, 'MSG_ID_CLOUDVELA_SYSPM_PERFM_REQ', 1, 1, 1),
(95, 'MSG_ID_SYSPM_CLOUDVELA_PERFM_RESP', 1, 1, 1),
(96, 'MSG_ID_SYSPM_CLOUDVELA_PERFM_REPORT', 1, 1, 1),
(97, 'MSG_ID_CLOUDVELA_SYSPM_PERFM_CONFIRM', 1, 1, 1),
(98, 'MSG_ID_CLOUDVELA_SYSSWM_INVENTORY_REQ', 1, 1, 1),
(99, 'MSG_ID_SYSSWM_CLOUDVELA_INVENTORY_RESP', 1, 1, 1),
(100, 'MSG_ID_SYSSWM_CLOUDVELA_INVENTORY_REPORT', 1, 1, 1),
(101, 'MSG_ID_CLOUDVELA_SYSSWM_INVENTORY_CONFIRM', 1, 1, 1),
(102, 'MSG_ID_CLOUDVELA_SYSSWM_SW_PACKAGE_REQ', 1, 1, 1),
(103, 'MSG_ID_SYSSWM_CLOUDVELA_SW_PACKAGE_RESP', 1, 1, 1),
(104, 'MSG_ID_SYSSWM_CLOUDVELA_SW_PACKAGE_REPORT', 1, 1, 1),
(105, 'MSG_ID_CLOUDVELA_SYSSWM_SW_PACKAGE_CONFIRM', 1, 1, 1),
(106, 'MSG_ID_CLOUDVELA_TEST_COMMAND_REQ', 1, 1, 1),
(107, 'MSG_ID_CLOUDVELA_TEST_COMMAND_RESP', 1, 1, 1),
(108, 'MSG_ID_CLOUDVELA_TEST_COMMAND_REPORT', 1, 1, 1),
(109, 'MSG_ID_CLOUDVELA_TEST_COMMAND_CONFIRM', 1, 1, 1),
(110, 'MSG_ID_CLOUDVELA_EMC_DATA_REQ', 1, 1, 1),
(111, 'MSG_ID_EMC_CLOUDVELA_DATA_RESP', 1, 1, 1),
(112, 'MSG_ID_CLOUDVELA_EMC_CTRL_REQ', 1, 1, 1),
(113, 'MSG_ID_EMC_CLOUDVELA_CTRL_RESP', 1, 1, 1),
(114, 'MSG_ID_EMC_CLOUDVELA_DATA_REPORT', 1, 1, 1),
(115, 'MSG_ID_CLOUDVELA_EMC_DATA_CONFIRM', 1, 1, 1),
(116, 'MSG_ID_CLOUDVELA_PM25_DATA_REQ', 1, 1, 1),
(117, 'MSG_ID_PM25_CLOUDVELA_DATA_RESP', 1, 1, 1),
(118, 'MSG_ID_CLOUDVELA_PM25_CTRL_REQ', 1, 1, 1),
(119, 'MSG_ID_PM25_CLOUDVELA_CTRL_RESP', 1, 1, 1),
(120, 'MSG_ID_PM25_CLOUDVELA_DATA_REPORT', 1, 1, 1),
(121, 'MSG_ID_CLOUDVELA_PM25_DATA_CONFIRM', 1, 1, 1),
(122, 'MSG_ID_CLOUDVELA_TEMP_DATA_REQ', 1, 1, 1),
(123, 'MSG_ID_TEMP_CLOUDVELA_DATA_RESP', 1, 1, 1),
(124, 'MSG_ID_CLOUDVELA_TEMP_CTRL_REQ', 1, 1, 1),
(125, 'MSG_ID_TEMP_CLOUDVELA_CTRL_RESP', 1, 1, 1),
(126, 'MSG_ID_TEMP_CLOUDVELA_DATA_REPORT', 1, 1, 1),
(127, 'MSG_ID_CLOUDVELA_TEMP_DATA_CONFIRM', 1, 1, 1),
(128, 'MSG_ID_CLOUDVELA_HUMID_DATA_REQ', 1, 1, 1),
(129, 'MSG_ID_HUMID_CLOUDVELA_DATA_RESP', 1, 1, 1),
(130, 'MSG_ID_CLOUDVELA_HUMID_CTRL_REQ', 1, 1, 1),
(131, 'MSG_ID_HUMID_CLOUDVELA_CTRL_RESP', 1, 1, 1),
(132, 'MSG_ID_HUMID_CLOUDVELA_DATA_REPORT', 1, 1, 1),
(133, 'MSG_ID_CLOUDVELA_HUMID_DATA_CONFIRM', 1, 1, 1),
(134, 'MSG_ID_CLOUDVELA_WINDDIR_DATA_REQ', 1, 1, 1),
(135, 'MSG_ID_WINDDIR_CLOUDVELA_DATA_RESP', 1, 1, 1),
(136, 'MSG_ID_CLOUDVELA_WINDDIR_CTRL_REQ', 1, 1, 1),
(137, 'MSG_ID_WINDDIR_CLOUDVELA_CTRL_RESP', 1, 1, 1),
(138, 'MSG_ID_WINDDIR_CLOUDVELA_DATA_REPORT', 1, 1, 1),
(139, 'MSG_ID_CLOUDVELA_WINDDIR_DATA_CONFIRM', 1, 1, 1),
(140, 'MSG_ID_CLOUDVELA_WINDSPD_DATA_REQ', 1, 1, 1),
(141, 'MSG_ID_WINDSPD_CLOUDVELA_DATA_RESP', 1, 1, 1),
(142, 'MSG_ID_CLOUDVELA_WINDSPD_CTRL_REQ', 1, 1, 1),
(143, 'MSG_ID_WINDSPD_CLOUDVELA_CTRL_RESP', 1, 1, 1),
(144, 'MSG_ID_WINDSPD_CLOUDVELA_DATA_REPORT', 1, 1, 1),
(145, 'MSG_ID_CLOUDVELA_WINDSPD_DATA_CONFIRM', 1, 1, 1),
(146, 'MSG_ID_CLOUDVELA_HSMMP_DATA_REQ', 1, 1, 1),
(147, 'MSG_ID_HSMMP_CLOUDVELA_DATA_RESP', 1, 1, 1),
(148, 'MSG_ID_CLOUDVELA_HSMMP_CTRL_REQ', 1, 1, 1),
(149, 'MSG_ID_HSMMP_CLOUDVELA_CTRL_RESP', 1, 1, 1),
(150, 'MSG_ID_HSMMP_CLOUDVELA_DATA_REPORT', 1, 1, 1),
(151, 'MSG_ID_CLOUDVELA_HSMMP_DATA_CONFIRM', 1, 1, 1),
(152, 'MSG_ID_PICTURE_CLOUDVELA_DATA_REPORT', 1, 1, 1),
(153, 'MSG_ID_CLOUDVELA_PICTURE_DATA_CONFIRM', 1, 1, 1),
(154, 'MSG_ID_CLOUDVELA_NOISE_DATA_REQ', 1, 1, 1),
(155, 'MSG_ID_NOISE_CLOUDVELA_DATA_RESP', 1, 1, 1),
(156, 'MSG_ID_CLOUDVELA_NOISE_CTRL_REQ', 1, 1, 1),
(157, 'MSG_ID_NOISE_CLOUDVELA_CTRL_RESP', 1, 1, 1),
(158, 'MSG_ID_NOISE_CLOUDVELA_DATA_REPORT', 1, 1, 1),
(159, 'MSG_ID_CLOUDVELA_NOISE_DATA_CONFIRM', 1, 1, 1),
(160, 'MSG_ID_CLOUDVELA_YCJK_DATA_REQ', 1, 1, 1),
(161, 'MSG_ID_YCJK_CLOUDVELA_DATA_RESP', 1, 1, 1),
(162, 'MSG_ID_CLOUDVELA_YCJK_CTRL_REQ', 1, 1, 1),
(163, 'MSG_ID_YCJK_CLOUDVELA_CTRL_RESP', 1, 1, 1),
(164, 'MSG_ID_YCJK_CLOUDVELA_DATA_REPORT', 1, 1, 1),
(165, 'MSG_ID_CLOUDVELA_YCJK_DATA_CONFIRM', 1, 1, 1),
(166, 'MSG_ID_CLOUDVELA_AIRPRS_DATA_REQ', 1, 1, 1),
(167, 'MSG_ID_AIRPRS_CLOUDVELA_DATA_RESP', 1, 1, 1),
(168, 'MSG_ID_AIRPRS_CLOUDVELA_DATA_REPORT', 1, 1, 1),
(169, 'MSG_ID_CLOUDVELA_AIRPRS_DATA_CONFIRM', 1, 1, 1),
(170, 'MSG_ID_CLOUDVELA_ALCOHOL_DATA_REQ', 1, 1, 1),
(171, 'MSG_ID_ALCOHOL_CLOUDVELA_DATA_RESP', 1, 1, 1),
(172, 'MSG_ID_ALCOHOL_CLOUDVELA_DATA_REPORT', 1, 1, 1),
(173, 'MSG_ID_CLOUDVELA_ALCOHOL_DATA_CONFIRM', 1, 1, 1),
(174, 'MSG_ID_CLOUDVELA_CO1_DATA_REQ', 1, 1, 1),
(175, 'MSG_ID_CO1_CLOUDVELA_DATA_RESP', 1, 1, 1),
(176, 'MSG_ID_CO1_CLOUDVELA_DATA_REPORT', 1, 1, 1),
(177, 'MSG_ID_CLOUDVELA_CO1_DATA_CONFIRM', 1, 1, 1),
(178, 'MSG_ID_CLOUDVELA_HCHO_DATA_REQ', 1, 1, 1),
(179, 'MSG_ID_HCHO_CLOUDVELA_DATA_RESP', 1, 1, 1),
(180, 'MSG_ID_HCHO_CLOUDVELA_DATA_REPORT', 1, 1, 1),
(181, 'MSG_ID_CLOUDVELA_HCHO_DATA_CONFIRM', 1, 1, 1),
(182, 'MSG_ID_CLOUDVELA_LIGHTSTR_DATA_REQ', 1, 1, 1),
(183, 'MSG_ID_LIGHTSTR_CLOUDVELA_DATA_RESP', 1, 1, 1),
(184, 'MSG_ID_LIGHTSTR_CLOUDVELA_DATA_REPORT', 1, 1, 1),
(185, 'MSG_ID_CLOUDVELA_LIGHTSTR_DATA_CONFIRM', 1, 1, 1),
(186, 'MSG_ID_CLOUDVELA_PM25SP_DATA_REQ', 1, 1, 1),
(187, 'MSG_ID_PM25SP_CLOUDVELA_DATA_RESP', 1, 1, 1),
(188, 'MSG_ID_PM25SP_CLOUDVELA_DATA_REPORT', 1, 1, 1),
(189, 'MSG_ID_CLOUDVELA_PM25SP_DATA_CONFIRM', 1, 1, 1),
(190, 'MSG_ID_CLOUDVELA_TOXICGAS_DATA_REQ', 1, 1, 1),
(191, 'MSG_ID_TOXICGAS_CLOUDVELA_DATA_RESP', 1, 1, 1),
(192, 'MSG_ID_TOXICGAS_CLOUDVELA_DATA_REPORT', 1, 1, 1),
(193, 'MSG_ID_CLOUDVELA_TOXICGAS_DATA_CONFIRM', 1, 1, 1),
(194, 'MSG_ID_CANITFLEO_DATA_REPORT', 1, 1, 1),
(195, 'MSG_ID_CANITFLEO_SYSSWM_INVENTORY_REPORT', 1, 1, 1),
(196, 'MSG_ID_SYSSWM_CANITFLEO_INVENTORY_CONFIRM', 1, 1, 1),
(197, 'MSG_ID_CANITFLEO_SYSSWM_SW_PACKAGE_REPORT', 1, 1, 1),
(198, 'MSG_ID_SYSSWM_CANITFLEO_SW_PACKAGE_CONFIRM', 1, 1, 1),
(199, 'MSG_ID_CLOUDVELA_L3BFSC_DATA_REQ', 1, 1, 1),
(200, 'MSG_ID_L3BFSC_CLOUDVELA_DATA_RESP', 1, 1, 1),
(201, 'MSG_ID_L3BFSC_CLOUDVELA_DATA_REPORT', 1, 1, 1),
(202, 'MSG_ID_CLOUDVELA_L3BFSC_DATA_CONFIRM', 1, 1, 1),
(203, 'MSG_ID_L3BFSC_CLOUDVELA_EVENT_REPORT', 1, 1, 1),
(204, 'MSG_ID_CLOUDVELA_L3BFSC_EVENT_CONFIRM', 1, 1, 1),
(205, 'MSG_ID_CLOUDVELA_L3BFSC_CTRL_REQ', 1, 1, 1),
(206, 'MSG_ID_L3BFSC_CLOUDVELA_CMD_RESP', 1, 1, 1),
(207, 'MSG_ID_L3BFSC_CLOUDVELA_STATISTIC_REPORT', 1, 1, 1),
(208, 'MSG_ID_CLOUDVELA_L3BFSC_STATISTIC_CONFIRM', 1, 1, 1),
(209, 'MSG_ID_CLOUDVELA_L3BFDF_DATA_REQ', 1, 1, 1),
(210, 'MSG_ID_L3BFDF_CLOUDVELA_DATA_RESP', 1, 1, 1),
(211, 'MSG_ID_L3BFDF_CLOUDVELA_DATA_REPORT', 1, 1, 1),
(212, 'MSG_ID_CLOUDVELA_L3BFDF_DATA_CONFIRM', 1, 1, 1),
(213, 'MSG_ID_L3BFDF_CLOUDVELA_EVENT_REPORT', 1, 1, 1),
(214, 'MSG_ID_CLOUDVELA_L3BFDF_EVENT_CONFIRM', 1, 1, 1),
(215, 'MSG_ID_CLOUDVELA_L3BFDF_CTRL_REQ', 1, 1, 1),
(216, 'MSG_ID_L3BFDF_CLOUDVELA_CTRL_RESP', 1, 1, 1),
(217, 'MSG_ID_L3BFDF_CLOUDVELA_STATISTIC_REPORT', 1, 1, 1),
(218, 'MSG_ID_CLOUDVELA_L3BFDF_STATISTIC_CONFIRM', 1, 1, 1),
(219, 'MSG_ID_CLOUDVELA_L3BFHS_DATA_REQ', 1, 1, 1),
(220, 'MSG_ID_L3BFHS_CLOUDVELA_DATA_RESP', 1, 1, 1),
(221, 'MSG_ID_L3BFHS_CLOUDVELA_DATA_REPORT', 1, 1, 1),
(222, 'MSG_ID_CLOUDVELA_L3BFHS_DATA_CONFIRM', 1, 1, 1),
(223, 'MSG_ID_L3BFHS_CLOUDVELA_EVENT_REPORT', 1, 1, 1),
(224, 'MSG_ID_CLOUDVELA_L3BFHS_EVENT_CONFIRM', 1, 1, 1),
(225, 'MSG_ID_CLOUDVELA_L3BFHS_CTRL_REQ', 1, 1, 1),
(226, 'MSG_ID_L3BFHS_CLOUDVELA_CTRL_RESP', 1, 1, 1),
(227, 'MSG_ID_L3BFHS_CLOUDVELA_STATISTIC_REPORT', 1, 1, 1),
(228, 'MSG_ID_CLOUDVELA_L3BFHS_STATISTIC_CONFIRM', 1, 1, 1),
(229, 'MSG_ID_L3BFSC_CAN_SYS_CFG_REQ', 1, 1, 1),
(230, 'MSG_ID_CAN_L3BFSC_SYS_CFG_RESP', 1, 1, 1),
(231, 'MSG_ID_L3BFSC_CAN_SYS_START_REQ', 1, 1, 1),
(232, 'MSG_ID_CAN_L3BFSC_SYS_START_RESP', 1, 1, 1),
(233, 'MSG_ID_L3BFSC_CAN_SYS_STOP_REQ', 1, 1, 1),
(234, 'MSG_ID_CAN_L3BFSC_SYS_STOP_RESP', 1, 1, 1),
(235, 'MSG_ID_CAN_L3BFSC_WS_NEW_READY_EVENT', 1, 1, 1),
(236, 'MSG_ID_L3BFSC_CAN_WS_COMB_OUT', 1, 1, 1),
(237, 'MSG_ID_CAN_L3BFSC_WS_COMB_OUT_FB', 1, 1, 1),
(238, 'MSG_ID_L3BFSC_CAN_WS_GIVE_UP', 1, 1, 1),
(239, 'MSG_ID_CAN_L3BFSC_WS_GIVE_UP_FB', 1, 1, 1),
(240, 'MSG_ID_L3BFSC_CAN_ERROR_INQ_CMD_REQ', 1, 1, 1),
(241, 'MSG_ID_CAN_L3BFSC_ERROR_INQ_CMD_RESP', 1, 1, 1),
(242, 'MSG_ID_USBCAN_L2FRAME_RCV', 1, 1, 1),
(243, 'MSG_ID_L3BFDF_CAN_SYS_CFG_REQ', 1, 1, 1),
(244, 'MSG_ID_CAN_L3BFDF_SYS_CFG_RESP', 1, 1, 1),
(245, 'MSG_ID_L3BFDF_CAN_SYS_SUSPEND_REQ', 1, 1, 1),
(246, 'MSG_ID_CAN_L3BFDF_SYS_SUSPEND_RESP', 1, 1, 1),
(247, 'MSG_ID_L3BFDF_CAN_SYS_RESUME_REQ', 1, 1, 1),
(248, 'MSG_ID_CAN_L3BFDF_SYS_RESUME_RESP', 1, 1, 1),
(249, 'MSG_ID_CAN_L3BFDF_WS_NEW_READY_EVENT', 1, 1, 1),
(250, 'MSG_ID_L3BFDF_CAN_SNC_PULLIN_REQ', 1, 1, 1),
(251, 'MSG_ID_CAN_L3BFDF_SNC_PULLIN_RESP', 1, 1, 1),
(252, 'MSG_ID_L3BFDF_CAN_WS_COMB_OUT', 1, 1, 1),
(253, 'MSG_ID_CAN_L3BFDF_WS_COMB_OUT_FB', 1, 1, 1),
(254, 'MSG_ID_CAN_L3BFDF_BASKET_CLEAN_IND', 1, 1, 1),
(255, 'MSG_ID_L3BFHS_CAN_SYS_CFG_REQ', 1, 1, 1),
(256, 'MSG_ID_CAN_L3BFHS_SYS_CFG_RESP', 1, 1, 1),
(257, 'MSG_ID_L3BFHS_CAN_SYS_SUSPEND_REQ', 1, 1, 1),
(258, 'MSG_ID_CAN_L3BFHS_SYS_SUSPEND_RESP', 1, 1, 1),
(259, 'MSG_ID_L3BFHS_CAN_SYS_RESUME_REQ', 1, 1, 1),
(260, 'MSG_ID_CAN_L3BFHS_SYS_RESUME_RESP', 1, 1, 1),
(261, 'MSG_ID_CAN_L3BFHS_WS_NEW_READY_EVENT', 1, 1, 1),
(262, 'MSG_ID_INOTIFY_UICOMM_FILE_CHANGE_IND', 1, 1, 1),
(263, 'MSG_ID_UICOMM_L3BFSC_CMD_REQ', 1, 1, 1),
(264, 'MSG_ID_L3BFSC_UICOMM_CMD_RESP', 1, 1, 1),
(265, 'MSG_ID_UICOMM_L3BFSC_CFG_REQ', 1, 1, 1),
(266, 'MSG_ID_L3BFSC_UICOMM_CFG_RESP', 1, 1, 1),
(267, 'MSG_ID_UICOMM_CAN_TEST_CMD_REQ', 1, 1, 1),
(268, 'MSG_ID_CAN_UICOMM_TEST_CMD_RESP', 1, 1, 1),
(269, 'MSG_ID_UICOMM_L3BFDF_CMD_REQ', 1, 1, 1),
(270, 'MSG_ID_L3BFDF_UICOMM_CMD_RESP', 1, 1, 1),
(271, 'MSG_ID_UICOMM_L3BFHS_CMD_REQ', 1, 1, 1),
(272, 'MSG_ID_L3BFHS_UICOMM_CMD_RESP', 1, 1, 1),
(273, 'MSG_ID_L3AQYC_EXG_CTRL_REQ', 1, 1, 1),
(274, 'MSG_ID_L3AQYC_EXG_CTRL_RESP', 1, 1, 1),
(275, 'MSG_ID_L3AQYC_EXG_DATA_REPORT', 1, 1, 1),
(276, 'MSG_ID_CLOUDVELA_LLCZHB_FRAME_REQ', 1, 1, 1),
(277, 'MSG_ID_LLCZHB_CLOUDVELA_FRAME_RESP', 1, 1, 1),
(278, 'MSG_ID_LLCZHB_L3MOD_CTRL_REQ', 1, 1, 1),
(279, 'MSG_ID_L3MOD_LLCZHB_CTRL_RESP', 1, 1, 1),
(280, 'MSG_ID_L3MOD_LLCZHB_DATA_REPORT', 1, 1, 1),
(281, 'MSG_ID_ZHBL3MOD_EXG_CTRL_REQ', 1, 1, 1),
(282, 'MSG_ID_ZHBL3MOD_EXG_CTRL_RESP', 1, 1, 1),
(283, 'MSG_ID_ZHBL3MOD_EXG_DATA_REPORT', 1, 1, 1),
(284, 'MSG_ID_L3HATE_TC_START', 1, 1, 1),
(285, 'MSG_ID_ETH_L3HATE_FRAME_RCV', 1, 1, 1),
(286, 'MSG_ID_SPS_L3HATE_FRAME_RCV', 1, 1, 1),
(287, 'MSG_ID_CAN_L3HATE_FRAME_RCV', 1, 1, 1),
(288, 'MSG_ID_MQTT_L3HATE_FRAME_RCV', 1, 1, 1),
(289, 'MSG_ID_COM_MAX', 0, 0, 0);

-- --------------------------------------------------------

--
-- 表的结构 `hcuwinddirdatainfo`
--

CREATE TABLE `hcuwinddirdatainfo` (
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
-- 表的结构 `hcuwindspddatainfo`
--

CREATE TABLE `hcuwindspddatainfo` (
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
-- Indexes for table `hcubfhsproductpara`
--
ALTER TABLE `hcubfhsproductpara`
  ADD PRIMARY KEY (`sid`);

--
-- Indexes for table `hcubfsccalibration`
--
ALTER TABLE `hcubfsccalibration`
  ADD PRIMARY KEY (`sid`);

--
-- Indexes for table `hcubfscconfigpara`
--
ALTER TABLE `hcubfscconfigpara`
  ADD PRIMARY KEY (`sid`),
  ADD UNIQUE KEY `confname` (`confname`);

--
-- Indexes for table `hcubfsccurrentinfo`
--
ALTER TABLE `hcubfsccurrentinfo`
  ADD PRIMARY KEY (`timestamp`);

--
-- Indexes for table `hcubfscfb2ui`
--
ALTER TABLE `hcubfscfb2ui`
  ADD PRIMARY KEY (`sid`);

--
-- Indexes for table `hcubfscflowsheet`
--
ALTER TABLE `hcubfscflowsheet`
  ADD PRIMARY KEY (`sid`);

--
-- Indexes for table `hcubfsclanguagedict`
--
ALTER TABLE `hcubfsclanguagedict`
  ADD PRIMARY KEY (`sid`);

--
-- Indexes for table `hcubfsclanguagelist`
--
ALTER TABLE `hcubfsclanguagelist`
  ADD PRIMARY KEY (`lang_name`);

--
-- Indexes for table `hcubfscstadatainfo`
--
ALTER TABLE `hcubfscstadatainfo`
  ADD PRIMARY KEY (`sid`);

--
-- Indexes for table `hcubfscstaticpara`
--
ALTER TABLE `hcubfscstaticpara`
  ADD PRIMARY KEY (`sid`);

--
-- Indexes for table `hcubfznaccount`
--
ALTER TABLE `hcubfznaccount`
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
-- Indexes for table `hcusysalarminfo`
--
ALTER TABLE `hcusysalarminfo`
  ADD PRIMARY KEY (`sid`);

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
-- Indexes for table `hcusysswm_swdl`
--
ALTER TABLE `hcusysswm_swdl`
  ADD PRIMARY KEY (`sid`);

--
-- Indexes for table `hcusysswm_swpkg`
--
ALTER TABLE `hcusysswm_swpkg`
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
-- 在导出的表使用AUTO_INCREMENT
--

--
-- 使用表AUTO_INCREMENT `hcuairprsaltitudebmp180datainfo`
--
ALTER TABLE `hcuairprsaltitudebmp180datainfo`
  MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT;
--
-- 使用表AUTO_INCREMENT `hcuairprsbmp180datainfo`
--
ALTER TABLE `hcuairprsbmp180datainfo`
  MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT;
--
-- 使用表AUTO_INCREMENT `hcuairprsdatainfo`
--
ALTER TABLE `hcuairprsdatainfo`
  MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT;
--
-- 使用表AUTO_INCREMENT `hcualcoholdatainfo`
--
ALTER TABLE `hcualcoholdatainfo`
  MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT;
--
-- 使用表AUTO_INCREMENT `hcualcoholmq3alcodatainfo`
--
ALTER TABLE `hcualcoholmq3alcodatainfo`
  MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT;
--
-- 使用表AUTO_INCREMENT `hcubfhsproductpara`
--
ALTER TABLE `hcubfhsproductpara`
  MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=20;
--
-- 使用表AUTO_INCREMENT `hcubfsccalibration`
--
ALTER TABLE `hcubfsccalibration`
  MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=2;
--
-- 使用表AUTO_INCREMENT `hcubfscconfigpara`
--
ALTER TABLE `hcubfscconfigpara`
  MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=13;
--
-- 使用表AUTO_INCREMENT `hcubfscfb2ui`
--
ALTER TABLE `hcubfscfb2ui`
  MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=4;
--
-- 使用表AUTO_INCREMENT `hcubfscflowsheet`
--
ALTER TABLE `hcubfscflowsheet`
  MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=6;
--
-- 使用表AUTO_INCREMENT `hcubfsclanguagedict`
--
ALTER TABLE `hcubfsclanguagedict`
  MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=76;
--
-- 使用表AUTO_INCREMENT `hcubfscstadatainfo`
--
ALTER TABLE `hcubfscstadatainfo`
  MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=24856;
--
-- 使用表AUTO_INCREMENT `hcubfscstaticpara`
--
ALTER TABLE `hcubfscstaticpara`
  MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=2;
--
-- 使用表AUTO_INCREMENT `hcubfznaccount`
--
ALTER TABLE `hcubfznaccount`
  MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=12;
--
-- 使用表AUTO_INCREMENT `hcuco1datainfo`
--
ALTER TABLE `hcuco1datainfo`
  MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT;
--
-- 使用表AUTO_INCREMENT `hcuemcdatainfo`
--
ALTER TABLE `hcuemcdatainfo`
  MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=12205;
--
-- 使用表AUTO_INCREMENT `hcuhchodatainfo`
--
ALTER TABLE `hcuhchodatainfo`
  MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT;
--
-- 使用表AUTO_INCREMENT `hcuhchoze08ch2odatainfo`
--
ALTER TABLE `hcuhchoze08ch2odatainfo`
  MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT;
--
-- 使用表AUTO_INCREMENT `hcuhsmmpdatainfo`
--
ALTER TABLE `hcuhsmmpdatainfo`
  MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT;
--
-- 使用表AUTO_INCREMENT `hcuhumiddatainfo`
--
ALTER TABLE `hcuhumiddatainfo`
  MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT;
--
-- 使用表AUTO_INCREMENT `hcuhumiddht11datainfo`
--
ALTER TABLE `hcuhumiddht11datainfo`
  MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT;
--
-- 使用表AUTO_INCREMENT `hcuhumidmth01datainfo`
--
ALTER TABLE `hcuhumidmth01datainfo`
  MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=2;
--
-- 使用表AUTO_INCREMENT `hcuhumidrht03datainfo`
--
ALTER TABLE `hcuhumidrht03datainfo`
  MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT;
--
-- 使用表AUTO_INCREMENT `hcuhumidsht20datainfo`
--
ALTER TABLE `hcuhumidsht20datainfo`
  MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT;
--
-- 使用表AUTO_INCREMENT `hcuigmcj188datainfo`
--
ALTER TABLE `hcuigmcj188datainfo`
  MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT;
--
-- 使用表AUTO_INCREMENT `hcuihmcj188datainfo`
--
ALTER TABLE `hcuihmcj188datainfo`
  MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT;
--
-- 使用表AUTO_INCREMENT `hcuipmcj188datainfo`
--
ALTER TABLE `hcuipmcj188datainfo`
  MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT;
--
-- 使用表AUTO_INCREMENT `hcuiwmcj188datainfo`
--
ALTER TABLE `hcuiwmcj188datainfo`
  MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT;
--
-- 使用表AUTO_INCREMENT `hculightstrbh1750datainfo`
--
ALTER TABLE `hculightstrbh1750datainfo`
  MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT;
--
-- 使用表AUTO_INCREMENT `hculightstrdatainfo`
--
ALTER TABLE `hculightstrdatainfo`
  MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT;
--
-- 使用表AUTO_INCREMENT `hcunoisedatainfo`
--
ALTER TABLE `hcunoisedatainfo`
  MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT;
--
-- 使用表AUTO_INCREMENT `hcupm25bmpd300datainfo`
--
ALTER TABLE `hcupm25bmpd300datainfo`
  MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT;
--
-- 使用表AUTO_INCREMENT `hcupm25datainfo`
--
ALTER TABLE `hcupm25datainfo`
  MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT;
--
-- 使用表AUTO_INCREMENT `hcupm25sharpdatainfo`
--
ALTER TABLE `hcupm25sharpdatainfo`
  MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT;
--
-- 使用表AUTO_INCREMENT `hcusysalarminfo`
--
ALTER TABLE `hcusysalarminfo`
  MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=2195;
--
-- 使用表AUTO_INCREMENT `hcusyspmglobaldatainfo`
--
ALTER TABLE `hcusyspmglobaldatainfo`
  MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=1521;
--
-- 使用表AUTO_INCREMENT `hcusysswm_swdl`
--
ALTER TABLE `hcusysswm_swdl`
  MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT;
--
-- 使用表AUTO_INCREMENT `hcusysswm_swpkg`
--
ALTER TABLE `hcusysswm_swpkg`
  MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=40;
--
-- 使用表AUTO_INCREMENT `hcutempbmp180datainfo`
--
ALTER TABLE `hcutempbmp180datainfo`
  MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT;
--
-- 使用表AUTO_INCREMENT `hcutempdatainfo`
--
ALTER TABLE `hcutempdatainfo`
  MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT;
--
-- 使用表AUTO_INCREMENT `hcutempdht11datainfo`
--
ALTER TABLE `hcutempdht11datainfo`
  MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT;
--
-- 使用表AUTO_INCREMENT `hcutempmth01datainfo`
--
ALTER TABLE `hcutempmth01datainfo`
  MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT;
--
-- 使用表AUTO_INCREMENT `hcutemprht03datainfo`
--
ALTER TABLE `hcutemprht03datainfo`
  MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT;
--
-- 使用表AUTO_INCREMENT `hcutempsht20datainfo`
--
ALTER TABLE `hcutempsht20datainfo`
  MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT;
--
-- 使用表AUTO_INCREMENT `hcutoxicgasdatainfo`
--
ALTER TABLE `hcutoxicgasdatainfo`
  MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT;
--
-- 使用表AUTO_INCREMENT `hcutoxicgasmq135datainfo`
--
ALTER TABLE `hcutoxicgasmq135datainfo`
  MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT;
--
-- 使用表AUTO_INCREMENT `hcutoxicgaszp01vocdatainfo`
--
ALTER TABLE `hcutoxicgaszp01vocdatainfo`
  MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT;
--
-- 使用表AUTO_INCREMENT `hcuwinddirdatainfo`
--
ALTER TABLE `hcuwinddirdatainfo`
  MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT;
--
-- 使用表AUTO_INCREMENT `hcuwindspddatainfo`
--
ALTER TABLE `hcuwindspddatainfo`
  MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
