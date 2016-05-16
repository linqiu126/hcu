/*
 * gpio.h
 *
 *  Created on: 2016年5月10日
 *      Author: test
 */

#ifndef L1HWOPT_GPIO_H_
#define L1HWOPT_GPIO_H_

#include "../l0comvm/vmlayer.h"

//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
enum FSM_STATE_GPIO
{
	FSM_STATE_GPIO_INITED = 0x02,
	FSM_STATE_GPIO_RECEIVED,  //所有人机命令在此状态下直接送达各个线程任务模块
	FSM_STATE_GPIO_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

//Global variables
extern FsmStateItem_t FsmGpio[];

//API
extern OPSTAT fsm_gpio_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_gpio_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_gpio_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);

//Local API
OPSTAT func_gpio_int_init(void);



/*
 * Old inherit apigpio.h
 * Function API prototype for Serial Port
 */
/*extern void print_n(const char *name, int last);
extern char *pin_a(int con, int dat, int pud);
extern char *pin_f(int con, int dat, int pud);
extern char *pin(const struct port *p, int num);
void set_a(const struct port *p, int num, int c, int d, int r);
void set_f(const struct port *p, int num, int c, int d, int r);
void set_h(const struct port *p, int num, int c, int d, int r);
void set_pin(const struct port *p, int num, int c, int d, int r);
extern void dump_all(void);
extern void __attribute__((noreturn)) usage(const char *name);
extern void port_op(const char *name, const char *op);*/
extern UINT32 GpioInit();
extern void GpioPortOp(char *op);


#endif /* L1HWOPT_GPIO_H_ */


