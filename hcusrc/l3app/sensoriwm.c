/*
 * sensoriwm.c
 *
 *  Created on: 2016年7月20日
 *      Author: hitpony
 */

#include "sensoriwm.h"

#include "../l0service/timer.h"
#include "../l0service/trace.h"
#include "../l1com/l1comdef.h"

/*
** FSM of the IWM
*/
HcuFsmStateItem_t HcuFsmIwm[] =
{
    //MessageId                 //State                   		 		//Function
	//启始点，固定定义，不要改动, 使用ENTRY/END，意味者MSGID肯定不可能在某个高位区段中；考虑到所有任务共享MsgId，即使分段，也无法实现
	//完全是为了给任务一个初始化的机会，按照状态转移机制，该函数不具备启动的机会，因为任务初始化后自动到FSM_STATE_IDLE
	//如果没有必要进行初始化，可以设置为NULL
	{MSG_ID_ENTRY,       						FSM_STATE_ENTRY,            			fsm_iwm_task_entry}, //Starting

	//System level initialization, only controlled by HCU-MAIN
    {MSG_ID_COM_INIT,       					FSM_STATE_IDLE,            				fsm_iwm_init},
    {MSG_ID_COM_INIT_FEEDBACK,					FSM_STATE_IDLE,            				fsm_com_do_nothing},

	//Task level initialization
    {MSG_ID_COM_INIT,       					FSM_STATE_IWM_INITED,            		fsm_iwm_init},
    {MSG_ID_COM_INIT_FEEDBACK,					FSM_STATE_IWM_INITED,            		fsm_com_do_nothing},

	//ANY state entry
    {MSG_ID_COM_INIT_FEEDBACK,					FSM_STATE_COMMON,          				fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT,       				FSM_STATE_COMMON,          				fsm_com_heart_beat_rcv},
	{MSG_ID_COM_STOP,       					FSM_STATE_COMMON,          				fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT_FB,       			FSM_STATE_COMMON,          				fsm_com_do_nothing},
    {MSG_ID_COM_RESTART,						FSM_STATE_COMMON,            			fsm_iwm_restart},
	{MSG_ID_COM_TIME_OUT,       				FSM_STATE_COMMON,          				fsm_iwm_time_out},

	//Normal working status
	{MSG_ID_NBIOTCJ188_IWM_DATA_REQ,     		FSM_STATE_IWM_ACTIVED,      			fsm_iwm_nbiotcj188_data_req},
	{MSG_ID_NBIOTCJ188_IWM_CONTROL_CMD,  		FSM_STATE_IWM_ACTIVED,          		fsm_iwm_nbiotcj188_control_cmd},

    //结束点，固定定义，不要改动
    {MSG_ID_END,            	FSM_STATE_END,             				NULL},  //Ending
};

//Global variables

//Main Entry
//Input parameter would be useless, but just for similar structure purpose
OPSTAT fsm_iwm_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//除了对全局变量进行操作之外，尽量不要做其它操作，因为该函数将被主任务/线程调用，不是本任务/线程调用
	//该API就是给本任务一个提早介入的入口，可以帮着做些测试性操作
	if (FsmSetState(TASK_ID_IWM, FSM_STATE_IDLE) == FAILURE){
		HcuErrorPrint("IWM: Error Set FSM State at fsm_iwm_task_entry\n");}
	return SUCCESS;
}

OPSTAT fsm_iwm_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;

	if ((src_id > TASK_ID_MIN) &&(src_id < TASK_ID_MAX)){
		//Send back MSG_ID_COM_INIT_FEEDBACK to SVRCON
		msg_struct_com_init_feedback_t snd0;
		memset(&snd0, 0, sizeof(msg_struct_com_init_feedback_t));
		snd0.length = sizeof(msg_struct_com_init_feedback_t);

		//to avoid all task send out the init fb msg at the same time which lead to msgque get stuck
		hcu_usleep(dest_id*HCU_SYSCFG_DURATION_OF_INIT_FB_WAIT_MAX);

		ret = hcu_message_send(MSG_ID_COM_INIT_FEEDBACK, src_id, TASK_ID_IWM, &snd0, snd0.length);
		if (ret == FAILURE){
			HcuErrorPrint("IWM: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_IWM].taskName, zHcuVmCtrTab.task[src_id].taskName);
			return FAILURE;
		}
	}

	//收到初始化消息后，进入初始化状态
	if (FsmSetState(TASK_ID_IWM, FSM_STATE_IWM_INITED) == FAILURE){
		HcuErrorPrint("IWM: Error Set FSM State!\n");
		return FAILURE;
	}

	//初始化硬件接口
	if (func_iwm_int_init() == FAILURE){
		HcuErrorPrint("IWM: Error initialize interface!\n");
		return FAILURE;
	}

	//Global Variables
	zHcuSysStaPm.taskRunErrCnt[TASK_ID_IWM] = 0;

	//启动周期性定时器
	ret = hcu_timer_start(TASK_ID_IWM, TIMER_ID_1S_IWM_PERIOD_READ, \
			zHcuSysEngPar.timer.array[TIMER_ID_1S_IWM_PERIOD_READ].dur, TIMER_TYPE_PERIOD, TIMER_RESOLUTION_1S);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_IWM]++;
		HcuErrorPrint("IWM: Error start period timer!\n");
		return FAILURE;
	}

	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_IWM, FSM_STATE_IWM_ACTIVED) == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_IWM]++;
		HcuErrorPrint("IWM: Error Set FSM State!\n");
		return FAILURE;
	}
	if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_FAT_ON) != FALSE){
		HcuDebugPrint("IWM: Enter FSM_STATE_IWM_ACTIVED status, Keeping refresh here!\n");
	}
	/*

	//进入阻塞式接收数据状态，然后继续发送
	while(1){
		//接收数据
		int dataLen=0;
		if (dataLen > 1){
			//发送数据给HSMMP
			msg_struct_iwm_hsmmp_data_rx_t snd;
			memset(&snd, 0, sizeof(msg_struct_iwm_hsmmp_data_rx_t));
			snd.length = sizeof(msg_struct_iwm_hsmmp_data_rx_t);
			ret = hcu_message_send(MSG_ID_IWM_HSMMP_DATA_RX, TASK_ID_HSMMP, TASK_ID_IWM, &snd, snd.length);
			if (ret == FAILURE){
				zHcuRunErrCnt[TASK_ID_IWM]++;
				HcuErrorPrint("IWM: Send message error, TASK [%s] to TASK[%s]!\n", zHcuTaskInfo[TASK_ID_IWM].taskName, zHcuTaskInfo[TASK_ID_HSMMP].taskName);
				return FAILURE;
			}
		}

		hcu_sleep(5);
	}
	*/

	return SUCCESS;
}

OPSTAT fsm_iwm_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HcuErrorPrint("IWM: Internal error counter reach DEAD level, SW-RESTART soon!\n");
	zHcuSysStaPm.statisCnt.restartCnt++;
	fsm_iwm_init(0, 0, NULL, 0);
	return SUCCESS;
}

OPSTAT func_iwm_int_init(void)
{
	return SUCCESS;
}

//暂时啥都不干，定时到达后，还不明确是否需要支持定时工作
OPSTAT fsm_iwm_time_out(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret;

	//Receive message and copy to local variable
	msg_struct_com_time_out_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_com_time_out_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_com_time_out_t))){
		HcuErrorPrint("IWM: Receive message error!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_IWM]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//钩子在此处，检查zHcuRunErrCnt[TASK_ID_IWM]是否超限
	if (zHcuSysStaPm.taskRunErrCnt[TASK_ID_IWM] > HCU_RUN_ERROR_LEVEL_2_MAJOR){
		//减少重复RESTART的概率
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_IWM] = zHcuSysStaPm.taskRunErrCnt[TASK_ID_IWM] - HCU_RUN_ERROR_LEVEL_2_MAJOR;
		msg_struct_com_restart_t snd0;
		memset(&snd0, 0, sizeof(msg_struct_com_restart_t));
		snd0.length = sizeof(msg_struct_com_restart_t);
		ret = hcu_message_send(MSG_ID_COM_RESTART, TASK_ID_IWM, TASK_ID_IWM, &snd0, snd0.length);
		if (ret == FAILURE){
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_IWM]++;
			HcuErrorPrint("IWM: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_IWM].taskName, zHcuVmCtrTab.task[TASK_ID_IWM].taskName);
			return FAILURE;
		}
	}

	//Period time out received
	if ((rcv.timeId == TIMER_ID_1S_IWM_PERIOD_READ) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		//保护周期读数的优先级，强制抢占状态，并简化问题
		if (FsmGetState(TASK_ID_IWM) != FSM_STATE_IWM_ACTIVED){
			ret = FsmSetState(TASK_ID_IWM, FSM_STATE_IWM_ACTIVED);
			if (ret == FAILURE){
				zHcuSysStaPm.taskRunErrCnt[TASK_ID_IWM]++;
				HcuErrorPrint("IWM: Error Set FSM State!\n");
				return FAILURE;
			}//FsmSetState
		}

		//Do nothing

	}

	return SUCCESS;
}

OPSTAT fsm_iwm_nbiotcj188_data_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;
	msg_struct_nbiotcj188_iwm_data_req_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_nbiotcj188_iwm_data_req_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_nbiotcj188_iwm_data_req_t))){
		HcuErrorPrint("IWM: Receive message error!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_IWM]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//验证入参的正确性
	if ((rcv.equtype < HCU_NBIOT_CJ188_T_TYPE_WATER_METER_MIN) || (rcv.equtype > HCU_NBIOT_CJ188_T_TYPE_WATER_METER_MAX)){
		HcuErrorPrint("IWM: Receive message error!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_IWM]++;
		return FAILURE;
	}

	//包括地址信息是否正确
	//仪表的状态是否合适等等


	//申明新的参量，目的是为了回复参数
	char tmp[17] = "";
	msg_struct_iwm_nbiotcj188_data_resp_t iwmResp;
	memset(&iwmResp, 0, sizeof(msg_struct_iwm_nbiotcj188_data_resp_t));
	iwmResp.equtype = rcv.equtype;
	iwmResp.iwmHead.ctrlId = rcv.iwmHead.ctrlId;
	strcpy(iwmResp.iwmHead.addr, rcv.iwmHead.addr);
	iwmResp.iwmHead.d0d1Id = rcv.iwmHead.d0d1Id;
	iwmResp.iwmHead.ser = rcv.iwmHead.ser;
	iwmResp.iwmHead.periodFlag = rcv.iwmHead.periodFlag;
	iwmResp.iwmHead.timestamp = time(0);

	//如果探测到通信异常，可以设置这个标志位
	iwmResp.iwmHead.communicationFlag = HCU_NBIOT_CJ188_COMM_CHANNEL_NORMAL;

	//根据命令，对仪表分别进行操控
	//准备读取控制的仪表的数据
	if ((rcv.iwmHead.ctrlId == HCU_NBIOT_CJ188_CTRL_READ_DATA) && (rcv.iwmHead.d0d1Id == HCU_NBIOT_CJ188_READ_DI0DI1_CURRENT_COUNTER_DATA)){
		//设置假数据，为了测试方便
		iwmResp.iwmData.currentaccuvolume = rand()%1000000;
		iwmResp.iwmData.currentaccuvolumeunit = HCU_NBIOT_CJ188_UNIT_M3;
		iwmResp.billtodayaccuvolume = rand()%1000000;
		iwmResp.billtodayaccuvolumeunit = HCU_NBIOT_CJ188_UNIT_M3;
		//如果需要设置当前的时间的话
		time_t lt;
		struct tm *cu;
		lt=time(NULL);
		cu = localtime(&lt);
		memset(tmp, 0, sizeof(tmp));
		sprintf(tmp,  "%04d%02d%02d%02d%02d%02d", (UINT16)(1900+cu->tm_year), (UINT8)cu->tm_mon, (UINT8)cu->tm_mday, (UINT8)cu->tm_hour, (UINT8)cu->tm_min, (UINT8)cu->tm_sec);
		strncpy(iwmResp.iwmData.realtime, tmp, 14);
		memset(tmp, 0, sizeof(tmp));
		sprintf(tmp, "%04d", rand()%10000);
		strncpy(iwmResp.iwmData.st, tmp, 4);
	}
	else if ((rcv.iwmHead.ctrlId == HCU_NBIOT_CJ188_CTRL_READ_DATA) && (rcv.iwmHead.d0d1Id == HCU_NBIOT_CJ188_READ_DI0DI1_HISTORY_COUNTER_DATA1)){
		//设置假数据，为了测试方便
		iwmResp.billtodayaccuvolume = rand()%1000000;
		iwmResp.billtodayaccuvolumeunit = HCU_NBIOT_CJ188_UNIT_M3;
		iwmResp.iwmData.lastmonth = 1;
	}
	else if ((rcv.iwmHead.ctrlId == HCU_NBIOT_CJ188_CTRL_READ_DATA) && (rcv.iwmHead.d0d1Id == HCU_NBIOT_CJ188_READ_DI0DI1_HISTORY_COUNTER_DATA2)){
		//设置假数据，为了测试方便
		iwmResp.billtodayaccuvolume = rand()%1000000;
		iwmResp.billtodayaccuvolumeunit = HCU_NBIOT_CJ188_UNIT_M3;
		iwmResp.iwmData.lastmonth = 2;
	}
	else if ((rcv.iwmHead.ctrlId == HCU_NBIOT_CJ188_CTRL_READ_DATA) && (rcv.iwmHead.d0d1Id == HCU_NBIOT_CJ188_READ_DI0DI1_HISTORY_COUNTER_DATA3)){
		//设置假数据，为了测试方便
		iwmResp.billtodayaccuvolume = rand()%1000000;
		iwmResp.billtodayaccuvolumeunit = HCU_NBIOT_CJ188_UNIT_M3;
		iwmResp.iwmData.lastmonth = 3;
	}
	else if ((rcv.iwmHead.ctrlId == HCU_NBIOT_CJ188_CTRL_READ_DATA) && (rcv.iwmHead.d0d1Id == HCU_NBIOT_CJ188_READ_DI0DI1_HISTORY_COUNTER_DATA4)){
		//设置假数据，为了测试方便
		iwmResp.billtodayaccuvolume = rand()%1000000;
		iwmResp.billtodayaccuvolumeunit = HCU_NBIOT_CJ188_UNIT_M3;
		iwmResp.iwmData.lastmonth = 4;
	}
	else if ((rcv.iwmHead.ctrlId == HCU_NBIOT_CJ188_CTRL_READ_DATA) && (rcv.iwmHead.d0d1Id == HCU_NBIOT_CJ188_READ_DI0DI1_HISTORY_COUNTER_DATA5)){
		//设置假数据，为了测试方便
		iwmResp.billtodayaccuvolume = rand()%1000000;
		iwmResp.billtodayaccuvolumeunit = HCU_NBIOT_CJ188_UNIT_M3;
		iwmResp.iwmData.lastmonth = 5;
	}
	else if ((rcv.iwmHead.ctrlId == HCU_NBIOT_CJ188_CTRL_READ_DATA) && (rcv.iwmHead.d0d1Id == HCU_NBIOT_CJ188_READ_DI0DI1_HISTORY_COUNTER_DATA6)){
		//设置假数据，为了测试方便
		iwmResp.billtodayaccuvolume = rand()%1000000;
		iwmResp.billtodayaccuvolumeunit = HCU_NBIOT_CJ188_UNIT_M3;
		iwmResp.iwmData.lastmonth = 6;
	}
	else if ((rcv.iwmHead.ctrlId == HCU_NBIOT_CJ188_CTRL_READ_DATA) && (rcv.iwmHead.d0d1Id == HCU_NBIOT_CJ188_READ_DI0DI1_HISTORY_COUNTER_DATA7)){
		//设置假数据，为了测试方便
		iwmResp.billtodayaccuvolume = rand()%1000000;
		iwmResp.billtodayaccuvolumeunit = HCU_NBIOT_CJ188_UNIT_M3;
		iwmResp.iwmData.lastmonth = 7;
	}
	else if ((rcv.iwmHead.ctrlId == HCU_NBIOT_CJ188_CTRL_READ_DATA) && (rcv.iwmHead.d0d1Id == HCU_NBIOT_CJ188_READ_DI0DI1_HISTORY_COUNTER_DATA8)){
		//设置假数据，为了测试方便
		iwmResp.billtodayaccuvolume = rand()%1000000;
		iwmResp.billtodayaccuvolumeunit = HCU_NBIOT_CJ188_UNIT_M3;
		iwmResp.iwmData.lastmonth = 8;
	}
	else if ((rcv.iwmHead.ctrlId == HCU_NBIOT_CJ188_CTRL_READ_DATA) && (rcv.iwmHead.d0d1Id == HCU_NBIOT_CJ188_READ_DI0DI1_HISTORY_COUNTER_DATA9)){
		//设置假数据，为了测试方便
		iwmResp.billtodayaccuvolume = rand()%1000000;
		iwmResp.billtodayaccuvolumeunit = HCU_NBIOT_CJ188_UNIT_M3;
		iwmResp.iwmData.lastmonth = 9;
	}
	else if ((rcv.iwmHead.ctrlId == HCU_NBIOT_CJ188_CTRL_READ_DATA) && (rcv.iwmHead.d0d1Id == HCU_NBIOT_CJ188_READ_DI0DI1_HISTORY_COUNTER_DATA10)){
		//设置假数据，为了测试方便
		iwmResp.billtodayaccuvolume = rand()%1000000;
		iwmResp.billtodayaccuvolumeunit = HCU_NBIOT_CJ188_UNIT_M3;
		iwmResp.iwmData.lastmonth = 10;
	}
	else if ((rcv.iwmHead.ctrlId == HCU_NBIOT_CJ188_CTRL_READ_DATA) && (rcv.iwmHead.d0d1Id == HCU_NBIOT_CJ188_READ_DI0DI1_HISTORY_COUNTER_DATA11)){
		//设置假数据，为了测试方便
		iwmResp.billtodayaccuvolume = rand()%1000000;
		iwmResp.billtodayaccuvolumeunit = HCU_NBIOT_CJ188_UNIT_M3;
		iwmResp.iwmData.lastmonth = 11;
	}
	else if ((rcv.iwmHead.ctrlId == HCU_NBIOT_CJ188_CTRL_READ_DATA) && (rcv.iwmHead.d0d1Id == HCU_NBIOT_CJ188_READ_DI0DI1_HISTORY_COUNTER_DATA12)){
		//设置假数据，为了测试方便
		iwmResp.billtodayaccuvolume = rand()%1000000;
		iwmResp.billtodayaccuvolumeunit = HCU_NBIOT_CJ188_UNIT_M3;
		iwmResp.iwmData.lastmonth = 12;
	}
	else if ((rcv.iwmHead.ctrlId == HCU_NBIOT_CJ188_CTRL_READ_DATA) && (rcv.iwmHead.d0d1Id == HCU_NBIOT_CJ188_READ_DI0DI1_PRICE_TABLE)){
		//设置假数据，为了测试方便
		iwmResp.iwmData.price1 = rand()%1000000;
		iwmResp.iwmData.volume1 = rand()%1000000;
		iwmResp.iwmData.price2 = rand()%1000000;
		iwmResp.iwmData.volume2 = rand()%1000000;
		iwmResp.iwmData.price3 = rand()%1000000;
	}
	else if ((rcv.iwmHead.ctrlId == HCU_NBIOT_CJ188_CTRL_READ_DATA) && (rcv.iwmHead.d0d1Id == HCU_NBIOT_CJ188_READ_DI0DI1_BILL_TODAY_DATE)){ //结算日
		//设置假数据，为了测试方便
		iwmResp.iwmData.billtodaydate = rand()%256;
	}
	else if ((rcv.iwmHead.ctrlId == HCU_NBIOT_CJ188_CTRL_READ_DATA) && (rcv.iwmHead.d0d1Id == HCU_NBIOT_CJ188_READ_DI0DI1_READ_ACCOUNT_CUR_DATE)){ //抄表日
		//设置假数据，为了测试方便
		iwmResp.iwmData.readamountcurdate = rand()%256;
	}
	else if ((rcv.iwmHead.ctrlId == HCU_NBIOT_CJ188_CTRL_READ_DATA) && (rcv.iwmHead.d0d1Id == HCU_NBIOT_CJ188_READ_DI0DI1_BUY_AMOUNT)){ //购入金额
		//设置假数据，为了测试方便
		iwmResp.iwmData.thisamount = rand()%1000000;
		iwmResp.iwmData.accuamount = rand()%1000000;
		iwmResp.iwmData.remainamount = rand()%1000000;
		memset(tmp, 0, sizeof(tmp));
		sprintf(tmp, "%04d", rand()%10000);
		strncpy(iwmResp.iwmData.st, tmp, 4);
	}
	else if ((rcv.iwmHead.ctrlId == HCU_NBIOT_CJ188_CTRL_READ_KEY_VER) && (rcv.iwmHead.d0d1Id == HCU_NBIOT_CJ188_READ_DI0DI1_KEY_VER)){
		//设置假数据，为了测试方便
		iwmResp.iwmData.keyver = rand()%256;
	}
	else if ((rcv.iwmHead.ctrlId == HCU_NBIOT_CJ188_CTRL_READ_ADDR) && (rcv.iwmHead.d0d1Id == HCU_NBIOT_CJ188_READ_DI0DI1_ADDRESS)){
		//设置假数据，为了测试方便
		//Do nothing
	}
	else if ((rcv.iwmHead.ctrlId == HCU_NBIOT_CJ188_CTRL_WRITE_DATA) && (rcv.iwmHead.d0d1Id == HCU_NBIOT_CJ188_WRITE_DI0DI1_PRICE_TABLE)){
		//设置假数据，为了测试方便
		memset(tmp, 0, sizeof(tmp));
		sprintf(tmp, "%04d", rand()%10000);
		strncpy(iwmResp.iwmData.st, tmp, 4);
	}
	else if ((rcv.iwmHead.ctrlId == HCU_NBIOT_CJ188_CTRL_WRITE_DATA) && (rcv.iwmHead.d0d1Id == HCU_NBIOT_CJ188_WRITE_DI0DI1_BILL_TODAY_DATE)){
		//设置假数据，为了测试方便
		//Do nothing
	}
	else if ((rcv.iwmHead.ctrlId == HCU_NBIOT_CJ188_CTRL_WRITE_DATA) && (rcv.iwmHead.d0d1Id == HCU_NBIOT_CJ188_WRITE_DI0DI1_READ_ACCOUNT_CUR_DATE)){
		//设置假数据，为了测试方便
		//Do nothing
	}
	else if ((rcv.iwmHead.ctrlId == HCU_NBIOT_CJ188_CTRL_WRITE_DATA) && (rcv.iwmHead.d0d1Id == HCU_NBIOT_CJ188_WRITE_DI0DI1_BUY_AMOUNT)){
		//设置假数据，为了测试方便
		iwmResp.iwmData.buycode = rand()%256;
		iwmResp.iwmData.thisamount = rand()%1000000;
	}
	else if ((rcv.iwmHead.ctrlId == HCU_NBIOT_CJ188_CTRL_WRITE_DATA) && (rcv.iwmHead.d0d1Id == HCU_NBIOT_CJ188_WRITE_DI0DI1_NEW_KEY)){
		//设置假数据，为了测试方便
		iwmResp.iwmData.keyver = rand()%256;
	}
	else if ((rcv.iwmHead.ctrlId == HCU_NBIOT_CJ188_CTRL_WRITE_DATA) && (rcv.iwmHead.d0d1Id == HCU_NBIOT_CJ188_WRITE_DI0DI1_STD_TIME)){
		//设置假数据，为了测试方便
		//Do nothing
	}
	else if ((rcv.iwmHead.ctrlId == HCU_NBIOT_CJ188_CTRL_WRITE_DATA) && (rcv.iwmHead.d0d1Id == HCU_NBIOT_CJ188_WRITE_DI0DI1_SWITCH_CTRL)){
		//设置假数据，为了测试方便
		memset(tmp, 0, sizeof(tmp));
		sprintf(tmp, "%04d", rand()%10000);
		strncpy(iwmResp.iwmData.st, tmp, 4);
	}
	else if ((rcv.iwmHead.ctrlId == HCU_NBIOT_CJ188_CTRL_WRITE_DATA) && (rcv.iwmHead.d0d1Id == HCU_NBIOT_CJ188_WRITE_DI0DI1_OFF_FACTORY_START)){
		//设置假数据，为了测试方便
		//Do nothing
	}
	else if ((rcv.iwmHead.ctrlId == HCU_NBIOT_CJ188_CTRL_WRITE_ADDR) && (rcv.iwmHead.d0d1Id == HCU_NBIOT_CJ188_WRITE_DI0DI1_ADDRESS)){
		//设置假数据，为了测试方便
		//Do nothing
	}
	else if ((rcv.iwmHead.ctrlId == HCU_NBIOT_CJ188_CTRL_WRITE_DEVICE_SYN) && (rcv.iwmHead.d0d1Id == HCU_NBIOT_CJ188_WRITE_DI0DI1_DEVICE_SYN_DATA)){
		//设置假数据，为了测试方便
		memset(tmp, 0, sizeof(tmp));
		sprintf(tmp, "%04d", rand()%10000);
		strncpy(iwmResp.iwmData.st, tmp, 4);
	}
	else{
		HcuErrorPrint("IWM: Receive message error!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_IWM]++;
		return FAILURE;
	}

	//准备存入本地数据库, RECORD还要存入数据库
	sensor_iwm_cj188_data_element_t iwmDbSave;
	memset(&iwmDbSave, 0, sizeof(sensor_iwm_cj188_data_element_t));
	memcpy(&iwmDbSave.iwm, &iwmResp.iwmData, sizeof(sensor_general_cj188_data_element_t));
	strcpy(iwmDbSave.cj188address, iwmResp.iwmHead.addr);
	iwmDbSave.equtype = iwmResp.equtype;
	iwmDbSave.timestamp = iwmResp.iwmHead.timestamp;
	iwmDbSave.billtodayaccuvolume = iwmResp.billtodayaccuvolume;
	iwmDbSave.billtodayaccuvolumeunit = iwmResp.billtodayaccuvolumeunit;
	ret = dbi_HcuIwmCj188DataInfo_save(&iwmDbSave);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_IWM]++;
		HcuErrorPrint("IWM: Can not save data into database!\n");
	}

	//准备发送数据回去给NBIOTCJ188，然后通过UL上行链路送回CLOUD平台
	iwmResp.length = sizeof(msg_struct_iwm_nbiotcj188_data_resp_t);
	ret = hcu_message_send(MSG_ID_IWM_NBIOTCJ188_DATA_RESP, TASK_ID_NBIOTCJ188, TASK_ID_IWM, &iwmResp, iwmResp.length);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_IWM]++;
		HcuErrorPrint("IWM: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_IWM].taskName, zHcuVmCtrTab.task[TASK_ID_NBIOTCJ188].taskName);
		return FAILURE;
	}

	//返回
	return SUCCESS;
}

//暂时不用，留待未来使用
OPSTAT fsm_iwm_nbiotcj188_control_cmd(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	return SUCCESS;
}

