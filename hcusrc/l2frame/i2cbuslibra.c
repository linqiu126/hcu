/*
 * i2cbuslibra.c
 *
 *  Created on: 2016年5月14日
 *      Author: hitpony
 */

#include "i2cbuslibra.h"

#include "../l0service/timer.h"
#include "../l0service/trace.h"
#include "../l1com/l1comdef.h"

/*
** FSM of the I2CBUSLIBRA
*/
HcuFsmStateItem_t HcuFsmI2cbuslibra[] =
{
    //MessageId                 //State                   		 		//Function
	//启始点，固定定义，不要改动, 使用ENTRY/END，意味者MSGID肯定不可能在某个高位区段中；考虑到所有任务共享MsgId，即使分段，也无法实现
	//完全是为了给任务一个初始化的机会，按照状态转移机制，该函数不具备启动的机会，因为任务初始化后自动到FSM_STATE_IDLE
	//如果没有必要进行初始化，可以设置为NULL
	{MSG_ID_ENTRY,       		FSM_STATE_ENTRY,            			fsm_i2cbuslibra_task_entry}, //Starting

	//System level initialization, only controlled by HCU-MAIN
    {MSG_ID_COM_INIT,       	FSM_STATE_IDLE,            				fsm_i2cbuslibra_init},
    {MSG_ID_COM_INIT_FEEDBACK,	FSM_STATE_IDLE,            				fsm_com_do_nothing},

	//Task level initialization
    {MSG_ID_COM_INIT,       	FSM_STATE_I2CBUSLIBRA_INITIED,            	fsm_i2cbuslibra_init},
    {MSG_ID_COM_INIT_FEEDBACK,	FSM_STATE_I2CBUSLIBRA_INITIED,            	fsm_com_do_nothing},

	//ANY state entry
    {MSG_ID_COM_INIT_FEEDBACK,				FSM_STATE_COMMON,          				fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT,       			FSM_STATE_COMMON,          				fsm_com_heart_beat_rcv},
	{MSG_ID_COM_STOP,       				FSM_STATE_COMMON,          				fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT_FB,       		FSM_STATE_COMMON,          				fsm_com_do_nothing},
    {MSG_ID_COM_RESTART,					FSM_STATE_COMMON,            			fsm_i2cbuslibra_restart},
	{MSG_ID_COM_TIME_OUT,       			FSM_STATE_COMMON,          				fsm_com_do_nothing},

    //Task level initialization
    {MSG_ID_COM_INIT_FEEDBACK,	FSM_STATE_I2CBUSLIBRA_ACTIVED,            	fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT,     FSM_STATE_I2CBUSLIBRA_ACTIVED,       		fsm_com_heart_beat_rcv},
	{MSG_ID_COM_HEART_BEAT_FB,  FSM_STATE_I2CBUSLIBRA_ACTIVED,       		fsm_com_do_nothing},

    //结束点，固定定义，不要改动
    {MSG_ID_END,            	FSM_STATE_END,             				NULL},  //Ending
};

//Global variables

//Main Entry
//Input parameter would be useless, but just for similar structure purpose
OPSTAT fsm_i2cbuslibra_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//除了对全局变量进行操作之外，尽量不要做其它操作，因为该函数将被主任务/线程调用，不是本任务/线程调用
	//该API就是给本任务一个提早介入的入口，可以帮着做些测试性操作
	FsmSetState(TASK_ID_I2CBUSLIBRA, FSM_STATE_IDLE);
	return SUCCESS;
}

OPSTAT fsm_i2cbuslibra_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;

	if ((src_id > TASK_ID_MIN) &&(src_id < TASK_ID_MAX)){
		//Send back MSG_ID_COM_INIT_FEEDBACK to SVRCON
		msg_struct_com_init_feedback_t snd0;
		memset(&snd0, 0, sizeof(msg_struct_com_init_feedback_t));
		snd0.length = sizeof(msg_struct_com_init_feedback_t);

		//to avoid all task send out the init fb msg at the same time which lead to msgque get stuck
		hcu_usleep(dest_id*HCU_SYSCFG_DURATION_OF_INIT_FB_WAIT_MAX);

		ret = hcu_message_send(MSG_ID_COM_INIT_FEEDBACK, src_id, TASK_ID_I2CBUSLIBRA, &snd0, snd0.length);
		if (ret == FAILURE){
			HcuErrorPrint("I2CBUSLIBRA: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_I2CBUSLIBRA].taskName, zHcuVmCtrTab.task[src_id].taskName);
			return FAILURE;
		}
	}

	//收到初始化消息后，进入初始化状态
	FsmSetState(TASK_ID_I2CBUSLIBRA, FSM_STATE_I2CBUSLIBRA_INITED);

	//初始化硬件接口
	if (func_i2cbuslibra_int_init() == FAILURE){
		HcuErrorPrint("I2CBUSLIBRA: Error initialize interface!\n");
		return FAILURE;
	}

	//Global Variables
	zHcuSysStaPm.taskRunErrCnt[TASK_ID_I2CBUSLIBRA] = 0;

	//设置状态机到目标状态
	FsmSetState(TASK_ID_I2CBUSLIBRA, FSM_STATE_I2CBUSLIBRA_ACTIVED);
	HCU_DEBUG_PRINT_FAT("I2CBUSLIBRA: Enter FSM_STATE_I2CBUSLIBRA_ACTIVED status, Keeping refresh here!\n");
	/*

	//进入阻塞式接收数据状态，然后继续发送
	while(1){
		//接收数据
		int dataLen=0;
		if (dataLen > 1){
			//发送数据给HSMMP
			msg_struct_i2cbuslibra_hsmmp_data_rx_t snd;
			memset(&snd, 0, sizeof(msg_struct_i2cbuslibra_hsmmp_data_rx_t));
			snd.length = sizeof(msg_struct_i2cbuslibra_hsmmp_data_rx_t);
			ret = hcu_message_send(MSG_ID_I2CBUSLIBRA_HSMMP_DATA_RX, TASK_ID_HSMMP, TASK_ID_I2CBUSLIBRA, &snd, snd.length);
			if (ret == FAILURE){
				zHcuRunErrCnt[TASK_ID_I2CBUSLIBRA]++;
				HcuErrorPrint("I2CBUSLIBRA: Send message error, TASK [%s] to TASK[%s]!\n", zHcuTaskInfo[TASK_ID_I2CBUSLIBRA].taskName, zHcuTaskInfo[TASK_ID_HSMMP].taskName);
				return FAILURE;
			}
		}

		hcu_sleep(5);
	}
	*/

	return SUCCESS;
}

OPSTAT fsm_i2cbuslibra_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HcuErrorPrint("I2CBUSLIBRA: Internal error counter reach DEAD level, SW-RESTART soon!\n");
	zHcuSysStaPm.statisCnt.restartCnt++;
	fsm_i2cbuslibra_init(0, 0, NULL, 0);
	return SUCCESS;
}

OPSTAT func_i2cbuslibra_int_init(void)
{
	return SUCCESS;
}

