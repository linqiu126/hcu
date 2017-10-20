/*
 * hate_commmon.h
 *
 *  Created on: 2017年9月4日
 *      Author: ZHANG Jianlin
 */

#ifndef L3APP_HATE_COMMON_H_
#define L3APP_HATE_COMMON_H_

#include "../l1com/l1comdef.h"
#include "../l0service/timer.h"
#include "../l0service/trace.h"
#include "../l3hate/l3hate.h"

extern gTaskL3hateTestCaseSet_t zHcuTc_HATE_TCID_MIN;
extern gTaskL3hateTestCaseSet_t zHcuTc_HATE_TCID_MAX;
extern gTaskL3hateTestCaseSet_t zHcuTc_HATE_TCID_COM_RESTART_ALL_MODULES;
extern gTaskL3hateTestCaseSet_t zHcuTc_HATE_TCID_COM_SETUP_BH_CONNECT;
extern gTaskL3hateTestCaseSet_t zHcuTc_HATE_TCID_COM_DISCON_BH_LINK;
extern gTaskL3hateTestCaseSet_t zHcuTc_HATE_TCID_COM_BH_HEAT_BEAT_START;

//Local API
OPSTAT func_l3hate_common_tc_snd_restart_all_modues(void);
OPSTAT func_l3hate_common_tc_HATE_TCID_COM_SETUP_BH_CONNECT_s1_snd_heart_beat_req(void);
OPSTAT func_l3hate_common_tc_HATE_TCID_COM_SETUP_BH_CONNECT_r1_rcv_heart_beat_fb(void);


//引用外部函数

#endif /* L3APP_HATE_COMMON_H_ */
