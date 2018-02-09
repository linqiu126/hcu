/*
 * bfhsuicomm.h
 *
 *  Created on: 2016年5月14日
 *      Author: hitpony
 */

#ifndef L3UI_BFHSUICOMM_H_
#define L3UI_BFHSUICOMM_H_

#include "../l1com/l1comdef.h"
#include "../l2codec/huitp.h"
#include "../l2codec/huicobus.h"
#include "../l2codec/huicobuscodec.h"

//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
//#define FSM_STATE_COMMON  	0x02
enum FSM_STATE_BFHSUICOMM
{
	FSM_STATE_BFHSUICOMM_INITED = FSM_STATE_COMMON + 1,
	FSM_STATE_BFHSUICOMM_ACTIVED,
	FSM_STATE_BFHSUICOMM_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

//Global variables
extern HcuFsmStateItem_t HcuFsmBfhsuicomm[];

//API
extern OPSTAT fsm_bfhsuicomm_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_bfhsuicomm_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_bfhsuicomm_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_bfhsuicomm_timeout(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_bfhsuicomm_l3bfhs_ctrl_cmd_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_bfhsuicomm_sui_test_cmd_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_bfhsuicomm_huicobus_uir_init_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_bfhsuicomm_huicobus_uir_start_resume_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_bfhsuicomm_huicobus_uir_stop_suspend_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_bfhsuicomm_huicobus_uir_static_cali_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_bfhsuicomm_huicobus_uir_dynamic_cali_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_bfhsuicomm_huicobus_uir_test_cmd_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_bfhsuicomm_huicobus_uir_one_key_zero_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);

//Local API
OPSTAT  func_bfhsuicomm_read_cfg_db_into_ctrl_table(UINT16 config_index);
OPSTAT  func_bfhsuicomm_time_out_period_read_process(void);


//External APIs
extern OPSTAT dbi_HcuBfhs_Fb2Ui_save(UINT32 cmdType, UINT32 validFlag, char *info);
extern OPSTAT hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_static_cali_resp(INT32 cmdValue, StrHlcIe_cui_hcu2uir_static_cali_resp_t *buf);
extern OPSTAT hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_dynamic_cali_resp(INT32 cmdValue, StrHlcIe_cui_hcu2uir_dynamic_cali_resp_t *buf);

//高级定义，简化程序的可读性
#define HCU_ERROR_PRINT_BFHSUICOMM(...)	do{zHcuSysStaPm.taskRunErrCnt[TASK_ID_BFHSUICOMM]++;  HcuErrorPrint(__VA_ARGS__);  return FAILURE;}while(0)


#endif /* L2FRAME_BFHSUICOMM_H_ */
