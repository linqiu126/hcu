/*
 * sensorco1.h
 *
 *  Created on: 2016年5月10日
 *      Author: hitpony
 */

#ifndef L3APP_SENSORCO1_H_
#define L3APP_SENSORCO1_H_

#include "../l0comvm/vmlayer.h"

//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
//#define FSM_STATE_COMMON  	0x02
enum FSM_STATE_CO1
{
	FSM_STATE_CO1_INITED = FSM_STATE_COMMON + 1,
	FSM_STATE_CO1_ACTIVED,
	FSM_STATE_CO1_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

//Global variables
extern HcuFsmStateItem_t HcuFsmCo1[];

typedef struct gTaskCo1Context
{
	UINT32 sendCloudCnt;  //用于描述发送到后台，多少次才发送一次
}gTaskCo1Context_t;




//API
extern OPSTAT fsm_co1_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_co1_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_co1_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_co1_time_out(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_co1_cloudvela_data_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_co1_cloudvela_data_confirm(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);

//Local API
OPSTAT func_co1_int_init(void);

#endif /* L3APP_SENSORCO1_H_ */
