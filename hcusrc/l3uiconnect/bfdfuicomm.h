/*
 * bfdfuicomm.h
 *
 *  Created on: 2016年5月14日
 *      Author: hitpony
 */

#ifndef L3UI_BFDFUICOMM_H_
#define L3UI_BFDFUICOMM_H_

#include "../l1com/l1comdef.h"
#include "../l2codec/huitp.h"
#include "../l2codec/huicobus.h"
#include "../l2codec/huicobuscodec.h"


//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
//#define FSM_STATE_COMMON  	0x02
enum FSM_STATE_BFDFUICOMM
{
	FSM_STATE_BFDFUICOMM_INITED = FSM_STATE_COMMON + 1,
	FSM_STATE_BFDFUICOMM_ACTIVED,
	FSM_STATE_BFDFUICOMM_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

//Global variables
extern HcuFsmStateItem_t HcuFsmBfdfuicomm[];

//API
extern OPSTAT fsm_bfdfuicomm_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_bfdfuicomm_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_bfdfuicomm_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_bfdfuicomm_timeout(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_bfdfuicomm_l3bfdf_cmd_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_bfdfuicomm_sui_test_cmd_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_bfdfuicomm_huicobus_uir_init_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_bfdfuicomm_huicobus_uir_start_resume_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_bfdfuicomm_huicobus_uir_stop_suspend_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_bfdfuicomm_huicobus_uir_static_cali_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_bfdfuicomm_huicobus_uir_dynamic_cali_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_bfdfuicomm_huicobus_uir_test_cmd_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_bfdfuicomm_huicobus_uir_one_key_zero_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);

//Local API
OPSTAT 	func_bfdfuicomm_read_cfg_file_into_ctrl_table (UINT16 configId);
OPSTAT  func_bfdfuicomm_time_out_period_read_process(void);
bool 	func_bfdfuicomm_hopper_bitmap_validate(UINT32 hopperBitmap, UINT8 hopperNum, UINT8 hopperArr[HCU_SYSMSG_SUI_SENSOR_NBR]);
bool 	func_bfdfuicomm_hopper_state_set_init(UINT8 streamId);

//External APIs
extern OPSTAT dbi_HcuBfdf_Fb2Ui_save(UINT32 cmdType, UINT32 validFlag, char *info);
extern UINT32 dbi_HcuBfdf_CallCellMaxIdGet(void);

//高级定义，简化程序的可读性
#define HCU_ERROR_PRINT_BFDFUICOMM(...)	do{zHcuSysStaPm.taskRunErrCnt[TASK_ID_BFDFUICOMM]++;  HcuErrorPrint(__VA_ARGS__);  return FAILURE;}while(0)


#endif /* L2FRAME_BFDFUICOMM_H_ */
