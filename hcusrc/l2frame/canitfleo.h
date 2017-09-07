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
	HcuUsbCanHandleTypeDef_t can1;  //Added by MYC 2017/05/15
}gTaskCanitfleoContext_t;

//API
extern OPSTAT fsm_canitfleo_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_canitfleo_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_canitfleo_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_canitfleo_timeout(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_canitfleo_l3bfsc_sys_cfg_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_canitfleo_can_sys_start_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_canitfleo_can_sys_stop_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_canitfleo_can_test_cmd_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_canitfleo_l3bfsc_ws_comb_out(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_canitfleo_l3bfsc_ws_give_up(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_canitfleo_l3bfsc_error_inq_cmd_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_canitfleo_usbcan_l2frame_receive(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_canitfleo_sysswm_inventory_confirm(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_canitfleo_sysswm_sw_package_confirm(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT hcu_canitfleo_hate_send_data(char *buf, int len, UINT8 node);

//Local API
OPSTAT func_canitfleo_int_init(void);
OPSTAT func_canitfleo_working_scan_process(void);
OPSTAT func_canitfleo_bfsc_simulation_data_process(void);
OPSTAT func_canitfleo_l2frame_msg_bfsc_startup_ind_received_handle(StrMsg_HUITP_MSGID_sui_bfsc_startup_ind_t *rcv, UINT8 nodeId);
OPSTAT func_canitfleo_l2frame_msg_bfsc_set_config_resp_received_handle(StrMsg_HUITP_MSGID_sui_bfsc_set_config_resp_t *rcv, UINT8 nodeId);
OPSTAT func_canitfleo_l2frame_msg_bfsc_start_resp_received_handle(StrMsg_HUITP_MSGID_sui_bfsc_start_resp_t *rcv, UINT8 nodeId);
OPSTAT func_canitfleo_l2frame_msg_bfsc_stop_resp_received_handle(StrMsg_HUITP_MSGID_sui_bfsc_stop_resp_t *rcv, UINT8 nodeId);
OPSTAT func_canitfleo_l2frame_msg_bfsc_new_ws_event_received_handle(StrMsg_HUITP_MSGID_sui_bfsc_new_ws_event_t *rcv, UINT8 nodeId);
OPSTAT func_canitfleo_l2frame_msg_bfsc_repeat_ws_received_handle(StrMsg_HUITP_MSGID_sui_bfsc_repeat_ws_event_t *rcv, UINT8 nodeId);
OPSTAT func_canitfleo_l2frame_msg_bfsc_ws_comb_out_received_handle(StrMsg_HUITP_MSGID_sui_bfsc_ws_comb_out_resp_t *rcv, UINT8 nodeId);
OPSTAT func_canitfleo_l2frame_msg_bfsc_command_resp_received_handle(StrMsg_HUITP_MSGID_sui_bfsc_command_resp_t *rcv, UINT8 nodeId);
OPSTAT func_canitfleo_l2frame_msg_bfsc_fault_ind_received_handle(StrMsg_HUITP_MSGID_sui_bfsc_fault_ind_t *rcv, UINT8 nodeId);
OPSTAT func_canitfleo_l2frame_msg_bfsc_err_ind_cmd_resp_received_handle(StrMsg_HUITP_MSGID_sui_bfsc_err_inq_cmd_resp_t *rcv, UINT8 nodeId);
OPSTAT func_canitfleo_l2frame_msg_bfsc_heart_beat_report_received_handle(StrMsg_HUITP_MSGID_sui_bfsc_heart_beat_report_t *rcv, UINT8 nodeId);
OPSTAT func_canitfleo_l2frame_msg_inventory_report_received_handle(StrMsg_HUITP_MSGID_sui_inventory_report_t *rcv, UINT8 nodeId);
OPSTAT func_canitfleo_l2frame_msg_sw_package_report_received_handle(StrMsg_HUITP_MSGID_sui_sw_package_report_t *rcv, UINT8 nodeId);



//Socket CAN APIs
OPSTAT func_canitfleo_socketcan_receive(int socket, canid_t *canid, char *canframe_hex, char *canid_canframe_char);
OPSTAT func_canitfleo_socketcan_send(int socket, char *canid_canframe);
OPSTAT func_canitfleo_socketcan_init(char *canitfname, int *sock);
int    func_canitfleo_socketcan_test_main(int argc, char **argv);

//USBCAN BSP function mapping
OPSTAT hcu_canitfleo_usbcan_interface_init(void);
OPSTAT hcu_canitfleo_usbcan_l2frame_send(UINT8 *buffer, UINT32 length, UINT32 wmc_id_bitmap);


//高级定义，简化程序的可读性
#define HCU_ERROR_PRINT_CANITFLEO(...)	do{zHcuSysStaPm.taskRunErrCnt[TASK_ID_CANITFLEO]++;  HcuErrorPrint(__VA_ARGS__);  return FAILURE;}while(0)

#endif /* L2FRAME_CANITFLEO_H_ */
