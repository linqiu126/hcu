/*
 * l3daycg21.c
 *
 *  Created on: 2017年11月22日
 *      Author: test
 */


#include "l3daycg21.h"

#include "../l0service/timer.h"
#include "../l0service/trace.h"
#include "../l1com/hwinv.h"
#include "../l1com/l1comdef.h"
#include "../l2frame/cloudvela.h"
#include "../l3app/sensornoise.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <wchar.h>

/*
** FSM of the L3DAYCG21
*/
HcuFsmStateItem_t HcuFsmL3daycg21[] =
{
    //MessageId                 //State                   		 		//Function
	//启始点，固定定义，不要改动, 使用ENTRY/END，意味者MSGID肯定不可能在某个高位区段中；考虑到所有任务共享MsgId，即使分段，也无法实现
	//完全是为了给任务一个初始化的机会，按照状态转移机制，该函数不具备启动的机会，因为任务初始化后自动到FSM_STATE_IDLE
	//如果没有必要进行初始化，可以设置为NULL
	{MSG_ID_ENTRY,       						FSM_STATE_ENTRY,            			fsm_l3daycg21_task_entry}, //Starting

	//System level initialization, only controlled by HCU-MAIN
    {MSG_ID_COM_INIT,       					FSM_STATE_IDLE,            				fsm_l3daycg21_init},
    {MSG_ID_COM_INIT_FEEDBACK,					FSM_STATE_IDLE,            				fsm_com_do_nothing},

	//Task level initialization
    {MSG_ID_COM_INIT,       					FSM_STATE_L3DAYCG21_INITED,            		fsm_l3daycg21_init},
    {MSG_ID_COM_INIT_FEEDBACK,					FSM_STATE_L3DAYCG21_INITED,            		fsm_com_do_nothing},

	//ANY state entry
    {MSG_ID_COM_INIT_FEEDBACK,					FSM_STATE_COMMON,          					fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT,       				FSM_STATE_COMMON,          					fsm_com_heart_beat_rcv},
	{MSG_ID_COM_STOP,       					FSM_STATE_COMMON,          					fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT_FB,       			FSM_STATE_COMMON,          					fsm_com_do_nothing},
    {MSG_ID_COM_RESTART,						FSM_STATE_COMMON,            				fsm_l3daycg21_restart},
	{MSG_ID_COM_TIME_OUT,       				FSM_STATE_COMMON,          					fsm_l3daycg21_time_out},

    //Normal working status
	//{MSG_ID_CLOUDVELA_YCJK_DATA_REQ,    		FSM_STATE_L3DAYCG21_ACTIVED,      			fsm_l3daycg21_cloudvela_data_req},

    //结束点，固定定义，不要改动
    {MSG_ID_END,            	FSM_STATE_END,             				NULL},  //Ending
};

//Global variables

//Task Global variables


//Main Entry
//Input parameter would be useless, but just for similar structure purpose
OPSTAT fsm_l3daycg21_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//除了对全局变量进行操作之外，尽量不要做其它操作，因为该函数将被主任务/线程调用，不是本任务/线程调用
	//该API就是给本任务一个提早介入的入口，可以帮着做些测试性操作
	if (FsmSetState(TASK_ID_L3DAYCG21, FSM_STATE_IDLE) == FAILURE){
		HcuErrorPrint("L3DAYCG21: Error Set FSM State at fsm_l3daycg21_task_entry\n");}
	return SUCCESS;
}

OPSTAT fsm_l3daycg21_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;
	//LCD_AlarmFlag = 0;

	if ((src_id > TASK_ID_MIN) &&(src_id < TASK_ID_MAX)){
		//Send back MSG_ID_COM_INIT_FEEDBACK to SVRCON
		msg_struct_com_init_feedback_t snd0;
		memset(&snd0, 0, sizeof(msg_struct_com_init_feedback_t));
		snd0.length = sizeof(msg_struct_com_init_feedback_t);

		//to avoid all task send out the init fb msg at the same time which lead to msgque get stuck
		hcu_usleep(dest_id*HCU_SYSCFG_DURATION_OF_INIT_FB_WAIT_MAX);

		ret = hcu_message_send(MSG_ID_COM_INIT_FEEDBACK, src_id, TASK_ID_L3DAYCG21, &snd0, snd0.length);
		if (ret == FAILURE){
			HcuErrorPrint("L3DAYCG21: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3DAYCG21].taskName, zHcuVmCtrTab.task[src_id].taskName);
			return FAILURE;
		}
	}

	//收到初始化消息后，进入初始化状态
	if (FsmSetState(TASK_ID_L3DAYCG21, FSM_STATE_L3DAYCG21_INITED) == FAILURE){
		HcuErrorPrint("L3DAYCG21: Error Set FSM State!\n");
		return FAILURE;
	}

	//初始化硬件接口
	if (func_l3daycg21_int_init() == FAILURE){
		HcuErrorPrint("L3DAYCG21: Error initialize interface!\n");
		return FAILURE;
	}

	//启动周期性定时器
	ret = hcu_timer_start(TASK_ID_L3DAYCG21, TIMER_ID_1S_L3DAYCG21_PERIOD_READ, \
			zHcuSysEngPar.timer.array[TIMER_ID_1S_L3DAYCG21_PERIOD_READ].dur, TIMER_TYPE_PERIOD, TIMER_RESOLUTION_1S);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3DAYCG21]++;
		HcuErrorPrint("L3DAYCG21: Error start period timer!\n");
		return FAILURE;
	}

	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_L3DAYCG21, FSM_STATE_L3DAYCG21_ACTIVED) == FAILURE)
		HCU_ERROR_PRINT_L3DAYCG21("L3DAYCG21: Error Set FSM State!\n");
	HCU_DEBUG_PRINT_FAT("L3DAYCG21: Enter FSM_STATE_L3DAYCG21_ACTIVED status, Keeping refresh here!\n");

	//返回
	return SUCCESS;
}

OPSTAT fsm_l3daycg21_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HcuErrorPrint("L3DAYCG21: Internal error counter reach DEAD level, SW-RESTART soon!\n");
	zHcuSysStaPm.statisCnt.restartCnt++;
	fsm_l3daycg21_init(0, 0, NULL, 0);
	return SUCCESS;
}

OPSTAT func_l3daycg21_int_init(void)
{
	return SUCCESS;
}

//暂时啥都不干，定时到达后，还不明确是否需要支持定时工作
OPSTAT fsm_l3daycg21_time_out(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret;

	//Receive message and copy to local variable
	msg_struct_com_time_out_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_com_time_out_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_com_time_out_t))){
		HcuErrorPrint("L3DAYCG21: Receive message error!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3DAYCG21]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//钩子在此处，检查zHcuRunErrCnt[TASK_ID_L3DAYCG21]是否超限
	if (zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3DAYCG21] > HCU_RUN_ERROR_LEVEL_2_MAJOR){
		//减少重复RESTART的概率
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3DAYCG21] = zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3DAYCG21] - HCU_RUN_ERROR_LEVEL_2_MAJOR;
		msg_struct_com_restart_t snd0;
		memset(&snd0, 0, sizeof(msg_struct_com_restart_t));
		snd0.length = sizeof(msg_struct_com_restart_t);
		ret = hcu_message_send(MSG_ID_COM_RESTART, TASK_ID_L3DAYCG21, TASK_ID_L3DAYCG21, &snd0, snd0.length);
		if (ret == FAILURE){
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3DAYCG21]++;
			HcuErrorPrint("L3DAYCG21: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3DAYCG21].taskName, zHcuVmCtrTab.task[TASK_ID_L3DAYCG21].taskName);
			return FAILURE;
		}
	}

	//Period time out received
	if ((rcv.timeId == TIMER_ID_1S_L3DAYCG21_PERIOD_READ) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		//READ processing func!
	}

	return SUCCESS;
}










