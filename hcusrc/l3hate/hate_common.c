/*
 * hate_common.c
 *
 *  Created on: 2017年9月4日
 *      Author: ZHANG Jianlin
 */

#include "hate_common.h"

//TEST CASE FUNCTIONS
OPSTAT func_l3hate_common_tc_HATE_TCID_COM_RESTART_ALL_MODULES_snd_s1_restart_all_modues(void)
{
	int task_id = 0;
	msg_struct_com_restart_t snd;

	for (task_id = TASK_ID_MIN + 1; task_id < TASK_ID_MAX; task_id++){
		if ((zHcuVmCtrTab.task[task_id].pnpState == HCU_SYSCFG_TASK_PNP_ON) && (task_id != TASK_ID_SVRCON) && (task_id != TASK_ID_L3HATE)){
			memset(&snd, 0, sizeof(msg_struct_com_restart_t));
			snd.length = sizeof(msg_struct_com_restart_t);
			if (hcu_message_send(MSG_ID_COM_RESTART, task_id, TASK_ID_L3HATE, &snd, snd.length) == FAILURE){
				HcuErrorPrint("L3HATE: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3HATE].taskName, zHcuVmCtrTab.task[task_id].taskName);
				return FAILURE;
			}
		}
	}
	return SUCCESS;
}

OPSTAT func_l3hate_common_tc_HATE_TCID_COM_SETUP_BH_CONNECT_s1_snd_heart_beat_req(void)
{
	CloudDataSendBuf_t data;
	memset(&data, 0, sizeof(CloudDataSendBuf_t));
	strcpy(data.curBuf, "<xml>111</xml>");
	data.curLen = strlen(data.curBuf);

	if (hcu_ethernet_hate_data_send(&data) == FAILURE) return FAILURE;
	return SUCCESS;
}

OPSTAT func_l3hate_common_tc_HATE_TCID_COM_SETUP_BH_CONNECT_r1_rcv_heart_beat_fb(void)
{
	return SUCCESS;
}

//TEST CASE ELEMENT
//头部，测试用途
gTaskL3hateTestCaseSet_t zHcuTc_HATE_TCID_MIN = {
	HATE_TCID_MIN, {{HATE_TCE_CTRL_NONE, NULL}}
};
//尾部，测试用途
gTaskL3hateTestCaseSet_t zHcuTc_HATE_TCID_MAX = {
	HATE_TCID_MAX, {{HATE_TCE_CTRL_NONE, NULL}}
};
gTaskL3hateTestCaseSet_t zHcuTc_HATE_TCID_COM_RESTART_ALL_MODULES = {
	HATE_TCID_COM_RESTART_ALL_MODULES,
	{{HATE_TCE_CTRL_START, NULL},
	{HATE_TCE_CTRL_SND, func_l3hate_common_tc_HATE_TCID_COM_RESTART_ALL_MODULES_snd_s1_restart_all_modues},
	{HATE_TCE_CTRL_CMPL, NULL}}
};

gTaskL3hateTestCaseSet_t zHcuTc_HATE_TCID_COM_SETUP_BH_CONNECT = {
	HATE_TCID_COM_SETUP_BH_CONNECT,
	{{HATE_TCE_CTRL_START, NULL},
	{HATE_TCE_CTRL_SND, func_l3hate_common_tc_HATE_TCID_COM_SETUP_BH_CONNECT_s1_snd_heart_beat_req},
	{HATE_TCE_CTRL_RCV, func_l3hate_common_tc_HATE_TCID_COM_SETUP_BH_CONNECT_r1_rcv_heart_beat_fb},
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



