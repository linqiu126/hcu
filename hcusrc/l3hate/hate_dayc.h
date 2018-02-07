/*
 * l3hate_dayc.h
 *
 *  Created on: 2017年9月4日
 *      Author: ZHANG Jianlin
 */

#ifndef L3APP_HATE_DAYC_H_
#define L3APP_HATE_DAYC_H_

#include "../l1com/l1comdef.h"
#include "../l0service/timer.h"
#include "../l0service/trace.h"
#include "../l3hate/l3hate.h"

//GLOBAL VARIABLES
extern gTaskL3hateTestCaseSet_t zHcuTc_HATE_TCID_DAYC_SET_PM25_WORK_CYCLE;

//Local API
OPSTAT func_l3hate_dayc_tc_snd_set_pm25_work_cycle(void);
OPSTAT func_l3hate_dayc_tc_rcv_set_pm25_work_cycle_result(void);

//引用外部函数

#endif /* L3APP_HATE_DAYC_H_ */
