/*
 * llczhb.h
 *
 *  Created on: 2016年5月14日
 *      Author: hitpony
 */

#ifndef L2FRAME_LLCZHB_H_
#define L2FRAME_LLCZHB_H_

#include "../l0comvm/vmlayer.h"

//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
//#define FSM_STATE_COMMON  	0x02
enum FSM_STATE_LLCZHB
{
	FSM_STATE_LLCZHB_INITED = FSM_STATE_COMMON + 1,
	FSM_STATE_LLCZHB_INITIED,
	FSM_STATE_LLCZHB_ACTIVED,
	FSM_STATE_LLCZHB_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

//Global variables
extern HcuFsmStateItem_t HcuFsmLlczhb[];

typedef struct gTaskLlczhbContextStatic
{
	UINT32 pswd;
}gTaskLlczhbContextStatic_t;
typedef struct gTaskLlczhbContextLinkDynamic
{
	msgie_struct_zhbhjt_frame_head_t frameHead;
	UINT32 overTime;
	UINT8  resndCnt;
}gTaskLlczhbContextLinkDynamic_t;

typedef struct gTaskLlczhbContext
{
	UINT32 pswd;
	gTaskLlczhbContextStatic_t 		env;
	gTaskLlczhbContextLinkDynamic_t llc;
}gTaskLlczhbContext_t;



//API
extern OPSTAT fsm_llczhb_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_llczhb_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_llczhb_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_llczhb_cloudvela_l2frame_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_llczhb_l3mod_llczhb_ctrl_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_llczhb_l3mod_llczhb_data_report(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);



//Local API
OPSTAT func_llczhb_int_init(void);


//高级定义，简化程序的可读性
#define HCU_ERROR_PRINT_LLCZHB(...)	do{zHcuSysStaPm.taskRunErrCnt[TASK_ID_LLCZHB]++;  HcuErrorPrint(__VA_ARGS__);  return FAILURE;}while(0)



#endif /* L2FRAME_LLCZHB_H_ */
