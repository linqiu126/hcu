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
#include "../l3app/sensorpm25.h"
#include "../l3app/sensorhumid.h"
#include "../l3app/sensortemp.h"
#include "../l3app/sensorwinddir.h"
#include "../l3app/sensorwindspd.h"

#include "../l0comvm/vmlayer.h"

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
LedMsgBuf_t currentLedBuf;

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

	memset(&currentLedBuf, 0, sizeof(LedMsgBuf_t));

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

/*
		if((gTaskL3aqycq20Context.staMin.a01001_Avg > HCU_L3AQYC_A01001_RANGE_MAX) || (gTaskL3aqycq20Context.staMin.a01001_Avg == 0)){
			gTaskL3aqycq20Context.staMin.a01001_Avg = HCU_L3AQYC_A01001_RANGE_MAX - 20;
		}
		if((gTaskL3aqycq20Context.staMin.a01002_Avg > HCU_L3AQYC_A01002_RANGE_MAX) || (gTaskL3aqycq20Context.staMin.a01002_Avg == 0)){
			gTaskL3aqycq20Context.staMin.a01002_Avg = HCU_L3AQYC_A01002_RANGE_MAX - 50;
		}
		if(gTaskL3aqycq20Context.staMin.a01007_Avg > HCU_L3AQYC_A01007_RANGE_MAX){
			gTaskL3aqycq20Context.staMin.a01007_Avg = HCU_L3AQYC_A01007_RANGE_MAX -25;
		}
		if(gTaskL3aqycq20Context.staMin.a01008_Avg > HCU_L3AQYC_A01008_RANGE_MAX){
			gTaskL3aqycq20Context.staMin.a01008_Avg = HCU_L3AQYC_A01008_RANGE_MAX;
		}
		if((gTaskL3aqycq20Context.staMin.a34001_Avg > (HCU_L3AQYC_A34001_RANGE_MAX*HCU_SYSMSG_NB_MICROS_IN_ONE_MS)) || (gTaskL3aqycq20Context.staMin.a34001_Avg == 0)){
			gTaskL3aqycq20Context.staMin.a34001_Avg = HCU_L3AQYC_A34001_RANGE_MAX + 100;
		}
		if((gTaskL3aqycq20Context.staMin.a50001_Avg > HCU_L3AQYC_A50001_RANGE_MAX) || (gTaskL3aqycq20Context.staMin.a50001_Avg == 0)){
			gTaskL3aqycq20Context.staMin.a50001_Avg = HCU_L3AQYC_A50001_RANGE_MAX - 20;
		}
*/

		if(gTaskTempContext.tempValue == 0)
			gTaskTempContext.tempValue = 0;

		if(gTaskHumidContext.humidValue == 0)
			gTaskHumidContext.humidValue = 50;

		if(gTaskWinddirContext.winddirValue == 0)
			gTaskWinddirContext.winddirValue = 120;

		if(gTaskWindspdContext.windspdValue == 0)
			gTaskWindspdContext.windspdValue = 0;

		if(gTaskPm25Context.PM01Value == 0)
			gTaskPm25Context.PM01Value = 11;

		if(gTaskPm25Context.PM25Value == 0)
			gTaskPm25Context.PM25Value = 22;

		if(gTaskPm25Context.PM10Value == 0)
			gTaskPm25Context.PM10Value = 33;

		if(gTaskPm25Context.TSP == 0)
			gTaskPm25Context.TSP = 66;

		if(gTaskNoiseContext.noiseValue == 0)
			gTaskNoiseContext.noiseValue = 26;

/*
		snd.ycjk.tempValue = HCU_SYSMSG_NB_MICROS_IN_ONE_MS*gTaskL3aqycq20Context.staMin.a01001_Avg;
		snd.ycjk.humidValue = HCU_SYSMSG_NB_MICROS_IN_ONE_MS*gTaskL3aqycq20Context.staMin.a01002_Avg;
		snd.ycjk.winddirValue = HCU_SYSMSG_NB_MICROS_IN_ONE_MS*gTaskL3aqycq20Context.staMin.a01008_Avg;
		snd.ycjk.windspdValue = HCU_SYSMSG_NB_MICROS_IN_ONE_MS*gTaskL3aqycq20Context.staMin.a01007_Avg;
		snd.ycjk.tspValue = HCU_SYSMSG_NB_MICROS_IN_ONE_MS*gTaskL3aqycq20Context.staMin.a34001_Avg;
		snd.ycjk.pm1d0Value = HCU_SYSMSG_NB_MICROS_IN_ONE_MS*gTaskL3aqycq20Context.staMin.a34001_Avg;
		snd.ycjk.pm2d5Value = HCU_SYSMSG_NB_MICROS_IN_ONE_MS*gTaskPm25Context.PM25Value;//to be update later
		snd.ycjk.pm10Value = HCU_SYSMSG_NB_MICROS_IN_ONE_MS*gTaskPm25Context.PM10Value;//to be update later
		snd.ycjk.noiseValue = HCU_SYSMSG_NB_MICROS_IN_ONE_MS*gTaskL3aqycq20Context.staMin.a50001_Avg;
		snd.ycjk.dataFormat = CLOUD_SENSOR_DATA_FOMAT_FLOAT_WITH_NF3; //no need, caculate directly for l3aqycg20??
		snd.ycjk.timeStamp = time(0);
*/


		snd.ycjk.tempValue = HCU_SYSMSG_NB_MICROS_IN_ONE_MS*gTaskTempContext.tempValue;
		snd.ycjk.humidValue = HCU_SYSMSG_NB_MICROS_IN_ONE_MS*gTaskHumidContext.humidValue;
		snd.ycjk.winddirValue = HCU_SYSMSG_NB_MICROS_IN_ONE_MS*gTaskWinddirContext.winddirValue;
		snd.ycjk.windspdValue = HCU_SYSMSG_NB_MICROS_IN_ONE_MS*gTaskWindspdContext.windspdValue;
		snd.ycjk.tspValue = HCU_SYSMSG_NB_MICROS_IN_ONE_MS*gTaskPm25Context.TSP;
		snd.ycjk.pm1d0Value = HCU_SYSMSG_NB_MICROS_IN_ONE_MS*gTaskPm25Context.PM01Value;
		snd.ycjk.pm2d5Value = HCU_SYSMSG_NB_MICROS_IN_ONE_MS*gTaskPm25Context.PM25Value;
		snd.ycjk.pm10Value = HCU_SYSMSG_NB_MICROS_IN_ONE_MS*gTaskPm25Context.PM10Value;
		snd.ycjk.noiseValue = HCU_SYSMSG_NB_MICROS_IN_ONE_MS*gTaskNoiseContext.noiseValue;
		snd.ycjk.dataFormat = CLOUD_SENSOR_DATA_FOMAT_FLOAT_WITH_NF3; //no need, caculate directly for l3aqycg20??
		snd.ycjk.timeStamp = time(0);

		HCU_DEBUG_PRINT_INF("L3AQYCG20: Temp=%.1f, Humid=%.1f, Windir=%.1f, Windspd=%.1f, tspValue=%.1f, pm2d5Value=%.1f, pm10Value=%.1f, noiseValue=%.1f\n",gTaskL3aqycq20Context.staMin.a01001_Avg, gTaskL3aqycq20Context.staMin.a01002_Avg, gTaskL3aqycq20Context.staMin.a01008_Avg, gTaskL3aqycq20Context.staMin.a01007_Avg, gTaskL3aqycq20Context.cur.a34001_Avg, gTaskPm25Context.PM25Value, gTaskPm25Context.PM10Value, gTaskNoiseContext.noiseValue);

		HCU_DEBUG_PRINT_INF("L3AQYCG20: Temp=%.1f, Humid=%.1f, Windir=%.1f, Windspd=%.1f, tspValue=%.1f, pm2d5Value=%.1f, pm10Value=%.1f, noiseValue=%.1f\n",gTaskL3aqycq20Context.staMin.a01001_Avg, gTaskL3aqycq20Context.staMin.a01002_Avg, gTaskL3aqycq20Context.staMin.a01008_Avg, gTaskL3aqycq20Context.staMin.a01007_Avg, gTaskL3aqycq20Context.staMin.a34001_Avg, gTaskPm25Context.PM25Value, gTaskPm25Context.PM10Value, gTaskL3aqycq20Context.staMin.a50001_Avg);
		HCU_DEBUG_PRINT_INF("L3AQYCG20: Temp=%d, Humid=%d, Windir=%d, Windspd=%d, tspValue=%d, pm2d5Value=%d, pm10Value=%d, noiseValue=%d\n\n\n",snd.ycjk.tempValue, snd.ycjk.humidValue, snd.ycjk.winddirValue, snd.ycjk.windspdValue, snd.ycjk.tspValue, snd.ycjk.pm2d5Value, snd.ycjk.pm10Value, snd.ycjk.noiseValue);



		////////////////////////////////////////////////////////////////////////////////////////////For 中航LED test
		if (zHcuSysEngPar.hwBurnId.hwType == HUITP_IEID_UNI_INVENT_HWTYPE_PDTYPE_G2_AQYC_RASP_2101)
		{


			ret=0;
			UINT32 i = 0;
			UINT32 j = 0;


	//对发送数据进行编码 - 仰邦卡 - Start
	//////////////////////////////////////////////////////////

			UINT8 sample_head[] = {0xA5,0xA5,0xA5,0xA5,0xA5,0xA5,0xA5,0xA5};
			UINT32 curLen_head = sizeof(sample_head)/sizeof(UINT8);

	        //北京时间
			UINT8 sample_chinese[] = {0xA5,0xA5,0xA5,0xA5,0xA5,0xA5,0xA5,0xA5,0x01,0x00,0x00,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0xFE,0x02,0x39,0x00,0xA3,0x06,0x01,0x34,
					0x00,0x00,0x01,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x00,0x20,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x02,0x01,0x01,0x00,0x00,0x02,0x15,0x00,0x00,
					0x00,0xB1,0xB1,0xBE,0xA9,0xCA,0xB1,0xBC,0xE4,0x5C,0x6E,0x5C,0x44,0x48,0x3A,0x5C,0x44,0x4D,0x3A,0x5C,0x44,0x53,0x7D,0xE9,0x5A};

			//四川达岸 扬尘监控
	        //UINT8 sample_chinese[] = {0xA5,0xA5,0xA5,0xA5,0xA5,0xA5,0xA5,0xA5,0x01,0x00,0x00,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0xFE,0x02,0x36,0x00,0xA3,0x06,0x01,0x34,
					//0x00,0x00,0x01,0x2D,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x00,0x20,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x02,0x01,0x01,0x00,0x00,0x02,0x12,0x00,0x00,
					//0x00,0xCB,0xC4,0xB4,0xA8,0xB4,0xEF,0xB0,0xB6,0x5C,0x6E,0xDE,0xEF,0xB3,0xBE,0xBC,0xE0,0xBF,0xD8,0x19,0x43,0x5A};



	        UINT32 curLen_chinese = sizeof(sample_chinese)/sizeof(UINT8);

	        UINT8 sample_PM25[] = {0x01,0x00,0x00,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0xFE,0x02,0x3B,0x00,0xA3,0x06,0x01,0x34,
					0x00,0x00,0x01,0x32,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x00,0x20,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x02,0x01,0x01,0x00,0x00,0x02,0x17,0x00,0x00,
					0x00,0x20,0x50,0x4D,0x32,0x2E,0x35,0xD6,0xB5,0x5C,0x6E,0x20};

	        UINT32 curLen_PM25 = sizeof(sample_PM25)/sizeof(UINT8);

	        //UINT32 curLen_no_CRC = 71;
	        UINT8 sample_PM10[] = {0x01,0x00,0x00,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0xFE,0x02,0x39,0x00,0xA3,0x06,0x01,0x34,
					0x00,0x00,0x01,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x08,0x00,0x20,0x00,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x02,0x01,0x01,0x00,0x00,0x02,0x15,0x00,0x00,
					0x00,0x20,0x50,0x4D,0x31,0x30,0xD6,0xB5,0x5C,0x6E,0x20};

	        UINT32 curLen_PM10 = sizeof(sample_PM10)/sizeof(UINT8);

	        UINT8  curBuf[HCU_SYSDIM_MSG_BODY_LEN_MAX];

			//send the first frame start
			memset(&curBuf, 0, sizeof(HCU_SYSDIM_MSG_BODY_LEN_MAX));
			memcpy(curBuf, sample_chinese, curLen_chinese);


			for(i=0;i<(curLen_chinese);i++)
			{
				printf("%02x  ",curBuf[i]);
			}
			printf("\n\n\n");

			//发送串口指令
			//ret = hcu_spsapi_serial_port_send(&(zHcuVmCtrTab.hwinv.sps485.modbus), currentSpsBuf.curBuf, currentSpsBuf.curLen);
			ret = hcu_spsapi_serial_port_send(&(zHcuVmCtrTab.hwinv.sps232.sp), curBuf, curLen_chinese);

			memset(&curBuf, 0, HCU_SYSDIM_MSG_BODY_LEN_MAX);

			if (FAILURE == ret)
			{
				zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
				HcuErrorPrint("L3DAYCG20: Error send YB LED command to serials port!\n");
				return FAILURE;
			}
			else
			{
				//HCU_DEBUG_PRINT_INF("L3DAYCG20: Send YB data succeed %02X %02x %02X %02X \n", currentSpsBuf.curBuf[0],currentSpsBuf.curBuf[1],currentSpsBuf.curBuf[2],currentSpsBuf.curBuf[3]);
				ret = hcu_sps485_serial_port_get(&zHcuVmCtrTab.hwinv.sps232.sp, curBuf, HCU_SYSDIM_MSG_BODY_LEN_MAX);

				if(ret > 0){
					HCU_DEBUG_PRINT_INF("L3DAYCG20: Received OuBanb LED data length %d\n", ret);
					HCU_DEBUG_PRINT_INF("L3DAYCG20: Received OuBanb LED response data succeed: %02X %02X %02X %02X %02X %02X %02X %02X\n",curBuf[0],curBuf[1],curBuf[2],curBuf[3],curBuf[4],curBuf[5],curBuf[6],curBuf[7]);
					HCU_DEBUG_PRINT_INF("L3DAYCG20: Chinese frame send successfully\n\n\n");
					//return SUCCESS;
				}
				else
					return FAILURE;
			}
			//send the first frame end


			size_t len;
			char CStr[HCU_SYSDIM_MSG_BODY_LEN_MAX];
	        UINT8  wholeBuf[HCU_SYSDIM_MSG_BODY_LEN_MAX];
	        UINT16 crc16;


			///////////////////////////////////////////////////
			//if(gTaskPm25Context.PM25Value == 0)
				//gTaskPm25Context.PM25Value = 306;

 			memset(&curBuf, 0, sizeof(HCU_SYSDIM_MSG_BODY_LEN_MAX));
			memcpy(curBuf, sample_PM25, curLen_PM25);

			UINT32 pm25 = gTaskPm25Context.PM25Value;

			//char CStr[HCU_SYSDIM_MSG_BODY_LEN_MAX];
			memset(&CStr, 0, sizeof(HCU_SYSDIM_MSG_BODY_LEN_MAX));

			//sprintf(CStr,"%.1f",tsp);
			sprintf(CStr,"%d",pm25);
			printf("integer = %d CStr = %s\n\n\n", pm25,CStr);

		   //把char*转换为wchar_t*
			len = strlen(CStr) + 1;
			wchar_t *pm25_value_str;
			pm25_value_str=(wchar_t*)malloc(len*sizeof(wchar_t));
			mbstowcs(pm25_value_str,CStr,len);

			UINT32 *p_pm25_value = (wchar_t*)pm25_value_str;


			printf("pm25_value_str len = %d \n\n\n", wcslen(pm25_value_str));
			j = 0;
			for(i=0;i<wcslen(pm25_value_str); i++)
			{
				curBuf[curLen_PM25+i] = p_pm25_value[i];
			}

			for(i=0;i<(wcslen(pm25_value_str));i++)
			{
				printf("%02x  ",curBuf[curLen_PM25+i]);
			}
			printf("\n\n\n");


			curBuf[46] = wcslen(pm25_value_str) + 14;
			curBuf[47] = (wcslen(pm25_value_str) + 14) >> 8;

			curBuf[21] = wcslen(pm25_value_str) + 14 + 27;
			curBuf[22] = (wcslen(pm25_value_str) + 14 + 27) >> 8;

			curBuf[12] = wcslen(pm25_value_str) + 14 + 27 + 9;
			curBuf[13] = (wcslen(pm25_value_str) + 14 + 27 + 9) >> 8;

			curLen_PM25 = curLen_PM25 + wcslen(pm25_value_str);
			curBuf[curLen_PM25] = 0x20;
			curBuf[curLen_PM25+1] = 0x75;
			curBuf[curLen_PM25+2] = 0x67;
			curLen_PM25 = curLen_PM25 + 3;

			for(i=0;i<(curLen_PM25);i++)
			{
				printf("%02x  ",curBuf[i]);
			}
			printf("\n\n\n");


			//增加CRC16-2B，高位在前，低位在后，但下面的函数本身得出的结果是低位在前，高位在后，晕乎吧
			crc16=0;
			crc16 = hcu_vm_crc16(curBuf, curLen_PM25);
			curBuf[curLen_PM25] = (UINT8)(crc16 & 0x0FF); //高位字节
			curLen_PM25 = curLen_PM25 + 1;
			curBuf[curLen_PM25] = (UINT8)((crc16 >> 8) & 0x0FF); //低位字节
			curLen_PM25 = curLen_PM25 + 1;

			curBuf[curLen_PM25] = 0x5A;
			curLen_PM25 = curLen_PM25 + 1;


			for(i=0;i<(curLen_PM25);i++)
			{
				printf("%02x  ",curBuf[i]);
			}

			printf("PM25 with CRC\n\n\n");


			memset(&wholeBuf, 0, sizeof(HCU_SYSDIM_MSG_BODY_LEN_MAX));

			memcpy(wholeBuf, sample_head, curLen_head);

			memcpy(&wholeBuf[curLen_head], curBuf, curLen_PM25);

			curLen_PM25 = curLen_PM25 + curLen_head;

			for(i=0;i<curLen_PM25; i++)
			{
				printf("%02x  ",wholeBuf[i]);
			}

			printf("PM25 whole\n\n\n");

	//////////////////////////////////////////////////////////


			hcu_sleep(4);

			//发送串口指令
			//ret = hcu_spsapi_serial_port_send(&(zHcuVmCtrTab.hwinv.sps485.modbus), currentSpsBuf.curBuf, currentSpsBuf.curLen);
			ret = hcu_spsapi_serial_port_send(&(zHcuVmCtrTab.hwinv.sps232.sp), wholeBuf, curLen_PM25);

			memset(&wholeBuf, 0, HCU_SYSDIM_MSG_BODY_LEN_MAX);

			if (FAILURE == ret)
			{
				zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
				HcuErrorPrint("L3DAYCG20: Error send YB command to serials port!\n");
				return FAILURE;
			}
			else
			{
				//HCU_DEBUG_PRINT_INF("L3DAYCG20: Send YB data succeed %02X %02x %02X %02X \n", currentSpsBuf.curBuf[0],currentSpsBuf.curBuf[1],currentSpsBuf.curBuf[2],currentSpsBuf.curBuf[3]);
				ret = hcu_sps485_serial_port_get(&zHcuVmCtrTab.hwinv.sps232.sp, wholeBuf, HCU_SYSDIM_MSG_BODY_LEN_MAX);

				if(ret > 0){
					HCU_DEBUG_PRINT_INF("L3DAYCG20: Received LED data length %d\n", ret);
					HCU_DEBUG_PRINT_INF("L3DAYCG20: Received LED response data succeed: %02X %02X %02X %02X %02X %02X %02X %02X\n",curBuf[0],curBuf[1],curBuf[2],curBuf[3],curBuf[4],curBuf[5],curBuf[6],curBuf[7]);
					HCU_DEBUG_PRINT_INF("L3DAYCG20: PM2.5 frame send successfully\n\n\n");
					//return SUCCESS;
				}
				else
					return FAILURE;
			}
			//send the second frame end


			///////////////////////////////////////////////////
			//if(gTaskPm25Context.PM10Value == 0)
				//gTaskPm25Context.PM10Value = 306;

			memset(&curBuf, 0, sizeof(HCU_SYSDIM_MSG_BODY_LEN_MAX));
			memcpy(curBuf, sample_PM10, curLen_PM10);

			UINT32 pm10 = gTaskPm25Context.PM10Value;

			//char CStr[HCU_SYSDIM_MSG_BODY_LEN_MAX];
			memset(&CStr, 0, sizeof(HCU_SYSDIM_MSG_BODY_LEN_MAX));

			//sprintf(CStr,"%.1f",tsp);
			sprintf(CStr,"%d",pm10);
			printf("integer = %d CStr = %s\n\n\n", pm10,CStr);

		   //把char*转换为wchar_t*
			len = strlen(CStr) + 1;
			wchar_t *pm10_value_str;
			pm10_value_str=(wchar_t*)malloc(len*sizeof(wchar_t));
			mbstowcs(pm10_value_str,CStr,len);

			UINT32 *p_pm10_value = (wchar_t*)pm10_value_str;


			printf("pm10_value_str len = %d \n\n\n", wcslen(pm10_value_str));
			j = 0;
			for(i=0;i<wcslen(pm10_value_str); i++)
			{
				curBuf[curLen_PM10+i] = p_pm10_value[i];
			}

			for(i=0;i<(wcslen(pm10_value_str));i++)
			{
				printf("%02x  ",curBuf[curLen_PM10+i]);
			}
			printf("\n\n\n");


			curBuf[46] = wcslen(pm10_value_str) + 13;
			curBuf[47] = (wcslen(pm10_value_str) + 13) >> 8;

			curBuf[21] = wcslen(pm10_value_str) + 13 + 27;
			curBuf[22] = (wcslen(pm10_value_str) + 13 + 27) >> 8;

			curBuf[12] = wcslen(pm10_value_str) + 13 + 27 + 9;
			curBuf[13] = (wcslen(pm10_value_str) + 13 + 27 + 9) >> 8;

			curLen_PM10 = curLen_PM10 + wcslen(pm10_value_str);
			curBuf[curLen_PM10] = 0x20;
			curBuf[curLen_PM10+1] = 0x75;
			curBuf[curLen_PM10+2] = 0x67;
			curLen_PM10 = curLen_PM10 + 3;


			//增加CRC16-2B，高位在前，低位在后，但下面的函数本身得出的结果是低位在前，高位在后，晕乎吧
			crc16=0;
			crc16 = hcu_vm_crc16(curBuf, curLen_PM10);
			curBuf[curLen_PM10] = (UINT8)(crc16 & 0x0FF); //高位字节
			curLen_PM10 = curLen_PM10 + 1;
			curBuf[curLen_PM10] = (UINT8)((crc16 >> 8) & 0x0FF); //低位字节
			curLen_PM10 = curLen_PM10 + 1;

			curBuf[curLen_PM10] = 0x5A;
			curLen_PM10 = curLen_PM10 + 1;


	        //UINT8  wholeBuf[HCU_SYSDIM_MSG_BODY_LEN_MAX];
			memset(&wholeBuf, 0, sizeof(HCU_SYSDIM_MSG_BODY_LEN_MAX));

			memcpy(wholeBuf, sample_head, curLen_head);

			memcpy(&wholeBuf[curLen_head], curBuf, curLen_PM10);

			curLen_PM10 = curLen_PM10 + curLen_head;

			for(i=0;i<curLen_PM10; i++)
			{
				printf("%02x  ", wholeBuf[i]);
			}

			printf("PM10 whole\n\n\n");

	//////////////////////////////////////////////////////////
			hcu_sleep(20);

			//发送串口指令
			//ret = hcu_spsapi_serial_port_send(&(zHcuVmCtrTab.hwinv.sps485.modbus), currentSpsBuf.curBuf, currentSpsBuf.curLen);
			ret = hcu_spsapi_serial_port_send(&(zHcuVmCtrTab.hwinv.sps232.sp), wholeBuf, curLen_PM10);

			memset(&wholeBuf, 0, HCU_SYSDIM_MSG_BODY_LEN_MAX);

			if (FAILURE == ret)
			{
				zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
				HcuErrorPrint("L3DAYCG20: Error send LED command to serials port!\n");
				return FAILURE;
			}
			else
			{
				//HCU_DEBUG_PRINT_INF("L3DAYCG20: Send YB data succeed %02X %02x %02X %02X \n", currentSpsBuf.curBuf[0],currentSpsBuf.curBuf[1],currentSpsBuf.curBuf[2],currentSpsBuf.curBuf[3]);
				ret = hcu_sps485_serial_port_get(&zHcuVmCtrTab.hwinv.sps232.sp, wholeBuf, HCU_SYSDIM_MSG_BODY_LEN_MAX);

				if(ret > 0){
					HCU_DEBUG_PRINT_INF("L3DAYCG20: Received LED data length %d\n", ret);
					HCU_DEBUG_PRINT_INF("L3DAYCG20: Received LED response data succeed: %02X %02X %02X %02X %02X %02X %02X %02X\n",curBuf[0],curBuf[1],curBuf[2],curBuf[3],curBuf[4],curBuf[5],curBuf[6],curBuf[7]);
					HCU_DEBUG_PRINT_INF("L3DAYCG20: PM10 frame send successfully\n\n\n");
					//return SUCCESS;
				}
				else
					return FAILURE;
			}
			//send the Third frame end

//对发送数据进行编码 - 仰邦卡 - Start
//////////////////////////////////////////////////////////




	/*
	//对发送数据进行编码 - 中航卡
	//////////////////////////////////////////////////////////ssddss
			wchar_t *chinese_str = L"扬尘监测四川达岸";
			UINT32 *p_chinese = (wchar_t*)chinese_str;

			//wchar_t *noise_str = L"噪声:";
			//UINT32 *p_noise = (wchar_t*)noise_str;

			//wchar_t *noise_unit_str = L"dB";
			//UINT32 *p_noise_unit = (wchar_t*)noise_unit_str;

			wchar_t *pm25_str = L"PM25: ";
			UINT32 *p_pm25 = (wchar_t*)pm25_str;

			wchar_t *pm10_str = L"PM10: ";
			UINT32 *p_pm10 = (wchar_t*)pm10_str;

			//wchar_t *pm25_unit_str = L"ug/m3;                                                                                                                     ";
			//UINT32 *p_pm25_unit = (wchar_t*)pm25_unit_str;


			wchar_t *temp0_str = L"温度:-";
			UINT32 *p_temp0 = (wchar_t*)temp0_str;

			wchar_t *temp_str = L"温度: ";
			UINT32 *p_temp = (wchar_t*)temp_str;

			//wchar_t *temp_unit_str = L" ℃   ";
			//UINT32 *p_temp_unit = (wchar_t*)temp_unit_str;


			wchar_t *humid_str = L"湿度: ";
			UINT32 *p_humid = (wchar_t*)humid_str;

			//wchar_t *humid_unit_str = L" RH%                                                                                                                     ";
			//UINT32 *p_humid_unit = (wchar_t*)humid_unit_str;

			size_t len;
			char CStr[HCU_SYSDIM_MSG_BODY_LEN_MAX];


			///////////////////////////////////////////////////
			if(gTaskPm25Context.PM25Value == 0)
				gTaskPm25Context.PM25Value = 99;

			UINT32 pm25 = gTaskPm25Context.PM25Value;

			//char CStr[HCU_SYSDIM_MSG_BODY_LEN_MAX];
			memset(&CStr, 0, sizeof(HCU_SYSDIM_MSG_BODY_LEN_MAX));

			//sprintf(CStr,"%.1f",tsp);
			sprintf(CStr,"%d",pm25);
			printf("integer = %d CStr = %s\n", pm25,CStr);

		   //把char*转换为wchar_t*
			len = strlen(CStr) + 1;
			wchar_t *pm25_value_str;
			pm25_value_str=(wchar_t*)malloc(len*sizeof(wchar_t));
			mbstowcs(pm25_value_str,CStr,len);

			UINT32 *p_pm25_value = (wchar_t*)pm25_value_str;
			///////////////////////////////////////////////////

			///////////////////////////////////////////////////
			if(gTaskPm25Context.PM10Value == 0)
				gTaskPm25Context.PM10Value = 88;

			UINT32 pm10 = gTaskPm25Context.PM10Value;

			//char CStr[HCU_SYSDIM_MSG_BODY_LEN_MAX];
			memset(&CStr, 0, sizeof(HCU_SYSDIM_MSG_BODY_LEN_MAX));

			//sprintf(CStr,"%.1f",tsp);
			sprintf(CStr,"%d",pm10);
			printf("integer = %d CStr = %s\n", pm10,CStr);

		   //把char*转换为wchar_t*
			len = strlen(CStr) + 1;
			wchar_t *pm10_value_str;
			pm10_value_str=(wchar_t*)malloc(len*sizeof(wchar_t));
			mbstowcs(pm10_value_str,CStr,len);

			UINT32 *p_pm10_value = (wchar_t*)pm10_value_str;
			///////////////////////////////////////////////////

			///////////////////////////////////////////////////
			//float temp = gTaskL3aqycq20Context.staMin.a01001_Avg;
			//float temp = 20;

			if(gTaskTempContext.tempValue == 0 || gTaskTempContext.tempValue >= 60)
				gTaskTempContext.tempValue = 16;
			UINT32 temp = gTaskTempContext.tempValue;

			//char CStr[HCU_SYSDIM_MSG_BODY_LEN_MAX];
			memset(&CStr, 0, sizeof(HCU_SYSDIM_MSG_BODY_LEN_MAX));
			//itoa(number,string,16);
			sprintf(CStr,"%d",temp);
			printf("integer = %d CStr = %s\n", temp,CStr);

		   //把char*转换为wchar_t*
			len = strlen(CStr) + 1;
			wchar_t *temp_value_str;
			temp_value_str=(wchar_t*)malloc(len*sizeof(wchar_t));
			mbstowcs(temp_value_str,CStr,len);

			UINT32 *p_temp_value = (wchar_t*)temp_value_str;
			///////////////////////////////////////////////////

			///////////////////////////////////////////////////
			//float humid = gTaskL3aqycq20Context.staMin.a01002_Avg;
			//float humid = 50;
			if(gTaskHumidContext.humidValue == 0 || gTaskHumidContext.humidValue >= 100)
				gTaskHumidContext.humidValue = 40;
			UINT32 humid = gTaskHumidContext.humidValue;

			//char CStr[HCU_SYSDIM_MSG_BODY_LEN_MAX];
			memset(&CStr, 0, sizeof(HCU_SYSDIM_MSG_BODY_LEN_MAX));
			//itoa(number,string,16);
			sprintf(CStr,"%d",humid);
			printf("integer = %d CStr = %s\n", humid,CStr);

		   //把char*转换为wchar_t*
			len = strlen(CStr) + 1;
			wchar_t *humid_value_str;
			humid_value_str=(wchar_t*)malloc(len*sizeof(wchar_t));
			mbstowcs(humid_value_str,CStr,len);

			UINT32 *p_humid_value = (wchar_t*)humid_value_str;
			///////////////////////////////////////////////////



			UINT32 curLen = 228;

	        UINT8 sample1[] = {0x7A,0x01,0x00,0x06,0xFA,0x00,0x01,0x01,0x01,0x0C,0x05,0x03,0x01,0x00,0x3C,0x01,0x01,0x00,0x00,0x7F,0x0A,0x00,0x01,0x00,0x00,0x00,
					0x00,0x00,0x40,0x00,0x20,0x00,0x02,0x01,0x02,0x09,0xF4,0xF1,0x10,0x10,0xFE,0x64,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
					0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
					0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
					0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
					0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
					0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
					0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x5C,0x5A,0x00,0x5C,0x54,0x01};

	        UINT8 sample2[] = {0x7A,0x01,0x00,0x06,0xFA,0x00,0x01,0x01,0x01,0x0C,0x05,0x03,0x01,0x00,0x3C,0x01,0x01,0x00,0x00,0x7F,0x0A,0x00,0x01,0x00,0x00,0x00,
					0x00,0x00,0x40,0x00,0x20,0x00,0x01,0x01,0x02,0x09,0xF4,0xF1,0x10,0x10,0xFE,0x64,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
					0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
					0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
					0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
					0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
					0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
					0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x5C,0x5A,0x00,0x5C,0x54,0x01};

	        UINT8  curBuf[HCU_SYSDIM_MSG_BODY_LEN_MAX];

			//send the first frame start
			memset(&curBuf, 0, sizeof(HCU_SYSDIM_MSG_BODY_LEN_MAX));
			memcpy(curBuf, sample1, curLen);

			UINT32 i = 0;
			UINT32 j = 0;

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
		/////////////////////////////////////////


			UINT8 sample3[] = {0x5C,0x00};
			memcpy(&curBuf[curLen], sample3, 2);

			UINT32 DataLen = curLen+2-6;
			printf("Total Length: %d\n",DataLen);
			printf("Total Length: %02x\n  ",DataLen);

			//第五位需要更新为DataLen的十六进制 unsigned char
			//memcpy(&curBuf[4], DataLen, 2);
			//curBuf[4] = DataLen;
			curBuf[4] = DataLen;
			curBuf[5] = DataLen >> 8;
			printf("Total Length: %02x\n  ",curBuf[4]);
			printf("Total Length: %02x\n  ",curBuf[5]);

			UINT8 checksum = 0;
			for(i=0;i<(curLen + 2);i++)
			{
				checksum = checksum + curBuf[i];
			}

			printf("checksum: 0x%2x\n  ", checksum);

			curBuf[curLen+2] = checksum;
			curBuf[curLen+3] = 0x1A;

			for(i=0;i<(curLen+4);i++)
			{
				printf("%02x  ",curBuf[i]);
			}
			printf("\n");

	//////////////////////////////////////////////////////////

			//发送串口指令
			//ret = hcu_spsapi_serial_port_send(&(zHcuVmCtrTab.hwinv.sps485.modbus), currentSpsBuf.curBuf, currentSpsBuf.curLen);
			ret = hcu_spsapi_serial_port_send(&(zHcuVmCtrTab.hwinv.sps232.sp), curBuf, curLen+4);

			memset(&curBuf, 0, HCU_SYSDIM_MSG_BODY_LEN_MAX);

			if (FAILURE == ret)
			{
				zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
				HcuErrorPrint("L3DAYCG20: Error send ZH command to serials port!\n");
				return FAILURE;
			}
			else
			{
				//HCU_DEBUG_PRINT_INF("L3DAYCG20: Send ZH data succeed %02X %02x %02X %02X \n", currentSpsBuf.curBuf[0],currentSpsBuf.curBuf[1],currentSpsBuf.curBuf[2],currentSpsBuf.curBuf[3]);
				ret = hcu_sps485_serial_port_get(&zHcuVmCtrTab.hwinv.sps232.sp, curBuf, HCU_SYSDIM_MSG_BODY_LEN_MAX);

				if(ret > 0){
					HCU_DEBUG_PRINT_INF("L3DAYCG20: Received LCD data length %d\n", ret);
					HCU_DEBUG_PRINT_INF("L3DAYCG20: Received LCD response data succeed: %02X %02X %02X %02X %02X %02X %02X %02X\n",curBuf[0],curBuf[1],curBuf[2],curBuf[3],curBuf[4],curBuf[5],curBuf[6],curBuf[7]);
					HCU_DEBUG_PRINT_INF("L3DAYCG20: First frame send successfully\n\n\n");
					//return SUCCESS;
				}
				else
					return FAILURE;
			}
			//send the first frame end


			//send the second frame start
			hcu_sleep(5);

			curLen = 228;
			//send the second frame start
			memset(&curBuf, 0, sizeof(HCU_SYSDIM_MSG_BODY_LEN_MAX));
			memcpy(curBuf, sample1, curLen);

			/////////////////////////////////////////
				j = 0;
				for(i=0;i<wcslen(pm25_str); i++)
				{
					curBuf[curLen+j] = p_pm25[i] >> 8;
					curBuf[curLen+j+1] = p_pm25[i];
					j=j+2;
				}
				curLen = curLen+2*wcslen(pm25_str);
			/////////////////////////////////////////

			/////////////////////////////////////////
				j = 0;
				for(i=0;i<wcslen(pm25_value_str); i++)
				{
					curBuf[curLen+j] = p_pm25_value[i] >> 8;
					curBuf[curLen+j+1] = p_pm25_value[i];
					j=j+2;
				}
				curLen = curLen+2*wcslen(pm25_value_str);
			/////////////////////////////////////////


			/////////////////////////////////////////
				j = 0;
				for(i=0;i<wcslen(pm10_str); i++)
				{
					curBuf[curLen+j] = p_pm10[i] >> 8;
					curBuf[curLen+j+1] = p_pm10[i];
					j=j+2;
				}
				curLen = curLen+2*wcslen(pm10_str);
			/////////////////////////////////////////

			/////////////////////////////////////////
				j = 0;
				for(i=0;i<wcslen(pm10_value_str); i++)
				{
					curBuf[curLen+j] = p_pm10_value[i] >> 8;
					curBuf[curLen+j+1] = p_pm10_value[i];
					j=j+2;
				}

				curLen = curLen+2*wcslen(pm10_value_str);
			/////////////////////////////////////////

			//unsigned char sample3[] = {0x5C,0x00};
			memcpy(&curBuf[curLen], sample3, 2);

			DataLen = curLen+2-6;
			printf("Total Length: %d\n",DataLen);
			printf("Total Length: %02x\n  ",DataLen);

			//第五位需要更新为DataLen的十六进制 unsigned char
			curBuf[4] = DataLen;
			curBuf[5] = DataLen >> 8;
			printf("Total Length: %02x\n  ",curBuf[4]);
			printf("Total Length: %02x\n  ",curBuf[5]);

			checksum = 0;
			for(i=0;i<(curLen + 2);i++)
			{
				checksum = checksum + curBuf[i];
			}

			printf("checksum: 0x%2x\n  ", checksum);

			curBuf[curLen+2] = checksum;
			curBuf[curLen+3] = 0x1A;

			for(i=0;i<(curLen+4);i++)
			{
				printf("%02x  ",curBuf[i]);
			}
			printf("\n");

	//////////////////////////////////////////////////////////

			//发送串口指令
			//ret = hcu_spsapi_serial_port_send(&(zHcuVmCtrTab.hwinv.sps485.modbus), currentSpsBuf.curBuf, currentSpsBuf.curLen);
			ret = hcu_spsapi_serial_port_send(&(zHcuVmCtrTab.hwinv.sps232.sp), curBuf, curLen+4);

			memset(&curBuf, 0, HCU_SYSDIM_MSG_BODY_LEN_MAX);

			if (FAILURE == ret)
			{
				zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
				HcuErrorPrint("L3DAYCG20: Error send ZH command to serials port!\n");
				return FAILURE;
			}
			else
			{
				//HCU_DEBUG_PRINT_INF("L3DAYCG20: Send ZH data succeed %02X %02x %02X %02X \n", currentSpsBuf.curBuf[0],currentSpsBuf.curBuf[1],currentSpsBuf.curBuf[2],currentSpsBuf.curBuf[3]);
				ret = hcu_sps485_serial_port_get(&zHcuVmCtrTab.hwinv.sps232.sp, curBuf, HCU_SYSDIM_MSG_BODY_LEN_MAX);

				if(ret > 0){
					HCU_DEBUG_PRINT_INF("L3DAYCG20: Received LCD data length %d\n", ret);
					HCU_DEBUG_PRINT_INF("L3DAYCG20: Received LCD response data succeed: %02X %02X %02X %02X %02X %02X %02X %02X\n",curBuf[0],curBuf[1],curBuf[2],curBuf[3],curBuf[4],curBuf[5],curBuf[6],curBuf[7]);
					HCU_DEBUG_PRINT_INF("L3DAYCG20: Second frame send successfully\n\n\n");
					//return SUCCESS;
				}
				else
					return FAILURE;
			}
			//send the second frame end


*/

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









