/*
 * l3hate.h
 *
 *  Created on: 2017年9月4日
 *      Author: ZHANG Jianlin
 */

#ifndef L3APP_HATE_H_
#define L3APP_HATE_H_

#include "../l1com/l1comdef.h"
#include "../l0service/timer.h"
#include "../l0service/trace.h"
#include "../l1com/hwinv.h"
#include "../l2frame/cloudvela.h"

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

//测试文件记录
#define HATE_LOG_FILE_NAME		"./hate_record.log"
#define HATE_LOG_RECORD_MAX		200

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

//TEST CASE LIB
typedef struct gTaskL3hateTestCaseLib
{
	gTaskL3hateTestCaseSet_t *tcSet;
}gTaskL3hateTestCaseLib_t;
extern gTaskL3hateTestCaseLib_t zHcuHateTcLibTable[HATE_TCID_MAX+1];
extern UINT32 zHcuHateTcCampaign[];

//任务上下文
typedef struct gTaskL3hateContext
{
	UINT32	tcIndex;	//TestCaseId
	UINT32  stepId; //运行到哪一步
	void    *par;   //全局参数指针，用来传递入口参数的，其实就是接收到的消息数据
}gTaskL3hateContext_t;

//Global variables
extern HcuFsmStateItem_t HcuFsmL3hate[];
extern gTaskL3hateContext_t gTaskL3hateContext;

//运行引擎的事件
#define HATE_TC_RUN_ENGINE_EVENT_NONE  		0
#define HATE_TC_RUN_ENGINE_EVENT_NEW_START  1
#define HATE_TC_RUN_ENGINE_EVENT_ETH_TRG  	2
#define HATE_TC_RUN_ENGINE_EVENT_SPS_TRG  	3
#define HATE_TC_RUN_ENGINE_EVENT_CAN_TRG  	4
#define HATE_TC_RUN_ENGINE_EVENT_TIME_OUT  	5
#define HATE_TC_RUN_ENGINE_EVENT_INVALID  	0xFF

//API
extern OPSTAT fsm_l3hate_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3hate_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3hate_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3hate_time_out(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3hate_start_new_tc(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3hate_eth_frame_rcv(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3hate_sps_frame_rcv(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3hate_can_frame_rcv(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);

//Global APIs
extern OPSTAT hcu_l3hate_test_case_log_file(char *s);

//Local API
OPSTAT func_l3hate_int_init(void);
void   func_l3hate_test_case_run_engine(UINT8 event);
void   func_l3hate_test_case_run_execution_new(UINT32 tcLibId);
void   func_l3hate_test_case_run_execution_rcv(UINT32 tcLibId);
void   func_l3hate_reset_all_modules(void);
UINT32 func_l3hate_search_test_case_number_in_lib(int tcCgnIndex);
void   func_l3hate_test_case_execute_error_process(UINT32 tcLibId);
void   func_l3hate_test_case_execute_success_process(UINT32 tcLibId);

//引用外部函数
extern OPSTAT hcu_ethernet_hate_data_send(CloudDataSendBuf_t *buf);
extern OPSTAT hcu_canitfleo_hate_send_data(char *buf, int len, UINT8 node);


//高级定义，简化程序的可读性
#define HCU_ERROR_PRINT_L3HATE(...)	do{zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3HATE]++;  HcuErrorPrint(__VA_ARGS__);  return FAILURE;}while(0)
#define HATE_START_WORK_TIMER 		do{hcu_timer_start(TASK_ID_L3HATE, TIMER_ID_1S_L3HATE_WORK_CTRL_FB, zHcuSysEngPar.timer.array[TIMER_ID_1S_L3HATE_WORK_CTRL_FB].dur, TIMER_TYPE_ONE_TIME, TIMER_RESOLUTION_1S);}while(0)
#define HATE_STOP_WORK_TIMER 		do{hcu_timer_stop(TASK_ID_L3HATE, TIMER_ID_1S_L3HATE_WORK_CTRL_FB, TIMER_RESOLUTION_1S);}while(0)

#endif /* L3APP_HATE_H_ */
