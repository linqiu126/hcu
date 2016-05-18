/*
 * spsvirgo.c
 *
 *  Created on: 2016年1月3日
 *      Author: hitpony
 */

#include "spsvirgo.h"

#include "../l0service/trace.h"
#include "../l1com/l1comdef.h"



/*
** FSM of the SPSVIRGO
*/
FsmStateItem_t FsmSpsvirgo[] =
{
    //MessageId                 //State                   		 		//Function
	//启始点，固定定义，不要改动, 使用ENTRY/END，意味者MSGID肯定不可能在某个高位区段中；考虑到所有任务共享MsgId，即使分段，也无法实现
	//完全是为了给任务一个初始化的机会，按照状态转移机制，该函数不具备启动的机会，因为任务初始化后自动到FSM_STATE_IDLE
	//如果没有必要进行初始化，可以设置为NULL
	{MSG_ID_ENTRY,       		FSM_STATE_ENTRY,            			fsm_spsvirgo_task_entry}, //Starting

	//System level initialization, only controlled by HCU-MAIN
    {MSG_ID_COM_INIT,       	FSM_STATE_IDLE,            				fsm_spsvirgo_init},
    {MSG_ID_COM_RESTART,		FSM_STATE_IDLE,            				fsm_spsvirgo_restart},
    {MSG_ID_COM_INIT_FEEDBACK,	FSM_STATE_IDLE,            				fsm_com_do_nothing},

    //Task level initialization
    {MSG_ID_COM_RESTART,        		FSM_STATE_SPSVIRGO_RECEIVED,       	fsm_spsvirgo_restart},
    {MSG_ID_COM_INIT_FEEDBACK,			FSM_STATE_SPSVIRGO_RECEIVED,       	fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT,       		FSM_STATE_SPSVIRGO_RECEIVED,       	fsm_com_heart_beat_rcv},
	{MSG_ID_COM_HEART_BEAT_FB,       	FSM_STATE_SPSVIRGO_RECEIVED,       	fsm_com_do_nothing},
    {MSG_ID_NOISE_SPSVIRGO_DATA_READ,   FSM_STATE_SPSVIRGO_RECEIVED,       	fsm_spsvirgo_noise_data_read},
    {MSG_ID_NOISE_SPSVIRGO_STOP,     	FSM_STATE_SPSVIRGO_RECEIVED,      	fsm_spsvirgo_stop_work},
	{MSG_ID_NOISE_SPSVIRGO_CONTROL_CMD, FSM_STATE_SPSVIRGO_RECEIVED,   		fsm_spsvirgo_noise_control_cmd},

    //结束点，固定定义，不要改动
    {MSG_ID_END,            	FSM_STATE_END,             				NULL},  //Ending
};

extern GpsPosInfo_t zHcuGpsPosInfo;
extern HcuSysEngParTablet_t zHcuSysEngPar; //全局工程参数控制表

//Main Entry
//Input parameter would be useless, but just for similar structure purpose
OPSTAT fsm_spsvirgo_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//除了对全局变量进行操作之外，尽量不要做其它操作，因为该函数将被主任务/线程调用，不是本任务/线程调用
	//该API就是给本任务一个提早介入的入口，可以帮着做些测试性操作
	if (FsmSetState(TASK_ID_SPSVIRGO, FSM_STATE_IDLE) == FAILURE){
		HcuErrorPrint("SPSVIRGO: Error Set FSM State at fsm_spsvirgo_task_entry\n");}
	return SUCCESS;
}

OPSTAT fsm_spsvirgo_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;

	if ((src_id > TASK_ID_MIN) &&(src_id < TASK_ID_MAX)){
		//Send back MSG_ID_COM_INIT_FEEDBACK to SVRCON
		msg_struct_com_init_feedback_t snd0;
		memset(&snd0, 0, sizeof(msg_struct_com_init_feedback_t));
		snd0.length = sizeof(msg_struct_com_init_feedback_t);

		//to avoid all task send out the init fb msg at the same time which lead to msgque get stuck
		hcu_usleep(dest_id*DURATION_OF_INIT_FB_WAIT_MAX);

		ret = hcu_message_send(MSG_ID_COM_INIT_FEEDBACK, src_id, TASK_ID_SPSVIRGO, &snd0, snd0.length);
		if (ret == FAILURE){
			HcuErrorPrint("SPSVIRGO: Send message error, TASK [%s] to TASK[%s]!\n", zHcuTaskNameList[TASK_ID_SPSVIRGO], zHcuTaskNameList[src_id]);
			return FAILURE;
		}
	}

	//收到初始化消息后，进入初始化状态
	if (FsmSetState(TASK_ID_SPSVIRGO, FSM_STATE_SPSVIRGO_INITED) == FAILURE){
		HcuErrorPrint("SPSVIRGO: Error Set FSM State!\n");
		return FAILURE;
	}

	//初始化硬件接口
	if (func_spsvirgo_int_init() == FAILURE){
		HcuErrorPrint("SPSVIRGO: Error initialize interface!\n");
		return FAILURE;
	}

	//Global Variables
	zHcuRunErrCnt[TASK_ID_SPSVIRGO] = 0;

	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_SPSVIRGO, FSM_STATE_SPSVIRGO_RECEIVED) == FAILURE){
		zHcuRunErrCnt[TASK_ID_SPSVIRGO]++;
		HcuErrorPrint("SPSVIRGO: Error Set FSM State!\n");
		return FAILURE;
	}
	if ((zHcuSysEngPar.debugMode & TRACE_DEBUG_FAT_ON) != FALSE){
		HcuDebugPrint("SPSVIRGO: Enter FSM_STATE_SPSVIRGO_ACTIVED status, Keeping refresh here!\n");
	}

	/*
	//进入阻塞式接收数据状态，然后继续发送
	while(1){
		//接收数据
		int dataLen=0;
		if (dataLen > 1){
			//发送数据给CLOUDCONT
			msg_struct_spsvirgo_hsmmp_data_rx_t snd;
			memset(&snd, 0, sizeof(msg_struct_spsvirgo_hsmmp_data_rx_t));
			snd.length = sizeof(msg_struct_spsvirgo_hsmmp_data_rx_t);
			ret = hcu_message_send(MSG_ID_SPSVIRGO_HSMMP_DATA_RX, TASK_ID_HSMMP, TASK_ID_SPSVIRGO, &snd, snd.length);
			if (ret == FAILURE){
				HcuErrorPrint("SPSVIRGO: Send message error, TASK [%s] to TASK[%s]!\n", zHcuTaskNameList[TASK_ID_SPSVIRGO], zHcuTaskNameList[TASK_ID_HSMMP]);
				return FAILURE;
			}
		}

		hcu_sleep(5);
	}*/

	return SUCCESS;
}

OPSTAT fsm_spsvirgo_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HcuErrorPrint("SPSVIRGO: Internal error counter reach DEAD level, SW-RESTART soon!\n");
	zHcuGlobalCounter.restartCnt++;
	fsm_spsvirgo_init(0, 0, NULL, 0);
	return SUCCESS;
}

OPSTAT func_spsvirgo_int_init(void)
{
	return SUCCESS;
}

OPSTAT fsm_spsvirgo_stop_work(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{

	return SUCCESS;
}

OPSTAT fsm_spsvirgo_noise_data_read(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0, currentSensorEqpId=0;

	//消息参数检查
	msg_struct_noise_spsvirgo_data_read_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_noise_spsvirgo_data_read_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_noise_spsvirgo_data_read_t))){
		HcuErrorPrint("SPSVIRGO: Receive message error!\n");
		zHcuRunErrCnt[TASK_ID_SPSVIRGO]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);
	currentSensorEqpId = rcv.equId;

	//Equipment Id can not be 0
	if ((currentSensorEqpId <=0) || (rcv.cmdId != L3CI_noise)){
		HcuErrorPrint("SPSVIRGO: Receive message with cmdId or EqpId error!\n");
		zHcuRunErrCnt[TASK_ID_SPSVIRGO]++;
		return FAILURE;
	}

	//对硬件进行操作
	msg_struct_spsvirgo_noise_data_report_t snd;
	memset(&snd, 0, sizeof(msg_struct_spsvirgo_noise_data_report_t));
	snd.length = sizeof(msg_struct_spsvirgo_noise_data_report_t);
	if (SPSVIRGO_ACTIVE_CHOICE_NOISE_FINAL == SPSVIRGO_ACTIVE_CHOICE_NOISE_MICROPHONE)
	{
		snd.noise.dataFormat = CLOUD_SENSOR_DATA_FOMAT_INT_ONLY;
		if (func_microphone_noise_apower_value_read(snd.noise.noiseValue) == FAILURE){
			HcuErrorPrint("SPSVIRGO: Receive message with noiseValue error!\n");
			zHcuRunErrCnt[TASK_ID_SPSVIRGO]++;
		}
	}

	//读取RS232接口，获取结果
	else if (SPSVIRGO_ACTIVE_CHOICE_NOISE_FINAL == SPSVIRGO_ACTIVE_CHOICE_NOISE_ZSY)
	{
		//对发送数据进行编码


		//发送串口指令

		//等待短时
		hcu_usleep(10);

		//读取串口数据

		//解码

		//检查返回结果

		//赋值

	}

	else
	{
		HcuErrorPrint("SPSVIRGO: Select wrong par error for SPSVIRGO_ACTIVE_CHOICE_NOISE_FINAL!\n");
		zHcuRunErrCnt[TASK_ID_SPSVIRGO]++;
		return FAILURE;
	}

	//准备发送数据给L3
	snd.usercmdid = rcv.cmdId;
	snd.noise.equipid = rcv.equId;
	snd.cmdIdBackType = rcv.cmdIdBackType;
	snd.noise.timeStamp = time(0);
	switch(rcv.optId){
	case L3PO_noise_data_req:
		snd.useroptid = L3PO_noise_data_report;
		break;
	case L3PO_noise_set_switch:
		snd.useroptid = L3PO_noise_read_switch;
		break;
	case L3PO_noise_set_modbus_address:
		snd.useroptid = L3PO_noise_read_modbus_address;
		break;
	case L3PO_noise_set_work_cycle:
		snd.useroptid = L3PO_noise_read_work_cycle;
		break;
	case L3PO_noise_set_sample_cycle:
		snd.useroptid = L3PO_noise_read_sample_cycle;
		break;
	case L3PO_noise_set_sample_number:
		snd.useroptid = L3PO_noise_read_sample_number;
		break;
	case L3PO_noise_data_report:
		snd.useroptid = L3PO_noise_data_report;
		break;
	case L3PO_noise_read_switch:
		snd.useroptid = L3PO_noise_read_switch;
		break;
	case L3PO_noise_read_modbus_address:
		snd.useroptid = L3PO_noise_read_modbus_address;
		break;
	case L3PO_noise_read_work_cycle:
		snd.useroptid = L3PO_noise_read_work_cycle;
		break;
	case L3PO_noise_read_sample_cycle:
		snd.useroptid = L3PO_noise_read_sample_cycle;
		break;
	case L3PO_noise_read_sample_number:
		snd.useroptid = L3PO_noise_read_sample_number;
		break;
	default:
		HcuErrorPrint("NOISE: Error operation code received!\n");
		zHcuRunErrCnt[TASK_ID_SPSVIRGO]++;
		return FAILURE;
		break;
	}
	snd.noise.gps.gpsx = zHcuGpsPosInfo.gpsX;
	snd.noise.gps.gpsy = zHcuGpsPosInfo.gpsY;
	snd.noise.gps.gpsz = zHcuGpsPosInfo.gpsZ;
	//Remaining data to be filled
	ret = hcu_message_send(MSG_ID_SPSVIRGO_NOISE_DATA_REPORT, TASK_ID_NOISE, TASK_ID_SPSVIRGO, &snd, snd.length);
	if (ret == FAILURE){
		zHcuRunErrCnt[TASK_ID_SPSVIRGO]++;
		HcuErrorPrint("SPSVIRGO: Send message error, TASK [%s] to TASK[%s]!\n", zHcuTaskNameList[TASK_ID_SPSVIRGO], zHcuTaskNameList[TASK_ID_NOISE]);
		return FAILURE;
	}

	//恢复当前传感器ID的初始值
	currentSensorEqpId = 0;
	return SUCCESS;

}

OPSTAT fsm_spsvirgo_noise_control_cmd(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{

	return SUCCESS;
}

