/*
 * TestHellopWorld.h
 *
 *  Created on: 2016年11月17日
 *      Author: hitpony
 */

#ifndef TESTHELLOPWORLD_H_
#define TESTHELLOPWORLD_H_


#define DLK_TICK			(1000) //1ms  =>400ms, 1H=3600/0.4 = 9000pcs/H. Target>=2000pcs/H
#define DLK_MAIN_BUF 		(200)
#define DLK_LIFE_CYCLE_MAX 	(DLK_MAIN_BUF)
#define DLK_INPUT_PIPE_MAX	(4)  //一定只能等于4个，修改为其他数据，会导致错误
#define DLK_INP_IND_MAX		5  //0-4 多少个物料小型号
#define DLK_OUTPUT_PIPE_MAX	6  //0-3 多少个输出源
#define DLK_TARGET_WO_MAX	(2000) //总共需要采用的包数
#define DLK_TARGET_PRI_LEN  (20) //输出优先级采用了游程长度随机分布，自动增长模型
#define DLK_INPUT_DIS_LEN   2 //输入游程长度分布模型
#define DLK_SEARCH_LEN_MAX  (DLK_MAIN_BUF-2) //算法搜索的最长长度，这是为了简化设置，并防止算法变得病态
#define DLK_INPUT_PKG_MAX	(unsigned int)(DLK_TARGET_WO_MAX*9/2) //2*DLK_TARGET_WO_MAX //入包数量，假设剔除损耗的条件下。未来考虑损耗回收，这部分应该跟DLK_TARGET_WO_MAX相等

//物料标识
typedef struct StrMatInd
{
	unsigned int productCat; //DLK_INPUT_PIPE_MAX=>0-3
	unsigned int productInd; //DLK_INP_IND_MAX
}StrMatInd_t;

//转动的方格
typedef struct StrHopperElement
{
	unsigned int index;
	//unsigned int preInd;
	//unsigned int postInd;
	unsigned int state;
	unsigned int woNbr;  //工单号
	unsigned int outputPort;
	unsigned int lifeRemain;
	StrMatInd_t  mat;
}StrHopperElement_t;
#define DLK_STATE_EMPTY 	0
#define DLK_STATE_OCCPUY 	1
#define DLK_STATE_ALLOC 	2

//全局表
typedef struct StrCtrlContext
{
	//料斗信息
	StrHopperElement_t hop[DLK_MAIN_BUF];  //各个料斗状态信息
	//入料管道
	unsigned int inputPort[DLK_INPUT_PIPE_MAX];   //入料操作口所处于的位置
	double  inputDis[DLK_INPUT_PIPE_MAX][DLK_INP_IND_MAX];
	double  inputDisRange[DLK_INPUT_PIPE_MAX][DLK_INP_IND_MAX];
	unsigned int inputFlCur[DLK_INPUT_PIPE_MAX];   //输入物料当前处于的物料进料号
	unsigned int inputCtrlCnt[DLK_INPUT_PIPE_MAX];  //用这个来控制各个物料具有相同的数量
	//出料管道
	unsigned int outputPort[DLK_OUTPUT_PIPE_MAX];   //出料操作口所处于的位置
	unsigned int rubbishPort;  //垃圾桶挂载
	unsigned int woRecGlobalIndex[DLK_OUTPUT_PIPE_MAX];
	unsigned int woExeGlobalIndex;  //工作的WO指示
	//统计信息
	unsigned int staMatInc;
	unsigned int staMatTtt;
	unsigned int staMatTgv;
	unsigned int staCombPkgTimes;
	//unsigned int woOutputLocalIndex[DLK_OUTPUT_PIPE_MAX];  //工单出料执行，本地编号
	//unsigned int woAlgoScanLocalIndex[DLK_OUTPUT_PIPE_MAX];  //工单算法搜索执行，本地编号
}StrCtrlContext_t;



//出包
typedef struct StrOutputPkg
{
	StrMatInd_t matEle[DLK_INPUT_PIPE_MAX];
	unsigned int pri; //优先级
}StrOutputPkg_t;

//输出工单包-动态生成
typedef struct StrWorkOrder
{
	StrOutputPkg_t wo[DLK_TARGET_WO_MAX];  //工单信息
	unsigned int  inputPkgNbrSta[DLK_INPUT_PIPE_MAX][DLK_INP_IND_MAX]; //这个数据将成为输入物料的参照数据，也是计算结果的统计数据，不用于控制进度。这里是用来统计各种小型号有多少个，用户概率分布控制。
	unsigned int  priMax;  //最多优先级数量
	unsigned int  priLenTable[DLK_TARGET_WO_MAX];  //每一个优先级的游程长度信息
	unsigned int  oPkgEleProgCtrl[DLK_OUTPUT_PIPE_MAX]; //一个完整的包，包含有4个物料，这里指进展到第几步
	unsigned int  woOutHopRemSteps[DLK_OUTPUT_PIPE_MAX][DLK_INPUT_PIPE_MAX]; //这是用来指示还差多少步，物料将到达出料箱子。因为不容易互斥，只是一个指示器，而不用来具体标识HOP物料的出料
}StrWorkOrder_t;



//输入数据包-动态生成
typedef struct StrIntputFlow
{
	unsigned int  fl[DLK_INPUT_PIPE_MAX][DLK_INPUT_PKG_MAX];  //每一个输入物料的具体型号，第一个参量是固定的，后面的第二维表示的是物料序号，从0-3999，相比目标大一倍。取值就是[0-4]，物料小型号范围
	unsigned int  chgMax[DLK_INPUT_PIPE_MAX];  //每一个输入管道中，不同物料变更次数
	unsigned int  chgLenTable[DLK_INPUT_PIPE_MAX][DLK_INPUT_PKG_MAX];  //每一个输入管道中，不同物料的游程长度信息
}StrIntputFlow_t;







//T除信息BUFFER存储 =? TBD











void global_init_coef(void);
void global_move_input_output_port(void);
void global_mat_outflow(void);
void global_mat_inflow(void);
void global_algo_search(void);
void global_state_update(void);






















/*
 * can.h - routine for can driver test.
 *
 * Copyright (c) 2011	reille
 * All rights reserved.
 *
 * Date : 2011.03.01
 * Ver  : Ver1.0
 *
 */

#ifndef CAN_H
#define CAN_H

#ifndef WIN32
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <libgen.h>

#include <net/if.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/socket.h>
#else //def WIN32
#include <winsock.h>
#endif //ndef WIN32


//typedef unsigned int	__u32;
//typedef unsigned char	__u8;

#ifdef WIN32
#define	ioctl(x,y,z) 1	//nothing
#define	__attribute__(x)	//nothing
#define sa_family_t	unsigned short
# define SIOCGIFINDEX 6
# define IFNAMSIZ 6
struct   ifreq
{
	char         ifr_name[IFNAMSIZ];     /*   interface   name,   e.g.,   "le0 "   */
	union   {
		struct     sockaddr   ifru_addr;
		struct     sockaddr   ifru_dstaddr;
		struct     sockaddr   ifru_broadaddr;
		short       ifru_flags;
		int           ifru_metric;
		//caddr_t   ifru_data;
	}   ifr_ifru;
	short       ifr_ifindex;
};

#endif // WIN32

//////////////////////////////////////////////////////////////////////////
/* Follow define from "linux/can/error.h" */

#define CAN_ERR_DLC 8 /* dlc for error frames */

/* error class (mask) in can_id */
#define CAN_ERR_TX_TIMEOUT   0x00000001U /* TX timeout (by netdevice driver) */
#define CAN_ERR_LOSTARB      0x00000002U /* lost arbitration    / data[0]    */
#define CAN_ERR_CRTL         0x00000004U /* controller problems / data[1]    */
#define CAN_ERR_PROT         0x00000008U /* protocol violations / data[2..3] */
#define CAN_ERR_TRX          0x00000010U /* transceiver status  / data[4]    */
#define CAN_ERR_ACK          0x00000020U /* received no ACK on transmission */
#define CAN_ERR_BUSOFF       0x00000040U /* bus off */
#define CAN_ERR_BUSERROR     0x00000080U /* bus error (may flood!) */
#define CAN_ERR_RESTARTED    0x00000100U /* controller restarted */

/* arbitration lost in bit ... / data[0] */
#define CAN_ERR_LOSTARB_UNSPEC   0x00 /* unspecified */
/* else bit number in bitstream */

/* error status of CAN-controller / data[1] */
#define CAN_ERR_CRTL_UNSPEC      0x00 /* unspecified */
#define CAN_ERR_CRTL_RX_OVERFLOW 0x01 /* RX buffer overflow */
#define CAN_ERR_CRTL_TX_OVERFLOW 0x02 /* TX buffer overflow */
#define CAN_ERR_CRTL_RX_WARNING  0x04 /* reached warning level for RX errors */
#define CAN_ERR_CRTL_TX_WARNING  0x08 /* reached warning level for TX errors */
#define CAN_ERR_CRTL_RX_PASSIVE  0x10 /* reached error passive status RX */
#define CAN_ERR_CRTL_TX_PASSIVE  0x20 /* reached error passive status TX */
/* (at least one error counter exceeds */
/* the protocol-defined level of 127)  */

/* error in CAN protocol (type) / data[2] */
#define CAN_ERR_PROT_UNSPEC      0x00 /* unspecified */
#define CAN_ERR_PROT_BIT         0x01 /* single bit error */
#define CAN_ERR_PROT_FORM        0x02 /* frame format error */
#define CAN_ERR_PROT_STUFF       0x04 /* bit stuffing error */
#define CAN_ERR_PROT_BIT0        0x08 /* unable to send dominant bit */
#define CAN_ERR_PROT_BIT1        0x10 /* unable to send recessive bit */
#define CAN_ERR_PROT_OVERLOAD    0x20 /* bus overload */
#define CAN_ERR_PROT_ACTIVE      0x40 /* active error announcement */
#define CAN_ERR_PROT_TX          0x80 /* error occured on transmission */

/* error in CAN protocol (location) / data[3] */
#define CAN_ERR_PROT_LOC_UNSPEC  0x00 /* unspecified */
#define CAN_ERR_PROT_LOC_SOF     0x03 /* start of frame */
#define CAN_ERR_PROT_LOC_ID28_21 0x02 /* ID bits 28 - 21 (SFF: 10 - 3) */
#define CAN_ERR_PROT_LOC_ID20_18 0x06 /* ID bits 20 - 18 (SFF: 2 - 0 )*/
#define CAN_ERR_PROT_LOC_SRTR    0x04 /* substitute RTR (SFF: RTR) */
#define CAN_ERR_PROT_LOC_IDE     0x05 /* identifier extension */
#define CAN_ERR_PROT_LOC_ID17_13 0x07 /* ID bits 17-13 */
#define CAN_ERR_PROT_LOC_ID12_05 0x0F /* ID bits 12-5 */
#define CAN_ERR_PROT_LOC_ID04_00 0x0E /* ID bits 4-0 */
#define CAN_ERR_PROT_LOC_RTR     0x0C /* RTR */
#define CAN_ERR_PROT_LOC_RES1    0x0D /* reserved bit 1 */
#define CAN_ERR_PROT_LOC_RES0    0x09 /* reserved bit 0 */
#define CAN_ERR_PROT_LOC_DLC     0x0B /* data length code */
#define CAN_ERR_PROT_LOC_DATA    0x0A /* data section */
#define CAN_ERR_PROT_LOC_CRC_SEQ 0x08 /* CRC sequence */
#define CAN_ERR_PROT_LOC_CRC_DEL 0x18 /* CRC delimiter */
#define CAN_ERR_PROT_LOC_ACK     0x19 /* ACK slot */
#define CAN_ERR_PROT_LOC_ACK_DEL 0x1B /* ACK delimiter */
#define CAN_ERR_PROT_LOC_EOF     0x1A /* end of frame */
#define CAN_ERR_PROT_LOC_INTERM  0x12 /* intermission */

/* error status of CAN-transceiver / data[4] */
/*                                             CANH CANL */
#define CAN_ERR_TRX_UNSPEC             0x00 /* 0000 0000 */
#define CAN_ERR_TRX_CANH_NO_WIRE       0x04 /* 0000 0100 */
#define CAN_ERR_TRX_CANH_SHORT_TO_BAT  0x05 /* 0000 0101 */
#define CAN_ERR_TRX_CANH_SHORT_TO_VCC  0x06 /* 0000 0110 */
#define CAN_ERR_TRX_CANH_SHORT_TO_GND  0x07 /* 0000 0111 */
#define CAN_ERR_TRX_CANL_NO_WIRE       0x40 /* 0100 0000 */
#define CAN_ERR_TRX_CANL_SHORT_TO_BAT  0x50 /* 0101 0000 */
#define CAN_ERR_TRX_CANL_SHORT_TO_VCC  0x60 /* 0110 0000 */
#define CAN_ERR_TRX_CANL_SHORT_TO_GND  0x70 /* 0111 0000 */
#define CAN_ERR_TRX_CANL_SHORT_TO_CANH 0x80 /* 1000 0000 */

/* controller specific additional information / data[5..7] */

//////////////////////////////////////////////////////////////////////////



/* Follow define for controller area network (CAN) from linux kernel, can-utils */

/* Controller Area Network */
#ifndef PF_CAN
#define PF_CAN 29
#endif

#ifndef AF_CAN
#define AF_CAN PF_CAN
#endif

/* particular protocols of the protocol family PF_CAN */
#define CAN_RAW			1				/* RAW sockets */
#define CAN_BCM			2				/* Broadcast Manager */
#define CAN_TP16		3				/* VAG Transport Protocol v1.6 */
#define CAN_TP20		4				/* VAG Transport Protocol v2.0 */
#define CAN_MCNET		5				/* Bosch MCNet */
#define CAN_ISOTP		6				/* ISO 15765-2 Transport Protocol */
#define CAN_NPROTO		7

#define SOL_CAN_BASE 100

/* special address description flags for the CAN_ID */
#define CAN_EFF_FLAG	0x80000000U		/* EFF/SFF is set in the MSB */
#define CAN_RTR_FLAG	0x40000000U		/* remote transmission request */
#define CAN_ERR_FLAG	0x20000000U		/* error frame */

/* valid bits in CAN ID for frame formats */
#define CAN_SFF_MASK	0x000007FFU		/* standard frame format (SFF) */
#define CAN_EFF_MASK	0x1FFFFFFFU		/* extended frame format (EFF) */
#define CAN_ERR_MASK	0x1FFFFFFFU		/* omit EFF, RTR, ERR flags */

/*
 * Controller Area Network Identifier structure
 *
 * bit 0-28	: CAN identifier (11/29 bit)
 * bit 29	: error frame flag (0 = data frame, 1 = error frame)
 * bit 30	: remote transmission request flag (1 = rtr frame)
 * bit 31	: frame format flag (0 = standard 11 bit, 1 = extended 29 bit)
 */
typedef __u32 canid_t;

/*
 * Controller Area Network Error Frame Mask structure
 *
 * bit 0-28	: error class mask (see include/socketcan/can/error.h)
 * bit 29-31	: set to zero
 */
typedef __u32 can_err_mask_t;

/**
 * struct can_frame - basic CAN frame structure
 * @can_id:  the CAN ID of the frame and CAN_*_FLAG flags, see above.
 * @can_dlc: the data length field of the CAN frame
 * @data:    the CAN frame payload.
 */
typedef  struct can_frame {
	canid_t can_id;  /* 32 bit CAN_ID + EFF/RTR/ERR flags */
	__u8    can_dlc; /* data length code: 0 .. 8 */
#ifdef WIN32
	__u8    can_na[3];//字节对齐问题
#endif // WIN32
	__u8    data[8] __attribute__((aligned(8)));
}S_CanFrame;

/**
 * struct sockaddr_can - the sockaddr structure for CAN sockets
 * @can_family:  address family number AF_CAN.
 * @can_ifindex: CAN network interface index.
 * @can_addr:    protocol specific address information
 */
struct sockaddr_can {
	sa_family_t can_family;
	int         can_ifindex;
	union {
		/* transport protocol class address information (e.g. ISOTP) */
		struct { canid_t rx_id, tx_id; } tp;

		/* reserved for future CAN protocols address information */
	} can_addr;
};

/**
 * struct can_filter - CAN ID based filter in can_register().
 * @can_id:   relevant bits of CAN ID which are not masked out.
 * @can_mask: CAN mask (see description)
 *
 * Description:
 * A filter matches, when
 *
 *          <received_can_id> & mask == can_id & mask
 *
 * The filter can be inverted (CAN_INV_FILTER bit set in can_id) or it can
 * filter for error frames (CAN_ERR_FLAG bit set in mask).
 */
struct can_filter {
	canid_t can_id;
	canid_t can_mask;
};

#define CAN_INV_FILTER 0x20000000U /* to be set in can_filter.can_id */

#endif /* CAN_H */


#endif /* TESTHELLOPWORLD_H_ */
