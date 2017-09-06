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
	//链路逻辑处理：待定


    //结束点，固定定义，不要改动
    {MSG_ID_END,            					FSM_STATE_END,             			NULL},  //Ending
};

//System Level Global variables

//Task Level Global variables
gTaskL3hateContext_t gTaskL3hateContext;

//TEST CASE LIBARY
//测试库集合，是所有可能存在或者不用的总库表单
gTaskL3hateTestCaseLib_t zHcuHateTcLibTable[HATE_TCID_MAX+1] = {
	//START
	{&zHcuTc_HATE_TCID_MIN},
	//FORMAL TEST CASE
	{&zHcuTc_HATE_TCID_COM_RESTART_ALL_MODULES},
	{&zHcuTc_HATE_TCID_COM_SETUP_BH_CONNECT},
	{&zHcuTc_HATE_TCID_COM_DISCON_BH_LINK},
	{&zHcuTc_HATE_TCID_COM_BH_HEAT_BEAT_START},
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

//各个分项目中真实安排的测试内容
UINT32 zHcuHateTcCampaign[] = {
	//START
	HATE_TCID_MIN,
#if (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_AQYCG20_RASBERRY_ID)

#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFSC_CBU_ID)

#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFDF_CBU_ID)
	HATE_TCID_COM_RESTART_ALL_MODULES,
	HATE_TCID_COM_SETUP_BH_CONNECT,
	HATE_TCID_COM_DISCON_BH_LINK,
	//HATE_TCID_COM_BH_HEAT_BEAT_START,
	//HATE_TCID_BFDF_STARTUP_IND_COMING,
	HATE_TCID_BFDF_WS_NEW_EVENT_COMING,
	HATE_TCID_BFDF_CONFIG_ALL_NODES,
	HATE_TCID_BFDF_START_ALL_NODES,
	HATE_TCID_BFDF_STOP_ALL_NODES,
	HATE_TCID_BFDF_SUSPEND_ALL_NODES,
#else
	#error Un-correct constant definition
#endif
	//END_FLAG
	HATE_TCID_MAX,
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

//主要运行的测试引擎
void func_l3hate_test_case_run_engine(UINT8 event)
{
	UINT32 tcLibId = 0;

	//停止定时器
	HATE_STOP_WORK_TIMER;

	//分类处理
	if (event == HATE_TC_RUN_ENGINE_EVENT_NEW_START){
		gTaskL3hateContext.tcIndex++;
		HCU_DEBUG_PRINT_FAT("L3HATE: New test case start, TCID in Campaign = %d\n", gTaskL3hateContext.tcIndex);
		func_l3hate_reset_all_modules();
		hcu_sleep(3);
		if (zHcuHateTcCampaign[gTaskL3hateContext.tcIndex] == HATE_TCID_MIN) gTaskL3hateContext.tcIndex++;
		if (zHcuHateTcCampaign[gTaskL3hateContext.tcIndex] >= HATE_TCID_MAX){
			hcu_l3hate_test_case_log_file("FINISHED. TCID in TcCampaign reach MAX.");
			return;
		}
		//搜索测试行号
		tcLibId = func_l3hate_search_test_case_number_in_lib(zHcuHateTcCampaign[gTaskL3hateContext.tcIndex]);
		if ((tcLibId == HATE_TCID_MIN) || (tcLibId >= HATE_TCID_MAX)){
			hcu_l3hate_test_case_log_file("FINISHED. TCID in TcLib reach MAX.");
			return;
		}
		//执行第一步
		gTaskL3hateContext.stepId = 0;
		func_l3hate_test_case_run_execution_new(tcLibId);
		return;
	}
	else if ((event == HATE_TC_RUN_ENGINE_EVENT_ETH_TRG) || (event == HATE_TC_RUN_ENGINE_EVENT_SPS_TRG) || (event == HATE_TC_RUN_ENGINE_EVENT_CAN_TRG)){
		tcLibId = func_l3hate_search_test_case_number_in_lib(zHcuHateTcCampaign[gTaskL3hateContext.tcIndex]);
		if ((tcLibId == HATE_TCID_MIN) || (tcLibId >= HATE_TCID_MAX)){
			hcu_l3hate_test_case_log_file("FINISHED. TCID in TcLib reach MAX.");
			return;
		}
		func_l3hate_test_case_run_execution_rcv(tcLibId);
	}
	else if (event == HATE_TC_RUN_ENGINE_EVENT_TIME_OUT){
		tcLibId = func_l3hate_search_test_case_number_in_lib(zHcuHateTcCampaign[gTaskL3hateContext.tcIndex]);
		if ((tcLibId == HATE_TCID_MIN) || (tcLibId >= HATE_TCID_MAX)){
			hcu_l3hate_test_case_log_file("FINISHED. TCID in TcLib reach MAX.");
			return;
		}
		hcu_l3hate_test_case_log_file("FINISHED. Time out to wait feedback.");
		func_l3hate_test_case_execute_error_process(tcLibId);
	}
	else{
		hcu_l3hate_test_case_log_file("FINISHED. TCID Exception.");
		return;
	}

	//返回
	return;
}

OPSTAT hcu_l3hate_test_case_log_file(char *s)
{
	char smtp[HATE_LOG_RECORD_MAX];
	FILE *fp;

	sprintf(smtp, "HATE: %s %s %s\r\n", __DATE__, __TIME__, s);
	if((fp=fopen(HATE_LOG_FILE_NAME, "at+"))== NULL){
		HCU_ERROR_PRINT_L3HATE("L3HATE: Open file %s Error!\n", HATE_LOG_FILE_NAME);
	}
	fseek(fp, 0L, SEEK_END);
	if (fwrite(smtp, 1, strlen(smtp), fp) != strlen(smtp)){
		fclose(fp);
		HCU_ERROR_PRINT_L3HATE("L3HATE: Write file %s Error!\n", HATE_LOG_FILE_NAME);
	}

	fclose(fp);
	return SUCCESS;
}

void func_l3hate_reset_all_modules(void)
{
	int task_id = 0;
	msg_struct_com_restart_t snd;

	for (task_id = TASK_ID_MIN + 1; task_id < TASK_ID_MAX; task_id++){
		if ((zHcuVmCtrTab.task[task_id].pnpState == HCU_SYSCFG_TASK_PNP_ON) && (task_id != TASK_ID_SVRCON) && (task_id != TASK_ID_TIMER) && (task_id != TASK_ID_L3HATE)){
			memset(&snd, 0, sizeof(msg_struct_com_restart_t));
			snd.length = sizeof(msg_struct_com_restart_t);
			if (hcu_message_send(MSG_ID_COM_RESTART, task_id, TASK_ID_L3HATE, &snd, snd.length) == FAILURE){
				HcuErrorPrint("L3HATE: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3HATE].taskName, zHcuVmCtrTab.task[task_id].taskName);
				return;
			}
		}
	}

	//返回
	return;
}

//搜索zHcuTcLibTable中的实际标号
//输入参数是zHcuHateTcCampaign[]中搜索的结果
//返回的i指得是zHcuHateTcLibTable[]中的index
UINT32 func_l3hate_search_test_case_number_in_lib(int tcCgnIndex)
{
	int i = 0;

	//为了加快效率，先探测是否属于全排列
	if (zHcuHateTcLibTable[tcCgnIndex].tcSet->tcId == tcCgnIndex) return tcCgnIndex;
	//不行再遍历
	while(zHcuHateTcLibTable[i].tcSet->tcId != HATE_TCID_MAX){
		if(zHcuHateTcLibTable[i].tcSet->tcId == tcCgnIndex)
			return i;
		i++;
	}
	return i; //最大行号，就是HATE_TCID_MAX
}

void func_l3hate_test_case_execute_error_process(UINT32 tcLibId)
{
	char smtp[100];

	sprintf(smtp, "#TEST NOK#, TC INDEX in Campaign = %d, TC ID in Lib = %d, TC STEP = %d.", gTaskL3hateContext.tcIndex, tcLibId, gTaskL3hateContext.stepId);
	hcu_l3hate_test_case_log_file(smtp);

	//发送给自己
	msg_struct_l3hate_tc_start_t snd1;
	memset(&snd1, 0, sizeof(msg_struct_l3hate_tc_start_t));
	snd1.length = sizeof(msg_struct_l3hate_tc_start_t);
	hcu_sleep(1);
	if (hcu_message_send(MSG_ID_L3HATE_TC_START, TASK_ID_L3HATE, TASK_ID_L3HATE, &snd1, snd1.length) == FAILURE){
		HcuErrorPrint("L3HATE: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3HATE].taskName, zHcuVmCtrTab.task[TASK_ID_L3HATE].taskName);
		return;
	}

	return;
}

void func_l3hate_test_case_execute_success_process(UINT32 tcLibId)
{
	char smtp[100];

	sprintf(smtp, "#TEST OK#, TC INDEX in Campaign = %d, TC ID in Lib = %d, TC STEP = %d.", gTaskL3hateContext.tcIndex, tcLibId, gTaskL3hateContext.stepId);
	hcu_l3hate_test_case_log_file(smtp);

	//发送给自己
	msg_struct_l3hate_tc_start_t snd1;
	memset(&snd1, 0, sizeof(msg_struct_l3hate_tc_start_t));
	snd1.length = sizeof(msg_struct_l3hate_tc_start_t);
	hcu_sleep(1);
	if (hcu_message_send(MSG_ID_L3HATE_TC_START, TASK_ID_L3HATE, TASK_ID_L3HATE, &snd1, snd1.length) == FAILURE){
		HcuErrorPrint("L3HATE: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3HATE].taskName, zHcuVmCtrTab.task[TASK_ID_L3HATE].taskName);
		return;
	}

	return;
}

void func_l3hate_test_case_run_execution_new(UINT32 tcLibId)
{
	if (zHcuHateTcLibTable[tcLibId].tcSet->tce[gTaskL3hateContext.stepId].ctrl != HATE_TCE_CTRL_START){
		hcu_l3hate_test_case_log_file("FINISHED. Illeagal TEST CASE Set.");
		func_l3hate_test_case_execute_error_process(tcLibId);
		return;
	}
	gTaskL3hateContext.stepId++;
	while(zHcuHateTcLibTable[tcLibId].tcSet->tce[gTaskL3hateContext.stepId].ctrl != HATE_TCE_CTRL_CMPL){
		if (zHcuHateTcLibTable[tcLibId].tcSet->tce[gTaskL3hateContext.stepId].ctrl == HATE_TCE_CTRL_SND){
			if (zHcuHateTcLibTable[tcLibId].tcSet->tce[gTaskL3hateContext.stepId].hateFunc() == FAILURE){
				func_l3hate_test_case_execute_error_process(tcLibId);
				return;
			}
			else{
				gTaskL3hateContext.stepId++;
				continue;
			}
		}//SND
		else if (zHcuHateTcLibTable[tcLibId].tcSet->tce[gTaskL3hateContext.stepId].ctrl == HATE_TCE_CTRL_RCV){
			HATE_START_WORK_TIMER;
			return;
		}//RCV
		else{
			func_l3hate_test_case_execute_error_process(tcLibId);
			return;
		}
	}
	//正常运行结束了
	func_l3hate_test_case_execute_success_process(tcLibId);
	return;
}

void func_l3hate_test_case_run_execution_rcv(UINT32 tcLibId)
{
	//先执行接收功能
	if (zHcuHateTcLibTable[tcLibId].tcSet->tce[gTaskL3hateContext.stepId].hateFunc() == FAILURE){
		func_l3hate_test_case_execute_error_process(tcLibId);
		return;
	}

	//继续执行
	gTaskL3hateContext.stepId++;
	if (zHcuHateTcLibTable[tcLibId].tcSet->tce[gTaskL3hateContext.stepId].ctrl != HATE_TCE_CTRL_START){
		hcu_l3hate_test_case_log_file("FINISHED. Illeagal TEST CASE Set.");
		func_l3hate_test_case_execute_error_process(tcLibId);
		return;
	}
	while(zHcuHateTcLibTable[tcLibId].tcSet->tce[gTaskL3hateContext.stepId].ctrl != HATE_TCE_CTRL_CMPL){
		if (zHcuHateTcLibTable[tcLibId].tcSet->tce[gTaskL3hateContext.stepId].ctrl == HATE_TCE_CTRL_SND){
			if (zHcuHateTcLibTable[tcLibId].tcSet->tce[gTaskL3hateContext.stepId].hateFunc() == FAILURE){
				func_l3hate_test_case_execute_error_process(tcLibId);
				return;
			}
			else{
				gTaskL3hateContext.stepId++;
				continue;
			}
		}//SND
		else if (zHcuHateTcLibTable[tcLibId].tcSet->tce[gTaskL3hateContext.stepId].ctrl == HATE_TCE_CTRL_RCV){
			HATE_START_WORK_TIMER;
			return;
		}//RCV
		else{
			func_l3hate_test_case_execute_error_process(tcLibId);
			return;
		}
	}
	//正常运行结束了
	func_l3hate_test_case_execute_success_process(tcLibId);
	return;
}



