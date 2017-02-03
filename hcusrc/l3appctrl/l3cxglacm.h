/*
 * l3cxglacm.h
 *
 *  Created on: 2015年11月22日
 *      Author: test
 */

#ifndef L3APP_CX_GLACM_H_
#define L3APP_CX_GLACM_H_

#include "../l0comvm/vmlayer.h"
#include "../l1com/l1comdef.h"

//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
enum FSM_STATE_L3CXGLACM
{
	FSM_STATE_L3CXGLACM_INITED = 0x02,
	FSM_STATE_L3CXGLACM_ACTIVED,
	FSM_STATE_L3CXGLACM_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

//Global variables
extern FsmStateItem_t HcuFsmL3cxglacm[];

//API
extern OPSTAT fsm_l3cxglacm_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3cxglacm_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3cxglacm_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3cxglacm_time_out(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);

//Local API
OPSTAT func_l3cxglacm_int_init(void);



#endif /* L3APP_CX_GLACM_H_ */
