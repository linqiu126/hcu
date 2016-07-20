/*
 * nbiotqg376.h
 *
 *  Created on: 2016年7月20日
 *      Author: hitpony
 */

#ifndef L2FRAME_NBIOTQG376_H_
#define L2FRAME_NBIOTQG376_H_

#include "../l0comvm/vmlayer.h"

//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
enum FSM_STATE_NBIOTQG376
{
	FSM_STATE_NBIOTQG376_INITED = 0x02,
	FSM_STATE_NBIOTQG376_INITIED,
	FSM_STATE_NBIOTQG376_ACTIVED,
	FSM_STATE_NBIOTQG376_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

//Global variables
extern FsmStateItem_t FsmNbiotqg376[];

//API
extern OPSTAT fsm_nbiotqg376_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_nbiotqg376_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_nbiotqg376_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);

//Local API
OPSTAT func_nbiotqg376_int_init(void);


#endif /* L2FRAME_NBIOTQG376_H_ */
