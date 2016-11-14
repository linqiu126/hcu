/*
 * switch.c
 *
 *  Created on: 2016年5月10日
 *      Author: hitpony
 */

#include "../l1hwopt/switch.h"
#include "../l0service/trace.h"

/*
** FSM of the SWITCH
*/
FsmStateItem_t FsmSwitch[] =
{
    //MessageId                 //State                   		 		//Function
	//启始点，固定定义，不要改动, 使用ENTRY/END，意味者MSGID肯定不可能在某个高位区段中；考虑到所有任务共享MsgId，即使分段，也无法实现
	//完全是为了给任务一个初始化的机会，按照状态转移机制，该函数不具备启动的机会，因为任务初始化后自动到FSM_STATE_IDLE
	//如果没有必要进行初始化，可以设置为NULL
	{MSG_ID_ENTRY,       		FSM_STATE_ENTRY,            			fsm_switch_task_entry}, //Starting

	//System level initialization, only controlled by HCU-MAIN
    {MSG_ID_COM_INIT,       	FSM_STATE_IDLE,            				fsm_switch_init},
    {MSG_ID_COM_RESTART,		FSM_STATE_IDLE,            				fsm_switch_restart},
    {MSG_ID_COM_INIT_FEEDBACK,	FSM_STATE_IDLE,            				fsm_com_do_nothing},

    //Task level initialization
    {MSG_ID_COM_RESTART,        FSM_STATE_SWITCH_RECEIVED,            		fsm_switch_restart},
    {MSG_ID_COM_INIT_FEEDBACK,	FSM_STATE_SWITCH_RECEIVED,            		fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT,     FSM_STATE_SWITCH_RECEIVED,       			fsm_com_heart_beat_rcv},
	{MSG_ID_COM_HEART_BEAT_FB,  FSM_STATE_SWITCH_RECEIVED,       			fsm_com_do_nothing},

    //结束点，固定定义，不要改动
    {MSG_ID_END,            	FSM_STATE_END,             				NULL},  //Ending
};

//Global variables
extern HcuSysEngParTablet_t zHcuSysEngPar; //全局工程参数控制表

//Main Entry
//Input parameter would be useless, but just for similar structure purpose
OPSTAT fsm_switch_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//除了对全局变量进行操作之外，尽量不要做其它操作，因为该函数将被主任务/线程调用，不是本任务/线程调用
	//该API就是给本任务一个提早介入的入口，可以帮着做些测试性操作
	if (FsmSetState(TASK_ID_SWITCH, FSM_STATE_IDLE) == FAILURE){
		HcuErrorPrint("SWITCH: Error Set FSM State at fsm_switch_task_entry\n");}
	return SUCCESS;
}

OPSTAT fsm_switch_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;

	if ((src_id > TASK_ID_MIN) &&(src_id < TASK_ID_MAX)){
		//Send back MSG_ID_COM_INIT_FEEDBACK to SVRCON
		msg_struct_com_init_feedback_t snd0;
		memset(&snd0, 0, sizeof(msg_struct_com_init_feedback_t));
		snd0.length = sizeof(msg_struct_com_init_feedback_t);

		//to avoid all task send out the init fb msg at the same time which lead to msgque get stuck
		hcu_usleep(dest_id*HCU_DURATION_OF_INIT_FB_WAIT_MAX);

		ret = hcu_message_send(MSG_ID_COM_INIT_FEEDBACK, src_id, TASK_ID_SWITCH, &snd0, snd0.length);
		if (ret == FAILURE){
			HcuErrorPrint("SWITCH: Send message error, TASK [%s] to TASK[%s]!\n", zHcuTaskNameList[TASK_ID_SWITCH], zHcuTaskNameList[src_id]);
			return FAILURE;
		}
	}

	//收到初始化消息后，进入初始化状态
	if (FsmSetState(TASK_ID_SWITCH, FSM_STATE_SWITCH_INITED) == FAILURE){
		HcuErrorPrint("SWITCH: Error Set FSM State!\n");
		return FAILURE;
	}

	//初始化硬件接口
	if (func_switch_int_init() == FAILURE){
		HcuErrorPrint("SWITCH: Error initialize interface!\n");
		return FAILURE;
	}

	//Global Variables
	zHcuRunErrCnt[TASK_ID_SWITCH] = 0;

	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_SWITCH, FSM_STATE_SWITCH_RECEIVED) == FAILURE){
		zHcuRunErrCnt[TASK_ID_SWITCH]++;
		HcuErrorPrint("SWITCH: Error Set FSM State!\n");
		return FAILURE;
	}
	if ((zHcuSysEngPar.debugMode & HCU_TRACE_DEBUG_FAT_ON) != FALSE){
		HcuDebugPrint("SWITCH: Enter FSM_STATE_SWITCH_ACTIVED status, Keeping refresh here!\n");
	}
	/*

	//进入阻塞式接收数据状态，然后继续发送
	while(1){
		//接收数据
		int dataLen=0;
		if (dataLen > 1){
			//发送数据给HSMMP
			msg_struct_switch_hsmmp_data_rx_t snd;
			memset(&snd, 0, sizeof(msg_struct_switch_hsmmp_data_rx_t));
			snd.length = sizeof(msg_struct_switch_hsmmp_data_rx_t);
			ret = hcu_message_send(MSG_ID_SWITCH_HSMMP_DATA_RX, TASK_ID_HSMMP, TASK_ID_SWITCH, &snd, snd.length);
			if (ret == FAILURE){
				zHcuRunErrCnt[TASK_ID_SWITCH]++;
				HcuErrorPrint("SWITCH: Send message error, TASK [%s] to TASK[%s]!\n", zHcuTaskNameList[TASK_ID_SWITCH], zHcuTaskNameList[TASK_ID_HSMMP]);
				return FAILURE;
			}
		}

		hcu_sleep(5);
	}
	*/

	return SUCCESS;
}

OPSTAT fsm_switch_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HcuErrorPrint("SWITCH: Internal error counter reach DEAD level, SW-RESTART soon!\n");
	zHcuGlobalCounter.restartCnt++;
	fsm_switch_init(0, 0, NULL, 0);
	return SUCCESS;
}

OPSTAT func_switch_int_init(void)
{
	return SUCCESS;
}
