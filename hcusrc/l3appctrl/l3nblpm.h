/*
 * l3nblpm.h
 *
 *  Created on: 2015年11月22日
 *      Author: test
 */

#ifndef L3APP_NBIOT_LPM_H_
#define L3APP_NBIOT_LPM_H_

#include "../l0comvm/vmlayer.h"
#include "../l1com/l1comdef.h"

//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
enum FSM_STATE_L3NBLPM
{
	FSM_STATE_L3NBLPM_INITED = 0x02,
	FSM_STATE_L3NBLPM_ACTIVED,
	FSM_STATE_L3NBLPM_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

//Global variables
extern FsmStateItem_t FsmL3nblpm[];

//API
extern OPSTAT fsm_l3nblpm_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3nblpm_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3nblpm_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3nblpm_time_out(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);

//Local API
OPSTAT func_l3nblpm_int_init(void);



#endif /* L3APP_NBIOT_LPM_H_ */
