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

	//扬尘数据表单控制表初始化
	memset(&gTaskL3aqycq20Context, 0, sizeof(gTaskL3aqycq20Context_t));

	//监测数据标识符判断表初始化
	func_l3aqycg20_judge_value_init();

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
		if (func_l3aqyc_time_out_aggregation_process() == FAILURE)
			HcuErrorPrint("L3AQYCG20: Error process time out message!\n");

	}

	//触发开始发送分钟报告
	else if ((rcv.timeId == TIMER_ID_1S_L3AQYCG20_START_1MIN_REPORT) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){

		gTaskL3aqycq20Context.MinReportFlag = TRUE;

	}

	//触发停止发送分钟报告
	else if ((rcv.timeId == TIMER_ID_1S_L3AQYCG20_STOP_1MIN_REPORT) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		gTaskL3aqycq20Context.MinReportFlag = FALSE;

	}

	//触发开始发送小时报告
	else if ((rcv.timeId == TIMER_ID_1S_L3AQYCG20_START_1HOUR_REPORT) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){

		gTaskL3aqycq20Context.HourReportFlag = TRUE;

	}

	//触发停止发送报告
	else if ((rcv.timeId == TIMER_ID_1S_L3AQYCG20_STOP_1HOUR_REPORT) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		gTaskL3aqycq20Context.HourReportFlag = FALSE;

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
		gTaskL3aqycq20Context.timeBegin_1Min = rcv.dl2Self.gapTime.BeginTime - timeCurrent;
		gTaskL3aqycq20Context.timeEnd_1Min = rcv.dl2Self.gapTime.EndTime - timeCurrent;

		//for test
		gTaskL3aqycq20Context.timeBegin_1Min = HCU_L3AQYC_STA_1M_REPORT_DURATION;
		gTaskL3aqycq20Context.timeEnd_1Min  = HCU_L3AQYC_STA_1H_REPORT_DURATION;

		if(gTaskL3aqycq20Context.timeBegin_1Min < 0 || gTaskL3aqycq20Context.timeEnd_1Min < 0)
			HcuErrorPrint("L3AQYCG20: Receive backward BeginTime=%d or EndTime=%d at min report request message !\n\n",rcv.dl2Self.gapTime.BeginTime, rcv.dl2Self.gapTime.EndTime);
		else
			HCU_DEBUG_PRINT_INF("L3AQYCG20: Receive 2051 CMD, get pollution date will start from BeginTime=%d, stop at EndTime=%d\n\n", gTaskL3aqycq20Context.timeBegin_1Min,  gTaskL3aqycq20Context.timeEnd_1Min);

		//启动分钟报告定时器
		ret = hcu_timer_start(TASK_ID_L3AQYCG20, TIMER_ID_1S_L3AQYCG20_START_1MIN_REPORT, gTaskL3aqycq20Context.timeBegin_1Min, TIMER_TYPE_ONE_TIME, TIMER_RESOLUTION_1S);
		if (ret == FAILURE){
			HCU_ERROR_PRINT_L3AQYCG20("L3AQYCG20: Error start timer!\n");
		}

		//停止分钟报告定时器
		ret = hcu_timer_start(TASK_ID_L3AQYCG20, TIMER_ID_1S_L3AQYCG20_STOP_1MIN_REPORT, gTaskL3aqycq20Context.timeEnd_1Min, TIMER_TYPE_ONE_TIME, TIMER_RESOLUTION_1S);
		if (ret == FAILURE){
			HCU_ERROR_PRINT_L3AQYCG20("L3AQYCG20: Error start timer!\n");
		}

		break;


	case HCU_SYSMSG_ZHBHJT_ACTION_GET_POL_HOUR_RPT_2061:
		//取得开始和结束时间，计算开始和结束时间和当前时间的差值，并同时起采集数据起始时间和结束时间的定时器，数据起始timeout后启动一分钟周期定时器来周期读取并上传分钟数据（此处可设Flag），
		//数据从聚合表中获得，聚合表由内部定义Timer定时从数据库中取数据计算（暂定一分钟），为简化设计，可依据Flag值来判断是否上传发送
		timeCurrent = time(0);
		HcuErrorPrint("L3AQYCG20: BeginTime=%d  EndTime=%d at min report request message,current Time=%d !\n\n",rcv.dl2Self.gapTime.BeginTime, rcv.dl2Self.gapTime.EndTime,timeCurrent);
		gTaskL3aqycq20Context.timeBegin_1Hour = rcv.dl2Self.gapTime.BeginTime - timeCurrent;
		gTaskL3aqycq20Context.timeEnd_1Hour = rcv.dl2Self.gapTime.EndTime - timeCurrent;

		//for test
		gTaskL3aqycq20Context.timeBegin_1Hour = HCU_L3AQYC_STA_1M_REPORT_DURATION;
		gTaskL3aqycq20Context.timeEnd_1Hour  = HCU_L3AQYC_STA_1H_REPORT_DURATION;

		if(gTaskL3aqycq20Context.timeBegin_1Hour < 0 || gTaskL3aqycq20Context.timeEnd_1Hour < 0)
			HcuErrorPrint("L3AQYCG20: Receive backward BeginTime=%d or EndTime=%d at min report request message !\n\n",rcv.dl2Self.gapTime.BeginTime, rcv.dl2Self.gapTime.EndTime);
		else
			HCU_DEBUG_PRINT_INF("L3AQYCG20: Receive 2051 CMD, get pollution date will start from BeginTime=%d, stop at EndTime=%d\n\n", gTaskL3aqycq20Context.timeBegin_1Hour,  gTaskL3aqycq20Context.timeEnd_1Hour);

		//启动分钟报告定时器
		ret = hcu_timer_start(TASK_ID_L3AQYCG20, TIMER_ID_1S_L3AQYCG20_START_1HOUR_REPORT, gTaskL3aqycq20Context.timeBegin_1Hour, TIMER_TYPE_ONE_TIME, TIMER_RESOLUTION_1S);
		if (ret == FAILURE){
			HCU_ERROR_PRINT_L3AQYCG20("L3AQYCG20: Error start timer!\n");
		}

		//停止分钟报告定时器
		ret = hcu_timer_start(TASK_ID_L3AQYCG20, TIMER_ID_1S_L3AQYCG20_STOP_1HOUR_REPORT, gTaskL3aqycq20Context.timeEnd_1Hour, TIMER_TYPE_ONE_TIME, TIMER_RESOLUTION_1S);
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
	/*
	// for test start
	char ds[HCU_SYSMSG_COM_MSG_BODY_LEN_MAX];
	char tmp[ZHBHJT_PROTOCOL_FRAME_SINGLE_SEG_LEN_MAX];
	memset(ds, 0, sizeof(ds));
	strcat(ds, "##QN=20160715025900000;ST=22;CN=2011;PW=123456;MN=SHHBZB1111;CP=&&DataTime=20160715025900;a34001-Avg=0.000,a34001-Max=0.000,a34001-Min=0.000,a34001-Flag=N;a50001-Avg=30.0,a50001-Max=30.0,a50001-Min=30.0,a50001-Flag=N;a01001-Avg=25.3,a01001-Max=25.3,a01001-Min=25.3,a01001-Flag=N;a01002-Avg=88.6,a01002-Max=88.6,a01002-Min=88.6,a01002-Flag=N;a01006-Avg=0.0,a01006-Max=0.0,a01006-Min=0.0,a01006-Flag=N;a01007-Avg=0.0,a01007-Max=0.0,a01007-Min=0.0,a01007-Flag=N;a01008-Avg=348,a01008-Max=348,a01008-Min=348,a01008-Flag=N&&");

	//计算长度
	UINT16 len = strlen(ds);
	UINT16 crc16 = 0;
	hcu_vm_calculate_crc_modbus((UINT8*)ds, len, &crc16);
	memset(tmp, 0, sizeof(tmp));
	sprintf(tmp, "%04X", crc16);
	HCU_DEBUG_PRINT_INF("L3AQYCG20: output=[%s]\n\n\n\n\n", tmp);
	*/

	//时间流逝的计数器
	gTaskL3aqycq20Context.elipseCnt++;


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//总悬浮物颗粒
	gTaskL3aqycq20Context.staOneMin.a34001_PolId = 99;
	gTaskL3aqycq20Context.staOneMin.a34001_Flag = 'N';

	ret = dbi_HcuPm25DataInfo_GetMin(HCU_L3AQYC_STA_5M_REPORT_DURATION,&gTaskL3aqycq20Context.staOneMin);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get min data from PM25 database!\n");
	}
	else{
		HCU_DEBUG_PRINT_INF("L3AQYCG20 get PM10 1M minimum data = %4.3f\n", gTaskL3aqycq20Context.staOneMin.a34001_Min);
	}

	ret = dbi_HcuPm25DataInfo_GetMax(HCU_L3AQYC_STA_5M_REPORT_DURATION,&gTaskL3aqycq20Context.staOneMin);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get max data from PM25 database!\n");
	}
	else{
		HCU_DEBUG_PRINT_INF("L3AQYCG20 get PM10 1M maximum data = %4.3f\n", gTaskL3aqycq20Context.staOneMin.a34001_Max);
	}

	ret = dbi_HcuPm25DataInfo_GetAvg(HCU_L3AQYC_STA_5M_REPORT_DURATION,&gTaskL3aqycq20Context.staOneMin);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get avg data from PM25 database!\n");
	}
	else{
		//gTaskL3aqycq20Context.staOneMin.a34001_Cou = gTaskL3aqycq20Context.staOneMin.a34001_Avg;

		HCU_DEBUG_PRINT_INF("L3AQYCG20 get PM10 1M average data = %4.3f\n", gTaskL3aqycq20Context.staOneMin.a34001_Avg);
	}

	//噪声
	gTaskL3aqycq20Context.staOneMin.a50001_PolId = 100;
	gTaskL3aqycq20Context.staOneMin.a50001_Flag = 'N';

	ret = dbi_HcuNoiseDataInfo_GetMin(HCU_L3AQYC_STA_5M_REPORT_DURATION,&gTaskL3aqycq20Context.staOneMin);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get min data from Noise database!\n");
	}
	else{
		HCU_DEBUG_PRINT_INF("L3AQYCG20 get Noise 1M minimum data = %4.3f\n", gTaskL3aqycq20Context.staOneMin.a50001_Min);
	}

	ret = dbi_HcuNoiseDataInfo_GetMax(HCU_L3AQYC_STA_5M_REPORT_DURATION,&gTaskL3aqycq20Context.staOneMin);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get max data from Noise database!\n");
	}
	else{
		HCU_DEBUG_PRINT_INF("L3AQYCG20 get Noise 1M maximum data = %4.3f\n", gTaskL3aqycq20Context.staOneMin.a50001_Max);
	}

	ret = dbi_HcuNoiseDataInfo_GetAvg(HCU_L3AQYC_STA_5M_REPORT_DURATION,&gTaskL3aqycq20Context.staOneMin);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get avg data from Noise database!\n");
	}
	else{
		//gTaskL3aqycq20Context.staOneMin.a50001_Cou = gTaskL3aqycq20Context.staOneMin.a50001_Avg;

		HCU_DEBUG_PRINT_INF("L3AQYCG20 get Noise 1M average data = %4.3f\n", gTaskL3aqycq20Context.staOneMin.a50001_Avg);
	}


	//温度
	gTaskL3aqycq20Context.staOneMin.a01001_PolId = 94;
	gTaskL3aqycq20Context.staOneMin.a01001_Flag = 'N';

	ret = dbi_HcuTempDataInfo_GetMin(HCU_L3AQYC_STA_5M_REPORT_DURATION,&gTaskL3aqycq20Context.staOneMin);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get min data from Temp database!\n");
	}
	else{
		HCU_DEBUG_PRINT_INF("L3AQYCG20 get Temp 1M minimum data = %4.3f\n", gTaskL3aqycq20Context.staOneMin.a01001_Min);
	}

	ret = dbi_HcuTempDataInfo_GetMax(HCU_L3AQYC_STA_5M_REPORT_DURATION,&gTaskL3aqycq20Context.staOneMin);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get max data from Temp database!\n");
	}
	else{
		HCU_DEBUG_PRINT_INF("L3AQYCG20 get Temp 1M maximum data = %4.3f\n", gTaskL3aqycq20Context.staOneMin.a01001_Max);
	}

	ret = dbi_HcuTempDataInfo_GetAvg(HCU_L3AQYC_STA_5M_REPORT_DURATION,&gTaskL3aqycq20Context.staOneMin);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get avg data from Temp database!\n");
	}
	else{
		//gTaskL3aqycq20Context.staOneMin.a01001_Cou = gTaskL3aqycq20Context.staOneMin.a01001_Avg;

		HCU_DEBUG_PRINT_INF("L3AQYCG20 get Temp 1M average data = %4.3f\n", gTaskL3aqycq20Context.staOneMin.a01001_Avg);
	}


	//湿度
	gTaskL3aqycq20Context.staOneMin.a01002_PolId = 95;
	gTaskL3aqycq20Context.staOneMin.a01002_Flag = 'N';

	ret = dbi_HcuHumidDataInfo_GetMin(HCU_L3AQYC_STA_5M_REPORT_DURATION,&gTaskL3aqycq20Context.staOneMin);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get min data from Humid database!\n");
	}
	else{
		HCU_DEBUG_PRINT_INF("L3AQYCG20 get Humid 1M minimum data = %4.3f\n", gTaskL3aqycq20Context.staOneMin.a01002_Min);
	}

	ret = dbi_HcuHumidDataInfo_GetMax(HCU_L3AQYC_STA_5M_REPORT_DURATION,&gTaskL3aqycq20Context.staOneMin);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get max data from Humid database!\n");
	}
	else{
		HCU_DEBUG_PRINT_INF("L3AQYCG20 get Humid 1M maximum data = %4.3f\n", gTaskL3aqycq20Context.staOneMin.a01002_Max);
	}

	ret = dbi_HcuHumidDataInfo_GetAvg(HCU_L3AQYC_STA_5M_REPORT_DURATION,&gTaskL3aqycq20Context.staOneMin);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get avg data from Humid database!\n");
	}
	else{
		//gTaskL3aqycq20Context.staOneMin.a01002_Cou = gTaskL3aqycq20Context.staOneMin.a01002_Avg;

		HCU_DEBUG_PRINT_INF("L3AQYCG20 get Humid 1M average data = %4.3f\n", gTaskL3aqycq20Context.staOneMin.a01002_Avg);
	}


	//风速
	gTaskL3aqycq20Context.staOneMin.a01007_PolId = 97;
	gTaskL3aqycq20Context.staOneMin.a01007_Flag = 'N';

	ret = dbi_HcuWindspdDataInfo_GetMin(HCU_L3AQYC_STA_5M_REPORT_DURATION,&gTaskL3aqycq20Context.staOneMin);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get min data from Windspd database!\n");
	}
	else{
		HCU_DEBUG_PRINT_INF("L3AQYCG20 get Windspd 1M minimum data = %4.3f\n", gTaskL3aqycq20Context.staOneMin.a01007_Min);
	}

	ret = dbi_HcuWindspdDataInfo_GetMax(HCU_L3AQYC_STA_5M_REPORT_DURATION,&gTaskL3aqycq20Context.staOneMin);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get Windspd max data from Windspd database!\n");
	}
	else{
		HCU_DEBUG_PRINT_INF("L3AQYCG20 get Windspd 1M maximum data = %4.3f\n", gTaskL3aqycq20Context.staOneMin.a01007_Max);
	}

	ret = dbi_HcuWindspdDataInfo_GetAvg(HCU_L3AQYC_STA_5M_REPORT_DURATION,&gTaskL3aqycq20Context.staOneMin);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get avg data from Windspd database!\n");
	}
	else{
		//gTaskL3aqycq20Context.staOneMin.a01007_Cou = gTaskL3aqycq20Context.staOneMin.a01007_Avg;

		HCU_DEBUG_PRINT_INF("L3AQYCG20 get Windspd 1M average data = %4.3f\n", gTaskL3aqycq20Context.staOneMin.a01007_Avg);
	}

	//风向
	gTaskL3aqycq20Context.staOneMin.a01008_PolId = 98;
	gTaskL3aqycq20Context.staOneMin.a01008_Flag = 'N';

	ret = dbi_HcuWinddirDataInfo_GetMin(HCU_L3AQYC_STA_5M_REPORT_DURATION,&gTaskL3aqycq20Context.staOneMin);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get min data from Winddir database!\n");
	}
	else{
		HCU_DEBUG_PRINT_INF("L3AQYCG20 get Winddir 1M minimum data = %4.3f\n", gTaskL3aqycq20Context.staOneMin.a01008_Min);
	}

	ret = dbi_HcuWinddirDataInfo_GetMax(HCU_L3AQYC_STA_5M_REPORT_DURATION,&gTaskL3aqycq20Context.staOneMin);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get max data from Winddir database!\n");
	}
	else{
		HCU_DEBUG_PRINT_INF("L3AQYCG20 get Winddir 1M maximum data = %4.3f\n", gTaskL3aqycq20Context.staOneMin.a01008_Max);
	}

	ret = dbi_HcuWinddirDataInfo_GetAvg(HCU_L3AQYC_STA_5M_REPORT_DURATION,&gTaskL3aqycq20Context.staOneMin);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get avg data from Winddir database!\n");
	}
	else{
		//gTaskL3aqycq20Context.staOneMin.a01008_Cou = gTaskL3aqycq20Context.staOneMin.a01008_Avg;

		HCU_DEBUG_PRINT_INF("L3AQYCG20 get Winddir 1M average data = %4.3f\n", gTaskL3aqycq20Context.staOneMin.a01008_Avg);
	}

	//气压
	gTaskL3aqycq20Context.staOneMin.a01006_PolId = 96;
	//gTaskL3aqycq20Context.staOneMin.a01006_Cou = 0;
	gTaskL3aqycq20Context.staOneMin.a01006_Avg = 0;
	gTaskL3aqycq20Context.staOneMin.a01006_Min = 0;
	gTaskL3aqycq20Context.staOneMin.a01006_Max = 0;
	gTaskL3aqycq20Context.staOneMin.a01006_Flag = 'D';

	func_l3aqyc_measurement_value_flag_judge(&gTaskL3aqycq20Context.staOneMin);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////



	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//总悬浮物颗粒
	gTaskL3aqycq20Context.sta60Min.a34001_PolId = 99;
	gTaskL3aqycq20Context.sta60Min.a34001_Flag = 'N';

	ret = dbi_HcuPm25DataInfo_GetMin(HCU_L3AQYC_STA_1H_REPORT_DURATION,&gTaskL3aqycq20Context.sta60Min);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get min data from PM25 database!\n");
	}
	else{
		HCU_DEBUG_PRINT_INF("L3AQYCG20 get PM10 Temp 1H minimum data = %4.3f\n", gTaskL3aqycq20Context.sta60Min.a34001_Min);
	}

	ret = dbi_HcuPm25DataInfo_GetMax(HCU_L3AQYC_STA_1H_REPORT_DURATION,&gTaskL3aqycq20Context.sta60Min);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get max data from PM25 database!\n");
	}
	else{
		HCU_DEBUG_PRINT_INF("L3AQYCG20 get PM10 Temp 1H maximum data = %4.3f\n", gTaskL3aqycq20Context.sta60Min.a34001_Max);
	}

	ret = dbi_HcuPm25DataInfo_GetAvg(HCU_L3AQYC_STA_1H_REPORT_DURATION,&gTaskL3aqycq20Context.sta60Min);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get avg data from PM25 database!\n");
	}
	else{
		//gTaskL3aqycq20Context.staOneMin.a34001_Cou = gTaskL3aqycq20Context.staOneMin.a34001_Avg;

		HCU_DEBUG_PRINT_INF("L3AQYCG20 get PM10 Temp 1H average data = %4.3f\n", gTaskL3aqycq20Context.sta60Min.a34001_Avg);
	}

	//噪声
	gTaskL3aqycq20Context.sta60Min.a50001_PolId = 100;
	gTaskL3aqycq20Context.sta60Min.a50001_Flag = 'N';

	ret = dbi_HcuNoiseDataInfo_GetMin(HCU_L3AQYC_STA_1H_REPORT_DURATION,&gTaskL3aqycq20Context.sta60Min);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get min data from Noise database!\n");
	}
	else{
		HCU_DEBUG_PRINT_INF("L3AQYCG20 get Noise Temp 1H minimum data = %4.3f\n", gTaskL3aqycq20Context.sta60Min.a50001_Min);
	}

	ret = dbi_HcuNoiseDataInfo_GetMax(HCU_L3AQYC_STA_1H_REPORT_DURATION,&gTaskL3aqycq20Context.sta60Min);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get max data from Noise database!\n");
	}
	else{
		HCU_DEBUG_PRINT_INF("L3AQYCG20 get Noise 1H maximum data = %4.3f\n", gTaskL3aqycq20Context.sta60Min.a50001_Max);
	}

	ret = dbi_HcuNoiseDataInfo_GetAvg(HCU_L3AQYC_STA_1H_REPORT_DURATION,&gTaskL3aqycq20Context.sta60Min);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get avg data from Noise database!\n");
	}
	else{
		//gTaskL3aqycq20Context.staOneMin.a50001_Cou = gTaskL3aqycq20Context.staOneMin.a50001_Avg;

		HCU_DEBUG_PRINT_INF("L3AQYCG20 get Noise 1H average data = %4.3f\n", gTaskL3aqycq20Context.sta60Min.a50001_Avg);
	}


	//温度
	gTaskL3aqycq20Context.sta60Min.a01001_PolId = 94;
	gTaskL3aqycq20Context.sta60Min.a01001_Flag = 'N';

	ret = dbi_HcuTempDataInfo_GetMin(HCU_L3AQYC_STA_1H_REPORT_DURATION,&gTaskL3aqycq20Context.sta60Min);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get min data from Temp database!\n");
	}
	else{
		HCU_DEBUG_PRINT_INF("L3AQYCG20 get Temp 1H minimum data = %4.3f\n", gTaskL3aqycq20Context.sta60Min.a01001_Min);
	}

	ret = dbi_HcuTempDataInfo_GetMax(HCU_L3AQYC_STA_1H_REPORT_DURATION,&gTaskL3aqycq20Context.sta60Min);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get max data from Temp database!\n");
	}
	else{
		HCU_DEBUG_PRINT_INF("L3AQYCG20 get Temp 1H maximum data = %4.3f\n", gTaskL3aqycq20Context.sta60Min.a01001_Max);
	}

	ret = dbi_HcuTempDataInfo_GetAvg(HCU_L3AQYC_STA_1H_REPORT_DURATION,&gTaskL3aqycq20Context.sta60Min);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get avg data from Temp database!\n");
	}
	else{
		//gTaskL3aqycq20Context.staOneMin.a01001_Cou = gTaskL3aqycq20Context.staOneMin.a01001_Avg;

		HCU_DEBUG_PRINT_INF("L3AQYCG20 get Temp 1H average data = %4.3f\n", gTaskL3aqycq20Context.sta60Min.a01001_Avg);
	}


	//湿度
	gTaskL3aqycq20Context.sta60Min.a01002_PolId = 95;
	gTaskL3aqycq20Context.sta60Min.a01002_Flag = 'N';

	ret = dbi_HcuHumidDataInfo_GetMin(HCU_L3AQYC_STA_1H_REPORT_DURATION,&gTaskL3aqycq20Context.sta60Min);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get min data from Humid database!\n");
	}
	else{
		HCU_DEBUG_PRINT_INF("L3AQYCG20 get Humid 1H minimum data = %4.3f\n", gTaskL3aqycq20Context.sta60Min.a01002_Min);
	}

	ret = dbi_HcuHumidDataInfo_GetMax(HCU_L3AQYC_STA_1H_REPORT_DURATION,&gTaskL3aqycq20Context.sta60Min);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get max data from Humid database!\n");
	}
	else{
		HCU_DEBUG_PRINT_INF("L3AQYCG20 get Humid 1H maximum data = %4.3f\n", gTaskL3aqycq20Context.sta60Min.a01002_Max);
	}

	ret = dbi_HcuHumidDataInfo_GetAvg(HCU_L3AQYC_STA_1H_REPORT_DURATION,&gTaskL3aqycq20Context.sta60Min);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get avg data from Humid database!\n");
	}
	else{
		//gTaskL3aqycq20Context.staOneMin.a01002_Cou = gTaskL3aqycq20Context.staOneMin.a01002_Avg;

		HCU_DEBUG_PRINT_INF("L3AQYCG20 get Humid 1H average data = %4.3f\n", gTaskL3aqycq20Context.sta60Min.a01002_Avg);
	}


	//风速
	gTaskL3aqycq20Context.sta60Min.a01007_PolId = 97;
	gTaskL3aqycq20Context.sta60Min.a01007_Flag = 'N';

	ret = dbi_HcuWindspdDataInfo_GetMin(HCU_L3AQYC_STA_1H_REPORT_DURATION,&gTaskL3aqycq20Context.sta60Min);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get min data from Windspd database!\n");
	}
	else{
		HCU_DEBUG_PRINT_INF("L3AQYCG20 get Windspd 1H minimum data = %4.3f\n", gTaskL3aqycq20Context.sta60Min.a01007_Min);
	}

	ret = dbi_HcuWindspdDataInfo_GetMax(HCU_L3AQYC_STA_1H_REPORT_DURATION,&gTaskL3aqycq20Context.sta60Min);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get Windspd max data from Windspd database!\n");
	}
	else{
		HCU_DEBUG_PRINT_INF("L3AQYCG20 get maximum 1H data = %4.3f\n", gTaskL3aqycq20Context.sta60Min.a01007_Max);
	}

	ret = dbi_HcuWindspdDataInfo_GetAvg(HCU_L3AQYC_STA_1H_REPORT_DURATION,&gTaskL3aqycq20Context.sta60Min);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get avg data from Windspd database!\n");
	}
	else{
		//gTaskL3aqycq20Context.staOneMin.a01007_Cou = gTaskL3aqycq20Context.staOneMin.a01007_Avg;

		HCU_DEBUG_PRINT_INF("L3AQYCG20 get Windspd 1H average data = %4.3f\n", gTaskL3aqycq20Context.sta60Min.a01007_Avg);
	}

	//风向
	gTaskL3aqycq20Context.sta60Min.a01008_PolId = 98;
	gTaskL3aqycq20Context.sta60Min.a01008_Flag = 'N';

	ret = dbi_HcuWinddirDataInfo_GetMin(HCU_L3AQYC_STA_1H_REPORT_DURATION,&gTaskL3aqycq20Context.sta60Min);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get min data from Winddir database!\n");
	}
	else{
		HCU_DEBUG_PRINT_INF("L3AQYCG20 get Winddir 1H minimum data = %4.3f\n", gTaskL3aqycq20Context.sta60Min.a01008_Min);
	}

	ret = dbi_HcuWinddirDataInfo_GetMax(HCU_L3AQYC_STA_1H_REPORT_DURATION,&gTaskL3aqycq20Context.sta60Min);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get max data from Winddir database!\n");
	}
	else{
		HCU_DEBUG_PRINT_INF("L3AQYCG20 get Winddir 1H maximum data = %4.3f\n", gTaskL3aqycq20Context.sta60Min.a01008_Max);
	}

	ret = dbi_HcuWinddirDataInfo_GetAvg(HCU_L3AQYC_STA_1H_REPORT_DURATION,&gTaskL3aqycq20Context.sta60Min);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get avg data from Winddir database!\n");
	}
	else{
		//gTaskL3aqycq20Context.staOneMin.a01008_Cou = gTaskL3aqycq20Context.staOneMin.a01008_Avg;

		HCU_DEBUG_PRINT_INF("L3AQYCG20 get Winddir 1H average data = %4.3f\n", gTaskL3aqycq20Context.sta60Min.a01008_Avg);
	}

	//气压
	gTaskL3aqycq20Context.sta60Min.a01006_PolId = 96;
	//gTaskL3aqycq20Context.staOneMin.a01006_Cou = 0;
	gTaskL3aqycq20Context.sta60Min.a01006_Avg = 0;
	gTaskL3aqycq20Context.sta60Min.a01006_Min = 0;
	gTaskL3aqycq20Context.sta60Min.a01006_Max = 0;
	gTaskL3aqycq20Context.sta60Min.a01006_Flag = 'D';

	func_l3aqyc_measurement_value_flag_judge(&gTaskL3aqycq20Context.sta60Min);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	//分钟统计表更新数据库
	//if (dbi_Hcu_StaDatainfo_save(HCU_L3AQYC_STA_DBI_TABLE_1MIN, &(gTaskL3aqycq20Context.staOneMin)) == FAILURE)
			//HCU_ERROR_PRINT_L3BFSC("L3AQYC: Save data to DB error!\n");


	//分钟统计表发送统计报告
	if((FALSE == gTaskL3aqycq20Context.MinReportFlag) || (TRUE == gTaskL3aqycq20Context.MinReportFlag))//always on for test
	{
		msg_struct_l3mod_llczhb_data_report_t snd;
		memset(&snd, 0, sizeof(msg_struct_l3mod_llczhb_data_report_t));
		snd.actionId = HCU_SYSMSG_ZHBHJT_ACTION_GET_POL_MIN_RPT_2051;
		snd.ul2Cloud.DataTime = time(0);
		snd.ul2Cloud.nbrOfCMinRpt = HCU_SYSMSG_ZHBHJT_POLID_NBR_MAX;

		memcpy(&(snd.ul2Cloud.min), &(gTaskL3aqycq20Context.staOneMin), sizeof(msgie_struct_zhbhjt_frame_data_pol_min_hour_t)*HCU_SYSMSG_ZHBHJT_POLID_NBR_MAX);

		//test by shanchun
		int i;
		for(i=0; i< HCU_SYSMSG_ZHBHJT_POLID_NBR_MAX;i++){
			HCU_DEBUG_PRINT_INF("L3AQYCG20: snd.ul2Cloud.min[%d].PolId=[%d]\n",i, snd.ul2Cloud.min[i].PolId);
			HCU_DEBUG_PRINT_INF("L3AQYCG20: snd.ul2Cloud.min[%d].PolFlag=[%c]\n",i, snd.ul2Cloud.min[i].PolFlag);
			HCU_DEBUG_PRINT_INF("L3AQYCG20: snd.ul2Cloud.min[%d].Avg=[%4.2f]\n",i, snd.ul2Cloud.min[i].Avg);
			HCU_DEBUG_PRINT_INF("L3AQYCG20: snd.ul2Cloud.min[%d].Max=[%4.2f]\n",i, snd.ul2Cloud.min[i].Max);
			HCU_DEBUG_PRINT_INF("L3AQYCG20: snd.ul2Cloud.min[%d].Min=[%4.2f]\n\n\n",i, snd.ul2Cloud.min[i].Min);
		}

		snd.length = sizeof(msg_struct_l3mod_llczhb_data_report_t);

		ret = hcu_message_send(MSG_ID_L3MOD_LLCZHB_DATA_REPORT, TASK_ID_LLCZHB, TASK_ID_L3AQYCG20, &snd, snd.length);
		if (ret == FAILURE){
			HCU_ERROR_PRINT_L3AQYCG20("L3AQYCG20: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3AQYCG20].taskName, zHcuVmCtrTab.task[TASK_ID_LLCZHB].taskName);
		}

	}


	if ((gTaskL3aqycq20Context.elipseCnt % HCU_L3AQYC_STA_1H_REPORT_DURATION) == 0){

		if((FALSE == gTaskL3aqycq20Context.HourReportFlag) || (TRUE == gTaskL3aqycq20Context.HourReportFlag))//always on for test
		{
			msg_struct_l3mod_llczhb_data_report_t snd;
			memset(&snd, 0, sizeof(msg_struct_l3mod_llczhb_data_report_t));
			snd.actionId = HCU_SYSMSG_ZHBHJT_ACTION_GET_POL_HOUR_RPT_2061;
			snd.ul2Cloud.DataTime = time(0);
			snd.ul2Cloud.nbrOfCMinRpt = HCU_SYSMSG_ZHBHJT_POLID_NBR_MAX;

			memcpy(&(snd.ul2Cloud.min), &(gTaskL3aqycq20Context.sta60Min), sizeof(msgie_struct_zhbhjt_frame_data_pol_min_hour_t)*HCU_SYSMSG_ZHBHJT_POLID_NBR_MAX);

			//test by shanchun
			int i;
			for(i=0; i< HCU_SYSMSG_ZHBHJT_POLID_NBR_MAX;i++){
				HCU_DEBUG_PRINT_INF("L3AQYCG20: snd.ul2Cloud.min[%d].PolId=[%d]\n",i, snd.ul2Cloud.min[i].PolId);
				HCU_DEBUG_PRINT_INF("L3AQYCG20: snd.ul2Cloud.min[%d].PolFlag=[%c]\n",i, snd.ul2Cloud.min[i].PolFlag);
				HCU_DEBUG_PRINT_INF("L3AQYCG20: snd.ul2Cloud.min[%d].Avg=[%4.2f]\n",i, snd.ul2Cloud.min[i].Avg);
				HCU_DEBUG_PRINT_INF("L3AQYCG20: snd.ul2Cloud.min[%d].Max=[%4.2f]\n",i, snd.ul2Cloud.min[i].Max);
				HCU_DEBUG_PRINT_INF("L3AQYCG20: snd.ul2Cloud.min[%d].Min=[%4.2f]\n\n\n",i, snd.ul2Cloud.min[i].Min);
			}

			snd.length = sizeof(msg_struct_l3mod_llczhb_data_report_t);

			ret = hcu_message_send(MSG_ID_L3MOD_LLCZHB_DATA_REPORT, TASK_ID_LLCZHB, TASK_ID_L3AQYCG20, &snd, snd.length);
			if (ret == FAILURE){
				HCU_ERROR_PRINT_L3AQYCG20("L3AQYCG20: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3AQYCG20].taskName, zHcuVmCtrTab.task[TASK_ID_LLCZHB].taskName);
			}
		}
	}

	//返回
	return SUCCESS;
}

//监测数据标识符判断表初始化
void func_l3aqycg20_judge_value_init(void)
{
	gTaskL3aqycq20Context.valueJudge.a34001_Range_Max = HCU_L3AQYC_A34001_RANGE_MAX;
	gTaskL3aqycq20Context.valueJudge.a34001_Range_Min = HCU_L3AQYC_A34001_RANGE_MIN;
	gTaskL3aqycq20Context.valueJudge.a50001_Range_Max = HCU_L3AQYC_A50001_RANGE_MAX;
	gTaskL3aqycq20Context.valueJudge.a50001_Range_Min = HCU_L3AQYC_A50001_RANGE_MIN;
	gTaskL3aqycq20Context.valueJudge.a01001_Range_Max = HCU_L3AQYC_A01001_RANGE_MAX;
	gTaskL3aqycq20Context.valueJudge.a01001_Range_Min = HCU_L3AQYC_A01001_RANGE_MIN;
	gTaskL3aqycq20Context.valueJudge.a01002_Range_Max = HCU_L3AQYC_A01002_RANGE_MAX;
	gTaskL3aqycq20Context.valueJudge.a01002_Range_Min = HCU_L3AQYC_A01002_RANGE_MIN;
	gTaskL3aqycq20Context.valueJudge.a01007_Range_Max = HCU_L3AQYC_A01007_RANGE_MAX;
	gTaskL3aqycq20Context.valueJudge.a01007_Range_Min = HCU_L3AQYC_A01007_RANGE_MIN;
	gTaskL3aqycq20Context.valueJudge.a01008_Range_Max = HCU_L3AQYC_A01008_RANGE_MAX;
	gTaskL3aqycq20Context.valueJudge.a01008_Range_Min = HCU_L3AQYC_A01008_RANGE_MIN;
	gTaskL3aqycq20Context.valueJudge.a01006_Range_Max = HCU_L3AQYC_A01006_RANGE_MAX;
	gTaskL3aqycq20Context.valueJudge.a01006_Range_Min = HCU_L3AQYC_A01006_RANGE_MIN;

	gTaskL3aqycq20Context.valueJudge.a34001_Range_Set_Max = HCU_L3AQYC_A34001_RANGE_SET_MAX;
	gTaskL3aqycq20Context.valueJudge.a34001_Range_Set_Min = HCU_L3AQYC_A34001_RANGE_SET_MIN;
	gTaskL3aqycq20Context.valueJudge.a50001_Range_Set_Max = HCU_L3AQYC_A50001_RANGE_SET_MAX;
	gTaskL3aqycq20Context.valueJudge.a50001_Range_Set_Min = HCU_L3AQYC_A50001_RANGE_SET_MIN;
	gTaskL3aqycq20Context.valueJudge.a01001_Range_Set_Max = HCU_L3AQYC_A01001_RANGE_SET_MAX;
	gTaskL3aqycq20Context.valueJudge.a01001_Range_Set_Min = HCU_L3AQYC_A01001_RANGE_SET_MIN;
	gTaskL3aqycq20Context.valueJudge.a01002_Range_Set_Max = HCU_L3AQYC_A01002_RANGE_SET_MAX;
	gTaskL3aqycq20Context.valueJudge.a01002_Range_Set_Min = HCU_L3AQYC_A01002_RANGE_SET_MIN;
	gTaskL3aqycq20Context.valueJudge.a01007_Range_Set_Max = HCU_L3AQYC_A01007_RANGE_SET_MAX;
	gTaskL3aqycq20Context.valueJudge.a01007_Range_Set_Min = HCU_L3AQYC_A01007_RANGE_SET_MIN;
	gTaskL3aqycq20Context.valueJudge.a01008_Range_Set_Max = HCU_L3AQYC_A01008_RANGE_SET_MAX;
	gTaskL3aqycq20Context.valueJudge.a01008_Range_Set_Min = HCU_L3AQYC_A01008_RANGE_SET_MIN;
	gTaskL3aqycq20Context.valueJudge.a01006_Range_Set_Max = HCU_L3AQYC_A01006_RANGE_SET_MAX;
	gTaskL3aqycq20Context.valueJudge.a01006_Range_Set_Min = HCU_L3AQYC_A01006_RANGE_SET_MIN;

	gTaskL3aqycq20Context.valueJudge.a34001_Calibration_Flag = HCU_L3AQYC_A34001_CALIBRATION_FLAG;
	gTaskL3aqycq20Context.valueJudge.a50001_Calibration_Flag = HCU_L3AQYC_A50001_CALIBRATION_FLAG;
	gTaskL3aqycq20Context.valueJudge.a01001_Calibration_Flag = HCU_L3AQYC_A01001_CALIBRATION_FLAG;
	gTaskL3aqycq20Context.valueJudge.a01002_Calibration_Flag = HCU_L3AQYC_A01002_CALIBRATION_FLAG;
	gTaskL3aqycq20Context.valueJudge.a01007_Calibration_Flag = HCU_L3AQYC_A01007_CALIBRATION_FLAG;
	gTaskL3aqycq20Context.valueJudge.a01008_Calibration_Flag = HCU_L3AQYC_A01008_CALIBRATION_FLAG;
	gTaskL3aqycq20Context.valueJudge.a01006_Calibration_Flag = HCU_L3AQYC_A01006_CALIBRATION_FLAG;

	gTaskL3aqycq20Context.valueJudge.RainSnow_Flag = HCU_L3AQYC_RAIN_SNOW_FLAG;

	gTaskL3aqycq20Context.valueJudge.WindThresholdForNoise = HCU_L3AQYC_WINDTHRESHOLD_FOR_NOISE;

}

//监测数据flag判断，依据《建设工程颗粒物与噪声在线监测技术规范》
void func_l3aqyc_measurement_value_flag_judge(HcuSysMsgIeL3aqycContextStaElement_t *aggReport)
{
	//int ret = 0;
	aggReport->a34001_Flag = 'N';
	aggReport->a50001_Flag = 'N';
	aggReport->a01001_Flag = 'N';
	aggReport->a01002_Flag = 'N';
	aggReport->a01007_Flag = 'N';
	aggReport->a01008_Flag = 'N';
	aggReport->a01006_Flag = 'D';

	//超仪器量程范围上下限
	if(aggReport->a34001_Flag < gTaskL3aqycq20Context.valueJudge.a34001_Range_Min)
			aggReport->a34001_Flag = 'L';

	if(aggReport->a34001_Flag > gTaskL3aqycq20Context.valueJudge.a34001_Range_Max)
			aggReport->a34001_Flag = 'H';

	if(aggReport->a50001_Flag < gTaskL3aqycq20Context.valueJudge.a50001_Range_Min)
			aggReport->a50001_Flag = 'L';

	if(aggReport->a50001_Flag > gTaskL3aqycq20Context.valueJudge.a50001_Range_Max)
			aggReport->a50001_Flag = 'H';

	if(aggReport->a01001_Flag < gTaskL3aqycq20Context.valueJudge.a01001_Range_Min)
			aggReport->a01001_Flag = 'L';

	if(aggReport->a01001_Flag > gTaskL3aqycq20Context.valueJudge.a01001_Range_Max)
			aggReport->a01001_Flag = 'H';

	if(aggReport->a01002_Flag < gTaskL3aqycq20Context.valueJudge.a01002_Range_Min)
			aggReport->a01002_Flag = 'L';

	if(aggReport->a01002_Flag > gTaskL3aqycq20Context.valueJudge.a01002_Range_Max)
			aggReport->a01002_Flag = 'H';

	if(aggReport->a01007_Flag < gTaskL3aqycq20Context.valueJudge.a01007_Range_Min)
			aggReport->a01007_Flag = 'L';

	if(aggReport->a01007_Flag > gTaskL3aqycq20Context.valueJudge.a01007_Range_Max)
			aggReport->a01007_Flag = 'H';

	if(aggReport->a01008_Flag < gTaskL3aqycq20Context.valueJudge.a01008_Range_Min)
			aggReport->a01008_Flag = 'L';

	if(aggReport->a01008_Flag > gTaskL3aqycq20Context.valueJudge.a01008_Range_Max)
			aggReport->a01008_Flag = 'H';

	//超数据设定范围上下限，上下限的值设置由云端设置，默认和仪器量程相等
	if(aggReport->a34001_Flag < gTaskL3aqycq20Context.valueJudge.a34001_Range_Min)
			aggReport->a34001_Flag = '-';

	if(aggReport->a34001_Flag > gTaskL3aqycq20Context.valueJudge.a34001_Range_Max)
			aggReport->a34001_Flag = '+';

	if(aggReport->a50001_Flag < gTaskL3aqycq20Context.valueJudge.a50001_Range_Min)
			aggReport->a50001_Flag = '_';

	if(aggReport->a50001_Flag > gTaskL3aqycq20Context.valueJudge.a50001_Range_Max)
			aggReport->a50001_Flag = '+';

	if(aggReport->a01001_Flag < gTaskL3aqycq20Context.valueJudge.a01001_Range_Min)
			aggReport->a01001_Flag = '_';

	if(aggReport->a01001_Flag > gTaskL3aqycq20Context.valueJudge.a01001_Range_Max)
			aggReport->a01001_Flag = '+';

	if(aggReport->a01002_Flag < gTaskL3aqycq20Context.valueJudge.a01002_Range_Min)
			aggReport->a01002_Flag = '_';

	if(aggReport->a01002_Flag > gTaskL3aqycq20Context.valueJudge.a01002_Range_Max)
			aggReport->a01002_Flag = '+';

	if(aggReport->a01007_Flag < gTaskL3aqycq20Context.valueJudge.a01007_Range_Min)
			aggReport->a01007_Flag = '_';

	if(aggReport->a01007_Flag > gTaskL3aqycq20Context.valueJudge.a01007_Range_Max)
			aggReport->a01007_Flag = '+';

	if(aggReport->a01008_Flag < gTaskL3aqycq20Context.valueJudge.a01008_Range_Min)
			aggReport->a01008_Flag = '_';

	if(aggReport->a01008_Flag > gTaskL3aqycq20Context.valueJudge.a01008_Range_Max)
			aggReport->a01008_Flag = '+';

	//当风速大于门限值时，默认5m/s
	if(aggReport->a01007_Avg > gTaskL3aqycq20Context.valueJudge.WindThresholdForNoise)
			aggReport->a50001_Flag = 'S';

	//当雨雪雷电天气，标志位由云端设定,
	if(TRUE == gTaskL3aqycq20Context.valueJudge.RainSnow_Flag)
	{
		aggReport->a34001_Flag = 'R';
		aggReport->a50001_Flag = 'R';
		aggReport->a01001_Flag = 'R';
		aggReport->a01002_Flag = 'R';
		aggReport->a01007_Flag = 'R';
		aggReport->a01008_Flag = 'R';
	}

	//当设备校准时，标志位由云端设定
	if(TRUE == gTaskL3aqycq20Context.valueJudge.a34001_Calibration_Flag){
		aggReport->a34001_Flag = 'R';
	}
	if(TRUE == gTaskL3aqycq20Context.valueJudge.a50001_Calibration_Flag){
		aggReport->a50001_Flag = 'R';
	}
	if(TRUE == gTaskL3aqycq20Context.valueJudge.a01001_Calibration_Flag){
		aggReport->a01001_Flag = 'R';
	}
	if(TRUE == gTaskL3aqycq20Context.valueJudge.a01002_Calibration_Flag){
		aggReport->a01002_Flag = 'R';
	}
	if(TRUE == gTaskL3aqycq20Context.valueJudge.a01007_Calibration_Flag){
		aggReport->a01007_Flag = 'R';
	}
	if(TRUE == gTaskL3aqycq20Context.valueJudge.a01008_Calibration_Flag){
		aggReport->a01008_Flag = 'R';
	}

}




