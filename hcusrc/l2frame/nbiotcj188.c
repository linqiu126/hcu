/*
 * nbiotcj188.c
 *
 *  Created on: 2016年7月20日
 *      Author: hitpony
 */

#include "nbiotcj188.h"

#include "../l0service/timer.h"
#include "../l0service/trace.h"
#include "../l1com/l1comdef.h"

/*
** FSM of the NBIOTCJ188
*/
FsmStateItem_t HcuFsmNbiotcj188[] =
{
    //MessageId                 //State                   		 		//Function
	//启始点，固定定义，不要改动, 使用ENTRY/END，意味者MSGID肯定不可能在某个高位区段中；考虑到所有任务共享MsgId，即使分段，也无CloudDataSendBuf法实现
	//完全是为了给任务一个初始化的机会，按照状态转移机制，该函数不具备启动的机会，因为任务初始化后自动到FSM_STATE_IDLE
	//如果没有必要进行初始化，可以设置为NULL
	{MSG_ID_ENTRY,       		FSM_STATE_ENTRY,            			fsm_nbiotcj188_task_entry}, //Starting

	//System level initialization, only controlled by HCU-MAIN
    {MSG_ID_COM_INIT,       	FSM_STATE_IDLE,            				fsm_nbiotcj188_init},
    {MSG_ID_COM_RESTART,		FSM_STATE_IDLE,            				fsm_nbiotcj188_restart},
    {MSG_ID_COM_INIT_FEEDBACK,	FSM_STATE_IDLE,            				fsm_com_do_nothing},

	//Task level initialization
    {MSG_ID_COM_INIT,       	FSM_STATE_NBIOTCJ188_INITIED,            	fsm_nbiotcj188_init},
    {MSG_ID_COM_RESTART,		FSM_STATE_NBIOTCJ188_INITIED,            	fsm_nbiotcj188_restart},
    {MSG_ID_COM_INIT_FEEDBACK,	FSM_STATE_NBIOTCJ188_INITIED,            	fsm_com_do_nothing},

	//Offline working, 定时重新启动链路，但不接受任何L3消息
    {MSG_ID_COM_RESTART,        FSM_STATE_NBIOTCJ188_OFFLINE,            	fsm_nbiotcj188_restart},
    {MSG_ID_COM_INIT_FEEDBACK,	FSM_STATE_NBIOTCJ188_OFFLINE,            	fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT,     FSM_STATE_NBIOTCJ188_OFFLINE,       		fsm_com_heart_beat_rcv},
	{MSG_ID_COM_HEART_BEAT_FB,  FSM_STATE_NBIOTCJ188_OFFLINE,       		fsm_com_do_nothing},
	{MSG_ID_COM_TIME_OUT,       FSM_STATE_NBIOTCJ188_OFFLINE,            	fsm_nbiotcj188_time_out},

    //Online working， 定时检查链路，并安排离线数据的及时上传
    {MSG_ID_COM_RESTART,        			FSM_STATE_NBIOTCJ188_ONLINE, 	fsm_nbiotcj188_restart},
    {MSG_ID_COM_INIT_FEEDBACK,				FSM_STATE_NBIOTCJ188_ONLINE,	fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT,     			FSM_STATE_NBIOTCJ188_ONLINE,    fsm_com_heart_beat_rcv},
	{MSG_ID_COM_HEART_BEAT_FB,  			FSM_STATE_NBIOTCJ188_ONLINE,    fsm_com_do_nothing},
	{MSG_ID_COM_TIME_OUT,       			FSM_STATE_NBIOTCJ188_ONLINE, 	fsm_nbiotcj188_time_out},
	{MSG_ID_IWM_NBIOTCJ188_DATA_RESP,   	FSM_STATE_NBIOTCJ188_ONLINE, 	fsm_nbiotcj188_iwm_data_resp},
	{MSG_ID_IWM_NBIOTCJ188_CONTROL_FB,   	FSM_STATE_NBIOTCJ188_ONLINE, 	fsm_nbiotcj188_iwm_contrl_fb},
	{MSG_ID_IHM_NBIOTCJ188_DATA_RESP,   	FSM_STATE_NBIOTCJ188_ONLINE, 	fsm_nbiotcj188_ihm_data_resp},
	{MSG_ID_IHM_NBIOTCJ188_CONTROL_FB,   	FSM_STATE_NBIOTCJ188_ONLINE, 	fsm_nbiotcj188_ihm_contrl_fb},
	{MSG_ID_IGM_NBIOTCJ188_DATA_RESP,   	FSM_STATE_NBIOTCJ188_ONLINE, 	fsm_nbiotcj188_igm_data_resp},
	{MSG_ID_IGM_NBIOTCJ188_CONTROL_FB,   	FSM_STATE_NBIOTCJ188_ONLINE, 	fsm_nbiotcj188_igm_contrl_fb},
	{MSG_ID_IPM_NBIOTCJ188_DATA_RESP,   	FSM_STATE_NBIOTCJ188_ONLINE, 	fsm_nbiotcj188_ipm_data_resp},
	{MSG_ID_IPM_NBIOTCJ188_CONTROL_FB,   	FSM_STATE_NBIOTCJ188_ONLINE, 	fsm_nbiotcj188_ipm_contrl_fb},

	//Online working，这里不再考虑不同的物理链接导致的差异性链路。从Linux系统来看，WIFI/ETHERNET/USBNET/34G都是同一种连接
	{MSG_ID_ETHERNET_NBIOTCJ188_DATA_RX,    FSM_STATE_NBIOTCJ188_ONLINE, 	fsm_nbiotcj188_ethernet_data_rx},

    //结束点，固定定义，不要改动
    {MSG_ID_END,            	FSM_STATE_END,             				NULL},  //Ending
};

//Global variables
extern HcuSysEngParTable_t zHcuSysEngPar; //全局工程参数控制表

//Main Entry
//Input parameter would be useless, but just for similar structure purpose
OPSTAT fsm_nbiotcj188_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//除了对全局变量进行操作之外，尽量不要做其它操作，因为该函数将被主任务/线程调用，不是本任务/线程调用
	//该API就是给本任务一个提早介入的入口，可以帮着做些测试性操作
	if (FsmSetState(TASK_ID_NBIOTCJ188, FSM_STATE_IDLE) == FAILURE){
		HcuErrorPrint("NBIOTCJ188: Error Set FSM State at fsm_nbiotcj188_task_entry\n");}
	return SUCCESS;
}

OPSTAT fsm_nbiotcj188_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;

	if ((src_id > TASK_ID_MIN) &&(src_id < TASK_ID_MAX)){
		//Send back MSG_ID_COM_INIT_FEEDBACK to SVRCON
		msg_struct_com_init_feedback_t snd0;
		memset(&snd0, 0, sizeof(msg_struct_com_init_feedback_t));
		snd0.length = sizeof(msg_struct_com_init_feedback_t);

		//to avoid all task send out the init fb msg at the same time which lead to msgque get stuck
		hcu_usleep(dest_id*HCU_DURATION_OF_INIT_FB_WAIT_MAX);

		ret = hcu_message_send(MSG_ID_COM_INIT_FEEDBACK, src_id, TASK_ID_NBIOTCJ188, &snd0, snd0.length);
		if (ret == FAILURE){
			HcuErrorPrint("NBIOTCJ188: Send message error, TASK [%s] to TASK[%s]!\n", zHcuTaskInfo[TASK_ID_NBIOTCJ188].taskName, zHcuTaskInfo[src_id].taskName);
			return FAILURE;
		}
	}

	//收到初始化消息后，进入初始化状态
	if (FsmSetState(TASK_ID_NBIOTCJ188, FSM_STATE_NBIOTCJ188_INITED) == FAILURE){
		HcuErrorPrint("NBIOTCJ188: Error Set FSM State!\n");
		return FAILURE;
	}

	//初始化硬件接口
	if (func_nbiotcj188_int_init() == FAILURE){
		HcuErrorPrint("NBIOTCJ188: Error initialize interface!\n");
		return FAILURE;
	}

	//Global Variables
	zHcuRunErrCnt[TASK_ID_NBIOTCJ188] = 0;

	//启动周期性定时器
	ret = hcu_timer_start(TASK_ID_NBIOTCJ188, TIMER_ID_1S_NBIOTCJ188_PERIOD_LINK_HEART_BEAT, zHcuSysEngPar.timer.nbiotcj188HbTimer, TIMER_TYPE_PERIOD, TIMER_RESOLUTION_1S);
	if (ret == FAILURE){
		zHcuRunErrCnt[TASK_ID_NBIOTCJ188]++;
		HcuErrorPrint("NBIOTCJ188: Error start timer!\n");
		return FAILURE;
	}

	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_NBIOTCJ188, FSM_STATE_NBIOTCJ188_OFFLINE) == FAILURE){
		zHcuRunErrCnt[TASK_ID_NBIOTCJ188]++;
		HcuErrorPrint("NBIOTCJ188: Error Set FSM State!\n");
		return FAILURE;
	}
	if ((zHcuSysEngPar.debugMode & HCU_TRACE_DEBUG_FAT_ON) != FALSE){
		HcuDebugPrint("NBIOTCJ188: Enter FSM_STATE_NBIOTCJ188_ACTIVED status, Keeping refresh here!\n");
	}

	return SUCCESS;
}

OPSTAT fsm_nbiotcj188_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HcuErrorPrint("NBIOTCJ188: Internal error counter reach DEAD level, SW-RESTART soon!\n");
	zHcuGlobalCounter.restartCnt++;
	fsm_nbiotcj188_init(0, 0, NULL, 0);
	return SUCCESS;
}

OPSTAT func_nbiotcj188_int_init(void)
{
	return SUCCESS;
}

//TIME_OUT负责链路的重连/非在线检查/奇异状态恢复等情况
OPSTAT fsm_nbiotcj188_time_out(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;

	//Receive message and copy to local variable
	msg_struct_com_time_out_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_com_time_out_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_com_time_out_t))){
		HcuErrorPrint("NBIOTCJ188: Receive message error!\n");
		zHcuRunErrCnt[TASK_ID_NBIOTCJ188]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//钩子在此处，检查zHcuRunErrCnt[TASK_ID_NBIOTCJ188]是否超限
	if (zHcuRunErrCnt[TASK_ID_NBIOTCJ188] > HCU_RUN_ERROR_LEVEL_3_CRITICAL){
		//减少重复RESTART的概率
		zHcuRunErrCnt[TASK_ID_NBIOTCJ188] = zHcuRunErrCnt[TASK_ID_NBIOTCJ188] - HCU_RUN_ERROR_LEVEL_3_CRITICAL;
		msg_struct_com_restart_t snd0;
		memset(&snd0, 0, sizeof(msg_struct_com_restart_t));
		snd0.length = sizeof(msg_struct_com_restart_t);
		ret = hcu_message_send(MSG_ID_COM_RESTART, TASK_ID_NBIOTCJ188, TASK_ID_NBIOTCJ188, &snd0, snd0.length);
		if (ret == FAILURE){
			zHcuRunErrCnt[TASK_ID_NBIOTCJ188]++;
			HcuErrorPrint("NBIOTCJ188: Send message error, TASK [%s] to TASK[%s]!\n", zHcuTaskInfo[TASK_ID_NBIOTCJ188].taskName, zHcuTaskInfo[TASK_ID_NBIOTCJ188].taskName);
			return FAILURE;
		}
	}

	//定时长时钟进行链路检测的
	if ((rcv.timeId == TIMER_ID_1S_NBIOTCJ188_PERIOD_LINK_HEART_BEAT) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		ret = func_nbiotcj188_time_out_period();
	}

	//定时短时钟进行离线数据回送，这里暂时没有啥用途，但基于跟NBIOTCJ188模块的一致性，还是放在这里
	else if ((rcv.timeId == TIMER_ID_1S_NBIOTCJ188_SEND_DATA_BACK) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		ret = func_nbiotcj188_time_out_sendback_offline_data();
	}

	//这里的ret=FAILURE並不算严重，只不过造成状态机返回差错而已，并不会造成程序崩溃和数据混乱，所以只是程序的自我保护而已
	return ret;
}

OPSTAT fsm_nbiotcj188_iwm_data_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;
	msg_struct_iwm_nbiotcj188_data_resp_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_iwm_nbiotcj188_data_resp_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_iwm_nbiotcj188_data_resp_t))){
		HcuErrorPrint("NBIOTCJ188: Receive IWM message error!\n");
		zHcuRunErrCnt[TASK_ID_NBIOTCJ188]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//参数检查
	if ((rcv.equtype < HCU_NBIOT_CJ188_T_TYPE_WATER_METER_MIN) || (rcv.equtype > HCU_NBIOT_CJ188_T_TYPE_WATER_METER_MAX) || (rcv.iwmHead.timestamp <=0)){
		HcuErrorPrint("NBIOTCJ188: Receive invalid message content from sensor!\n");
		zHcuRunErrCnt[TASK_ID_NBIOTCJ188]++;
		return FAILURE;
	}

	//发送数据给后台，这里不打算重新激活链路，而是由守护任务自动完成
	if (FsmGetState(TASK_ID_NBIOTCJ188) != FSM_STATE_NBIOTCJ188_ONLINE){
		zHcuRunErrCnt[TASK_ID_NBIOTCJ188]++;
		HcuErrorPrint("NBIOTCJ188: Error send data to cloud, IWM with cloud-link is not under right state!\n");
		return FAILURE;
	}

	//初始化变量
	NbiotCj188BhItfComElement_t input;
	memset(&input, 0, sizeof(NbiotCj188BhItfComElement_t));
	CloudDataSendBuf_t buf;
	memset(&buf, 0, sizeof(CloudDataSendBuf_t));

	//数据拷贝
	memcpy(&(input.head), &(rcv.iwmHead), sizeof(sensor_general_cj188_control_head_t));
	memcpy(&(input.data), &(rcv.iwmData), sizeof(sensor_general_cj188_data_element_t));
	input.equtype = rcv.equtype;
	input.billtodayaccuvolume = rcv.billtodayaccuvolume;
	input.billtodayaccuvolumeunit = rcv.billtodayaccuvolumeunit;
	input.length = sizeof(NbiotCj188BhItfComElement_t);

	//打包数据
	if (func_nbiotcj188_ul_msg_pack(&input, &buf) == FAILURE){
		HcuErrorPrint("NBIOTCJ188: Package message error!\n");
		zHcuRunErrCnt[TASK_ID_NBIOTCJ188]++;
		return FAILURE;
	}

	//Send out
	ret = func_nbiotcj188_send_data_to_cloud(&buf);
	if ( ret == FAILURE){
		zHcuRunErrCnt[TASK_ID_NBIOTCJ188]++;
		HcuErrorPrint("NBIOTCJ188: Package message error!\n");
		return FAILURE;
	}

	//结束
	if ((zHcuSysEngPar.debugMode & HCU_TRACE_DEBUG_NOR_ON) != FALSE){
		HcuDebugPrint("NBIOTCJ188: Online state, send instance/period IWM to cloud success!\n"); }
	//State no change
	return SUCCESS;
}

OPSTAT fsm_nbiotcj188_iwm_contrl_fb(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;
	msg_struct_iwm_nbiotcj188_control_fb_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_iwm_nbiotcj188_control_fb_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_iwm_nbiotcj188_control_fb_t))){
		HcuErrorPrint("NBIOTCJ188: Receive IWM message error!\n");
		zHcuRunErrCnt[TASK_ID_NBIOTCJ188]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//参数检查
	if ((rcv.equtype < HCU_NBIOT_CJ188_T_TYPE_WATER_METER_MIN) || (rcv.equtype > HCU_NBIOT_CJ188_T_TYPE_WATER_METER_MAX) || (rcv.iwmHead.timestamp <=0)){
		HcuErrorPrint("NBIOTCJ188: Receive invalid message content from sensor!\n");
		zHcuRunErrCnt[TASK_ID_NBIOTCJ188]++;
		return FAILURE;
	}

	//发送数据给后台，这里不打算重新激活链路，而是由守护任务自动完成
	if (FsmGetState(TASK_ID_NBIOTCJ188) != FSM_STATE_NBIOTCJ188_ONLINE){
		zHcuRunErrCnt[TASK_ID_NBIOTCJ188]++;
		HcuErrorPrint("NBIOTCJ188: Error send data to cloud, IWM with cloud-link is not under right state!\n");
		return FAILURE;
	}

	//初始化变量
	NbiotCj188BhItfComElement_t input;
	memset(&input, 0, sizeof(NbiotCj188BhItfComElement_t));
	CloudDataSendBuf_t buf;
	memset(&buf, 0, sizeof(CloudDataSendBuf_t));

	//数据拷贝
	memcpy(&(input.head), &(rcv.iwmHead), sizeof(sensor_general_cj188_control_head_t));
	memcpy(&(input.data), &(rcv.iwmData), sizeof(sensor_general_cj188_data_element_t));
	input.equtype = rcv.equtype;
	input.billtodayaccuvolume = rcv.billtodayaccuvolume;
	input.billtodayaccuvolumeunit = rcv.billtodayaccuvolumeunit;
	input.length = sizeof(NbiotCj188BhItfComElement_t);

	//打包数据
	if (func_nbiotcj188_ul_msg_pack(&input, &buf) == FAILURE){
		HcuErrorPrint("NBIOTCJ188: Package message error!\n");
		zHcuRunErrCnt[TASK_ID_NBIOTCJ188]++;
		return FAILURE;
	}

	//Send out
	ret = func_nbiotcj188_send_data_to_cloud(&buf);
	if ( ret == FAILURE){
		zHcuRunErrCnt[TASK_ID_NBIOTCJ188]++;
		HcuErrorPrint("NBIOTCJ188: Package message error!\n");
		return FAILURE;
	}

	//结束
	if ((zHcuSysEngPar.debugMode & HCU_TRACE_DEBUG_NOR_ON) != FALSE){
		HcuDebugPrint("NBIOTCJ188: Online state, send instance/period IWM to cloud success!\n"); }
	//State no change
	return SUCCESS;
}

OPSTAT fsm_nbiotcj188_ihm_data_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;
	msg_struct_ihm_nbiotcj188_data_resp_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_ihm_nbiotcj188_data_resp_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_ihm_nbiotcj188_data_resp_t))){
		HcuErrorPrint("NBIOTCJ188: Receive IHM message error!\n");
		zHcuRunErrCnt[TASK_ID_NBIOTCJ188]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//参数检查
	if ((rcv.equtype < HCU_NBIOT_CJ188_T_TYPE_HEAT_METER_MIN) || (rcv.equtype > HCU_NBIOT_CJ188_T_TYPE_HEAT_METER_MAX) || (rcv.ihmHead.timestamp <=0)){
		HcuErrorPrint("NBIOTCJ188: Receive invalid message content from sensor!\n");
		zHcuRunErrCnt[TASK_ID_NBIOTCJ188]++;
		return FAILURE;
	}

	//发送数据给后台，这里不打算重新激活链路，而是由守护任务自动完成
	if (FsmGetState(TASK_ID_NBIOTCJ188) != FSM_STATE_NBIOTCJ188_ONLINE){
		zHcuRunErrCnt[TASK_ID_NBIOTCJ188]++;
		HcuErrorPrint("NBIOTCJ188: Error send data to cloud, IHM with cloud-link is not under right state!\n");
		return FAILURE;
	}

	//初始化变量
	NbiotCj188BhItfComElement_t input;
	memset(&input, 0, sizeof(NbiotCj188BhItfComElement_t));
	CloudDataSendBuf_t buf;
	memset(&buf, 0, sizeof(CloudDataSendBuf_t));

	//数据拷贝
	memcpy(&(input.head), &(rcv.ihmHead), sizeof(sensor_general_cj188_control_head_t));
	memcpy(&(input.data), &(rcv.ihmData), sizeof(sensor_general_cj188_data_element_t));
	input.equtype = rcv.equtype;
	input.currentheat = rcv.currentheat;
	input.currentheatunit = rcv.currentheatunit;
	input.heatpower = rcv.heatpower;
	input.heatpowerunit = rcv.heatpowerunit;
	input.billtodayheat = rcv.billtodayheat;
	input.billtodayheatunit = rcv.billtodayheatunit;
	input.length = sizeof(NbiotCj188BhItfComElement_t);

	//打包数据
	if (func_nbiotcj188_ul_msg_pack(&input, &buf) == FAILURE){
		HcuErrorPrint("NBIOTCJ188: Package message error!\n");
		zHcuRunErrCnt[TASK_ID_NBIOTCJ188]++;
		return FAILURE;
	}

	//Send out
	ret = func_nbiotcj188_send_data_to_cloud(&buf);
	if ( ret == FAILURE){
		zHcuRunErrCnt[TASK_ID_NBIOTCJ188]++;
		HcuErrorPrint("NBIOTCJ188: Package message error!\n");
		return FAILURE;
	}

	//结束
	if ((zHcuSysEngPar.debugMode & HCU_TRACE_DEBUG_NOR_ON) != FALSE){
		HcuDebugPrint("NBIOTCJ188: Online state, send instance/period IHM to cloud success!\n"); }
	//State no change
	return SUCCESS;
}

OPSTAT fsm_nbiotcj188_ihm_contrl_fb(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;
	msg_struct_ihm_nbiotcj188_control_fb_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_ihm_nbiotcj188_control_fb_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_ihm_nbiotcj188_control_fb_t))){
		HcuErrorPrint("NBIOTCJ188: Receive IHM message error!\n");
		zHcuRunErrCnt[TASK_ID_NBIOTCJ188]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//参数检查
	if ((rcv.equtype < HCU_NBIOT_CJ188_T_TYPE_HEAT_METER_MIN) || (rcv.equtype > HCU_NBIOT_CJ188_T_TYPE_HEAT_METER_MAX) || (rcv.ihmHead.timestamp <=0)){
		HcuErrorPrint("NBIOTCJ188: Receive invalid message content from sensor!\n");
		zHcuRunErrCnt[TASK_ID_NBIOTCJ188]++;
		return FAILURE;
	}

	//发送数据给后台，这里不打算重新激活链路，而是由守护任务自动完成
	if (FsmGetState(TASK_ID_NBIOTCJ188) != FSM_STATE_NBIOTCJ188_ONLINE){
		zHcuRunErrCnt[TASK_ID_NBIOTCJ188]++;
		HcuErrorPrint("NBIOTCJ188: Error send data to cloud, IHM with cloud-link is not under right state!\n");
		return FAILURE;
	}

	//初始化变量
	NbiotCj188BhItfComElement_t input;
	memset(&input, 0, sizeof(NbiotCj188BhItfComElement_t));
	CloudDataSendBuf_t buf;
	memset(&buf, 0, sizeof(CloudDataSendBuf_t));

	//数据拷贝
	memcpy(&(input.head), &(rcv.ihmHead), sizeof(sensor_general_cj188_control_head_t));
	memcpy(&(input.data), &(rcv.ihmData), sizeof(sensor_general_cj188_data_element_t));
	input.equtype = rcv.equtype;
	input.currentheat = rcv.currentheat;
	input.currentheatunit = rcv.currentheatunit;
	input.heatpower = rcv.heatpower;
	input.heatpowerunit = rcv.heatpowerunit;
	input.billtodayheat = rcv.billtodayheat;
	input.billtodayheatunit = rcv.billtodayheatunit;
	input.length = sizeof(NbiotCj188BhItfComElement_t);

	//打包数据
	if (func_nbiotcj188_ul_msg_pack(&input, &buf) == FAILURE){
		HcuErrorPrint("NBIOTCJ188: Package message error!\n");
		zHcuRunErrCnt[TASK_ID_NBIOTCJ188]++;
		return FAILURE;
	}

	//Send out
	ret = func_nbiotcj188_send_data_to_cloud(&buf);
	if ( ret == FAILURE){
		zHcuRunErrCnt[TASK_ID_NBIOTCJ188]++;
		HcuErrorPrint("NBIOTCJ188: Package message error!\n");
		return FAILURE;
	}

	//结束
	if ((zHcuSysEngPar.debugMode & HCU_TRACE_DEBUG_NOR_ON) != FALSE){
		HcuDebugPrint("NBIOTCJ188: Online state, send instance/period IHM to cloud success!\n"); }
	//State no change
	return SUCCESS;
}

OPSTAT fsm_nbiotcj188_igm_data_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;
	msg_struct_igm_nbiotcj188_data_resp_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_igm_nbiotcj188_data_resp_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_igm_nbiotcj188_data_resp_t))){
		HcuErrorPrint("NBIOTCJ188: Receive IGM message error!\n");
		zHcuRunErrCnt[TASK_ID_NBIOTCJ188]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//参数检查
	if ((rcv.equtype < HCU_NBIOT_CJ188_T_TYPE_GAS_METER_MIN) || (rcv.equtype > HCU_NBIOT_CJ188_T_TYPE_GAS_METER_MAX) || (rcv.igmHead.timestamp <=0)){
		HcuErrorPrint("NBIOTCJ188: Receive invalid message content from sensor!\n");
		zHcuRunErrCnt[TASK_ID_NBIOTCJ188]++;
		return FAILURE;
	}

	//发送数据给后台，这里不打算重新激活链路，而是由守护任务自动完成
	if (FsmGetState(TASK_ID_NBIOTCJ188) != FSM_STATE_NBIOTCJ188_ONLINE){
		zHcuRunErrCnt[TASK_ID_NBIOTCJ188]++;
		HcuErrorPrint("NBIOTCJ188: Error send data to cloud, IGM with cloud-link is not under right state!\n");
		return FAILURE;
	}

	//初始化变量
	NbiotCj188BhItfComElement_t input;
	memset(&input, 0, sizeof(NbiotCj188BhItfComElement_t));
	CloudDataSendBuf_t buf;
	memset(&buf, 0, sizeof(CloudDataSendBuf_t));

	//数据拷贝
	memcpy(&(input.head), &(rcv.igmHead), sizeof(sensor_general_cj188_control_head_t));
	memcpy(&(input.data), &(rcv.igmData), sizeof(sensor_general_cj188_data_element_t));
	input.equtype = rcv.equtype;
	input.billtodayaccuvolume = rcv.billtodayaccuvolume;
	input.billtodayaccuvolumeunit = rcv.billtodayaccuvolumeunit;
	input.length = sizeof(NbiotCj188BhItfComElement_t);

	//打包数据
	if (func_nbiotcj188_ul_msg_pack(&input, &buf) == FAILURE){
		HcuErrorPrint("NBIOTCJ188: Package message error!\n");
		zHcuRunErrCnt[TASK_ID_NBIOTCJ188]++;
		return FAILURE;
	}

	//Send out
	ret = func_nbiotcj188_send_data_to_cloud(&buf);
	if ( ret == FAILURE){
		zHcuRunErrCnt[TASK_ID_NBIOTCJ188]++;
		HcuErrorPrint("NBIOTCJ188: Package message error!\n");
		return FAILURE;
	}

	//结束
	if ((zHcuSysEngPar.debugMode & HCU_TRACE_DEBUG_NOR_ON) != FALSE){
		HcuDebugPrint("NBIOTCJ188: Online state, send instance/period IGM to cloud success!\n"); }
	//State no change
	return SUCCESS;
}

OPSTAT fsm_nbiotcj188_igm_contrl_fb(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;
	msg_struct_igm_nbiotcj188_control_fb_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_igm_nbiotcj188_control_fb_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_igm_nbiotcj188_control_fb_t))){
		HcuErrorPrint("NBIOTCJ188: Receive IGM message error!\n");
		zHcuRunErrCnt[TASK_ID_NBIOTCJ188]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//参数检查
	if ((rcv.equtype < HCU_NBIOT_CJ188_T_TYPE_GAS_METER_MIN) || (rcv.equtype > HCU_NBIOT_CJ188_T_TYPE_GAS_METER_MAX) || (rcv.igmHead.timestamp <=0)){
		HcuErrorPrint("NBIOTCJ188: Receive invalid message content from sensor!\n");
		zHcuRunErrCnt[TASK_ID_NBIOTCJ188]++;
		return FAILURE;
	}

	//发送数据给后台，这里不打算重新激活链路，而是由守护任务自动完成
	if (FsmGetState(TASK_ID_NBIOTCJ188) != FSM_STATE_NBIOTCJ188_ONLINE){
		zHcuRunErrCnt[TASK_ID_NBIOTCJ188]++;
		HcuErrorPrint("NBIOTCJ188: Error send data to cloud, IGM with cloud-link is not under right state!\n");
		return FAILURE;
	}

	//初始化变量
	NbiotCj188BhItfComElement_t input;
	memset(&input, 0, sizeof(NbiotCj188BhItfComElement_t));
	CloudDataSendBuf_t buf;
	memset(&buf, 0, sizeof(CloudDataSendBuf_t));

	//数据拷贝
	memcpy(&(input.head), &(rcv.igmHead), sizeof(sensor_general_cj188_control_head_t));
	memcpy(&(input.data), &(rcv.igmData), sizeof(sensor_general_cj188_data_element_t));
	input.equtype = rcv.equtype;
	input.billtodayaccuvolume = rcv.billtodayaccuvolume;
	input.billtodayaccuvolumeunit = rcv.billtodayaccuvolumeunit;
	input.length = sizeof(NbiotCj188BhItfComElement_t);

	//打包数据
	if (func_nbiotcj188_ul_msg_pack(&input, &buf) == FAILURE){
		HcuErrorPrint("NBIOTCJ188: Package message error!\n");
		zHcuRunErrCnt[TASK_ID_NBIOTCJ188]++;
		return FAILURE;
	}

	//Send out
	ret = func_nbiotcj188_send_data_to_cloud(&buf);
	if ( ret == FAILURE){
		zHcuRunErrCnt[TASK_ID_NBIOTCJ188]++;
		HcuErrorPrint("NBIOTCJ188: Package message error!\n");
		return FAILURE;
	}

	//结束
	if ((zHcuSysEngPar.debugMode & HCU_TRACE_DEBUG_NOR_ON) != FALSE){
		HcuDebugPrint("NBIOTCJ188: Online state, send instance/period IGM to cloud success!\n"); }
	//State no change
	return SUCCESS;
}

OPSTAT fsm_nbiotcj188_ipm_data_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;
	msg_struct_ipm_nbiotcj188_data_resp_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_ipm_nbiotcj188_data_resp_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_ipm_nbiotcj188_data_resp_t))){
		HcuErrorPrint("NBIOTCJ188: Receive IPM message error!\n");
		zHcuRunErrCnt[TASK_ID_NBIOTCJ188]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//参数检查
	if ((rcv.equtype < HCU_NBIOT_CJ188_T_TYPE_POWER_METER_MIN) || (rcv.equtype > HCU_NBIOT_CJ188_T_TYPE_POWER_METER_MAX) || (rcv.ipmHead.timestamp <=0)){
		HcuErrorPrint("NBIOTCJ188: Receive invalid message content from sensor!\n");
		zHcuRunErrCnt[TASK_ID_NBIOTCJ188]++;
		return FAILURE;
	}

	//发送数据给后台，这里不打算重新激活链路，而是由守护任务自动完成
	if (FsmGetState(TASK_ID_NBIOTCJ188) != FSM_STATE_NBIOTCJ188_ONLINE){
		zHcuRunErrCnt[TASK_ID_NBIOTCJ188]++;
		HcuErrorPrint("NBIOTCJ188: Error send data to cloud, IPM with cloud-link is not under right state!\n");
		return FAILURE;
	}

	//初始化变量
	NbiotCj188BhItfComElement_t input;
	memset(&input, 0, sizeof(NbiotCj188BhItfComElement_t));
	CloudDataSendBuf_t buf;
	memset(&buf, 0, sizeof(CloudDataSendBuf_t));

	//数据拷贝
	memcpy(&(input.head), &(rcv.ipmHead), sizeof(sensor_general_cj188_control_head_t));
	memcpy(&(input.data), &(rcv.ipmData), sizeof(sensor_general_cj188_data_element_t));
	input.equtype = rcv.equtype;
	input.billtodayaccuvolume = rcv.billtodayaccuvolume;
	input.billtodayaccuvolumeunit = rcv.billtodayaccuvolumeunit;
	input.length = sizeof(NbiotCj188BhItfComElement_t);

	//打包数据
	if (func_nbiotcj188_ul_msg_pack(&input, &buf) == FAILURE){
		HcuErrorPrint("NBIOTCJ188: Package message error!\n");
		zHcuRunErrCnt[TASK_ID_NBIOTCJ188]++;
		return FAILURE;
	}

	//Send out
	ret = func_nbiotcj188_send_data_to_cloud(&buf);
	if ( ret == FAILURE){
		zHcuRunErrCnt[TASK_ID_NBIOTCJ188]++;
		HcuErrorPrint("NBIOTCJ188: Package message error!\n");
		return FAILURE;
	}

	//结束
	if ((zHcuSysEngPar.debugMode & HCU_TRACE_DEBUG_NOR_ON) != FALSE){
		HcuDebugPrint("NBIOTCJ188: Online state, send instance/period IPM to cloud success!\n"); }
	//State no change
	return SUCCESS;
}

OPSTAT fsm_nbiotcj188_ipm_contrl_fb(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;
	msg_struct_ipm_nbiotcj188_control_fb_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_ipm_nbiotcj188_control_fb_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_ipm_nbiotcj188_control_fb_t))){
		HcuErrorPrint("NBIOTCJ188: Receive IPM message error!\n");
		zHcuRunErrCnt[TASK_ID_NBIOTCJ188]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//参数检查
	if ((rcv.equtype < HCU_NBIOT_CJ188_T_TYPE_POWER_METER_MIN) || (rcv.equtype > HCU_NBIOT_CJ188_T_TYPE_POWER_METER_MAX) || (rcv.ipmHead.timestamp <=0)){
		HcuErrorPrint("NBIOTCJ188: Receive invalid message content from sensor!\n");
		zHcuRunErrCnt[TASK_ID_NBIOTCJ188]++;
		return FAILURE;
	}

	//发送数据给后台，这里不打算重新激活链路，而是由守护任务自动完成
	if (FsmGetState(TASK_ID_NBIOTCJ188) != FSM_STATE_NBIOTCJ188_ONLINE){
		zHcuRunErrCnt[TASK_ID_NBIOTCJ188]++;
		HcuErrorPrint("NBIOTCJ188: Error send data to cloud, IPM with cloud-link is not under right state!\n");
		return FAILURE;
	}

	//初始化变量
	NbiotCj188BhItfComElement_t input;
	memset(&input, 0, sizeof(NbiotCj188BhItfComElement_t));
	CloudDataSendBuf_t buf;
	memset(&buf, 0, sizeof(CloudDataSendBuf_t));

	//数据拷贝
	memcpy(&(input.head), &(rcv.ipmHead), sizeof(sensor_general_cj188_control_head_t));
	memcpy(&(input.data), &(rcv.ipmData), sizeof(sensor_general_cj188_data_element_t));
	input.equtype = rcv.equtype;
	input.billtodayaccuvolume = rcv.billtodayaccuvolume;
	input.billtodayaccuvolumeunit = rcv.billtodayaccuvolumeunit;
	input.length = sizeof(NbiotCj188BhItfComElement_t);

	//打包数据
	if (func_nbiotcj188_ul_msg_pack(&input, &buf) == FAILURE){
		HcuErrorPrint("NBIOTCJ188: Package message error!\n");
		zHcuRunErrCnt[TASK_ID_NBIOTCJ188]++;
		return FAILURE;
	}

	//Send out
	ret = func_nbiotcj188_send_data_to_cloud(&buf);
	if ( ret == FAILURE){
		zHcuRunErrCnt[TASK_ID_NBIOTCJ188]++;
		HcuErrorPrint("NBIOTCJ188: Package message error!\n");
		return FAILURE;
	}

	//结束
	if ((zHcuSysEngPar.debugMode & HCU_TRACE_DEBUG_NOR_ON) != FALSE){
		HcuDebugPrint("NBIOTCJ188: Online state, send instance/period IPM to cloud success!\n"); }
	//State no change
	return SUCCESS;
}

//下行链路数据报文处理过程
OPSTAT fsm_nbiotcj188_ethernet_data_rx(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret=0;
	msg_struct_ethernet_nbiotcj188_data_rx_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_ethernet_nbiotcj188_data_rx_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_ethernet_nbiotcj188_data_rx_t))){
		HcuErrorPrint("NBIOTCJ188: Receive ETHERNET package error!\n");
		zHcuRunErrCnt[TASK_ID_NBIOTCJ188]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//跟msg_struct_ethernet_nbiotcj188_data_rx_t是完全一致的结构
	CloudDataSendBuf_t buf;
	memset(&buf, 0, sizeof(CloudDataSendBuf_t));
	memcpy(&buf, &rcv, sizeof(CloudDataSendBuf_t));

	//格式的区分，这里不再做，所以这里就比较简单了。
	//如果需要通过这里进行消息解码的格式区分，也可以通过全局变量进行区分
	if (func_nbiotcj188_dl_msg_unpack(&buf) == FAILURE){
		zHcuRunErrCnt[TASK_ID_NBIOTCJ188]++;
		HcuErrorPrint("NBIOTCJ188: Unpack receive message error from [%s] module!\n", zHcuTaskInfo[src_id].taskName);
		return FAILURE;
	}

	return SUCCESS;
}

//长周期定时器, 周期性心跳时钟处理机制
OPSTAT func_nbiotcj188_time_out_period(void)
{
	//检查链路状态，离线，则再连接
	if (FsmGetState(TASK_ID_NBIOTCJ188) == FSM_STATE_NBIOTCJ188_OFFLINE){
		if (hcu_ethernet_socket_link_setup() == SUCCESS){
			//State Transfer to FSM_STATE_NBIOTCJ188_ONLINE
			if (FsmSetState(TASK_ID_NBIOTCJ188, FSM_STATE_NBIOTCJ188_ONLINE) == FAILURE){
				zHcuRunErrCnt[TASK_ID_NBIOTCJ188]++;
				HcuErrorPrint("NBIOTCJ188: Error Set FSM State!\n");
				return FAILURE;
			}
			HcuDebugPrint("NBIOTCJ188: Connect state change, from OFFLINE to ONLINE!\n");
		}
		//如果是失败情况，并不返回错误，属于正常情况
		//当链路不可用时，这个打印结果会非常频繁，放开比较好
		else{
			if ((zHcuSysEngPar.debugMode & HCU_TRACE_DEBUG_IPT_ON) != FALSE){
				HcuDebugPrint("NBIOTCJ188: Try to setup connection with back-cloud, but not success!\n");
			}
		}
	}

	//在线状态，则检查
	else if(FsmGetState(TASK_ID_NBIOTCJ188) == FSM_STATE_NBIOTCJ188_ONLINE){
		if (func_nbiotcj188_heart_beat_check() == FAILURE){
			zHcuRunErrCnt[TASK_ID_NBIOTCJ188]++;
			//State Transfer to FSM_STATE_NBIOTCJ188_OFFLINE
			if (FsmSetState(TASK_ID_NBIOTCJ188, FSM_STATE_NBIOTCJ188_OFFLINE) == FAILURE){
				zHcuRunErrCnt[TASK_ID_NBIOTCJ188]++;
				HcuErrorPrint("NBIOTCJ188: Error Set FSM State!\n");
				return FAILURE;
			}
			HcuDebugPrint("NBIOTCJ188: Connect state change, from ONLINE to OFFLINE!\n");
			//并不立即启动连接的建立，而是等待下一个周期带来，否则状态机过于复杂
		}//心跳握手失败
		//在线而且心跳握手正常
		//Do nothing
	}//end of 长周期定时在线状态

	//既不在线，也不离线，强制转移到离线状态以便下次恢复，这种情况很难得，一般不会跑到这儿来，这种情况通常发生在初始化期间或者状态机胡乱的情况下
	else{
		if (FsmSetState(TASK_ID_NBIOTCJ188, FSM_STATE_NBIOTCJ188_OFFLINE) == FAILURE){
			zHcuRunErrCnt[TASK_ID_NBIOTCJ188]++;
			HcuErrorPrint("NBIOTCJ188: Error Set FSM State!\n");
			return FAILURE;
		}
	}

	return SUCCESS;
}

//心跳检测机制
OPSTAT func_nbiotcj188_heart_beat_check(void)
{
	int ret = 0;

	//发送数据给后台
	if (FsmGetState(TASK_ID_NBIOTCJ188) != FSM_STATE_NBIOTCJ188_ONLINE){
		zHcuRunErrCnt[TASK_ID_NBIOTCJ188]++;
		HcuErrorPrint("NBIOTCJ188: Error send HEART_BEAT to cloud, get by ONLINE, but back off line so quick!\n");
		return FAILURE;
	}

	//初始化变量
	CloudDataSendBuf_t buf;
	memset(&buf, 0, sizeof(CloudDataSendBuf_t));

	//打包数据
	if (func_nbiotcj188_heart_beat_msg_pack(&buf) == FAILURE){
		zHcuRunErrCnt[TASK_ID_NBIOTCJ188]++;
		HcuErrorPrint("NBIOTCJ188: Package message error!\n");
		return FAILURE;
	}

	//Send out
	ret = func_nbiotcj188_send_data_to_cloud(&buf);
	if ( ret == FAILURE){
		zHcuRunErrCnt[TASK_ID_NBIOTCJ188]++;
		HcuErrorPrint("NBIOTCJ188: Error send data to back-cloud!\n");
		return FAILURE;
	}

	//结束
	if ((zHcuSysEngPar.debugMode & HCU_TRACE_DEBUG_NOR_ON) != FALSE){
		HcuDebugPrint("NBIOTCJ188: Online state, send HEART_BEAT message out to cloud success!\n");
	}
	//State no change
	return SUCCESS;
}

//Heart Beat消息的组包
OPSTAT func_nbiotcj188_heart_beat_msg_pack(CloudDataSendBuf_t *buf)
{
	return SUCCESS;
}

//Send to backhawl cloud
OPSTAT func_nbiotcj188_send_data_to_cloud(CloudDataSendBuf_t *buf)
{
	//参数检查
	if ((buf->curLen <=0) || (buf->curLen >MAX_HCU_MSG_BUF_LENGTH)){
		HcuErrorPrint("NBIOTCJ188: Error message length to send back for cloud!\n");
		zHcuRunErrCnt[TASK_ID_NBIOTCJ188]++;
		return FAILURE;
	}

	//这里只考虑ETHERNET一种网络配置情况，其它的不考虑
	if (hcu_ethernet_socket_date_send(buf) == FAILURE){
		zHcuRunErrCnt[TASK_ID_NBIOTCJ188]++;
		HcuErrorPrint("NBIOTCJ188: Error send data to back-cloud!\n");
		return FAILURE;
	}

	return SUCCESS;
}

//用于积累的缓冲数据回送服务器，这里暂时没有啥用，为了维持跟NBIOTCJ188类似的架构，保持它而已，暂时不用。
OPSTAT func_nbiotcj188_time_out_sendback_offline_data(void)
{
	return SUCCESS;
}

//消息pack函数
OPSTAT func_nbiotcj188_ul_msg_pack(NbiotCj188BhItfComElement_t *input, CloudDataSendBuf_t *buf)
{
	//检查参数
	if (input == NULL || buf == NULL){
		HcuErrorPrint("NBIOTCJ188: Invalid received data buffer!\n");
		zHcuRunErrCnt[TASK_ID_NBIOTCJ188]++;
		return FAILURE;
	}
	if ((input->equtype < HCU_NBIOT_CJ188_T_TYPE_WATER_METER_MIN) || (input->equtype > HCU_NBIOT_CJ188_T_TYPE_POWER_METER_MAX)){
		HcuErrorPrint("NBIOTCJ188: Invalid received equipment Type!\n");
		zHcuRunErrCnt[TASK_ID_NBIOTCJ188]++;
		return FAILURE;
	}
	if (strlen(input->head.addr) != 14) {
		HcuErrorPrint("NBIOTCJ188: Invalid received equipment address!\n");
		zHcuRunErrCnt[TASK_ID_NBIOTCJ188]++;
		return FAILURE;
	}
	if ((input->head.ctrlId < HCU_NBIOT_CJ188_CTRL_MIN) || (input->head.ctrlId > HCU_NBIOT_CJ188_CTRL_MAX)){
		HcuErrorPrint("NBIOTCJ188: Invalid received control command!\n");
		zHcuRunErrCnt[TASK_ID_NBIOTCJ188]++;
		return FAILURE;
	}

	//初始化变量
	NbiotCj188BhItfComCtrFlag_t ctrFlag;
	memset(&ctrFlag, 0, sizeof(NbiotCj188BhItfComCtrFlag_t));
	char s[MAX_HCU_MSG_BUF_LENGTH];
	memset(s, 0, sizeof(s));
	char da[MAX_HCU_MSG_BUF_LENGTH];
	memset(da, 0, sizeof(da));
	char tmp[17] = ""; //最长8个字节
	INT8 ctrl=0;
	UINT8 msgLen = 3; //最小长度3字节，注意，CHAR和字节是差一倍的
	INT32 tp=0, t0=0, t2=0, t4=0;

	//固定消息头
	memset(tmp, 0, sizeof(tmp));
	sprintf(tmp, "%02X", HCU_NBIOT_CJ188_FRAME_FIX_HEAD & 0xFF);
	strcat(s, tmp);

	//消息TYPE
	memset(tmp, 0, sizeof(tmp));
	sprintf(tmp, "%02X", input->equtype & 0xFF);
	strcat(s, tmp);

	//消息地址
	memset(tmp, 0, sizeof(tmp));
	strncpy(tmp, input->head.addr, 14);
	strcat(s, tmp);

	//控制域-通信比特异常与否
	ctrl = input->head.ctrlId & 0x3F;
	ctrl = (ctrl + 0x80) & 0xFF; //上行方向标示比特
	if (input->head.communicationFlag != HCU_NBIOT_CJ188_COMM_CHANNEL_NORMAL) ctrl = (ctrl + 0x40) & 0xFF;  //通信异常标示比特
	memset(tmp, 0, sizeof(tmp));
	sprintf(tmp, "%02X", ctrl & 0xFF);
	strcat(s, tmp);

	//根据指令，生成控制字段，确定消息变化长度
	BOOL historyD0D1 = (input->head.d0d1Id == HCU_NBIOT_CJ188_READ_DI0DI1_HISTORY_COUNTER_DATA1) || (input->head.d0d1Id == HCU_NBIOT_CJ188_READ_DI0DI1_HISTORY_COUNTER_DATA2)
		|| (input->head.d0d1Id == HCU_NBIOT_CJ188_READ_DI0DI1_HISTORY_COUNTER_DATA3) || (input->head.d0d1Id == HCU_NBIOT_CJ188_READ_DI0DI1_HISTORY_COUNTER_DATA4)
		|| (input->head.d0d1Id == HCU_NBIOT_CJ188_READ_DI0DI1_HISTORY_COUNTER_DATA5) || (input->head.d0d1Id == HCU_NBIOT_CJ188_READ_DI0DI1_HISTORY_COUNTER_DATA6)
		|| (input->head.d0d1Id == HCU_NBIOT_CJ188_READ_DI0DI1_HISTORY_COUNTER_DATA7) || (input->head.d0d1Id == HCU_NBIOT_CJ188_READ_DI0DI1_HISTORY_COUNTER_DATA8)
		|| (input->head.d0d1Id == HCU_NBIOT_CJ188_READ_DI0DI1_HISTORY_COUNTER_DATA9) || (input->head.d0d1Id == HCU_NBIOT_CJ188_READ_DI0DI1_HISTORY_COUNTER_DATA10)
		|| (input->head.d0d1Id == HCU_NBIOT_CJ188_READ_DI0DI1_HISTORY_COUNTER_DATA11) || (input->head.d0d1Id == HCU_NBIOT_CJ188_READ_DI0DI1_HISTORY_COUNTER_DATA12);
	if (input->head.communicationFlag != HCU_NBIOT_CJ188_COMM_CHANNEL_NORMAL){
		msgLen = 0x03;
		ctrFlag.stFlag = TRUE;
	}
	else if ((input->head.ctrlId == HCU_NBIOT_CJ188_CTRL_READ_DATA) && (input->head.d0d1Id == HCU_NBIOT_CJ188_READ_DI0DI1_CURRENT_COUNTER_DATA) &&
			((input->equtype <=HCU_NBIOT_CJ188_T_TYPE_HEAT_METER_MAX) && (input->equtype >=HCU_NBIOT_CJ188_T_TYPE_HEAT_METER_MIN))){
		msgLen = 0x2E;
		ctrFlag.d0d1Flag = TRUE;
		ctrFlag.billtodaydateFlag = TRUE;
		ctrFlag.currentheatFlag = TRUE;
		ctrFlag.heatpowerFlag = TRUE;
		ctrFlag.flowvolumeFlag = TRUE;
		ctrFlag.accuamountFlag = TRUE;
		ctrFlag.supplywatertempFlag = TRUE;
		ctrFlag.backwatertempFlag = TRUE;
		ctrFlag.accumuworktimeFlag = TRUE;
		ctrFlag.realtimeFlag = TRUE;
		ctrFlag.stFlag = TRUE;
	}
	else if ((input->head.ctrlId == HCU_NBIOT_CJ188_CTRL_READ_DATA) && (input->head.d0d1Id == HCU_NBIOT_CJ188_READ_DI0DI1_CURRENT_COUNTER_DATA) &&
			!((input->equtype <=HCU_NBIOT_CJ188_T_TYPE_HEAT_METER_MAX) && (input->equtype >=HCU_NBIOT_CJ188_T_TYPE_HEAT_METER_MIN))){
		msgLen = 0x16;
		ctrFlag.d0d1Flag = TRUE;
		ctrFlag.currentaccuvolumeFlag = TRUE;
		ctrFlag.billtodayaccuvolumeFlag = TRUE;
		ctrFlag.realtimeFlag = TRUE;
		ctrFlag.stFlag = TRUE;
	}
	else if ((input->head.ctrlId == HCU_NBIOT_CJ188_CTRL_READ_DATA) && historyD0D1 &&
			((input->equtype <=HCU_NBIOT_CJ188_T_TYPE_HEAT_METER_MAX) && (input->equtype >=HCU_NBIOT_CJ188_T_TYPE_HEAT_METER_MIN))){
		msgLen = 0x08;
		ctrFlag.d0d1Flag = TRUE;
		ctrFlag.billtodayheatFlag = TRUE;
	}
	else if ((input->head.ctrlId == HCU_NBIOT_CJ188_CTRL_READ_DATA) && historyD0D1 &&
			!((input->equtype <=HCU_NBIOT_CJ188_T_TYPE_HEAT_METER_MAX) && (input->equtype >=HCU_NBIOT_CJ188_T_TYPE_HEAT_METER_MIN))){
		msgLen = 0x08;
		ctrFlag.d0d1Flag = TRUE;
		ctrFlag.billtodayaccuvolumeFlag = TRUE;
	}
	else if ((input->head.ctrlId == HCU_NBIOT_CJ188_CTRL_READ_DATA) && (input->head.d0d1Id == HCU_NBIOT_CJ188_READ_DI0DI1_PRICE_TABLE)){
		msgLen = 0x12;
		ctrFlag.d0d1Flag = TRUE;
		ctrFlag.price1Flag = TRUE;
		ctrFlag.volume1Flag = TRUE;
		ctrFlag.price2Flag = TRUE;
		ctrFlag.volume2Flag = TRUE;
		ctrFlag.price3Flag = TRUE;
	}
	else if ((input->head.ctrlId == HCU_NBIOT_CJ188_CTRL_READ_DATA) && (input->head.d0d1Id == HCU_NBIOT_CJ188_READ_DI0DI1_BILL_TODAY_DATE)){
		msgLen = 0x04;
		ctrFlag.d0d1Flag = TRUE;
		ctrFlag.billtodaydateFlag = TRUE;
	}
	else if ((input->head.ctrlId == HCU_NBIOT_CJ188_CTRL_READ_DATA) && (input->head.d0d1Id == HCU_NBIOT_CJ188_READ_DI0DI1_READ_ACCOUNT_CUR_DATE)){
		msgLen = 0x04;
		ctrFlag.d0d1Flag = TRUE;
		ctrFlag.readamountcurdateFlag = TRUE;
	}
	else if ((input->head.ctrlId == HCU_NBIOT_CJ188_CTRL_READ_DATA) && (input->head.d0d1Id == HCU_NBIOT_CJ188_READ_DI0DI1_BUY_AMOUNT)){
		msgLen = 0x12;
		ctrFlag.d0d1Flag = TRUE;
		ctrFlag.buycodeFlag = TRUE;
		ctrFlag.thisamountFlag = TRUE;
		ctrFlag.accuamountFlag = TRUE;
		ctrFlag.remainamountFlag = TRUE;
		ctrFlag.stFlag = TRUE;
	}
	else if ((input->head.ctrlId == HCU_NBIOT_CJ188_CTRL_READ_KEY_VER) && (input->head.d0d1Id == HCU_NBIOT_CJ188_READ_DI0DI1_KEY_VER)){
		msgLen = 0x12;
		ctrFlag.d0d1Flag = TRUE;
		ctrFlag.keyverFlag = TRUE;
	}
	else if ((input->head.ctrlId == HCU_NBIOT_CJ188_CTRL_READ_ADDR) && (input->head.d0d1Id == HCU_NBIOT_CJ188_READ_DI0DI1_ADDRESS)){
		msgLen = 0x03;
		ctrFlag.d0d1Flag = TRUE;
	}
	else if ((input->head.ctrlId == HCU_NBIOT_CJ188_CTRL_WRITE_DATA) && (input->head.d0d1Id == HCU_NBIOT_CJ188_WRITE_DI0DI1_PRICE_TABLE)){
		msgLen = 0x05;
		ctrFlag.d0d1Flag = TRUE;
		ctrFlag.stFlag = TRUE;
	}
	else if ((input->head.ctrlId == HCU_NBIOT_CJ188_CTRL_WRITE_DATA) && (input->head.d0d1Id == HCU_NBIOT_CJ188_WRITE_DI0DI1_BILL_TODAY_DATE)){
		msgLen = 0x03;
		ctrFlag.d0d1Flag = TRUE;
	}
	else if ((input->head.ctrlId == HCU_NBIOT_CJ188_CTRL_WRITE_DATA) && (input->head.d0d1Id == HCU_NBIOT_CJ188_WRITE_DI0DI1_READ_ACCOUNT_CUR_DATE)){
		msgLen = 0x03;
		ctrFlag.d0d1Flag = TRUE;
	}
	else if ((input->head.ctrlId == HCU_NBIOT_CJ188_CTRL_WRITE_DATA) && (input->head.d0d1Id == HCU_NBIOT_CJ188_WRITE_DI0DI1_BUY_AMOUNT)){
		msgLen = 0x08;
		ctrFlag.d0d1Flag = TRUE;
		ctrFlag.buycodeFlag = TRUE;
		ctrFlag.thisamountFlag = TRUE;
	}
	else if ((input->head.ctrlId == HCU_NBIOT_CJ188_CTRL_WRITE_DATA) && (input->head.d0d1Id == HCU_NBIOT_CJ188_WRITE_DI0DI1_NEW_KEY)){
		msgLen = 0x04;
		ctrFlag.d0d1Flag = TRUE;
		ctrFlag.keyverFlag = TRUE;
	}
	else if ((input->head.ctrlId == HCU_NBIOT_CJ188_CTRL_WRITE_DATA) && (input->head.d0d1Id == HCU_NBIOT_CJ188_WRITE_DI0DI1_STD_TIME)){
		msgLen = 0x03;
		ctrFlag.d0d1Flag = TRUE;
	}
	else if ((input->head.ctrlId == HCU_NBIOT_CJ188_CTRL_WRITE_DATA) && (input->head.d0d1Id == HCU_NBIOT_CJ188_WRITE_DI0DI1_SWITCH_CTRL)){
		msgLen = 0x05;
		ctrFlag.d0d1Flag = TRUE;
		ctrFlag.stFlag = TRUE;
	}
	else if ((input->head.ctrlId == HCU_NBIOT_CJ188_CTRL_WRITE_DATA) && (input->head.d0d1Id == HCU_NBIOT_CJ188_WRITE_DI0DI1_OFF_FACTORY_START)){
		msgLen = 0x03;
		ctrFlag.d0d1Flag = TRUE;
	}
	else if ((input->head.ctrlId == HCU_NBIOT_CJ188_CTRL_WRITE_ADDR) && (input->head.d0d1Id == HCU_NBIOT_CJ188_WRITE_DI0DI1_ADDRESS)){
		msgLen = 0x03;
		ctrFlag.d0d1Flag = TRUE;
	}
	else if ((input->head.ctrlId == HCU_NBIOT_CJ188_CTRL_WRITE_DEVICE_SYN) && (input->head.d0d1Id == HCU_NBIOT_CJ188_WRITE_DI0DI1_DEVICE_SYN_DATA)){
		msgLen = 0x05;
		ctrFlag.d0d1Flag = TRUE;
		ctrFlag.stFlag = TRUE;
	}
	else{
		HcuErrorPrint("NBIOTCJ188: Invalid received CTRL ID + D0D1ID!\n");
		zHcuRunErrCnt[TASK_ID_NBIOTCJ188]++;
		return FAILURE;
	}

	//长度域
	sprintf(tmp, "%02X", msgLen & 0xFF);
	strcat(s, tmp);

	UINT32 index = 0;
	//变长部分：顺序是编码的黄金规则，千万不能错，否则就无法解开了!!!
	//D0D1
	if (ctrFlag.d0d1Flag == TRUE){
		memset(tmp, 0, sizeof(tmp));
		sprintf(tmp, "%04X", input->head.d0d1Id & 0xFFFF);
		strcat(da, tmp);
		index = index + 2;
	}

	//SER是固定必然有的，所以没有判定的必要
	memset(tmp, 0, sizeof(tmp));
	sprintf(tmp, "%02X", input->head.ser & 0xFF);
	strcat(da, tmp);
	index++;

	//当前累计流量
	if (ctrFlag.currentaccuvolumeFlag == TRUE){
		t4 = (int)(input->data.currentaccuvolume / 10000);
		t2 = (int)(input->data.currentaccuvolume / 100) - t4*100;
		t0 = (int)(input->data.currentaccuvolume) - t4*10000 - t2*100;
		tp = (int)((input->data.currentaccuvolume - t4*10000 - t2*100 - t0) * 100);
		memset(tmp, 0, sizeof(tmp));
		sprintf(tmp, "%02d%02d%02d%02d%02X", tp&0xFF, t0&0xFF, t0&0xFF, t0&0xFF, (input->data.currentaccuvolumeunit) & 0xFF);
		strcat(da, tmp);
		index = index + 5;
	}

	//结算日累计流量
	if (ctrFlag.billtodayaccuvolumeFlag == TRUE){
		t4 = (int)(input->billtodayaccuvolume / 10000);
		t2 = (int)(input->billtodayaccuvolume / 100) - t4*100;
		t0 = (int)(input->billtodayaccuvolume) - t4*10000 - t2*100;
		tp = (int)((input->billtodayaccuvolume - t4*10000 - t2*100 - t0) * 100);
		memset(tmp, 0, sizeof(tmp));
		sprintf(tmp, "%02d%02d%02d%02d%02X", tp&0xFF, t0&0xFF, t0&0xFF, t0&0xFF, (input->billtodayaccuvolumeunit) & 0xFF);
		strcat(da, tmp);
		index = index + 5;
	}

	//结算日热量
	if (ctrFlag.billtodayheatFlag== TRUE){
		t4 = (int)(input->billtodayheat / 10000);
		t2 = (int)(input->billtodayheat / 100) - t4*100;
		t0 = (int)(input->billtodayheat) - t4*10000 - t2*100;
		tp = (int)((input->billtodayheat - t4*10000 - t2*100 - t0) * 100);
		memset(tmp, 0, sizeof(tmp));
		sprintf(tmp, "%02d%02d%02d%02d%02X", tp&0xFF, t0&0xFF, t0&0xFF, t0&0xFF, (input->billtodayheatunit) & 0xFF);
		strcat(da, tmp);
		index = index + 5;
	}

	//当前热量
	if (ctrFlag.currentheatFlag== TRUE){
		t4 = (int)(input->currentheat / 10000);
		t2 = (int)(input->currentheat / 100) - t4*100;
		t0 = (int)(input->currentheat) - t4*10000 - t2*100;
		tp = (int)((input->currentheat - t4*10000 - t2*100 - t0) * 100);
		memset(tmp, 0, sizeof(tmp));
		sprintf(tmp, "%02d%02d%02d%02d%02X", tp&0xFF, t0&0xFF, t0&0xFF, t0&0xFF, (input->currentheatunit) & 0xFF);
		strcat(da, tmp);
		index = index + 5;
	}

	//热功率
	if (ctrFlag.heatpowerFlag== TRUE){
		t4 = (int)(input->heatpower / 10000);
		t2 = (int)(input->heatpower / 100) - t4*100;
		t0 = (int)(input->heatpower) - t4*10000 - t2*100;
		tp = (int)((input->heatpower - t4*10000 - t2*100 - t0) * 100);
		memset(tmp, 0, sizeof(tmp));
		sprintf(tmp, "%02d%02d%02d%02d%02X", tp&0xFF, t0&0xFF, t0&0xFF, t0&0xFF, (input->heatpowerunit) & 0xFF);
		strcat(da, tmp);
		index = index + 5;
	}

	//流量
	if (ctrFlag.flowvolumeFlag== TRUE){
		t4 = (int)(input->data.flowvolume / 10000);
		t2 = (int)(input->data.flowvolume / 100) - t4*100;
		t0 = (int)(input->data.flowvolume) - t4*10000 - t2*100;
		tp = (int)((input->data.flowvolume - t4*10000 - t2*100 - t0) * 100);
		memset(tmp, 0, sizeof(tmp));
		sprintf(tmp, "%02d%02d%02d%02d%02X", tp&0xFF, t0&0xFF, t0&0xFF, t0&0xFF, (input->data.flowvolumeunit) & 0xFF);
		strcat(da, tmp);
		index = index + 5;
	}

	//累计流量
	if (ctrFlag.currentaccuvolumeFlag== TRUE){
		t4 = (int)(input->data.currentaccuvolume / 10000);
		t2 = (int)(input->data.currentaccuvolume / 100) - t4*100;
		t0 = (int)(input->data.currentaccuvolume) - t4*10000 - t2*100;
		tp = (int)((input->data.currentaccuvolume - t4*10000 - t2*100 - t0) * 100);
		memset(tmp, 0, sizeof(tmp));
		sprintf(tmp, "%02d%02d%02d%02d%02X", tp&0xFF, t0&0xFF, t0&0xFF, t0&0xFF, (input->data.currentaccuvolumeunit) & 0xFF);
		strcat(da, tmp);
		index = index + 5;
	}

	//供水温度
	if (ctrFlag.supplywatertempFlag== TRUE){
		t2 = (int)(input->data.supplywatertemp / 100);
		t0 = (int)(input->data.supplywatertemp) - t2*100;
		tp = (int)((input->data.supplywatertemp - t2*100 - t0) * 100);
		memset(tmp, 0, sizeof(tmp));
		sprintf(tmp, "%02d%02d%02d", tp&0xFF, t0&0xFF, t2&0xFF);
		strcat(da, tmp);
		index = index + 3;
	}

	//回水温度
	if (ctrFlag.backwatertempFlag== TRUE){
		t2 = (int)(input->data.backwatertemp / 100);
		t0 = (int)(input->data.backwatertemp) - t2*100;
		tp = (int)((input->data.backwatertemp - t2*100 - t0) * 100);
		memset(tmp, 0, sizeof(tmp));
		sprintf(tmp, "%02d%02d%02d", tp&0xFF, t0&0xFF, t2&0xFF);
		strcat(da, tmp);
		index = index + 3;
	}

	//累计工作时间
	if (ctrFlag.accumuworktimeFlag== TRUE){
		t2 = (int)(input->data.accumuworktime / 100);
		t0 = (int)(input->data.accumuworktime) - t2*100;
		tp = (int)((input->data.accumuworktime - t2*100 - t0) * 100);
		memset(tmp, 0, sizeof(tmp));
		sprintf(tmp, "%02d%02d%02d", tp&0xFF, t0&0xFF, t2&0xFF);
		strcat(da, tmp);
		index = index + 3;
	}

	//价格1
	//累计工作时间
	if (ctrFlag.price1Flag== TRUE){
		t2 = (int)(input->data.price1 / 100);
		t0 = (int)(input->data.price1) - t2*100;
		tp = (int)((input->data.price1 - t2*100 - t0) * 100);
		memset(tmp, 0, sizeof(tmp));
		sprintf(tmp, "%02d%02d%02d", tp&0xFF, t0&0xFF, t2&0xFF);
		strcat(da, tmp);
		index = index + 3;
	}

	//用量1
	if (ctrFlag.volume1Flag== TRUE){
		t4 = (int)(input->data.volume1 / 10000);
		t2 = (int)(input->data.volume1/100) - t4*100;
		t0 = (int)(input->data.volume1) - t4*10000 - t2*100;
		memset(tmp, 0, sizeof(tmp));
		sprintf(tmp, "%02d%02d%02d", t0&0xFF, t0&0xFF, t4&0xFF);
		strcat(da, tmp);
		index = index + 3;
	}

	//价格2
	if (ctrFlag.price2Flag== TRUE){
		t2 = (int)(input->data.price2 / 100);
		t0 = (int)(input->data.price2) - t2*100;
		tp = (int)((input->data.price2 - t2*100 - t0) * 100);
		memset(tmp, 0, sizeof(tmp));
		sprintf(tmp, "%02d%02d%02d", tp&0xFF, t0&0xFF, t2&0xFF);
		strcat(da, tmp);
		index = index + 3;
	}

	//用量2
	if (ctrFlag.volume2Flag== TRUE){
		t4 = (int)(input->data.volume2 / 10000);
		t2 = (int)(input->data.volume2/100) - t4*100;
		t0 = (int)(input->data.volume2) - t4*10000 - t2*100;
		memset(tmp, 0, sizeof(tmp));
		sprintf(tmp, "%02d%02d%02d", t0&0xFF, t0&0xFF, t4&0xFF);
		strcat(da, tmp);
		index = index + 3;
	}

	//价格3
	if (ctrFlag.price3Flag== TRUE){
		t2 = (int)(input->data.price3 / 100);
		t0 = (int)(input->data.price3) - t2*100;
		tp = (int)((input->data.price3 - t2*100 - t0) * 100);
		memset(tmp, 0, sizeof(tmp));
		sprintf(tmp, "%02d%02d%02d", tp&0xFF, t0&0xFF, t2&0xFF);
		strcat(da, tmp);
		index = index + 3;
	}

	//结算日
	if (ctrFlag.billtodaydateFlag == TRUE){
		memset(tmp, 0, sizeof(tmp));
		sprintf(tmp, "%02d", input->data.billtodaydate);
		strcat(da, tmp);
		index = index + 1;
	}

	//抄表日
	if (ctrFlag.readamountcurdateFlag == TRUE){
		memset(tmp, 0, sizeof(tmp));
		sprintf(tmp, "%02d", input->data.readamountcurdate);
		strcat(da, tmp);
		index = index + 1;
	}

	//启动日
	if (ctrFlag.startdateFlag == TRUE){
		memset(tmp, 0, sizeof(tmp));
		sprintf(tmp, "%02d", input->data.startdate);
		strcat(da, tmp);
		index = index + 1;
	}

	//本次购买序号
	if (ctrFlag.buycodeFlag == TRUE){
		memset(tmp, 0, sizeof(tmp));
		sprintf(tmp, "%02X", input->data.buycode);
		strcat(da, tmp);
		index = index + 1;
	}

	//本次买入金额
	if (ctrFlag.thisamountFlag== TRUE){
		t4 = (int)(input->data.thisamount / 10000);
		t2 = (int)(input->data.thisamount / 100) - t4*100;
		t0 = (int)(input->data.thisamount) - t4*10000 - t2*100;
		tp = (int)((input->data.thisamount - t4*10000 - t2*100 - t0) * 100);
		memset(tmp, 0, sizeof(tmp));
		sprintf(tmp, "%02d%02d%02d%02d", tp&0xFF, t0&0xFF, t0&0xFF, t4&0xFF);
		strcat(da, tmp);
		index = index + 4;
	}

	//累计买入金额
	if (ctrFlag.accuamountFlag== TRUE){
		t4 = (int)(input->data.accuamount / 10000);
		t2 = (int)(input->data.accuamount / 100) - t4*100;
		t0 = (int)(input->data.accuamount) - t4*10000 - t2*100;
		tp = (int)((input->data.accuamount - t4*10000 - t2*100 - t0) * 100);
		memset(tmp, 0, sizeof(tmp));
		sprintf(tmp, "%02d%02d%02d%02d", tp&0xFF, t0&0xFF, t0&0xFF, t4&0xFF);
		strcat(da, tmp);
		index = index + 4;
	}

	//剩余金额
	if (ctrFlag.remainamountFlag== TRUE){
		t4 = (int)(input->data.remainamount / 10000);
		t2 = (int)(input->data.remainamount / 100) - t4*100;
		t0 = (int)(input->data.remainamount) - t4*10000 - t2*100;
		tp = (int)((input->data.remainamount - t4*10000 - t2*100 - t0) * 100);
		memset(tmp, 0, sizeof(tmp));
		sprintf(tmp, "%02d%02d%02d%02d", tp&0xFF, t0&0xFF, t0&0xFF, t4&0xFF);
		strcat(da, tmp);
		index = index + 4;
	}

	//密钥版本号
	if (ctrFlag.keyverFlag == TRUE){
		memset(tmp, 0, sizeof(tmp));
		sprintf(tmp, "%02X", input->data.keyver);
		strcat(da, tmp);
		index = index + 1;
	}

	//实时时间: 另外一种考虑就是直接拷贝REALTIME字符串
	if (ctrFlag.realtimeFlag== TRUE){
		time_t lt;
		struct tm *cu;
		lt=time(NULL);
		cu = localtime(&lt);
		memset(tmp, 0, sizeof(tmp));
		sprintf(tmp,  "%04d%02d%02d%02d%02d%02d", (UINT16)(1900+cu->tm_year), (UINT8)cu->tm_mon, (UINT8)cu->tm_mday, (UINT8)cu->tm_hour, (UINT8)cu->tm_min, (UINT8)cu->tm_sec);
		strcat(da, tmp);
		index = index + 7;

		//另外一种设计方式，是否选择它，待定
		/*
		if (strlen(input->data.realtime) != 14){
			HcuErrorPrint("NBIOTCJ188: Invalid received control command!\n");
			zHcuRunErrCnt[TASK_ID_NBIOTCJ188]++;
			return FAILURE;
		}
		memset(tmp, 0, sizeof(tmp));
		strncpy(tmp, input->data.realtime, 14);
		strcat(da, tmp);
		index = index + 7;
		*/
	}

	//状态
	if (ctrFlag.stFlag == TRUE){
		memset(tmp, 0, sizeof(tmp));
		strncpy(tmp, input->data.st, 4);
		strcat(da, tmp);
		index = index + 2;
	}

	//阀门控制操作
	if (ctrFlag.switchctrlFlag == TRUE){
		memset(tmp, 0, sizeof(tmp));
		if ((input->data.switchctrl != HCU_NBIOT_CJ188_SWITCH_CONTROL_ON) && (input->data.switchctrl != HCU_NBIOT_CJ188_SWITCH_CONTROL_OFF)){
			HcuErrorPrint("NBIOTCJ188: Invalid received switch control command!\n");
			zHcuRunErrCnt[TASK_ID_NBIOTCJ188]++;
			return FAILURE;
		}
		sprintf(tmp, "%02d", input->data.switchctrl);
		strcat(da, tmp);
		index = index + 1;
	}

	//检查长度
	if (msgLen != index){
		HcuErrorPrint("NBIOTCJ188: Pack message error!\n");
		zHcuRunErrCnt[TASK_ID_NBIOTCJ188]++;
		return FAILURE;
	}

	//头+变长的数据部分
	strcat(s, da);

	//计算CHECKSUM部分
	INT8 checksum = 0;
	//检查长度
	if (func_nbiotcj188_checksum_caculate(da, checksum) == FAILURE){
		HcuErrorPrint("NBIOTCJ188: Checksum caculation error!\n");
		zHcuRunErrCnt[TASK_ID_NBIOTCJ188]++;
		return FAILURE;
	}
	memset(tmp, 0, sizeof(tmp));
	sprintf(tmp, "%02X", checksum & 0xFF);
	strcat(s, tmp);

	//固定尾巴
	memset(tmp, 0, sizeof(tmp));
	sprintf(tmp, "%02X", HCU_NBIOT_CJ188_FRAME_FIX_TAIL & 0xFF);
	strcat(s, tmp);

	//输出
	strcpy(buf->curBuf, s);
	buf->curLen = strlen(s);

	//返回
	return SUCCESS;
}

//消息unpack函数
OPSTAT func_nbiotcj188_checksum_caculate(char *s, INT8 output)
{
	UINT32 index = 0;
	output = 0;
	UINT32 tmp = 0;
	char tc[2] = "";

	if (strlen(s) == 0 ) return SUCCESS;
	if ((strlen(s)/2)*2 != strlen(s)) return FAILURE;

	for (index = 0; index < strlen(s)/2; index++){
		strncpy(tc, &s[2*index], 2);
		tmp = strtoul(tc, NULL, 16);
		output = (output + tmp) & 0xFF;
	}
	return SUCCESS;
}

//消息unpack函数
OPSTAT func_nbiotcj188_dl_msg_unpack(CloudDataSendBuf_t *buf)
{
	UINT32 ret = 0;

	//检查参数
	if (buf == NULL){
		HcuErrorPrint("NBIOTCJ188: Invalid received data buffer!\n");
		zHcuRunErrCnt[TASK_ID_NBIOTCJ188]++;
		return FAILURE;
	}
	if ((buf->curLen <= 26) || (buf->curLen >= HCU_NBIOT_CJ188_FRAME_READ_MAX_LEN*2)|| (buf->curLen != strlen(buf->curBuf)) || (strlen(buf->curBuf) / 2 * 2 != strlen(buf->curBuf))){
		HcuErrorPrint("NBIOTCJ188: Invalid received data length!\n");
		zHcuRunErrCnt[TASK_ID_NBIOTCJ188]++;
		return FAILURE;
	}

	//初始化变量
	UINT64 tmp = 0;
	char stmp[17] = "";
	INT8 equType = 0;
	INT8 ctrl =0;
	UINT8 msgLen = 0;
	char addr[15] = "";
	UINT32 index =0;
	INT16 d0d1Id = 0;
	INT8 serId = 0;
	INT32 tp=0, t0=0, t2=0, t4=0;
	float ftmp =0;

	//固定头部解码
	memset(stmp, 0, sizeof(stmp));
	strncpy(stmp, &buf->curBuf[index], 2);
	tmp = strtoul(stmp, NULL, 16);
	if (tmp != HCU_NBIOT_CJ188_FRAME_FIX_HEAD) {
		HcuErrorPrint("NBIOTCJ188: Invalid received data head!\n");
		zHcuRunErrCnt[TASK_ID_NBIOTCJ188]++;
		return FAILURE;
	}

	//固定尾部解码
	memset(stmp, 0, sizeof(stmp));
	index = buf->curLen*2 - 2;
	strncpy(stmp, &buf->curBuf[index], 2);
	tmp = strtoul(stmp, NULL, 16);
	if (tmp != HCU_NBIOT_CJ188_FRAME_FIX_TAIL) {
		HcuErrorPrint("NBIOTCJ188: Invalid received data head!\n");
		zHcuRunErrCnt[TASK_ID_NBIOTCJ188]++;
		return FAILURE;
	}

	//固定CHECKSUM部分解码
	memset(stmp, 0, sizeof(stmp));
	index = buf->curLen*2 - 4;
	strncpy(stmp, &buf->curBuf[index], 2);
	tmp = strtoul(stmp, NULL, 16);
	INT8 checksum = 0;
	char s[MAX_HCU_MSG_BUF_LENGTH];
	memset(s, 0, sizeof(s));
	strncpy(s, &buf->curBuf[22], buf->curLen-26);
	if (func_nbiotcj188_checksum_caculate(s, checksum) == FAILURE)
	if (tmp != checksum) {
		HcuErrorPrint("NBIOTCJ188: Invalid received data checksum!\n");
		zHcuRunErrCnt[TASK_ID_NBIOTCJ188]++;
		return FAILURE;
	}

	//固定设备类型解码
	index = 2;
	memset(stmp, 0, sizeof(stmp));
	strncpy(stmp, &buf->curBuf[index], 2);
	tmp = strtoul(stmp, NULL, 16);
	if ((tmp < HCU_NBIOT_CJ188_T_TYPE_WATER_METER_MIN) || (tmp > HCU_NBIOT_CJ188_T_TYPE_WATER_METER_MAX)) {
		HcuErrorPrint("NBIOTCJ188: Invalid received data equipment type!\n");
		zHcuRunErrCnt[TASK_ID_NBIOTCJ188]++;
		return FAILURE;
	}
	equType = (INT8)tmp;

	//固定设备地址域解码
	index = index + 2;
	memset(addr, 0, sizeof(stmp));
	strncpy(addr, &buf->curBuf[index], 14);

	//固定控制域解码
	index = index + 14;
	memset(stmp, 0, sizeof(stmp));
	strncpy(stmp, &buf->curBuf[index], 2);
	tmp = strtoul(stmp, NULL, 16);
	ctrl = tmp & 0xFF;
	if ((ctrl&0x80) >> 7 != 1){
		HcuErrorPrint("NBIOTCJ188: Invalid received communication direction!\n");
		zHcuRunErrCnt[TASK_ID_NBIOTCJ188]++;
		return FAILURE;
	}
	if ((ctrl&0x40 >> 6) == 1){
		HcuErrorPrint("NBIOTCJ188: Invalid received communication abnormal!\n");
		zHcuRunErrCnt[TASK_ID_NBIOTCJ188]++;
		return FAILURE;
	}
	ctrl = ctrl & 0x3F;
	if ((ctrl < HCU_NBIOT_CJ188_CTRL_MIN) || (ctrl > HCU_NBIOT_CJ188_CTRL_MAX)){
		HcuErrorPrint("NBIOTCJ188: Invalid received control command!\n");
		zHcuRunErrCnt[TASK_ID_NBIOTCJ188]++;
		return FAILURE;
	}

	//固定长度域解码
	index = index + 2;
	memset(stmp, 0, sizeof(stmp));
	strncpy(stmp, &buf->curBuf[index], 2);
	tmp = strtoul(stmp, NULL, 16);
	msgLen = tmp & 0xFF;
	//进一步，如果msgLen < 3，从主站发出的命令也是有问题的
	if (((buf->curLen - 26) != msgLen) || (msgLen < 3)){
		HcuErrorPrint("NBIOTCJ188: Invalid received in-line message length!\n");
		zHcuRunErrCnt[TASK_ID_NBIOTCJ188]++;
		return FAILURE;
	}

	//解码数据部分长度部分
	index = index + 2;
	memset(stmp, 0, sizeof(stmp));
	strncpy(stmp, &buf->curBuf[index], 4);
	tmp = strtoul(stmp, NULL, 16);
	d0d1Id = tmp & 0xFFFF;

	index = index + 4;
	memset(stmp, 0, sizeof(stmp));
	strncpy(stmp, &buf->curBuf[index], 2);
	tmp = strtoul(stmp, NULL, 16);
	serId = tmp & 0xFF;

	//准备向L3发送消息，为了后面的统一处理，这里必须浪费性的申请这么多消息体，不然没法实现较为精简的程序处理

	//初始化发送函数，CONTROL_CMD在这里并没有生成
	msg_struct_nbiotcj188_iwm_data_req_t snd1;
	memset(&snd1, 0, sizeof(msg_struct_nbiotcj188_iwm_data_req_t));
	snd1.length = sizeof(msg_struct_nbiotcj188_iwm_data_req_t);
	snd1.equtype = equType;
	snd1.iwmHead.ser = serId;
	strncpy(snd1.iwmHead.addr, addr, 14);
	snd1.iwmHead.ctrlId = ctrl;
	snd1.iwmHead.d0d1Id = d0d1Id;
	snd1.iwmHead.periodFlag = L3CI_cmdid_back_type_instance;

	//初始化发送函数，CONTROL_CMD在这里并没有生成
	msg_struct_nbiotcj188_ihm_data_req_t snd2;
	memset(&snd2, 0, sizeof(msg_struct_nbiotcj188_ihm_data_req_t));
	snd2.length = sizeof(msg_struct_nbiotcj188_ihm_data_req_t);
	snd2.equtype = equType;
	snd2.ihmHead.ser = serId;
	strncpy(snd2.ihmHead.addr, addr, 14);
	snd2.ihmHead.ctrlId = ctrl;
	snd2.ihmHead.d0d1Id = d0d1Id;
	snd2.ihmHead.periodFlag = L3CI_cmdid_back_type_instance;

	//初始化发送函数，CONTROL_CMD在这里并没有生成
	msg_struct_nbiotcj188_igm_data_req_t snd3;
	memset(&snd3, 0, sizeof(msg_struct_nbiotcj188_igm_data_req_t));
	snd3.length = sizeof(msg_struct_nbiotcj188_igm_data_req_t);
	snd3.equtype = equType;
	snd3.igmHead.ser = serId;
	strncpy(snd3.igmHead.addr, addr, 14);
	snd3.igmHead.ctrlId = ctrl;
	snd3.igmHead.d0d1Id = d0d1Id;
	snd3.igmHead.periodFlag = L3CI_cmdid_back_type_instance;

	//初始化发送函数，CONTROL_CMD在这里并没有生成
	msg_struct_nbiotcj188_ipm_data_req_t snd4;
	memset(&snd4, 0, sizeof(msg_struct_nbiotcj188_ipm_data_req_t));
	snd4.length = sizeof(msg_struct_nbiotcj188_ipm_data_req_t);
	snd4.equtype = equType;
	snd4.ipmHead.ser = serId;
	strncpy(snd4.ipmHead.addr, addr, 14);
	snd4.ipmHead.ctrlId = ctrl;
	snd4.ipmHead.d0d1Id = d0d1Id;
	snd4.ipmHead.periodFlag = L3CI_cmdid_back_type_instance;

	//分类型处理，正常的读取
	if (((ctrl == HCU_NBIOT_CJ188_CTRL_READ_DATA) && ((d0d1Id == HCU_NBIOT_CJ188_READ_DI0DI1_CURRENT_COUNTER_DATA)
			 || (d0d1Id == HCU_NBIOT_CJ188_READ_DI0DI1_HISTORY_COUNTER_DATA1) || (d0d1Id == HCU_NBIOT_CJ188_READ_DI0DI1_HISTORY_COUNTER_DATA2)
			 || (d0d1Id == HCU_NBIOT_CJ188_READ_DI0DI1_HISTORY_COUNTER_DATA3) || (d0d1Id == HCU_NBIOT_CJ188_READ_DI0DI1_HISTORY_COUNTER_DATA4)
			 || (d0d1Id == HCU_NBIOT_CJ188_READ_DI0DI1_HISTORY_COUNTER_DATA5) || (d0d1Id == HCU_NBIOT_CJ188_READ_DI0DI1_HISTORY_COUNTER_DATA6)
			 || (d0d1Id == HCU_NBIOT_CJ188_READ_DI0DI1_HISTORY_COUNTER_DATA7) || (d0d1Id == HCU_NBIOT_CJ188_READ_DI0DI1_HISTORY_COUNTER_DATA8)
			 || (d0d1Id == HCU_NBIOT_CJ188_READ_DI0DI1_HISTORY_COUNTER_DATA9) || (d0d1Id == HCU_NBIOT_CJ188_READ_DI0DI1_HISTORY_COUNTER_DATA10)
			 || (d0d1Id == HCU_NBIOT_CJ188_READ_DI0DI1_HISTORY_COUNTER_DATA11) || (d0d1Id == HCU_NBIOT_CJ188_READ_DI0DI1_HISTORY_COUNTER_DATA12)
			 || (d0d1Id == HCU_NBIOT_CJ188_READ_DI0DI1_PRICE_TABLE) || (d0d1Id == HCU_NBIOT_CJ188_READ_DI0DI1_BILL_TODAY_DATE)
			 || (d0d1Id == HCU_NBIOT_CJ188_READ_DI0DI1_READ_ACCOUNT_CUR_DATE) || (d0d1Id == HCU_NBIOT_CJ188_READ_DI0DI1_BUY_AMOUNT))) ||
			((ctrl == HCU_NBIOT_CJ188_CTRL_READ_KEY_VER) && (d0d1Id == HCU_NBIOT_CJ188_READ_DI0DI1_KEY_VER)) ||
			((ctrl == HCU_NBIOT_CJ188_CTRL_READ_ADDR) && (d0d1Id == HCU_NBIOT_CJ188_READ_DI0DI1_ADDRESS)))
	{
		if (msgLen != 0x3){
			HcuErrorPrint("NBIOTCJ188: Invalid received in-line message length on HCU_NBIOT_CJ188_CTRL_READ_DATA!\n");
			zHcuRunErrCnt[TASK_ID_NBIOTCJ188]++;
			return FAILURE;
		}
		//没有普其它消息体，故而不再继续解码

	}

	//写价格表
	else if ((ctrl == HCU_NBIOT_CJ188_CTRL_WRITE_DATA) && (d0d1Id == HCU_NBIOT_CJ188_WRITE_DI0DI1_PRICE_TABLE)){
		if (msgLen != 0x13){
			HcuErrorPrint("NBIOTCJ188: Invalid received in-line message length on HCU_NBIOT_CJ188_WRITE_DI0DI1_PRICE_TABLE!\n");
			zHcuRunErrCnt[TASK_ID_NBIOTCJ188]++;
			return FAILURE;
		}
		//价格１
		index = index + 2;
		memset(stmp, 0, sizeof(stmp));
		strncpy(stmp, &buf->curBuf[index], 2);
		tp = strtoul(stmp, NULL, 10);
		index = index + 2;
		memset(stmp, 0, sizeof(stmp));
		strncpy(stmp, &buf->curBuf[index], 2);
		t0 = strtoul(stmp, NULL, 10);
		index = index + 2;
		memset(stmp, 0, sizeof(stmp));
		strncpy(stmp, &buf->curBuf[index], 2);
		t2 = strtoul(stmp, NULL, 10);
		ftmp = (float)((tp&0xFF)/100);
		snd1.iwmData.price1 = (t2&0xFF)*100 + (t0&0xFF) + ftmp;
		snd2.ihmData.price1 = snd1.iwmData.price1;
		snd3.igmData.price1 = snd1.iwmData.price1;
		snd4.ipmData.price1 = snd1.iwmData.price1;
		//用量１
		index = index + 2;
		memset(stmp, 0, sizeof(stmp));
		strncpy(stmp, &buf->curBuf[index], 2);
		t0 = strtoul(stmp, NULL, 10);
		index = index + 2;
		memset(stmp, 0, sizeof(stmp));
		strncpy(stmp, &buf->curBuf[index], 2);
		t2 = strtoul(stmp, NULL, 10);
		index = index + 2;
		memset(stmp, 0, sizeof(stmp));
		strncpy(stmp, &buf->curBuf[index], 2);
		t4 = strtoul(stmp, NULL, 10);
		snd1.iwmData.volume1 = (t4&0xFF)*10000 + (t2&0xFF)*100 + (t0&0xFF);
		snd2.ihmData.volume1 = snd1.iwmData.volume1;
		snd3.igmData.volume1 = snd1.iwmData.volume1;
		snd4.ipmData.volume1 = snd1.iwmData.volume1;
		//价格2
		index = index + 2;
		memset(stmp, 0, sizeof(stmp));
		strncpy(stmp, &buf->curBuf[index], 2);
		tp = strtoul(stmp, NULL, 10);
		index = index + 2;
		memset(stmp, 0, sizeof(stmp));
		strncpy(stmp, &buf->curBuf[index], 2);
		t0 = strtoul(stmp, NULL, 10);
		index = index + 2;
		memset(stmp, 0, sizeof(stmp));
		strncpy(stmp, &buf->curBuf[index], 2);
		t2 = strtoul(stmp, NULL, 10);
		ftmp = (float)((tp&0xFF)/100);
		snd1.iwmData.price2 = (t2&0xFF)*100 + (t0&0xFF) + ftmp;
		snd2.ihmData.price2 = snd1.iwmData.price2;
		snd3.igmData.price2 = snd1.iwmData.price2;
		snd4.ipmData.price2 = snd1.iwmData.price2;
		//用量2
		index = index + 2;
		memset(stmp, 0, sizeof(stmp));
		strncpy(stmp, &buf->curBuf[index], 2);
		t0 = strtoul(stmp, NULL, 10);
		index = index + 2;
		memset(stmp, 0, sizeof(stmp));
		strncpy(stmp, &buf->curBuf[index], 2);
		t2 = strtoul(stmp, NULL, 10);
		index = index + 2;
		memset(stmp, 0, sizeof(stmp));
		strncpy(stmp, &buf->curBuf[index], 2);
		t4 = strtoul(stmp, NULL, 10);
		snd1.iwmData.volume2 = (t4&0xFF)*10000 + (t2&0xFF)*100 + (t0&0xFF);
		snd2.ihmData.volume2 = snd1.iwmData.volume2;
		snd3.igmData.volume2 = snd1.iwmData.volume2;
		snd4.ipmData.volume2 = snd1.iwmData.volume2;
		//价格3
		index = index + 2;
		memset(stmp, 0, sizeof(stmp));
		strncpy(stmp, &buf->curBuf[index], 2);
		tp = strtoul(stmp, NULL, 10);
		index = index + 2;
		memset(stmp, 0, sizeof(stmp));
		strncpy(stmp, &buf->curBuf[index], 2);
		t0 = strtoul(stmp, NULL, 10);
		index = index + 2;
		memset(stmp, 0, sizeof(stmp));
		strncpy(stmp, &buf->curBuf[index], 2);
		t2 = strtoul(stmp, NULL, 10);
		ftmp = (float)((tp&0xFF)/100);
		snd1.iwmData.price3 = (t2&0xFF)*100 + (t0&0xFF) + ftmp;
		snd2.ihmData.price3 = snd1.iwmData.price3;
		snd3.igmData.price3 = snd1.iwmData.price3;
		snd4.ipmData.price3 = snd1.iwmData.price3;
		//启动日期
		index = index + 2;
		memset(stmp, 0, sizeof(stmp));
		strncpy(stmp, &buf->curBuf[index], 2);
		tmp = strtoul(stmp, NULL, 10);
		snd1.iwmData.startdate = tmp & 0xFF;
		snd2.ihmData.startdate = snd1.iwmData.startdate;
		snd3.igmData.startdate = snd1.iwmData.startdate;
		snd4.ipmData.startdate = snd1.iwmData.startdate;
	}

	//写结算日
	else if ((ctrl == HCU_NBIOT_CJ188_CTRL_WRITE_DATA) && (d0d1Id == HCU_NBIOT_CJ188_WRITE_DI0DI1_BILL_TODAY_DATE)){
		if (msgLen != 0x4){
			HcuErrorPrint("NBIOTCJ188: Invalid received in-line message length on HCU_NBIOT_CJ188_WRITE_DI0DI1_BILL_TODAY_DATE!\n");
			zHcuRunErrCnt[TASK_ID_NBIOTCJ188]++;
			return FAILURE;
		}
		//结算日期
		index = index + 2;
		memset(stmp, 0, sizeof(stmp));
		strncpy(stmp, &buf->curBuf[index], 2);
		tmp = strtoul(stmp, NULL, 10);
		snd1.iwmData.billtodaydate = tmp & 0xFF;
		snd2.ihmData.billtodaydate = snd1.iwmData.billtodaydate;
		snd3.igmData.billtodaydate = snd1.iwmData.billtodaydate;
		snd4.ipmData.billtodaydate = snd1.iwmData.billtodaydate;
	}

	//写抄表日
	else if ((ctrl == HCU_NBIOT_CJ188_CTRL_WRITE_DATA) && (d0d1Id == HCU_NBIOT_CJ188_WRITE_DI0DI1_READ_ACCOUNT_CUR_DATE)){
		if (msgLen != 0x4){
			HcuErrorPrint("NBIOTCJ188: Invalid received in-line message length on HCU_NBIOT_CJ188_WRITE_DI0DI1_READ_ACCOUNT_CUR_DATE!\n");
			zHcuRunErrCnt[TASK_ID_NBIOTCJ188]++;
			return FAILURE;
		}
		//抄表日期
		index = index + 2;
		memset(stmp, 0, sizeof(stmp));
		strncpy(stmp, &buf->curBuf[index], 2);
		tmp = strtoul(stmp, NULL, 10);
		snd1.iwmData.readamountcurdate = tmp & 0xFF;
		snd2.ihmData.readamountcurdate = snd1.iwmData.readamountcurdate;
		snd3.igmData.readamountcurdate = snd1.iwmData.readamountcurdate;
		snd4.ipmData.readamountcurdate = snd1.iwmData.readamountcurdate;
	}

	//写购入金额
	else if ((ctrl == HCU_NBIOT_CJ188_CTRL_WRITE_DATA) && (d0d1Id == HCU_NBIOT_CJ188_WRITE_DI0DI1_BUY_AMOUNT)){
		if (msgLen != 0x8){
			HcuErrorPrint("NBIOTCJ188: Invalid received in-line message length on HCU_NBIOT_CJ188_WRITE_DI0DI1_BUY_AMOUNT!\n");
			zHcuRunErrCnt[TASK_ID_NBIOTCJ188]++;
			return FAILURE;
		}
		//本次买入序号
		index = index + 2;
		memset(stmp, 0, sizeof(stmp));
		strncpy(stmp, &buf->curBuf[index], 2);
		tmp = strtoul(stmp, NULL, 16);
		snd1.iwmData.buycode = tmp & 0xFF;
		snd2.ihmData.buycode = snd1.iwmData.buycode;
		snd3.igmData.buycode = snd1.iwmData.buycode;
		snd4.ipmData.buycode = snd1.iwmData.buycode;
		//本次购入金额
		index = index + 2;
		memset(stmp, 0, sizeof(stmp));
		strncpy(stmp, &buf->curBuf[index], 2);
		tp = strtoul(stmp, NULL, 10);
		index = index + 2;
		memset(stmp, 0, sizeof(stmp));
		strncpy(stmp, &buf->curBuf[index], 2);
		t0 = strtoul(stmp, NULL, 10);
		index = index + 2;
		memset(stmp, 0, sizeof(stmp));
		strncpy(stmp, &buf->curBuf[index], 2);
		t2 = strtoul(stmp, NULL, 10);
		index = index + 2;
		memset(stmp, 0, sizeof(stmp));
		strncpy(stmp, &buf->curBuf[index], 2);
		t4 = strtoul(stmp, NULL, 10);
		ftmp = (float)((tp&0xFF)/100);
		snd1.iwmData.thisamount = (t4&0xFF)*10000 + (t2&0xFF)*100 + (t0&0xFF) + ftmp;
		snd2.ihmData.thisamount = snd1.iwmData.thisamount;
		snd3.igmData.thisamount = snd1.iwmData.thisamount;
		snd4.ipmData.thisamount = snd1.iwmData.thisamount;
	}

	//写新秘钥
	else if ((ctrl == HCU_NBIOT_CJ188_CTRL_WRITE_DATA) && (d0d1Id == HCU_NBIOT_CJ188_WRITE_DI0DI1_NEW_KEY)){
		if (msgLen != 0x0C){
			HcuErrorPrint("NBIOTCJ188: Invalid received in-line message length on HCU_NBIOT_CJ188_WRITE_DI0DI1_NEW_KEY!\n");
			zHcuRunErrCnt[TASK_ID_NBIOTCJ188]++;
			return FAILURE;
		}
		//新秘钥版本
		index = index + 2;
		memset(stmp, 0, sizeof(stmp));
		strncpy(stmp, &buf->curBuf[index], 2);
		tmp = strtoul(stmp, NULL, 16);
		snd1.iwmData.keyver = tmp & 0xFF;
		snd2.ihmData.keyver = snd1.iwmData.keyver;
		snd3.igmData.keyver = snd1.iwmData.keyver;
		snd4.ipmData.keyver = snd1.iwmData.keyver;
		//新秘钥
		index = index + 2;
		memset(stmp, 0, sizeof(stmp));
		strncpy(stmp, &buf->curBuf[index], 16);
		snd1.iwmData.key = strtoul(stmp, NULL, 16);  //不考虑高低位
		snd2.ihmData.key = snd1.iwmData.key;
		snd3.igmData.key = snd1.iwmData.key;
		snd4.ipmData.key = snd1.iwmData.key;
	}

	//写标准时间
	else if ((ctrl == HCU_NBIOT_CJ188_CTRL_WRITE_DATA) && (d0d1Id == HCU_NBIOT_CJ188_WRITE_DI0DI1_STD_TIME)){
		if (msgLen != 0x0A){
			HcuErrorPrint("NBIOTCJ188: Invalid received in-line message length on HCU_NBIOT_CJ188_WRITE_DI0DI1_STD_TIME!\n");
			zHcuRunErrCnt[TASK_ID_NBIOTCJ188]++;
			return FAILURE;
		}
		//标准时间
		index = index + 2;
		memset(stmp, 0, sizeof(stmp));
		strncpy(stmp, &buf->curBuf[index], 14);
		if (strtoul(stmp, NULL, 10) < HCU_NBIOT_CJ188_REAL_TIME_IN_REALITY){
			HcuErrorPrint("NBIOTCJ188: Invalid received real time!\n");
			zHcuRunErrCnt[TASK_ID_NBIOTCJ188]++;
			return FAILURE;
		}
		strncpy(snd1.iwmData.realtime, &buf->curBuf[index], 14);
		strncpy(snd2.ihmData.realtime, snd1.iwmData.realtime, 14);
		strncpy(snd3.igmData.realtime, snd1.iwmData.realtime, 14);
		strncpy(snd4.ipmData.realtime, snd1.iwmData.realtime, 14);
	}

	//写阀门控制
	else if ((ctrl == HCU_NBIOT_CJ188_CTRL_WRITE_DATA) && (d0d1Id == HCU_NBIOT_CJ188_WRITE_DI0DI1_SWITCH_CTRL)){
		if (msgLen != 0x04){
			HcuErrorPrint("NBIOTCJ188: Invalid received in-line message length on HCU_NBIOT_CJ188_WRITE_DI0DI1_SWITCH_CTRL!\n");
			zHcuRunErrCnt[TASK_ID_NBIOTCJ188]++;
			return FAILURE;
		}
		//阀门开关操作
		index = index + 2;
		memset(stmp, 0, sizeof(stmp));
		strncpy(stmp, &buf->curBuf[index], 2);
		tmp = strtoul(stmp, NULL, 10);
		if ((tmp != HCU_NBIOT_CJ188_SWITCH_CONTROL_ON) && (tmp != HCU_NBIOT_CJ188_SWITCH_CONTROL_OFF)){
			HcuErrorPrint("NBIOTCJ188: Invalid received switch control command!\n");
			zHcuRunErrCnt[TASK_ID_NBIOTCJ188]++;
			return FAILURE;
		}
		snd1.iwmData.switchctrl = tmp;
		snd2.ihmData.switchctrl = snd1.iwmData.switchctrl;
		snd3.igmData.switchctrl = snd1.iwmData.switchctrl;
		snd4.ipmData.switchctrl = snd1.iwmData.switchctrl;
	}

	//写出厂设置
	else if ((ctrl == HCU_NBIOT_CJ188_CTRL_WRITE_DATA) && (d0d1Id == HCU_NBIOT_CJ188_WRITE_DI0DI1_OFF_FACTORY_START)){
		if (msgLen != 0x03){
			HcuErrorPrint("NBIOTCJ188: Invalid received in-line message length on HCU_NBIOT_CJ188_WRITE_DI0DI1_OFF_FACTORY_START!\n");
			zHcuRunErrCnt[TASK_ID_NBIOTCJ188]++;
			return FAILURE;
		}
		//Do nothing
	}

	//写地址
	else if ((ctrl == HCU_NBIOT_CJ188_CTRL_WRITE_ADDR) && (d0d1Id == HCU_NBIOT_CJ188_WRITE_DI0DI1_ADDRESS)){
		if (msgLen != 0x0A){
			HcuErrorPrint("NBIOTCJ188: Invalid received in-line message length on HCU_NBIOT_CJ188_WRITE_DI0DI1_ADDRESS!\n");
			zHcuRunErrCnt[TASK_ID_NBIOTCJ188]++;
			return FAILURE;
		}
		//新地址
		index = index + 2;
		strncpy(snd1.iwmHead.addr, &buf->curBuf[index], 14);
		strncpy(snd2.ihmHead.addr, snd1.iwmHead.addr, 14);
		strncpy(snd3.igmHead.addr, snd1.iwmHead.addr, 14);
		strncpy(snd4.ipmHead.addr, snd1.iwmHead.addr, 14);
	}

	//写电机同步数据
	else if ((ctrl == HCU_NBIOT_CJ188_CTRL_WRITE_DEVICE_SYN) && (d0d1Id == HCU_NBIOT_CJ188_WRITE_DI0DI1_DEVICE_SYN_DATA)){
		if (msgLen != 0x08){
			HcuErrorPrint("NBIOTCJ188: Invalid received in-line message length on HCU_NBIOT_CJ188_WRITE_DI0DI1_DEVICE_SYN_DATA!\n");
			zHcuRunErrCnt[TASK_ID_NBIOTCJ188]++;
			return FAILURE;
		}
		//当前累计流量
		index = index + 2;
		memset(stmp, 0, sizeof(stmp));
		strncpy(stmp, &buf->curBuf[index], 2);
		tp = strtoul(stmp, NULL, 10);
		index = index + 2;
		memset(stmp, 0, sizeof(stmp));
		strncpy(stmp, &buf->curBuf[index], 2);
		t0 = strtoul(stmp, NULL, 10);
		index = index + 2;
		memset(stmp, 0, sizeof(stmp));
		strncpy(stmp, &buf->curBuf[index], 2);
		t2 = strtoul(stmp, NULL, 10);
		index = index + 2;
		memset(stmp, 0, sizeof(stmp));
		strncpy(stmp, &buf->curBuf[index], 2);
		t4 = strtoul(stmp, NULL, 10);
		ftmp = (float)((tp&0xFF)/100);
		snd1.iwmData.currentaccuvolume = (t4&0xFF)*10000 + (t2&0xFF)*100 + (t0&0xFF) + ftmp;
		snd2.ihmData.currentaccuvolume = snd1.iwmData.currentaccuvolume;
		snd3.igmData.currentaccuvolume = snd1.iwmData.currentaccuvolume;
		snd4.ipmData.currentaccuvolume = snd1.iwmData.currentaccuvolume;
		index = index + 2;
		memset(stmp, 0, sizeof(stmp));
		strncpy(stmp, &buf->curBuf[index], 2);
		tmp = strtoul(stmp, NULL, 16);
		snd1.iwmData.currentaccuvolumeunit = tmp & 0xFF;
		snd2.ihmData.currentaccuvolumeunit = snd1.iwmData.currentaccuvolumeunit;
		snd3.igmData.currentaccuvolumeunit = snd1.iwmData.currentaccuvolumeunit;
		snd4.ipmData.currentaccuvolumeunit = snd1.iwmData.currentaccuvolumeunit;
	}

	//命令组合不存在
	else{
		zHcuRunErrCnt[TASK_ID_NBIOTCJ188]++;
		HcuErrorPrint("NBIOTCJ188: Received error CTRL + D0D1 combination command!\n");
		return FAILURE;
	}


	//终于到了将消息发送到各个L３进行具体的处理的过程了
	if ((equType >= HCU_NBIOT_CJ188_T_TYPE_WATER_METER_MIN) && (equType <= HCU_NBIOT_CJ188_T_TYPE_WATER_METER_MAX)){
		//发送
		ret = hcu_message_send(MSG_ID_NBIOTCJ188_IWM_DATA_REQ, TASK_ID_IWM, TASK_ID_NBIOTCJ188, &snd1, snd1.length);
		if (ret == FAILURE){
			zHcuRunErrCnt[TASK_ID_NBIOTCJ188]++;
			HcuErrorPrint("NBIOTCJ188: Send message error, TASK [%s] to TASK[%s]!\n", zHcuTaskInfo[TASK_ID_NBIOTCJ188].taskName, zHcuTaskInfo[TASK_ID_IWM].taskName);
			return FAILURE;
		}
	}//IWM
	else if ((equType >= HCU_NBIOT_CJ188_T_TYPE_HEAT_METER_MIN) && (equType <= HCU_NBIOT_CJ188_T_TYPE_HEAT_METER_MAX)){

		//发送
		ret = hcu_message_send(MSG_ID_NBIOTCJ188_IHM_DATA_REQ, TASK_ID_IHM, TASK_ID_NBIOTCJ188, &snd2, snd2.length);
		if (ret == FAILURE){
			zHcuRunErrCnt[TASK_ID_NBIOTCJ188]++;
			HcuErrorPrint("NBIOTCJ188: Send message error, TASK [%s] to TASK[%s]!\n", zHcuTaskInfo[TASK_ID_NBIOTCJ188].taskName, zHcuTaskInfo[TASK_ID_IHM].taskName);
			return FAILURE;
		}
	}//IHM
	else if ((equType >= HCU_NBIOT_CJ188_T_TYPE_GAS_METER_MIN) && (equType <= HCU_NBIOT_CJ188_T_TYPE_GAS_METER_MAX)){

		//发送
		ret = hcu_message_send(MSG_ID_NBIOTCJ188_IGM_DATA_REQ, TASK_ID_IGM, TASK_ID_NBIOTCJ188, &snd3, snd3.length);
		if (ret == FAILURE){
			zHcuRunErrCnt[TASK_ID_NBIOTCJ188]++;
			HcuErrorPrint("NBIOTCJ188: Send message error, TASK [%s] to TASK[%s]!\n", zHcuTaskInfo[TASK_ID_NBIOTCJ188].taskName, zHcuTaskInfo[TASK_ID_IGM].taskName);
			return FAILURE;
		}
	}//IGM
	else if ((equType >= HCU_NBIOT_CJ188_T_TYPE_GAS_METER_MIN) && (equType <= HCU_NBIOT_CJ188_T_TYPE_GAS_METER_MAX)){

		//发送
		ret = hcu_message_send(MSG_ID_NBIOTCJ188_IPM_DATA_REQ, TASK_ID_IPM, TASK_ID_NBIOTCJ188, &snd4, snd4.length);
		if (ret == FAILURE){
			zHcuRunErrCnt[TASK_ID_NBIOTCJ188]++;
			HcuErrorPrint("NBIOTCJ188: Send message error, TASK [%s] to TASK[%s]!\n", zHcuTaskInfo[TASK_ID_NBIOTCJ188].taskName, zHcuTaskInfo[TASK_ID_IPM].taskName);
			return FAILURE;
		}
	}//IPM
	else{
		zHcuRunErrCnt[TASK_ID_NBIOTCJ188]++;
		HcuErrorPrint("NBIOTCJ188: Received error equipment + D0D1 combination command!\n");
		return FAILURE;
	}

	return SUCCESS;
}

