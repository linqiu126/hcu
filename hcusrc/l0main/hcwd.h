/*
 * hcu.h
 *
 *  Created on: 2015年11月2日
 *      Author: test
 */

#ifndef _HCWD_H_
#define _HCWD_H_

#include "../l0comvm/comtype.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#ifdef TARGET_RASPBERRY_PI3B
	#include <wiringPi.h>
	//GPIO
	//Serial port
	#include <wiringSerial.h>
	#include <termios.h>
	//I2C
	#include <wiringPiI2C.h>
	//SPI
	#include <wiringPiSPI.h>
	//Software driving tone
	#include <softTone.h>
	//PWM
	#include <softPwm.h>
#endif


/*
 *   GPIO21配置为WDI，在程序启动后，3秒一个周期去喂狗；GPIO20为掉电检测输入脚，低电平表示主电失电；GPIO16为切断备电控制输出脚，低电平关闭备电。
 *   HCWD服务进程喂狗GPIO21，同时监控GPIO20管脚。低电平则执行软关机。
 *
 *   具体WiringPi的管脚映射，参见《设计要点》文档
 */



//CONST Variables
#define RPI_GPIO_PIN_EXT_WATCHDOG_FEED 24  						//BCM-Pin#35 (WDI) => WiPi#24
#define RPI_GPIO_PIN_EXT_WATCHDOG_SHUT_DOWN_TRIGGER 23  		//BCM-Pin#33 (PWDOWN) => WiPi#23
//#define RPI_GPIO_PIN_EXT_WATCHDOG_SHUT_DOWN_TRIGGER 25  		//BCM-Pin#37 (PWDOWN) => WiPi#25
#define RPI_GPIO_READ_REPEAT_TIMES 10

//Local APIs
void func_hcwd_feed_ext_watchdog(void);
OPSTAT func_gpio_read_data_ext_watchdog(void);
void func_hcwd_read_ext_trigger_and_soft_shut_down(void);
bool func_hcwd_read_ext_soft_shut_trigger_value(void);



//bug fix by shanchun
#define RPI_GPIO_HIGH_TIME 3
UINT8 func_gpio_read_pin_data_from_ext_watchdog(UINT32 pin);


#endif /* _HCWD_H_ */
