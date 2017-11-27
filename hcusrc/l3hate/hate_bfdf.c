/*
 * hate_bfdf.c
 *
 *  Created on: 2017年9月4日
 *      Author: ZHANG Jianlin
 */
#include "hate_bfdf.h"
#include "hate_common.h"

//TEST CASE FUNCTIONS
OPSTAT func_l3hate_bfdf_tc_HATE_TCID_BFDF_STARTUP_IND_COMING_snd_s1_startup_ind(void)
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

//测试目的：以下内容并没有真实意义，就是为了测试网络侧发送过来的消息而已
OPSTAT func_l3hate_bfdf_tc_HATE_TCID_BFDF_WS_NEW_EVENT_COMING_snd_s1_ws_new_event(void)
{
	CloudDataSendBuf_t tmp;
	hcu_ethernet_socket_data_send(&tmp);
	return SUCCESS;
}

OPSTAT func_l3hate_bfdf_tc_HATE_TCID_BFDF_CONFIG_ALL_NODES_snd1_commu_set_config(void)
{
	return SUCCESS;
}

OPSTAT func_l3hate_bfdf_tc_HATE_TCID_BFDF_CONFIG_ALL_NODES_rcv1_can_config_to_node(void)
{
	return SUCCESS;
}

OPSTAT func_l3hate_bfdf_tc_HATE_TCID_BFDF_CONFIG_ALL_NODES_snd2_can_config_resp_ok(void)
{
	return SUCCESS;
}

OPSTAT func_l3hate_bfdf_tc_HATE_TCID_BFDF_CONFIG_ALL_NODES_rcv2_db_config_set(void)
{
	return SUCCESS;
}


//TEST CASE ELEMENT
gTaskL3hateTestCaseSet_t zHcuTc_HATE_TCID_BFDF_STARTUP_IND_COMING = {
	HATE_TCID_BFDF_STARTUP_IND_COMING,
	{{HATE_TCE_CTRL_START, NULL},
	{HATE_TCE_CTRL_SND, func_l3hate_bfdf_tc_HATE_TCID_BFDF_STARTUP_IND_COMING_snd_s1_startup_ind},
	{HATE_TCE_CTRL_CMPL, NULL}}
};

gTaskL3hateTestCaseSet_t zHcuTc_HATE_TCID_BFDF_WS_NEW_EVENT_COMING = {
	HATE_TCID_BFDF_WS_NEW_EVENT_COMING,
	{{HATE_TCE_CTRL_START, NULL},
	{HATE_TCE_CTRL_SND, func_l3hate_bfdf_tc_HATE_TCID_BFDF_WS_NEW_EVENT_COMING_snd_s1_ws_new_event},
	{HATE_TCE_CTRL_CMPL, NULL}}
};

gTaskL3hateTestCaseSet_t zHcuTc_HATE_TCID_BFDF_CONFIG_ALL_NODES = {
	HATE_TCID_BFDF_CONFIG_ALL_NODES,
	{{HATE_TCE_CTRL_START, NULL},
	{HATE_TCE_CTRL_SND, func_l3hate_bfdf_tc_HATE_TCID_BFDF_CONFIG_ALL_NODES_snd1_commu_set_config},
	{HATE_TCE_CTRL_RCV, func_l3hate_bfdf_tc_HATE_TCID_BFDF_CONFIG_ALL_NODES_rcv1_can_config_to_node},
	{HATE_TCE_CTRL_SND, func_l3hate_bfdf_tc_HATE_TCID_BFDF_CONFIG_ALL_NODES_snd2_can_config_resp_ok},
	{HATE_TCE_CTRL_RCV, func_l3hate_bfdf_tc_HATE_TCID_BFDF_CONFIG_ALL_NODES_rcv2_db_config_set},
	{HATE_TCE_CTRL_CMPL, NULL}}
};

gTaskL3hateTestCaseSet_t zHcuTc_HATE_TCID_BFDF_START_ALL_NODES = {
	HATE_TCID_BFDF_START_ALL_NODES,
	{{HATE_TCE_CTRL_START, NULL},
	{HATE_TCE_CTRL_CMPL, NULL}}
};

gTaskL3hateTestCaseSet_t zHcuTc_HATE_TCID_BFDF_STOP_ALL_NODES = {
	HATE_TCID_BFDF_STOP_ALL_NODES,
	{{HATE_TCE_CTRL_START, NULL},
	{HATE_TCE_CTRL_CMPL, NULL}}
};

gTaskL3hateTestCaseSet_t zHcuTc_HATE_TCID_BFDF_SUSPEND_ALL_NODES = {
	HATE_TCID_BFDF_SUSPEND_ALL_NODES,
	{{HATE_TCE_CTRL_START, NULL},
	{HATE_TCE_CTRL_CMPL, NULL}}
};


