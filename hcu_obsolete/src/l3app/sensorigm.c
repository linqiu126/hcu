/*
 * sensorigm.c
 *
 *  Created on: 2016年7月20日
 *      Author: hitpony
 */

#include "sensorigm.h"

#include "../l0service/timer.h"
#include "../l0service/trace.h"
#include "../l1com/l1comdef.h"

/*
** FSM of the IGM
*/
HcuFsmStateItem_t FsmIgm[] =
{
    //MessageId                 //State                   		 		//Function
	//启始点，固定定义，不要改动, 使用ENTRY/END，意味者MSGID肯定不可能在某个高位区段中；考虑到所有任务共享MsgId，即使分段，也无法实现
	//完全是为了给任务一个初始化的机会，按照状态转移机制，该函数不具备启动的机会，因为任务初始化后自动到FSM_STATE_IDLE
	//如果没有必要进行初始化，可以设置为NULL
	{MSG_ID_ENTRY,       						FSM_STATE_ENTRY,            			fsm_igm_task_entry}, //Starting

	//System level initialization, only controlled by HCU-MAIN
    {MSG_ID_COM_INIT,       					FSM_STATE_IDLE,            				fsm_igm_init},
    {MSG_ID_COM_RESTART,						FSM_STATE_IDLE,            				fsm_igm_restart},
    {MSG_ID_COM_INIT_FEEDBACK,					FSM_STATE_IDLE,            				fsm_com_do_nothing},

	//Task level initialization
    {MSG_ID_COM_INIT,       					FSM_STATE_IGM_INITIED,            		fsm_igm_init},
    {MSG_ID_COM_RESTART,						FSM_STATE_IGM_INITIED,            		fsm_igm_restart},
    {MSG_ID_COM_INIT_FEEDBACK,					FSM_STATE_IGM_INITIED,            		fsm_com_do_nothing},

	//Normal working status
    {MSG_ID_COM_RESTART,        				FSM_STATE_IGM_ACTIVED,            		fsm_igm_restart},
    {MSG_ID_COM_INIT_FEEDBACK,					FSM_STATE_IGM_ACTIVED,            		fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT,     				FSM_STATE_IGM_ACTIVED,       			fsm_com_heart_beat_rcv},
	{MSG_ID_COM_HEART_BEAT_FB,  				FSM_STATE_IGM_ACTIVED,       			fsm_com_do_nothing},
	{MSG_ID_COM_TIME_OUT,       				FSM_STATE_IGM_ACTIVED,          		fsm_igm_time_out},
	{MSG_ID_NBIOTCJ188_IGM_DATA_REQ,     		FSM_STATE_IGM_ACTIVED,      			fsm_igm_nbiotcj188_data_req},
	{MSG_ID_NBIOTCJ188_IGM_CTRL_REQ,  		FSM_STATE_IGM_ACTIVED,          		fsm_igm_nbiotcj188_ctrl_req},

    //结束点，固定定义，不要改动
    {MSG_ID_END,            	FSM_STATE_END,             				NULL},  //Ending
};

//Global variables
extern HcuSysEngParTable_t zHcuSysEngPar; //全局工程参数控制表

//Main Entry
//Input parameter would be useless, but just for similar structure purpose
OPSTAT fsm_igm_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//除了对全局变量进行操作之外，尽量不要做其它操作，因为该函数将被主任务/线程调用，不是本任务/线程调用
	//该API就是给本任务一个提早介入的入口，可以帮着做些测试性操作
	if (FsmSetState(TASK_ID_IGM, FSM_STATE_IDLE) == FAILURE){
		HcuErrorPrint("IGM: Error Set FSM State at fsm_igm_task_entry\n");}
	return SUCCESS;
}

OPSTAT fsm_igm_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;

	if ((src_id > TASK_ID_MIN) &&(src_id < TASK_ID_MAX)){
		//Send back MSG_ID_COM_INIT_FEEDBACK to SVRCON
		msg_struct_com_init_feedback_t snd0;
		memset(&snd0, 0, sizeof(msg_struct_com_init_feedback_t));
		snd0.length = sizeof(msg_struct_com_init_feedback_t);

		//to avoid all task send out the init fb msg at the same time which lead to msgque get stuck
		hcu_usleep(dest_id*HCU_SYSCFG_DURATION_OF_INIT_FB_WAIT_MAX);

		ret = hcu_message_send(MSG_ID_COM_INIT_FEEDBACK, src_id, TASK_ID_IGM, &snd0, snd0.length);
		if (ret == FAILURE){
			HcuErrorPrint("IGM: Send message error, TASK [%s] to TASK[%s]!\n", zHcuSysCrlTab.taskRun.taskName[TASK_ID_IGM], zHcuSysCrlTab.taskRun.taskName[src_id]);
			return FAILURE;
		}
	}

	//收到初始化消息后，进入初始化状态
	if (FsmSetState(TASK_ID_IGM, FSM_STATE_IGM_INITED) == FAILURE){
		HcuErrorPrint("IGM: Error Set FSM State!\n");
		return FAILURE;
	}

	//初始化硬件接口
	if (func_igm_int_init() == FAILURE){
		HcuErrorPrint("IGM: Error initialize interface!\n");
		return FAILURE;
	}

	//Global Variables
	zHcuSysStaPm.taskRunErrCnt[TASK_ID_IGM] = 0;

	//启动周期性定时器
	ret = hcu_timer_start(TASK_ID_IGM, TIMER_ID_1S_IGM_PERIOD_READ, zHcuSysEngPar.timer.igmReqTimer, TIMER_TYPE_PERIOD, TIMER_RESOLUTION_1S);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_IGM]++;
		HcuErrorPrint("IGM: Error start period timer!\n");
		return FAILURE;
	}

	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_IGM, FSM_STATE_IGM_ACTIVED) == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_IGM]++;
		HcuErrorPrint("IGM: Error Set FSM State!\n");
		return FAILURE;
	}
	if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_FAT_ON) != FALSE){
		HcuDebugPrint("IGM: Enter FSM_STATE_IGM_ACTIVED status, Keeping refresh here!\n");
	}
	/*

	//进入阻塞式接收数据状态，然后继续发送
	while(1){
		//接收数据
		int dataLen=0;
		if (dataLen > 1){
			//发送数据给HSMMP
			msg_struct_igm_hsmmp_data_rx_t snd;
			memset(&snd, 0, sizeof(msg_struct_igm_hsmmp_data_rx_t));
			snd.length = sizeof(msg_struct_igm_hsmmp_data_rx_t);
			ret = hcu_message_send(MSG_ID_IGM_HSMMP_DATA_RX, TASK_ID_HSMMP, TASK_ID_IGM, &snd, snd.length);
			if (ret == FAILURE){
				zHcuRunErrCnt[TASK_ID_IGM]++;
				HcuErrorPrint("IGM: Send message error, TASK [%s] to TASK[%s]!\n", zHcuTaskNameList[TASK_ID_IGM], zHcuTaskNameList[TASK_ID_HSMMP]);
				return FAILURE;
			}
		}

		hcu_sleep(5);
	}
	*/

	return SUCCESS;
}

OPSTAT fsm_igm_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HcuErrorPrint("IGM: Internal error counter reach DEAD level, SW-RESTART soon!\n");
	zHcuSysStaPm.statisCnt.restartCnt++;
	fsm_igm_init(0, 0, NULL, 0);
	return SUCCESS;
}

OPSTAT func_igm_int_init(void)
{
	return SUCCESS;
}

//暂时啥都不干，定时到达后，还不明确是否需要支持定时工作
OPSTAT fsm_igm_time_out(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret;

	//Receive message and copy to local variable
	msg_struct_com_time_out_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_com_time_out_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_com_time_out_t))){
		HcuErrorPrint("IGM: Receive message error!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_IGM]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//钩子在此处，检查zHcuRunErrCnt[TASK_ID_IGM]是否超限
	if (zHcuSysStaPm.taskRunErrCnt[TASK_ID_IGM] > HCU_RUN_ERROR_LEVEL_2_MAJOR){
		//减少重复RESTART的概率
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_IGM] = zHcuSysStaPm.taskRunErrCnt[TASK_ID_IGM] - HCU_RUN_ERROR_LEVEL_2_MAJOR;
		msg_struct_com_restart_t snd0;
		memset(&snd0, 0, sizeof(msg_struct_com_restart_t));
		snd0.length = sizeof(msg_struct_com_restart_t);
		ret = hcu_message_send(MSG_ID_COM_RESTART, TASK_ID_IGM, TASK_ID_IGM, &snd0, snd0.length);
		if (ret == FAILURE){
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_IGM]++;
			HcuErrorPrint("IGM: Send message error, TASK [%s] to TASK[%s]!\n", zHcuSysCrlTab.taskRun.taskName[TASK_ID_IGM], zHcuSysCrlTab.taskRun.taskName[TASK_ID_IGM]);
			return FAILURE;
		}
	}

	//Period time out received
	if ((rcv.timeId == TIMER_ID_1S_IGM_PERIOD_READ) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		//保护周期读数的优先级，强制抢占状态，并简化问题
		if (FsmGetState(TASK_ID_IGM) != FSM_STATE_IGM_ACTIVED){
			ret = FsmSetState(TASK_ID_IGM, FSM_STATE_IGM_ACTIVED);
			if (ret == FAILURE){
				zHcuSysStaPm.taskRunErrCnt[TASK_ID_IGM]++;
				HcuErrorPrint("IGM: Error Set FSM State!\n");
				return FAILURE;
			}//FsmSetState
		}

		//Do nothing

	}

	return SUCCESS;
}

OPSTAT fsm_igm_nbiotcj188_data_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;
	msg_struct_nbiotcj188_igm_data_req_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_nbiotcj188_igm_data_req_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_nbiotcj188_igm_data_req_t))){
		HcuErrorPrint("IGM: Receive message error!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_IGM]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//验证入参的正确性
	if ((rcv.equtype < HCU_NBIOT_CJ188_T_TYPE_WATER_METER_MIN) || (rcv.equtype > HCU_NBIOT_CJ188_T_TYPE_WATER_METER_MAX)){
		HcuErrorPrint("IGM: Receive message error!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_IGM]++;
		return FAILURE;
	}

	//包括地址信息是否正确
	//仪表的状态是否合适等等


	//申明新的参量，目的是为了回复参数
	char tmp[17] = "";
	msg_struct_igm_nbiotcj188_data_resp_t igmResp;
	memset(&igmResp, 0, sizeof(msg_struct_igm_nbiotcj188_data_resp_t));
	igmResp.equtype = rcv.equtype;
	igmResp.igmHead.ctrlId = rcv.igmHead.ctrlId;
	strcpy(igmResp.igmHead.addr, rcv.igmHead.addr);
	igmResp.igmHead.d0d1Id = rcv.igmHead.d0d1Id;
	igmResp.igmHead.ser = rcv.igmHead.ser;
	igmResp.igmHead.periodFlag = rcv.igmHead.periodFlag;
	igmResp.igmHead.timestamp = time(0);

	//如果探测到通信异常，可以设置这个标志位
	igmResp.igmHead.communicationFlag = HCU_NBIOT_CJ188_COMM_CHANNEL_NORMAL;

	//根据命令，对仪表分别进行操控
	//准备读取控制的仪表的数据
	if ((rcv.igmHead.ctrlId == HCU_NBIOT_CJ188_CTRL_READ_DATA) && (rcv.igmHead.d0d1Id == HCU_NBIOT_CJ188_READ_DI0DI1_CURRENT_COUNTER_DATA)){
		//设置假数据，为了测试方便
		igmResp.igmData.currentaccuvolume = rand()%1000000;
		igmResp.igmData.currentaccuvolumeunit = HCU_NBIOT_CJ188_UNIT_M3;
		igmResp.billtodayaccuvolume = rand()%1000000;
		igmResp.billtodayaccuvolumeunit = HCU_NBIOT_CJ188_UNIT_M3;
		//如果需要设置当前的时间的话
		time_t lt;
		struct tm *cu;
		lt=time(NULL);
		cu = localtime(&lt);
		memset(tmp, 0, sizeof(tmp));
		sprintf(tmp,  "%04d%02d%02d%02d%02d%02d", (UINT16)(1900+cu->tm_year), (UINT8)cu->tm_mon, (UINT8)cu->tm_mday, (UINT8)cu->tm_hour, (UINT8)cu->tm_min, (UINT8)cu->tm_sec);
		strncpy(igmResp.igmData.realtime, tmp, 14);
		memset(tmp, 0, sizeof(tmp));
		sprintf(tmp, "%04d", rand()%10000);
		strncpy(igmResp.igmData.st, tmp, 4);
	}
	else if ((rcv.igmHead.ctrlId == HCU_NBIOT_CJ188_CTRL_READ_DATA) && (rcv.igmHead.d0d1Id == HCU_NBIOT_CJ188_READ_DI0DI1_HISTORY_COUNTER_DATA1)){
		//设置假数据，为了测试方便
		igmResp.billtodayaccuvolume = rand()%1000000;
		igmResp.billtodayaccuvolumeunit = HCU_NBIOT_CJ188_UNIT_M3;
		igmResp.igmData.lastmonth = 1;
	}
	else if ((rcv.igmHead.ctrlId == HCU_NBIOT_CJ188_CTRL_READ_DATA) && (rcv.igmHead.d0d1Id == HCU_NBIOT_CJ188_READ_DI0DI1_HISTORY_COUNTER_DATA2)){
		//设置假数据，为了测试方便
		igmResp.billtodayaccuvolume = rand()%1000000;
		igmResp.billtodayaccuvolumeunit = HCU_NBIOT_CJ188_UNIT_M3;
		igmResp.igmData.lastmonth = 2;
	}
	else if ((rcv.igmHead.ctrlId == HCU_NBIOT_CJ188_CTRL_READ_DATA) && (rcv.igmHead.d0d1Id == HCU_NBIOT_CJ188_READ_DI0DI1_HISTORY_COUNTER_DATA3)){
		//设置假数据，为了测试方便
		igmResp.billtodayaccuvolume = rand()%1000000;
		igmResp.billtodayaccuvolumeunit = HCU_NBIOT_CJ188_UNIT_M3;
		igmResp.igmData.lastmonth = 3;
	}
	else if ((rcv.igmHead.ctrlId == HCU_NBIOT_CJ188_CTRL_READ_DATA) && (rcv.igmHead.d0d1Id == HCU_NBIOT_CJ188_READ_DI0DI1_HISTORY_COUNTER_DATA4)){
		//设置假数据，为了测试方便
		igmResp.billtodayaccuvolume = rand()%1000000;
		igmResp.billtodayaccuvolumeunit = HCU_NBIOT_CJ188_UNIT_M3;
		igmResp.igmData.lastmonth = 4;
	}
	else if ((rcv.igmHead.ctrlId == HCU_NBIOT_CJ188_CTRL_READ_DATA) && (rcv.igmHead.d0d1Id == HCU_NBIOT_CJ188_READ_DI0DI1_HISTORY_COUNTER_DATA5)){
		//设置假数据，为了测试方便
		igmResp.billtodayaccuvolume = rand()%1000000;
		igmResp.billtodayaccuvolumeunit = HCU_NBIOT_CJ188_UNIT_M3;
		igmResp.igmData.lastmonth = 5;
	}
	else if ((rcv.igmHead.ctrlId == HCU_NBIOT_CJ188_CTRL_READ_DATA) && (rcv.igmHead.d0d1Id == HCU_NBIOT_CJ188_READ_DI0DI1_HISTORY_COUNTER_DATA6)){
		//设置假数据，为了测试方便
		igmResp.billtodayaccuvolume = rand()%1000000;
		igmResp.billtodayaccuvolumeunit = HCU_NBIOT_CJ188_UNIT_M3;
		igmResp.igmData.lastmonth = 6;
	}
	else if ((rcv.igmHead.ctrlId == HCU_NBIOT_CJ188_CTRL_READ_DATA) && (rcv.igmHead.d0d1Id == HCU_NBIOT_CJ188_READ_DI0DI1_HISTORY_COUNTER_DATA7)){
		//设置假数据，为了测试方便
		igmResp.billtodayaccuvolume = rand()%1000000;
		igmResp.billtodayaccuvolumeunit = HCU_NBIOT_CJ188_UNIT_M3;
		igmResp.igmData.lastmonth = 7;
	}
	else if ((rcv.igmHead.ctrlId == HCU_NBIOT_CJ188_CTRL_READ_DATA) && (rcv.igmHead.d0d1Id == HCU_NBIOT_CJ188_READ_DI0DI1_HISTORY_COUNTER_DATA8)){
		//设置假数据，为了测试方便
		igmResp.billtodayaccuvolume = rand()%1000000;
		igmResp.billtodayaccuvolumeunit = HCU_NBIOT_CJ188_UNIT_M3;
		igmResp.igmData.lastmonth = 8;
	}
	else if ((rcv.igmHead.ctrlId == HCU_NBIOT_CJ188_CTRL_READ_DATA) && (rcv.igmHead.d0d1Id == HCU_NBIOT_CJ188_READ_DI0DI1_HISTORY_COUNTER_DATA9)){
		//设置假数据，为了测试方便
		igmResp.billtodayaccuvolume = rand()%1000000;
		igmResp.billtodayaccuvolumeunit = HCU_NBIOT_CJ188_UNIT_M3;
		igmResp.igmData.lastmonth = 9;
	}
	else if ((rcv.igmHead.ctrlId == HCU_NBIOT_CJ188_CTRL_READ_DATA) && (rcv.igmHead.d0d1Id == HCU_NBIOT_CJ188_READ_DI0DI1_HISTORY_COUNTER_DATA10)){
		//设置假数据，为了测试方便
		igmResp.billtodayaccuvolume = rand()%1000000;
		igmResp.billtodayaccuvolumeunit = HCU_NBIOT_CJ188_UNIT_M3;
		igmResp.igmData.lastmonth = 10;
	}
	else if ((rcv.igmHead.ctrlId == HCU_NBIOT_CJ188_CTRL_READ_DATA) && (rcv.igmHead.d0d1Id == HCU_NBIOT_CJ188_READ_DI0DI1_HISTORY_COUNTER_DATA11)){
		//设置假数据，为了测试方便
		igmResp.billtodayaccuvolume = rand()%1000000;
		igmResp.billtodayaccuvolumeunit = HCU_NBIOT_CJ188_UNIT_M3;
		igmResp.igmData.lastmonth = 11;
	}
	else if ((rcv.igmHead.ctrlId == HCU_NBIOT_CJ188_CTRL_READ_DATA) && (rcv.igmHead.d0d1Id == HCU_NBIOT_CJ188_READ_DI0DI1_HISTORY_COUNTER_DATA12)){
		//设置假数据，为了测试方便
		igmResp.billtodayaccuvolume = rand()%1000000;
		igmResp.billtodayaccuvolumeunit = HCU_NBIOT_CJ188_UNIT_M3;
		igmResp.igmData.lastmonth = 12;
	}
	else if ((rcv.igmHead.ctrlId == HCU_NBIOT_CJ188_CTRL_READ_DATA) && (rcv.igmHead.d0d1Id == HCU_NBIOT_CJ188_READ_DI0DI1_PRICE_TABLE)){
		//设置假数据，为了测试方便
		igmResp.igmData.price1 = rand()%1000000;
		igmResp.igmData.volume1 = rand()%1000000;
		igmResp.igmData.price2 = rand()%1000000;
		igmResp.igmData.volume2 = rand()%1000000;
		igmResp.igmData.price3 = rand()%1000000;
	}
	else if ((rcv.igmHead.ctrlId == HCU_NBIOT_CJ188_CTRL_READ_DATA) && (rcv.igmHead.d0d1Id == HCU_NBIOT_CJ188_READ_DI0DI1_BILL_TODAY_DATE)){ //结算日
		//设置假数据，为了测试方便
		igmResp.igmData.billtodaydate = rand()%256;
	}
	else if ((rcv.igmHead.ctrlId == HCU_NBIOT_CJ188_CTRL_READ_DATA) && (rcv.igmHead.d0d1Id == HCU_NBIOT_CJ188_READ_DI0DI1_READ_ACCOUNT_CUR_DATE)){ //抄表日
		//设置假数据，为了测试方便
		igmResp.igmData.readamountcurdate = rand()%256;
	}
	else if ((rcv.igmHead.ctrlId == HCU_NBIOT_CJ188_CTRL_READ_DATA) && (rcv.igmHead.d0d1Id == HCU_NBIOT_CJ188_READ_DI0DI1_BUY_AMOUNT)){ //购入金额
		//设置假数据，为了测试方便
		igmResp.igmData.thisamount = rand()%1000000;
		igmResp.igmData.accuamount = rand()%1000000;
		igmResp.igmData.remainamount = rand()%1000000;
		memset(tmp, 0, sizeof(tmp));
		sprintf(tmp, "%04d", rand()%10000);
		strncpy(igmResp.igmData.st, tmp, 4);
	}
	else if ((rcv.igmHead.ctrlId == HCU_NBIOT_CJ188_CTRL_READ_KEY_VER) && (rcv.igmHead.d0d1Id == HCU_NBIOT_CJ188_READ_DI0DI1_KEY_VER)){
		//设置假数据，为了测试方便
		igmResp.igmData.keyver = rand()%256;
	}
	else if ((rcv.igmHead.ctrlId == HCU_NBIOT_CJ188_CTRL_READ_ADDR) && (rcv.igmHead.d0d1Id == HCU_NBIOT_CJ188_READ_DI0DI1_ADDRESS)){
		//设置假数据，为了测试方便
		//Do nothing
	}
	else if ((rcv.igmHead.ctrlId == HCU_NBIOT_CJ188_CTRL_WRITE_DATA) && (rcv.igmHead.d0d1Id == HCU_NBIOT_CJ188_WRITE_DI0DI1_PRICE_TABLE)){
		//设置假数据，为了测试方便
		memset(tmp, 0, sizeof(tmp));
		sprintf(tmp, "%04d", rand()%10000);
		strncpy(igmResp.igmData.st, tmp, 4);
	}
	else if ((rcv.igmHead.ctrlId == HCU_NBIOT_CJ188_CTRL_WRITE_DATA) && (rcv.igmHead.d0d1Id == HCU_NBIOT_CJ188_WRITE_DI0DI1_BILL_TODAY_DATE)){
		//设置假数据，为了测试方便
		//Do nothing
	}
	else if ((rcv.igmHead.ctrlId == HCU_NBIOT_CJ188_CTRL_WRITE_DATA) && (rcv.igmHead.d0d1Id == HCU_NBIOT_CJ188_WRITE_DI0DI1_READ_ACCOUNT_CUR_DATE)){
		//设置假数据，为了测试方便
		//Do nothing
	}
	else if ((rcv.igmHead.ctrlId == HCU_NBIOT_CJ188_CTRL_WRITE_DATA) && (rcv.igmHead.d0d1Id == HCU_NBIOT_CJ188_WRITE_DI0DI1_BUY_AMOUNT)){
		//设置假数据，为了测试方便
		igmResp.igmData.buycode = rand()%256;
		igmResp.igmData.thisamount = rand()%1000000;
	}
	else if ((rcv.igmHead.ctrlId == HCU_NBIOT_CJ188_CTRL_WRITE_DATA) && (rcv.igmHead.d0d1Id == HCU_NBIOT_CJ188_WRITE_DI0DI1_NEW_KEY)){
		//设置假数据，为了测试方便
		igmResp.igmData.keyver = rand()%256;
	}
	else if ((rcv.igmHead.ctrlId == HCU_NBIOT_CJ188_CTRL_WRITE_DATA) && (rcv.igmHead.d0d1Id == HCU_NBIOT_CJ188_WRITE_DI0DI1_STD_TIME)){
		//设置假数据，为了测试方便
		//Do nothing
	}
	else if ((rcv.igmHead.ctrlId == HCU_NBIOT_CJ188_CTRL_WRITE_DATA) && (rcv.igmHead.d0d1Id == HCU_NBIOT_CJ188_WRITE_DI0DI1_SWITCH_CTRL)){
		//设置假数据，为了测试方便
		memset(tmp, 0, sizeof(tmp));
		sprintf(tmp, "%04d", rand()%10000);
		strncpy(igmResp.igmData.st, tmp, 4);
	}
	else if ((rcv.igmHead.ctrlId == HCU_NBIOT_CJ188_CTRL_WRITE_DATA) && (rcv.igmHead.d0d1Id == HCU_NBIOT_CJ188_WRITE_DI0DI1_OFF_FACTORY_START)){
		//设置假数据，为了测试方便
		//Do nothing
	}
	else if ((rcv.igmHead.ctrlId == HCU_NBIOT_CJ188_CTRL_WRITE_ADDR) && (rcv.igmHead.d0d1Id == HCU_NBIOT_CJ188_WRITE_DI0DI1_ADDRESS)){
		//设置假数据，为了测试方便
		//Do nothing
	}
	else if ((rcv.igmHead.ctrlId == HCU_NBIOT_CJ188_CTRL_WRITE_DEVICE_SYN) && (rcv.igmHead.d0d1Id == HCU_NBIOT_CJ188_WRITE_DI0DI1_DEVICE_SYN_DATA)){
		//设置假数据，为了测试方便
		memset(tmp, 0, sizeof(tmp));
		sprintf(tmp, "%04d", rand()%10000);
		strncpy(igmResp.igmData.st, tmp, 4);
	}
	else{
		HcuErrorPrint("IGM: Receive message error!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_IGM]++;
		return FAILURE;
	}

	//准备存入本地数据库, RECORD还要存入数据库
	if (HCU_SENSOR_DATA_SAVE_TO_LOCAL_DB_SET == HCU_SYSCFG_SENSOR_SAVE_TO_LOCAL_DB_FLAG_YES)
	{
		sensor_igm_cj188_data_element_t igmDbSave;
		memset(&igmDbSave, 0, sizeof(sensor_igm_cj188_data_element_t));
		memcpy(&igmDbSave.igm, &igmResp.igmData, sizeof(sensor_general_cj188_data_element_t));
		strcpy(igmDbSave.cj188address, igmResp.igmHead.addr);
		igmDbSave.equtype = igmResp.equtype;
		igmDbSave.timestamp = igmResp.igmHead.timestamp;
		igmDbSave.billtodayaccuvolume = igmResp.billtodayaccuvolume;
		igmDbSave.billtodayaccuvolumeunit = igmResp.billtodayaccuvolumeunit;
		ret = dbi_HcuIgmCj188DataInfo_save(&igmDbSave);
		if (ret == FAILURE){
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_IGM]++;
			HcuErrorPrint("IGM: Can not save data into database!\n");
		}
	}

	//准备发送数据回去给NBIOTCJ188，然后通过UL上行链路送回CLOUD平台
	igmResp.length = sizeof(msg_struct_igm_nbiotcj188_data_resp_t);
	ret = hcu_message_send(MSG_ID_IGM_NBIOTCJ188_DATA_RESP, TASK_ID_NBIOTCJ188, TASK_ID_IGM, &igmResp, igmResp.length);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_IGM]++;
		HcuErrorPrint("IGM: Send message error, TASK [%s] to TASK[%s]!\n", zHcuSysCrlTab.taskRun.taskName[TASK_ID_IGM], zHcuSysCrlTab.taskRun.taskName[TASK_ID_NBIOTCJ188]);
		return FAILURE;
	}

	//返回
	return SUCCESS;
}

//暂时不用，留待未来使用
OPSTAT fsm_igm_nbiotcj188_ctrl_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	return SUCCESS;
}

