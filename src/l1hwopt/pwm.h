/*
 * pwm.h
 *
 *  Created on: 2016年5月10日
 *      Author: hitpony
 */

#ifndef L1HWOPT_PWM_H_
#define L1HWOPT_PWM_H_

#include "../l0comvm/vmlayer.h"

//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
enum FSM_STATE_PWM
{
	FSM_STATE_PWM_INITED = 0x02,
	FSM_STATE_PWM_RECEIVED,  //所有人机命令在此状态下直接送达各个线程任务模块
	FSM_STATE_PWM_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

//Global variables
extern FsmStateItem_t FsmPwm[];

#define RPI_PWM_SENSOR_WRITE_GAP 60 //in second
#define RPI_PWM_WRITE_REPEAT_TIMES 10

//API
extern OPSTAT fsm_pwm_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_pwm_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_pwm_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);

//Local API
OPSTAT func_pwm_int_init(void);

OPSTAT func_pwm_write_data_motor_sg90(void);
OPSTAT func_pwm_write_data_led_2pin(void);

#endif /* L1HWOPT_PWM_H_ */
