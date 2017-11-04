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
#include "../l3app/sensornoise.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <wchar.h>

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
SerialLedMsgBuf_t currentLedBuf;
UINT32 LCD_AlarmFlag;

//Task Global variables
gTaskL3aqycq20Context_t gTaskL3aqycq20Context; //扬尘监测的L3总控表


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
	LCD_AlarmFlag = 0;

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

	//SerialLedMsgBuf_t currentLedBuf;

	memset(&currentLedBuf, 0, sizeof(SerialLedMsgBuf_t));

	//监测数据标识符判断表初始化
	func_l3aqycg20_judge_value_init();

	//传感器运行状态表初始化
	func_l3aqycg20_eqt_rs_init();

	//传感器告警门限值初始化
	func_l3aqycg20_alarm_limit_init();

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
			HCU_L3AQYC_STA_DURATION, TIMER_TYPE_PERIOD, TIMER_RESOLUTION_1S);
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

		HCU_DEBUG_PRINT_INF("L3AQYCG20 start aggregation process\n\n\n");
		//周期性聚合表生成
		if (func_l3aqyc_time_out_aggregation_process() == FAILURE)
			HcuErrorPrint("L3AQYCG20: Error process time out message!\n");
	}

	//触发开始发送分钟报告
	else if ((rcv.timeId == TIMER_ID_1S_L3AQYCG20_START_MIN_REPORT) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		gTaskL3aqycq20Context.MinReportFlag = TRUE;
	}

	//触发停止发送分钟报告
	else if ((rcv.timeId == TIMER_ID_1S_L3AQYCG20_STOP_MIN_REPORT) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		gTaskL3aqycq20Context.MinReportFlag = FALSE;
		//还需要触发发送 返回操作执行结果（9012 procedure）,原设计每发送一个数据包就返回操作执行结果是不对的，而是直至发送完符合时间段内的所有包，再返回执行结束命令
	}

	//触发开始发送小时报告
	else if ((rcv.timeId == TIMER_ID_1S_L3AQYCG20_START_HOUR_REPORT) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		gTaskL3aqycq20Context.HourReportFlag = TRUE;
	}

	//触发停止发送小时报告
	else if ((rcv.timeId == TIMER_ID_1S_L3AQYCG20_STOP_HOUR_REPORT) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		gTaskL3aqycq20Context.HourReportFlag = FALSE;
		//还需要触发发送 返回操作执行结果（9012 procedure）,原设计每发送一个数据包就返回操作执行结果是不对的，而是直至发送完符合时间段内的所有包，再返回执行结束命令
	}

	//触发开始发送天报告
	else if ((rcv.timeId == TIMER_ID_1S_L3AQYCG20_START_DAY_REPORT) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		gTaskL3aqycq20Context.DayReportFlag = TRUE;
	}

	//触发停止发送天报告
	else if ((rcv.timeId == TIMER_ID_1S_L3AQYCG20_STOP_DAY_REPORT) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		gTaskL3aqycq20Context.DayReportFlag = FALSE;
		//还需要触发发送 返回操作执行结果（9012 procedure）,原设计每发送一个数据包就返回操作执行结果是不对的，而是直至发送完符合时间段内的所有包，再返回执行结束命令
	}
/*
	//触发开始发送报警记录
	else if ((rcv.timeId == TIMER_ID_1S_L3AQYCG20_START_ALA_REPORT) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		gTaskL3aqycq20Context.AlaReportFlag = TRUE;
	}

	//触发停止发送报警记录
	else if ((rcv.timeId == TIMER_ID_1S_L3AQYCG20_STOP_ALA_REPORT) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		gTaskL3aqycq20Context.AlaReportFlag = FALSE;
		//还需要触发发送 返回操作执行结果（9012 procedure）,原设计每发送一个数据包就返回操作执行结果是不对的，而是直至发送完符合时间段内的所有包，再返回执行结束命令
	}
*/
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
	UINT8 i = 0;
	UINT8 limit = 0;
	msg_struct_llczhb_l3mod_ctrl_req_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_llczhb_l3mod_ctrl_req_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_llczhb_l3mod_ctrl_req_t)))
		HCU_ERROR_PRINT_L3AQYCG20("AQYC: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);

	UINT32 timeCurrent;
	timeCurrent = time(0);
	msg_struct_l3mod_llczhb_ctrl_resp_t sndResp;
	msg_struct_l3mod_llczhb_data_report_t sndReport;

	switch (rcv.actionId){
	case HCU_SYSMSG_ZHBHJT_ACTION_EXECUTE_FINISH_9012:
		HCU_ERROR_PRINT_L3AQYCG20("AQYC: Error action received!\n");
		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_DATA_CFM_9014:
		HCU_ERROR_PRINT_L3AQYCG20("AQYC: Error action received!\n");//消息终结在LLC
		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_SET_PSWD_1072:
		HCU_ERROR_PRINT_L3AQYCG20("AQYC: Error action received!\n");//消息终结在LLC
		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_GET_FIELD_TIME_1011:
		HCU_ERROR_PRINT_L3AQYCG20("AQYC: Error action received!\n");//消息终结在LLC
		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_SET_FIELD_TIME_1012:
		HCU_ERROR_PRINT_L3AQYCG20("AQYC: Error action received!\n");//消息终结在LLC
		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_GET_POL_RTD_2011:
		gTaskL3aqycq20Context.InstReportFlag = TRUE;//应该取实时采样数据上报间隔RtdInterval(ref: 1062),然后起Timer定时上报，暂时简化处理每30s(缺省一个扫描周期：30s)上报
		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_STOP_RTD_2012:
		gTaskL3aqycq20Context.InstReportFlag = FALSE;//停止Timer(RtdInterval)
		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_GET_EQU_RUN_2021://设备运行状态，需要从L3 Context取得，L3需要记录每个设备运行状态(on/off),modbus or spr 串口数据不通需要告知L3: off
		gTaskL3aqycq20Context.EquStatusReportFlag =TRUE;
		//msg_struct_l3mod_llczhb_data_report_t snd;
		memset(&sndReport, 0, sizeof(msg_struct_l3mod_llczhb_data_report_t));
		sndReport.actionId = HCU_SYSMSG_ZHBHJT_ACTION_GET_EQU_RUN_2021;
		sndReport.ul2Cloud.DataTime = time(0);
		sndReport.ul2Cloud.nbrOfCRtd = HCU_SYSMSG_ZHBHJT_POLID_NBR_MAX;

		memcpy(&(sndReport.ul2Cloud.RS), &(gTaskL3aqycq20Context.eqtStatus), sizeof(msgie_struct_zhbhjt_frame_data_RS_value_t)*HCU_SYSMSG_ZHBHJT_POLID_NBR_MAX);

		//test by shanchun
		for(i=0; i< HCU_SYSMSG_ZHBHJT_POLID_NBR_MAX;i++){
			HCU_DEBUG_PRINT_INF("L3AQYCG20: sndReport.ul2Cloud.RS[%d].PolId=[%d]\n",i, sndReport.ul2Cloud.RS[i].PolId);
			HCU_DEBUG_PRINT_INF("L3AQYCG20: sndReport.ul2Cloud.RS[%d].RS=[%d]\n",i, sndReport.ul2Cloud.RS[i].RS);
		}

		sndReport.length = sizeof(msg_struct_l3mod_llczhb_data_report_t);

		ret = hcu_message_send(MSG_ID_L3MOD_LLCZHB_DATA_REPORT, TASK_ID_LLCZHB, TASK_ID_L3AQYCG20, &sndReport, sndReport.length);
		if (ret == FAILURE){
			HCU_ERROR_PRINT_L3AQYCG20("L3AQYCG20: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3AQYCG20].taskName, zHcuVmCtrTab.task[TASK_ID_LLCZHB].taskName);
		}
		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_STOP_EQU_RUN_2022:
		gTaskL3aqycq20Context.EquStatusReportFlag =FALSE;
		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_GET_POL_MIN_RPT_2051:
		//取得开始和结束时间，计算开始和结束时间和当前时间的差值，并同时起采集数据起始时间和结束时间的定时器，
		//数据从聚合表中获得，聚合表由内部定义Timer定时从数据库中取数据计算（暂定一分钟），为简化设计，可依据Flag值来判断是否上传发送
		HcuErrorPrint("L3AQYCG20: BeginTime=%d  EndTime=%d at min report request message,current Time=%d !\n\n",rcv.dl2Self.gapTime.BeginTime, rcv.dl2Self.gapTime.EndTime,timeCurrent);
		gTaskL3aqycq20Context.timeBegin_Min = rcv.dl2Self.gapTime.BeginTime - timeCurrent;
		gTaskL3aqycq20Context.timeEnd_Min = rcv.dl2Self.gapTime.EndTime - timeCurrent;

		if(gTaskL3aqycq20Context.timeBegin_Min < 0 || gTaskL3aqycq20Context.timeEnd_Min < 0)
			HcuErrorPrint("L3AQYCG20: Receive backward BeginTime=%d or EndTime=%d at min report request message !\n\n",rcv.dl2Self.gapTime.BeginTime, rcv.dl2Self.gapTime.EndTime);
		else
			HCU_DEBUG_PRINT_INF("L3AQYCG20: Receive 2051 CMD, get pollution date will start from BeginTime=%d, stop at EndTime=%d\n\n", gTaskL3aqycq20Context.timeBegin_Min,  gTaskL3aqycq20Context.timeEnd_Min);

		//启动分钟报告定时器
		ret = hcu_timer_start(TASK_ID_L3AQYCG20, TIMER_ID_1S_L3AQYCG20_START_MIN_REPORT, gTaskL3aqycq20Context.timeBegin_Min, TIMER_TYPE_ONE_TIME, TIMER_RESOLUTION_1S);
		if (ret == FAILURE){
			HCU_ERROR_PRINT_L3AQYCG20("L3AQYCG20: Error start timer!\n");
		}

		//停止分钟报告定时器
		ret = hcu_timer_start(TASK_ID_L3AQYCG20, TIMER_ID_1S_L3AQYCG20_STOP_MIN_REPORT, gTaskL3aqycq20Context.timeEnd_Min, TIMER_TYPE_ONE_TIME, TIMER_RESOLUTION_1S);
		if (ret == FAILURE){
			HCU_ERROR_PRINT_L3AQYCG20("L3AQYCG20: Error start timer!\n");
		}

		break;


	case HCU_SYSMSG_ZHBHJT_ACTION_GET_POL_HOUR_RPT_2061:
		HcuErrorPrint("L3AQYCG20: BeginTime=%d  EndTime=%d at min report request message,current Time=%d !\n\n",rcv.dl2Self.gapTime.BeginTime, rcv.dl2Self.gapTime.EndTime,timeCurrent);
		gTaskL3aqycq20Context.timeBegin_Hour = rcv.dl2Self.gapTime.BeginTime - timeCurrent;
		gTaskL3aqycq20Context.timeEnd_Hour = rcv.dl2Self.gapTime.EndTime - timeCurrent;

		if(gTaskL3aqycq20Context.timeBegin_Hour < 0 || gTaskL3aqycq20Context.timeEnd_Hour < 0)
			HcuErrorPrint("L3AQYCG20: Receive backward BeginTime=%d or EndTime=%d at min report request message !\n\n",rcv.dl2Self.gapTime.BeginTime, rcv.dl2Self.gapTime.EndTime);
		else
			HCU_DEBUG_PRINT_INF("L3AQYCG20: Receive 2061 CMD, get pollution date will start from BeginTime=%d, stop at EndTime=%d\n\n", gTaskL3aqycq20Context.timeBegin_Hour,  gTaskL3aqycq20Context.timeEnd_Hour);

		//启动分钟报告定时器
		ret = hcu_timer_start(TASK_ID_L3AQYCG20, TIMER_ID_1S_L3AQYCG20_START_HOUR_REPORT, gTaskL3aqycq20Context.timeBegin_Hour, TIMER_TYPE_ONE_TIME, TIMER_RESOLUTION_1S);
		if (ret == FAILURE){
			HCU_ERROR_PRINT_L3AQYCG20("L3AQYCG20: Error start timer!\n");
		}

		//停止分钟报告定时器
		ret = hcu_timer_start(TASK_ID_L3AQYCG20, TIMER_ID_1S_L3AQYCG20_STOP_HOUR_REPORT, gTaskL3aqycq20Context.timeEnd_Hour, TIMER_TYPE_ONE_TIME, TIMER_RESOLUTION_1S);
		if (ret == FAILURE){
			HCU_ERROR_PRINT_L3AQYCG20("L3AQYCG20: Error start timer!\n");
		}


		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_GET_POL_DAY_RPT_2031:
		timeCurrent = time(0);
		HcuErrorPrint("L3AQYCG20: BeginTime=%d  EndTime=%d at min report request message,current Time=%d !\n\n",rcv.dl2Self.gapTime.BeginTime, rcv.dl2Self.gapTime.EndTime,timeCurrent);
		gTaskL3aqycq20Context.timeBegin_Day = rcv.dl2Self.gapTime.BeginTime - timeCurrent;
		gTaskL3aqycq20Context.timeEnd_Day = rcv.dl2Self.gapTime.EndTime - timeCurrent;

		if(gTaskL3aqycq20Context.timeBegin_Day < 0 || gTaskL3aqycq20Context.timeEnd_Day < 0)
			HcuErrorPrint("L3AQYCG20: Receive backward BeginTime=%d or EndTime=%d at min report request message !\n\n",rcv.dl2Self.gapTime.BeginTime, rcv.dl2Self.gapTime.EndTime);
		else
			HCU_DEBUG_PRINT_INF("L3AQYCG20: Receive 2051 CMD, get pollution date will start from BeginTime=%d, stop at EndTime=%d\n\n", gTaskL3aqycq20Context.timeBegin_Day,  gTaskL3aqycq20Context.timeEnd_Day);

		//启动分钟报告定时器
		ret = hcu_timer_start(TASK_ID_L3AQYCG20, TIMER_ID_1S_L3AQYCG20_START_DAY_REPORT, gTaskL3aqycq20Context.timeBegin_Day, TIMER_TYPE_ONE_TIME, TIMER_RESOLUTION_1S);
		if (ret == FAILURE){
			HCU_ERROR_PRINT_L3AQYCG20("L3AQYCG20: Error start timer!\n");
		}

		//停止分钟报告定时器
		ret = hcu_timer_start(TASK_ID_L3AQYCG20, TIMER_ID_1S_L3AQYCG20_STOP_DAY_REPORT, gTaskL3aqycq20Context.timeEnd_Day, TIMER_TYPE_ONE_TIME, TIMER_RESOLUTION_1S);
		if (ret == FAILURE){
			HCU_ERROR_PRINT_L3AQYCG20("L3AQYCG20: Error start timer!\n");
		}

		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_GET_POL_RT_2041:
		HCU_ERROR_PRINT_L3AQYCG20("AQYC: Error action received!\n");//一日内运行时间的累计值，对扬尘系统不适用
		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_GET_POL_ALA_2071://取污染物报警记录(需要确定是否是取指定时间段内的历史记录？如果是，需要查询PM10和noise在指定时间段内的报警数据并上报，待实现。。)
		timeCurrent = time(0);
		HcuErrorPrint("L3AQYCG20: BeginTime=%d  EndTime=%d at min report request message,current Time=%d !\n\n",rcv.dl2Self.gapTime.BeginTime, rcv.dl2Self.gapTime.EndTime,timeCurrent);
		gTaskL3aqycq20Context.timeBegin_Ala = rcv.dl2Self.gapTime.BeginTime;
		gTaskL3aqycq20Context.timeEnd_Ala = rcv.dl2Self.gapTime.EndTime;

		HCU_DEBUG_PRINT_INF("L3AQYCG20: Receive 2071 CMD, get pollution date will start from BeginTime=%d, stop at EndTime=%d\n\n", gTaskL3aqycq20Context.timeBegin_Ala,  gTaskL3aqycq20Context.timeEnd_Ala);

		//查询PM10和noise在指定时间段内的报警数据并上报,待实现,若无报警记录返回啥

		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_SND_ALM_EVT_2072://非上位机来的命令，是主动上报
		HCU_ERROR_PRINT_L3AQYCG20("AQYC: Error action received!\n");
		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_SET_ALMLIM_1022://设置污染物报警门限值

		limit = rcv.dl2Self.nbrOfLimitation;
		for(i=0; i< limit; i++)
		{
			if( HCU_L3AQYC_A01001_INDEX == rcv.dl2Self.limitation[i].PolId){
				gTaskL3aqycq20Context.alarmLimit.a01001_Alarm_LowValue = rcv.dl2Self.limitation[i].LowValue;
				gTaskL3aqycq20Context.alarmLimit.a01001_Alarm_UpValue = rcv.dl2Self.limitation[i].UpValue;
			}
			else if( HCU_L3AQYC_A01002_INDEX == rcv.dl2Self.limitation[i].PolId){
				gTaskL3aqycq20Context.alarmLimit.a01002_Alarm_LowValue = rcv.dl2Self.limitation[i].LowValue;
				gTaskL3aqycq20Context.alarmLimit.a01002_Alarm_UpValue = rcv.dl2Self.limitation[i].UpValue;
			}
			else if( HCU_L3AQYC_A01006_INDEX == rcv.dl2Self.limitation[i].PolId){
				gTaskL3aqycq20Context.alarmLimit.a01006_Alarm_LowValue = rcv.dl2Self.limitation[i].LowValue;
				gTaskL3aqycq20Context.alarmLimit.a01006_Alarm_UpValue = rcv.dl2Self.limitation[i].UpValue;
			}
			else if( HCU_L3AQYC_A01007_INDEX == rcv.dl2Self.limitation[i].PolId){
				gTaskL3aqycq20Context.alarmLimit.a01007_Alarm_LowValue = rcv.dl2Self.limitation[i].LowValue;
				gTaskL3aqycq20Context.alarmLimit.a01007_Alarm_UpValue = rcv.dl2Self.limitation[i].UpValue;
			}
			else if( HCU_L3AQYC_A01007_INDEX == rcv.dl2Self.limitation[i].PolId){
				gTaskL3aqycq20Context.alarmLimit.a01008_Alarm_LowValue = rcv.dl2Self.limitation[i].LowValue;
				gTaskL3aqycq20Context.alarmLimit.a01008_Alarm_UpValue = rcv.dl2Self.limitation[i].UpValue;
			}
			else if( HCU_L3AQYC_A34001_INDEX == rcv.dl2Self.limitation[i].PolId){
				gTaskL3aqycq20Context.alarmLimit.a34001_Alarm_LowValue = rcv.dl2Self.limitation[i].LowValue;
				gTaskL3aqycq20Context.alarmLimit.a34001_Alarm_UpValue = rcv.dl2Self.limitation[i].UpValue;
			}
			else if( HCU_L3AQYC_A50001_INDEX == rcv.dl2Self.limitation[i].PolId){
				gTaskL3aqycq20Context.alarmLimit.a50001_Alarm_LowValue = rcv.dl2Self.limitation[i].LowValue;
				gTaskL3aqycq20Context.alarmLimit.a50001_Alarm_UpValue = rcv.dl2Self.limitation[i].UpValue;
			}
		}

		memset(&sndResp, 0, sizeof(msg_struct_l3mod_llczhb_ctrl_resp_t));
		sndResp.actionId = HCU_SYSMSG_ZHBHJT_ACTION_SET_ALMLIM_1022;
		sndResp.ul2Cloud.ExeRtn = TRUE;
		sndResp.length = sizeof(msg_struct_l3mod_llczhb_ctrl_resp_t);

		ret = hcu_message_send(MSG_ID_L3MOD_LLCZHB_CTRL_RESP, TASK_ID_LLCZHB, TASK_ID_L3AQYCG20, &sndResp, sndResp.length);
		if (ret == FAILURE){
			HCU_ERROR_PRINT_L3AQYCG20("L3AQYCG20: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3AQYCG20].taskName, zHcuVmCtrTab.task[TASK_ID_LLCZHB].taskName);
		}

		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_GET_ALMLIM_1021:
		memset(&sndResp, 0, sizeof(msg_struct_l3mod_llczhb_ctrl_resp_t));
		sndResp.actionId = HCU_SYSMSG_ZHBHJT_ACTION_GET_ALMLIM_1021;
		sndResp.length = sizeof(msg_struct_l3mod_llczhb_ctrl_resp_t);
		sndResp.ul2Cloud.nbrOfAlmLim = HCU_SYSMSG_ZHBHJT_POLID_NBR_MAX;
		memcpy(&(sndResp.ul2Cloud.limitation), &(gTaskL3aqycq20Context.alarmLimit), sizeof(msgie_struct_zhbhjt_frame_data_low_upvalue_t)*HCU_SYSMSG_ZHBHJT_POLID_NBR_MAX);

		ret = hcu_message_send(MSG_ID_L3MOD_LLCZHB_CTRL_RESP, TASK_ID_LLCZHB, TASK_ID_L3AQYCG20, &sndResp, sndResp.length);
		if (ret == FAILURE){
			HCU_ERROR_PRINT_L3AQYCG20("L3AQYCG20: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3AQYCG20].taskName, zHcuVmCtrTab.task[TASK_ID_LLCZHB].taskName);
		}
		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_SET_MNADDR_1032:
		HCU_ERROR_PRINT_L3AQYCG20("AQYC: Error action received!\n");//消息终结在LLC
		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_GET_MNADDR_1031:
		HCU_ERROR_PRINT_L3AQYCG20("AQYC: Error action received!\n");//消息终结在LLC
		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_SET_RPT_TIME_1042://数据上报时间
		gTaskL3aqycq20Context.ReportTime = rcv.dl2Self.ReportTime;

		//msg_struct_l3mod_llczhb_ctrl_resp_t snd;
		memset(&sndResp, 0, sizeof(msg_struct_l3mod_llczhb_ctrl_resp_t));
		sndResp.actionId = HCU_SYSMSG_ZHBHJT_ACTION_SET_RPT_TIME_1042;
		sndResp.ul2Cloud.ExeRtn = TRUE;
		sndResp.length = sizeof(msg_struct_l3mod_llczhb_ctrl_resp_t);

		ret = hcu_message_send(MSG_ID_L3MOD_LLCZHB_CTRL_RESP, TASK_ID_LLCZHB, TASK_ID_L3AQYCG20, &sndResp, sndResp.length);
		if (ret == FAILURE){
			HCU_ERROR_PRINT_L3AQYCG20("L3AQYCG20: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3AQYCG20].taskName, zHcuVmCtrTab.task[TASK_ID_LLCZHB].taskName);
		}
		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_GET_RPT_TIME_1041://提取数据上报时间
		memset(&sndResp, 0, sizeof(msg_struct_l3mod_llczhb_ctrl_resp_t));
		sndResp.actionId = HCU_SYSMSG_ZHBHJT_ACTION_GET_RPT_TIME_1041;
		sndResp.length = sizeof(msg_struct_l3mod_llczhb_ctrl_resp_t);
		sndResp.ul2Cloud.ReportTime = gTaskL3aqycq20Context.ReportTime;

		ret = hcu_message_send(MSG_ID_L3MOD_LLCZHB_CTRL_RESP, TASK_ID_LLCZHB, TASK_ID_L3AQYCG20, &sndResp, sndResp.length);
		if (ret == FAILURE){
			HCU_ERROR_PRINT_L3AQYCG20("L3AQYCG20: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3AQYCG20].taskName, zHcuVmCtrTab.task[TASK_ID_LLCZHB].taskName);
		}
		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_SET_ZERO_3011://下端监测设备较零较满
		gTaskL3aqycq20Context.singlePolId = rcv.dl2Self.singlePolId;//shanchun:收到监控设备较零较满污染物ID后该干啥，需要研究下下。

		//msg_struct_l3mod_llczhb_ctrl_resp_t snd;
		memset(&sndResp, 0, sizeof(msg_struct_l3mod_llczhb_ctrl_resp_t));
		sndResp.actionId = HCU_SYSMSG_ZHBHJT_ACTION_SET_ZERO_3011;
		sndResp.ul2Cloud.ExeRtn = TRUE;
		sndResp.length = sizeof(msg_struct_l3mod_llczhb_ctrl_resp_t);

		ret = hcu_message_send(MSG_ID_L3MOD_LLCZHB_CTRL_RESP, TASK_ID_LLCZHB, TASK_ID_L3AQYCG20, &sndResp, sndResp.length);
		if (ret == FAILURE){
			HCU_ERROR_PRINT_L3AQYCG20("L3AQYCG20: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3AQYCG20].taskName, zHcuVmCtrTab.task[TASK_ID_LLCZHB].taskName);
		}
		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_SET_RTDI_1062:
		gTaskL3aqycq20Context.RtdInterval = rcv.dl2Self.RtdInterval;

		//msg_struct_l3mod_llczhb_ctrl_resp_t snd;
		memset(&sndResp, 0, sizeof(msg_struct_l3mod_llczhb_ctrl_resp_t));
		sndResp.actionId = HCU_SYSMSG_ZHBHJT_ACTION_SET_RTDI_1062;
		sndResp.ul2Cloud.ExeRtn = TRUE;
		sndResp.length = sizeof(msg_struct_l3mod_llczhb_ctrl_resp_t);

		ret = hcu_message_send(MSG_ID_L3MOD_LLCZHB_CTRL_RESP, TASK_ID_LLCZHB, TASK_ID_L3AQYCG20, &sndResp, sndResp.length);
		if (ret == FAILURE){
			HCU_ERROR_PRINT_L3AQYCG20("L3AQYCG20: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3AQYCG20].taskName, zHcuVmCtrTab.task[TASK_ID_LLCZHB].taskName);
		}
		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_GET_RTDI_1061:
		memset(&sndResp, 0, sizeof(msg_struct_l3mod_llczhb_ctrl_resp_t));
		sndResp.actionId = HCU_SYSMSG_ZHBHJT_ACTION_GET_RTDI_1061;
		sndResp.length = sizeof(msg_struct_l3mod_llczhb_ctrl_resp_t);
		sndResp.ul2Cloud.RtdInterval = gTaskL3aqycq20Context.RtdInterval;

		ret = hcu_message_send(MSG_ID_L3MOD_LLCZHB_CTRL_RESP, TASK_ID_LLCZHB, TASK_ID_L3AQYCG20, &sndResp, sndResp.length);
		if (ret == FAILURE){
			HCU_ERROR_PRINT_L3AQYCG20("L3AQYCG20: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3AQYCG20].taskName, zHcuVmCtrTab.task[TASK_ID_LLCZHB].taskName);
		}

		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_SET_OT_RC_1000://消息终结在LLC
		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_SET_WARN_TIME_1001:
		gTaskL3aqycq20Context.WarnTime = rcv.dl2Self.WarnTime;//shanchun: 需要研究下下L3该如何下一步Action

		//msg_struct_l3mod_llczhb_ctrl_resp_t snd;
		memset(&sndResp, 0, sizeof(msg_struct_l3mod_llczhb_ctrl_resp_t));
		sndResp.actionId = HCU_SYSMSG_ZHBHJT_ACTION_SET_WARN_TIME_1001;
		sndResp.ul2Cloud.ExeRtn = TRUE;
		sndResp.length = sizeof(msg_struct_l3mod_llczhb_ctrl_resp_t);

		ret = hcu_message_send(MSG_ID_L3MOD_LLCZHB_CTRL_RESP, TASK_ID_LLCZHB, TASK_ID_L3AQYCG20, &sndResp, sndResp.length);
		if (ret == FAILURE){
			HCU_ERROR_PRINT_L3AQYCG20("L3AQYCG20: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3AQYCG20].taskName, zHcuVmCtrTab.task[TASK_ID_LLCZHB].taskName);
		}
		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_SET_INST_SAMPLE_3012:
		gTaskL3aqycq20Context.singlePolId = rcv.dl2Self.singlePolId;//shanchun:收到监控设备即时采样污染物ID后该干啥，需要研究下下。

		//msg_struct_l3mod_llczhb_ctrl_resp_t snd;
		memset(&sndResp, 0, sizeof(msg_struct_l3mod_llczhb_ctrl_resp_t));
		sndResp.actionId = HCU_SYSMSG_ZHBHJT_ACTION_SET_INST_SAMPLE_3012;
		sndResp.ul2Cloud.ExeRtn = TRUE;
		sndResp.length = sizeof(msg_struct_l3mod_llczhb_ctrl_resp_t);

		ret = hcu_message_send(MSG_ID_L3MOD_LLCZHB_CTRL_RESP, TASK_ID_LLCZHB, TASK_ID_L3AQYCG20, &sndResp, sndResp.length);
		if (ret == FAILURE){
			HCU_ERROR_PRINT_L3AQYCG20("L3AQYCG20: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3AQYCG20].taskName, zHcuVmCtrTab.task[TASK_ID_LLCZHB].taskName);
		}
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
	gTaskL3aqycq20Context.cur.a34001_PolId = HCU_L3AQYC_A34001_INDEX;
	gTaskL3aqycq20Context.cur.a34001_Flag = 'N';
	ret = dbi_HcuPm25DataInfo_GetAvg(HCU_L3AQYC_STA_1M_REPORT_DURATION,&gTaskL3aqycq20Context.staInstant);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get avg data from PM25 database!\n");
	}
	else{
		//HCU_DEBUG_PRINT_INF("L3AQYCG20 get PM10 avg data = %4.3f\n", gTaskL3aqycq20Context.staInstant.a34001_Avg);
		gTaskL3aqycq20Context.cur.a34001_Avg = gTaskL3aqycq20Context.staInstant.a34001_Avg;
	}

	//噪声
	gTaskL3aqycq20Context.cur.a50001_PolId = HCU_L3AQYC_A50001_INDEX;
	gTaskL3aqycq20Context.cur.a50001_Flag = 'N';
	ret = dbi_HcuNoiseDataInfo_GetAvg(HCU_L3AQYC_STA_1M_REPORT_DURATION,&gTaskL3aqycq20Context.staInstant);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get avg data from Noise database!\n");
	}
	else{
		//HCU_DEBUG_PRINT_INF("L3AQYCG20 get Noise avg  data = %4.3f\n", gTaskL3aqycq20Context.staInstant.a50001_Avg);
		gTaskL3aqycq20Context.cur.a50001_Avg = gTaskL3aqycq20Context.staInstant.a50001_Avg;
	}


	//温度
	gTaskL3aqycq20Context.staInstant.a01001_PolId = HCU_L3AQYC_A01001_INDEX;
	gTaskL3aqycq20Context.staInstant.a01001_Flag = 'N';
	ret = dbi_HcuTempDataInfo_GetAvg(HCU_L3AQYC_STA_1M_REPORT_DURATION,&gTaskL3aqycq20Context.staInstant);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get avg data from Temp database!\n");
	}
	else{
		//HCU_DEBUG_PRINT_INF("L3AQYCG20 get Temp avg data = %4.3f\n", gTaskL3aqycq20Context.staInstant.a01001_Avg);
		gTaskL3aqycq20Context.cur.a01001_Avg = gTaskL3aqycq20Context.staInstant.a01001_Avg;
	}


	//湿度
	gTaskL3aqycq20Context.staInstant.a01002_PolId = HCU_L3AQYC_A01002_INDEX;
	gTaskL3aqycq20Context.staInstant.a01002_Flag = 'N';
	ret = dbi_HcuHumidDataInfo_GetAvg(HCU_L3AQYC_STA_1M_REPORT_DURATION,&gTaskL3aqycq20Context.staInstant);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get avg data from Humid database!\n");
	}
	else{
		//HCU_DEBUG_PRINT_INF("L3AQYCG20 get Humid avg data = %4.3f\n", gTaskL3aqycq20Context.staInstant.a01002_Avg);
		gTaskL3aqycq20Context.cur.a01002_Avg = gTaskL3aqycq20Context.staInstant.a01002_Avg;
	}


	//风速
	gTaskL3aqycq20Context.staInstant.a01007_PolId = HCU_L3AQYC_A01007_INDEX;
	gTaskL3aqycq20Context.staInstant.a01007_Flag = 'N';
	ret = dbi_HcuWindspdDataInfo_GetAvg(HCU_L3AQYC_STA_1M_REPORT_DURATION,&gTaskL3aqycq20Context.staInstant);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get avg data from Windspd database!\n");
	}
	else{
		//HCU_DEBUG_PRINT_INF("L3AQYCG20 get Windspd avg data = %4.3f\n", gTaskL3aqycq20Context.staInstant.a01007_Avg);
		gTaskL3aqycq20Context.cur.a01007_Avg = gTaskL3aqycq20Context.staInstant.a01007_Avg;
	}

	//风向
	gTaskL3aqycq20Context.staInstant.a01008_PolId = HCU_L3AQYC_A01008_INDEX;
	gTaskL3aqycq20Context.staInstant.a01008_Flag = 'N';
	ret = dbi_HcuWinddirDataInfo_GetAvg(HCU_L3AQYC_STA_1M_REPORT_DURATION,&gTaskL3aqycq20Context.staInstant);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get avg data from Winddir database!\n");
	}
	else{
		//HCU_DEBUG_PRINT_INF("L3AQYCG20 get Winddir avg data = %4.3f\n", gTaskL3aqycq20Context.staInstant.a01008_Avg);
		gTaskL3aqycq20Context.cur.a01008_Avg = gTaskL3aqycq20Context.staInstant.a01008_Avg;
	}

	//气压
	gTaskL3aqycq20Context.cur.a01006_PolId = HCU_L3AQYC_A01006_INDEX;
	gTaskL3aqycq20Context.cur.a01006_Avg = 0;
	gTaskL3aqycq20Context.cur.a01006_Flag = 'D';

	//超标报警判断并发送
	ret = func_l3aqyc_pm10_noise_value_alarm_judge_2072(&gTaskL3aqycq20Context.staInstant);
	if(FAILURE == ret){
		HcuErrorPrint("L3AQYCG20: Error on 2072 alarm event report!\n");
	}

	//flag值判断
	func_l3aqyc_measurement_value_flag_judge_inst(&gTaskL3aqycq20Context.cur);


	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////



	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//总悬浮物颗粒
	gTaskL3aqycq20Context.staMin.a34001_PolId = HCU_L3AQYC_A34001_INDEX;
	gTaskL3aqycq20Context.staMin.a34001_Flag = 'N';

	ret = dbi_HcuPm25DataInfo_GetMin(HCU_L3AQYC_STA_5M_REPORT_DURATION,&gTaskL3aqycq20Context.staMin);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get min data from PM25 database!\n");
	}
	else{
		HCU_DEBUG_PRINT_INF("L3AQYCG20 get PM10 1M minimum data = %4.3f\n", gTaskL3aqycq20Context.staMin.a34001_Min);
	}

	ret = dbi_HcuPm25DataInfo_GetMax(HCU_L3AQYC_STA_5M_REPORT_DURATION,&gTaskL3aqycq20Context.staMin);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get max data from PM25 database!\n");
	}
	else{
		HCU_DEBUG_PRINT_INF("L3AQYCG20 get PM10 1M maximum data = %4.3f\n", gTaskL3aqycq20Context.staMin.a34001_Max);
	}

	ret = dbi_HcuPm25DataInfo_GetAvg(HCU_L3AQYC_STA_5M_REPORT_DURATION,&gTaskL3aqycq20Context.staMin);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get avg data from PM25 database!\n");
	}
	else{
		HCU_DEBUG_PRINT_INF("L3AQYCG20 get PM10 1M average data = %4.3f\n", gTaskL3aqycq20Context.staMin.a34001_Avg);
	}

	//噪声
	gTaskL3aqycq20Context.staMin.a50001_PolId = HCU_L3AQYC_A50001_INDEX;
	gTaskL3aqycq20Context.staMin.a50001_Flag = 'N';

	ret = dbi_HcuNoiseDataInfo_GetMin(HCU_L3AQYC_STA_5M_REPORT_DURATION,&gTaskL3aqycq20Context.staMin);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get min data from Noise database!\n");
	}
	else{
		HCU_DEBUG_PRINT_INF("L3AQYCG20 get Noise 1M minimum data = %4.3f\n", gTaskL3aqycq20Context.staMin.a50001_Min);
	}

	ret = dbi_HcuNoiseDataInfo_GetMax(HCU_L3AQYC_STA_5M_REPORT_DURATION,&gTaskL3aqycq20Context.staMin);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get max data from Noise database!\n");
	}
	else{
		HCU_DEBUG_PRINT_INF("L3AQYCG20 get Noise 1M maximum data = %4.3f\n", gTaskL3aqycq20Context.staMin.a50001_Max);
	}

	ret = dbi_HcuNoiseDataInfo_GetAvg(HCU_L3AQYC_STA_5M_REPORT_DURATION,&gTaskL3aqycq20Context.staMin);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get avg data from Noise database!\n");
	}
	else{
		HCU_DEBUG_PRINT_INF("L3AQYCG20 get Noise 1M average data = %4.3f\n", gTaskL3aqycq20Context.staMin.a50001_Avg);
	}


	//温度
	gTaskL3aqycq20Context.staMin.a01001_PolId = HCU_L3AQYC_A01001_INDEX;
	gTaskL3aqycq20Context.staMin.a01001_Flag = 'N';

	ret = dbi_HcuTempDataInfo_GetMin(HCU_L3AQYC_STA_5M_REPORT_DURATION,&gTaskL3aqycq20Context.staMin);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get min data from Temp database!\n");
	}
	else{
		HCU_DEBUG_PRINT_INF("L3AQYCG20 get Temp 1M minimum data = %4.3f\n", gTaskL3aqycq20Context.staMin.a01001_Min);
	}

	ret = dbi_HcuTempDataInfo_GetMax(HCU_L3AQYC_STA_5M_REPORT_DURATION,&gTaskL3aqycq20Context.staMin);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get max data from Temp database!\n");
	}
	else{
		HCU_DEBUG_PRINT_INF("L3AQYCG20 get Temp 1M maximum data = %4.3f\n", gTaskL3aqycq20Context.staMin.a01001_Max);
	}

	ret = dbi_HcuTempDataInfo_GetAvg(HCU_L3AQYC_STA_5M_REPORT_DURATION,&gTaskL3aqycq20Context.staMin);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get avg data from Temp database!\n");
	}
	else{
		HCU_DEBUG_PRINT_INF("L3AQYCG20 get Temp 1M average data = %4.3f\n", gTaskL3aqycq20Context.staMin.a01001_Avg);
	}


	//湿度
	gTaskL3aqycq20Context.staMin.a01002_PolId = HCU_L3AQYC_A01002_INDEX;
	gTaskL3aqycq20Context.staMin.a01002_Flag = 'N';

	ret = dbi_HcuHumidDataInfo_GetMin(HCU_L3AQYC_STA_5M_REPORT_DURATION,&gTaskL3aqycq20Context.staMin);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get min data from Humid database!\n");
	}
	else{
		HCU_DEBUG_PRINT_INF("L3AQYCG20 get Humid 1M minimum data = %4.3f\n", gTaskL3aqycq20Context.staMin.a01002_Min);
	}

	ret = dbi_HcuHumidDataInfo_GetMax(HCU_L3AQYC_STA_5M_REPORT_DURATION,&gTaskL3aqycq20Context.staMin);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get max data from Humid database!\n");
	}
	else{
		HCU_DEBUG_PRINT_INF("L3AQYCG20 get Humid 1M maximum data = %4.3f\n", gTaskL3aqycq20Context.staMin.a01002_Max);
	}

	ret = dbi_HcuHumidDataInfo_GetAvg(HCU_L3AQYC_STA_5M_REPORT_DURATION,&gTaskL3aqycq20Context.staMin);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get avg data from Humid database!\n");
	}
	else{
		HCU_DEBUG_PRINT_INF("L3AQYCG20 get Humid 1M average data = %4.3f\n", gTaskL3aqycq20Context.staMin.a01002_Avg);
	}


	//风速
	gTaskL3aqycq20Context.staMin.a01007_PolId = HCU_L3AQYC_A01007_INDEX;
	gTaskL3aqycq20Context.staMin.a01007_Flag = 'N';

	ret = dbi_HcuWindspdDataInfo_GetMin(HCU_L3AQYC_STA_5M_REPORT_DURATION,&gTaskL3aqycq20Context.staMin);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get min data from Windspd database!\n");
	}
	else{
		HCU_DEBUG_PRINT_INF("L3AQYCG20 get Windspd 1M minimum data = %4.3f\n", gTaskL3aqycq20Context.staMin.a01007_Min);
	}

	ret = dbi_HcuWindspdDataInfo_GetMax(HCU_L3AQYC_STA_5M_REPORT_DURATION,&gTaskL3aqycq20Context.staMin);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get Windspd max data from Windspd database!\n");
	}
	else{
		HCU_DEBUG_PRINT_INF("L3AQYCG20 get Windspd 1M maximum data = %4.3f\n", gTaskL3aqycq20Context.staMin.a01007_Max);
	}

	ret = dbi_HcuWindspdDataInfo_GetAvg(HCU_L3AQYC_STA_5M_REPORT_DURATION,&gTaskL3aqycq20Context.staMin);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get avg data from Windspd database!\n");
	}
	else{
		HCU_DEBUG_PRINT_INF("L3AQYCG20 get Windspd 1M average data = %4.3f\n", gTaskL3aqycq20Context.staMin.a01007_Avg);
	}

	//风向
	gTaskL3aqycq20Context.staMin.a01008_PolId = HCU_L3AQYC_A01008_INDEX;
	gTaskL3aqycq20Context.staMin.a01008_Flag = 'N';

	ret = dbi_HcuWinddirDataInfo_GetMin(HCU_L3AQYC_STA_5M_REPORT_DURATION,&gTaskL3aqycq20Context.staMin);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get min data from Winddir database!\n");
	}
	else{
		HCU_DEBUG_PRINT_INF("L3AQYCG20 get Winddir 1M minimum data = %4.3f\n", gTaskL3aqycq20Context.staMin.a01008_Min);
	}

	ret = dbi_HcuWinddirDataInfo_GetMax(HCU_L3AQYC_STA_5M_REPORT_DURATION,&gTaskL3aqycq20Context.staMin);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get max data from Winddir database!\n");
	}
	else{
		HCU_DEBUG_PRINT_INF("L3AQYCG20 get Winddir 1M maximum data = %4.3f\n", gTaskL3aqycq20Context.staMin.a01008_Max);
	}

	ret = dbi_HcuWinddirDataInfo_GetAvg(HCU_L3AQYC_STA_5M_REPORT_DURATION,&gTaskL3aqycq20Context.staMin);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get avg data from Winddir database!\n");
	}
	else{
		HCU_DEBUG_PRINT_INF("L3AQYCG20 get Winddir 1M average data = %4.3f\n", gTaskL3aqycq20Context.staMin.a01008_Avg);
	}

	//气压
	gTaskL3aqycq20Context.staMin.a01006_PolId = HCU_L3AQYC_A01006_INDEX;
	gTaskL3aqycq20Context.staMin.a01006_Avg = 0;
	gTaskL3aqycq20Context.staMin.a01006_Min = 0;
	gTaskL3aqycq20Context.staMin.a01006_Max = 0;
	gTaskL3aqycq20Context.staMin.a01006_Flag = 'D';

	func_l3aqyc_measurement_value_flag_judge_min(&gTaskL3aqycq20Context.staMin);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////



	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//总悬浮物颗粒
	gTaskL3aqycq20Context.staHour.a34001_PolId = HCU_L3AQYC_A34001_INDEX;
	gTaskL3aqycq20Context.staHour.a34001_Flag = 'N';

	ret = dbi_HcuPm25DataInfo_GetMin(HCU_L3AQYC_STA_1H_REPORT_DURATION,&gTaskL3aqycq20Context.staHour);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get min data from PM25 database!\n");
	}
	else{
		//HCU_DEBUG_PRINT_INF("L3AQYCG20 get PM10 1H minimum data = %4.3f\n", gTaskL3aqycq20Context.staHour.a34001_Min);
	}

	ret = dbi_HcuPm25DataInfo_GetMax(HCU_L3AQYC_STA_1H_REPORT_DURATION,&gTaskL3aqycq20Context.staHour);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get max data from PM25 database!\n");
	}
	else{
		//HCU_DEBUG_PRINT_INF("L3AQYCG20 get PM10 1H maximum data = %4.3f\n", gTaskL3aqycq20Context.staHour.a34001_Max);
	}

	ret = dbi_HcuPm25DataInfo_GetAvg(HCU_L3AQYC_STA_1H_REPORT_DURATION,&gTaskL3aqycq20Context.staHour);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get avg data from PM25 database!\n");
	}
	else{
		//HCU_DEBUG_PRINT_INF("L3AQYCG20 get PM10 1H average data = %4.3f\n", gTaskL3aqycq20Context.staHour.a34001_Avg);
	}

	//噪声
	gTaskL3aqycq20Context.staHour.a50001_PolId = HCU_L3AQYC_A50001_INDEX;
	gTaskL3aqycq20Context.staHour.a50001_Flag = 'N';

	ret = dbi_HcuNoiseDataInfo_GetMin(HCU_L3AQYC_STA_1H_REPORT_DURATION,&gTaskL3aqycq20Context.staHour);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get min data from Noise database!\n");
	}
	else{
		//HCU_DEBUG_PRINT_INF("L3AQYCG20 get Noise 1H minimum data = %4.3f\n", gTaskL3aqycq20Context.staHour.a50001_Min);
	}

	ret = dbi_HcuNoiseDataInfo_GetMax(HCU_L3AQYC_STA_1H_REPORT_DURATION,&gTaskL3aqycq20Context.staHour);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get max data from Noise database!\n");
	}
	else{
		//HCU_DEBUG_PRINT_INF("L3AQYCG20 get Noise 1H maximum data = %4.3f\n", gTaskL3aqycq20Context.staHour.a50001_Max);
	}

	ret = dbi_HcuNoiseDataInfo_GetAvg(HCU_L3AQYC_STA_1H_REPORT_DURATION,&gTaskL3aqycq20Context.staHour);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get avg data from Noise database!\n");
	}
	else{
		//HCU_DEBUG_PRINT_INF("L3AQYCG20 get Noise 1H average data = %4.3f\n", gTaskL3aqycq20Context.staHour.a50001_Avg);
	}


	//温度
	gTaskL3aqycq20Context.staHour.a01001_PolId = HCU_L3AQYC_A01001_INDEX;
	gTaskL3aqycq20Context.staHour.a01001_Flag = 'N';

	ret = dbi_HcuTempDataInfo_GetMin(HCU_L3AQYC_STA_1H_REPORT_DURATION,&gTaskL3aqycq20Context.staHour);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get min data from Temp database!\n");
	}
	else{
		//HCU_DEBUG_PRINT_INF("L3AQYCG20 get Temp 1H minimum data = %4.3f\n", gTaskL3aqycq20Context.staHour.a01001_Min);
	}

	ret = dbi_HcuTempDataInfo_GetMax(HCU_L3AQYC_STA_1H_REPORT_DURATION,&gTaskL3aqycq20Context.staHour);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get max data from Temp database!\n");
	}
	else{
		//HCU_DEBUG_PRINT_INF("L3AQYCG20 get Temp 1H maximum data = %4.3f\n", gTaskL3aqycq20Context.staHour.a01001_Max);
	}

	ret = dbi_HcuTempDataInfo_GetAvg(HCU_L3AQYC_STA_1H_REPORT_DURATION,&gTaskL3aqycq20Context.staHour);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get avg data from Temp database!\n");
	}
	else{
		//HCU_DEBUG_PRINT_INF("L3AQYCG20 get Temp 1H average data = %4.3f\n", gTaskL3aqycq20Context.staHour.a01001_Avg);
	}


	//湿度
	gTaskL3aqycq20Context.staHour.a01002_PolId = HCU_L3AQYC_A01002_INDEX;
	gTaskL3aqycq20Context.staHour.a01002_Flag = 'N';

	ret = dbi_HcuHumidDataInfo_GetMin(HCU_L3AQYC_STA_1H_REPORT_DURATION,&gTaskL3aqycq20Context.staHour);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get min data from Humid database!\n");
	}
	else{
		//HCU_DEBUG_PRINT_INF("L3AQYCG20 get Humid 1H minimum data = %4.3f\n", gTaskL3aqycq20Context.staHour.a01002_Min);
	}

	ret = dbi_HcuHumidDataInfo_GetMax(HCU_L3AQYC_STA_1H_REPORT_DURATION,&gTaskL3aqycq20Context.staHour);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get max data from Humid database!\n");
	}
	else{
		//HCU_DEBUG_PRINT_INF("L3AQYCG20 get Humid 1H maximum data = %4.3f\n", gTaskL3aqycq20Context.staHour.a01002_Max);
	}

	ret = dbi_HcuHumidDataInfo_GetAvg(HCU_L3AQYC_STA_1H_REPORT_DURATION,&gTaskL3aqycq20Context.staHour);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get avg data from Humid database!\n");
	}
	else{
		//HCU_DEBUG_PRINT_INF("L3AQYCG20 get Humid 1H average data = %4.3f\n", gTaskL3aqycq20Context.staHour.a01002_Avg);
	}


	//风速
	gTaskL3aqycq20Context.staHour.a01007_PolId = HCU_L3AQYC_A01007_INDEX;
	gTaskL3aqycq20Context.staHour.a01007_Flag = 'N';

	ret = dbi_HcuWindspdDataInfo_GetMin(HCU_L3AQYC_STA_1H_REPORT_DURATION,&gTaskL3aqycq20Context.staHour);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get min data from Windspd database!\n");
	}
	else{
		//HCU_DEBUG_PRINT_INF("L3AQYCG20 get Windspd 1H minimum data = %4.3f\n", gTaskL3aqycq20Context.staHour.a01007_Min);
	}

	ret = dbi_HcuWindspdDataInfo_GetMax(HCU_L3AQYC_STA_1H_REPORT_DURATION,&gTaskL3aqycq20Context.staHour);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get Windspd max data from Windspd database!\n");
	}
	else{
		//HCU_DEBUG_PRINT_INF("L3AQYCG20 get Windspd 1H maximum data = %4.3f\n", gTaskL3aqycq20Context.staHour.a01007_Max);
	}

	ret = dbi_HcuWindspdDataInfo_GetAvg(HCU_L3AQYC_STA_1H_REPORT_DURATION,&gTaskL3aqycq20Context.staHour);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get avg data from Windspd database!\n");
	}
	else{
		//HCU_DEBUG_PRINT_INF("L3AQYCG20 get Windspd 1H average data = %4.3f\n", gTaskL3aqycq20Context.staHour.a01007_Avg);
	}

	//风向
	gTaskL3aqycq20Context.staHour.a01008_PolId = HCU_L3AQYC_A01008_INDEX;
	gTaskL3aqycq20Context.staHour.a01008_Flag = 'N';

	ret = dbi_HcuWinddirDataInfo_GetMin(HCU_L3AQYC_STA_1H_REPORT_DURATION,&gTaskL3aqycq20Context.staHour);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get min data from Winddir database!\n");
	}
	else{
		//HCU_DEBUG_PRINT_INF("L3AQYCG20 get Winddir 1H minimum data = %4.3f\n", gTaskL3aqycq20Context.staHour.a01008_Min);
	}

	ret = dbi_HcuWinddirDataInfo_GetMax(HCU_L3AQYC_STA_1H_REPORT_DURATION,&gTaskL3aqycq20Context.staHour);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get max data from Winddir database!\n");
	}
	else{
		//HCU_DEBUG_PRINT_INF("L3AQYCG20 get Winddir 1H maximum data = %4.3f\n", gTaskL3aqycq20Context.staHour.a01008_Max);
	}

	ret = dbi_HcuWinddirDataInfo_GetAvg(HCU_L3AQYC_STA_1H_REPORT_DURATION,&gTaskL3aqycq20Context.staHour);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get avg data from Winddir database!\n");
	}
	else{
		//HCU_DEBUG_PRINT_INF("L3AQYCG20 get Winddir 1H average data = %4.3f\n", gTaskL3aqycq20Context.staHour.a01008_Avg);
	}

	//气压
	gTaskL3aqycq20Context.staHour.a01006_PolId = HCU_L3AQYC_A01006_INDEX;
	gTaskL3aqycq20Context.staHour.a01006_Avg = 0;
	gTaskL3aqycq20Context.staHour.a01006_Min = 0;
	gTaskL3aqycq20Context.staHour.a01006_Max = 0;
	gTaskL3aqycq20Context.staHour.a01006_Flag = 'D';

	func_l3aqyc_measurement_value_flag_judge_min(&gTaskL3aqycq20Context.staHour);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////





	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//总悬浮物颗粒
	gTaskL3aqycq20Context.staDay.a34001_PolId = HCU_L3AQYC_A34001_INDEX;
	gTaskL3aqycq20Context.staDay.a34001_Flag = 'N';

	ret = dbi_HcuPm25DataInfo_GetMin(HCU_L3AQYC_STA_1D_REPORT_DURATION,&gTaskL3aqycq20Context.staDay);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get min data from PM25 database!\n");
	}
	else{
		//HCU_DEBUG_PRINT_INF("L3AQYCG20 get PM10 1D minimum data = %4.3f\n", gTaskL3aqycq20Context.staDay.a34001_Min);
	}

	ret = dbi_HcuPm25DataInfo_GetMax(HCU_L3AQYC_STA_1D_REPORT_DURATION,&gTaskL3aqycq20Context.staDay);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get max data from PM25 database!\n");
	}
	else{
		//HCU_DEBUG_PRINT_INF("L3AQYCG20 get PM10 1D maximum data = %4.3f\n", gTaskL3aqycq20Context.staDay.a34001_Max);
	}

	ret = dbi_HcuPm25DataInfo_GetAvg(HCU_L3AQYC_STA_1D_REPORT_DURATION,&gTaskL3aqycq20Context.staDay);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get avg data from PM25 database!\n");
	}
	else{
		//HCU_DEBUG_PRINT_INF("L3AQYCG20 get PM10 1D average data = %4.3f\n", gTaskL3aqycq20Context.staDay.a34001_Avg);
	}

	//噪声
	gTaskL3aqycq20Context.staDay.a50001_PolId = HCU_L3AQYC_A50001_INDEX;
	gTaskL3aqycq20Context.staDay.a50001_Flag = 'N';

	ret = dbi_HcuNoiseDataInfo_GetMin(HCU_L3AQYC_STA_1D_REPORT_DURATION,&gTaskL3aqycq20Context.staDay);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get min data from Noise database!\n");
	}
	else{
		//HCU_DEBUG_PRINT_INF("L3AQYCG20 get Noise 1D minimum data = %4.3f\n", gTaskL3aqycq20Context.staDay.a50001_Min);
	}

	ret = dbi_HcuNoiseDataInfo_GetMax(HCU_L3AQYC_STA_1D_REPORT_DURATION,&gTaskL3aqycq20Context.staDay);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get max data from Noise database!\n");
	}
	else{
		//HCU_DEBUG_PRINT_INF("L3AQYCG20 get Noise 1D maximum data = %4.3f\n", gTaskL3aqycq20Context.staDay.a50001_Max);
	}

	ret = dbi_HcuNoiseDataInfo_GetAvg(HCU_L3AQYC_STA_1D_REPORT_DURATION,&gTaskL3aqycq20Context.staDay);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get avg data from Noise database!\n");
	}
	else{
		//HCU_DEBUG_PRINT_INF("L3AQYCG20 get Noise 1D average data = %4.3f\n", gTaskL3aqycq20Context.staDay.a50001_Avg);
	}


	//温度
	gTaskL3aqycq20Context.staDay.a01001_PolId = HCU_L3AQYC_A01001_INDEX;
	gTaskL3aqycq20Context.staDay.a01001_Flag = 'N';

	ret = dbi_HcuTempDataInfo_GetMin(HCU_L3AQYC_STA_1D_REPORT_DURATION,&gTaskL3aqycq20Context.staDay);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get min data from Temp database!\n");
	}
	else{
		//HCU_DEBUG_PRINT_INF("L3AQYCG20 get Temp 1D minimum data = %4.3f\n", gTaskL3aqycq20Context.staHour.a01001_Min);
	}

	ret = dbi_HcuTempDataInfo_GetMax(HCU_L3AQYC_STA_1D_REPORT_DURATION,&gTaskL3aqycq20Context.staDay);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get max data from Temp database!\n");
	}
	else{
		//HCU_DEBUG_PRINT_INF("L3AQYCG20 get Temp 1D maximum data = %4.3f\n", gTaskL3aqycq20Context.staDay.a01001_Max);
	}

	ret = dbi_HcuTempDataInfo_GetAvg(HCU_L3AQYC_STA_1D_REPORT_DURATION,&gTaskL3aqycq20Context.staDay);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get avg data from Temp database!\n");
	}
	else{
		//HCU_DEBUG_PRINT_INF("L3AQYCG20 get Temp 1D average data = %4.3f\n", gTaskL3aqycq20Context.staDay.a01001_Avg);
	}


	//湿度
	gTaskL3aqycq20Context.staDay.a01002_PolId = HCU_L3AQYC_A01002_INDEX;
	gTaskL3aqycq20Context.staDay.a01002_Flag = 'N';

	ret = dbi_HcuHumidDataInfo_GetMin(HCU_L3AQYC_STA_1D_REPORT_DURATION,&gTaskL3aqycq20Context.staDay);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get min data from Humid database!\n");
	}
	else{
		//HCU_DEBUG_PRINT_INF("L3AQYCG20 get Humid 1D minimum data = %4.3f\n", gTaskL3aqycq20Context.staDay.a01002_Min);
	}

	ret = dbi_HcuHumidDataInfo_GetMax(HCU_L3AQYC_STA_1D_REPORT_DURATION,&gTaskL3aqycq20Context.staDay);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get max data from Humid database!\n");
	}
	else{
		//HCU_DEBUG_PRINT_INF("L3AQYCG20 get Humid 1D maximum data = %4.3f\n", gTaskL3aqycq20Context.staDay.a01002_Max);
	}

	ret = dbi_HcuHumidDataInfo_GetAvg(HCU_L3AQYC_STA_1D_REPORT_DURATION,&gTaskL3aqycq20Context.staDay);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get avg data from Humid database!\n");
	}
	else{
		//HCU_DEBUG_PRINT_INF("L3AQYCG20 get Humid 1D average data = %4.3f\n", gTaskL3aqycq20Context.staDay.a01002_Avg);
	}


	//风速
	gTaskL3aqycq20Context.staDay.a01007_PolId = HCU_L3AQYC_A01007_INDEX;
	gTaskL3aqycq20Context.staDay.a01007_Flag = 'N';

	ret = dbi_HcuWindspdDataInfo_GetMin(HCU_L3AQYC_STA_1D_REPORT_DURATION,&gTaskL3aqycq20Context.staDay);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get min data from Windspd database!\n");
	}
	else{
		//HCU_DEBUG_PRINT_INF("L3AQYCG20 get Windspd 1D minimum data = %4.3f\n", gTaskL3aqycq20Context.staDay.a01007_Min);
	}

	ret = dbi_HcuWindspdDataInfo_GetMax(HCU_L3AQYC_STA_1D_REPORT_DURATION,&gTaskL3aqycq20Context.staDay);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get Windspd max data from Windspd database!\n");
	}
	else{
		//HCU_DEBUG_PRINT_INF("L3AQYCG20 get Windspd 1D maximum data = %4.3f\n", gTaskL3aqycq20Context.staDay.a01007_Max);
	}

	ret = dbi_HcuWindspdDataInfo_GetAvg(HCU_L3AQYC_STA_1D_REPORT_DURATION,&gTaskL3aqycq20Context.staDay);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get avg data from Windspd database!\n");
	}
	else{
		//HCU_DEBUG_PRINT_INF("L3AQYCG20 get Windspd 1D average data = %4.3f\n", gTaskL3aqycq20Context.staDay.a01007_Avg);
	}

	//风向
	gTaskL3aqycq20Context.staDay.a01008_PolId = HCU_L3AQYC_A01008_INDEX;
	gTaskL3aqycq20Context.staDay.a01008_Flag = 'N';

	ret = dbi_HcuWinddirDataInfo_GetMin(HCU_L3AQYC_STA_1D_REPORT_DURATION,&gTaskL3aqycq20Context.staDay);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get min data from Winddir database!\n");
	}
	else{
		//HCU_DEBUG_PRINT_INF("L3AQYCG20 get Winddir 1D minimum data = %4.3f\n", gTaskL3aqycq20Context.staDay.a01008_Min);
	}

	ret = dbi_HcuWinddirDataInfo_GetMax(HCU_L3AQYC_STA_1D_REPORT_DURATION,&gTaskL3aqycq20Context.staDay);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get max data from Winddir database!\n");
	}
	else{
		//HCU_DEBUG_PRINT_INF("L3AQYCG20 get Winddir 1D maximum data = %4.3f\n", gTaskL3aqycq20Context.staDay.a01008_Max);
	}

	ret = dbi_HcuWinddirDataInfo_GetAvg(HCU_L3AQYC_STA_1D_REPORT_DURATION,&gTaskL3aqycq20Context.staDay);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Can not get avg data from Winddir database!\n");
	}
	else{
		//HCU_DEBUG_PRINT_INF("L3AQYCG20 get Winddir 1D average data = %4.3f\n", gTaskL3aqycq20Context.staDay.a01008_Avg);
	}

	//气压
	gTaskL3aqycq20Context.staDay.a01006_PolId = HCU_L3AQYC_A01006_INDEX;
	gTaskL3aqycq20Context.staDay.a01006_Avg = 0;
	gTaskL3aqycq20Context.staDay.a01006_Min = 0;
	gTaskL3aqycq20Context.staDay.a01006_Max = 0;
	gTaskL3aqycq20Context.staDay.a01006_Flag = 'D';

	func_l3aqyc_measurement_value_flag_judge_min(&gTaskL3aqycq20Context.staDay);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////




	//实时报告
	if(TRUE == gTaskL3aqycq20Context.InstReportFlag)
	{
		msg_struct_l3mod_llczhb_data_report_t snd;
		memset(&snd, 0, sizeof(msg_struct_l3mod_llczhb_data_report_t));
		snd.actionId = HCU_SYSMSG_ZHBHJT_ACTION_GET_POL_RTD_2011;
		snd.ul2Cloud.DataTime = time(0);
		snd.ul2Cloud.nbrOfCRtd = HCU_SYSMSG_ZHBHJT_POLID_NBR_MAX;

		memcpy(&(snd.ul2Cloud.rtd), &(gTaskL3aqycq20Context.cur), sizeof(msgie_struct_zhbhjt_frame_data_pol_rtd_t)*HCU_SYSMSG_ZHBHJT_POLID_NBR_MAX);

		/*
		//test by shanchun
		int i;
		for(i=0; i< HCU_SYSMSG_ZHBHJT_POLID_NBR_MAX;i++){
			HCU_DEBUG_PRINT_INF("L3AQYCG20: snd.ul2Cloud.rtd[%d].PolId=[%d]\n",i, snd.ul2Cloud.rtd[i].PolId);
			HCU_DEBUG_PRINT_INF("L3AQYCG20: snd.ul2Cloud.rtd[%d].PolFlag=[%c]\n",i, snd.ul2Cloud.rtd[i].PolFlag);
			HCU_DEBUG_PRINT_INF("L3AQYCG20: snd.ul2Cloud.rtd[%d].Rtd=[%4.2f]\n",i, snd.ul2Cloud.rtd[i].Rtd);
		}
		*/

		snd.length = sizeof(msg_struct_l3mod_llczhb_data_report_t);

		ret = hcu_message_send(MSG_ID_L3MOD_LLCZHB_DATA_REPORT, TASK_ID_LLCZHB, TASK_ID_L3AQYCG20, &snd, snd.length);
		if (ret == FAILURE){
			HCU_ERROR_PRINT_L3AQYCG20("L3AQYCG20: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3AQYCG20].taskName, zHcuVmCtrTab.task[TASK_ID_LLCZHB].taskName);
		}

	}

/*
	//报警记录上报
	if(TRUE == gTaskL3aqycq20Context.AlaReportFlag)
	{
		if ( (gTaskL3aqycq20Context.cur.a34001_Avg >= HCU_SENSOR_PM25_VALUE_ALARM_THRESHOLD) || (gTaskL3aqycq20Context.cur.a50001_Avg >= HCU_SENSOR_NOISE_VALUE_ALARM_THRESHOLD))
		{
			msg_struct_l3mod_llczhb_data_report_t snd;
			memset(&snd, 0, sizeof(msg_struct_l3mod_llczhb_data_report_t));
			snd.actionId = HCU_SYSMSG_ZHBHJT_ACTION_GET_POL_ALA_2071;
			snd.ul2Cloud.DataTime = time(0);
			snd.length = sizeof(msg_struct_l3mod_llczhb_data_report_t);

			if (gTaskL3aqycq20Context.cur.a34001_Avg >= HCU_SENSOR_PM25_VALUE_ALARM_THRESHOLD){
				snd.ul2Cloud.Ala.PolId = gTaskL3aqycq20Context.cur.a34001_PolId;
			    snd.ul2Cloud.Ala.Ala = gTaskL3aqycq20Context.cur.a34001_Avg;
			}
			else if (gTaskL3aqycq20Context.cur.a50001_Avg >= HCU_SENSOR_NOISE_VALUE_ALARM_THRESHOLD){
				snd.ul2Cloud.Ala.PolId = gTaskL3aqycq20Context.cur.a50001_PolId;
			    snd.ul2Cloud.Ala.Ala = gTaskL3aqycq20Context.cur.a50001_Avg;
			}

			ret = hcu_message_send(MSG_ID_L3MOD_LLCZHB_DATA_REPORT, TASK_ID_LLCZHB, TASK_ID_L3AQYCG20, &snd, snd.length);
			if (ret == FAILURE){
				HCU_ERROR_PRINT_L3AQYCG20("L3AQYCG20: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3AQYCG20].taskName, zHcuVmCtrTab.task[TASK_ID_LLCZHB].taskName);
			}
		}
	}
*/


	//分钟统计表更新数据库
	//if (dbi_Hcu_StaDatainfo_save(HCU_L3AQYC_STA_DBI_TABLE_1MIN, &(gTaskL3aqycq20Context.staMin)) == FAILURE)
			//HCU_ERROR_PRINT_L3BFSC("L3AQYC: Save data to DB error!\n");


	//分钟统计表发送统计报告
	if ((gTaskL3aqycq20Context.elipseCnt % HCU_L3AQYC_STA_1M_REPORT_CYCLE) == 0)
	{
		//if((FALSE == gTaskL3aqycq20Context.MinReportFlag) || (TRUE == gTaskL3aqycq20Context.MinReportFlag))//always on for test
		if(TRUE == gTaskL3aqycq20Context.MinReportFlag)
		{
			msg_struct_l3mod_llczhb_data_report_t snd;
			memset(&snd, 0, sizeof(msg_struct_l3mod_llczhb_data_report_t));
			snd.actionId = HCU_SYSMSG_ZHBHJT_ACTION_GET_POL_MIN_RPT_2051;
			snd.ul2Cloud.DataTime = time(0);
			snd.ul2Cloud.nbrOfCMinRpt = HCU_SYSMSG_ZHBHJT_POLID_NBR_MAX;

			memcpy(&(snd.ul2Cloud.min), &(gTaskL3aqycq20Context.staMin), sizeof(msgie_struct_zhbhjt_frame_data_pol_min_hour_t)*HCU_SYSMSG_ZHBHJT_POLID_NBR_MAX);

			//test by shanchun
			/*
			int i;
			for(i=0; i< HCU_SYSMSG_ZHBHJT_POLID_NBR_MAX;i++){
				HCU_DEBUG_PRINT_INF("L3AQYCG20: snd.ul2Cloud.min[%d].PolId=[%d]\n",i, snd.ul2Cloud.min[i].PolId);
				HCU_DEBUG_PRINT_INF("L3AQYCG20: snd.ul2Cloud.min[%d].PolFlag=[%c]\n",i, snd.ul2Cloud.min[i].PolFlag);
				HCU_DEBUG_PRINT_INF("L3AQYCG20: snd.ul2Cloud.min[%d].Avg=[%4.2f]\n",i, snd.ul2Cloud.min[i].Avg);
				HCU_DEBUG_PRINT_INF("L3AQYCG20: snd.ul2Cloud.min[%d].Max=[%4.2f]\n",i, snd.ul2Cloud.min[i].Max);
				HCU_DEBUG_PRINT_INF("L3AQYCG20: snd.ul2Cloud.min[%d].Min=[%4.2f]\n\n\n",i, snd.ul2Cloud.min[i].Min);
			}
			*/

			snd.length = sizeof(msg_struct_l3mod_llczhb_data_report_t);

			ret = hcu_message_send(MSG_ID_L3MOD_LLCZHB_DATA_REPORT, TASK_ID_LLCZHB, TASK_ID_L3AQYCG20, &snd, snd.length);
			if (ret == FAILURE){
				HCU_ERROR_PRINT_L3AQYCG20("L3AQYCG20: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3AQYCG20].taskName, zHcuVmCtrTab.task[TASK_ID_LLCZHB].taskName);
			}
		}
	}

	//小时统计表发送统计报告
	if ((gTaskL3aqycq20Context.elipseCnt % HCU_L3AQYC_STA_1H_REPORT_CYCLE) == 0)
	{
		//if((FALSE == gTaskL3aqycq20Context.HourReportFlag) || (TRUE == gTaskL3aqycq20Context.HourReportFlag))//always on for test
		if(TRUE == gTaskL3aqycq20Context.HourReportFlag)
		{
			msg_struct_l3mod_llczhb_data_report_t snd;
			memset(&snd, 0, sizeof(msg_struct_l3mod_llczhb_data_report_t));
			snd.actionId = HCU_SYSMSG_ZHBHJT_ACTION_GET_POL_HOUR_RPT_2061;
			snd.ul2Cloud.DataTime = time(0);
			snd.ul2Cloud.nbrOfCMinRpt = HCU_SYSMSG_ZHBHJT_POLID_NBR_MAX;

			memcpy(&(snd.ul2Cloud.min), &(gTaskL3aqycq20Context.staHour), sizeof(msgie_struct_zhbhjt_frame_data_pol_min_hour_t)*HCU_SYSMSG_ZHBHJT_POLID_NBR_MAX);

			/*
			//test by shanchun
			int i;
			for(i=0; i< HCU_SYSMSG_ZHBHJT_POLID_NBR_MAX;i++){
				HCU_DEBUG_PRINT_INF("L3AQYCG20: snd.ul2Cloud.min[%d].PolId=[%d]\n",i, snd.ul2Cloud.min[i].PolId);
				HCU_DEBUG_PRINT_INF("L3AQYCG20: snd.ul2Cloud.min[%d].PolFlag=[%c]\n",i, snd.ul2Cloud.min[i].PolFlag);
				HCU_DEBUG_PRINT_INF("L3AQYCG20: snd.ul2Cloud.min[%d].Avg=[%4.2f]\n",i, snd.ul2Cloud.min[i].Avg);
				HCU_DEBUG_PRINT_INF("L3AQYCG20: snd.ul2Cloud.min[%d].Max=[%4.2f]\n",i, snd.ul2Cloud.min[i].Max);
				HCU_DEBUG_PRINT_INF("L3AQYCG20: snd.ul2Cloud.min[%d].Min=[%4.2f]\n\n\n",i, snd.ul2Cloud.min[i].Min);
			}
			*/

			snd.length = sizeof(msg_struct_l3mod_llczhb_data_report_t);

			ret = hcu_message_send(MSG_ID_L3MOD_LLCZHB_DATA_REPORT, TASK_ID_LLCZHB, TASK_ID_L3AQYCG20, &snd, snd.length);
			if (ret == FAILURE){
				HCU_ERROR_PRINT_L3AQYCG20("L3AQYCG20: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3AQYCG20].taskName, zHcuVmCtrTab.task[TASK_ID_LLCZHB].taskName);
			}
		}
	}

	//天统计表发送统计报告
	if ((gTaskL3aqycq20Context.elipseCnt % HCU_L3AQYC_STA_1D_REPORT_CYCLE) == 0)
	{
		//if((FALSE == gTaskL3aqycq20Context.DayReportFlag) || (TRUE == gTaskL3aqycq20Context.DayReportFlag))//always on for test
		if(TRUE == gTaskL3aqycq20Context.DayReportFlag)
		{
			msg_struct_l3mod_llczhb_data_report_t snd;
			memset(&snd, 0, sizeof(msg_struct_l3mod_llczhb_data_report_t));
			snd.actionId = HCU_SYSMSG_ZHBHJT_ACTION_GET_POL_DAY_RPT_2031;
			snd.ul2Cloud.DataTime = time(0);
			snd.ul2Cloud.nbrOfCMinRpt = HCU_SYSMSG_ZHBHJT_POLID_NBR_MAX;

			memcpy(&(snd.ul2Cloud.min), &(gTaskL3aqycq20Context.staDay), sizeof(msgie_struct_zhbhjt_frame_data_pol_min_hour_t)*HCU_SYSMSG_ZHBHJT_POLID_NBR_MAX);

			/*
			//test by shanchun
			int i;
			for(i=0; i< HCU_SYSMSG_ZHBHJT_POLID_NBR_MAX;i++){
				HCU_DEBUG_PRINT_INF("L3AQYCG20: snd.ul2Cloud.min[%d].PolId=[%d]\n",i, snd.ul2Cloud.min[i].PolId);
				HCU_DEBUG_PRINT_INF("L3AQYCG20: snd.ul2Cloud.min[%d].PolFlag=[%c]\n",i, snd.ul2Cloud.min[i].PolFlag);
				HCU_DEBUG_PRINT_INF("L3AQYCG20: snd.ul2Cloud.min[%d].Avg=[%4.2f]\n",i, snd.ul2Cloud.min[i].Avg);
				HCU_DEBUG_PRINT_INF("L3AQYCG20: snd.ul2Cloud.min[%d].Max=[%4.2f]\n",i, snd.ul2Cloud.min[i].Max);
				HCU_DEBUG_PRINT_INF("L3AQYCG20: snd.ul2Cloud.min[%d].Min=[%4.2f]\n\n\n",i, snd.ul2Cloud.min[i].Min);
			}
			*/

			snd.length = sizeof(msg_struct_l3mod_llczhb_data_report_t);

			ret = hcu_message_send(MSG_ID_L3MOD_LLCZHB_DATA_REPORT, TASK_ID_LLCZHB, TASK_ID_L3AQYCG20, &snd, snd.length);
			if (ret == FAILURE){
				HCU_ERROR_PRINT_L3AQYCG20("L3AQYCG20: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3AQYCG20].taskName, zHcuVmCtrTab.task[TASK_ID_LLCZHB].taskName);
			}
		}
	}


	//HCU_DEBUG_PRINT_INF("L3AQYCG20 aggregation report to default cloud\n\n\n\n");

	//使用HUITP发送MSG_ID_YCJK_CLOUDVELA_DATA_REPORT报告给后台
	//
	if (((gTaskL3aqycq20Context.elipseCnt % HCU_L3AQYC_STA_1M_REPORT_CYCLE) == 0) && (HCU_SYSCFG_SENSOR_REPORT_MODE_SET == HCU_SYSCFG_SENSOR_REPORT_MODE_GROUP)){

	    HCU_DEBUG_PRINT_INF("L3AQYCG20 start aggregation report to default cloud\n\n\n");

		//发送
		msg_struct_ycjk_cloudvela_data_report_t snd;
		memset(&snd, 0, sizeof(msg_struct_ycjk_cloudvela_data_report_t));
		//strncpy(snd.comHead.destUser, zHcuSysEngPar.cloud.svrNameHome, strlen(zHcuSysEngPar.cloud.svrNameHome)<\
			//sizeof(snd.comHead.destUser)?strlen(zHcuSysEngPar.cloud.svrNameHome):sizeof(snd.comHead.destUser));
		strncpy(snd.comHead.destUser, zHcuSysEngPar.cloud.svrNameDefault, strlen(zHcuSysEngPar.cloud.svrNameDefault)<\
			sizeof(snd.comHead.destUser)?strlen(zHcuSysEngPar.cloud.svrNameDefault):sizeof(snd.comHead.destUser));
		strncpy(snd.comHead.srcUser, zHcuSysEngPar.hwBurnId.equLable, strlen(zHcuSysEngPar.hwBurnId.equLable)<\
				sizeof(snd.comHead.srcUser)?strlen(zHcuSysEngPar.hwBurnId.equLable):sizeof(snd.comHead.srcUser));
		snd.comHead.timeStamp = time(0);
		snd.comHead.msgType = HUITP_MSG_HUIXML_MSGTYPE_COMMON_ID;
		strcpy(snd.comHead.funcFlag, "0");

		//CONTENT: Adjust later:TBD
		snd.baseReport = HUITP_IEID_UNI_COM_REPORT_YES;
		snd.ycjk.equipid = 1;
		if(gTaskL3aqycq20Context.staMin.a01001_Avg > HCU_L3AQYC_A01001_RANGE_MAX){
			gTaskL3aqycq20Context.staMin.a01001_Avg = HCU_L3AQYC_A01001_RANGE_MAX - 20;
		}
		if(gTaskL3aqycq20Context.staMin.a01002_Avg > HCU_L3AQYC_A01002_RANGE_MAX){
			gTaskL3aqycq20Context.staMin.a01002_Avg = HCU_L3AQYC_A01002_RANGE_MAX - 20;
		}
		if(gTaskL3aqycq20Context.staMin.a01007_Avg > HCU_L3AQYC_A01008_RANGE_MAX){
			gTaskL3aqycq20Context.staMin.a01007_Avg = HCU_L3AQYC_A01008_RANGE_MAX -25;
		}
		if(gTaskL3aqycq20Context.staMin.a01008_Avg > HCU_L3AQYC_A01008_RANGE_MAX){
			gTaskL3aqycq20Context.staMin.a01008_Avg = HCU_L3AQYC_A01008_RANGE_MAX;
		}
		if(gTaskL3aqycq20Context.staMin.a34001_Avg > HCU_L3AQYC_A34001_RANGE_MAX){
			gTaskL3aqycq20Context.staMin.a34001_Avg = HCU_L3AQYC_A34001_RANGE_MAX;
		}
		if(gTaskL3aqycq20Context.staMin.a50001_Avg > HCU_L3AQYC_A50001_RANGE_MAX){
			gTaskL3aqycq20Context.staMin.a50001_Avg = HCU_L3AQYC_A50001_RANGE_MAX - 20;
		}


		snd.ycjk.tempValue = HCU_SYSMSG_NB_MICROS_IN_ONE_MS*gTaskL3aqycq20Context.staMin.a01001_Avg;
		snd.ycjk.humidValue = HCU_SYSMSG_NB_MICROS_IN_ONE_MS*gTaskL3aqycq20Context.staMin.a01002_Avg;
		snd.ycjk.winddirValue = HCU_SYSMSG_NB_MICROS_IN_ONE_MS*gTaskL3aqycq20Context.staMin.a01008_Avg;
		snd.ycjk.windspdValue = HCU_SYSMSG_NB_MICROS_IN_ONE_MS*gTaskL3aqycq20Context.staMin.a01007_Avg;
		snd.ycjk.tspValue = HCU_SYSMSG_NB_MICROS_IN_ONE_MS*gTaskL3aqycq20Context.staMin.a34001_Avg;
		snd.ycjk.pm1d0Value = HCU_SYSMSG_NB_MICROS_IN_ONE_MS*gTaskL3aqycq20Context.staMin.a34001_Avg;
		snd.ycjk.pm2d5Value = HCU_SYSMSG_NB_MICROS_IN_ONE_MS*gTaskL3aqycq20Context.staMin.a34001_Avg;//to be update later
		snd.ycjk.pm10Value = HCU_SYSMSG_NB_MICROS_IN_ONE_MS*gTaskL3aqycq20Context.staMin.a34001_Avg;//to be update later
		snd.ycjk.noiseValue = HCU_SYSMSG_NB_MICROS_IN_ONE_MS*gTaskL3aqycq20Context.staMin.a50001_Avg;
		snd.ycjk.dataFormat = CLOUD_SENSOR_DATA_FOMAT_FLOAT_WITH_NF3; //no need, caculate directly for l3aqycg20??
		snd.ycjk.timeStamp = time(0);

		HCU_DEBUG_PRINT_INF("L3AQYCG20: Temp=%.1f, Humid=%.1f, Windir=%.1f, Windspd=%.1f, tspValue=%.1f, pm2d5Value=%.1f, pm10Value=%.1f, noiseValue=%.1f\n",gTaskL3aqycq20Context.staMin.a01001_Avg, gTaskL3aqycq20Context.staMin.a01002_Avg, gTaskL3aqycq20Context.staMin.a01008_Avg, gTaskL3aqycq20Context.staMin.a01007_Avg, gTaskL3aqycq20Context.staMin.a34001_Avg, gTaskL3aqycq20Context.staMin.a34001_Avg, gTaskL3aqycq20Context.staMin.a34001_Avg, gTaskNoiseContext.noiseValue);

		HCU_DEBUG_PRINT_INF("L3AQYCG20: Temp=%.1f, Humid=%.1f, Windir=%.1f, Windspd=%.1f, tspValue=%.1f, pm2d5Value=%.1f, pm10Value=%.1f, noiseValue=%.1f\n",gTaskL3aqycq20Context.staMin.a01001_Avg, gTaskL3aqycq20Context.staMin.a01002_Avg, gTaskL3aqycq20Context.staMin.a01008_Avg, gTaskL3aqycq20Context.staMin.a01007_Avg, gTaskL3aqycq20Context.staMin.a34001_Avg, gTaskL3aqycq20Context.staMin.a34001_Avg, gTaskL3aqycq20Context.staMin.a34001_Avg, gTaskL3aqycq20Context.staMin.a50001_Avg);
		HCU_DEBUG_PRINT_INF("L3AQYCG20: Temp=%d, Humid=%d, Windir=%d, Windspd=%d, tspValue=%d, pm2d5Value=%d, pm10Value=%d, noiseValue=%d\n\n\n",snd.ycjk.tempValue, snd.ycjk.humidValue, snd.ycjk.winddirValue, snd.ycjk.windspdValue, snd.ycjk.tspValue, snd.ycjk.pm2d5Value, snd.ycjk.pm10Value, snd.ycjk.noiseValue);



		////////////////////////////////////////////////////////////////////////////////////////////For 中航LED test
		if (zHcuSysEngPar.hwBurnId.hwType == HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G2_AQYC_RASP_2009)
		{
			int ret=0;

			//对信息进行ZH系列字符控制卡协议的编码
			memset(&currentLedBuf, 0, sizeof(SerialLedMsgBuf_t));


			//对发送数据进行编码
	//////////////////////////////////////////////////////////
			//wchar_t *chinese_str = L"扬尘监控系统";
			wchar_t *chinese_str = L"欢迎习总光临小慧智能  噪声：";
			unsigned int *p_chinese = (wchar_t*)chinese_str;

			wchar_t *tsp_str = L" dB    TSP：";
			unsigned int *p_tsp = (wchar_t*)tsp_str;

			wchar_t *temp_str = L" ug/m3    温度：";
			unsigned int *p_temp = (wchar_t*)temp_str;

			wchar_t *humid_str = L"℃    湿度：";
			unsigned int *p_humid = (wchar_t*)humid_str;


			wchar_t *windspd_str = L" RH%    风速：";
			unsigned int *p_windspd = (wchar_t*)windspd_str;

			wchar_t *winddir_str = L" 米/秒    风向：";
			unsigned int *p_winddir = (wchar_t*)winddir_str;


			//float noise = (unsigned int)gTaskL3aqycq20Context.staMin.a50001_Avg;
			float noise = gTaskNoiseContext.noiseValue;
			char CStr[HCU_SYSDIM_MSG_BODY_LEN_MAX];
			memset(&CStr, 0, sizeof(HCU_SYSDIM_MSG_BODY_LEN_MAX));
			//itoa(number,string,16);
			sprintf(CStr,"%.1f",noise);
			printf("integer = %f CStr = %s\n", noise,CStr);

		   //把char*转换为wchar_t*
			size_t len = strlen(CStr) + 1;
			wchar_t *noise_str;
			noise_str=(wchar_t*)malloc(len*sizeof(wchar_t));
			mbstowcs(noise_str,CStr,len);

			unsigned int *p_noise = (wchar_t*)noise_str;


			///////////////////////////////////////////////////
			float tsp = gTaskL3aqycq20Context.staMin.a34001_Avg;
			//char CStr[HCU_SYSDIM_MSG_BODY_LEN_MAX];
			memset(&CStr, 0, sizeof(HCU_SYSDIM_MSG_BODY_LEN_MAX));
			//itoa(number,string,16);
			sprintf(CStr,"%.1f",tsp);
			printf("integer = %f CStr = %s\n", tsp,CStr);

		   //把char*转换为wchar_t*
			len = strlen(CStr) + 1;
			wchar_t *tsp_value_str;
			tsp_value_str=(wchar_t*)malloc(len*sizeof(wchar_t));
			mbstowcs(tsp_value_str,CStr,len);

			unsigned int *p_tsp_value = (wchar_t*)tsp_value_str;
			///////////////////////////////////////////////////

			///////////////////////////////////////////////////
			float temp = gTaskL3aqycq20Context.staMin.a01001_Avg;
			//char CStr[HCU_SYSDIM_MSG_BODY_LEN_MAX];
			memset(&CStr, 0, sizeof(HCU_SYSDIM_MSG_BODY_LEN_MAX));
			//itoa(number,string,16);
			sprintf(CStr,"%.1f",temp);
			printf("integer = %f CStr = %s\n", temp,CStr);

		   //把char*转换为wchar_t*
			len = strlen(CStr) + 1;
			wchar_t *temp_value_str;
			temp_value_str=(wchar_t*)malloc(len*sizeof(wchar_t));
			mbstowcs(temp_value_str,CStr,len);

			unsigned int *p_temp_value = (wchar_t*)temp_value_str;
			///////////////////////////////////////////////////


			///////////////////////////////////////////////////
			float humid = gTaskL3aqycq20Context.staMin.a01002_Avg;
			//char CStr[HCU_SYSDIM_MSG_BODY_LEN_MAX];
			memset(&CStr, 0, sizeof(HCU_SYSDIM_MSG_BODY_LEN_MAX));
			//itoa(number,string,16);
			sprintf(CStr,"%.1f",humid);
			printf("integer = %f CStr = %s\n", humid,CStr);

		   //把char*转换为wchar_t*
			len = strlen(CStr) + 1;
			wchar_t *humid_value_str;
			humid_value_str=(wchar_t*)malloc(len*sizeof(wchar_t));
			mbstowcs(humid_value_str,CStr,len);

			unsigned int *p_humid_value = (wchar_t*)humid_value_str;
			///////////////////////////////////////////////////

			///////////////////////////////////////////////////
			float windspd = gTaskL3aqycq20Context.staMin.a01007_Avg;
			//char CStr[HCU_SYSDIM_MSG_BODY_LEN_MAX];
			memset(&CStr, 0, sizeof(HCU_SYSDIM_MSG_BODY_LEN_MAX));
			//itoa(number,string,16);
			sprintf(CStr,"%.1f",windspd);
			printf("integer = %f CStr = %s\n", windspd,CStr);

		   //把char*转换为wchar_t*
			len = strlen(CStr) + 1;
			wchar_t *windspd_value_str;
			windspd_value_str=(wchar_t*)malloc(len*sizeof(wchar_t));
			mbstowcs(windspd_value_str,CStr,len);

			unsigned int *p_windspd_value = (wchar_t*)windspd_value_str;
			///////////////////////////////////////////////////

			///////////////////////////////////////////////////
			unsigned int winddir = (unsigned int)gTaskL3aqycq20Context.staMin.a01008_Avg;
			wchar_t *winddir_value_str;
			if(winddir>=0 && winddir<=11){
				winddir_value_str = L"北";
			}
			else if(winddir>349 && winddir<=360){
				winddir_value_str = L"北";
			}

			else if(winddir>11 && winddir<=34){
				winddir_value_str = L"东北偏北";
			}

			else if(winddir>34 && winddir<=56){
				winddir_value_str = L"东北";
			}

			else if(winddir>56 && winddir<=79){
				winddir_value_str = L"东北偏东";
			}

			else if(winddir>79 && winddir<=101){
				winddir_value_str = L"东";
			}

			else if(winddir>101 && winddir<=124){
				winddir_value_str = L"东南偏东";
			}

			else if(winddir>124 && winddir<=146){
				winddir_value_str = L"东南";
			}

			else if(winddir>146 && winddir<=169){
				winddir_value_str = L"东南偏南";
			}

			else if(winddir>169 && winddir<=191){
				winddir_value_str = L"南";
			}

			else if(winddir>191 && winddir<=214){
				winddir_value_str = L"西南偏南";
			}

			else if(winddir>214 && winddir<=236){
				winddir_value_str = L"西南";
			}

			else if(winddir>236 && winddir<=259){
				winddir_value_str = L"西南偏西";
			}

			else if(winddir>259 && winddir<=281){
				winddir_value_str = L"西";
			}

			else if(winddir>281 && winddir<=304){
				winddir_value_str = L"西北偏西";
			}

			else if(winddir>304 && winddir<=326){
				winddir_value_str = L"西北";
			}

			else if(winddir>326 && winddir<=349){
				winddir_value_str = L"西北偏北";
			}

			unsigned int *p_winddir_value = (wchar_t*)winddir_value_str;

			///////////////////////////////////////////////////

			printf("Unicode: \n");
			printf("Chinese str len: %d\n",wcslen(chinese_str));

			unsigned int i = 0;

			for(i=0;i<wcslen(chinese_str);i++)
			{
				printf("0x%x  ",p_chinese[i]);
			}
			printf("\n");


			printf("Noise str len: %d\n",wcslen(noise_str));
			for(i=0;i<wcslen(noise_str);i++)
			{
				printf("0x%x  ",p_noise[i]);
			}
			printf("\n");

			printf("tsp value len: %d\n",wcslen(tsp_value_str));
			for(i=0;i<wcslen(tsp_value_str);i++)
			{
				printf("0x%x  ",p_tsp_value[i]);
			}
			printf("\n\n\n\n\n");


			unsigned int curLen = 228;

			unsigned char sample1[] = {0x7A,0x01,0x00,0x06,0xFA,0x00,0x01,0x01,0x01,0x0C,0x05,0x03,0x01,0x00,0x3C,0x01,0x01,0x00,0x00,0x7F,0x0A,0x00,0x01,0x00,0x00,0x00,
					0x00,0x00,0x80,0x00,0x10,0x00,0x01,0x04,0x04,0x10,0xF4,0x01,0x00,0x00,0x05,0x64,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
					0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
					0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
					0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
					0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
					0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
					0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x5C,0x5A,0x00,0x5C,0x54,0x01};
			//"小慧智能科技   噪声： "    0x0f,0x61,0x67,0x66,0x7a,0x80,0xfd,0x79,0xd1,0x62,0x80,0x20,0x20,0x20,0x56,0x6a,0x58,0xf0,0xff,0x1a,0x20

			//unsigned int curLen2 = 26;
			//unsigned char sample2[] = {0x4E,0x0A,0x6D,0x77,0x5C,0xA9,0x76,0xDB,0x75,0x35,0x5B,0x50,0x5B,0x57,0x7B,0x26,0x53,0x61,0x6F,0x14,0x79,0x3A,0x7A,0x0B,0x5E,0x8F};
			unsigned char  curBuf[HCU_SYSDIM_MSG_BODY_LEN_MAX];
			memset(&curBuf, 0, sizeof(HCU_SYSDIM_MSG_BODY_LEN_MAX));
			memcpy(curBuf, sample1, curLen);

		/*
			memcpy(&curBuf[curLen], sample2, curLen2);

			curLen = curLen+curLen2;

			printf("\n");

			for(i=0;i<curLen;i++)
			{

				//curBuf[i] = p[i-curLen1];
				printf("%02x  ",curBuf[i]);

			}

			printf("\n\n");
		*/

			unsigned int j = 0;
		/////////////////////////////////////////
			for(i=0;i<wcslen(chinese_str); i++)
			{
				curBuf[curLen+j] = p_chinese[i] >> 8;
				printf("%02x  ",curBuf[curLen+j]);
				curBuf[curLen+j+1] = p_chinese[i];
				printf("%02x  ",curBuf[curLen+j+1]);
				j=j+2;
			}

			printf("\n");
			curLen = curLen+2*wcslen(chinese_str);


			for(i=0;i<curLen;i++)
			{

				printf("%02x  ",curBuf[i]);
			}
			printf("\n");
		/////////////////////////////////////////

		/////////////////////////////////////////
			j = 0;
			for(i=0;i<wcslen(noise_str); i++)
			{
				curBuf[curLen+j] = p_noise[i] >> 8;
				printf("%02x  ",curBuf[curLen+j]);
				curBuf[curLen+j+1] = p_noise[i];
				printf("%02x  ",curBuf[curLen+j+1]);
				j=j+2;
			}

			printf("\n");
			curLen = curLen+2*wcslen(noise_str);


			for(i=0;i<curLen;i++)
			{

				printf("%02x  ",curBuf[i]);
			}
			printf("\n");
		/////////////////////////////////////////

		/////////////////////////////////////////
			j = 0;
			for(i=0;i<wcslen(tsp_str); i++)
			{
				curBuf[curLen+j] = p_tsp[i] >> 8;
				printf("%02x  ",curBuf[curLen+j]);
				curBuf[curLen+j+1] = p_tsp[i];
				printf("%02x  ",curBuf[curLen+j+1]);
				j=j+2;
			}

			printf("\n");
			curLen = curLen+2*wcslen(tsp_str);


			for(i=0;i<curLen;i++)
			{

				printf("%02x  ",curBuf[i]);
			}
			printf("\n");
		/////////////////////////////////////////

		/////////////////////////////////////////
			j = 0;
			for(i=0;i<wcslen(tsp_value_str); i++)
			{
				curBuf[curLen+j] = p_tsp_value[i] >> 8;
				printf("%02x  ",curBuf[curLen+j]);
				curBuf[curLen+j+1] = p_tsp_value[i];
				printf("%02x  ",curBuf[curLen+j+1]);
				j=j+2;
			}

			printf("\n\n\n\n\n\n\n\n\n");
			curLen = curLen+2*wcslen(tsp_value_str);


			for(i=0;i<curLen;i++)
			{

				printf("%02x  ",curBuf[i]);
			}
			printf("\n");
		/////////////////////////////////////////


		/////////////////////////////////////////
			j = 0;
			for(i=0;i<wcslen(temp_str); i++)
			{
				curBuf[curLen+j] = p_temp[i] >> 8;
				printf("%02x  ",curBuf[curLen+j]);
				curBuf[curLen+j+1] = p_temp[i];
				printf("%02x  ",curBuf[curLen+j+1]);
				j=j+2;
			}

			printf("\n");
			curLen = curLen+2*wcslen(temp_str);


			for(i=0;i<curLen;i++)
			{

				printf("%02x  ",curBuf[i]);
			}
			printf("\n");
		/////////////////////////////////////////

		/////////////////////////////////////////
			j = 0;
			for(i=0;i<wcslen(temp_value_str); i++)
			{
				curBuf[curLen+j] = p_temp_value[i] >> 8;
				printf("%02x  ",curBuf[curLen+j]);
				curBuf[curLen+j+1] = p_temp_value[i];
				printf("%02x  ",curBuf[curLen+j+1]);
				j=j+2;
			}

			printf("\n\n\n\n\n\n\n\n\n");
			curLen = curLen+2*wcslen(temp_value_str);

			for(i=0;i<curLen;i++)
			{

				printf("%02x  ",curBuf[i]);
			}
			printf("\n");
		/////////////////////////////////////////


		/////////////////////////////////////////
			j = 0;
			for(i=0;i<wcslen(humid_str); i++)
			{
				curBuf[curLen+j] = p_humid[i] >> 8;
				printf("%02x  ",curBuf[curLen+j]);
				curBuf[curLen+j+1] = p_humid[i];
				printf("%02x  ",curBuf[curLen+j+1]);
				j=j+2;
			}

			printf("\n");
			curLen = curLen+2*wcslen(humid_str);


			for(i=0;i<curLen;i++)
			{
				printf("%02x  ",curBuf[i]);
			}
			printf("\n");
		/////////////////////////////////////////

		/////////////////////////////////////////
			j = 0;
			for(i=0;i<wcslen(humid_value_str); i++)
			{

				curBuf[curLen+j] = p_humid_value[i] >> 8;
				printf("%02x  ",curBuf[curLen+j]);
				curBuf[curLen+j+1] = p_humid_value[i];
				printf("%02x  ",curBuf[curLen+j+1]);
				j=j+2;
			}

			printf("\n\n\n\n\n\n\n\n\n");
			curLen = curLen+2*wcslen(humid_value_str);


			for(i=0;i<curLen;i++)
			{

				printf("%02x  ",curBuf[i]);
			}
			printf("\n");
		/////////////////////////////////////////


		/////////////////////////////////////////
			j = 0;
			for(i=0;i<wcslen(windspd_str); i++)
			{

				curBuf[curLen+j] = p_windspd[i] >> 8;
				printf("%02x  ",curBuf[curLen+j]);
				curBuf[curLen+j+1] = p_windspd[i];
				printf("%02x  ",curBuf[curLen+j+1]);
				j=j+2;
			}

			printf("\n");
			curLen = curLen+2*wcslen(windspd_str);


			for(i=0;i<curLen;i++)
			{

				printf("%02x  ",curBuf[i]);
			}
			printf("\n");
		/////////////////////////////////////////

		/////////////////////////////////////////
			j = 0;
			for(i=0;i<wcslen(windspd_value_str); i++)
			{
				curBuf[curLen+j] = p_windspd_value[i] >> 8;
				printf("%02x  ",curBuf[curLen+j]);
				curBuf[curLen+j+1] = p_windspd_value[i];
				printf("%02x  ",curBuf[curLen+j+1]);
				j=j+2;
			}

			printf("\n\n\n\n\n\n\n\n\n");
			curLen = curLen+2*wcslen(windspd_value_str);

			for(i=0;i<curLen;i++)
			{

				printf("%02x  ",curBuf[i]);
			}
			printf("\n");

		/////////////////////////////////////////

		/////////////////////////////////////////
			j = 0;
			for(i=0;i<wcslen(winddir_str); i++)
			{
				curBuf[curLen+j] = p_winddir[i] >> 8;
				printf("%02x  ",curBuf[curLen+j]);
				curBuf[curLen+j+1] = p_winddir[i];
				printf("%02x  ",curBuf[curLen+j+1]);
				j=j+2;
			}

			printf("\n");
			curLen = curLen+2*wcslen(winddir_str);


			for(i=0;i<curLen;i++)
			{

				printf("%02x  ",curBuf[i]);
			}
			printf("\n");
		/////////////////////////////////////////


		/////////////////////////////////////////
			j = 0;
			for(i=0;i<wcslen(winddir_value_str); i++)
			{
				curBuf[curLen+j] = p_winddir_value[i] >> 8;
				printf("%02x  ",curBuf[curLen+j]);
				curBuf[curLen+j+1] = p_winddir_value[i];
				printf("%02x  ",curBuf[curLen+j+1]);
				j=j+2;
			}

			printf("\n\n\n\n\n\n\n\n\n");
			curLen = curLen+2*wcslen(winddir_value_str);

			for(i=0;i<curLen;i++)
			{

			printf("%02x  ",curBuf[i]);
			}
			printf("\n");
		/////////////////////////////////////////


			unsigned char sample3[] = {0x5C,0x00};
			memcpy(&curBuf[curLen], sample3, 2);

			unsigned int DataLen = curLen+2-6;
			printf("Total Length: %d\n",DataLen);
			printf("Total Length: %02x\n  ",DataLen);

			//第五位需要更新为DataLen的十六进制 unsigned char
			//memcpy(&curBuf[4], DataLen, 2);
			//curBuf[4] = DataLen;
			curBuf[4] = DataLen;
			curBuf[5] = DataLen >> 8;
			printf("Total Length: %02x\n  ",curBuf[4]);
			printf("Total Length: %02x\n  ",curBuf[5]);

			unsigned char checksum = 0;
			for(i=0;i<(curLen + 2);i++)
			{
				//printf("0x%x  ",sample[i]);
				printf("%02x  ",curBuf[i]);
				checksum = checksum + curBuf[i];

			}

			printf("\n");
			printf("checksum: 0x%2x  ", checksum);
			printf("\n");

			curBuf[curLen+2] = checksum;
			curBuf[curLen+3] = 0x1A;


			for(i=0;i<(curLen+4);i++)
			{
				printf("%02x  ",curBuf[i]);
			}

	//////////////////////////////////////////////////////////

			//发送串口指令
			//ret = hcu_spsapi_serial_port_send(&(zHcuVmCtrTab.hwinv.sps485.modbus), currentSpsBuf.curBuf, currentSpsBuf.curLen);
			ret = hcu_spsapi_serial_port_send(&(zHcuVmCtrTab.hwinv.sps232.sp), curBuf, curLen+4);

			if (FAILURE == ret)
			{
				zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
				HcuErrorPrint("L3AQYCG20: Error send ZH command to serials port!\n");
				return FAILURE;
			}
			else
			{
				//HCU_DEBUG_PRINT_INF("L3AQYCG20: Send ZH data succeed %02X %02x %02X %02X \n", currentSpsBuf.curBuf[0],currentSpsBuf.curBuf[1],currentSpsBuf.curBuf[2],currentSpsBuf.curBuf[3]);
			}

			//等待短时
			hcu_usleep(5);

			//读取串口数据
			//从相应的从设备中读取数据
			memset(&currentLedBuf, 0, sizeof(SerialLedMsgBuf_t));
			//ret = hcu_spsapi_serial_port_get(&(zHcuVmCtrTab.hwinv.sps485.modbus), currentSpsBuf.curBuf, HCU_SYSDIM_MSG_BODY_LEN_MAX);//获得的数据存在currentSpsBuf中
			ret = hcu_spsapi_serial_port_get(&(zHcuVmCtrTab.hwinv.sps232.sp), currentLedBuf.curBuf, HCU_SYSDIM_MSG_BODY_LEN_MAX);//获得的数据存在currentSpsBuf中

			if ((ret <= 0) && (LCD_AlarmFlag == OFF))
			{

				zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
				HcuErrorPrint("L3AQYCG20: Can not read ZH data from serial port, return of read %d \n", ret);
				LCD_AlarmFlag = ON;

				msg_struct_com_alarm_report_t snd;
				memset(&snd, 0, sizeof(msg_struct_com_alarm_report_t));

				snd.length = sizeof(msg_struct_com_alarm_report_t);
				snd.usercmdid = L3CI_alarm;
				snd.useroptid = L3PO_hcualarm_report;
				snd.cmdIdBackType = L3CI_cmdid_back_type_instance;
				snd.alarmServerity = HUITP_IEID_UNI_ALARM_SEVERITY_HIGH;
				snd.alarmClearFlag = HUITP_IEID_UNI_ALARM_CLEAR_FLAG_OFF;
				snd.timeStamp = time(0);
				//snd.equID = rcv.equId;
				//snd.alarmType = HUITP_IEID_UNI_ALARM_TYPE_SENSOR;
				//snd.alarmContent = HUITP_IEID_UNI_ALARM_CONTENT_NOISE_NO_CONNECT;

				if (hcu_message_send(MSG_ID_COM_ALARM_REPORT, TASK_ID_SYSPM, TASK_ID_L3AQYCG20, &snd, snd.length) == FAILURE)
					HCU_ERROR_PRINT_TASK(TASK_ID_L3AQYCG20, "L3AQYCG20: Send message erro//route to L3 or direct to cloudvela, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_MODBUS].taskName, zHcuVmCtrTab.task[TASK_ID_SYSPM].taskName);

				//return FAILURE;

			}

			else if ((ret <= 0) && (LCD_AlarmFlag == ON))
			{
				zHcuSysStaPm.taskRunErrCnt[TASK_ID_SPSVIRGO]++;
				HcuErrorPrint("L3AQYCG20: Can not read ZH data from serial port, return of read %d \n", ret);
				//return FAILURE;
			}


			else if ((ret > 0) && (LCD_AlarmFlag == OFF))
			{
				//currentModbusBuf.curLen =ret;
				HcuDebugPrint("L3AQYCG20: Received ZH Len %d\n", ret);
				//HcuDebugPrint("L3AQYCG20: Received ZH resp data succeed: %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X\n",currentSpsBuf.curBuf[0],currentSpsBuf.curBuf[1],currentSpsBuf.curBuf[2],currentSpsBuf.curBuf[3],currentSpsBuf.curBuf[4],currentSpsBuf.curBuf[5],currentSpsBuf.curBuf[6],currentSpsBuf.curBuf[7],currentSpsBuf.curBuf[8],currentSpsBuf.curBuf[9],currentSpsBuf.curBuf[10],currentSpsBuf.curBuf[11],currentSpsBuf.curBuf[12],currentSpsBuf.curBuf[13],currentSpsBuf.curBuf[14],currentSpsBuf.curBuf[15]);

			}

			else if ((ret > 0) && (LCD_AlarmFlag == ON))
			{

				LCD_AlarmFlag = OFF;
				//currentModbusBuf.curLen = ret;
				HcuDebugPrint("L3AQYCG20: Received ZH Len %d\n", ret);
				//HcuDebugPrint("L3AQYCG20: Received ZH resp data succeed: %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X\n",currentSpsBuf.curBuf[0],currentSpsBuf.curBuf[1],currentSpsBuf.curBuf[2],currentSpsBuf.curBuf[3],currentSpsBuf.curBuf[4],currentSpsBuf.curBuf[5],currentSpsBuf.curBuf[6],currentSpsBuf.curBuf[7],currentSpsBuf.curBuf[8],currentSpsBuf.curBuf[9],currentSpsBuf.curBuf[10],currentSpsBuf.curBuf[11],currentSpsBuf.curBuf[12],currentSpsBuf.curBuf[13],currentSpsBuf.curBuf[14],currentSpsBuf.curBuf[15]);

				msg_struct_com_alarm_report_t snd;
				memset(&snd, 0, sizeof(msg_struct_com_alarm_report_t));

				snd.length = sizeof(msg_struct_com_alarm_report_t);
				snd.usercmdid = L3CI_alarm;
				snd.useroptid = L3PO_hcualarm_report;
				snd.cmdIdBackType = L3CI_cmdid_back_type_instance;
				snd.alarmServerity = HUITP_IEID_UNI_ALARM_SEVERITY_HIGH;
				snd.alarmClearFlag = HUITP_IEID_UNI_ALARM_CLEAR_FLAG_ON;
				snd.timeStamp = time(0);
				//snd.equID = rcv.equId;
				//snd.alarmType = HUITP_IEID_UNI_ALARM_TYPE_SENSOR;
				//snd.alarmContent = HUITP_IEID_UNI_ALARM_CONTENT_NOISE_NO_CONNECT;

				if (hcu_message_send(MSG_ID_COM_ALARM_REPORT, TASK_ID_SYSPM, TASK_ID_L3AQYCG20, &snd, snd.length) == FAILURE)
					HCU_ERROR_PRINT_TASK(TASK_ID_SPSVIRGO, "L3AQYCG20: Send message erro//route to L3 or direct to cloudvela, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_MODBUS].taskName, zHcuVmCtrTab.task[TASK_ID_SYSPM].taskName);

			}

	}
	////////

/////////////////////////////////////////////////////////////////////////////////////////

		snd.length = sizeof(msg_struct_ycjk_cloudvela_data_report_t);

		//发送后台
		if (hcu_message_send(MSG_ID_YCJK_CLOUDVELA_DATA_REPORT, TASK_ID_CLOUDVELA, TASK_ID_L3AQYCG20, &snd, snd.length) == FAILURE)
			HCU_ERROR_PRINT_L3AQYCG20("L3AQYCG20: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3AQYCG20].taskName, zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName);
	}//使用HUITP发送MSG_ID_YCJK_CLOUDVELA_DATA_REPORT报告给后台

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
/*
	gTaskL3aqycq20Context.valueJudge.a34001_Alarm_UpValue  = HCU_L3AQYC_A34001_ALAMR_MAX;
	gTaskL3aqycq20Context.valueJudge.a34001_Alarm_LowValue = HCU_L3AQYC_A34001_ALAMR_MIN;
	gTaskL3aqycq20Context.valueJudge.a50001_Alarm_UpValue  = HCU_L3AQYC_A50001_ALAMR_MAX;
	gTaskL3aqycq20Context.valueJudge.a50001_Alarm_LowValue = HCU_L3AQYC_A50001_ALAMR_MIN;
	gTaskL3aqycq20Context.valueJudge.a01001_Alarm_UpValue  = HCU_L3AQYC_A01001_ALAMR_MAX;
	gTaskL3aqycq20Context.valueJudge.a01001_Alarm_LowValue = HCU_L3AQYC_A01002_ALAMR_MIN;
	gTaskL3aqycq20Context.valueJudge.a01002_Alarm_UpValue  = HCU_L3AQYC_A01002_ALAMR_MAX;
	gTaskL3aqycq20Context.valueJudge.a01002_Alarm_LowValue = HCU_L3AQYC_A01002_ALAMR_MIN;
	gTaskL3aqycq20Context.valueJudge.a01007_Alarm_UpValue  = HCU_L3AQYC_A01007_ALAMR_MAX;
	gTaskL3aqycq20Context.valueJudge.a01007_Alarm_LowValue = HCU_L3AQYC_A01007_ALAMR_MIN;
	gTaskL3aqycq20Context.valueJudge.a01008_Alarm_UpValue  = HCU_L3AQYC_A01008_ALAMR_MAX;
	gTaskL3aqycq20Context.valueJudge.a01008_Alarm_LowValue = HCU_L3AQYC_A01008_ALAMR_MIN;
	gTaskL3aqycq20Context.valueJudge.a01006_Alarm_UpValue  = HCU_L3AQYC_A01006_ALAMR_MAX;
	gTaskL3aqycq20Context.valueJudge.a01006_Alarm_LowValue = HCU_L3AQYC_A01006_ALAMR_MIN;
*/
	gTaskL3aqycq20Context.valueJudge.a34001_Calibration_Flag = HCU_L3AQYC_A34001_CALIBRATION_FLAG;
	gTaskL3aqycq20Context.valueJudge.a50001_Calibration_Flag = HCU_L3AQYC_A50001_CALIBRATION_FLAG;
	gTaskL3aqycq20Context.valueJudge.a01001_Calibration_Flag = HCU_L3AQYC_A01001_CALIBRATION_FLAG;
	gTaskL3aqycq20Context.valueJudge.a01002_Calibration_Flag = HCU_L3AQYC_A01002_CALIBRATION_FLAG;
	gTaskL3aqycq20Context.valueJudge.a01007_Calibration_Flag = HCU_L3AQYC_A01007_CALIBRATION_FLAG;
	gTaskL3aqycq20Context.valueJudge.a01008_Calibration_Flag = HCU_L3AQYC_A01008_CALIBRATION_FLAG;
	gTaskL3aqycq20Context.valueJudge.a01006_Calibration_Flag = HCU_L3AQYC_A01006_CALIBRATION_FLAG;

	gTaskL3aqycq20Context.valueJudge.RainSnow_Flag = HCU_L3AQYC_RAIN_SNOW_FLAG;

	//最大风速值（噪声仪失效）, 当风速大于5m/s时噪声数据无效（设噪声数据为S）
	gTaskL3aqycq20Context.valueJudge.WindThresholdForNoise = gTaskL3aqycq20Context.alarmLimit.a01007_Alarm_UpValue;

}


//告警门限值初始化
void func_l3aqycg20_alarm_limit_init(void)
{

	gTaskL3aqycq20Context.alarmLimit.a34001_Alarm_UpValue  = HCU_L3AQYC_A34001_ALAMR_MAX;
	gTaskL3aqycq20Context.alarmLimit.a34001_Alarm_LowValue = HCU_L3AQYC_A34001_ALAMR_MIN;
	gTaskL3aqycq20Context.alarmLimit.a50001_Alarm_UpValue  = HCU_L3AQYC_A50001_ALAMR_MAX;
	gTaskL3aqycq20Context.alarmLimit.a50001_Alarm_LowValue = HCU_L3AQYC_A50001_ALAMR_MIN;
	gTaskL3aqycq20Context.alarmLimit.a01001_Alarm_UpValue  = HCU_L3AQYC_A01001_ALAMR_MAX;
	gTaskL3aqycq20Context.alarmLimit.a01001_Alarm_LowValue = HCU_L3AQYC_A01002_ALAMR_MIN;
	gTaskL3aqycq20Context.alarmLimit.a01002_Alarm_UpValue  = HCU_L3AQYC_A01002_ALAMR_MAX;
	gTaskL3aqycq20Context.alarmLimit.a01002_Alarm_LowValue = HCU_L3AQYC_A01002_ALAMR_MIN;
	gTaskL3aqycq20Context.alarmLimit.a01007_Alarm_UpValue  = HCU_L3AQYC_A01007_ALAMR_MAX;
	gTaskL3aqycq20Context.alarmLimit.a01007_Alarm_LowValue = HCU_L3AQYC_A01007_ALAMR_MIN;
	gTaskL3aqycq20Context.alarmLimit.a01008_Alarm_UpValue  = HCU_L3AQYC_A01008_ALAMR_MAX;
	gTaskL3aqycq20Context.alarmLimit.a01008_Alarm_LowValue = HCU_L3AQYC_A01008_ALAMR_MIN;
	gTaskL3aqycq20Context.alarmLimit.a01006_Alarm_UpValue  = HCU_L3AQYC_A01006_ALAMR_MAX;
	gTaskL3aqycq20Context.alarmLimit.a01006_Alarm_LowValue = HCU_L3AQYC_A01006_ALAMR_MIN;


}

//监测数据flag判断，依据《建设工程颗粒物与噪声在线监测技术规范》
void func_l3aqyc_measurement_value_flag_judge_min(HcuSysMsgIeL3aqycContextStaElement_t *aggReport)
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
	if(aggReport->a34001_Avg < gTaskL3aqycq20Context.valueJudge.a34001_Range_Min)
			aggReport->a34001_Flag = 'L';

	if(aggReport->a34001_Avg > gTaskL3aqycq20Context.valueJudge.a34001_Range_Max)
			aggReport->a34001_Flag = 'H';

	if(aggReport->a50001_Avg < gTaskL3aqycq20Context.valueJudge.a50001_Range_Min)
			aggReport->a50001_Flag = 'L';

	if(aggReport->a50001_Avg > gTaskL3aqycq20Context.valueJudge.a50001_Range_Max)
			aggReport->a50001_Flag = 'H';

	if(aggReport->a01001_Avg < gTaskL3aqycq20Context.valueJudge.a01001_Range_Min)
			aggReport->a01001_Flag = 'L';

	if(aggReport->a01001_Avg > gTaskL3aqycq20Context.valueJudge.a01001_Range_Max)
			aggReport->a01001_Flag = 'H';

	if(aggReport->a01002_Avg < gTaskL3aqycq20Context.valueJudge.a01002_Range_Min)
			aggReport->a01002_Flag = 'L';

	if(aggReport->a01002_Avg > gTaskL3aqycq20Context.valueJudge.a01002_Range_Max)
			aggReport->a01002_Flag = 'H';

	if(aggReport->a01007_Avg < gTaskL3aqycq20Context.valueJudge.a01007_Range_Min)
			aggReport->a01007_Flag = 'L';

	if(aggReport->a01007_Avg > gTaskL3aqycq20Context.valueJudge.a01007_Range_Max)
			aggReport->a01007_Flag = 'H';

	if(aggReport->a01008_Avg < gTaskL3aqycq20Context.valueJudge.a01008_Range_Min)
			aggReport->a01008_Flag = 'L';

	if(aggReport->a01008_Avg > gTaskL3aqycq20Context.valueJudge.a01008_Range_Max)
			aggReport->a01008_Flag = 'H';

	//超数据设定范围上下限，上下限的值设置由云端设置，默认和仪器量程相等
	if(aggReport->a34001_Avg < gTaskL3aqycq20Context.valueJudge.a34001_Range_Set_Min)
			aggReport->a34001_Flag = '-';

	if(aggReport->a34001_Avg > gTaskL3aqycq20Context.valueJudge.a34001_Range_Set_Max)
			aggReport->a34001_Flag = '+';

	if(aggReport->a50001_Avg < gTaskL3aqycq20Context.valueJudge.a50001_Range_Set_Min)
			aggReport->a50001_Flag = '-';

	if(aggReport->a50001_Avg > gTaskL3aqycq20Context.valueJudge.a50001_Range_Set_Max)
			aggReport->a50001_Flag = '+';

	if(aggReport->a01001_Avg < gTaskL3aqycq20Context.valueJudge.a01001_Range_Set_Min)
			aggReport->a01001_Flag = '-';

	if(aggReport->a01001_Avg > gTaskL3aqycq20Context.valueJudge.a01001_Range_Set_Max)
			aggReport->a01001_Flag = '+';

	if(aggReport->a01002_Avg < gTaskL3aqycq20Context.valueJudge.a01002_Range_Set_Min)
			aggReport->a01002_Flag = '-';

	if(aggReport->a01002_Avg > gTaskL3aqycq20Context.valueJudge.a01002_Range_Set_Max)
			aggReport->a01002_Flag = '+';

	if(aggReport->a01007_Avg < gTaskL3aqycq20Context.valueJudge.a01007_Range_Set_Min)
			aggReport->a01007_Flag = '-';

	if(aggReport->a01007_Avg > gTaskL3aqycq20Context.valueJudge.a01007_Range_Set_Max)
			aggReport->a01007_Flag = '+';

	if(aggReport->a01008_Avg < gTaskL3aqycq20Context.valueJudge.a01008_Range_Set_Min)
			aggReport->a01008_Flag = '-';

	if(aggReport->a01008_Avg > gTaskL3aqycq20Context.valueJudge.a01008_Range_Set_Max)
			aggReport->a01008_Flag = '+';

	//当风速大于门限值时，默认5m/s
	if(aggReport->a01007_Avg > gTaskL3aqycq20Context.alarmLimit.a01007_Alarm_UpValue)
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

void func_l3aqyc_measurement_value_flag_judge_inst(HcuSysMsgIeL3aqycContextCurrentElement_t *aggReport)
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
	if(aggReport->a34001_Avg < gTaskL3aqycq20Context.valueJudge.a34001_Range_Min)
			aggReport->a34001_Flag = 'L';

	if(aggReport->a34001_Avg > gTaskL3aqycq20Context.valueJudge.a34001_Range_Max)
			aggReport->a34001_Flag = 'H';

	if(aggReport->a50001_Avg < gTaskL3aqycq20Context.valueJudge.a50001_Range_Min)
			aggReport->a50001_Flag = 'L';

	if(aggReport->a50001_Avg > gTaskL3aqycq20Context.valueJudge.a50001_Range_Max)
			aggReport->a50001_Flag = 'H';

	if(aggReport->a01001_Avg < gTaskL3aqycq20Context.valueJudge.a01001_Range_Min)
			aggReport->a01001_Flag = 'L';

	if(aggReport->a01001_Avg > gTaskL3aqycq20Context.valueJudge.a01001_Range_Max)
			aggReport->a01001_Flag = 'H';

	if(aggReport->a01002_Avg < gTaskL3aqycq20Context.valueJudge.a01002_Range_Min)
			aggReport->a01002_Flag = 'L';

	if(aggReport->a01002_Avg > gTaskL3aqycq20Context.valueJudge.a01002_Range_Max)
			aggReport->a01002_Flag = 'H';

	if(aggReport->a01007_Avg < gTaskL3aqycq20Context.valueJudge.a01007_Range_Min)
			aggReport->a01007_Flag = 'L';

	if(aggReport->a01007_Avg > gTaskL3aqycq20Context.valueJudge.a01007_Range_Max)
			aggReport->a01007_Flag = 'H';

	if(aggReport->a01008_Avg < gTaskL3aqycq20Context.valueJudge.a01008_Range_Min)
			aggReport->a01008_Flag = 'L';

	if(aggReport->a01008_Avg > gTaskL3aqycq20Context.valueJudge.a01008_Range_Max)
			aggReport->a01008_Flag = 'H';

	//超数据设定范围上下限，上下限的值设置由云端设置，默认和仪器量程相等
	if(aggReport->a34001_Avg < gTaskL3aqycq20Context.valueJudge.a34001_Range_Set_Min)
			aggReport->a34001_Flag = '-';

	if(aggReport->a34001_Avg > gTaskL3aqycq20Context.valueJudge.a34001_Range_Set_Max)
			aggReport->a34001_Flag = '+';

	if(aggReport->a50001_Avg < gTaskL3aqycq20Context.valueJudge.a50001_Range_Set_Min)
			aggReport->a50001_Flag = '-';

	if(aggReport->a50001_Avg > gTaskL3aqycq20Context.valueJudge.a50001_Range_Set_Max)
			aggReport->a50001_Flag = '+';

	if(aggReport->a01001_Avg < gTaskL3aqycq20Context.valueJudge.a01001_Range_Set_Min)
			aggReport->a01001_Flag = '-';

	if(aggReport->a01001_Avg > gTaskL3aqycq20Context.valueJudge.a01001_Range_Set_Max)
			aggReport->a01001_Flag = '+';

	if(aggReport->a01002_Avg < gTaskL3aqycq20Context.valueJudge.a01002_Range_Set_Min)
			aggReport->a01002_Flag = '-';

	if(aggReport->a01002_Avg > gTaskL3aqycq20Context.valueJudge.a01002_Range_Set_Max)
			aggReport->a01002_Flag = '+';

	if(aggReport->a01007_Avg < gTaskL3aqycq20Context.valueJudge.a01007_Range_Set_Min)
			aggReport->a01007_Flag = '-';

	if(aggReport->a01007_Avg > gTaskL3aqycq20Context.valueJudge.a01007_Range_Set_Max)
			aggReport->a01007_Flag = '+';

	if(aggReport->a01008_Avg < gTaskL3aqycq20Context.valueJudge.a01008_Range_Set_Min)
			aggReport->a01008_Flag = '-';

	if(aggReport->a01008_Avg > gTaskL3aqycq20Context.valueJudge.a01008_Range_Set_Max)
			aggReport->a01008_Flag = '+';

	//当风速大于门限值时，默认5m/s
	if(aggReport->a01007_Avg > gTaskL3aqycq20Context.alarmLimit.a01007_Alarm_UpValue)
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


//传感器运行状态表初始化
void func_l3aqycg20_eqt_rs_init(void)
{
	gTaskL3aqycq20Context.eqtStatus.a34001_PolId = HCU_L3AQYC_A34001_INDEX;
	gTaskL3aqycq20Context.eqtStatus.a34001_RS = ON;

	gTaskL3aqycq20Context.eqtStatus.a50001_PolId = HCU_L3AQYC_A50001_INDEX;
	gTaskL3aqycq20Context.eqtStatus.a50001_RS = ON;

	gTaskL3aqycq20Context.eqtStatus.a01001_PolId = HCU_L3AQYC_A01001_INDEX;
	gTaskL3aqycq20Context.eqtStatus.a01001_RS = ON;

	gTaskL3aqycq20Context.eqtStatus.a01002_PolId = HCU_L3AQYC_A01002_INDEX;
	gTaskL3aqycq20Context.eqtStatus.a01002_RS = ON;

	gTaskL3aqycq20Context.eqtStatus.a01006_PolId = HCU_L3AQYC_A01006_INDEX;
	gTaskL3aqycq20Context.eqtStatus.a01006_RS = OFF;

	gTaskL3aqycq20Context.eqtStatus.a01007_PolId = HCU_L3AQYC_A01007_INDEX;
	gTaskL3aqycq20Context.eqtStatus.a01007_RS = ON;

	gTaskL3aqycq20Context.eqtStatus.a01008_PolId = HCU_L3AQYC_A01008_INDEX;
	gTaskL3aqycq20Context.eqtStatus.a01008_RS = ON;

}

OPSTAT func_l3aqyc_pm10_noise_value_alarm_judge_2072(HcuSysMsgIeL3aqycContextStaElement_t *staInstant)
{
	int ret = 0;
	//超标报警判断并发送
	if(OFF == gTaskL3aqycq20Context.PM10AlarmOnOff)
	{
		if (staInstant->a34001_Avg >= gTaskL3aqycq20Context.alarmLimit.a34001_Alarm_UpValue)//暂时只考虑最大值门限值告警
		{
			msg_struct_l3mod_llczhb_data_report_t snd;
			memset(&snd, 0, sizeof(msg_struct_l3mod_llczhb_data_report_t));
			snd.actionId = HCU_SYSMSG_ZHBHJT_ACTION_SND_ALM_EVT_2072;
			snd.ul2Cloud.AlarmTime = time(0);
			snd.ul2Cloud.AlarmType = ON;
			snd.length = sizeof(msg_struct_l3mod_llczhb_data_report_t);

			snd.ul2Cloud.Ala.PolId = staInstant->a34001_PolId;
			snd.ul2Cloud.Ala.Ala = staInstant->a34001_Avg;
			gTaskL3aqycq20Context.PM10AlarmOnOff = ON;

			ret = hcu_message_send(MSG_ID_L3MOD_LLCZHB_DATA_REPORT, TASK_ID_LLCZHB, TASK_ID_L3AQYCG20, &snd, snd.length);

			if (ret == FAILURE){
				HCU_ERROR_PRINT_L3AQYCG20("L3AQYCG20: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3AQYCG20].taskName, zHcuVmCtrTab.task[TASK_ID_LLCZHB].taskName);
			}
		}
	}

	if(OFF == gTaskL3aqycq20Context.NoiseAlarmOnOff)
	{
		if(staInstant->a50001_Avg >= gTaskL3aqycq20Context.alarmLimit.a50001_Alarm_UpValue)
		{
			msg_struct_l3mod_llczhb_data_report_t snd;
			memset(&snd, 0, sizeof(msg_struct_l3mod_llczhb_data_report_t));
			snd.actionId = HCU_SYSMSG_ZHBHJT_ACTION_SND_ALM_EVT_2072;
			snd.ul2Cloud.AlarmTime = time(0);
			snd.ul2Cloud.AlarmType = ON;
			snd.length = sizeof(msg_struct_l3mod_llczhb_data_report_t);

			snd.ul2Cloud.Ala.PolId = staInstant->a50001_PolId;
			snd.ul2Cloud.Ala.Ala = staInstant->a50001_Avg;
			gTaskL3aqycq20Context.NoiseAlarmOnOff = ON;

			ret = hcu_message_send(MSG_ID_L3MOD_LLCZHB_DATA_REPORT, TASK_ID_LLCZHB, TASK_ID_L3AQYCG20, &snd, snd.length);

			if (ret == FAILURE){
				HCU_ERROR_PRINT_L3AQYCG20("L3AQYCG20: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3AQYCG20].taskName, zHcuVmCtrTab.task[TASK_ID_LLCZHB].taskName);
			}
		}
	}

	if(ON == gTaskL3aqycq20Context.PM10AlarmOnOff)
	{
		if (staInstant->a34001_Avg < gTaskL3aqycq20Context.alarmLimit.a34001_Alarm_UpValue)
		{
			msg_struct_l3mod_llczhb_data_report_t snd;
			memset(&snd, 0, sizeof(msg_struct_l3mod_llczhb_data_report_t));
			snd.actionId = HCU_SYSMSG_ZHBHJT_ACTION_SND_ALM_EVT_2072;
			snd.ul2Cloud.AlarmTime = time(0);
			snd.ul2Cloud.AlarmType = OFF;
			snd.length = sizeof(msg_struct_l3mod_llczhb_data_report_t);

			snd.ul2Cloud.Ala.PolId = staInstant->a34001_PolId;
			snd.ul2Cloud.Ala.Ala = staInstant->a34001_Avg;
			gTaskL3aqycq20Context.PM10AlarmOnOff = OFF;

			ret = hcu_message_send(MSG_ID_L3MOD_LLCZHB_DATA_REPORT, TASK_ID_LLCZHB, TASK_ID_L3AQYCG20, &snd, snd.length);

			if (ret == FAILURE){
				HCU_ERROR_PRINT_L3AQYCG20("L3AQYCG20: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3AQYCG20].taskName, zHcuVmCtrTab.task[TASK_ID_LLCZHB].taskName);
			}
		}
	}

	if(ON == gTaskL3aqycq20Context.NoiseAlarmOnOff)
	{
		if (staInstant->a50001_Avg < gTaskL3aqycq20Context.alarmLimit.a50001_Alarm_UpValue)
		{
			msg_struct_l3mod_llczhb_data_report_t snd;
			memset(&snd, 0, sizeof(msg_struct_l3mod_llczhb_data_report_t));
			snd.actionId = HCU_SYSMSG_ZHBHJT_ACTION_SND_ALM_EVT_2072;
			snd.ul2Cloud.AlarmTime = time(0);
			snd.ul2Cloud.AlarmType = OFF;
			snd.length = sizeof(msg_struct_l3mod_llczhb_data_report_t);

			snd.ul2Cloud.Ala.PolId = staInstant->a50001_PolId;
			snd.ul2Cloud.Ala.Ala = staInstant->a50001_Avg;
			gTaskL3aqycq20Context.NoiseAlarmOnOff = OFF;

			ret = hcu_message_send(MSG_ID_L3MOD_LLCZHB_DATA_REPORT, TASK_ID_LLCZHB, TASK_ID_L3AQYCG20, &snd, snd.length);

			if (ret == FAILURE){
				HCU_ERROR_PRINT_L3AQYCG20("L3AQYCG20: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3AQYCG20].taskName, zHcuVmCtrTab.task[TASK_ID_LLCZHB].taskName);
			}
		}
	}

	return SUCCESS;

}









