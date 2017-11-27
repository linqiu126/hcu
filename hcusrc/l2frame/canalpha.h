/*
 * canalpha.h
 *
 *  Created on: 2016年5月14日
 *      Author: hitpony
 */

#ifndef L2FRAME_CANALPHA_H_
#define L2FRAME_CANALPHA_H_

#include "../l0comvm/vmlayer.h"
#include "../l1com/l1comdef.h"
#include "l2usbcan.h"   		//Added by MYC 2017/05/15
#include "../l2codec/huitp.h"	//Added by MYC 2017/05/15

//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
//#define FSM_STATE_COMMON  	0x02
enum FSM_STATE_CANALPHA
{
	FSM_STATE_CANALPHA_INITED = FSM_STATE_COMMON + 1,
	FSM_STATE_CANALPHA_ACTIVED,
	FSM_STATE_CANALPHA_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

//Global variables
extern HcuFsmStateItem_t HcuFsmCanalpha[];

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

typedef struct gTaskCanalphaContext
{
	UINT32 sensorIdRoundBing;
	UINT32 can_socket_id;
	HcuUsbCanHandleTypeDef_t can1;  //Added by MYC 2017/05/15
}gTaskCanalphaContext_t;


/*
 *
 *  公共函数
 *
 *
 *
 */

//API
extern OPSTAT fsm_canalpha_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_canalpha_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_canalpha_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_canalpha_timeout(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_canalpha_usbcan_l2frame_receive(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_canalpha_sysswm_inventory_confirm(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_canalpha_sysswm_sw_package_confirm(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_canalpha_sui_test_cmd_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_canalpha_sui_suspend_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_canalpha_sui_resume_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_canalpha_sui_heart_beat_confirm(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT hcu_canalpha_hate_send_data(UINT8 *buf, int len, UINT8 node);


//Local API
OPSTAT func_canalpha_int_init(void);
OPSTAT func_canalpha_working_scan_process(void);
OPSTAT func_canalpha_l2frame_msg_inventory_report_received_handle(StrMsg_HUITP_MSGID_sui_inventory_report_t *rcv, UINT8 nodeId);
OPSTAT func_canalpha_l2frame_msg_sw_package_report_received_handle(StrMsg_HUITP_MSGID_sui_sw_package_report_t *rcv, UINT8 nodeId);
OPSTAT func_canalpha_l2frame_msg_com_test_command_resp_received_handle(StrMsg_HUITP_MSGID_sui_com_test_command_resp_t *rcv, UINT8 nodeId);
OPSTAT func_canalpha_l2frame_msg_com_heart_beat_report_received_handle(StrMsg_HUITP_MSGID_sui_com_heart_beat_report_t *rcv, UINT8 nodeId);
OPSTAT func_canalpha_l2frame_msg_com_startup_ind_received_handle(StrMsg_HUITP_MSGID_sui_com_startup_ind_t *rcv, UINT8 nodeId);
OPSTAT func_canalpha_l2frame_msg_com_fault_ind_received_handle(StrMsg_HUITP_MSGID_sui_com_fault_ind_t *rcv, UINT8 nodeId);
OPSTAT func_canalpha_l2frame_msg_com_suspend_resp_received_handle(StrMsg_HUITP_MSGID_sui_com_suspend_resp_t *rcv, UINT8 nodeId);
OPSTAT func_canalpha_l2frame_msg_com_resume_resp_received_handle(StrMsg_HUITP_MSGID_sui_com_resume_resp_t *rcv, UINT8 nodeId);

/*
 *
 *  BFDF
 *
 *
 *
 */

//API
OPSTAT fsm_canalpha_l3bfdf_sys_cfg_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
OPSTAT fsm_canalpha_l3bfdf_ws_comb_out(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
OPSTAT fsm_canalpha_l3bfdf_snc_pullin_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
//Local API
OPSTAT func_canalpha_bfdf_simulation_data_process(void);
OPSTAT func_canalpha_l2frame_msg_bfdf_set_config_resp_received_handle(StrMsg_HUITP_MSGID_sui_bfdf_set_config_resp_t *rcv, UINT8 nodeId);
OPSTAT func_canalpha_l2frame_msg_bfdf_new_ws_event_received_handle(StrMsg_HUITP_MSGID_sui_bfdf_new_ws_event_t *rcv, UINT8 nodeId);
OPSTAT func_canalpha_l2frame_msg_bfdf_ws_comb_out_received_handle(StrMsg_HUITP_MSGID_sui_bfdf_ws_comb_out_resp_t *rcv, UINT8 nodeId);
OPSTAT func_canalpha_l2frame_msg_bfdf_basket_clean_ind_received_handle(StrMsg_HUITP_MSGID_sui_bfdf_basket_clean_ind_t *rcv, UINT8 nodeId);

/*
 *
 *  BFHS
 *
 *
 *
 */

//BFHS API
OPSTAT fsm_canalpha_l3bfhs_sys_cfg_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
OPSTAT fsm_canalpha_l3bfhs_cal_zero_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
OPSTAT fsm_canalpha_l3bfhs_cal_full_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
//Local API
OPSTAT func_canalpha_bfhs_simulation_data_process(void);
OPSTAT func_canalpha_l2frame_msg_bfhs_calibration_zero_resp_received_handle(StrMsg_HUITP_MSGID_sui_bfhs_calibration_zero_resp_t *rcv, UINT8 nodeId);
OPSTAT func_canalpha_l2frame_msg_bfhs_calibration_full_resp_received_handle(StrMsg_HUITP_MSGID_sui_bfhs_calibration_full_resp_t *rcv, UINT8 nodeId);
OPSTAT func_canalpha_l2frame_msg_bfhs_set_config_resp_received_handle(StrMsg_HUITP_MSGID_sui_bfhs_set_config_resp_t *rcv, UINT8 nodeId);
OPSTAT func_canalpha_l2frame_msg_bfhs_new_ws_event_received_handle(StrMsg_HUITP_MSGID_sui_bfhs_new_ws_event_t *rcv, UINT8 nodeId);

/*
 *
 *  公共支撑函数部分
 *
 *
 *
 */

//Socket CAN APIs
OPSTAT func_canalpha_socketcan_receive(int socket, canid_t *canid, char *canframe_hex, char *canid_canframe_char);
OPSTAT func_canalpha_socketcan_send(int socket, char *canid_canframe);
OPSTAT func_canalpha_socketcan_init(char *canitfname, int *sock);
int    func_canalpha_socketcan_test_main(int argc, char **argv);
void   func_canalpha_huicobus_trigger_uir(UINT32 cmdId, UINT32 cmdValue);

//USBCAN BSP function mapping
OPSTAT hcu_canalpha_usbcan_interface_init(void);
OPSTAT hcu_canalpha_usbcan_l2frame_send(UINT8 *buffer, UINT32 length, UINT32 wmc_id_bitmap);


//高级定义，简化程序的可读性
#define HCU_ERROR_PRINT_CANALPHA(...)	do{zHcuSysStaPm.taskRunErrCnt[TASK_ID_CANALPHA]++;  HcuErrorPrint(__VA_ARGS__);  return FAILURE;}while(0)

#endif /* L2FRAME_CANALPHA_H_ */
