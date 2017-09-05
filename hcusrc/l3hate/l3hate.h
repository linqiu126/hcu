/*
 * l3hate.h
 *
 *  Created on: 2015年11月22日
 *      Author: test
 */

#ifndef L3APP_BFDF_H_
#define L3APP_BFDF_H_

#include "../l1com/l1comdef.h"
#include "../l0service/timer.h"


//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
//#define FSM_STATE_COMMON  	0x02
enum FSM_STATE_L3HATE
{
	FSM_STATE_L3HATE_INITED = FSM_STATE_COMMON + 1,
	FSM_STATE_L3HATE_ACTIVED,
	FSM_STATE_L3HATE_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

//主体上下文
typedef struct gTaskL3hateContext
{
	UINT32	tcId;
}gTaskL3hateContext_t;

//Global variables
extern HcuFsmStateItem_t HcuFsmL3hate[];
extern gTaskL3hateContext_t gTaskL3hateContext;


//API
extern OPSTAT fsm_l3hate_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3hate_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3hate_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3hate_time_out(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3hate_eth_frame_rcv(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3hate_sps_frame_rcv(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3hate_can_frame_rcv(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);

//Local API
OPSTAT func_l3hate_int_init(void);

//API for HKvision

//引用外部函数

//高级定义，简化程序的可读性
#define HCU_ERROR_PRINT_L3HATE(...)	do{zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3HATE]++;  HcuErrorPrint(__VA_ARGS__);  return FAILURE;}while(0)
#define HCU_ERROR_PRINT_L3HATE_RECOVERY(...) do{zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3HATE]++; func_l3bfdf_stm_main_recovery_from_fault(); HcuErrorPrint(__VA_ARGS__);  return FAILURE;}while(0)

#endif /* L3APP_BFDF_H_ */
