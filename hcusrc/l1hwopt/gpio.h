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
//#define FSM_STATE_COMMON  	0x02
enum FSM_STATE_GPIO
{
	FSM_STATE_GPIO_INITED = FSM_STATE_COMMON + 1,
	FSM_STATE_GPIO_ACTIVIED,  //所有人机命令在此状态下直接送达各个线程任务模块
	FSM_STATE_GPIO_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

//Global variables
extern HcuFsmStateItem_t HcuFsmGpio[];

//Local definition
#define RPI_GPIO_PIN_VCC 1
#define RPI_GPIO_PIN_GND 9
#define RPI_GPIO_PIN_DHT11_DATA 0  //Pin#11
#define RPI_GPIO_PIN_MQ135_DATA 5  //Pin#18  //应CZ要求，管脚改变，从GPIO#5改为GPIO#6，管脚从pin#18改为pin#22
#define RPI_GPIO_PIN_MQ3ALCO_DATA 26  //Pin#32
#define RPI_GPIO_PIN_ZP01VOC_DATA_A 27  //Pin#36
#define RPI_GPIO_PIN_ZP01VOC_DATA_B 28  //Pin#38

#define RPI_GPIO_PIN_PWM_CONTROL 0  //Pin#15 //粉尘仪电机控制测试，暂时只开和关

#define RPI_GPIO_HIGH_TIME 32
#define RPI_GPIO_SENSOR_READ_GAP 60 //in second
#define RPI_GPIO_READ_REPEAT_TIMES 10

#define RPI_GPIO_PIN_PWM_CONTROL_LOW 5*60
#define RPI_GPIO_PIN_PWM_CONTROL_HIGH 60

//API
extern OPSTAT fsm_gpio_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_gpio_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_gpio_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);

//Local API
OPSTAT func_gpio_int_init(void);
UINT8 func_gpio_readSensorDht11Data(UINT32 pin);
OPSTAT func_gpio_read_data_dht11(void);
OPSTAT func_gpio_read_data_mq135(void);
OPSTAT func_gpio_read_data_mq3alco(void);
OPSTAT func_gpio_read_data_zp01voc(void);

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


