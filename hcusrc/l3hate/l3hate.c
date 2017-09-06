/*
 * l3hate.c
 *
 *  Created on: 2017年9月4日
 *      Author: ZHANG Jianlin
 */

#include "l3hate.h"
#include "../l3hate/hate_common.h"

#if (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_AQYCG20_RASBERRY_ID)
	#include "../l3hate/hate_aqyc.h"
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFSC_CBU_ID)
	#include "../l3hate/hate_bfsc.h"
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFDF_CBU_ID)
	#include "../l3hate/hate_bfdf.h"
#else
	#error Un-correct constant definition
#endif

/*
** FSM of the L3HATE
*/
HcuFsmStateItem_t HcuFsmL3hate[] =
{
    //MessageId                 //State                   		 		//Function
	//启始点，固定定义，不要改动, 使用ENTRY/END，意味者MSGID肯定不可能在某个高位区段中；考虑到所有任务共享MsgId，即使分段，也无法实现
	//完全是为了给任务一个初始化的机会，按照状态转移机制，该函数不具备启动的机会，因为任务初始化后自动到FSM_STATE_IDLE
	//如果没有必要进行初始化，可以设置为NULL
	{MSG_ID_ENTRY,       						FSM_STATE_ENTRY,            		fsm_l3hate_task_entry}, //Starting

	//System level initialization, only controlled by HCU-MAIN
    {MSG_ID_COM_INIT,       					FSM_STATE_IDLE,            			fsm_l3hate_init},
    {MSG_ID_COM_INIT_FEEDBACK,					FSM_STATE_IDLE,            			fsm_com_do_nothing},

	//Task level initialization
    {MSG_ID_COM_INIT,       					FSM_STATE_L3HATE_INITED,            fsm_l3hate_init},
    {MSG_ID_COM_INIT_FEEDBACK,					FSM_STATE_L3HATE_INITED,            fsm_com_do_nothing},

	//ANY state entry
    {MSG_ID_COM_INIT_FEEDBACK,					FSM_STATE_COMMON,          			fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT,       				FSM_STATE_COMMON,          			fsm_com_heart_beat_rcv},
	{MSG_ID_COM_STOP,       					FSM_STATE_COMMON,          			fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT_FB,       			FSM_STATE_COMMON,          			fsm_com_do_nothing},
    {MSG_ID_COM_RESTART,						FSM_STATE_COMMON,            		fsm_l3hate_restart},
	{MSG_ID_COM_TIME_OUT,       				FSM_STATE_COMMON,          			fsm_l3hate_time_out},

    //Normal working status
	{MSG_ID_L3HATE_TC_START,					FSM_STATE_L3HATE_ACTIVED,      	  	fsm_l3hate_start_new_tc},
	{MSG_ID_ETH_L3HATE_FRAME_RCV,				FSM_STATE_L3HATE_ACTIVED,      	  	fsm_l3hate_eth_frame_rcv},
	{MSG_ID_SPS_L3HATE_FRAME_RCV,				FSM_STATE_L3HATE_ACTIVED,      	  	fsm_l3hate_sps_frame_rcv},
	{MSG_ID_CAN_L3HATE_FRAME_RCV,				FSM_STATE_L3HATE_ACTIVED,      	  	fsm_l3hate_can_frame_rcv},


    //结束点，固定定义，不要改动
    {MSG_ID_END,            					FSM_STATE_END,             			NULL},  //Ending
};

//System Level Global variables

//Task Level Global variables
gTaskL3hateContext_t gTaskL3hateContext;

//TEST CASE LIBARY
gTaskL3hateTestCaseLib_t zHcuTcLibTable[HATE_TCID_MAX+1] = {
	//START
	{&zHcuTc_HATE_TCID_MIN},
	//FORMAL TEST CASE
	{&zHcuTc_HATE_TCID_COM_RESTART_ALL_MODULES},
	{&zHcuTc_HATE_TCID_COM_SETUP_BH_CONNECT},
	{&zHcuTc_HATE_TCID_COM_DISCON_BH_LINK},
	//分项目测试库函数
#if (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_AQYCG20_RASBERRY_ID)

#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFSC_CBU_ID)

#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFDF_CBU_ID)
	{&zHcuTc_HATE_TCID_BFDF_STARTUP_IND_COMING},
	{&zHcuTc_HATE_TCID_BFDF_WS_NEW_EVENT_COMING},
	{&zHcuTc_HATE_TCID_BFDF_CONFIG_ALL_NODES},
	{&zHcuTc_HATE_TCID_BFDF_START_ALL_NODES},
	{&zHcuTc_HATE_TCID_BFDF_STOP_ALL_NODES},
	{&zHcuTc_HATE_TCID_BFDF_SUSPEND_ALL_NODES},

#else
	#error Un-correct constant definition
#endif
	//END_FLAG
	{&zHcuTc_HATE_TCID_MAX}
};

//Main Entry
/***************************************************************************************************************************
 *
 * 　FSM初始化部分
 *
 ***************************************************************************************************************************/
//Input parameter would be useless, but just for similar structure purpose
OPSTAT fsm_l3hate_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//除了对全局变量进行操作之外，尽量不要做其它操作，因为该函数将被主任务/线程调用，不是本任务/线程调用
	//该API就是给本任务一个提早介入的入口，可以帮着做些测试性操作
	if (FsmSetState(TASK_ID_L3HATE, FSM_STATE_IDLE) == FAILURE){
		HcuErrorPrint("L3HATE: Error Set FSM State at fsm_l3hate_task_entry\n");}
	return SUCCESS;
}

OPSTAT fsm_l3hate_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;

	if ((src_id > TASK_ID_MIN) &&(src_id < TASK_ID_MAX)){
		//Send back MSG_ID_COM_INIT_FEEDBACK to SVRCON
		msg_struct_com_init_feedback_t snd0;
		memset(&snd0, 0, sizeof(msg_struct_com_init_feedback_t));
		snd0.length = sizeof(msg_struct_com_init_feedback_t);

		//to avoid all task send out the init fb msg at the same time which lead to msgque get stuck
		hcu_usleep(dest_id*HCU_SYSCFG_DURATION_OF_INIT_FB_WAIT_MAX);

		ret = hcu_message_send(MSG_ID_COM_INIT_FEEDBACK, src_id, TASK_ID_L3HATE, &snd0, snd0.length);
		if (ret == FAILURE){
			HcuErrorPrint("L3HATE: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3HATE].taskName, zHcuVmCtrTab.task[src_id].taskName);
			return FAILURE;
		}
	}

	//收到初始化消息后，进入初始化状态
	if (FsmSetState(TASK_ID_L3HATE, FSM_STATE_L3HATE_INITED) == FAILURE){
		HcuErrorPrint("L3HATE: Error Set FSM State!\n");
		return FAILURE;
	}

	//初始化硬件接口
	if (func_l3hate_int_init() == FAILURE){
		HcuErrorPrint("L3HATE: Error initialize interface!\n");
		return FAILURE;
	}

	//Global Variables
	zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3HATE] = 0;
	memset(&gTaskL3hateContext, 0, sizeof(gTaskL3hateContext_t));

	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_L3HATE, FSM_STATE_L3HATE_ACTIVED) == FAILURE)
		HCU_ERROR_PRINT_L3HATE("L3HATE: Error Set FSM State!\n");
	HCU_DEBUG_PRINT_FAT("L3HATE: Enter FSM_STATE_L3HATE_ACTIVED status, Keeping refresh here!\n");

	//发送MSG_ID_L3HATE_TC_START给自己：如果没有设置该标示，则不会自动发送消息给自己
#ifdef HATE_TRIGGER_ENABLE
	hcu_sleep(3);
	msg_struct_l3hate_tc_start_t snd1;
	memset(&snd1, 0, sizeof(msg_struct_l3hate_tc_start_t));
	snd1.length = sizeof(msg_struct_l3hate_tc_start_t);
	if (hcu_message_send(MSG_ID_L3HATE_TC_START, TASK_ID_L3HATE, TASK_ID_L3HATE, &snd1, snd1.length) == FAILURE){
		HcuErrorPrint("L3HATE: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3HATE].taskName, zHcuVmCtrTab.task[TASK_ID_L3HATE].taskName);
		return FAILURE;
	}
#endif
	//返回
	return SUCCESS;
}

OPSTAT fsm_l3hate_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HcuErrorPrint("L3HATE: Internal error counter reach DEAD level, SW-RESTART soon!\n");
	zHcuSysStaPm.statisCnt.restartCnt++;
	fsm_l3hate_init(0, 0, NULL, 0);
	return SUCCESS;
}

OPSTAT func_l3hate_int_init(void)
{
	return SUCCESS;
}

//暂时啥都不干，定时到达后，还不明确是否需要支持定时工作
OPSTAT fsm_l3hate_time_out(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret;

	//Receive message and copy to local variable
	msg_struct_com_time_out_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_com_time_out_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_com_time_out_t))){
		HCU_ERROR_PRINT_L3HATE("L3HATE: Receive message error!\n");
	}
	memcpy(&rcv, param_ptr, param_len);

	//定时长时钟进行链路检测的
	if ((rcv.timeId == TIMER_ID_1S_L3HATE_WORK_CTRL_FB) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		gTaskL3hateContext.par = &rcv;
		func_l3hate_test_case_run_engine(HATE_TC_RUN_ENGINE_EVENT_TIME_OUT);
	}

	return SUCCESS;
}

OPSTAT fsm_l3hate_start_new_tc(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//Receive message and copy to local variable
	msg_struct_l3hate_tc_start_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_l3hate_tc_start_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_l3hate_tc_start_t))){
		HCU_ERROR_PRINT_L3HATE("L3HATE: Receive message error!\n");
	}
	memcpy(&rcv, param_ptr, param_len);
	gTaskL3hateContext.par = &rcv;

	func_l3hate_test_case_run_engine(HATE_TC_RUN_ENGINE_EVENT_NEW_START);
	return SUCCESS;
}

OPSTAT fsm_l3hate_eth_frame_rcv(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//Receive message and copy to local variable
	msg_struct_l3hate_eth_frame_rcv_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_l3hate_eth_frame_rcv_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_l3hate_eth_frame_rcv_t))){
		HCU_ERROR_PRINT_L3HATE("L3HATE: Receive message error!\n");
	}
	memcpy(&rcv, param_ptr, param_len);
	gTaskL3hateContext.par = &rcv;

	func_l3hate_test_case_run_engine(HATE_TC_RUN_ENGINE_EVENT_ETH_TRG);
	return SUCCESS;
}

OPSTAT fsm_l3hate_sps_frame_rcv(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//Receive message and copy to local variable
	msg_struct_l3hate_sps_frame_rcv_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_l3hate_sps_frame_rcv_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_l3hate_sps_frame_rcv_t))){
		HCU_ERROR_PRINT_L3HATE("L3HATE: Receive message error!\n");
	}
	memcpy(&rcv, param_ptr, param_len);
	gTaskL3hateContext.par = &rcv;

	func_l3hate_test_case_run_engine(HATE_TC_RUN_ENGINE_EVENT_SPS_TRG);
	return SUCCESS;
}

OPSTAT fsm_l3hate_can_frame_rcv(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//Receive message and copy to local variable
	msg_struct_l3hate_can_frame_rcv_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_l3hate_can_frame_rcv_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_l3hate_can_frame_rcv_t))){
		HCU_ERROR_PRINT_L3HATE("L3HATE: Receive message error!\n");
	}
	memcpy(&rcv, param_ptr, param_len);
	gTaskL3hateContext.par = &rcv;

	func_l3hate_test_case_run_engine(HATE_TC_RUN_ENGINE_EVENT_CAN_TRG);
	return SUCCESS;
}

void func_l3hate_test_case_run_engine(UINT8 event)
{
	return;
}

void hcu_l3hate_test_case_log_file(char *s)
{
	return;
}


