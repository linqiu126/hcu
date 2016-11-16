/*
 * l3bfsc.h
 *
 *  Created on: 2015年11月22日
 *      Author: test
 */

#ifndef L3APP_BFSC_H_
#define L3APP_BFSC_H_

#include "../l0comvm/vmlayer.h"
#include "../l1com/l1comdef.h"


//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
enum FSM_STATE_L3BFSC
{
	FSM_STATE_L3BFSC_INITED = 0x02,
	FSM_STATE_L3BFSC_INITIED,
	FSM_STATE_L3BFSC_ACTIVED,
	FSM_STATE_L3BFSC_OPR_CFG,  	//人工配置状态
	FSM_STATE_L3BFSC_OOS_SCAN,  //进料组合态
	FSM_STATE_L3BFSC_OOS_TTT,  	//出料流程态
	FSM_STATE_L3BFSC_OOS_TGU,  	//放弃物料态
	FSM_STATE_L3BFSC_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

//Global variables
extern FsmStateItem_t FsmL3bfsc[];

//API
extern OPSTAT fsm_l3bfsc_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3bfsc_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3bfsc_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3bfsc_time_out(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3bfsc_canitf_cmd_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3bfsc_canitf_ws_report(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3bfsc_can_ws_comb_out_fb(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3bfsc_can_ws_give_up_fb(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3bfsc_uicomm_cmd_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3bfsc_cloudvela_cmd_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);


//Local API
OPSTAT func_l3bfsc_int_init(void);



#endif /* L3APP_BFSC_H_ */