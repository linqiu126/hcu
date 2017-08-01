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
}SysEngParElementComm_t;

//数据库部分
#define HCU_SYSENG_PAR_ELEMENT_DB_LEN 20
typedef struct SysEngParElementDb
{
	char  hcuDbHost[HCU_SYSENG_PAR_ELEMENT_DB_LEN];
	char  hcuDbUser[HCU_SYSENG_PAR_ELEMENT_DB_LEN];
	char  hcuDbPsw[HCU_SYSENG_PAR_ELEMENT_DB_LEN];
	char  hcuDbName[HCU_SYSENG_PAR_ELEMENT_DB_LEN];
	UINT8 hcuDbPort;
}SysEngParElementDbi_t;

//定时器长度定义
typedef struct SysEngParElementTimUnit
{
	INT16	id;
	char 	name[HCU_SYSDIM_TIMERID_NAME_LEN_MAX];
	UINT8 	gradunarity;
	UINT32 	dur;
}SysEngParElementTimUnit_t;
typedef struct SysEngParElementSensorTimer
{
	SysEngParElementTimUnit_t array[HCU_SYSDIM_TIMERID_NBR_MAX];
}SysEngParElementSensorTimer_t;

//Series port config by Shanchun
#define HCU_SYSENG_PAR_ELEMENT_SERIESPORT_LEN 20
typedef struct SysEngParElementSeriesPort
{
	UINT32 SeriesPortForModbus;
	UINT32 SeriesPortForGPS;
	UINT32 SeriesPortForPm25Sharp;
}SysEngParElementSeriesPort_t;

//后台服务器地址部分
#define HCU_SYSENG_PAR_CLOUD_ADDR_LEN_MAX 256
#define HCU_SYSENG_PAR_CLOUD_NAME_LEN_MAX 20
typedef struct SysEngParElementCloudvela
{
	char  svrAddrHttpLocal[HCU_SYSENG_PAR_CLOUD_ADDR_LEN_MAX];
	char  svrAddrHttpDefault[HCU_SYSENG_PAR_CLOUD_ADDR_LEN_MAX];
	char  svrAddrSocketipDefault[HCU_SYSENG_PAR_CLOUD_ADDR_LEN_MAX];
	char  svrNameDefault[HCU_SYSENG_PAR_CLOUD_NAME_LEN_MAX];
	UINT8 svrBhItfFrameStdDefault;
	char  svrAddrHttpHome[HCU_SYSENG_PAR_CLOUD_ADDR_LEN_MAX];
	char  svrAddrSocketipHome[HCU_SYSENG_PAR_CLOUD_ADDR_LEN_MAX];
	char  svrNameHome[HCU_SYSENG_PAR_CLOUD_NAME_LEN_MAX];
	UINT8 svrBhItfFrameStdHome;
	char  cloudFtpAdd[HCU_SYSENG_PAR_CLOUD_ADDR_LEN_MAX];//for HCU SW FTP by shanchun
	char  cloudFtpUser[HCU_SYSENG_PAR_CLOUD_NAME_LEN_MAX];//for HCU SW FTP by shanchun
	char  cloudFtpPwd[HCU_SYSENG_PAR_CLOUD_NAME_LEN_MAX];//for HCU SW FTP by shanchun
	char  cloudFtpUserVideo[HCU_SYSENG_PAR_CLOUD_NAME_LEN_MAX];//for video FTP by upload shanchun
	char  cloudFtpPwdVideo[HCU_SYSENG_PAR_CLOUD_NAME_LEN_MAX];//for video FTP upload by shanchun
}SysEngParElementCloudvela_t;

//local SW storage address for HCU SW upgrade by shanchun
#define HCU_SYSENG_PAR_ELEMENT_SWDOWNLOAD_LEN 64
typedef struct SysEngParElementHcuSwDownload
{
	char  hcuSwRunDir[HCU_SYSENG_PAR_ELEMENT_SWDOWNLOAD_LEN];
	char  hcuSwDownloadDir[HCU_SYSENG_PAR_ELEMENT_SWDOWNLOAD_LEN];
	char  hcuSwActiveDir[HCU_SYSENG_PAR_ELEMENT_SWDOWNLOAD_LEN];
	char  hcuSwBackupDir[HCU_SYSENG_PAR_ELEMENT_SWDOWNLOAD_LEN];
}SysEngParElementHcuSwDownload_t;

//本地视频服务器的地址
#define HCU_SYSENG_PAR_ELEMENT_VIDEO_SERVER_LEN 64
typedef struct SysEngParElementVideoServer
{
	char  hcuVideoServerDir[HCU_SYSENG_PAR_ELEMENT_VIDEO_SERVER_LEN];
	char  hcuVideoServerHttp[HCU_SYSENG_PAR_ELEMENT_VIDEO_SERVER_LEN];
}SysEngParElementVideoServer_t;

//TRACE采用黑白名单的方式
//关系表单，采用INDEX索引的方式，引导到相应的配置表中，进行详细定义
typedef struct SysEngParElementTraceModule
{
	UINT16 moduleId;
	char   moduleName[HCU_SYSDIM_TASK_NAME_LEN_MAX];
	UINT8  moduleCtrFlag;
	UINT8  moduleToAllow;
	UINT8  moduleToRestrict;
	UINT8  moduleFromAllow;
	UINT8  moduleFromRestrict;
}SysEngParElementTraceModule_t;
typedef struct SysEngParElementTraceMsg
{
	UINT16 msgId;
	char   msgName[HCU_SYSDIM_MSGID_NAME_LEN_MAX];
	UINT8  msgCtrFlag;
	UINT8  msgAllow;
	UINT8  msgRestrict;
}SysEngParElementTraceMsg_t;
typedef struct SysEngParElementTrace
{
	SysEngParElementTraceModule_t mod[HCU_SYSDIM_TASK_NBR_MAX];
	SysEngParElementTraceMsg_t msg[HCU_SYSDIM_MSGID_NBR_MAX];
}SysEngParElementTrace_t;

//本地界面firefox的自动启动
#define HCU_SYSENG_PAR_ELEMENT_LOCAL_UI_PROG_LEN 20
#define HCU_SYSENG_PAR_ELEMENT_LOCAL_UI_ADD_LEN 128
#define HCU_SYSENG_PAR_ELEMENT_LOCAL_UI_OPTION_LEN 128
#define HCU_SYSENG_PAR_ELEMENT_UI_MAX_LEN HCU_SYSENG_PAR_ELEMENT_LOCAL_UI_PROG_LEN+HCU_SYSENG_PAR_ELEMENT_LOCAL_UI_ADD_LEN+HCU_SYSENG_PAR_ELEMENT_LOCAL_UI_OPTION_LEN+10
typedef struct SysEngParElementLocalUi
{
	UINT8 browselAutoStartUpFlag;
	char  browselProg[HCU_SYSENG_PAR_ELEMENT_LOCAL_UI_PROG_LEN];
	char  browselStartUpAddress[HCU_SYSENG_PAR_ELEMENT_LOCAL_UI_ADD_LEN];
	char  browselWorkingOption[HCU_SYSENG_PAR_ELEMENT_LOCAL_UI_OPTION_LEN];
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
#define HCU_SYSENG_PAR_HWBURN_MAC_LEN_MAX 32
#define HCU_SYSENG_PAR_HWBURN_ZHBMN_LEN_MAX 15
typedef struct SysEngParElementHwBurnPhyIdAddr
{
	char  	equLable[HCU_SYSENG_PAR_CLOUD_NAME_LEN_MAX];
	char 	hw_mac[HCU_SYSENG_PAR_HWBURN_MAC_LEN_MAX];
	char 	zhbMnLable[HCU_SYSENG_PAR_HWBURN_ZHBMN_LEN_MAX];
	UINT16 	hwType;
	UINT16 	hwPemId;
	UINT16 	swRelId;
	UINT16 	swVerId;
	UINT16 	dbVerId;
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
	UINT16 	nodeHwType;
	UINT16 	nodeHwPemId;
}SysEngParElementHwBurnPhyIdAddr_t;




/*
 *
 *
 *   BFSC项目中的核心配置参数
 *
 *
 */
//typedef struct HcuSysEngBfscCombinationAlgorithmParamaters
//{
//	UINT32	MinScaleNumberCombination;				//组合搜索的最小Scale的个数
//	UINT32	MaxScaleNumberCombination;				//组合搜索的最大Scale的个数
//	UINT32	MinScaleNumberStartCombination;				//开始查找的最小个数，就是说大于这个个数就开始搜索
//	UINT32	TargetCombinationWeight;				//组合目标重量
//	UINT32	TargetCombinationUpperWeight;				//组合目标重量上限
//	UINT32	IsPriorityScaleEnabled;				// 1: Enable, 0: Disable
//	UINT32	IsProximitCombinationMode;				// 1: AboveUpperLimit, 2: BelowLowerLimit, 0: Disable
//	UINT32	CombinationBias;				//每个Scale要求放几个物品
//	UINT32	IsRemainDetectionEnable;				//Scale处于LAOD状态超过remainDetectionTimeS, 被认为是Remain状态，提示要将物品拿走: 1:Enable， 0：Disble
//	UINT32	RemainDetectionTimeSec;				// RemainDetionTime in Seconds
//	UINT32	RemainScaleTreatment;				// 1: Discharge (提示用户移走），0：Enforce（强制进行组合）
//	UINT32	CombinationSpeedMode;				// 0：SpeedPriority，1: PrecisePriority
//	UINT32	CombinationAutoMode;				// 0: Auto, 1: Manual
//	UINT32	MovingAvrageSpeedCount;				//计算平均速度的时候使用最近多少个组合做统计
//	UINT32	AlgSpare1;
//	UINT32	AlgSpare2;
//	UINT32	AlgSpare3;
//	UINT32	AlgSpare4;
//}HcuSysEngBfscCombinationAlgorithmParamaters_t;
//typedef struct HcuSysEngBfscWeightSensorFilterParameter
//{
//	UINT32	filer_parameter1;
//	UINT32	filer_parameter2;
//	UINT32	filer_parameter3;
//	UINT32	filer_parameter4;
//}HcuSysEngBfscWeightSensorFilterParameter_t;
//typedef struct HcuSysEngBfscWeightSensorParamaters
//{
//	UINT32	WeightSensorAdcParameter;
//	UINT32	WeightSensorFilterMode;
//	HcuSysEngBfscWeightSensorFilterParameter_t	FilterParam;	//裸重量滤波的参数 (TO BE COMPLETED)
//	UINT32	WeightSensorAutoZeroThread;
//	UINT32	WeightSensorFixCompesation;
//	UINT32	WeightSensorLoadDetectionTimeMs;		//称台稳定的判断时间
//	UINT32	WeightSensorLoadThread;
//	UINT32	WeightSensorEmptyThread;
//	UINT32	WeightSensorEmptyDetectionTimeMs;
//	UINT32	WeightSensorPickupThread;
//	UINT32	WeightSensorPickupDetectionTimeMs;
//	UINT32	StardardReadyTimeMs;
//	UINT32	MaxAllowedWeight;
//	UINT32	WeightSpare1;
//	UINT32	WeightSpare2;
//	UINT32	WeightSpare3;
//	UINT32	WeightSpare4;
//}HcuSysEngBfscWeightSensorParamaters_t;
//typedef struct HcuSysEngBfscMotorControlParamaters
//{
//	UINT32	MotorSpeed;
//	UINT32	MotorDirection;				//0: Clockwise; 1: Counter-Clockwise
//	UINT32	MotorRollingStartMs;				//how long do the motor rolling for start action
//	UINT32	MotorRollingStopMs;				//how long do the motor rolling for stop action
//	UINT32	MotorRollingInveralMs;				//If the motor is rolling, how long the motor will stay in still before roll back (stop action).
//	UINT32	MotorFailureDetectionVaration;				// % of the MotorSpeed
//	UINT32	MotorFailureDetectionTimeMs;				// within TimeMs, 如果速度都在外面，认为故障
//	UINT32	MotorSpare1;
//	UINT32	MotorSpare2;
//	UINT32	MotorSpare3;
//	UINT32	MotorSpare4;
//}HcuSysEngBfscMotorControlParamaters_t;
//typedef struct HcuSysEngBfscCfgpar
//{
//	UINT32 sid;   //可能有多套参数，缺省使用第一套
//	HcuSysEngBfscCombinationAlgorithmParamaters_t 	combAlg;
//	HcuSysEngBfscWeightSensorParamaters_t			wsPar;
//	HcuSysEngBfscMotorControlParamaters_t			motoPar;
//}HcuSysEngBfscCfgpar_t;


/*
 *
 *
 *   //工程参数总控制表
 *
 *
 */

#define HCU_SYSENG_PAR_PRJ_NAME_LEN 100  //保持跟数据库的一致性
typedef struct HcuSysEngParTab
{
	char prjname[HCU_SYSENG_PAR_PRJ_NAME_LEN];
	SysEngParElementComm_t 					comm;
	SysEngParElementDbi_t 					dbi;
	SysEngParElementSensorTimer_t 			timer;//by Shanchun
	SysEngParElementSeriesPort_t 			serialport;//by Shanchun
	SysEngParElementCloudvela_t 			cloud;
	SysEngParElementHcuSwDownload_t 		swm;//by Shanchun
	SysEngParElementVideoServer_t 			videoSev;
	UINT8 debugMode;
	UINT8 traceMode;
	SysEngParElementTrace_t 				traceList;
	SysEngParElementLocalUi_t 				localUI;
	SysEngParElementProgramCodeDefineFix_t 	codeDefineFix;
	SysEngParElementHwBurnPhyIdAddr_t 		hwBurnId;
}HcuSysEngParTab_t;


#endif /* L0COMVM_SYSENGPAR_H_ */
