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
#define IHU_L3BFSC_SENSOR_WS_STATUS_INVALID 	0  		//秤盘无效
#define IHU_L3BFSC_SENSOR_WS_STATUS_INVALID1 255  		//秤盘无效
#define IHU_L3BFSC_SENSOR_WS_STATUS_EMPTY 1       		//秤盘空
#define IHU_L3BFSC_SENSOR_WS_STATUS_VALID_ERROR 2 		//秤盘有料数值错误
#define IHU_L3BFSC_SENSOR_WS_STATUS_VALID_TO_COMB 3 	//秤盘有料待组合
#define IHU_L3BFSC_SENSOR_WS_STATUS_VALID_TO_TTT 4		//秤盘有料待出料
#define IHU_L3BFSC_SENSOR_WS_STATUS_VALID_TO_TGU 5		//秤盘有料待抛弃

//组合目标的控制表
typedef struct L3BfscGenCtrlTable
{
	UINT32  targetValue;
	UINT32	targetUpLimit;
	UINT8	minWsNbr;
	UINT8	maxWsNbr;
	UINT8	currentStatus;
	L3BfscSensorWsInfo_t	sensorWs[HCU_BFSC_SENSOR_WS_NBR_MAX];
	UINT8 wsRrSearchStart;  //素搜算法从哪一个秤盘传感器开始
}L3BfscGenCtrlTable_t;
#define IHU_L3BFSC_WHOLE_STATUS_INVALID		0
#define IHU_L3BFSC_WHOLE_STATUS_INVALID1	255
#define IHU_L3BFSC_WHOLE_STATUS_TO_COMB		1
#define IHU_L3BFSC_WHOLE_STATUS_ERROR		2
#define IHU_L3BFSC_WHOLE_STATUS_TTT			3
#define IHU_L3BFSC_WHOLE_STATUS_TGU			4


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
extern OPSTAT fsm_l3bfsc_canitf_ws_report(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3bfsc_can_ws_comb_out_fb(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3bfsc_can_ws_give_up_fb(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3bfsc_uicomm_cmd_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3bfsc_cloudvela_cmd_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3bfsc_canitf_ws_new_ready_event(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3bfsc_can_ws_init_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);

//Local API
OPSTAT func_l3bfsc_int_init(void);



#endif /* L3APP_BFSC_H_ */
