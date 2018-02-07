/*
 * l3hate_dayc.c
 *
 *  Created on: 2017年9月4日
 *      Author: ZHANG Jianlin
 */

#include "hate_dayc.h"
#include "hate_common.h"

//TEST CASE ELEMENT
OPSTAT func_l3hate_dayc_tc_snd_set_pm25_work_cycle(void)
{
	UINT16 bufSize = 100;
	UINT8 data[bufSize];
	UINT16 actualLen = 10;
	memset(data, 0, bufSize);
	data[0] = 0x11;
	data[1] = 0x22;
	data[2] = 0x33;
	data[4] = 0x44;

	if (hcu_sps_hate_data_send(data, actualLen) == FAILURE) return FAILURE;
	return SUCCESS;
}

OPSTAT func_l3hate_dayc_tc_rcv_set_pm25_work_cycle_result(void)
{

	return SUCCESS;
}


//TEST CASE FUNCTIONS
gTaskL3hateTestCaseSet_t zHcuTc_HATE_TCID_DAYC_SET_PM25_WORK_CYCLE = {
	HATE_TCID_DAYC_SET_PM25_WORK_CYCLE,
	{{HATE_TCE_CTRL_START, NULL},
	{HATE_TCE_CTRL_SND, func_l3hate_dayc_tc_snd_set_pm25_work_cycle},
	{HATE_TCE_CTRL_RCV, func_l3hate_dayc_tc_rcv_set_pm25_work_cycle_result},
	{HATE_TCE_CTRL_CMPL, NULL}}
};

