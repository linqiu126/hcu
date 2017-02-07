/*
 * i2cbuslibra.h
 *
 *  Created on: 2016年5月14日
 *      Author: hitpony
 */

#ifndef L2FRAME_I2CBUSLIBRA_H_
#define L2FRAME_I2CBUSLIBRA_H_

#include "../l0comvm/vmlayer.h"

//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
enum FSM_STATE_I2CBUSLIBRA
{
	FSM_STATE_I2CBUSLIBRA_INITED = 0x02,
	FSM_STATE_I2CBUSLIBRA_INITIED,
	FSM_STATE_I2CBUSLIBRA_ACTIVED,
	FSM_STATE_I2CBUSLIBRA_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

//Global variables
extern HcuFsmStateItem_t HcuFsmI2cbuslibra[];

//API
extern OPSTAT fsm_i2cbuslibra_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_i2cbuslibra_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_i2cbuslibra_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);

//Local API
OPSTAT func_i2cbuslibra_int_init(void);

#endif /* L2FRAME_I2CBUSLIBRA_H_ */
