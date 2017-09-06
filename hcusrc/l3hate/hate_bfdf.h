/*
 * hate_bfdf.h
 *
 *  Created on: 2017年9月4日
 *      Author: ZHANG Jianlin
 */

#ifndef L3APP_HATE_BFDF_H_
#define L3APP_HATE_BFDF_H_

#include "../l1com/l1comdef.h"
#include "../l0service/timer.h"
#include "../l0service/trace.h"
#include "../l3hate/l3hate.h"

extern gTaskL3hateTestCaseSet_t zHcuTc_HATE_TCID_BFDF_STARTUP_IND_COMING;
extern gTaskL3hateTestCaseSet_t zHcuTc_HATE_TCID_BFDF_WS_NEW_EVENT_COMING;
extern gTaskL3hateTestCaseSet_t zHcuTc_HATE_TCID_BFDF_CONFIG_ALL_NODES;
extern gTaskL3hateTestCaseSet_t zHcuTc_HATE_TCID_BFDF_START_ALL_NODES;
extern gTaskL3hateTestCaseSet_t zHcuTc_HATE_TCID_BFDF_STOP_ALL_NODES;
extern gTaskL3hateTestCaseSet_t zHcuTc_HATE_TCID_BFDF_SUSPEND_ALL_NODES;

//Local API
OPSTAT func_l3hate_bfdf_tc_snd_startup_ind_incoming(void);


//引用外部函数

#endif /* L3APP_HATE_BFDF_H_ */
