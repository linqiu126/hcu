/*
 * canitfleo.h
 *
 *  Created on: 2016年5月14日
 *      Author: hitpony
 */

#ifndef L2FRAME_CANITFLEO_H_
#define L2FRAME_CANITFLEO_H_

#include "../l0comvm/vmlayer.h"
#include "../l1com/l1comdef.h"
#include "l2usbcan.h"   		//Added by MYC 2017/05/15
#include "../l2codec/huitp.h"	//Added by MYC 2017/05/15

//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
//#define FSM_STATE_COMMON  	0x02
enum FSM_STATE_CANITFLEO
{
	FSM_STATE_CANITFLEO_INITED = FSM_STATE_COMMON + 1,
	FSM_STATE_CANITFLEO_ACTIVED,
	FSM_STATE_CANITFLEO_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

//Global variables
extern HcuFsmStateItem_t HcuFsmCanitfleo[];

/* ************ CAN ************** */
#define MAXSOCK 16    /* max. number of CAN interfaces given on the cmdline */
#define MAXIFNAMES 30 /* size of receive name index to omit ioctls */
#define MAXCOL 6      /* number of different colors for colorized output */
#define ANYDEV "any"  /* name of interface to receive from any CAN interface */
#define ANL "\r\n"    /* newline in ASC mode */
#define SILENT_INI 42 /* detect user setting on commandline */
#define SILENT_OFF 0  /* no silent mode */
#define SILENT_ANI 1  /* silent mode with animation */
#define SILENT_ON  2  /* silent mode (completely silent) */
#define MAXANI 4
#define MAX_CANFRAME      "12345678#01.23.45.67.89.AB.CD.EF"
/* ************ CAN ************** */

typedef struct gTaskCanitfleoContext
{
	UINT32 sensorIdRoundBing;
	UINT32 can_socket_id;
	//Added by MYC 2017/05/15
	USB_CAN_HandleTypeDef can1;
}gTaskCanitfleoContext_t;

//API
extern OPSTAT fsm_canitfleo_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_canitfleo_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_canitfleo_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_canitfleo_timeout(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_canitfleo_l3bfsc_error_inq_cmd_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_canitfleo_l3bfsc_ws_comb_out(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_canitfleo_l3bfsc_ws_give_up(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_canitfleo_l3bfsc_ws_init_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_canitfleo_l3bfsc_ws_read_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_canitfleo_l3bfsc_general_cmd_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_canitfleo_can_l2frame_receive(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
//Local API
OPSTAT func_canitfleo_int_init(void);
OPSTAT func_canitfleo_frame_encode(UINT8 prefixcmdid, UINT8 optid, UINT8 optpar, UINT32 modbusval, strHcuCanitfleoCmdFrame_t *pframe);
OPSTAT func_canitfleo_frame_decode(strHcuCanitfleoCmdFrame_t *pframe, UINT8 prefixcmdid, UINT8 optid, UINT8 optpar, UINT32 modbusval);
OPSTAT func_canitfleo_can_receive(int socket, canid_t *canid, char *canframe_hex, char *canid_canframe_char);
OPSTAT func_canitfleo_can_send(int socket, char *canid_canframe);
OPSTAT func_canitfleo_can_init(char *canitfname, int *sock);
OPSTAT func_canitfleo_working_scan_process(void);
OPSTAT func_canitfleo_bfsc_simulation_data_process(void);

void BigSmallEndianMapping(uint8_t *In, uint8_t *Out, uint32_t len);
OPSTAT canitfleo_can_l2frame_receive_process(uint8_t *p_l2_frame, uint32_t l2_frame_len);
void canitfleo_can_l2frame_receive_process_bfsc_start_ind(uint8_t *ptr);
void canitfleo_can_l2frame_receive_process_bfsc_new_ws_event(uint8_t *ptr);

// WILL BE REPLACED BY HUITP, ADDED 2017/05/15 */



//高级定义，简化程序的可读性
#define HCU_ERROR_PRINT_CANITFLEO(...)	do{zHcuSysStaPm.taskRunErrCnt[TASK_ID_CANITFLEO]++;  HcuErrorPrint(__VA_ARGS__);  return FAILURE;}while(0)

#endif /* L2FRAME_CANITFLEO_H_ */
