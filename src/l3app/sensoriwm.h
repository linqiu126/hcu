/*
 * sensoriwm.h
 *
 *  Created on: 2016年7月20日
 *      Author: hitpony
 */

#ifndef L3APP_SENSORIWM_H_
#define L3APP_SENSORIWM_H_

#include "../l0comvm/vmlayer.h"

//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
enum FSM_STATE_IWM
{
	FSM_STATE_IWM_INITED = 0x02,
	FSM_STATE_IWM_INITIED,
	FSM_STATE_IWM_ACTIVED,
	FSM_STATE_IWM_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

//Global variables
extern FsmStateItem_t FsmIwm[];

//API
extern OPSTAT fsm_iwm_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_iwm_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_iwm_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_iwm_time_out(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);

//Local API
OPSTAT func_iwm_int_init(void);

#endif /* L3APP_SENSORIWM_H_ */
