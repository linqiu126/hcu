/*
 * l3hate.h
 *
 *  Created on: 2015年11月22日
 *      Author: test
 */

#ifndef L3APP_HATE_H_
#define L3APP_HATE_H_

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

//CONST定义
enum HCU_HATE_TCID_ENUM
{
	HATE_TCID_MIN = 0,
	HATE_TCID_COM_RESTART_ALL_MODULES,
	HATE_TCID_COM_SETUP_BH_CONNECT,
	HATE_TCID_COM_DISCON_BH_LINK,
	HATE_TCID_COM_BH_HEAT_BEAT_START,
	HATE_TCID_BFDF_STARTUP_IND_COMING,
	HATE_TCID_BFDF_WS_NEW_EVENT_COMING,
	HATE_TCID_BFDF_CONFIG_ALL_NODES,
	HATE_TCID_BFDF_START_ALL_NODES,
	HATE_TCID_BFDF_STOP_ALL_NODES,
	HATE_TCID_BFDF_SUSPEND_ALL_NODES,
	HATE_TCID_MAX,
	HATE_TCID_INVALID = 0xFFFFFFFF,
}; //end of HCU_HATE_TCID_ENUM

//任务上下文
typedef struct gTaskL3hateTestCaseElement
{
	UINT8	ctrl;
	OPSTAT  (*hateFunc)(void);
}gTaskL3hateTestCaseElement_t;
#define HATE_TCE_CTRL_NONE  0
#define HATE_TCE_CTRL_START 1
#define HATE_TCE_CTRL_SND   2
#define HATE_TCE_CTRL_RCV   3
#define HATE_TCE_CTRL_CMPL  4
#define HATE_TCE_CTRL_INVALID  0xFF
#define HATE_TCE_STEPS_MAX_NBR  	30
typedef struct gTaskL3hateTestCaseSet
{
	UINT32	tcId;
	gTaskL3hateTestCaseElement_t tce[HATE_TCE_STEPS_MAX_NBR];
}gTaskL3hateTestCaseSet_t;
extern gTaskL3hateTestCaseSet_t zHcuTc_HATE_TCID_MIN;
extern gTaskL3hateTestCaseSet_t zHcuTc_HATE_TCID_MAX;
extern gTaskL3hateTestCaseSet_t zHcuTc_HATE_TCID_COM_RESTART_ALL_MODULES;

//TEST CASE LIB
typedef struct gTaskL3hateTestCaseLib
{
	gTaskL3hateTestCaseSet_t *tcSet;
}gTaskL3hateTestCaseLib_t;
//extern gTaskL3hateTestCaseSet_t zHcuTcLibTable[HATE_TCID_MAX+1];
extern gTaskL3hateTestCaseLib_t zHcuTcLibTable[HATE_TCID_MAX+1];



//任务上下文
typedef struct gTaskL3hateContext
{
	UINT32	tcId;	//TestCaseId
	UINT32  stepId; //运行到哪一步
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
OPSTAT func_l3hate_tc_snd_restart_all_modues(void);

//引用外部函数

//高级定义，简化程序的可读性
#define HCU_ERROR_PRINT_L3HATE(...)	do{zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3HATE]++;  HcuErrorPrint(__VA_ARGS__);  return FAILURE;}while(0)
#define HCU_ERROR_PRINT_L3HATE_RECOVERY(...) do{zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3HATE]++; func_l3bfdf_stm_main_recovery_from_fault(); HcuErrorPrint(__VA_ARGS__);  return FAILURE;}while(0)

#endif /* L3APP_HATE_H_ */
