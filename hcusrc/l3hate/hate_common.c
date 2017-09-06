/*
 * hate_common.c
 *
 *  Created on: 2017年9月4日
 *      Author: ZHANG Jianlin
 */

#include "hate_common.h"

//TEST CASE FUNCTIONS
OPSTAT func_l3hate_common_tc_snd_restart_all_modues(void)
{
	return SUCCESS;
}


//TEST CASE ELEMENT
gTaskL3hateTestCaseSet_t zHcuTc_HATE_TCID_MIN = {
	HATE_TCID_MIN, {{HATE_TCE_CTRL_NONE, NULL}}
};
gTaskL3hateTestCaseSet_t zHcuTc_HATE_TCID_MAX = {
	HATE_TCID_MAX, {{HATE_TCE_CTRL_NONE, NULL}}
};
gTaskL3hateTestCaseSet_t zHcuTc_HATE_TCID_COM_RESTART_ALL_MODULES = {
	HATE_TCID_COM_RESTART_ALL_MODULES,
	{{HATE_TCE_CTRL_START, NULL},
	{HATE_TCE_CTRL_SND, func_l3hate_common_tc_snd_restart_all_modues},
	{HATE_TCE_CTRL_CMPL, NULL}}
};

gTaskL3hateTestCaseSet_t zHcuTc_HATE_TCID_COM_SETUP_BH_CONNECT = {
	HATE_TCID_COM_SETUP_BH_CONNECT,
	{{HATE_TCE_CTRL_START, NULL},
	{HATE_TCE_CTRL_CMPL, NULL}}
};

gTaskL3hateTestCaseSet_t zHcuTc_HATE_TCID_COM_DISCON_BH_LINK = {
	HATE_TCID_COM_DISCON_BH_LINK,
	{{HATE_TCE_CTRL_START, NULL},
	{HATE_TCE_CTRL_CMPL, NULL}}
};

gTaskL3hateTestCaseSet_t zHcuTc_HATE_TCID_COM_BH_HEAT_BEAT_START = {
	HATE_TCID_COM_BH_HEAT_BEAT_START,
	{{HATE_TCE_CTRL_START, NULL},
	{HATE_TCE_CTRL_CMPL, NULL}}
};



