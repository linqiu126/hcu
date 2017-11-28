/*
 * l2usbcan.h
 *
 *  Created on: May 15, 2017
 *      Author: MA Yuchu
 */

#ifndef L2USBCAN_H_
#define L2USBCAN_H_

#include "../l0comvm/comtype.h"
#include "../l0comvm/vmlayer.h"
#include "../l2codec/huitp.h"   //Added by MYC 2017/05/30

#ifdef WIN32 // for windows
	#include <windows.h>
	#include <process.h>
	#include <stdio.h>
	#include <time.h>
	#include "controlcan.h"
	#pragma comment(lib, "controlcan.lib")
	#define msleep(ms)  Sleep(ms)
	typedef HANDLE pthread_t;
#else // for linux
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <unistd.h>
	#include <sys/types.h>
	#include <sys/stat.h>
	#include <fcntl.h>
	#include <pthread.h>
	#include "../l0service/trace.h"
	#include "../l0comvm/comtype.h"
	#include "../l1com/l1comdef.h"
	#include "l2packet.h"
	#define msleep(ms)  usleep((ms)*1000)
	#define min(a,b)  (((a) < (b)) ? (a) : (b))
#endif


//接口卡类型定义
#define VCI_PCI5121		1
#define VCI_PCI9810		2
#define VCI_USBCAN1		3
#define VCI_USBCAN2		4
#define VCI_PCI9820		5
#define VCI_CAN232		6
#define VCI_PCI5110		7
#define VCI_CANLite		8
#define VCI_ISA9620		9
#define VCI_ISA5420		10

//CAN错误码
#define	ERR_CAN_OVERFLOW			0x0001	//CAN控制器内部FIFO溢出
#define	ERR_CAN_ERRALARM			0x0002	//CAN控制器错误报警
#define	ERR_CAN_PASSIVE				0x0004	//CAN控制器消极错误
#define	ERR_CAN_LOSE				0x0008	//CAN控制器仲裁丢失
#define	ERR_CAN_BUSERR				0x0010	//CAN控制器总线错误

//通用错误码
#define	ERR_DEVICEOPENED			0x0100	//设备已经打开
#define	ERR_DEVICEOPEN				0x0200	//打开设备错误
#define	ERR_DEVICENOTOPEN			0x0400	//设备没有打开
#define	ERR_BUFFEROVERFLOW			0x0800	//缓冲区溢出
#define	ERR_DEVICENOTEXIST			0x1000	//此设备不存在
#define	ERR_LOADKERNELDLL			0x2000	//装载动态库失败
#define ERR_CMDFAILED				0x4000	//执行命令失败错误码
#define	ERR_BUFFERCREATE			0x8000	//内存不足

#define MAX_CHANNELS  4
#define CHECK_POINT  200
#define RX_WAIT_TIME  100
#define RX_BUFF_SIZE  1000

//函数调用返回状态值
#ifndef STATUS_OK
	#define	STATUS_OK					1
#endif
#ifndef STATUS_ERR
	#define	STATUS_ERR					0
#endif
#ifndef USHORT
	#define USHORT unsigned short int
#endif
#ifndef BYTE
	#define BYTE unsigned char
#endif
#ifndef UCHAR
	#define UCHAR unsigned char
#endif
//#define UINT unsigned int
#ifndef DWORD
	#define DWORD unsigned int
#endif
#ifndef PVOID
	#define PVOID void*
#endif
#ifndef INT
	#define INT int
#endif
#ifndef LPVOID
	#define LPVOID void*
#endif
#ifndef BOOL
	#define BOOL BYTE
#endif

//1.ZLGCAN系列接口卡信息的数据类型。
typedef  struct  _VCI_BOARD_INFO{
		USHORT	hw_Version;
		USHORT	fw_Version;
		USHORT	dr_Version;
		USHORT	in_Version;
		USHORT	irq_Num;
		BYTE	can_Num;
		char	str_Serial_Num[20];
		char	str_hw_Type[40];
		USHORT	Reserved[4];
} VCI_BOARD_INFO,*PVCI_BOARD_INFO;

//2.定义CAN信息帧的数据类型。
typedef  struct  _VCI_CAN_OBJ{
	unsigned int	ID;
	unsigned int	TimeStamp;
	BYTE	TimeFlag;
	BYTE	SendType;
	BYTE	RemoteFlag;//是否是远程帧
	BYTE	ExternFlag;//是否是扩展帧
	BYTE	DataLen;
	BYTE	Data[8];
	BYTE	Reserved[3];
}VCI_CAN_OBJ,*PVCI_CAN_OBJ;

//3.定义CAN控制器状态的数据类型。
typedef struct _VCI_CAN_STATUS{
	UCHAR	ErrInterrupt;
	UCHAR	regMode;
	UCHAR	regStatus;
	UCHAR	regALCapture;
	UCHAR	regECCapture;
	UCHAR	regEWLimit;
	UCHAR	regRECounter;
	UCHAR	regTECounter;
	DWORD	Reserved;
}VCI_CAN_STATUS,*PVCI_CAN_STATUS;

//4.定义错误信息的数据类型。
typedef struct _ERR_INFO{
		unsigned int	ErrCode;
		BYTE	Passive_ErrData[3];
		BYTE	ArLost_ErrData;
} VCI_ERR_INFO,*PVCI_ERR_INFO;

//5.定义初始化CAN的数据类型
typedef struct _INIT_CONFIG{
	DWORD	AccCode;
	DWORD	AccMask;
	DWORD	Reserved;
	UCHAR	Filter;
	UCHAR	Timing0;
	UCHAR	Timing1;
	UCHAR	Mode;
}VCI_INIT_CONFIG,*PVCI_INIT_CONFIG;

DWORD VCI_OpenDevice(DWORD DeviceType,DWORD DeviceInd,DWORD Reserved);
DWORD VCI_CloseDevice(DWORD DeviceType,DWORD DeviceInd);
DWORD VCI_InitCAN(DWORD DeviceType, DWORD DeviceInd, DWORD CANInd, PVCI_INIT_CONFIG pInitConfig);

DWORD VCI_ReadBoardInfo(DWORD DeviceType,DWORD DeviceInd,PVCI_BOARD_INFO pInfo);
DWORD VCI_ReadErrInfo(DWORD DeviceType,DWORD DeviceInd,DWORD CANInd,PVCI_ERR_INFO pErrInfo);
DWORD VCI_ReadCANStatus(DWORD DeviceType,DWORD DeviceInd,DWORD CANInd,PVCI_CAN_STATUS pCANStatus);

DWORD VCI_GetReference(DWORD DeviceType,DWORD DeviceInd,DWORD CANInd,DWORD RefType,PVOID pData);
DWORD VCI_SetReference(DWORD DeviceType,DWORD DeviceInd,DWORD CANInd,DWORD RefType,PVOID pData);

ULONG VCI_GetReceiveNum(DWORD DeviceType,DWORD DeviceInd,DWORD CANInd);
DWORD VCI_ClearBuffer(DWORD DeviceType,DWORD DeviceInd,DWORD CANInd);

DWORD VCI_StartCAN(DWORD DeviceType,DWORD DeviceInd,DWORD CANInd);
DWORD VCI_ResetCAN(DWORD DeviceType,DWORD DeviceInd,DWORD CANInd);

ULONG VCI_Transmit(DWORD DeviceType,DWORD DeviceInd,DWORD CANInd,PVCI_CAN_OBJ pSend,unsigned int Len);
ULONG VCI_Receive(DWORD DeviceType,DWORD DeviceInd,DWORD CANInd,PVCI_CAN_OBJ pReceive,unsigned int Len,INT WaitTime);

unsigned func_zlg_usbcan_s2n(const char *s);
void func_zlg_usbcan_generate_frame(VCI_CAN_OBJ *can);
int func_zlg_usbcan_verify_frame(VCI_CAN_OBJ *can);
void * func_zlg_usbcan_rx_thread(void *data);
int func_zlg_usbcan_test_main1();
int func_zlg_usbcan_test_main2(int argc, char* argv[]);



/* ============== */
/* 5-pci9820, 14-pci9840, 16-pci9820i, ....\n */

/* CAN Device Type */
#define 	CAN_DEVICE_TYPE_PCI9820		(5)
#define 	CAN_DEVICE_TYPE_PCI9840		(14)
#define 	CAN_DEVICE_TYPE_PCI9820I	(16)

#define 	CAN_DEVICE_TYPE_0			(0)
#define 	CAN_DEVICE_TYPE_1			(1)
#define 	CAN_DEVICE_TYPE_2			(2)
#define 	CAN_DEVICE_TYPE_3			(3)
#define 	CAN_DEVICE_TYPE_4			(4)
#define 	CAN_DEVICE_TYPE_5			(5)
#define 	CAN_DEVICE_TYPE_6			(6)
#define 	CAN_DEVICE_TYPE_7			(7)
#define 	CAN_DEVICE_TYPE_8			(8)
#define 	CAN_DEVICE_TYPE_9			(9)
#define 	CAN_DEVICE_TYPE_10			(10)
#define 	CAN_DEVICE_TYPE_11			(11)
#define 	CAN_DEVICE_TYPE_12			(12)
#define 	CAN_DEVICE_TYPE_13			(13)
#define 	CAN_DEVICE_TYPE_14			(14)
#define 	CAN_DEVICE_TYPE_15			(15)
#define 	CAN_DEVICE_TYPE_16			(16)

/* CARD ID */
/* NOTE: For AdvanTech, ONLY CARD1 is working */
#define 	CAN_DEVIDE_IDX_CARD0		(0)
#define 	CAN_DEVIDE_IDX_CARD1		(1)

/* CHANNEL ID */
#define 	CAN_DEVIDE_CHANNEL_CAN0		(1)  	/* 01 */
#define 	CAN_DEVIDE_CHANNEL_CAN1		(2)		/* 10 */

/* CAN BAUD RATE */
#define 	CAN_BANDRATE_5KBPS			(5)
#define 	CAN_BANDRATE_10KBPS			(10)
#define 	CAN_BANDRATE_20KBPS			(20)
#define 	CAN_BANDRATE_40KBPS			(40)
#define 	CAN_BANDRATE_50KBPS			(50)
#define 	CAN_BANDRATE_80KBPS			(80)
#define 	CAN_BANDRATE_100KBPS		(100)
#define 	CAN_BANDRATE_125KBPS		(125)
#define 	CAN_BANDRATE_200KBPS		(200)
#define 	CAN_BANDRATE_250KBPS		(250)
#define 	CAN_BANDRATE_400KBPS		(400)
#define 	CAN_BANDRATE_500KBPS		(500)
#define 	CAN_BANDRATE_666KBPS		(666)
#define 	CAN_BANDRATE_800KBPS		(800)
#define 	CAN_BANDRATE_1000KBPS		(1000)

#define 	CAN_STATUS_NULL				(0)
#define 	CAN_STATUS_INITIALIZED		(1)

#define 	CAN_L2_FRAME_FORWARD_YES	(1)
#define 	CAN_L2_FRAME_FORWARD_NO		(0)

#define MAX_CHANNELS  4
#define CHECK_POINT  200
#define RX_WAIT_TIME  100
#define RX_BUFF_SIZE  1000

/*
 * @brief  CAN handle Structure definition
 */
typedef struct HcuUsbCanHandleTypeDef
{
	UINT32 can_dev_type;
	UINT32 can_dev_idx;
	UINT32 can_channel_id;
	UINT32 band_rate_kbps;
	pthread_t can_receiving_thread_id;
	pthread_t can_forwarding_thread_id;
	UINT32 can_l2_forwarding_mode;
	UINT32 can_status;
	VCI_INIT_CONFIG can_config;
	VCI_CAN_OBJ can_tx_data;
	VCI_CAN_OBJ can_rx_data[RX_BUFF_SIZE]; // buffer
}HcuUsbCanHandleTypeDef_t;

/*
 * @brief  L2 Frame over CAN Interface between CAN and CANITFLOE
 */
#if (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFSC_CBU_ID)
	#define WMC_NODE_NUMBER HCU_SYSCFG_BFSC_SNR_WS_NBR_MAX
#else
	#define WMC_NODE_NUMBER 0
#endif
typedef struct HcuCanL2framItfDef
{
	UINT32 can_id_tx_wmc_bitmap;
	UINT32 can_id_rx_wmc_id;
	UINT32 can_l2frame_len;
	UINT8  can_l2frame[HCU_SYSMSG_BFSC_USBCAN_MAX_RX_BUF_SIZE+1];  //为了防止尾巴重叠
}HcuCanL2framItfDef_t;
#define 	CAN_L2_FRAME_ITF_LEN	(sizeof(HcuCanL2framItfDef_t))

//Local APIs
UINT32 func_usbcan_WmcCanIdMapToWmcId(UINT32 wmc_can_id);
UINT32 func_usbcan_bandrate_to_timing_mapping(UINT32 band_rate_kbps, UINT8 *timing0, UINT8 *timing1);
void *func_usbcan_rx_thread(void *data);
UINT32 hcu_bsp_usbcan_init(HcuUsbCanHandleTypeDef_t *husbcan, UINT32 can_dev_type, UINT32 can_dev_idx, UINT32 can_channel_id, UINT32 band_rate_kbps, pthread_t can_forwarding_thread_id, UINT32 can_l2_forwarding_mode);
UINT32 func_usbcan_transmit(HcuUsbCanHandleTypeDef_t *husbcan, UINT8 *ptr_data, UINT32 data_len, UINT32 can_id, UINT32 extern_flag);
void func_usbcan_RxCpltCallback(HcuUsbCanHandleTypeDef_t* CanHandle, VCI_CAN_OBJ *Can, UINT32 wmc_id);
void func_usbcan_loopback_callback(HCU_HUITP_L2FRAME_Desc_t *pdesc);
int func_bsp_usbcan_start_rx(HcuUsbCanHandleTypeDef_t* CanHandle, void (*app_rx_callback)(), uint8_t *pRxBuffPtr, uint32_t wmc_id, uint16_t rxBufferSize, void *user_data);
void func_bfsc_usbcan_test(void);

//Global APIs
UINT32   hcu_bsp_usbcan_deinit(HcuUsbCanHandleTypeDef_t *husbcan);
UINT32   hcu_bsp_usbcan_init(HcuUsbCanHandleTypeDef_t *husbcan, UINT32 can_dev_type, UINT32 can_dev_idx, UINT32 can_channel_id, UINT32 band_rate_kbps, pthread_t can_forwarding_thread_id, UINT32 can_l2_forwarding_mode);
uint32_t hcu_bsp_usbcan_l2frame_transmit(HcuUsbCanHandleTypeDef_t* CanHandle, uint8_t *buffer, uint32_t length, UINT32 wmc_id_bitmap);


#endif /* L2USBCAN_H_ */
