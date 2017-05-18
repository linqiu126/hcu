/*
 * l2usbcan.h
 *
 *  Created on: May 15, 2017
 *      Author: MA Yuchu
 */

#ifndef L2USBCAN_H_
#define L2USBCAN_H_

#include "../l0comvm/comtype.h"

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


//函数调用返回状态值
#define	STATUS_OK					1
#define STATUS_ERR					0

#define USHORT unsigned short int
#define BYTE unsigned char
//#define CHAR char
#define UCHAR unsigned char
//#define UINT unsigned int
#define DWORD unsigned int
#define PVOID void*
#define ULONG unsigned int
#define INT int
//#define UINT32 UINT
#define LPVOID void*
#define BOOL BYTE
#define TRUE 1
#define FALSE 0


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
typedef struct
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
}USB_CAN_HandleTypeDef;

/*
 * @brief  L2 Frame over CAN Interface between CAN and CANITFLOE
 */
#define BFSC_CAN_MAX_RX_BUF_SIZE 256
#define WMC_NODE_NUMBER 17

typedef struct
{
	UINT32 can_id_tx_wmc_bitmap;
	UINT32 can_id_rx_wmc_id;
	UINT32 can_l2frame_len;
	UINT8  can_l2frame[BFSC_CAN_MAX_RX_BUF_SIZE];
}can_l2frame_itf_t;

#define 	CAN_L2_FRAME_ITF_LEN	(sizeof(can_l2frame_itf_t))


/* Can ID for communication between AWS and WMC */
/* 1: AWS to WMC: 1 to n, n = 0 ... 15       */
/* Node ID bitmap: CAN ID = 0x0010XXXX    */
/* CAN ID          direction => ^~~~~ <= 1111 1111 1111 1111, */
#define AWS_TO_WMC_CAN_ID_PREFIX		(0x00100000U)
//#define AWS_TO_WMC_CAN_ID_SUFFIX		(mwc_id_bitmap)

/* 2: AWS to WMC-S: 1 to 1  */
/* CAN ID: 0x00100000 */
#define AWS_TO_WMCS_CAN_ID				(0x00110000U)

/* 3: WMC to AWS: */
/* CAN ID: 0X00020000 (Node ID 0) to 0X0003000F (Node ID 15) */
#define WMC_TO_AWS_CAN_ID_PREFIX		(0x00030000U)
//#define WMC_TO_AWS_CAN_ID_SUFFIX		(mwc_id)

/* 4: WMC-S to AWS: */
/* CAN ID: 0X00020010 (Node ID 0) */
#define WMCS_TO_AWS_CAN_ID				(0x00030010U)


/* CAN Interface APIs */
UINT32 usb_can_deinit(USB_CAN_HandleTypeDef *husbcan);
UINT32 usb_can_init(USB_CAN_HandleTypeDef *husbcan, UINT32 can_dev_type, UINT32 can_dev_idx, UINT32 can_channel_id, UINT32 band_rate_kbps, pthread_t can_forwarding_thread_id, UINT32 can_l2_forwarding_mode);
uint32_t bsp_can_l2_frame_transmit(USB_CAN_HandleTypeDef* CanHandle, uint8_t *buffer, uint32_t length, UINT32 wmc_id_bitmap);

/* API Usage */
//ret = usb_can_init(&(gTaskCanitfleoContext.can1), CAN_DEVICE_TYPE_PCI9820I, \
//		CAN_DEVIDE_IDX_CARD1, CAN_DEVIDE_CHANNEL_CAN0, \
//		CAN_BANDRATE_500KBPS, 0, CAN_L2_FRAME_FORWARD_YES);
//
//HcuDebugPrint("CANITFLEO: usb_can_init() called, ret = %d\r\n", ret);

//OPSTAT fsm_canitfleo_can_l2frame_receive(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
//{
//	int ret=0;
//	can_l2frame_itf_t *p = (can_l2frame_itf_t *)param_ptr;
//
//	HcuDebugPrint("CANITFLEO: Received CAN L2 FRAME: [0x%02X, 0x%02X, 0x%02X, 0x%02X], Len = [%d]\r\n", p[0], p[1], p[2], p[3], param_len);
//
//	bsp_can_l2_frame_transmit(&(gTaskCanitfleoContext.can1), p->can_l2frame, p->can_l2frame_len, 0xFFFF);
//
//}

#endif /* L2USBCAN_H_ */
