/*
 * dbiihm.c
 *
 *  Created on: 2016年7月20日
 *      Author: hitpony
 */

#include "../l0dbi/dbiihm.h"
#include "../l0service/trace.h"
#include "../l0dbi/dbicom.h"

/*

--
-- 表的结构 `hcuihmcj188datainfo`
--

CREATE TABLE IF NOT EXISTS `hcuihmcj188datainfo` (
  `sid` int(4) NOT NULL AUTO_INCREMENT,
  `cj188address` char(14) NOT NULL,
  `timestamp` int(4) NOT NULL,
  `equtype` int(1) NOT NULL,
  `heatpower` float(8,2) NOT NULL,
  `heatpoweruint` int(1) NOT NULL,
  `currentheat` float(8,2) NOT NULL,
  `currentheatuint` int(1) NOT NULL,
  `todayheat` float(8,2) NOT NULL,
  `todayheatuint` int(1) NOT NULL,
  `currentaccuvolume` float(8,2) NOT NULL,
  `currentaccuvolumeuint` int(1) NOT NULL,
  `flowvolume` float(8,2) NOT NULL,
  `flowvolumeuint` int(1) NOT NULL,
  `lastmonth` int(1) NOT NULL,
  `accumuworktime` int(3) NOT NULL,
  `supplywatertemp` float(6,2) NOT NULL,
  `backwatertemp` float(6,2) NOT NULL,
  `realtime` char(14) NOT NULL,
  `st` char(4) NOT NULL,
  `billdate` int(1) NOT NULL,
  `readdate` int(1) NOT NULL,
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
  PRIMARY KEY (`sid`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8 AUTO_INCREMENT=1 ;



*/

