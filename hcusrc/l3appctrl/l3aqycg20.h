/*
 * l3aqycg20.h
 *
 *  Created on: 2015年11月22日
 *      Author: test
 */

#ifndef L3APP_AQYC_G20_H_
#define L3APP_AQYC_G20_H_

#include "../l0comvm/vmlayer.h"
#include "../l1com/l1comdef.h"

//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
enum FSM_STATE_L3AQYCG20
{
	FSM_STATE_L3AQYCG20_INITED = 0x02,
	FSM_STATE_L3AQYCG20_ACTIVED,
	FSM_STATE_L3AQYCG20_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

//Global variables
extern FsmStateItem_t FsmL3aqycg20[];

//API
extern OPSTAT fsm_l3aqycg20_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3aqycg20_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3aqycg20_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_l3aqycg20_time_out(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);

//Local API
OPSTAT func_l3aqycg20_int_init(void);



#endif /* L3APP_AQYC_G20_H_ */
