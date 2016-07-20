/*
 * sensoripm.h
 *
 *  Created on: 2016年7月20日
 *      Author: hitpony
 */

#ifndef L3APP_SENSORIPM_H_
#define L3APP_SENSORIPM_H_

#include "../l0comvm/vmlayer.h"

//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
enum FSM_STATE_IPM
{
	FSM_STATE_IPM_INITED = 0x02,
	FSM_STATE_IPM_INITIED,
	FSM_STATE_IPM_ACTIVED,
	FSM_STATE_IPM_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

//Global variables
extern FsmStateItem_t FsmIpm[];

//API
extern OPSTAT fsm_ipm_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_ipm_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_ipm_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_ipm_time_out(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);

//Local API
OPSTAT func_ipm_int_init(void);

#endif /* L3APP_SENSORIPM_H_ */
