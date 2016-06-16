/*
 * led.h
 *
 *  Created on: 2016年6月16日
 *      Author: hitpony
 */

#ifndef L1HWOPT_LED_H_
#define L1HWOPT_LED_H_


#include "../l0comvm/vmlayer.h"

//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
enum FSM_STATE_LED
{
	FSM_STATE_LED_INITED = 0x02,
	FSM_STATE_LED_RECEIVED,  //所有人机命令在此状态下直接送达各个线程任务模块
	FSM_STATE_LED_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

//Global variables
extern FsmStateItem_t FsmLed[];

//API
extern OPSTAT fsm_led_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_led_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_led_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);

//Local API
OPSTAT func_led_int_init(void);

#endif /* L1HWOPT_LED_H_ */

