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
	MSG_ID_CANITFLEO_DATA_REPORT,

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

	//BFSC项目：L3BFSC SCALE_WEIGHT / CANITFLEO组合秤
	MSG_ID_L3BFSC_CAN_SYS_CFG_REQ,
	MSG_ID_CAN_L3BFSC_SYS_CFG_RESP,
	MSG_ID_L3BFSC_CAN_SYS_START_REQ,
	MSG_ID_CAN_L3BFSC_SYS_START_RESP,
	MSG_ID_L3BFSC_CAN_SYS_STOP_REQ,
	MSG_ID_CAN_L3BFSC_SYS_STOP_RESP,

	MSG_ID_CAN_L3BFSC_WS_NEW_READY_EVENT,  	//传感器新数据事件
	MSG_ID_L3BFSC_CAN_WS_COMB_OUT,  		//出料
	MSG_ID_CAN_L3BFSC_WS_COMB_OUT_FB,  		//出料确认
	MSG_ID_L3BFSC_CAN_WS_GIVE_UP,   		//放弃物料
	MSG_ID_CAN_L3BFSC_WS_GIVE_UP_FB,   		//放弃物料确认
	MSG_ID_L3BFSC_CAN_ERROR_INQ_CMD_REQ,  	//差错情况下的查询请求  TBD
	MSG_ID_CAN_L3BFSC_ERROR_INQ_CMD_RESP,   //差错情况下的查询反馈
	MSG_ID_CAN_L2FRAME_RCV,  //MYC 2017/05/15  TBD

	//BFSC项目：BFSCUICOMM
	MSG_ID_UICOMM_L3BFSC_CMD_REQ,       	//命令请求
	MSG_ID_L3BFSC_UICOMM_CMD_RESP,  		//命令反馈
	MSG_ID_UICOMM_L3BFSC_CFG_REQ,  			//配置信息
	MSG_ID_L3BFSC_UICOMM_CFG_RESP,  		//配置结果
	MSG_ID_UICOMM_CAN_TEST_CMD_REQ,  		//测试命令
	MSG_ID_CAN_UICOMM_TEST_CMD_RESP,  		//测试结果


	//BFSC项目：MYC
	MSG_ID_L3BFSC_WMC_STARTUP_IND,          //       = 0x3B90,
	MSG_ID_L3BFSC_WMC_SET_CONFIG_REQ,       //       = 0x3B11,
	MSG_ID_L3BFSC_WMC_SET_CONFIG_RESP,      //       = 0x3B91,
	MSG_ID_L3BFSC_WMC_START_REQ,            //       = 0x3B12,
	MSG_ID_L3BFSC_WMC_START_RESP,           //       = 0x3B92,
	MSG_ID_L3BFSC_WMC_STOP_REQ,             //       = 0x3B13,
	MSG_ID_L3BFSC_WMC_STOP_RESP,            //       = 0x3B93,
	MSG_ID_L3BFSC_WMC_NEW_WS_EVENT,         //       = 0x3B94,
	MSG_ID_L3BFSC_WMC_REPEAT_WS_EVENT,		//		 = 0x3B95,
	MSG_ID_L3BFSC_WMC_WS_COMB_OUT_REQ,      //       = 0x3B15,
	MSG_ID_L3BFSC_WMC_WS_COMB_OUT_RESP,     //       = 0x3B95,
	MSG_ID_L3BFSC_WMC_COMMAND_REQ,          //       = 0x3B17,
	MSG_ID_L3BFSC_WMC_COMMAND_RESP,         //       = 0x3B97,
	MSG_ID_L3BFSC_WMC_FAULT_IND,            //       = 0x3B98,
	MSG_ID_L3BFSC_WMC_ERR_INQ_CMD_REQ,      //       = 0x3B19,
	MSG_ID_L3BFSC_WMC_ERR_INQ_CMD_RESP,     //       = 0x3B99,

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
	msgie_struct_com_gps_pos_t gps;
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
	UINT8 dataFormat;
	UINT32 emcValue;
	UINT32 tempValue;
	UINT32 humidValue;
	UINT32 winddirValue;
	UINT32 windspdValue;
	UINT32 pm1d0Value;
	UINT32 pm2d5Value;
	UINT32 pm10Value;
	UINT32 noiseValue;
	UINT32 airprsValue;
	msgie_struct_com_gps_pos_t gps;
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
	char   linkName[HCU_SYSDIM_FILE_NAME_LEN_MAX];
	msgie_struct_com_gps_pos_t gps;
	UINT32 timeStampStart;
	UINT32 timeStampEnd;
	UINT32 nTimes;
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
	UINT32 mem_occupy;
	UINT32 disk_occupy;
	UINT32 timeStamp;
	UINT32 length;
}msg_struct_com_pm_report_t;

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
#define HCU_SYSMSG_SPSPM_ALARM_DESC_LEN_MAX 100
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
	char   alarmDesc[HCU_SYSMSG_SPSPM_ALARM_DESC_LEN_MAX];
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
	char   alarmDesc[HCU_SYSMSG_SPSPM_ALARM_DESC_LEN_MAX];
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
#define HCU_SYSMSG_SYSSWM_INVENTORY_ELEMENT_DESC_LEN_MAX 50
typedef struct msg_struct_cloudvela_spspm_inventory_req
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseReq;
	UINT16 hwType;
	UINT16 hwId;
	UINT16 swRel;
	UINT16 swVer;
	UINT8  upgradeFlag;
	char   desc[HCU_SYSMSG_SYSSWM_INVENTORY_ELEMENT_DESC_LEN_MAX];
	UINT32 length;
}msg_struct_cloudvela_spspm_inventory_req_t;

//MSG_ID_SYSSWM_CLOUDVELA_INVENTORY_RESP,
typedef struct msg_struct_spspm_cloudvela_inventory_resp
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseResp;
	UINT16 hwType;
	UINT16 hwId;
	UINT16 swRel;
	UINT16 swVer;
	UINT8  upgradeFlag;
	char   desc[HCU_SYSMSG_SYSSWM_INVENTORY_ELEMENT_DESC_LEN_MAX];
	UINT32 length;
}msg_struct_spspm_cloudvela_inventory_resp_t;

//MSG_ID_SYSSWM_CLOUDVELA_INVENTORY_REPORT,
typedef struct msg_struct_spspm_cloudvela_inventory_report
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseReport;
	UINT16 hwType;
	UINT16 hwId;
	UINT16 swRel;
	UINT16 swVer;
	UINT8  upgradeFlag;
	UINT32 timeStamp;
	char   desc[HCU_SYSMSG_SYSSWM_INVENTORY_ELEMENT_DESC_LEN_MAX];
	UINT32 length;
}msg_struct_spspm_cloudvela_inventory_report_t;

//MSG_ID_CLOUDVELA_SYSSWM_INVENTORY_CONFIRM,
typedef struct msg_struct_cloudvela_spspm_inventory_confirm
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseConfirm;
	UINT16 hwType;
	UINT16 hwId;
	UINT16 swRel;
	UINT16 swVer;
	UINT8  upgradeFlag;
	char   desc[HCU_SYSMSG_SYSSWM_INVENTORY_ELEMENT_DESC_LEN_MAX];
	UINT32 length;
}msg_struct_cloudvela_spspm_inventory_confirm_t;

//MSG_ID_CLOUDVELA_SYSSWM_SW_PACKAGE_REQ,
#define HCU_SYSMSG_SYSSWM_SW_PACKAGE_BODY_MAX_LEN 400
typedef struct msg_struct_cloudvela_spspm_sw_package_req
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseReq;
	UINT16 segIndex;
	UINT16 segTotal;
	UINT16 segLen;
	UINT16 validLen;
	UINT8  body[HCU_SYSMSG_SYSSWM_SW_PACKAGE_BODY_MAX_LEN];
	UINT32 length;
}msg_struct_cloudvela_spspm_sw_package_req_t;

//MSG_ID_SYSSWM_CLOUDVELA_SW_PACKAGE_RESP,
typedef struct msg_struct_spspm_cloudvela_sw_package_resp
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseResp;
	UINT16 segIndex;
	UINT16 segTotal;
	UINT16 segLen;
	UINT32 length;
}msg_struct_spspm_cloudvela_sw_package_resp_t;

//MSG_ID_SYSSWM_CLOUDVELA_SW_PACKAGE_REPORT,
typedef struct msg_struct_spspm_cloudvela_sw_package_report
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseReport;
	UINT16 segIndex;
	UINT16 segTotal;
	UINT16 segLen;
	UINT32 length;
}msg_struct_spspm_cloudvela_sw_package_report_t;

//MSG_ID_CLOUDVELA_SYSSWM_SW_PACKAGE_CONFIRM,
typedef struct msg_struct_cloudvela_spspm_sw_packag_confirm
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseConfirm;
	UINT16 segIndex;
	UINT16 segTotal;
	UINT16 segLen;
	UINT16 validLen;
	UINT8  body[HCU_SYSMSG_SYSSWM_SW_PACKAGE_BODY_MAX_LEN];
	UINT32 length;
}msg_struct_cloudvela_spspm_sw_packag_confirm_t;


//CANITF
//MSG_ID_CANITFLEO_DATA_REPORT,
typedef struct msg_struct_canitfleo_data_report
{
	UINT8  usercmdid;
	UINT32 timeStamp;
	UINT32 length;
}msg_struct_canitfleo_data_report_t;


/**************************************************************************************
 *
 *  分类处理各个项目相关的消息结构
 *
 *************************************************************************************/
//BFSC/SCALE_WEIGHT组合秤
//MSG_ID_L3BFSC_CAN_ERROR_INQ_CMD_REQ,  	//差错情况下的查询请求
#define HCU_SYSMSG_L3BFSC_MAX_SENSOR_NBR 20

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


//BFSC项目：L3BFSC SCALE_WEIGHT组合秤
//BFSC项目：CANITFLEO
//MSG_ID_L3BFSC_CAN_ERROR_INQ_CMD_REQ,  	//差错情况下的查询请求  TBD
typedef struct msg_struct_l3bfsc_can_error_inq_cmd_req
{
	UINT8  sensorid;
	UINT8  sensorBitmap[HCU_SYSMSG_L3BFSC_MAX_SENSOR_NBR];
	UINT32 length;
}msg_struct_l3bfsc_can_error_inq_cmd_req_t;

//MSG_ID_CAN_L3BFSC_ERROR_INQ_CMD_RESP,   //差错情况下的查询反馈
typedef struct msg_struct_can_l3bfsc_error_inq_cmd_resp
{
	UINT8 sensorid;
	UINT32 sensorWsValue;
	UINT8 flag;
	UINT32 length;
}msg_struct_can_l3bfsc_error_inq_cmd_resp_t;

//MSG_ID_CAN_L2FRAME_RCV,  //MYC 2017/05/15  TBD

//MSG_ID_L3BFSC_CAN_WS_INIT_REQ,  		//传感器初始化  TBD
//typedef struct msg_struct_l3bfsc_can_ws_init_req
//{
//	UINT8  wsBitmap[HCU_SYSMSG_L3BFSC_MAX_SENSOR_NBR];
//	UINT32 length;
//}msg_struct_l3bfsc_can_ws_init_req_t;

//MSG_ID_CAN_L3BFSC_WS_INIT_FB,       	//传感器初始化确认
//typedef struct msg_struct_can_l3bfsc_ws_init_fb
//{
//	UINT8  sensorid;
//	UINT8  initFlag;
//	UINT32 length;
//}msg_struct_can_l3bfsc_ws_init_fb_t;

//MSG_ID_L3BFSC_CAN_WS_READ_REQ,  		//所有传感器读取一次性读取请求  TBD
//typedef struct msg_struct_l3bfsc_can_ws_read_req
//{
//	UINT8 sensorid;
//	UINT8  wsBitmap[HCU_SYSMSG_L3BFSC_MAX_SENSOR_NBR];
//	UINT32 length;
//}msg_struct_l3bfsc_can_ws_read_req_t;

//MSG_ID_CAN_L3BFSC_WS_READ_RESP,  		//所有传感器读取一次性读取确认
//typedef struct msg_struct_can_l3bfsc_ws_read_resp
//{
//	UINT32 sensorWsValue[HCU_SYSMSG_L3BFSC_MAX_SENSOR_NBR];
//	UINT32 length;
//}msg_struct_can_l3bfsc_ws_read_resp_t;

//MSG_ID_L3BFSC_CAN_WS_COMB_OUT,  		//出料
typedef struct msg_struct_l3bfsc_can_ws_comb_out
{
	UINT8  combnbr;
	UINT8  sensorBitmap[HCU_SYSMSG_L3BFSC_MAX_SENSOR_NBR];
	UINT32 length;
}msg_struct_l3bfsc_can_ws_comb_out_t;

//MSG_ID_CAN_L3BFSC_WS_COMB_OUT_FB,  		//出料确认
typedef struct msg_struct_can_l3bfsc_ws_comb_out_fb
{
	UINT8  sensorid;
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
	UINT32 length;
}msg_struct_can_l3bfsc_ws_give_up_fb_t;

//MSG_ID_CAN_L3BFSC_WS_NEW_READY_EVENT,  	//传感器新数据事件
typedef struct msg_struct_can_l3bfsc_new_ready_event
{
	UINT8  sensorid;
	UINT32  sensorWsValue;
	UINT32 length;
}msg_struct_can_l3bfsc_new_ready_event_t;

//MSG_ID_L3BFSC_CAN_GENERAL_CMD_REQ,  	//来自后台的控制命令，只能在SCAN下工作
//typedef struct msg_struct_l3bfsc_can_general_cmd_req
//{
//	UINT8 sensorid;
//	UINT8 optid;
//	UINT8 optpar;
//	UINT32 modbusVal;
//	UINT32 length;
//}msg_struct_l3bfsc_can_general_cmd_req_t;

//MSG_ID_CAN_L3BFSC_GENERAL_CMD_RESP, 	//来自后台的控制命令反馈，只能在SCAN下工作
//typedef struct msg_struct_can_l3bfsc_general_cmd_resp
//{
//	UINT8  sensorid;
//	UINT8  cmdid;
//	UINT8  optid;
//	UINT8  optpar;
//	UINT32 modbusVal;
//	UINT32 length;
//}msg_struct_can_l3bfsc_general_cmd_resp_t;

//MSG_ID_L3BFSC_CAN_SYS_CFG_REQ,
typedef struct msg_struct_l3bfsc_can_sys_cfg_req
{
	UINT32 length;
}msg_struct_l3bfsc_can_sys_cfg_req_t;

//MSG_ID_CAN_L3BFSC_SYS_CFG_RESP,
typedef struct msg_struct_can_l3bfsc_sys_cfg_resp
{
	UINT32 length;
}msg_struct_can_l3bfsc_sys_cfg_resp_t;

//MSG_ID_L3BFSC_CAN_SYS_START_REQ,
typedef struct msg_struct_l3bfsc_can_sys_start_req
{
	UINT8  wsBitmap[HCU_SYSMSG_L3BFSC_MAX_SENSOR_NBR];
	UINT32 length;
}msg_struct_l3bfsc_can_sys_start_req_t;

//MSG_ID_CAN_L3BFSC_SYS_START_RESP,
typedef struct msg_struct_can_l3bfsc_sys_start_resp
{
	UINT8  wsBitmap[HCU_SYSMSG_L3BFSC_MAX_SENSOR_NBR];
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
	UINT8  wsBitmap[HCU_SYSMSG_L3BFSC_MAX_SENSOR_NBR];
	UINT32 length;
}msg_struct_can_l3bfsc_sys_stop_resp_t;


//BFSCUICOMM
//MSG_ID_UICOMM_L3BFSC_CMD_REQ,
#define HCU_SYSMSG_BFSC_UICOMM_CMDID_INVALL  	0
#define HCU_SYSMSG_BFSC_UICOMM_CMDID_START  	1
#define HCU_SYSMSG_BFSC_UICOMM_CMDID_STOP  		2
#define HCU_SYSMSG_BFSC_UICOMM_CMDID_SUSPEND  	3
#define HCU_SYSMSG_BFSC_UICOMM_CMDID_RESUME  	4
#define HCU_SYSMSG_BFSC_UICOMM_CMDID_NULL  		255
typedef struct msg_struct_uicomm_l3bfsc_cmd_req
{
	UINT8  cmdid;
	UINT32 timestamp;
	UINT32 length;
}msg_struct_uicomm_l3bfsc_cmd_req_t;
//MSG_ID_L3BFSC_UICOMM_CMD_RESP,
typedef struct msg_struct_l3bfsc_uicomm_cmd_resp
{
	UINT8  cmdid;
	UINT32 timestamp;
	UINT32 length;
}msg_struct_l3bfsc_uicomm_cmd_resp_t;

//MSG_ID_UICOMM_L3BFSC_CFG_REQ
typedef struct msg_struct_uicomm_l3bfsc_cfg_req
{
	UINT32  targetValue;
	UINT32	targetUpLimit;
	UINT32  parSetId; //指明是哪一套参数启作用了
	UINT8	minWsNbr;
	UINT8	maxWsNbr;
	UINT32 	length;
}msg_struct_uicomm_l3bfsc_cfg_req_t;

//MSG_ID_L3BFSC_UICOMM_CFG_RESP,  		//配置结果
typedef struct msg_struct_l3bfsc_uicomm_cfg_resp
{
	UINT32  targetValue;
	UINT32	targetUpLimit;
	UINT32  parSetId; //指明是哪一套参数启作用了
	UINT8	minWsNbr;
	UINT8	maxWsNbr;
	UINT32 	length;
}msg_struct_l3bfsc_uicomm_cfg_resp_t;

//MSG_ID_UICOMM_CAN_TEST_CMD_REQ,  		//测试命令
typedef struct msg_struct_uicomm_can_test_cmd_req
{
	UINT8   cmdid;
	UINT32 	length;
}msg_struct_uicomm_can_test_cmd_req_t;

//MSG_ID_CAN_UICOMM_TEST_CMD_RESP,  		//测试结果
typedef struct msg_struct_can_uicomm_test_cmd_resp
{
	UINT8   cmdid;
	UINT32 	length;
}msg_struct_can_uicomm_test_cmd_resp_t;


/*
** =============================================================================
** ============================ MYC START FOR BFSC =============================
** =============================================================================
*/
//MSG_ID_L3BFSC_WMC_STARTUP_IND,          //       = 0x3B90,
//MSG_ID_L3BFSC_WMC_SET_CONFIG_REQ,       //       = 0x3B11,
//MSG_ID_L3BFSC_WMC_SET_CONFIG_RESP,      //       = 0x3B91,
//MSG_ID_L3BFSC_WMC_START_REQ,            //       = 0x3B12,
//MSG_ID_L3BFSC_WMC_START_RESP,           //       = 0x3B92,
//MSG_ID_L3BFSC_WMC_STOP_REQ,             //       = 0x3B13,
//MSG_ID_L3BFSC_WMC_STOP_RESP,            //       = 0x3B93,
//MSG_ID_L3BFSC_WMC_NEW_WS_EVENT,         //       = 0x3B94,
//MSG_ID_L3BFSC_WMC_REPEAT_WS_EVENT,	  //	   = 0x3B95,
//MSG_ID_L3BFSC_WMC_WS_COMB_OUT_REQ,      //       = 0x3B15,
//MSG_ID_L3BFSC_WMC_WS_COMB_OUT_RESP,     //       = 0x3B95,
//MSG_ID_L3BFSC_WMC_COMMAND_REQ,          //       = 0x3B17,
//MSG_ID_L3BFSC_WMC_COMMAND_RESP,         //       = 0x3B97,
//MSG_ID_L3BFSC_WMC_FAULT_IND,            //       = 0x3B98,
//MSG_ID_L3BFSC_WMC_ERR_INQ_CMD_REQ,      //       = 0x3B19,
//MSG_ID_L3BFSC_WMC_ERR_INQ_CMD_RESP,     //       = 0x3B99,

// =====================================================================
// ========== FOLLOWING SHOULD BE SAME AS IN WMC, COPY START ===========
// =====================================================================
typedef struct WmcErrorCode
{
	UINT16 error_code;
	UINT16 spare1;
}WmcErrorCode_t;

typedef struct WmcId
{
	UINT8 wmc_id;               /* 0 ~ 15 is the DIP defined, ID 16 is the main rolling */
	UINT8 spare1;               /* 0 ~ 15 is the DIP defined, ID 16 is the main rolling */
	UINT8 spare2;               /* 0 ~ 15 is the DIP defined, ID 16 is the main rolling */
	UINT8 spare3;               /* 0 ~ 15 is the DIP defined, ID 16 is the main rolling */
}WmcId_t;

typedef struct msg_struct_l3bfsc_wmc_resp
{
	UINT16	msgid;
	UINT16  length;
	WmcId_t wmc_id;               /* 0 ~ 15 is the DIP defined, ID 16 is the main rolling */
	WmcErrorCode_t result;
}msg_struct_l3bfsc_wmc_resp_t;

typedef struct WmcInventory
{
	UINT32 hw_inventory_id;
	UINT32 sw_inventory_id;
	UINT32 stm32_cpu_id;
	UINT32 weight_sensor_type;
	UINT32 motor_type;
	WmcId_t wmc_id;               /* 0 ~ 15 is the DIP defined, ID 16 is the main rolling */
	UINT32 spare1;
	UINT32 spare2;
}WmcInventory_t;

//==========================================================
/*
**	MSG_ID_L3BFSC_WMC_STARTUP_IND,          //       = 0x3B90,
*/
typedef struct msg_struct_l3bfsc_wmc_startup_ind
{
	UINT16 msgid;
	UINT16 length;
	WmcInventory_t wmc_inventory;
}msg_struct_l3bfsc_wmc_startup_ind_t;

/*
**	MSG_ID_L3BFSC_WMC_SET_CONFIG_REQ,       //       = 0x3B11,
*/
typedef struct CombinationAlgorithmParamaters
{
	UINT32	MinScaleNumberCombination;				//组合搜索的最小Scale的个数
	UINT32	MaxScaleNumberCombination;				//组合搜索的最大Scale的个数
	UINT32	MinScaleNumberStartCombination;		//开始查找的最小个数，就是说大于这个个数就开始搜索
	UINT32	TargetCombinationWeight;				  //组合目标重量
	UINT32	TargetCombinationUpperWeight;			//组合目标重量上限
	UINT32	IsPriorityScaleEnabled;					  // 1: Enable, 0: Disable
	UINT32	IsProximitCombinationMode;				// 1: AboveUpperLimit, 2: BelowLowerLimit, 0: Disable
	UINT32	CombinationBias;						      //每个Scale要求放几个物品
	UINT32	IsRemainDetectionEnable;				  //Scale处于LAOD状态超过remainDetectionTimeS, 被认为是Remain状态，提示要将物品拿走: 1:Enable， 0：Disble
	UINT32	RemainDetectionTimeSec;					  // RemainDetionTime in Seconds
	UINT32	RemainScaleTreatment;					    // 1: Discharge (提示用户移走），0：Enforce（强制进行组合）
	UINT32	CombinationSpeedMode;					    // 0：SpeedPriority，1: PrecisePriority
	UINT32	CombinationAutoMode;					    // 0: Auto, 1: Manual
	UINT32	MovingAvrageSpeedCount;					  //计算平均速度的时候使用最近多少个组合做统计
	UINT32	spare1;
	UINT32	spare2;
	UINT32	spare3;
	UINT32	spare4;
}CombinationAlgorithmParamaters_t;

typedef struct WeightSensorParamaters
{
	UINT32	WeightSensorLoadDetectionTimeMs;		//称台稳定的判断时间
	UINT32	WeightSensorLoadThread;							//称台稳定门限，如果在WeightSensorLoadDetectionTime内，重量变化都小于WeightSensorLoadThread
	UINT32	WeightSensorEmptyThread;
	UINT32	WeightSensorEmptyDetectionTimeMs;
	UINT32	WeightSensorPickupThread;						// NOT for GUI
	UINT32	WeightSensorPickupDetectionTimeMs;	// NOT for GUI
	UINT32	StardardReadyTimeMs;								//???
	UINT32	MaxAllowedWeight;										//如果发现超过这个最大值，说明Sensor出错
	//UINT32	RemainDetectionTimeSec;					  // RemainDetionTime in Seconds

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
	//UINT32  WeightSensorFilterCoeff[32];				// NOT for GUI
	//UINT32  WeightSensorOutputValue[32];				// NOT for GUI
}WeightSensorParamaters_t;

typedef struct MotorControlParamaters
{
	UINT32	MotorSpeed;
	UINT32	MotorDirection;									//0: Clockwise; 1: Counter-Clockwise
	UINT32	MotorRollingStartMs;						//how long do the motor rolling for start action
	UINT32	MotorRollingStopMs;							//how long do the motor rolling for stop action
	UINT32	MotorRollingInveralMs;					//If the motor is rolling, how long the motor will stay in still before roll back (stop action).
	UINT32	MotorFailureDetectionVaration;	// % of the MotorSpeed
	UINT32	MotorFailureDetectionTimeMs;		// within TimeMs, 如果速度都在外面，认为故障
	UINT32	spare1;
	UINT32	spare2;
	UINT32	spare3;
	UINT32	spare4;
}MotorControlParamaters_t;

typedef struct msg_struct_l3bfsc_wmc_set_config_req
{
	UINT16 msgid;
	UINT16 length;
	WeightSensorParamaters_t weight_sensor_param;
	MotorControlParamaters_t motor_control_param;
}msg_struct_l3bfsc_wmc_set_config_req_t;

/*
**	MSG_ID_L3BFSC_WMC_START_REQ,            //       = 0x3B12,
*/
typedef struct msg_struct_l3bfsc_wmc_start_req
{
	UINT16 msgid;
	UINT16 length;
}msg_struct_l3bfsc_wmc_start_req_t;

/*
**	MSG_ID_L3BFSC_WMC_STOP_REQ,             //       = 0x3B13,
*/
typedef struct msg_struct_l3bfsc_wmc_stop_req
{
	UINT16 msgid;
	UINT16 length;
}msg_struct_l3bfsc_wmc_stop_req_t;

/*
**	MSG_ID_L3BFSC_WMC_NEW_WS_EVENT,         //       = 0x3B94,
**	MSG_ID_L3BFSC_WMC_REPEAT_WS_EVENT,	  	//	  	 = 0x3B95,
**
**	NOTE: These two are used for report Weight event, used same struct
**	MSG_ID_L3BFSC_WMC_NEW_WS_EVENT -> EMPTY -> LOAD
**	MSG_ID_L3BFSC_WMC_REPEAT_WS_EVENT -> LOAD -> LOAD. with repeat time, used for AWS to
**	**
**
*/
#define 	WEIGHT_EVENT_ID_LOAD						(0)
#define 	WEIGHT_EVENT_ID_EMPTY						(1)
#define 	WEIGHT_EVENT_ID_PICKUP						(2)

typedef struct WeightIndication
{
	UINT32 weight_event;		//LOAD, EMPTY, PICKUP(FFS)
	UINT32 average_weight;		//average value in the detect window  // <--- MUST
	//UINT32 repeat_times;		// always = 0 for MSG_ID_L3BFSC_WMC_NEW_WS_EVENT, = n for MSG_ID_L3BFSC_WMC_REPEAT_WS_EVENT
}WeightIndication_t;

typedef struct msg_struct_l3bfsc_wmc_ws_event
{
	UINT16 msgid;
	UINT16 length;
	WmcId_t wmc_id;             /* 0 ~ 15 is the DIP defined, ID 16 is the main rolling */
	WeightIndication_t weight_ind;
}msg_struct_l3bfsc_wmc_ws_event_t;

/*
**	MSG_ID_L3BFSC_WMC_WS_COMB_OUT_REQ,      //       = 0x3B15,
*/
//#define 	LED_COMMNAD_ID_IGNORE					(0)//MUSR BE 0
//#define 	LED_COMMNAD_ID_ON						(1)
//#define 	LED_COMMNAD_ID_OFF						(2)
//#define 	LED_COMMNAD_ID_BINKING_HIGHSPEED		(3)
//#define 	LED_COMMNAD_ID_BINKING_LOWSPEED			(4)

//#define 	MOTOR_COMMAND_ID_IGORE					(0) //MUSR BE 0
//#define 	MOTOR_COMMAND_ID_START					(1)
//#define 	MOTOR_COMMAND_ID_STOP					(2)
//#define 	MOTOR_COMMAND_ID_ROLLONCE				(3)
//#define 	MOTOR_COMMAND_ID_SPEED_READ				(4)

// CombineType Definition
// COMNINETPYE_ROOLOUT_START
// COMNINETPYE_ROOLOUT_COMPLETE
// COMNINETPYE_DROP_START
// COMNINETPYE_DROP_COMPLETE
// COMNINETPYE_WARNING_START
// COMNINETPYE_WARNING_COMPLETE
// COMNINETPYE_ERROR_START
// COMNINETPYE_ERROR_COMPLETE

typedef struct CombineType
{
	UINT32	WeightCombineType;
	UINT32	ActionDelayMs;
}CombineType_t;

typedef struct msg_struct_l3bfsc_wmc_comb_out_req
{
	UINT16 msgid;
	UINT16 length;
	CombineType_t weight_combin_type;
}msg_struct_l3bfsc_wmc_comb_out_req_t;

/*
**	MSG_ID_L3BFSC_WMC_FAULT_IND
*/
typedef struct msg_struct_l3bfsc_wmc_fault_ind
{
	UINT16 	msgid;
	UINT16 	length;
	WmcId_t wmc_id;               /* 0 ~ 15 is the DIP defined, ID 16 is the main rolling */
	UINT16	error_code;
}msg_struct_l3bfsc_wmc_fault_ind_t;

/*
**	MSG_ID_L3BFSC_WMC_COMMAND_REQ,          //       = 0x3B17,
*/
#define SENSOR_COMMAND_ID_WEITGH_READ (0x0001)
#define MOTOR_COMMAND_ID (0x0002)
#define LED1_COMMAND_ID (0x0004)
#define LED2_COMMAND_ID (0x0008)
#define LED3_COMMAND_ID (0x0010)
#define LED4_COMMAND_ID (0x0020)

#define 	LED_COMMNAD_ON						(1)
#define 	LED_COMMNAD_OFF						(2)
#define 	LED_COMMNAD_BINKING_HIGHSPEED		(3)
#define 	LED_COMMNAD_BINKING_LOWSPEED		(4)

#define 	SESOR_COMMAND_ID_IGORE						(0) //MUSR BE 0
#define 	SESOR_COMMAND_ID_WEITGH_READ				(3)
#define 	SESOR_COMMAND_ID_CALIBRATION_ZERO			(4)
#define 	SESOR_COMMAND_ID_CALIBRATION_FULL			(5)

typedef struct msg_struct_l3bfsc_wmc_command_req
{
	UINT32 msgid;
	UINT16 length;
	UINT16 spare;
	UINT32 wmc_state;
	UINT8 led1_command;
	UINT8 led2_command;
	UINT8 led3_command;
	UINT8 led4_command;
	UINT32 motor_command;
	UINT32 sensor_command;
}msg_struct_l3bfsc_wmc_command_req_t;

/*
**	MSG_ID_L3BFSC_WMC_COMMAND_RESP,         //       = 0x3B97,
*/
typedef struct msg_struct_l3bfsc_wmc_command_resp
{
	UINT16	msgid;
	UINT16	result;
	UINT16	error_code;
	UINT32 	motor_speed;
	UINT32 	sensor_weight;
}msg_struct_l3bfsc_wmc_command_resp_t;

/*
**	MSG_ID_L3BFSC_WMC_MSH_HEADER
*/
typedef struct msg_struct_l3bfsc_wmc_msg_header
{
	UINT16 msgid;
	UINT16 length;
}msg_struct_l3bfsc_wmc_msg_header_t;

//MSG_ID_CAN_L3BFSC_WMC_ERR_INQ_CMD_REQ,      //       = 0x3B19,
//TBD

//MSG_ID_CAN_L3BFSC_WMC_ERR_INQ_CMD_RESP,     //       = 0x3B99,
//TBD

/* Message Length definition */
#define 	MSG_SIZE_L3BFSC_WMC_STARTUP_IND				(sizeof(msg_struct_l3bfsc_wmc_startup_ind_t))
#define 	MSG_SIZE_L3BFSC_WMC_SET_CONFIG_REQ			(sizeof(msg_struct_l3bfsc_wmc_set_config_req_t))
#define 	MSG_SIZE_L3BFSC_WMC_SET_CONFIG_RESP			(sizeof(msg_struct_l3bfsc_wmc_resp_t))
#define 	MSG_SIZE_L3BFSC_WMC_START_REQ				(sizeof(msg_struct_l3bfsc_wmc_start_req_t))
#define 	MSG_SIZE_L3BFSC_WMC_START_RESP				(sizeof(msg_struct_l3bfsc_wmc_resp_t))
#define 	MSG_SIZE_L3BFSC_WMC_STOP_REQ				(sizeof(msg_struct_l3bfsc_wmc_stop_req_t))
#define 	MSG_SIZE_L3BFSC_WMC_STOP_RESP				(sizeof(msg_struct_l3bfsc_wmc_resp_t))
#define 	MSG_SIZE_L3BFSC_WMC_NEW_WS_EVENT			(sizeof(msg_struct_l3bfsc_wmc_ws_event_t))
#define 	MSG_SIZE_L3BFSC_WMC_REPEAT_WS_EVENT			(sizeof(msg_struct_l3bfsc_wmc_ws_event_t))
#define 	MSG_SIZE_L3BFSC_WMC_WS_COMB_OUT_REQ			(sizeof(msg_struct_l3bfsc_wmc_comb_out_req_t))
#define 	MSG_SIZE_L3BFSC_WMC_WS_COMB_OUT_RESP		(sizeof(msg_struct_l3bfsc_wmc_resp_t))
#define 	MSG_SIZE_L3BFSC_WMC_FAULT_IND				(sizeof(msg_struct_l3bfsc_wmc_fault_ind_t))
#define 	MSG_SIZE_L3BFSC_WMC_COMMAND_REQ				(sizeof(msg_struct_l3bfsc_wmc_command_req_t))
#define 	MSG_SIZE_L3BFSC_WMC_COMMAND_RESP			(sizeof(msg_struct_l3bfsc_wmc_command_resp_t))
#define 	MSG_SIZE_L3BFSC_WMC_ERR_INQ_CMD_REQ			(sizeof(msg_struct_l3bfsc_wmc_req_t))
#define 	MSG_SIZE_L3BFSC_WMC_ERR_INQ_CMD_RESP		(sizeof(msg_struct_l3bfsc_wmc_resp_t))

/* Can ID for communication between AWS and WMC */
/* 1: AWS to WMC: 1 to n, n = 0 ... 15       */
/* Node ID bitmap: CAN ID = 0x0010 XXXX    */
/* CAN ID          direction => ^  ~~~~ <= 1111 1111 1111 1111, */
#define AWS_TO_WMC_CAN_ID_PREFIX		(0x00100000U)
//#define AWS_TO_WMC_CAN_ID_SUFFIX		(mwc_id_bitmap)

/* 2: AWS to WMC-S: 1 to 1  */
/* CAN ID: 0x0010 0000 */
#define AWS_TO_WMCS_CAN_ID				(0x00110000U)

/* 3: WMC to AWS: */
/* CAN ID: 0x0030 0000 (WMC Node ID 0) to 0x0030 000F (WMC Node ID 15) */
#define WMC_TO_AWS_CAN_ID_PREFIX		(0x00300000U)
//#define WMC_TO_AWS_CAN_ID_SUFFIX		(mwc_id)

/* 4: WMC-S to AWS: */
/* CAN ID: 0x0030 0010 (Node ID 0) */
#define WMCS_TO_AWS_CAN_ID				(0x00300010U)

/* CAN Msg Length */
#define		MAX_WMC_CONTROL_MSG_LEN				(256U)
#define		MAX_WMC_CONTROL_MSG_HEADER_LEN		(sizeof(IHU_HUITP_L2FRAME_STD_frame_header_t))
#define		MAX_WMC_CONTROL_MSG_BODY_LEN		(MAX_WMC_CONTROL_MSG_LEN - MAX_WMC_CONTROL_MSG_HEADER_LEN)

/* ERROR CODE */
typedef enum IHU_ERROR_CODE
{
	//ERROR CODE ID
	ERROR_CODE_NO_ERROR = 0, //Starting point

	//COMMON ERROR CODE
	ERROR_CODE_CALLING_ERROR,
	ERROR_CODE_INPUT_PARAMETER_KO,
	ERROR_CODE_WRONG_WMC_STATE,
	ERROR_CODE_UNKNOWN,

	/* TO BE ADDED FOR EACH OF THE MESSAGE */
	//**	MSG_ID_L3BFSC_WMC_SET_CONFIG_REQ,

	//**	MSG_ID_L3BFSC_WMC_GET_CONFIG_REQ,

	//**	MSG_ID_L3BFSC_WMC_START_REQ,

	//**	MSG_ID_L3BFSC_WMC_STOP_REQ,

	//**	MSG_ID_L3BFSC_WMC_COMBIN_REQ,

	//**	MSG_ID_L3BFSC_WMC_COMMAND_REQ,

	ERROR_CODE_MAX, //Ending point

}error_code_t; //end of IHU_INTER_TASK_MSG_ID

// =====================================================================
// ============== ABOVE SHOULD BE SAME AS IN WMC, COPY END =============
// =====================================================================

/*
** =============================================================================
** ============================= MYC END FOR BFSC ==============================
** =============================================================================
*/

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
	sensor_zhb_transport_format_dl_t zhbDl;
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
	sensor_zhb_transport_format_ul_t zhbUl;
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
	sensor_zhb_transport_format_dl_t zhbDl;
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
	sensor_zhb_transport_format_ul_t zhbUl;
	UINT32 length;
}msg_struct_emc_cloudvela_ctrl_resp_t;

//MSG_ID_EMC_CLOUDVELA_DATA_REPORT,
typedef struct msg_struct_emc_cloudvela_data_report
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseReport;
	sensor_emc_data_element_t emc;
	sensor_zhb_transport_format_ul_t zhbUl;
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
	sensor_zhb_transport_format_dl_t zhbDl;
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
	sensor_zhb_transport_format_ul_t zhbUl;
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
	sensor_zhb_transport_format_dl_t zhbDl;
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
	sensor_zhb_transport_format_ul_t zhbUl;
	UINT32 length;
}msg_struct_pm25_cloudvela_ctrl_resp_t;

//MSG_ID_PM25_CLOUDVELA_DATA_REPORT,
typedef struct msg_struct_pm25_cloudvela_data_report
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseReport;
	sensor_pm25_data_element_t pm25;
	sensor_zhb_transport_format_ul_t zhbUl;
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
	sensor_zhb_transport_format_dl_t zhbDl;
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
	sensor_zhb_transport_format_ul_t zhbUl;
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
	sensor_zhb_transport_format_dl_t zhbDl;
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
	sensor_zhb_transport_format_ul_t zhbUl;
	UINT32 length;
}msg_struct_temp_cloudvela_ctrl_resp_t;

//MSG_ID_TEMP_CLOUDVELA_DATA_REPORT,
typedef struct msg_struct_temp_cloudvela_data_report
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseReport;
	sensor_temp_data_element_t temp;
	sensor_zhb_transport_format_ul_t zhbUl;
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
	sensor_zhb_transport_format_dl_t zhbDl;
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
	sensor_zhb_transport_format_ul_t zhbUl;
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
	sensor_zhb_transport_format_dl_t zhbDl;
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
	sensor_zhb_transport_format_ul_t zhbUl;
	UINT32 length;
}msg_struct_humid_cloudvela_ctrl_resp_t;

//MSG_ID_HUMID_CLOUDVELA_DATA_REPORT,
typedef struct msg_struct_humid_cloudvela_data_report
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseReport;
	sensor_humid_data_element_t humid;
	sensor_zhb_transport_format_ul_t zhbUl;
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
	sensor_zhb_transport_format_dl_t zhbDl;
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
	sensor_zhb_transport_format_ul_t zhbUl;
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
	sensor_zhb_transport_format_dl_t zhbDl;
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
	sensor_zhb_transport_format_ul_t zhbUl;
	UINT32 length;
}msg_struct_winddir_cloudvela_ctrl_resp_t;

//MSG_ID_WINDDIR_CLOUDVELA_DATA_REPORT,
typedef struct msg_struct_winddir_cloudvela_data_report
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseReport;
	sensor_winddir_data_element_t winddir;
	sensor_zhb_transport_format_ul_t zhbUl;
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
	sensor_zhb_transport_format_dl_t zhbDl;
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
	sensor_zhb_transport_format_ul_t zhbUl;
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
	sensor_zhb_transport_format_dl_t zhbDl;
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
	sensor_zhb_transport_format_ul_t zhbUl;
	UINT32 length;
}msg_struct_windspd_cloudvela_ctrl_resp_t;

//MSG_ID_WINDSPD_CLOUDVELA_DATA_REPORT,
typedef struct msg_struct_windspd_cloudvela_data_report
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseReport;
	sensor_windspd_data_element_t windspd;
	sensor_zhb_transport_format_ul_t zhbUl;
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
	sensor_zhb_transport_format_dl_t zhbDl;
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
	sensor_zhb_transport_format_ul_t zhbUl;
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
	sensor_zhb_transport_format_dl_t zhbDl;
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
	sensor_zhb_transport_format_ul_t zhbUl;
	UINT32 length;
}msg_struct_hsmmp_cloudvela_ctrl_resp_t;

//MSG_ID_HSMMP_CLOUDVELA_DATA_REPORT,
typedef struct msg_struct_hsmmp_cloudvela_data_report
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseReport;
	sensor_hsmmp_link_element_t link;
	sensor_zhb_transport_format_ul_t zhbUl;
	UINT32 length;
}msg_struct_hsmmp_cloudvela_data_report_t;

//MSG_ID_CLOUDVELA_HSMMP_DATA_CONFIRM,
typedef struct msg_struct_cloudvela_hsmmp_data_confirm
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseConfirm;
	UINT32 length;
}msg_struct_cloudvela_hsmmp_data_confirm_t;

//MSG_ID_CLOUDVELA_NOISE_DATA_REQ,
typedef struct  msg_struct_cloudvela_noise_data_req
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseReq;
	UINT8  cmdId;
	UINT8  optId;
	UINT8  cmdIdBackType; //指明是瞬时，还是周期性读数
	UINT32 equId;
	sensor_zhb_transport_format_dl_t zhbDl;
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
	sensor_zhb_transport_format_ul_t zhbUl;
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
	sensor_zhb_transport_format_dl_t zhbDl;
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
	sensor_zhb_transport_format_ul_t zhbUl;
	UINT32 length;
}msg_struct_noise_cloudvela_ctrl_resp_t;

//MSG_ID_NOISE_CLOUDVELA_DATA_REPORT,
typedef struct msg_struct_noise_cloudvela_data_report
{
	msgie_struct_bh_com_head_t comHead;
	UINT8  baseReport;
	sensor_noise_data_element_t noise;
	sensor_zhb_transport_format_ul_t zhbUl;
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
	sensor_zhb_transport_format_dl_t zhbDl;
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
	sensor_zhb_transport_format_ul_t zhbUl;
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
	sensor_zhb_transport_format_dl_t zhbDl;
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
	sensor_zhb_transport_format_ul_t zhbUl;
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
	sensor_zhb_transport_format_ul_t zhbUl;
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
	sensor_zhb_transport_format_dl_t zhbDl;
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
	sensor_zhb_transport_format_ul_t zhbUl;
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
	sensor_zhb_transport_format_ul_t zhbUl;
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





