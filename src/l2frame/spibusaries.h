/*
 * spibusaries.h
 *
 *  Created on: 2016年5月14日
 *      Author: hitpony
 */

#ifndef L2FRAME_SPIBUSARIES_H_
#define L2FRAME_SPIBUSARIES_H_

#include "../l0comvm/vmlayer.h"

//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
enum FSM_STATE_SPIBUSARIES
{
	FSM_STATE_SPIBUSARIES_INITED = 0x02,
	FSM_STATE_SPIBUSARIES_INITIED,
	FSM_STATE_SPIBUSARIES_ACTIVED,
	FSM_STATE_SPIBUSARIES_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

//Global variables
extern FsmStateItem_t FsmSpibusaries[];

//API
extern OPSTAT fsm_spibusaries_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_spibusaries_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_spibusaries_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);

//Local API
OPSTAT func_spibusaries_int_init(void);

#endif /* L2FRAME_SPIBUSARIES_H_ */
