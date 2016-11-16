/*
 * l3bfsc.c
 *
 *  Created on: 2015年11月22日
 *      Author: test
 */


#include "l3bfsc.h"

#include "../l0service/timer.h"
#include "../l0service/trace.h"
#include "../l1com/hwinv.h"
#include "../l1com/l1comdef.h"
#include "../l2frame/cloudvela.h"


/*
** FSM of the L3BFSC
*/
FsmStateItem_t FsmL3bfsc[] =
{
    //MessageId                 //State                   		 		//Function
	//启始点，固定定义，不要改动, 使用ENTRY/END，意味者MSGID肯定不可能在某个高位区段中；考虑到所有任务共享MsgId，即使分段，也无法实现
	//完全是为了给任务一个初始化的机会，按照状态转移机制，该函数不具备启动的机会，因为任务初始化后自动到FSM_STATE_IDLE
	//如果没有必要进行初始化，可以设置为NULL
	{MSG_ID_ENTRY,       						FSM_STATE_ENTRY,            			fsm_l3bfsc_task_entry}, //Starting

	//System level initialization, only controlled by HCU-MAIN
    {MSG_ID_COM_INIT,       					FSM_STATE_IDLE,            				fsm_l3bfsc_init},
    {MSG_ID_COM_RESTART,						FSM_STATE_IDLE,            				fsm_l3bfsc_restart},
    {MSG_ID_COM_INIT_FEEDBACK,					FSM_STATE_IDLE,            				fsm_com_do_nothing},

	//Task level initialization
    {MSG_ID_COM_INIT,       					FSM_STATE_L3BFSC_INITIED,            		fsm_l3bfsc_init},
    {MSG_ID_COM_RESTART,						FSM_STATE_L3BFSC_INITIED,            		fsm_l3bfsc_restart},
    {MSG_ID_COM_INIT_FEEDBACK,					FSM_STATE_L3BFSC_INITIED,            		fsm_com_do_nothing},

	//Normal working status
    {MSG_ID_COM_RESTART,        				FSM_STATE_L3BFSC_ACTIVED,            		fsm_l3bfsc_restart},
    {MSG_ID_COM_INIT_FEEDBACK,					FSM_STATE_L3BFSC_ACTIVED,            		fsm_com_do_nothing},
	{MSG_ID_COM_TIME_OUT,       				FSM_STATE_L3BFSC_ACTIVED,          			fsm_l3bfsc_time_out},

	//人工配置状态
    {MSG_ID_COM_RESTART,        				FSM_STATE_L3BFSC_OPR_CFG,            		fsm_l3bfsc_restart},
	{MSG_ID_COM_TIME_OUT,       				FSM_STATE_L3BFSC_OPR_CFG,          			fsm_l3bfsc_time_out},
	{MSG_ID_UICOMM_L3BFSC_CMD_REQ,       		FSM_STATE_L3BFSC_OPR_CFG,          			fsm_l3bfsc_uicomm_cmd_req},
	{MSG_ID_CLOUDVELA_L3BFSC_CMD_REQ,       	FSM_STATE_L3BFSC_OPR_CFG,          			fsm_l3bfsc_cloudvela_cmd_req},

	//进料组合态
    {MSG_ID_COM_RESTART,        				FSM_STATE_L3BFSC_OOS_SCAN,            		fsm_l3bfsc_restart},
	{MSG_ID_COM_TIME_OUT,       				FSM_STATE_L3BFSC_OOS_SCAN,          		fsm_l3bfsc_time_out},
	{MSG_ID_CAN_L3BFSC_CMD_RESP,       			FSM_STATE_L3BFSC_OOS_SCAN,          		fsm_l3bfsc_canitf_cmd_resp},
	{MSG_ID_CAN_L3BFSC_WS_REPORT,       		FSM_STATE_L3BFSC_OOS_SCAN,          		fsm_l3bfsc_canitf_ws_report},

	//出料流程态
    {MSG_ID_COM_RESTART,        				FSM_STATE_L3BFSC_OOS_TTT,            		fsm_l3bfsc_restart},
	{MSG_ID_COM_TIME_OUT,       				FSM_STATE_L3BFSC_OOS_TTT,          			fsm_l3bfsc_time_out},
	{MSG_ID_L3BFSC_CAN_WS_COMB_OUT_FB,       	FSM_STATE_L3BFSC_OOS_TTT,          			fsm_l3bfsc_can_ws_comb_out_fb},

	//放弃物料态
    {MSG_ID_COM_RESTART,        				FSM_STATE_L3BFSC_OOS_TGU,            		fsm_l3bfsc_restart},
	{MSG_ID_COM_TIME_OUT,       				FSM_STATE_L3BFSC_OOS_TGU,          			fsm_l3bfsc_time_out},
	{MSG_ID_L3BFSC_CAN_WS_GIVE_UP_FB,       	FSM_STATE_L3BFSC_OOS_TGU,          			fsm_l3bfsc_can_ws_give_up_fb},

    //结束点，固定定义，不要改动
    {MSG_ID_END,            	FSM_STATE_END,             				NULL},  //Ending
};

//Global variables
extern HcuSysEngParTablet_t zHcuSysEngPar; //全局工程参数控制表

//Main Entry
//Input parameter would be useless, but just for similar structure purpose
OPSTAT fsm_l3bfsc_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//除了对全局变量进行操作之外，尽量不要做其它操作，因为该函数将被主任务/线程调用，不是本任务/线程调用
	//该API就是给本任务一个提早介入的入口，可以帮着做些测试性操作
	if (FsmSetState(TASK_ID_L3BFSC, FSM_STATE_IDLE) == FAILURE){
		HcuErrorPrint("L3BFSC: Error Set FSM State at fsm_l3bfsc_task_entry\n");}
	return SUCCESS;
}

OPSTAT fsm_l3bfsc_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;

	if ((src_id > TASK_ID_MIN) &&(src_id < TASK_ID_MAX)){
		//Send back MSG_ID_COM_INIT_FEEDBACK to SVRCON
		msg_struct_com_init_feedback_t snd0;
		memset(&snd0, 0, sizeof(msg_struct_com_init_feedback_t));
		snd0.length = sizeof(msg_struct_com_init_feedback_t);

		//to avoid all task send out the init fb msg at the same time which lead to msgque get stuck
		hcu_usleep(dest_id*HCU_DURATION_OF_INIT_FB_WAIT_MAX);

		ret = hcu_message_send(MSG_ID_COM_INIT_FEEDBACK, src_id, TASK_ID_L3BFSC, &snd0, snd0.length);
		if (ret == FAILURE){
			HcuErrorPrint("L3BFSC: Send message error, TASK [%s] to TASK[%s]!\n", zHcuTaskNameList[TASK_ID_L3BFSC], zHcuTaskNameList[src_id]);
			return FAILURE;
		}
	}

	//收到初始化消息后，进入初始化状态
	if (FsmSetState(TASK_ID_L3BFSC, FSM_STATE_L3BFSC_INITED) == FAILURE){
		HcuErrorPrint("L3BFSC: Error Set FSM State!\n");
		return FAILURE;
	}

	//初始化硬件接口
	if (func_l3bfsc_int_init() == FAILURE){
		HcuErrorPrint("L3BFSC: Error initialize interface!\n");
		return FAILURE;
	}

	//Global Variables
	zHcuRunErrCnt[TASK_ID_L3BFSC] = 0;

	//启动周期性定时器
	ret = hcu_timer_start(TASK_ID_L3BFSC, TIMER_ID_1S_L3BFSC_PERIOD_READ, HCU_L3BFSC_TIMER_DURATION_PERIOD_READ, TIMER_TYPE_PERIOD, TIMER_RESOLUTION_1S);
	if (ret == FAILURE){
		zHcuRunErrCnt[TASK_ID_L3BFSC]++;
		HcuErrorPrint("L3BFSC: Error start period timer!\n");
		return FAILURE;
	}

	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_L3BFSC, FSM_STATE_L3BFSC_ACTIVED) == FAILURE){
		zHcuRunErrCnt[TASK_ID_L3BFSC]++;
		HcuErrorPrint("L3BFSC: Error Set FSM State!\n");
		return FAILURE;
	}
	if ((zHcuSysEngPar.debugMode & HCU_TRACE_DEBUG_FAT_ON) != FALSE){
		HcuDebugPrint("L3BFSC: Enter FSM_STATE_L3BFSC_ACTIVED status, Keeping refresh here!\n");
	}

	//返回
	return SUCCESS;
}

OPSTAT fsm_l3bfsc_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HcuErrorPrint("L3BFSC: Internal error counter reach DEAD level, SW-RESTART soon!\n");
	zHcuGlobalCounter.restartCnt++;
	fsm_l3bfsc_init(0, 0, NULL, 0);
	return SUCCESS;
}

OPSTAT func_l3bfsc_int_init(void)
{
	return SUCCESS;
}

//暂时啥都不干，定时到达后，还不明确是否需要支持定时工作
OPSTAT fsm_l3bfsc_time_out(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret;

	//Receive message and copy to local variable
	msg_struct_com_time_out_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_com_time_out_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_com_time_out_t))){
		HcuErrorPrint("L3BFSC: Receive message error!\n");
		zHcuRunErrCnt[TASK_ID_L3BFSC]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//钩子在此处，检查zHcuRunErrCnt[TASK_ID_L3BFSC]是否超限
	if (zHcuRunErrCnt[TASK_ID_L3BFSC] > HCU_RUN_ERROR_LEVEL_2_MAJOR){
		//减少重复RESTART的概率
		zHcuRunErrCnt[TASK_ID_L3BFSC] = zHcuRunErrCnt[TASK_ID_L3BFSC] - HCU_RUN_ERROR_LEVEL_2_MAJOR;
		msg_struct_com_restart_t snd0;
		memset(&snd0, 0, sizeof(msg_struct_com_restart_t));
		snd0.length = sizeof(msg_struct_com_restart_t);
		ret = hcu_message_send(MSG_ID_COM_RESTART, TASK_ID_L3BFSC, TASK_ID_L3BFSC, &snd0, snd0.length);
		if (ret == FAILURE){
			zHcuRunErrCnt[TASK_ID_L3BFSC]++;
			HcuErrorPrint("L3BFSC: Send message error, TASK [%s] to TASK[%s]!\n", zHcuTaskNameList[TASK_ID_L3BFSC], zHcuTaskNameList[TASK_ID_L3BFSC]);
			return FAILURE;
		}
	}

	//Period time out received
	if ((rcv.timeId == TIMER_ID_1S_L3BFSC_PERIOD_READ) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		//保护周期读数的优先级，强制抢占状态，并简化问题
		if (FsmGetState(TASK_ID_L3BFSC) != FSM_STATE_L3BFSC_ACTIVED){
			ret = FsmSetState(TASK_ID_L3BFSC, FSM_STATE_L3BFSC_ACTIVED);
			if (ret == FAILURE){
				zHcuRunErrCnt[TASK_ID_L3BFSC]++;
				HcuErrorPrint("L3BFSC: Error Set FSM State!\n");
				return FAILURE;
			}//FsmSetState
		}

		//Do nothing

	}

	return SUCCESS;
}

OPSTAT fsm_l3bfsc_canitf_cmd_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret=0;
/*	HcuDiscDataSampleStorageArray_t record;

	msg_struct_modbus_pm25_data_report_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_modbus_pm25_data_report_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_modbus_pm25_data_report_t))){
		HcuErrorPrint("L3BFSC: Receive message error!\n");
		zHcuRunErrCnt[TASK_ID_L3BFSC]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);*/

	//检查收到的数据的正确性，然后再继续往CLOUD发送，仍然以平淡消息的格式，让L2_CLOUDVELA进行编码

	//停止定时器


	//返回
	return SUCCESS;
}

OPSTAT fsm_l3bfsc_canitf_ws_report(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret=0;
/*	HcuDiscDataSampleStorageArray_t record;

	msg_struct_modbus_pm25_data_report_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_modbus_pm25_data_report_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_modbus_pm25_data_report_t))){
		HcuErrorPrint("L3BFSC: Receive message error!\n");
		zHcuRunErrCnt[TASK_ID_L3BFSC]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);*/

	//检查收到的数据的正确性，然后再继续往CLOUD发送，仍然以平淡消息的格式，让L2_CLOUDVELA进行编码

	//停止定时器


	//返回
	return SUCCESS;
}

OPSTAT fsm_l3bfsc_can_ws_comb_out_fb(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret=0;
/*	HcuDiscDataSampleStorageArray_t record;

	msg_struct_modbus_pm25_data_report_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_modbus_pm25_data_report_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_modbus_pm25_data_report_t))){
		HcuErrorPrint("L3BFSC: Receive message error!\n");
		zHcuRunErrCnt[TASK_ID_L3BFSC]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);*/

	//检查收到的数据的正确性，然后再继续往CLOUD发送，仍然以平淡消息的格式，让L2_CLOUDVELA进行编码

	//停止定时器


	//返回
	return SUCCESS;
}

OPSTAT fsm_l3bfsc_can_ws_give_up_fb(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret=0;
/*	HcuDiscDataSampleStorageArray_t record;

	msg_struct_modbus_pm25_data_report_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_modbus_pm25_data_report_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_modbus_pm25_data_report_t))){
		HcuErrorPrint("L3BFSC: Receive message error!\n");
		zHcuRunErrCnt[TASK_ID_L3BFSC]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);*/

	//检查收到的数据的正确性，然后再继续往CLOUD发送，仍然以平淡消息的格式，让L2_CLOUDVELA进行编码

	//停止定时器


	//返回
	return SUCCESS;
}

//从BFSCUICOMM来的消息和命令
OPSTAT fsm_l3bfsc_uicomm_cmd_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret=0;
/*	HcuDiscDataSampleStorageArray_t record;

	msg_struct_modbus_pm25_data_report_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_modbus_pm25_data_report_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_modbus_pm25_data_report_t))){
		HcuErrorPrint("L3BFSC: Receive message error!\n");
		zHcuRunErrCnt[TASK_ID_L3BFSC]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);*/

	//检查收到的数据的正确性，然后再继续往CLOUD发送，仍然以平淡消息的格式，让L2_CLOUDVELA进行编码

	//停止定时器


	//返回
	return SUCCESS;
}


OPSTAT fsm_l3bfsc_cloudvela_cmd_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret=0;
/*	HcuDiscDataSampleStorageArray_t record;

	msg_struct_modbus_pm25_data_report_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_modbus_pm25_data_report_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_modbus_pm25_data_report_t))){
		HcuErrorPrint("L3BFSC: Receive message error!\n");
		zHcuRunErrCnt[TASK_ID_L3BFSC]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);*/

	//检查收到的数据的正确性，然后再继续往CLOUD发送，仍然以平淡消息的格式，让L2_CLOUDVELA进行编码

	//停止定时器


	//返回
	return SUCCESS;
}