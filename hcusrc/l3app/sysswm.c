/*
 * sysswm.c
 *
 *  Created on: 2016年5月10日
 *      Author: hitpony
 */

#include "sysswm.h"

#include "../l0service/timer.h"
#include "../l0service/trace.h"
#include "../l1com/l1comdef.h"
#include "../l2frame/cloudvela.h"

/*
** FSM of the SYSSWM
*/
HcuFsmStateItem_t HcuFsmSysswm[] =
{
    //MessageId                 //State                   		 		//Function
	//启始点，固定定义，不要改动, 使用ENTRY/END，意味者MSGID肯定不可能在某个高位区段中；考虑到所有任务共享MsgId，即使分段，也无法实现
	//完全是为了给任务一个初始化的机会，按照状态转移机制，该函数不具备启动的机会，因为任务初始化后自动到FSM_STATE_IDLE
	//如果没有必要进行初始化，可以设置为NULL
	{MSG_ID_ENTRY,       						FSM_STATE_ENTRY,            			fsm_sysswm_task_entry}, //Starting

	//System level initialization, only controlled by HCU-MAIN
    {MSG_ID_COM_INIT,       					FSM_STATE_IDLE,            				fsm_sysswm_init},
    {MSG_ID_COM_INIT_FEEDBACK,					FSM_STATE_IDLE,            				fsm_com_do_nothing},

	//Task level initialization
    {MSG_ID_COM_INIT,       					FSM_STATE_SYSSWM_INITED,            	fsm_sysswm_init},
    {MSG_ID_COM_INIT_FEEDBACK,					FSM_STATE_SYSSWM_INITED,            	fsm_com_do_nothing},

	//ANY state entry
    {MSG_ID_COM_INIT_FEEDBACK,					FSM_STATE_COMMON,          				fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT,       				FSM_STATE_COMMON,          				fsm_com_heart_beat_rcv},
	{MSG_ID_COM_STOP,       					FSM_STATE_COMMON,          				fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT_FB,       			FSM_STATE_COMMON,          				fsm_com_do_nothing},
    {MSG_ID_COM_RESTART,						FSM_STATE_COMMON,            			fsm_sysswm_restart},
	{MSG_ID_COM_TIME_OUT,       				FSM_STATE_COMMON,          				fsm_sysswm_time_out},

    //Task level functions
	{MSG_ID_CLOUDVELA_SYSSWM_INVENTORY_REQ,     	FSM_STATE_SYSSWM_ACTIVED,                fsm_sysswm_cloudvela_inventory_req},
	{MSG_ID_CLOUDVELA_SYSSWM_INVENTORY_CONFIRM, 	FSM_STATE_SYSSWM_ACTIVED,                fsm_sysswm_cloudvela_inventory_confirm},
	{MSG_ID_CLOUDVELA_SYSSWM_SW_PACKAGE_REQ,        FSM_STATE_SYSSWM_ACTIVED,                fsm_sysswm_cloudvela_sw_package_req},
	{MSG_ID_CLOUDVELA_SYSSWM_SW_PACKAGE_CONFIRM,    FSM_STATE_SYSSWM_ACTIVED,                fsm_sysswm_cloudvela_sw_package_confirm},

    //结束点，固定定义，不要改动
    {MSG_ID_END,            	FSM_STATE_END,             				NULL},  //Ending
};

//Global variables

//Task Global variables
gTaskSysswmContext_t gTaskSysswmContext;

//Main Entry
//Input parameter would be useless, but just for similar structure purpose
OPSTAT fsm_sysswm_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//除了对全局变量进行操作之外，尽量不要做其它操作，因为该函数将被主任务/线程调用，不是本任务/线程调用
	//该API就是给本任务一个提早介入的入口，可以帮着做些测试性操作
	if (FsmSetState(TASK_ID_SYSSWM, FSM_STATE_IDLE) == FAILURE){
		HcuErrorPrint("SYSSWM: Error Set FSM State at fsm_sysswm_task_entry\n");}
	return SUCCESS;
}

OPSTAT fsm_sysswm_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;

	if ((src_id > TASK_ID_MIN) &&(src_id < TASK_ID_MAX)){
		//Send back MSG_ID_COM_INIT_FEEDBACK to SVRCON
		msg_struct_com_init_feedback_t snd0;
		memset(&snd0, 0, sizeof(msg_struct_com_init_feedback_t));
		snd0.length = sizeof(msg_struct_com_init_feedback_t);

		//to avoid all task send out the init fb msg at the same time which lead to msgque get stuck
		hcu_usleep(dest_id*HCU_SYSCFG_DURATION_OF_INIT_FB_WAIT_MAX);

		ret = hcu_message_send(MSG_ID_COM_INIT_FEEDBACK, src_id, TASK_ID_SYSSWM, &snd0, snd0.length);
		if (ret == FAILURE){
			HcuErrorPrint("SYSSWM: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_SYSSWM].taskName, zHcuVmCtrTab.task[src_id].taskName);
			return FAILURE;
		}
	}

	//收到初始化消息后，进入初始化状态
	if (FsmSetState(TASK_ID_SYSSWM, FSM_STATE_SYSSWM_INITED) == FAILURE){
		HcuErrorPrint("SYSSWM: Error Set FSM State!\n");
		return FAILURE;
	}

	//初始化硬件接口
	if (func_sysswm_int_init() == FAILURE){
		HcuErrorPrint("SYSSWM: Error initialize interface!\n");
		return FAILURE;
	}

	//Global Variables
	zHcuSysStaPm.taskRunErrCnt[TASK_ID_SYSSWM] = 0;
	memset(&gTaskSysswmContext, 0, sizeof(gTaskSysswmContext_t));

	//启动周期性定时器
	ret = hcu_timer_start(TASK_ID_SYSSWM, TIMER_ID_1S_SYSSWM_PERIOD_WORKING, \
	zHcuSysEngPar.timer.array[TIMER_ID_1S_SYSSWM_PERIOD_WORKING].dur, TIMER_TYPE_PERIOD, TIMER_RESOLUTION_1S);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_SYSSWM]++;
		HcuErrorPrint("SYSSWM: Error start period timer!\n");
		return FAILURE;
	}

	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_SYSSWM, FSM_STATE_SYSSWM_ACTIVED) == FAILURE)
		HCU_ERROR_PRINT_TASK(TASK_ID_SYSSWM, "SYSSWM: Error Set FSM State!\n");
	HCU_DEBUG_PRINT_FAT("SYSSWM: Enter FSM_STATE_SYSSWM_ACTIVED status, Keeping refresh here!\n");

	//返回
	return SUCCESS;
}

OPSTAT fsm_sysswm_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HcuErrorPrint("SYSSWM: Internal error counter reach DEAD level, SW-RESTART soon!\n");
	zHcuSysStaPm.statisCnt.restartCnt++;
	fsm_sysswm_init(0, 0, NULL, 0);
	return SUCCESS;
}

OPSTAT func_sysswm_int_init(void)
{
	return SUCCESS;
}

OPSTAT fsm_sysswm_time_out(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;

	//Receive message and copy to local variable
	msg_struct_com_time_out_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_com_time_out_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_com_time_out_t)))
		HCU_ERROR_PRINT_TASK(TASK_ID_SYSSWM, "SYSSWM: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);

	//钩子在此处，检查zHcuSysStaPm.taskRunErrCnt[TASK_ID_SYSSWM]是否超限
	if (zHcuSysStaPm.taskRunErrCnt[TASK_ID_SYSSWM] > HCU_RUN_ERROR_LEVEL_3_CRITICAL){
		//减少重复RESTART的概率
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_SYSSWM] = zHcuSysStaPm.taskRunErrCnt[TASK_ID_SYSSWM] - HCU_RUN_ERROR_LEVEL_3_CRITICAL;
		msg_struct_com_restart_t snd0;
		memset(&snd0, 0, sizeof(msg_struct_com_restart_t));
		snd0.length = sizeof(msg_struct_com_restart_t);
		if (hcu_message_send(MSG_ID_COM_RESTART, TASK_ID_SYSSWM, TASK_ID_SYSSWM, &snd0, snd0.length) == FAILURE)
			HCU_ERROR_PRINT_TASK(TASK_ID_SYSSWM, "SYSSWM: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_SYSSWM].taskName, zHcuVmCtrTab.task[TASK_ID_SYSSWM].taskName);
	}

	//PERIOD WORKING TIMER
	else if ((rcv.timeId == TIMER_ID_1S_SYSSWM_PERIOD_WORKING) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		ret = func_sysswm_time_out_period_working_scan();
	}

	return ret;
}

OPSTAT fsm_sysswm_cloudvela_inventory_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	return SUCCESS;
}

OPSTAT fsm_sysswm_cloudvela_inventory_confirm(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	return SUCCESS;
}

OPSTAT fsm_sysswm_cloudvela_sw_package_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	return SUCCESS;
}

OPSTAT fsm_sysswm_cloudvela_sw_package_confirm(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	return SUCCESS;
}

OPSTAT func_sysswm_time_out_period_working_scan(void)
{
	//int ret=0;

	//发送数据给后台
	if (FsmGetState(TASK_ID_CLOUDVELA) == FSM_STATE_CLOUDVELA_ONLINE){
		msg_struct_spspm_cloudvela_inventory_report_t snd;
		memset(&snd, 0, sizeof(msg_struct_spspm_cloudvela_inventory_report_t));

		//L2信息
		strncpy(snd.comHead.destUser, zHcuSysEngPar.cloud.svrNameDefault, strlen(zHcuSysEngPar.cloud.svrNameDefault)<\
			sizeof(snd.comHead.destUser)?strlen(zHcuSysEngPar.cloud.svrNameDefault):sizeof(snd.comHead.destUser));
		strncpy(snd.comHead.srcUser, zHcuSysEngPar.cloud.hcuName, strlen(zHcuSysEngPar.cloud.hcuName)<\
				sizeof(snd.comHead.srcUser)?strlen(zHcuSysEngPar.cloud.hcuName):sizeof(snd.comHead.srcUser));
		snd.comHead.timeStamp = time(0);
		snd.comHead.msgType = HUITP_MSG_HUIXML_MSGTYPE_DEVICE_REPORT_ID;
		strcpy(snd.comHead.funcFlag, "0");

		//CONTENT
		snd.baseReport = HUITP_IEID_UNI_COM_REPORT_YES;
		snd.hwType = zHcuSysEngPar.hwBurnId.hwType;
		snd.hwId = zHcuSysEngPar.hwBurnId.hwPemId;
		snd.swRel = zHcuSysEngPar.hwBurnId.swRelId;
		snd.swVer = zHcuSysEngPar.hwBurnId.swVerId;
		snd.upgradeFlag = zHcuSysEngPar.hwBurnId.swUpgradeFlag;
		strcpy(snd.desc, "");
		snd.length = sizeof(msg_struct_spspm_cloudvela_inventory_report_t);
		if (hcu_message_send(MSG_ID_SYSSWM_CLOUDVELA_INVENTORY_REPORT, TASK_ID_CLOUDVELA, TASK_ID_SYSSWM, &snd, snd.length) == FAILURE)
			HCU_ERROR_PRINT_TASK(TASK_ID_SYSSWM, "SYSSWM: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_SYSSWM].taskName, zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName);
	}

	//State no change
	return SUCCESS;
}



