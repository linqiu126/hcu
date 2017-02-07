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
	FSM_STATE_PWM_ACTIVIED,  //所有人机命令在此状态下直接送达各个线程任务模块
	FSM_STATE_PWM_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

//Global variables
extern HcuFsmStateItem_t HcuFsmPwm[];

//Local variables
#define RPI_PWM_PIN_VCC 2  //5V
#define RPI_PWM_PIN_GND 6
#define RPI_PWM_PIN_OUTPUT_SG90 1  //Pin#12   //用于SG90马达的测试
#define RPI_PWM_PIN_OUTPUT_LED2PIN 29  //Pin#40  //用于LED的测试
//有两种解释
//第一种解释：0表示全关，100表示全开，所以范围就是100
//第二种解释：1表示100us,200=20ms，正好等于50Hz/一个SG90的工作周期
#define RPI_PWM_WORKING_RANGE 100  //，另外一种情形用200，然后模拟成7-28， 7+x*21/255，比较丑陋

#define RPI_PWM_SENSOR_WRITE_GAP 10 //in second
#define RPI_PWM_WRITE_REPEAT_TIMES 3

//API
extern OPSTAT fsm_pwm_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_pwm_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_pwm_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);

//Local API
OPSTAT func_pwm_int_init(void);

OPSTAT func_pwm_write_data_motor_sg90(void);
OPSTAT func_pwm_write_data_led_2pin(void);

#endif /* L1HWOPT_PWM_H_ */
