/*
 * sensoralcohol.c
 *
 *  Created on: 2016年5月10日
 *      Author: hitpony
 */

#include "sensoralcohol.h"

#include "../l0service/timer.h"
#include "../l0service/trace.h"
#include "../l1com/l1comdef.h"

/*
** FSM of the ALCOHOL
*/
HcuFsmStateItem_t HcuFsmAlcohol[] =
{
    //MessageId                 //State                   		 		//Function
	//启始点，固定定义，不要改动, 使用ENTRY/END，意味者MSGID肯定不可能在某个高位区段中；考虑到所有任务共享MsgId，即使分段，也无法实现
	//完全是为了给任务一个初始化的机会，按照状态转移机制，该函数不具备启动的机会，因为任务初始化后自动到FSM_STATE_IDLE
	//如果没有必要进行初始化，可以设置为NULL
	{MSG_ID_ENTRY,       		FSM_STATE_ENTRY,            			fsm_alcohol_task_entry}, //Starting

	//System level initialization, only controlled by HCU-MAIN
    {MSG_ID_COM_INIT,       	FSM_STATE_IDLE,            				fsm_alcohol_init},
    {MSG_ID_COM_INIT_FEEDBACK,	FSM_STATE_IDLE,            				fsm_com_do_nothing},

	//Task level initialization
    {MSG_ID_COM_INIT,       	FSM_STATE_ALCOHOL_INITED,            	fsm_alcohol_init},
    {MSG_ID_COM_INIT_FEEDBACK,	FSM_STATE_ALCOHOL_INITED,            	fsm_com_do_nothing},

	//ANY state entry
    {MSG_ID_COM_INIT_FEEDBACK,				FSM_STATE_COMMON,          				fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT,       			FSM_STATE_COMMON,          				fsm_com_heart_beat_rcv},
	{MSG_ID_COM_STOP,       				FSM_STATE_COMMON,          				fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT_FB,       		FSM_STATE_COMMON,          				fsm_com_do_nothing},
    {MSG_ID_COM_RESTART,					FSM_STATE_COMMON,            			fsm_alcohol_restart},
	{MSG_ID_COM_TIME_OUT,       			FSM_STATE_COMMON,          				fsm_alcohol_time_out},

    //Task level initialization
	{MSG_ID_CLOUDVELA_ALCOHOL_DATA_REQ,		FSM_STATE_ALCOHOL_ACTIVED,      	  	fsm_alcohol_cloudvela_data_req},
	{MSG_ID_CLOUDVELA_ALCOHOL_DATA_CONFIRM,	FSM_STATE_ALCOHOL_ACTIVED,      	  	fsm_alcohol_cloudvela_data_confirm},
	{MSG_ID_L3AQYC_EXG_CTRL_REQ,			FSM_STATE_ALCOHOL_ACTIVED,      	  	fsm_alcohol_l3aqyc_exg_ctrl_req},

    //结束点，固定定义，不要改动
    {MSG_ID_END,            	FSM_STATE_END,             				NULL},  //Ending
};

//Global variables

//Task Global variables
gTaskAlcoholContext_t gTaskAlcoholContext;

//Main Entry
//Input parameter would be useless, but just for similar structure purpose
OPSTAT fsm_alcohol_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//除了对全局变量进行操作之外，尽量不要做其它操作，因为该函数将被主任务/线程调用，不是本任务/线程调用
	//该API就是给本任务一个提早介入的入口，可以帮着做些测试性操作
	if (FsmSetState(TASK_ID_ALCOHOL, FSM_STATE_IDLE) == FAILURE){
		HcuErrorPrint("ALCOHOL: Error Set FSM State at fsm_alcohol_task_entry\n");}
	return SUCCESS;
}

OPSTAT fsm_alcohol_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;

	if ((src_id > TASK_ID_MIN) &&(src_id < TASK_ID_MAX)){
		//Send back MSG_ID_COM_INIT_FEEDBACK to SVRCON
		msg_struct_com_init_feedback_t snd0;
		memset(&snd0, 0, sizeof(msg_struct_com_init_feedback_t));
		snd0.length = sizeof(msg_struct_com_init_feedback_t);

		//to avoid all task send out the init fb msg at the same time which lead to msgque get stuck
		hcu_usleep(dest_id*HCU_SYSCFG_DURATION_OF_INIT_FB_WAIT_MAX);

		ret = hcu_message_send(MSG_ID_COM_INIT_FEEDBACK, src_id, TASK_ID_ALCOHOL, &snd0, snd0.length);
		if (ret == FAILURE){
			HcuErrorPrint("ALCOHOL: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_ALCOHOL].taskName, zHcuVmCtrTab.task[src_id].taskName);
			return FAILURE;
		}
	}

	//收到初始化消息后，进入初始化状态
	if (FsmSetState(TASK_ID_ALCOHOL, FSM_STATE_ALCOHOL_INITED) == FAILURE){
		HcuErrorPrint("ALCOHOL: Error Set FSM State!\n");
		return FAILURE;
	}

	//初始化硬件接口
	if (func_alcohol_int_init() == FAILURE){
		HcuErrorPrint("ALCOHOL: Error initialize interface!\n");
		return FAILURE;
	}

	//Global Variables
	zHcuSysStaPm.taskRunErrCnt[TASK_ID_ALCOHOL] = 0;
	memset(&gTaskAlcoholContext, 0, sizeof(gTaskAlcoholContext_t));

	//启动周期性定时器
	ret = hcu_timer_start(TASK_ID_ALCOHOL, TIMER_ID_1S_ALCOHOL_PERIOD_READ, \
			zHcuSysEngPar.timer.array[TIMER_ID_1S_ALCOHOL_PERIOD_READ].dur, TIMER_TYPE_PERIOD, TIMER_RESOLUTION_1S);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_ALCOHOL]++;
		HcuErrorPrint("ALCOHOL: Error start period timer!\n");
		return FAILURE;
	}

	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_ALCOHOL, FSM_STATE_ALCOHOL_ACTIVED) == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_ALCOHOL]++;
		HcuErrorPrint("ALCOHOL: Error Set FSM State!\n");
		return FAILURE;
	}
	if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_FAT_ON) != FALSE){
		HcuDebugPrint("ALCOHOL: Enter FSM_STATE_ALCOHOL_ACTIVED status, Keeping refresh here!\n");
	}
	/*

	//进入阻塞式接收数据状态，然后继续发送
	while(1){
		//接收数据
		int dataLen=0;
		if (dataLen > 1){
			//发送数据给HSMMP
			msg_struct_alcohol_hsmmp_data_rx_t snd;
			memset(&snd, 0, sizeof(msg_struct_alcohol_hsmmp_data_rx_t));
			snd.length = sizeof(msg_struct_alcohol_hsmmp_data_rx_t);
			ret = hcu_message_send(MSG_ID_ALCOHOL_HSMMP_DATA_RX, TASK_ID_HSMMP, TASK_ID_ALCOHOL, &snd, snd.length);
			if (ret == FAILURE){
				zHcuRunErrCnt[TASK_ID_ALCOHOL]++;
				HcuErrorPrint("ALCOHOL: Send message error, TASK [%s] to TASK[%s]!\n", zHcuTaskInfo[TASK_ID_ALCOHOL].taskName, zHcuTaskInfo[TASK_ID_HSMMP].taskName);
				return FAILURE;
			}
		}

		hcu_sleep(5);
	}
	*/

	return SUCCESS;
}

OPSTAT fsm_alcohol_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HcuErrorPrint("ALCOHOL: Internal error counter reach DEAD level, SW-RESTART soon!\n");
	zHcuSysStaPm.statisCnt.restartCnt++;
	fsm_alcohol_init(0, 0, NULL, 0);
	return SUCCESS;
}

OPSTAT func_alcohol_int_init(void)
{
	return SUCCESS;
}

OPSTAT fsm_alcohol_time_out(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret;

	//Receive message and copy to local variable
	msg_struct_com_time_out_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_com_time_out_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_com_time_out_t))){
		HcuErrorPrint("ALCOHOL: Receive message error!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_ALCOHOL]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//钩子在此处，检查zHcuRunErrCnt[TASK_ID_ALCOHOL]是否超限
	if (zHcuSysStaPm.taskRunErrCnt[TASK_ID_ALCOHOL] > HCU_RUN_ERROR_LEVEL_2_MAJOR){
		//减少重复RESTART的概率
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_ALCOHOL] = zHcuSysStaPm.taskRunErrCnt[TASK_ID_ALCOHOL] - HCU_RUN_ERROR_LEVEL_2_MAJOR;
		msg_struct_com_restart_t snd0;
		memset(&snd0, 0, sizeof(msg_struct_com_restart_t));
		snd0.length = sizeof(msg_struct_com_restart_t);
		ret = hcu_message_send(MSG_ID_COM_RESTART, TASK_ID_ALCOHOL, TASK_ID_ALCOHOL, &snd0, snd0.length);
		if (ret == FAILURE){
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_ALCOHOL]++;
			HcuErrorPrint("ALCOHOL: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_ALCOHOL].taskName, zHcuVmCtrTab.task[TASK_ID_ALCOHOL].taskName);
			return FAILURE;
		}
	}

	//Period time out received
	if ((rcv.timeId == TIMER_ID_1S_ALCOHOL_PERIOD_READ) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		//保护周期读数的优先级，强制抢占状态，并简化问题
		if (FsmGetState(TASK_ID_ALCOHOL) != FSM_STATE_ALCOHOL_ACTIVED){
			ret = FsmSetState(TASK_ID_ALCOHOL, FSM_STATE_ALCOHOL_ACTIVED);
			if (ret == FAILURE){
				zHcuSysStaPm.taskRunErrCnt[TASK_ID_ALCOHOL]++;
				HcuErrorPrint("ALCOHOL: Error Set FSM State!\n");
				return FAILURE;
			}//FsmSetState
		}

#ifdef TARGET_RASPBERRY_PI3B
		//if ((SENSOR_ALCOHOL_RPI_MQ3ALCO_PRESENT == SENSOR_ALCOHOL_RPI_PRESENT_TRUE) && (HCU_SENSOR_PRESENT_MQ3ALCO == HCU_SENSOR_PRESENT_YES)) func_alcohol_time_out_read_data_from_mq3alco();
#endif

		//目前在非树莓派条件下，DO NOTHING

	}

	return SUCCESS;
}

//暂时没考虑发送给后台云平台
OPSTAT func_alcohol_time_out_read_data_from_mq3alco(void)
{
	int ret=0;

	//存入数据库
	if ((zHcuVmCtrTab.codab.si[SENSOR_ID_GPIOALCOHOLMQ3ALCO].fVal >= HCU_SENSOR_ALCOHOL_VALUE_MIN) && (zHcuVmCtrTab.codab.si[SENSOR_ID_GPIOALCOHOLMQ3ALCO].fVal <= HCU_SENSOR_ALCOHOL_VALUE_MAX))
	{
		sensor_alcohol_mq3alco_data_element_t alcoholData;
		memset(&alcoholData, 0, sizeof(sensor_alcohol_mq3alco_data_element_t));
		alcoholData.equipid = 0;
		alcoholData.timeStamp = time(0);
		alcoholData.dataFormat = CLOUD_SENSOR_DATA_FOMAT_INT_ONLY;
		alcoholData.alcoholValue = (int)(zHcuVmCtrTab.codab.si[SENSOR_ID_GPIOALCOHOLMQ3ALCO].fVal);

		ret = dbi_HcuAlcoholMq3alcoDataInfo_save(&alcoholData);
		if (ret == FAILURE){
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_ALCOHOL]++;
			HcuErrorPrint("ALCOHOL: Can not save AlcoholMq3alco data into database!\n");
		}
	}

	return SUCCESS;
}

//收到来自CLOUD和后台云的命令，从而重新配置本地控制信息
OPSTAT fsm_alcohol_cloudvela_data_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	return SUCCESS;
}

//收到来自CLOUD和后台云的命令，从而重新配置本地控制信息
OPSTAT fsm_alcohol_cloudvela_data_confirm(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	return SUCCESS;
}

OPSTAT fsm_alcohol_l3aqyc_exg_ctrl_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	return SUCCESS;
}




