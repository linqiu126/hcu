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

typedef struct L3BfscSensorWsInfo
{
	UINT8  sensorWsId;
	UINT32 sensorValue;
	UINT8  sensorStatus; //无效，空料，有料数值错误，有料待组合，有料待出料
}L3BfscSensorWsInfo_t;
//秤盘状态定义
#define HCU_L3BFSC_SENSOR_WS_STATUS_INVALID 	0  		//秤盘无效
#define HCU_L3BFSC_SENSOR_WS_STATUS_INVALID1 255  		//秤盘无效
#define HCU_L3BFSC_SENSOR_WS_STATUS_EMPTY 1       		//秤盘空
#define HCU_L3BFSC_SENSOR_WS_STATUS_VALID_ERROR 2 		//秤盘有料数值错误
#define HCU_L3BFSC_SENSOR_WS_STATUS_VALID_TO_COMB 3 	//秤盘有料待组合
#define HCU_L3BFSC_SENSOR_WS_STATUS_VALID_TO_TTT 4		//秤盘有料待出料
#define HCU_L3BFSC_SENSOR_WS_STATUS_VALID_TO_TGU 5		//秤盘有料待抛弃

//组合目标的控制表
typedef struct L3BfscGenCtrlTable
{
	UINT32  targetValue;
	UINT32	targetUpLimit;
	UINT8	minWsNbr;
	UINT8	maxWsNbr;
	//UINT8	currentStatus;
	L3BfscSensorWsInfo_t	sensorWs[HCU_BFSC_SENSOR_WS_NBR_MAX];
	UINT32 	wsRrSearchStart; //搜索算法从哪一个搜索系数开始
	UINT8  	wsValueNbrFree;  	//空闲的0值秤盘数量
	UINT8   wsValueNbrWeight;	//空闲有值的秤盘数量
	UINT8 	wsValueNbrTtt;  	//待出料有值秤盘数量
	UINT8 	wsValueNbrTgu;  	//待出料有值秤盘数量
	UINT8 	wsBitmap[HCU_BFSC_SENSOR_WS_NBR_MAX];  //组合出的秤盘标示
	UINT32	wsTotalncomingCnt;  		//总共称重和计料数量
	UINT32	wsTotalCombSucTimes;  		//总共成功素搜到目标的次数
	UINT32	wsTotalCombOutMatCnt;		//总共出料的数量
	UINT32	wsTotalGiveupTimes;  		//总共被TGU的次数
	UINT32	wsTotalGiveupMatCnt;		//总共被抛弃的数量
}L3BfscGenCtrlTable_t;
//#define HCU_L3BFSC_WHOLE_STATUS_INVALID		0
//#define HCU_L3BFSC_WHOLE_STATUS_INVALID1	255
//#define HCU_L3BFSC_WHOLE_STATUS_TO_COMB		1
//#define HCU_L3BFSC_WHOLE_STATUS_ERROR		2
//#define HCU_L3BFSC_WHOLE_STATUS_TTT			3
//#define HCU_L3BFSC_WHOLE_STATUS_TGU			4

//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
enum FSM_STATE_L3BFSC
{
	FSM_STATE_L3BFSC_INITED = 0x02,
	FSM_STATE_L3BFSC_ACTIVED,
	FSM_STATE_L3BFSC_OPR_CFG,  	//人工配置状态
	FSM_STATE_L3BFSC_WS_INIT,  	//初始化下位机
	FSM_STATE_L3BFSC_OOS_SCAN,  //进料组合态
	FSM_STATE_L3BFSC_OOS_TTT,  	//出料流程态
	FSM_STATE_L3BFSC_OOS_TGU,  	//放弃物料态
	FSM_STATE_L3BFSC_ERROR_INQ, //数据差错，重新采样所有数据
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
extern OPSTAT fsm_l3bfsc_canitf_error_inq_cmd_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3bfsc_canitf_period_read_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3bfsc_canitf_ws_comb_out_fb(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3bfsc_canitf_ws_give_up_fb(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3bfsc_uicomm_cmd_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3bfsc_cloudvela_cmd_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3bfsc_canitf_general_cmd_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3bfsc_canitf_ws_new_ready_event(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3bfsc_canitf_ws_init_fb(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3bfsc_uicomm_param_set_result(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);

//Local API
OPSTAT func_l3bfsc_int_init(void);
void func_l3bfsc_cacluate_sensor_ws_valid_value(void);
INT32 func_l3bfsc_ws_sensor_search_combination(void);
void func_l3bfsc_ws_sensor_search_give_up(void);
OPSTAT func_l3bfsc_time_out_ws_init_req_process(void);
OPSTAT func_l3bfsc_time_out_ttt_wait_fb_process(void);
OPSTAT func_l3bfsc_time_out_tgu_wait_fb_process(void);
OPSTAT func_l3bfsc_time_out_error_scan_process(void);
OPSTAT func_l3bfsc_time_out_period_read_process(void);


//高级定义，简化程序的可读性
#define HCU_ERROR_PRINT_L3BFSC	zHcuRunErrCnt[TASK_ID_L3BFSC]++; HcuErrorPrint
//验证新技巧，还未用上
#define HCU_ERROR_PRINT_L3BFSC1(arg...)	do{zHcuRunErrCnt[TASK_ID_L3BFSC]++;  HcuErrorPrint(##arg);  return FAILURE;}while(0)
//高级定义，简化程序的可读性=>状态机需要考虑回复
#define HCU_ERROR_PRINT_L3BFSC2 zHcuRunErrCnt[TASK_ID_L3BFSC]++; func_bfsc_stm_main_recovery_from_fault(); HcuErrorPrint



#endif /* L3APP_BFSC_H_ */
