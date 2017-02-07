/*
 * vmlayer.h
 *
 *  Created on: 2016年1月3日
 *      Author: hitpony
 */

#ifndef L0COMVM_VMLAYER_H_
#define L0COMVM_VMLAYER_H_

#include "../l0comvm/commsg.h"
#include "../l0comvm/sysdim.h"
#include "../l0comvm/sysconfig.h"
#include "../l0comvm/sysengpar.h"


/*
 *
 *   系统全局常量定义区
 *
 *
 *   定义TASK对应的名字//!!!!!!!!!!!!!!!!ATTENTION!!!!!!!!!!!!!!!!
 *   【增加任务】，必须同时修改四个地方：
 *   - HCU_TASK_NAME_ID
 *   - HCU_TASK_QUEUE_ID
 *   - zHcuGlobalTaskInputConfig
 *   - 将新模块包含在hcu.c中
 *   - 如果需要完美表现，还得最终需要升级L3UI的CRUD，不然相应的工具会出错
 *
 */
//由于内存不是太大的问题，这里将所有任务全部平铺直叙在一起，以简化不同项目的设置问题
//采用这种技巧的，还有MessageQueue，MessageId等内容，全部采用U32的方式
enum HCU_TASK_NAME_ID
{
	TASK_ID_MIN = 0,
	TASK_ID_HCUMAIN,
	TASK_ID_HCUVM,
	TASK_ID_TRACE,
	TASK_ID_CONFIG,
	TASK_ID_TIMER,
	TASK_ID_MMC, //Man Machine Communication
	TASK_ID_GPIO,
	TASK_ID_I2C,
	TASK_ID_SPI,
	TASK_ID_PWM,    //脉冲控制
	TASK_ID_ADC,    //模拟量通道
	TASK_ID_SWITCH, //开关量
	TASK_ID_RELAY,  //继电器
	TASK_ID_MOTOR,  //马达
	TASK_ID_ZEEGBE,
	TASK_ID_GPRS,
	TASK_ID_SPS232,
	TASK_ID_SPS485,
	TASK_ID_BLE,
	TASK_ID_ETHERNET,  //ETHERNET
	TASK_ID_WIFI,
	TASK_ID_USBNET,  //USB used for network connection
	TASK_ID_3G4G,
	TASK_ID_HARDDISK,
	TASK_ID_CAMERA,
	TASK_ID_MICROPHONE,
	TASK_ID_FLASH,
	TASK_ID_GPS,
	TASK_ID_LCD,
	TASK_ID_LED,
	TASK_ID_HWINV, //Hardware Inventory
	TASK_ID_SPSVIRGO,
	TASK_ID_CLOUDVELA,
	TASK_ID_MODBUS,
	TASK_ID_AVORION,
	TASK_ID_I2CBUSLIBRA,
	TASK_ID_SPIBUSARIES,
	TASK_ID_NBIOTCJ188,
	TASK_ID_NBIOTQG376,
	TASK_ID_HSMMP,  //High Speed MultiMedia processing
	TASK_ID_EMC,
	TASK_ID_HUMID,
	TASK_ID_PM25,
	TASK_ID_TEMP,
	TASK_ID_WINDDIR,
	TASK_ID_WINDSPD,
	TASK_ID_NOISE,
	TASK_ID_AIRPRS,  	//气压
	TASK_ID_CO1,     	//一氧化碳
	TASK_ID_LIGHTSTR,	//光照
	TASK_ID_ALCOHOL, 	//酒精
	TASK_ID_HCHO,    	//甲醛
	TASK_ID_TOXICGAS,	//有毒气体
	TASK_ID_IWM,  		//智能水表
	TASK_ID_IHM,  		//智能热表
	TASK_ID_IGM,  		//智能煤气表
	TASK_ID_IPM,  		//智能电表
	TASK_ID_SVRCON,
	TASK_ID_SYSPM,    	//性能统计
	TASK_ID_SYSSWM,  	//软件管理
	TASK_ID_PM25SHARP,  //for pm25sharp sensor by shanchun
	TASK_ID_CANITFLEO,
	TASK_ID_L3AQYCG10,
	TASK_ID_L3AQYCG20,
	TASK_ID_L3TBSWRG30,
	TASK_ID_L3GQYBG40,
	TASK_ID_L3CXILC,
	TASK_ID_L3CXGLACM,
	TASK_ID_L3NBLPM,
	TASK_ID_L3NBHPM,
	TASK_ID_L3BFSC,
	TASK_ID_BFSCUICOMM,
	TASK_ID_L3OPWLOTDR,
	TASK_ID_MAX,
	TASK_ID_INVALID = 0xFFFFFFFF,
}; //end of HCU_TASK_NAME_ID
//定义TASK对应的MESSAGE_QUEUE的ID
enum HCU_TASK_QUEUE_ID
{
	TASK_QUE_ID_MIN = HCU_TASK_QUEUE_ID_START,
	TASK_QUE_ID_HCUMAIN,
	TASK_QUE_ID_HCUVM,
	TASK_QUE_ID_TRACE,
	TASK_QUE_ID_CONFIG,
	TASK_QUE_ID_TIMER,
	TASK_QUE_ID_MMC, //Man Machine Communication
	TASK_QUE_ID_GPIO,
	TASK_QUE_ID_I2C,
	TASK_QUE_ID_SPI,
	TASK_QUE_ID_PWM,
	TASK_QUE_ID_ADC,
	TASK_QUE_ID_SWITCH, //开关量
	TASK_QUE_ID_RELAY,  //继电器
	TASK_QUE_ID_MOTOR,  //马达
	TASK_QUE_ID_ZEEGBE,
	TASK_QUE_ID_GRPS,
	TASK_QUE_ID_SPS232,
	TASK_QUE_ID_SPS485,
	TASK_QUE_ID_BLE,
	TASK_QUE_ID_ETHERNET,
	TASK_QUE_ID_WIFI,
	TASK_QUE_ID_USBNET,
	TASK_QUE_ID_3G4G,
	TASK_QUE_ID_HARDDISK,
	TASK_QUE_ID_CAMERA,
	TASK_QUE_ID_MICROPHONE,
	TASK_QUE_ID_FLASH,
	TASK_QUE_ID_GPS,
	TASK_QUE_ID_LCD,
	TASK_QUE_ID_LED,
	TASK_QUE_ID_HWINV,
	TASK_QUE_ID_SPSVIRGO,
	TASK_QUE_ID_CLOUDVELA,
	TASK_QUE_ID_MODBUS,
	TASK_QUE_ID_AVORION,
	TASK_QUE_ID_I2CBUSLIBRA,
	TASK_QUE_ID_SPIBUSARIES,
	TASK_QUE_ID_NBIOTCJ188,
	TASK_QUE_ID_NBIOTQG376,
	TASK_QUE_ID_HSMMP,
	TASK_QUE_ID_EMC,
	TASK_QUE_ID_HUMID,
	TASK_QUE_ID_PM25,
	TASK_QUE_ID_TEMP,
	TASK_QUE_ID_WINDDIR,
	TASK_QUE_ID_WINDSPD,
	TASK_QUE_ID_NOISE,
	TASK_QUE_ID_AIRPRS,  	//气压
	TASK_QUE_ID_CO1,     	//一氧化碳
	TASK_QUE_ID_LIGHTSTR,	//光照
	TASK_QUE_ID_ALCOHOL, 	//酒精
	TASK_QUE_ID_HCHO,    	//甲醛
	TASK_QUE_ID_TOXICGAS,	//有毒气体
	TASK_QUE_ID_IWM,  		//智能水表
	TASK_QUE_ID_IHM,  		//智能热表
	TASK_QUE_ID_IGM, 		//智能煤气表
	TASK_QUE_ID_IPM, 		//智能电表
	TASK_QUE_ID_SVRCON,
	TASK_QUE_ID_SYSPM,
	TASK_QUE_IID_SYSSWM,  	//软件管理
	TASK_QUE_ID_PM25SHARP, 	//for pm25sharp sensor by shanchun
	TASK_QUE_ID_CANITFLEO,
	TASK_QUE_ID_COM_BOTTOM,
	TASK_QUE_ID_L3AQYCG10,
	TASK_QUE_ID_L3AQYCG20,
	TASK_QUE_ID_L3TBSWRG40,
	TASK_QUE_ID_L3GQYBG40,
	TASK_QUE_ID_L3CXILC,
	TASK_QUE_ID_L3CXGLACM,
	TASK_QUE_ID_L3NBLPM,
	TASK_QUE_ID_L3NBHPM,
	TASK_QUE_ID_L3BFSC,
	TASK_QUE_ID_BFSCUICOMM,
	TASK_QUE_ID_L3OPWLOTDR,
	TASK_QUE_ID_MAX,
	TASK_QUE_ID_INVALID = 0xFFFFFFFF,
}; //end of HCU_TASK_QUEUE_ID



/*
 *
 *   三大表单之一： 系统总控表HcuVmCtrTab_t定义区
 *
 *
*/

/*
 *   1.1 Task & FSM
*/
#define FSM_STATE_ENTRY  0x00
#define FSM_STATE_IDLE  0x01
#define FSM_STATE_END   0xFE
#define FSM_STATE_INVALID 0xFF
//状态机表
typedef struct HcuFsmStateItem
{
	UINT32 msg_id;
	UINT8 state;
	OPSTAT (*stateFunc)(UINT32 dest_id, UINT32 src_id, void *param_ptr, UINT32 param_len);
}HcuFsmStateItem_t;
//状态机列表
typedef struct HcuFsmArrayElement
{
	OPSTAT (*stateFunc)(UINT32 dest_id, UINT32 src_id, void *param_ptr, UINT32 param_len);
}HcuFsmArrayElement_t;
//状态机控制表
typedef struct HcuFsmCtrlTable
{
	UINT32 numOfFsmArrayElement;  //每一个具体任务TASK中，定义了多少个STATE-MSGID映射表单
	UINT32 taskId;
	HcuFsmArrayElement_t pFsmArray[MAX_STATE_NUM_IN_ONE_TASK][MAX_MSGID_NUM_IN_ONE_TASK];
}HcuFsmCtrlTable_t;
//状态机总表
typedef struct HcuFsmTable
{
	UINT32 numOfFsmCtrlTable;  //Number of running (Task + Instance)
	UINT32 currentTaskId;  //transfer task_id to launched FSM machine, then useless
	HcuFsmCtrlTable_t  pFsmCtrlTable[MAX_TASK_NUM_IN_ONE_HCU];
}HcuFsmTable_t;
//任务表
typedef struct HcuTaskTag
{
	UINT32 		TaskId;
	UINT8  		pnpState;
	INT32  		processId;
	pthread_t 	ThrId;
	INT32  		QueId;
	UINT8  		state;
	char   		taskName[TASK_NAME_MAX_LENGTH];
	HcuFsmStateItem_t *fsmPtr;
	UINT8 		QueFullFlag;
	void*  		taskFuncEntry;
}HcuTaskTag_t;
#define HCU_TASK_QUEUE_FULL_NULL 0
#define HCU_TASK_QUEUE_FULL_TRUE 1
#define HCU_TASK_QUEUE_FULL_FALSE 2
#define HCU_TASK_QUEUE_FULL_INVALID 0xFF

/*
 *   1.2 Process
*/
//当前进程表
typedef struct HcuCurrentTaskTag
{
	UINT32 curProcId;
	char   curProcName[TASK_NAME_MAX_LENGTH];
}HcuCurrentTaskTag_t;

/*
 *   1.3 Hwinv
*/
//System level hardware inventory
#define HCU_HWINV_RES_NAME_MAX_LEN 10
#define HCU_HWINV_TYPE_NAME_MAX_LEN 20
#define HCU_HWINV_MANU_NAME_MAX_LEN 20
typedef struct HcuHwinvBaseElement
{
	UINT32 hwLogicId; //给每一个硬件资源命令一个独特的系统内部ID
	UINT32 taskId; //如果有对应的单独任务，则为任务ID标识，否则设置为0或者0xFFFFFFFF
	char hwResName[HCU_HWINV_RES_NAME_MAX_LEN];  //Description in sentence
	UINT8 hwStatus;
	UINT32 hwResistence;  //扫描其离线的周期数
	UINT32 hwType;
	char hwTypeName[HCU_HWINV_TYPE_NAME_MAX_LEN];  //Description in sentence
	UINT32 hwManu;
	char hwManufacture[HCU_HWINV_MANU_NAME_MAX_LEN]; //Description in sentence
}HcuHwinvBaseElement_t;

typedef struct HcuHwinvCpuElement
{
	HcuHwinvBaseElement_t hwBase;
	char cpuType[20];
}HcuHwinvCpuElement_t;
typedef struct HcuHwinvFlashElement
{
	HcuHwinvBaseElement_t hwBase;
	UINT32 hwVolume;  //in MB
}HcuHwinvFlashElement_t;
typedef struct HcuHwinvRtcElement
{
	HcuHwinvBaseElement_t hwBase;
}HcuHwinvRtcElement_t;
typedef struct HcuHwinvMemoryElement
{
	HcuHwinvBaseElement_t hwBase;
	UINT32 hwVolume;  //in MB
}HcuHwinvMemoryElement_t;
typedef struct HcuHwinvMainBoardElement
{
	HcuHwinvBaseElement_t hwBase;
}HcuHwinvMainBoardElement_t;
typedef struct HcuHwinvHardDiscElement
{
	HcuHwinvBaseElement_t hwBase;
	UINT32 hwVolume;  //in MB
}HcuHwinvHardDiscElement_t;
typedef struct HcuHwinvGpioElement
{
	HcuHwinvBaseElement_t hwBase;
	UINT32 number; //多少路GPIO
}HcuHwinvGpioElement_t;
typedef struct HcuHwinvSps232Element
{
	HcuHwinvBaseElement_t hwBase;
	UINT32 portNbr;
}HcuHwinvSps232Element_t;
typedef struct HcuHwinvSps485Element
{
	HcuHwinvBaseElement_t hwBase;
	UINT32 portNbr;
}HcuHwinvSps485Element_t;
typedef struct HcuHwinvEthernetElement
{
	HcuHwinvBaseElement_t hwBase;
	UINT8 speed;  //1-10M, 2-100M, 3-10/100M, 4-1G, 5-100/1G, 6-Others
	UINT8 macAddress[12];
	UINT8 ipv4Address[4];
	UINT8 ipv6Address[8];
}HcuHwinvEthernetElement_t;
typedef struct HcuHwinvUsbElement
{
	HcuHwinvBaseElement_t hwBase;
	UINT8 supportUTG2;  //YES-1, NO-0
	UINT8 speed; //1-USB2.0, 2-USB3.0, 3-USB2/3, 4-others
}HcuHwinvUsbElement_t;
typedef struct HcuHwinv3g4gElement
{
	HcuHwinvBaseElement_t hwBase;
	UINT8 speed;
	UINT8 operator; //1-CMCC, 2-CTC, 4-CU, 4-No, 5-others
	UINT8 simCardNbr[16];
	UINT8 macAddress[12];
	UINT8 ipv4Address[4];
	UINT8 ipv6Address[8];
}HcuHwinv3g4gElement_t;
typedef struct HcuHwinvWifiElement
{
	HcuHwinvBaseElement_t hwBase;
	UINT8 speed;
	UINT8 operator; //1-CMCC, 2-CTC, 4-CU, 4-No, 5-others
	UINT8 macAddress[12];
	UINT8 ipv4Address[4];
	UINT8 ipv6Address[8];
}HcuHwinvWifiElement_t;
typedef struct HcuHwinvGpsElement
{
	HcuHwinvBaseElement_t hwBase;
	UINT8 tech; //1-GPS, 2-GLONOSS, 3-BEIDU, 4-GALINIO, 5-others
}HcuHwinvGpsElement_t;
typedef struct HcuHwinvBleElement
{
	HcuHwinvBaseElement_t hwBase;
	UINT8 speed;
	UINT8 tech; //1-BLE4, 2-LowPower, 3-others
	UINT8 macAddress[12];
	UINT8 ipv4Address[4];
	UINT8 ipv6Address[8];
}HcuHwinvBleElement_t;
typedef struct HcuHwinvMicrophoneElement
{
	HcuHwinvBaseElement_t hwBase;
	UINT8 tech;
}HcuHwinvMicrophoneElement_t;
typedef struct HcuHwinvCameraElement
{
	HcuHwinvBaseElement_t hwBase;
	UINT8 tech;
	UINT8 resolution;  //1-50W, 2-60W, 3-80W, 4-100W, 5-200W, 6-300W, 7-400W, 8-500W, 9-600W, 10-800W, 11-1000W, 12-Others
}HcuHwinvCameraElement_t;
typedef struct HcuHwinvEmcElement
{
	HcuHwinvBaseElement_t hwBase;
	UINT32 emcEqupId;
}HcuHwinvEmcElement_t;
typedef struct HcuHwinvPm25Element
{
	HcuHwinvBaseElement_t hwBase;
	UINT32 pm25EqupId;
}HcuHwinvPm25Element_t;
typedef struct HcuHwinvWindDirElement
{
	HcuHwinvBaseElement_t hwBase;
	UINT32 windDirEqupId;
}HcuHwinvWindDirElement_t;
typedef struct HcuHwinvWindSpdElement
{
	HcuHwinvBaseElement_t hwBase;
	UINT32 windSpdEqupId;
}HcuHwinvWindSpdElement_t;
typedef struct HcuHwinvTempElement
{
	HcuHwinvBaseElement_t hwBase;
	UINT32 tempEqupId;
}HcuHwinvTempElement_t;
typedef struct HcuHwinvHumidElement
{
	HcuHwinvBaseElement_t hwBase;
	UINT32 humidEqupId;
}HcuHwinvHumidElement_t;
typedef struct HcuHwinvCo2Element
{
	HcuHwinvBaseElement_t hwBase;
	UINT32 co2EqupId;
}HcuHwinvCo2Element_t;
//All in one
typedef struct HcuHwinvCtrlTable
{
	HcuSysOsNetInfo_t 				sysInfo;
	HcuHwinvFlashElement_t 			flash;
	HcuHwinvRtcElement_t 			rtc;
	HcuHwinvMemoryElement_t 		memroy;
	HcuHwinvMainBoardElement_t 		mainBoard;
	HcuHwinvHardDiscElement_t 		hardDisc;
	HcuHwinvGpioElement_t 			gpio;
	HcuHwinvSps232Element_t 		sps232[4];
	HcuHwinvSps485Element_t			sps485;
	HcuHwinvEthernetElement_t		ethernet;
	HcuHwinvUsbElement_t			usbnet;
	HcuHwinv3g4gElement_t			g3g4;
	HcuHwinvWifiElement_t			wifi;
	HcuHwinvGpsElement_t			gps;
	HcuHwinvBleElement_t			ble;
	HcuHwinvMicrophoneElement_t		microphone;
	HcuHwinvCameraElement_t			camera;
	HcuHwinvUsbElement_t			usb;
	HcuHwinvEmcElement_t			emc;
	HcuHwinvPm25Element_t			pm25;
	HcuHwinvWindDirElement_t		windDir;
	HcuHwinvWindSpdElement_t		windSpd;
	HcuHwinvTempElement_t			temp;
	HcuHwinvHumidElement_t			humid;
	HcuHwinvCo2Element_t			co2;
}HcuHwinvCtrlTable_t;

#define FILE_LENGTH_RECORD_MAX 1024  			//一条存入文件的记录最长长度
#define HCU_DIR_LENGTH_MAX 						HCU_FILE_NAME_LENGTH_MAX
#define HCU_RECORD_LOG_DIR_NAME_LOCAL			"./log"
#define HCU_RECORD_LOG_DIR_NAME_CLEAN 			"/log"
#define HCU_RECORD_FILE_NAME_SENSOR    			"/sensor"
#define HCU_RECORD_FILE_NAME_SENSOR_CLEAN   	"sensor"
#define HCU_RECORD_FILE_NAME_MICROPHONE    		"/microphone"
#define HCU_RECORD_FILE_NAME_MICROPHONE_CLEAN	"microhpone"
#define HCU_RECORD_FILE_NAME_AVORION    		"/av"
#define HCU_RECORD_FILE_NAME_AVORION_CLEAN		"av"
#define HCU_RECORD_FILE_NAME_EXTEND				".dat"
#define HCU_RECORD_FILE_NAME_EXTEND_AVI 		".avi"
#define HCU_RECORD_FILE_NAME_EXTEND_H264 		".h264"
#define HCU_RECORD_FILE_NAME_EXTEND_MKV			".mkv"
#define HCU_HARDDISK_TRESHOLD 10

/*
 *   1.4 TimeDate
*/
//时间全局变量
typedef struct HcuTimeDateTable
{
	UINT16 year;
	UINT8 mon;
	UINT8 day;
	UINT8 hour;
	UINT8 min;
	UINT8 sec;
	UINT16 grad;  //网格in Minutes
	UINT8 dayChange;  //如果改变了，则设置该标识位，否则一直为FALSE
	UINT8 monChange;  //如果改变了，则设置该标识位，否则一直为FALSE
	char sYear[5];		//"2015"
	char strYear[7]; 	//"./2015"
	char sMon[7]; 		//"201510"
	char strMon[9]; 	//"./201510"
	char sDay[9]; 		//"20151021"
	char strDay[11]; 	//"./20151021"
	char sHour[11]; 	//"2015102119"
	char strHour[13]; 	//"./2015102119"
	char sMin[13]; 		//"201510211955";
	char strMin[15]; 	//"./201510211955";
	char sSec[15];  	//"20151021195502";
	char strSec[17];   //"./20151021195502";
	char curHcuDir[HCU_DIR_LENGTH_MAX];
	char curLogDir[HCU_DIR_LENGTH_MAX];
	char curYmDir[HCU_DIR_LENGTH_MAX];
	char curSensorFd[HCU_DIR_LENGTH_MAX];
	char curMicrophoneFd[HCU_DIR_LENGTH_MAX];
	char curAvorionFdAvi[HCU_DIR_LENGTH_MAX];  //完整的文件名，带目录结构的
	char curAvorionFnameAvi[HCU_DIR_LENGTH_MAX];  //为了处理方便，只有文件信息，并且不带"/"标示
	char curAvorionFdH264[HCU_DIR_LENGTH_MAX];
	char curAvorionFnameH264[HCU_DIR_LENGTH_MAX];
	char curAvorionFdMkv[HCU_DIR_LENGTH_MAX];
	char curAvorionFnameMkv[HCU_DIR_LENGTH_MAX];
}HcuTimeDateTable_t;

//三表之一的系统总控表
typedef struct HcuVmCtrTab
{
	HcuFsmTable_t  			fsm;
	HcuTaskTag_t			task[MAX_TASK_NUM_IN_ONE_HCU];
	HcuCurrentTaskTag_t  	process;
	HcuHwinvCtrlTable_t		hwinv;
	HcuTimeDateTable_t		clock;
}HcuVmCtrTab_t;
//系统总控表
extern HcuVmCtrTab_t zHcuVmCtrTab;

//统一定义，即便任务文件没有被包含，也不会影响编译
extern HcuFsmStateItem_t HcuFsmHcuvm[];                             //状态机
extern HcuFsmStateItem_t HcuFsmTrace[];                             //状态机
extern HcuFsmStateItem_t HcuFsmConfig[];                            //状态机
extern HcuFsmStateItem_t HcuFsmTimer[];                             //状态机
extern HcuFsmStateItem_t HcuFsmMmc[];                               //状态机
extern HcuFsmStateItem_t HcuFsmGpio[];                              //状态机
extern HcuFsmStateItem_t HcuFsmI2c[];                               //状态机
extern HcuFsmStateItem_t HcuFsmSpi[];                               //状态机
extern HcuFsmStateItem_t HcuFsmPwm[];                               //状态机
extern HcuFsmStateItem_t HcuFsmAdc[];                               //状态机
extern HcuFsmStateItem_t HcuFsmSwitch[];                            //状态机
extern HcuFsmStateItem_t HcuFsmRelay[];                             //状态机
extern HcuFsmStateItem_t HcuFsmMotor[];                             //状态机
extern HcuFsmStateItem_t HcuFsmZeegbe[];                            //状态机
extern HcuFsmStateItem_t HcuFsmGprs[];                              //状态机
extern HcuFsmStateItem_t HcuFsmSps232[];                            //状态机
extern HcuFsmStateItem_t HcuFsmSps485[];                            //状态机
extern HcuFsmStateItem_t HcuFsmBle[];                               //状态机
extern HcuFsmStateItem_t HcuFsmEthernet[];                          //状态机
extern HcuFsmStateItem_t HcuFsmWifi[];                              //状态机
extern HcuFsmStateItem_t HcuFsmUsbnet[];                            //状态机
extern HcuFsmStateItem_t HcuFsm3g4g[];                              //状态机
extern HcuFsmStateItem_t HcuFsmHarddisk[];                          //状态机
extern HcuFsmStateItem_t HcuFsmCamera[];                            //状态机
extern HcuFsmStateItem_t HcuFsmMicrophone[];                        //状态机
extern HcuFsmStateItem_t HcuFsmFlash[];                             //状态机
extern HcuFsmStateItem_t HcuFsmGps[];                               //状态机
extern HcuFsmStateItem_t HcuFsmLcd[];                               //状态机
extern HcuFsmStateItem_t HcuFsmLed[];                               //状态机
extern HcuFsmStateItem_t HcuFsmHwinv[];                             //状态机
extern HcuFsmStateItem_t HcuFsmSpsvirgo[];                          //状态机
extern HcuFsmStateItem_t HcuFsmCloudvela[];                         //状态机
extern HcuFsmStateItem_t HcuFsmModbus[];                            //状态机
extern HcuFsmStateItem_t HcuFsmAvorion[];                           //状态机
extern HcuFsmStateItem_t HcuFsmI2cbuslibra[];                       //状态机
extern HcuFsmStateItem_t HcuFsmSpibusaries[];                       //状态机
extern HcuFsmStateItem_t HcuFsmNbiotcj188[];                        //状态机
extern HcuFsmStateItem_t HcuFsmNbiotqg376[];                        //状态机
extern HcuFsmStateItem_t HcuFsmHsmmp[];                             //状态机
extern HcuFsmStateItem_t HcuFsmEmc[];                               //状态机
extern HcuFsmStateItem_t HcuFsmHumid[];                             //状态机
extern HcuFsmStateItem_t HcuFsmPm25[];                              //状态机
extern HcuFsmStateItem_t HcuFsmTemp[];                              //状态机
extern HcuFsmStateItem_t HcuFsmWinddir[];                           //状态机
extern HcuFsmStateItem_t HcuFsmWindspd[];                           //状态机
extern HcuFsmStateItem_t HcuFsmNoise[];                             //状态机
extern HcuFsmStateItem_t HcuFsmAirprs[];                            //状态机
extern HcuFsmStateItem_t HcuFsmCo1[];                               //状态机
extern HcuFsmStateItem_t HcuFsmLightstr[];                          //状态机
extern HcuFsmStateItem_t HcuFsmAlcohol[];                           //状态机
extern HcuFsmStateItem_t HcuFsmHcho[];                              //状态机
extern HcuFsmStateItem_t HcuFsmToxicgas[];                          //状态机
extern HcuFsmStateItem_t HcuFsmIwm[];                               //状态机
extern HcuFsmStateItem_t HcuFsmIhm[];                               //状态机
extern HcuFsmStateItem_t HcuFsmIgm[];                               //状态机
extern HcuFsmStateItem_t HcuFsmIpm[];                               //状态机
extern HcuFsmStateItem_t HcuFsmSvrcon[];                            //状态机
extern HcuFsmStateItem_t HcuFsmSyspm[];                             //状态机
extern HcuFsmStateItem_t HcuFsmSysswm[];                            //状态机
extern HcuFsmStateItem_t HcuFsmPm25sharp[];                         //状态机
extern HcuFsmStateItem_t HcuFsmCanitfleo[];                         //状态机
extern HcuFsmStateItem_t HcuFsmL3aqycg10[];                         //状态机
extern HcuFsmStateItem_t HcuFsmL3aqycg20[];                         //状态机
extern HcuFsmStateItem_t HcuFsmL3tbswrg30[];                        //状态机
extern HcuFsmStateItem_t HcuFsmLgqgybg40[];                         //状态机
extern HcuFsmStateItem_t HcuFsmL3cxilc[];                           //状态机
extern HcuFsmStateItem_t HcuFsmL3cxglacm[];                         //状态机
extern HcuFsmStateItem_t HcuFsmL3nblpm[];                           //状态机
extern HcuFsmStateItem_t HcuFsmL3nbhpm[];                           //状态机
extern HcuFsmStateItem_t HcuFsmL3bfsc[];                            //状态机
extern HcuFsmStateItem_t HcuFsmBfscuicomm[];                        //状态机
extern HcuFsmStateItem_t HcuFsmL3opwlotdr[];                        //状态机

//任务配置的基础配置信息
typedef struct HcuVmCtrTaskStaticCfg
{
	const UINT8 taskInputId;
	const char  taskInputName[TASK_NAME_MAX_LENGTH];
	      void* fsmFuncEntry;
	const UINT8 traceCtrFlag;
	const UINT8 traceModToAllowFlag;
	const UINT8 traceModToRestrictFlag;
	const UINT8 traceModFromAllowFlag;
	const UINT8 traceModFromRestrictFlag;
}HcuVmCtrTaskStaticCfg_t;
extern HcuVmCtrTaskStaticCfg_t zHcuVmCtrTaskStaticCfg[];


/*
 *
 *   三大表单之二： 系统工参表HcuSysEngPar_t定义区
 *
 *
*/
//全局工程参数控制表
extern HcuSysEngParTab_t zHcuSysEngPar;


//消息初始化的数据表单
typedef struct HcuSysEngTrcMsgCtrStaticCfg
{
	const UINT32 msgId;
	const char   msgName[MSG_NAME_MAX_LENGTH];
	const UINT8 traceCtrFlag;
	const UINT8 traceMsgAllowFlag;
	const UINT8 traceMsgRestrictFlag;
}HcuSysEngTrcMsgCtrStaticCfg_t;
extern HcuSysEngTrcMsgCtrStaticCfg_t zHcuSysEngTrcMsgCtrStaticCfg[];  //MAX_MSGID_NUM_IN_ONE_TASK

//物理BOOT区域结构
typedef struct HcuSysEngPhyBootCfg
{
	const UINT8 level;
	const char left[30];
	const char right[30];
}HcuSysEngPhyBootCfg_t;

/*
 *
 *   三大表单之三： 系统运行统计性能表HcuSysStaPm_t定义区
 *
 *
*/

//任务模块RESTART的一些全局定义
#define HCU_RUN_ERROR_LEVEL_0_WARNING 10
#define HCU_RUN_ERROR_LEVEL_1_MINOR 100
#define HCU_RUN_ERROR_LEVEL_2_MAJOR 1000
#define HCU_RUN_ERROR_LEVEL_3_CRITICAL 10000
#define HCU_RUN_ERROR_LEVEL_4_DEAD 100000

//全局Counter，用于性能指标统计之用
typedef struct HcuGlobalCounter
{
	UINT32 errCnt[MAX_TASK_NUM_IN_ONE_HCU];  //以每个任务为单位
	UINT32 restartCnt;
	UINT32 cloudVelaConnCnt;
	UINT32 cloudVelaConnFailCnt;
	UINT32 cloudVelaDiscCnt;
	UINT32 CloudDataTimeOutCnt;
	UINT32 SocketDiscCnt;
	UINT32 cpu_occupy;
	UINT32 mem_occupy;
	UINT32 disk_occupy;
}HcuGlobalCounter_t;
extern HcuGlobalCounter_t zHcuGlobalCounter;

//三大表之三：系统统计性能表
typedef struct HcuSysStaPm
{
	HcuGlobalCounter_t 	statisCnt;
	UINT32 				taskRunErrCnt[MAX_TASK_NUM_IN_ONE_HCU];
}HcuSysStaPm_t;

//全局性能统计表
extern HcuSysStaPm_t		zHcuSysStaPm;



/*
 *
 *   通信机制核心API，对外提供的基础服务
 *
 *
*/

//Global VM layer basic API and functions
extern void hcu_sw_restart(void);  //软件状态机初始化, TBD
extern OPSTAT hcu_task_create(UINT32 task_id, void *(*task_func)(void *), void *arg, int prio);
extern OPSTAT hcu_task_delete(UINT32 task_id);
extern OPSTAT hcu_msgque_create(UINT32 task_id);
extern OPSTAT hcu_msgque_delete(UINT32 task_id);
UINT32 hcu_msgque_inquery(UINT32 task_id);
extern OPSTAT hcu_msgque_resync(void);
extern OPSTAT hcu_task_create_and_run(UINT32 task_id, HcuFsmStateItem_t* pFsmStateItem);
extern OPSTAT hcu_message_send(UINT32 msg_id, UINT32 dest_id, UINT32 src_id, void *param_ptr, UINT32 param_len); //message send
extern OPSTAT hcu_message_rcv(UINT32 dest_id, HcuMsgSruct_t *msg);
extern UINT32 hcu_message_rcv_no_wait(UINT32 dest_id, HcuMsgSruct_t *msg);
extern OPSTAT taskid_to_string(UINT32 id, char *string);
extern OPSTAT msgid_to_string(UINT32 id, char *string);
extern OPSTAT hcu_timer_polling(time_t sec, UINT32 nsec, void *handler());
extern OPSTAT hcu_timer_set(UINT32 timerid, UINT32 taskid, UINT32 delay);
extern OPSTAT hcu_timer_clear(UINT32 timerid, UINT32 taskid);
extern void hcu_sleep(UINT32 second);
extern void hcu_usleep(UINT32 usecond);  //resulution 10^(-6)s = 1 microsecond
//UNIX下时钟函数非常丰富，这里不再做任何抽象化，上层应用可以直接调用系统库函数进行使用和处理
extern UINT16 hcu_CRC_16(unsigned char *data,int len);


/*
 *
 *   状态机核心API，对外提供的基础服务
 *
 *
*/

/*FSM related APIs */
extern UINT32 FsmInit(void);
extern UINT32 FsmAddNew(UINT32 task_id, HcuFsmStateItem_t* pFsmStateItem);
extern UINT32 FsmRemove(UINT32 task_id);
extern UINT32 FsmRunEngine(UINT32 msg_id, UINT32 dest_id, UINT32 src_id, void *param_ptr, UINT32 param_len);
extern UINT32 FsmProcessingLaunch(void);
extern OPSTAT FsmSetState(UINT32 task_id, UINT8 newState);
extern UINT8  FsmGetState(UINT32 task_id);
extern OPSTAT fsm_com_do_nothing(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_com_heart_beat_rcv(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);

/*
 *
 *   HCU初始化核心API，本地私有使用
 *
 *
*/

//移植并来自HCU主入口模块
extern int hcu_vm_main_entry(void);
//VM初始化
OPSTAT hcu_vm_system_ctr_table_init(void);  //系统级别的初始化
OPSTAT hcu_vm_application_task_env_init();
//不同WORK MODE下进程的启动函数
void hcu_vm_working_mode_single_process_start(void);
void hcu_vm_working_mode_double_process_start(void);
void hcu_vm_working_mode_multipy_process_start(void);
//进程启动函数主入口
void hcu_vm_process_single_mainapp_entry(void);
//对于双进程及多进程模式，还需要再仔细考量，目前暂不推荐使用，而且也没有经过仔细测试
void hcu_vm_process_double_mainapp_entry(void);
void hcu_vm_process_multipy_mainapp_entry(void);
void hcu_vm_process_create_sensor_avorion_only(void);
void hcu_vm_process_multipy_entry_supervisor(void);
//VM本地函数
OPSTAT hcu_vm_system_task_init_call(UINT32 task_id, HcuFsmStateItem_t *p);
void hcu_vm_task_send_init_to_timer(void);
void hcu_vm_task_send_init_to_avorion(void);
void hcu_vm_task_send_init_to_svrcon(void);
void hcu_vm_task_send_hb_to_svrcon(void);
void hcu_vm_task_create_all(void);
void hcu_vm_task_create_all_but_avorion(void);
void hcu_vm_task_create_hcumain_env(void);
void hcu_vm_task_delete_all_and_queue(void);
void hcu_vm_task_delete_except_avorion_and_hcumain(void);
void hcu_vm_task_delete_except_timer_and_hcumain(void);
void hcu_vm_task_delete_except_svrcon_and_hcumain(void);

//获取系统设备唯一标示区的数据。由于敏感性，这块数据不能做成工程参数配置方式，而必须采用工厂烧录方式
OPSTAT hcu_vm_engpar_get_phy_burn_block_data(void);
OPSTAT hcu_vm_engpar_read_phy_boot_cfg(void);
void   hcu_vm_engpar_translate_phy_boot_cfg_into_mem(char *pRecord, int index, UINT8 *target);


/*
 *
 *  外部参考API
 *
 *
*/

//外部参考API, Extern APIs
extern int msgget(key_t key, int msgflg);
extern int msgctl(int msqid, int cmd, struct msqid_ds *buf);
extern int msgsnd(int msqid, const void *msgp, size_t msgsz, int msgflg);
extern ssize_t msgrcv(int msqid, void *msgp, size_t msgsz, long msgtyp, int msgflg);
extern OPSTAT hcu_hwinv_engpar_read_pop_data_into_mem(void);
extern OPSTAT hcu_hwinv_engpar_create_storage_dir_env(void);
extern OPSTAT hcu_hwinv_engpar_read_mac_address(void);
extern OPSTAT dbi_HcuTraceModuleCtr_engpar_intelligence_init(void);
extern OPSTAT dbi_HcuTraceMsgCtr_engpar_intelligence_init(void);

/*
 *
 *  提高效率的宏定义API部分
 *
 *
*/

//高级错误打印方式
#define HCU_ERROR_PRINT_TASK(taskid, ...)	do{zHcuSysStaPm.taskRunErrCnt[taskid]++;  HcuErrorPrint(__VA_ARGS__);  return FAILURE;}while(0)

//高级定义，简化程序的可读性
#define HCU_DEBUG_PRINT_INF		if ((zHcuSysEngPar.debugMode & HCU_TRACE_DEBUG_INF_ON) != FALSE) HcuDebugPrint
#define HCU_DEBUG_PRINT_NOR		if ((zHcuSysEngPar.debugMode & HCU_TRACE_DEBUG_NOR_ON) != FALSE) HcuDebugPrint
#define HCU_DEBUG_PRINT_IPT		if ((zHcuSysEngPar.debugMode & HCU_TRACE_DEBUG_IPT_ON) != FALSE) HcuDebugPrint
#define HCU_DEBUG_PRINT_CRT		if ((zHcuSysEngPar.debugMode & HCU_TRACE_DEBUG_CRT_ON) != FALSE) HcuDebugPrint
#define HCU_DEBUG_PRINT_FAT		if ((zHcuSysEngPar.debugMode & HCU_TRACE_DEBUG_FAT_ON) != FALSE) HcuDebugPrint



#endif /* L0COMVM_VMLAYER_H_ */
