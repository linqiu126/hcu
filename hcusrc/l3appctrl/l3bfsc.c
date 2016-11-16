/*
 * l3bfsc.c
 *
 *  Created on: 2015年11月22日
 *      Author: test
 */


#include "l3bfsc.h"

#include "../l0service/timer.h"
#include "../l0service/trace.h"
#include "../l1com/hwinv.h"
#include "../l1com/l1comdef.h"
#include "../l2frame/cloudvela.h"

/*
** FSM of the L3BFSC
*/
FsmStateItem_t FsmL3bfsc[] =
{
    //MessageId                 //State                   		 		//Function
	//启始点，固定定义，不要改动, 使用ENTRY/END，意味者MSGID肯定不可能在某个高位区段中；考虑到所有任务共享MsgId，即使分段，也无法实现
	//完全是为了给任务一个初始化的机会，按照状态转移机制，该函数不具备启动的机会，因为任务初始化后自动到FSM_STATE_IDLE
	//如果没有必要进行初始化，可以设置为NULL
	{MSG_ID_ENTRY,       						FSM_STATE_ENTRY,            				fsm_l3bfsc_task_entry}, //Starting

	//System level initialization, only controlled by HCU-MAIN
    {MSG_ID_COM_INIT,       					FSM_STATE_IDLE,            					fsm_l3bfsc_init},
    {MSG_ID_COM_RESTART,						FSM_STATE_IDLE,            					fsm_l3bfsc_restart},
    {MSG_ID_COM_INIT_FEEDBACK,					FSM_STATE_IDLE,            					fsm_com_do_nothing},

	//Task level initialization
    {MSG_ID_COM_INIT,       					FSM_STATE_L3BFSC_INITED,            		fsm_l3bfsc_init},
    {MSG_ID_COM_RESTART,						FSM_STATE_L3BFSC_INITED,            		fsm_l3bfsc_restart},
    {MSG_ID_COM_INIT_FEEDBACK,					FSM_STATE_L3BFSC_INITED,            		fsm_com_do_nothing},

	//Normal working status：等待人工干预-登录触发
    {MSG_ID_COM_RESTART,        				FSM_STATE_L3BFSC_ACTIVED,            		fsm_l3bfsc_restart},
    {MSG_ID_COM_INIT_FEEDBACK,					FSM_STATE_L3BFSC_ACTIVED,            		fsm_com_do_nothing},
	{MSG_ID_COM_TIME_OUT,       				FSM_STATE_L3BFSC_ACTIVED,          			fsm_l3bfsc_time_out},
	{MSG_ID_UICOMM_L3BFSC_CMD_REQ,       		FSM_STATE_L3BFSC_ACTIVED,          			fsm_l3bfsc_uicomm_cmd_req},

	//人工配置状态：等待参数配置完成。完成后，发送MSG_ID_L3BFSC_CAN_WS_INIT_REQ，进入FSM_STATE_L3BFSC_WS_INIT
	//任何状态下，允许人工界面强行发送重启命令（后台或者本地界面），BFSCUICOMM模块将发送MSG_ID_COM_RESTART，从而重启整个系统
	//重启需要重新登录并初始化整个秤盘传感器
    {MSG_ID_COM_RESTART,        				FSM_STATE_L3BFSC_OPR_CFG,            		fsm_l3bfsc_restart},
	{MSG_ID_COM_TIME_OUT,       				FSM_STATE_L3BFSC_OPR_CFG,          			fsm_l3bfsc_time_out},
	{MSG_ID_UICOMM_L3BFSC_CMD_REQ,       		FSM_STATE_L3BFSC_OPR_CFG,          			fsm_l3bfsc_uicomm_cmd_req},
	{MSG_ID_CLOUDVELA_L3BFSC_CMD_REQ,       	FSM_STATE_L3BFSC_OPR_CFG,          			fsm_l3bfsc_cloudvela_cmd_req},

	//等待下位机完成参数初始化：等待MSG_ID_CAN_L3FSC_WS_INIT_RESP，完成后进入FSM_STATE_L3BFSC_OOS_SCAN
    {MSG_ID_COM_RESTART,        				FSM_STATE_L3BFSC_WS_INIT,            		fsm_l3bfsc_restart},
	{MSG_ID_COM_TIME_OUT,       				FSM_STATE_L3BFSC_WS_INIT,          			fsm_l3bfsc_time_out},
	{MSG_ID_CAN_L3FSC_WS_INIT_RESP,       		FSM_STATE_L3BFSC_WS_INIT,          			fsm_l3bfsc_can_ws_init_resp},

	//进料组合态：等待正常的MSG_ID_CAN_L3BFSC_WS_NEW_READY_EVENT，每一次进来均触发一次组合算法。结果无动作，或发送MSG_ID_L3BFSC_CAN_WS_COMB_OUT/MSG_ID_L3BFSC_CAN_WS_GIVE_UP
    {MSG_ID_COM_RESTART,        				FSM_STATE_L3BFSC_OOS_SCAN,            		fsm_l3bfsc_restart},
	{MSG_ID_COM_TIME_OUT,       				FSM_STATE_L3BFSC_OOS_SCAN,          		fsm_l3bfsc_time_out},
	{MSG_ID_CAN_L3BFSC_WS_NEW_READY_EVENT,      FSM_STATE_L3BFSC_OOS_SCAN,          		fsm_l3bfsc_canitf_ws_new_ready_event},  //只能触发数据存储，不进入组合算法的执行

	//出料流程态：单纯等待MSG_ID_CAN_L3BFSC_WS_COMB_OUT_FB，一旦收到无误后进入FSM_STATE_L3BFSC_OOS_SCAN。差错进入FSM_STATE_L3BFSC_ERROR_INQ。
    {MSG_ID_COM_RESTART,        				FSM_STATE_L3BFSC_OOS_TTT,            		fsm_l3bfsc_restart},
	{MSG_ID_COM_TIME_OUT,       				FSM_STATE_L3BFSC_OOS_TTT,          			fsm_l3bfsc_time_out},
	{MSG_ID_CAN_L3BFSC_WS_COMB_OUT_FB,       	FSM_STATE_L3BFSC_OOS_TTT,          			fsm_l3bfsc_can_ws_comb_out_fb},

	//放弃物料态：单纯等待MSG_ID_CAN_L3BFSC_WS_GIVE_UP_FB，一旦收到无误后进入FSM_STATE_L3BFSC_OOS_SCAN。差错进入FSM_STATE_L3BFSC_ERROR_INQ。
    {MSG_ID_COM_RESTART,        				FSM_STATE_L3BFSC_OOS_TGU,            		fsm_l3bfsc_restart},
	{MSG_ID_COM_TIME_OUT,       				FSM_STATE_L3BFSC_OOS_TGU,          			fsm_l3bfsc_time_out},
	{MSG_ID_CAN_L3BFSC_WS_GIVE_UP_FB,       	FSM_STATE_L3BFSC_OOS_TGU,          			fsm_l3bfsc_can_ws_give_up_fb},

	//数据差错，重新采样所有数据：收到MSG_ID_CAN_L3BFSC_WS_NEW_READY_EVENT差错，发送MSG_ID_L3BFSC_CAN_CMD_REQ，等待传感器传回所有差错的重测值。结束以后进入FSM_STATE_L3BFSC_OOS_SCAN。
    {MSG_ID_COM_RESTART,        				FSM_STATE_L3BFSC_ERROR_INQ,            		fsm_l3bfsc_restart},
	{MSG_ID_COM_TIME_OUT,       				FSM_STATE_L3BFSC_ERROR_INQ,          		fsm_l3bfsc_time_out},
	{MSG_ID_CAN_L3BFSC_WS_REPORT,       		FSM_STATE_L3BFSC_ERROR_INQ,          		fsm_l3bfsc_canitf_ws_report},  //只能触发数据存储，不进入组合算法的执行

    //结束点，固定定义，不要改动
    {MSG_ID_END,            	FSM_STATE_END,             				NULL},  //Ending
};

//Global variables
L3BfscGenCtrlTable_t zHcuL3BfscGenCtrlTable; //波峰秤盘的总控表
UINT8 *zHcuSearchCoefficientPointer;
UINT32 zHcuSearchSpaceTotalNbr = 0; //搜索的长度，12对应4096

//Main Entry
//Input parameter would be useless, but just for similar structure purpose
OPSTAT fsm_l3bfsc_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//除了对全局变量进行操作之外，尽量不要做其它操作，因为该函数将被主任务/线程调用，不是本任务/线程调用
	//该API就是给本任务一个提早介入的入口，可以帮着做些测试性操作
	if (FsmSetState(TASK_ID_L3BFSC, FSM_STATE_IDLE) == FAILURE){
		HcuErrorPrint("L3BFSC: Error Set FSM State at fsm_l3bfsc_task_entry\n");}
	return SUCCESS;
}

OPSTAT fsm_l3bfsc_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;

	if ((src_id > TASK_ID_MIN) &&(src_id < TASK_ID_MAX)){
		//Send back MSG_ID_COM_INIT_FEEDBACK to SVRCON
		msg_struct_com_init_feedback_t snd0;
		memset(&snd0, 0, sizeof(msg_struct_com_init_feedback_t));
		snd0.length = sizeof(msg_struct_com_init_feedback_t);

		//to avoid all task send out the init fb msg at the same time which lead to msgque get stuck
		hcu_usleep(dest_id*HCU_DURATION_OF_INIT_FB_WAIT_MAX);

		ret = hcu_message_send(MSG_ID_COM_INIT_FEEDBACK, src_id, TASK_ID_L3BFSC, &snd0, snd0.length);
		if (ret == FAILURE){
			HcuErrorPrint("L3BFSC: Send message error, TASK [%s] to TASK[%s]!\n", zHcuTaskNameList[TASK_ID_L3BFSC], zHcuTaskNameList[src_id]);
			return FAILURE;
		}
	}

	//收到初始化消息后，进入初始化状态
	if (FsmSetState(TASK_ID_L3BFSC, FSM_STATE_L3BFSC_INITED) == FAILURE){
		HcuErrorPrint("L3BFSC: Error Set FSM State!\n");
		return FAILURE;
	}

	//初始化硬件接口
	if (func_l3bfsc_int_init() == FAILURE){
		HcuErrorPrint("L3BFSC: Error initialize interface!\n");
		return FAILURE;
	}

	//Global Variables
	zHcuRunErrCnt[TASK_ID_L3BFSC] = 0;

	//秤盘数据表单控制表初始化
	memset(&zHcuL3BfscGenCtrlTable, 0, sizeof(L3BfscGenCtrlTable_t));
	int i=0;
	for (i=0; i<HCU_BFSC_SENSOR_WS_NBR_MAX; i++){
		zHcuL3BfscGenCtrlTable.sensorWs[i].sensorWsId = i;
		zHcuL3BfscGenCtrlTable.sensorWs[i].sensorValue = 0;
		zHcuL3BfscGenCtrlTable.sensorWs[i].sensorStatus = HCU_L3BFSC_SENSOR_WS_STATUS_EMPTY;
	}
	zHcuL3BfscGenCtrlTable.minWsNbr = 1;
	zHcuL3BfscGenCtrlTable.minWsNbr = HCU_BFSC_SENSOR_WS_NBR_MAX;
	zHcuL3BfscGenCtrlTable.targetValue = 0;
	zHcuL3BfscGenCtrlTable.targetUpLimit = 0;
	zHcuL3BfscGenCtrlTable.currentStatus = HCU_L3BFSC_WHOLE_STATUS_TO_COMB;
	zHcuL3BfscGenCtrlTable.wsRrSearchStart = 0;
	zHcuL3BfscGenCtrlTable.wsValueNbrFree = 0;
	zHcuL3BfscGenCtrlTable.wsValueNbrTtt = 0;
	zHcuL3BfscGenCtrlTable.wsValueNbrTgu = 0;

	//为搜索空间申请内存
	zHcuSearchSpaceTotalNbr = pow(2, HCU_BFSC_SENSOR_WS_NBR_MAX);
	zHcuSearchCoefficientPointer = malloc(zHcuSearchSpaceTotalNbr * HCU_BFSC_SENSOR_WS_NBR_MAX * sizeof(UINT8));
	if (zHcuSearchCoefficientPointer == NULL){
		zHcuRunErrCnt[TASK_ID_L3BFSC]++;
		HcuErrorPrint("L3BFSC: Init parameter error!\n");
		return FAILURE;
	}
	memset(zHcuSearchCoefficientPointer, 0, zHcuSearchSpaceTotalNbr * HCU_BFSC_SENSOR_WS_NBR_MAX  * sizeof(UINT8));

	//初始化搜索系数表
	//UINT8 *p = NULL;
	UINT8 j=0;
	UINT32 k=0, t=0;
	UINT32 kUp = 0;
	//UINT32 tLow=0;
	UINT32 tUp = 0, Index=0;
	//UINT32 total = 0;
	for (j=0; j<HCU_BFSC_SENSOR_WS_NBR_MAX; j++){
		kUp = pow(2, HCU_BFSC_SENSOR_WS_NBR_MAX-j-1);
		for (k=0; k< kUp; k++){
			//tLow = k*pow(2, j+1);
			tUp = pow(2, j);
			for (t=0; t<tUp; t++){
				//Index = j*zHcuSearchSpaceTotalNbr + k*pow(2, j+1) + t + tLow;
				Index = j + (k * pow(2, j+1) + t) * HCU_BFSC_SENSOR_WS_NBR_MAX;
				*(zHcuSearchCoefficientPointer + Index) = 1;
				//total++;
				//HcuDebugPrint("L3BFSC: Total set [%d], index = [%d]\n", total, Index);
			}
		}
	}
	//p = zHcuSearchCoefficientPointer;
/*	for (i=0; i<zHcuSearchSpaceTotalNbr; i++){
		//HcuDebugPrint("L3BFSC: Matrix Table[%d]=[%d%d%d]\n", i, *(p+3*i),*(p+3*i+1), *(p+3*i+2));
		HcuDebugPrint("L3BFSC: Matrix Table[%d]=[%d%d%d%d%d%d%d%d%d%d%d%d]\n", i, *(p+12*i),*(p+12*i+1),*(p+12*i+2),*(p+12*i+3),*(p+12*i+4),*(p+12*i+5),*(p+12*i+6),*(p+12*i+7),*(p+12*i+8),*(p+12*i+9),*(p+12*i+10),*(p+12*i+11));
	}*/
	//启动周期性定时器
	ret = hcu_timer_start(TASK_ID_L3BFSC, TIMER_ID_1S_L3BFSC_PERIOD_READ, HCU_L3BFSC_TIMER_DURATION_PERIOD_READ, TIMER_TYPE_PERIOD, TIMER_RESOLUTION_1S);
	if (ret == FAILURE){
		zHcuRunErrCnt[TASK_ID_L3BFSC]++;
		HcuErrorPrint("L3BFSC: Error start period timer!\n");
		return FAILURE;
	}

	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_L3BFSC, FSM_STATE_L3BFSC_ACTIVED) == FAILURE){
		zHcuRunErrCnt[TASK_ID_L3BFSC]++;
		HcuErrorPrint("L3BFSC: Error Set FSM State!\n");
		return FAILURE;
	}
	if ((zHcuSysEngPar.debugMode & HCU_TRACE_DEBUG_FAT_ON) != FALSE){
		HcuDebugPrint("L3BFSC: Enter FSM_STATE_L3BFSC_ACTIVED status, Keeping refresh here!\n");
	}

	//返回
	return SUCCESS;
}

OPSTAT fsm_l3bfsc_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HcuErrorPrint("L3BFSC: Internal error counter reach DEAD level, SW-RESTART soon!\n");
	zHcuGlobalCounter.restartCnt++;
	fsm_l3bfsc_init(0, 0, NULL, 0);
	return SUCCESS;
}

OPSTAT func_l3bfsc_int_init(void)
{
	return SUCCESS;
}

//暂时啥都不干，定时到达后，还不明确是否需要支持定时工作
OPSTAT fsm_l3bfsc_time_out(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret;

	//Receive message and copy to local variable
	msg_struct_com_time_out_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_com_time_out_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_com_time_out_t))){
		HcuErrorPrint("L3BFSC: Receive message error!\n");
		zHcuRunErrCnt[TASK_ID_L3BFSC]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//钩子在此处，检查zHcuRunErrCnt[TASK_ID_L3BFSC]是否超限
	if (zHcuRunErrCnt[TASK_ID_L3BFSC] > HCU_RUN_ERROR_LEVEL_2_MAJOR){
		//减少重复RESTART的概率
		zHcuRunErrCnt[TASK_ID_L3BFSC] = zHcuRunErrCnt[TASK_ID_L3BFSC] - HCU_RUN_ERROR_LEVEL_2_MAJOR;
		msg_struct_com_restart_t snd0;
		memset(&snd0, 0, sizeof(msg_struct_com_restart_t));
		snd0.length = sizeof(msg_struct_com_restart_t);
		ret = hcu_message_send(MSG_ID_COM_RESTART, TASK_ID_L3BFSC, TASK_ID_L3BFSC, &snd0, snd0.length);
		if (ret == FAILURE){
			zHcuRunErrCnt[TASK_ID_L3BFSC]++;
			HcuErrorPrint("L3BFSC: Send message error, TASK [%s] to TASK[%s]!\n", zHcuTaskNameList[TASK_ID_L3BFSC], zHcuTaskNameList[TASK_ID_L3BFSC]);
			return FAILURE;
		}
	}

	//Period time out received
	if ((rcv.timeId == TIMER_ID_1S_L3BFSC_PERIOD_READ) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		//保护周期读数的优先级，强制抢占状态，并简化问题
		if (FsmGetState(TASK_ID_L3BFSC) != FSM_STATE_L3BFSC_ACTIVED){
			ret = FsmSetState(TASK_ID_L3BFSC, FSM_STATE_L3BFSC_ACTIVED);
			if (ret == FAILURE){
				zHcuRunErrCnt[TASK_ID_L3BFSC]++;
				HcuErrorPrint("L3BFSC: Error Set FSM State!\n");
				return FAILURE;
			}//FsmSetState
		}

		//Do nothing

	}

	return SUCCESS;
}

OPSTAT fsm_l3bfsc_canitf_cmd_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret=0;
/*	HcuDiscDataSampleStorageArray_t record;

	msg_struct_modbus_pm25_data_report_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_modbus_pm25_data_report_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_modbus_pm25_data_report_t))){
		HcuErrorPrint("L3BFSC: Receive message error!\n");
		zHcuRunErrCnt[TASK_ID_L3BFSC]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);*/

	//检查收到的数据的正确性，然后再继续往CLOUD发送，仍然以平淡消息的格式，让L2_CLOUDVELA进行编码

	//停止定时器


	//返回
	return SUCCESS;
}

OPSTAT fsm_l3bfsc_canitf_ws_report(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret=0;
/*	HcuDiscDataSampleStorageArray_t record;

	msg_struct_modbus_pm25_data_report_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_modbus_pm25_data_report_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_modbus_pm25_data_report_t))){
		HcuErrorPrint("L3BFSC: Receive message error!\n");
		zHcuRunErrCnt[TASK_ID_L3BFSC]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);*/

	//检查收到的数据的正确性，然后再继续往CLOUD发送，仍然以平淡消息的格式，让L2_CLOUDVELA进行编码

	//停止定时器


	//返回
	return SUCCESS;
}

OPSTAT fsm_l3bfsc_can_ws_comb_out_fb(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret=0;
/*	HcuDiscDataSampleStorageArray_t record;

	msg_struct_modbus_pm25_data_report_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_modbus_pm25_data_report_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_modbus_pm25_data_report_t))){
		HcuErrorPrint("L3BFSC: Receive message error!\n");
		zHcuRunErrCnt[TASK_ID_L3BFSC]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);*/

	//检查收到的数据的正确性，然后再继续往CLOUD发送，仍然以平淡消息的格式，让L2_CLOUDVELA进行编码

	//停止定时器


	//返回
	return SUCCESS;
}

OPSTAT fsm_l3bfsc_can_ws_give_up_fb(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret=0;
/*	HcuDiscDataSampleStorageArray_t record;

	msg_struct_modbus_pm25_data_report_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_modbus_pm25_data_report_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_modbus_pm25_data_report_t))){
		HcuErrorPrint("L3BFSC: Receive message error!\n");
		zHcuRunErrCnt[TASK_ID_L3BFSC]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);*/

	//检查收到的数据的正确性，然后再继续往CLOUD发送，仍然以平淡消息的格式，让L2_CLOUDVELA进行编码

	//停止定时器


	//返回
	return SUCCESS;
}

//从BFSCUICOMM来的消息和命令
OPSTAT fsm_l3bfsc_uicomm_cmd_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret=0;
/*	HcuDiscDataSampleStorageArray_t record;

	msg_struct_modbus_pm25_data_report_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_modbus_pm25_data_report_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_modbus_pm25_data_report_t))){
		HcuErrorPrint("L3BFSC: Receive message error!\n");
		zHcuRunErrCnt[TASK_ID_L3BFSC]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);*/

	//检查收到的数据的正确性，然后再继续往CLOUD发送，仍然以平淡消息的格式，让L2_CLOUDVELA进行编码
	//检查参数配置的合法性，目标值必须在一定的合理范围内，否则拒绝进入进料状态

	//停止定时器


	//返回
	return SUCCESS;
}


OPSTAT fsm_l3bfsc_cloudvela_cmd_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret=0;
/*	HcuDiscDataSampleStorageArray_t record;

	msg_struct_modbus_pm25_data_report_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_modbus_pm25_data_report_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_modbus_pm25_data_report_t))){
		HcuErrorPrint("L3BFSC: Receive message error!\n");
		zHcuRunErrCnt[TASK_ID_L3BFSC]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);*/

	//检查收到的数据的正确性，然后再继续往CLOUD发送，仍然以平淡消息的格式，让L2_CLOUDVELA进行编码

	//停止定时器


	//返回
	return SUCCESS;
}

//触发组合算法
OPSTAT fsm_l3bfsc_canitf_ws_new_ready_event(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret=0;

	//入参检查
	msg_struct_can_l3bfsc_new_ready_event_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_can_l3bfsc_new_ready_event_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_can_l3bfsc_new_ready_event_t))){
		HcuErrorPrint("L3BFSC: Receive message error!\n");
		zHcuRunErrCnt[TASK_ID_L3BFSC]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//检查收到的数据的正确性，然后再继续往CLOUD发送，仍然以平淡消息的格式，让L2_CLOUDVELA进行编码

	//停止定时器


	//返回
	return SUCCESS;
}

OPSTAT fsm_l3bfsc_can_ws_init_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret=0;
/*	HcuDiscDataSampleStorageArray_t record;

	msg_struct_modbus_pm25_data_report_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_modbus_pm25_data_report_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_modbus_pm25_data_report_t))){
		HcuErrorPrint("L3BFSC: Receive message error!\n");
		zHcuRunErrCnt[TASK_ID_L3BFSC]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);*/

	//检查收到的数据的正确性，然后再继续往CLOUD发送，仍然以平淡消息的格式，让L2_CLOUDVELA进行编码

	//停止定时器


	//返回
	return SUCCESS;
}

//系统参数的合法性检查，均在参数初始化中完成，后面不再检查
INT32 func_l3bfsc_ws_sensor_search_combination(void)
{
	UINT32 WsSensorStart = 0;
	UINT32 i=0, j=0;
	UINT32 result=0;

	//选取起始位置
	WsSensorStart = zHcuL3BfscGenCtrlTable.wsRrSearchStart % zHcuSearchSpaceTotalNbr;

	//组合搜索
	for (i=WsSensorStart; i< (zHcuSearchSpaceTotalNbr + WsSensorStart); i++){
		result = 0;
		for (j=0; j<HCU_BFSC_SENSOR_WS_NBR_MAX; j++){
			if (zHcuL3BfscGenCtrlTable.sensorWs[j].sensorStatus == HCU_L3BFSC_SENSOR_WS_STATUS_VALID_TO_COMB){
				result += zHcuL3BfscGenCtrlTable.sensorWs[j].sensorValue * (*(zHcuSearchCoefficientPointer + (i%zHcuSearchSpaceTotalNbr) * HCU_BFSC_SENSOR_WS_NBR_MAX + j));
			}
		}
		//如果落入目标范围
		if ((result >= zHcuL3BfscGenCtrlTable.targetValue) && (result <= zHcuL3BfscGenCtrlTable.targetUpLimit)){
			zHcuL3BfscGenCtrlTable.wsRrSearchStart = ((i+1) % zHcuSearchSpaceTotalNbr);
			return (i % zHcuSearchSpaceTotalNbr);
		}
	}
	zHcuL3BfscGenCtrlTable.wsRrSearchStart = ((i+1) % zHcuSearchSpaceTotalNbr);
	return -1;
}

void func_l3bfsc_cacluate_sensor_ws_valid_value(void)
{
	int i=0, resFree = 0, resTtt =0, resTgu = 0;
	for (i=0; i<HCU_BFSC_SENSOR_WS_NBR_MAX; i++)
	{
		if (zHcuL3BfscGenCtrlTable.sensorWs[i].sensorStatus == HCU_L3BFSC_SENSOR_WS_STATUS_VALID_TO_COMB)
			resFree++;
		else if (zHcuL3BfscGenCtrlTable.sensorWs[i].sensorStatus == HCU_L3BFSC_SENSOR_WS_STATUS_VALID_TO_TTT)
			resTtt++;
		else if (zHcuL3BfscGenCtrlTable.sensorWs[i].sensorStatus == HCU_L3BFSC_SENSOR_WS_STATUS_VALID_TO_TGU)
			resTgu++;
	}

	//存入全局控制表
	zHcuL3BfscGenCtrlTable.wsValueNbrFree = resFree;
	zHcuL3BfscGenCtrlTable.wsValueNbrTtt = resTtt;
	zHcuL3BfscGenCtrlTable.wsValueNbrTgu = resTgu;

	//返回
	return;
}



