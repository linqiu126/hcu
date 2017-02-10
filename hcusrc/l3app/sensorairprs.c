/*
 * sensorairprs.c
 *
 *  Created on: 2016年5月10日
 *      Author: hitpony
 */

#include "sensorairprs.h"

#include "../l0service/timer.h"
#include "../l0service/trace.h"
#include "../l1com/l1comdef.h"

/*
** FSM of the AIRPRS
*/
HcuFsmStateItem_t HcuFsmAirprs[] =
{
    //MessageId                 //State                   		 		//Function
	//启始点，固定定义，不要改动, 使用ENTRY/END，意味者MSGID肯定不可能在某个高位区段中；考虑到所有任务共享MsgId，即使分段，也无法实现
	//完全是为了给任务一个初始化的机会，按照状态转移机制，该函数不具备启动的机会，因为任务初始化后自动到FSM_STATE_IDLE
	//如果没有必要进行初始化，可以设置为NULL
	{MSG_ID_ENTRY,       		FSM_STATE_ENTRY,            			fsm_airprs_task_entry}, //Starting

	//System level initialization, only controlled by HCU-MAIN
    {MSG_ID_COM_INIT,       	FSM_STATE_IDLE,            				fsm_airprs_init},
    {MSG_ID_COM_INIT_FEEDBACK,	FSM_STATE_IDLE,            				fsm_com_do_nothing},

	//Task level initialization
    {MSG_ID_COM_INIT,       	FSM_STATE_AIRPRS_INITIED,            	fsm_airprs_init},
    {MSG_ID_COM_INIT_FEEDBACK,	FSM_STATE_AIRPRS_INITIED,            	fsm_com_do_nothing},

	//ANY state entry
    {MSG_ID_COM_RESTART,					FSM_STATE_COMMON,            			fsm_airprs_restart},
	{MSG_ID_COM_TIME_OUT,       			FSM_STATE_COMMON,          				fsm_airprs_time_out},
    {MSG_ID_COM_INIT_FEEDBACK,				FSM_STATE_COMMON,          				fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT,       			FSM_STATE_COMMON,          				fsm_com_heart_beat_rcv},
	{MSG_ID_COM_STOP,       				FSM_STATE_COMMON,          				fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT_FB,       		FSM_STATE_COMMON,          				fsm_com_do_nothing},

    //Task level initialization
    {MSG_ID_COM_INIT_FEEDBACK,	FSM_STATE_AIRPRS_ACTIVED,            	fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT,     FSM_STATE_AIRPRS_ACTIVED,       		fsm_com_heart_beat_rcv},
	{MSG_ID_COM_HEART_BEAT_FB,  FSM_STATE_AIRPRS_ACTIVED,       		fsm_com_do_nothing},

    //结束点，固定定义，不要改动
    {MSG_ID_END,            	FSM_STATE_END,             				NULL},  //Ending
};

//Global variables
extern HcuSysEngParTab_t zHcuSysEngPar; //全局工程参数控制表
extern float zHcuI2cAirprsBmp180;
extern float zHcuI2cAltitudeBmp180;

//Main Entry
//Input parameter would be useless, but just for similar structure purpose
OPSTAT fsm_airprs_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//除了对全局变量进行操作之外，尽量不要做其它操作，因为该函数将被主任务/线程调用，不是本任务/线程调用
	//该API就是给本任务一个提早介入的入口，可以帮着做些测试性操作
	if (FsmSetState(TASK_ID_AIRPRS, FSM_STATE_IDLE) == FAILURE){
		HcuErrorPrint("AIRPRS: Error Set FSM State at fsm_airprs_task_entry\n");}
	return SUCCESS;
}

OPSTAT fsm_airprs_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;

	if ((src_id > TASK_ID_MIN) &&(src_id < TASK_ID_MAX)){
		//Send back MSG_ID_COM_INIT_FEEDBACK to SVRCON
		msg_struct_com_init_feedback_t snd0;
		memset(&snd0, 0, sizeof(msg_struct_com_init_feedback_t));
		snd0.length = sizeof(msg_struct_com_init_feedback_t);

		//to avoid all task send out the init fb msg at the same time which lead to msgque get stuck
		hcu_usleep(dest_id*HCU_SYSCFG_DURATION_OF_INIT_FB_WAIT_MAX);

		ret = hcu_message_send(MSG_ID_COM_INIT_FEEDBACK, src_id, TASK_ID_AIRPRS, &snd0, snd0.length);
		if (ret == FAILURE){
			HcuErrorPrint("AIRPRS: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_AIRPRS].taskName, zHcuVmCtrTab.task[src_id].taskName);
			return FAILURE;
		}
	}

	//收到初始化消息后，进入初始化状态
	if (FsmSetState(TASK_ID_AIRPRS, FSM_STATE_AIRPRS_INITED) == FAILURE){
		HcuErrorPrint("AIRPRS: Error Set FSM State!\n");
		return FAILURE;
	}

	//初始化硬件接口
	if (func_airprs_int_init() == FAILURE){
		HcuErrorPrint("AIRPRS: Error initialize interface!\n");
		return FAILURE;
	}

	//Global Variables
	zHcuSysStaPm.taskRunErrCnt[TASK_ID_AIRPRS] = 0;

	//启动周期性定时器
	ret = hcu_timer_start(TASK_ID_AIRPRS, TIMER_ID_1S_AIRPRS_PERIOD_READ, \
			zHcuSysEngPar.timer.array[TIMER_ID_1S_AIRPRS_PERIOD_READ].dur, TIMER_TYPE_PERIOD, TIMER_RESOLUTION_1S);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_AIRPRS]++;
		HcuErrorPrint("AIRPRS: Error start period timer!\n");
		return FAILURE;
	}

	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_AIRPRS, FSM_STATE_AIRPRS_ACTIVED) == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_AIRPRS]++;
		HcuErrorPrint("AIRPRS: Error Set FSM State!\n");
		return FAILURE;
	}
	if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_FAT_ON) != FALSE){
		HcuDebugPrint("AIRPRS: Enter FSM_STATE_AIRPRS_ACTIVED status, Keeping refresh here!\n");
	}
	/*

	//进入阻塞式接收数据状态，然后继续发送
	while(1){
		//接收数据
		int dataLen=0;
		if (dataLen > 1){
			//发送数据给HSMMP
			msg_struct_airprs_hsmmp_data_rx_t snd;
			memset(&snd, 0, sizeof(msg_struct_airprs_hsmmp_data_rx_t));
			snd.length = sizeof(msg_struct_airprs_hsmmp_data_rx_t);
			ret = hcu_message_send(MSG_ID_AIRPRS_HSMMP_DATA_RX, TASK_ID_HSMMP, TASK_ID_AIRPRS, &snd, snd.length);
			if (ret == FAILURE){
				zHcuRunErrCnt[TASK_ID_AIRPRS]++;
				HcuErrorPrint("AIRPRS: Send message error, TASK [%s] to TASK[%s]!\n", zHcuTaskInfo[TASK_ID_AIRPRS].taskName, zHcuTaskInfo[TASK_ID_HSMMP].taskName);
				return FAILURE;
			}
		}

		hcu_sleep(5);
	}
	*/

	return SUCCESS;
}

OPSTAT fsm_airprs_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HcuErrorPrint("AIRPRS: Internal error counter reach DEAD level, SW-RESTART soon!\n");
	zHcuSysStaPm.statisCnt.restartCnt++;
	fsm_airprs_init(0, 0, NULL, 0);
	return SUCCESS;
}

OPSTAT func_airprs_int_init(void)
{
	return SUCCESS;
}

OPSTAT fsm_airprs_time_out(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret;

	//Receive message and copy to local variable
	msg_struct_com_time_out_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_com_time_out_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_com_time_out_t))){
		HcuErrorPrint("AIRPRS: Receive message error!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_AIRPRS]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//钩子在此处，检查zHcuRunErrCnt[TASK_ID_AIRPRS]是否超限
	if (zHcuSysStaPm.taskRunErrCnt[TASK_ID_AIRPRS] > HCU_RUN_ERROR_LEVEL_2_MAJOR){
		//减少重复RESTART的概率
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_AIRPRS] = zHcuSysStaPm.taskRunErrCnt[TASK_ID_AIRPRS] - HCU_RUN_ERROR_LEVEL_2_MAJOR;
		msg_struct_com_restart_t snd0;
		memset(&snd0, 0, sizeof(msg_struct_com_restart_t));
		snd0.length = sizeof(msg_struct_com_restart_t);
		ret = hcu_message_send(MSG_ID_COM_RESTART, TASK_ID_AIRPRS, TASK_ID_AIRPRS, &snd0, snd0.length);
		if (ret == FAILURE){
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_AIRPRS]++;
			HcuErrorPrint("AIRPRS: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_AIRPRS].taskName, zHcuVmCtrTab.task[TASK_ID_AIRPRS].taskName);
			return FAILURE;
		}
	}

	//Period time out received
	if ((rcv.timeId == TIMER_ID_1S_AIRPRS_PERIOD_READ) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		//保护周期读数的优先级，强制抢占状态，并简化问题
		if (FsmGetState(TASK_ID_AIRPRS) != FSM_STATE_AIRPRS_ACTIVED){
			ret = FsmSetState(TASK_ID_AIRPRS, FSM_STATE_AIRPRS_ACTIVED);
			if (ret == FAILURE){
				zHcuSysStaPm.taskRunErrCnt[TASK_ID_AIRPRS]++;
				HcuErrorPrint("AIRPRS: Error Set FSM State!\n");
				return FAILURE;
			}//FsmSetState
		}

#ifdef TARGET_RASPBERRY_PI3B
		//if ((SENSOR_AIRPRS_RPI_BMP180_PRESENT == SENSOR_AIRPRS_RPI_PRESENT_TRUE) && (HCU_SENSOR_PRESENT_BMP180 == HCU_SENSOR_PRESENT_YES)) func_airprs_time_out_read_data_from_bmp180();
#endif

		//目前在非树莓派条件下，DO NOTHING

	}

	return SUCCESS;
}

//暂时没考虑发送给后台云平台
OPSTAT func_airprs_time_out_read_data_from_bmp180(void)
{
	int ret=0;

	//存入数据库
	if ((HCU_SYSCFG_SNR_DATA_SAVE_TO_LOCAL_DB_SET == HCU_SYSCFG_SENSOR_SAVE_TO_LOCAL_DB_FLAG_YES) && (zHcuI2cAirprsBmp180 >= HCU_SENSOR_AIRPRS_VALUE_MIN) && (zHcuI2cAirprsBmp180 <= HCU_SENSOR_AIRPRS_VALUE_MAX))
	{
		sensor_airprs_bmp180_data_element_t airprsData;
		memset(&airprsData, 0, sizeof(sensor_airprs_bmp180_data_element_t));
		airprsData.equipid = 0;
		airprsData.timeStamp = time(0);
		airprsData.dataFormat = CLOUD_SENSOR_DATA_FOMAT_FLOAT_WITH_NF2;
		airprsData.airprsValue = (int)(zHcuI2cAirprsBmp180*100);

		ret = dbi_HcuAirprsBmp180DataInfo_save(&airprsData);
		if (ret == FAILURE){
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_AIRPRS]++;
			HcuErrorPrint("AIRPRS: Can not save AirprsBmp180 data into database!\n");
		}
	}

	//存入数据库
	if ((HCU_SYSCFG_SNR_DATA_SAVE_TO_LOCAL_DB_SET == HCU_SYSCFG_SENSOR_SAVE_TO_LOCAL_DB_FLAG_YES) && (zHcuI2cAltitudeBmp180 >= HCU_SENSOR_ALTITUDE_VALUE_MIN) && (zHcuI2cAltitudeBmp180 <= HCU_SENSOR_ALTITUDE_VALUE_MAX))
	{
		sensor_airprs_altitude_bmp180_data_element_t altitudeData;
		memset(&altitudeData, 0, sizeof(sensor_airprs_altitude_bmp180_data_element_t));
		altitudeData.equipid = 0;
		altitudeData.timeStamp = time(0);
		altitudeData.dataFormat = CLOUD_SENSOR_DATA_FOMAT_FLOAT_WITH_NF2;
		altitudeData.altitudeValue = (int)(zHcuI2cAltitudeBmp180*100);

		ret = dbi_HcuAirprsAltitudeBmp180DataInfo_save(&altitudeData);
		if (ret == FAILURE){
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_AIRPRS]++;
			HcuErrorPrint("AIRPRS: Can not save AltitudeBmp180 data into database!\n");
		}
	}

	return SUCCESS;
}

