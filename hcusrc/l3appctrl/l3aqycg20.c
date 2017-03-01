/*
 * l3aqycg20.c
 *
 *  Created on: 2015年11月22日
 *      Author: test
 */


#include "l3aqycg20.h"

#include "../l0service/timer.h"
#include "../l0service/trace.h"
#include "../l1com/hwinv.h"
#include "../l1com/l1comdef.h"
#include "../l2frame/cloudvela.h"

/*
** FSM of the L3AQYCG20
*/
HcuFsmStateItem_t HcuFsmL3aqycg20[] =
{
    //MessageId                 //State                   		 		//Function
	//启始点，固定定义，不要改动, 使用ENTRY/END，意味者MSGID肯定不可能在某个高位区段中；考虑到所有任务共享MsgId，即使分段，也无法实现
	//完全是为了给任务一个初始化的机会，按照状态转移机制，该函数不具备启动的机会，因为任务初始化后自动到FSM_STATE_IDLE
	//如果没有必要进行初始化，可以设置为NULL
	{MSG_ID_ENTRY,       						FSM_STATE_ENTRY,            			fsm_l3aqycg20_task_entry}, //Starting

	//System level initialization, only controlled by HCU-MAIN
    {MSG_ID_COM_INIT,       					FSM_STATE_IDLE,            				fsm_l3aqycg20_init},
    {MSG_ID_COM_INIT_FEEDBACK,					FSM_STATE_IDLE,            				fsm_com_do_nothing},

	//Task level initialization
    {MSG_ID_COM_INIT,       					FSM_STATE_L3AQYCG20_INITED,            		fsm_l3aqycg20_init},
    {MSG_ID_COM_INIT_FEEDBACK,					FSM_STATE_L3AQYCG20_INITED,            		fsm_com_do_nothing},

	//ANY state entry
    {MSG_ID_COM_INIT_FEEDBACK,					FSM_STATE_COMMON,          					fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT,       				FSM_STATE_COMMON,          					fsm_com_heart_beat_rcv},
	{MSG_ID_COM_STOP,       					FSM_STATE_COMMON,          					fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT_FB,       			FSM_STATE_COMMON,          					fsm_com_do_nothing},
    {MSG_ID_COM_RESTART,						FSM_STATE_COMMON,            				fsm_l3aqycg20_restart},
	{MSG_ID_COM_TIME_OUT,       				FSM_STATE_COMMON,          					fsm_l3aqycg20_time_out},

    //Normal working status
	//YCJK HUITP L2：发自底层来的请求
	{MSG_ID_CLOUDVELA_YCJK_DATA_REQ,    	FSM_STATE_L3AQYCG20_ACTIVED,      			fsm_l3aqycg20_cloudvela_data_req},
	{MSG_ID_CLOUDVELA_YCJK_DATA_CONFIRM,   	FSM_STATE_L3AQYCG20_ACTIVED,      			fsm_l3aqycg20_cloudvela_data_confirm},
	{MSG_ID_CLOUDVELA_YCJK_CTRL_REQ,    	FSM_STATE_L3AQYCG20_ACTIVED,          		fsm_l3aqycg20_cloudvela_ctrl_req},
	//ZHBHJT212 L2：底层命令
	{MSG_ID_LLCZHB_L3MOD_CTRL_REQ,    		FSM_STATE_L3AQYCG20_ACTIVED,          		fsm_l3aqycg20_llczhb_ctrl_req},

	//等待上层模块的控制反馈
	//YCJK HUITP L3：各个L3模块收集数据
	{MSG_ID_L3AQYC_EXG_CTRL_RESP,    		FSM_STATE_L3AQYCG20_WFFB_CTRL,          	fsm_l3aqycg20_huil3mod_exg_ctrl_resp},
	//ZHBHJT212 L3：从L3模块收集数据
	{MSG_ID_ZHBL3MOD_EXG_CTRL_RESP,    		FSM_STATE_L3AQYCG20_WFFB_CTRL,          	fsm_l3aqycg20_zhbl3mod_exg_ctrl_resp},

	//等待上层模块的控制反馈
	//YCJK HUITP L3：各个L3模块收集数据
	{MSG_ID_L3AQYC_EXG_DATA_REPORT,    		FSM_STATE_L3AQYCG20_WFFB_DATA,          	fsm_l3aqycg20_huil3mod_exg_data_report},
	//ZHBHJT212 L3：从L3模块收集数据
	{MSG_ID_ZHBL3MOD_EXG_DATA_REPORT,    	FSM_STATE_L3AQYCG20_WFFB_DATA,          	fsm_l3aqycg20_zhbl3mod_exg_data_report},

    //结束点，固定定义，不要改动
    {MSG_ID_END,            	FSM_STATE_END,             				NULL},  //Ending
};

//Global variables


//Task Global variables
gTaskL3aqycq20Context_t gTaskL3aqycq20Context; //扬尘监测的总控表


//Main Entry
//Input parameter would be useless, but just for similar structure purpose
OPSTAT fsm_l3aqycg20_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//除了对全局变量进行操作之外，尽量不要做其它操作，因为该函数将被主任务/线程调用，不是本任务/线程调用
	//该API就是给本任务一个提早介入的入口，可以帮着做些测试性操作
	if (FsmSetState(TASK_ID_L3AQYCG20, FSM_STATE_IDLE) == FAILURE){
		HcuErrorPrint("L3AQYCG20: Error Set FSM State at fsm_l3aqycg20_task_entry\n");}
	return SUCCESS;
}

OPSTAT fsm_l3aqycg20_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;

	if ((src_id > TASK_ID_MIN) &&(src_id < TASK_ID_MAX)){
		//Send back MSG_ID_COM_INIT_FEEDBACK to SVRCON
		msg_struct_com_init_feedback_t snd0;
		memset(&snd0, 0, sizeof(msg_struct_com_init_feedback_t));
		snd0.length = sizeof(msg_struct_com_init_feedback_t);

		//to avoid all task send out the init fb msg at the same time which lead to msgque get stuck
		hcu_usleep(dest_id*HCU_SYSCFG_DURATION_OF_INIT_FB_WAIT_MAX);

		ret = hcu_message_send(MSG_ID_COM_INIT_FEEDBACK, src_id, TASK_ID_L3AQYCG20, &snd0, snd0.length);
		if (ret == FAILURE){
			HcuErrorPrint("L3AQYCG20: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3AQYCG20].taskName, zHcuVmCtrTab.task[src_id].taskName);
			return FAILURE;
		}
	}

	//收到初始化消息后，进入初始化状态
	if (FsmSetState(TASK_ID_L3AQYCG20, FSM_STATE_L3AQYCG20_INITED) == FAILURE){
		HcuErrorPrint("L3AQYCG20: Error Set FSM State!\n");
		return FAILURE;
	}

	//初始化硬件接口
	if (func_l3aqycg20_int_init() == FAILURE){
		HcuErrorPrint("L3AQYCG20: Error initialize interface!\n");
		return FAILURE;
	}

	//秤盘数据表单控制表初始化
	memset(&gTaskL3aqycq20Context, 0, sizeof(gTaskL3aqycq20Context_t));

	//Global Variables
	zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20] = 0;

	//启动周期性定时器
	ret = hcu_timer_start(TASK_ID_L3AQYCG20, TIMER_ID_1S_L3AQYCG20_PERIOD_READ, \
			zHcuSysEngPar.timer.array[TIMER_ID_1S_L3AQYCG20_PERIOD_READ].dur, TIMER_TYPE_PERIOD, TIMER_RESOLUTION_1S);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Error start period timer!\n");
		return FAILURE;
	}


	//启动周期性定时器
	ret = hcu_timer_start(TASK_ID_L3AQYCG20, TIMER_ID_1S_L3AQYCG20_PERIOD_STA_SCAN, \
			HCU_L3AQYC_STA_1M_CYCLE, TIMER_TYPE_PERIOD, TIMER_RESOLUTION_1S);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Error start period timer!\n");
		return FAILURE;
	}


	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_L3AQYCG20, FSM_STATE_L3AQYCG20_ACTIVED) == FAILURE)
		HCU_ERROR_PRINT_L3AQYCG20("L3AQYCG20: Error Set FSM State!\n");
	HCU_DEBUG_PRINT_FAT("L3AQYCG20: Enter FSM_STATE_L3AQYCG20_ACTIVED status, Keeping refresh here!\n");

	//返回
	return SUCCESS;
}

OPSTAT fsm_l3aqycg20_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HcuErrorPrint("L3AQYCG20: Internal error counter reach DEAD level, SW-RESTART soon!\n");
	zHcuSysStaPm.statisCnt.restartCnt++;
	fsm_l3aqycg20_init(0, 0, NULL, 0);
	return SUCCESS;
}

OPSTAT func_l3aqycg20_int_init(void)
{
	return SUCCESS;
}

//暂时啥都不干，定时到达后，还不明确是否需要支持定时工作
OPSTAT fsm_l3aqycg20_time_out(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret;

	//Receive message and copy to local variable
	msg_struct_com_time_out_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_com_time_out_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_com_time_out_t))){
		HcuErrorPrint("L3AQYCG20: Receive message error!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//钩子在此处，检查zHcuRunErrCnt[TASK_ID_L3AQYCG20]是否超限
	if (zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20] > HCU_RUN_ERROR_LEVEL_2_MAJOR){
		//减少重复RESTART的概率
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20] = zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20] - HCU_RUN_ERROR_LEVEL_2_MAJOR;
		msg_struct_com_restart_t snd0;
		memset(&snd0, 0, sizeof(msg_struct_com_restart_t));
		snd0.length = sizeof(msg_struct_com_restart_t);
		ret = hcu_message_send(MSG_ID_COM_RESTART, TASK_ID_L3AQYCG20, TASK_ID_L3AQYCG20, &snd0, snd0.length);
		if (ret == FAILURE){
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
			HcuErrorPrint("L3AQYCG20: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3AQYCG20].taskName, zHcuVmCtrTab.task[TASK_ID_L3AQYCG20].taskName);
			return FAILURE;
		}
	}

	//Period time out received
	if ((rcv.timeId == TIMER_ID_1S_L3AQYCG20_PERIOD_READ) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		//保护周期读数的优先级，强制抢占状态，并简化问题
		if (FsmGetState(TASK_ID_L3AQYCG20) != FSM_STATE_L3AQYCG20_ACTIVED){
			ret = FsmSetState(TASK_ID_L3AQYCG20, FSM_STATE_L3AQYCG20_ACTIVED);
			if (ret == FAILURE){
				zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
				HcuErrorPrint("L3AQYCG20: Error Set FSM State!\n");
				return FAILURE;
			}//FsmSetState
		}
	}


	//周期性聚合表生成
	else if ((rcv.timeId == TIMER_ID_1S_L3AQYCG20_PERIOD_STA_SCAN) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){

		//周期性聚合表生成
		if (func_l3aqyc_time_out_aggregation_process() == FAILURE){
			HcuErrorPrint("L3AQYCG20: Error process time out message!\n");
			}

	}



	//命令触发开始启动监测
	else if ((rcv.timeId == TIMER_ID_1S_L3AQYCG20_START_MONITOR) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){

		//监测报告标志开
		gTaskL3aqycq20Context.StartMonitorFlag = TRUE;

		//启动周期性数据报告定时器
		ret = hcu_timer_start(TASK_ID_L3AQYCG20, TIMER_ID_1S_L3AQYCG20_PERIOD_REPORT, HCU_L3AQYC_STA_1M_CYCLE, TIMER_TYPE_PERIOD, TIMER_RESOLUTION_1S);
		if (ret == FAILURE){
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
			HcuErrorPrint("L3AQYCG20: Error start period timer!\n");
			return FAILURE;
		}
	}

	//命令触发停止监测
	else if ((rcv.timeId == TIMER_ID_1S_L3AQYCG20_STOP_MONITOR) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		//监测报告标志关
		gTaskL3aqycq20Context.StartMonitorFlag = FALSE;

	}



	//周期性工作
	else if ((rcv.timeId == TIMER_ID_1S_L3AQYCG20_PERIOD_REPORT) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		//设置分钟，小时，天数据报告标志位为ON,在周期性统计扫描中生成报告后依据标志位是否为ON来确定是否发送，发送后设置相应标志位为OFF
		gTaskL3aqycq20Context.MinReportFlag = TRUE;

	}

	return SUCCESS;
}

//这个模块收到这个命令后，需要跟更多的模块进行交互
OPSTAT fsm_l3aqycg20_cloudvela_data_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret=0;
	msg_struct_cloudvela_ycjk_data_req_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_cloudvela_ycjk_data_req_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_cloudvela_ycjk_data_req_t))){
		HcuErrorPrint("L3AQYCG20: Receive message error!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);



	return SUCCESS;
}

OPSTAT fsm_l3aqycg20_cloudvela_ctrl_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	return SUCCESS;
}

OPSTAT fsm_l3aqycg20_cloudvela_data_confirm(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	return SUCCESS;
}

OPSTAT fsm_l3aqycg20_huil3mod_exg_ctrl_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	return SUCCESS;
}

OPSTAT fsm_l3aqycg20_huil3mod_exg_data_report(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	return SUCCESS;
}

OPSTAT fsm_l3aqycg20_llczhb_ctrl_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;
	msg_struct_llczhb_l3mod_ctrl_req_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_llczhb_l3mod_ctrl_req_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_llczhb_l3mod_ctrl_req_t)))
		HCU_ERROR_PRINT_L3AQYCG20("AQYC: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);

	UINT32 timeCurrent;

	switch (rcv.actionId){
	case HCU_SYSMSG_ZHBHJT_ACTION_EXECUTE_FINISH_9012:
	break;

	case HCU_SYSMSG_ZHBHJT_ACTION_DATA_CFM_9014:
		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_SET_PSWD_1072:
		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_GET_FIELD_TIME_1011:
		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_SET_FIELD_TIME_1012:
		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_GET_POL_RTD_2011:
		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_STOP_RTD_2012:
		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_GET_EQU_RUN_2021:
		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_STOP_EQU_RUN_2022:
		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_GET_POL_MIN_RPT_2051:
		//取得开始和结束时间，计算开始和结束时间和当前时间的差值，并同时起采集数据起始时间和结束时间的定时器，数据起始timeout后启动一分钟周期定时器来周期读取并上传分钟数据（此处可设Flag），
		//数据从聚合表中获得，聚合表由内部定义Timer定时从数据库中取数据计算（暂定一分钟），为简化设计，可依据Flag值来判断是否上传发送
		timeCurrent = time(0);
		HcuErrorPrint("L3AQYCG20: BeginTime=%d  EndTime=%d at min report request message,current Time=%d !\n\n",rcv.dl2Self.gapTime.BeginTime, rcv.dl2Self.gapTime.EndTime,timeCurrent);
		gTaskL3aqycq20Context.timeBegin = rcv.dl2Self.gapTime.BeginTime - timeCurrent;
		gTaskL3aqycq20Context.timeEnd = rcv.dl2Self.gapTime.EndTime - timeCurrent;

		//for test
		gTaskL3aqycq20Context.timeBegin = 5;
		gTaskL3aqycq20Context.timeEnd  = 10;

		if(gTaskL3aqycq20Context.timeBegin < 0 || gTaskL3aqycq20Context.timeEnd < 0)
			HcuErrorPrint("L3AQYCG20: Receive backward BeginTime=%d or EndTime=%d at min report request message !\n\n",rcv.dl2Self.gapTime.BeginTime, rcv.dl2Self.gapTime.EndTime);
		else
			HCU_DEBUG_PRINT_INF("L3AQYCG20: Receive 2051 CMD, get pollution date will start from BeginTime=%d, stop at EndTime=%d\n\n", gTaskL3aqycq20Context.timeBegin,  gTaskL3aqycq20Context.timeEnd);

		//启动数据采集定时器
		ret = hcu_timer_start(TASK_ID_L3AQYCG20, TIMER_ID_1S_L3AQYCG20_START_MONITOR, gTaskL3aqycq20Context.timeBegin, TIMER_TYPE_ONE_TIME, TIMER_RESOLUTION_1S);
		if (ret == FAILURE){
			HCU_ERROR_PRINT_L3AQYCG20("L3AQYCG20: Error start timer!\n");
		}


		//停止数据采集定时器
		ret = hcu_timer_start(TASK_ID_L3AQYCG20, TIMER_ID_1S_L3AQYCG20_STOP_MONITOR, gTaskL3aqycq20Context.timeEnd, TIMER_TYPE_ONE_TIME, TIMER_RESOLUTION_1S);
		if (ret == FAILURE){
			HCU_ERROR_PRINT_L3AQYCG20("L3AQYCG20: Error start timer!\n");
		}

		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_GET_POL_DAY_RPT_2031:
		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_GET_POL_RT_2041:
		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_GET_POL_ALA_2071:
		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_SND_ALM_EVT_2072:
		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_SET_ALMLIM_1022:
		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_GET_ALMLIM_1021:
		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_SET_MNADDR_1032:
		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_GET_MNADDR_1031:
		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_SET_RPT_TIME_1042:
		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_GET_RPT_TIME_1041:
		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_SET_ZERO_3011:
		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_SET_RTDI_1062:
		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_GET_RTDI_1061:
		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_SET_OT_RC_1000:
		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_SET_WARN_TIME_1001:
		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_SET_INST_SAMPLE_3012:
		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_SET_SAMPLE_CTIME_3014:
		break;

	default:
		HCU_ERROR_PRINT_L3AQYCG20("L3AQYCG20: Error action received!\n");
		break;
	}

	//设置状态机到目标状态
	//根据情况，决定是希望等待控制命令的反馈，还是数据汇报的反馈。如果不需要对方反馈，也可以保持在ACTIVE状态
	//设置多状态，就是为了更好的控制数据与控制命令的反馈。目前各种控制命令都聚合在一起，必须通过状态机将情况很清楚，不然很容易漏掉某些组合情况。
	/*
	if (FsmSetState(TASK_ID_L3AQYCG20, FSM_STATE_L3AQYCG20_WFFB_CTRL) == FAILURE)
		HCU_ERROR_PRINT_L3AQYCG20("L3AQYCG20: Error Set FSM State!\n");
		*/

	return SUCCESS;
}

OPSTAT fsm_l3aqycg20_zhbl3mod_exg_ctrl_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//根据不同的命令，判定是否需要再次接受到反馈，从而决定状态是直接转移到ACTIVE，还是继续留在WFFB
	//根据发送控制命令的多寡，可能需要判定是否接收齐活了所有传感器的反馈
	//注意启动和停止定时器

	//收集齐活了，发送MSG_ID_L3MOD_LLCZHB_CTRL_RESP给LLCZHB模块
	//因为是内部消息，所有内容一次性全部发给LLCZHB模块，让其决定是否分段。L3MOD不考虑数据分段问题，只控制并管理所有传感器的数据完整性

	//状态转移


	return SUCCESS;
}

OPSTAT fsm_l3aqycg20_zhbl3mod_exg_data_report(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//根据不同的命令，判定是否需要再次接受到反馈，从而决定状态是直接转移到ACTIVE，还是继续留在WFFB
	//根据发送控制命令的多寡，可能需要判定是否接收齐活了所有传感器的反馈
	//注意启动和停止定时器

	//收集齐活了，发送MSG_ID_L3MOD_LLCZHB_DATA_REPORT给LLCZHB模块
	//因为是内部消息，所有内容一次性全部发给LLCZHB模块，让其决定是否分段。L3MOD不考虑数据分段问题，只控制并管理所有传感器的数据完整性

	//状态转移

	return SUCCESS;
}


//周期性统计扫描定时器
OPSTAT func_l3aqyc_time_out_aggregation_process(void)
{


	//从各个传感器DB中取数据生成数据聚合表
	//统计分钟聚合表（avg,max,min）
	//1min or 1hour or 1day 数据聚合表，聚合表存数据库，再依据标志为判断是否发送

	int ret = 0;


	ret = dbi_HcuPm25DataInfo_GetMin(50000,&gTaskL3aqycq20Context.sta60Min);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get min data from PM25 database!\n");
	}
	else{
		HCU_DEBUG_PRINT_INF("L3AQYCG20 get minimum data = %d\n\n", gTaskL3aqycq20Context.sta60Min.a34001_Min);
	}

	ret = dbi_HcuPm25DataInfo_GetMax(50000,&gTaskL3aqycq20Context.sta60Min);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get max data from PM25 database!\n");
	}
	else{
		HCU_DEBUG_PRINT_INF("L3AQYCG20 get maximum data = %d\n\n", gTaskL3aqycq20Context.sta60Min.a34001_Max);
	}

	ret = dbi_HcuPm25DataInfo_GetAvg(50000,&gTaskL3aqycq20Context.sta60Min);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get avg data from PM25 database!\n");
	}
	else{
		HCU_DEBUG_PRINT_INF("L3AQYCG20 get average data = %d\n\n", gTaskL3aqycq20Context.sta60Min.a34001_Avg);
	}

	//首先增加时间流逝的计数器
	gTaskL3aqycq20Context.elipseCnt++;
	gTaskL3aqycq20Context.elipse24HourCnt++;

	//从各个数据库表单中获取最近值，可依据timeframe判断数据有效性
	//温度
	gTaskL3aqycq20Context.cur.a01001_PolId = 95;
	gTaskL3aqycq20Context.cur.a01001_Cou = 10.01;
	gTaskL3aqycq20Context.cur.a01001_Avg = 10.01;
	gTaskL3aqycq20Context.cur.a01001_Min = 10.01;
	gTaskL3aqycq20Context.cur.a01001_Max = 10.01;
	//gTaskL3aqycq20Context.staOneMin.a01001_Flag = 'N';
	//湿度
	gTaskL3aqycq20Context.cur.a01002_PolId = 96;
	gTaskL3aqycq20Context.cur.a01002_Cou = 10.02;
	gTaskL3aqycq20Context.cur.a01002_Avg = 10.02;
	gTaskL3aqycq20Context.cur.a01002_Min = 10.02;
	gTaskL3aqycq20Context.cur.a01002_Max = 10.02;
	//gTaskL3aqycq20Context.staOneMin.a01002_Flag = 'N';
	//气压
	gTaskL3aqycq20Context.cur.a01006_PolId = 97;
	gTaskL3aqycq20Context.cur.a01006_Cou = 0;
	gTaskL3aqycq20Context.cur.a01006_Avg = 0;
	gTaskL3aqycq20Context.cur.a01006_Min = 0;
	gTaskL3aqycq20Context.cur.a01006_Max = 0;
	//gTaskL3aqycq20Context.staOneMin.a01006_Flag = 'D';
	//风速
	gTaskL3aqycq20Context.cur.a01007_PolId = 98;
	gTaskL3aqycq20Context.cur.a01007_Cou = 10.07;
	gTaskL3aqycq20Context.cur.a01007_Avg = 10.07;
	gTaskL3aqycq20Context.cur.a01007_Min = 10.07;
	gTaskL3aqycq20Context.cur.a01007_Max = 10.07;
	//gTaskL3aqycq20Context.staOneMin.a01007_Flag = 'N';
	//风向
	gTaskL3aqycq20Context.cur.a01008_PolId = 99;
	gTaskL3aqycq20Context.cur.a01008_Cou = 10.08;
	gTaskL3aqycq20Context.cur.a01008_Avg = 10.08;
	gTaskL3aqycq20Context.cur.a01008_Min = 10.08;
	gTaskL3aqycq20Context.cur.a01008_Max = 10.08;
	//gTaskL3aqycq20Context.staOneMin.a01008_Flag = 'N';
	//总悬浮物颗粒
	gTaskL3aqycq20Context.cur.a34001_PolId = 100;
	gTaskL3aqycq20Context.cur.a34001_Cou = 34.001;
	gTaskL3aqycq20Context.cur.a34001_Avg = 34.001;
	gTaskL3aqycq20Context.cur.a34001_Min = 34.001;
	gTaskL3aqycq20Context.cur.a34001_Max = 34.001;
	//gTaskL3aqycq20Context.staOneMin.a34001_Flag = 'N';
	//噪声
	gTaskL3aqycq20Context.cur.a50001_PolId = 101;
	gTaskL3aqycq20Context.cur.a50001_Cou = 50.001;
	gTaskL3aqycq20Context.cur.a50001_Avg = 50.001;
	gTaskL3aqycq20Context.cur.a50001_Min = 50.001;
	gTaskL3aqycq20Context.cur.a50001_Max = 50.001;
	//gTaskL3aqycq20Context.staOneMin.a50001_Flag = 'N';

	//更新1Min各个统计表单，因为传感器采集目前最小粒度是一分钟，所以直接赋当前读出值
	//温度
	gTaskL3aqycq20Context.staOneMin.a01001_PolId = gTaskL3aqycq20Context.cur.a01001_PolId;
	gTaskL3aqycq20Context.staOneMin.a01001_Cou = gTaskL3aqycq20Context.cur.a01001_Cou;
	gTaskL3aqycq20Context.staOneMin.a01001_Avg = gTaskL3aqycq20Context.cur.a01001_Avg;
	gTaskL3aqycq20Context.staOneMin.a01001_Min = gTaskL3aqycq20Context.cur.a01001_Min;
	gTaskL3aqycq20Context.staOneMin.a01001_Max = gTaskL3aqycq20Context.cur.a01001_Max;


	//湿度
	gTaskL3aqycq20Context.staOneMin.a01002_PolId = gTaskL3aqycq20Context.cur.a01002_PolId;
	gTaskL3aqycq20Context.staOneMin.a01002_Cou = gTaskL3aqycq20Context.cur.a01002_Cou ;
	gTaskL3aqycq20Context.staOneMin.a01002_Avg = gTaskL3aqycq20Context.cur.a01002_Avg ;
	gTaskL3aqycq20Context.staOneMin.a01002_Min = gTaskL3aqycq20Context.cur.a01002_Min;
	gTaskL3aqycq20Context.staOneMin.a01002_Max = gTaskL3aqycq20Context.cur.a01002_Max;

	//气压
	gTaskL3aqycq20Context.staOneMin.a01006_PolId = gTaskL3aqycq20Context.cur.a01006_PolId;
	gTaskL3aqycq20Context.staOneMin.a01006_Cou = gTaskL3aqycq20Context.cur.a01006_Cou;
	gTaskL3aqycq20Context.staOneMin.a01006_Avg = gTaskL3aqycq20Context.cur.a01006_Avg;
	gTaskL3aqycq20Context.staOneMin.a01006_Min = gTaskL3aqycq20Context.cur.a01006_Min;
	gTaskL3aqycq20Context.staOneMin.a01006_Max = gTaskL3aqycq20Context.cur.a01006_Max;

	//风速
	gTaskL3aqycq20Context.staOneMin.a01007_PolId = gTaskL3aqycq20Context.cur.a01007_PolId;
	gTaskL3aqycq20Context.staOneMin.a01007_Cou = gTaskL3aqycq20Context.cur.a01007_Cou;
	gTaskL3aqycq20Context.staOneMin.a01007_Avg = gTaskL3aqycq20Context.cur.a01007_Avg;
	gTaskL3aqycq20Context.staOneMin.a01007_Min = gTaskL3aqycq20Context.cur.a01007_Min;
	gTaskL3aqycq20Context.staOneMin.a01007_Max = gTaskL3aqycq20Context.cur.a01007_Max;

	//风向
	gTaskL3aqycq20Context.staOneMin.a01008_PolId = gTaskL3aqycq20Context.cur.a01008_PolId;
	gTaskL3aqycq20Context.staOneMin.a01008_Cou = gTaskL3aqycq20Context.cur.a01008_Cou;
	gTaskL3aqycq20Context.staOneMin.a01008_Avg = gTaskL3aqycq20Context.cur.a01008_Avg;
	gTaskL3aqycq20Context.staOneMin.a01008_Min = gTaskL3aqycq20Context.cur.a01008_Min;
	gTaskL3aqycq20Context.staOneMin.a01008_Max = gTaskL3aqycq20Context.cur.a01008_Max;

	//总悬浮物颗粒
	gTaskL3aqycq20Context.staOneMin.a34001_PolId = gTaskL3aqycq20Context.cur.a34001_PolId;
	gTaskL3aqycq20Context.staOneMin.a34001_Cou = gTaskL3aqycq20Context.cur.a34001_Cou;
	gTaskL3aqycq20Context.staOneMin.a34001_Avg = gTaskL3aqycq20Context.cur.a34001_Avg;
	gTaskL3aqycq20Context.staOneMin.a34001_Min = gTaskL3aqycq20Context.cur.a34001_Min;
	gTaskL3aqycq20Context.staOneMin.a34001_Max = gTaskL3aqycq20Context.cur.a34001_Max;

	//噪声
	gTaskL3aqycq20Context.staOneMin.a50001_PolId = gTaskL3aqycq20Context.cur.a50001_PolId;
	gTaskL3aqycq20Context.staOneMin.a50001_Cou = gTaskL3aqycq20Context.cur.a50001_Cou;
	gTaskL3aqycq20Context.staOneMin.a50001_Avg = gTaskL3aqycq20Context.cur.a50001_Avg;
	gTaskL3aqycq20Context.staOneMin.a50001_Min = gTaskL3aqycq20Context.cur.a50001_Min;
	gTaskL3aqycq20Context.staOneMin.a50001_Max = gTaskL3aqycq20Context.cur.a50001_Max;


	//分钟统计表更新数据库
	//if (dbi_Hcu_StaDatainfo_save(HCU_L3AQYC_STA_DBI_TABLE_1MIN, &(gTaskL3aqycq20Context.staOneMin)) == FAILURE)
			//HCU_ERROR_PRINT_L3BFSC("L3AQYC: Save data to DB error!\n");

	//再发送统计报告
	if(TRUE == gTaskL3aqycq20Context.MinReportFlag)
	{
		msg_struct_l3mod_llczhb_data_report_t snd;
		memset(&snd, 0, sizeof(msg_struct_l3mod_llczhb_data_report_t));
		snd.actionId = HCU_SYSMSG_ZHBHJT_ACTION_GET_POL_MIN_RPT_2051;
		snd.ul2Cloud.DataTime = time(0);
		snd.ul2Cloud.nbrOfCMinRpt = HCU_SYSMSG_ZHBHJT_POLID_NBR_MAX;

		memcpy(&(snd.ul2Cloud.min), &(gTaskL3aqycq20Context.staOneMin), sizeof(msgie_struct_zhbhjt_frame_data_pol_min_hour_t)*HCU_SYSMSG_ZHBHJT_POLID_NBR_MAX);

		snd.length = sizeof(msg_struct_l3mod_llczhb_data_report_t);

		ret = hcu_message_send(MSG_ID_L3MOD_LLCZHB_DATA_REPORT, TASK_ID_LLCZHB, TASK_ID_L3AQYCG20, &snd, snd.length);
		if (ret == FAILURE){
			HCU_ERROR_PRINT_L3AQYCG20("L3AQYCG20: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3AQYCG20].taskName, zHcuVmCtrTab.task[TASK_ID_LLCZHB].taskName);
		}

	}


	//更新1hour各个统计表单
	//温度
	gTaskL3aqycq20Context.sta60Min.a01001_PolId = gTaskL3aqycq20Context.cur.a01001_PolId;
	gTaskL3aqycq20Context.sta60Min.a01001_Avg = gTaskL3aqycq20Context.cur.a01001_Avg;
	gTaskL3aqycq20Context.sta60Min.a01001_Min = gTaskL3aqycq20Context.cur.a01001_Min;
	gTaskL3aqycq20Context.sta60Min.a01001_Max = gTaskL3aqycq20Context.cur.a01001_Max;
	//strncpy(gTaskL3aqycq20Context.staOneMin.a01001_Flag, "N", strlen(gTaskL3aqycq20Context.staOneMin.a01001_Flag));
	//湿度
	gTaskL3aqycq20Context.sta60Min.a01002_PolId = gTaskL3aqycq20Context.cur.a01002_PolId;
	gTaskL3aqycq20Context.sta60Min.a01002_Avg = gTaskL3aqycq20Context.cur.a01002_Avg ;
	gTaskL3aqycq20Context.sta60Min.a01002_Min = gTaskL3aqycq20Context.cur.a01002_Min;
	gTaskL3aqycq20Context.sta60Min.a01002_Max = gTaskL3aqycq20Context.cur.a01002_Max;
	//strncpy(gTaskL3aqycq20Context.staOneMin.a01002_Flag, "N", strlen(gTaskL3aqycq20Context.staOneMin.a01002_Flag));
	//气压
	gTaskL3aqycq20Context.sta60Min.a01006_PolId = gTaskL3aqycq20Context.cur.a01006_PolId;
	gTaskL3aqycq20Context.sta60Min.a01006_Avg = gTaskL3aqycq20Context.cur.a01006_Avg;
	gTaskL3aqycq20Context.sta60Min.a01006_Min = gTaskL3aqycq20Context.cur.a01006_Min;
	gTaskL3aqycq20Context.sta60Min.a01006_Max = gTaskL3aqycq20Context.cur.a01006_Max;
	//strncpy(gTaskL3aqycq20Context.staOneMin.a01006_Flag, "N", strlen(gTaskL3aqycq20Context.staOneMin.a01006_Flag));
	//风速
	gTaskL3aqycq20Context.sta60Min.a01007_PolId = gTaskL3aqycq20Context.cur.a01007_PolId;
	gTaskL3aqycq20Context.sta60Min.a01007_Avg = gTaskL3aqycq20Context.cur.a01007_Avg;
	gTaskL3aqycq20Context.sta60Min.a01007_Min = gTaskL3aqycq20Context.cur.a01007_Min;
	gTaskL3aqycq20Context.sta60Min.a01007_Max = gTaskL3aqycq20Context.cur.a01007_Max;
	//strncpy(gTaskL3aqycq20Context.staOneMin.a01007_Flag, "N", strlen(gTaskL3aqycq20Context.staOneMin.a01007_Flag));
	//风向
	gTaskL3aqycq20Context.sta60Min.a01008_PolId = gTaskL3aqycq20Context.cur.a01008_PolId;

	gTaskL3aqycq20Context.sta60Min.a01008_Avg = gTaskL3aqycq20Context.cur.a01008_Avg;
	gTaskL3aqycq20Context.sta60Min.a01008_Min = gTaskL3aqycq20Context.cur.a01008_Min;
	gTaskL3aqycq20Context.sta60Min.a01008_Max = gTaskL3aqycq20Context.cur.a01008_Max;
	//strncpy(gTaskL3aqycq20Context.staOneMin.a01008_Flag, "N", strlen(gTaskL3aqycq20Context.staOneMin.a01008_Flag));
	//总悬浮物颗粒
	gTaskL3aqycq20Context.sta60Min.a34001_PolId = gTaskL3aqycq20Context.cur.a34001_PolId;
	gTaskL3aqycq20Context.sta60Min.a34001_Avg = gTaskL3aqycq20Context.cur.a34001_Avg;
	gTaskL3aqycq20Context.sta60Min.a34001_Min = gTaskL3aqycq20Context.cur.a34001_Min;
	gTaskL3aqycq20Context.sta60Min.a34001_Max = gTaskL3aqycq20Context.cur.a34001_Max;
	//strncpy(gTaskL3aqycq20Context.staOneMin.a34001_Flag, "N", strlen(gTaskL3aqycq20Context.staOneMin.a34001_Flag));
	//噪声
	gTaskL3aqycq20Context.sta60Min.a50001_PolId = gTaskL3aqycq20Context.cur.a50001_PolId;
	gTaskL3aqycq20Context.sta60Min.a50001_Avg = gTaskL3aqycq20Context.cur.a50001_Avg;
	gTaskL3aqycq20Context.sta60Min.a50001_Min = gTaskL3aqycq20Context.cur.a50001_Min;
	gTaskL3aqycq20Context.sta60Min.a50001_Max = gTaskL3aqycq20Context.cur.a50001_Max;
	//strncpy(gTaskL3aqycq20Context.staOneMin.a50001_Flag, "N", strlen(gTaskL3aqycq20Context.staOneMin.a50001_Flag));


	//将当前基础统计周期的数据清零
	memset(&(gTaskL3aqycq20Context.cur), 0, sizeof(HcuSysMsgIeL3aqycContextStaElement_t));

	//返回
	return SUCCESS;
}




