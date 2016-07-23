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
FsmStateItem_t FsmNbiotcj188[] =
{
    //MessageId                 //State                   		 		//Function
	//启始点，固定定义，不要改动, 使用ENTRY/END，意味者MSGID肯定不可能在某个高位区段中；考虑到所有任务共享MsgId，即使分段，也无法实现
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
extern HcuSysEngParTablet_t zHcuSysEngPar; //全局工程参数控制表

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
			HcuErrorPrint("NBIOTCJ188: Send message error, TASK [%s] to TASK[%s]!\n", zHcuTaskNameList[TASK_ID_NBIOTCJ188], zHcuTaskNameList[src_id]);
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
			HcuErrorPrint("NBIOTCJ188: Send message error, TASK [%s] to TASK[%s]!\n", zHcuTaskNameList[TASK_ID_NBIOTCJ188], zHcuTaskNameList[TASK_ID_NBIOTCJ188]);
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
	input.todayaccuvolume = rcv.todayaccuvolume;
	input.todayaccuvolumeunit = rcv.todayaccuvolumeunit;
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
	input.todayaccuvolume = rcv.todayaccuvolume;
	input.todayaccuvolumeunit = rcv.todayaccuvolumeunit;
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
	input.todayheat = rcv.todayheat;
	input.todayheatunit = rcv.todayheatunit;
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
	input.todayheat = rcv.todayheat;
	input.todayheatunit = rcv.todayheatunit;
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
	input.todayaccuvolume = rcv.todayaccuvolume;
	input.todayaccuvolumeunit = rcv.todayaccuvolumeunit;
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
	input.todayaccuvolume = rcv.todayaccuvolume;
	input.todayaccuvolumeunit = rcv.todayaccuvolumeunit;
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
	input.todayaccuvolume = rcv.todayaccuvolume;
	input.todayaccuvolumeunit = rcv.todayaccuvolumeunit;
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
	input.todayaccuvolume = rcv.todayaccuvolume;
	input.todayaccuvolumeunit = rcv.todayaccuvolumeunit;
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

//上行链路处理过程
OPSTAT fsm_nbiotcj188_ethernet_data_rx(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
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
	if (FsmGetState(TASK_ID_NBIOTCJ188) == FSM_STATE_NBIOTCJ188_ONLINE){
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
	}else{
		zHcuRunErrCnt[TASK_ID_NBIOTCJ188]++;
		HcuErrorPrint("NBIOTCJ188: Error send HEART_BEAT to cloud, get by ONLINE, but back off line so quick!\n");
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


	return SUCCESS;
}

//消息unpack函数
OPSTAT func_nbiotcj188_dl_msg_unpack(NbiotCj188BhItfComElement_t *output, CloudDataSendBuf_t *buf)
{
	return SUCCESS;
}


