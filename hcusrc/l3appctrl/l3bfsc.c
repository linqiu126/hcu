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
HcuFsmStateItem_t HcuFsmL3bfsc[] =
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
	{MSG_ID_UICOMM_L3BFSC_PARAM_SET_RESULT,     FSM_STATE_L3BFSC_ACTIVED,          			fsm_l3bfsc_uicomm_param_set_result},

	//人工配置状态：等待参数配置完成。完成后，发送MSG_ID_L3BFSC_CAN_WS_INIT_REQ，进入FSM_STATE_L3BFSC_WS_INIT
	//任何状态下，允许人工界面强行发送重启命令（后台或者本地界面），BFSCUICOMM模块将发送MSG_ID_COM_RESTART，从而重启整个系统
	//重启需要重新登录并初始化整个秤盘传感器
    {MSG_ID_COM_RESTART,        				FSM_STATE_L3BFSC_OPR_CFG,            		fsm_l3bfsc_restart},
	{MSG_ID_COM_TIME_OUT,       				FSM_STATE_L3BFSC_OPR_CFG,          			fsm_l3bfsc_time_out},
	{MSG_ID_UICOMM_L3BFSC_CMD_REQ,       		FSM_STATE_L3BFSC_OPR_CFG,          			fsm_l3bfsc_uicomm_cmd_req},
	{MSG_ID_CLOUDVELA_L3BFSC_CMD_REQ,       	FSM_STATE_L3BFSC_OPR_CFG,          			fsm_l3bfsc_cloudvela_cmd_req},
	{MSG_ID_UICOMM_L3BFSC_PARAM_SET_RESULT,     FSM_STATE_L3BFSC_OPR_CFG,          			fsm_l3bfsc_uicomm_param_set_result},

	//等待下位机完成参数初始化：等待MSG_ID_CAN_L3FSC_WS_INIT_RESP，完成后进入FSM_STATE_L3BFSC_OOS_SCAN
    {MSG_ID_COM_RESTART,        				FSM_STATE_L3BFSC_WS_INIT,            		fsm_l3bfsc_restart},
	{MSG_ID_COM_TIME_OUT,       				FSM_STATE_L3BFSC_WS_INIT,          			fsm_l3bfsc_time_out},
	{MSG_ID_CAN_L3BFSC_WS_INIT_FB,       		FSM_STATE_L3BFSC_WS_INIT,          			fsm_l3bfsc_canitf_ws_init_fb},

	//进料组合态：等待正常的MSG_ID_CAN_L3BFSC_WS_NEW_READY_EVENT，每一次进来均触发一次组合算法。结果无动作，或发送MSG_ID_L3BFSC_CAN_WS_COMB_OUT/MSG_ID_L3BFSC_CAN_WS_GIVE_UP
    {MSG_ID_COM_RESTART,        				FSM_STATE_L3BFSC_OOS_SCAN,            		fsm_l3bfsc_restart},
	{MSG_ID_COM_TIME_OUT,       				FSM_STATE_L3BFSC_OOS_SCAN,          		fsm_l3bfsc_time_out},
	{MSG_ID_CAN_L3BFSC_WS_NEW_READY_EVENT,      FSM_STATE_L3BFSC_OOS_SCAN,          		fsm_l3bfsc_canitf_ws_new_ready_event},  //只能触发数据存储，不进入组合算法的执行
	{MSG_ID_CLOUDVELA_L3BFSC_CMD_REQ,       	FSM_STATE_L3BFSC_OOS_SCAN,          		fsm_l3bfsc_cloudvela_cmd_req},  //只有在最为常见的SCAN状态下才允许后台干预，否则太复杂
	{MSG_ID_CAN_L3BFSC_WS_READ_RESP,       		FSM_STATE_L3BFSC_OOS_SCAN,          		fsm_l3bfsc_canitf_period_read_resp},  //只有在最为常见的SCAN状态下才允许定时上报，否则太复杂
	{MSG_ID_CAN_L3BFSC_GENERAL_CMD_RESP,       	FSM_STATE_L3BFSC_OOS_SCAN,          		fsm_l3bfsc_canitf_general_cmd_resp},  //只有在最为常见的SCAN状态下才允许后台，否则太复杂

	//出料流程态：单纯等待MSG_ID_CAN_L3BFSC_WS_COMB_OUT_FB，一旦收到无误后进入FSM_STATE_L3BFSC_OOS_SCAN。差错进入FSM_STATE_L3BFSC_ERROR_INQ。
    {MSG_ID_COM_RESTART,        				FSM_STATE_L3BFSC_OOS_TTT,            		fsm_l3bfsc_restart},
	{MSG_ID_COM_TIME_OUT,       				FSM_STATE_L3BFSC_OOS_TTT,          			fsm_l3bfsc_time_out},
	{MSG_ID_CAN_L3BFSC_WS_COMB_OUT_FB,       	FSM_STATE_L3BFSC_OOS_TTT,          			fsm_l3bfsc_canitf_ws_comb_out_fb},

	//放弃物料态：单纯等待MSG_ID_CAN_L3BFSC_WS_GIVE_UP_FB，一旦收到无误后进入FSM_STATE_L3BFSC_OOS_SCAN。差错进入FSM_STATE_L3BFSC_ERROR_INQ。
    {MSG_ID_COM_RESTART,        				FSM_STATE_L3BFSC_OOS_TGU,            		fsm_l3bfsc_restart},
	{MSG_ID_COM_TIME_OUT,       				FSM_STATE_L3BFSC_OOS_TGU,          			fsm_l3bfsc_time_out},
	{MSG_ID_CAN_L3BFSC_WS_GIVE_UP_FB,       	FSM_STATE_L3BFSC_OOS_TGU,          			fsm_l3bfsc_canitf_ws_give_up_fb},

	//数据差错，重新采样所有数据：收到MSG_ID_CAN_L3BFSC_WS_NEW_READY_EVENT差错，发送MSG_ID_L3BFSC_CAN_CMD_REQ，等待传感器传回所有差错的重测值。结束以后进入FSM_STATE_L3BFSC_OOS_SCAN。
    {MSG_ID_COM_RESTART,        				FSM_STATE_L3BFSC_ERROR_INQ,            		fsm_l3bfsc_restart},
	{MSG_ID_COM_TIME_OUT,       				FSM_STATE_L3BFSC_ERROR_INQ,          		fsm_l3bfsc_time_out},
	{MSG_ID_CAN_L3BFSC_ERROR_INQ_CMD_RESP,      FSM_STATE_L3BFSC_ERROR_INQ,          		fsm_l3bfsc_canitf_error_inq_cmd_resp},  //只能触发数据存储，不进入组合算法的执行

    //结束点，固定定义，不要改动
    {MSG_ID_END,            	FSM_STATE_END,             				NULL},  //Ending
};

//状态机中的一些遗留问题：
//1. 什么是否允许后台查询或者发送命令？任何时候？这意味着状态机需要变得较为复杂了
//2.为了演示任务，定时读取


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
			HcuErrorPrint("L3BFSC: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3BFSC].taskName, zHcuVmCtrTab.task[src_id].taskName);
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
	zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3BFSC] = 0;
	#if (HCU_BFSC_SENSOR_WS_NBR_MAX > HUITP_SCALE_WEIGHT_SENSOR_NBR_MAX)
		#error L3BFSC module level configuration number error!
	#endif

	//秤盘数据表单控制表初始化
	memset(&zHcuL3BfscGenCtrlTable, 0, sizeof(L3BfscGenCtrlTable_t));
	int i=0;
	for (i=0; i<HCU_BFSC_SENSOR_WS_NBR_MAX; i++){
		zHcuL3BfscGenCtrlTable.sensorWs[i].sensorWsId = i;
	}
	zHcuL3BfscGenCtrlTable.minWsNbr = 1;
	zHcuL3BfscGenCtrlTable.minWsNbr = HCU_BFSC_SENSOR_WS_NBR_MAX;

	//为搜索空间申请内存
	zHcuSearchSpaceTotalNbr = pow(2, HCU_BFSC_SENSOR_WS_NBR_MAX);
	zHcuSearchCoefficientPointer = malloc(zHcuSearchSpaceTotalNbr * HCU_BFSC_SENSOR_WS_NBR_MAX * sizeof(UINT8));
	if (zHcuSearchCoefficientPointer == NULL)
		HCU_ERROR_PRINT_L3BFSC("L3BFSC: Init parameter error!\n");
	memset(zHcuSearchCoefficientPointer, 0, zHcuSearchSpaceTotalNbr * HCU_BFSC_SENSOR_WS_NBR_MAX  * sizeof(UINT8));

	//初始化搜索系数表
	UINT8 *p = NULL;
	UINT8 j=0;
	UINT32 k=0, t=0;
	UINT32 kUp = 0;
	UINT32 tUp = 0, Index=0;
	for (j=0; j<HCU_BFSC_SENSOR_WS_NBR_MAX; j++){
		kUp = pow(2, HCU_BFSC_SENSOR_WS_NBR_MAX-j-1);
		for (k=0; k< kUp; k++){
			tUp = pow(2, j);
			for (t=0; t<tUp; t++){
				Index = j + (k * pow(2, j+1) + t) * HCU_BFSC_SENSOR_WS_NBR_MAX;
				*(zHcuSearchCoefficientPointer + Index) = 1;
			}
		}
	}
	p = zHcuSearchCoefficientPointer;
	char targetStr[100];
	char tStr[10];
	for (i=0; i<zHcuSearchSpaceTotalNbr; i++){
		//这个长度是按照HCU_BFSC_SENSOR_WS_NBR_MAX=12来固定设定的，如果改变，改参数打印函数需要从新改变，不然会导致程序死机
		sprintf(targetStr, "L3BFSC: Matrix Table[%d]=[", i);
		for (j=0; j<HCU_BFSC_SENSOR_WS_NBR_MAX; j++){
			sprintf(tStr, "%d", *(p+HCU_BFSC_SENSOR_WS_NBR_MAX*i+j));
			strcat(targetStr, tStr);
		}
		strcat(targetStr, "]\n");
		//HCU_DEBUG_PRINT_INF(targetStr);
	}

	//启动周期性定时器：这个定时器是为了定时汇报12个传感器的测量数量
	ret = hcu_timer_start(TASK_ID_L3BFSC, TIMER_ID_1S_L3BFSC_PERIOD_READ, \
			zHcuSysEngPar.timer.array[TIMER_ID_1S_L3BFSC_PERIOD_READ].dur, TIMER_TYPE_PERIOD, TIMER_RESOLUTION_1S);
	if (ret == FAILURE)
		HCU_ERROR_PRINT_L3BFSC("L3BFSC: Error start period timer!\n");

	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_L3BFSC, FSM_STATE_L3BFSC_ACTIVED) == FAILURE)
		HCU_ERROR_PRINT_L3BFSC("L3BFSC: Error Set FSM State!\n");
	HCU_DEBUG_PRINT_FAT("L3BFSC: Enter FSM_STATE_L3BFSC_ACTIVED status, Keeping refresh here!\n");

	//返回
	return SUCCESS;
}

OPSTAT fsm_l3bfsc_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HcuErrorPrint("L3BFSC: Internal error counter reach DEAD level, SW-RESTART soon!\n");
	zHcuSysStaPm.statisCnt.restartCnt++;
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
		HCU_ERROR_PRINT_L3BFSC("L3BFSC: Receive message error!\n");
	}
	memcpy(&rcv, param_ptr, param_len);

	//钩子在此处，检查zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3BFSC]是否超限
	if (zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3BFSC] > HCU_RUN_ERROR_LEVEL_2_MAJOR){
		//减少重复RESTART的概率
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3BFSC] = zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3BFSC] - HCU_RUN_ERROR_LEVEL_2_MAJOR;
		msg_struct_com_restart_t snd0;
		memset(&snd0, 0, sizeof(msg_struct_com_restart_t));
		snd0.length = sizeof(msg_struct_com_restart_t);
		ret = hcu_message_send(MSG_ID_COM_RESTART, TASK_ID_L3BFSC, TASK_ID_L3BFSC, &snd0, snd0.length);
		if (ret == FAILURE){
			HCU_ERROR_PRINT_L3BFSC("L3BFSC: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3BFSC].taskName, zHcuVmCtrTab.task[TASK_ID_L3BFSC].taskName);
		}
	}

	//Period time out received
	if ((rcv.timeId == TIMER_ID_1S_L3BFSC_PERIOD_READ) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		//保护周期读数的优先级，强制抢占状态，并简化问题
		//在这个较为复杂的状态机中，这里的简单状态判定不适用，暂时去掉
		//周期性读取，只能在FSM_STATE_L3BFSC_OOS_SCAN下，否则不允许上报。如果不在此状态，直接返回，但此时不认为是差错
		if (FsmGetState(TASK_ID_L3BFSC) != FSM_STATE_L3BFSC_OOS_SCAN){
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3BFSC]++;
			HcuErrorPrint("L3BFSC: Not in FSM_STATE_L3BFSC_OOS_SCAN, so can not execute period read command!\n");
			return SUCCESS;
		}
		else if (func_l3bfsc_time_out_period_read_process() == FAILURE){
			HCU_ERROR_PRINT_L3BFSC("L3BFSC: Error process time out message!\n");
		}
	}

	//这个地方无限循环初始化
	else if ((rcv.timeId == TIMER_ID_1S_L3BFSC_INIT_FB_WAIT) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		if (func_l3bfsc_time_out_ws_init_req_process() == FAILURE){
			HCU_ERROR_PRINT_L3BFSC("L3BFSC: Error process time out message!\n");
		}
	}

	//一次性工作
	else if ((rcv.timeId == TIMER_ID_1S_L3BFSC_TTT_WAIT_FB) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		if (func_l3bfsc_time_out_ttt_wait_fb_process() == FAILURE){
			HCU_ERROR_PRINT_L3BFSC("L3BFSC: Error prceoss time out message!\n");
		}
	}

	//一次性工作
	else if ((rcv.timeId == TIMER_ID_1S_L3BFSC_TGU_WAIT_FB) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		if (func_l3bfsc_time_out_tgu_wait_fb_process() == FAILURE){
			HCU_ERROR_PRINT_L3BFSC("L3BFSC: Error prceoss time out message!\n");
		}
	}

	//一次性工作
	else if ((rcv.timeId == TIMER_ID_1S_L3BFSC_PERIOD_ERROR_SCAN) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		if (func_l3bfsc_time_out_error_scan_process() == FAILURE){
			HCU_ERROR_PRINT_L3BFSC("L3BFSC: Error prceoss time out message!\n");
		}
	}

	//返回
	return SUCCESS;
}

//专门用于处理在差错状态下的状态查询
OPSTAT fsm_l3bfsc_canitf_error_inq_cmd_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;

	msg_struct_can_l3bfsc_error_inq_cmd_resp_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_can_l3bfsc_error_inq_cmd_resp_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_can_l3bfsc_error_inq_cmd_resp_t))){
		HCU_ERROR_PRINT_L3BFSC("L3BFSC: Receive message error!\n");
	}
	memcpy(&rcv, param_ptr, param_len);

	//入参检查
	if ((rcv.sensorid > HCU_BFSC_SENSOR_WS_NBR_MAX) || (rcv.sensorWsValue > (zHcuL3BfscGenCtrlTable.targetValue + zHcuL3BfscGenCtrlTable.targetUpLimit))){
		HCU_ERROR_PRINT_L3BFSC("L3BFSC: Receive message error!\n");
	}

	//停止定时器
	ret = hcu_timer_stop(TASK_ID_L3BFSC, TIMER_ID_1S_L3BFSC_PERIOD_ERROR_SCAN, TIMER_RESOLUTION_1S);
	if (ret == FAILURE){
		HCU_ERROR_PRINT_L3BFSC("L3BFSC: Error stop period timer!\n");
	}

	//重发INQ_CMD_REQ，持续等待在这个错误状态中
	if (rcv.flag != 1){
		zHcuL3BfscGenCtrlTable.sensorWs[rcv.sensorid].sensorStatus = HCU_L3BFSC_SENSOR_WS_STATUS_INVALID;

		//启动周期性定时器
		ret = hcu_timer_start(TASK_ID_L3BFSC, TIMER_ID_1S_L3BFSC_PERIOD_ERROR_SCAN, \
				zHcuSysEngPar.timer.array[TIMER_ID_1S_L3BFSC_PERIOD_ERROR_SCAN].dur, TIMER_TYPE_PERIOD, TIMER_RESOLUTION_1S);
		if (ret == FAILURE){
			HCU_ERROR_PRINT_L3BFSC("L3BFSC: Error start period timer!\n");
		}

		//重新发送查询命令
		msg_struct_l3bfsc_can_error_inq_cmd_req_t snd;
		memset(&snd, 0, sizeof(msg_struct_l3bfsc_can_error_inq_cmd_req_t));
		snd.length = sizeof(msg_struct_l3bfsc_can_error_inq_cmd_req_t);
		snd.sensorid = rcv.sensorid;
		ret = hcu_message_send(MSG_ID_L3BFSC_CAN_ERROR_INQ_CMD_REQ, TASK_ID_CANITFLEO, TASK_ID_L3BFSC, &snd, snd.length);
		if (ret == FAILURE){
			HCU_ERROR_PRINT_L3BFSC("L3BFSC: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3BFSC].taskName, zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName);
		}

		//保留状态不变

		//返回
		return SUCCESS;
	}

	//正常处理，并回复到SCAN状态
	if (rcv.sensorWsValue == 0) {
		zHcuL3BfscGenCtrlTable.sensorWs[rcv.sensorid].sensorStatus = HCU_L3BFSC_SENSOR_WS_STATUS_EMPTY;
		zHcuL3BfscGenCtrlTable.wsValueNbrFree++;
	}
	else{
		zHcuL3BfscGenCtrlTable.sensorWs[rcv.sensorid].sensorValue = rcv.sensorWsValue;
		zHcuL3BfscGenCtrlTable.sensorWs[rcv.sensorid].sensorStatus = HCU_L3BFSC_SENSOR_WS_STATUS_VALID_TO_COMB;
		zHcuL3BfscGenCtrlTable.wsValueNbrWeight++;
	}

	//如果达到了条件，可以转移：这里的条件是TTT和TGU必须完成
	func_l3bfsc_cacluate_sensor_ws_valid_value();
	if ((zHcuL3BfscGenCtrlTable.wsValueNbrFree + zHcuL3BfscGenCtrlTable.wsValueNbrWeight) == HCU_BFSC_SENSOR_WS_NBR_MAX){
		//停止周期定时器
		ret = hcu_timer_stop(TASK_ID_L3BFSC, TIMER_ID_1S_L3BFSC_PERIOD_ERROR_SCAN, TIMER_RESOLUTION_1S);
		if (ret == FAILURE){
			HCU_ERROR_PRINT_L3BFSC_RECOVERY("L3BFSC: Error start timer!\n");
		}

		//设置状态机到目标状态
		if (FsmSetState(TASK_ID_L3BFSC, FSM_STATE_L3BFSC_OOS_SCAN) == FAILURE){
			HCU_ERROR_PRINT_L3BFSC_RECOVERY("L3BFSC: Error Set FSM State!\n");
		}
	}

	//返回
	return SUCCESS;
}

OPSTAT fsm_l3bfsc_canitf_ws_comb_out_fb(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;
	msg_struct_can_l3bfsc_ws_comb_out_fb_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_can_l3bfsc_ws_comb_out_fb_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_can_l3bfsc_ws_comb_out_fb_t))){
		HCU_ERROR_PRINT_L3BFSC_RECOVERY("L3BFSC: Receive message error!\n");
	}
	memcpy(&rcv, param_ptr, param_len);

	zHcuL3BfscGenCtrlTable.wsTotalCombOutMatCnt++;

	//恢复
	if (zHcuL3BfscGenCtrlTable.wsBitmap[rcv.sensorid] == 1){
		zHcuL3BfscGenCtrlTable.sensorWs[rcv.sensorid].sensorValue = 0;
		zHcuL3BfscGenCtrlTable.sensorWs[rcv.sensorid].sensorStatus = HCU_L3BFSC_SENSOR_WS_STATUS_EMPTY;
		zHcuL3BfscGenCtrlTable.wsBitmap[rcv.sensorid] = 0;
		zHcuL3BfscGenCtrlTable.wsValueNbrFree++;
		if (zHcuL3BfscGenCtrlTable.wsValueNbrTtt>0) zHcuL3BfscGenCtrlTable.wsValueNbrTtt--;
	}

	//检查是否收到所有传感器的反馈
	if (zHcuL3BfscGenCtrlTable.wsValueNbrTtt == 0){
		//停止定时器
		ret = hcu_timer_stop(TASK_ID_L3BFSC, TIMER_ID_1S_L3BFSC_TTT_WAIT_FB, TIMER_RESOLUTION_1S);
		if (ret == FAILURE){
			HCU_ERROR_PRINT_L3BFSC_RECOVERY("L3BFSC: Error stop timer!\n");
		}
		//设置状态机
		if (FsmSetState(TASK_ID_L3BFSC, FSM_STATE_L3BFSC_OOS_SCAN) == FAILURE){
			HCU_ERROR_PRINT_L3BFSC_RECOVERY("L3BFSC: Error Set FSM State!\n");
		}
	}
/*
	for (i=0; i<HCU_BFSC_SENSOR_WS_NBR_MAX; i++){
		if (zHcuL3BfscGenCtrlTable.wsBitmap[i] == 1){
			zHcuL3BfscGenCtrlTable.sensorWs[i].sensorValue = 0;
			zHcuL3BfscGenCtrlTable.sensorWs[i].sensorStatus = HCU_L3BFSC_SENSOR_WS_STATUS_EMPTY;
			zHcuL3BfscGenCtrlTable.wsValueNbrFree++;
		}
	}
*/
	//memset(zHcuL3BfscGenCtrlTable.wsBitmap, 0, HCU_BFSC_SENSOR_WS_NBR_MAX);
	//zHcuL3BfscGenCtrlTable.wsValueNbrTtt = 0;

	//返回
	return SUCCESS;
}

OPSTAT fsm_l3bfsc_canitf_ws_give_up_fb(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;
	msg_struct_can_l3bfsc_ws_give_up_fb_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_can_l3bfsc_ws_give_up_fb_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_can_l3bfsc_ws_give_up_fb_t))){
		HCU_ERROR_PRINT_L3BFSC_RECOVERY("L3BFSC: Receive message error!\n");
	}
	memcpy(&rcv, param_ptr, param_len);

	zHcuL3BfscGenCtrlTable.wsTotalGiveupMatCnt++;

	//恢复
	if (zHcuL3BfscGenCtrlTable.wsBitmap[rcv.sensorid] == 1){
		zHcuL3BfscGenCtrlTable.sensorWs[rcv.sensorid].sensorValue = 0;
		zHcuL3BfscGenCtrlTable.sensorWs[rcv.sensorid].sensorStatus = HCU_L3BFSC_SENSOR_WS_STATUS_EMPTY;
		zHcuL3BfscGenCtrlTable.wsBitmap[rcv.sensorid] = 0;
		zHcuL3BfscGenCtrlTable.wsValueNbrFree++;
		if (zHcuL3BfscGenCtrlTable.wsValueNbrTgu>0) zHcuL3BfscGenCtrlTable.wsValueNbrTgu--;
	}

	//检查是否收到所有传感器的反馈
	if (zHcuL3BfscGenCtrlTable.wsValueNbrTgu == 0){
		//停止定时器
		ret = hcu_timer_stop(TASK_ID_L3BFSC, TIMER_ID_1S_L3BFSC_TGU_WAIT_FB, TIMER_RESOLUTION_1S);
		if (ret == FAILURE){
			HCU_ERROR_PRINT_L3BFSC_RECOVERY("L3BFSC: Error stop timer!\n");
		}
		//设置状态机
		if (FsmSetState(TASK_ID_L3BFSC, FSM_STATE_L3BFSC_OOS_SCAN) == FAILURE){
			HCU_ERROR_PRINT_L3BFSC_RECOVERY("L3BFSC: Error Set FSM State!\n");
		}
	}

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
		HCU_ERROR_PRINT_L3BFSC("L3BFSC: Receive message error!\n");
	}
	memcpy(&rcv, param_ptr, param_len);*/

	//检查收到的数据的正确性，然后再继续往CLOUD发送，仍然以平淡消息的格式，让L2_CLOUDVELA进行编码
	//检查参数配置的合法性，目标值必须在一定的合理范围内，否则拒绝进入进料状态

	//停止定时器


	//返回
	return SUCCESS;
}


//后来来的命令，发送到CANITFLEO模块，执行命令后，再返回给后台
//由于是内部消息命令执行，为了简化整个执行，不设置超时状态，以简化整个状态机的设计
OPSTAT fsm_l3bfsc_cloudvela_cmd_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;

	//入参检查
	msg_struct_cloudvela_l3bfsc_cmd_req_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_cloudvela_l3bfsc_cmd_req_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_cloudvela_l3bfsc_cmd_req_t))){
		HCU_ERROR_PRINT_L3BFSC_RECOVERY("L3BFSC: Receive message error!\n");
	}
	memcpy(&rcv, param_ptr, param_len);
	if (rcv.cmdid != L3CI_bfsc_comb_scale){
		HCU_ERROR_PRINT_L3BFSC_RECOVERY("L3BFSC: Receive message error!\n");
	}

	//将命令发送到传感器下位机
	msg_struct_l3bfsc_can_general_cmd_req_t snd;
	memset(&snd, 0, sizeof(msg_struct_l3bfsc_can_general_cmd_req_t));
	snd.length = sizeof(msg_struct_l3bfsc_can_general_cmd_req_t);

	//这里只支持一种启动和停止命令，其它的暂时不支持
	if ((rcv.optid != L3PO_bfsc_start_cmd) && (rcv.optid != L3PO_bfsc_stop_cmd)){
		HCU_ERROR_PRINT_L3BFSC_RECOVERY("L3BFSC: Can not supported command coming from cloud!\n");
	}

	snd.optid = rcv.optid;
	snd.optpar = rcv.optopr;
	//这里如此设置，表示是为了全局所有的传感器
	snd.sensorid = HCU_BFSC_SENSOR_WS_NBR_MAX;
	ret = hcu_message_send(MSG_ID_L3BFSC_CAN_GENERAL_CMD_REQ, TASK_ID_CANITFLEO, TASK_ID_L3BFSC, &snd, snd.length);
	if (ret == FAILURE){
		HCU_ERROR_PRINT_L3BFSC_RECOVERY("L3BFSC: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3BFSC].taskName, zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName);
	}

	//状态不转移

	//短时钟不启动

	//返回
	return SUCCESS;
}

//后台命令执行的结果
OPSTAT fsm_l3bfsc_canitf_general_cmd_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;

	//入参检查
	msg_struct_can_l3bfsc_general_cmd_resp_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_can_l3bfsc_general_cmd_resp_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_can_l3bfsc_general_cmd_resp_t))){
		HCU_ERROR_PRINT_L3BFSC_RECOVERY("L3BFSC: Receive message error!\n");
	}
	memcpy(&rcv, param_ptr, param_len);

	//将数据发送到CLOUDVELA中去
	//这里暂时不做数据合法性的判定，未来再考虑提高可靠性和安全性
	msg_struct_l3bfsc_cloudvela_cmd_resp_t snd;
	memset(&snd, 0, sizeof(msg_struct_l3bfsc_cloudvela_cmd_resp_t));
	snd.length = sizeof(msg_struct_l3bfsc_cloudvela_cmd_resp_t);
	snd.optid = rcv.optid;
	snd.optpar = rcv.optpar;
	snd.modbusVal = rcv.modbusVal;
	snd.timestamp = time(0);
	ret = hcu_message_send(MSG_ID_L3BFSC_CLOUDVELA_CMD_RESP, TASK_ID_CLOUDVELA, TASK_ID_L3BFSC, &snd, snd.length);
	if (ret == FAILURE){
		HCU_ERROR_PRINT_L3BFSC_RECOVERY("L3BFSC: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3BFSC].taskName, zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName);
	}

	//状态不转移

	//短时钟不启动

	//返回
	return SUCCESS;
}

//触发组合算法
OPSTAT fsm_l3bfsc_canitf_ws_new_ready_event(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0, i=0;

	//入参检查
	msg_struct_can_l3bfsc_new_ready_event_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_can_l3bfsc_new_ready_event_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_can_l3bfsc_new_ready_event_t))){
		HCU_ERROR_PRINT_L3BFSC_RECOVERY("L3BFSC: Receive message error!\n");
	}
	memcpy(&rcv, param_ptr, param_len);

	if ((rcv.sensorid > HCU_BFSC_SENSOR_WS_NBR_MAX) || (rcv.sensorWsValue > (zHcuL3BfscGenCtrlTable.targetValue + zHcuL3BfscGenCtrlTable.targetUpLimit))){
		HCU_ERROR_PRINT_L3BFSC_RECOVERY("L3BFSC: Receive message error!\n");
	}

	//正常处理
	zHcuL3BfscGenCtrlTable.wsTotalncomingCnt++;
	HCU_DEBUG_PRINT_INF("L3BFSC: Sensor Input Id = %d, Status = %d\n", rcv.sensorid, zHcuL3BfscGenCtrlTable.sensorWs[rcv.sensorid].sensorStatus);
	if (zHcuL3BfscGenCtrlTable.sensorWs[rcv.sensorid].sensorStatus == HCU_L3BFSC_SENSOR_WS_STATUS_EMPTY){
		zHcuL3BfscGenCtrlTable.sensorWs[rcv.sensorid].sensorValue = rcv.sensorWsValue;
		zHcuL3BfscGenCtrlTable.sensorWs[rcv.sensorid].sensorStatus = HCU_L3BFSC_SENSOR_WS_STATUS_VALID_TO_COMB;
		if (zHcuL3BfscGenCtrlTable.wsValueNbrFree>0) zHcuL3BfscGenCtrlTable.wsValueNbrFree--;
		zHcuL3BfscGenCtrlTable.wsValueNbrWeight++;
		zHcuL3BfscGenCtrlTable.wsValueNbrWeight = zHcuL3BfscGenCtrlTable.wsValueNbrWeight % (HCU_BFSC_SENSOR_WS_NBR_MAX + 1);
		HCU_DEBUG_PRINT_NOR("L3BFSC: Receive EVENT, Normally status!\n");
	}
	//重复来临
	else if (zHcuL3BfscGenCtrlTable.sensorWs[rcv.sensorid].sensorStatus == HCU_L3BFSC_SENSOR_WS_STATUS_VALID_TO_COMB){
		zHcuL3BfscGenCtrlTable.sensorWs[rcv.sensorid].sensorValue = rcv.sensorWsValue;
		HCU_DEBUG_PRINT_NOR("L3BFSC: Receive EVENT, Repeat status!\n");
	}
	//恢复错误
	else if (zHcuL3BfscGenCtrlTable.sensorWs[rcv.sensorid].sensorStatus == HCU_L3BFSC_SENSOR_WS_STATUS_VALID_ERROR){
		zHcuL3BfscGenCtrlTable.sensorWs[rcv.sensorid].sensorValue = rcv.sensorWsValue;
		zHcuL3BfscGenCtrlTable.sensorWs[rcv.sensorid].sensorStatus = HCU_L3BFSC_SENSOR_WS_STATUS_VALID_TO_COMB;
		zHcuL3BfscGenCtrlTable.wsValueNbrWeight++;
		zHcuL3BfscGenCtrlTable.wsValueNbrWeight = zHcuL3BfscGenCtrlTable.wsValueNbrWeight % (HCU_BFSC_SENSOR_WS_NBR_MAX + 1);
		HCU_DEBUG_PRINT_NOR("L3BFSC: Receive EVENT, Error recover status!\n");
	}
	//错误来临：进入ERROR_INQ流程 (TTT/TGU或者其它状态）
	else
	{
		HCU_DEBUG_PRINT_NOR("L3BFSC: Receive EVENT, Error status!\n");
		//为了全局控制表统计的正确性
		if (zHcuL3BfscGenCtrlTable.sensorWs[rcv.sensorid].sensorStatus == HCU_L3BFSC_SENSOR_WS_STATUS_VALID_TO_TTT) {
			if (zHcuL3BfscGenCtrlTable.wsValueNbrTtt > 0) zHcuL3BfscGenCtrlTable.wsValueNbrTtt--;
		}
		else if (zHcuL3BfscGenCtrlTable.sensorWs[rcv.sensorid].sensorStatus == HCU_L3BFSC_SENSOR_WS_STATUS_VALID_TO_TGU) {
			if (zHcuL3BfscGenCtrlTable.wsValueNbrTgu > 0) zHcuL3BfscGenCtrlTable.wsValueNbrTgu--;
		}
		zHcuL3BfscGenCtrlTable.sensorWs[rcv.sensorid].sensorStatus = HCU_L3BFSC_SENSOR_WS_STATUS_INVALID;

		//启动周期性定时器
		ret = hcu_timer_start(TASK_ID_L3BFSC, TIMER_ID_1S_L3BFSC_PERIOD_ERROR_SCAN, \
				zHcuSysEngPar.timer.array[TIMER_ID_1S_L3BFSC_PERIOD_ERROR_SCAN].dur, TIMER_TYPE_PERIOD, TIMER_RESOLUTION_1S);
		if (ret == FAILURE){
			HCU_ERROR_PRINT_L3BFSC_RECOVERY("L3BFSC: Error start period timer!\n");
		}

		//发送查询命令
		msg_struct_l3bfsc_can_error_inq_cmd_req_t snd0;
		memset(&snd0, 0, sizeof(msg_struct_l3bfsc_can_error_inq_cmd_req_t));
		snd0.length = sizeof(msg_struct_l3bfsc_can_error_inq_cmd_req_t);
		snd0.sensorid = rcv.sensorid;
		ret = hcu_message_send(MSG_ID_L3BFSC_CAN_ERROR_INQ_CMD_REQ, TASK_ID_CANITFLEO, TASK_ID_L3BFSC, &snd0, snd0.length);
		if (ret == FAILURE) {
			HCU_ERROR_PRINT_L3BFSC_RECOVERY("L3BFSC: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3BFSC].taskName, zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName);
		}

		//设置新状态
		if (FsmSetState(TASK_ID_L3BFSC, FSM_STATE_L3BFSC_ERROR_INQ) == FAILURE) {
			HCU_ERROR_PRINT_L3BFSC_RECOVERY("L3BFSC: Error Set FSM State!\n");
		}

		return SUCCESS;
	}

	//重新统计各个参数
	func_l3bfsc_cacluate_sensor_ws_valid_value();

	//这里不可能出现以下情况
	if ((zHcuL3BfscGenCtrlTable.wsValueNbrTtt !=0) && (zHcuL3BfscGenCtrlTable.wsValueNbrTgu !=0)){
		HCU_ERROR_PRINT_L3BFSC_RECOVERY("L3BFSC: Wrong TTT/TGU figure during SCAN mode!\n");
	}

	//是否要进入搜索
	if (zHcuL3BfscGenCtrlTable.wsValueNbrWeight >= zHcuL3BfscGenCtrlTable.minWsNbr){
		if (func_l3bfsc_ws_sensor_search_combination() == -1){
			if (zHcuL3BfscGenCtrlTable.wsValueNbrWeight == HCU_BFSC_SENSOR_WS_NBR_MAX){
				//得到抛弃的传感器
				func_l3bfsc_ws_sensor_search_give_up();

				zHcuL3BfscGenCtrlTable.wsTotalGiveupTimes++;
				//发送抛弃命令
				msg_struct_l3bfsc_can_ws_give_up_t snd1;
				memset(&snd1, 0, sizeof(msg_struct_l3bfsc_can_ws_give_up_t));
				snd1.length = sizeof(msg_struct_l3bfsc_can_ws_give_up_t);
				memcpy(snd1.sensorBitmap, zHcuL3BfscGenCtrlTable.wsBitmap, HCU_BFSC_SENSOR_WS_NBR_MAX);
				ret = hcu_message_send(MSG_ID_L3BFSC_CAN_WS_GIVE_UP, TASK_ID_CANITFLEO, TASK_ID_L3BFSC, &snd1, snd1.length);
				if (ret == FAILURE){
					HCU_ERROR_PRINT_L3BFSC_RECOVERY("L3BFSC: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3BFSC].taskName, zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName);
				}

				//测试打印
				char targetStr[100];
				char tStr[10];
				sprintf(targetStr, "L3BFSC: Send out TGU_OUT bitmap [%d]=[", zHcuL3BfscGenCtrlTable.wsValueNbrTgu);
				for (i=0; i<HCU_BFSC_SENSOR_WS_NBR_MAX; i++){
					sprintf(tStr, "%d", *(zHcuL3BfscGenCtrlTable.wsBitmap+i));
					strcat(targetStr, tStr);
				}
				strcat(targetStr, "]\n");
				HCU_DEBUG_PRINT_IPT(targetStr);

				//启动定时器
				ret = hcu_timer_start(TASK_ID_L3BFSC, TIMER_ID_1S_L3BFSC_TGU_WAIT_FB, \
						zHcuSysEngPar.timer.array[TIMER_ID_1S_L3BFSC_TGU_WAIT_FB].dur, TIMER_TYPE_ONE_TIME, TIMER_RESOLUTION_1S);
				if (ret == FAILURE){
					HCU_ERROR_PRINT_L3BFSC_RECOVERY("L3BFSC: Error start timer!\n");
				}

				//进入TGU状态，设置新状态
				if (FsmSetState(TASK_ID_L3BFSC, FSM_STATE_L3BFSC_OOS_TGU) == FAILURE){
					HCU_ERROR_PRINT_L3BFSC_RECOVERY("L3BFSC: Error Set FSM State!\n");
				}
			}
		}//-1

		//返回有意义的数值
		else{
			//发送出料命令
			zHcuL3BfscGenCtrlTable.wsTotalCombSucTimes++;

			msg_struct_l3bfsc_can_ws_comb_out_t snd2;
			memset(&snd2, 0, sizeof(msg_struct_l3bfsc_can_ws_comb_out_t));
			snd2.length = sizeof(msg_struct_l3bfsc_can_ws_comb_out_t);
			memcpy(snd2.sensorBitmap, zHcuL3BfscGenCtrlTable.wsBitmap, HCU_BFSC_SENSOR_WS_NBR_MAX);
			snd2.combnbr = zHcuL3BfscGenCtrlTable.wsValueNbrTtt;
			ret = hcu_message_send(MSG_ID_L3BFSC_CAN_WS_COMB_OUT, TASK_ID_CANITFLEO, TASK_ID_L3BFSC, &snd2, snd2.length);
			if (ret == FAILURE){
				HCU_ERROR_PRINT_L3BFSC_RECOVERY("L3BFSC: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3BFSC].taskName, zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName);
			}

			//测试打印
			char targetStr[100];
			char tStr[10];
			sprintf(targetStr, "L3BFSC: Send out TTT_OUT bitmap [%d]=[", zHcuL3BfscGenCtrlTable.wsValueNbrTtt);
			for (i=0; i<HCU_BFSC_SENSOR_WS_NBR_MAX; i++){
				sprintf(tStr, "%d", *(zHcuL3BfscGenCtrlTable.wsBitmap+i));
				strcat(targetStr, tStr);
			}
			strcat(targetStr, "]\n");
			HCU_DEBUG_PRINT_IPT(targetStr);


			//启动定时器
			ret = hcu_timer_start(TASK_ID_L3BFSC, TIMER_ID_1S_L3BFSC_TTT_WAIT_FB, \
					zHcuSysEngPar.timer.array[TIMER_ID_1S_L3BFSC_TTT_WAIT_FB].dur, TIMER_TYPE_ONE_TIME, TIMER_RESOLUTION_1S);
			if (ret == FAILURE){
				HCU_ERROR_PRINT_L3BFSC_RECOVERY("L3BFSC: Error start timer!\n");
			}

			//进入TTT状态
			if (FsmSetState(TASK_ID_L3BFSC, FSM_STATE_L3BFSC_OOS_TTT) == FAILURE){
				HCU_ERROR_PRINT_L3BFSC_RECOVERY("L3BFSC: Error Set FSM State!\n");
			}
		}
	}

	//重要的统计功能挂载
	HCU_DEBUG_PRINT_CRT("L3BFSC: Control statistics, Running Free/Weight/Ttt/Tgu = [%d, %d, %d, %d], Total Inc Cnt = [%d], Combination Rate = [%5.2f\%], Give-up Rate = [%5.2f\%]\n",\
			zHcuL3BfscGenCtrlTable.wsValueNbrFree, zHcuL3BfscGenCtrlTable.wsValueNbrWeight,\
			zHcuL3BfscGenCtrlTable.wsValueNbrTtt, zHcuL3BfscGenCtrlTable.wsValueNbrTgu, zHcuL3BfscGenCtrlTable.wsTotalncomingCnt, \
			(float)(zHcuL3BfscGenCtrlTable.wsTotalCombOutMatCnt)/(float)(zHcuL3BfscGenCtrlTable.wsTotalncomingCnt+0.01) * 100, \
			(float)(zHcuL3BfscGenCtrlTable.wsTotalGiveupMatCnt)/(float)(zHcuL3BfscGenCtrlTable.wsTotalncomingCnt+0.01) * 100);
	//返回
	return SUCCESS;
}

OPSTAT fsm_l3bfsc_canitf_ws_init_fb(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;

	//入参检查
	msg_struct_can_l3bfsc_ws_init_fb_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_can_l3bfsc_ws_init_fb_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_can_l3bfsc_ws_init_fb_t))){
		HCU_ERROR_PRINT_L3BFSC("L3BFSC: Receive message error!\n");
	}
	memcpy(&rcv, param_ptr, param_len);

	if (rcv.sensorid >= HCU_BFSC_SENSOR_WS_NBR_MAX){
		HCU_ERROR_PRINT_L3BFSC("L3BFSC: Receive message error!\n");
	}

	//单独接受每一个反馈消息，而且还不能重复
	if ((rcv.initFlag == 1) && (zHcuL3BfscGenCtrlTable.sensorWs[rcv.sensorid].sensorStatus !=HCU_L3BFSC_SENSOR_WS_STATUS_EMPTY)) {
		zHcuL3BfscGenCtrlTable.sensorWs[rcv.sensorid].sensorStatus = HCU_L3BFSC_SENSOR_WS_STATUS_EMPTY;
		zHcuL3BfscGenCtrlTable.wsValueNbrFree++;
		HCU_DEBUG_PRINT_INF("L3BFSC: SensorId = %d, Status = %d\n", rcv.sensorid, zHcuL3BfscGenCtrlTable.sensorWs[rcv.sensorid].sensorStatus);
	}
	else{
		zHcuL3BfscGenCtrlTable.sensorWs[rcv.sensorid].sensorStatus = HCU_L3BFSC_SENSOR_WS_STATUS_INVALID;
		//如果返回差错的初始化情况，不处理，继续无限初始化，当然也无法继续往下处理了

		//未来需要考虑发送报告给后台
	}

	//判定是否是最后一个传感器返回
	if (zHcuL3BfscGenCtrlTable.wsValueNbrFree == HCU_BFSC_SENSOR_WS_NBR_MAX){
		//停止定时器
		ret = hcu_timer_stop(TASK_ID_L3BFSC, TIMER_ID_1S_L3BFSC_INIT_FB_WAIT, TIMER_RESOLUTION_1S);
		if (ret == FAILURE){
			HCU_ERROR_PRINT_L3BFSC("L3BFSC: Error start timer!\n");
		}

		//设置新状态
		if (FsmSetState(TASK_ID_L3BFSC, FSM_STATE_L3BFSC_OOS_SCAN) == FAILURE){
			HCU_ERROR_PRINT_L3BFSC("L3BFSC: Error Set FSM State!\n");
		}
	}

	//返回
	return SUCCESS;
}

//周期性读取数据并送往后台
OPSTAT fsm_l3bfsc_canitf_period_read_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0, i=0;

	//入参检查
	msg_struct_can_l3bfsc_ws_read_resp_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_can_l3bfsc_ws_read_resp_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_can_l3bfsc_ws_read_resp_t))){
		HCU_ERROR_PRINT_L3BFSC_RECOVERY("L3BFSC: Receive message error!\n");
	}
	memcpy(&rcv, param_ptr, param_len);

	//将数据发送到CLOUDVELA中去
	//这里暂时不做数据合法性的判定，未来再考虑提高可靠性和安全性
	msg_struct_l3bfsc_cloudvela_data_report_t snd;
	memset(&snd, 0, sizeof(msg_struct_l3bfsc_cloudvela_data_report_t));
	snd.length = sizeof(msg_struct_l3bfsc_cloudvela_data_report_t);
	snd.cmdid = L3CI_bfsc_comb_scale;
	snd.optid = L3PO_bfsc_data_report;
	snd.sensorNbr = HCU_BFSC_SENSOR_WS_NBR_MAX;
	for (i=0; i<HCU_BFSC_SENSOR_WS_NBR_MAX; i++){
		snd.sensorWsValue[i] = rcv.sensorWsValue[i];
	}
	snd.timestamp = time(0);

	ret = hcu_message_send(MSG_ID_L3BFSC_CLOUDVELA_DATA_REPORT, TASK_ID_CLOUDVELA, TASK_ID_L3BFSC, &snd, snd.length);
	if (ret == FAILURE){
		HCU_ERROR_PRINT_L3BFSC_RECOVERY("L3BFSC: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3BFSC].taskName, zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName);
	}

	//状态不转移

	//短时钟不启动

	//返回
	return SUCCESS;
}






OPSTAT fsm_l3bfsc_uicomm_param_set_result(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0, i=0;
	msg_struct_uicomm_l3bfsc_param_set_result_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_uicomm_l3bfsc_param_set_result_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_uicomm_l3bfsc_param_set_result_t))){
		HCU_ERROR_PRINT_L3BFSC("L3BFSC: Receive message error!\n");
	}
	memcpy(&rcv, param_ptr, param_len);

	//检查参数并存入系统区
	zHcuL3BfscGenCtrlTable.targetValue = rcv.targetValue;
	zHcuL3BfscGenCtrlTable.targetUpLimit = rcv.targetUpLimit;
	if (rcv.minWsNbr < 1) zHcuL3BfscGenCtrlTable.minWsNbr = 1;
	else if (rcv.minWsNbr > HCU_BFSC_SENSOR_WS_NBR_MAX) zHcuL3BfscGenCtrlTable.minWsNbr = rcv.minWsNbr % (HCU_BFSC_SENSOR_WS_NBR_MAX+1); //不能超过秤盘最大值
	else zHcuL3BfscGenCtrlTable.minWsNbr = rcv.minWsNbr;

	if (rcv.maxWsNbr < 1) zHcuL3BfscGenCtrlTable.maxWsNbr = 1;
	else if (rcv.minWsNbr > HCU_BFSC_SENSOR_WS_NBR_MAX) zHcuL3BfscGenCtrlTable.maxWsNbr = rcv.maxWsNbr % (HCU_BFSC_SENSOR_WS_NBR_MAX+1); //不能超过秤盘最大值
	else zHcuL3BfscGenCtrlTable.maxWsNbr = rcv.maxWsNbr;

	//设置完成后，发送初始化命令给各个传感器
	msg_struct_l3bfsc_can_ws_init_req_t snd;
	memset(&snd, 0, sizeof(msg_struct_l3bfsc_can_ws_init_req_t));
	snd.length = sizeof(msg_struct_l3bfsc_can_ws_init_req_t);
	for (i=0; i<HCU_BFSC_SENSOR_WS_NBR_MAX; i++) snd.wsBitmap[i] = 1;

	ret = hcu_message_send(MSG_ID_L3BFSC_CAN_WS_INIT_REQ, TASK_ID_CANITFLEO, TASK_ID_L3BFSC, &snd, snd.length);
	if (ret == FAILURE){
		HCU_ERROR_PRINT_L3BFSC("L3BFSC: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3BFSC].taskName, zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName);
	}

	//启动定时器
	ret = hcu_timer_start(TASK_ID_L3BFSC, TIMER_ID_1S_L3BFSC_INIT_FB_WAIT, \
			zHcuSysEngPar.timer.array[TIMER_ID_1S_L3BFSC_INIT_FB_WAIT].dur, TIMER_TYPE_PERIOD, TIMER_RESOLUTION_1S);
	if (ret == FAILURE){
		HCU_ERROR_PRINT_L3BFSC("L3BFSC: Error start timer!\n");
	}

	//设置新状态
	if (FsmSetState(TASK_ID_L3BFSC, FSM_STATE_L3BFSC_WS_INIT) == FAILURE){
		HCU_ERROR_PRINT_L3BFSC("L3BFSC: Error Set FSM State!\n");
	}

	//返回
	return SUCCESS;
}


//系统参数的合法性检查，均在参数初始化中完成，后面不再检查
INT32 func_l3bfsc_ws_sensor_search_combination(void)
{
	UINT32 WsSensorStart = 0;
	UINT32 i=0, j=0, t=0;
	UINT32 result;
	UINT8 resBitmap[HCU_BFSC_SENSOR_WS_NBR_MAX];

	//选取起始位置
	WsSensorStart = zHcuL3BfscGenCtrlTable.wsRrSearchStart % zHcuSearchSpaceTotalNbr;

	//组合搜索
	for (i=WsSensorStart; i< (zHcuSearchSpaceTotalNbr + WsSensorStart); i++){
		result = 0;
		memset(resBitmap, 0, sizeof(resBitmap));
		for (j=0; j<HCU_BFSC_SENSOR_WS_NBR_MAX; j++){
			if (zHcuL3BfscGenCtrlTable.sensorWs[j].sensorStatus == HCU_L3BFSC_SENSOR_WS_STATUS_VALID_TO_COMB){
				t = ((i%zHcuSearchSpaceTotalNbr) * HCU_BFSC_SENSOR_WS_NBR_MAX + j);// % Cycle;
				result += zHcuL3BfscGenCtrlTable.sensorWs[j].sensorValue * (*(zHcuSearchCoefficientPointer + t));
				if (*(zHcuSearchCoefficientPointer + t) == 1) resBitmap[j] = 1;
			}
		}
		//HCU_DEBUG_PRINT_NOR("L3BFSC: Temp value Target=[%d], TargetUp=[%d], result=[%d]\n", zHcuL3BfscGenCtrlTable.targetValue, zHcuL3BfscGenCtrlTable.targetUpLimit, result);
		//如果落入目标范围
		if ((result >= zHcuL3BfscGenCtrlTable.targetValue) && (result <= (zHcuL3BfscGenCtrlTable.targetValue + zHcuL3BfscGenCtrlTable.targetUpLimit))){
			zHcuL3BfscGenCtrlTable.wsRrSearchStart = ((i+1) % zHcuSearchSpaceTotalNbr);
			memcpy(zHcuL3BfscGenCtrlTable.wsBitmap, resBitmap, sizeof(resBitmap));
			zHcuL3BfscGenCtrlTable.wsValueNbrTtt = 0;
			for (j = 0; j<HCU_BFSC_SENSOR_WS_NBR_MAX; j++){
				zHcuL3BfscGenCtrlTable.wsValueNbrTtt += zHcuL3BfscGenCtrlTable.wsBitmap[j];
			}
			i = i % zHcuSearchSpaceTotalNbr;
			return i;
		}
	}
	zHcuL3BfscGenCtrlTable.wsRrSearchStart = ((i+1) % zHcuSearchSpaceTotalNbr);
	return -1;
}


//丢弃必然成功
//先使用随机方式
void func_l3bfsc_ws_sensor_search_give_up(void)
{
	UINT32 i=0;
	memset(zHcuL3BfscGenCtrlTable.wsBitmap, 0, HCU_BFSC_SENSOR_WS_NBR_MAX);
	for (i=0; i<HCU_BFSC_SENSOR_WS_NBR_MAX; i++){
		if (((rand()%11) < 6) &&(zHcuL3BfscGenCtrlTable.sensorWs[i].sensorStatus == HCU_L3BFSC_SENSOR_WS_STATUS_VALID_TO_COMB)){
			zHcuL3BfscGenCtrlTable.wsBitmap[i]=1;
			zHcuL3BfscGenCtrlTable.wsValueNbrTgu++;
		}

	}
	return;
}

void func_l3bfsc_cacluate_sensor_ws_valid_value(void)
{
	int i=0, resFree = 0, resWeight=0, resTtt =0, resTgu = 0;
	for (i=0; i<HCU_BFSC_SENSOR_WS_NBR_MAX; i++)
	{
		if (zHcuL3BfscGenCtrlTable.sensorWs[i].sensorStatus == HCU_L3BFSC_SENSOR_WS_STATUS_EMPTY)
			resFree++;
		else if (zHcuL3BfscGenCtrlTable.sensorWs[i].sensorStatus == HCU_L3BFSC_SENSOR_WS_STATUS_VALID_TO_COMB)
			resWeight++;
		else if (zHcuL3BfscGenCtrlTable.sensorWs[i].sensorStatus == HCU_L3BFSC_SENSOR_WS_STATUS_VALID_TO_TTT)
			resTtt++;
		else if (zHcuL3BfscGenCtrlTable.sensorWs[i].sensorStatus == HCU_L3BFSC_SENSOR_WS_STATUS_VALID_TO_TGU)
			resTgu++;
	}

	//存入全局控制表
	zHcuL3BfscGenCtrlTable.wsValueNbrFree = resFree;
	zHcuL3BfscGenCtrlTable.wsValueNbrWeight = resWeight;
	zHcuL3BfscGenCtrlTable.wsValueNbrTtt = resTtt;
	zHcuL3BfscGenCtrlTable.wsValueNbrTgu = resTgu;

	//返回
	return;
}

OPSTAT func_l3bfsc_time_out_ws_init_req_process(void)
{
	int ret = 0, i=0;

	//先清掉上一次可能回复的数量和状态
	zHcuL3BfscGenCtrlTable.wsValueNbrFree = 0;
	for (i=0; i<HCU_BFSC_SENSOR_WS_NBR_MAX; i++){
		zHcuL3BfscGenCtrlTable.sensorWs[i].sensorStatus = HCU_L3BFSC_SENSOR_WS_STATUS_INVALID;
	}

	//发送初始化命令给各个传感器
	msg_struct_l3bfsc_can_ws_init_req_t snd;
	memset(&snd, 0, sizeof(msg_struct_l3bfsc_can_ws_init_req_t));
	snd.length = sizeof(msg_struct_l3bfsc_can_ws_init_req_t);
	for (i=0; i<HCU_BFSC_SENSOR_WS_NBR_MAX; i++) snd.wsBitmap[i] = 1;

	ret = hcu_message_send(MSG_ID_L3BFSC_CAN_WS_INIT_REQ, TASK_ID_CANITFLEO, TASK_ID_L3BFSC, &snd, snd.length);
	if (ret == FAILURE){
		HCU_ERROR_PRINT_L3BFSC("L3BFSC: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3BFSC].taskName, zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName);
	}

	//返回
	return SUCCESS;
}

OPSTAT func_l3bfsc_time_out_ttt_wait_fb_process(void)
{
	int ret = 0, i=0;

	//启动周期性定时器
	ret = hcu_timer_start(TASK_ID_L3BFSC, TIMER_ID_1S_L3BFSC_PERIOD_ERROR_SCAN, \
			zHcuSysEngPar.timer.array[TIMER_ID_1S_L3BFSC_PERIOD_ERROR_SCAN].dur, TIMER_TYPE_PERIOD, TIMER_RESOLUTION_1S);
	if (ret == FAILURE){
		HCU_ERROR_PRINT_L3BFSC_RECOVERY("L3BFSC: Error start period timer!\n");
	}

	//发送扫描需求
	msg_struct_l3bfsc_can_error_inq_cmd_req_t snd;
	memset(&snd, 0, sizeof(msg_struct_l3bfsc_can_error_inq_cmd_req_t));
	snd.length = sizeof(msg_struct_l3bfsc_can_error_inq_cmd_req_t);
	snd.sensorid = HCU_BFSC_SENSOR_WS_NBR_MAX;
	for (i=0; i<HCU_BFSC_SENSOR_WS_NBR_MAX; i++) snd.sensorBitmap[i] = 1;

	ret = hcu_message_send(MSG_ID_L3BFSC_CAN_ERROR_INQ_CMD_REQ, TASK_ID_CANITFLEO, TASK_ID_L3BFSC, &snd, snd.length);
	if (ret == FAILURE){
		HCU_ERROR_PRINT_L3BFSC_RECOVERY("L3BFSC: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3BFSC].taskName, zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName);
	}

	//设置新状态
	if (FsmSetState(TASK_ID_L3BFSC, FSM_STATE_L3BFSC_ERROR_INQ) == FAILURE){
		HCU_ERROR_PRINT_L3BFSC_RECOVERY("L3BFSC: Error Set FSM State!\n");
	}

	//返回
	return SUCCESS;
}

OPSTAT func_l3bfsc_time_out_tgu_wait_fb_process(void)
{
	int ret = 0, i=0;

	//启动周期性定时器
	ret = hcu_timer_start(TASK_ID_L3BFSC, TIMER_ID_1S_L3BFSC_PERIOD_ERROR_SCAN, \
			zHcuSysEngPar.timer.array[TIMER_ID_1S_L3BFSC_PERIOD_ERROR_SCAN].dur, TIMER_TYPE_PERIOD, TIMER_RESOLUTION_1S);
	if (ret == FAILURE){
		HCU_ERROR_PRINT_L3BFSC_RECOVERY("L3BFSC: Error start period timer!\n");
	}

	//发送扫描需求
	msg_struct_l3bfsc_can_error_inq_cmd_req_t snd;
	memset(&snd, 0, sizeof(msg_struct_l3bfsc_can_error_inq_cmd_req_t));
	snd.length = sizeof(msg_struct_l3bfsc_can_error_inq_cmd_req_t);
	snd.sensorid = HCU_BFSC_SENSOR_WS_NBR_MAX;
	for (i=0; i<HCU_BFSC_SENSOR_WS_NBR_MAX; i++) snd.sensorBitmap[i] = 1;

	ret = hcu_message_send(MSG_ID_L3BFSC_CAN_ERROR_INQ_CMD_REQ, TASK_ID_CANITFLEO, TASK_ID_L3BFSC, &snd, snd.length);
	if (ret == FAILURE){
		HCU_ERROR_PRINT_L3BFSC_RECOVERY("L3BFSC: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3BFSC].taskName, zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName);
	}

	//设置新状态
	if (FsmSetState(TASK_ID_L3BFSC, FSM_STATE_L3BFSC_ERROR_INQ) == FAILURE){
		HCU_ERROR_PRINT_L3BFSC_RECOVERY("L3BFSC: Error Set FSM State!\n");
	}

	//返回
	return SUCCESS;
}


OPSTAT func_l3bfsc_time_out_error_scan_process(void)
{
	int ret = 0, i=0;

	//发送扫描需求
	msg_struct_l3bfsc_can_error_inq_cmd_req_t snd;
	memset(&snd, 0, sizeof(msg_struct_l3bfsc_can_error_inq_cmd_req_t));
	snd.length = sizeof(msg_struct_l3bfsc_can_error_inq_cmd_req_t);
	snd.sensorid = HCU_BFSC_SENSOR_WS_NBR_MAX;
	for (i=0; i<HCU_BFSC_SENSOR_WS_NBR_MAX; i++) snd.sensorBitmap[i] = 1;

	ret = hcu_message_send(MSG_ID_L3BFSC_CAN_ERROR_INQ_CMD_REQ, TASK_ID_CANITFLEO, TASK_ID_L3BFSC, &snd, snd.length);
	if (ret == FAILURE){
		HCU_ERROR_PRINT_L3BFSC_RECOVERY("L3BFSC: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3BFSC].taskName, zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName);
	}

	//返回
	return SUCCESS;
}

//周期性定时读取
OPSTAT func_l3bfsc_time_out_period_read_process(void)
{
	int ret = 0, i=0;

	//发送读取命令到CAN接口，但并不启动定时器，以简化整个状态机的设置
	msg_struct_l3bfsc_can_ws_read_req_t snd;
	memset(&snd, 0, sizeof(msg_struct_l3bfsc_can_ws_read_req_t));
	snd.length = sizeof(msg_struct_l3bfsc_can_ws_read_req_t);
	snd.sensorid = HCU_BFSC_SENSOR_WS_NBR_MAX;
	for (i=0; i<HCU_BFSC_SENSOR_WS_NBR_MAX; i++) snd.wsBitmap[i] = 1;

	ret = hcu_message_send(MSG_ID_L3BFSC_CAN_WS_READ_REQ, TASK_ID_CANITFLEO, TASK_ID_L3BFSC, &snd, snd.length);
	if (ret == FAILURE){
		HCU_ERROR_PRINT_L3BFSC("L3BFSC: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3BFSC].taskName, zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName);
	}

	//返回
	return SUCCESS;
}

//由于错误，直接从差错中转入扫描状态
//它提供了一种比RESTART更快更低级的方式，让L3状态机直接返回到扫描状态
void func_l3bfsc_stm_main_recovery_from_fault(void)
{
	//控制其它模块进入正常状态

	//状态转移到SLEEP状态
	FsmSetState(TASK_ID_L3BFSC, FSM_STATE_L3BFSC_OOS_SCAN);

	//初始化模块的任务资源
	//初始化定时器：暂时决定不做，除非该模块重新RESTART
	//初始化模块级全局变量：暂时决定不做，除非该模块重新RESTART

	return;
}




