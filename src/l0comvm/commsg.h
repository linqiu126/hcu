/*
 * commsg.h
 *
 *  Created on: 2016年1月3日
 *      Author: test
 */

#ifndef L0COMVM_COMMSG_H_
#define L0COMVM_COMMSG_H_

#include "../l0comvm/comtype.h"
#include "../l0comvm/sysdim.h"

#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <stdarg.h>
#include <errno.h>
#include <assert.h>
#include <string.h>
#include <dirent.h>
#include <time.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <termios.h>
#include <fcntl.h>
#include <dirent.h>
#include <ctype.h>
#include <getopt.h>
#include <limits.h>
#include <locale.h>
#include <math.h>

#include <sys/socket.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/ipc.h>
#include <sys/time.h>
#include <sys/msg.h>

#include <libxml/xmlmemory.h>
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xmlstring.h>
#include <linux/netdevice.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include <asm/types.h>

//V4L2视频捕获
#include <linux/videodev2.h>
//#include <jpeglib.h> //jpeg

//CURL
#include <curl/curl.h>

//ffmpeg库
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavdevice/avdevice.h>
#include <libavutil/mathematics.h>
#include <libavutil/avconfig.h>
#include <libavutil/opt.h>
#include <libavutil/imgutils.h>
//#include <SDL/SDL.h>
//#include <SDL/SDL_thread.h>

//Local MYSQL DB
//MYC split into x86 and arm, for mysql the header file location saved in different directory
#ifdef TARGET_LINUX_ARM
#include <mysql.h>
#endif

#ifdef TARGET_LINUX_X86
#include <mysql/mysql.h>
#endif

#ifdef TARGET_RASPBERRY_PI3B
#include <mysql/mysql.h>
#endif

//sqlite3 DB
#include <sqlite3.h>

#ifdef TARGET_RASPBERRY_PI3B
#include <wiringPi.h>
//GPIO

//Serial port
#include <wiringSerial.h>
#include <termios.h>
//I2C
#include <wiringPiI2C.h>
//SPI
#include <wiringPiSPI.h>
//Software driving tone
#include <softTone.h>
//PWM
#include <softPwm.h>
#endif

//I2C
#include <linux/i2c-dev.h>

//2. 公共消息结构体定义
//Under normal case, 1024Byte shall be enough for internal message communication purpose.
//If not enough, need modify here to enlarge
#define MAX_HCU_MSG_BUF_LENGTH MAX_HCU_MSG_BODY_LENGTH-16
typedef struct HcuMsgSruct
{
	UINT32 msgType;
	UINT32 dest_id;
	UINT32 src_id;
	UINT32 msgLen;
	INT8 msgBody[MAX_HCU_MSG_BODY_LENGTH];
}HcuMsgSruct_t;


//3. 原有程序的消息及数据结构体定义
/*
 * Type define for Socket
 */
typedef struct SocketPort
{
	char ServerIpAddress[16];
	UINT16 ServerPort;
	UINT16 SocketFd;
	struct hostent *pHostName;
	struct sockaddr_in ServerSocketAddress;
	struct sockaddr_in ClientSocketAddress;
} SocketPort_t;

#define		HCU_MAX_NUM_OF_WED	 			16
#define		WED_MAX_NUM_OF_MEASURE			7
#define		HCU_MAX_LENGTH_CMD_SERIAL 		(256 + 4 + 1)*4 /* 4 for MsgHeader, 1 for FCS */
#define		HCU_MAX_LENGTH_CMD_TCP 			1024

#define		HCU_MAX_NUM_POLLING_LOOP		100
#define		HCU_MAX_NUM_RECORD				(60*60*24*2)  /* two days of record, if we have 1 sec per measurement */


/* Sensor Type Definition */
#define		WED_SENSOR_TYPE_PM10			0x00000001
#define		WED_SENSOR_TYPE_PM25			0x00000002
#define		WED_SENSOR_TYPE_SO2				0x00000004
#define		WED_SENSOR_TYPE_NOX				0x00000008
#define		WED_SENSOR_TYPE_OO3				0x00000010
#define		WED_SENSOR_TYPE_CO				0x00000020
#define		WED_SENSOR_TYPE_TEMP			0x00000040

//#define 	SENSOR_ACTIVE	 				1
//#define		SENSOR_INACTIVE		 			0

#define		NUM_OF_SMT_COPY					2

/*
 * WED Wireless End Point Properties
 */
typedef struct WedSensorProperty
{
	UINT16 NodeId;
	UINT16 NodeZbDynAddress;
	UINT32 NodeZbIeeeAddress[2]; /* ZigBee IEEE address */
	UINT16 NodeStatus;
	UINT32 NodeSensorType;
} WedSensorProperty_t;

/*
 * WED All Sensors
 */
typedef struct WedSensors
{
	UINT32 NumOfActiveSensors;
	WedSensorProperty_t Sensor[HCU_MAX_NUM_OF_WED];
} WedSensors_t;

typedef struct MeasurePerSensor
{
	INT32 pm10; 	/* unit: mg/m3 devided by 1000 */
	INT32 pm25;		/* unit: mg/m3 devided by 1000 */
	INT32 so2;		/* unit: tbd */
	INT32 nox;		/* unit: tbd */
	INT32 oo3;		/* unit: tbd */
	INT32 co;		/* unit: tbd */
	INT32 temp;
} MeasurePerSensor_t;

typedef struct MeasureItem
{
	double 	value;
	time_t	time;
	UINT32 	sensorid;
	UINT32	sensortype;
}MeasureItem_t;
/*
 * WED All Sensors' Measurement Results
 *
 * For each of the sensor (which can measure several variables), there are total HCU_MAX_NUM_POLLING_LOOP records could be saved in the table.
 * This does not mean that for each of the loop, all HCU_MAX_NUM_POLLING_LOOP will be poll, teh actual number of loop for each of the Polling is defined in
 * NumOfPollingLoop in HCUMeasureBehavior_t
 */
typedef struct SensorsMeasurementsTable
{
	UINT32	activeTable;   /* copy 0 or copy 1 */
	UINT32	curRecord;     /* 0 to HCU_MAX_NUM_RECORD - 1*/
	MeasurePerSensor_t smt[NUM_OF_SMT_COPY][HCU_MAX_NUM_POLLING_LOOP][HCU_MAX_NUM_OF_WED];
	MeasureItem_t mi[NUM_OF_SMT_COPY][HCU_MAX_NUM_RECORD];
} SensorsMeasurementsTable_t;

typedef struct HCUMeasureBehavior
{
	UINT8  SerialVtime;				/* Timeout if no char received, timeout = Vtime * 100ms */
	UINT8  SerialVmin;				/* Number of char received before block read return */
	UINT32 DelayBeforeRead;		 	/* unit: ms */
	UINT32 DelayAfterRead; 			/* unit: ms */
	time_t SerialPollItvlSec;		/* unit: s */
	UINT32 SerialPollItvlnSec;		/* unit: nsec */
	UINT32 NumOfPollingLoop;   		/*  */

	UINT32 SocketInterval;
	UINT32 socketCnt;/*  */

	/* System Counter */
	ULONG NumberOfFrameTotalRx;
	ULONG NumberOfFrameValidRx;
	ULONG NumberOfFrameTotalTx;
	ULONG NumberOfTicksTriggerred;

	/* max counter when network is down before another socket connection tried */
	UINT32 MaxCounterTicksNetworkDown;

} HcuMeasureBehavior_t;

/*
 * HCU ZB interface
 *
 */


typedef struct HcuZbMsgHeader
{
	UINT8 sop;
	UINT8 len;
	UINT8 cmd0;
	UINT8 cmd1;
} HcuZbMsgHeader_t;

typedef struct PollingLoopBehavior
{
	UINT32 NumOfPoll;
	UINT32 NumOfSensor;
	UINT32 NumOfMeasure;
	UINT32 CurrentTick;
}PollingLoopBehavior_t;

typedef struct AnalyzeDataBehavior
{
	UINT32 NumOfType;
	UINT32 NumOfPolling;
	UINT32 NumOfWed;
}AnalyzeDataBehavior_t;

/* data header for msg send to sever */
typedef struct MsgSeverDataSegHeader
{
	UINT32 LenOfData;   /* ST=31;CN=2011;PW=123456;MN=12345678901002;CP=&&DataTime=2012101700100.....;CO-Rtd=0.00,CO-Flag=N;PMB-Rtd=0.00,PMB-Flag=N&& */
	char ST[16];       /* ST: 31 */
	char CN[16];       /* CN: 2011 */
	char PW[16];       /* PW: 123456 */
	char MN[16];       /* MN: 12345677654321 */
	char DataTime[64];
}MsgSeverDataSegHeader_t;

/* System Information */
#define     MAX_SYS_INFO_STRING_LENGTH			128
#define     MAX_SYS_INFO_INTERFACE_NUMBER		16

typedef struct SysHwInfo
{
	char cpu_processor[MAX_SYS_INFO_STRING_LENGTH];
	char cpu_vendor[MAX_SYS_INFO_STRING_LENGTH];
	char cpu_modelname[MAX_SYS_INFO_STRING_LENGTH];
	char cpu_mhz[MAX_SYS_INFO_STRING_LENGTH];
}SysHwInfo_t;

typedef struct SysOsInfo
{
	char version_signature[MAX_SYS_INFO_STRING_LENGTH];
}SysOsInfo_t;

typedef struct SysNetworkInfo
{

	int itfnum;
	char itfname[MAX_SYS_INFO_INTERFACE_NUMBER][MAX_SYS_INFO_STRING_LENGTH];
	char itfip[MAX_SYS_INFO_INTERFACE_NUMBER][MAX_SYS_INFO_STRING_LENGTH];
	/* whether there is PPP */
	pid_t ppppid;

}SysNetworkInfo_t;

typedef struct SysInfo
{
	SysHwInfo_t hw;
	SysOsInfo_t os;
	SysNetworkInfo_t net;
}SysInfo_t;

/* For thread creation */
typedef void *(*CALLBACK)(void *);

#define		MSG_SOP 						0xFE
#define 	HCU_SOP 						0xFE

#define 	MSG_COORD_START_IND_LEN 		0x00
#define 	MSG_COORD_START_IND_CMD1		0x80
#define		MSG_COORD_START_IND_CMD2		0x16

#define 	MSG_NODE_STATUS_IND_LEN    		0x0C
#define 	MSG_NODE_STATUS_IND_CMD1		0x80
#define 	MSG_NODE_STATUS_IND_CMD2		0x17

#define 	MSG_SENSOR_REG_IND_LEN			0x02
#define 	MSG_SENSOR_REG_IND_CMD1			0x80
#define 	MSG_SENSOR_REG_IND_CMD2			0x18

#define 	MSG_SENSOR_DEREG_IND_LEN		0x02
#define 	MSG_SENSOR_DEREG_IND_CMD1		0x80
#define 	MSG_SENSOR_DEREG_IND_CMD2		0x19

#define		MSG_APP_DATA_REG_CMD1			0x01
#define 	MSG_APP_DATA_REG_CMD2			0x20

#define 	MSG_APP_DATA_IND_CMD1			0x80
#define 	MSG_APP_DATA_IND_CMD2			0x20


#define		SENSOR_STATUS_INIT				0x0000
#define		SENSOR_STATUS_NODE_CONN			0x0001
#define		SENSOR_STATUS_DATA_CONN			0x0002

#define 	CLOCKID 						CLOCK_REALTIME
#define 	THREAD_COM_TIMER_ID 			111
#define		NB_NANOS_IN_ONE_SECOND			1000000000	//number of nanos in one second
#define 	NB_NANOS_IN_ONE_MS          	1000000     //number of nanos secondes in 1 ms
#define 	NB_MICROS_IN_ONE_SECOND        	1000000     //number of micros secondes in 1 second
#define 	NB_MICROS_IN_ONE_MS    	     	1000	    //number of micros secondes in 1 ms
#define 	START_DELAY                 	4           //delay(in second) before the launcher starts (to have time for clock synchro)

#define     HCU_THREAD_PRIO             	10          //priority of the main loop de 1 a 99 max



/* HCU MAIN STATE MACHINE BIT MAP */
#define		HCU_STATE_ALL_INIT				0x00000000
#define		HCU_STATE_SENSOR_OK				0x00000001
#define		HCU_STATE_NET_SERVICE_OK		0x00010000
#define		HCU_STATE_NET_OAM_OK			0x01000000
#define		HCU_STATE_AUTO_POLLING			0x00000100

#define 	HCU_MAX_TICKS_NETWORKDOWN		10

/* OAM CONTROL MESSAGE */
#define		MAX_OAM_CONTROL_MSG_LEN			64
#define		MAX_OAM_CONTROL_MSG_BUFFER		16

#define		MSG_ID_SENSOR_POLLING			0x00000001
#define		MSG_ID_DEBUG_TRACE_TOGGLE		0x00000002
#define		MSG_ID_STOP_AUTO_POLLING		0x00000003
#define		MSG_ID_START_AUTO_POLLING		0x00000004

typedef struct OamControlMsg
{
	UINT32 MsgId;
	UINT32 MsgLen;
	char MsgBody[MAX_OAM_CONTROL_MSG_LEN];
}OamControlMsg_t;

typedef struct OamControlMsgBuf
{
	UINT32 First;
	UINT32 Last;
	OamControlMsg_t MsgBuf[MAX_OAM_CONTROL_MSG_BUFFER];
}OamControlMsgBuf_t;

extern INT32  outputOnCrt;


/*
 *
 * 每定义一个新消息，请去修改vmlayer.c中的变量zHcuMsgNameList[]，不然TRACE会出现消息内容解析的错误
 *
 */

//4. 新消息新程序结构体定义部分
//复用下位机的消息定义
#define MSG_ID_ENTRY 0
enum HCU_INTER_TASK_MSG_ID
{
	//COMMOM message, KE_FIRST_MSG(TASK_AIRSYNC) as starting
	MSG_ID_COM_MIN = 0x00, //Starting point
	MSG_ID_COM_INIT,
	MSG_ID_COM_INIT_FEEDBACK,
	MSG_ID_COM_RESTART,  //L1->重新初始化上下文数据
	MSG_ID_COM_STOP,
	MSG_ID_COM_COMPLETE,
	MSG_ID_COM_TIME_OUT,
	MSG_ID_COM_DEBUG_TEST,
	MSG_ID_COM_HEART_BEAT,
	MSG_ID_COM_HEART_BEAT_FB,
	MSG_ID_COM_PROCESS_REBOOT,  //L2->重新创建任务和进程, 包括装载数据。还有一种层次，是L3->重新RESET硬件

	//Service Control message

	//ETHERNET
	MSG_ID_ETHERNET_CLOUDVELA_DATA_RX,

	//WIFI
	MSG_ID_WIFI_CLOUDVELA_DATA_RX,

	//USBNET
	MSG_ID_USBNET_CLOUDVELA_DATA_RX,

	//3G4G
	MSG_ID_3G4G_CLOUDVELA_DATA_RX,

	//RS232
	MSG_ID_SPS232_MODBUS_DATA_RX,

	//RS485
	MSG_ID_SPS485_MODBUS_DATA_RX,

	//SPSVIRGO
	MSG_ID_SPSVIRGO_HSMMP_DATA_RX,
	MSG_ID_SPSVIRGO_NOISE_DATA_REPORT,
	MSG_ID_SPSVIRGO_NOISE_CONTROL_FB,

	//AVORION
	MSG_ID_AVORION_HSMMP_DATA_READ_FB,
	MSG_ID_AVORION_HSMMP_DATA_RX,

	//BLE
	MSG_ID_BLE_HSMMP_DATA_RX,
	MSG_ID_BLE_MODBUS_DATA_RX,

	//GPS

	//LCD
	MSG_ID_LCD_AVORION_DATA_RX,

	//CAMERA
	MSG_ID_CAMERA_AVORION_DATA_RX,

	//MICROPHONE
	MSG_ID_MICROPHONE_AVORION_DATA_RX,

	//HWINV
	MSG_ID_HWINV_CLOUDVELA_PHY_STATUS_CHG,

	//CloudConnection message
	MSG_ID_CLOUDVELA_EMC_DATA_REQ,
	MSG_ID_CLOUDVELA_EMC_CONTROL_CMD,

	MSG_ID_CLOUDVELA_PM25_DATA_REQ,
	MSG_ID_CLOUDVELA_PM25_CONTROL_CMD,

	MSG_ID_CLOUDVELA_WINDDIR_DATA_REQ,
	MSG_ID_CLOUDVELA_WINDDIR_CONTROL_CMD,

	MSG_ID_CLOUDVELA_WINDSPD_DATA_REQ,
	MSG_ID_CLOUDVELA_WINDSPD_CONTROL_CMD,

	MSG_ID_CLOUDVELA_TEMP_DATA_REQ,
	MSG_ID_CLOUDVELA_TEMP_CONTROL_CMD,

	MSG_ID_CLOUDVELA_HUMID_DATA_REQ,
	MSG_ID_CLOUDVELA_HUMID_CONTROL_CMD,

	MSG_ID_CLOUDVELA_HSMMP_CONTROL_CMD,

	MSG_ID_CLOUDVELA_NOISE_DATA_REQ,
	MSG_ID_CLOUDVELA_NOISE_CONTROL_CMD,

	//Modbus report
	MSG_ID_MODBUS_EMC_DATA_REPORT,
	MSG_ID_MODBUS_EMC_CONTROL_FB,

	MSG_ID_MODBUS_PM25_DATA_REPORT,
	MSG_ID_MODBUS_PM25_CONTROL_FB,

	MSG_ID_MODBUS_WINDDIR_DATA_REPORT,
	MSG_ID_MODBUS_WINDDIR_CONTROL_FB,

	MSG_ID_MODBUS_WINDSPD_DATA_REPORT,
	MSG_ID_MODBUS_WINDSPD_CONTROL_FB,

	MSG_ID_MODBUS_TEMP_DATA_REPORT,
	MSG_ID_MODBUS_TEMP_CONTROL_FB,

	MSG_ID_MODBUS_HUMID_DATA_REPORT,
	MSG_ID_MODBUS_HUMID_CONTROL_FB,

	MSG_ID_MODBUS_NOISE_DATA_REPORT,
	MSG_ID_MODBUS_NOISE_CONTROL_FB,

	//EMC
	MSG_ID_EMC_MODBUS_DATA_READ,
	MSG_ID_EMC_CLOUDVELA_DATA_RESP,
	MSG_ID_EMC_MODBUS_CONTROL_CMD,
	MSG_ID_EMC_CLOUDVELA_CONTROL_FB,

	//PM25 message
	MSG_ID_PM25_MODBUS_DATA_READ,
	MSG_ID_PM25_CLOUDVELA_DATA_RESP,
	MSG_ID_PM25_MODBUS_CONTROL_CMD,
	MSG_ID_PM25_CLOUDVELA_CONTROL_FB,

	//WIND_DIRECTION message
	MSG_ID_WINDDIR_MODBUS_DATA_READ,
	MSG_ID_WINDDIR_CLOUDVELA_DATA_RESP,
	MSG_ID_WINDDIR_MODBUS_CONTROL_CMD,
	MSG_ID_WINDDIR_CLOUDVELA_CONTROL_FB,

	//WIND_SPEED message
	MSG_ID_WINDSPD_MODBUS_DATA_READ,
	MSG_ID_WINDSPD_CLOUDVELA_DATA_RESP,
	MSG_ID_WINDSPD_MODBUS_CONTROL_CMD,
	MSG_ID_WINDSPD_CLOUDVELA_CONTROL_FB,

	//TEMPERATURE message
	MSG_ID_TEMP_MODBUS_DATA_READ,
	MSG_ID_TEMP_CLOUDVELA_DATA_RESP,
	MSG_ID_TEMP_MODBUS_CONTROL_CMD,
	MSG_ID_TEMP_CLOUDVELA_CONTROL_FB,

	//HUMIDITY message
	MSG_ID_HUMID_MODBUS_DATA_READ,
	MSG_ID_HUMID_CLOUDVELA_DATA_RESP,
	MSG_ID_HUMID_MODBUS_CONTROL_CMD,
	MSG_ID_HUMID_CLOUDVELA_CONTROL_FB,

	//HSMMP
	MSG_ID_HSMMP_AVORION_DATA_READ,
	MSG_ID_HSMMP_AVORION_STOP,
	MSG_ID_HSMMP_CLOUDVELA_DATA_RESP,
	MSG_ID_HSMMP_CLOUDVELA_CONTROL_FB,
	MSG_ID_HSMMP_CLOUDVELA_DATA_LINK_RESP,

	//NOISE
	MSG_ID_NOISE_SPSVIRGO_DATA_READ,
	MSG_ID_NOISE_SPSVIRGO_CONTROL_CMD,
	MSG_ID_NOISE_SPSVIRGO_STOP,
	MSG_ID_NOISE_CLOUDVELA_DATA_RESP,
	MSG_ID_NOISE_CLOUDVELA_CONTROL_FB,
	MSG_ID_NOISE_MODBUS_DATA_READ,
	MSG_ID_NOISE_MODBUS_CONTROL_CMD,

	//AirSync message
	MSG_ID_AIRSYNC_INIT,
	MSG_ID_AIRSYNC_BLE_CONNNECTD,
	MSG_ID_AIRSYNC_AUTH_REQ,
	MSG_ID_AIRSYNC_AUTH_RESP,
	MSG_ID_AIRSYNC_INIT_REQ,
	MSG_ID_AIRSYNC_INIT_RESP,
	MSG_ID_AIRSYNC_CONNECT_READY,
	MSG_ID_AIRSYNC_DATA_PUSH,
	MSG_ID_AIRSYNC_DATA_REQ,
	MSG_ID_AIRSYNC_DATA_RESP,
	MSG_ID_AIRSYNC_AUTH_WAIT_TIMER,
	MSG_ID_AIRSYNC_DISCONNECT_TIGGER_L3,
	MSG_ID_AIRSYNC_SEND_BLE_DATA_TIMER,
	//WxApp message
	MSG_ID_WXAPP_3MIN_TIMEOUT,
	MSG_ID_WXAPP_5SECOND_TIMEOUT,
	MSG_ID_WXAPP_DATA_REQ_EMC,
	MSG_ID_WXAPP_PERIOD_REPORT_EMC_TRIGGER,
	MSG_ID_WXAPP_DATA_REQ_TIME_SYNC,
	MSG_ID_WXAPP_EQUIPMENT_INFO_SYNC,
	MSG_ID_WXAPP_MODBUS_CONNECT_READY, //trigger for MODBUS connection ready
	MSG_ID_WXAPP_MODBUS_DATA_PUSH, //send push command to MODBUS
	MSG_ID_WXAPP_MODBUS_DISCONNECT, //send for modbus
	//Modbus message
	MSG_ID_MODBUS_10MIN_TIMEOUT,
	MSG_ID_MODBUS_5SECOND_TIMEOUT, //
	MSG_ID_MODBUS_PERIOD_REPORT_PM1025_TRIGGER,  //TIMR FOR internal process
	MSG_ID_MODBUS_PERIOD_REPORT_WIND_SPEED_TRIGGER,  //TIMR FOR internal process
	MSG_ID_MODBUS_PERIOD_REPORT_WIND_DIR_TRIGGER,  //TIMR FOR internal process
	MSG_ID_MODBUS_PERIOD_REPORT_TEMPERATURE_TRIGGER,  //TIMR FOR internal process
	MSG_ID_MODBUS_PERIOD_REPORT_HUMIDITY_TRIGGER,  //TIMR FOR internal process
	MSG_ID_MODBUS_DATA_REQ_PM1025_REPORT,  //Send to AIRSYNC directly
	MSG_ID_MODBUS_DATA_REQ_WIND_SPEED_REPORT,  //Send to AIRSYNC directly
	MSG_ID_MODBUS_DATA_REQ_WIND_DIR_REPORT,  //Send to AIRSYNC directly
	MSG_ID_MODBUS_DATA_REQ_TEMPERATURE_REPORT,  //Send to AIRSYNC directly
	MSG_ID_MODBUS_DATA_REQ_HUMIDITY_REPORT,  //Send to AIRSYNC directly
	MSG_ID_MODBUS_UART1_FRAME_TIMEOUT,  //TIMR FOR UART1 Frame construction
	MSG_ID_COM_MAX, //Ending point

}; //end of HCU_INTER_TASK_MSG_ID
#define MSG_ID_END 0xFF  //跟MASK_MSGID_NUM_IN_ONE_TASK设置息息相关，不能随便改动
#define MSG_ID_INVALID 0xFFFFFFFF

//COMMON MESSAGE STRUCTURE
typedef struct com_gps_pos //
{
	char ew;
	UINT32 gpsx;
	char ns;
	UINT32 gpsy;
	UINT32 gpsz;
}com_gps_pos_t;
typedef struct  sensor_modbus_opertion_general //
{
	UINT32 equId;
	UINT8  powerOnOff;
	UINT32 newEquId;
	UINT32 workCycle;
	UINT32 interSample;
	UINT32 meausTimes;
}sensor_modbus_opertion_general_t;
typedef struct sensor_zhb_transport_format_dl //
{
	char   qn[21]; //中环保标准，20c, QN=YYYYMMDDHHMMSSZZZ，用来唯一标识一个命令请求, 用于请求命令或通知命令
	char   st[6];  //中环保标准，5c, ST=系统编号
	char   cn[8];  //中环保标准，7c, 命令编号 CN
	char   pw[7];  //中环保标准， 6c, PW=访问密码
	char   mn[13]; //中环保标准， 12c, 设备唯一标识MN
	UINT8  ansFlag;//中环保标准， 3c, 数据包是否拆分及应答标志 Flag，从云后台的接收方向
}sensor_zhb_transport_format_dl_t;
typedef struct  sensor_zhb_transport_format_ul //
{
	char   qn[21]; //中环保标准，20c, QN=YYYYMMDDHHMMSSZZZ，用来唯一标识一个命令请求, 用于请求命令或通知命令
	char   st[6];  //中环保标准，5c, ST=系统编号
	char   cn[8];  //中环保标准，7c, 命令编号 CN
	char   pw[7];  //中环保标准， 6c, PW=访问密码
	char   mn[13]; //中环保标准， 12c, 设备唯一标识MN
	UINT32 pnmu;     //总包号，满足中环保标准要求
	UINT32 pno;      //当前包号，满足中环保标准要求
}sensor_zhb_transport_format_ul_t;
typedef struct  sensor_emc_data_element //
{
	UINT32 equipid;
	UINT8  dataFormat;
	UINT32 emcValue;
	com_gps_pos_t gps;
	UINT32 timeStamp;
	UINT32 nTimes;
	UINT8 onOffLineFlag;
}sensor_emc_data_element_t;
typedef struct  sensor_pm25_data_element //
{
	UINT32 equipid;
	UINT8 dataFormat;
	UINT32 pm1d0Value;
	UINT32 pm2d5Value;
	UINT32 pm10Value;
	com_gps_pos_t gps;
	UINT32 timeStamp;
	UINT32 nTimes;
	UINT8 onOffLineFlag;
}sensor_pm25_data_element_t;
typedef struct  sensor_pm25_sharp_data_element //
{
	UINT32 equipid;
	UINT8 dataFormat;
	UINT32 pm2d5Value;
	UINT32 timeStamp;
}sensor_pm25_sharp_data_element_t;
typedef struct  sensor_winddir_data_element //
{
	UINT32 equipid;
	UINT8 dataFormat;
	UINT32 winddirValue;
	com_gps_pos_t gps;
	UINT32 timeStamp;
	UINT32 nTimes;
	UINT8 onOffLineFlag;
}sensor_winddir_data_element_t;
typedef struct  sensor_windspd_data_element //
{
	UINT32 equipid;
	UINT8 dataFormat;
	UINT32 windspdValue;
	com_gps_pos_t gps;
	UINT32 timeStamp;
	UINT32 nTimes;
	UINT8 onOffLineFlag;
}sensor_windspd_data_element_t;
typedef struct  sensor_temp_data_element //
{
	UINT32 equipid;
	UINT8 dataFormat;
	UINT32 tempValue;
	com_gps_pos_t gps;
	UINT32 timeStamp;
	UINT32 nTimes;
	UINT8 onOffLineFlag;
}sensor_temp_data_element_t;
typedef struct  sensor_temp_dht11_data_element //
{
	UINT32 equipid;
	UINT8 dataFormat;
	UINT32 tempValue;
	UINT32 timeStamp;
}sensor_temp_dht11_data_element_t;
typedef struct  sensor_temp_sht20_data_element //
{
	UINT32 equipid;
	UINT8 dataFormat;
	UINT32 tempValue;
	UINT32 timeStamp;
}sensor_temp_sht20_data_element_t;
typedef struct  sensor_temp_rht03_data_element //
{
	UINT32 equipid;
	UINT8 dataFormat;
	UINT32 tempValue;
	UINT32 timeStamp;
}sensor_temp_rht03_data_element_t;
typedef struct  sensor_humid_data_element //
{
	UINT32 equipid;
	UINT8 dataFormat;
	UINT32 humidValue;
	com_gps_pos_t gps;
	UINT32 timeStamp;
	UINT32 nTimes;
	UINT8 onOffLineFlag;
}sensor_humid_data_element_t;
typedef struct  sensor_humid_dht11_data_element //
{
	UINT32 equipid;
	UINT8 dataFormat;
	UINT32 humidValue;
	UINT32 timeStamp;
}sensor_humid_dht11_data_element_t;
typedef struct  sensor_humid_sht20_data_element //
{
	UINT32 equipid;
	UINT8 dataFormat;
	UINT32 humidValue;
	UINT32 timeStamp;
}sensor_humid_sht20_data_element_t;
typedef struct  sensor_humid_rht03_data_element //
{
	UINT32 equipid;
	UINT8 dataFormat;
	UINT32 humidValue;
	UINT32 timeStamp;
}sensor_humid_rht03_data_element_t;
typedef struct  sensor_noise_data_element //
{
	UINT32 equipid;
	UINT8 dataFormat;
	UINT32 noiseValue;
	com_gps_pos_t gps;
	UINT32 timeStamp;
	UINT32 nTimes;
	UINT8 onOffLineFlag;
}sensor_noise_data_element_t;
typedef struct  sensor_hsmmp_data_element //
{
	UINT32 equipid;
	char hsmmpFdir[HCU_FILE_NAME_LENGTH_MAX];
	char hsmmpFname[HCU_FILE_NAME_LENGTH_MAX];
	char hsmmpLink[HCU_FILE_NAME_LENGTH_MAX];
	com_gps_pos_t gps;
	UINT32 timeStamp;
	UINT32 nTimes;
	UINT8 onOffLineFlag;
}sensor_hsmmp_data_element_t;
typedef struct sensor_hsmmp_link_element
{
	UINT32 equipid;
	UINT32 linkLen;
	char   linkName[HCU_FILE_NAME_LENGTH_MAX];
	com_gps_pos_t gps;
	UINT32 timeStampStart;
	UINT32 timeStampEnd;
	UINT32 nTimes;
}sensor_hsmmp_link_element_t;
typedef struct  sensor_airprs_data_element //
{
	UINT32 equipid;
	UINT8 dataFormat;
	UINT32 airprsValue;
	com_gps_pos_t gps;
	UINT32 timeStamp;
	UINT32 nTimes;
	UINT8 onOffLineFlag;
}sensor_airprs_data_element_t;
typedef struct  sensor_airprs_bmp180_data_element //
{
	UINT32 equipid;
	UINT8 dataFormat;
	UINT32 airprsValue;
	UINT32 timeStamp;
}sensor_airprs_bmp180_data_element_t;
typedef struct  sensor_co1_data_element //
{
	UINT32 equipid;
	UINT8 dataFormat;
	UINT32 co1Value;
	com_gps_pos_t gps;
	UINT32 timeStamp;
	UINT32 nTimes;
	UINT8 onOffLineFlag;
}sensor_co1_data_element_t;
typedef struct  sensor_lightstr_data_element //
{
	UINT32 equipid;
	UINT8 dataFormat;
	UINT32 lightstrValue;
	com_gps_pos_t gps;
	UINT32 timeStamp;
	UINT32 nTimes;
	UINT8 onOffLineFlag;
}sensor_lightstr_data_element_t;
typedef struct  sensor_lightstr_bh1750_data_element //
{
	UINT32 equipid;
	UINT8 dataFormat;
	UINT32 lightstrValue;
	UINT32 timeStamp;
}sensor_lightstr_bh1750_data_element_t;
typedef struct  sensor_alcohol_data_element //
{
	UINT32 equipid;
	UINT8 dataFormat;
	UINT32 alcoholValue;
	com_gps_pos_t gps;
	UINT32 timeStamp;
	UINT32 nTimes;
	UINT8 onOffLineFlag;
}sensor_alcohol_data_element_t;
typedef struct  sensor_hcho_data_element //
{
	UINT32 equipid;
	UINT8 dataFormat;
	UINT32 hchoValue;
	com_gps_pos_t gps;
	UINT32 timeStamp;
	UINT32 nTimes;
	UINT8 onOffLineFlag;
}sensor_hcho_data_element_t;
typedef struct  sensor_toxicgas_data_element //
{
	UINT32 equipid;
	UINT8 dataFormat;
	UINT32 toxicgasValue;
	com_gps_pos_t gps;
	UINT32 timeStamp;
	UINT32 nTimes;
	UINT8 onOffLineFlag;
}sensor_toxicgas_data_element_t;
typedef struct  sensor_toxicgas_mq135_data_element //
{
	UINT32 equipid;
	UINT8 dataFormat;
	UINT32 toxicgasValue;
	UINT32 timeStamp;
}sensor_toxicgas_mq135_data_element_t;
//缺省消息都使用UINT32进行定义，在内存不是最重要的制约因素下，这种统一的方式，是为了更加不容易出错，不用在不同
//长度的字型之间进行转换。如果遇到INT类型，直接强制转换即可，符号单独处理
//message definition
typedef struct msg_struct_com_init //
{
	UINT32 length;
}msg_struct_com_init_t;

typedef struct msg_struct_com_init_feedback //
{
	UINT32 length;
}msg_struct_com_init_feedback_t;

typedef struct  msg_struct_com_restart //
{
	UINT32 length;
}msg_struct_com_restart_t;

typedef struct  msg_struct_com_stop //
{
	UINT32 length;
}msg_struct_com_stop_t;

typedef struct  msg_struct_com_complete //
{
	UINT32 length;
}msg_struct_com_complete_t;

typedef struct  msg_struct_com_time_out //
{
	UINT32 timeId;
	UINT8 timeRes;
	UINT32 length;
}msg_struct_com_time_out_t;

typedef struct  msg_struct_com_debug //
{
	UINT32 testid;
	char testinfo[10];
	UINT32 length;
}msg_struct_com_debug_t;

typedef struct  msg_struct_com_heart_beat //
{
	UINT32 length;
}msg_struct_com_heart_beat_t;
typedef struct  msg_struct_com_heart_beat_fb //
{
	UINT32 length;
}msg_struct_com_heart_beat_fb_t;
typedef struct  msg_struct_com_process_reboot //
{
	UINT32 taskId;
	UINT32 length;
}msg_struct_com_process_reboot_t;

#define HWINV_PHY_STATUS_NULL 0
#define HWINV_PHY_STATUS_DEACTIVE_TO_ACTIVE 1
#define HWINV_PHY_STATUS_ACTIVE_TO_DEACTIVE 2
#define HWINV_PHY_STATUS_INVALID 0xFF
typedef struct  msg_struct_hwinv_cloudvela_phy_status_chg //
{
	UINT8 ethStatChg;
	UINT8 usbnetStatChg;
	UINT8 wifiStatChg;
	UINT8 g3g4StatChg;
	UINT32 length;
}msg_struct_hwinv_cloudvela_phy_status_chg_t;

//Rx data coming from CLOUD direction, shall be controller data, no need too much data traffic
typedef struct  msg_struct_ethernet_cloudvela_data_rx //
{
	UINT32 length;
	char buf[MAX_HCU_MSG_BUF_LENGTH];
}msg_struct_ethernet_cloudvela_data_rx_t;
typedef struct  msg_struct_wifi_cloudvela_data_rx //
{
	UINT32 length;
	char buf[MAX_HCU_MSG_BUF_LENGTH];
}msg_struct_wifi_cloudvela_data_rx_t;
typedef struct  msg_struct_usbnet_cloudvela_data_rx //
{
	UINT32 length;
	char buf[MAX_HCU_MSG_BUF_LENGTH];
}msg_struct_usbnet_cloudvela_data_rx_t;
typedef struct  msg_struct_3g4g_cloudvela_data_rx //
{
	UINT32 length;
	char buf[MAX_HCU_MSG_BUF_LENGTH];
}msg_struct_3g4g_cloudvela_data_rx_t;
//Used to common all different received data buffer
typedef struct  msg_struct_com_cloudvela_data_rx //
{
	UINT32 length;
	char buf[MAX_HCU_MSG_BUF_LENGTH];
}msg_struct_com_cloudvela_data_rx_t;

typedef struct  msg_struct_sps232_modbus_data_rx //
{
	UINT32 length;
	UINT8 buf[MAX_HCU_MSG_BUF_LENGTH];
}msg_struct_sps232_modbus_data_rx_t;
typedef struct msg_struct_sps485_modbus_data_rx //
{
	UINT32 length;
	UINT8 buf[MAX_HCU_MSG_BUF_LENGTH];
}msg_struct_sps485_modbus_data_rx_t;
typedef struct msg_struct_spsvirgo_hsmmp_data_rx //
{
	UINT32 length;
	UINT8 buf[MAX_HCU_MSG_BUF_LENGTH];
}msg_struct_spsvirgo_hsmmp_data_rx_t;
typedef struct msg_struct_avorion_hsmmp_data_rx //
{
	UINT32 length;
	UINT8 buf[MAX_HCU_MSG_BUF_LENGTH];
}msg_struct_avorion_hsmmp_data_rx_t;
typedef struct msg_struct_ble_hsmmp_data_rx //
{
	UINT32 length;
	UINT8 buf[MAX_HCU_MSG_BUF_LENGTH];
}msg_struct_ble_hsmmp_data_rx_t;
typedef struct msg_struct_ble_modbus_data_rx //
{
	UINT32 length;
	UINT8 buf[MAX_HCU_MSG_BUF_LENGTH];
}msg_struct_ble_modbus_data_rx_t;
typedef struct msg_struct_lcd_avorion_data_rx //
{
	UINT32 length;
	UINT8 buf[MAX_HCU_MSG_BUF_LENGTH];
}msg_struct_lcd_avorion_data_rx_t;
typedef struct msg_struct_camera_avorion_data_rx //
{
	UINT32 length;
	UINT8 buf[MAX_HCU_MSG_BUF_LENGTH];
}msg_struct_camera_avorion_data_rx_t;
typedef struct msg_struct_microphone_avorion_data_rx //
{
	UINT32 length;
	UINT8 buf[MAX_HCU_MSG_BUF_LENGTH];
}msg_struct_microphone_avorion_data_rx_t;

//Data request from Cloud to Sensors
typedef struct  msg_struct_cloudvela_emc_data_req //
{
	UINT8  cmdId;
	UINT8  optId;
	UINT8  cmdIdBackType; //指明是瞬时，还是周期性读数
	UINT32 equId;
	sensor_zhb_transport_format_dl_t zhbDl;
	UINT32 length;
}msg_struct_cloudvela_emc_data_req_t;
typedef struct  msg_struct_cloudvela_pm25_data_req //
{
	UINT8  cmdId;
	UINT8  optId;
	UINT8  cmdIdBackType; //指明是瞬时，还是周期性读数
	UINT32 equId;
	sensor_zhb_transport_format_dl_t zhbDl;
	UINT32 length;
}msg_struct_cloudvela_pm25_data_req_t;
typedef struct  msg_struct_cloudvela_winddir_data_req //
{
	UINT8  cmdId;
	UINT8  optId;
	UINT8  cmdIdBackType; //指明是瞬时，还是周期性读数
	UINT32 equId;
	sensor_zhb_transport_format_dl_t zhbDl;
	UINT32 length;
}msg_struct_cloudvela_winddir_data_req_t;
typedef struct  msg_struct_cloudvela_windspd_data_req //
{
	UINT8  cmdId;
	UINT8  optId;
	UINT8  cmdIdBackType; //指明是瞬时，还是周期性读数
	UINT32 equId;
	sensor_zhb_transport_format_dl_t zhbDl;
	UINT32 length;
}msg_struct_cloudvela_windspd_data_req_t;
typedef struct  msg_struct_cloudvela_temp_data_req //
{
	UINT8  cmdId;
	UINT8  optId;
	UINT8  cmdIdBackType; //指明是瞬时，还是周期性读数
	UINT32 equId;
	sensor_zhb_transport_format_dl_t zhbDl;
	UINT32 length;
}msg_struct_cloudvela_temp_data_req_t;
typedef struct  msg_struct_cloudvela_humid_data_req //
{
	UINT8  cmdId;
	UINT8  optId;
	UINT8  cmdIdBackType; //指明是瞬时，还是周期性读数
	UINT32 equId;
	sensor_zhb_transport_format_dl_t zhbDl;
	UINT32 length;
}msg_struct_cloudvela_humid_data_req_t;
typedef struct  msg_struct_cloudvela_noise_data_req //
{
	UINT8  cmdId;
	UINT8  optId;
	UINT8  cmdIdBackType; //指明是瞬时，还是周期性读数
	UINT32 equId;
	sensor_zhb_transport_format_dl_t zhbDl;
	UINT32 length;
}msg_struct_cloudvela_noise_data_req_t;

typedef struct  msg_struct_cloudvela_emc_control_cmd //
{
	UINT8  cmdId;
	UINT8  optId;
	UINT8  backType;
	sensor_modbus_opertion_general_t opt;
	sensor_zhb_transport_format_dl_t zhbDl;
	UINT32 length;
}msg_struct_cloudvela_emc_control_cmd_t;
typedef struct  msg_struct_cloudvela_pm25_control_cmd //
{
	UINT8  cmdId;
	UINT8  optId;
	UINT8  backType;
	sensor_modbus_opertion_general_t opt;
	sensor_zhb_transport_format_dl_t zhbDl;
	UINT32 length;
}msg_struct_cloudvela_pm25_control_cmd_t;
typedef struct  msg_struct_cloudvela_winddir_control_cmd //
{
	UINT8  cmdId;
	UINT8  optId;
	UINT8  backType;
	sensor_modbus_opertion_general_t opt;
	sensor_zhb_transport_format_dl_t zhbDl;
	UINT32 length;
}msg_struct_cloudvela_winddir_control_cmd_t;
typedef struct  msg_struct_cloudvela_windspd_control_cmd //
{
	UINT8  cmdId;
	UINT8  optId;
	UINT8  backType;
	sensor_modbus_opertion_general_t opt;
	sensor_zhb_transport_format_dl_t zhbDl;
	UINT32 length;
}msg_struct_cloudvela_windspd_control_cmd_t;
typedef struct  msg_struct_cloudvela_temp_control_cmd //
{
	UINT8  cmdId;
	UINT8  optId;
	UINT8  backType;
	sensor_modbus_opertion_general_t opt;
	sensor_zhb_transport_format_dl_t zhbDl;
	UINT32 length;
}msg_struct_cloudvela_temp_control_cmd_t;
typedef struct  msg_struct_cloudvela_humid_control_cmd //
{
	UINT8  cmdId;
	UINT8  optId;
	UINT8  backType;
	sensor_modbus_opertion_general_t opt;
	sensor_zhb_transport_format_dl_t zhbDl;
	UINT32 length;
}msg_struct_cloudvela_humid_control_cmd_t;
typedef struct  msg_struct_cloudvela_noise_control_cmd //
{
	UINT8  cmdId;
	UINT8  optId;
	UINT8  backType;
	sensor_modbus_opertion_general_t opt;
	sensor_zhb_transport_format_dl_t zhbDl;
	UINT32 length;
}msg_struct_cloudvela_noise_control_cmd_t;
typedef struct  msg_struct_cloudvela_hsmmp_control_cmd //
{
	UINT8  cmdId;
	UINT8  optId;
	UINT8  backType;
	sensor_modbus_opertion_general_t opt;
	sensor_zhb_transport_format_dl_t zhbDl;
	UINT32 length;
}msg_struct_cloudvela_hsmmp_control_cmd_t;

//SENSOR Data request to MODBUS
typedef struct  msg_struct_emc_modbus_data_read //
{
	UINT8  cmdId;
	UINT8  optId;
	UINT8  cmdIdBackType; //指明是瞬时，还是周期性读数
	UINT32 equId;
	UINT32 length;
}msg_struct_emc_modbus_data_read_t;
typedef struct  msg_struct_pm25_modbus_data_read //
{
	UINT8  cmdId;
	UINT8  optId;
	UINT8  cmdIdBackType; //指明是瞬时，还是周期性读数
	UINT32 equId;
	UINT32 length;
}msg_struct_pm25_modbus_data_read_t;
typedef struct  msg_struct_winddir_modbus_data_read //
{
	UINT8  cmdId;
	UINT8  optId;
	UINT8  cmdIdBackType; //指明是瞬时，还是周期性读数
	UINT32 equId;
	UINT32 length;
}msg_struct_winddir_modbus_data_read_t;
typedef struct  msg_struct_windspd_modbus_data_read //
{
	UINT8  cmdId;
	UINT8  optId;
	UINT8  cmdIdBackType; //指明是瞬时，还是周期性读数
	UINT32 equId;
	UINT32 length;
}msg_struct_windspd_modbus_data_read_t;
typedef struct  msg_struct_temp_modbus_data_read //
{
	UINT8  cmdId;
	UINT8  optId;
	UINT8  cmdIdBackType; //指明是瞬时，还是周期性读数
	UINT32 equId;
	UINT32 length;
}msg_struct_temp_modbus_data_read_t;
typedef struct  msg_struct_humid_modbus_data_read //
{
	UINT8  cmdId;
	UINT8  optId;
	UINT8  cmdIdBackType; //指明是瞬时，还是周期性读数
	UINT32 equId;
	UINT32 length;
}msg_struct_humid_modbus_data_read_t;
typedef struct  msg_struct_noise_modbus_data_read //
{
	UINT8  cmdId;
	UINT8  optId;
	UINT8  cmdIdBackType; //指明是瞬时，还是周期性读数
	UINT32 equId;
	UINT32 length;
}msg_struct_noise_modbus_data_read_t;
typedef struct  msg_struct_noise_spsvirgo_data_read //
{
	UINT8  cmdId;
	UINT8  optId;
	UINT8  cmdIdBackType; //指明是瞬时，还是周期性读数
	UINT32 equId;
	UINT32 length;
}msg_struct_noise_spsvirgo_data_read_t;

typedef struct  msg_struct_emc_modbus_control_cmd //
{
	UINT8  cmdId;
	UINT8  optId;
	UINT8  backType;
	sensor_modbus_opertion_general_t opt;
	UINT32 length;
}msg_struct_emc_modbus_control_cmd_t;
typedef struct  msg_struct_pm25_modbus_control_cmd //
{
	UINT8  cmdId;
	UINT8  optId;
	UINT8  backType;
	sensor_modbus_opertion_general_t opt;
	UINT32 length;
}msg_struct_pm25_modbus_control_cmd_t;
typedef struct  msg_struct_winddir_modbus_control_cmd //
{
	UINT8  cmdId;
	UINT8  optId;
	UINT8  backType;
	sensor_modbus_opertion_general_t opt;
	UINT32 length;
}msg_struct_winddir_modbus_control_cmd_t;
typedef struct  msg_struct_windspd_modbus_control_cmd //
{
	UINT8  cmdId;
	UINT8  optId;
	UINT8  backType;
	sensor_modbus_opertion_general_t opt;
	UINT32 length;
}msg_struct_windspd_modbus_control_cmd_t;
typedef struct  msg_struct_temp_modbus_control_cmd //
{
	UINT8  cmdId;
	UINT8  optId;
	UINT8  backType;
	sensor_modbus_opertion_general_t opt;
	UINT32 length;
}msg_struct_temp_modbus_control_cmd_t;
typedef struct  msg_struct_humid_modbus_control_cmd //
{
	UINT8  cmdId;
	UINT8  optId;
	UINT8  backType;
	sensor_modbus_opertion_general_t opt;
	UINT32 length;
}msg_struct_humid_modbus_control_cmd_t;
typedef struct  msg_struct_noise_modbus_control_cmd //
{
	UINT8  cmdId;
	UINT8  optId;
	UINT8  backType;
	sensor_modbus_opertion_general_t opt;
	UINT32 length;
}msg_struct_noise_modbus_control_cmd_t;
typedef struct  msg_struct_noise_spsvirgo_control_cmd //
{
	UINT8  cmdId;
	UINT8  optId;
	UINT8  backType;
	sensor_modbus_opertion_general_t opt;
	UINT32 length;
}msg_struct_noise_spsvirgo_control_cmd_t;

//Data Report message from Modbus
typedef struct  msg_struct_modbus_emc_data_report //
{
	UINT8  usercmdid;
	UINT8  useroptid;
	UINT8  cmdIdBackType; //指明是瞬时，还是周期性读数
	sensor_emc_data_element_t emc;
	UINT32 length;
}msg_struct_modbus_emc_data_report_t;

typedef struct  msg_struct_modbus_pm25_data_report //
{
	UINT8  usercmdid;
	UINT8  useroptid;
	UINT8  cmdIdBackType; //指明是瞬时，还是周期性读数
	sensor_pm25_data_element_t pm25;
	UINT32 length;
}msg_struct_modbus_pm25_data_report_t;
typedef struct msg_struct_modbus_winddir_data_report
{
	UINT8  usercmdid;
	UINT8  useroptid;
	UINT8  cmdIdBackType; //指明是瞬时，还是周期性读数
	sensor_winddir_data_element_t winddir;
	UINT32 length;
}msg_struct_modbus_winddir_data_report_t;

typedef struct msg_struct_modbus_windspd_data_report
{
	UINT8  usercmdid;
	UINT8  useroptid;
	UINT8  cmdIdBackType; //指明是瞬时，还是周期性读数
	sensor_windspd_data_element_t windspd;
	UINT32 length;
}msg_struct_modbus_windspd_data_report_t;

typedef struct msg_struct_modbus_temp_data_report
{
	UINT8  usercmdid;
	UINT8  useroptid;
	UINT8  cmdIdBackType; //指明是瞬时，还是周期性读数
	sensor_temp_data_element_t temp;
	UINT32 length;
}msg_struct_modbus_temp_data_report_t;

typedef struct msg_struct_modbus_humid_data_report
{
	UINT8  usercmdid;
	UINT8  useroptid;
	UINT8  cmdIdBackType; //指明是瞬时，还是周期性读数
	sensor_humid_data_element_t humid;
	UINT32 length;
}msg_struct_modbus_humid_data_report_t;

typedef struct msg_struct_modbus_noise_data_report
{
	UINT8  usercmdid;
	UINT8  useroptid;
	UINT8  cmdIdBackType; //指明是瞬时，还是周期性读数
	sensor_noise_data_element_t noise;
	UINT32 length;
}msg_struct_modbus_noise_data_report_t;

typedef struct msg_struct_spsvirgo_noise_data_report
{
	UINT8  usercmdid;
	UINT8  useroptid;
	UINT8  cmdIdBackType; //指明是瞬时，还是周期性读数
	sensor_noise_data_element_t noise;
	UINT32 length;
}msg_struct_spsvirgo_noise_data_report_t;

typedef struct msg_struct_modbus_emc_control_fb
{
	UINT8  cmdId;
	UINT8  optId;
	UINT8  backType;
	sensor_modbus_opertion_general_t opt;
	UINT32 length;
}msg_struct_modbus_emc_control_fb_t;
typedef struct msg_struct_modbus_pm25_control_fb
{
	UINT8  cmdId;
	UINT8  optId;
	UINT8  backType;
	sensor_modbus_opertion_general_t opt;
	UINT32 length;
}msg_struct_modbus_pm25_control_fb_t;
typedef struct msg_struct_modbus_winddir_control_fb
{
	UINT8  cmdId;
	UINT8  optId;
	UINT8  backType;
	sensor_modbus_opertion_general_t opt;
	UINT32 length;
}msg_struct_modbus_winddir_control_fb_t;
typedef struct msg_struct_modbus_windspd_control_fb
{
	UINT8  cmdId;
	UINT8  optId;
	UINT8  backType;
	sensor_modbus_opertion_general_t opt;
	UINT32 length;
}msg_struct_modbus_windspd_control_fb_t;
typedef struct msg_struct_modbus_temp_control_fb
{
	UINT8  cmdId;
	UINT8  optId;
	UINT8  backType;
	sensor_modbus_opertion_general_t opt;
	UINT32 length;
}msg_struct_modbus_temp_control_fb_t;
typedef struct msg_struct_modbus_humid_control_fb
{
	UINT8  cmdId;
	UINT8  optId;
	UINT8  backType;
	sensor_modbus_opertion_general_t opt;
	UINT32 length;
}msg_struct_modbus_humid_control_fb_t;
typedef struct msg_struct_modbus_noise_control_fb
{
	UINT8  cmdId;
	UINT8  optId;
	UINT8  backType;
	sensor_modbus_opertion_general_t opt;
	UINT32 length;
}msg_struct_modbus_noise_control_fb_t;
typedef struct msg_struct_spsvirgo_noise_control_fb
{
	UINT8  cmdId;
	UINT8  optId;
	UINT8  backType;
	sensor_modbus_opertion_general_t opt;
	UINT32 length;
}msg_struct_spsvirgo_noise_control_fb_t;


//Data response message to Cloud
typedef struct  msg_struct_emc_cloudvela_data_resp //
{
	UINT8  usercmdid;
	UINT8  useroptid;
	UINT8  cmdIdBackType; //指明是瞬时，还是周期性读数
	sensor_emc_data_element_t emc;
	sensor_zhb_transport_format_ul_t zhbUl;
	UINT32 length;
}msg_struct_emc_cloudvela_data_resp_t;
typedef struct  msg_struct_pm25_cloudvela_data_resp_t //
{
	UINT8  usercmdid;
	UINT8  useroptid;
	UINT8  cmdIdBackType; //指明是瞬时，还是周期性读数
	sensor_pm25_data_element_t pm25;
	sensor_zhb_transport_format_ul_t zhbUl;
	UINT32 length;
}msg_struct_pm25_cloudvela_data_resp_t;
typedef struct msg_struct_winddir_cloudvela_data_resp
{
	UINT8  usercmdid;
	UINT8  useroptid;
	UINT8  cmdIdBackType; //指明是瞬时，还是周期性读数
	sensor_winddir_data_element_t winddir;
	sensor_zhb_transport_format_ul_t zhbUl;
	UINT32 length;
}msg_struct_winddir_cloudvela_data_resp_t;
typedef struct msg_struct_windspd_cloudvela_data_resp
{
	UINT8  usercmdid;
	UINT8  useroptid;
	UINT8  cmdIdBackType; //指明是瞬时，还是周期性读数
	sensor_windspd_data_element_t windspd;
	sensor_zhb_transport_format_ul_t zhbUl;
	UINT32 length;
}msg_struct_windspd_cloudvela_data_resp_t;
typedef struct msg_struct_temp_cloudvela_data_resp
{
	UINT8  usercmdid;
	UINT8  useroptid;
	UINT8  cmdIdBackType; //指明是瞬时，还是周期性读数
	sensor_temp_data_element_t temp;
	sensor_zhb_transport_format_ul_t zhbUl;
	UINT32 length;
}msg_struct_temp_cloudvela_data_resp_t;
typedef struct msg_struct_humid_cloudvela_data_resp
{
	UINT8  usercmdid;
	UINT8  useroptid;
	UINT8  cmdIdBackType; //指明是瞬时，还是周期性读数
	sensor_humid_data_element_t humid;
	sensor_zhb_transport_format_ul_t zhbUl;
	UINT32 length;
}msg_struct_humid_cloudvela_data_resp_t;
typedef struct msg_struct_noise_cloudvela_data_resp
{
	UINT8  usercmdid;
	UINT8  useroptid;
	UINT8  cmdIdBackType; //指明是瞬时，还是周期性读数
	sensor_noise_data_element_t noise;
	sensor_zhb_transport_format_ul_t zhbUl;
	UINT32 length;
}msg_struct_noise_cloudvela_data_resp_t;

typedef struct msg_struct_emc_cloudvela_control_fb
{
	UINT8  cmdId;
	UINT8  optId;
	UINT8  backType;
	sensor_modbus_opertion_general_t opt;
	sensor_zhb_transport_format_ul_t zhbUl;
	UINT32 length;
}msg_struct_emc_cloudvela_control_fb_t;
typedef struct msg_struct_pm25_cloudvela_control_fb
{
	UINT8  cmdId;
	UINT8  optId;
	UINT8  backType;
	sensor_modbus_opertion_general_t opt;
	sensor_zhb_transport_format_ul_t zhbUl;
	UINT32 length;
}msg_struct_pm25_cloudvela_control_fb_t;
typedef struct msg_struct_winddir_cloudvela_control_fb
{
	UINT8  cmdId;
	UINT8  optId;
	UINT8  backType;
	sensor_modbus_opertion_general_t opt;
	sensor_zhb_transport_format_ul_t zhbUl;
	UINT32 length;
}msg_struct_winddir_cloudvela_control_fb_t;
typedef struct msg_struct_windspd_cloudvela_control_fb
{
	UINT8  cmdId;
	UINT8  optId;
	UINT8  backType;
	sensor_modbus_opertion_general_t opt;
	sensor_zhb_transport_format_ul_t zhbUl;
	UINT32 length;
}msg_struct_windspd_cloudvela_control_fb_t;
typedef struct msg_struct_temp_cloudvela_control_fb
{
	UINT8  cmdId;
	UINT8  optId;
	UINT8  backType;
	sensor_modbus_opertion_general_t opt;
	sensor_zhb_transport_format_ul_t zhbUl;
	UINT32 length;
}msg_struct_temp_cloudvela_control_fb_t;
typedef struct msg_struct_humid_cloudvela_control_fb
{
	UINT8  cmdId;
	UINT8  optId;
	UINT8  backType;
	sensor_modbus_opertion_general_t opt;
	sensor_zhb_transport_format_ul_t zhbUl;
	UINT32 length;
}msg_struct_humid_cloudvela_control_fb_t;
typedef struct msg_struct_noise_cloudvela_control_fb
{
	UINT8  cmdId;
	UINT8  optId;
	UINT8  backType;
	sensor_modbus_opertion_general_t opt;
	sensor_zhb_transport_format_ul_t zhbUl;
	UINT32 length;
}msg_struct_noise_cloudvela_control_fb_t;

//HSMMP-AVORION交互消息
typedef struct msg_struct_hsmmp_avorion_data_read //
{
	UINT32 equipid;
	UINT32 captureDur; //抓取视频的时间长度
	UINT32 refreshRate; //刷新率
	UINT8  fileType; // FileOperationTypeEnum defined at L2COMDEF.h
	UINT8  boolBackCloud; //指明是否需要将该数据发送到后台
	UINT32 timeStampStart;
	char   fDirName[HCU_FILE_NAME_LENGTH_MAX];
	char   fName[HCU_FILE_NAME_LENGTH_MAX];
	char   tmpFname[HCU_FILE_NAME_LENGTH_MAX];
	UINT32 length;
}msg_struct_hsmmp_avorion_data_read_t;
typedef struct msg_struct_hsmmp_avorion_stop //
{
	UINT32 length;
}msg_struct_hsmmp_avorion_stop_t;

typedef struct msg_struct_avorion_hsmmp_data_read_fb //
{
	UINT8  boolBackCloud; //指明是否需要将该数据发送到后台
	sensor_hsmmp_data_element_t hsmmp; //tmpFname暂时被hsmmpLink所替代
	UINT32 length;
}msg_struct_avorion_hsmmp_data_read_fb_t;

typedef struct msg_struct_hsmmp_cloudvela_data_resp
{
	UINT8  usercmdid;
	UINT8  useroptid;
	UINT8  cmdIdBackType; //指明是瞬时，还是周期性读数
	sensor_hsmmp_link_element_t link;
	sensor_zhb_transport_format_ul_t zhbUl;
	UINT32 length;
}msg_struct_hsmmp_cloudvela_data_resp_t;
typedef struct msg_struct_hsmmp_cloudvela_control_fb
{
	UINT8  cmdId;
	UINT8  optId;
	UINT8  backType;
	sensor_modbus_opertion_general_t opt;
	sensor_zhb_transport_format_ul_t zhbUl;
	UINT32 length;
}msg_struct_hsmmp_cloudvela_control_fb_t;
typedef struct msg_struct_hsmmp_cloudvela_data_link_resp
{
	UINT8  usercmdid;
	UINT8  useroptid;
	UINT8  cmdIdBackType; //指明是瞬时，还是周期性读数
	sensor_hsmmp_link_element_t link;
	sensor_zhb_transport_format_ul_t zhbUl;
	UINT32 length;
}msg_struct_hsmmp_cloudvela_data_link_resp_t;


typedef struct msg_struct_noise_spsvirgo_stop //
{
	UINT32 length;
}msg_struct_noise_spsvirgo_stop_t;





















//Old world message structure, for removal
typedef struct  msg_struct_airsync_ble_connected //
{
	UINT32 length;
}msg_struct_airsync_ble_connected_t;

typedef struct  msg_struct_airsync_auth_resp
{
	/// Handle of the attribute that has to be written
	UINT32 handle;
	/// Data length to be written
	UINT32 length;
	/// offset at which the data has to be written
	UINT32 offset;
	/// Destination task shall send back a write response command if true.
	BOOL response;
	///Informs that it's last request of a multiple prepare write request.
	bool last;
	/// Data to be written in attribute database
	UINT8  value[MAX_HCU_MSG_BUF_LENGTH];
}msg_struct_airsync_auth_resp_t;

typedef struct  msg_struct_airsync_init_resp
{
	/// Handle of the attribute that has to be written
	UINT32 handle;
	/// Data length to be written
	UINT32 length;
	/// offset at which the data has to be written
	UINT32 offset;
	/// Destination task shall send back a write response command if true.
	bool response;
	///Informs that it's last request of a multiple prepare write request.
	bool last;
	/// Data to be written in attribute database
	UINT8  value[MAX_HCU_MSG_BUF_LENGTH];
}msg_struct_airsync_init_resp_t;

typedef struct  msg_struct_airsync_connect_ready
{
	UINT32 length;
}msg_struct_airsync_connect_ready_t;

#define ASY_DATA_PUSH_LENGTH_MAX 20
typedef struct  msg_struct_airsync_data_push
{
	UINT8 UserCmdId;
	UINT16 EmDeviceDataType;
	UINT32 length;
	UINT8 data[ASY_DATA_PUSH_LENGTH_MAX];
}msg_struct_airsync_data_push_t;
//
typedef struct  msg_struct_airsync_data_resp
{
	/// Handle of the attribute that has to be written
	UINT32 handle;
	/// Data length to be written
	UINT32 length;
	/// offset at which the data has to be written
	UINT32 offset;
	/// Destination task shall send back a write response command if true.
	bool response;
	///Informs that it's last request of a multiple prepare write request.
	bool last;
	/// Data to be written in attribute database
	UINT8  value[MAX_HCU_MSG_BUF_LENGTH];
}msg_struct_airsync_data_resp_t;

typedef struct  msg_struct_airsync_disconnect_tigger_l3
{
	UINT32 length;
}msg_struct_airsync_disconnect_tigger_l3_t;

typedef struct  msg_struct_airsync_send_ble_data_timer
{
	UINT32 length;
}msg_struct_airsync_send_ble_data_timer_t;

//All L3 message structure defined here

typedef struct  msg_struct_3min_timtout
{
	UINT32 length;
}msg_struct_3min_timtout_t;

typedef struct  msg_struct_5second_timtout
{
	UINT32 length;
}msg_struct_5second_timtout_t;

typedef struct  msg_struct_period_report_emc_trigger
{
	UINT32 length;
}msg_struct_period_report_emc_trigger_t;
//
typedef struct  msg_struct_wxapp_data_req_emc
{
	UINT8 usercmdid;
	UINT32 length;
	UINT16 emc_value;
	UINT8 year;
	UINT8 month;
	UINT8 day;
	UINT8 hour;
	UINT8 minute;
	UINT8 second;
	UINT32 gps_x;
	UINT32 gps_y;
	UINT16 ntimes;
	UINT16 EmDeviceDataType;
}msg_struct_wxapp_data_req_emc_t;

typedef struct  msg_struct_wxapp_data_req_time_sync
{
	UINT32 length;
}msg_struct_wxapp_data_req_time_sync_t;

typedef struct  msg_struct_wxapp_equipment_info_sync
{
	UINT32 length;
	UINT8 hw_uuid[6];
	UINT8 hw_type;
	UINT16 hw_version;
	UINT8 sw_release;
	UINT16 sw_delivery;
}msg_struct_wxapp_equipment_info_sync_t;

typedef struct  msg_struct_wxapp_modbus_connect_ready
{
	UINT32 length;
}msg_struct_wxapp_modbus_connect_ready_t;

#define WXAPP_DATA_PUSH_LENGTH_MAX 20
typedef struct  msg_struct_wxapp_modbus_data_push
{
	UINT8 UserCmdId;
	UINT16 EmDeviceDataType;
	UINT32 length;
	UINT8 data[WXAPP_DATA_PUSH_LENGTH_MAX];
}msg_struct_wxapp_modbus_data_push_t;

typedef struct  msg_struct_wxapp_modbus_disconnect
{
	UINT32 length;
}msg_struct_wxapp_modbus_disconnect_t;

//All L3 message structure defined here

typedef struct  msg_struct_mobus_10min_timtout
{
	UINT32 length;
}msg_struct_mobus_10min_timtout_t;

typedef struct  msg_struct_modbus_5second_timtout
{
	UINT32 length;
}msg_struct_modbus_5second_timtout_t;

typedef struct  msg_struct_modbus_period_report_pm1025_trigger
{
	UINT32 length;
}msg_struct_modbus_period_report_pm1025_trigger_t;

typedef struct  msg_struct_modbus_period_report_windspd_trigger
{
	UINT32 length;
}msg_struct_modbus_period_report_windspd_trigger_t;

typedef struct  msg_struct_modbus_period_report_winddir_trigger
{
	UINT32 length;
}msg_struct_modbus_period_report_winddir_trigger_t;

typedef struct  msg_struct_modbus_period_report_temp_trigger
{
	UINT32 length;
}msg_struct_modbus_period_report_temp_trigger_t;

typedef struct  msg_struct_modbus_period_report_humid_trigger
{
	UINT32 length;
}msg_struct_modbus_period_report_humid_trigger_t;


typedef struct  msg_struct_modbus_uart1_frame_timtout
{
	UINT32 length;
}msg_struct_modbus_uart1_frame_timtout_t;

#endif /* L0COMVM_COMMSG_H_ */
