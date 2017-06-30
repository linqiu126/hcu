/*
 * sysswm.h
 *
 *  Created on: 2016年5月10日
 *      Author: hitpony
 */

#ifndef L3APP_SYSSWM_H_
#define L3APP_SYSSWM_H_

#include "../l0comvm/vmlayer.h"

//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
//#define FSM_STATE_COMMON  	0x02
enum FSM_STATE_SYSSWM
{
	FSM_STATE_SYSSWM_INITED = FSM_STATE_COMMON + 1,
	FSM_STATE_SYSSWM_ACTIVED,
	FSM_STATE_SYSSWM_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

//Global variables
extern HcuFsmStateItem_t HcuFsmSysswm[];

typedef struct gTaskSysswmContextBody
{
	UINT32 totalSeg;
	UINT32 segLen;
	UINT32 cfmSegIndex;
	UINT16 targetSwRel;
	UINT16 targetSwVer;
	UINT32 retransTimes;
}gTaskSysswmContextBody_t;
typedef struct gTaskSysswmContext
{
	gTaskSysswmContextBody_t  	bhSw;
	gTaskSysswmContextBody_t  	ihuSw;
}gTaskSysswmContext_t;

#define HCU_SYSSWM_SW_PACKAGE_RETRANS_MAX_TIMES  10

//API
extern OPSTAT fsm_sysswm_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_sysswm_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_sysswm_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_sysswm_time_out(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_sysswm_cloudvela_inventory_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_sysswm_cloudvela_inventory_confirm(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_sysswm_cloudvela_sw_package_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_sysswm_cloudvela_sw_package_confirm(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);


//Local API
OPSTAT func_sysswm_int_init(void);
OPSTAT func_sysswm_time_out_period_working_scan(void);

//高级定义，简化程序的可读性
#define HCU_ERROR_PRINT_SYSSWM(...)	do{zHcuSysStaPm.taskRunErrCnt[TASK_ID_SYSSWM]++;  HcuErrorPrint(__VA_ARGS__);  return FAILURE;}while(0)


#endif /* L3APP_SYSSWM_H_ */
