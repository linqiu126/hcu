/*
 * con3g4g.h
 *
 *  Created on: 2015年11月2日
 *      Author: test
 */

#ifndef L1HWOPT_CON3G4G_H_
#define L1HWOPT_CON3G4G_H_

#include "../l0comvm/vmlayer.h"
#include "../l1hwopt/spsapi.h"
#include "../l1com/l1comdef.h"


//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
//#define FSM_STATE_COMMON  	0x02
enum FSM_STATE_3G4G
{
	FSM_STATE_3G4G_INITED = FSM_STATE_COMMON + 1,
	FSM_STATE_3G4G_RECEIVED,  //所有人机命令在此状态下直接送达各个线程任务模块
	FSM_STATE_3G4G_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

//Global variables
extern HcuFsmStateItem_t HcuFsm3g4g[];

//API
extern OPSTAT fsm_3g4g_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_3g4g_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_3g4g_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT hcu_3g4g_phy_link_setup(void);
extern OPSTAT hcu_3g4g_phy_link_disconnect(void);

//Local API
OPSTAT func_3g4g_int_init(void);

//对外引用的API
extern OPSTAT hcu_3g4g_data_send(CloudDataSendBuf_t *buf);


//Add by MYC
extern OPSTAT hcu_3g_data_send();


//MYC Add for Forlinx A8 Linux USB 3G Module
/* Comamnd Sequence */
/*
 * 3g_test start_um ZET-AC528
 * 3g_test start_ppp ZET-AC528
 *
 * Type Supported:
 *
 * ZET-MF190 (Not Test)
 * HW-E1750 (Not Tested
 * HW-E261 (Tested)
 * ZET-AC582 (Not Tested)
 * ZET-AC2787 (Not Tested)
 * ZET-A356 (Not Tested)
 * FL-AD3812 (Not Tested)
 *
 */

enum USB_UMTS_PPP_CARD_TYPE
{
	USB_UMTS_PPP_CARD_TYPE_ZET_MF190 = 0x00, //Starting point
	USB_UMTS_PPP_CARD_TYPE_HW_E1750,
	USB_UMTS_PPP_CARD_TYPE_HW_E261,
	USB_UMTS_PPP_CARD_TYPE_ZET_AC528,
	USB_UMTS_PPP_CARD_TYPE_ZET_AC2787,
	USB_UMTS_PPP_CARD_TYPE_ZET_A356,
	USB_UMTS_PPP_CARD_TYPE_FL_AD3812,
	USB_UMTS_PPP_CARD_TYPE_MAX_INVALID = 0x10, //Ending point
} UsbUmtsCardType_t; //end of HCU_INTER_TASK_MSG_ID

/*
 * Type define for USB UMTS PPP
 */

#define USB_UMTS_TYPE_CMD_NAME_MAX_LENTH	128
typedef struct UsbUmtsPppControl
{
	UINT16	activeUsbUmtsType;
	char 	usbUmtsTypeName[USB_UMTS_PPP_CARD_TYPE_MAX_INVALID][USB_UMTS_TYPE_CMD_NAME_MAX_LENTH];
	char 	_3g_test_start_ums[USB_UMTS_PPP_CARD_TYPE_MAX_INVALID][USB_UMTS_TYPE_CMD_NAME_MAX_LENTH];
	char 	_3g_test_start_ppp[USB_UMTS_PPP_CARD_TYPE_MAX_INVALID][USB_UMTS_TYPE_CMD_NAME_MAX_LENTH];
} UsbUmtsPppControl_t;


/*
 * Function API prototype for USB UMTS PPP Link
 */
UINT32 UsbUmtsPppInit();
UINT32 UsbUmtsCardInit(char *devPath);
UINT32 UsbUmtsPppdStart();
UINT32 UsbUmtsPppdSwShutdown();
UINT32 UsbUmtsHwReset();
//UINT32 UsbUmtsPppdSystemStartup();
//UINT32 AtCommand(SerialPort_t *pGprsSerialPortConfig, char *AtCmd, char *ReplyStr, int *ReplyCnt);
UINT32 UsbUmtsAtCommand(UINT16 fd, char *AtCmd, char *ReplyStr, int *ReplyCnt);
void SetSystemTime(char *t);

//External APIs
extern UINT32 IsGprsPppdOk();
extern void GpioPortOp(char *op);
extern UINT32 IsNetItfExist(char *itfname);
extern void KillPppdProcess();



#endif /* L1HWOPT_CON3G4G_H_ */
