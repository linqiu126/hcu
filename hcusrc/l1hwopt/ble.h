/*
 * ble.h
 *
 *  Created on: 2015年11月22日
 *      Author: test
 */

#ifndef L1HWOPT_BLE_H_
#define L1HWOPT_BLE_H_

#include "../l0comvm/vmlayer.h"

//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
//#define FSM_STATE_COMMON  	0x02
enum FSM_STATE_BLE
{
	FSM_STATE_BLE_INITED = FSM_STATE_COMMON + 1,
	FSM_STATE_BLE_ACTIVIED,  //所有人机命令在此状态下直接送达各个线程任务模块
	FSM_STATE_BLE_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

//Global variables
extern HcuFsmStateItem_t HcuFsmBle[];

//API
extern OPSTAT fsm_ble_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_ble_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_ble_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);

//Local API
OPSTAT func_ble_int_init(void);

#endif /* L1HWOPT_BLE_H_ */
