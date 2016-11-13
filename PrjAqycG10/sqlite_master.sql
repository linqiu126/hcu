BEGIN TRANSACTION;
INSERT INTO "sqlite_master" VALUES('table','hcuemcdatainfo','hcuemcdatainfo',3,'CREATE TABLE `hcuemcdatainfo` (
  `sid` int(4) NOT NULL,
  `deviceid` int(4) NOT NULL,
  `timestamp` int(4) NOT NULL,
  `dataformat` int(1) NOT NULL,
  `emcvalue` int(4) NOT NULL,
  `gpsx` int(4) NOT NULL,
  `gpsy` int(4) NOT NULL,
  `gpsz` int(4) NOT NULL,
  `onofflineflag` int(2) NOT NULL,
  PRIMARY KEY (`sid`)
)');
INSERT INTO "sqlite_master" VALUES('index','sqlite_autoindex_hcuemcdatainfo_1','hcuemcdatainfo',4,NULL);
INSERT INTO "sqlite_master" VALUES('table','hcuhsmmpdatainfo','hcuhsmmpdatainfo',2,'CREATE TABLE `hcuhsmmpdatainfo` (
  `sid` int(4) NOT NULL,
  `deviceid` int(4) NOT NULL,
  `timestamp` int(4) NOT NULL,
  `hsmmpfdir` char(100) NOT NULL,
  `hsmmpfname` char(100) NOT NULL,
  `hsmmplink` char(100) NOT NULL,
  `gpsx` int(4) NOT NULL,
  `gpsy` int(4) NOT NULL,
  `gpsz` int(4) NOT NULL,
  `onofflineflag` int(2) NOT NULL,
  PRIMARY KEY (`sid`)
)');
INSERT INTO "sqlite_master" VALUES('index','sqlite_autoindex_hcuhsmmpdatainfo_1','hcuhsmmpdatainfo',5,NULL);
INSERT INTO "sqlite_master" VALUES('table','hcuhumiddatainfo','hcuhumiddatainfo',6,'CREATE TABLE `hcuhumiddatainfo` (
  `sid` int(4) NOT NULL,
  `deviceid` int(4) NOT NULL,
  `timestamp` int(4) NOT NULL,
  `dataformat` int(1) NOT NULL,
  `humidvalue` int(4) NOT NULL,
  `gpsx` int(4) NOT NULL,
  `gpsy` int(4) NOT NULL,
  `gpsz` int(4) NOT NULL,
  `onofflineflag` int(2) NOT NULL,
  PRIMARY KEY (`sid`)
)');
INSERT INTO "sqlite_master" VALUES('index','sqlite_autoindex_hcuhumiddatainfo_1','hcuhumiddatainfo',7,NULL);
INSERT INTO "sqlite_master" VALUES('table','hcunoisedatainfo','hcunoisedatainfo',8,'CREATE TABLE `hcunoisedatainfo` (
  `sid` int(4) NOT NULL ,
  `deviceid` int(4) NOT NULL,
  `timestamp` int(4) NOT NULL,
  `dataformat` int(1) NOT NULL,
  `noisevalue` int(4) NOT NULL,
  `gpsx` int(4) NOT NULL,
  `gpsy` int(4) NOT NULL,
  `gpsz` int(4) NOT NULL,
  `onofflineflag` int(2) NOT NULL,
  PRIMARY KEY (`sid`)
)');
INSERT INTO "sqlite_master" VALUES('index','sqlite_autoindex_hcunoisedatainfo_1','hcunoisedatainfo',9,NULL);
INSERT INTO "sqlite_master" VALUES('table','hcupm25datainfo','hcupm25datainfo',10,'CREATE TABLE `hcupm25datainfo` (
  `sid` int(4) NOT NULL ,
  `deviceid` int(4) NOT NULL,
  `timestamp` int(4) NOT NULL,
  `dataformat` int(1) NOT NULL,
  `pm1d0value` int(4) NOT NULL,
  `pm2d5value` int(4) NOT NULL,
  `pm10dvalue` int(4) NOT NULL,
  `gpsx` int(4) NOT NULL,
  `gpsy` int(4) NOT NULL,
  `gpsz` int(4) NOT NULL,
  `onofflineflag` int(2) NOT NULL,
  PRIMARY KEY (`sid`)
)');
INSERT INTO "sqlite_master" VALUES('index','sqlite_autoindex_hcupm25datainfo_1','hcupm25datainfo',11,NULL);
INSERT INTO "sqlite_master" VALUES('table','hcusperrcntdatainfo','hcusperrcntdatainfo',12,'CREATE TABLE `hcusperrcntdatainfo` (
  `sid` int(4) NOT NULL,
  `taskhcuvmec` int(4) NOT NULL,
  `taskhwinvec` int(4) NOT NULL,
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
  `timestamp` int(4) NOT NULL,
  PRIMARY KEY (`sid`)
)');
INSERT INTO "sqlite_master" VALUES('index','sqlite_autoindex_hcusperrcntdatainfo_1','hcusperrcntdatainfo',13,NULL);
INSERT INTO "sqlite_master" VALUES('table','hcusysengpar','hcusysengpar',14,'CREATE TABLE `hcusysengpar` (
  `prjname` char(20) NOT NULL,
  `commbackhawlcon` int(1) NOT NULL,
  `commhwboardethernet` int(1) NOT NULL,
  `commhwboardusbnet` int(1) NOT NULL,
  `commhwboardwifi` int(1) NOT NULL,
  `commhwboard3g4g` int(1) NOT NULL,
  `commhwboardgps` int(1) NOT NULL,
  `commhwboardlcd` int(1) NOT NULL,
  `commhwboardzeegbe` int(1) NOT NULL,
  `commhwboardflash` int(1) NOT NULL,
  `commframemodbus` int(1) NOT NULL,
  `commframespsvirgo` int(1) NOT NULL,
  `commframeavorion` int(1) NOT NULL,
  `commframecloudvela` int(1) NOT NULL,
  `commfrontsps485` int(1) NOT NULL,
  `commfrontsps232` int(1) NOT NULL,
  `commfrontmicrophone` int(1) NOT NULL,
  `commfrontcamera` int(1) NOT NULL,
  `commfrontble` int(1) NOT NULL,
  `commfrontsensoremc` int(1) NOT NULL,
  `commfrontsensorpm25` int(1) NOT NULL,
  `commfrontsensortemp` int(1) NOT NULL,
  `commfrontsensorhumid` int(1) NOT NULL,
  `commfrontsensorwinddir` int(1) NOT NULL,
  `commfrontsensorwindspd` int(1) NOT NULL,
  `commfrontsensornoise` int(1) NOT NULL,
  `commfrontsensorhsmmp` int(1) NOT NULL,
  `hcudbhost` char(20) NOT NULL,
  `hcudbuser` char(20) NOT NULL,
  `hcudbpsw` char(20) NOT NULL,
  `hcudbname` char(20) NOT NULL,
  `hcudbport` int(1) NOT NULL,
  `cloudhttpaddlocal` char(64) NOT NULL,
  `cloudhttpaddtest` char(64) NOT NULL,
  `cloudhttpaddsae` char(64) NOT NULL,
  `cloudhttpaddjd` char(64) NOT NULL,
  `cloudhttpaddwechat` char(64) NOT NULL,
  `cloudbhservername` char(12) NOT NULL,
  `cloudbhhcuname` char(12) NOT NULL,
  `cloudbhitfframestd` int(1) NOT NULL,
  `hcuvideoserverdir` char(64) NOT NULL,
  `hcuvideoserverhttp` char(64) NOT NULL,
  `debugmode` int(1) NOT NULL,
  `tracemode` int(1) NOT NULL,
  `browselautostartupflag` int(1) NOT NULL,
  `browselprog` char(20) NOT NULL,
  `browselstartupaddress` char(128) NOT NULL,
  `browselworkingoption` char(128) NOT NULL,
  PRIMARY KEY (`prjname`)
)');
INSERT INTO "sqlite_master" VALUES('index','sqlite_autoindex_hcusysengpar_1','hcusysengpar',15,NULL);
INSERT INTO "sqlite_master" VALUES('table','hcutempdatainfo','hcutempdatainfo',16,'CREATE TABLE `hcutempdatainfo` (
  `sid` int(4) NOT NULL ,
  `deviceid` int(4) NOT NULL,
  `timestamp` int(4) NOT NULL,
  `dataformat` int(1) NOT NULL,
  `tempvalue` int(4) NOT NULL,
  `gpsx` int(4) NOT NULL,
  `gpsy` int(4) NOT NULL,
  `gpsz` int(4) NOT NULL,
  `onofflineflag` int(2) NOT NULL,
  PRIMARY KEY (`sid`)
)');
INSERT INTO "sqlite_master" VALUES('index','sqlite_autoindex_hcutempdatainfo_1','hcutempdatainfo',17,NULL);
INSERT INTO "sqlite_master" VALUES('table','hcutracemodulectr','hcutracemodulectr',18,'CREATE TABLE `hcutracemodulectr` (
  `moduleid` int(2) NOT NULL,
  `modulename` char(12) NOT NULL,
  `modulectrflag` int(1) NOT NULL,
  `moduletoallow` int(1) NOT NULL,
  `moduletorestrict` int(1) NOT NULL,
  `modulefromallow` int(1) NOT NULL,
  `modulefromrestrict` int(1) NOT NULL,
  PRIMARY KEY (`moduleid`)
)');
INSERT INTO "sqlite_master" VALUES('index','sqlite_autoindex_hcutracemodulectr_1','hcutracemodulectr',19,NULL);
INSERT INTO "sqlite_master" VALUES('table','hcutracemsgctr','hcutracemsgctr',20,'CREATE TABLE `hcutracemsgctr` (
  `msgid` int(2) NOT NULL,
  `msgname` char(70) NOT NULL,
  `msgctrflag` int(1) NOT NULL,
  `msgallow` int(1) NOT NULL,
  `msgrestrict` int(1) NOT NULL,
  PRIMARY KEY (`msgid`)
)');
INSERT INTO "sqlite_master" VALUES('index','sqlite_autoindex_hcutracemsgctr_1','hcutracemsgctr',21,NULL);
INSERT INTO "sqlite_master" VALUES('table','hcuwinddirdatainfo','hcuwinddirdatainfo',22,'CREATE TABLE `hcuwinddirdatainfo` (
  `sid` int(4) NOT NULL ,
  `deviceid` int(4) NOT NULL,
  `timestamp` int(4) NOT NULL,
  `dataformat` int(1) NOT NULL,
  `winddirvalue` int(4) NOT NULL,
  `gpsx` int(4) NOT NULL,
  `gpsy` int(4) NOT NULL,
  `gpsz` int(4) NOT NULL,
  `onofflineflag` int(2) NOT NULL,
  PRIMARY KEY (`sid`)
)');
INSERT INTO "sqlite_master" VALUES('index','sqlite_autoindex_hcuwinddirdatainfo_1','hcuwinddirdatainfo',23,NULL);
INSERT INTO "sqlite_master" VALUES('table','hcuwindspddatainfo','hcuwindspddatainfo',24,'CREATE TABLE `hcuwindspddatainfo` (
  `sid` int(4) NOT NULL,
  `deviceid` int(4) NOT NULL,
  `timestamp` int(4) NOT NULL,
  `dataformat` int(1) NOT NULL,
  `windspdvalue` int(4) NOT NULL,
  `gpsx` int(4) NOT NULL,
  `gpsy` int(4) NOT NULL,
  `gpsz` int(4) NOT NULL,
  `onofflineflag` int(2) NOT NULL,
  PRIMARY KEY (`sid`)
)');
INSERT INTO "sqlite_master" VALUES('index','sqlite_autoindex_hcuwindspddatainfo_1','hcuwindspddatainfo',25,NULL);
COMMIT;
