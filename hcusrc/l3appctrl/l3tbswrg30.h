/*
 * l3tbswrg30.h
 *
 *  Created on: 2015年11月22日
 *      Author: test
 */

#ifndef L3APP_TBSWR_G30_H_
#define L3APP_TBSWR_G30_H_

#include "../l0comvm/vmlayer.h"
#include "../l1com/l1comdef.h"

//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
//#define FSM_STATE_COMMON  	0x02
enum FSM_STATE_L3TBSWRG30
{
	FSM_STATE_L3TBSWRG30_INITED = FSM_STATE_COMMON + 1,
	FSM_STATE_L3TBSWRG30_ACTIVED,
	FSM_STATE_L3TBSWRG30_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

//Global variables
extern HcuFsmStateItem_t HcuFsmL3tbswrg30[];

//API
extern OPSTAT fsm_l3tbswrg30_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3tbswrg30_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3tbswrg30_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3tbswrg30_time_out(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);

//Local API
OPSTAT func_l3tbswrg30_int_init(void);



#endif /* L3APP_TBSWR_G30_H_ */
