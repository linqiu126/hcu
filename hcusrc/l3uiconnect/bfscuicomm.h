/*
 * bfscuicomm.h
 *
 *  Created on: 2016年5月14日
 *      Author: hitpony
 */

#ifndef L3UI_BFSCUICOMM_H_
#define L2FRAME_BFSCUICOMM_H_

#include "../l0comvm/vmlayer.h"

//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
//#define FSM_STATE_COMMON  	0x02
enum FSM_STATE_BFSCUICOMM
{
	FSM_STATE_BFSCUICOMM_INITED = FSM_STATE_COMMON + 1,
	FSM_STATE_BFSCUICOMM_ACTIVED,
	FSM_STATE_BFSCUICOMM_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

//本地常量定义
#define HCU_BFSCCOMM_JASON_CMD_START 		1
#define HCU_BFSCCOMM_JASON_CMD_STOP 		2
#define HCU_BFSCCOMM_JASON_CMD_CONFIG 		3
#define HCU_BFSCCOMM_JASON_CMD_TEST 		4

//Global variables
extern HcuFsmStateItem_t HcuFsmBfscuicomm[];

//API
extern OPSTAT fsm_bfscuicomm_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_bfscuicomm_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_bfscuicomm_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_bfscuicomm_timeout(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_bfscuicomm_l3bfsc_cmd_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_bfscuicomm_l3bfsc_cfg_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_bfscuicomm_can_test_cmd_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);

//Local API
OPSTAT func_bfscuicomm_int_init(void);
void func_bfscuicomm_install_file_scan(void);
void func_bfscuicomm_scan_jason_callback(void);
OPSTAT func_bfscuicomm_read_cfg_file_into_ctrl_table(void);

//External APIs
extern OPSTAT dbi_HcuBfsc_Fb2Ui_save(UINT32 cmdType, UINT32 validFlag, char *info);

//高级定义，简化程序的可读性
#define HCU_ERROR_PRINT_BFSCUICOMM(...)	do{zHcuSysStaPm.taskRunErrCnt[TASK_ID_BFSCUICOMM]++;  HcuErrorPrint(__VA_ARGS__);  return FAILURE;}while(0)


#endif /* L2FRAME_BFSCUICOMM_H_ */
