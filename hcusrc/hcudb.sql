-- phpMyAdmin SQL Dump
-- version 4.2.12deb2+deb8u1
-- http://www.phpmyadmin.net
--
-- Host: localhost
-- Generation Time: Jul 29, 2017 at 06:03 PM
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
) ENGINE=InnoDB AUTO_INCREMENT=21 DEFAULT CHARSET=utf8;

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
) ENGINE=InnoDB AUTO_INCREMENT=21 DEFAULT CHARSET=utf8;

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
) ENGINE=InnoDB AUTO_INCREMENT=21 DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Table structure for table `hcubfsccalibration`
--

CREATE TABLE IF NOT EXISTS `hcubfsccalibration` (
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
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=utf8;

--
-- Dumping data for table `hcubfsccalibration`
--

INSERT INTO `hcubfsccalibration` (`sid`, `zeroadc_01`, `fulladc_01`, `fullwgt_01`, `zeroadc_02`, `fulladc_02`, `fullwgt_02`, `zeroadc_03`, `fulladc_03`, `fullwgt_03`, `zeroadc_04`, `fulladc_04`, `fullwgt_04`, `zeroadc_05`, `fulladc_05`, `fullwgt_05`, `zeroadc_06`, `fulladc_06`, `fullwgt_06`, `zeroadc_07`, `fulladc_07`, `fullwgt_07`, `zeroadc_08`, `fulladc_08`, `fullwgt_08`, `zeroadc_09`, `fulladc_09`, `fullwgt_09`, `zeroadc_10`, `fulladc_10`, `fullwgt_10`, `zeroadc_11`, `fulladc_11`, `fullwgt_11`, `zeroadc_12`, `fulladc_12`, `fullwgt_12`, `zeroadc_13`, `fulladc_13`, `fullwgt_13`, `zeroadc_14`, `fulladc_14`, `fullwgt_14`, `zeroadc_15`, `fulladc_15`, `fullwgt_15`, `zeroadc_16`, `fulladc_16`, `fullwgt_16`) VALUES
(1, 434221, 506206, 100000, 474360, 515157, 100000, 455673, 508063, 100000, 447451, 519359, 100000, 432639, 504921, 100000, 442397, 514862, 100000, 443020, 515286, 100000, 468387, 543587, 100000, 438766, 510644, 100000, 450173, 520188, 100000, 0, 0, 100000, 0, 0, 100000, 0, 0, 100000, 0, 0, 100000, 0, 0, 100000, 0, 0, 100000);

-- --------------------------------------------------------

--
-- Table structure for table `hcubfscconfigpara`
--

CREATE TABLE IF NOT EXISTS `hcubfscconfigpara` (
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
) ENGINE=InnoDB AUTO_INCREMENT=14 DEFAULT CHARSET=utf8;

--
-- Dumping data for table `hcubfscconfigpara`
--

INSERT INTO `hcubfscconfigpara` (`sid`, `confname`, `currentconf`, `baseconf`, `confowner`, `conficon`, `confdescription`, `minscalenum`, `maxscalenum`, `minscalenumstart`, `targetweight`, `upperweightlimit`, `proximitmode`, `combinationbias`, `remaindetectind`, `remaindetecttime`, `remainobjtreat`, `prioritymode`, `automode`, `averagenum`, `loaddetecttime`, `loaddetectthread`, `emptydetecttime`, `emptydetectthread`, `standardreadytime`, `motorspeed`, `motordirection`, `rollingstart`, `rollingstop`, `rollinginterval`, `failuredetectvaration`, `failuredetecttime`) VALUES
(4, 'candy 1', 'Y', 'N', 'Bofeng', 'candy.svg', 'candy 200g', 1, 16, 1, 20000, 5000, 0, 1, 0, 2, 0, 0, 0, 1, 400, 100, 400, 500, 0, 150, 0, 5000, 0, 0, 0, 0),
(7, 'apple 1', 'N', 'N', 'Bofeng', 'apple12.svg', 'Apple 1Kg', 2, 16, 1, 100000, 5000, 0, 1, 0, 2, 0, 0, 0, 1, 400, 100, 400, 500, 0, 150, 0, 5000, 0, 0, 0, 0),
(8, '-apple', 'N', 'Y', 'System', 'apple54.svg', 'Apple standard config (1Kg) ', 1, 16, 1, 100000, 5000, 0, 1, 0, 2, 0, 0, 0, 1, 400, 100, 400, 500, 0, 150, 0, 5000, 0, 0, 0, 0),
(9, '-tomato', 'N', 'Y', 'System', 'tomato1.svg', 'Tomato standard config 1Kg', 1, 16, 1, 100000, 5000, 0, 1, 0, 2, 0, 0, 0, 1, 400, 100, 400, 500, 0, 150, 0, 5000, 0, 0, 0, 0),
(10, '-candy', 'N', 'Y', 'System', 'candy.svg', 'Candy standard config 500g', 1, 16, 1, 50000, 5000, 0, 1, 0, 2, 0, 0, 0, 1, 400, 100, 400, 500, 0, 150, 0, 5000, 0, 0, 0, 0),
(11, '-fish', 'N', 'Y', 'System', 'fish12.svg', 'fish 500g', 1, 16, 1, 50000, 5000, 0, 1, 0, 2, 0, 0, 0, 1, 400, 100, 400, 500, 0, 150, 0, 5000, 0, 0, 0, 0),
(12, 'fish 1', 'N', 'N', 'System', 'fish12.svg', 'fish 500g', 1, 16, 1, 50000, 5000, 0, 1, 0, 2, 0, 0, 0, 1, 400, 100, 400, 500, 0, 150, 0, 5000, 0, 0, 0, 0);

-- --------------------------------------------------------

--
-- Table structure for table `hcubfsccurrentinfo`
--

CREATE TABLE IF NOT EXISTS `hcubfsccurrentinfo` (
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

INSERT INTO `hcubfsccurrentinfo` (`timestamp`, `status_00`, `value_00`, `status_01`, `value_01`, `status_02`, `value_02`, `status_03`, `value_03`, `status_04`, `value_04`, `status_05`, `value_05`, `status_06`, `value_06`, `status_07`, `value_07`, `status_08`, `value_08`, `status_09`, `value_09`, `status_10`, `value_10`, `status_11`, `value_11`, `status_12`, `value_12`, `status_13`, `value_13`, `status_14`, `value_14`, `status_15`, `value_15`, `status_16`, `value_16`, `curcomwgt`) VALUES
(1501215098, 0, 35483, 0, 36906, 0, 36906, 0, 33144, 0, 36520, 0, 15508, 0, 28983, 1, 0, 1, 0, 1, 0, 0, 0, 0, 111, 0, 121, 0, 131, 0, 141, 0, 151, 0, 161, 128566);

-- --------------------------------------------------------

--
-- Table structure for table `hcubfscfb2ui`
--

CREATE TABLE IF NOT EXISTS `hcubfscfb2ui` (
`sid` int(4) NOT NULL,
  `cmdtype` int(1) NOT NULL,
  `validflag` int(1) NOT NULL,
  `fbinfo` char(80) NOT NULL
) ENGINE=InnoDB AUTO_INCREMENT=4 DEFAULT CHARSET=utf8;

--
-- Dumping data for table `hcubfscfb2ui`
--

INSERT INTO `hcubfscfb2ui` (`sid`, `cmdtype`, `validflag`, `fbinfo`) VALUES
(1, 1, 1, 'Configuration success'),
(2, 2, 0, 'Start failure due to WS not reachable'),
(3, 3, 0, 'Stop failure due to WS not reachable');

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
  `wsAvgTttMatWgt` double(18,2) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Dumping data for table `hcubfscstadatainfo`
--

INSERT INTO `hcubfscstadatainfo` (`StaType`, `timestamp`, `wsIncMatCnt`, `wsIncMatWgt`, `wsCombTimes`, `wsTttTimes`, `wsTgvTimes`, `wsTttMatCnt`, `wsTgvMatCnt`, `wsTttMatWgt`, `wsTgvMatWgt`, `wsAvgTttTimes`, `wsAvgTttMatCnt`, `wsAvgTttMatWgt`) VALUES
('BFSC_STA_24_HOUR', 1501126545, 0, 0.00, 0, 0, 0, 0, 0, 0.00, 0.00, 0, 0, 0.00),
('BFSC_STA_LOCAL_UI', 1501215084, 0, 0.00, 0, 0, 0, 0, 0, 0.00, 0.00, 0, 0, 0.00),
('BFSC_STA_UP_2_NOW', 1501126550, 0, 0.00, 0, 0, 0, 0, 0, 0.00, 0.00, 0, 0, 0.00);

-- --------------------------------------------------------

--
-- Table structure for table `hcubfscstaticpara`
--

CREATE TABLE IF NOT EXISTS `hcubfscstaticpara` (
`sid` int(4) NOT NULL,
  `calmaxallowedwgt` int(4) NOT NULL DEFAULT '0',
  `calfullwgt` int(4) NOT NULL DEFAULT '0',
  `caladcgain` int(4) NOT NULL DEFAULT '0',
  `caladcwordrate` int(4) NOT NULL DEFAULT '0',
  `snrstaticzerovalue` int(4) NOT NULL DEFAULT '0',
  `snrtailorvalue` int(4) NOT NULL DEFAULT '0',
  `snrdynzerothread` int(4) NOT NULL DEFAULT '0',
  `snrdynzerohysteresis` int(4) NOT NULL DEFAULT '0'
) ENGINE=InnoDB AUTO_INCREMENT=2 DEFAULT CHARSET=utf8 COMMENT='calibration and sensor configuration table';

--
-- Dumping data for table `hcubfscstaticpara`
--

INSERT INTO `hcubfscstaticpara` (`sid`, `calmaxallowedwgt`, `calfullwgt`, `caladcgain`, `caladcwordrate`, `snrstaticzerovalue`, `snrtailorvalue`, `snrdynzerothread`, `snrdynzerohysteresis`) VALUES
(1, 1000000, 100000, 6, 3, 0, 0, 499, 5000);

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
) ENGINE=InnoDB AUTO_INCREMENT=12205 DEFAULT CHARSET=utf8;

--
-- Dumping data for table `hcuemcdatainfo`
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
) ENGINE=InnoDB AUTO_INCREMENT=696 DEFAULT CHARSET=utf8;

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
) ENGINE=InnoDB AUTO_INCREMENT=21 DEFAULT CHARSET=utf8;

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
(195);

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
) ENGINE=InnoDB AUTO_INCREMENT=2195 DEFAULT CHARSET=utf8;

--
-- Dumping data for table `hcusysalarminfo`
--

INSERT INTO `hcusysalarminfo` (`sid`, `eqpid`, `alarmtype`, `alarmcontent`, `alarmseverity`, `alarmclearflag`, `alarmdesc`, `timestamp`) VALUES
(1, 65, 1, 6, 1, 0, '', 1488791310),
(2, 65, 1, 6, 1, 0, '', 1488791316),
(3, 65, 1, 6, 1, 0, '', 1488791322),
(4, 65, 1, 6, 1, 0, '', 1488791328),
(5, 65, 1, 6, 1, 0, '', 1488791334),
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
(1365, 6, 1, 2, 1, 0, '', 1500795431);
INSERT INTO `hcusysalarminfo` (`sid`, `eqpid`, `alarmtype`, `alarmcontent`, `alarmseverity`, `alarmclearflag`, `alarmdesc`, `timestamp`) VALUES
(1366, 7, 1, 6, 1, 0, '', 1500795435),
(1367, 1, 1, 1, 1, 0, '', 1500795439),
(1368, 6, 1, 3, 1, 0, '', 1500795449),
(1369, 7, 1, 6, 1, 0, '', 1500795449),
(1370, 35, 1, 4, 1, 0, '', 1500795449),
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
('HCU_PRJ_AQYCG20_RASBERRY', 1, 'localhost', 'root', '123456', 'hcudb', 3306, 0, 4, 1, 'http://127.0.0.1/test.php', '112.64.16.27', 'http://www.hkrob.com/mfunhcu/l1mainentry/cloud_callback_wechat.php', 'XHZN_HCU', 'ftp://121.40.118.33/', 'forlinx', 'Forlinx321', 'avorion', 'Avorion321', '/var/hcu/hcu_sw_download/', '/var/hcu/hcu_sw_active/', '/var/hcu/hcu_sw_backup/', '121.40.118.33', 'http://192.168.1.232:8000/avorion/', 31, 4, 0, 'firefox', 'http://localhost/yii2basic/web/index.php', '-'),
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
(4, 'TID_1S_EMC_PERIOD_READ', 1, 600),
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
(17, 'TID_1S_NOISE_MODBUS_FB', 1, 10),
(18, 'TID_1S_NOISE_SPSVIRGO_FB', 1, 10),
(19, 'TID_1S_HSMMP_PERIOD_AVORION_READ', 1, 600),
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
(38, 'TID_1S_SYSPM_PERIOD_WORKING', 1, 600),
(39, 'TID_1S_SYSSWM_PERIOD_WORKING', 1, 610),
(40, 'TID_1S_CANITFLEO_WORKING_SCAN', 1, 10),
(41, 'TID_1S_L3BFSC_SYS_CFG_WAIT_FB', 1, 15),
(42, 'TID_1S_L3BFSC_SYS_START_WAIT_FB', 1, 15),
(43, 'TID_1S_L3BFSC_SYS_STOP_WAIT_FB', 1, 15),
(44, 'TID_1S_L3BFSC_TTT_WAIT_FB', 1, 60),
(45, 'TID_1S_L3BFSC_TGU_WAIT_FB', 1, 60),
(46, 'TIMER_ID_1S_L3BFSC_ERROR_INQ', 1, 15),
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
(68, 'TID_10MS_CANITFLEO_SIMULATION_DATA', 2, 40),
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
) ENGINE=InnoDB AUTO_INCREMENT=439 DEFAULT CHARSET=utf8;

--
-- Dumping data for table `hcusyspmglobaldatainfo`
--

INSERT INTO `hcusyspmglobaldatainfo` (`sid`, `taskhcuvmec`, `taskhwinvec`, `tasksyspmec`, `taskmodbusec`, `taskcloudvelaec`, `taskavorionec`, `taskspsvirgoec`, `taskhsmmpec`, `taskemcec`, `taskpm25ec`, `taskwinddirec`, `taskwindspdec`, `tasktempec`, `taskhumidec`, `tasknoiseec`, `taskairprsec`, `taskco1ec`, `tasklightstrec`, `taskalcoholec`, `taskhchoec`, `tasktoxicgasec`, `restartcnt`, `cloudvelaconncnt`, `cloudvelaconnfailcnt`, `cloudveladisccnt`, `clouddatatimeoutcnt`, `socketdisccnt`, `cpuoccupy`, `memoccupy`, `diskoccupy`, `cputemp`, `timestamp`) VALUES
(4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1497083649),
(5, 0, 24, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 6, 29, 35, 0, 1497083964),
(6, 0, 45, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 9, 29, 35, 0, 1497084075),
(7, 0, 69, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 6, 29, 35, 0, 1497084186),
(8, 0, 90, 0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 6, 29, 35, 0, 1497084297),
(9, 0, 114, 0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 13, 28, 35, 0, 1497084408),
(10, 0, 135, 0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 31, 28, 35, 0, 1497084519),
(11, 0, 156, 0, 0, 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 31, 28, 35, 0, 1497084630),
(12, 0, 24, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 29, 35, 0, 1497085367),
(13, 0, 24, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 5, 28, 35, 0, 1497085814),
(14, 0, 45, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 2, 28, 35, 0, 1497085925),
(15, 0, 69, 0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 3, 28, 35, 0, 1497086036),
(16, 0, 24, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 8, 35, 0, 1497086910),
(17, 0, 45, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 5, 8, 35, 0, 1497087021),
(18, 0, 69, 0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 64, 8, 35, 0, 1497087132),
(19, 0, 90, 0, 0, 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 58, 9, 35, 0, 1497087243),
(20, 0, 114, 0, 0, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 61, 9, 35, 0, 1497087354),
(21, 0, 135, 0, 0, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 8, 9, 35, 0, 1497087465),
(22, 0, 156, 0, 0, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 4, 9, 35, 0, 1497087577),
(23, 0, 180, 0, 0, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 95, 10, 35, 0, 1497087687),
(24, 0, 24, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 17, 11, 35, 0, 1497088027),
(25, 0, 24, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, 10, 34, 0, 1497089409),
(26, 0, 45, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, 10, 34, 0, 1497089519),
(27, 0, 69, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 16, 10, 34, 0, 1497089630),
(28, 0, 90, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 11, 10, 34, 0, 1497089741),
(29, 0, 114, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 10, 10, 34, 0, 1497089852),
(30, 0, 135, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 12, 10, 34, 0, 1497089963),
(31, 0, 156, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 11, 10, 34, 0, 1497090074),
(32, 0, 180, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 50, 10, 34, 0, 1497090185),
(33, 0, 201, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 11, 10, 34, 0, 1497090296),
(34, 0, 225, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 12, 10, 34, 0, 1497090407),
(35, 0, 246, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 25, 10, 34, 0, 1497090518),
(36, 0, 267, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 17, 10, 34, 0, 1497090629),
(37, 0, 291, 0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 15, 10, 34, 0, 1497090741),
(38, 0, 312, 0, 0, 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 14, 10, 34, 0, 1497090852),
(39, 0, 336, 0, 0, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 11, 10, 34, 0, 1497090962),
(40, 0, 357, 0, 0, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 12, 10, 34, 0, 1497091073),
(41, 0, 378, 0, 0, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 10, 10, 34, 0, 1497091185),
(42, 0, 402, 0, 0, 16, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 10, 10, 34, 0, 1497091295),
(43, 0, 423, 0, 0, 18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 12, 10, 34, 0, 1497091407),
(44, 0, 447, 0, 0, 18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 11, 10, 34, 0, 1497091517),
(45, 0, 468, 0, 0, 18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 11, 10, 34, 0, 1497091629),
(46, 0, 489, 0, 0, 18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 11, 10, 34, 0, 1497091739),
(47, 0, 513, 0, 0, 18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 12, 10, 34, 0, 1497091850),
(48, 0, 534, 0, 0, 18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, 11, 10, 34, 0, 1497091961),
(49, 0, 558, 0, 0, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, 11, 10, 34, 0, 1497092072),
(50, 0, 579, 0, 0, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, 11, 10, 34, 0, 1497092183),
(51, 0, 600, 0, 0, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, 11, 10, 34, 0, 1497092294),
(52, 0, 624, 0, 0, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, 11, 10, 34, 0, 1497092406),
(53, 0, 645, 0, 0, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, 13, 10, 34, 0, 1497092516),
(54, 0, 669, 0, 0, 20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, 12, 10, 34, 0, 1497092627),
(55, 0, 690, 0, 0, 24, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 11, 11, 10, 34, 0, 1497092738),
(56, 0, 711, 0, 0, 26, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 12, 11, 11, 34, 0, 1497092849),
(57, 0, 735, 0, 0, 28, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 13, 13, 11, 34, 0, 1497092960),
(58, 0, 756, 0, 0, 30, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 14, 11, 11, 34, 0, 1497093072),
(59, 0, 780, 0, 0, 30, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 14, 12, 11, 34, 0, 1497093182),
(60, 0, 801, 0, 0, 30, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 14, 10, 11, 34, 0, 1497093294),
(61, 0, 822, 0, 0, 30, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 15, 9, 11, 34, 0, 1497093405),
(62, 0, 846, 0, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 15, 6, 11, 34, 0, 1497093515),
(63, 0, 867, 0, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 15, 10, 11, 34, 0, 1497093626),
(64, 0, 891, 0, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 15, 7, 11, 34, 0, 1497093738),
(65, 0, 912, 0, 0, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 15, 8, 11, 34, 0, 1497093849),
(66, 0, 933, 0, 0, 34, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, 7, 11, 34, 0, 1497093959),
(67, 0, 957, 0, 0, 34, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, 7, 11, 34, 0, 1497094071),
(68, 0, 978, 0, 0, 34, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, 7, 11, 34, 0, 1497094181),
(69, 0, 999, 0, 0, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 18, 7, 11, 34, 0, 1497094293),
(70, 0, 1023, 0, 0, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 18, 54, 11, 34, 0, 1497094403),
(71, 0, 1044, 0, 0, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 18, 16, 11, 34, 0, 1497094515),
(72, 0, 1068, 0, 0, 38, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 19, 11, 11, 34, 0, 1497094626),
(73, 0, 1089, 0, 0, 40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 19, 5, 11, 34, 0, 1497094737),
(74, 0, 1110, 0, 0, 40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 19, 4, 11, 34, 0, 1497094847),
(75, 0, 1134, 0, 0, 40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 19, 4, 11, 34, 0, 1497094958),
(76, 0, 1155, 0, 0, 40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 19, 16, 11, 34, 0, 1497095070),
(77, 0, 1179, 0, 0, 40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 19, 4, 11, 34, 0, 1497095180),
(78, 0, 1200, 0, 0, 40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 19, 4, 11, 34, 0, 1497095292),
(79, 0, 1221, 0, 0, 40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 19, 12, 12, 34, 0, 1497095403),
(80, 0, 1245, 0, 0, 40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 19, 13, 12, 34, 0, 1497095513),
(81, 0, 1266, 0, 0, 40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 19, 4, 12, 34, 0, 1497095624),
(82, 0, 1290, 0, 0, 40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 19, 4, 12, 34, 0, 1497095736),
(83, 0, 1311, 0, 0, 40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 19, 7, 12, 34, 0, 1497095847),
(84, 0, 1332, 0, 0, 40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 19, 11, 12, 34, 0, 1497095958),
(85, 0, 1356, 0, 0, 40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 19, 5, 12, 34, 0, 1497096069),
(86, 0, 1377, 0, 0, 40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 19, 44, 12, 34, 0, 1497096179),
(87, 0, 24, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 13, 34, 0, 1497096401),
(88, 0, 45, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 13, 34, 0, 1497096513),
(89, 0, 69, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 13, 34, 0, 1497096624),
(90, 0, 90, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 13, 34, 0, 1497096734),
(91, 0, 114, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 13, 34, 0, 1497096845),
(92, 0, 135, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 66, 13, 34, 0, 1497096956),
(93, 0, 156, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 13, 13, 34, 0, 1497097067),
(94, 0, 180, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 29, 13, 34, 0, 1497097179),
(95, 0, 201, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 53, 13, 34, 0, 1497097289),
(96, 0, 225, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 45, 13, 34, 0, 1497097400),
(97, 0, 246, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 16, 13, 34, 0, 1497097511),
(98, 0, 267, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 70, 13, 34, 0, 1497097622),
(99, 0, 291, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 4, 13, 34, 0, 1497097734),
(100, 0, 312, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 6, 13, 34, 0, 1497097845),
(101, 0, 336, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 18, 13, 34, 0, 1497097955),
(102, 0, 357, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 4, 13, 34, 0, 1497098066),
(103, 0, 378, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 4, 13, 34, 0, 1497098178),
(104, 0, 402, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 5, 14, 34, 0, 1497098289),
(105, 0, 423, 0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 14, 34, 0, 1497098399),
(106, 0, 447, 0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 7, 14, 34, 0, 1497098511),
(107, 0, 468, 0, 0, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 3, 14, 34, 0, 1497098621),
(108, 0, 489, 0, 0, 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 4, 14, 34, 0, 1497098732),
(109, 0, 513, 0, 0, 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 6, 14, 34, 0, 1497098844),
(110, 0, 534, 0, 0, 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 14, 34, 0, 1497098954),
(111, 0, 558, 0, 0, 12, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 5, 14, 34, 0, 1497099065),
(112, 0, 24, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 6, 14, 34, 0, 1497099264),
(113, 0, 45, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 5, 14, 34, 0, 1497099376),
(114, 0, 69, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 5, 14, 34, 0, 1497099487),
(115, 0, 90, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 4, 14, 34, 0, 1497099597),
(116, 0, 114, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 6, 14, 34, 0, 1497099708),
(117, 0, 135, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 4, 14, 34, 0, 1497099820),
(118, 0, 156, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 5, 14, 34, 0, 1497099931),
(119, 0, 180, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 7, 14, 34, 0, 1497100042),
(120, 0, 24, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 15, 34, 0, 1497100259),
(121, 0, 45, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 13, 34, 0, 1497100370),
(122, 0, 69, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 78, 13, 34, 0, 1497100481),
(123, 0, 90, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 13, 34, 0, 1497100592),
(124, 0, 24, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 13, 34, 0, 1497100788),
(125, 0, 45, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 55, 13, 34, 0, 1497100899),
(126, 0, 69, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 66, 13, 34, 0, 1497101010),
(127, 0, 90, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 61, 13, 34, 0, 1497101121),
(128, 0, 114, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 27, 13, 34, 0, 1497101232),
(129, 0, 24, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 39, 13, 34, 0, 1497101395),
(130, 0, 45, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 48, 13, 34, 0, 1497101506),
(131, 0, 69, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 36, 13, 34, 0, 1497101617),
(132, 0, 90, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 27, 13, 34, 0, 1497101728),
(133, 0, 114, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 50, 13, 34, 0, 1497101839),
(134, 0, 135, 0, 0, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 80, 13, 34, 0, 1497101950),
(135, 0, 156, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 62, 13, 34, 0, 1497102061),
(136, 0, 180, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 79, 13, 34, 0, 1497102172),
(137, 0, 201, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 68, 13, 34, 0, 1497102283),
(138, 0, 225, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 52, 13, 34, 0, 1497102394),
(139, 0, 246, 0, 0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 48, 13, 34, 0, 1497102505),
(140, 0, 267, 0, 0, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 60, 13, 34, 0, 1497102616),
(141, 0, 291, 0, 0, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 98, 13, 34, 0, 1497102727),
(142, 0, 24, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 69, 15, 34, 0, 1497102959),
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
(428, 0, 3996, 0, 0, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 7, 4, 0, 197, 2, 18, 35, 0, 1501147100);
INSERT INTO `hcusyspmglobaldatainfo` (`sid`, `taskhcuvmec`, `taskhwinvec`, `tasksyspmec`, `taskmodbusec`, `taskcloudvelaec`, `taskavorionec`, `taskspsvirgoec`, `taskhsmmpec`, `taskemcec`, `taskpm25ec`, `taskwinddirec`, `taskwindspdec`, `tasktempec`, `taskhumidec`, `tasknoiseec`, `taskairprsec`, `taskco1ec`, `tasklightstrec`, `taskalcoholec`, `taskhchoec`, `tasktoxicgasec`, `restartcnt`, `cloudvelaconncnt`, `cloudvelaconnfailcnt`, `cloudveladisccnt`, `clouddatatimeoutcnt`, `socketdisccnt`, `cpuoccupy`, `memoccupy`, `diskoccupy`, `cputemp`, `timestamp`) VALUES
(429, 0, 4017, 0, 0, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 7, 4, 0, 197, 3, 18, 35, 0, 1501147211),
(430, 0, 4041, 0, 0, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 7, 4, 0, 199, 1, 18, 35, 0, 1501147322),
(431, 0, 4062, 0, 0, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 7, 4, 0, 199, 1, 18, 35, 0, 1501147433),
(432, 0, 4086, 0, 0, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 7, 4, 0, 201, 1, 18, 35, 0, 1501147544),
(433, 0, 4107, 0, 0, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 7, 4, 0, 201, 1, 18, 35, 0, 1501147656),
(434, 0, 4128, 0, 0, 14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 7, 4, 0, 203, 1, 18, 35, 0, 1501147766),
(435, 0, 312, 4, 12, 28, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 11, 0, 156, 0, 22, 0, 8, 18, 63, 53, 1501320463),
(436, 0, 333, 4, 12, 28, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 12, 0, 167, 0, 24, 0, 26, 15, 63, 54, 1501320574),
(437, 0, 354, 4, 12, 28, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 13, 0, 178, 0, 26, 0, 6, 15, 63, 53, 1501320685),
(438, 0, 24, 0, 19, 10, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 12, 0, 14, 0, 4, 14, 63, 53, 1501320892);

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
) ENGINE=InnoDB AUTO_INCREMENT=702 DEFAULT CHARSET=utf8;

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
) ENGINE=InnoDB AUTO_INCREMENT=713 DEFAULT CHARSET=utf8;

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
) ENGINE=InnoDB AUTO_INCREMENT=21 DEFAULT CHARSET=utf8;

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
) ENGINE=InnoDB AUTO_INCREMENT=21 DEFAULT CHARSET=utf8;

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
(74, 'JSONINOTIFY', 1, 1, 1, 1, 1),
(75, 'L3OPWLOTDR', 1, 1, 1, 1, 1),
(76, 'TASKMAX', 1, 1, 1, 1, 1);

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
(198, 'MSG_ID_L3BFSC_CLOUDVELA_CMD_RESP', 1, 1, 1),
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
-- Indexes for table `hcubfsccalibration`
--
ALTER TABLE `hcubfsccalibration`
 ADD PRIMARY KEY (`sid`);

--
-- Indexes for table `hcubfscconfigpara`
--
ALTER TABLE `hcubfscconfigpara`
 ADD PRIMARY KEY (`sid`), ADD UNIQUE KEY `confname` (`confname`);

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
-- Indexes for table `hcubfscstadatainfo`
--
ALTER TABLE `hcubfscstadatainfo`
 ADD PRIMARY KEY (`StaType`);

--
-- Indexes for table `hcubfscstaticpara`
--
ALTER TABLE `hcubfscstaticpara`
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
 ADD UNIQUE KEY `dbver` (`dbver`);

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
MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT,AUTO_INCREMENT=21;
--
-- AUTO_INCREMENT for table `hcuairprsbmp180datainfo`
--
ALTER TABLE `hcuairprsbmp180datainfo`
MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT,AUTO_INCREMENT=21;
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
MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT,AUTO_INCREMENT=21;
--
-- AUTO_INCREMENT for table `hcubfsccalibration`
--
ALTER TABLE `hcubfsccalibration`
MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT,AUTO_INCREMENT=2;
--
-- AUTO_INCREMENT for table `hcubfscconfigpara`
--
ALTER TABLE `hcubfscconfigpara`
MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT,AUTO_INCREMENT=14;
--
-- AUTO_INCREMENT for table `hcubfscfb2ui`
--
ALTER TABLE `hcubfscfb2ui`
MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT,AUTO_INCREMENT=4;
--
-- AUTO_INCREMENT for table `hcubfscstaticpara`
--
ALTER TABLE `hcubfscstaticpara`
MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT,AUTO_INCREMENT=2;
--
-- AUTO_INCREMENT for table `hcuco1datainfo`
--
ALTER TABLE `hcuco1datainfo`
MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT;
--
-- AUTO_INCREMENT for table `hcuemcdatainfo`
--
ALTER TABLE `hcuemcdatainfo`
MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT,AUTO_INCREMENT=12205;
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
MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT,AUTO_INCREMENT=696;
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
MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT,AUTO_INCREMENT=21;
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
MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT,AUTO_INCREMENT=2195;
--
-- AUTO_INCREMENT for table `hcusyspmglobaldatainfo`
--
ALTER TABLE `hcusyspmglobaldatainfo`
MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT,AUTO_INCREMENT=439;
--
-- AUTO_INCREMENT for table `hcutempbmp180datainfo`
--
ALTER TABLE `hcutempbmp180datainfo`
MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT,AUTO_INCREMENT=702;
--
-- AUTO_INCREMENT for table `hcutempdatainfo`
--
ALTER TABLE `hcutempdatainfo`
MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT;
--
-- AUTO_INCREMENT for table `hcutempdht11datainfo`
--
ALTER TABLE `hcutempdht11datainfo`
MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT,AUTO_INCREMENT=713;
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
MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT,AUTO_INCREMENT=21;
--
-- AUTO_INCREMENT for table `hcutoxicgaszp01vocdatainfo`
--
ALTER TABLE `hcutoxicgaszp01vocdatainfo`
MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT,AUTO_INCREMENT=21;
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
