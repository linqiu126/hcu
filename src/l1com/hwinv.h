/*
 * hwinv.h
 *
 *  Created on: 2016年1月3日
 *      Author: hitpony
 */

#ifndef L1COM_HWINV_H_
#define L1COM_HWINV_H_

#include "../hcu.h"
#include "../l0comvm/vmlayer.h"
#include "../l0service/sysinfo.h"

//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
enum FSM_STATE_HWINV
{
	FSM_STATE_HWINV_INITED = 0x02,
	FSM_STATE_HWINV_ACTIVED,  //所有人机命令在此状态下直接送达各个线程任务模块
	FSM_STATE_HWINV_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

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
	SysInfo_t 						sysInfo;
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

#define FILE_LENGTH_RECORD_MAX 1024  //一条存入文件的记录最长长度
#define HCU_DIR_LENGTH_MAX HCU_FILE_NAME_LENGTH_MAX
#define HCU_RECORD_LOG_DIR_NAME_LOCAL			"./log"
#define HCU_RECORD_LOG_DIR_NAME_CLEAN 			"/log"
#define HCU_RECORD_FILE_NAME_SENSOR    			"/sensor"
#define HCU_RECORD_FILE_NAME_SENSOR_CLEAN   	"sensor"
#define HCU_RECORD_FILE_NAME_MICROPHONE    		"/microphone"
#define HCU_RECORD_FILE_NAME_MICROPHONE_CLEAN	"microhpone"
#define HCU_RECORD_FILE_NAME_AVORION    		"/avorion"
#define HCU_RECORD_FILE_NAME_AVORION_CLEAN		"avorion"
#define HCU_RECORD_FILE_NAME_EXTEND				".dat"
#define HCU_RECORD_FILE_NAME_EXTEND_AVI 		".avi"
#define HCU_RECORD_FILE_NAME_EXTEND_H264 		".h264"
#define HCU_RECORD_FILE_NAME_EXTEND_MKV			".mkv"

//时间全局变量
typedef struct zHcuTimeDateTable
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
}zHcuTimeDateTable_t;

//Global variables
extern HcuSysEngParTablet_t zHcuSysEngPar; //全局工程参数控制表
extern FsmStateItem_t FsmHwinv[];
extern HcuHwinvCtrlTable_t zHcuHwinvTable;
extern zHcuTimeDateTable_t zCurTimeDate;

//API
extern OPSTAT fsm_hwinv_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_hwinv_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_hwinv_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT func_hwinv_global_par_init(void);
extern OPSTAT hcu_hwinv_read_engineering_data_into_mem(void);
extern void func_hwinv_copy_right(void);
extern void func_hwinv_scan_all(void);
extern void func_hwinv_scan_date();
extern void func_hwinv_scan_sysinfo(void);
extern void func_hwinv_scan_flash(void);
extern void func_hwinv_scan_rtc(void);
extern void func_hwinv_scan_memroy(void);
extern void func_hwinv_scan_main_board(void);
extern void func_hwinv_scan_hard_disc(void);
extern void func_hwinv_scan_gpio(void);
extern void func_hwinv_scan_sps232(void);
extern void func_hwinv_scan_sps485(void);
extern void func_hwinv_scan_ethernet(void);
extern void func_hwinv_scan_usbnet(void);
extern void func_hwinv_scan_3g4g(void);
extern void func_hwinv_scan_wifi(void);
extern void func_hwinv_scan_usb(void);
extern void func_hwinv_scan_microphone(void);
extern void func_hwinv_scan_camera(void);
extern void func_hwinv_scan_eng_par(void);
extern void func_hwinv_scan_dir(void);
extern void func_hwinv_scan_db(void);
extern void func_hwinv_scan_emc(void);
extern void func_hwinv_scan_pm25(void);
extern void func_hwinv_scan_winddir(void);
extern void func_hwinv_scan_windspd(void);
extern void func_hwinv_scan_temp(void);
extern void func_hwinv_scan_humid(void);
extern void func_hwinv_scan_airprs(void);
extern void func_hwinv_scan_co1(void);
extern void func_hwinv_scan_lightstr(void);
extern void func_hwinv_scan_alcohol(void);
extern void func_hwinv_scan_hcho(void);
extern void func_hwinv_scan_toxicgas(void);
extern void func_hwinv_scan_pm25sharp(void);
extern void func_hwinv_scan_local_ui();
extern void func_hwinv_scan_message_queue(void);

//External APIs
extern UINT32 hcu_disk_write(UINT32 fId, void *dataBuffer, UINT32 dataLen);
extern OPSTAT dbi_HcuEmcDataInfo_save(sensor_emc_data_element_t *emcData);
extern OPSTAT dbi_HcuSysEngPar_inqury(HcuSysEngParTablet_t *engPar, char *prjname);
extern OPSTAT dbi_HcuEmcDataInfo_delete_3monold(UINT32 days);
extern OPSTAT dbi_HcuPm25DataInfo_delete_3monold(UINT32 days);
extern OPSTAT dbi_HcuPm25SharpDataInfo_delete_3monold(UINT32 days);
extern OPSTAT dbi_HcuWinddirDataInfo_delete_3monold(UINT32 days);
extern OPSTAT dbi_HcuWindspdDataInfo_delete_3monold(UINT32 days);
extern OPSTAT dbi_HcuHumidDataInfo_delete_3monold(UINT32 days);
extern OPSTAT dbi_HcuHumidDht11DataInfo_delete_3monold(UINT32 days);
extern OPSTAT dbi_HcuHumidSht20DataInfo_delete_3monold(UINT32 days);
extern OPSTAT dbi_HcuHumidRht03DataInfo_delete_3monold(UINT32 days);
extern OPSTAT dbi_HcuTempDataInfo_delete_3monold(UINT32 days);
extern OPSTAT dbi_HcuTempDht11DataInfo_delete_3monold(UINT32 days);
extern OPSTAT dbi_HcuTempSht20DataInfo_delete_3monold(UINT32 days);
extern OPSTAT dbi_HcuTempRht03DataInfo_delete_3monold(UINT32 days);
extern OPSTAT dbi_HcuNoiseDataInfo_delete_3monold(UINT32 days);
extern OPSTAT dbi_HcuHsmmpDataInfo_delete_3monold(UINT32 days);
extern OPSTAT dbi_HcuTraceModuleCtr_inqury(HcuSysEngParTablet_t *engPar);
extern OPSTAT dbi_HcuTraceMsgCtr_inqury(HcuSysEngParTablet_t *engPar);
extern OPSTAT dbi_HcuSpErrcntDataInfo_save(void);
extern OPSTAT dbi_HcuSpErrcntDataInfo_delete_3monold(UINT32 days);
extern OPSTAT dbi_HcuAirprsDataInfo_delete_3monold(UINT32 days);
extern OPSTAT dbi_HcuCo1DataInfo_delete_3monold(UINT32 days);
extern OPSTAT dbi_HcuLightstrDataInfo_delete_3monold(UINT32 days);
extern OPSTAT dbi_HcuAlcoholDataInfo_delete_3monold(UINT32 days);
extern OPSTAT dbi_HcuHchoDataInfo_delete_3monold(UINT32 days);
extern OPSTAT dbi_HcuToxicgasDataInfo_delete_3monold(UINT32 days);
extern OPSTAT dbi_HcuSyspmGlobalDataInfo_delete_3monold(UINT32 days);
extern OPSTAT dbi_HcuLightstrBh1750DataInfo_delete_3monold(UINT32 days);
extern OPSTAT dbi_HcuAirprsBmp180DataInfo_delete_3monold(UINT32 days);
extern OPSTAT dbi_HcuToxicgasMq135DataInfo_delete_3monold(UINT32 days);
extern OPSTAT dbi_HcuTempBmp180DataInfo_delete_3monold(UINT32 days);
extern OPSTAT dbi_HcuAirprsAltitudeBmp180DataInfo_delete_3monold(UINT32 days);

//create video server directory by Shanchun
UINT32 hcu_create_multi_dir(const char *path);



#endif /* L1COM_HWINV_H_ */
