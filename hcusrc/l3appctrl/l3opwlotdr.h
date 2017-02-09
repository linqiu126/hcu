/*
 * l3opwlotdr.h
 *
 *  Created on: 2015年11月22日
 *      Author: test
 */

#ifndef L3APP_OPWL_OTDR_H_
#define L3APP_OPWL_OTDR_H_

#include "../l0comvm/vmlayer.h"
#include "../l1com/l1comdef.h"

//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
//#define FSM_STATE_COMMON  	0x02
enum FSM_STATE_L3OPWLOTDR
{
	FSM_STATE_L3OPWLOTDR_INITED = FSM_STATE_COMMON + 1,
	FSM_STATE_L3OPWLOTDR_ACTIVED,
	FSM_STATE_L3OPWLOTDR_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

//Global variables
extern HcuFsmStateItem_t HcuFsmL3opwlotdr[];

//API
extern OPSTAT fsm_l3opwlotdr_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3opwlotdr_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3opwlotdr_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3opwlotdr_time_out(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);

//Local API
OPSTAT func_l3opwlotdr_int_init(void);



#endif /* L3APP_OPWL_OTDR_H_ */
