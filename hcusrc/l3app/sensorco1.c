/*
 * sensorco1.c
 *
 *  Created on: 2016年5月10日
 *      Author: hitpony
 */

#include "sensorco1.h"

#include "../l0service/timer.h"
#include "../l0service/trace.h"

/*
** FSM of the CO1
*/
HcuFsmStateItem_t HcuFsmCo1[] =
{
    //MessageId                 //State                   		 		//Function
	//启始点，固定定义，不要改动, 使用ENTRY/END，意味者MSGID肯定不可能在某个高位区段中；考虑到所有任务共享MsgId，即使分段，也无法实现
	//完全是为了给任务一个初始化的机会，按照状态转移机制，该函数不具备启动的机会，因为任务初始化后自动到FSM_STATE_IDLE
	//如果没有必要进行初始化，可以设置为NULL
	{MSG_ID_ENTRY,       		FSM_STATE_ENTRY,            			fsm_co1_task_entry}, //Starting

	//System level initialization, only controlled by HCU-MAIN
    {MSG_ID_COM_INIT,       	FSM_STATE_IDLE,            				fsm_co1_init},
    {MSG_ID_COM_INIT_FEEDBACK,	FSM_STATE_IDLE,            				fsm_com_do_nothing},

	//Task level initialization
    {MSG_ID_COM_INIT,       	FSM_STATE_CO1_INITED,            	fsm_co1_init},
    {MSG_ID_COM_INIT_FEEDBACK,	FSM_STATE_CO1_INITED,            	fsm_com_do_nothing},

	//ANY state entry
    {MSG_ID_COM_INIT_FEEDBACK,				FSM_STATE_COMMON,          				fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT,       			FSM_STATE_COMMON,          				fsm_com_heart_beat_rcv},
	{MSG_ID_COM_STOP,       				FSM_STATE_COMMON,          				fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT_FB,       		FSM_STATE_COMMON,          				fsm_com_do_nothing},
    {MSG_ID_COM_RESTART,					FSM_STATE_COMMON,            			fsm_co1_restart},
	{MSG_ID_COM_TIME_OUT,       			FSM_STATE_COMMON,          				fsm_co1_time_out},

    //Task level initialization
	{MSG_ID_CLOUDVELA_CO1_DATA_REQ,		FSM_STATE_CO1_ACTIVED,      	  	fsm_co1_cloudvela_data_req},
	{MSG_ID_CLOUDVELA_CO1_DATA_CONFIRM,	FSM_STATE_CO1_ACTIVED,      	  	fsm_co1_cloudvela_data_confirm},
	{MSG_ID_L3AQYC_EXG_CTRL_REQ,		FSM_STATE_CO1_ACTIVED,      	  	fsm_co1_l3aqyc_exg_ctrl_req},


    //结束点，固定定义，不要改动
    {MSG_ID_END,            	FSM_STATE_END,             				NULL},  //Ending
};

//Global variables

//Task Global variables
gTaskCo1Context_t gTaskCo1Context;


//Main Entry
//Input parameter would be useless, but just for similar structure purpose
OPSTAT fsm_co1_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//除了对全局变量进行操作之外，尽量不要做其它操作，因为该函数将被主任务/线程调用，不是本任务/线程调用
	//该API就是给本任务一个提早介入的入口，可以帮着做些测试性操作
	FsmSetState(TASK_ID_CO1, FSM_STATE_IDLE);
	return SUCCESS;
}

OPSTAT fsm_co1_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;

	if ((src_id > TASK_ID_MIN) &&(src_id < TASK_ID_MAX)){
		//Send back MSG_ID_COM_INIT_FEEDBACK to SVRCON
		msg_struct_com_init_feedback_t snd0;
		memset(&snd0, 0, sizeof(msg_struct_com_init_feedback_t));
		snd0.length = sizeof(msg_struct_com_init_feedback_t);

		//to avoid all task send out the init fb msg at the same time which lead to msgque get stuck
		hcu_usleep(dest_id*HCU_SYSCFG_DURATION_OF_INIT_FB_WAIT_MAX);

		ret = hcu_message_send(MSG_ID_COM_INIT_FEEDBACK, src_id, TASK_ID_CO1, &snd0, snd0.length);
		if (ret == FAILURE){
			HcuErrorPrint("CO1: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CO1].taskName, zHcuVmCtrTab.task[src_id].taskName);
			return FAILURE;
		}
	}

	//收到初始化消息后，进入初始化状态
	FsmSetState(TASK_ID_CO1, FSM_STATE_CO1_INITED);

	//初始化硬件接口
	if (func_co1_int_init() == FAILURE){
		HcuErrorPrint("CO1: Error initialize interface!\n");
		return FAILURE;
	}

	//Global Variables
	zHcuSysStaPm.taskRunErrCnt[TASK_ID_CO1] = 0;
	memset(&gTaskCo1Context, 0, sizeof(gTaskCo1Context_t));

	//启动周期性定时器
	hcu_timer_start(TASK_ID_CO1, HCU_TIMERID_WITH_DUR(TIMER_ID_1S_CO1_PERIOD_READ), TIMER_TYPE_PERIOD, TIMER_RESOLUTION_1S);

	//设置状态机到目标状态
	FsmSetState(TASK_ID_CO1, FSM_STATE_CO1_ACTIVED);
	HCU_DEBUG_PRINT_FAT("CO1: Enter FSM_STATE_CO1_ACTIVED status, Keeping refresh here!\n");
	/*

	//进入阻塞式接收数据状态，然后继续发送
	while(1){
		//接收数据
		int dataLen=0;
		if (dataLen > 1){
			//发送数据给HSMMP
			msg_struct_co1_hsmmp_data_rx_t snd;
			memset(&snd, 0, sizeof(msg_struct_co1_hsmmp_data_rx_t));
			snd.length = sizeof(msg_struct_co1_hsmmp_data_rx_t);
			ret = hcu_message_send(MSG_ID_CO1_HSMMP_DATA_RX, TASK_ID_HSMMP, TASK_ID_CO1, &snd, snd.length);
			if (ret == FAILURE){
				zHcuRunErrCnt[TASK_ID_CO1]++;
				HcuErrorPrint("CO1: Send message error, TASK [%s] to TASK[%s]!\n", zHcuTaskInfo[TASK_ID_CO1].taskName, zHcuTaskInfo[TASK_ID_HSMMP].taskName);
				return FAILURE;
			}
		}

		hcu_sleep(5);
	}
	*/

	return SUCCESS;
}

OPSTAT fsm_co1_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HcuErrorPrint("CO1: Internal error counter reach DEAD level, SW-RESTART soon!\n");
	zHcuSysStaPm.statisCnt.restartCnt++;
	fsm_co1_init(0, 0, NULL, 0);
	return SUCCESS;
}

OPSTAT func_co1_int_init(void)
{
	return SUCCESS;
}

OPSTAT fsm_co1_time_out(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	return SUCCESS;
}

//收到来自CLOUD和后台云的命令，从而重新配置本地控制信息
OPSTAT fsm_co1_cloudvela_data_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	return SUCCESS;
}

//收到来自CLOUD和后台云的命令，从而重新配置本地控制信息
OPSTAT fsm_co1_cloudvela_data_confirm(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	return SUCCESS;
}

OPSTAT fsm_co1_l3aqyc_exg_ctrl_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	return SUCCESS;
}


