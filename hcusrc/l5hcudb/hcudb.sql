-- phpMyAdmin SQL Dump
-- version 4.5.4.1deb2ubuntu2
-- http://www.phpmyadmin.net
--
-- Host: localhost
-- Generation Time: Mar 10, 2018 at 02:33 PM
-- Server version: 5.7.20-0ubuntu0.16.04.1
-- PHP Version: 7.0.22-0ubuntu0.16.04.1

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
-- Table structure for table `hcuairprsaltitudebmp180datainfo`
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
-- Table structure for table `hcuairprsbmp180datainfo`
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
-- Table structure for table `hcuairprsdatainfo`
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
-- Table structure for table `hcualcoholdatainfo`
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
-- Table structure for table `hcualcoholmq3alcodatainfo`
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
-- Table structure for table `hcubfdfcallcell`
--

CREATE TABLE `hcubfdfcallcell` (
  `sid` int(4) NOT NULL,
  `configid` int(4) DEFAULT NULL,
  `timestamp` int(4) DEFAULT NULL,
  `operator` char(20) DEFAULT NULL,
  `groupid` int(1) DEFAULT NULL,
  `grouprangehigh` int(4) DEFAULT NULL,
  `grouprangelow` int(4) DEFAULT NULL,
  `targetweight` int(4) DEFAULT NULL,
  `uplimitweight` int(4) DEFAULT NULL,
  `actualweight` int(4) DEFAULT NULL,
  `streamline` int(1) DEFAULT NULL,
  `hooperid` int(1) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Dumping data for table `hcubfdfcallcell`
--

INSERT INTO `hcubfdfcallcell` (`sid`, `configid`, `timestamp`, `operator`, `groupid`, `grouprangehigh`, `grouprangelow`, `targetweight`, `uplimitweight`, `actualweight`, `streamline`, `hooperid`) VALUES
(2, 16, 1520601450, 'apple', 1, 40000, 10000, 990000, 1000000, 1011180, 0, 1),
(3, 16, 1520601506, 'apple', 1, 40000, 10000, 990000, 1000000, 1002627, 0, 2),
(4, 16, 1520599319, 'apple', 1, 40000, 10000, 990000, 1000000, 1006336, 0, 3),
(5, 16, 1520599390, 'apple', 1, 40000, 10000, 990000, 1000000, 997824, 0, 4),
(6, 16, 1520583608, 'apple', 1, 40000, 10000, 100000, 110000, 101615, 0, 5),
(7, 16, 1520583610, 'apple', 1, 40000, 10000, 100000, 110000, 127702, 0, 6),
(8, 16, 1520583613, 'apple', 1, 40000, 10000, 100000, 110000, 106470, 0, 8),
(9, 16, 1520583693, 'apple', 1, 40000, 10000, 100000, 110000, 129753, 0, 7),
(10, 16, 1520583713, 'apple', 1, 40000, 10000, 100000, 110000, 122580, 0, 1),
(11, 16, 1520583715, 'apple', 1, 40000, 10000, 100000, 110000, 108588, 0, 2),
(12, 16, 1520583717, 'apple', 1, 40000, 10000, 100000, 110000, 127039, 0, 3),
(13, 16, 1520583719, 'apple', 1, 40000, 10000, 100000, 110000, 109557, 0, 4),
(14, 16, 1520583720, 'apple', 1, 40000, 10000, 100000, 110000, 113492, 0, 5),
(15, 16, 1520583723, 'apple', 1, 40000, 10000, 100000, 110000, 118964, 0, 7),
(16, 16, 1520583724, 'apple', 1, 40000, 10000, 100000, 110000, 112017, 0, 8),
(17, 16, 1520583726, 'apple', 1, 40000, 10000, 100000, 110000, 105356, 0, 1),
(18, 16, 1520583729, 'apple', 1, 40000, 10000, 100000, 110000, 106592, 0, 3),
(19, 16, 1520583737, 'apple', 1, 40000, 10000, 100000, 110000, 100085, 0, 7),
(20, 16, 1520583744, 'apple', 1, 40000, 10000, 100000, 110000, 107844, 0, 1),
(21, 16, 1520583745, 'apple', 1, 40000, 10000, 100000, 110000, 127244, 0, 2),
(22, 16, 1520583746, 'apple', 1, 40000, 10000, 100000, 110000, 109276, 0, 3),
(23, 16, 1520583747, 'apple', 1, 40000, 10000, 100000, 110000, 113338, 0, 4),
(24, 16, 1520583749, 'apple', 1, 40000, 10000, 100000, 110000, 126083, 0, 5),
(25, 16, 1520583750, 'apple', 1, 40000, 10000, 100000, 110000, 113469, 0, 8),
(26, 16, 1520583803, 'apple', 1, 40000, 10000, 100000, 110000, 106125, 0, 6),
(27, 16, 1520583809, 'apple', 1, 40000, 10000, 100000, 110000, 115432, 0, 2),
(28, 16, 1520583859, 'apple', 1, 40000, 10000, 100000, 110000, 109478, 0, 1),
(29, 16, 1520583860, 'apple', 1, 40000, 10000, 100000, 110000, 113988, 0, 2),
(30, 16, 1520583861, 'apple', 1, 40000, 10000, 100000, 110000, 128303, 0, 3),
(31, 16, 1520583863, 'apple', 1, 40000, 10000, 100000, 110000, 100652, 0, 4),
(32, 16, 1520583864, 'apple', 1, 40000, 10000, 100000, 110000, 123808, 0, 5),
(33, 16, 1520583865, 'apple', 1, 40000, 10000, 100000, 110000, 105832, 0, 6),
(34, 16, 1520583866, 'apple', 1, 40000, 10000, 100000, 110000, 107681, 0, 7),
(35, 16, 1520583867, 'apple', 1, 40000, 10000, 100000, 110000, 102348, 0, 8),
(36, 16, 1520583871, 'apple', 1, 40000, 10000, 100000, 110000, 103287, 0, 1),
(37, 16, 1520583872, 'apple', 1, 40000, 10000, 100000, 110000, 118689, 0, 2),
(38, 16, 1520583874, 'apple', 1, 40000, 10000, 100000, 110000, 103071, 0, 4),
(39, 16, 1520583878, 'apple', 1, 40000, 10000, 100000, 110000, 114170, 0, 8),
(40, 16, 1520581265, 'apple', 1, 40000, 10000, 100000, 110000, 124135, 0, 4),
(41, 16, 1520581267, 'apple', 1, 40000, 10000, 100000, 110000, 131899, 0, 5),
(42, 16, 1520581269, 'apple', 1, 40000, 10000, 100000, 110000, 107443, 0, 5),
(43, 16, 1520581271, 'apple', 1, 40000, 10000, 100000, 110000, 62215, 0, 5),
(44, 16, 1520581273, 'apple', 1, 40000, 10000, 100000, 110000, 0, 0, 8),
(45, 16, 1520581274, 'apple', 1, 40000, 10000, 100000, 110000, 108812, 0, 7),
(46, 16, 1520581288, 'apple', 1, 40000, 10000, 100000, 110000, 116955, 0, 5),
(47, 16, 1520581294, 'apple', 1, 40000, 10000, 100000, 110000, 113354, 0, 6),
(48, 16, 1520581301, 'apple', 1, 40000, 10000, 100000, 110000, 121312, 0, 7),
(49, 16, 1520581307, 'apple', 1, 40000, 10000, 100000, 110000, 106821, 0, 7),
(50, 16, 1520581314, 'apple', 1, 40000, 10000, 100000, 110000, 119513, 0, 8),
(51, 16, 1520581318, 'apple', 1, 40000, 10000, 100000, 110000, 110338, 0, 1),
(52, 16, 1520581325, 'apple', 1, 40000, 10000, 100000, 110000, 127640, 0, 3),
(53, 16, 1520581326, 'apple', 1, 40000, 10000, 100000, 110000, 75556, 0, 3),
(54, 16, 1520581327, 'apple', 1, 40000, 10000, 100000, 110000, 104728, 0, 4),
(55, 16, 1520581328, 'apple', 1, 40000, 10000, 100000, 110000, 117833, 0, 5),
(56, 16, 1520581330, 'apple', 1, 40000, 10000, 100000, 110000, 111541, 0, 6),
(57, 16, 1520581402, 'apple', 1, 40000, 10000, 100000, 110000, 117940, 0, 7),
(58, 16, 1520581428, 'apple', 1, 40000, 10000, 100000, 110000, 111390, 0, 1),
(59, 16, 1520581429, 'apple', 1, 40000, 10000, 100000, 110000, 110266, 0, 2),
(60, 16, 1520581430, 'apple', 1, 40000, 10000, 100000, 110000, 120508, 0, 3),
(61, 16, 1520581431, 'apple', 1, 40000, 10000, 100000, 110000, 110226, 0, 4),
(62, 16, 1520581432, 'apple', 1, 40000, 10000, 100000, 110000, 111506, 0, 5),
(63, 16, 1520581433, 'apple', 1, 40000, 10000, 100000, 110000, 101764, 0, 6),
(64, 16, 1520581434, 'apple', 1, 40000, 10000, 100000, 110000, 120786, 0, 7),
(65, 16, 1520581436, 'apple', 1, 40000, 10000, 100000, 110000, 117043, 0, 8);

-- --------------------------------------------------------

--
-- Table structure for table `hcubfdfgrouppara`
--

CREATE TABLE `hcubfdfgrouppara` (
  `sid` int(4) NOT NULL,
  `groupid` int(1) NOT NULL,
  `lineid` int(1) NOT NULL,
  `configid` int(4) NOT NULL,
  `hoppernum` int(1) NOT NULL,
  `hopperbitmap` int(4) NOT NULL,
  `targetweight` int(4) NOT NULL,
  `targetuplimit` int(4) NOT NULL,
  `bufwgttarget` int(4) NOT NULL,
  `rangelow` int(4) NOT NULL,
  `rangehigh` int(4) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Dumping data for table `hcubfdfgrouppara`
--

INSERT INTO `hcubfdfgrouppara` (`sid`, `groupid`, `lineid`, `configid`, `hoppernum`, `hopperbitmap`, `targetweight`, `targetuplimit`, `bufwgttarget`, `rangelow`, `rangehigh`) VALUES
(1, 1, 0, 2, 4, 15, 25000, 200, 10000, 400, 500),
(2, 2, 0, 2, 2, 192, 10000, 100, 5000, 200, 300),
(3, 3, 0, 2, 2, 48, 10000, 100, 5000, 200, 300),
(4, 1, 1, 2, 4, 15, 25000, 200, 10000, 400, 500),
(5, 2, 1, 2, 3, 112, 10000, 100, 5000, 200, 300),
(6, 3, 1, 2, 1, 128, 10000, 100, 5000, 200, 300),
(16, 1, 0, 16, 8, 255, 1000000, 1100000, 500000, 10000, 80000),
(26, 1, 0, 19, 4, 15, 25000, 200, 10000, 400, 500),
(27, 2, 0, 19, 4, 240, 10000, 100, 5000, 200, 300),
(28, 1, 1, 19, 4, 15, 25000, 200, 10000, 400, 500),
(29, 2, 1, 19, 4, 240, 10000, 100, 5000, 200, 300),
(30, 1, 1, 16, 8, 255, 1000000, 1100000, 500000, 10000, 80000);

-- --------------------------------------------------------

--
-- Table structure for table `hcubfdfproductpara`
--

CREATE TABLE `hcubfdfproductpara` (
  `configid` int(4) NOT NULL,
  `groupnum` int(1) NOT NULL,
  `confname` varchar(50) NOT NULL,
  `confowner` varchar(20) NOT NULL DEFAULT 'System',
  `currentconf` varchar(1) NOT NULL DEFAULT 'N',
  `baseconf` varchar(5) NOT NULL DEFAULT 'N',
  `conficon` varchar(20) NOT NULL,
  `confdescription` varchar(100) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4;

--
-- Dumping data for table `hcubfdfproductpara`
--

INSERT INTO `hcubfdfproductpara` (`configid`, `groupnum`, `confname`, `confowner`, `currentconf`, `baseconf`, `conficon`, `confdescription`) VALUES
(2, 3, 'fish', 'Admin', 'N', 'Y', 'fish37.svg', ''),
(16, 1, 'apple', 'admin', 'Y', 'N', 'apple54.svg', ''),
(19, 2, 'yellowfish', 'admin', 'N', 'N', 'fish37.svg', '');

-- --------------------------------------------------------

--
-- Table structure for table `hcubfdfstadatainfo`
--

CREATE TABLE `hcubfdfstadatainfo` (
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
-- Dumping data for table `hcubfdfstadatainfo`
--

INSERT INTO `hcubfdfstadatainfo` (`sid`, `statype`, `configid`, `timestamp`, `wsincmatcnt`, `wsincmatwgt`, `wscombtimes`, `wsttttimes`, `wstgvtimes`, `wstttmatcnt`, `wstgvmatcnt`, `wstttmatwgt`, `wstgvmatwgt`, `wsavgttttimes`, `wsavgtttmatcnt`, `wsavgtttmatwgt`) VALUES
(1, 'BFDF_STA_24_HOUR', 1, 1501126545, 0, 0.00, 0, 0, 0, 0, 0, 0.00, 0.00, 0, 0, 0.00),
(2, 'BFDF_STA_LOCAL_UI', 0, 1501641455, 0, 0.00, 0, 0, 0, 0, 0, 0.00, 0.00, 0, 0, 0.00),
(3, 'BFDF_STA_LOCAL_UI', 1, 1501641455, 0, 0.00, 0, 0, 0, 0, 0, 0.00, 0.00, 0, 0, 0.00),
(4, 'BFDF_STA_UP_2_NOW', 1, 1501641451, 0, 0.00, 0, 0, 0, 0, 0, 0.00, 0.00, 0, 0, 0.00),
(1127, 'BFDF_STA_60_MIN', 16, 1520592244, 0, 0.00, 0, 0, 0, 0, 0, 0.00, 0.00, 0, 0, 0.00),
(1267, 'BFDF_STA_24_HOUR', 16, 1520601687, 301, 7622032.00, 300, 300, 0, 300, 0, 7585629.00, 0.00, 86400, 86400, -2110306048.00);

-- --------------------------------------------------------

--
-- Table structure for table `hcubfdfsystempara`
--

CREATE TABLE `hcubfdfsystempara` (
  `sid` int(4) NOT NULL,
  `linenum` int(1) NOT NULL,
  `boardnumperline` int(1) NOT NULL,
  `mainmotspeed` int(4) NOT NULL,
  `mainmotdirection` int(4) NOT NULL,
  `mainmotfailvaration` int(4) NOT NULL,
  `mainmotfailtime` int(4) NOT NULL,
  `secmotspeed` int(4) NOT NULL,
  `secmotdirection` int(4) NOT NULL,
  `secmotfailvaration` int(4) NOT NULL,
  `secmotfailtime` int(4) NOT NULL,
  `tweightind` int(2) NOT NULL,
  `t0bis` int(2) NOT NULL,
  `ta0` int(2) NOT NULL,
  `tactcmd` int(2) NOT NULL,
  `tarmstart` int(2) NOT NULL,
  `tarmstop` int(2) NOT NULL,
  `tdoorcloselight` int(2) NOT NULL,
  `tapintervalmin` int(2) NOT NULL,
  `tapinterval` int(2) NOT NULL,
  `tlocalap` int(2) NOT NULL,
  `delaynode1tox` int(2) NOT NULL,
  `delayuphcualgo` int(2) NOT NULL,
  `zerocaliteration` int(2) NOT NULL,
  `fullcaliteration` int(2) NOT NULL,
  `fullweight` int(4) NOT NULL,
  `adcsamplefreq` int(4) NOT NULL,
  `adcgain` int(4) NOT NULL,
  `nffilterfactor` int(4) NOT NULL,
  `maxallowedwgt` int(4) NOT NULL,
  `tailorvalue` int(4) NOT NULL,
  `engswitch` int(1) NOT NULL DEFAULT '0',
  `sysstatus` int(1) NOT NULL DEFAULT '0',
  `hcusw` varchar(100) DEFAULT NULL,
  `ihusw` varchar(100) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Dumping data for table `hcubfdfsystempara`
--

INSERT INTO `hcubfdfsystempara` (`sid`, `linenum`, `boardnumperline`, `mainmotspeed`, `mainmotdirection`, `mainmotfailvaration`, `mainmotfailtime`, `secmotspeed`, `secmotdirection`, `secmotfailvaration`, `secmotfailtime`, `tweightind`, `t0bis`, `ta0`, `tactcmd`, `tarmstart`, `tarmstop`, `tdoorcloselight`, `tapintervalmin`, `tapinterval`, `tlocalap`, `delaynode1tox`, `delayuphcualgo`, `zerocaliteration`, `fullcaliteration`, `fullweight`, `adcsamplefreq`, `adcgain`, `nffilterfactor`, `maxallowedwgt`, `tailorvalue`, `engswitch`, `sysstatus`, `hcusw`, `ihusw`) VALUES
(1, 2, 1, 200, 0, 1, 1, 300, 0, 1, 1, 40, 10, 120, 1, 20, 50, 70, 20, 45, 1, 5, 1, 8, 4, 20000, 3, 6, 20, 1000000, 100000, 1, 0, 'HCU', 'IHU');

-- --------------------------------------------------------

--
-- Table structure for table `hcubfhscallcell`
--

CREATE TABLE `hcubfhscallcell` (
  `sid` int(4) NOT NULL,
  `configid` int(4) DEFAULT NULL,
  `timestamp` int(4) DEFAULT NULL,
  `operator` char(20) DEFAULT NULL,
  `targetweight` int(4) DEFAULT NULL,
  `tu1` int(4) DEFAULT NULL,
  `tu2` int(4) DEFAULT NULL,
  `actualweight` int(4) DEFAULT NULL,
  `state` int(1) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Dumping data for table `hcubfhscallcell`
--

INSERT INTO `hcubfhscallcell` (`sid`, `configid`, `timestamp`, `operator`, `targetweight`, `tu1`, `tu2`, `actualweight`, `state`) VALUES
(1, 1, 124124214, 'TEST', 940000, 5000, 8000, 938000, 1);

-- --------------------------------------------------------

--
-- Table structure for table `hcubfhsproductpara`
--

CREATE TABLE `hcubfhsproductpara` (
  `configid` int(4) NOT NULL,
  `confname` varchar(20) CHARACTER SET utf8 NOT NULL,
  `currentconf` char(1) CHARACTER SET utf8 NOT NULL DEFAULT 'N',
  `baseconf` char(1) CHARACTER SET utf8 NOT NULL DEFAULT 'N',
  `confowner` varchar(20) CHARACTER SET utf8 NOT NULL DEFAULT 'System',
  `conficon` varchar(20) CHARACTER SET utf8 NOT NULL,
  `confdescription` varchar(100) CHARACTER SET utf8 DEFAULT NULL,
  `targetweight` int(4) NOT NULL,
  `targetspeed` int(4) NOT NULL,
  `productlength` int(4) NOT NULL,
  `upperlimit` int(4) NOT NULL,
  `algorithmoption` int(4) NOT NULL,
  `rejectoroption` int(4) NOT NULL,
  `targetthroughput` int(4) NOT NULL DEFAULT '10000',
  `tareweight` int(4) NOT NULL,
  `tu1limit` int(4) NOT NULL,
  `maxtu1` int(4) NOT NULL,
  `tu2limit` int(4) NOT NULL,
  `readstarttime` int(4) NOT NULL,
  `readstoptime` int(4) NOT NULL,
  `armfailtime` int(4) NOT NULL,
  `armfailvaration` int(4) NOT NULL,
  `armrollinginterval` int(4) NOT NULL,
  `armrollingstart` int(4) NOT NULL,
  `armrollingstop` int(4) NOT NULL,
  `armstartaction` int(4) NOT NULL,
  `motordirection` int(4) NOT NULL,
  `motorspeed` int(4) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Dumping data for table `hcubfhsproductpara`
--

INSERT INTO `hcubfhsproductpara` (`configid`, `confname`, `currentconf`, `baseconf`, `confowner`, `conficon`, `confdescription`, `targetweight`, `targetspeed`, `productlength`, `upperlimit`, `algorithmoption`, `rejectoroption`, `targetthroughput`, `tareweight`, `tu1limit`, `maxtu1`, `tu2limit`, `readstarttime`, `readstoptime`, `armfailtime`, `armfailvaration`, `armrollinginterval`, `armrollingstart`, `armrollingstop`, `armstartaction`, `motordirection`, `motorspeed`) VALUES
(16, 'apple', 'N', 'Y', 'System', 'apple54.svg', 'Apple base configuratuion', 1000, 1000, 100, 1000, 1, 3, 0, 0, 995, 250, 990, 300, 380, 100, 1, 100, 1, 1, 2000, 0, 750),
(24, 'fish', 'N', 'N', 'System', 'fish13.svg', 'Fish configuratuion', 1000, 1000, 100, 1000, 1, 3, 0, 0, 995, 250, 990, 300, 380, 2, 2, 500, 2, 2, 2, 1, 2),
(25, 'Banana', 'N', 'N', 'System', 'banana3.svg', 'Banana configuratuion', 1000, 1000, 100, 1000, 1, 3, 0, 0, 995, 250, 990, 300, 380, 100, 1, 100, 1, 1, 2000, 1, 750);

-- --------------------------------------------------------

--
-- Table structure for table `hcubfhsstadatainfo`
--

CREATE TABLE `hcubfhsstadatainfo` (
  `sid` int(4) NOT NULL,
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
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Dumping data for table `hcubfhsstadatainfo`
--

INSERT INTO `hcubfhsstadatainfo` (`sid`, `statype`, `configid`, `timestamp`, `wsincmatwgt`, `wsincmatcnt`, `wsnormalcnt`, `wsnormalwgt`, `wsovercnt`, `wsoverwgt`, `wsundertotalcnt`, `wsundertotalwgt`, `wsundertu1cnt`, `wsundertu1wgt`, `wsundertu2cnt`, `wsundertu2wgt`, `wsunspecificcnt`, `wsunspecificwgt`) VALUES
(1, 'BFHS_STA_UP_2_NOW', 1, 1231241, 3232, 2, 424, 222, 12, 323, 321, 3123213, 22, 31231, 33, 1232321, 12, 3213);

-- --------------------------------------------------------

--
-- Table structure for table `hcubfhssystempara`
--

CREATE TABLE `hcubfhssystempara` (
  `sid` int(4) NOT NULL,
  `weighterlength` int(4) NOT NULL,
  `rejectorposition` int(4) NOT NULL,
  `statisticsswitch` int(1) NOT NULL,
  `counterwgttolerance` int(4) NOT NULL,
  `counterweight` int(4) NOT NULL,
  `autozerosignal` int(4) NOT NULL,
  `maxallowedweight` int(4) NOT NULL,
  `minallowedweight` int(4) NOT NULL,
  `snrtimegrid` int(1) NOT NULL,
  `autozeroswitch` int(1) NOT NULL,
  `snrautotaringtime` int(2) NOT NULL,
  `snrautozerorange` int(4) NOT NULL,
  `snrfiltercutoffreq` int(4) NOT NULL,
  `snrmeasrangeno` int(1) NOT NULL,
  `snrpreloadvalueformat` int(2) NOT NULL,
  `snrpreloadvalue` int(2) NOT NULL,
  `snrringbuftime` int(2) NOT NULL,
  `snrstandstillrange` int(4) NOT NULL,
  `snrstandstilltime` int(2) NOT NULL,
  `snrstandstilltimeout` int(2) NOT NULL,
  `engswitch` int(1) NOT NULL DEFAULT '0',
  `sysstatus` int(1) NOT NULL DEFAULT '0',
  `hcusw` varchar(100) DEFAULT NULL,
  `ihusw` varchar(100) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Dumping data for table `hcubfhssystempara`
--

INSERT INTO `hcubfhssystempara` (`sid`, `weighterlength`, `rejectorposition`, `statisticsswitch`, `counterwgttolerance`, `counterweight`, `autozerosignal`, `maxallowedweight`, `minallowedweight`, `snrtimegrid`, `autozeroswitch`, `snrautotaringtime`, `snrautozerorange`, `snrfiltercutoffreq`, `snrmeasrangeno`, `snrpreloadvalueformat`, `snrpreloadvalue`, `snrringbuftime`, `snrstandstillrange`, `snrstandstilltime`, `snrstandstilltimeout`, `engswitch`, `sysstatus`, `hcusw`, `ihusw`) VALUES
(1, 400, 500, 0, 3000, 20000, 50000, 500, 5, 0, 2, 100, 500, 1000, 1, 2, 625, 100, 500, 100, 10, 1, 0, 'HCU', 'IHU');

-- --------------------------------------------------------

--
-- Table structure for table `hcubfsccalibration`
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
-- Dumping data for table `hcubfsccalibration`
--

INSERT INTO `hcubfsccalibration` (`sid`, `zeroadc_01`, `fulladc_01`, `fullwgt_01`, `zeroadc_02`, `fulladc_02`, `fullwgt_02`, `zeroadc_03`, `fulladc_03`, `fullwgt_03`, `zeroadc_04`, `fulladc_04`, `fullwgt_04`, `zeroadc_05`, `fulladc_05`, `fullwgt_05`, `zeroadc_06`, `fulladc_06`, `fullwgt_06`, `zeroadc_07`, `fulladc_07`, `fullwgt_07`, `zeroadc_08`, `fulladc_08`, `fullwgt_08`, `zeroadc_09`, `fulladc_09`, `fullwgt_09`, `zeroadc_10`, `fulladc_10`, `fullwgt_10`, `zeroadc_11`, `fulladc_11`, `fullwgt_11`, `zeroadc_12`, `fulladc_12`, `fullwgt_12`, `zeroadc_13`, `fulladc_13`, `fullwgt_13`, `zeroadc_14`, `fulladc_14`, `fullwgt_14`, `zeroadc_15`, `fulladc_15`, `fullwgt_15`, `zeroadc_16`, `fulladc_16`, `fullwgt_16`) VALUES
(1, 402121, 475568, 100000, 397526, 469447, 100000, 398223, 471012, 100000, 412981, 487157, 100000, 402460, 475539, 100000, 400936, 473725, 100000, 404690, 477764, 100000, 397114, 469254, 100000, 398982, 471891, 100000, 409975, 482795, 100000, 448185, 519955, 100000, 433829, 506237, 100000, 433829, 506237, 100000, 433829, 506237, 100000, 433829, 506237, 100000, 433829, 506237, 100000);

-- --------------------------------------------------------

--
-- Table structure for table `hcubfscconfigpara`
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
-- Dumping data for table `hcubfscconfigpara`
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
-- Table structure for table `hcubfsccurrentinfo`
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
-- Dumping data for table `hcubfsccurrentinfo`
--

INSERT INTO `hcubfsccurrentinfo` (`timestamp`, `status_00`, `value_00`, `status_01`, `value_01`, `status_02`, `value_02`, `status_03`, `value_03`, `status_04`, `value_04`, `status_05`, `value_05`, `status_06`, `value_06`, `status_07`, `value_07`, `status_08`, `value_08`, `status_09`, `value_09`, `status_10`, `value_10`, `status_11`, `value_11`, `status_12`, `value_12`, `status_13`, `value_13`, `status_14`, `value_14`, `status_15`, `value_15`, `status_16`, `value_16`, `curcomwgt`, `hcusw`, `ihusw`) VALUES
(1520391983, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 111, 0, 121, 0, 131, 0, 141, 0, 151, 0, 161, 0, 'NOW: Inventory Req HCU-SW-R3.V265.DB201.STABLE!', 'NOW: Inventory Req IHU-SW-R0.V0.DB0.STABLE!');

-- --------------------------------------------------------

--
-- Table structure for table `hcubfscfb2ui`
--

CREATE TABLE `hcubfscfb2ui` (
  `sid` int(4) NOT NULL,
  `cmdtype` int(1) NOT NULL,
  `validflag` int(1) NOT NULL,
  `fbinfo` char(80) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Dumping data for table `hcubfscfb2ui`
--

INSERT INTO `hcubfscfb2ui` (`sid`, `cmdtype`, `validflag`, `fbinfo`) VALUES
(1, 1, 1, 'Configuration success'),
(2, 2, 0, 'Start failure due to WS not reachable'),
(3, 3, 0, 'Stop failure due to WS not reachable'),
(4, 1, 0, '');

-- --------------------------------------------------------

--
-- Table structure for table `hcubfscflowsheet`
--

CREATE TABLE `hcubfscflowsheet` (
  `sid` int(4) NOT NULL,
  `configid` int(2) NOT NULL,
  `targetwgt` int(4) NOT NULL,
  `realwgt` int(4) NOT NULL,
  `pkgnum` int(4) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Dumping data for table `hcubfscflowsheet`
--

INSERT INTO `hcubfscflowsheet` (`sid`, `configid`, `targetwgt`, `realwgt`, `pkgnum`) VALUES
(1, 4, 20000, 23215, 1),
(2, 4, 20000, 23302, 1),
(3, 4, 20000, 22658, 1),
(4, 4, 20000, 22596, 1),
(5, 4, 20000, 24155, 4);

-- --------------------------------------------------------

--
-- Table structure for table `hcubfsclanguagedict`
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
-- Dumping data for table `hcubfsclanguagedict`
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
(75, 'New Version available, please reboot whole system', '有新版本，请重启系统', NULL, NULL, NULL, NULL, NULL),
(76, 'Weighter Length', NULL, NULL, NULL, NULL, NULL, NULL),
(77, 'Scale table length (1 mm)', NULL, NULL, NULL, NULL, NULL, NULL),
(78, 'Rejector Position', NULL, NULL, NULL, NULL, NULL, NULL),
(79, 'Cull position (1mm)', NULL, NULL, NULL, NULL, NULL, NULL),
(80, 'MaxAllowed Weight', NULL, NULL, NULL, NULL, NULL, NULL),
(81, 'Min Allowed Weight', NULL, NULL, NULL, NULL, NULL, NULL),
(82, 'Minimum allowable weight weight(0.01 grams)', NULL, NULL, NULL, NULL, NULL, NULL),
(83, 'Counter Weight', NULL, NULL, NULL, NULL, NULL, NULL),
(84, 'Calibrate weight weight(0.01grams)', NULL, NULL, NULL, NULL, NULL, NULL),
(85, 'Auto-zero Signal', NULL, NULL, NULL, NULL, NULL, NULL),
(86, 'Automatic zeroing signal(1pcs)', NULL, NULL, NULL, NULL, NULL, NULL),
(87, 'Auto-zero Switch', NULL, NULL, NULL, NULL, NULL, NULL),
(88, 'Automatic zero switch', NULL, NULL, NULL, NULL, NULL, NULL),
(89, 'Auto-zero Limit', NULL, NULL, NULL, NULL, NULL, NULL),
(90, 'Zero tracking threshold', NULL, NULL, NULL, NULL, NULL, NULL),
(91, 'Target Weight', '目标重量', NULL, NULL, NULL, NULL, NULL),
(92, 'Tare Weight', NULL, NULL, NULL, NULL, NULL, NULL),
(93, 'tare', NULL, NULL, NULL, NULL, NULL, NULL),
(94, 'Target Speed', NULL, NULL, NULL, NULL, NULL, NULL),
(95, 'Target speed(pcs/min)', NULL, NULL, NULL, NULL, NULL, NULL),
(96, 'Product Length', NULL, NULL, NULL, NULL, NULL, NULL),
(97, 'Length of the longest side of the product', NULL, NULL, NULL, NULL, NULL, NULL),
(98, 'Arm Rolling Interval', NULL, NULL, NULL, NULL, NULL, NULL),
(99, 'The rocker arm retracted the waiting time', NULL, NULL, NULL, NULL, NULL, NULL),
(100, 'Algorithm Option', NULL, NULL, NULL, NULL, NULL, NULL),
(101, 'Algorithm selection', '算法选择', NULL, NULL, NULL, NULL, NULL),
(102, 'Upper Limit', '重量上限', NULL, NULL, NULL, NULL, NULL),
(103, 'Overweight threshold', '超重门限', NULL, NULL, NULL, NULL, NULL),
(104, 'TU1 Limit', 'TU1门限', NULL, NULL, NULL, NULL, NULL),
(105, 'TU1 threshold value', 'TU1门限值', NULL, NULL, NULL, NULL, NULL),
(106, 'TU2 Limit', 'TU2门限', NULL, NULL, NULL, NULL, NULL),
(107, 'TU2 threshold value', 'TU2门限值', NULL, NULL, NULL, NULL, NULL),
(108, 'maxTU1', NULL, NULL, NULL, NULL, NULL, NULL),
(109, 'MaximumTU1', NULL, NULL, NULL, NULL, NULL, NULL),
(110, 'Rejector Option', NULL, NULL, NULL, NULL, NULL, NULL),
(111, 'Selector selection', NULL, NULL, NULL, NULL, NULL, NULL),
(112, 'Statistics Switch', NULL, NULL, NULL, NULL, NULL, NULL),
(113, 'Statistical functional switch', NULL, NULL, NULL, NULL, NULL, NULL),
(114, 'Target Throughput', NULL, NULL, NULL, NULL, NULL, NULL),
(115, 'Given through overdose(pcs)', NULL, NULL, NULL, NULL, NULL, NULL),
(116, 'RejectorParameters', '分选参数', NULL, NULL, NULL, NULL, NULL),
(117, 'Total Weight', '总重量', NULL, NULL, NULL, NULL, NULL),
(118, 'Total Package', '总包数', NULL, NULL, NULL, NULL, NULL),
(119, 'Throughput', '分选速度', NULL, NULL, NULL, NULL, NULL),
(120, 'Good Package', '合格包数', NULL, NULL, NULL, NULL, NULL),
(121, 'Over Weight', '超重包数', NULL, NULL, NULL, NULL, NULL),
(122, 'Under Weight', '欠重包数', NULL, NULL, NULL, NULL, NULL),
(123, 'Status Info', '状态信息', NULL, NULL, NULL, NULL, NULL),
(124, 'Current Weight', '当前重量', NULL, NULL, NULL, NULL, NULL),
(125, 'g', '克', NULL, NULL, NULL, NULL, NULL),
(126, 'pcs', '包', NULL, NULL, NULL, NULL, NULL),
(127, 'pcs/min', '包/分钟', NULL, NULL, NULL, NULL, NULL),
(128, 'Target UpLimit', NULL, NULL, NULL, NULL, NULL, NULL),
(129, 'Bufwgt Target', NULL, NULL, NULL, NULL, NULL, NULL),
(130, 'Range Low', NULL, NULL, NULL, NULL, NULL, NULL),
(131, 'Range High', NULL, NULL, NULL, NULL, NULL, NULL),
(132, 'Sorting parameters', NULL, NULL, NULL, NULL, NULL, NULL),
(133, 'Main  Motor Speed', NULL, NULL, NULL, NULL, NULL, NULL),
(134, 'Main Motor Speed', NULL, NULL, NULL, NULL, NULL, NULL),
(135, 'Main Motor Direction', NULL, NULL, NULL, NULL, NULL, NULL),
(136, 'Main Motor Failure Detection Vation', NULL, NULL, NULL, NULL, NULL, NULL),
(137, 'Main Motor Failure Detection Time Ms', NULL, NULL, NULL, NULL, NULL, NULL),
(138, 'Main MotorSpeed', NULL, NULL, NULL, NULL, NULL, NULL),
(139, 'Second Motor Speed', NULL, NULL, NULL, NULL, NULL, NULL),
(140, 'Seconde Motor Speed', NULL, NULL, NULL, NULL, NULL, NULL),
(141, 'Second Motor Failure Detection Varation', NULL, NULL, NULL, NULL, NULL, NULL),
(142, 'Second Motor Failure Detection Time Ms', NULL, NULL, NULL, NULL, NULL, NULL),
(143, 'Second MotorSpeed', NULL, NULL, NULL, NULL, NULL, NULL),
(144, 'TWeightInd', NULL, NULL, NULL, NULL, NULL, NULL),
(145, 'T0bis', NULL, NULL, NULL, NULL, NULL, NULL),
(146, 'TA0', NULL, NULL, NULL, NULL, NULL, NULL),
(147, 'TActCmd', NULL, NULL, NULL, NULL, NULL, NULL),
(148, 'TArmStart', NULL, NULL, NULL, NULL, NULL, NULL),
(149, 'TArmStop', NULL, NULL, NULL, NULL, NULL, NULL),
(150, 'TDoorCloseLightOn', NULL, NULL, NULL, NULL, NULL, NULL),
(151, 'TApIntervalMin', NULL, NULL, NULL, NULL, NULL, NULL),
(152, 'TApInterval', NULL, NULL, NULL, NULL, NULL, NULL),
(153, 'TLocalAp', NULL, NULL, NULL, NULL, NULL, NULL),
(154, 'DelayNodelTox', NULL, NULL, NULL, NULL, NULL, NULL),
(155, 'DelayUpHcuALgoDl', NULL, NULL, NULL, NULL, NULL, NULL),
(156, 'Rocket parameters', NULL, NULL, NULL, NULL, NULL, NULL),
(157, 'Calibration Zero ', NULL, NULL, NULL, NULL, NULL, NULL),
(158, 'Calibration Full', NULL, NULL, NULL, NULL, NULL, NULL),
(159, ' Full Weight', NULL, NULL, NULL, NULL, NULL, NULL),
(160, 'Full Weight', NULL, NULL, NULL, NULL, NULL, NULL),
(161, 'Adc Sample Freq', NULL, NULL, NULL, NULL, NULL, NULL),
(162, 'Abc Sample Freq', NULL, NULL, NULL, NULL, NULL, NULL),
(163, 'Adc Gain', NULL, NULL, NULL, NULL, NULL, NULL),
(164, 'Noise Floor Filter Factor', NULL, NULL, NULL, NULL, NULL, NULL),
(165, 'Max Allowed Weight', NULL, NULL, NULL, NULL, NULL, NULL),
(166, 'Weight Sensor Tailor Value', NULL, NULL, NULL, NULL, NULL, NULL),
(167, 'Calibration parameters', NULL, NULL, NULL, NULL, NULL, NULL),
(168, 'StatisticsSwitch', NULL, NULL, NULL, NULL, NULL, NULL),
(169, 'Counterwgttolerance', NULL, NULL, NULL, NULL, NULL, NULL),
(170, 'Engineering Mode Switch', NULL, NULL, NULL, NULL, NULL, NULL),
(171, 'GeneralPara', NULL, NULL, NULL, NULL, NULL, NULL),
(172, 'SnrTimeGrid', NULL, NULL, NULL, NULL, NULL, NULL),
(173, 'SnrAutoZeroSwitch', NULL, NULL, NULL, NULL, NULL, NULL),
(174, 'AutoZero', NULL, NULL, NULL, NULL, NULL, NULL),
(175, 'SnrAutotaringTime', NULL, NULL, NULL, NULL, NULL, NULL),
(176, 'AutoZeroAutotaringTimeMs', NULL, NULL, NULL, NULL, NULL, NULL),
(177, 'SnrAutoZeroRange', NULL, NULL, NULL, NULL, NULL, NULL),
(178, 'AutoZeroCaptureRangeGrams', NULL, NULL, NULL, NULL, NULL, NULL),
(179, 'SnrFilterCutoFFreq', NULL, NULL, NULL, NULL, NULL, NULL),
(180, 'FilterCutOffFreqHz', NULL, NULL, NULL, NULL, NULL, NULL),
(181, 'SnrMeasurementRange', NULL, NULL, NULL, NULL, NULL, NULL),
(182, 'MeasurementRangeNo', NULL, NULL, NULL, NULL, NULL, NULL),
(183, 'SnrPreloadValueFormat', NULL, NULL, NULL, NULL, NULL, NULL),
(184, 'PreloadComPensationPlacesAfterDecimalPoint', NULL, NULL, NULL, NULL, NULL, NULL),
(185, 'SnrPreloadValue', NULL, NULL, NULL, NULL, NULL, NULL),
(186, 'PreloadComPensationValuePercent', NULL, NULL, NULL, NULL, NULL, NULL),
(187, 'SnrRingbufTime', NULL, NULL, NULL, NULL, NULL, NULL),
(188, 'RingBufTimeMs', NULL, NULL, NULL, NULL, NULL, NULL),
(189, 'SnrStandstillRange', NULL, NULL, NULL, NULL, NULL, NULL),
(190, 'StandstillRangeGrams', NULL, NULL, NULL, NULL, NULL, NULL),
(191, 'SnrStandstillTime', NULL, NULL, NULL, NULL, NULL, NULL),
(192, 'StandstillTime', NULL, NULL, NULL, NULL, NULL, NULL),
(193, 'SnrstandstillTimeOut', NULL, NULL, NULL, NULL, NULL, NULL),
(194, 'StandstillTimeoutMs', NULL, NULL, NULL, NULL, NULL, NULL),
(195, 'ArmFailureDetectionTimeMs', NULL, NULL, NULL, NULL, NULL, NULL),
(196, 'ArmFailureDetectionVaration', NULL, NULL, NULL, NULL, NULL, NULL),
(197, 'ArmRollingTntervalMs', NULL, NULL, NULL, NULL, NULL, NULL),
(198, 'ArmRollingStartMs', NULL, NULL, NULL, NULL, NULL, NULL),
(199, 'ArmRollingStopMs', NULL, NULL, NULL, NULL, NULL, NULL),
(200, 'ArmStartActionMs', NULL, NULL, NULL, NULL, NULL, NULL),
(201, 'RockerPara', NULL, NULL, NULL, NULL, NULL, NULL),
(202, 'MotorPara', NULL, NULL, NULL, NULL, NULL, NULL),
(203, 'AlgorithmOption', NULL, NULL, NULL, NULL, NULL, NULL),
(204, 'RejectorOption', NULL, NULL, NULL, NULL, NULL, NULL),
(205, 'TargetThroughput', NULL, NULL, NULL, NULL, NULL, NULL),
(206, 'TargetThroughput(pcs/min)', NULL, NULL, NULL, NULL, NULL, NULL),
(207, 'TareWeight', NULL, NULL, NULL, NULL, NULL, NULL),
(208, 'UpperLimit', NULL, NULL, NULL, NULL, NULL, NULL),
(209, 'Tu1Limit', NULL, NULL, NULL, NULL, NULL, NULL),
(210, 'Tu2Limit', NULL, NULL, NULL, NULL, NULL, NULL),
(211, 'TargetSpeed', NULL, NULL, NULL, NULL, NULL, NULL),
(212, 'TargetSpeed(pcs/min)', NULL, NULL, NULL, NULL, NULL, NULL),
(213, 'ProductLength', NULL, NULL, NULL, NULL, NULL, NULL),
(214, 'ProductLength(mm)', NULL, NULL, NULL, NULL, NULL, NULL),
(215, 'ReadStartTime', NULL, NULL, NULL, NULL, NULL, NULL),
(216, 'ReadStopTime', NULL, NULL, NULL, NULL, NULL, NULL),
(217, 'Sorting', NULL, NULL, NULL, NULL, NULL, NULL),
(218, 'CurrentWeight-1', NULL, NULL, NULL, NULL, NULL, NULL),
(219, 'CurrentWeight-2', NULL, NULL, NULL, NULL, NULL, NULL),
(220, 'PackageTarget-1', NULL, NULL, NULL, NULL, NULL, NULL),
(221, 'UpperLimit-1', NULL, NULL, NULL, NULL, NULL, NULL),
(222, 'PackageTarget-2', NULL, NULL, NULL, NULL, NULL, NULL),
(223, 'UpperLimit-2', NULL, NULL, NULL, NULL, NULL, NULL),
(224, 'Throughput-1', NULL, NULL, NULL, NULL, NULL, NULL),
(225, 'TotalReject-1', NULL, NULL, NULL, NULL, NULL, NULL),
(226, 'Throughput-2', NULL, NULL, NULL, NULL, NULL, NULL),
(227, 'TotalReject-2', NULL, NULL, NULL, NULL, NULL, NULL),
(228, 'TotalWeight-1', NULL, NULL, NULL, NULL, NULL, NULL),
(229, 'TotalPackage-1', NULL, NULL, NULL, NULL, NULL, NULL),
(230, 'TotalWeight-2', NULL, NULL, NULL, NULL, NULL, NULL),
(231, 'TotalPackage-2', NULL, NULL, NULL, NULL, NULL, NULL),
(232, 'Engswitch', NULL, NULL, NULL, NULL, NULL, NULL);

-- --------------------------------------------------------

--
-- Table structure for table `hcubfsclanguagelist`
--

CREATE TABLE `hcubfsclanguagelist` (
  `lang_name` varchar(20) NOT NULL,
  `lang_icon` varchar(20) NOT NULL,
  `lang_abbr` varchar(20) NOT NULL,
  `defaultflag` tinyint(1) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Dumping data for table `hcubfsclanguagelist`
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
-- Table structure for table `hcubfscstadatainfo`
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
-- Dumping data for table `hcubfscstadatainfo`
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
-- Table structure for table `hcubfscstaticpara`
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
-- Dumping data for table `hcubfscstaticpara`
--

INSERT INTO `hcubfscstaticpara` (`sid`, `calmaxallowedwgt`, `calfullwgt`, `caladcgain`, `caladcwordrate`, `snrstaticzerovalue`, `snrtailorvalue`, `snrdynzerothread`, `snrdynzerohysteresis`) VALUES
(1, 1000000, 100000, 6, 3, 0, 0, 499, 5000);

-- --------------------------------------------------------

--
-- Table structure for table `hcubfznaccount`
--

CREATE TABLE `hcubfznaccount` (
  `uid` int(4) NOT NULL,
  `username` varchar(20) CHARACTER SET utf8 NOT NULL,
  `password` varchar(50) CHARACTER SET utf8 NOT NULL,
  `memo` text CHARACTER SET utf8
) ENGINE=InnoDB DEFAULT CHARSET=latin1;

--
-- Dumping data for table `hcubfznaccount`
--

INSERT INTO `hcubfznaccount` (`uid`, `username`, `password`, `memo`) VALUES
(1, 'admin', '0DPiKuNIrrVmD8IUCuw1hQxNqZc', '管理员'),
(4, 'bfzn', '123456', NULL),
(10, 'wxl', '123456', NULL),
(11, 'www', 'fEqNCco3Yq9h5ZUglD3CZJT4lBs', NULL);

-- --------------------------------------------------------

--
-- Table structure for table `hcuco1datainfo`
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
-- Table structure for table `hcuemcdatainfo`
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

-- --------------------------------------------------------

--
-- Table structure for table `hcuhchodatainfo`
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
-- Table structure for table `hcuhchoze08ch2odatainfo`
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
-- Table structure for table `hcuhsmmpdatainfo`
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
-- Table structure for table `hcuhumiddatainfo`
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
-- Table structure for table `hcuhumiddht11datainfo`
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
-- Table structure for table `hcuhumidmth01datainfo`
--

CREATE TABLE `hcuhumidmth01datainfo` (
  `sid` int(4) NOT NULL,
  `deviceid` int(4) NOT NULL,
  `timestamp` int(4) NOT NULL,
  `dataformat` int(1) NOT NULL,
  `humidvalue` int(4) NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

--
-- Dumping data for table `hcuhumidmth01datainfo`
--

INSERT INTO `hcuhumidmth01datainfo` (`sid`, `deviceid`, `timestamp`, `dataformat`, `humidvalue`) VALUES
(1, 3, 2, 1, 4);

-- --------------------------------------------------------

--
-- Table structure for table `hcuhumidrht03datainfo`
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
-- Table structure for table `hcuhumidsht20datainfo`
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
-- Table structure for table `hcuigmcj188datainfo`
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
-- Table structure for table `hcuihmcj188datainfo`
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
-- Table structure for table `hcuipmcj188datainfo`
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
-- Table structure for table `hcuiwmcj188datainfo`
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
-- Table structure for table `hculightstrbh1750datainfo`
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
-- Table structure for table `hculightstrdatainfo`
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
-- Table structure for table `hcunoisedatainfo`
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
-- Table structure for table `hcupm25bmpd300datainfo`
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
-- Table structure for table `hcupm25datainfo`
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
-- Table structure for table `hcupm25sharpdatainfo`
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
-- Table structure for table `hcusysalarminfo`
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

-- --------------------------------------------------------

--
-- Table structure for table `hcusysengpar`
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
-- Dumping data for table `hcusysengpar`
--

INSERT INTO `hcusysengpar` (`prjname`, `commbackhawlcon`, `hcudbhost`, `hcudbuser`, `hcudbpsw`, `hcudbname`, `hcudbport`, `seriesportformodbus`, `seriesportforgps`, `seriesportforpm25sharp`, `seriesportforqrprinter`, `svraddhttplocal`, `svraddsocketipdefault`, `svraddhttpdefault`, `svrnamedefault`, `cloudftppwdvideo`, `cloudftpuservideo`, `hcuvideoserverdir`, `hcuvideoserverhttp`, `debugmode`, `tracemode`, `browselautostartupflag`, `browselprog`, `browselstartupaddress`, `browselworkingoption`, `windircalibration`, `swdlduration`) VALUES
('HCU_PRJ_AQYCG10_335D', 1, 'localhost', 'root', '123456', 'hcudb', 3306, 0, 4, 2, 1, 'http://127.0.0.1/test.php', 'http://aaa.bbb/', 'http://www.hkrob.com/mfunhcu/l1mainentry/cloud_callback_hcu.php', 'AQ_HCU', 'avorion', 'Avorion321', '/usr/local/apache_arm/htdocs/avorion/', 'http://192.168.1.232:8000/avorion/', 31, 4, 0, 'firefox', 'http://localhost/yii2basic/web/index.php', '-', 0, 4),
('HCU_PRJ_AQYCG20_RASBERRY', 1, 'localhost', 'root', '123456', 'hcudb', 3306, 0, 4, 1, 1, 'http://127.0.0.1/test.php', '112.64.16.27', 'http://www.hkrob.com/mfunhcu/l1mainentry/cloud_callback_wechat.php', 'XHZN_HCU', 'avorion', 'Avorion321', '/usr/local/apache_arm/htdocs/avorion/', 'http://192.168.1.232:8000/avorion/', 31, 4, 0, 'firefox', 'http://localhost/yii2basic/web/index.php', '-kiosk', 0, 4),
('HCU_PRJ_AQYC_OBSOLETE', 1, 'localhost', 'root', '123456', 'hcudb', 3306, 0, 4, 2, 1, 'http://127.0.0.1/test.php', 'http://aaa.bbb/', 'http://www.hkrob.com/mfunhcu/l1mainentry/cloud_callback_hcu.php', 'AQ_HCU', 'avorion', 'Avorion321', '/usr/local/apache_arm/htdocs/avorion/', 'http://192.168.1.232:8000/avorion/', 31, 4, 0, 'firefox', 'http://localhost/yii2basic/web/index.php', '-', 0, 4),
('HCU_PRJ_BFDF_CBU', 1, 'localhost', 'root', '123456', 'hcudb', 3306, 0, 4, 2, 1, 'http://127.0.0.1/test.php', '121.40.118.33', 'http://www.hkrob.com/mfunhcu/l1mainentry/cloud_callback_hcu.php', 'XHZN_HCU', 'avorion', 'Avorion321', '/usr/local/apache_arm/htdocs/avorion/', 'http://192.168.1.232:8000/avorion/', 28, 4, 0, 'chromium-browser', '--app=http://localhost/bfscui/index.html', '--no-sandbox --kiosk --password-store --start-fullscreen', 0, 4),
('HCU_PRJ_BFHS_CBU', 1, 'localhost', 'root', '123456', 'hcudb', 3306, 0, 4, 2, 1, 'http://127.0.0.1/test.php', '121.40.118.33', 'http://www.hkrob.com/mfunhcu/l1mainentry/cloud_callback_hcu.php', 'XHZN_HCU', 'avorion', 'Avorion321', '/usr/local/apache_arm/htdocs/avorion/', 'http://192.168.1.232:8000/avorion/', 28, 4, 0, 'chromium-browser', '--app=http://localhost/bfscui/index.html', '--no-sandbox --kiosk --password-store --start-fullscreen', 0, 4),
('HCU_PRJ_BFSC_CBU', 1, 'localhost', 'root', '123456', 'hcudb', 3306, 0, 4, 2, 1, 'http://127.0.0.1/test.php', '121.40.118.33', 'http://www.hkrob.com/mfunhcu/l1mainentry/cloud_callback_hcu.php', 'XHZN_HCU', 'avorion', 'Avorion321', '/usr/local/apache_arm/htdocs/avorion/', 'http://192.168.1.232:8000/avorion/', 31, 4, 0, 'chromium-browser', '--app=http://localhost/bfscui/index.html', '--no-sandbox --kiosk --password-store --start-fullscreen', 0, 4),
('HCU_PRJ_CXGLACM', 1, 'localhost', 'root', '123456', 'hcudb', 3306, 0, 4, 2, 1, 'http://127.0.0.1/test.php', 'http://aaa.bbb/', 'http://www.hkrob.com/mfunhcu/l1mainentry/cloud_callback_hcu.php', 'AQ_HCU', 'avorion', 'Avorion321', '/usr/local/apache_arm/htdocs/avorion/', 'http://192.168.1.232:8000/avorion/', 31, 4, 0, 'firefox', 'http://localhost/yii2basic/web/index.php', '-', 0, 4),
('HCU_PRJ_CXILC', 1, 'localhost', 'root', '123456', 'hcudb', 3306, 0, 4, 2, 1, 'http://127.0.0.1/test.php', 'http://aaa.bbb/', 'http://www.hkrob.com/mfunhcu/l1mainentry/cloud_callback_hcu.php', 'AQ_HCU', 'avorion', 'Avorion321', '/usr/local/apache_arm/htdocs/avorion/', 'http://192.168.1.232:8000/avorion/', 31, 4, 0, 'firefox', 'http://localhost/yii2basic/web/index.php', '-', 0, 4),
('HCU_PRJ_DAYCG21_RASBERRY', 1, 'localhost', 'root', '123456', 'hcudb', 3306, 0, 4, 1, 1, 'http://127.0.0.1/test.php', '112.64.16.27', 'http://www.hkrob.com/mfunhcu/l1mainentry/cloud_callback_wechat.php', 'XHZN_HCU', 'avorion', 'Avorion321', '/usr/local/apache_arm/htdocs/avorion/', 'http://192.168.1.232:8000/avorion/', 31, 4, 0, 'firefox', 'http://localhost/yii2basic/web/index.php', '-kiosk', 0, 4),
('HCU_PRJ_GQYBG40', 1, 'localhost', 'root', '123456', 'hcudb', 3306, 0, 4, 2, 1, 'http://127.0.0.1/test.php', 'http://aaa.bbb/', 'http://www.hkrob.com/mfunhcu/l1mainentry/cloud_callback_hcu.php', 'AQ_HCU', 'avorion', 'Avorion321', '/usr/local/apache_arm/htdocs/avorion/', 'http://192.168.1.232:8000/avorion/', 31, 4, 0, 'firefox', 'http://localhost/yii2basic/web/index.php', '-', 0, 4),
('HCU_PRJ_NBIOT_HPM_QG', 1, 'localhost', 'root', '123456', 'hcudb', 3306, 3, 1, 0, 1, 'http://127.0.0.1/test.php', 'http://aaa.bbb/', 'http://121.40.185.177/xhzn/mfunhcu/l1mainentry/cloud_callback_hcu.php', 'AQ_HCU', 'avorion', 'Avorion321', '/usr/local/apache_arm/htdocs/avorion/', 'http://192.168.1.232:8000/avorion/', 31, 4, 0, 'firefox', 'http://localhost/yii2basic/web/index.php', '-', 0, 4),
('HCU_PRJ_NBIOT_LPM_CJ', 1, 'localhost', 'root', '123456', 'hcudb', 3306, 3, 1, 0, 1, 'http://127.0.0.1/test.php', 'http://aaa.bbb/', 'http://121.40.185.177/xhzn/mfunhcu/l1mainentry/cloud_callback_hcu.php', 'AQ_HCU', 'avorion', 'Avorion321', '/usr/local/apache_arm/htdocs/avorion/', 'http://192.168.1.232:8000/avorion/', 31, 4, 0, 'firefox', 'http://localhost/yii2basic/web/index.php', '-', 0, 4),
('HCU_PRJ_OPWL_OTDR', 1, 'localhost', 'root', '123456', 'hcudb', 3306, 0, 4, 2, 1, 'http://127.0.0.1/test.php', 'http://aaa.bbb/', 'http://www.hkrob.com/mfunhcu/l1mainentry/cloud_callback_hcu.php', 'AQ_HCU', 'avorion', 'Avorion321', '/usr/local/apache_arm/htdocs/avorion/', 'http://192.168.1.232:8000/avorion/', 31, 4, 0, 'firefox', 'http://localhost/yii2basic/web/index.php', '-', 0, 4),
('HCU_PRJ_TBSWRG30', 1, 'localhost', 'root', '123456', 'hcudb', 3306, 3, 1, 0, 1, 'http://127.0.0.1/test.php', 'http://aaa.bbb/', 'http://121.40.185.177/xhzn/mfunhcu/l1mainentry/cloud_callback_hcu.php', 'AQ_HCU', 'avorion', 'Avorion321', '/usr/local/apache_arm/htdocs/avorion/', 'http://192.168.1.232:8000/avorion/', 31, 4, 0, 'firefox', 'http://localhost/yii2basic/web/index.php', '-', 0, 4),
('HCU_PRJ_TEST_MODE', 1, 'localhost', 'root', '123456', 'hcudb', 3306, 3, 1, 0, 1, 'http://127.0.0.1/test.php', 'http://aaa.bbb/', 'http://121.40.185.177/xhzn/mfunhcu/l1mainentry/cloud_callback_hcu.php', 'AQ_HCU', 'avorion', 'Avorion321', '/usr/local/apache_arm/htdocs/avorion/', 'http://192.168.1.232:8000/avorion/', 31, 4, 0, 'firefox', 'http://localhost/yii2basic/web/index.php', '-', 0, 4);

-- --------------------------------------------------------

--
-- Table structure for table `hcusysengtimer`
--

CREATE TABLE `hcusysengtimer` (
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
(22, 'TID_1S_CLOUDVELA_PERIOD_LINK_HBT', 1, 3600),
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
(39, 'TID_1S_SYSPM_PERIOD_WORKING', 1, 3600),
(40, 'TID_1S_SYSSWM_PERIOD_WORKING', 1, 900),
(41, 'TID_1S_SYSSWM_SEG_DL_WAIT', 1, 3),
(42, 'TID_1S_CANITFLEO_WORKING_SCAN', 1, 10),
(43, 'TID_1S_CANALPHA_WORKING_SCAN', 1, 2),
(44, 'TID_1S_L3BFSC_SYS_CFG_WAIT_FB', 1, 15),
(45, 'TID_1S_L3BFSC_SYS_START_WAIT_FB', 1, 15),
(46, 'TID_1S_L3BFSC_SYS_STOP_WAIT_FB', 1, 15),
(47, 'TID_1S_L3BFSC_TTT_WAIT_FB', 1, 15),
(48, 'TID_1S_L3BFSC_TGU_WAIT_FB', 1, 15),
(49, 'TID_1S_L3BFSC_ERROR_INQ', 1, 60),
(50, 'TID_1S_BFSCUICOMM_PERIOD_READ', 1, 600),
(51, 'TID_1S_L3BFDF_CFG_START_WAIT_FB', 1, 15),
(52, 'TID_1S_L3BFDF_SUSPEND_WAIT_FB', 1, 15),
(53, 'TID_1S_L3BFDF_RESUME_WAIT_FB', 1, 15),
(54, 'TID_1S_L3BFDF_TTT_WAIT_FB', 1, 3),
(55, 'TID_1S_BFDFUICOMM_PERIOD_READ', 1, 100),
(56, 'TID_1S_L3BFHS_CFG_START_WAIT_FB', 1, 5),
(57, 'TID_1S_L3BFHS_SUSPEND_WAIT_FB', 1, 5),
(58, 'TID_1S_L3BFHS_RESUME_WAIT_FB', 1, 5),
(59, 'TID_1S_L3BFHS_CAL_ZERO_WAIT_FB', 1, 5),
(60, 'TID_1S_L3BFHS_CAL_FULL_WAIT_FB', 1, 5),
(61, 'TID_1S_BFHSUICOMM_PERIOD_READ', 1, 600),
(62, 'TID_1S_L3AQYCG10_PERIOD_READ', 1, 600),
(63, 'TID_1S_L3AQYCG20_PERIOD_READ', 1, 600),
(64, 'TID_1S_L3AQYCG20_PERIOD_STA_SCAN', 1, 30),
(65, 'TID_1S_L3AQYCG20_START_MIN_REPORT', 1, 60),
(66, 'TID_1S_L3AQYCG20_STOP_MIN_REPORT', 1, 60),
(67, 'TID_1S_L3AQYCG20_START_HOUR_REPORT', 1, 60),
(68, 'TID_1S_L3AQYCG20_STOP_HOUR_REPORT', 1, 60),
(69, 'TID_1S_L3AQYCG20_START_DAY_REPORT', 1, 60),
(70, 'TID_1S_L3AQYCG20_STOP_DAY_REPORT', 1, 60),
(71, 'TID_1S_L3DAYCG21_PERIOD_READ', 1, 60),
(72, 'TID_1S_L3TBSWRG30_PERIOD_READ', 1, 600),
(73, 'TID_1S_L3GQYBG40_PERIOD_READ', 1, 600),
(74, 'TID_1S_L3CXGLACM_PERIOD_READ', 1, 600),
(75, 'TID_1S_L3CXILC_PERIOD_READ', 1, 600),
(76, 'TID_1S_L3NBHPM_PERIOD_READ', 1, 600),
(77, 'TID_1S_L3NBLPM_PERIOD_READ', 1, 600),
(78, 'TID_1S_L3OPWLOTDR_PERIOD_READ', 1, 600),
(79, 'TID_1S_L3HATE_WORK_CTRL_FB', 1, 5),
(80, 'TID_1S_MAX', 1, 0),
(81, 'TID_10MS_MIN', 2, 0),
(82, 'TID_10MS_SVRCON_TEST', 2, 10000),
(83, 'TID_10MS_L3BFSC_PERIOD_STA_SCAN', 2, 50),
(84, 'TID_10MS_CANITFLEO_SIMULATION_DATA', 2, 20),
(85, 'TID_10MS_CANALPHA_SIMULATION_DATA', 2, 5),
(86, 'TID_10MS_L3BFDF_PERIOD_STA_SCAN', 2, 50),
(87, 'TID_10MS_L3BFHS_PERIOD_STA_SCAN', 2, 50),
(88, 'TID_10MS_MAX', 2, 0),
(89, 'TID_1MS_MIN', 3, 0),
(90, 'TID_1MS_SVRCON_TEST', 3, 100),
(91, 'TID_1MS_MAX', 3, 0),
(92, 'TID_MAX', 1, 0);

-- --------------------------------------------------------

--
-- Table structure for table `hcusyspmglobaldatainfo`
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
-- Dumping data for table `hcusyspmglobaldatainfo`
--

INSERT INTO `hcusyspmglobaldatainfo` (`sid`, `taskhcuvmec`, `taskhwinvec`, `tasksyspmec`, `taskmodbusec`, `taskcloudvelaec`, `taskavorionec`, `taskspsvirgoec`, `taskhsmmpec`, `taskemcec`, `taskpm25ec`, `taskwinddirec`, `taskwindspdec`, `tasktempec`, `taskhumidec`, `tasknoiseec`, `taskairprsec`, `taskco1ec`, `tasklightstrec`, `taskalcoholec`, `taskhchoec`, `tasktoxicgasec`, `restartcnt`, `cloudvelaconncnt`, `cloudvelaconnfailcnt`, `cloudveladisccnt`, `clouddatatimeoutcnt`, `socketdisccnt`, `cpuoccupy`, `memoccupy`, `diskoccupy`, `cputemp`, `timestamp`) VALUES
(1521, 0, 723, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 655, 3, 12, 30, 27, 1517909632),
(1522, 0, 1443, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 1855, 3, 12, 30, 27, 1517913232),
(1523, 0, 2160, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 3055, 3, 12, 30, 27, 1517916832),
(1524, 0, 723, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 13, 35, 27, 1520395594),
(1525, 0, 1443, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 0, 13, 35, 27, 1520399194),
(1526, 0, 2163, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6, 0, 14, 35, 27, 1520402794),
(1527, 0, 723, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 13, 36, 27, 1520484475),
(1528, 0, 1443, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 3, 0, 13, 36, 27, 1520488075),
(1529, 0, 2163, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 3, 0, 13, 36, 27, 1520491675),
(1530, 0, 723, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 2, 13, 35, 27, 1520507508),
(1531, 0, 723, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 13, 35, 27, 1520512065),
(1532, 0, 1443, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 13, 35, 27, 1520515665),
(1533, 0, 2163, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 13, 35, 27, 1520519265),
(1534, 0, 2883, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 13, 35, 27, 1520522865),
(1535, 0, 3600, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 13, 35, 27, 1520526465),
(1536, 0, 4320, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 13, 35, 27, 1520530065),
(1537, 0, 5040, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 13, 35, 27, 1520533665),
(1538, 0, 5760, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 13, 35, 27, 1520537265),
(1539, 0, 6480, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 13, 35, 27, 1520540865),
(1540, 0, 7200, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 13, 35, 27, 1520544465),
(1541, 0, 7920, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 13, 35, 27, 1520548065),
(1542, 0, 8640, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 13, 35, 27, 1520551665),
(1543, 0, 9360, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 13, 35, 27, 1520555265),
(1544, 0, 723, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 13, 36, 27, 1520561027),
(1545, 0, 1443, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 13, 36, 27, 1520564627),
(1546, 0, 723, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 13, 36, 27, 1520572970),
(1547, 0, 723, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 11, 36, 27, 1520588651),
(1548, 0, 1443, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 2, 1, 12, 36, 27, 1520592251);

-- --------------------------------------------------------

--
-- Table structure for table `hcusysswm_swdl`
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
-- Table structure for table `hcusysswm_swpkg`
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

-- --------------------------------------------------------

--
-- Table structure for table `hcutempbmp180datainfo`
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
-- Table structure for table `hcutempdatainfo`
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
-- Table structure for table `hcutempdht11datainfo`
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
-- Table structure for table `hcutempmth01datainfo`
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
-- Table structure for table `hcutemprht03datainfo`
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
-- Table structure for table `hcutempsht20datainfo`
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
-- Table structure for table `hcutoxicgasdatainfo`
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
-- Table structure for table `hcutoxicgasmq135datainfo`
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
-- Table structure for table `hcutoxicgaszp01vocdatainfo`
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
-- Table structure for table `hcutracemodulectr`
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
(42, 'HUICOBUSCODEC', 1, 1, 1, 1, 1),
(43, 'HSMMP', 1, 1, 1, 1, 1),
(44, 'EMC', 1, 1, 1, 1, 1),
(45, 'HUMID', 1, 1, 1, 1, 1),
(46, 'PM25', 1, 1, 1, 1, 1),
(47, 'TEMP', 1, 1, 1, 1, 1),
(48, 'WINDDIR', 1, 1, 1, 1, 1),
(49, 'WINDSPD', 1, 1, 1, 1, 1),
(50, 'NOISE', 1, 1, 1, 1, 1),
(51, 'AIRPRS', 1, 1, 1, 1, 1),
(52, 'CO1', 1, 1, 1, 1, 1),
(53, 'LIGHTSTR', 1, 1, 1, 1, 1),
(54, 'ALCOHOL', 1, 1, 1, 1, 1),
(55, 'HCHO', 1, 1, 1, 1, 1),
(56, 'TOXICGAS', 1, 1, 1, 1, 1),
(57, 'IWM', 1, 1, 1, 1, 1),
(58, 'IHM', 1, 1, 1, 1, 1),
(59, 'IGM', 1, 1, 1, 1, 1),
(60, 'IPM', 1, 1, 1, 1, 1),
(61, 'SVRCON', 1, 1, 1, 1, 1),
(62, 'SYSPM', 1, 1, 1, 1, 1),
(63, 'SYSSWM', 1, 1, 1, 1, 1),
(64, 'PM25SHARP', 1, 1, 1, 1, 1),
(65, 'CANITFLEO', 1, 1, 1, 1, 1),
(66, 'CANALPHA', 1, 1, 1, 1, 1),
(67, 'L3AQYCG10', 1, 1, 1, 1, 1),
(68, 'L3AQYCG20', 1, 1, 1, 1, 1),
(69, 'L3DAYCG21', 1, 1, 1, 1, 1),
(70, 'L3TBSWRG30', 1, 1, 1, 1, 1),
(71, 'L3GQYBG40', 1, 1, 1, 1, 1),
(72, 'L3CXILC', 1, 1, 1, 1, 1),
(73, 'L3CXGLACM', 1, 1, 1, 1, 1),
(74, 'L3NBLPM', 1, 1, 1, 1, 1),
(75, 'L3NBHPM', 1, 1, 1, 1, 1),
(76, 'L3BFSC', 1, 1, 1, 1, 1),
(77, 'BFSCUICOMM', 1, 1, 1, 1, 1),
(78, 'JSONINOTIFY', 1, 1, 1, 1, 1),
(79, 'BFDFUICOMM', 1, 1, 1, 1, 1),
(80, 'BFHSUICOMM', 1, 1, 1, 1, 1),
(81, 'L3OPWLOTDR', 1, 1, 1, 1, 1),
(82, 'L3BFDF', 1, 1, 1, 1, 1),
(83, 'L3BFHS', 1, 1, 1, 1, 1),
(84, 'L3HATE', 1, 1, 1, 1, 1),
(85, 'TASKMAX', 1, 1, 1, 1, 1);

-- --------------------------------------------------------

--
-- Table structure for table `hcutracemsgctr`
--

CREATE TABLE `hcutracemsgctr` (
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
(194, 'MSG_ID_CANITF_DATA_REPORT', 1, 1, 1),
(195, 'MSG_ID_CANITF_SYSSWM_INVENTORY_REPORT', 1, 1, 1),
(196, 'MSG_ID_SYSSWM_CANITF_INVENTORY_CONFIRM', 1, 1, 1),
(197, 'MSG_ID_CANITF_SYSSWM_SW_PACKAGE_REPORT', 1, 1, 1),
(198, 'MSG_ID_SYSSWM_CANITF_SW_PACKAGE_CONFIRM', 1, 1, 1),
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
(245, 'MSG_ID_L3BFDF_CAN_DYN_CAL_REQ', 1, 1, 1),
(246, 'MSG_ID_CAN_L3BFDF_DYN_CAL_RESP', 1, 1, 1),
(247, 'MSG_ID_CAN_L3BFDF_WS_NEW_READY_EVENT', 1, 1, 1),
(248, 'MSG_ID_L3BFDF_CAN_WS_COMB_OUT', 1, 1, 1),
(249, 'MSG_ID_CAN_L3BFDF_WS_COMB_OUT_FB', 1, 1, 1),
(250, 'MSG_ID_CAN_L3BFDF_BASKET_CLEAN_IND', 1, 1, 1),
(251, 'MSG_ID_L3BFHS_CAN_SYS_CFG_REQ', 1, 1, 1),
(252, 'MSG_ID_CAN_L3BFHS_SYS_CFG_RESP', 1, 1, 1),
(253, 'MSG_ID_L3BFHS_CAN_CAL_ZERO_REQ', 1, 1, 1),
(254, 'MSG_ID_CAN_L3BFHS_CAL_ZERO_RESP', 1, 1, 1),
(255, 'MSG_ID_L3BFHS_CAN_CAL_FULL_REQ', 1, 1, 1),
(256, 'MSG_ID_CAN_L3BFHS_CAL_FULL_RESP', 1, 1, 1),
(257, 'MSG_ID_L3BFHS_CAN_DYN_ZERO_REQ', 1, 1, 1),
(258, 'MSG_ID_CAN_L3BFHS_DYN_ZERO_RESP', 1, 1, 1),
(259, 'MSG_ID_L3BFHS_CAN_DYN_FULL_REQ', 1, 1, 1),
(260, 'MSG_ID_CAN_L3BFHS_DYN_FULL_RESP', 1, 1, 1),
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
(271, 'MSG_ID_UICOMM_L3BFHS_CTRL_CMD_REQ', 1, 1, 1),
(272, 'MSG_ID_L3BFHS_UICOMM_CTRL_CMD_RESP', 1, 1, 1),
(273, 'MSG_ID_SUI_TEST_CMD_REQ', 1, 1, 1),
(274, 'MSG_ID_SUI_TEST_CMD_RESP', 1, 1, 1),
(275, 'MSG_ID_SUI_STARTUP_IND', 1, 1, 1),
(276, 'MSG_ID_SUI_FAULT_IND', 1, 1, 1),
(277, 'MSG_ID_SUI_SUSPEND_REQ', 1, 1, 1),
(278, 'MSG_ID_SUI_SUSPEND_RESP', 1, 1, 1),
(279, 'MSG_ID_SUI_RESUME_REQ', 1, 1, 1),
(280, 'MSG_ID_SUI_RESUME_RESP', 1, 1, 1),
(281, 'MSG_ID_SUI_HEART_BEAT_REPORT', 1, 1, 1),
(282, 'MSG_ID_SUI_HEART_BEAT_CONFIRM', 1, 1, 1),
(283, 'MSG_ID_HUICOBUS_UIR_INIT_REQ', 1, 1, 1),
(284, 'MSG_ID_HUICOBUS_UIR_START_RESUME_REQ', 1, 1, 1),
(285, 'MSG_ID_HUICOBUS_UIR_STOP_SUSPEND_REQ', 1, 1, 1),
(286, 'MSG_ID_HUICOBUS_UIR_STATIC_CALI_REQ', 1, 1, 1),
(287, 'MSG_ID_HUICOBUS_UIR_DYNAMIC_CALI_REQ', 1, 1, 1),
(288, 'MSG_ID_HUICOBUS_UIR_TEST_CMD_REQ', 1, 1, 1),
(289, 'MSG_ID_HUICOBUS_UIR_ONE_KEY_ZERO_REQ', 1, 1, 1),
(290, 'MSG_ID_L3AQYC_EXG_CTRL_REQ', 1, 1, 1),
(291, 'MSG_ID_L3AQYC_EXG_CTRL_RESP', 1, 1, 1),
(292, 'MSG_ID_L3AQYC_EXG_DATA_REPORT', 1, 1, 1),
(293, 'MSG_ID_CLOUDVELA_LLCZHB_FRAME_REQ', 1, 1, 1),
(294, 'MSG_ID_LLCZHB_CLOUDVELA_FRAME_RESP', 1, 1, 1),
(295, 'MSG_ID_LLCZHB_L3MOD_CTRL_REQ', 1, 1, 1),
(296, 'MSG_ID_L3MOD_LLCZHB_CTRL_RESP', 1, 1, 1),
(297, 'MSG_ID_L3MOD_LLCZHB_DATA_REPORT', 1, 1, 1),
(298, 'MSG_ID_ZHBL3MOD_EXG_CTRL_REQ', 1, 1, 1),
(299, 'MSG_ID_ZHBL3MOD_EXG_CTRL_RESP', 1, 1, 1),
(300, 'MSG_ID_ZHBL3MOD_EXG_DATA_REPORT', 1, 1, 1),
(301, 'MSG_ID_L3HATE_TC_START', 1, 1, 1),
(302, 'MSG_ID_ETH_L3HATE_FRAME_RCV', 1, 1, 1),
(303, 'MSG_ID_SPS_L3HATE_FRAME_RCV', 1, 1, 1),
(304, 'MSG_ID_CAN_L3HATE_FRAME_RCV', 1, 1, 1),
(305, 'MSG_ID_MQTT_L3HATE_FRAME_RCV', 1, 1, 1),
(306, 'MSG_ID_COM_MAX', 0, 0, 0);

-- --------------------------------------------------------

--
-- Table structure for table `hcuwinddirdatainfo`
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
-- Table structure for table `hcuwindspddatainfo`
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
-- Indexes for table `hcubfdfcallcell`
--
ALTER TABLE `hcubfdfcallcell`
  ADD PRIMARY KEY (`sid`);

--
-- Indexes for table `hcubfdfgrouppara`
--
ALTER TABLE `hcubfdfgrouppara`
  ADD PRIMARY KEY (`sid`);

--
-- Indexes for table `hcubfdfproductpara`
--
ALTER TABLE `hcubfdfproductpara`
  ADD PRIMARY KEY (`configid`),
  ADD UNIQUE KEY `confname` (`confname`),
  ADD UNIQUE KEY `confname_2` (`confname`);

--
-- Indexes for table `hcubfdfstadatainfo`
--
ALTER TABLE `hcubfdfstadatainfo`
  ADD PRIMARY KEY (`sid`);

--
-- Indexes for table `hcubfdfsystempara`
--
ALTER TABLE `hcubfdfsystempara`
  ADD PRIMARY KEY (`sid`);

--
-- Indexes for table `hcubfhscallcell`
--
ALTER TABLE `hcubfhscallcell`
  ADD PRIMARY KEY (`sid`);

--
-- Indexes for table `hcubfhsproductpara`
--
ALTER TABLE `hcubfhsproductpara`
  ADD PRIMARY KEY (`configid`),
  ADD UNIQUE KEY `confname` (`confname`),
  ADD UNIQUE KEY `confname_2` (`confname`);

--
-- Indexes for table `hcubfhsstadatainfo`
--
ALTER TABLE `hcubfhsstadatainfo`
  ADD PRIMARY KEY (`sid`);

--
-- Indexes for table `hcubfhssystempara`
--
ALTER TABLE `hcubfhssystempara`
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
  ADD PRIMARY KEY (`uid`);

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
-- AUTO_INCREMENT for table `hcubfdfcallcell`
--
ALTER TABLE `hcubfdfcallcell`
  MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=66;
--
-- AUTO_INCREMENT for table `hcubfdfgrouppara`
--
ALTER TABLE `hcubfdfgrouppara`
  MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=31;
--
-- AUTO_INCREMENT for table `hcubfdfproductpara`
--
ALTER TABLE `hcubfdfproductpara`
  MODIFY `configid` int(4) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=20;
--
-- AUTO_INCREMENT for table `hcubfdfstadatainfo`
--
ALTER TABLE `hcubfdfstadatainfo`
  MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=1268;
--
-- AUTO_INCREMENT for table `hcubfdfsystempara`
--
ALTER TABLE `hcubfdfsystempara`
  MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=2;
--
-- AUTO_INCREMENT for table `hcubfhscallcell`
--
ALTER TABLE `hcubfhscallcell`
  MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=2;
--
-- AUTO_INCREMENT for table `hcubfhsproductpara`
--
ALTER TABLE `hcubfhsproductpara`
  MODIFY `configid` int(4) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=26;
--
-- AUTO_INCREMENT for table `hcubfhsstadatainfo`
--
ALTER TABLE `hcubfhsstadatainfo`
  MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=2;
--
-- AUTO_INCREMENT for table `hcubfhssystempara`
--
ALTER TABLE `hcubfhssystempara`
  MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=2;
--
-- AUTO_INCREMENT for table `hcubfsccalibration`
--
ALTER TABLE `hcubfsccalibration`
  MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=2;
--
-- AUTO_INCREMENT for table `hcubfscconfigpara`
--
ALTER TABLE `hcubfscconfigpara`
  MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=13;
--
-- AUTO_INCREMENT for table `hcubfscfb2ui`
--
ALTER TABLE `hcubfscfb2ui`
  MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=5;
--
-- AUTO_INCREMENT for table `hcubfscflowsheet`
--
ALTER TABLE `hcubfscflowsheet`
  MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=6;
--
-- AUTO_INCREMENT for table `hcubfsclanguagedict`
--
ALTER TABLE `hcubfsclanguagedict`
  MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=233;
--
-- AUTO_INCREMENT for table `hcubfscstadatainfo`
--
ALTER TABLE `hcubfscstadatainfo`
  MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=24856;
--
-- AUTO_INCREMENT for table `hcubfscstaticpara`
--
ALTER TABLE `hcubfscstaticpara`
  MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=2;
--
-- AUTO_INCREMENT for table `hcubfznaccount`
--
ALTER TABLE `hcubfznaccount`
  MODIFY `uid` int(4) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=12;
--
-- AUTO_INCREMENT for table `hcuco1datainfo`
--
ALTER TABLE `hcuco1datainfo`
  MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT;
--
-- AUTO_INCREMENT for table `hcuemcdatainfo`
--
ALTER TABLE `hcuemcdatainfo`
  MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT;
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
  MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=2;
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
  MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT;
--
-- AUTO_INCREMENT for table `hcusyspmglobaldatainfo`
--
ALTER TABLE `hcusyspmglobaldatainfo`
  MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=1549;
--
-- AUTO_INCREMENT for table `hcusysswm_swdl`
--
ALTER TABLE `hcusysswm_swdl`
  MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT;
--
-- AUTO_INCREMENT for table `hcusysswm_swpkg`
--
ALTER TABLE `hcusysswm_swpkg`
  MODIFY `sid` int(4) NOT NULL AUTO_INCREMENT;
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
