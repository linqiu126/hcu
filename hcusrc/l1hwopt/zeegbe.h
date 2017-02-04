/*
 * zeegbe.h
 *
 *  Created on: 2015年11月25日
 *      Author: test
 */

#ifndef L1HWOPT_ZEEGBE_H_
#define L1HWOPT_ZEEGBE_H_

#include "../l0comvm/vmlayer.h"

//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
enum FSM_STATE_ZEEGBE
{
	FSM_STATE_ZEEGBE_INITED = 0x02,
	FSM_STATE_ZEEGBE_ACTIVED,  //所有人机命令在此状态下直接送达各个线程任务模块
	FSM_STATE_ZEEGBE_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

//Global variables
extern FsmStateItem_t HcuFsmZeegbe[];

//API
extern OPSTAT fsm_zeegbe_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_zeegbe_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_zeegbe_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);

//Local API
OPSTAT func_zeegbe_int_init(void);

#endif /* L1HWOPT_ZEEGBE_H_ */