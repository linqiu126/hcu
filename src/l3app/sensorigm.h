/*
 * sensorigm.h
 *
 *  Created on: 2016年7月20日
 *      Author: hitpony
 */

#ifndef L3APP_SENSORIGM_H_
#define L3APP_SENSORIGM_H_

#include "../l0comvm/vmlayer.h"

//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
enum FSM_STATE_IGM
{
	FSM_STATE_IGM_INITED = 0x02,
	FSM_STATE_IGM_INITIED,
	FSM_STATE_IGM_ACTIVED,
	FSM_STATE_IGM_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

//Global variables
extern FsmStateItem_t FsmIgm[];

//API
extern OPSTAT fsm_igm_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_igm_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_igm_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_igm_time_out(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);

//Local API
OPSTAT func_igm_int_init(void);

#endif /* L3APP_SENSORIGM_H_ */