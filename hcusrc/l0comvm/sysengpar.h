/*
 * sysengpar.h
 *
 *  Created on: 2016年1月6日
 *      Author: test
 */

#ifndef L0COMVM_SYSENGPAR_H_
#define L0COMVM_SYSENGPAR_H_

#include "../l0comvm/comtype.h"
#include "../l0comvm/sysdim.h"

//基础的通信和硬件部分
//由于内存和硬件大小，相对于这么一点配置参数来说，不是一个太大的问题，故而在针对不同项目的时候，这部分将会时全集，从而简化本地配置界面，以及相应数据库的操纵
//从未来进一步发展的角度，过于复杂的参数配置以及控制，都会影响这个系统的运作。如果没有特殊情况，不再增加新的可配参数内容。剩下的参数，都作为软件程序级参数配置内容。
typedef struct SysEngParElementComm
{
	UINT8  commBackHawlCon;
	UINT8  commHwBoardEthernet;
	UINT8  commHwBoardUsbnet;
	UINT8  commHwBoardWifi;
	UINT8  commHwBoard3g4g;
	UINT8  commHwBoardGps;
	UINT8  commHwBoardLcd;
	UINT8  commHwBoardLed;
	UINT8  commHwBoardZeegbe;
	UINT8  commHwBoardFlash;
	UINT8  commFrameModbus;
	UINT8  commFrameSpsvirgo;
	UINT8  commFrameAvorion;
	UINT8  commFrameCloudvela;
	UINT8  commFrameI2cbuslibra;
	UINT8  commFrameSpibusaries;
	UINT8  commFrameNbiotcj188;
	UINT8  commFrameNbiotqg376;
	UINT8  commFrontSps485;
	UINT8  commFrontSps232;
	UINT8  commFrontMicrophone;
	UINT8  commFrontCamera;
	UINT8  commFrontBle;
	UINT8  commFrontGpio;
	UINT8  commFrontI2c;
	UINT8  commFrontSpi;
	UINT8  commFrontPwm;
	UINT8  commFrontAdc;
	UINT8  commFrontSwitch;
	UINT8  commFrontRelay;
	UINT8  commFrontMotor;
	UINT8  commFrontSensorEmc;
	UINT8  commFrontSensorPm25;
	UINT8  commFrontSensorTemp;
	UINT8  commFrontSensorHumid;
	UINT8  commFrontSensorWinddir;
	UINT8  commFrontSensorWindspd;
	UINT8  commFrontSensorNoise;
	UINT8  commFrontSensorHsmmp;
	UINT8  commFrontSensorAirprs;
	UINT8  commFrontSensorCo1;
	UINT8  commFrontSensorLightstr;
	UINT8  commFrontSensorAlcohol;
	UINT8  commFrontSensorHcho;
	UINT8  commFrontSensorToxicgas;
	UINT8  commFrontSensorIwm;
	UINT8  commFrontSensorIhm;
	UINT8  commFrontSensorIgm;
	UINT8  commFrontSensorIpm;
	UINT8  commFrontSensorPm25Sharp;//by shanchun
	UINT8  commFrontCanitf;
	//不通过数据库配置的参数区域
#if (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_AQYC_OBSOLETE_ID)
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_TEST_MODE_ID)
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_AQYCG10_335D_ID)
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_AQYCG20_RASBERRY_ID)
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_TBSWRG30_ID)
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_GQYBG40_ID)
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_CXILC_ID)
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_CXGLACM_ID)
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_NBIOT_LPM_CJ_ID)
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_NBIOT_HPM_QG_ID)
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFSC_CBU_ID)
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_OPWL_OTDR_ID)

//小技巧，不要这部分，以便加强编译检查
#else
	#error Un-correct constant definition
#endif

}SysEngParElementComm_t;

//数据库部分
#define SYS_ENG_PAR_ELEMENT_DB_LEN 20
typedef struct SysEngParElementDb
{
	char  hcuDbHost[SYS_ENG_PAR_ELEMENT_DB_LEN];
	char  hcuDbUser[SYS_ENG_PAR_ELEMENT_DB_LEN];
	char  hcuDbPsw[SYS_ENG_PAR_ELEMENT_DB_LEN];
	char  hcuDbName[SYS_ENG_PAR_ELEMENT_DB_LEN];
	UINT8 hcuDbPort;
}SysEngParElementDbi_t;


//Timer setting for sensor data req by Shanchun
typedef struct SysEngParElementSensor
{
	INT32 emcReqTimer;
	INT32 emcReqTimerFB;
	INT32 humidReqTimer;
	INT32 humidReqTimerFB;
	INT32 noiseReqTimer;
	INT32 noiseReqTimerFB;
	INT32 pm25ReqTimer;
	INT32 pm25ReqTimerFB;
	INT32 tempReqTimer;
	INT32 tempReqTimerFB;
	INT32 winddirReqTimer;
	INT32 winddirReqTimerFB;
	INT32 windspdReqTimer;
	INT32 windspdReqTimerFB;
	INT32 cloudvelaHbTimer;
	INT32 cloudvelaHbBackTimer;
	INT32 nbiotcj188HbTimer;
	INT32 nbiotcj188HbBackTimer;
	INT32 nbiotqg376HbTimer;
	INT32 nbiotqg376HbBackTimer;
	INT32 cloudSocketHbTimer;
	INT32 dbVerReportTimer;
	INT32 hsmmpReqTimer;
	INT32 hsmmpCapDuration;
	INT32 hsmmpCapDurationFB;
	INT32 hsmmpRefRate;
	INT32 airprsReqTimer;
	INT32 co1ReqTimer;
	INT32 lightstrReqTimer;
	INT32 alcoholReqTimer;
	INT32 hchoReqTimer;
	INT32 toxicgasReqTimer;
	INT32 iwmReqTimer;
	INT32 ihmReqTimer;
	INT32 igmReqTimer;
	INT32 ipmReqTimer;
	INT32 pm25sharpReqTimer;
	INT32 syspmWorkingTimer;
	INT32 canitfWorkingTimer;
	//不通过数据库配置的参数区域
#if (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_AQYC_OBSOLETE_ID)
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_TEST_MODE_ID)
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_AQYCG10_335D_ID)
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_AQYCG20_RASBERRY_ID)
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_TBSWRG30_ID)
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_GQYBG40_ID)
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_CXILC_ID)
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_CXGLACM_ID)
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_NBIOT_LPM_CJ_ID)
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_NBIOT_HPM_QG_ID)
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFSC_CBU_ID)
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_OPWL_OTDR_ID)
//小技巧，不要这部分，以便加强编译检查
#else
	#error Un-correct constant definition
#endif
}SysEngParElementSensorTimer_t;

//Series port config by Shanchun
#define SYS_ENG_PAR_ELEMENT_SERIESPORT_LEN 20
typedef struct SysEngParElementSeriesPort
{
	UINT32 SeriesPortForModbus;
	UINT32 SeriesPortForGPS;
	UINT32 SeriesPortForPm25Sharp;
}SysEngParElementSeriesPort_t;

//后台服务器地址部分
#define SYS_ENG_PAR_ELEMENT_CLOUDVELA_LEN 256
#define SYS_ENG_PAR_ELEMENT_CLOUDVELA_NAME 20
typedef struct SysEngParElementCloudvela
{
	char  cloudHttpAddLocal[SYS_ENG_PAR_ELEMENT_CLOUDVELA_LEN];
	char  cloudSocketSrvAdd[SYS_ENG_PAR_ELEMENT_CLOUDVELA_LEN];
	char  cloudHttpAddSae[SYS_ENG_PAR_ELEMENT_CLOUDVELA_LEN];
	char  cloudHttpAddJd[SYS_ENG_PAR_ELEMENT_CLOUDVELA_LEN];
	char  cloudHttpAddWechat[SYS_ENG_PAR_ELEMENT_CLOUDVELA_LEN];
	char  cloudBhServerName[SYS_ENG_PAR_ELEMENT_CLOUDVELA_NAME];
	char  cloudBhHcuName[SYS_ENG_PAR_ELEMENT_CLOUDVELA_NAME];
	UINT8 cloudBhItfFrameStd;
	char  cloudFtpAdd[SYS_ENG_PAR_ELEMENT_CLOUDVELA_LEN];//for HCU SW FTP by shanchun
	char  cloudFtpUser[SYS_ENG_PAR_ELEMENT_CLOUDVELA_NAME];//for HCU SW FTP by shanchun
	char  cloudFtpPwd[SYS_ENG_PAR_ELEMENT_CLOUDVELA_NAME];//for HCU SW FTP by shanchun
	char  cloudFtpUserVideo[SYS_ENG_PAR_ELEMENT_CLOUDVELA_NAME];//for video FTP by upload shanchun
	char  cloudFtpPwdVideo[SYS_ENG_PAR_ELEMENT_CLOUDVELA_NAME];//for video FTP upload by shanchun
}SysEngParElementCloudvela_t;

//local SW storage address for HCU SW upgrade by shanchun
#define SYS_ENG_PAR_ELEMENT_SWDOWNLOAD_LEN 64
typedef struct SysEngParElementHcuSwDownload
{
	char  hcuSwDownloadDir[SYS_ENG_PAR_ELEMENT_SWDOWNLOAD_LEN];
	char  hcuSwActiveDir[SYS_ENG_PAR_ELEMENT_SWDOWNLOAD_LEN];
	char  hcuSwBackupDir[SYS_ENG_PAR_ELEMENT_SWDOWNLOAD_LEN];
}SysEngParElementHcuSwDownload_t;

//本地视频服务器的地址
#define SYS_ENG_PAR_ELEMENT_VIDEO_SERVER_LEN 64
typedef struct SysEngParElementVideoServer
{
	char  hcuVideoServerDir[SYS_ENG_PAR_ELEMENT_VIDEO_SERVER_LEN];
	char  hcuVideoServerHttp[SYS_ENG_PAR_ELEMENT_VIDEO_SERVER_LEN];
}SysEngParElementVideoServer_t;

//Debug采用完全的等级方式，并通过按位操作，详细定义在sysconfig.h中，不应该重复定义，放在这里只是为了参考方便
/*
#define TRACE_DEBUG_ALL_OFF 0 //全关
#define TRACE_DEBUG_INF_ON 1  //全开
#define TRACE_DEBUG_NOR_ON 2  //普通级
#define TRACE_DEBUG_IPT_ON 4  //重要级
#define TRACE_DEBUG_CRT_ON 8  //关键级
#define TRACE_DEBUG_FAT_ON 16 //致命级
*/

//TRACE采用黑白名单的方式
//关系表单，采用INDEX索引的方式，引导到相应的配置表中，进行详细定义
typedef struct SysEngParElementTraceModule
{
	UINT16 moduleId;
	char   moduleName[TASK_NAME_MAX_LENGTH];
	UINT8  moduleCtrFlag;
	UINT8  moduleToAllow;
	UINT8  moduleToRestrict;
	UINT8  moduleFromAllow;
	UINT8  moduleFromRestrict;
}SysEngParElementTraceModule_t;
typedef struct SysEngParElementTraceMsg
{
	UINT16 msgId;
	char   msgName[MSG_NAME_MAX_LENGTH];
	UINT8  msgCtrFlag;
	UINT8  msgAllow;
	UINT8  msgRestrict;
}SysEngParElementTraceMsg_t;
typedef struct SysEngParElementTrace
{
	SysEngParElementTraceModule_t mod[MAX_TASK_NUM_IN_ONE_HCU];
	SysEngParElementTraceMsg_t msg[MAX_MSGID_NUM_IN_ONE_TASK];
}SysEngParElementTrace_t;

//本地界面firefox的自动启动
#define SYS_ENG_PAR_ELEMENT_LOCAL_UI_PROG_LEN 20
#define SYS_ENG_PAR_ELEMENT_LOCAL_UI_ADD_LEN 128
#define SYS_ENG_PAR_ELEMENT_LOCAL_UI_OPTION_LEN 128
#define SYS_ENG_PAR_ELEMENT_UI_MAX_LEN SYS_ENG_PAR_ELEMENT_LOCAL_UI_PROG_LEN+SYS_ENG_PAR_ELEMENT_LOCAL_UI_ADD_LEN+SYS_ENG_PAR_ELEMENT_LOCAL_UI_OPTION_LEN+10
typedef struct SysEngParElementLocalUi
{
	UINT8 browselAutoStartUpFlag;
	char  browselProg[SYS_ENG_PAR_ELEMENT_LOCAL_UI_PROG_LEN];
	char  browselStartUpAddress[SYS_ENG_PAR_ELEMENT_LOCAL_UI_ADD_LEN];
	char  browselWorkingOption[SYS_ENG_PAR_ELEMENT_LOCAL_UI_OPTION_LEN];
}SysEngParElementLocalUi_t;

//由程序指定的配置参数区，不用到数据库中进行配置，以降低项目工程的复杂度
typedef struct SysEngParElementProgramCodeDefineFix
{
	//不通过数据库配置的参数区域
#if (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_AQYC_OBSOLETE_ID)
	UINT8 test;
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_TEST_MODE_ID)
	UINT8 test;
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_AQYCG10_335D_ID)
	UINT8 test;
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_AQYCG20_RASBERRY_ID)
	UINT8 test;
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_TBSWRG30_ID)
	UINT8 test;
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_GQYBG40_ID)
	UINT8 test;
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_CXILC_ID)
	UINT8 test;
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_CXGLACM_ID)
	UINT8 test;
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_NBIOT_LPM_CJ_ID)
	UINT8 test;
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_NBIOT_HPM_QG_ID)
	UINT8 test;
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFSC_CBU_ID)
	UINT8 test;
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_OPWL_OTDR_ID)
	UINT8 test;
//小技巧，不要这部分，以便加强编译检查
#else
	#error Un-correct constant definition
#endif
}SysEngParElementProgramCodeDefineFix_t;

//通用硬件固定位置的标签存储内容，由工厂生产过程中烧录器写入
typedef struct SysEngParElementHwBurnPhyIdAddr
{
	char  	equLable[20];
	UINT16 	hwType;
	UINT16 	hwPemId;
	UINT16 	swRelId;
	UINT16 	swVerId;
	UINT8  	swUpgradeFlag;
	UINT8 	swUpgPollId;
	UINT8 	bootIndex;
	UINT8 	bootAreaMax;   //32
	UINT32  facLoadAddr;
	UINT16	facLoadSwRel;
	UINT16	facLoadSwVer;
	UINT16 	facLoadCheckSum;
	UINT16  facLoadValid;
	UINT32	spare2;
	UINT32  bootLoad1Addr;
	UINT16 	bootLoad1RelId;
	UINT16 	bootLoad1VerId;
	UINT16 	bootLoad1CheckSum;
	UINT16  bootLoad1Valid;
	UINT32	spare3;   //32
	UINT32  bootLoad2Addr;
	UINT16 	bootLoad2RelId;
	UINT16 	bootLoad2VerId;
	UINT16 	bootLoad2CheckSum;
	UINT16  bootLoad2Valid;
	UINT32	spare4;
	UINT32  bootLoad3Addr;
	UINT16 	bootLoad3RelId;
	UINT16 	bootLoad3VerId;
	UINT16 	bootLoad3CheckSum;
	UINT16  bootLoad3Valid;
	UINT32  spare5;       //32
	UINT8   cipherKey[16];
	UINT8 	rsv[16];   //32
}SysEngParElementHwBurnPhyIdAddr_t;

//工程参数总控制表
#define SYS_ENG_PAR_PRJ_NAME_LEN 100  //保持跟数据库的一致性
typedef struct HcuSysEngParTable
{
	char prjname[SYS_ENG_PAR_PRJ_NAME_LEN];
	SysEngParElementComm_t comm;
	SysEngParElementDbi_t dbi;
	SysEngParElementSensorTimer_t timer;//by Shanchun
	SysEngParElementSeriesPort_t serialport;//by Shanchun
	SysEngParElementCloudvela_t cloud;
	SysEngParElementHcuSwDownload_t swDownload;//by Shanchun
	SysEngParElementVideoServer_t videoSev;
	UINT8 debugMode;
	UINT8 traceMode;
	SysEngParElementTrace_t traceList;
	SysEngParElementLocalUi_t localUI;
	SysEngParElementProgramCodeDefineFix_t codeDefineFix;
	SysEngParElementHwBurnPhyIdAddr_t hwBurnId;
}HcuSysEngParTablet_t;


//就SYSTEM PERFORMANCE中zHcuRunErrCnt参数表达的错误计数器，进行定期存储数据库，并通过界面展示出来，以方便调测与监控
typedef struct  sp_errcnt_data_element //
{
	UINT32 sid;
	UINT32 taskHcuvmEC;
	UINT32 taskHwinvEC;
	UINT32 taskModbusEC;
	UINT32 taskCloudvelaEC;
	UINT32 taskAvorionEC;
	UINT32 taskSpsvirgoEC;
	UINT32 taskHsmmpEC;
	UINT32 taskEmcEC;
	UINT32 taskPm25EC;
	UINT32 taskWinddirEC;
	UINT32 taskWindspdEC;
	UINT32 taskTempEC;
	UINT32 taskHumidEC;
	UINT32 taskNoiseEC;
	UINT32 taskPm25SharpEC; //by shanchun
	UINT32 timeStamp;
}sp_errcnt_data_element_t;

#endif /* L0COMVM_SYSENGPAR_H_ */
