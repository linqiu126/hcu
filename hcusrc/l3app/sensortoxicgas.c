/*
 * sensortoxicgas.c
 *
 *  Created on: 2016年5月10日
 *      Author: hitpony
 */

#include "sensortoxicgas.h"

#include "../l0service/timer.h"
#include "../l0service/trace.h"
#include "../l1com/l1comdef.h"

/*
** FSM of the TOXICGAS
*/
HcuFsmStateItem_t HcuFsmToxicgas[] =
{
    //MessageId                 //State                   		 		//Function
	//启始点，固定定义，不要改动, 使用ENTRY/END，意味者MSGID肯定不可能在某个高位区段中；考虑到所有任务共享MsgId，即使分段，也无法实现
	//完全是为了给任务一个初始化的机会，按照状态转移机制，该函数不具备启动的机会，因为任务初始化后自动到FSM_STATE_IDLE
	//如果没有必要进行初始化，可以设置为NULL
	{MSG_ID_ENTRY,       		FSM_STATE_ENTRY,            			fsm_toxicgas_task_entry}, //Starting

	//System level initialization, only controlled by HCU-MAIN
    {MSG_ID_COM_INIT,       	FSM_STATE_IDLE,            				fsm_toxicgas_init},
    {MSG_ID_COM_INIT_FEEDBACK,	FSM_STATE_IDLE,            				fsm_com_do_nothing},

	//Task level initialization
    {MSG_ID_COM_INIT,       	FSM_STATE_TOXICGAS_INITED,            	fsm_toxicgas_init},
    {MSG_ID_COM_INIT_FEEDBACK,	FSM_STATE_TOXICGAS_INITED,            	fsm_com_do_nothing},

	//ANY state entry
    {MSG_ID_COM_INIT_FEEDBACK,				FSM_STATE_COMMON,          				fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT,       			FSM_STATE_COMMON,          				fsm_com_heart_beat_rcv},
	{MSG_ID_COM_STOP,       				FSM_STATE_COMMON,          				fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT_FB,       		FSM_STATE_COMMON,          				fsm_com_do_nothing},
    {MSG_ID_COM_RESTART,					FSM_STATE_COMMON,            			fsm_toxicgas_restart},
	{MSG_ID_COM_TIME_OUT,       			FSM_STATE_COMMON,          				fsm_toxicgas_time_out},

    //Task level initialization
    {MSG_ID_COM_INIT_FEEDBACK,	FSM_STATE_TOXICGAS_ACTIVED,            	fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT,     FSM_STATE_TOXICGAS_ACTIVED,       		fsm_com_heart_beat_rcv},
	{MSG_ID_COM_HEART_BEAT_FB,  FSM_STATE_TOXICGAS_ACTIVED,       		fsm_com_do_nothing},

    //结束点，固定定义，不要改动
    {MSG_ID_END,            	FSM_STATE_END,             				NULL},  //Ending
};

//Global variables
extern float zHcuGpioToxicgasMq135;
extern float zHcuGpioToxicgasZp01voc;

//Main Entry
//Input parameter would be useless, but just for similar structure purpose
OPSTAT fsm_toxicgas_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//除了对全局变量进行操作之外，尽量不要做其它操作，因为该函数将被主任务/线程调用，不是本任务/线程调用
	//该API就是给本任务一个提早介入的入口，可以帮着做些测试性操作
	if (FsmSetState(TASK_ID_TOXICGAS, FSM_STATE_IDLE) == FAILURE){
		HcuErrorPrint("TOXICGAS: Error Set FSM State at fsm_toxicgas_task_entry\n");}
	return SUCCESS;
}

OPSTAT fsm_toxicgas_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;

	if ((src_id > TASK_ID_MIN) &&(src_id < TASK_ID_MAX)){
		//Send back MSG_ID_COM_INIT_FEEDBACK to SVRCON
		msg_struct_com_init_feedback_t snd0;
		memset(&snd0, 0, sizeof(msg_struct_com_init_feedback_t));
		snd0.length = sizeof(msg_struct_com_init_feedback_t);

		//to avoid all task send out the init fb msg at the same time which lead to msgque get stuck
		hcu_usleep(dest_id*HCU_DURATION_OF_INIT_FB_WAIT_MAX);

		ret = hcu_message_send(MSG_ID_COM_INIT_FEEDBACK, src_id, TASK_ID_TOXICGAS, &snd0, snd0.length);
		if (ret == FAILURE){
			HcuErrorPrint("TOXICGAS: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_TOXICGAS].taskName, zHcuVmCtrTab.task[src_id].taskName);
			return FAILURE;
		}
	}

	//收到初始化消息后，进入初始化状态
	if (FsmSetState(TASK_ID_TOXICGAS, FSM_STATE_TOXICGAS_INITED) == FAILURE){
		HcuErrorPrint("TOXICGAS: Error Set FSM State!\n");
		return FAILURE;
	}

	//初始化硬件接口
	if (func_toxicgas_int_init() == FAILURE){
		HcuErrorPrint("TOXICGAS: Error initialize interface!\n");
		return FAILURE;
	}

	//Global Variables
	zHcuSysStaPm.taskRunErrCnt[TASK_ID_TOXICGAS] = 0;

	//启动周期性定时器
	ret = hcu_timer_start(TASK_ID_TOXICGAS, TIMER_ID_1S_TOXICGAS_PERIOD_READ, \
			zHcuSysEngPar.timer.array[TIMER_ID_1S_TOXICGAS_PERIOD_READ].dur, TIMER_TYPE_PERIOD, TIMER_RESOLUTION_1S);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_TOXICGAS]++;
		HcuErrorPrint("TOXICGAS: Error start period timer!\n");
		return FAILURE;
	}

	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_TOXICGAS, FSM_STATE_TOXICGAS_ACTIVED) == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_TOXICGAS]++;
		HcuErrorPrint("TOXICGAS: Error Set FSM State!\n");
		return FAILURE;
	}
	if ((zHcuSysEngPar.debugMode & HCU_TRACE_DEBUG_FAT_ON) != FALSE){
		HcuDebugPrint("TOXICGAS: Enter FSM_STATE_TOXICGAS_ACTIVED status, Keeping refresh here!\n");
	}
	/*

	//进入阻塞式接收数据状态，然后继续发送
	while(1){
		//接收数据
		int dataLen=0;
		if (dataLen > 1){
			//发送数据给HSMMP
			msg_struct_toxicgas_hsmmp_data_rx_t snd;
			memset(&snd, 0, sizeof(msg_struct_toxicgas_hsmmp_data_rx_t));
			snd.length = sizeof(msg_struct_toxicgas_hsmmp_data_rx_t);
			ret = hcu_message_send(MSG_ID_TOXICGAS_HSMMP_DATA_RX, TASK_ID_HSMMP, TASK_ID_TOXICGAS, &snd, snd.length);
			if (ret == FAILURE){
				zHcuRunErrCnt[TASK_ID_TOXICGAS]++;
				HcuErrorPrint("TOXICGAS: Send message error, TASK [%s] to TASK[%s]!\n", zHcuTaskInfo[TASK_ID_TOXICGAS].taskName, zHcuTaskInfo[TASK_ID_HSMMP].taskName);
				return FAILURE;
			}
		}

		hcu_sleep(5);
	}
	*/

	return SUCCESS;
}

OPSTAT fsm_toxicgas_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HcuErrorPrint("TOXICGAS: Internal error counter reach DEAD level, SW-RESTART soon!\n");
	zHcuSysStaPm.statisCnt.restartCnt++;
	fsm_toxicgas_init(0, 0, NULL, 0);
	return SUCCESS;
}

OPSTAT func_toxicgas_int_init(void)
{
	return SUCCESS;
}

OPSTAT fsm_toxicgas_time_out(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret;

	//Receive message and copy to local variable
	msg_struct_com_time_out_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_com_time_out_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_com_time_out_t))){
		HcuErrorPrint("TOXICGAS: Receive message error!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_TOXICGAS]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//钩子在此处，检查zHcuRunErrCnt[TASK_ID_TOXICGAS]是否超限
	if (zHcuSysStaPm.taskRunErrCnt[TASK_ID_TOXICGAS] > HCU_RUN_ERROR_LEVEL_2_MAJOR){
		//减少重复RESTART的概率
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_TOXICGAS] = zHcuSysStaPm.taskRunErrCnt[TASK_ID_TOXICGAS] - HCU_RUN_ERROR_LEVEL_2_MAJOR;
		msg_struct_com_restart_t snd0;
		memset(&snd0, 0, sizeof(msg_struct_com_restart_t));
		snd0.length = sizeof(msg_struct_com_restart_t);
		ret = hcu_message_send(MSG_ID_COM_RESTART, TASK_ID_TOXICGAS, TASK_ID_TOXICGAS, &snd0, snd0.length);
		if (ret == FAILURE){
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_TOXICGAS]++;
			HcuErrorPrint("TOXICGAS: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_TOXICGAS].taskName, zHcuVmCtrTab.task[TASK_ID_TOXICGAS].taskName);
			return FAILURE;
		}
	}

	//Period time out received
	if ((rcv.timeId == TIMER_ID_1S_TOXICGAS_PERIOD_READ) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		//保护周期读数的优先级，强制抢占状态，并简化问题
		if (FsmGetState(TASK_ID_TOXICGAS) != FSM_STATE_TOXICGAS_ACTIVED){
			ret = FsmSetState(TASK_ID_TOXICGAS, FSM_STATE_TOXICGAS_ACTIVED);
			if (ret == FAILURE){
				zHcuSysStaPm.taskRunErrCnt[TASK_ID_TOXICGAS]++;
				HcuErrorPrint("TOXICGAS: Error Set FSM State!\n");
				return FAILURE;
			}//FsmSetState
		}

#ifdef TARGET_RASPBERRY_PI3B
		//if ((SENSOR_TOXICGAS_RPI_MQ135_PRESENT == SENSOR_TOXICGAS_RPI_PRESENT_TRUE) && (HCU_SENSOR_PRESENT_MQ135 == HCU_SENSOR_PRESENT_YES)) func_toxicgas_time_out_read_data_from_mq135();
		//if ((SENSOR_TOXICGAS_RPI_ZP01VOC_PRESENT == SENSOR_TOXICGAS_RPI_PRESENT_TRUE) && (HCU_SENSOR_PRESENT_ZP01VOC == HCU_SENSOR_PRESENT_YES)) func_toxicgas_time_out_read_data_from_zp01voc();
#endif

		//目前在非树莓派条件下，DO NOTHING

	}

	return SUCCESS;
}

//暂时没考虑发送给后台云平台
OPSTAT func_toxicgas_time_out_read_data_from_mq135(void)
{
	int ret=0;

	//存入数据库
	if ((HCU_SENSOR_DATA_SAVE_TO_LOCAL_DB_SET == HCU_DB_SENSOR_SAVE_FLAG_YES) && (zHcuGpioToxicgasMq135 >= HCU_SENSOR_TOXICGAS_VALUE_MIN) && (zHcuGpioToxicgasMq135 <= HCU_SENSOR_TOXICGAS_VALUE_MAX))
	{
		sensor_toxicgas_mq135_data_element_t toxicgasData;
		memset(&toxicgasData, 0, sizeof(sensor_toxicgas_mq135_data_element_t));
		toxicgasData.equipid = 0;
		toxicgasData.timeStamp = time(0);
		toxicgasData.dataFormat = CLOUD_SENSOR_DATA_FOMAT_INT_ONLY;
		toxicgasData.toxicgasValue = (int)(zHcuGpioToxicgasMq135);

		ret = dbi_HcuToxicgasMq135DataInfo_save(&toxicgasData);
		if (ret == FAILURE){
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_TOXICGAS]++;
			HcuErrorPrint("TOXICGAS: Can not save ToxicgasMq135 data into database!\n");
		}
	}

	return SUCCESS;
}

//暂时没考虑发送给后台云平台
OPSTAT func_toxicgas_time_out_read_data_from_zp01voc(void)
{
	int ret=0;

	//存入数据库
	if ((HCU_SENSOR_DATA_SAVE_TO_LOCAL_DB_SET == HCU_DB_SENSOR_SAVE_FLAG_YES) && (zHcuGpioToxicgasZp01voc >= HCU_SENSOR_TOXICGAS_VALUE_MIN) && (zHcuGpioToxicgasZp01voc <= HCU_SENSOR_TOXICGAS_VALUE_MAX))
	{
		sensor_toxicgas_zp01voc_data_element_t toxicgasData;
		memset(&toxicgasData, 0, sizeof(sensor_toxicgas_zp01voc_data_element_t));
		toxicgasData.equipid = 0;
		toxicgasData.timeStamp = time(0);
		toxicgasData.dataFormat = CLOUD_SENSOR_DATA_FOMAT_INT_ONLY;
		toxicgasData.toxicgasValue = (int)(zHcuGpioToxicgasZp01voc);

		ret = dbi_HcuToxicgasZp01vocDataInfo_save(&toxicgasData);
		if (ret == FAILURE){
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_TOXICGAS]++;
			HcuErrorPrint("TOXICGAS: Can not save ToxicgasZp01voc data into database!\n");
		}
	}

	return SUCCESS;
}

