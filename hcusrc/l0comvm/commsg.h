/*
 * commsg.h
 *
 *  Created on: 2016年1月3日
 *      Author: test
 */

#ifndef L0COMVM_COMMSG_H_
#define L0COMVM_COMMSG_H_

//Local Include
#include "../l0comvm/comtype.h"
#include "../l0comvm/sysdim.h"
//Basic Lib
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
#include <termios.h>
#include <fcntl.h>
#include <dirent.h>
#include <ctype.h>
#include <getopt.h>
#include <limits.h>
#include <locale.h>
#include <math.h>
//Network Lib
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
#include <sys/reboot.h>
#include <libxml/xmlmemory.h>
#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xmlstring.h>
#include <linux/netdevice.h>
#include <sys/prctl.h>
#include <netinet/tcp.h>
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
#include <libavutil/avutil.h>
#include <libavutil/opt.h>
#include <libavutil/pixdesc.h>
//#include <SDL/SDL.h>
//#include <SDL/SDL_thread.h>
//I2C
#include <linux/i2c-dev.h>
//CAN
#include <linux/can.h>
#include <linux/can/raw.h>
//文件删去
#include <sys/vfs.h>

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


//2. 公共消息结构体定义
//Under normal case, 1024Byte shall be enough for internal message communication purpose.
//If not enough, need modify here to enlarge
#define HCU_SYSMSG_COM_MSG_BODY_LEN_MAX 	HCU_SYSDIM_MSG_BODY_LEN_MAX-16
#define HCU_SYSMSG_BH_BUF_BODY_LEN_MAX 		HCU_SYSDIM_MSG_BODY_LEN_MAX-4  //跟L2FRAME长度保持同步，都是比消息BUFFER小4字节，以便容纳进消息BODY之中
typedef struct HcuMsgSruct
{
	UINT32 msgType;
	UINT32 dest_id;
	UINT32 src_id;
	UINT32 msgLen;
	INT8 msgBody[HCU_SYSDIM_MSG_BODY_LEN_MAX];
}HcuMsgSruct_t;
#define HCU_SYSMSG_TASK_THREAD_PRIO             	10          //priority of the main loop de 1 a 99 max

//3. 借用过来的消息及数据结构体定义
/*
 * Type define for Socket
 */
#define		HCU_SYSMSG_MAX_NUM_OF_WED	 			16
#define		HCU_SYSMSG_MAX_LENGTH_CMD_SERIAL 		(256 + 4 + 1)*4 /* 4 for MsgHeader, 1 for FCS */

typedef struct SocketPort
{
	char ServerIpAddress[16];
	UINT16 ServerPort;
	UINT16 SocketFd;
	struct hostent *pHostName;
	struct sockaddr_in ServerSocketAddress;
	struct sockaddr_in ClientSocketAddress;
}HcuComMsgSocketPort_t;

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
}WedSensorProperty_t;

/*
 * WED All Sensors
 */
typedef struct HcuComMsgWedSensors
{
	UINT32 NumOfActiveSensors;
	WedSensorProperty_t Sensor[HCU_SYSMSG_MAX_NUM_OF_WED];
}HcuComMsgWedSensors_t;

typedef struct HcuComMsgMeasureBehavior
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
}HcuComMsgMeasureBehavior_t;


/* data header for msg send to sever */
typedef struct HcuMsgSeverDataSegHeader
{
	UINT32 LenOfData;   /* ST=31;CN=2011;PW=123456;MN=12345678901002;CP=&&DataTime=2012101700100.....;CO-Rtd=0.00,CO-Flag=N;PMB-Rtd=0.00,PMB-Flag=N&& */
	char ST[16];       /* ST: 31 */
	char CN[16];       /* CN: 2011 */
	char PW[16];       /* PW: 123456 */
	char MN[16];       /* MN: 12345677654321 */
	char DataTime[64];
}HcuMsgSeverDataSegHeader_t;

/* System Information */
#define     HCU_SYSMSG_MAX_SYS_INFO_STRING_LENGTH			128
#define     HCU_SYSMSG_MAX_SYS_INFO_INTERFACE_NUMBER		16
typedef struct SysHwInfo
{
	char cpu_processor[HCU_SYSMSG_MAX_SYS_INFO_STRING_LENGTH];
	char cpu_vendor[HCU_SYSMSG_MAX_SYS_INFO_STRING_LENGTH];
	char cpu_modelname[HCU_SYSMSG_MAX_SYS_INFO_STRING_LENGTH];
	char cpu_mhz[HCU_SYSMSG_MAX_SYS_INFO_STRING_LENGTH];
}SysHwInfo_t;

typedef struct SysOsInfo
{
	char version_signature[HCU_SYSMSG_MAX_SYS_INFO_STRING_LENGTH];
}SysOsInfo_t;

typedef struct SysNetworkInfo
{
	int itfnum;
	char itfname[HCU_SYSMSG_MAX_SYS_INFO_INTERFACE_NUMBER][HCU_SYSMSG_MAX_SYS_INFO_STRING_LENGTH];
	char itfip[HCU_SYSMSG_MAX_SYS_INFO_INTERFACE_NUMBER][HCU_SYSMSG_MAX_SYS_INFO_STRING_LENGTH];
	/* whether there is PPP */
	pid_t ppppid;
}SysNetworkInfo_t;

typedef struct HcuComMsgSysInfo
{
	SysHwInfo_t hw;
	SysOsInfo_t os;
	SysNetworkInfo_t net;
}HcuSysOsNetInfo_t;
#define		HCU_SYSMSG_NB_NANOS_IN_ONE_SECOND			1000000000	//number of nanos in one second
#define 	HCU_SYSMSG_NB_NANOS_IN_ONE_MS          		1000000     //number of nanos secondes in 1 ms
#define 	HCU_SYSMSG_NB_MICROS_IN_ONE_SECOND        	1000000     //number of micros secondes in 1 second
#define 	HCU_SYSMSG_NB_MICROS_IN_ONE_MS    	     	1000	    //number of micros secondes in 1 ms

/**************************************************************************************
 *                                                                                    *
 * 【增加消息】总共需要修改三个地方：
 * - HCU_INTER_TASK_MSG_ID
 * - 每定义一个新消息，请去修改vmlayer.c中的变量zHcuMsgNameList[]，不然TRACE会出现消息内容解析的错误
 * - 还要去DBICOM模块中对应的MessageTrace机制一并修改，并备份数据库
 * - 如果需要完美表现，还得最终需要升级L3UI的CRUD，不然相应的工具会出错                       *
 *                                                                                    *
 *************************************************************************************/
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
	MSG_ID_COM_ALARM_REPORT,
	MSG_ID_COM_PM_REPORT,
	MSG_ID_COM_MQTT_SEND,
	MSG_ID_COM_MQTT_RCV,

	//Service Control message

	//ETHERNET
	//因为发送采用了API方式，接收采用了消息方式，所以这里只需要定义接收消息的格式，发送放在缓冲区中，定义在L1COMDEF.H中了
	MSG_ID_ETHERNET_CLOUDVELA_DATA_RX,
	MSG_ID_ETHERNET_NBIOTCJ188_DATA_RX,
	MSG_ID_ETHERNET_NBIOTQG376_DATA_RX,
	MSG_ID_ETHERNET_CLOUDVELA_SOCKET_DATA_RX,

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

	//NBIOT message //IWM/IHM/IGM/IPM
	MSG_ID_NBIOTCJ188_IWM_DATA_REQ,
	MSG_ID_IWM_NBIOTCJ188_DATA_RESP,
	MSG_ID_NBIOTCJ188_IWM_CTRL_REQ,
	MSG_ID_IWM_NBIOTCJ188_CTRL_RESP,
	MSG_ID_NBIOTCJ188_IHM_DATA_REQ,
	MSG_ID_IHM_NBIOTCJ188_DATA_RESP,
	MSG_ID_NBIOTCJ188_IHM_CTRL_REQ,
	MSG_ID_IHM_NBIOTCJ188_CTRL_RESP,
	MSG_ID_NBIOTCJ188_IGM_DATA_REQ,
	MSG_ID_IGM_NBIOTCJ188_DATA_RESP,
	MSG_ID_NBIOTCJ188_IGM_CTRL_REQ,
	MSG_ID_IGM_NBIOTCJ188_CTRL_RESP,
	MSG_ID_NBIOTCJ188_IPM_DATA_REQ,
	MSG_ID_IPM_NBIOTCJ188_DATA_RESP,
	MSG_ID_NBIOTCJ188_IPM_CTRL_REQ,
	MSG_ID_IPM_NBIOTCJ188_CTRL_RESP,
	MSG_ID_NBIOTQG376_IPM_DATA_REQ,
	MSG_ID_IPM_NBIOTQG376_DATA_RESP,
	MSG_ID_NBIOTQG376_IPM_CTRL_REQ,
	MSG_ID_IPM_NBIOTQG376_CTRL_RESP,

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

	//基础传感器
	MSG_ID_EMC_MODBUS_DATA_READ,
	MSG_ID_EMC_MODBUS_CONTROL_CMD,
	MSG_ID_PM25_MODBUS_DATA_READ,
	MSG_ID_PM25_MODBUS_CONTROL_CMD,
	MSG_ID_WINDDIR_MODBUS_DATA_READ,
	MSG_ID_WINDDIR_MODBUS_CONTROL_CMD,
	MSG_ID_WINDSPD_MODBUS_DATA_READ,
	MSG_ID_WINDSPD_MODBUS_CONTROL_CMD,
	MSG_ID_TEMP_MODBUS_DATA_READ,
	MSG_ID_TEMP_SPIBUSARIES_DATA_READ,//SPIBUSARIES
	MSG_ID_TEMP_MODBUS_CONTROL_CMD,
	MSG_ID_TEMP_SPIBUSARIES_CONTROL_CMD,//SPIBUSARIES
	MSG_ID_HUMID_MODBUS_DATA_READ,
	MSG_ID_HUMID_MODBUS_CONTROL_CMD,
	MSG_ID_HSMMP_AVORION_DATA_READ,
	MSG_ID_HSMMP_AVORION_STOP,
	MSG_ID_NOISE_SPSVIRGO_DATA_READ,
	MSG_ID_NOISE_SPSVIRGO_CONTROL_CMD,
	MSG_ID_NOISE_SPSVIRGO_STOP,
	MSG_ID_NOISE_MODBUS_DATA_READ,
	MSG_ID_NOISE_MODBUS_CONTROL_CMD,

	//FM/PM/INVENTORY/SW-PACKAGE
	MSG_ID_CLOUDVELA_SYSPM_ALARM_REQ,
	MSG_ID_SYSPM_CLOUDVELA_ALARM_RESP,
	MSG_ID_SYSPM_CLOUDVELA_ALARM_REPORT,
	MSG_ID_CLOUDVELA_SYSPM_ALARM_CONFIRM,
	MSG_ID_CLOUDVELA_SYSPM_PERFM_REQ,
	MSG_ID_SYSPM_CLOUDVELA_PERFM_RESP,
	MSG_ID_SYSPM_CLOUDVELA_PERFM_REPORT,
	MSG_ID_CLOUDVELA_SYSPM_PERFM_CONFIRM,
	MSG_ID_CLOUDVELA_SYSSWM_INVENTORY_REQ,
	MSG_ID_SYSSWM_CLOUDVELA_INVENTORY_RESP,
	MSG_ID_SYSSWM_CLOUDVELA_INVENTORY_REPORT,
	MSG_ID_CLOUDVELA_SYSSWM_INVENTORY_CONFIRM,
	MSG_ID_CLOUDVELA_SYSSWM_SW_PACKAGE_REQ,
	MSG_ID_SYSSWM_CLOUDVELA_SW_PACKAGE_RESP,
	MSG_ID_SYSSWM_CLOUDVELA_SW_PACKAGE_REPORT,
	MSG_ID_CLOUDVELA_SYSSWM_SW_PACKAGE_CONFIRM,
	MSG_ID_CLOUDVELA_TEST_COMMAND_REQ,
	MSG_ID_CLOUDVELA_TEST_COMMAND_RESP,
	MSG_ID_CLOUDVELA_TEST_COMMAND_REPORT,
	MSG_ID_CLOUDVELA_TEST_COMMAND_CONFIRM,

	//SENSOR-CLOUDVELA
	MSG_ID_CLOUDVELA_EMC_DATA_REQ,
	MSG_ID_EMC_CLOUDVELA_DATA_RESP,
	MSG_ID_CLOUDVELA_EMC_CTRL_REQ,
	MSG_ID_EMC_CLOUDVELA_CTRL_RESP,
	MSG_ID_EMC_CLOUDVELA_DATA_REPORT,
	MSG_ID_CLOUDVELA_EMC_DATA_CONFIRM,
	MSG_ID_CLOUDVELA_PM25_DATA_REQ,
	MSG_ID_PM25_CLOUDVELA_DATA_RESP,
	MSG_ID_CLOUDVELA_PM25_CTRL_REQ,
	MSG_ID_PM25_CLOUDVELA_CTRL_RESP,
	MSG_ID_PM25_CLOUDVELA_DATA_REPORT,
	MSG_ID_CLOUDVELA_PM25_DATA_CONFIRM,
	MSG_ID_CLOUDVELA_TEMP_DATA_REQ,
	MSG_ID_TEMP_CLOUDVELA_DATA_RESP,
	MSG_ID_CLOUDVELA_TEMP_CTRL_REQ,
	MSG_ID_TEMP_CLOUDVELA_CTRL_RESP,
	MSG_ID_TEMP_CLOUDVELA_DATA_REPORT,
	MSG_ID_CLOUDVELA_TEMP_DATA_CONFIRM,
	MSG_ID_CLOUDVELA_HUMID_DATA_REQ,
	MSG_ID_HUMID_CLOUDVELA_DATA_RESP,
	MSG_ID_CLOUDVELA_HUMID_CTRL_REQ,
	MSG_ID_HUMID_CLOUDVELA_CTRL_RESP,
	MSG_ID_HUMID_CLOUDVELA_DATA_REPORT,
	MSG_ID_CLOUDVELA_HUMID_DATA_CONFIRM,
	MSG_ID_CLOUDVELA_WINDDIR_DATA_REQ,
	MSG_ID_WINDDIR_CLOUDVELA_DATA_RESP,
	MSG_ID_CLOUDVELA_WINDDIR_CTRL_REQ,
	MSG_ID_WINDDIR_CLOUDVELA_CTRL_RESP,
	MSG_ID_WINDDIR_CLOUDVELA_DATA_REPORT,
	MSG_ID_CLOUDVELA_WINDDIR_DATA_CONFIRM,
	MSG_ID_CLOUDVELA_WINDSPD_DATA_REQ,
	MSG_ID_WINDSPD_CLOUDVELA_DATA_RESP,
	MSG_ID_CLOUDVELA_WINDSPD_CTRL_REQ,
	MSG_ID_WINDSPD_CLOUDVELA_CTRL_RESP,
	MSG_ID_WINDSPD_CLOUDVELA_DATA_REPORT,
	MSG_ID_CLOUDVELA_WINDSPD_DATA_CONFIRM,
	MSG_ID_CLOUDVELA_HSMMP_DATA_REQ,
	MSG_ID_HSMMP_CLOUDVELA_DATA_RESP,
	MSG_ID_CLOUDVELA_HSMMP_CTRL_REQ,
	MSG_ID_HSMMP_CLOUDVELA_CTRL_RESP,
	MSG_ID_HSMMP_CLOUDVELA_DATA_REPORT,
	MSG_ID_CLOUDVELA_HSMMP_DATA_CONFIRM,
	MSG_ID_PICTURE_CLOUDVELA_DATA_REPORT,
	MSG_ID_CLOUDVELA_PICTURE_DATA_CONFIRM,
	MSG_ID_CLOUDVELA_NOISE_DATA_REQ,
	MSG_ID_NOISE_CLOUDVELA_DATA_RESP,
	MSG_ID_CLOUDVELA_NOISE_CTRL_REQ,
	MSG_ID_NOISE_CLOUDVELA_CTRL_RESP,
	MSG_ID_NOISE_CLOUDVELA_DATA_REPORT,
	MSG_ID_CLOUDVELA_NOISE_DATA_CONFIRM,
	MSG_ID_CLOUDVELA_YCJK_DATA_REQ,
	MSG_ID_YCJK_CLOUDVELA_DATA_RESP,
	MSG_ID_CLOUDVELA_YCJK_CTRL_REQ,
	MSG_ID_YCJK_CLOUDVELA_CTRL_RESP,
	MSG_ID_YCJK_CLOUDVELA_DATA_REPORT,
	MSG_ID_CLOUDVELA_YCJK_DATA_CONFIRM,

	//CONVENIENCE SENSOR-CLOUD
	MSG_ID_CLOUDVELA_AIRPRS_DATA_REQ,
	MSG_ID_AIRPRS_CLOUDVELA_DATA_RESP,
	MSG_ID_AIRPRS_CLOUDVELA_DATA_REPORT,
	MSG_ID_CLOUDVELA_AIRPRS_DATA_CONFIRM,
	MSG_ID_CLOUDVELA_ALCOHOL_DATA_REQ,
	MSG_ID_ALCOHOL_CLOUDVELA_DATA_RESP,
	MSG_ID_ALCOHOL_CLOUDVELA_DATA_REPORT,
	MSG_ID_CLOUDVELA_ALCOHOL_DATA_CONFIRM,
	MSG_ID_CLOUDVELA_CO1_DATA_REQ,
	MSG_ID_CO1_CLOUDVELA_DATA_RESP,
	MSG_ID_CO1_CLOUDVELA_DATA_REPORT,
	MSG_ID_CLOUDVELA_CO1_DATA_CONFIRM,
	MSG_ID_CLOUDVELA_HCHO_DATA_REQ,
	MSG_ID_HCHO_CLOUDVELA_DATA_RESP,
	MSG_ID_HCHO_CLOUDVELA_DATA_REPORT,
	MSG_ID_CLOUDVELA_HCHO_DATA_CONFIRM,
	MSG_ID_CLOUDVELA_LIGHTSTR_DATA_REQ,
	MSG_ID_LIGHTSTR_CLOUDVELA_DATA_RESP,
	MSG_ID_LIGHTSTR_CLOUDVELA_DATA_REPORT,
	MSG_ID_CLOUDVELA_LIGHTSTR_DATA_CONFIRM,
	MSG_ID_CLOUDVELA_PM25SP_DATA_REQ,
	MSG_ID_PM25SP_CLOUDVELA_DATA_RESP,
	MSG_ID_PM25SP_CLOUDVELA_DATA_REPORT,
	MSG_ID_CLOUDVELA_PM25SP_DATA_CONFIRM,
	MSG_ID_CLOUDVELA_TOXICGAS_DATA_REQ,
	MSG_ID_TOXICGAS_CLOUDVELA_DATA_RESP,
	MSG_ID_TOXICGAS_CLOUDVELA_DATA_REPORT,
	MSG_ID_CLOUDVELA_TOXICGAS_DATA_CONFIRM,

	//CANITF
	MSG_ID_CANITF_DATA_REPORT,
	MSG_ID_CANITF_SYSSWM_INVENTORY_REPORT,
	MSG_ID_SYSSWM_CANITF_INVENTORY_CONFIRM,
	MSG_ID_CANITF_SYSSWM_SW_PACKAGE_REPORT,
	MSG_ID_SYSSWM_CANITF_SW_PACKAGE_CONFIRM,

	//后台通信部分：REQ/RESP, REPORT/CONFIRM严格遵循HUITP的成对消息体系
	MSG_ID_CLOUDVELA_L3BFSC_DATA_REQ,
	MSG_ID_L3BFSC_CLOUDVELA_DATA_RESP,
	MSG_ID_L3BFSC_CLOUDVELA_DATA_REPORT,
	MSG_ID_CLOUDVELA_L3BFSC_DATA_CONFIRM,
	MSG_ID_L3BFSC_CLOUDVELA_EVENT_REPORT,
	MSG_ID_CLOUDVELA_L3BFSC_EVENT_CONFIRM,
	MSG_ID_CLOUDVELA_L3BFSC_CTRL_REQ,
	MSG_ID_L3BFSC_CLOUDVELA_CTRL_RESP,
	MSG_ID_L3BFSC_CLOUDVELA_STATISTIC_REPORT,
	MSG_ID_CLOUDVELA_L3BFSC_STATISTIC_CONFIRM,

	MSG_ID_CLOUDVELA_L3BFDF_DATA_REQ,
	MSG_ID_L3BFDF_CLOUDVELA_DATA_RESP,
	MSG_ID_L3BFDF_CLOUDVELA_DATA_REPORT,
	MSG_ID_CLOUDVELA_L3BFDF_DATA_CONFIRM,
	MSG_ID_L3BFDF_CLOUDVELA_EVENT_REPORT,
	MSG_ID_CLOUDVELA_L3BFDF_EVENT_CONFIRM,
	MSG_ID_CLOUDVELA_L3BFDF_CTRL_REQ,
	MSG_ID_L3BFDF_CLOUDVELA_CTRL_RESP,
	MSG_ID_L3BFDF_CLOUDVELA_STATISTIC_REPORT,
	MSG_ID_CLOUDVELA_L3BFDF_STATISTIC_CONFIRM,

	MSG_ID_CLOUDVELA_L3BFHS_DATA_REQ,
	MSG_ID_L3BFHS_CLOUDVELA_DATA_RESP,
	MSG_ID_L3BFHS_CLOUDVELA_DATA_REPORT,
	MSG_ID_CLOUDVELA_L3BFHS_DATA_CONFIRM,
	MSG_ID_L3BFHS_CLOUDVELA_EVENT_REPORT,
	MSG_ID_CLOUDVELA_L3BFHS_EVENT_CONFIRM,
	MSG_ID_CLOUDVELA_L3BFHS_CTRL_REQ,
	MSG_ID_L3BFHS_CLOUDVELA_CTRL_RESP,
	MSG_ID_L3BFHS_CLOUDVELA_STATISTIC_REPORT,
	MSG_ID_CLOUDVELA_L3BFHS_STATISTIC_CONFIRM,

	//BFSC项目：L3BFSC SCALE_WEIGHT / CANITF组合秤
	MSG_ID_L3BFSC_CAN_SYS_CFG_REQ,
	MSG_ID_CAN_L3BFSC_SYS_CFG_RESP,
	MSG_ID_L3BFSC_CAN_SYS_CALI_REQ,
	MSG_ID_CAN_L3BFSC_SYS_CALI_RESP,
	MSG_ID_L3BFSC_CAN_SYS_START_REQ,
	MSG_ID_CAN_L3BFSC_SYS_START_RESP,
	MSG_ID_L3BFSC_CAN_SYS_STOP_REQ,
	MSG_ID_CAN_L3BFSC_SYS_STOP_RESP,
	MSG_ID_CAN_L3BFSC_WS_NEW_READY_EVENT,  	//传感器新数据事件
	MSG_ID_L3BFSC_CAN_WS_COMB_OUT,  		//出料
	MSG_ID_CAN_L3BFSC_WS_COMB_OUT_FB,  		//出料确认
	MSG_ID_L3BFSC_CAN_WS_GIVE_UP,   		//放弃物料
	MSG_ID_CAN_L3BFSC_WS_GIVE_UP_FB,   		//放弃物料确认
	MSG_ID_L3BFSC_CAN_ERROR_INQ_CMD_REQ,  	//差错情况下的查询请求
	MSG_ID_CAN_L3BFSC_ERROR_INQ_CMD_RESP,   //差错情况下的查询反馈
	MSG_ID_USBCAN_L2FRAME_RCV,  			//MYC 2017/05/15

	//BFDF
	MSG_ID_L3BFDF_CAN_SYS_CFG_REQ,
	MSG_ID_CAN_L3BFDF_SYS_CFG_RESP,
	MSG_ID_L3BFDF_CAN_DYN_CAL_REQ,
	MSG_ID_CAN_L3BFDF_DYN_CAL_RESP,
	MSG_ID_CAN_L3BFDF_WS_NEW_READY_EVENT,
	MSG_ID_L3BFDF_CAN_WS_COMB_OUT,
	MSG_ID_CAN_L3BFDF_WS_COMB_OUT_FB,
	MSG_ID_CAN_L3BFDF_BASKET_CLEAN_IND,

	//BFHS
	MSG_ID_L3BFHS_CAN_SYS_CFG_REQ,
	MSG_ID_CAN_L3BFHS_SYS_CFG_RESP,
	MSG_ID_L3BFHS_CAN_CAL_ZERO_REQ,
	MSG_ID_CAN_L3BFHS_CAL_ZERO_RESP,
	MSG_ID_L3BFHS_CAN_CAL_FULL_REQ,
	MSG_ID_CAN_L3BFHS_CAL_FULL_RESP,
	MSG_ID_L3BFHS_CAN_DYN_ZERO_REQ,
	MSG_ID_CAN_L3BFHS_DYN_ZERO_RESP,
	MSG_ID_L3BFHS_CAN_DYN_FULL_REQ,
	MSG_ID_CAN_L3BFHS_DYN_FULL_RESP,
	MSG_ID_CAN_L3BFHS_WS_NEW_READY_EVENT,

	//BFSC项目：BFSCUICOMM
	MSG_ID_INOTIFY_UICOMM_FILE_CHANGE_IND,    //JSON文件变更通知
	MSG_ID_UICOMM_L3BFSC_CMD_REQ,       	//命令请求
	MSG_ID_L3BFSC_UICOMM_CMD_RESP,  		//命令反馈
	MSG_ID_UICOMM_L3BFSC_CFG_REQ,  			//配置信息
	MSG_ID_L3BFSC_UICOMM_CFG_RESP,  		//配置结果
	MSG_ID_UICOMM_L3BFSC_CALI_REQ,  		//校准请求
	MSG_ID_L3BFSC_UICOMM_CALI_RESP,  		//校准反馈
	MSG_ID_UICOMM_CAN_TEST_CMD_REQ,  		//测试命令
	MSG_ID_CAN_UICOMM_TEST_CMD_RESP,  		//测试结果

	//BFDF
	MSG_ID_UICOMM_L3BFDF_CTRL_CMD_REQ,			//人工系统命令
	MSG_ID_L3BFDF_UICOMM_CTRL_CMD_RESP,			//人工系统命令
	//BFHS
	MSG_ID_UICOMM_L3BFHS_CTRL_CMD_REQ,			//人工系统命令
	MSG_ID_L3BFHS_UICOMM_CTRL_CMD_RESP,			//人工系统命令

	//SUI接口公共消息
	MSG_ID_SUI_TEST_CMD_REQ,  				//测试命令
	MSG_ID_SUI_TEST_CMD_RESP,  				//测试结果
	MSG_ID_SUI_STARTUP_IND,					//启动指示
	MSG_ID_SUI_FAULT_IND,					//差错指示
	MSG_ID_SUI_SUSPEND_REQ,
	MSG_ID_SUI_SUSPEND_RESP,
	MSG_ID_SUI_RESUME_REQ,
	MSG_ID_SUI_RESUME_RESP,
	MSG_ID_SUI_STOP_REQ,
	MSG_ID_SUI_STOP_RESP,
	MSG_ID_SUI_HEART_BEAT_REPORT,
	MSG_ID_SUI_HEART_BEAT_CONFIRM,

	//HUICOBUS业务消息
	MSG_ID_HUICOBUS_UIR_INIT_REQ,
	MSG_ID_HUICOBUS_UIR_START_REQ,
	MSG_ID_HUICOBUS_UIR_STOP_REQ,
	MSG_ID_HUICOBUS_UIR_STATIC_CALI_REQ,
	MSG_ID_HUICOBUS_UIR_DYNAMIC_CALI_REQ,
	MSG_ID_HUICOBUS_UIR_TEST_CMD_REQ,
	MSG_ID_HUICOBUS_UIR_ONE_KEY_ZERO_REQ,
	MSG_ID_HUICOBUS_UIR_RESUME_REQ,
	MSG_ID_HUICOBUS_UIR_SUSPEND_REQ,

	//L3AQYCG20
	MSG_ID_L3AQYC_EXG_CTRL_REQ,
	MSG_ID_L3AQYC_EXG_CTRL_RESP,
	MSG_ID_L3AQYC_EXG_DATA_REPORT,

	//ZHBHJT
	MSG_ID_CLOUDVELA_LLCZHB_FRAME_REQ,
	MSG_ID_LLCZHB_CLOUDVELA_FRAME_RESP,
	MSG_ID_LLCZHB_L3MOD_CTRL_REQ,
	MSG_ID_L3MOD_LLCZHB_CTRL_RESP,
	MSG_ID_L3MOD_LLCZHB_DATA_REPORT,
	MSG_ID_ZHBL3MOD_EXG_CTRL_REQ,
	MSG_ID_ZHBL3MOD_EXG_CTRL_RESP,
	MSG_ID_ZHBL3MOD_EXG_DATA_REPORT,

	//L3HATE:测试环境对应的消息
	MSG_ID_L3HATE_TC_START,
	MSG_ID_ETH_L3HATE_FRAME_RCV,
	MSG_ID_SPS_L3HATE_FRAME_RCV,
	MSG_ID_CAN_L3HATE_FRAME_RCV,
	MSG_ID_MQTT_L3HATE_FRAME_RCV,

	//END
	MSG_ID_COM_MAX, //Ending point

}; //end of HCU_INTER_TASK_MSG_ID
#define MSG_ID_END 0xFF  //跟MASK_MSGID_NUM_IN_ONE_TASK设置息息相关，不能随便改动
#define MSG_ID_INVALID 0xFFFFFFFF

/**************************************************************************************
 *                                                                                    *
 *            常量定义                                                                 *
 *                                                                                    *
 *************************************************************************************/
#define HCU_SYSMSG_COMMON_ACTION_NULL 		0
#define HCU_SYSMSG_COMMON_ACTION_YES 		1
#define HCU_SYSMSG_COMMON_ACTION_NO 		2
#define HCU_SYSMSG_COMMON_ACTION_INVALID 	0xFF



/**************************************************************************************
 *                                                                                    *
 *            COMMON MESSAGE STRUCTURE                                                *
 *                                                                                    *
 *************************************************************************************/
typedef struct msgie_struct_com_gps_pos //
{
	char ew;
	UINT32 gpsx;
	char ns;
	UINT32 gpsy;
	UINT32 gpsz;
}msgie_struct_com_gps_pos_t;
typedef struct  sensor_modbus_opertion_general //
{
	UINT32 equId;
	UINT8  powerOnOff;
	UINT32 newEquId;
	UINT32 workCycle;
	UINT32 interSample;  //sample_number
	UINT32 meausTimes;   //sample_cycle
}sensor_modbus_opertion_general_t;
//typedef struct sensor_zhb_transport_format_dl //
//{
//	char   qn[21]; //中环保标准，20c, QN=YYYYMMDDHHMMSSZZZ，用来唯一标识一个命令请求, 用于请求命令或通知命令
//	char   st[6];  //中环保标准，5c, ST=系统编号
//	char   cn[8];  //中环保标准，7c, 命令编号 CN
//	char   pw[7];  //中环保标准， 6c, PW=访问密码
//	char   mn[13]; //中环保标准， 12c, 设备唯一标识MN
//	UINT8  ansFlag;//中环保标准， 3c, 数据包是否拆分及应答标志 Flag，从云后台的接收方向
//}sensor_zhb_transport_format_dl_t;
//typedef struct  sensor_zhb_transport_format_ul //
//{
//	char   qn[21]; //中环保标准，20c, QN=YYYYMMDDHHMMSSZZZ，用来唯一标识一个命令请求, 用于请求命令或通知命令
//	char   st[6];  //中环保标准，5c, ST=系统编号
//	char   cn[8];  //中环保标准，7c, 命令编号 CN
//	char   pw[7];  //中环保标准， 6c, PW=访问密码
//	char   mn[13]; //中环保标准， 12c, 设备唯一标识MN
//	UINT32 pnmu;     //总包号，满足中环保标准要求
//	UINT32 pno;      //当前包号，满足中环保标准要求
//}sensor_zhb_transport_format_ul_t;
typedef struct  sensor_emc_data_element //
{
	UINT32 equipid;
	UINT8  dataFormat;
	UINT32 emcValue;
	msgie_struct_com_gps_pos_t gps;
	UINT32 timeStamp;
	UINT32 nTimes;
	UINT8 onOffLineFlag;
}sensor_emc_data_element_t;
typedef struct  sensor_pm25_data_element //
{
	UINT32 equipid;
	UINT8 dataFormat;
	UINT32 pmTSPValue;
	UINT32 pm1d0Value;
	UINT32 pm2d5Value;
	UINT32 pm10Value;
	msgie_struct_com_gps_pos_t gps;
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
typedef struct  sensor_pm25_bmpd300_data_element //
{
	UINT32 equipid;
	UINT8 dataFormat;
	UINT32 pm2d5Value;
	UINT32 timeStamp;
}sensor_pm25_bmpd300_data_element_t;
typedef struct  sensor_winddir_data_element //
{
	UINT32 equipid;
	UINT8 dataFormat;
	UINT32 winddirValue;
	msgie_struct_com_gps_pos_t gps;
	UINT32 timeStamp;
	UINT32 nTimes;
	UINT8 onOffLineFlag;
}sensor_winddir_data_element_t;
typedef struct  sensor_windspd_data_element //
{
	UINT32 equipid;
	UINT8 dataFormat;
	UINT32 windspdValue;
	msgie_struct_com_gps_pos_t gps;
	UINT32 timeStamp;
	UINT32 nTimes;
	UINT8 onOffLineFlag;
}sensor_windspd_data_element_t;
typedef struct  sensor_temp_data_element //
{
	UINT32 equipid;
	UINT8 dataFormat;
	UINT32 tempValue;
	msgie_struct_com_gps_pos_t gps;
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
typedef struct  sensor_temp_mth01_data_element //
{
	UINT32 equipid;
	UINT8 dataFormat;
	UINT32 tempValue;
	UINT32 timeStamp;
}sensor_temp_mth01_data_element_t;
typedef struct  sensor_temp_bmp180_data_element //
{
	UINT32 equipid;
	UINT8 dataFormat;
	UINT32 tempValue;
	UINT32 timeStamp;
}sensor_temp_bmp180_data_element_t;
typedef struct  sensor_humid_data_element //
{
	UINT32 equipid;
	UINT8 dataFormat;
	UINT32 humidValue;
	msgie_struct_com_gps_pos_t gps;
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
typedef struct  sensor_humid_mth01_data_element //
{
	UINT32 equipid;
	UINT8 dataFormat;
	UINT32 humidValue;
	UINT32 timeStamp;
}sensor_humid_mth01_data_element_t;
typedef struct  sensor_noise_data_element //
{
	UINT32 equipid;
	UINT8 dataFormat;
	UINT32 noiseValue;
	msgie_struct_com_gps_pos_t gps;
	UINT32 timeStamp;
	UINT32 nTimes;
	UINT8 onOffLineFlag;
}sensor_noise_data_element_t;
typedef struct  sensor_ycjk_data_element //
{
	UINT32 equipid;
	UINT8  dataFormat;
	INT32  tempValue;
	INT32  humidValue;
	INT32  winddirValue;
	INT32  windspdValue;
	INT32  noiseValue;
	INT32  pm1d0Value;
	INT32  pm2d5Value;
	INT32  pm10Value;
	INT32  tspValue;
	//msgie_struct_com_gps_pos_t gps;
	UINT32 timeStamp;
	UINT32 nTimes;
	UINT8 onOffLineFlag;
}sensor_ycjk_data_element_t;
typedef struct  sensor_hsmmp_data_element //
{
	UINT32 equipid;
	char hsmmpFdir[HCU_SYSDIM_FILE_NAME_LEN_MAX];
	char hsmmpFname[HCU_SYSDIM_FILE_NAME_LEN_MAX];
	char hsmmpLink[HCU_SYSDIM_FILE_NAME_LEN_MAX];
	msgie_struct_com_gps_pos_t gps;
	UINT32 timeStamp;
	UINT32 nTimes;
	UINT8 onOffLineFlag;
}sensor_hsmmp_data_element_t;
typedef struct sensor_hsmmp_link_element
{
	UINT32 equipid;
	UINT32 linkLen;
	//char   linkName[HCU_SYSDIM_FILE_NAME_LEN_MAX];
	//msgie_struct_com_gps_pos_t gps;
	//UINT32 timeStampStart;
	//UINT32 timeStampEnd;
	//UINT32 nTimes;
	UINT8 alarmFlag;
	UINT32 timeStamp;
}sensor_hsmmp_link_element_t;
typedef struct  sensor_airprs_data_element //
{
	UINT32 equipid;
	UINT8 dataFormat;
	UINT32 airprsValue;
	msgie_struct_com_gps_pos_t gps;
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
typedef struct  sensor_airprs_altitude_bmp180_data_element //
{
	UINT32 equipid;
	UINT8 dataFormat;
	UINT32 altitudeValue;
	UINT32 timeStamp;
}sensor_airprs_altitude_bmp180_data_element_t;
typedef struct  sensor_co1_data_element //
{
	UINT32 equipid;
	UINT8 dataFormat;
	UINT32 co1Value;
	msgie_struct_com_gps_pos_t gps;
	UINT32 timeStamp;
	UINT32 nTimes;
	UINT8 onOffLineFlag;
}sensor_co1_data_element_t;
typedef struct  sensor_lightstr_data_element //
{
	UINT32 equipid;
	UINT8 dataFormat;
	UINT32 lightstrValue;
	msgie_struct_com_gps_pos_t gps;
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
	msgie_struct_com_gps_pos_t gps;
	UINT32 timeStamp;
	UINT32 nTimes;
	UINT8 onOffLineFlag;
}sensor_alcohol_data_element_t;
typedef struct  sensor_alcohol_mq3alco_data_element //
{
	UINT32 equipid;
	UINT8 dataFormat;
	UINT32 alcoholValue;
	UINT32 timeStamp;
}sensor_alcohol_mq3alco_data_element_t;
typedef struct  sensor_hcho_data_element //
{
	UINT32 equipid;
	UINT8 dataFormat;
	UINT32 hchoValue;
	msgie_struct_com_gps_pos_t gps;
	UINT32 timeStamp;
	UINT32 nTimes;
	UINT8 onOffLineFlag;
}sensor_hcho_data_element_t;
typedef struct  sensor_hcho_ze08ch2o_data_element //
{
	UINT32 equipid;
	UINT8 dataFormat;
	UINT32 hchoValue;
	UINT32 timeStamp;
}sensor_hcho_ze08ch2o_data_element_t;
typedef struct  sensor_toxicgas_data_element //
{
	UINT32 equipid;
	UINT8 dataFormat;
	UINT32 toxicgasValue;
	msgie_struct_com_gps_pos_t gps;
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
typedef struct  sensor_toxicgas_zp01voc_data_element //
{
	UINT32 equipid;
	UINT8 dataFormat;
	UINT32 toxicgasValue;
	UINT32 timeStamp;
}sensor_toxicgas_zp01voc_data_element_t;

//BFSC的汇报数据内容
#define IHU_SYSMSG_BH_COM_HEAD_NAME_LEN_MAX 20
typedef struct msgie_struct_bh_com_head
{
	UINT8 msgType;
	char  srcUser[IHU_SYSMSG_BH_COM_HEAD_NAME_LEN_MAX];
	char  destUser[IHU_SYSMSG_BH_COM_HEAD_NAME_LEN_MAX];
	UINT32 timeStamp;
	char  funcFlag[IHU_SYSMSG_BH_COM_HEAD_NAME_LEN_MAX];
}msgie_struct_bh_com_head_t;
typedef struct msgie_struct_bfsc_scale_weight_sta_element
{
	UINT32 combTimes;		//组合总次数
	UINT32 tttTimes;		//组合出料次数
	UINT32 tgvTimes;		//组合放弃次数
	UINT32 combMatNbr;  	//组合的物料数量
	UINT32 tttMatNbr;		//组合出料的物料数量
	UINT32 tgvMatNbr;		//组合放弃的物料数量
	UINT32 combAvgSpeed;	//组合平均速度
	UINT32 totalWeight;		//总共处理的物料总重量
	UINT32 totalMatNbr;  	//总共处理的物料总数量
	UINT32 totalWorkMin;	//连续工作的总分钟数
	UINT32 timeInUnix;  	//报告生成的unix时间，网络可能报告滞留
	UINT32 errNbr;			//出错次数
}msgie_struct_bfsc_scale_weight_sta_element_t;
typedef struct msgie_struct_bfdf_scale_weight_sta_element
{
	UINT32 combTimes;		//组合总次数
	UINT32 tttTimes;		//组合出料次数
	UINT32 tgvTimes;		//组合放弃次数
	UINT32 combMatNbr;  	//组合的物料数量
	UINT32 tttMatNbr;		//组合出料的物料数量
	UINT32 tgvMatNbr;		//组合放弃的物料数量
	UINT32 combAvgSpeed;	//组合平均速度
	UINT64 totalWeight;		//总共处理的物料总重量
	UINT32 totalMatNbr;  	//总共处理的物料总数量
	UINT32 totalWorkMin;	//连续工作的总分钟数
	UINT32 timeInUnix;  	//报告生成的unix时间，网络可能报告滞留
	UINT32 errNbr;			//出错次数
}msgie_struct_bfdf_scale_weight_sta_element_t;
typedef struct msgie_struct_bfhs_scale_weight_sta_element
{
	UINT32	wsIncMatCnt;  		//物料数量
	UINT64	wsIncMatWgt;  		//物料重量
	UINT32  wsNormalCnt;		//正常
	UINT64  wsNormalWgt;		//正常
	UINT32  wsOverCnt;			//超重
	UINT64  wsOverWgt;			//超重
	UINT32  wsUnderTotalCnt;	//欠重
	UINT64	wsUnderTotalWgt;	//欠重
	UINT32  wsUnderTu1Cnt;		//欠重TU1
	UINT64	wsUnderTu1Wgt;		//欠重TU1
	UINT32  wsUnderTu2Cnt;		//欠重TU2
	UINT64	wsUnderTu2Wgt;		//欠重TU2
	UINT32  wsUnspecificCnt;	//非特定
	UINT64	wsUnspecificWgt;	//非特定
	UINT32  wsAvgMatTimes;		//平均进料次数
	UINT32  wsAvgTttTimes;		//平均成功次数
	UINT32  wsAvgTgvTimes;		//平均失败次数
}msgie_struct_bfhs_scale_weight_sta_element_t;


/***************************************************************************************************************
 *                                                                                                             *
 *  消息结构定义                                                                                                 *
 *                                                                                                             *
 *  - 缺省消息都使用UINT32进行定义，在内存不是最重要的制约因素下，这种统一的方式，是为了更加不容易出错，不用在不同            *
 *  - 长度的字型之间进行转换。如果遇到INT类型，直接强制转换即可，符号单独处理                                            *
 *                                                                                                             *
 **************************************************************************************************************/
//message definition
//	MSG_ID_COM_INIT,
typedef struct msg_struct_com_init //
{
	UINT32 length;
}msg_struct_com_init_t;

//MSG_ID_COM_INIT_FEEDBACK,
typedef struct msg_struct_com_init_feedback //
{
	UINT32 length;
}msg_struct_com_init_feedback_t;

//MSG_ID_COM_RESTART,  //L1->重新初始化上下文数据
typedef struct  msg_struct_com_restart //
{
	UINT32 length;
}msg_struct_com_restart_t;

//MSG_ID_COM_STOP,
typedef struct  msg_struct_com_stop //
{
	UINT32 length;
}msg_struct_com_stop_t;

//MSG_ID_COM_COMPLETE,
typedef struct  msg_struct_com_complete //
{
	UINT32 length;
}msg_struct_com_complete_t;

//MSG_ID_COM_TIME_OUT,
typedef struct  msg_struct_com_time_out //
{
	UINT32 timeId;
	UINT8 timeRes;
	UINT32 length;
}msg_struct_com_time_out_t;

//MSG_ID_COM_DEBUG_TEST,
typedef struct  msg_struct_com_debug //
{
	UINT32 testid;
	char testinfo[10];
	UINT32 length;
}msg_struct_com_debug_t;

//MSG_ID_COM_HEART_BEAT,
typedef struct  msg_struct_com_heart_beat //
{
	UINT32 length;
}msg_struct_com_heart_beat_t;

//MSG_ID_COM_HEART_BEAT_FB,
typedef struct  msg_struct_com_heart_beat_fb //
{
	UINT32 length;
}msg_struct_com_heart_beat_fb_t;

//MSG_ID_COM_PROCESS_REBOOT,  //L2->重新创建任务和进程, 包括装载数据。还有一种层次，是L3->重新RESET硬件
typedef struct  msg_struct_com_process_reboot //
{
	UINT32 taskId;
	UINT32 length;
}msg_struct_com_process_reboot_t;


//MSG_ID_COM_ALARM_REPORT,
//for alarm report added by ZSC
typedef struct msg_struct_com_alarm_report
{
	UINT8  usercmdid;
	UINT8  useroptid;
	UINT8  cmdIdBackType;
	UINT8  alarmServerity;
	UINT8  alarmClearFlag;
	UINT8  equID;
	UINT8 alarmType;
	UINT32 alarmContent;
	char   alarmDesc[HCU_SYSDIM_FILE_NAME_LEN_MAX];  //photo file name, only valid when PM25 exceed the threshold
	UINT32 timeStamp;
	UINT32 length;
}msg_struct_com_alarm_report_t;

//MSG_ID_COM_PM_REPORT,
//for PM report added by ZSC
typedef struct msg_struct_com_pm_report
{
	UINT8  usercmdid;
	UINT8  useroptid;
	UINT8  cmdIdBackType;
	UINT32 TaskRestartCnt;
	UINT32 CloudVelaConnCnt;
	UINT32 CloudVelaConnFailCnt;
	UINT32 CloudVelaDiscCnt;
	UINT32 SocketDiscCnt;
	UINT32 cpu_occupy;
	UINT32 cpu_temp;
	UINT32 mem_occupy;
	UINT32 disk_occupy;
	UINT32 timeStamp;
	UINT32 length;
}msg_struct_com_pm_report_t;

//MSG_ID_COM_MQTT_SEND,
#define HCU_SYSMSG_MQTT_DESC_MAX_LEN  800
typedef struct msg_struct_com_mqtt_send
{
	UINT16 srcNode;
	UINT16 destNode;
	UINT16 srcId;
	UINT16 destId;
	UINT16 topicId;
	UINT32 cmdId;
	INT32  cmdValue;
	UINT16 hlcLen;
	char   hlContent[HCU_SYSMSG_MQTT_DESC_MAX_LEN];
	UINT32 length;
}msg_struct_com_mqtt_send_t;

//MSG_ID_COM_MQTT_RCV,
typedef struct msg_struct_com_mqtt_rcv
{
	UINT16 srcNode;
	UINT16 destNode;
	UINT16 srcId;
	UINT16 destId;
	UINT16 topicId;
	UINT32 cmdId;
	INT32  cmdValue;
	UINT16 hlcLen;
	char   hlContent[HCU_SYSMSG_MQTT_DESC_MAX_LEN];
	UINT32 length;
}msg_struct_com_mqtt_rcv_t;

#define HCU_SYSMSG_HWINV_PHY_STATUS_NULL 0
#define HCU_SYSMSG_HWINV_PHY_STATUS_DEACTIVE_TO_ACTIVE 1
#define HCU_SYSMSG_HWINV_PHY_STATUS_ACTIVE_TO_DEACTIVE 2
#define HCU_SYSMSG_HWINV_PHY_STATUS_INVALID 0xFF
//MSG_ID_HWINV_CLOUDVELA_PHY_STATUS_CHG,
typedef struct  msg_struct_hwinv_cloudvela_phy_status_chg //
{
	UINT8 ethStatChg;
	UINT8 usbnetStatChg;
	UINT8 wifiStatChg;
	UINT8 g3g4StatChg;
	UINT32 length;
}msg_struct_hwinv_cloudvela_phy_status_chg_t;

//MSG_ID_ETHERNET_CLOUDVELA_DATA_RX,
//Rx data coming from CLOUD direction, shall be controller data, no need too much data traffic
typedef struct  msg_struct_ethernet_cloudvela_data_rx //
{
	UINT32 length;
	char buf[HCU_SYSMSG_COM_MSG_BODY_LEN_MAX];
}msg_struct_ethernet_cloudvela_data_rx_t;

//MSG_ID_ETHERNET_CLOUDVELA_SOCKET_DATA_RX,
typedef struct  msg_struct_ethernet_cloudvela_socket_data_rx //
{
	UINT32 length;
	char buf[HCU_SYSMSG_COM_MSG_BODY_LEN_MAX];
}msg_struct_ethernet_cloudvela_socket_data_rx_t;

//MSG_ID_TEMP_SPIBUSARIES_CONTROL_CMD,//SPIBUSARIES
typedef struct  msg_struct_temp_spibusaries_control_cmd
{
	UINT32 length;
}msg_struct_temp_spibusaries_control_cmd_t;

//MSG_ID_WIFI_CLOUDVELA_DATA_RX,
typedef struct  msg_struct_wifi_cloudvela_data_rx //
{
	UINT32 length;
	char buf[HCU_SYSMSG_COM_MSG_BODY_LEN_MAX];
}msg_struct_wifi_cloudvela_data_rx_t;

//MSG_ID_USBNET_CLOUDVELA_DATA_RX,
typedef struct  msg_struct_usbnet_cloudvela_data_rx //
{
	UINT32 length;
	char buf[HCU_SYSMSG_COM_MSG_BODY_LEN_MAX];
}msg_struct_usbnet_cloudvela_data_rx_t;

//MSG_ID_3G4G_CLOUDVELA_DATA_RX,
typedef struct  msg_struct_3g4g_cloudvela_data_rx //
{
	UINT32 length;
	char buf[HCU_SYSMSG_COM_MSG_BODY_LEN_MAX];
}msg_struct_3g4g_cloudvela_data_rx_t;

//Used to common all different received data buffer
typedef struct  msg_struct_com_cloudvela_data_rx //
{
	UINT32 length;
	char buf[HCU_SYSMSG_COM_MSG_BODY_LEN_MAX];
}msg_struct_com_cloudvela_data_rx_t;

//MSG_ID_SPS232_MODBUS_DATA_RX,
typedef struct  msg_struct_sps232_modbus_data_rx //
{
	UINT32 length;
	UINT8 buf[HCU_SYSMSG_COM_MSG_BODY_LEN_MAX];
}msg_struct_sps232_modbus_data_rx_t;

//MSG_ID_SPS485_MODBUS_DATA_RX,
typedef struct msg_struct_sps485_modbus_data_rx //
{
	UINT32 length;
	UINT8 buf[HCU_SYSMSG_COM_MSG_BODY_LEN_MAX];
}msg_struct_sps485_modbus_data_rx_t;

//MSG_ID_SPSVIRGO_HSMMP_DATA_RX,
typedef struct msg_struct_spsvirgo_hsmmp_data_rx //
{
	UINT32 length;
	UINT8 buf[HCU_SYSMSG_COM_MSG_BODY_LEN_MAX];
}msg_struct_spsvirgo_hsmmp_data_rx_t;

//MSG_ID_AVORION_HSMMP_DATA_RX,
typedef struct msg_struct_avorion_hsmmp_data_rx //
{
	UINT32 length;
	UINT8 buf[HCU_SYSMSG_COM_MSG_BODY_LEN_MAX];
}msg_struct_avorion_hsmmp_data_rx_t;

//MSG_ID_BLE_HSMMP_DATA_RX,
typedef struct msg_struct_ble_hsmmp_data_rx //
{
	UINT32 length;
	UINT8 buf[HCU_SYSMSG_COM_MSG_BODY_LEN_MAX];
}msg_struct_ble_hsmmp_data_rx_t;

//MSG_ID_BLE_MODBUS_DATA_RX,
typedef struct msg_struct_ble_modbus_data_rx //
{
	UINT32 length;
	UINT8 buf[HCU_SYSMSG_COM_MSG_BODY_LEN_MAX];
}msg_struct_ble_modbus_data_rx_t;

//MSG_ID_LCD_AVORION_DATA_RX,
typedef struct msg_struct_lcd_avorion_data_rx //
{
	UINT32 length;
	UINT8 buf[HCU_SYSMSG_COM_MSG_BODY_LEN_MAX];
}msg_struct_lcd_avorion_data_rx_t;

//MSG_ID_CAMERA_AVORION_DATA_RX,
typedef struct msg_struct_camera_avorion_data_rx //
{
	UINT32 length;
	UINT8 buf[HCU_SYSMSG_COM_MSG_BODY_LEN_MAX];
}msg_struct_camera_avorion_data_rx_t;

//MSG_ID_MICROPHONE_AVORION_DATA_RX,
typedef struct msg_struct_microphone_avorion_data_rx //
{
	UINT32 length;
	UINT8 buf[HCU_SYSMSG_COM_MSG_BODY_LEN_MAX];
}msg_struct_microphone_avorion_data_rx_t;



//SENSOR Data request to MODBUS
//MSG_ID_EMC_MODBUS_DATA_READ,
typedef struct  msg_struct_emc_modbus_data_read //
{
	UINT8  cmdId;
	UINT8  optId;
	UINT8  cmdIdBackType; //指明是瞬时，还是周期性读数
	UINT32 equId;
	UINT32 length;
}msg_struct_emc_modbus_data_read_t;

//MSG_ID_PM25_MODBUS_DATA_READ,
typedef struct  msg_struct_pm25_modbus_data_read //
{
	UINT8  cmdId;
	UINT8  optId;
	UINT8  cmdIdBackType; //指明是瞬时，还是周期性读数
	UINT32 equId;
	UINT32 length;
}msg_struct_pm25_modbus_data_read_t;

//MSG_ID_WINDDIR_MODBUS_DATA_READ,
typedef struct  msg_struct_winddir_modbus_data_read //
{
	UINT8  cmdId;
	UINT8  optId;
	UINT8  cmdIdBackType; //指明是瞬时，还是周期性读数
	UINT32 equId;
	UINT32 length;
}msg_struct_winddir_modbus_data_read_t;

//MSG_ID_WINDSPD_MODBUS_DATA_READ,
typedef struct  msg_struct_windspd_modbus_data_read //
{
	UINT8  cmdId;
	UINT8  optId;
	UINT8  cmdIdBackType; //指明是瞬时，还是周期性读数
	UINT32 equId;
	UINT32 length;
}msg_struct_windspd_modbus_data_read_t;

//MSG_ID_TEMP_MODBUS_DATA_READ,
typedef struct  msg_struct_temp_modbus_data_read //
{
	UINT8  cmdId;
	UINT8  optId;
	UINT8  cmdIdBackType; //指明是瞬时，还是周期性读数
	UINT32 equId;
	UINT32 length;
}msg_struct_temp_modbus_data_read_t;

//MSG_ID_TEMP_SPIBUSARIES_DATA_READ,//SPIBUSARIES
//for SPIBUSARIES
typedef struct  msg_struct_temp_spibusaries_data_read //
{
	UINT8  cmdId;
	UINT8  optId;
	UINT8  cmdIdBackType; //指明是瞬时，还是周期性读数
	UINT32 equId;
	UINT32 length;
}msg_struct_temp_spibusaries_data_read_t;

//MSG_ID_HUMID_MODBUS_DATA_READ,
typedef struct  msg_struct_humid_modbus_data_read //
{
	UINT8  cmdId;
	UINT8  optId;
	UINT8  cmdIdBackType; //指明是瞬时，还是周期性读数
	UINT32 equId;
	UINT32 length;
}msg_struct_humid_modbus_data_read_t;

//MSG_ID_NOISE_MODBUS_DATA_READ,//0x67,103?
typedef struct  msg_struct_noise_modbus_data_read //
{
	UINT8  cmdId;
	UINT8  optId;
	UINT8  cmdIdBackType; //指明是瞬时，还是周期性读数
	UINT32 equId;
	UINT32 length;
}msg_struct_noise_modbus_data_read_t;

//MSG_ID_NOISE_SPSVIRGO_DATA_READ,
typedef struct  msg_struct_noise_spsvirgo_data_read //
{
	UINT8  cmdId;
	UINT8  optId;
	UINT8  cmdIdBackType; //指明是瞬时，还是周期性读数
	UINT32 equId;
	UINT32 length;
}msg_struct_noise_spsvirgo_data_read_t;

//MSG_ID_EMC_MODBUS_CONTROL_CMD,
typedef struct  msg_struct_emc_modbus_control_cmd
{
	UINT8  cmdId;
	UINT8  optId;
	UINT8  backType;
	sensor_modbus_opertion_general_t opt;
	UINT32 length;
}msg_struct_emc_modbus_control_cmd_t;

//MSG_ID_PM25_MODBUS_CONTROL_CMD,
typedef struct  msg_struct_pm25_modbus_control_cmd //
{
	UINT8  cmdId;
	UINT8  optId;
	UINT8  backType;
	sensor_modbus_opertion_general_t opt;
	UINT32 length;
}msg_struct_pm25_modbus_control_cmd_t;

//MSG_ID_WINDDIR_MODBUS_CONTROL_CMD,
typedef struct  msg_struct_winddir_modbus_control_cmd //
{
	UINT8  cmdId;
	UINT8  optId;
	UINT8  backType;
	sensor_modbus_opertion_general_t opt;
	UINT32 length;
}msg_struct_winddir_modbus_control_cmd_t;

//MSG_ID_WINDSPD_MODBUS_CONTROL_CMD,
typedef struct  msg_struct_windspd_modbus_control_cmd //
{
	UINT8  cmdId;
	UINT8  optId;
	UINT8  backType;
	sensor_modbus_opertion_general_t opt;
	UINT32 length;
}msg_struct_windspd_modbus_control_cmd_t;

//MSG_ID_TEMP_MODBUS_CONTROL_CMD,
typedef struct  msg_struct_temp_modbus_control_cmd //
{
	UINT8  cmdId;
	UINT8  optId;
	UINT8  backType;
	sensor_modbus_opertion_general_t opt;
	UINT32 length;
}msg_struct_temp_modbus_control_cmd_t;

//MSG_ID_HUMID_MODBUS_CONTROL_CMD,
typedef struct  msg_struct_humid_modbus_control_cmd //
{
	UINT8  cmdId;
	UINT8  optId;
	UINT8  backType;
	sensor_modbus_opertion_general_t opt;
	UINT32 length;
}msg_struct_humid_modbus_control_cmd_t;

//MSG_ID_NOISE_MODBUS_CONTROL_CMD,
typedef struct  msg_struct_noise_modbus_control_cmd //
{
	UINT8  cmdId;
	UINT8  optId;
	UINT8  backType;
	sensor_modbus_opertion_general_t opt;
	UINT32 length;
}msg_struct_noise_modbus_control_cmd_t;

//MSG_ID_NOISE_SPSVIRGO_CONTROL_CMD,
typedef struct  msg_struct_noise_spsvirgo_control_cmd //
{
	UINT8  cmdId;
	UINT8  optId;
	UINT8  backType;
	sensor_modbus_opertion_general_t opt;
	UINT32 length;
}msg_struct_noise_spsvirgo_control_cmd_t;

//MSG_ID_MODBUS_EMC_DATA_REPORT,
//Data Report message from Modbus
typedef struct  msg_struct_modbus_emc_data_report //
{
	UINT8  usercmdid;
	UINT8  useroptid;
	UINT8  cmdIdBackType; //指明是瞬时，还是周期性读数
	sensor_emc_data_element_t emc;
	UINT32 length;
}msg_struct_modbus_emc_data_report_t;

//MSG_ID_MODBUS_PM25_DATA_REPORT,
typedef struct  msg_struct_modbus_pm25_data_report //
{
	UINT8  usercmdid;
	UINT8  useroptid;
	UINT8  cmdIdBackType; //指明是瞬时，还是周期性读数
	sensor_pm25_data_element_t pm25;
	UINT32 length;
}msg_struct_modbus_pm25_data_report_t;

//MSG_ID_MODBUS_WINDDIR_DATA_REPORT,
typedef struct msg_struct_modbus_winddir_data_report
{
	UINT8  usercmdid;
	UINT8  useroptid;
	UINT8  cmdIdBackType; //指明是瞬时，还是周期性读数
	sensor_winddir_data_element_t winddir;
	UINT32 length;
}msg_struct_modbus_winddir_data_report_t;

//MSG_ID_MODBUS_WINDSPD_DATA_REPORT,
typedef struct msg_struct_modbus_windspd_data_report
{
	UINT8  usercmdid;
	UINT8  useroptid;
	UINT8  cmdIdBackType; //指明是瞬时，还是周期性读数
	sensor_windspd_data_element_t windspd;
	UINT32 length;
}msg_struct_modbus_windspd_data_report_t;

//MSG_ID_MODBUS_TEMP_DATA_REPORT,
typedef struct msg_struct_modbus_temp_data_report
{
	UINT8  usercmdid;
	UINT8  useroptid;
	UINT8  cmdIdBackType; //指明是瞬时，还是周期性读数
	sensor_temp_data_element_t temp;
	UINT32 length;
}msg_struct_modbus_temp_data_report_t;

//MSG_ID_MODBUS_HUMID_DATA_REPORT,
typedef struct msg_struct_modbus_humid_data_report
{
	UINT8  usercmdid;
	UINT8  useroptid;
	UINT8  cmdIdBackType; //指明是瞬时，还是周期性读数
	sensor_humid_data_element_t humid;
	UINT32 length;
}msg_struct_modbus_humid_data_report_t;

//MSG_ID_MODBUS_NOISE_DATA_REPORT,
typedef struct msg_struct_modbus_noise_data_report
{
	UINT8  usercmdid;
	UINT8  useroptid;
	UINT8  cmdIdBackType; //指明是瞬时，还是周期性读数
	sensor_noise_data_element_t noise;
	UINT32 length;
}msg_struct_modbus_noise_data_report_t;

//MSG_ID_SPSVIRGO_NOISE_DATA_REPORT,
typedef struct msg_struct_spsvirgo_noise_data_report
{
	UINT8  usercmdid;
	UINT8  useroptid;
	UINT8  cmdIdBackType; //指明是瞬时，还是周期性读数
	sensor_noise_data_element_t noise;
	UINT32 length;
}msg_struct_spsvirgo_noise_data_report_t;

//MSG_ID_MODBUS_EMC_CONTROL_FB,
typedef struct msg_struct_modbus_emc_control_fb
{
	UINT8  cmdId;
	UINT8  optId;
	UINT8  backType;
	sensor_modbus_opertion_general_t opt;
	UINT32 length;
}msg_struct_modbus_emc_control_fb_t;

//MSG_ID_MODBUS_PM25_CONTROL_FB,
typedef struct msg_struct_modbus_pm25_control_fb
{
	UINT8  cmdId;
	UINT8  optId;
	UINT8  backType;
	sensor_modbus_opertion_general_t opt;
	UINT32 length;
}msg_struct_modbus_pm25_control_fb_t;

//MSG_ID_MODBUS_WINDDIR_CONTROL_FB,
typedef struct msg_struct_modbus_winddir_control_fb
{
	UINT8  cmdId;
	UINT8  optId;
	UINT8  backType;
	sensor_modbus_opertion_general_t opt;
	UINT32 length;
}msg_struct_modbus_winddir_control_fb_t;

//MSG_ID_MODBUS_WINDSPD_CONTROL_FB,
typedef struct msg_struct_modbus_windspd_control_fb
{
	UINT8  cmdId;
	UINT8  optId;
	UINT8  backType;
	sensor_modbus_opertion_general_t opt;
	UINT32 length;
}msg_struct_modbus_windspd_control_fb_t;

//MSG_ID_MODBUS_TEMP_CONTROL_FB,
typedef struct msg_struct_modbus_temp_control_fb
{
	UINT8  cmdId;
	UINT8  optId;
	UINT8  backType;
	sensor_modbus_opertion_general_t opt;
	UINT32 length;
}msg_struct_modbus_temp_control_fb_t;

//MSG_ID_MODBUS_HUMID_CONTROL_FB,
typedef struct msg_struct_modbus_humid_control_fb
{
	UINT8  cmdId;
	UINT8  optId;
	UINT8  backType;
	sensor_modbus_opertion_general_t opt;
	UINT32 length;
}msg_struct_modbus_humid_control_fb_t;

//MSG_ID_MODBUS_NOISE_CONTROL_FB,
typedef struct msg_struct_modbus_noise_control_fb
{
	UINT8  cmdId;
	UINT8  optId;
	UINT8  backType;
	sensor_modbus_opertion_general_t opt;
	UINT32 length;
}msg_struct_modbus_noise_control_fb_t;

//MSG_ID_SPSVIRGO_NOISE_CONTROL_FB,
typedef struct msg_struct_spsvirgo_noise_control_fb
{
	UINT8  cmdId;
	UINT8  optId;
	UINT8  backType;
	sensor_modbus_opertion_general_t opt;
	UINT32 length;
}msg_struct_spsvirgo_noise_control_fb_t;

//MSG_ID_HSMMP_AVORION_DATA_READ,
typedef struct msg_struct_hsmmp_avorion_data_read //
{
	UINT32 equipid;
	UINT32 captureDur; //抓取视频的时间长度
	UINT32 refreshRate; //刷新率
	UINT8  fileType; // FileOperationTypeEnum defined at L2COMDEF.h
	UINT8  boolBackCloud; //指明是否需要将该数据发送到后台
	UINT32 timeStampStart;
	char   fDirName[HCU_SYSDIM_FILE_NAME_LEN_MAX];
	char   fName[HCU_SYSDIM_FILE_NAME_LEN_MAX];
	char   tmpFname[HCU_SYSDIM_FILE_NAME_LEN_MAX];
	UINT32 length;
}msg_struct_hsmmp_avorion_data_read_t;

//MSG_ID_HSMMP_AVORION_STOP,
typedef struct msg_struct_hsmmp_avorion_stop //
{
	UINT32 length;
}msg_struct_hsmmp_avorion_stop_t;

//MSG_ID_AVORION_HSMMP_DATA_READ_FB
typedef struct msg_struct_avorion_hsmmp_data_read_fb //
{
	UINT8  boolBackCloud; //指明是否需要将该数据发送到后台
	sensor_hsmmp_data_element_t hsmmp; //tmpFname暂时被hsmmpLink所替代
	UINT32 length;
}msg_struct_avorion_hsmmp_data_read_fb_t;

//MSG_ID_NOISE_SPSVIRGO_STOP,
typedef struct msg_struct_noise_spsvirgo_stop //
{
	UINT32 length;
}msg_struct_noise_spsvirgo_stop_t;


/**************************************************************************************
 *                                                                                    *
 *  NBIOT CJ188消息结构                                                                *
 *                                                                                    *
 *  - 定义的逻辑是，每条消息都是全集消息，这样针对不同的命令字处理，由层3模块IWM在收到具体的      *
 *    命令后，根据命令字进行分拣，这样大大降低这里消息定义的难度，并且增加其适应性，不会因为      *
 *    不同的内容，导致消息完全不一样                                                      *
 *                                                                                    *
 *************************************************************************************/
//智能表数据结构

typedef struct  sensor_general_cj188_control_head //
{
	INT8  ctrlId;
	INT16  d0d1Id;
	INT8  ser;
	UINT8  periodFlag; //指明是瞬时，还是周期性读数
	UINT8 communicationFlag;  //反应通信是否异常的标示
	char addr[15];
	UINT32 timestamp;
}sensor_general_cj188_control_head_t;
typedef struct  sensor_general_cj188_data_element //
{
	float currentaccuvolume;   //当前累计流量
	INT8 currentaccuvolumeunit;
	float flowvolume;         //流量
	INT8 flowvolumeunit;
	INT8 lastmonth;
	INT32 accumuworktime;     //累计工作时间
	float supplywatertemp;    //供水温度
	float backwatertemp;      //回水温度
	char realtime[15];        //多申请一位， 实时时间
	char st[5];               //多申请一位    状态
	INT8 billtodaydate;       //结算日
	INT8 readamountcurdate;   //抄表日
	INT8 startdate;           //启动日
	INT64 key;
	float price1;
	INT32 volume1;
	float price2;
	INT32 volume2;
	float price3;
	INT8 buycode;
	float thisamount;    //本次金额
	float accuamount;    //累计金额
	float remainamount;   //剩余金额
	INT8 keyver;
	INT8 switchctrl;
}sensor_general_cj188_data_element_t;

//用于数据库的存入和交互
typedef struct  sensor_iwm_cj188_data_element //
{
	char cj188address[15];  //多申请一位
	UINT32 timestamp;
	INT8 equtype;
	float billtodayaccuvolume;
	INT8 billtodayaccuvolumeunit;
	sensor_general_cj188_data_element_t iwm;
}sensor_iwm_cj188_data_element_t;
typedef struct  sensor_igm_cj188_data_element //
{
	char cj188address[14];
	UINT32 timestamp;
	INT8 equtype;
	float billtodayaccuvolume;
	INT8 billtodayaccuvolumeunit;
	sensor_general_cj188_data_element_t igm;
}sensor_igm_cj188_data_element_t;
typedef struct  sensor_ipm_cj188_data_element //
{
	char cj188address[14];
	UINT32 timestamp;
	INT8 equtype;
	float billtodayaccuvolume;
	INT8 billtodayaccuvolumeunit;
	sensor_general_cj188_data_element_t ipm;
}sensor_ipm_cj188_data_element_t;
typedef struct  sensor_ihm_cj188_data_element //
{
	char cj188address[14];
	UINT32 timestamp;
	INT8 equtype;
	float heatpower;    //热功率
	INT8 heatpowerunit;
	float currentheat;   //当前热量
	INT8 currentheatunit;
	float billtodayheat;     //结算日热量
	INT8 billtodayheatunit;
	sensor_general_cj188_data_element_t ihm;
}sensor_ihm_cj188_data_element_t;
typedef struct  sensor_ipm_qg376_data_element //
{
	char ipmaddress[14];
	UINT32 timestamp;
	INT8 equtype;
}sensor_ipm_qg376_data_element_t;

//为了四种仪表IWM/IHM/IGM/IPM定义的通用消息交互结构
//MSG_ID_ETHERNET_NBIOTCJ188_DATA_RX,
typedef struct  msg_struct_ethernet_nbiotcj188_data_rx //
{
	UINT32 length;
	char buf[HCU_SYSMSG_COM_MSG_BODY_LEN_MAX];
}msg_struct_ethernet_nbiotcj188_data_rx_t;

//MSG_ID_NBIOTCJ188_IWM_DATA_REQ
typedef struct  msg_struct_nbiotcj188_iwm_data_req //
{
	INT8 equtype;
	sensor_general_cj188_control_head_t iwmHead;
	float billtodayaccuvolume;
	INT8 billtodayaccuvolumeunit;
	sensor_general_cj188_data_element_t iwmData;
	UINT32 length;
}msg_struct_nbiotcj188_iwm_data_req_t;

//MSG_ID_NBIOTCJ188_IHM_DATA_REQ,
typedef struct  msg_struct_nbiotcj188_ihm_data_req //
{
	INT8 equtype;
	sensor_general_cj188_control_head_t ihmHead;
	float heatpower;
	INT8 heatpowerunit;
	float currentheat;
	INT8 currentheatunit;
	float billtodayheat;
	INT8 billtodayheatunit;
	sensor_general_cj188_data_element_t ihmData;
	UINT32 length;
}msg_struct_nbiotcj188_ihm_data_req_t;

//MSG_ID_NBIOTCJ188_IGM_DATA_REQ,
typedef struct  msg_struct_nbiotcj188_igm_data_req //
{
	INT8 equtype;
	sensor_general_cj188_control_head_t igmHead;
	float billtodayaccuvolume;
	INT8 billtodayaccuvolumeunit;
	sensor_general_cj188_data_element_t igmData;
	UINT32 length;
}msg_struct_nbiotcj188_igm_data_req_t;

//MSG_ID_NBIOTCJ188_IPM_DATA_REQ,
typedef struct  msg_struct_nbiotcj188_ipm_data_req //
{
	INT8 equtype;
	sensor_general_cj188_control_head_t ipmHead;
	float billtodayaccuvolume;
	INT8 billtodayaccuvolumeunit;
	sensor_general_cj188_data_element_t ipmData;
	UINT32 length;
}msg_struct_nbiotcj188_ipm_data_req_t;

//MSG_ID_NBIOTCJ188_IWM_CTRL_REQ,
typedef struct  msg_struct_nbiotcj188_iwm_ctrl_req
{
	INT8 equtype;
	sensor_general_cj188_control_head_t iwmHead;
	float billtodayaccuvolume;
	INT8 billtodayaccuvolumeunit;
	sensor_general_cj188_data_element_t iwmData;
	UINT32 length;
}msg_struct_nbiotcj188_iwm_ctrl_req_t;

//MSG_ID_NBIOTCJ188_IHM_CTRL_REQ
typedef struct  msg_struct_nbiotcj188_ihm_ctrl_req
{
	INT8 equtype;
	sensor_general_cj188_control_head_t ihmHead;
	float heatpower;
	INT8 heatpowerunit;
	float currentheat;
	INT8 currentheatunit;
	float billtodayheat;
	INT8 billtodayheatunit;
	sensor_general_cj188_data_element_t ihmData;
	UINT32 length;
}msg_struct_nbiotcj188_ihm_ctrl_req_t;

//MSG_ID_NBIOTCJ188_IGM_CTRL_REQ
typedef struct  msg_struct_nbiotcj188_igm_ctrl_req
{
	INT8 equtype;
	sensor_general_cj188_control_head_t igmHead;
	float billtodayaccuvolume;
	INT8 billtodayaccuvolumeunit;
	sensor_general_cj188_data_element_t igmData;
	UINT32 length;
}msg_struct_nbiotcj188_igm_ctrl_req_t;

//MSG_ID_NBIOTCJ188_IPM_CTRL_REQ
typedef struct  msg_struct_nbiotcj188_ipm_ctrl_req
{
	INT8 equtype;
	sensor_general_cj188_control_head_t igmHead;
	float billtodayaccuvolume;
	INT8 billtodayaccuvolumeunit;
	sensor_general_cj188_data_element_t igmData;
	UINT32 length;
}msg_struct_nbiotcj188_ipm_ctrl_req_t;

//MSG_ID_IWM_NBIOTCJ188_DATA_RESP,
typedef struct msg_struct_iwm_nbiotcj188_data_resp
{
	INT8 equtype;
	sensor_general_cj188_control_head_t iwmHead;
	float billtodayaccuvolume;
	INT8 billtodayaccuvolumeunit;
	sensor_general_cj188_data_element_t iwmData;
	UINT32 length;
}msg_struct_iwm_nbiotcj188_data_resp_t;

//MSG_ID_IHM_NBIOTCJ188_DATA_RESP,
typedef struct msg_struct_ihm_nbiotcj188_data_resp
{
	INT8 equtype;
	sensor_general_cj188_control_head_t ihmHead;
	float heatpower;
	INT8 heatpowerunit;
	float currentheat;
	INT8 currentheatunit;
	float billtodayheat;
	INT8 billtodayheatunit;
	sensor_general_cj188_data_element_t ihmData;
	UINT32 length;
}msg_struct_ihm_nbiotcj188_data_resp_t;

//MSG_ID_IGM_NBIOTCJ188_DATA_RESP,
typedef struct msg_struct_igm_nbiotcj188_data_resp
{
	INT8 equtype;
	sensor_general_cj188_control_head_t igmHead;
	float billtodayaccuvolume;
	INT8 billtodayaccuvolumeunit;
	sensor_general_cj188_data_element_t igmData;
	UINT32 length;
}msg_struct_igm_nbiotcj188_data_resp_t;

//MSG_ID_IPM_NBIOTCJ188_DATA_RESP,
typedef struct msg_struct_ipm_nbiotcj188_data_resp
{
	INT8 equtype;
	sensor_general_cj188_control_head_t ipmHead;
	float billtodayaccuvolume;
	INT8 billtodayaccuvolumeunit;
	sensor_general_cj188_data_element_t ipmData;
	UINT32 length;
}msg_struct_ipm_nbiotcj188_data_resp_t;

//MSG_ID_IWM_NBIOTCJ188_CTRL_RESP,
typedef struct msg_struct_iwm_nbiotcj188_ctrl_resp
{
	INT8 equtype;
	sensor_general_cj188_control_head_t iwmHead;
	float billtodayaccuvolume;
	INT8 billtodayaccuvolumeunit;
	sensor_general_cj188_data_element_t iwmData;
	UINT32 length;
}msg_struct_iwm_nbiotcj188_ctrl_resp_t;

//MSG_ID_IHM_NBIOTCJ188_CTRL_RESP,
typedef struct msg_struct_ihm_nbiotcj188_ctrl_resp
{
	INT8 equtype;
	sensor_general_cj188_control_head_t ihmHead;
	float heatpower;
	INT8 heatpowerunit;
	float currentheat;
	INT8 currentheatunit;
	float billtodayheat;
	INT8 billtodayheatunit;
	sensor_general_cj188_data_element_t ihmData;
	UINT32 length;
}msg_struct_ihm_nbiotcj188_ctrl_resp_t;

//MSG_ID_IGM_NBIOTCJ188_CTRL_RESP,
typedef struct msg_struct_igm_nbiotcj188_ctrl_resp
{
	INT8 equtype;
	sensor_general_cj188_control_head_t igmHead;
	float billtodayaccuvolume;
	INT8 billtodayaccuvolumeunit;
	sensor_general_cj188_data_element_t igmData;
	UINT32 length;
}msg_struct_igm_nbiotcj188_ctrl_resp_t;

//MSG_ID_IPM_NBIOTCJ188_CTRL_RESP,
typedef struct msg_struct_ipm_nbiotcj188_ctrl_resp
{
	INT8 equtype;
	sensor_general_cj188_control_head_t ipmHead;
	float billtodayaccuvolume;
	INT8 billtodayaccuvolumeunit;
	sensor_general_cj188_data_element_t ipmData;
	UINT32 length;
}msg_struct_ipm_nbiotcj188_ctrl_resp_t;


/**************************************************************************************
 *                                                                                    *
 *                            NBIOT QG376消息结构                                      *
 *                                                                                    *
 *************************************************************************************/
//MSG_ID_ETHERNET_NBIOTQG376_DATA_RX,
typedef struct  msg_struct_ethernet_nbiotqg376_data_rx //
{
	UINT32 length;
	char buf[HCU_SYSMSG_COM_MSG_BODY_LEN_MAX];
}msg_struct_ethernet_nbiotqg376_data_rx_t;

//MSG_ID_NBIOTQG376_IPM_CTRL_REQ,
typedef struct  msg_struct_nbiotqg376_ipm_ctrl_req
{
	UINT8  cmdId;
	UINT8  optId;
	UINT8  backType;
	UINT32 length;
}msg_struct_nbiotqg376_ipm_ctrl_req_t;

//MSG_ID_IPM_NBIOTQG376_DATA_RESP,
typedef struct msg_struct_ipm_nbiotqg376_data_resp
{
	UINT8  usercmdid;
	UINT8  useroptid;
	UINT8  cmdIdBackType; //指明是瞬时，还是周期性读数
	UINT32 length;
}msg_struct_ipm_nbiotqg376_data_resp_t;

//MSG_ID_IPM_NBIOTQG376_CTRL_RESP,
typedef struct msg_struct_ipm_nbiotqg376_ctrl_resp
{
	UINT8  cmdId;
	UINT8  optId;
	UINT8  backType;
	UINT32 length;
}msg_struct_ipm_nbiotqg376_ctrl_resp_t;

//MSG_ID_NBIOTQG376_IPM_DATA_REQ,
typedef struct  msg_struct_nbiotqg376_ipm_data_req //
{
	UINT8  cmdId;
	UINT8  optId;
	UINT8  cmdIdBackType; //指明是瞬时，还是周期性读数
	UINT32 equId;
	UINT32 length;
}msg_struct_nbiotqg376_ipm_data_req_t;

//FM/PM/INVENTORY/SW-PACKAGE
//MSG_ID_CLOUDVELA_SYSPM_ALARM_REQ,
typedef struct msg_struct_cloudvela_spspm_alarm_req
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseReq;
	UINT32 length;
}msg_struct_cloudvela_spspm_alarm_req_t;

//MSG_ID_SYSPM_CLOUDVELA_ALARM_RESP,
typedef struct msg_struct_spspm_cloudvela_alarm_resp
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseResp;
	UINT16 alarmType;
	UINT8  alarmServerity;
	UINT8  alarmClearFlag;
	UINT32 equID;
	UINT32 causeId;
	UINT32 alarmContent;
	UINT32 timeStamp;
	UINT32 length;
}msg_struct_spspm_cloudvela_alarm_resp_t;

//MSG_ID_SYSPM_CLOUDVELA_ALARM_REPORT,
typedef struct msg_struct_spspm_cloudvela_alarm_report
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseReport;
	UINT16 alarmType;
	UINT8  alarmServerity;
	UINT8  alarmClearFlag;
	UINT32 equID;
	UINT32 causeId;
	UINT32 alarmContent;
	UINT32 timeStamp;
	UINT32 length;
}msg_struct_spspm_cloudvela_alarm_report_t;

//MSG_ID_CLOUDVELA_SYSPM_ALARM_CONFIRM,
typedef struct msg_struct_cloudvela_spspm_alarm_confirm
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseConfirm;
	UINT32 length;
}msg_struct_cloudvela_spspm_alarm_confirm_t;

//MSG_ID_CLOUDVELA_SYSPM_PERFM_REQ,
typedef struct msg_struct_cloudvela_spspm_perfm_req
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseReq;
	UINT32 length;
}msg_struct_cloudvela_spspm_perfm_req_t;

//MSG_ID_SYSPM_CLOUDVELA_PERFM_RESP,
typedef struct msg_struct_spspm_cloudvela_perfm_resp
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseResp;
	UINT32 restartCnt;
	UINT32 networkConnCnt;
	UINT32 networkConnFailCnt;
	UINT32 networkDiscCnt;
	UINT32 socketDiscCnt;
	UINT32 cpuOccupy;
	UINT32 memOccupy;
	UINT32 diskOccupy;
	UINT32 cpuTemp;
	UINT32 timeStamp;
	UINT32 length;
}msg_struct_spspm_cloudvela_perfm_resp_t;

//MSG_ID_SYSPM_CLOUDVELA_PERFM_REPORT,
typedef struct msg_struct_spspm_cloudvela_perfm_report
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseReport;
	UINT32 restartCnt;
	//UINT32 networkConnCnt;
	//UINT32 networkConnFailCnt;
	//UINT32 networkDiscCnt;
	UINT32 socketDiscCnt;
	UINT32 cpuOccupy;
	UINT32 memOccupy;
	UINT32 diskOccupy;
	UINT32 cpuTemp;
	UINT32 timeStamp;
	UINT32 length;
}msg_struct_spspm_cloudvela_perfm_report_t;

//MSG_ID_CLOUDVELA_SYSPM_PERFM_CONFIRM,
typedef struct msg_struct_cloudvela_spspm_perfm_confirm
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseConfirm;
	UINT32 length;
}msg_struct_cloudvela_spspm_perfm_confirm_t;

//MSG_ID_CLOUDVELA_SYSSWM_INVENTORY_REQ,
typedef struct msg_struct_cloudvela_sysswm_inventory_req
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseReq;
	UINT8  equEntry;
	UINT16 hwType;
	UINT16 hwId;
	UINT16 swRel;
	UINT16 swVer;
	UINT16 dbVer;
	UINT8  upgradeFlag;
	UINT16 	swCheckSum;
	UINT32  swTotalLen;
	UINT16 	dbCheckSum;
	UINT32  dbTotalLen;
	UINT32 length;
}msg_struct_cloudvela_sysswm_inventory_req_t;
#define HCU_SYSMSG_SYSSWM_FW_UPGRADE_NONE 0
#define HCU_SYSMSG_SYSSWM_FW_UPGRADE_NO 1
#define HCU_SYSMSG_SYSSWM_FW_UPGRADE_YES_STABLE 2
#define HCU_SYSMSG_SYSSWM_FW_UPGRADE_YES_TRIAL 3
#define HCU_SYSMSG_SYSSWM_FW_UPGRADE_YES_PATCH 4
#define HCU_SYSMSG_SYSSWM_FW_UPGRADE_YES_INVALID 0xFF

//MSG_ID_SYSSWM_CLOUDVELA_INVENTORY_RESP,
typedef struct msg_struct_sysswm_cloudvela_inventory_resp
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseResp;
	UINT8  equEntry;
	UINT16 hwType;
	UINT16 hwId;
	UINT16 swRel;
	UINT16 swVer;
	UINT16 dbVer;
	UINT8  upgradeFlag;
	UINT32 length;
}msg_struct_sysswm_cloudvela_inventory_resp_t;

//MSG_ID_SYSSWM_CLOUDVELA_INVENTORY_REPORT,
typedef struct msg_struct_sysswm_cloudvela_inventory_report
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseReport;
	UINT8  equEntry;
	UINT16 hwType;
	UINT16 hwId;
	UINT16 swRel;
	UINT16 swVer;
	UINT16 dbVer;
	UINT8  upgradeFlag;
	UINT32 timeStamp;
	UINT32 length;
}msg_struct_sysswm_cloudvela_inventory_report_t;

//MSG_ID_CLOUDVELA_SYSSWM_INVENTORY_CONFIRM,
typedef struct msg_struct_cloudvela_sysswm_inventory_confirm
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseConfirm;
	UINT8  equEntry;
	UINT16 hwType;
	UINT16 hwId;
	UINT16 swRel;
	UINT16 swVer;
	UINT16 dbVer;
	UINT8  upgradeFlag;
	UINT16 	swCheckSum;
	UINT32  swTotalLen;
	UINT16 	dbCheckSum;
	UINT32  dbTotalLen;
	UINT32 length;
}msg_struct_cloudvela_sysswm_inventory_confirm_t;

//MSG_ID_CLOUDVELA_SYSSWM_SW_PACKAGE_REQ,
#define HCU_SYSMSG_SYSSWM_SW_PACKAGE_BODY_MAX_LEN (HCU_SYSMSG_COM_MSG_BODY_LEN_MAX - 100)
typedef struct msg_struct_cloudvela_sysswm_sw_package_req
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseReq;
	UINT8  equEntry;
	UINT16 hwType;
	UINT16 hwPem;
	UINT16 swRelId;
	UINT16 verId;
	UINT8  upgradeFlag;
	UINT16 segIndex;
	UINT16 segTotal;
	UINT16 segSplitLen;
	UINT16 segValidLen;
	UINT16 segCheckSum;
	UINT8  body[HCU_SYSMSG_SYSSWM_SW_PACKAGE_BODY_MAX_LEN];
	UINT32 length;
}msg_struct_cloudvela_sysswm_sw_package_req_t;

//MSG_ID_SYSSWM_CLOUDVELA_SW_PACKAGE_RESP,
typedef struct msg_struct_sysswm_cloudvela_sw_package_resp
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseResp;
	UINT8  equEntry;
	UINT16 hwType;
	UINT16 hwPem;
	UINT16 swRelId;
	UINT16 verId;
	UINT8  upgradeFlag;
	UINT16 segIndex;
	UINT16 segTotal;
	UINT16 segSplitLen;
	UINT32 length;
}msg_struct_sysswm_cloudvela_sw_package_resp_t;

//MSG_ID_SYSSWM_CLOUDVELA_SW_PACKAGE_REPORT,
typedef struct msg_struct_sysswm_cloudvela_sw_package_report
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseReport;
	UINT8  equEntry;
	UINT16 hwType;
	UINT16 hwPem;
	UINT16 swRelId;
	UINT16 verId;
	UINT8  upgradeFlag;
	UINT16 segIndex;
	UINT16 segTotal;
	UINT16 segSplitLen;
	UINT32 length;
}msg_struct_sysswm_cloudvela_sw_package_report_t;

//MSG_ID_CLOUDVELA_SYSSWM_SW_PACKAGE_CONFIRM,
typedef struct msg_struct_cloudvela_sysswm_sw_package_confirm
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseConfirm;
	UINT8  equEntry;
	UINT16 hwType;
	UINT16 hwPem;
	UINT16 swRelId;
	UINT16 verId;
	UINT8  upgradeFlag;
	UINT16 segIndex;
	UINT16 segTotal;
	UINT16 segSplitLen;
	UINT16 segValidLen;
	UINT16 segCheckSum;
	UINT8  body[HCU_SYSMSG_SYSSWM_SW_PACKAGE_BODY_MAX_LEN];
	UINT32 length;
}msg_struct_cloudvela_sysswm_sw_package_confirm_t;

//MSG_ID_CLOUDVELA_TEST_COMMAND_REQ,
#define HCU_SYSMSG_SYSPM_TEST_COMMAND_DESC_MAX_LEN 50
typedef struct msg_struct_cloudvela_test_command_req
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseReq;
	UINT32 testCmdId;
	UINT32 testCmdPar1;
	UINT32 testCmdPar2;
	UINT32 testCmdPar3;
	UINT32 testCmdPar4;
	char   desc[HCU_SYSMSG_SYSPM_TEST_COMMAND_DESC_MAX_LEN];
	UINT32 length;
}msg_struct_cloudvela_test_command_req_t;

//MSG_ID_CLOUDVELA_TEST_COMMAND_RESP,
typedef struct msg_struct_cloudvela_test_command_resp
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseResp;
	UINT32 testCmdId;
	UINT32 testCmdPar1;
	UINT32 testCmdPar2;
	UINT32 testCmdPar3;
	UINT32 testCmdPar4;
	char   desc[HCU_SYSMSG_SYSPM_TEST_COMMAND_DESC_MAX_LEN];
	UINT32 length;
}msg_struct_cloudvela_test_command_resp_t;

//MSG_ID_CLOUDVELA_TEST_COMMAND_REPORT,
typedef struct msg_struct_cloudvela_test_command_report
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseReport;
	UINT32 testCmdId;
	UINT32 testCmdPar1;
	UINT32 testCmdPar2;
	UINT32 testCmdPar3;
	UINT32 testCmdPar4;
	char   desc[HCU_SYSMSG_SYSPM_TEST_COMMAND_DESC_MAX_LEN];
	UINT32 length;
}msg_struct_cloudvela_test_command_report_t;

//MSG_ID_CLOUDVELA_TEST_COMMAND_CONFIRM,
typedef struct msg_struct_cloudvela_test_command_confirm
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseConfirm;
	UINT32 testCmdId;
	UINT32 testCmdPar1;
	UINT32 testCmdPar2;
	UINT32 testCmdPar3;
	UINT32 testCmdPar4;
	char   desc[HCU_SYSMSG_SYSPM_TEST_COMMAND_DESC_MAX_LEN];
	UINT32 length;
}msg_struct_cloudvela_test_command_confirm_t;

//CANITF
//MSG_ID_CANITF_DATA_REPORT,
typedef struct msg_struct_canitf_data_report
{
	UINT8  usercmdid;
	UINT32 timeStamp;
	UINT32 length;
}msg_struct_canitf_data_report_t;

//MSG_ID_CANITF_SYSSWM_INVENTORY_REPORT,
typedef struct msg_struct_canitf_sysswm_inventory_report
{
	UINT16 hwType;
	UINT16 hwId;
	UINT16 swRel;
	UINT16 swVer;
	UINT8  upgradeFlag;
	UINT32 timeStamp;
	UINT8  nodeId;
	UINT32 length;
}msg_struct_canitf_sysswm_inventory_report_t;

//MSG_ID_SYSSWM_CANITF_INVENTORY_CONFIRM,
typedef struct msg_struct_sysswm_canitf_inventory_confirm
{
	UINT16 swRel;
	UINT16 swVer;
	UINT8  upgradeFlag;
	UINT16 	swCheckSum;
	UINT32  swTotalLengthInBytes;
	UINT32 timeStamp;
	UINT8  nodeId;
	UINT32 length;
}msg_struct_sysswm_canitf_inventory_confirm_t;

//MSG_ID_CANITF_SYSSWM_SW_PACKAGE_REPORT,
typedef struct msg_struct_canitf_sysswm_sw_package_report
{
	UINT16 swRelId;
	UINT16 swVerId;
	UINT8  upgradeFlag;
	UINT16 segIndex;
	UINT16 segTotal;
	UINT16 segSplitLen;
	UINT8  nodeId;
	UINT32 length;
}msg_struct_canitf_sysswm_sw_package_report_t;

//MSG_ID_SYSSWM_CANITF_SW_PACKAGE_CONFIRM,
#define HCU_SYSMSG_CANITF_SYSSWM_SW_PACKAGE_BODY_MAX_LEN 500
#if (HCU_SYSMSG_CANITF_SYSSWM_SW_PACKAGE_BODY_MAX_LEN < HUITP_IEID_SUI_SW_PACKAGE_BODY_MAX_LEN)
	#error Internal SYSMSG parameter setting error!
#endif
typedef struct msg_struct_sysswm_canitf_sw_package_confirm
{
	UINT16 swRelId;
	UINT16 swVerId;
	UINT8  upgradeFlag;
	UINT16 segIndex;
	UINT16 segTotal;
	UINT16 segSplitLen;
	UINT16 segValidLen;
	UINT16 segCheckSum;
	UINT8  swPkgBody[HCU_SYSMSG_CANITF_SYSSWM_SW_PACKAGE_BODY_MAX_LEN];
	UINT8  nodeId;
	UINT32 length;
}msg_struct_sysswm_canitf_sw_package_confirm_t;

/**************************************************************************************
 *
 *  分类处理各个项目相关的消息结构
 *
 *************************************************************************************/
//BFSC/SCALE_WEIGHT组合秤
#define HCU_SYSMSG_L3BFSC_MAX_SENSOR_NBR 20
#define HCU_SYSMSG_SUI_SENSOR_NBR		32 //bitmap可以使用一个U32就搞定．在可遇见的将来，都够用

//后台通信部分：REQ/RESP, REPORT/CONFIRM严格遵循HUITP的成对消息体系
//MSG_ID_CLOUDVELA_L3BFSC_DATA_REQ,
typedef struct msg_struct_cloudvela_l3bfsc_data_req
{
	UINT8  baseReq;
	UINT8  reqType;
	msgie_struct_bh_com_head_t comHead;
	UINT32 length;
}msg_struct_cloudvela_l3bfsc_data_req_t;
//reqType -> StrIe_HUITP_IEID_uni_scale_weight_req_t.reqType (HUITP.H)

//MSG_ID_L3BFSC_CLOUDVELA_DATA_RESP,
typedef struct msg_struct_l3bfsc_cloudvela_data_resp
{
	UINT8  baseResp;
	UINT8  type;			//数据汇报的类型
	UINT8  dataFormat;
	UINT8  snrCfgNbrMax;  	//系统配置的称重传感器最大数
	UINT32 snrUsedBitmap;  	//汇报哪些称重传感器对应的数据
	UINT32 snrValue[HCU_SYSMSG_L3BFSC_MAX_SENSOR_NBR];
	msgie_struct_bfsc_scale_weight_sta_element_t sta;
	msgie_struct_bh_com_head_t comHead;
	UINT32 length;
}msg_struct_l3bfsc_cloudvela_data_resp_t;
//type -> StrIe_HUITP_IEID_uni_scale_weight_req_t.reqType (HUITP.H)

//MSG_ID_L3BFSC_CLOUDVELA_DATA_REPORT,
typedef struct msg_struct_l3bfsc_cloudvela_data_report
{
	UINT8  baseReport;
	UINT8  type;			//数据汇报的类型
	UINT8  dataFormat;
	UINT8  snrCfgNbrMax;  	//系统配置的称重传感器最大数
	UINT32 snrUsedBitmap;  	//汇报哪些称重传感器对应的数据
	UINT32 snrValue[HCU_SYSMSG_L3BFSC_MAX_SENSOR_NBR];
	msgie_struct_bfsc_scale_weight_sta_element_t sta;
	msgie_struct_bh_com_head_t comHead;
	UINT32 length;
}msg_struct_l3bfsc_cloudvela_data_report_t;
//type -> StrIe_HUITP_IEID_uni_scale_weight_req_t.reqType (HUITP.H)

//MSG_ID_CLOUDVELA_L3BFSC_DATA_CONFIRM,
typedef struct msg_struct_cloudvela_l3bfsc_data_confirm
{
	UINT8  baseConfirm;
	msgie_struct_bh_com_head_t comHead;
	UINT32 length;
}msg_struct_cloudvela_l3bfsc_data_confirm_t;

//MSG_ID_L3BFSC_CLOUDVELA_EVENT_REPORT,
#define HCU_SYSMSG_SCALE_WEIGHT_EVENT_USER_NAME_MAX_LEN 20
typedef struct msg_struct_l3bfsc_cloudvela_event_report
{
	UINT8  baseReport;
	UINT8  eventType;
	UINT8  sensorId;
	UINT16 eventValue;
	char   userName[HCU_SYSMSG_SCALE_WEIGHT_EVENT_USER_NAME_MAX_LEN];
	UINT16 cause;
	msgie_struct_bh_com_head_t comHead;
	UINT32 length;
}msg_struct_l3bfsc_cloudvela_event_report_t;
//eventType -> StrIe_HUITP_IEID_uni_scale_weight_event_t.eventType (HUITP.H)

//MSG_ID_CLOUDVELA_L3BFSC_EVENT_CONFIRM,
typedef struct msg_struct_cloudvela_l3bfsc_event_confirm
{
	UINT8  baseConfirm;
	msgie_struct_bh_com_head_t comHead;
	UINT32 length;
}msg_struct_cloudvela_l3bfsc_event_confirm_t;

//MSG_ID_CLOUDVELA_L3BFSC_CTRL_REQ,
typedef struct msg_struct_cloudvela_l3bfsc_ctrl_req
{
	UINT8  baseReq;
	UINT8  scaleWeightCmd;
	UINT8  workMode;
	UINT8  scaleWeightNbr;  		//配置数量
	UINT32 staStartTime;  			//24小时之内，哪个时间作为统计的起点
	UINT32 staReportDuration;     	//统计报告周期
	msgie_struct_bh_com_head_t comHead;
	UINT32 length;
}msg_struct_cloudvela_l3bfsc_ctrl_req_t;
//scaleWeightCmd => StrIe_HUITP_IEID_uni_scale_weight_cmd_t.scaleWeightCmd (HUITP.H)
//workMode=>  StrIe_HUITP_IEID_uni_scale_weight_cfg_par_t(HUITP.H)

//MSG_ID_L3BFSC_CLOUDVELA_CTRL_RESP,
typedef struct msg_struct_l3bfsc_cloudvela_ctrl_resp
{
	UINT8  baseResp;
	UINT8  scaleWeightCmd;
	UINT8  workMode;
	UINT8  scaleWeightNbr;  		//配置数量
	UINT32 staStartTime;  			//24小时之内，哪个时间作为统计的起点
	UINT32 staReportDuration;     	//统计报告周期
	msgie_struct_bh_com_head_t comHead;
	UINT32 length;
}msg_struct_l3bfsc_cloudvela_ctrl_resp_t;
//scaleWeightCmd => StrIe_HUITP_IEID_uni_scale_weight_cmd_t.scaleWeightCmd (HUITP.H)
//workMode=>  StrIe_HUITP_IEID_uni_scale_weight_cfg_par_t(HUITP.H)

//MSG_ID_L3BFSC_CLOUDVELA_STATISTIC_REPORT,
typedef struct msg_struct_l3bfsc_cloudvela_statistic_report
{
	UINT8  baseReport;
	UINT8  staRepType;
	UINT8  dataFormat;
	msgie_struct_bfsc_scale_weight_sta_element_t sta;
	msgie_struct_bh_com_head_t comHead;
	UINT32 length;
}msg_struct_l3bfsc_cloudvela_statistic_report_t;
//staRepType=> StrIe_HUITP_IEID_uni_scale_weight_statistic_t(HUITP.H)

//MSG_ID_CLOUDVELA_L3BFSC_STATISTIC_CONFIRM,
typedef struct msg_struct_cloudvela_l3bfsc_statistic_confirm
{
	UINT8  baseConfirm;
	msgie_struct_bh_com_head_t comHead;
	UINT32 length;
}msg_struct_cloudvela_l3bfsc_statistic_confirm_t;


//MSG_ID_CLOUDVELA_L3BFDF_DATA_REQ,
typedef struct msg_struct_cloudvela_l3bfdf_data_req
{
	UINT8  baseReq;
	UINT8  reqType;
	msgie_struct_bh_com_head_t comHead;
	UINT32 length;
}msg_struct_cloudvela_l3bfdf_data_req_t;

//MSG_ID_L3BFDF_CLOUDVELA_DATA_RESP,
typedef struct msg_struct_l3bfdf_cloudvela_data_resp
{
	UINT8  baseResp;
	UINT8  type;			//数据汇报的类型
	UINT8  dataFormat;
	msgie_struct_bh_com_head_t comHead;
	msgie_struct_bfdf_scale_weight_sta_element_t sta;
	UINT32 length;
}msg_struct_l3bfdf_cloudvela_data_resp_t;

//MSG_ID_L3BFDF_CLOUDVELA_DATA_REPORT,
typedef struct msg_struct_l3bfdf_cloudvela_data_report
{
	UINT8  baseReport;
	UINT8  type;			//数据汇报的类型
	UINT8  dataFormat;
	msgie_struct_bh_com_head_t comHead;
	msgie_struct_bfdf_scale_weight_sta_element_t sta;
	UINT32 length;
}msg_struct_l3bfdf_cloudvela_data_report_t;

//MSG_ID_CLOUDVELA_L3BFDF_DATA_CONFIRM,
typedef struct msg_struct_cloudvela_l3bfdf_data_confirm
{
	UINT8  baseConfirm;
	msgie_struct_bh_com_head_t comHead;
	UINT32 length;
}msg_struct_cloudvela_l3bfdf_data_confirm_t;

//MSG_ID_L3BFDF_CLOUDVELA_EVENT_REPORT,
typedef struct msg_struct_l3bfdf_cloudvela_event_report
{
	UINT8  baseReport;
	UINT8  eventType;
	UINT8  sensorId;
	UINT16 eventValue;
	UINT16 cause;
	msgie_struct_bh_com_head_t comHead;
	UINT32 length;
}msg_struct_l3bfdf_cloudvela_event_report_t;

//MSG_ID_CLOUDVELA_L3BFDF_EVENT_CONFIRM,
typedef struct msg_struct_cloudvela_l3bfdf_event_confirm
{
	UINT8  baseConfirm;
	msgie_struct_bh_com_head_t comHead;
	UINT32 length;
}msg_struct_cloudvela_l3bfdf_event_confirm_t;

//MSG_ID_CLOUDVELA_L3BFDF_CTRL_REQ,
typedef struct msg_struct_cloudvela_l3bfdf_ctrl_req
{
	UINT8  baseReq;
	msgie_struct_bh_com_head_t comHead;
	UINT32 length;
}msg_struct_cloudvela_l3bfdf_ctrl_req_t;

//MSG_ID_L3BFDF_CLOUDVELA_CTRL_RESP,
typedef struct msg_struct_l3bfdf_cloudvela_ctrl_resp
{
	UINT8  baseResp;
	msgie_struct_bh_com_head_t comHead;
	UINT32 length;
}msg_struct_l3bfdf_cloudvela_ctrl_resp_t;

//MSG_ID_L3BFDF_CLOUDVELA_STATISTIC_REPORT,
typedef struct msg_struct_l3bfdf_cloudvela_statistic_report
{
	UINT8  baseReport;
	UINT8  staRepType;
	UINT8  dataFormat;
	msgie_struct_bh_com_head_t comHead;
	msgie_struct_bfdf_scale_weight_sta_element_t sta;
	UINT32 length;
}msg_struct_l3bfdf_cloudvela_statistic_report_t;

//MSG_ID_CLOUDVELA_L3BFDF_STATISTIC_CONFIRM,
typedef struct msg_struct_cloudvela_l3bfdf_statistic_confirm
{
	UINT8  baseConfirm;
	msgie_struct_bh_com_head_t comHead;
	UINT32 length;
}msg_struct_cloudvela_l3bfdf_statistic_confirm_t;

//MSG_ID_CLOUDVELA_L3BFHS_DATA_REQ,
typedef struct msg_struct_cloudvela_l3bfhs_data_req
{
	UINT8  baseReq;
	UINT8  reqType;
	msgie_struct_bh_com_head_t comHead;
	UINT32 length;
}msg_struct_cloudvela_l3bfhs_data_req_t;

//MSG_ID_L3BFHS_CLOUDVELA_DATA_RESP,
typedef struct msg_struct_l3bfhs_cloudvela_data_resp
{
	UINT8  baseResp;
	UINT8  type;			//数据汇报的类型
	UINT8  dataFormat;
	msgie_struct_bh_com_head_t comHead;
	UINT32 length;
}msg_struct_l3bfhs_cloudvela_data_resp_t;

//MSG_ID_L3BFHS_CLOUDVELA_DATA_REPORT,
typedef struct msg_struct_l3bfhs_cloudvela_data_report
{
	UINT8  baseReport;
	UINT8  type;			//数据汇报的类型
	UINT8  dataFormat;
	msgie_struct_bh_com_head_t comHead;
	UINT32 length;
}msg_struct_l3bfhs_cloudvela_data_report_t;

//MSG_ID_CLOUDVELA_L3BFHS_DATA_CONFIRM,
typedef struct msg_struct_cloudvela_l3bfhs_data_confirm
{
	UINT8  baseConfirm;
	msgie_struct_bh_com_head_t comHead;
	UINT32 length;
}msg_struct_cloudvela_l3bfhs_data_confirm_t;

//MSG_ID_L3BFHS_CLOUDVELA_EVENT_REPORT,
typedef struct msg_struct_l3bfhs_cloudvela_event_report
{
	UINT8  baseReport;
	UINT8  eventType;
	UINT8  sensorId;
	UINT16 eventValue;
	UINT16 cause;
	msgie_struct_bh_com_head_t comHead;
	UINT32 length;
}msg_struct_l3bfhs_cloudvela_event_report_t;

//MSG_ID_CLOUDVELA_L3BFHS_EVENT_CONFIRM,
typedef struct msg_struct_cloudvela_l3bfhs_event_confirm
{
	UINT8  baseConfirm;
	msgie_struct_bh_com_head_t comHead;
	UINT32 length;
}msg_struct_cloudvela_l3bfhs_event_confirm_t;

//MSG_ID_CLOUDVELA_L3BFHS_CTRL_REQ,
typedef struct msg_struct_cloudvela_l3bfhs_ctrl_req
{
	UINT8  baseReq;
	msgie_struct_bh_com_head_t comHead;
	UINT32 length;
}msg_struct_cloudvela_l3bfhs_ctrl_req_t;

//MSG_ID_L3BFHS_CLOUDVELA_CTRL_RESP,
typedef struct msg_struct_l3bfhs_cloudvela_ctrl_resp
{
	UINT8  baseResp;
	msgie_struct_bh_com_head_t comHead;
	UINT32 length;
}msg_struct_l3bfhs_cloudvela_ctrl_resp_t;

//MSG_ID_L3BFHS_CLOUDVELA_STATISTIC_REPORT,
typedef struct msg_struct_l3bfhs_cloudvela_statistic_report
{
	UINT8  baseReport;
	UINT8  staRepType;
	UINT8  dataFormat;
	msgie_struct_bh_com_head_t comHead;
	msgie_struct_bfhs_scale_weight_sta_element_t sta;
	UINT32 length;
}msg_struct_l3bfhs_cloudvela_statistic_report_t;

//MSG_ID_CLOUDVELA_L3BFHS_STATISTIC_CONFIRM,
typedef struct msg_struct_cloudvela_l3bfhs_statistic_confirm
{
	UINT8  baseConfirm;
	msgie_struct_bh_com_head_t comHead;
	UINT32 length;
}msg_struct_cloudvela_l3bfhs_statistic_confirm_t;

//BFSC项目：L3BFSC SCALE_WEIGHT组合秤
//MSG_ID_L3BFSC_CAN_SYS_CFG_REQ,
#define HCU_SYSMSG_BFSC_ERR_CODE_INVALIID	  	0
#define HCU_SYSMSG_BFSC_ERR_CODE_UNSPECIFIC  	1
#define HCU_SYSMSG_BFSC_ERR_CODE_TIME_OUT  		2
#define HCU_SYSMSG_BFSC_ERR_CODE_NULL  		0xFFFF

typedef struct msg_struct_l3bfsc_can_sys_cfg_req
{
	UINT8  wsBitmap[HCU_SYSMSG_L3BFSC_MAX_SENSOR_NBR];
	UINT32 length;
}msg_struct_l3bfsc_can_sys_cfg_req_t;

//MSG_ID_CAN_L3BFSC_SYS_CFG_RESP,
typedef struct msg_struct_can_l3bfsc_sys_cfg_resp
{
	UINT8  sensorid;
	UINT8  validFlag;  //是否执行成功
	UINT16 errCode;
	UINT32 length;
}msg_struct_can_l3bfsc_sys_cfg_resp_t;

//MSG_ID_L3BFSC_CAN_SYS_CALI_REQ
typedef struct msg_struct_l3bfsc_can_sys_cali_req
{
	UINT8  sensorid;
	UINT8  cali_mode;
	UINT32 length;
}msg_struct_l3bfsc_can_sys_cali_req_t;

//MSG_ID_CAN_L3BFSC_SYS_CALI_RESP
typedef struct strMsgIe_bfsc_wgtSnrParamaters
{
	UINT32	WeightSensorLoadDetectionTimeMs;		//称台稳定的判断时间
	UINT32	WeightSensorLoadThread;							//称台稳定门限，如果在WeightSensorLoadDetectionTime内，重量变化都小于WeightSensorLoadThread
	UINT32	WeightSensorEmptyThread;
	UINT32	WeightSensorEmptyDetectionTimeMs;
	UINT32	WeightSensorPickupThread;						// NOT for GUI
	UINT32	WeightSensorPickupDetectionTimeMs;	// NOT for GUI
	UINT32	StardardReadyTimeMs;								//???
	UINT32	MaxAllowedWeight;										//如果发现超过这个最大值，说明Sensor出错
	UINT32	RemainDetectionTimeSec;					  // RemainDetionTime in Seconds

	UINT32	WeightSensorInitOrNot;							// NOT for GUI
	UINT32	WeightSensorAdcSampleFreq;
	UINT32	WeightSensorAdcGain;
	UINT32	WeightSensorAdcBitwidth;						// NOT for GUI
	UINT32  WeightSensorAdcValue;								// NOT for GUI
	UINT32	WeightSensorCalibrationZeroAdcValue;// NOT for GUI
	UINT32	WeightSensorCalibrationFullAdcValue;// NOT for GUI
	UINT32	WeightSensorCalibrationFullWeight;
	UINT32	WeightSensorStaticZeroValue;
	UINT32	WeightSensorTailorValue;
	UINT32	WeightSensorDynamicZeroThreadValue;
	UINT32	WeightSensorDynamicZeroHysteresisMs;
	UINT32  WeightSensorFilterCoeff[4];				// NOT for GUI
	UINT32  WeightSensorOutputValue[4];				// NOT for GUI
}strMsgIe_bfsc_wgtSnrParamaters_t;

typedef struct strMsgIe_bfsc_calibration_resp
{
    UINT8   calibration_cmdvalue;
    UINT8   calibration_result;
    UINT8   spare1;
    UINT8   spare2;
    strMsgIe_bfsc_wgtSnrParamaters_t wgtSnrPar;
}strMsgIe_bfsc_calibration_resp_t;
typedef struct msg_struct_can_l3bfsc_sys_cali_resp
{
	UINT8  sensorid;
	UINT8  validFlag;  //是否执行成功
	UINT16 errCode;
	strMsgIe_bfsc_calibration_resp_t cali_resp;
	UINT32 length;
}msg_struct_can_l3bfsc_sys_cali_resp_t;

//MSG_ID_L3BFSC_CAN_SYS_START_REQ,
typedef struct msg_struct_l3bfsc_can_sys_start_req
{
	UINT8  wsBitmap[HCU_SYSMSG_L3BFSC_MAX_SENSOR_NBR];
	UINT32 length;
}msg_struct_l3bfsc_can_sys_start_req_t;

//MSG_ID_CAN_L3BFSC_SYS_START_RESP,
typedef struct msg_struct_can_l3bfsc_sys_start_resp
{
	UINT8  sensorid;
	UINT8  validFlag;  //是否执行成功
	UINT16 errCode;
	UINT32 length;
}msg_struct_can_l3bfsc_sys_start_resp_t;

//MSG_ID_L3BFSC_CAN_SYS_STOP_REQ,
typedef struct msg_struct_l3bfsc_can_sys_stop_req
{
	UINT8  wsBitmap[HCU_SYSMSG_L3BFSC_MAX_SENSOR_NBR];
	UINT32 length;
}msg_struct_l3bfsc_can_sys_stop_req_t;

//MSG_ID_CAN_L3BFSC_SYS_STOP_RESP,
typedef struct msg_struct_can_l3bfsc_sys_stop_resp
{
	UINT8  sensorid;
	UINT8  validFlag;  //是否执行成功
	UINT16 errCode;
	UINT32 length;
}msg_struct_can_l3bfsc_sys_stop_resp_t;

//MSG_ID_CAN_L3BFSC_WS_NEW_READY_EVENT,  	//传感器新数据事件
typedef struct msg_struct_can_l3bfsc_new_ready_event
{
	UINT8  sensorid;
	INT32  sensorWsValue;
	UINT32 length;
}msg_struct_can_l3bfsc_new_ready_event_t;

//MSG_ID_L3BFSC_CAN_WS_COMB_OUT,  		//出料
typedef struct msg_struct_l3bfsc_can_ws_comb_out
{
	UINT8  combnbr;
	UINT8  sensorBitmap[HCU_SYSMSG_L3BFSC_MAX_SENSOR_NBR];
	UINT8  smallest_wmc_id; 								//组合出来的最小的ID，用于分堆
	INT32  delayInMs[HCU_SYSMSG_L3BFSC_MAX_SENSOR_NBR];		//组合出来的每一个ID所对应的延迟
	UINT32 length;
}msg_struct_l3bfsc_can_ws_comb_out_t;

//MSG_ID_CAN_L3BFSC_WS_COMB_OUT_FB,  		//出料确认
typedef struct msg_struct_can_l3bfsc_ws_comb_out_fb
{
	UINT8  sensorid;
	UINT16 errCode;
	UINT8  validFlag; //是否执行成功
	UINT32 length;
}msg_struct_can_l3bfsc_ws_comb_out_fb_t;

//MSG_ID_L3BFSC_CAN_WS_GIVE_UP,   		//放弃物料
typedef struct msg_struct_l3bfsc_can_ws_give_up
{
	UINT8  sensorBitmap[HCU_SYSMSG_L3BFSC_MAX_SENSOR_NBR];
	UINT32 length;
}msg_struct_l3bfsc_can_ws_give_up_t;

//MSG_ID_CAN_L3BFSC_WS_GIVE_UP_FB,   		//放弃物料确认
typedef struct msg_struct_can_l3bfsc_ws_give_up_fb
{
	UINT8  sensorid;
	UINT16 errCode;
	UINT8  validFlag; //是否执行成功
	UINT32 length;
}msg_struct_can_l3bfsc_ws_give_up_fb_t;

//MSG_ID_L3BFSC_CAN_ERROR_INQ_CMD_REQ,  	//差错情况下的查询请求
typedef struct msg_struct_l3bfsc_can_error_inq_cmd_req
{
	UINT8  sensorid;
	UINT8  sensorBitmap[HCU_SYSMSG_L3BFSC_MAX_SENSOR_NBR];
	UINT16 errCode;
	UINT32 length;
}msg_struct_l3bfsc_can_error_inq_cmd_req_t;

//MSG_ID_CAN_L3BFSC_ERROR_INQ_CMD_RESP,   //差错情况下的查询反馈
typedef struct msg_struct_can_l3bfsc_error_inq_cmd_resp
{
	UINT8  sensorid;
	UINT32 sensorWsValue;
	UINT16 errCode;
	UINT8  validFlag; //是否执行成功
	UINT32 length;
}msg_struct_can_l3bfsc_error_inq_cmd_resp_t;

//MSG_ID_USBCAN_L2FRAME_RCV,
#define HCU_SYSMSG_BFSC_USBCAN_MAX_RX_BUF_SIZE 256
#define HCU_SYSMSG_USBCAN_MAX_RX_BUF_SIZE 	HCU_SYSMSG_BFSC_USBCAN_MAX_RX_BUF_SIZE
typedef struct msg_struct_usbcan_l2frame_rcv
{
	UINT8  nodeId;
	UINT8  databuf[HCU_SYSMSG_BFSC_USBCAN_MAX_RX_BUF_SIZE+1];  //为了防止尾巴重叠
	UINT32 validDataLen;
	UINT32 length;
}msg_struct_usbcan_l2frame_rcv_t;

//BFSCUICOMM
//BFSCUI Json文件修改通知
//MSG_ID_INOTIFY_UICOMM_FILE_CHANGE_IND
typedef struct msg_struct_inotify_uicomm_file_change_ind
{
	UINT32 	length;
}msg_struct_inotify_uicomm_file_change_ind_t;

//MSG_ID_UICOMM_L3BFSC_CMD_REQ,
#define HCU_SYSMSG_BFSC_UICOMM_CMDID_INVALL  			0
#define HCU_SYSMSG_BFSC_UICOMM_CMDID_START  			1
#define HCU_SYSMSG_BFSC_UICOMM_CMDID_STOP  				2
#define HCU_SYSMSG_BFSC_UICOMM_CMDID_SUSPEND  			3
#define HCU_SYSMSG_BFSC_UICOMM_CMDID_RESUME  			4
#define HCU_SYSMSG_BFSC_UICOMM_CMDID_NULL  				255
typedef struct msg_struct_uicomm_l3bfsc_cmd_req
{
	UINT8  cmdid;
	UINT32 length;
}msg_struct_uicomm_l3bfsc_cmd_req_t;
//MSG_ID_L3BFSC_UICOMM_CMD_RESP,
typedef struct msg_struct_l3bfsc_uicomm_cmd_resp
{
	UINT8   cmdid;
	UINT8   validFlag;  //是否执行成功
	UINT16  errCode;
	UINT8	sensorid;
	UINT32  length;
}msg_struct_l3bfsc_uicomm_cmd_resp_t;

//MSG_ID_UICOMM_L3BFSC_CFG_REQ
typedef struct msg_struct_uicomm_l3bfsc_cfg_req
{
	UINT32 	length;
}msg_struct_uicomm_l3bfsc_cfg_req_t;

//MSG_ID_L3BFSC_UICOMM_CFG_RESP,  		//配置结果
typedef struct msg_struct_l3bfsc_uicomm_cfg_resp
{
	UINT8	sensorid;
	UINT8   validFlag;  //是否执行成功
	UINT16  errCode;
	UINT32 	length;
}msg_struct_l3bfsc_uicomm_cfg_resp_t;

/*
 * LZH@2018/4/19：
 * 为配合HUITP接口上将BFSC校准过程独立，在BFSCUICOMM<->L3BFSC接口上将原来通过test_cmd发送到CAN的校准消息转而通过独立的calibration消息发送给L3BFSC
 */
//MSG_ID_UICOMM_L3BFSC_CALI_REQ
typedef struct msg_struct_uicomm_l3bfsc_calibration_req
{
	UINT8	cali_mode;
	UINT8	sensorid;
	UINT32 	length;
}msg_struct_uicomm_l3bfsc_calibration_req_t;

//MSG_ID_L3BFSC_UICOMM_CALI_RESP,  		//配置结果
typedef struct msg_struct_l3bfsc_uicomm_calibration_resp
{
	UINT8	cali_mode;
	UINT8	cali_result;
	UINT8	sensorid;
	UINT8   validFlag;  //是否执行成功
	UINT16  errCode;
	UINT32  adcValue;
	UINT32 	length;
}msg_struct_l3bfsc_uicomm_calibration_resp_t;

//MSG_ID_UICOMM_CAN_TEST_CMD_REQ,  		//测试命令
typedef struct msg_struct_uicomm_can_test_cmd_req
{
	UINT8   cmdid;
	UINT8  	wsBitmap[HCU_SYSMSG_L3BFSC_MAX_SENSOR_NBR];
	UINT32 	command_flags;
	UINT32 	cmdvalue;
	UINT16 	length;
}msg_struct_uicomm_can_test_cmd_req_t;

//MSG_ID_CAN_UICOMM_TEST_CMD_RESP,  		//测试结果
typedef struct msg_struct_can_uicomm_test_cmd_resp
{
	UINT8   cmdid;
	UINT8	sensorid;
	UINT8   validFlag;  //是否执行成功
	UINT16  errCode;
	UINT32  cmdvalue1;
	UINT32  cmdvalue2;
	UINT32 	length;
}msg_struct_can_uicomm_test_cmd_resp_t;


/*****************************BFHS*********************************/
//MSG_ID_UICOMM_L3BFHS_CTRL_CMD_REQ,
#define HCU_SYSMSG_BFHS_UICOMM_CMDID_NULL  				0
#define HCU_SYSMSG_BFHS_UICOMM_CMDID_CFG_START  		1
#define HCU_SYSMSG_BFHS_UICOMM_CMDID_STOP  				2
#define HCU_SYSMSG_BFHS_UICOMM_CMDID_SUSPEND  			3
#define HCU_SYSMSG_BFHS_UICOMM_CMDID_RESUME  			4
#define HCU_SYSMSG_BFHS_UICOMM_CMDID_STATIC_CALI		5
#define HCU_SYSMSG_BFHS_UICOMM_CMDID_DYNAMIC_CALI		6
#define HCU_SYSMSG_BFHS_UICOMM_CMDID_ONE_KEY_ZERO		7
#define HCU_SYSMSG_BFHS_UICOMM_CMDID_TEST				8
#define HCU_SYSMSG_BFHS_UICOMM_CMDID_INVALID  			0xFF

//BFHS CMD_VALUE
#define HCU_SYSMSG_BFHS_UICOMM_CMDVALUE_NULL  							0
#define HCU_SYSMSG_BFHS_UICOMM_CMDVALUE_STATIC_CALI_ZERO  				1
#define HCU_SYSMSG_BFHS_UICOMM_CMDVALUE_STATIC_CALI_FULL  				2
#define HCU_SYSMSG_BFHS_UICOMM_CMDVALUE_DYNAMIC_CALI_ZERO		  		3
#define HCU_SYSMSG_BFHS_UICOMM_CMDVALUE_DYNAMIC_CALI_FULL		  		4
#define HCU_SYSMSG_BFHS_UICOMM_CMDVALUE_INVALID							0xFF

typedef struct StrMsgIe_WeightSensorBfhsCalibrationZeroParamaters
{
	UINT32  WeightSensorFilterCutOffFreqHz; //object 0x2061,the same function as above, LPF cutoff freq, fs=1KHz, 0<= cut <=fs/2
	UINT32  WeightSensorAutoZeroCaptureRangeGrams; //object 0x2076, act. zero point - capture range <=new zero point<= act. zero point + capture range
	UINT32  WeightSensorStandstillRangeGrams; //object 0x2087, Standstill monitoring facilitates detecting a stable weight value, The standstill range specifies the accuracy of internal standstill
	UINT16  WeightSensorAutoZeroAutotaringTimeMs; //object 0x2075, should be multiply of 50ms, zero tracking interval = 2*this value;
    UINT16  WeightSensorPreloadComPensationValuePercent; //object 0x2085, default is 6.25(%), limited range [6.25,50]
	UINT16  WeightSensorPreloadComPensationPlacesAfterDecimalPoint; //location of decimal point for Preload Compensation, for example,6.25%, this value is 2.
							//detection, If the standstill range that is selected is too small, the result can be that standstill will never be detected
	UINT16  WeightSensorStandstillTimeoutMs; //object 0x2088, default value is 10000ms, time wait for large than this value,will generate an error
	UINT16  WeightSensorStandstillTime; //object 0x2089, only for firmware(FS276/FS911, combined with 0x2087)
	UINT16  WeightSensorRingBufTimeMs; //object 0x2060, Default is 100ms to moving average
	//UINT8   WeightSensorFilterCoeff;  //NOT for GUI, object 0x2011, [0...255], default 10th LPF, calc cutoff freq according to this value
	UINT8   WeightSensorMeasurementRangeNo; //object 0x2040, Default is 0, set measurement range no(totally 3),which is displayed in 0x2041
	UINT8   WeightSensorAutoZero;    //object 0x2074, 0:off 1:On
	UINT8   spare1;
	UINT8   spare2;
}StrMsgIe_WeightSensorBfhsCalibrationZeroParamaters_t;

typedef struct StrMsgIe_WeightSensorBfhsCalibrationFullParamaters
{
	UINT32  WeightSensorAdjustingWeightGrams;      //object 0x2080, adjusting weight,first set this value, then combined with command 'C'
	UINT32  WeightSensorAdjustingTolerancePercent; //object0x2082, Current adjusting factor = 0.500000, adjusting tolerance = 1 %,The new factor must lie in the range 0.495000 ≤ Factornew ≤ 0.505000
}StrMsgIe_WeightSensorBfhsCalibrationFullParamaters_t;

typedef struct strMsgIe_bfhs_MotoCtrlParamaters
{
	UINT32	MotorSpeed;
	UINT32	MotorDirection;									//0: Clockwise; 1: Counter-Clockwise
}strMsgIe_bfhs_MotoCtrlParamaters_t;

//MSG_ID_L3BFHS_UICOMM_CTRL_CMD_REQ
typedef struct msg_struct_uicomm_l3bfhs_ctrl_cmd_req
{
	UINT8  cmdid;
	UINT32 length;
	UINT32 cmdValue;
}msg_struct_uicomm_l3bfhs_ctrl_cmd_req_t;

//MSG_ID_L3BFHS_UICOMM_CTRL_CMD_RESP,
typedef struct StrMsgIe_WeightSensorBfhsCalibrationFullRespParamaters
{
	UINT32  WeightSensorFilterCutOffFreqHz; //object 0x2061,the same function as above, LPF cutoff freq, fs=1KHz, 0<= cut <=fs/2
    UINT32  WeightSensorCurrentZeroPointGrams; //object 0x2070, This value is displayed here as a weight. It contains information about how
																						 //far apart the zero point is from the lower limit of the A/D converter range
	UINT32  WeightSensorReferenceZeroPointGrams; /*object 0x2071, Zero setting can be performed for the Weigh Cell with the commands "T" or
																								"Z", but only if the new zero point lies within the permissible zero setting range.
																									Condition 1:
																										new zero point>=reference zero point - neg. zero setting range
																							  Condition 2:
																									new zero point <= reference zero point + pos. zero setting range */
	UINT32  WeightSensorNegativeZeroSettingRangeGrams; //object 0x2072;
	UINT32  WeightSensorPositiveZeroSettingRangeGrams; //object 0x2073;
	UINT32  WeightSensorMeasurementRange; //object0x2041
	UINT32  WeightSensorScaleIntervalValue; //object0x2043
	UINT32  WeightSensorCalibrationValue; //object0x2044
	UINT32  WeightSensorAutoZeroCaptureRangeGrams; //object 0x2076, act. zero point - capture range <=new zero point<= act. zero point + capture range

	UINT32  WeightSensorAdjustingWeightGrams;      //object 0x2080, adjusting weight,first set this value, then combined with command 'C'
	UINT32  WeightSensorAdjustingFactor;      //object 0x2081, (Pnts ZeroPnt) AdjustingFactor Weight* 10^decimal point
	UINT32  WeightSensorAdjustingTolerancePercent; //object0x2082, Current adjusting factor = 0.500000, adjusting tolerance = 1 %,The new factor must lie in the range 0.495000 ≤ Factornew ≤ 0.505000
	UINT32  WeightSensorStandstillRangeGrams; //object 0x2087, Standstill monitoring facilitates detecting a stable weight value, The standstill range specifies the accuracy of internal standstill
							//detection, If the standstill range that is selected is too small, the result can be that standstill will never be detected
	INT16  WeightSensorTemperatureInMagnetSystem; //object 0x2300, sub index1 format NF2
	//UINT16  TemperatureInMagnetSystemPlacesAfterDecimalPoint;//
	INT16  WeightSensorTemperatureAtMeasuringShunt; //object 0x2300, sub index2 format NF2
	//UINT16  TemperatureAtMeasuringShuntPlacesAfterDecimalPoint;//
	UINT16  WeightSensorSamplingFreqHz; //object 0x2049, sampling freq default is 1000Hz
	UINT16  WeightSensorRingBufTimeMs; //object 0x2060, Default is 100ms to moving average
	UINT16  WeightSensorAutoZeroAutotaringTimeMs; //object 0x2075, should be multiply of 50ms, zero tracking interval = 2*this value;

	UINT16  WeightSensorPreloadComPensationValuePercent; //object 0x2085, default is 6.25(%), limited range [6.25,50]
	UINT16  WeightSensorPreloadComPensationPlacesAfterDecimalPoint; //

	UINT16  WeightSensorStandstillTimeoutMs; //object 0x2088, default value is 10000ms, time wait for large than this value,will generate an error
	UINT16  WeightSensorStandstillTime; //object 0x2089, only for firmware(FS276/FS911, combined with 0x2087)
	UINT8   WeightSensorMeasurementRangeNo; //object0x2040
	UINT8   WeightSensorPlacesAfterDecimalPoint; //object0x2042
	UINT8   WeightSensorUintString[64]; //object 0x2045, The unit in which weight values are displayed.
	UINT8   WeightSensorAutoZero;    //object 0x2074, 0:off 1:On
	UINT8   WeightSensorCellAddress; //object 0x2098, node ID = cell address +48
	UINT8   WeightSensorTimeGrid;  //object 0x2222, send weight value in a fixed time grid.
	INT32   Weight; //format NF2;
    UINT8   spare2;
}StrMsgIe_WeightSensorBfhsCalibrationFullRespParamaters_t;

typedef struct msg_struct_l3bfhs_uicomm_ctrl_cmd_resp
{
	UINT8   cmdid;
	UINT16  cmdValue;
	UINT8   validFlag;  //是否执行成功
	UINT16  errCode;
	UINT8   iteration;
	UINT32  dynCaliCoeff; //NF5 format
	INT32   weight; //format NF2;
	StrMsgIe_WeightSensorBfhsCalibrationFullRespParamaters_t calFullRespPar;
	UINT32  length;
}msg_struct_l3bfhs_uicomm_ctrl_cmd_resp_t;

//MSG_ID_L3BFHS_CAN_SYS_CFG_REQ,
//#define HCU_SYSMSG_L3BFHS_MAX_SNC_BOARD_NBR		8
#define HCU_SYSMSG_BFHS_ERR_CODE_INVALIID	  	0
#define HCU_SYSMSG_BFHS_ERR_CODE_UNSPECIFIC  	1
#define HCU_SYSMSG_BFHS_ERR_CODE_TIME_OUT  		2
#define HCU_SYSMSG_BFHS_ERR_CODE_NULL  		0xFFFF

typedef struct strMsgIe_bfhs_WgtSnrParamaters
{
	UINT32  WeightSensorAutoZeroCaptureRangeGrams; //object 0x2076, act. zero point - capture range <=new zero point<= act. zero point + capture range
    UINT32  WeightSensorStandstillRangeGrams; //object 0x2087, Standstill monitoring facilitates detecting a stable weight value, The standstill range specifies the accuracy of internal standstill
							//detection, If the standstill range that is selected is too small, the result can be that standstill will never be detected
	UINT32	MaxAllowedWeight;				//称重物品的范围上限 NF2 format
	UINT32  MinAllowedWeight;        //称重物品的范围下限 NF2 format
	UINT32  WeightSensorFilterCutOffFreqHz; //object 0x2061,the same function as above, LPF cutoff freq, fs=1KHz, 0<= cut <=fs/2
	UINT16  WeightSensorRingBufTimeMs; //object 0x2060, Default is 100ms to moving average
	UINT16  WeightSensorAutoZeroAutotaringTimeMs; //object 0x2075, should be multiply of 50ms, zero tracking interval = 2*this value;
    UINT16  WeightSensorPreloadComPensationValuePercent; //object 0x2085, default is 6.25(%), limited range [6.25,50]
	UINT16  WeightSensorPreloadComPensationPlacesAfterDecimalPoint; //location of decimal point for Preload Compensation, for example,6.25%, this value is 2.
							//detection, If the standstill range that is selected is too small, the result can be that standstill will never be detected
	UINT16  WeightSensorStandstillTimeoutMs; //object 0x2088, default value is 10000ms, time wait for large than this value,will generate an error
	UINT16  WeightSensorStandstillTime; //object 0x2089, only for firmware(FS276/FS911, combined with 0x2087)
	UINT8   WeightSensorMeasurementRangeNo; //object 0x2040, Default is 0, set measurement range no(totally 3),which is displayed in 0x2041
	UINT8   WeightSensorAutoZero;    //object 0x2074, 0:off 1:On
	UINT8   WeightSensorTimeGrid;  //object 0x2222, send weight value in a fixed time grid.
	UINT8   WeightSensorAlgoSelect;  //weight algorithm select
	UINT32  WeightSensorReadStartMs;  //Weight sensor start sampling after infrared detector trigger
	UINT32  WeightSensorReadStopMs;   //Weight sensor stop sampling after infrared detector trigger
	UINT32  WeightSensorDynCaliCoeff; //NF5 format
	UINT32  WeightSensorTareWeight;			//Tare weight of each material
	UINT32  WeightSensorTargetThroughput;	//Selection stop as given 'this para' pcs good material
	UINT32  WeightSensorAlgoAutoZeroSignal; //If autozero cannot be done during 'this para' pcs input, there will be a warning for operator
	UINT32	WeightSensorUpperLimit;
	UINT32	WeightSensorAlgoTu1Limit;
	UINT32	WeightSensorAlgoTu2Limit;
	UINT32	WeightSensorAlgoMaxTu1Ratio;  	//NF2, for example, 250 means 2.5%
	UINT8	WeightSensorAlgoRejectOption;
}strMsgIe_bfhs_WgtSnrParamaters_t;

typedef struct strMsgIe_bfhs_ArmCtrlParamaters
{
	UINT32	ArmRollingStartMs;						//how long do the arm rolling for start action
	UINT32	ArmRollingStopMs;							//how long do the arm rolling for stop action
	UINT32	ArmStartActionMs;					//The time delay of arm start action after infrared detector trigger
	UINT32	ArmRollingIntervalMs;					//If the arm is rolling, how long the motor will stay in still before roll back (stop action).
	UINT32	ArmFailureDetectionVaration;	// % of the MotorSpeed
	UINT32	ArmFailureDetectionTimeMs;		// within TimeMs, 如果速度都在外面，认为故障
}strMsgIe_bfhs_ArmCtrlParamaters_t;
typedef struct msg_struct_l3bfhs_can_sys_cfg_req
{
	UINT8  boardBitmap[HCU_SYSMSG_SUI_SENSOR_NBR];
	strMsgIe_bfhs_WgtSnrParamaters_t  	wgtSnrPar;
	strMsgIe_bfhs_MotoCtrlParamaters_t  motoCtrlPar;
	strMsgIe_bfhs_ArmCtrlParamaters_t   armCtrlPar;
	UINT32 length;
}msg_struct_l3bfhs_can_sys_cfg_req_t;

//MSG_ID_CAN_L3BFHS_SYS_CFG_RESP,
typedef struct msg_struct_can_l3bfhs_sys_cfg_resp
{
	UINT8  snrId;
	UINT8  validFlag;  //是否执行成功
	UINT16 errCode;
	UINT32 length;
}msg_struct_can_l3bfhs_sys_cfg_resp_t;

//MSG_ID_L3BFHS_CAN_CAL_ZERO_REQ,
typedef struct msg_struct_l3bfhs_can_cal_zero_req
{
	UINT8  boardBitmap[HCU_SYSMSG_SUI_SENSOR_NBR];
	StrMsgIe_WeightSensorBfhsCalibrationZeroParamaters_t calZeroPar;
	UINT32 length;
}msg_struct_l3bfhs_can_cal_zero_req_t;

//MSG_ID_CAN_L3BFHS_CAL_ZERO_RESP,
typedef struct msg_struct_can_l3bfhs_cal_zero_resp
{
	UINT8  validFlag;  //是否执行成功
	UINT16 errCode;
	UINT32 length;
}msg_struct_can_l3bfhs_cal_zero_resp_t;

//MSG_ID_L3BFHS_CAN_CAL_FULL_REQ,
typedef struct msg_struct_l3bfhs_can_cal_full_req
{
	UINT8  boardBitmap[HCU_SYSMSG_SUI_SENSOR_NBR];
	StrMsgIe_WeightSensorBfhsCalibrationFullParamaters_t calFullPar;
	UINT32 length;
}msg_struct_l3bfhs_can_cal_full_req_t;

//MSG_ID_CAN_L3BFHS_CAL_FULL_RESP,
typedef struct msg_struct_can_l3bfhs_cal_full_resp
{
	UINT8  validFlag;  //是否执行成功
	UINT16 errCode;
	StrMsgIe_WeightSensorBfhsCalibrationFullRespParamaters_t calFullRespPar;
	UINT32 length;
}msg_struct_can_l3bfhs_cal_full_resp_t;

//MSG_ID_L3BFHS_CAN_DYN_ZERO_REQ,
typedef struct msg_struct_l3bfhs_can_dyn_zero_req
{
	UINT8  boardBitmap[HCU_SYSMSG_SUI_SENSOR_NBR];
	UINT32 length;
}msg_struct_l3bfhs_can_dyn_zero_req_t;

//MSG_ID_CAN_L3BFHS_DYN_ZERO_RESP,
typedef struct msg_struct_can_l3bfhs_dyn_zero_resp
{
	UINT8  validFlag;  //是否执行成功
	UINT16 errCode;
	UINT32 length;
}msg_struct_can_l3bfhs_dyn_zero_resp_t;

//MSG_ID_L3BFHS_CAN_DYN_FULL_REQ,
typedef struct msg_struct_l3bfhs_can_dyn_full_req
{
	UINT8  boardBitmap[HCU_SYSMSG_SUI_SENSOR_NBR];
	strMsgIe_bfhs_WgtSnrParamaters_t wgtSnrPar;
    UINT32 adjustingWeight;
	UINT32 motorSpeed;
	UINT32 motorDirection;
	UINT32 length;
}msg_struct_l3bfhs_can_dyn_full_req_t;

//MSG_ID_CAN_L3BFHS_DYN_FULL_RESP,
typedef struct msg_struct_can_l3bfhs_dyn_full_resp
{
	UINT8  validFlag;  //是否执行成功
	UINT8  iteration;
	UINT16 errCode;
	UINT32 length;
	UINT32 dynCaliCoeff; //NF5 format
	INT32  weight; //format NF2;
}msg_struct_can_l3bfhs_dyn_full_resp_t;

//MSG_ID_CAN_L3BFHS_WS_NEW_READY_EVENT,  	//传感器新数据事件
typedef struct msg_struct_can_l3bfhs_new_ready_event
{
	INT32 snrWsValue;
	UINT8  snrState;
	UINT32 length;
}msg_struct_can_l3bfhs_new_ready_event_t;


/*****************************BFDF项目*********************************/
#define HCU_SYSMSG_BFDF_UICOMM_CMDID_INVALL  			0
#define HCU_SYSMSG_BFDF_UICOMM_CMDID_CFG_START  		1
#define HCU_SYSMSG_BFDF_UICOMM_CMDID_STOP  				2
#define HCU_SYSMSG_BFDF_UICOMM_CMDID_SUSPEND  			3
#define HCU_SYSMSG_BFDF_UICOMM_CMDID_RESUME  			4
#define HCU_SYSMSG_BFDF_UICOMM_CMDID_STATIC_CALI		5
#define HCU_SYSMSG_BFDF_UICOMM_CMDID_DYNAMIC_CALI		6
#define HCU_SYSMSG_BFDF_UICOMM_CMDID_ONE_KEY_ZERO		7
#define HCU_SYSMSG_BFDF_UICOMM_CMDID_TEST				8
#define HCU_SYSMSG_BFDF_UICOMM_CMDID_NULL  				255

//BFDF CMD_VALUE
#define HCU_SYSMSG_BFDF_UICOMM_CMDVALUE_NULL  							0
#define HCU_SYSMSG_BFDF_UICOMM_CMDVALUE_STATIC_CALI_ZERO  				1
#define HCU_SYSMSG_BFDF_UICOMM_CMDVALUE_STATIC_CALI_FULL  				2
#define HCU_SYSMSG_BFDF_UICOMM_CMDVALUE_DYNAMIC_CALI_ZERO_LINE0		  	3
#define HCU_SYSMSG_BFDF_UICOMM_CMDVALUE_DYNAMIC_CALI_FULL_LINE0	  		4
#define HCU_SYSMSG_BFDF_UICOMM_CMDVALUE_DYNAMIC_CALI_ZERO_LINE1		 	7
#define HCU_SYSMSG_BFDF_UICOMM_CMDVALUE_DYNAMIC_CALI_FULL_LINE1	  		8
#define HCU_SYSMSG_BFDF_UICOMM_CMDVALUE_INVALID							0xFF

//MSG_ID_UICOMM_L3BFDF_CTRL_CMD_REQ,
typedef struct msg_struct_uicomm_l3bfdf_ctrl_cmd_req
{
	UINT8  cmdid;
	UINT32 length;
	UINT32 cmdValue;
}msg_struct_uicomm_l3bfdf_ctrl_cmd_req_t;

//MSG_ID_L3BFDF_UICOMM_CTRL_CMD_RESP,
typedef struct strMsgIe_bfdf_calibration_resp
{
    UINT8   calibration_zero_or_full;
    UINT8   calibration_cur_iteration;
    UINT8   calibration_result;
    UINT8   spare2;
    UINT32  full_weight; /* in 0.01g */
    UINT32  noise_floor_period_10ms;
    UINT32  noise_floor_period_10ms_max;
    UINT32  weight_report_offset_wrt_infra;
    UINT32  noise_floor_sd_mean;
    UINT32  noise_floor_sd_max;
    UINT32  noise_floor_sd_min;
    UINT32  noise_floor_sd_sd;
    UINT32  noise_floor_mean_mean;
    UINT32  noise_floor_mean_max;
    UINT32  noise_floor_mean_min;
    UINT32  noise_floor_mean_sd;
    UINT32  full_offset_peak_wrt_infra[4];
    UINT32  full_coefficiency_average;            /* in 0.0001, MIN: 0.0001, MAX: 429496.7295 */
    UINT32  estimated_error_iteration[4];
    UINT32  estimated_error_max_possible;
    UINT32  estimated_error_average;
}strMsgIe_bfdf_calibration_resp_t;
typedef struct msg_struct_l3bfdf_uicomm_ctrl_cmd_resp
{
	UINT8   cmdid;
	UINT8   validFlag;  //是否执行成功
	UINT16  errCode;
	UINT8	sensorid;
	strMsgIe_bfdf_calibration_resp_t dynCalResp;
	UINT32  length;
}msg_struct_l3bfdf_uicomm_ctrl_cmd_resp_t;

//MSG_ID_L3BFDF_CAN_SYS_CFG_REQ,
#define HCU_SYSMSG_BFDF_ERR_CODE_INVALIID	  	0
#define HCU_SYSMSG_BFDF_ERR_CODE_UNSPECIFIC  	1
#define HCU_SYSMSG_BFDF_ERR_CODE_TIME_OUT  		2
#define HCU_SYSMSG_BFDF_ERR_CODE_NULL  		0xFFFF
typedef struct strMsgIe_l3bfdf_WeightSensorParamaters
{
	UINT32	WeightSensorLoadDetectionTimeMs;		//称台稳定的判断时间
	UINT32	WeightSensorLoadThread;							//称台稳定门限，如果在WeightSensorLoadDetectionTime内，重量变化都小于WeightSensorLoadThread
	UINT32	WeightSensorEmptyThread;
	UINT32	WeightSensorEmptyDetectionTimeMs;
	UINT32	WeightSensorPickupThread;						// NOT for GUI
	UINT32	WeightSensorPickupDetectionTimeMs;	// NOT for GUI
	UINT32	StardardReadyTimeMs;								//???
	UINT32	MaxAllowedWeight;										//BFDF: 如果发现超过这个最大值，说明Sensor出错
	UINT32	RemainDetectionTimeSec;					  // RemainDetionTime in Seconds
	UINT32	WeightSensorCalibrationZeroAdcValue;// NOT for GUI
	UINT32	WeightSensorCalibrationFullAdcValue;// NOT for GUI
	UINT32	WeightSensorCalibrationFullWeight;
	UINT32	WeightSensorStaticZeroValue;
	UINT32	WeightSensorTailorValue;
	UINT32	WeightSensorDynamicZeroThreadValue;
	UINT32	WeightSensorDynamicZeroHysteresisMs;
	UINT32  WeightSensorMovingEverageSample;
}strMsgIe_l3bfdf_WeightSensorParamaters_t;
typedef struct strMsgIe_l3bfdf_MotorControlParamaters
{
	UINT32	MotorSpeed;
	UINT32	MotorDirection;									//0: Clockwise; 1: Counter-Clockwise
	UINT32	MotorFailureDetectionVaration;	// % of the MotorSpeed
	UINT32	MotorFailureDetectionTimeMs;		// within TimeMs, 如果速度都在外面，认为故障
}strMsgIe_l3bfdf_MotorControlParamaters_t;
#define	HCU_SYSMSG_BFDF_SET_CFG_HOPPER_MAX			33
#define	HCU_SYSMSG_BFDF_SET_CFG_HOP_IN_BOARD_MAX	8
typedef struct strMsgIe_l3bfdf_ActionControlParamaters
{
	UINT16  TWeightInd;                     /* After INFRA INT, Wait for how long to send WEIGHT_IND, unit is 10ms Tick */
	UINT16  T0bis;                          /* After INFRA INT, INFRA INT sent to Node 2 to 5, unit is 10ms Tick */
	UINT16  TA0;                            /* After INFRA INT, Deay to AP01 */
	UINT16  TActCmd;
	UINT16  TArmStart;
	UINT16  TArmStop;
	UINT16  TDoorCloseLightOn;
	UINT16  TApIntervalMin;
	UINT16  TApInterval[HCU_SYSMSG_BFDF_SET_CFG_HOPPER_MAX];
	UINT16  TLocalAp[HCU_SYSMSG_BFDF_SET_CFG_HOP_IN_BOARD_MAX];
	UINT16  DelayNode1ToX;
	UINT16  DelayUpHcuAlgoDl;
}strMsgIe_l3bfdf_ActionControlParamaters_t;
typedef struct msg_struct_l3bfdf_can_sys_cfg_req
{
	UINT8  boardBitmap[HCU_SYSMSG_SUI_SENSOR_NBR];
	strMsgIe_l3bfdf_WeightSensorParamaters_t			wgtSnrPar;
	strMsgIe_l3bfdf_MotorControlParamaters_t			motMainPar;
	strMsgIe_l3bfdf_MotorControlParamaters_t			motSecondPar;
	strMsgIe_l3bfdf_ActionControlParamaters_t			actionCtrlPar;
	UINT32 length;
}msg_struct_l3bfdf_can_sys_cfg_req_t;

//MSG_ID_CAN_L3BFDF_SYS_CFG_RESP,
typedef struct msg_struct_can_l3bfdf_sys_cfg_resp
{
	UINT8  snrId;
	UINT8  validFlag;  //是否执行成功
	UINT16 errCode;
	UINT32 length;
}msg_struct_can_l3bfdf_sys_cfg_resp_t;

//MSG_ID_L3BFDF_CAN_DYN_CAL_REQ
typedef struct strMsgIe_bfdf_calibration_req
{
    UINT8            calibration_zero_or_full; /* 1 for ZERO, 2 for FULL */
    UINT8            calibration_iteration;    /* 8 for ZERO, 4 for FULL */
    UINT16           TWeightInd;
    UINT32           full_weight; /* in 0.01g */
    UINT32           motor_speed;
    UINT32           adc_sample_freq;
    UINT32           adc_gain;
    UINT32           noise_floor_filter_factor;   /* 0 - 100: 0.00 to 1.00, y = factor * x(n) + (1-factor) * x(n-1), 0 means disable */
    UINT32           max_allowed_weight; /* in 0.01g */
    UINT32           WeightSensorTailorValue;
}strMsgIe_bfdf_calibration_req_t;

typedef struct msg_struct_l3bfdf_can_dyn_cal_req
{
	UINT8  boardBitmap[HCU_SYSMSG_SUI_SENSOR_NBR];
	strMsgIe_bfdf_calibration_req_t dynCalReq;
	UINT32 length;
}msg_struct_l3bfdf_can_dyn_cal_req_t;

//MSG_ID_CAN_L3BFDF_DYN_CAL_RESP
typedef struct msg_struct_can_l3bfdf_dyn_cal_resp
{
	UINT8  snrId;
	UINT8  validFlag;  //是否执行成功
	UINT16 errCode;
	UINT32 length;
	strMsgIe_bfdf_calibration_resp_t dynCalResp;
}msg_struct_can_l3bfdf_dyn_cal_resp_t;

//MSG_ID_CAN_L3BFDF_WS_NEW_READY_EVENT,  	//传感器新数据事件
typedef struct msg_struct_can_l3bfdf_new_ready_event
{
	UINT8  snrId;
	INT32 sensorWsValue;
	UINT32 length;
}msg_struct_can_l3bfdf_new_ready_event_t;

//MSG_ID_L3BFDF_CAN_WS_COMB_OUT,  		//出料
typedef struct msg_struct_l3bfdf_can_ws_comb_out
{
	UINT8  snrId;
	UINT8  hopperId;
	UINT8   basketFullStatus;    //0-FALSE, 1-TRUE
	UINT8   bufferFullStatus;	   //0-FALSE, 1-TRUE
	UINT32 length;
}msg_struct_l3bfdf_can_ws_comb_out_t;

//MSG_ID_CAN_L3BFDF_WS_COMB_OUT_FB,  		//出料确认
typedef struct msg_struct_can_l3bfdf_ws_comb_out_fb
{
	UINT8  snrId;
	UINT8  hopperId;
	UINT16 errCode;
	UINT8  validFlag; //是否执行成功
	UINT32 length;
}msg_struct_can_l3bfdf_ws_comb_out_fb_t;

//MSG_ID_CAN_L3BFDF_BASKET_CLEAN_IND,
typedef struct msg_struct_can_l3bfdf_basket_clean_ind
{
	UINT8  snrId;
	UINT8 hopperId;
	UINT32 length;
}msg_struct_can_l3bfdf_basket_clean_ind_t;

//SUI接口公共消息
//MSG_ID_SUI_TEST_CMD_REQ,  				//测试命令
#define HCU_SYSMSG_SUI_COM_TEST_CMD_BUF		200
typedef struct msg_struct_sui_test_cmd_req
{
	UINT32  cmdid;
	UINT8  	boardBitmap[HCU_SYSMSG_SUI_SENSOR_NBR];
	UINT32 	cmdValue1;
	UINT32 	cmdValue2;
	UINT32 	cmdValue3;
	UINT32 	cmdValue4;
	UINT8	cmdBuf[HCU_SYSMSG_SUI_COM_TEST_CMD_BUF];
	UINT32 	length;
}msg_struct_sui_test_cmd_req_t;

//MSG_ID_SUI_TEST_CMD_RESP,  				//测试结果
typedef struct msg_struct_sui_test_cmd_resp
{
	UINT32  cmdid;
	UINT8	snrId;
	UINT8   validFlag;  //是否执行成功 0-FALSE, 1-TRUE
	UINT16  errCode;
	UINT32  cmdValue1;
	UINT32  cmdValue2;
	UINT32  cmdValue3;
	UINT32  cmdValue4;
	UINT32 	length;
}msg_struct_sui_test_cmd_resp_t;

//MSG_ID_SUI_STARTUP_IND,				//启动指示
typedef struct msg_struct_sui_startup_ind
{
	UINT8  snrId;
	UINT32 hwId;
	UINT32 swId;
	UINT32 cpuId;
	UINT32 wgtType;
	UINT32 motoType;
	UINT32 length;
}msg_struct_sui_startup_ind_t;

//MSG_ID_SUI_FAULT_IND,					//差错指示
typedef struct msg_struct_sui_fault_ind
{
	UINT8   snrId;               /* 0 ~ 15 is the DIP defined, ID 16 is the main rolling */
	UINT16	error_code;
	UINT32  length;
}msg_struct_sui_fault_ind_t;

//MSG_ID_SUI_SUSPEND_REQ,
typedef struct msg_struct_sui_suspend_req
{
	UINT8  boardBitmap[HCU_SYSMSG_SUI_SENSOR_NBR];
	UINT32 length;
}msg_struct_sui_suspend_req_t;

//MSG_ID_SUI_SUSPEND_RESP,
typedef struct msg_struct_sui_suspend_resp
{
	UINT8   snrId;               /* 0 ~ 15 is the DIP defined, ID 16 is the main rolling */
	UINT8  validFlag;  //是否执行成功 0-FALSE, 1-TRUE
	UINT16 errCode;
	UINT32 length;
}msg_struct_sui_suspend_resp_t;

//MSG_ID_SUI_RESUME_REQ,
typedef struct msg_struct_sui_resume_req
{
	UINT8  boardBitmap[HCU_SYSMSG_SUI_SENSOR_NBR];
	UINT32 length;
}msg_struct_sui_resume_req_t;

//MSG_ID_SUI_RESUME_RESP,
typedef struct msg_struct_sui_resume_resp
{
	UINT8   snrId;               /* 0 ~ 15 is the DIP defined, ID 16 is the main rolling */
	UINT8  validFlag; //是否执行成功 0-FALSE, 1-TRUE
	UINT16 errCode;
	UINT32 length;
}msg_struct_sui_resume_resp_t;

//MSG_ID_SUI_STOP_REQ,
typedef struct msg_struct_sui_stop_req
{
	UINT8  boardBitmap[HCU_SYSMSG_SUI_SENSOR_NBR];
	UINT32 length;
}msg_struct_sui_stop_req_t;

//MSG_ID_SUI_STOP_RESP,
typedef struct msg_struct_sui_stop_resp
{
	UINT8  snrId;      /* 0 ~ 15 is the DIP defined, ID 16 is the main rolling */
	UINT8  validFlag;	//是否执行成功 0-FALSE, 1-TRUE
	UINT16 errCode;
	UINT32 length;
}msg_struct_sui_stop_resp_t;

//MSG_ID_SUI_HEART_BEAT_REPORT,
typedef struct msg_struct_sui_heart_beat_report
{
	UINT8  snrId;
	UINT32 timeStamp;
	UINT32 length;
}msg_struct_sui_heart_beat_report_t;

//MSG_ID_SUI_HEART_BEAT_CONFIRM,
typedef struct msg_struct_sui_heart_beat_confirm
{
	UINT8  snrId;               /* 0 ~ 15 is the DIP defined, ID 16 is the main rolling */
	UINT8  state;
	UINT32 timeStamp;
	UINT32 length;
}msg_struct_sui_heart_beat_confirm_t;


//HUICOBUS业务消息
//MSG_ID_HUICOBUS_UIR_INIT_REQ,
typedef struct msg_struct_huicobus_uir_init_req
{
	INT32  cmdValue;
	UINT32 length;
}msg_struct_huicobus_uir_init_req_t;

//MSG_ID_HUICOBUS_UIR_STARTE_REQ,
typedef struct msg_struct_huicobus_uir_start_req
{
	INT32  cmdValue;
	UINT32 length;
}msg_struct_huicobus_uir_start_req_t;

//MSG_ID_HUICOBUS_UIR_RESUME_REQ,
typedef struct msg_struct_huicobus_uir_resume_req
{
	INT32  cmdValue;
	UINT32 length;
}msg_struct_huicobus_uir_resume_req_t;

//MSG_ID_HUICOBUS_UIR_STOP_REQ,
typedef struct msg_struct_huicobus_uir_stop_req
{
	INT32  cmdValue;
	UINT32 length;
}msg_struct_huicobus_uir_stop_req_t;

//MSG_ID_HUICOBUS_UIR_SUSPEND_REQ,
typedef struct msg_struct_huicobus_uir_suspend_req
{
	INT32  cmdValue;
	UINT32 length;
}msg_struct_huicobus_uir_suspend_req_t;

//MSG_ID_HUICOBUS_UIR_STATIC_CALI_REQ,
typedef struct msg_struct_huicobus_uir_static_cali_req
{
	INT32  cmdValue;
	UINT32 length;
}msg_struct_huicobus_uir_static_cali_req_t;

//MSG_ID_HUICOBUS_UIR_DYNAMIC_CALI_REQ,
typedef struct msg_struct_huicobus_uir_dynamic_cali_req
{
	INT32  cmdValue;
	UINT32 length;
}msg_struct_huicobus_uir_dynamic_cali_req_t;

//MSG_ID_HUICOBUS_UIR_CALI_ZERO_REQ,
typedef struct msg_struct_huicobus_uir_cali_zero_req
{
	INT32  cmdValue;
	UINT32 length;
}msg_struct_huicobus_uir_cali_zero_req_t;

//MSG_ID_HUICOBUS_UIR_CALI_FULL_REQ,
typedef struct msg_struct_huicobus_uir_cali_full_req
{
	INT32  cmdValue;
	UINT32 length;
}msg_struct_huicobus_uir_cali_full_req_t;

//MSG_ID_HUICOBUS_UIR_STUDY_START_REQ,
typedef struct msg_struct_huicobus_uir_study_start_req
{
	INT32  cmdValue;
	UINT32 length;
	UINT32 counterWgt;
}msg_struct_huicobus_uir_study_start_req_t;

//MSG_ID_HUICOBUS_UIR_STUDY_STOP_REQ,
typedef struct msg_struct_huicobus_uir_study_stop_req
{
	INT32  cmdValue;
	UINT32 length;
	UINT32 counterWgt;
}msg_struct_huicobus_uir_study_stop_req_t;

//MSG_ID_HUICOBUS_UIR_TEST_CMD_REQ,
typedef struct msg_struct_huicobus_uir_test_cmd_req
{
	UINT8  	boardBitmap[HCU_SYSMSG_SUI_SENSOR_NBR];
	UINT32 	cmdValue1;
	UINT32 	cmdValue2;
	UINT32 	cmdValue3;
	UINT32 	cmdValue4;
	UINT8  	cmdBuf[HCU_SYSMSG_SUI_COM_TEST_CMD_BUF];
	UINT32 length;
}msg_struct_huicobus_uir_test_cmd_req_t;

//MSG_ID_HUICOBUS_UIR_ONE_KEY_CLEAN_ZERO_REQ
typedef struct msg_struct_huicobus_uir_one_key_clean_zero_req
{
	INT32  cmdValue;
	UINT32 length;
}msg_struct_huicobus_uir_one_key_clean_zero_req_t;

/*
 *
 *  传感器-CLOUDVELA之间的消息对
 *
 */
//MSG_ID_CLOUDVELA_EMC_DATA_REQ,
//Data request from Cloud to Sensors
typedef struct  msg_struct_cloudvela_emc_data_req
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseReq;
	UINT8  cmdId;
	UINT8  optId;
	UINT8  cmdIdBackType; //指明是瞬时，还是周期性读数
	UINT32 equId;
	//sensor_zhb_transport_format_dl_t zhbDl;
	UINT32 length;
}msg_struct_cloudvela_emc_data_req_t;

//MSG_ID_EMC_CLOUDVELA_DATA_RESP,
//Data response message to Cloud
typedef struct  msg_struct_emc_cloudvela_data_resp
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseResp;
	UINT8  usercmdid;
	UINT8  useroptid;
	UINT8  cmdIdBackType; //指明是瞬时，还是周期性读数
	sensor_emc_data_element_t emc;
	//sensor_zhb_transport_format_ul_t zhbUl;
	UINT32 length;
}msg_struct_emc_cloudvela_data_resp_t;

//MSG_ID_CLOUDVELA_EMC_CTRL_REQ,
typedef struct  msg_struct_cloudvela_emc_ctrl_req
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseReq;
	UINT8  cmdTag;
	UINT8  cmdId;  //STDXML放弃，该域将去掉
	UINT8  optId;  //STDXML放弃，该域将去掉
	UINT8  backType;  //STDXML放弃，该域将去掉
	sensor_modbus_opertion_general_t opt;
	//sensor_zhb_transport_format_dl_t zhbDl;
	UINT32 length;
}msg_struct_cloudvela_emc_ctrl_req_t;
//cmdTag => huitp.h StrIe_HUITP_IEID_uni_com_snr_cmd_tag_t

//MSG_ID_EMC_CLOUDVELA_CTRL_RESP,
typedef struct msg_struct_emc_cloudvela_ctrl_resp
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseResp;
	UINT8  cmdTag;
	UINT8  cmdId;  //STDXML放弃，该域将去掉
	UINT8  optId;  //STDXML放弃，该域将去掉
	UINT8  backType;  //STDXML放弃，该域将去掉
	sensor_modbus_opertion_general_t opt;
	//sensor_zhb_transport_format_ul_t zhbUl;
	UINT32 length;
}msg_struct_emc_cloudvela_ctrl_resp_t;

//MSG_ID_EMC_CLOUDVELA_DATA_REPORT,
typedef struct msg_struct_emc_cloudvela_data_report
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseReport;
	sensor_emc_data_element_t emc;
	//sensor_zhb_transport_format_ul_t zhbUl;
	UINT32 length;
}msg_struct_emc_cloudvela_data_report_t;

//MSG_ID_CLOUDVELA_EMC_DATA_CONFIRM,
typedef struct msg_struct_cloudvela_emc_data_confirm
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseConfirm;
	UINT32 length;
}msg_struct_cloudvela_emc_data_confirm_t;

//MSG_ID_CLOUDVELA_PM25_DATA_REQ,
typedef struct  msg_struct_cloudvela_pm25_data_req
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseReq;
	UINT8  cmdId;
	UINT8  optId;
	UINT8  cmdIdBackType; //指明是瞬时，还是周期性读数
	UINT32 equId;
	//sensor_zhb_transport_format_dl_t zhbDl;
	UINT32 length;
}msg_struct_cloudvela_pm25_data_req_t;

//MSG_ID_PM25_CLOUDVELA_DATA_RESP,
typedef struct  msg_struct_pm25_cloudvela_data_resp_t //
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseResp;
	UINT8  usercmdid;
	UINT8  useroptid;
	UINT8  cmdIdBackType; //指明是瞬时，还是周期性读数
	sensor_pm25_data_element_t pm25;
	//sensor_zhb_transport_format_ul_t zhbUl;
	UINT32 length;
}msg_struct_pm25_cloudvela_data_resp_t;

//MSG_ID_CLOUDVELA_PM25_CTRL_REQ,
typedef struct  msg_struct_cloudvela_pm25_ctrl_req
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseReq;
	UINT8  cmdTag;
	UINT8  cmdId;
	UINT8  optId;
	UINT8  backType;
	sensor_modbus_opertion_general_t opt;
	//sensor_zhb_transport_format_dl_t zhbDl;
	UINT32 length;
}msg_struct_cloudvela_pm25_ctrl_req_t;

//MSG_ID_PM25_CLOUDVELA_CTRL_RESP,
typedef struct msg_struct_pm25_cloudvela_ctrl_resp
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseResp;
	UINT8  cmdTag;
	UINT8  cmdId;
	UINT8  optId;
	UINT8  backType;
	sensor_modbus_opertion_general_t opt;
	//sensor_zhb_transport_format_ul_t zhbUl;
	UINT32 length;
}msg_struct_pm25_cloudvela_ctrl_resp_t;

//MSG_ID_PM25_CLOUDVELA_DATA_REPORT,
typedef struct msg_struct_pm25_cloudvela_data_report
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseReport;
	sensor_pm25_data_element_t pm25;
	//sensor_zhb_transport_format_ul_t zhbUl;
	UINT32 length;
}msg_struct_pm25_cloudvela_data_report_t;

//MSG_ID_CLOUDVELA_PM25_DATA_CONFIRM,
typedef struct msg_struct_cloudvela_pm25_data_confirm
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseConfirm;
	UINT32 length;
}msg_struct_cloudvela_pm25_data_confirm_t;

//MSG_ID_CLOUDVELA_TEMP_DATA_REQ,
typedef struct  msg_struct_cloudvela_temp_data_req
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseReq;
	UINT8  cmdId;
	UINT8  optId;
	UINT8  cmdIdBackType; //指明是瞬时，还是周期性读数
	UINT32 equId;
	//sensor_zhb_transport_format_dl_t zhbDl;
	UINT32 length;
}msg_struct_cloudvela_temp_data_req_t;

//MSG_ID_TEMP_CLOUDVELA_DATA_RESP,
typedef struct msg_struct_temp_cloudvela_data_resp
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseResp;
	UINT8  usercmdid;
	UINT8  useroptid;
	UINT8  cmdIdBackType; //指明是瞬时，还是周期性读数
	sensor_temp_data_element_t temp;
	//sensor_zhb_transport_format_ul_t zhbUl;
	UINT32 length;
}msg_struct_temp_cloudvela_data_resp_t;

//MSG_ID_CLOUDVELA_TEMP_CTRL_REQ,
typedef struct  msg_struct_cloudvela_temp_ctrl_req
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseReq;
	UINT8  cmdTag;
	UINT8  cmdId;
	UINT8  optId;
	UINT8  backType;
	sensor_modbus_opertion_general_t opt;
	//sensor_zhb_transport_format_dl_t zhbDl;
	UINT32 length;
}msg_struct_cloudvela_temp_ctrl_req_t;

//MSG_ID_TEMP_CLOUDVELA_CTRL_RESP,
typedef struct msg_struct_temp_cloudvela_ctrl_resp
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseResp;
	UINT8  cmdTag;
	UINT8  cmdId;
	UINT8  optId;
	UINT8  backType;
	sensor_modbus_opertion_general_t opt;
	//sensor_zhb_transport_format_ul_t zhbUl;
	UINT32 length;
}msg_struct_temp_cloudvela_ctrl_resp_t;

//MSG_ID_TEMP_CLOUDVELA_DATA_REPORT,
typedef struct msg_struct_temp_cloudvela_data_report
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseReport;
	sensor_temp_data_element_t temp;
	//sensor_zhb_transport_format_ul_t zhbUl;
	UINT32 length;
}msg_struct_temp_cloudvela_data_report_t;

//MSG_ID_CLOUDVELA_TEMP_DATA_CONFIRM,
typedef struct msg_struct_cloudvela_temp_data_confirm
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseConfirm;
	UINT32 length;
}msg_struct_cloudvela_temp_data_confirm_t;

//MSG_ID_CLOUDVELA_HUMID_DATA_REQ,
typedef struct  msg_struct_cloudvela_humid_data_req
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseReq;
	UINT8  cmdId;
	UINT8  optId;
	UINT8  cmdIdBackType; //指明是瞬时，还是周期性读数
	UINT32 equId;
	//sensor_zhb_transport_format_dl_t zhbDl;
	UINT32 length;
}msg_struct_cloudvela_humid_data_req_t;

//MSG_ID_HUMID_CLOUDVELA_DATA_RESP,
typedef struct msg_struct_humid_cloudvela_data_resp
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseResp;
	UINT8  usercmdid;
	UINT8  useroptid;
	UINT8  cmdIdBackType; //指明是瞬时，还是周期性读数
	sensor_humid_data_element_t humid;
	//sensor_zhb_transport_format_ul_t zhbUl;
	UINT32 length;
}msg_struct_humid_cloudvela_data_resp_t;

//MSG_ID_CLOUDVELA_HUMID_CTRL_REQ,
typedef struct  msg_struct_cloudvela_humid_ctrl_req
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseReq;
	UINT8  cmdTag;
	UINT8  cmdId;
	UINT8  optId;
	UINT8  backType;
	sensor_modbus_opertion_general_t opt;
	//sensor_zhb_transport_format_dl_t zhbDl;
	UINT32 length;
}msg_struct_cloudvela_humid_ctrl_req_t;

//MSG_ID_HUMID_CLOUDVELA_CTRL_RESP,
typedef struct msg_struct_humid_cloudvela_ctrl_resp
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseResp;
	UINT8  cmdTag;
	UINT8  cmdId;
	UINT8  optId;
	UINT8  backType;
	sensor_modbus_opertion_general_t opt;
	//sensor_zhb_transport_format_ul_t zhbUl;
	UINT32 length;
}msg_struct_humid_cloudvela_ctrl_resp_t;

//MSG_ID_HUMID_CLOUDVELA_DATA_REPORT,
typedef struct msg_struct_humid_cloudvela_data_report
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseReport;
	sensor_humid_data_element_t humid;
	//sensor_zhb_transport_format_ul_t zhbUl;
	UINT32 length;
}msg_struct_humid_cloudvela_data_report_t;

//MSG_ID_CLOUDVELA_HUMID_DATA_CONFIRM,
typedef struct msg_struct_cloudvela_humid_data_confirm
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseConfirm;
	UINT32 length;
}msg_struct_cloudvela_humid_data_confirm_t;

//MSG_ID_CLOUDVELA_WINDDIR_DATA_REQ,
typedef struct  msg_struct_cloudvela_winddir_data_req
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseReq;
	UINT8  cmdId;
	UINT8  optId;
	UINT8  cmdIdBackType; //指明是瞬时，还是周期性读数
	UINT32 equId;
	//sensor_zhb_transport_format_dl_t zhbDl;
	UINT32 length;
}msg_struct_cloudvela_winddir_data_req_t;

//MSG_ID_WINDDIR_CLOUDVELA_DATA_RESP,
typedef struct msg_struct_winddir_cloudvela_data_resp
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseResp;
	UINT8  usercmdid;
	UINT8  useroptid;
	UINT8  cmdIdBackType; //指明是瞬时，还是周期性读数
	sensor_winddir_data_element_t winddir;
	//sensor_zhb_transport_format_ul_t zhbUl;
	UINT32 length;
}msg_struct_winddir_cloudvela_data_resp_t;

//MSG_ID_CLOUDVELA_WINDDIR_CTRL_REQ,
typedef struct  msg_struct_cloudvela_winddir_ctrl_req
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseReq;
	UINT8  cmdTag;
	UINT8  cmdId;
	UINT8  optId;
	UINT8  backType;
	sensor_modbus_opertion_general_t opt;
	//sensor_zhb_transport_format_dl_t zhbDl;
	UINT32 length;
}msg_struct_cloudvela_winddir_ctrl_req_t;

//MSG_ID_WINDDIR_CLOUDVELA_CTRL_RESP,
typedef struct msg_struct_winddir_cloudvela_ctrl_resp
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseResp;
	UINT8  cmdTag;
	UINT8  cmdId;
	UINT8  optId;
	UINT8  backType;
	sensor_modbus_opertion_general_t opt;
	//sensor_zhb_transport_format_ul_t zhbUl;
	UINT32 length;
}msg_struct_winddir_cloudvela_ctrl_resp_t;

//MSG_ID_WINDDIR_CLOUDVELA_DATA_REPORT,
typedef struct msg_struct_winddir_cloudvela_data_report
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseReport;
	sensor_winddir_data_element_t winddir;
	//sensor_zhb_transport_format_ul_t zhbUl;
	UINT32 length;
}msg_struct_winddir_cloudvela_data_report_t;

//MSG_ID_CLOUDVELA_WINDDIR_DATA_CONFIRM,
typedef struct msg_struct_cloudvela_winddir_data_confirm
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseConfirm;
	UINT32 length;
}msg_struct_cloudvela_winddir_data_confirm_t;

//MSG_ID_CLOUDVELA_WINDSPD_DATA_REQ,
typedef struct  msg_struct_cloudvela_windspd_data_req
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseReq;
	UINT8  cmdId;
	UINT8  optId;
	UINT8  cmdIdBackType; //指明是瞬时，还是周期性读数
	UINT32 equId;
	//sensor_zhb_transport_format_dl_t zhbDl;
	UINT32 length;
}msg_struct_cloudvela_windspd_data_req_t;

//MSG_ID_WINDSPD_CLOUDVELA_DATA_RESP,
typedef struct msg_struct_windspd_cloudvela_data_resp
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseResp;
	UINT8  usercmdid;
	UINT8  useroptid;
	UINT8  cmdIdBackType; //指明是瞬时，还是周期性读数
	sensor_windspd_data_element_t windspd;
	//sensor_zhb_transport_format_ul_t zhbUl;
	UINT32 length;
}msg_struct_windspd_cloudvela_data_resp_t;

//MSG_ID_CLOUDVELA_WINDSPD_CTRL_REQ,
typedef struct  msg_struct_cloudvela_windspd_ctrl_req
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseReq;
	UINT8  cmdTag;
	UINT8  cmdId;
	UINT8  optId;
	UINT8  backType;
	sensor_modbus_opertion_general_t opt;
	//sensor_zhb_transport_format_dl_t zhbDl;
	UINT32 length;
}msg_struct_cloudvela_windspd_ctrl_req_t;

//MSG_ID_WINDSPD_CLOUDVELA_CTRL_RESP,
typedef struct msg_struct_windspd_cloudvela_ctrl_resp
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseResp;
	UINT8  cmdTag;
	UINT8  cmdId;
	UINT8  optId;
	UINT8  backType;
	sensor_modbus_opertion_general_t opt;
	//sensor_zhb_transport_format_ul_t zhbUl;
	UINT32 length;
}msg_struct_windspd_cloudvela_ctrl_resp_t;

//MSG_ID_WINDSPD_CLOUDVELA_DATA_REPORT,
typedef struct msg_struct_windspd_cloudvela_data_report
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseReport;
	sensor_windspd_data_element_t windspd;
	//sensor_zhb_transport_format_ul_t zhbUl;
	UINT32 length;
}msg_struct_windspd_cloudvela_data_report_t;

//MSG_ID_CLOUDVELA_WINDSPD_DATA_CONFIRM,
typedef struct msg_struct_cloudvela_windspd_data_confirm
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseConfirm;
	UINT32 length;
}msg_struct_cloudvela_windspd_data_confirm_t;

//MSG_ID_CLOUDVELA_HSMMP_DATA_REQ,
typedef struct  msg_struct_cloudvela_hsmmp_data_req
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseReq;
	UINT8  cmdId;
	UINT8  optId;
	UINT8  cmdIdBackType; //指明是瞬时，还是周期性读数
	UINT32 equId;
	UINT32 length;
}msg_struct_cloudvela_hsmmp_data_req_t;

//MSG_ID_HSMMP_CLOUDVELA_DATA_RESP,
typedef struct msg_struct_hsmmp_cloudvela_data_resp
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseResp;
	UINT8  usercmdid;
	UINT8  useroptid;
	UINT8  cmdIdBackType; //指明是瞬时，还是周期性读数
	sensor_hsmmp_link_element_t link;
	UINT8  dataFormat;
	INT32 xPos;
	INT32 yPos;
	INT32 zoomPos;
	UINT32 length;
}msg_struct_hsmmp_cloudvela_data_resp_t;

//MSG_ID_CLOUDVELA_HSMMP_CTRL_REQ,
typedef struct  msg_struct_cloudvela_hsmmp_ctrl_req
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseReq;
	UINT8  cmdTag;
	UINT8  cmdId;
	UINT8  optId;
	UINT8  backType;
	sensor_modbus_opertion_general_t opt;
	UINT8  motive;
	UINT32 motiveValue;
	UINT32 length;
}msg_struct_cloudvela_hsmmp_ctrl_req_t;

//MSG_ID_HSMMP_CLOUDVELA_CTRL_RESP
typedef struct msg_struct_hsmmp_cloudvela_ctrl_resp
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseResp;
	UINT8  cmdTag;
	UINT8  cmdId;
	UINT8  optId;
	UINT8  backType;
	sensor_modbus_opertion_general_t opt;
	UINT8  motive;
	UINT32 motiveValue;
	UINT32 length;
}msg_struct_hsmmp_cloudvela_ctrl_resp_t;

//MSG_ID_HSMMP_CLOUDVELA_DATA_REPORT,
typedef struct msg_struct_hsmmp_cloudvela_data_report
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseReport;
	sensor_hsmmp_link_element_t link;
	UINT32 length;
}msg_struct_hsmmp_cloudvela_data_report_t;

//MSG_ID_CLOUDVELA_HSMMP_DATA_CONFIRM,
typedef struct msg_struct_cloudvela_hsmmp_data_confirm
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseConfirm;
	UINT32 length;
}msg_struct_cloudvela_hsmmp_data_confirm_t;

//MSG_ID_PICTURE_CLOUDVELA_DATA_REPORT,
#define HCU_SYSMSG_PICTURE_NAME_MAX_LEN    	 100
typedef struct msg_struct_picture_cloudvela_data_report
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseReport;
	sensor_hsmmp_link_element_t link;
	UINT8  flag;
	UINT32 eventId;
	char   picName[HCU_SYSMSG_PICTURE_NAME_MAX_LEN];
	UINT32 length;
}msg_struct_picture_cloudvela_data_report_t;

//MSG_ID_CLOUDVELA_PICTURE_DATA_CONFIRM,
typedef struct msg_struct_cloudvela_picture_data_confirm
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseConfirm;
	UINT32 length;
}msg_struct_cloudvela_picture_data_confirm_t;

//MSG_ID_CLOUDVELA_NOISE_DATA_REQ,
typedef struct  msg_struct_cloudvela_noise_data_req
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseReq;
	UINT8  cmdId;
	UINT8  optId;
	UINT8  cmdIdBackType; //指明是瞬时，还是周期性读数
	UINT32 equId;
	//sensor_zhb_transport_format_dl_t zhbDl;
	UINT32 length;
}msg_struct_cloudvela_noise_data_req_t;

//MSG_ID_NOISE_CLOUDVELA_DATA_RESP,
typedef struct msg_struct_noise_cloudvela_data_resp
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseResp;
	UINT8  usercmdid;
	UINT8  useroptid;
	UINT8  cmdIdBackType; //指明是瞬时，还是周期性读数
	sensor_noise_data_element_t noise;
	//sensor_zhb_transport_format_ul_t zhbUl;
	UINT32 length;
}msg_struct_noise_cloudvela_data_resp_t;

//MSG_ID_CLOUDVELA_NOISE_CTRL_REQ,
typedef struct  msg_struct_cloudvela_noise_ctrl_req
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseReq;
	UINT8  cmdTag;
	UINT8  cmdId;
	UINT8  optId;
	UINT8  backType;
	sensor_modbus_opertion_general_t opt;
	//sensor_zhb_transport_format_dl_t zhbDl;
	UINT32 length;
}msg_struct_cloudvela_noise_ctrl_req_t;

//MSG_ID_NOISE_CLOUDVELA_CTRL_RESP,
typedef struct msg_struct_noise_cloudvela_ctrl_resp
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseResp;
	UINT8  cmdTag;
	UINT8  cmdId;
	UINT8  optId;
	UINT8  backType;
	sensor_modbus_opertion_general_t opt;
	//sensor_zhb_transport_format_ul_t zhbUl;
	UINT32 length;
}msg_struct_noise_cloudvela_ctrl_resp_t;

//MSG_ID_NOISE_CLOUDVELA_DATA_REPORT,
typedef struct msg_struct_noise_cloudvela_data_report
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseReport;
	sensor_noise_data_element_t noise;
	//sensor_zhb_transport_format_ul_t zhbUl;
	UINT32 length;
}msg_struct_noise_cloudvela_data_report_t;

//MSG_ID_CLOUDVELA_NOISE_DATA_CONFIRM,
typedef struct msg_struct_cloudvela_noise_data_confirm
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseConfirm;
	UINT32 length;
}msg_struct_cloudvela_noise_data_confirm_t;


//MSG_ID_CLOUDVELA_YCJK_DATA_REQ,
typedef struct  msg_struct_cloudvela_ycjk_data_req
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseReq;
	UINT8  cmdId;
	UINT8  optId;
	UINT8  cmdIdBackType; //指明是瞬时，还是周期性读数
	UINT32 equId;
	//sensor_zhb_transport_format_dl_t zhbDl;
	UINT32 length;
}msg_struct_cloudvela_ycjk_data_req_t;

//MSG_ID_YCJK_CLOUDVELA_DATA_RESP,
typedef struct msg_struct_ycjk_cloudvela_data_resp
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseResp;
	UINT8  usercmdid;
	UINT8  useroptid;
	UINT8  cmdIdBackType; //指明是瞬时，还是周期性读数
	sensor_ycjk_data_element_t ycjk;
	//sensor_zhb_transport_format_ul_t zhbUl;
	UINT32 length;
}msg_struct_ycjk_cloudvela_data_resp_t;

//MSG_ID_CLOUDVELA_YCJK_CTRL_REQ,
typedef struct  msg_struct_cloudvela_ycjk_ctrl_req
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseReq;
	UINT8  cmdTag;
	UINT8  cmdId;
	UINT8  optId;
	UINT8  backType;
	sensor_modbus_opertion_general_t opt;
	//sensor_zhb_transport_format_dl_t zhbDl;
	UINT8  sensorType;
	UINT8  sensorId;
	UINT32 sensorBitmap;
	UINT32 length;
}msg_struct_cloudvela_ycjk_ctrl_req_t;

//MSG_ID_YCJK_CLOUDVELA_CTRL_RESP,
typedef struct msg_struct_ycjk_cloudvela_ctrl_resp
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseResp;
	UINT8  cmdTag;
	UINT8  cmdId;
	UINT8  optId;
	UINT8  backType;
	sensor_modbus_opertion_general_t opt;
	//sensor_zhb_transport_format_ul_t zhbUl;
	UINT8  sensorType;
	UINT8  sensorId;
	UINT32 sensorBitmap;
	UINT32 length;
}msg_struct_ycjk_cloudvela_ctrl_resp_t;

//MSG_ID_YCJK_CLOUDVELA_DATA_REPORT,
typedef struct msg_struct_ycjk_cloudvela_data_report
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseReport;
	sensor_ycjk_data_element_t ycjk;
	UINT32 length;
}msg_struct_ycjk_cloudvela_data_report_t;

//MSG_ID_CLOUDVELA_YCJK_DATA_CONFIRM,
typedef struct msg_struct_cloudvela_ycjk_data_confirm
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseConfirm;
	UINT32 length;
}msg_struct_cloudvela_ycjk_data_confirm_t;


/*
 *
 *  一般性传感器-CLOUDVELA之间的消息对
 *
 */

//MSG_ID_CLOUDVELA_AIRPRS_DATA_REQ,
typedef struct  msg_struct_cloudvela_airprs_data_req //
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseReq;
	UINT32 length;
}msg_struct_cloudvela_airprs_data_req_t;

//MSG_ID_AIRPRS_CLOUDVELA_DATA_RESP,
typedef struct msg_struct_airprs_cloudvela_data_resp
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseResp;
	UINT8  dataFormat;
	UINT32 airprsValue;
	UINT32 length;
}msg_struct_airprs_cloudvela_data_resp_t;

//MSG_ID_AIRPRS_CLOUDVELA_DATA_REPORT,
typedef struct  msg_struct_airprs_cloudvela_data_report //
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseReport;
	UINT8  dataFormat;
	UINT32 airprsValue;
	UINT32 length;
}msg_struct_airprs_cloudvela_data_report_t;

//MSG_ID_CLOUDVELA_AIRPRS_DATA_CONFIRM,
typedef struct  msg_struct_cloudvela_airprs_data_confirm //
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseConfirm;
	UINT32 length;
}msg_struct_cloudvela_airprs_data_confirm_t;

//MSG_ID_CLOUDVELA_ALCOHOL_DATA_REQ,
typedef struct  msg_struct_cloudvela_alcohol_data_req //
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseReq;
	UINT32 length;
}msg_struct_cloudvela_alcohol_data_req_t;

//MSG_ID_ALCOHOL_CLOUDVELA_DATA_RESP,
typedef struct msg_struct_alcohol_cloudvela_data_resp
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseResp;
	UINT8  dataFormat;
	UINT32 alcoholValue;
	UINT32 length;
}msg_struct_alcohol_cloudvela_data_resp_t;

//MSG_ID_ALCOHOL_CLOUDVELA_DATA_REPORT,
typedef struct  msg_struct_alcohol_cloudvela_data_report
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseReport;
	UINT8  dataFormat;
	UINT32 alcoholValue;
	UINT32 length;
}msg_struct_alcohol_cloudvela_data_report_t;

//MSG_ID_CLOUDVELA_ALCOHOL_DATA_CONFIRM,
typedef struct  msg_struct_cloudvela_alcohol_data_confirm
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseConfirm;
	UINT32 length;
}msg_struct_cloudvela_alcohol_data_confirm_t;

//MSG_ID_CLOUDVELA_CO1_DATA_REQ,
typedef struct  msg_struct_cloudvela_co1_data_req
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseReq;
	UINT32 length;
}msg_struct_cloudvela_co1_data_req_t;

//MSG_ID_CO1_CLOUDVELA_DATA_RESP,
typedef struct msg_struct_co1_cloudvela_data_resp
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseResp;
	UINT8  dataFormat;
	UINT32 co1Value;
	UINT32 length;
}msg_struct_co1_cloudvela_data_resp_t;

//MSG_ID_CO1_CLOUDVELA_DATA_REPORT,
typedef struct  msg_struct_co1_cloudvela_data_report
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseReport;
	UINT8  dataFormat;
	UINT32 co1Value;
	UINT32 length;
}msg_struct_co1_cloudvela_data_report_t;

//MSG_ID_CLOUDVELA_CO1_DATA_CONFIRM,
typedef struct  msg_struct_cloudvela_co1_data_confirm
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseConfirm;
	UINT32 length;
}msg_struct_cloudvela_co1_data_confirm_t;

//MSG_ID_CLOUDVELA_HCHO_DATA_REQ,
typedef struct  msg_struct_cloudvela_hcho_data_req
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseReq;
	UINT32 length;
}msg_struct_cloudvela_hcho_data_req_t;

//MSG_ID_HCHO_CLOUDVELA_DATA_RESP,
typedef struct msg_struct_hcho_cloudvela_data_resp
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseResp;
	UINT8  dataFormat;
	UINT32 hchoValue;
	UINT32 length;
}msg_struct_hcho_cloudvela_data_resp_t;

//MSG_ID_HCHO_CLOUDVELA_DATA_REPORT,
typedef struct  msg_struct_hcho_cloudvela_data_report
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseReport;
	UINT8  dataFormat;
	UINT32 hchoValue;
	UINT32 length;
}msg_struct_hcho_cloudvela_data_report_t;

//MSG_ID_CLOUDVELA_HCHO_DATA_CONFIRM,
typedef struct  msg_struct_cloudvela_hcho_data_confirm
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseConfirm;
	UINT32 length;
}msg_struct_cloudvela_hcho_data_confirm_t;

//MSG_ID_CLOUDVELA_LIGHTSTR_DATA_REQ,
typedef struct  msg_struct_cloudvela_lightstr_data_req
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseReq;
	UINT32 length;
}msg_struct_cloudvela_lightstr_data_req_t;

//MSG_ID_LIGHTSTR_CLOUDVELA_DATA_RESP,
typedef struct msg_struct_lightstr_cloudvela_data_resp
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseResp;
	UINT8  dataFormat;
	UINT32 lightstrValue;
	UINT32 length;
}msg_struct_lightstr_cloudvela_data_resp_t;

//MSG_ID_LIGHTSTR_CLOUDVELA_DATA_REPORT,
typedef struct  msg_struct_lightstr_cloudvela_data_report
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseReport;
	UINT8  dataFormat;
	UINT32 lightstrValue;
	UINT32 length;
}msg_struct_lightstr_cloudvela_data_report_t;

//MSG_ID_CLOUDVELA_LIGHTSTR_DATA_CONFIRM,
typedef struct  msg_struct_cloudvela_lightstr_data_confirm
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseConfirm;
	UINT8  dataFormat;
	UINT32 lightstrValue;
	UINT32 length;
}msg_struct_cloudvela_lightstr_data_confirm_t;

//MSG_ID_CLOUDVELA_PM25SP_DATA_REQ,
typedef struct  msg_struct_cloudvela_pm25sp_data_req
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseReq;
	UINT32 length;
}msg_struct_cloudvela_pm25sp_data_req_t;

//MSG_ID_PM25SP_CLOUDVELA_DATA_RESP,
typedef struct msg_struct_pm25sp_cloudvela_data_resp
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseResp;
	UINT8  dataFormat;
	UINT32 pm25sp1d0Value;
	UINT32 pm25sp2d5Value;
	UINT32 pm25sp10Value;
	UINT32 length;
}msg_struct_pm25sp_cloudvela_data_resp_t;

//MSG_ID_PM25SP_CLOUDVELA_DATA_REPORT,
typedef struct  msg_struct_pm25sp_cloudvela_data_report
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseReport;
	UINT8  dataFormat;
	UINT32 pm25sp1d0Value;
	UINT32 pm25sp2d5Value;
	UINT32 pm25sp10Value;
	UINT32 length;
}msg_struct_pm25sp_cloudvela_data_report_t;

//MSG_ID_CLOUDVELA_PM25SP_DATA_CONFIRM,
typedef struct  msg_struct_cloudvela_pm25sp_data_confirm
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseConfirm;
	UINT32 length;
}msg_struct_cloudvela_pm25sp_data_confirm_t;

//MSG_ID_CLOUDVELA_TOXICGAS_DATA_REQ,
typedef struct  msg_struct_cloudvela_toxicgas_data_req
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseReq;
	UINT32 length;
}msg_struct_cloudvela_toxicgas_data_req_t;

//MSG_ID_TOXICGAS_CLOUDVELA_DATA_RESP,
typedef struct msg_struct_toxicgas_cloudvela_data_resp
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseResp;
	UINT8  dataFormat;
	UINT32 toxicgasValue;
	UINT32 length;
}msg_struct_toxicgas_cloudvela_data_resp_t;

//MSG_ID_TOXICGAS_CLOUDVELA_DATA_REPORT,
typedef struct  msg_struct_toxicgas_cloudvela_data_report
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseReport;
	UINT8  dataFormat;
	UINT32 toxicgasValue;
	UINT32 length;
}msg_struct_toxicgas_cloudvela_data_report_t;

//MSG_ID_CLOUDVELA_TOXICGAS_DATA_CONFIRM,
typedef struct  msg_struct_cloudvela_toxicgas_data_confirm
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseConfirm;
	UINT32 length;
}msg_struct_cloudvela_toxicgas_data_confirm_t;

//L3AQYCG20
//MSG_ID_L3AQYC_EXG_CTRL_REQ,
typedef struct  msg_struct_l3aqyc_exg_ctrl_req
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseReq;
	UINT8  cmdTag;
	UINT8  cmdId;
	UINT8  optId;
	UINT8  backType;
	sensor_modbus_opertion_general_t opt;
	//sensor_zhb_transport_format_dl_t zhbDl;
	UINT8  sensorType;
	UINT8  sensorId;
	UINT32 sensorBitmap;
	UINT32 length;
}msg_struct_l3aqyc_exg_ctrl_req_t;

//MSG_ID_L3AQYC_EXG_CTRL_RESP,
typedef struct msg_struct_l3aqyc_exg_ctrl_resp
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseResp;
	UINT8  cmdTag;
	UINT8  cmdId;
	UINT8  optId;
	UINT8  backType;
	sensor_modbus_opertion_general_t opt;
	//sensor_zhb_transport_format_ul_t zhbUl;
	UINT8  sensorType;
	UINT8  sensorId;
	UINT32 sensorBitmap;
	UINT32 length;
}msg_struct_l3aqyc_exg_ctrl_resp_t;

//MSG_ID_L3AQYC_EXG_DATA_REPORT,
typedef struct msg_struct_l3aqyc_exg_data_report
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseReport;
	sensor_ycjk_data_element_t ycjk;
	//sensor_zhb_transport_format_ul_t zhbUl;
	UINT32 length;
}msg_struct_l3aqyc_exg_data_report_t;


/*
 *
 *
 *   //ZHBHJT
 *
 *
 */
//ZHBHJT212
#define HCU_SYSMSG_ZHBHJT_POLID_NBR_MAX 7
#define HCU_SYSMSG_ZHBHJT_CTIME_NBR_MAX 24
typedef struct msgie_struct_zhbhjt_frame_head //
{
	//UINT64 qn;//bug fix by shanchun for time change tick to YYYYMMDDHHMMSSZZZ
	LLP64 qn;
	UINT8  st;  //污染物种类
	UINT16 cn;  //命令
	char   pw[7];  //最长6位密码
	UINT16 pnum;  //上位机也可能通过数据应答，将PNO/PMUM发送下来的。这是链路层的包证实机制，确保包收到。这个功能就留在LLC层次了
	UINT16 pno;
	UINT8  ansFlag;//中环保标准， 3c, 数据包是否拆分及应答标志 Flag，从云后台的接收方向
}msgie_struct_zhbhjt_frame_head_t;

typedef struct  msgie_struct_zhbhjt_frame_data_pol_rtd
{
	UINT8  PolId;
	char   PolFlag;
	float  Rtd;
}msgie_struct_zhbhjt_frame_data_pol_rtd_t;
typedef struct  msgie_struct_zhbhjt_frame_data_pol_min_hour
{
	UINT8  PolId;
	char   PolFlag;
	float  Avg;
	float  Max;
	float  Min;
	//float  Cou;

}msgie_struct_zhbhjt_frame_data_pol_min_hour_t;
typedef struct  msgie_struct_zhbhjt_frame_data_low_upvalue
{
	UINT8  PolId;
	float  UpValue;
	float  LowValue;
}msgie_struct_zhbhjt_frame_data_low_upvalue_t;
typedef struct  msgie_struct_zhbhjt_frame_data_ctime
{
	UINT8  PolId;
	UINT8  CTime[HCU_SYSMSG_ZHBHJT_CTIME_NBR_MAX];
}msgie_struct_zhbhjt_frame_data_ctime_t;
typedef struct  msgie_struct_zhbhjt_frame_data_begin_end_time
{
	UINT32 BeginTime;
	UINT32 EndTime;
}msgie_struct_zhbhjt_frame_data_begin_end_time_t;

typedef struct  msgie_struct_zhbhjt_frame_data_alarm_event
{
	UINT8  PolId;
	float  Ala;
	UINT8  AlarmType;
}msgie_struct_zhbhjt_frame_data_alarm_event_t;
typedef struct  msgie_struct_zhbhjt_frame_data_ala_value
{
	UINT8  PolId;
	float  Ala;
}msgie_struct_zhbhjt_frame_data_ala_value_t;
typedef struct  msgie_struct_zhbhjt_frame_data_RS_value
{
	UINT8  PolId;
	UINT8  RS;
}msgie_struct_zhbhjt_frame_data_RS_value_t;
typedef struct  msgie_struct_zhbhjt_frame_data_RT_value
{
	UINT8  PolId;
	INT32  RT;
}msgie_struct_zhbhjt_frame_data_RT_value_t;
typedef struct  msgie_struct_zhbhjt_element_dl2hcu
{
	UINT32 SystemTime;
	UINT32 AlarmTarget;
	UINT16 ReportTime;
	UINT8  singlePolId;  //一样引到
	UINT16 RtdInterval;
	UINT32 OverTime;
	UINT8  ReCount;
	UINT32 WarnTime;
	msgie_struct_zhbhjt_frame_data_begin_end_time_t gapTime;
	UINT8  nbrOfLimitation;
	msgie_struct_zhbhjt_frame_data_low_upvalue_t limitation[HCU_SYSMSG_ZHBHJT_POLID_NBR_MAX];
	UINT8  nbrOfPolId;
	UINT8  multiPolid[HCU_SYSMSG_ZHBHJT_POLID_NBR_MAX];
	UINT8  nbrOfCTime;
	msgie_struct_zhbhjt_frame_data_ctime_t ctime;
}msgie_struct_zhbhjt_element_dl2hcu_t;

typedef struct  msgie_struct_zhbhjt_element_ul2cloud
{
	UINT8  QnRtn;
	UINT32 SystemTime;
	UINT8  ExeRtn;
	UINT32 DataTime;
	msgie_struct_zhbhjt_frame_data_ala_value_t  Ala;
	UINT32 AlarmTime;
	UINT8  AlarmType;
	msgie_struct_zhbhjt_frame_data_alarm_event_t AlarmEvent;
	UINT32 AlarmTarget;
	UINT16 ReportTime;
	UINT16 RtdInterval;
	UINT8 nbrOfCRtd;
	msgie_struct_zhbhjt_frame_data_pol_rtd_t rtd[HCU_SYSMSG_ZHBHJT_POLID_NBR_MAX];
	UINT8 nbrOfRS;
	msgie_struct_zhbhjt_frame_data_RS_value_t RS[HCU_SYSMSG_ZHBHJT_POLID_NBR_MAX];
	UINT8 nbrOfRT;
	msgie_struct_zhbhjt_frame_data_RT_value_t RT[HCU_SYSMSG_ZHBHJT_POLID_NBR_MAX];
	UINT8 nbrOfCMinRpt;
	msgie_struct_zhbhjt_frame_data_pol_min_hour_t min[HCU_SYSMSG_ZHBHJT_POLID_NBR_MAX];
	UINT8 nbrOfAlmLim;
	msgie_struct_zhbhjt_frame_data_low_upvalue_t limitation[HCU_SYSMSG_ZHBHJT_POLID_NBR_MAX];
	UINT32 length;
}msgie_struct_zhbhjt_element_ul2cloud_t;

//MSG_ID_CLOUDVELA_LLCZHB_FRAME_REQ,
typedef struct  msg_struct_cloudvela_llczhb_frame_req
{
	msgie_struct_zhbhjt_frame_head_t head;
	LLP64 cfmQn;//bug fix by shanchun for time change tick to YYYYMMDDHHMMSSZZZ
	UINT16 cfmCN;
	UINT16 cfmPNUM;
	UINT16 cfmPNO;
	char setpw[7];
	msgie_struct_zhbhjt_element_dl2hcu_t dl2Self;
	UINT32 length;
}msg_struct_cloudvela_llczhb_frame_req_t;

//MSG_ID_LLCZHB_CLOUDVELA_FRAME_RESP,
typedef struct  msg_struct_llczhb_cloudvela_frame_resp
{
	msgie_struct_zhbhjt_frame_head_t head;
	LLP64 cfmQn; //bug fix by shanchun for time change tick to YYYYMMDDHHMMSSZZZ
	msgie_struct_zhbhjt_element_ul2cloud_t ul2Cloud;
	UINT32 length;
}msg_struct_llczhb_cloudvela_frame_resp_t;

typedef enum
{
	//控制命令，完成
	HCU_SYSMSG_ZHBHJT_ACTION_EXECUTE_FINISH_9012 = 1,
	//控制命令，数据证实
	HCU_SYSMSG_ZHBHJT_ACTION_DATA_CFM_9014,
	//设置密码
	HCU_SYSMSG_ZHBHJT_ACTION_SET_PSWD_1072,
	//现场设备的时间
	HCU_SYSMSG_ZHBHJT_ACTION_GET_FIELD_TIME_1011,
	HCU_SYSMSG_ZHBHJT_ACTION_SET_FIELD_TIME_1012,
	//污染物实时数据
	HCU_SYSMSG_ZHBHJT_ACTION_GET_POL_RTD_2011,
	HCU_SYSMSG_ZHBHJT_ACTION_STOP_RTD_2012,
	//设备运行状态
	HCU_SYSMSG_ZHBHJT_ACTION_GET_EQU_RUN_2021,
	HCU_SYSMSG_ZHBHJT_ACTION_STOP_EQU_RUN_2022,
	//分钟报告
	HCU_SYSMSG_ZHBHJT_ACTION_GET_POL_MIN_RPT_2051,
	//小时报告
	HCU_SYSMSG_ZHBHJT_ACTION_GET_POL_HOUR_RPT_2061, //shanchun

	HCU_SYSMSG_ZHBHJT_ACTION_GET_POL_DAY_RPT_2031,
	//日数据
	HCU_SYSMSG_ZHBHJT_ACTION_GET_POL_RT_2041,
	//告警数据
	HCU_SYSMSG_ZHBHJT_ACTION_GET_POL_ALA_2071,
	//告警事件
	HCU_SYSMSG_ZHBHJT_ACTION_SND_ALM_EVT_2072,
	//告警门限
	HCU_SYSMSG_ZHBHJT_ACTION_SET_ALMLIM_1022,
	HCU_SYSMSG_ZHBHJT_ACTION_GET_ALMLIM_1021,
	//设备地址
	HCU_SYSMSG_ZHBHJT_ACTION_SET_MNADDR_1032,
	HCU_SYSMSG_ZHBHJT_ACTION_GET_MNADDR_1031,
	//报告时间
	HCU_SYSMSG_ZHBHJT_ACTION_SET_RPT_TIME_1042,
	HCU_SYSMSG_ZHBHJT_ACTION_GET_RPT_TIME_1041,
	//校准清零
	HCU_SYSMSG_ZHBHJT_ACTION_SET_ZERO_3011,
	//实时数据上报间隔
	HCU_SYSMSG_ZHBHJT_ACTION_SET_RTDI_1062,
	HCU_SYSMSG_ZHBHJT_ACTION_GET_RTDI_1061,
	//超时与重传次数
	HCU_SYSMSG_ZHBHJT_ACTION_SET_OT_RC_1000,
	//告警时间
	HCU_SYSMSG_ZHBHJT_ACTION_SET_WARN_TIME_1001,
	HCU_SYSMSG_ZHBHJT_ACTION_SET_INST_SAMPLE_3012,
	HCU_SYSMSG_ZHBHJT_ACTION_SET_SAMPLE_CTIME_3014,
}ZhbhjtActionDefination;

//MSG_ID_LLCZHB_L3MOD_CTRL_REQ,
typedef struct  msg_struct_llczhb_l3mod_ctrl_req
{
	UINT8  actionId;
	msgie_struct_zhbhjt_element_dl2hcu_t dl2Self;
	UINT32 length;
}msg_struct_llczhb_l3mod_ctrl_req_t;

//MSG_ID_L3MOD_LLCZHB_CTRL_RESP,
typedef struct  msg_struct_l3mod_llczhb_ctrl_resp
{
	UINT8  actionId;
	msgie_struct_zhbhjt_element_ul2cloud_t ul2Cloud;
	UINT32 length;//added by zsc
}msg_struct_l3mod_llczhb_ctrl_resp_t;

//MSG_ID_L3MOD_LLCZHB_DATA_REPORT,
typedef struct  msg_struct_l3mod_llczhb_data_report
{
	UINT8  actionId;
	msgie_struct_zhbhjt_element_ul2cloud_t ul2Cloud;
	UINT32 length;//added by zsc
}msg_struct_l3mod_llczhb_data_report_t;

//MSG_ID_ZHBL3MOD_EXG_CTRL_REQ,
typedef struct  msg_struct_l3mod_exg_ctrl_req
{
	UINT8  actionId;
	msgie_struct_zhbhjt_element_dl2hcu_t dl2Self;
	UINT32 length;
}msg_struct_l3mod_exg_ctrl_req_t;

//MSG_ID_ZHBL3MOD_EXG_CTRL_RESP,
typedef struct  msg_struct_l3mod_exg_ctrl_resp
{
	UINT8  actionId;
	msgie_struct_zhbhjt_element_ul2cloud_t ul2Cloud;
}msg_struct_l3mod_exg_ctrl_resp_t;

//MSG_ID_ZHBL3MOD_EXG_DATA_REPORT,
typedef struct  msg_struct_l3mod_exg_data_report
{
	UINT8  actionId;
	msgie_struct_zhbhjt_element_ul2cloud_t ul2Cloud;
}msg_struct_l3mod_exg_data_report_t;

//L3HATE:测试环境对应的消息
//MSG_ID_L3HATE_TC_START,
typedef struct  msg_struct_l3hate_tc_start
{
	UINT32 length;
}msg_struct_l3hate_tc_start_t;

//MSG_ID_ETH_L3HATE_FRAME_RCV,
#define HCU_SYSMSG_HATE_BUF_LEN_MAX  	HCU_SYSMSG_COM_MSG_BODY_LEN_MAX - 8
typedef struct  msg_struct_l3hate_eth_frame_rcv
{
	UINT8  dataBuf[HCU_SYSMSG_HATE_BUF_LEN_MAX];
	UINT32 bufValidLen;
	UINT32 length;
}msg_struct_l3hate_eth_frame_rcv_t;

//MSG_ID_SPS_L3HATE_FRAME_RCV,
typedef struct  msg_struct_l3hate_sps_frame_rcv
{
	UINT8  dataBuf[HCU_SYSMSG_HATE_BUF_LEN_MAX];
	UINT32 bufValidLen;
	UINT32 length;
}msg_struct_l3hate_sps_frame_rcv_t;

//MSG_ID_CAN_L3HATE_FRAME_RCV,
typedef struct  msg_struct_l3hate_can_frame_rcv
{
	UINT8  dataBuf[HCU_SYSMSG_HATE_BUF_LEN_MAX];
	UINT32 bufValidLen;
	UINT32 length;
}msg_struct_l3hate_can_frame_rcv_t;

//MSG_ID_HUICOBUS_L3HATE_FRAME_RCV,
typedef struct  msg_struct_l3hate_mqtt_frame_rcv
{
	UINT8  dataBuf[HCU_SYSMSG_HATE_BUF_LEN_MAX];
	UINT32 bufValidLen;
	UINT32 length;
}msg_struct_l3hate_mqtt_frame_rcv_t;

/*
 *
 *
 *   //组合秤的统计报表合数数据结构
 *
 *
 */

typedef struct HcuSysMsgIeL3bfscContextStaElement
{
	UINT32	wsIncMatCnt;  			//物料数量
	float	wsIncMatWgt;  			//物料重量
	UINT32	wsCombTimes;  			//总共成功素搜到目标的次数
	UINT32	wsTttTimes;  			//TTT次数
	UINT32	wsTgvTimes;  			//TGV次数
	UINT32	wsTttMatCnt;			//TTT物料数量
	UINT32	wsTgvMatCnt;			//TGV物料数量
	float	wsTttMatWgt;			//TTT物料重量
	float	wsTgvMatWgt;			//TGV物料重量
	UINT32	wsAvgTttTimes;			//TTT平均次数
	UINT32	wsAvgTttMatCnt;			//TTT平均物料数
	float	wsAvgTttMatWgt;			//TTT平均重量
}HcuSysMsgIeL3bfscContextStaElement_t;

typedef struct HcuSysMsgIeL3bfdfContextStaElement
{
	UINT32	wsIncMatCnt;  			//物料数量
	UINT64	wsIncMatWgt;  			//物料重量
	UINT32	wsCombTimes;  			//总共成功素搜到目标的次数
	UINT32	wsTttTimes;  			//TTT次数
	UINT32	wsTgvTimes;  			//TGV次数
	UINT32	wsTttMatCnt;			//TTT物料数量
	UINT32	wsTgvMatCnt;			//TGV物料数量
	UINT64	wsTttMatWgt;			//TTT物料重量
	UINT64	wsTgvMatWgt;			//TGV物料重量
	UINT32  wsCallCellTimes;		//成功出料到外部的次数
	UINT32	wsAvgTttTimes;			//TTT平均次数
	UINT32	wsAvgTttMatCnt;			//TTT平均物料数
	UINT64	wsAvgTttMatWgt;			//TTT平均重量
}HcuSysMsgIeL3bfdfContextStaElement_t;

typedef struct HcuSysMsgIeL3bfhsContextStaElement
{
	UINT32	wsIncMatCnt;  		//物料数量
	UINT64	wsIncMatWgt;  		//物料重量
	UINT32  wsNormalCnt;		//正常
	UINT64  wsNormalWgt;		//正常
	UINT32  wsOverCnt;			//超重
	UINT64  wsOverWgt;			//超重
	UINT32  wsUnderTotalCnt;	//欠重
	UINT64	wsUnderTotalWgt;	//欠重
	UINT32  wsUnderTu1Cnt;		//欠重TU1
	UINT64	wsUnderTu1Wgt;		//欠重TU1
	UINT32  wsUnderTu2Cnt;		//欠重TU2
	UINT64	wsUnderTu2Wgt;		//欠重TU2
	UINT32  wsUnspecificCnt;	//非特定
	UINT64	wsUnspecificWgt;	//非特定
	UINT32  wsAvgMatTimes;		//平均进料次数
	UINT32  wsAvgTttTimes;		//平均成功次数
	UINT32  wsAvgTgvTimes;		//平均失败次数
}HcuSysMsgIeL3bfhsContextStaElement_t;

/*
 *
 *
 *   //组合秤的CALLCELL数据结构
 *
 *
 */
typedef struct HcuSysMsgIeL3bfscCallcellElement
{
	UINT32 sid;
	UINT32 configId;
	UINT32 timestamp;
	char   operator[20];
	UINT32 targetWeight; //in NF2
	UINT32 upLimitWeight; //in NF2
	UINT32 actualWeight; //in NF2
}HcuSysMsgIeL3bfscCallcellElement_t;

typedef struct HcuSysMsgIeL3bfdfCallcellElement
{
	UINT32 sid;
	UINT32 configId;
	UINT32 timestamp;
	char   operator[20];
	UINT8  groupId;
	UINT32 groupRangeHigh; //in NF2
	UINT32 groupRangeLow;  //in NF2
	UINT32 targetWeight; //in NF2
	UINT32 upLimitWeight; //in NF2
	UINT32 actualWeight; //in NF2
	UINT8  streamLine;
	UINT8  hopperId;
}HcuSysMsgIeL3bfdfCallcellElement_t;

typedef struct HcuSysMsgIeL3bfhsCallcellElement
{
	UINT32 sid;
	UINT32 configId;
	UINT32 timestamp;
	char   operator[20];
	UINT32 targetWeight; //in NF2
	UINT32 tu1; //in NF2
	UINT32 tu2; //in NF2
	UINT32 actualWeight; //in NF2
	UINT8  state;
}HcuSysMsgIeL3bfhsCallcellElement_t;

/*
 *
 *
 *   //软件下载数据结构
 *
 *
 */
#define		HCU_SYSMSG_SYSSWM_SW_PKG_FILE_NAME_MAX_LEN		61
typedef struct HcuSysMsgIeL3SysSwmSwPkgElement
{
	UINT8	equEntry;  			//标识是由谁去触发软件下载的标识
	UINT16	hwType;
	UINT16	hwPem;
	UINT16	swRel;
	UINT16	swVer;
	UINT16	dbVer;
	UINT8	upgradeFlag;
	UINT32	swTotalLen;
	UINT16	swCksum;
	UINT32	dbTotalLen;
	UINT16	dbCksum;
	char	fileName[HCU_SYSMSG_SYSSWM_SW_PKG_FILE_NAME_MAX_LEN];
	char	dbName[HCU_SYSMSG_SYSSWM_SW_PKG_FILE_NAME_MAX_LEN];
	char	currentActive[11];
	UINT32	updateTime;
}HcuSysMsgIeL3SysSwmSwPkgElement_t;

#define HCU_SYSMSG_SYSSWM_EQU_ENTRY_NULL  			0
#define HCU_SYSMSG_SYSSWM_EQU_ENTRY_HCU_CLIENT_SW 	1
#define HCU_SYSMSG_SYSSWM_EQU_ENTRY_HCU_CLIENT_DB 	2
#define HCU_SYSMSG_SYSSWM_EQU_ENTRY_IHU_CLIENT 		3
#define HCU_SYSMSG_SYSSWM_EQU_ENTRY_IHU_SERVER 		4   //用于ＳＵＩ接口，HCU-IHU之间的软件下载
#define HCU_SYSMSG_SYSSWM_EQU_ENTRY_INVALID  		0xFF
//必须跟以下定义相互兼容，不然将出现编解码问题
//#define HUITP_IEID_UNI_EQU_ENTRY_NONE 0
//#define HUITP_IEID_UNI_EQU_ENTRY_HCU 	1
//#define HUITP_IEID_UNI_EQU_ENTRY_IHU 	2
//#define HUITP_IEID_SUI_EQU_ENTRY_INVALID 0xFF
#define HCU_SYSMSG_SYSSWM_CUR_ACTIVE_START 		"START"
#define HCU_SYSMSG_SYSSWM_CUR_ACTIVE_HALF_COMP 	"HALF_COMP"
#define HCU_SYSMSG_SYSSWM_CUR_ACTIVE_COMPLETE 	"COMPLETE"
#define HCU_SYSMSG_SYSSWM_CUR_ACTIVE_RUNNING 	"RUNNING"  //暂时没有设置这个标识


typedef struct HcuSysMsgIeL3SysSwmSwDlElement
{
	UINT8	equEntry;  			//标识是由谁去触发软件下载的标识
	UINT16	hwType;
	UINT16	hwPem;
	UINT16	swRel;
	UINT16	verId;
	UINT8	upgradeFlag;
	UINT32	totalLen;
	UINT16	checksum;
	UINT8	nodeId;
	UINT16	segTotal;
	UINT16	segIndex;
	UINT16	segSplitLen;
	UINT16 	segValidLen;
	UINT16	segCkSum;
	char	fileName[HCU_SYSMSG_SYSSWM_SW_PKG_FILE_NAME_MAX_LEN];
	UINT32	dlTime;
}HcuSysMsgIeL3SysSwmSwDlElement_t;


/*
 *
 *
 *   //扬尘监测系统的统计报表表单
 *
 *
 */

typedef struct HcuSysMsgIeL3aqycContextStaElement
{
	UINT8   a34001_PolId;
	char    a34001_Flag;			//总悬浮颗粒物浓度实时字段标识（见规范数据标识符定义，有效数据：N, 设备校准：C..）
	float	a34001_Avg;  			//总悬浮颗粒物浓度监测时间内的均值
	float	a34001_Max;  			//总悬浮颗粒物浓度监测时间内最大值
	float	a34001_Min;  			//总悬浮颗粒物浓度监测时间内最小值
	//float   a34001_Cou;


	UINT8   a50001_PolId;
	char    a50001_Flag;			//噪声实时字段标识（见规范数据标识符定义，有效数据：N, 设备校准：C..）
	float	a50001_Avg;  			//噪声监测时间内的均值
	float	a50001_Max;  			//噪声监测时间内最大值
	float	a50001_Min;  			//噪声监测时间内最小值
	//float   a50001_Cou;


	UINT8   a01001_PolId;
	char    a01001_Flag;			//温度实时字段标识（见规范数据标识符定义，有效数据：N, 设备校准：C..）
	float	a01001_Avg;  			//温度监测时间内的均值
	float	a01001_Max;  			//温度监测时间内最大值
	float	a01001_Min;  			//温度监测时间内最小值
	//float   a01001_Cou;


	UINT8   a01002_PolId;
	char    a01002_Flag;			//湿度实时字段标识（见规范数据标识符定义，有效数据：N, 设备校准：C..）
	float	a01002_Avg;  			//湿监测时间内的均值
	float	a01002_Max;  			//湿度监测时间内最大值
	float	a01002_Min;  			//湿度监测时间内最小值
	//float   a01002_Cou;

	//气压暂不支持但先定义此处，粉尘和雨量未使用暂不定义
	UINT8   a01006_PolId;
	char    a01006_Flag;			//气压实时字段标识（见规范数据标识符定义，有效数据：N, 设备校准：C..）
	float	a01006_Avg;  			//气压测时间内的均值
	float	a01006_Max;  			//气压监测时间内最大值
	float	a01006_Min;  			//气压监测时间内最小值
	//float   a01006_Cou;

	//风速风向无最大最小值要求，先定义成统一格式以后按需要再行调整
	UINT8   a01007_PolId;
	char    a01007_Flag;			//风速实时字段标识（见规范数据标识符定义，有效数据：N, 设备校准：C..）
	float	a01007_Avg;  			//风速测时间内的均值
	float	a01007_Max;  			//风速监测时间内最大值
	float	a01007_Min;  			//风速监测时间内最小值
	//float   a01007_Cou;

	UINT8   a01008_PolId;
	char    a01008_Flag;			//风向实时字段标识（见规范数据标识符定义，有效数据：N, 设备校准：C..）
	float	a01008_Avg;  			//风向测时间内的均值
	float	a01008_Max;  			//风向监测时间内最大值
	float	a01008_Min;  			//风向监测时间内最小值
	//float   a01008_Cou;

}HcuSysMsgIeL3aqycContextStaElement_t;


/*
 *
 *
 *   //扬尘监测系统的实时数据表单
 *
 *
 */

typedef struct HcuSysMsgIeL3aqycContextCurrentElement
{
	UINT8   a34001_PolId;
	char    a34001_Flag;			//总悬浮颗粒物浓度实时字段标识（见规范数据标识符定义，有效数据：N, 设备校准：C..）
	float	a34001_Avg;  			//总悬浮颗粒物浓度监测时间内的均值

	UINT8   a50001_PolId;
	char    a50001_Flag;			//噪声实时字段标识（见规范数据标识符定义，有效数据：N, 设备校准：C..）
	float	a50001_Avg;  			//噪声监测时间内的均值

	UINT8   a01001_PolId;
	char    a01001_Flag;			//温度实时字段标识（见规范数据标识符定义，有效数据：N, 设备校准：C..）
	float	a01001_Avg;  			//温度监测时间内的均值

	UINT8   a01002_PolId;
	char    a01002_Flag;			//湿度实时字段标识（见规范数据标识符定义，有效数据：N, 设备校准：C..）
	float	a01002_Avg;  			//湿监测时间内的均值

	//气压暂不支持但先定义此处，粉尘和雨量未使用暂不定义
	UINT8   a01006_PolId;
	char    a01006_Flag;			//气压实时字段标识（见规范数据标识符定义，有效数据：N, 设备校准：C..）
	float	a01006_Avg;  			//气压测时间内的均值

	//风速风向无最大最小值要求，先定义成统一格式以后按需要再行调整
	UINT8   a01007_PolId;
	char    a01007_Flag;			//风速实时字段标识（见规范数据标识符定义，有效数据：N, 设备校准：C..）
	float	a01007_Avg;  			//风速测时间内的均值

	UINT8   a01008_PolId;
	char    a01008_Flag;			//风向实时字段标识（见规范数据标识符定义，有效数据：N, 设备校准：C..）
	float	a01008_Avg;  			//风向测时间内的均值
}HcuSysMsgIeL3aqycContextCurrentElement_t;



/*
 *
 *
 *   //扬尘监测系统的设备运行状态
 *
 *
 */

typedef struct HcuSysMsgIeL3aqycContextEqtStatusElement
{
	UINT8   a34001_PolId;
	UINT8   a34001_RS;			//扬尘传感器设备运行状态

	UINT8   a50001_PolId;
	UINT8   a50001_RS;

	UINT8   a01001_PolId;
	UINT8   a01001_RS;

	UINT8   a01002_PolId;
	UINT8	a01002_RS;

	UINT8   a01006_PolId;
	UINT8	a01006_RS;

	UINT8   a01007_PolId;
	UINT8	a01007_RS;

	UINT8   a01008_PolId;
	UINT8	a01008_RS;
}HcuSysMsgIeL3aqycContextEqtStatusElement_t;

/*
 *
 *
 *   //扬尘监测的统计报表数据标识符判断值
 *
 *
 */
typedef struct HcuSysL3aqycValueJudgement
{
	//颗粒物监测仪量程
	float   a34001_Range_Max;
	float   a34001_Range_Min;

	//噪声监测仪量程
	float   a50001_Range_Max;
	float   a50001_Range_Min;

	//温度监测仪量程
	float   a01001_Range_Max;
	float   a01001_Range_Min;

	//湿度监测仪量程
	float   a01002_Range_Max;
	float   a01002_Range_Min;

	//风速监测仪量程
	float   a01007_Range_Max;
	float   a01007_Range_Min;

	//风向监测仪量程
	float   a01008_Range_Max;
	float   a01008_Range_Min;

	//气压监测仪量程
	float   a01006_Range_Max;
	float   a01006_Range_Min;
	/////////////////////////////////////

	//颗粒物监测仪数据设定范围
	float   a34001_Range_Set_Max;
	float   a34001_Range_Set_Min;

	//噪声监测仪数据设定范围
	float   a50001_Range_Set_Max;
	float   a50001_Range_Set_Min;

	//温度监测仪数据设定范围
	float   a01001_Range_Set_Max;
	float   a01001_Range_Set_Min;

	//湿度监测仪数据设定范围
	float   a01002_Range_Set_Max;
	float   a01002_Range_Set_Min;

	//风速监测仪数据设定范围
	float   a01007_Range_Set_Max;
	float   a01007_Range_Set_Min;

	//风向监测仪数据设定范围
	float   a01008_Range_Set_Max;
	float   a01008_Range_Set_Min;

	//气压监测仪数据设定范围
	float   a01006_Range_Set_Max;
	float   a01006_Range_Set_Min;
	/////////////////////////////////////

/*
	//颗粒物监测仪告警设定范围
	float   a34001_Alarm_UpValue;
	float   a34001_Alarm_LowValue;

	//噪声监测仪告警设定范围
	float   a50001_Alarm_UpValue;
	float   a50001_Alarm_LowValue;

	//温度监测仪告警设定范围
	float   a01001_Alarm_UpValue;
	float   a01001_Alarm_LowValue;

	//湿度监测仪告警设定范围
	float   a01002_Alarm_UpValue;
	float   a01002_Alarm_LowValue;

	//风速监测仪告警设定范围
	float   a01007_Alarm_UpValue;
	float   a01007_Alarm_LowValue;

	//风向监测仪告警设定范围
	float   a01008_Alarm_UpValue;
	float   a01008_Alarm_LowValue;

	//气压监测仪告警设定范围
	float   a01006_Alarm_UpValue;
	float   a01006_Alarm_LowValue;
	/////////////////////////////////////
*/

	//颗粒监测仪校准标志位
	UINT32   a34001_Calibration_Flag;

	//噪声监测仪校准标志位
	UINT32   a50001_Calibration_Flag;


	//温度监测仪校准标志位
	UINT32   a01001_Calibration_Flag;

	//湿度监测仪校准标志位
	UINT32   a01002_Calibration_Flag;

	//风速监测仪校准标志位
	UINT32   a01007_Calibration_Flag;

	//风向监测仪校准标志位
	UINT32   a01008_Calibration_Flag;

	//气压监测仪校准标志位
	UINT32   a01006_Calibration_Flag;

	//雨，雪，雷，电标志位
	UINT32 RainSnow_Flag;

	//最大风速值（噪声仪失效）
	float WindThresholdForNoise;

}HcuSysL3aqycValueJudgement_t;


/*
 *
 *
 *   //扬尘监测的统计报表数据标识符判断值
 *
 *
 */
typedef struct HcuSysL3aqycAlarmLimit
{
	//颗粒物监测仪告警设定范围
	float   a34001_Alarm_UpValue;
	float   a34001_Alarm_LowValue;

	//噪声监测仪告警设定范围
	float   a50001_Alarm_UpValue;
	float   a50001_Alarm_LowValue;

	//温度监测仪告警设定范围
	float   a01001_Alarm_UpValue;
	float   a01001_Alarm_LowValue;

	//湿度监测仪告警设定范围
	float   a01002_Alarm_UpValue;
	float   a01002_Alarm_LowValue;

	//风速监测仪告警设定范围
	float   a01007_Alarm_UpValue;
	float   a01007_Alarm_LowValue;

	//风向监测仪告警设定范围
	float   a01008_Alarm_UpValue;
	float   a01008_Alarm_LowValue;

	//气压监测仪告警设定范围
	float   a01006_Alarm_UpValue;
	float   a01006_Alarm_LowValue;
	/////////////////////////////////////


}HcuSysL3aqycAlarmLimit_t;



#endif /* L0COMVM_COMMSG_H_ */





