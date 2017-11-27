/*
 * hate_bfhs.c
 *
 *  Created on: 2017年9月4日
 *      Author: ZHANG Jianlin
 */
#include "hate_bfhs.h"
#include "hate_common.h"

//TEST CASE ELEMENT
OPSTAT func_l3hate_bfhs_tc_HATE_TCID_BFHS_STARTUP_IND_COMING_snd_s1_startup_ind(void)
{
	UINT16 bufSize = 100;
	UINT8 data[bufSize];
	UINT16 actualLen = 10;
	memset(data, 0, bufSize);
	data[0] = 0x11;
	data[1] = 0x22;
	data[2] = 0x33;
	data[4] = 0x44;

	if (hcu_canalpha_hate_send_data(data, actualLen, 1) == FAILURE) return FAILURE;
	return SUCCESS;
}

OPSTAT func_l3hate_bfhs_tc_HATE_TCID_BFHS_WS_NEW_EVENT_COMING_snd_s1_ws_new_event(void)
{
	return SUCCESS;
}

OPSTAT func_l3hate_bfhs_tc_HATE_TCID_BFHS_CONFIG_ALL_NODES_snd1_commu_set_config(void)
{
	return SUCCESS;
}

OPSTAT func_l3hate_bfhs_tc_HATE_TCID_BFHS_CONFIG_ALL_NODES_rcv1_can_config_to_node(void)
{
	return SUCCESS;
}

OPSTAT func_l3hate_bfhs_tc_HATE_TCID_BFHS_CONFIG_ALL_NODES_snd2_can_config_resp_ok(void)
{
	return SUCCESS;
}

OPSTAT func_l3hate_bfhs_tc_HATE_TCID_BFHS_CONFIG_ALL_NODES_rcv2_db_config_set(void)
{
	return SUCCESS;
}

//TEST CASE FUNCTIONS
gTaskL3hateTestCaseSet_t zHcuTc_HATE_TCID_BFHS_STARTUP_IND_COMING = {
	HATE_TCID_BFHS_STARTUP_IND_COMING,
	{{HATE_TCE_CTRL_START, NULL},
	{HATE_TCE_CTRL_SND, func_l3hate_bfhs_tc_HATE_TCID_BFHS_STARTUP_IND_COMING_snd_s1_startup_ind},
	{HATE_TCE_CTRL_CMPL, NULL}}
};

gTaskL3hateTestCaseSet_t zHcuTc_HATE_TCID_BFHS_WS_NEW_EVENT_COMING = {
	HATE_TCID_BFHS_WS_NEW_EVENT_COMING,
	{{HATE_TCE_CTRL_START, NULL},
	{HATE_TCE_CTRL_SND, func_l3hate_bfhs_tc_HATE_TCID_BFHS_WS_NEW_EVENT_COMING_snd_s1_ws_new_event},
	{HATE_TCE_CTRL_CMPL, NULL}}
};

gTaskL3hateTestCaseSet_t zHcuTc_HATE_TCID_BFHS_CONFIG_ALL_NODES = {
	HATE_TCID_BFHS_CONFIG_ALL_NODES,
	{{HATE_TCE_CTRL_START, NULL},
	{HATE_TCE_CTRL_SND, func_l3hate_bfhs_tc_HATE_TCID_BFHS_CONFIG_ALL_NODES_snd1_commu_set_config},
	{HATE_TCE_CTRL_RCV, func_l3hate_bfhs_tc_HATE_TCID_BFHS_CONFIG_ALL_NODES_rcv1_can_config_to_node},
	{HATE_TCE_CTRL_SND, func_l3hate_bfhs_tc_HATE_TCID_BFHS_CONFIG_ALL_NODES_snd2_can_config_resp_ok},
	{HATE_TCE_CTRL_RCV, func_l3hate_bfhs_tc_HATE_TCID_BFHS_CONFIG_ALL_NODES_rcv2_db_config_set},
	{HATE_TCE_CTRL_CMPL, NULL}}
};


