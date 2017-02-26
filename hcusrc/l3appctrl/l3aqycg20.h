/*
 * l3aqycg20.h
 *
 *  Created on: 2015年11月22日
 *      Author: test
 */

#ifndef L3APP_AQYC_G20_H_
#define L3APP_AQYC_G20_H_

#include "../l0comvm/vmlayer.h"
#include "../l1com/l1comdef.h"

//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
//#define FSM_STATE_COMMON  	0x02
enum FSM_STATE_L3AQYCG20
{
	FSM_STATE_L3AQYCG20_INITED = FSM_STATE_COMMON + 1,
	FSM_STATE_L3AQYCG20_ACTIVED,
	FSM_STATE_L3AQYCG20_WFFB_CTRL,
	FSM_STATE_L3AQYCG20_WFFB_DATA,
	FSM_STATE_L3AQYCG20_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

//Global variables
extern HcuFsmStateItem_t HcuFsmL3aqycg20[];

typedef struct gTaskL3aqycq20Context
{
	//实时统计部分：均以一个统计周期为单位
	HcuSysMsgIeL3aqycContextStaElement_t cur;  		//当前统计基础颗粒中的数值
	HcuSysMsgIeL3aqycContextStaElement_t  curAge;	//使用老化算法，需要该域存下中间结果，不然每一次计算均采用近似会导致数据失真

	//统计报告部分
	HcuSysMsgIeL3aqycContextStaElement_t staInstant;  	//瞬时值结果，是否需要带Max,Min带商榷
	HcuSysMsgIeL3aqycContextStaElement_t staOneMin;  	//1分钟统计结果
	HcuSysMsgIeL3aqycContextStaElement_t sta60Min;		//60分钟统计结果
	HcuSysMsgIeL3aqycContextStaElement_t sta24H;		//24H统计结果
}gTaskL3aqycq20Context_t;



//API
extern OPSTAT fsm_l3aqycg20_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3aqycg20_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3aqycg20_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3aqycg20_time_out(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3aqycg20_cloudvela_data_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3aqycg20_cloudvela_data_confirm(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3aqycg20_cloudvela_ctrl_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3aqycg20_huil3mod_exg_ctrl_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3aqycg20_huil3mod_exg_data_report(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3aqycg20_llczhb_ctrl_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3aqycg20_zhbl3mod_exg_ctrl_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3aqycg20_zhbl3mod_exg_data_report(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);

//Local API
OPSTAT func_l3aqycg20_int_init(void);


//高级定义，简化程序的可读性
#define HCU_ERROR_PRINT_L3AQYCG20(...)	do{zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;  HcuErrorPrint(__VA_ARGS__);  return FAILURE;}while(0)



#endif /* L3APP_AQYC_G20_H_ */
