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
	UINT32 overTimeDur;
	UINT8  resndTimesSet;
	UINT32 AlarmTarget; //号称叫上位机地址，未来可能需要将这些参数存入数据库
}gTaskLlczhbContextStatic_t;
typedef struct gTaskLlczhbContextLinkDynamic
{
	UINT32 overTime;
	UINT8  resndCnt;
	msgie_struct_zhbhjt_element_dl2hcu_t dlRx;
	msgie_struct_zhbhjt_element_ul2cloud_t ulTx;
}gTaskLlczhbContextLinkDynamic_t;

typedef struct gTaskLlczhbContext
{
	msgie_struct_zhbhjt_frame_head_t frameHead;
	gTaskLlczhbContextStatic_t 		envSd;  //静态数据
	gTaskLlczhbContextLinkDynamic_t llcSn;  //动态链路会话
	UINT8 resndCnt;
	UINT8 llcState;
}gTaskLlczhbContext_t;

#define LLCZHB_CFG_STATIC_LLC_OVERTIME_DUR   10 //in second
#define LLCZHB_CFG_STATIC_LLC_RESND_CNT   3
#define LLCZHB_STATE_CTRL_NULL 0
#define LLCZHB_STATE_CTRL_DEACTIVE 1
#define LLCZHB_STATE_CTRL_ACTIVE 2
#define LLCZHB_STATE_CTRL_WFFB_FROM_L3MOD_GET_RTD 3
#define LLCZHB_STATE_CTRL_WFFB_FROM_CLOUD_GET_RTD 4
#define LLCZHB_STATE_CTRL_WFFB_FROM_L3MOD_GET_EQU_RUN 5
#define LLCZHB_STATE_CTRL_WFFB_FROM_CLOUD_GET_EQU_RUN 6
#define LLCZHB_STATE_CTRL_WFFB_FROM_L3MOD_GET_POL_MIN 7
#define LLCZHB_STATE_CTRL_WFFB_FROM_CLOUD_GET_POL_MIN 8
#define LLCZHB_STATE_CTRL_WFFB_FROM_L3MOD_GET_POL_DAY 9
#define LLCZHB_STATE_CTRL_WFFB_FROM_CLOUD_GET_POL_DAY 10
#define LLCZHB_STATE_CTRL_WFFB_FROM_L3MOD_GET_POL_TODAY_RT 11
#define LLCZHB_STATE_CTRL_WFFB_FROM_L3MOD_GET_POL_ALA 12
#define LLCZHB_STATE_CTRL_WFFB_FROM_CLOUD_REPORT_ALA_TYPE 13
#define LLCZHB_STATE_CTRL_WFFB_FROM_L3MOD_SET_ALARM_LIMITATION 14
#define LLCZHB_STATE_CTRL_WFFB_FROM_L3MOD_GET_ALARM_LIMUTATION 15
#define LLCZHB_STATE_CTRL_WFFB_FROM_L3MOD_SET_DATA_RPT_TIME 16
#define LLCZHB_STATE_CTRL_WFFB_FROM_L3MOD_GET_DATA_RPT_TIME 17
#define LLCZHB_STATE_CTRL_WFFB_FROM_L3MOD_SET_CALIBRATION_ZERO 18
#define LLCZHB_STATE_CTRL_WFFB_FROM_L3MOD_SET_RTDI 19
#define LLCZHB_STATE_CTRL_WFFB_FROM_L3MOD_GET_RTDI 20
#define LLCZHB_STATE_CTRL_WFFB_FROM_L3MOD_SET_WARN_TIME 21
#define LLCZHB_STATE_CTRL_WFFB_FROM_L3MOD_SET_INSTANCE_SAMPLE 22
#define LLCZHB_STATE_CTRL_WFFB_FROM_L3MOD_SET_SAMPLE_CYCLE 22



//API
extern OPSTAT fsm_llczhb_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_llczhb_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_llczhb_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_llczhb_cloudvela_l2frame_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_llczhb_l3mod_llczhb_ctrl_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_llczhb_l3mod_llczhb_data_report(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_llczhb_time_out(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);



//Local API
OPSTAT func_llczhb_int_init(void);
void   func_llczhb_stm_main_recovery_from_fault(void);
OPSTAT func_llczhb_time_out_link_control_process(void);
//CONTROL COMMAND
OPSTAT fsm_llczhb_send_to_cloud_ctrl_req_answer_directly_in_l2llc(UINT8 qnRtn);
OPSTAT fsm_llczhb_send_to_cloud_ctrl_execute_operation_result(UINT8 exeRtn);
OPSTAT fsm_llczhb_send_to_cloud_ctrl_notify_answer(void);
//DATA COMMAND
OPSTAT fsm_llczhb_send_to_cloud_data_get_field_time(void);
OPSTAT fsm_llczhb_send_to_cloud_data_get_field_addr(void);
OPSTAT fsm_llczhb_send_to_cloud_data_data_get_pol_rtd(void);
OPSTAT fsm_llczhb_send_to_cloud_data_get_equ_run(void);
OPSTAT fsm_llczhb_send_to_cloud_data_get_pol_min_rpt(void);
OPSTAT fsm_llczhb_send_to_cloud_data_get_pol_day_rpt(void);
OPSTAT fsm_llczhb_send_to_cloud_data_get_pol_today_rt(void);
OPSTAT fsm_llczhb_send_to_cloud_data_get_pol_his_day_rpt(void);
OPSTAT fsm_llczhb_send_to_cloud_data_get_pol_ala_record(void);
OPSTAT fsm_llczhb_send_to_cloud_data_report_ala_event(void);
OPSTAT fsm_llczhb_send_to_cloud_data_get_alarm_limitation(void);
OPSTAT fsm_llczhb_send_to_cloud_data_get_report_time(void);
OPSTAT fsm_llczhb_send_to_cloud_data_get_rtdi_real_time_data_tti(void);





//高级定义，简化程序的可读性
#define HCU_ERROR_PRINT_LLCZHB(...)	do{zHcuSysStaPm.taskRunErrCnt[TASK_ID_LLCZHB]++; HcuErrorPrint(__VA_ARGS__);  return FAILURE;}while(0)
#define HCU_ERROR_PRINT_LLCZHB_RECOVERY(...)	do{zHcuSysStaPm.taskRunErrCnt[TASK_ID_LLCZHB]++; func_llczhb_stm_main_recovery_from_fault(); HcuErrorPrint(__VA_ARGS__);  return FAILURE;}while(0)



#endif /* L2FRAME_LLCZHB_H_ */
