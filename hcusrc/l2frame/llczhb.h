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
	char pswd[7];
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
	UINT8 llcState;  //=>链路状态，因为消息简单，必须采用不同的状态来设定状态机
}gTaskLlczhbContext_t;

typedef enum
{
	LLCZHB_STATE_CTRL_NULL = 0,
	LLCZHB_STATE_CTRL_DEACTIVE,
	LLCZHB_STATE_CTRL_ACTIVE,
	LLCZHB_STATE_CTRL_WFFB_FROM_L3MOD_GET_RTD_2011,
	LLCZHB_STATE_CTRL_WFFB_FROM_CLOUD_GET_RTD_2011,
	LLCZHB_STATE_CTRL_WFFB_FROM_L3MOD_GET_EQU_RUN_2021,
	LLCZHB_STATE_CTRL_WFFB_FROM_CLOUD_GET_EQU_RUN_2021,
	LLCZHB_STATE_CTRL_WFFB_FROM_L3MOD_GET_POL_MIN_2051,
	LLCZHB_STATE_CTRL_WFFB_FROM_CLOUD_GET_POL_MIN_2051,
	LLCZHB_STATE_CTRL_WFFB_FROM_L3MOD_GET_POL_HOUR_2061,//by shanchun
	LLCZHB_STATE_CTRL_WFFB_FROM_CLOUD_GET_POL_HOUR_2061,//by shanchun
	LLCZHB_STATE_CTRL_WFFB_FROM_L3MOD_GET_POL_DAY_2031,
	LLCZHB_STATE_CTRL_WFFB_FROM_CLOUD_GET_POL_DAY_2031,
	LLCZHB_STATE_CTRL_WFFB_FROM_L3MOD_GET_POL_TODAY_RT_2041,
	LLCZHB_STATE_CTRL_WFFB_FROM_L3MOD_GET_POL_ALA_2071,
	LLCZHB_STATE_CTRL_WFFB_FROM_CLOUD_REPORT_ALA_TYPE_2072,
	LLCZHB_STATE_CTRL_WFFB_FROM_L3MOD_SET_ALARM_LIMITATION_1022,
	LLCZHB_STATE_CTRL_WFFB_FROM_L3MOD_GET_ALARM_LIMUTATION_1021,
	LLCZHB_STATE_CTRL_WFFB_FROM_L3MOD_SET_DATA_RPT_TIME_1042,
	LLCZHB_STATE_CTRL_WFFB_FROM_L3MOD_GET_DATA_RPT_TIME_1041,
	LLCZHB_STATE_CTRL_WFFB_FROM_L3MOD_SET_CALIBRATION_ZERO_3011,
	LLCZHB_STATE_CTRL_WFFB_FROM_L3MOD_SET_RTDI_1062,
	LLCZHB_STATE_CTRL_WFFB_FROM_L3MOD_GET_RTDI_1061,
	LLCZHB_STATE_CTRL_WFFB_FROM_L3MOD_SET_WARN_TIME_1001,
	LLCZHB_STATE_CTRL_WFFB_FROM_L3MOD_SET_INSTANCE_SAMPLE_3012,
	LLCZHB_STATE_CTRL_WFFB_FROM_L3MOD_SET_SAMPLE_CYCLE_3014,
}gTaskLlczhbLlcStateDefinition_t;

#define LLCZHB_CFG_STATIC_LLC_OVERTIME_DUR   10 //in second
#define LLCZHB_CFG_STATIC_LLC_RESND_CNT   3




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
OPSTAT fsm_llczhb_send_to_cloud_ctrl_req_answer_directly_in_l2llc_9011(UINT8 qnRtn);
OPSTAT fsm_llczhb_send_to_cloud_ctrl_execute_operation_result_9012(UINT8 exeRtn);
OPSTAT fsm_llczhb_send_to_cloud_ctrl_notify_answer_9013(void);
//DATA COMMAND
OPSTAT fsm_llczhb_send_to_cloud_data_get_field_time_1011(void);
OPSTAT fsm_llczhb_send_to_cloud_data_get_field_addr_1031(void);
OPSTAT fsm_llczhb_send_to_cloud_data_data_get_pol_rtd_2011(void);
OPSTAT fsm_llczhb_send_to_cloud_data_get_equ_run_2021(void);
OPSTAT fsm_llczhb_send_to_cloud_data_get_pol_min_rpt_2051(void);
OPSTAT fsm_llczhb_send_to_cloud_data_get_pol_day_rpt_2031(void);
OPSTAT fsm_llczhb_send_to_cloud_data_get_pol_today_rt_2041(void);
OPSTAT fsm_llczhb_send_to_cloud_data_get_pol_his_day_rpt_2031(void);
OPSTAT fsm_llczhb_send_to_cloud_data_get_pol_ala_record_2071(void);
OPSTAT fsm_llczhb_send_to_cloud_data_report_ala_event_2072(void);
OPSTAT fsm_llczhb_send_to_cloud_data_get_alarm_limitation_1021(void);
OPSTAT fsm_llczhb_send_to_cloud_data_get_report_time_1041(void);
OPSTAT fsm_llczhb_send_to_cloud_data_get_rtdi_interval_1061(void);
OPSTAT fsm_llczhb_send_to_cloud_data_get_pol_hour_rpt_2061(void);




//高级定义，简化程序的可读性
#define HCU_ERROR_PRINT_LLCZHB(...)	do{zHcuSysStaPm.taskRunErrCnt[TASK_ID_LLCZHB]++; HcuErrorPrint(__VA_ARGS__);  return FAILURE;}while(0)
#define HCU_ERROR_PRINT_LLCZHB_RECOVERY(...)	do{zHcuSysStaPm.taskRunErrCnt[TASK_ID_LLCZHB]++; func_llczhb_stm_main_recovery_from_fault(); HcuErrorPrint(__VA_ARGS__);  return FAILURE;}while(0)



#endif /* L2FRAME_LLCZHB_H_ */
