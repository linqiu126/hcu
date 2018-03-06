/*
 * l3bfhs.c
 *
 *  Created on: 2015年11月22日
 *      Author: test
 */
#include "l3bfhs.h"

/*
** FSM of the L3BFHS
*/
HcuFsmStateItem_t HcuFsmL3bfhs[] =
{
    //MessageId                 //State                   		 		//Function
	//启始点，固定定义，不要改动, 使用ENTRY/END，意味者MSGID肯定不可能在某个高位区段中；考虑到所有任务共享MsgId，即使分段，也无法实现
	//完全是为了给任务一个初始化的机会，按照状态转移机制，该函数不具备启动的机会，因为任务初始化后自动到FSM_STATE_IDLE
	//如果没有必要进行初始化，可以设置为NULL
	{MSG_ID_ENTRY,       						FSM_STATE_ENTRY,            		fsm_l3bfhs_task_entry}, //Starting

	//System level initialization, only controlled by HCU-MAIN
    {MSG_ID_COM_INIT,       					FSM_STATE_IDLE,            			fsm_l3bfhs_init},
    {MSG_ID_COM_INIT_FEEDBACK,					FSM_STATE_IDLE,            			fsm_com_do_nothing},

	//Task level initialization
    {MSG_ID_COM_INIT,       					FSM_STATE_L3BFHS_INITED,            fsm_l3bfhs_init},
    {MSG_ID_COM_INIT_FEEDBACK,					FSM_STATE_L3BFHS_INITED,            fsm_com_do_nothing},

	//ANY state entry
    {MSG_ID_COM_INIT_FEEDBACK,					FSM_STATE_COMMON,          			fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT,       				FSM_STATE_COMMON,          			fsm_com_heart_beat_rcv},
	{MSG_ID_COM_STOP,       					FSM_STATE_COMMON,          			fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT_FB,       			FSM_STATE_COMMON,          			fsm_com_do_nothing},
    {MSG_ID_COM_RESTART,						FSM_STATE_COMMON,            		fsm_l3bfhs_restart},
	{MSG_ID_COM_TIME_OUT,       				FSM_STATE_COMMON,          			fsm_l3bfhs_time_out},
	{MSG_ID_CLOUDVELA_L3BFHS_DATA_REQ,          FSM_STATE_COMMON,                   fsm_l3bfhs_cloudvela_data_req},
	{MSG_ID_CLOUDVELA_L3BFHS_DATA_CONFIRM,      FSM_STATE_COMMON,                   fsm_l3bfhs_cloudvela_data_confirm},
	{MSG_ID_CLOUDVELA_L3BFHS_EVENT_CONFIRM,     FSM_STATE_COMMON,                   fsm_l3bfhs_cloudvela_event_confirm},
	{MSG_ID_CLOUDVELA_L3BFHS_CTRL_REQ,          FSM_STATE_COMMON,                   fsm_l3bfhs_cloudvela_ctrl_req},
	{MSG_ID_CLOUDVELA_L3BFHS_STATISTIC_CONFIRM, FSM_STATE_COMMON,                   fsm_l3bfhs_cloudvela_statistic_confirm},

	//任意状态下的命令执行
	{MSG_ID_UICOMM_L3BFHS_CTRL_CMD_REQ,       	FSM_STATE_COMMON,          			fsm_l3bfhs_uicomm_ctrl_cmd_req},
	{MSG_ID_CAN_L3BFHS_CAL_ZERO_RESP,      		FSM_STATE_COMMON,          			fsm_l3bfhs_canitf_cal_zero_resp},
	{MSG_ID_CAN_L3BFHS_CAL_FULL_RESP,       	FSM_STATE_COMMON,          			fsm_l3bfhs_canitf_cal_full_resp},
	{MSG_ID_CAN_L3BFHS_DYN_ZERO_RESP,      		FSM_STATE_COMMON,          			fsm_l3bfhs_canitf_dyn_zero_resp},
	{MSG_ID_CAN_L3BFHS_DYN_FULL_RESP,       	FSM_STATE_COMMON,          			fsm_l3bfhs_canitf_dyn_full_resp},
	{MSG_ID_SUI_STARTUP_IND,       				FSM_STATE_COMMON,          			fsm_l3bfhs_canitf_startup_ind},
	{MSG_ID_SUI_FAULT_IND,       				FSM_STATE_COMMON,          			fsm_l3bfhs_canitf_fault_ind},
	{MSG_ID_SUI_HEART_BEAT_REPORT,       		FSM_STATE_COMMON,          			fsm_l3bfhs_canitf_heart_beat_report},

	//只为出现ACTIVED状态，入口自动被COMMON屏蔽
	{MSG_ID_CAN_L3BFHS_SYS_CFG_RESP,       		FSM_STATE_L3BFHS_ACTIVED,          	fsm_l3bfhs_canitf_sys_config_resp},
	{MSG_ID_SUI_SUSPEND_RESP,       			FSM_STATE_L3BFHS_ACTIVED,          	fsm_l3bfhs_canitf_sys_suspend_resp},

	//进料组合态
	{MSG_ID_CAN_L3BFHS_WS_NEW_READY_EVENT,      FSM_STATE_L3BFHS_OOS_SCAN,          fsm_l3bfhs_canitf_ws_new_ready_event},

	//休眠状态：只允许收到RESUME指令
	{MSG_ID_SUI_RESUME_RESP,       				FSM_STATE_L3BFHS_SUSPEND,          	fsm_l3bfhs_canitf_sys_resume_resp},

    //结束点，固定定义，不要改动
    {MSG_ID_END,            					FSM_STATE_END,             			NULL},  //Ending
};

//System Level Global variables


//Task Level Global variables
gTaskL3bfhsContext_t gTaskL3bfhsContext; //波峰秤盘的总控表

//Main Entry
/***************************************************************************************************************************
 *
 * 　FSM初始化部分
 *
 ***************************************************************************************************************************/
//Input parameter would be useless, but just for similar structure purpose
OPSTAT fsm_l3bfhs_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//除了对全局变量进行操作之外，尽量不要做其它操作，因为该函数将被主任务/线程调用，不是本任务/线程调用
	//该API就是给本任务一个提早介入的入口，可以帮着做些测试性操作
	if (FsmSetState(TASK_ID_L3BFHS, FSM_STATE_IDLE) == FAILURE){
		HcuErrorPrint("L3BFHS: Error Set FSM State at fsm_l3bfhs_task_entry\n");}
	return SUCCESS;
}

OPSTAT fsm_l3bfhs_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;

	if ((src_id > TASK_ID_MIN) &&(src_id < TASK_ID_MAX)){
		//Send back MSG_ID_COM_INIT_FEEDBACK to SVRCON
		msg_struct_com_init_feedback_t snd0;
		memset(&snd0, 0, sizeof(msg_struct_com_init_feedback_t));
		snd0.length = sizeof(msg_struct_com_init_feedback_t);

		//to avoid all task send out the init fb msg at the same time which lead to msgque get stuck
		hcu_usleep(dest_id*HCU_SYSCFG_DURATION_OF_INIT_FB_WAIT_MAX);

		ret = hcu_message_send(MSG_ID_COM_INIT_FEEDBACK, src_id, TASK_ID_L3BFHS, &snd0, snd0.length);
		if (ret == FAILURE){
			HcuErrorPrint("L3BFHS: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3BFHS].taskName, zHcuVmCtrTab.task[src_id].taskName);
			return FAILURE;
		}
	}

	//收到初始化消息后，进入初始化状态
	if (FsmSetState(TASK_ID_L3BFHS, FSM_STATE_L3BFHS_INITED) == FAILURE){
		HcuErrorPrint("L3BFHS: Error Set FSM State!\n");
		return FAILURE;
	}

	//初始化硬件接口
	if (func_l3bfhs_int_init() == FAILURE){
		HcuErrorPrint("L3BFHS: Error initialize interface!\n");
		return FAILURE;
	}

	//Global Variables
	zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3BFHS] = 0;
	//严格防止HUITP消息跟内部消息在关键结构上定义的不一致
	if ((sizeof(gTaskL3bfhsContextWgtSnrParamaters_t) != sizeof(strMsgIe_bfhs_WgtSnrParamaters_t)) ||\
			(sizeof(gTaskL3bfhsContextMotoCtrlParamaters_t) != sizeof(strMsgIe_bfhs_MotoCtrlParamaters_t)) ||\
			(sizeof(gTaskL3bfhsContextArmCtrlParamaters_t) != sizeof(strMsgIe_bfhs_ArmCtrlParamaters_t)))
		HCU_ERROR_PRINT_L3BFHS("L3BFHS: module message definition on statistic element error!\n");
	//严格保证统计周期的一致性
	if (HCU_L3BFHS_STA_UNIT_DUR != (10*zHcuSysEngPar.timer.array[TIMER_ID_10MS_L3BFHS_PERIOD_STA_SCAN].dur))  //静态表是以10ms为单位的
		HCU_ERROR_PRINT_L3BFHS("L3BFHS: module timer statistic parameter set error!\n");

	//秤盘数据表单控制表初始化
	memset(&gTaskL3bfhsContext, 0, sizeof(gTaskL3bfhsContext_t));

	//初始化界面交互数据
	HCU_L3BFHS_TRIGGER_UI_STATUS_REPORT(HUICOBUS_CMDID_CUI_HCU2UIR_GENERAL_CMDVAL_NULL);

	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_L3BFHS, FSM_STATE_L3BFHS_ACTIVED) == FAILURE)
		HCU_ERROR_PRINT_L3BFHS("L3BFHS: Error Set FSM State!\n");
	HCU_DEBUG_PRINT_FAT("L3BFHS: Enter FSM_STATE_L3BFHS_ACTIVED status, Keeping refresh here!\n");

	//返回
	return SUCCESS;
}

OPSTAT fsm_l3bfhs_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HcuErrorPrint("L3BFHS: Internal error counter reach DEAD level, SW-RESTART soon!\n");
	zHcuSysStaPm.statisCnt.restartCnt++;
	fsm_l3bfhs_init(0, 0, NULL, 0);
	return SUCCESS;
}

OPSTAT func_l3bfhs_int_init(void)
{
	if (hcu_sps232_qr_printer_init() == FAILURE)
		HcuDebugPrint("L3BFHS: Init QR Printer not success, but not impact whole system working!\n");

	//测试一把试试，正式场所不需要
	//打印二维码／条形码：二维码＋条形码的内容

	char s[100];
	time_t lt;
	struct tm *cu;
	memset(s, 0, sizeof(s));

	//初始本地时间
	lt=time(NULL);
	cu = localtime(&lt);
	cu->tm_mon = cu->tm_mon + 1; //月份是从0-11的，+1是为了符合正常逻辑
	sprintf(s, "BOFENGZHINENG-%s-%4.2f-%04d.%02d.%02d.%02d:%02d:%02d", gTaskL3bfhsContext.configName, (float)gTaskL3bfhsContext.wgtSnrPar.minAllowedWeight, \
			(UINT16)(1900+cu->tm_year), (UINT8)cu->tm_mon, (UINT8)cu->tm_mday, (UINT8)cu->tm_hour, (UINT8)cu->tm_min, (UINT8)cu->tm_sec);
	hcu_sps232_send_char_to_ext_printer(s, strlen(s));

	return SUCCESS;
}

//暂时啥都不干，定时到达后，还不明确是否需要支持定时工作
OPSTAT fsm_l3bfhs_time_out(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret;

	//Receive message and copy to local variable
	HCU_MSG_RCV_CHECK_FOR_GEN_LOCAL(TASK_ID_L3BFHS, msg_struct_com_time_out_t);

	//钩子在此处，检查zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3BFHS]是否超限
	if (zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3BFHS] > HCU_RUN_ERROR_LEVEL_2_MAJOR){
		//减少重复RESTART的概率
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3BFHS] = zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3BFHS] - HCU_RUN_ERROR_LEVEL_2_MAJOR;
		msg_struct_com_restart_t snd0;
		memset(&snd0, 0, sizeof(msg_struct_com_restart_t));
		snd0.length = sizeof(msg_struct_com_restart_t);
		ret = hcu_message_send(MSG_ID_COM_RESTART, TASK_ID_L3BFHS, TASK_ID_L3BFHS, &snd0, snd0.length);
		if (ret == FAILURE){
			HCU_ERROR_PRINT_L3BFHS("L3BFHS: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3BFHS].taskName, zHcuVmCtrTab.task[TASK_ID_L3BFHS].taskName);
		}
	}

	//配置初始化：一次性工作
	else if ((rcv.timeId == TIMER_ID_1S_L3BFHS_CFG_START_WAIT_FB) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		if (func_l3bfhs_time_out_sys_cfg_start_wait_fb_process() == FAILURE){
			HCU_ERROR_PRINT_L3BFHS("L3BFHS: Error process time out message!\n");
		}
	}

	//配置初始化：一次性工作
	else if ((rcv.timeId == TIMER_ID_1S_L3BFHS_SUSPEND_WAIT_FB) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		if (func_l3bfhs_time_out_sys_suspend_wait_fb_process() == FAILURE){
			HCU_ERROR_PRINT_L3BFHS("L3BFHS: Error process time out message!\n");
		}
	}

	//配置初始化：一次性工作
	else if ((rcv.timeId == TIMER_ID_1S_L3BFHS_RESUME_WAIT_FB) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		if (func_l3bfhs_time_out_sys_resume_wait_fb_process() == FAILURE){
			HCU_ERROR_PRINT_L3BFHS("L3BFHS: Error process time out message!\n");
		}
	}

	//配置初始化：一次性工作
	else if ((rcv.timeId == TIMER_ID_1S_L3BFHS_CAL_ZERO_WAIT_FB) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		if (func_l3bfhs_time_out_cal_zero_wait_fb_process() == FAILURE){
			HCU_ERROR_PRINT_L3BFHS("L3BFHS: Error process time out message!\n");
		}
	}

	//配置初始化：一次性工作
	else if ((rcv.timeId == TIMER_ID_1S_L3BFHS_CAL_FULL_WAIT_FB) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		if (func_l3bfhs_time_out_cal_full_wait_fb_process() == FAILURE){
			HCU_ERROR_PRINT_L3BFHS("L3BFHS: Error process time out message!\n");
		}
	}

	//周期性统计扫描定时器
	else if ((rcv.timeId == TIMER_ID_10MS_L3BFHS_PERIOD_STA_SCAN) &&(rcv.timeRes == TIMER_RESOLUTION_10MS)){
		if (func_l3bfhs_time_out_statistic_scan_process() == FAILURE){
			HCU_ERROR_PRINT_L3BFHS("L3BFHS: Error process time out message!\n");
		}
	}

	//返回
	return SUCCESS;
}

/***************************************************************************************************************************
 *
 * 　核心状态机处理过程
 *
 ***************************************************************************************************************************/
OPSTAT fsm_l3bfhs_canitf_sys_config_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HCU_MSG_RCV_CHECK_FOR_GEN_LOCAL(TASK_ID_L3BFHS, msg_struct_can_l3bfhs_sys_cfg_resp_t);

	//收到错误的反馈，就回复差错给界面
	if (rcv.validFlag == FALSE){
		msg_struct_l3bfhs_uicomm_ctrl_cmd_resp_t snd;
		memset(&snd, 0, sizeof(msg_struct_l3bfhs_uicomm_ctrl_cmd_resp_t));
		snd.cmdid = HCU_SYSMSG_BFHS_UICOMM_CMDID_CFG_START;
		snd.validFlag = FALSE;
		snd.errCode = rcv.errCode;
		snd.length = sizeof(msg_struct_l3bfhs_uicomm_ctrl_cmd_resp_t);
		HCU_MSG_SEND_GENERNAL_PROCESS(MSG_ID_L3BFHS_UICOMM_CTRL_CMD_RESP, TASK_ID_BFHSUICOMM, TASK_ID_L3BFHS);
		hcu_timer_stop(TASK_ID_L3BFHS, TIMER_ID_1S_L3BFHS_CFG_START_WAIT_FB, TIMER_RESOLUTION_1S);
		FsmSetState(TASK_ID_L3BFHS, FSM_STATE_L3BFHS_ACTIVED);

		//存储本地
		gTaskL3bfhsContext.sensorWs[0].nodeStatus = HCU_L3BFHS_NODE_BOARD_STATUS_STARTUP;
		//通知界面
		HCU_L3BFHS_TRIGGER_UI_STATUS_REPORT(HUICOBUS_CMDID_CUI_HCU2UIR_GENERAL_CMDVAL_CFG_ERR);
	}

	//收到正确的反馈
	else{
		msg_struct_l3bfhs_uicomm_ctrl_cmd_resp_t snd;
		memset(&snd, 0, sizeof(msg_struct_l3bfhs_uicomm_ctrl_cmd_resp_t));
		snd.cmdid = HCU_SYSMSG_BFHS_UICOMM_CMDID_CFG_START;
		snd.validFlag = TRUE;
		snd.length = sizeof(msg_struct_l3bfhs_uicomm_ctrl_cmd_resp_t);
		HCU_MSG_SEND_GENERNAL_PROCESS(MSG_ID_L3BFHS_UICOMM_CTRL_CMD_RESP, TASK_ID_BFHSUICOMM, TASK_ID_L3BFHS);
		hcu_timer_stop(TASK_ID_L3BFHS, TIMER_ID_1S_L3BFHS_CFG_START_WAIT_FB, TIMER_RESOLUTION_1S);
		FsmSetState(TASK_ID_L3BFHS, FSM_STATE_L3BFHS_OOS_SCAN);

		//存储本地
		gTaskL3bfhsContext.sensorWs[0].nodeStatus = HCU_L3BFHS_NODE_BOARD_STATUS_VALID;
		//通知界面
		HCU_L3BFHS_TRIGGER_UI_STATUS_REPORT(HUICOBUS_CMDID_CUI_HCU2UIR_GENERAL_CMDVAL_START_OK);

		//启动统计扫描定时器
		hcu_timer_start(TASK_ID_L3BFHS, HCU_TIMERID_WITH_DUR(TIMER_ID_10MS_L3BFHS_PERIOD_STA_SCAN), TIMER_TYPE_PERIOD, TIMER_RESOLUTION_10MS);
		//设置工作启动时间
		gTaskL3bfhsContext.startWorkTimeInUnix = time(0);
	}

	//返回
	return SUCCESS;
}

OPSTAT fsm_l3bfhs_canitf_sys_suspend_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HCU_MSG_RCV_CHECK_FOR_GEN_LOCAL(TASK_ID_L3BFHS, msg_struct_sui_suspend_resp_t);

	//收到错误的反馈，就回复差错给界面
	if (rcv.validFlag == FALSE){
		msg_struct_l3bfhs_uicomm_ctrl_cmd_resp_t snd;
		memset(&snd, 0, sizeof(msg_struct_l3bfhs_uicomm_ctrl_cmd_resp_t));
		snd.validFlag = FALSE;
		snd.errCode = rcv.errCode;
		snd.cmdid = HCU_SYSMSG_BFHS_UICOMM_CMDID_SUSPEND;
		snd.length = sizeof(msg_struct_l3bfhs_uicomm_ctrl_cmd_resp_t);
		HCU_MSG_SEND_GENERNAL_PROCESS(MSG_ID_L3BFHS_UICOMM_CTRL_CMD_RESP, TASK_ID_BFHSUICOMM, TASK_ID_L3BFHS);
		hcu_timer_stop(TASK_ID_L3BFHS, TIMER_ID_1S_L3BFHS_SUSPEND_WAIT_FB, TIMER_RESOLUTION_1S);
		FsmSetState(TASK_ID_L3BFHS, FSM_STATE_L3BFHS_ACTIVED);

		//存储本地
		gTaskL3bfhsContext.sensorWs[0].nodeStatus = HCU_L3BFHS_NODE_BOARD_STATUS_STARTUP;
		//通知界面
		HCU_L3BFHS_TRIGGER_UI_STATUS_REPORT(HUICOBUS_CMDID_CUI_HCU2UIR_GENERAL_CMDVAL_CFG_OK);
	}

	//收到正确的反馈
	else{
		msg_struct_l3bfhs_uicomm_ctrl_cmd_resp_t snd;
		memset(&snd, 0, sizeof(msg_struct_l3bfhs_uicomm_ctrl_cmd_resp_t));
		snd.cmdid = HCU_SYSMSG_BFHS_UICOMM_CMDID_SUSPEND;
		snd.validFlag = TRUE;
		snd.length = sizeof(msg_struct_l3bfhs_uicomm_ctrl_cmd_resp_t);
		HCU_MSG_SEND_GENERNAL_PROCESS(MSG_ID_L3BFHS_UICOMM_CTRL_CMD_RESP, TASK_ID_BFHSUICOMM, TASK_ID_L3BFHS);
		hcu_timer_stop(TASK_ID_L3BFHS, TIMER_ID_1S_L3BFHS_SUSPEND_WAIT_FB, TIMER_RESOLUTION_1S);
		FsmSetState(TASK_ID_L3BFHS, FSM_STATE_L3BFHS_SUSPEND);

		//存储本地
		gTaskL3bfhsContext.sensorWs[0].nodeStatus = HCU_L3BFHS_NODE_BOARD_STATUS_SUSPEND;
		//通知界面
		HCU_L3BFHS_TRIGGER_UI_STATUS_REPORT(HUICOBUS_CMDID_CUI_HCU2UIR_GENERAL_CMDVAL_SUSPEND);
	}

	//返回
	return SUCCESS;
}

OPSTAT fsm_l3bfhs_canitf_sys_resume_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HCU_MSG_RCV_CHECK_FOR_GEN_LOCAL(TASK_ID_L3BFHS, msg_struct_sui_resume_resp_t);

	//收到错误的反馈，就回复差错给界面
	if (rcv.validFlag == FALSE){
		msg_struct_l3bfhs_uicomm_ctrl_cmd_resp_t snd;
		memset(&snd, 0, sizeof(msg_struct_l3bfhs_uicomm_ctrl_cmd_resp_t));
		snd.validFlag = FALSE;
		snd.errCode = rcv.errCode;
		snd.cmdid = HCU_SYSMSG_BFHS_UICOMM_CMDID_RESUME;
		snd.length = sizeof(msg_struct_l3bfhs_uicomm_ctrl_cmd_resp_t);
		HCU_MSG_SEND_GENERNAL_PROCESS(MSG_ID_L3BFHS_UICOMM_CTRL_CMD_RESP, TASK_ID_BFHSUICOMM, TASK_ID_L3BFHS);
		hcu_timer_stop(TASK_ID_L3BFHS, TIMER_ID_1S_L3BFHS_RESUME_WAIT_FB, TIMER_RESOLUTION_1S);
		FsmSetState(TASK_ID_L3BFHS, FSM_STATE_L3BFHS_ACTIVED);

		//存储本地
		gTaskL3bfhsContext.sensorWs[0].nodeStatus = HCU_L3BFHS_NODE_BOARD_STATUS_STARTUP;
		//通知界面
		HCU_L3BFHS_TRIGGER_UI_STATUS_REPORT(HUICOBUS_CMDID_CUI_HCU2UIR_GENERAL_CMDVAL_CFG_OK);
	}

	//收到正确的反馈
	else{
		msg_struct_l3bfhs_uicomm_ctrl_cmd_resp_t snd;
		memset(&snd, 0, sizeof(msg_struct_l3bfhs_uicomm_ctrl_cmd_resp_t));
		snd.cmdid = HCU_SYSMSG_BFHS_UICOMM_CMDID_RESUME;
		snd.validFlag = TRUE;
		snd.length = sizeof(msg_struct_l3bfhs_uicomm_ctrl_cmd_resp_t);
		HCU_MSG_SEND_GENERNAL_PROCESS(MSG_ID_L3BFHS_UICOMM_CTRL_CMD_RESP, TASK_ID_BFHSUICOMM, TASK_ID_L3BFHS);
		hcu_timer_stop(TASK_ID_L3BFHS, TIMER_ID_1S_L3BFHS_RESUME_WAIT_FB, TIMER_RESOLUTION_1S);
		FsmSetState(TASK_ID_L3BFHS, FSM_STATE_L3BFHS_OOS_SCAN);

		//存储本地
		gTaskL3bfhsContext.sensorWs[0].nodeStatus = HCU_L3BFHS_NODE_BOARD_STATUS_VALID;
		//通知界面
		HCU_L3BFHS_TRIGGER_UI_STATUS_REPORT(HUICOBUS_CMDID_CUI_HCU2UIR_GENERAL_CMDVAL_DATA_VALID);
	}

	//返回
	return SUCCESS;
}

OPSTAT fsm_l3bfhs_canitf_cal_zero_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HCU_MSG_RCV_CHECK_FOR_GEN_LOCAL(TASK_ID_L3BFHS, msg_struct_can_l3bfhs_cal_zero_resp_t);

	//收到错误的反馈，就回复差错给界面
	if (rcv.validFlag == FALSE){
		msg_struct_l3bfhs_uicomm_ctrl_cmd_resp_t snd;
		memset(&snd, 0, sizeof(msg_struct_l3bfhs_uicomm_ctrl_cmd_resp_t));
		snd.validFlag = FALSE;
		snd.errCode = rcv.errCode;
		snd.cmdid = HCU_SYSMSG_BFHS_UICOMM_CMDID_STATIC_CALI;
		snd.cmdValue = HCU_SYSMSG_BFHS_UICOMM_CMDVALUE_STATIC_CALI_ZERO;
		snd.length = sizeof(msg_struct_l3bfhs_uicomm_ctrl_cmd_resp_t);
		HCU_MSG_SEND_GENERNAL_PROCESS(MSG_ID_L3BFHS_UICOMM_CTRL_CMD_RESP, TASK_ID_BFHSUICOMM, TASK_ID_L3BFHS);
		hcu_timer_stop(TASK_ID_L3BFHS, TIMER_ID_1S_L3BFHS_CAL_ZERO_WAIT_FB, TIMER_RESOLUTION_1S);
	}

	//收到正确的反馈
	else{
		msg_struct_l3bfhs_uicomm_ctrl_cmd_resp_t snd;
		memset(&snd, 0, sizeof(msg_struct_l3bfhs_uicomm_ctrl_cmd_resp_t));
		snd.cmdid = HCU_SYSMSG_BFHS_UICOMM_CMDID_STATIC_CALI;
		snd.cmdValue = HCU_SYSMSG_BFHS_UICOMM_CMDVALUE_STATIC_CALI_ZERO;
		snd.validFlag = TRUE;
		snd.length = sizeof(msg_struct_l3bfhs_uicomm_ctrl_cmd_resp_t);
		HCU_MSG_SEND_GENERNAL_PROCESS(MSG_ID_L3BFHS_UICOMM_CTRL_CMD_RESP, TASK_ID_BFHSUICOMM, TASK_ID_L3BFHS);
		hcu_timer_stop(TASK_ID_L3BFHS, TIMER_ID_1S_L3BFHS_CAL_ZERO_WAIT_FB, TIMER_RESOLUTION_1S);
	}

	//返回
	return SUCCESS;
}

OPSTAT fsm_l3bfhs_canitf_cal_full_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HCU_MSG_RCV_CHECK_FOR_GEN_LOCAL(TASK_ID_L3BFHS, msg_struct_can_l3bfhs_cal_full_resp_t);

	//收到错误的反馈，就回复差错给界面
	if (rcv.validFlag == FALSE){
		//发送反馈给UICOMM
		msg_struct_l3bfhs_uicomm_ctrl_cmd_resp_t snd;
		memset(&snd, 0, sizeof(msg_struct_l3bfhs_uicomm_ctrl_cmd_resp_t));
		snd.validFlag = FALSE;
		snd.errCode = rcv.errCode;
		snd.cmdid = HCU_SYSMSG_BFHS_UICOMM_CMDID_STATIC_CALI;
		snd.cmdValue = HCU_SYSMSG_BFHS_UICOMM_CMDVALUE_STATIC_CALI_FULL;
		memcpy(&snd.calFullRespPar, &rcv.calFullRespPar, sizeof(StrMsgIe_WeightSensorBfhsCalibrationFullRespParamaters_t));
		snd.length = sizeof(msg_struct_l3bfhs_uicomm_ctrl_cmd_resp_t);
		HCU_MSG_SEND_GENERNAL_PROCESS(MSG_ID_L3BFHS_UICOMM_CTRL_CMD_RESP, TASK_ID_BFHSUICOMM, TASK_ID_L3BFHS);
		hcu_timer_stop(TASK_ID_L3BFHS, TIMER_ID_1S_L3BFHS_CAL_ZERO_WAIT_FB, TIMER_RESOLUTION_1S);
	}

	//收到正确的反馈
	else{
		msg_struct_l3bfhs_uicomm_ctrl_cmd_resp_t snd;
		memset(&snd, 0, sizeof(msg_struct_l3bfhs_uicomm_ctrl_cmd_resp_t));
		snd.validFlag = TRUE;
		snd.errCode = rcv.errCode;
		snd.cmdid = HCU_SYSMSG_BFHS_UICOMM_CMDID_STATIC_CALI;
		snd.cmdValue = HCU_SYSMSG_BFHS_UICOMM_CMDVALUE_STATIC_CALI_FULL;
		memcpy(&snd.calFullRespPar, &rcv.calFullRespPar, sizeof(StrMsgIe_WeightSensorBfhsCalibrationFullRespParamaters_t));
		//Save calibration response parameters into DB
		if (dbi_HcuBfhs_calData_save(&rcv.calFullRespPar) == FAILURE)
			HCU_ERROR_PRINT_L3BFHS("L3BFHS: Save calibration data into DB error!\n");
		snd.length = sizeof(msg_struct_l3bfhs_uicomm_ctrl_cmd_resp_t);
		HCU_MSG_SEND_GENERNAL_PROCESS(MSG_ID_L3BFHS_UICOMM_CTRL_CMD_RESP, TASK_ID_BFHSUICOMM, TASK_ID_L3BFHS);
		hcu_timer_stop(TASK_ID_L3BFHS, TIMER_ID_1S_L3BFHS_CAL_FULL_WAIT_FB, TIMER_RESOLUTION_1S);
	}

	//返回
	return SUCCESS;
}

OPSTAT fsm_l3bfhs_canitf_dyn_zero_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HCU_MSG_RCV_CHECK_FOR_GEN_LOCAL(TASK_ID_L3BFHS, msg_struct_can_l3bfhs_dyn_zero_resp_t);

	//收到错误的反馈，就回复差错给界面
	if (rcv.validFlag == FALSE){
		msg_struct_l3bfhs_uicomm_ctrl_cmd_resp_t snd;
		memset(&snd, 0, sizeof(msg_struct_l3bfhs_uicomm_ctrl_cmd_resp_t));
		snd.validFlag = FALSE;
		snd.errCode = rcv.errCode;
		snd.cmdid = HCU_SYSMSG_BFHS_UICOMM_CMDID_DYNAMIC_CALI;
		snd.cmdValue = HCU_SYSMSG_BFHS_UICOMM_CMDVALUE_DYNAMIC_CALI_ZERO;
		snd.length = sizeof(msg_struct_l3bfhs_uicomm_ctrl_cmd_resp_t);
		HCU_MSG_SEND_GENERNAL_PROCESS(MSG_ID_L3BFHS_UICOMM_CTRL_CMD_RESP, TASK_ID_BFHSUICOMM, TASK_ID_L3BFHS);
	}

	//收到正确的反馈
	else{
		msg_struct_l3bfhs_uicomm_ctrl_cmd_resp_t snd;
		memset(&snd, 0, sizeof(msg_struct_l3bfhs_uicomm_ctrl_cmd_resp_t));
		snd.cmdid = HCU_SYSMSG_BFHS_UICOMM_CMDID_DYNAMIC_CALI;
		snd.cmdValue = HCU_SYSMSG_BFHS_UICOMM_CMDVALUE_DYNAMIC_CALI_ZERO;
		snd.validFlag = TRUE;
		snd.length = sizeof(msg_struct_l3bfhs_uicomm_ctrl_cmd_resp_t);
		HCU_MSG_SEND_GENERNAL_PROCESS(MSG_ID_L3BFHS_UICOMM_CTRL_CMD_RESP, TASK_ID_BFHSUICOMM, TASK_ID_L3BFHS);
	}

	//返回
	return SUCCESS;
}

OPSTAT fsm_l3bfhs_canitf_dyn_full_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HCU_MSG_RCV_CHECK_FOR_GEN_LOCAL(TASK_ID_L3BFHS, msg_struct_can_l3bfhs_dyn_full_resp_t);

	//收到错误的反馈，就回复差错给界面
	if (rcv.validFlag == FALSE){
		//发送反馈给UICOMM
		msg_struct_l3bfhs_uicomm_ctrl_cmd_resp_t snd;
		memset(&snd, 0, sizeof(msg_struct_l3bfhs_uicomm_ctrl_cmd_resp_t));
		snd.validFlag = FALSE;
		snd.errCode = rcv.errCode;
		snd.cmdid = HCU_SYSMSG_BFHS_UICOMM_CMDID_DYNAMIC_CALI;
		snd.cmdValue = HCU_SYSMSG_BFHS_UICOMM_CMDVALUE_DYNAMIC_CALI_FULL;
		snd.length = sizeof(msg_struct_l3bfhs_uicomm_ctrl_cmd_resp_t);
		HCU_MSG_SEND_GENERNAL_PROCESS(MSG_ID_L3BFHS_UICOMM_CTRL_CMD_RESP, TASK_ID_BFHSUICOMM, TASK_ID_L3BFHS);
		hcu_timer_stop(TASK_ID_L3BFHS, TIMER_ID_1S_L3BFHS_CAL_ZERO_WAIT_FB, TIMER_RESOLUTION_1S);
	}

	//收到正确的反馈
	else{
		msg_struct_l3bfhs_uicomm_ctrl_cmd_resp_t snd;
		memset(&snd, 0, sizeof(msg_struct_l3bfhs_uicomm_ctrl_cmd_resp_t));
		snd.cmdid = HCU_SYSMSG_BFHS_UICOMM_CMDID_DYNAMIC_CALI;
		snd.cmdValue = HCU_SYSMSG_BFHS_UICOMM_CMDVALUE_DYNAMIC_CALI_FULL;
		snd.validFlag = TRUE;
		snd.length = sizeof(msg_struct_l3bfhs_uicomm_ctrl_cmd_resp_t);
		HCU_MSG_SEND_GENERNAL_PROCESS(MSG_ID_L3BFHS_UICOMM_CTRL_CMD_RESP, TASK_ID_BFHSUICOMM, TASK_ID_L3BFHS);
		hcu_timer_stop(TASK_ID_L3BFHS, TIMER_ID_1S_L3BFHS_CAL_FULL_WAIT_FB, TIMER_RESOLUTION_1S);
	}

	//返回
	return SUCCESS;
}

OPSTAT fsm_l3bfhs_canitf_startup_ind(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HCU_MSG_RCV_CHECK_FOR_GEN_LOCAL(TASK_ID_L3BFHS, msg_struct_sui_startup_ind_t);

	//存储本地
	gTaskL3bfhsContext.sensorWs[0].nodeStatus = HCU_L3BFHS_NODE_BOARD_STATUS_STARTUP;
	HCU_DEBUG_PRINT_CRT("L3BFHS: Sensor ID = %d is set to be startup!\n", rcv.snrId);

	//通知界面
	HCU_L3BFHS_TRIGGER_UI_STATUS_REPORT(HUICOBUS_CMDID_CUI_HCU2UIR_GENERAL_CMDVAL_STARTUP);

	//如果判定为SUSPEND状态，则直接复位主状态机
	if (FsmGetState(TASK_ID_L3BFHS) == FSM_STATE_L3BFHS_SUSPEND){
		FsmSetState(TASK_ID_L3BFHS, FSM_STATE_L3BFHS_ACTIVED);
		return SUCCESS;
	}

	//判定状态
	if (FsmGetState(TASK_ID_L3BFHS) > FSM_STATE_L3BFHS_ACTIVED){
		//Send CFG_REQ到下位机
		if (func_l3bfhs_send_out_sys_cfg_req() == FAILURE)
			HCU_ERROR_PRINT_L3BFHS("L3BFHS: Send message error!\n");
		FsmSetState(TASK_ID_L3BFHS, FSM_STATE_L3BFHS_ACTIVED);
	}

	//返回
	return SUCCESS;
}

OPSTAT fsm_l3bfhs_canitf_fault_ind(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HCU_MSG_RCV_CHECK_FOR_GEN_LOCAL(TASK_ID_L3BFHS, msg_struct_sui_fault_ind_t);

	//本地存储
	if (gTaskL3bfhsContext.sensorWs[0].nodeStatus < HCU_L3BFHS_NODE_BOARD_STATUS_INIT_MAX)
		gTaskL3bfhsContext.sensorWs[0].nodeStatus = HCU_L3BFHS_NODE_BOARD_STATUS_INIT_ERR;
	else
		//隔离该传感器
		gTaskL3bfhsContext.sensorWs[0].nodeStatus = HCU_L3BFHS_NODE_BOARD_STATUS_VALID_ERROR;

	//通知界面
	StrHlcIe_cui_hcu2uir_status_report_t status;
	memset(&status, 0, sizeof(StrHlcIe_cui_hcu2uir_status_report_t));
	if (gTaskL3bfhsContext.sensorWs[0].nodeStatus == HCU_L3BFHS_NODE_BOARD_STATUS_INIT_ERR)
		status.boardStatus = HUICOBUS_CMDID_CUI_HCU2UIR_GENERAL_CMDVAL_CFG_ERR;
	else
	status.boardStatus = HUICOBUS_CMDID_CUI_HCU2UIR_GENERAL_CMDVAL_ERROR;
	hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_status_report(0, &status);

	//是否要根据ERR_CODE，赋予不同的差错情形，待定

	//设置到ACTIVE状态，等待界面进一步反馈
	FsmSetState(TASK_ID_L3BFHS, FSM_STATE_L3BFHS_ACTIVED);

	//返回
	return SUCCESS;
}

OPSTAT fsm_l3bfhs_canitf_heart_beat_report(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HCU_MSG_RCV_CHECK_FOR_GEN_LOCAL(TASK_ID_L3BFHS, msg_struct_sui_heart_beat_report_t);

	//本地存储：只是为了防止下位机重启
	if (gTaskL3bfhsContext.sensorWs[0].nodeStatus < HCU_L3BFHS_NODE_BOARD_STATUS_STARTUP){
		gTaskL3bfhsContext.sensorWs[0].nodeStatus = HCU_L3BFHS_NODE_BOARD_STATUS_STARTUP;
		//通知界面
		HCU_L3BFHS_TRIGGER_UI_STATUS_REPORT(HUICOBUS_CMDID_CUI_HCU2UIR_GENERAL_CMDVAL_STARTUP);
	}

	//回送
	msg_struct_sui_heart_beat_confirm_t snd;
	memset(&snd, 0, sizeof(msg_struct_sui_heart_beat_confirm_t));
	snd.snrId = rcv.snrId;
	if (gTaskL3bfhsContext.sensorWs[0].nodeStatus <= HCU_L3BFHS_NODE_BOARD_STATUS_OFFLINE_MAX)
		snd.state = HUITP_IEID_SUI_COM_HEATT_BEAT_WMC_STATE_OFFLINE;
	else if (gTaskL3bfhsContext.sensorWs[0].nodeStatus < HCU_L3BFHS_NODE_BOARD_STATUS_VALID)
		snd.state = HUITP_IEID_SUI_COM_HEATT_BEAT_WMC_STATE_INIT;
	else if(gTaskL3bfhsContext.sensorWs[0].nodeStatus <= HCU_L3BFHS_NODE_BOARD_STATUS_WORK_MAX)
		snd.state = HUITP_IEID_SUI_COM_HEATT_BEAT_WMC_STATE_WORKING;
	else
		snd.state = HUITP_IEID_SUI_COM_HEATT_BEAT_WMC_STATE_INVALID;
	snd.timeStamp = time(0);
	snd.length = sizeof(msg_struct_sui_heart_beat_confirm_t);
	HCU_MSG_SEND_GENERNAL_PROCESS(MSG_ID_SUI_HEART_BEAT_CONFIRM, TASK_ID_CANALPHA, TASK_ID_L3BFHS);

	//返回
	return SUCCESS;
}


/***************************************************************************************************************************
 *
 *  CLOUDVELA部分的消息处理
 *
 ***************************************************************************************************************************/
//后台来的命令，发送到CANALPHA模块，执行命令后，再返回给后台
//由于是内部消息命令执行，为了简化整个执行，不设置超时状态，以简化整个状态机的设计
OPSTAT fsm_l3bfhs_cloudvela_data_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	return SUCCESS;
}

//由于是内部消息命令执行，为了简化整个执行，不设置超时状态，以简化整个状态机的设计
OPSTAT fsm_l3bfhs_cloudvela_data_confirm(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	return SUCCESS;
}

//由于是内部消息命令执行，为了简化整个执行，不设置超时状态，以简化整个状态机的设计
OPSTAT fsm_l3bfhs_cloudvela_event_confirm(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	return SUCCESS;
}

//由于是内部消息命令执行，为了简化整个执行，不设置超时状态，以简化整个状态机的设计
OPSTAT fsm_l3bfhs_cloudvela_ctrl_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret=0;

	//入参检查
	msg_struct_cloudvela_l3bfhs_ctrl_req_t rcv;
	HCU_MSG_RCV_CHECK_FOR_LOCALIZE(TASK_ID_L3BFHS, msg_struct_cloudvela_l3bfhs_ctrl_req_t);
//	if (rcv.cmdid != L3CI_bfhs_comb_scale){
//		HCU_ERROR_PRINT_L3BFHS_RECOVERY("L3BFHS: Receive message error!\n");
//	}

	//将命令发送到传感器下位机
//	msg_struct_l3bfhs_can_general_cmd_req_t snd;
//	memset(&snd, 0, sizeof(msg_struct_l3bfhs_can_general_cmd_req_t));
//	snd.length = sizeof(msg_struct_l3bfhs_can_general_cmd_req_t);

	//这里只支持一种启动和停止命令，其它的暂时不支持
	//if ((rcv.optid != L3PO_bfhs_start_cmd) && (rcv.optid != L3PO_bfhs_stop_cmd)){
//	if ((rcv.scaleWeightCmd != L3PO_bfhs_start_cmd) && (rcv.scaleWeightCmd != L3PO_bfhs_stop_cmd)){
//				HCU_ERROR_PRINT_L3BFHS_RECOVERY("L3BFHS: Can not supported command coming from cloud!\n");
//	}

//	snd.optid = rcv.optid;
//	snd.optpar = rcv.optopr;
	//这里如此设置，表示是为了全局所有的传感器
//	snd.sensorid = HCU_SYSCFG_BFHS_SNR_WS_NBR_MAX;
//	ret = hcu_message_send(MSG_ID_L3BFHS_CAN_GENERAL_CMD_REQ, TASK_ID_CANALPHA, TASK_ID_L3BFHS, &snd, snd.length);
//	if (ret == FAILURE){
//		HCU_ERROR_PRINT_L3BFHS_RECOVERY("L3BFHS: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3BFHS].taskName, zHcuVmCtrTab.task[TASK_ID_CANALPHA].taskName);
//	}

	//状态不转移

	//短时钟不启动

	//返回
	return SUCCESS;
}

//由于是内部消息命令执行，为了简化整个执行，不设置超时状态，以简化整个状态机的设计
OPSTAT fsm_l3bfhs_cloudvela_statistic_confirm(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	return SUCCESS;
}



/***************************************************************************************************************************
 *
 *  CANITF下位机传感器处理的结果
 *
 ***************************************************************************************************************************/
//触发组合算法
OPSTAT fsm_l3bfhs_canitf_ws_new_ready_event(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HCU_MSG_RCV_CHECK_FOR_GEN_LOCAL(TASK_ID_L3BFHS, msg_struct_can_l3bfhs_new_ready_event_t);

	//通知界面
	StrHlcIe_cui_hcu2uir_callcell_bfhs_report_t status;
	memset(&status, 0, sizeof(StrHlcIe_cui_hcu2uir_callcell_bfhs_report_t));
	status.totalWeight = rcv.snrWsValue;
	status.upperLimit = rcv.snrState;
	hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_callcell_bfhs_report(0, &status);

	//PURE TEST, to be removed!
	StrHlcIe_cui_hcu2uir_inswgt_bfhs_report_t buf;
	memset(&buf, 0, sizeof(StrHlcIe_cui_hcu2uir_inswgt_bfhs_report_t));
	buf.weight = rcv.snrWsValue;
	buf.wmcState = rcv.snrState;
	hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_inswgt_bfhs_report(buf.weight, &buf);

	//正常处理
	gTaskL3bfhsContext.sessionId++;
	gTaskL3bfhsContext.cur.wsIncMatCnt++;
	gTaskL3bfhsContext.cur.wsIncMatWgt += rcv.snrWsValue;
	if (rcv.snrState == HUITP_IEID_SUI_BFHS_NEW_EVENT_STATE_NORMAL){
		gTaskL3bfhsContext.cur.wsNormalCnt++;
		gTaskL3bfhsContext.cur.wsNormalWgt += rcv.snrWsValue;
	}
	else if (rcv.snrState == HUITP_IEID_SUI_BFHS_NEW_EVENT_STATE_OVERLOAD){
		gTaskL3bfhsContext.cur.wsOverCnt++;
		gTaskL3bfhsContext.cur.wsOverWgt += rcv.snrWsValue;
	}
	//未来进一步分解为TU1/TU2
	else if (rcv.snrState == HUITP_IEID_SUI_BFHS_NEW_EVENT_STATE_UNDERLOAD){
		gTaskL3bfhsContext.cur.wsUnderTotalCnt++;
		gTaskL3bfhsContext.cur.wsUnderTotalWgt += rcv.snrWsValue;
		gTaskL3bfhsContext.cur.wsUnderTu2Cnt++;
		gTaskL3bfhsContext.cur.wsUnderTu2Wgt += rcv.snrWsValue;
	}
	else{
		gTaskL3bfhsContext.cur.wsUnspecificCnt++;
		gTaskL3bfhsContext.cur.wsUnspecificWgt += rcv.snrWsValue;
	}

	//存入数据库表单
	HcuSysMsgIeL3bfhsCallcellElement_t callcell;
	memset(&callcell, 0, sizeof(HcuSysMsgIeL3bfhsCallcellElement_t));
	callcell.sid = gTaskL3bfhsContext.sessionId;
	callcell.configId = gTaskL3bfhsContext.configId;
	callcell.timestamp = time(0);
	strncpy(callcell.operator, gTaskL3bfhsContext.operatorName, sizeof(gTaskL3bfhsContext.operatorName)<sizeof(callcell.operator)?sizeof(gTaskL3bfhsContext.operatorName):sizeof(callcell.operator));
	callcell.targetWeight = gTaskL3bfhsContext.wgtSnrPar.minAllowedWeight;
	callcell.actualWeight = rcv.snrWsValue;
	callcell.state = rcv.snrState;
	callcell.tu1 = 0;  //TBD
	callcell.tu2 = 0;  //TBD
	if (dbi_HcuBfhs_callcell_save(&callcell) == FAILURE)
		HCU_ERROR_PRINT_L3BFHS("L3BFHS: Save data into db error!\n");

	//返回
	return SUCCESS;
}

/***************************************************************************************************************************
 *
 *  本地界面UI通信部分
 *
 ***************************************************************************************************************************/
OPSTAT fsm_l3bfhs_uicomm_ctrl_cmd_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HCU_MSG_RCV_CHECK_FOR_GEN_LOCAL(TASK_ID_L3BFHS, msg_struct_uicomm_l3bfhs_ctrl_cmd_req_t);

	//启动命令
	if ((rcv.cmdid == HCU_SYSMSG_BFHS_UICOMM_CMDID_CFG_START) || (rcv.cmdid == HCU_SYSMSG_BFHS_UICOMM_CMDID_RESUME)){
		//不合法，直接退回
		if (gTaskL3bfhsContext.sensorWs[0].nodeStatus < HCU_L3BFHS_NODE_BOARD_STATUS_STARTUP){
			HCU_L3BFHS_FEEDBACK_CTRL_RESP_MESSAGE(HCU_SYSMSG_BFHS_UICOMM_CMDID_CFG_START);
		}
		//如果是工作态度，则需要发送RESUME
		else if (FsmGetState(TASK_ID_L3BFHS) == FSM_STATE_L3BFHS_SUSPEND){
			msg_struct_sui_resume_req_t snd;
			memset(&snd, 0, sizeof(msg_struct_sui_resume_req_t));
			snd.length = sizeof(msg_struct_sui_resume_req_t);
			HCU_MSG_SEND_GENERNAL_PROCESS(MSG_ID_SUI_RESUME_REQ, TASK_ID_CANALPHA, TASK_ID_L3BFHS);
			hcu_timer_start(TASK_ID_L3BFHS, HCU_TIMERID_WITH_DUR(TIMER_ID_1S_L3BFHS_RESUME_WAIT_FB), TIMER_TYPE_ONE_TIME, TIMER_RESOLUTION_1S);
		}
		else{
			//合法，发送给下位机
			if (func_l3bfhs_send_out_sys_cfg_req() == FAILURE)
				HCU_ERROR_PRINT_L3BFHS("L3BFHS: Send message error!\n");
			hcu_timer_start(TASK_ID_L3BFHS, HCU_TIMERID_WITH_DUR(TIMER_ID_1S_L3BFHS_CFG_START_WAIT_FB), TIMER_TYPE_ONE_TIME, TIMER_RESOLUTION_1S);
			FsmSetState(TASK_ID_L3BFHS, FSM_STATE_L3BFHS_ACTIVED);
		}
	}

	//STOP/SUSPEND：可以不用处理，因为收不到
	else if ((rcv.cmdid == HCU_SYSMSG_BFHS_UICOMM_CMDID_STOP) || (rcv.cmdid == HCU_SYSMSG_BFHS_UICOMM_CMDID_SUSPEND)){
		if (FsmGetState(TASK_ID_L3BFHS) == FSM_STATE_L3BFHS_OOS_SCAN){
			msg_struct_sui_suspend_req_t snd;
			memset(&snd, 0, sizeof(msg_struct_sui_suspend_req_t));
			snd.length = sizeof(msg_struct_sui_suspend_req_t);
			HCU_MSG_SEND_GENERNAL_PROCESS(MSG_ID_SUI_SUSPEND_REQ, TASK_ID_CANALPHA, TASK_ID_L3BFHS);
			hcu_timer_start(TASK_ID_L3BFHS, HCU_TIMERID_WITH_DUR(TIMER_ID_1S_L3BFHS_SUSPEND_WAIT_FB), TIMER_TYPE_ONE_TIME, TIMER_RESOLUTION_1S);
			FsmSetState(TASK_ID_L3BFHS, FSM_STATE_L3BFHS_ACTIVED);
		}
		else{
			HCU_L3BFHS_FEEDBACK_CTRL_RESP_MESSAGE(HCU_SYSMSG_BFHS_UICOMM_CMDID_SUSPEND);
		}
	}

	//STATIC_CALI
	else if (rcv.cmdid == HCU_SYSMSG_BFHS_UICOMM_CMDID_STATIC_CALI){
		if(rcv.cmdValue == HCU_SYSMSG_BFHS_UICOMM_CMDVALUE_STATIC_CALI_ZERO){
			msg_struct_l3bfhs_can_cal_zero_req_t snd;
			memset(&snd, 0, sizeof(msg_struct_l3bfhs_can_cal_zero_req_t));
			snd.boardBitmap[1] = 1;
			snd.calZeroPar.WeightSensorAutoZero = gTaskL3bfhsContext.wgtSnrPar.snrAutoZeroSwitch;
			snd.calZeroPar.WeightSensorAutoZeroAutotaringTimeMs = gTaskL3bfhsContext.wgtSnrPar.snrAutoZeroAutotaringTimeMs;
			snd.calZeroPar.WeightSensorAutoZeroCaptureRangeGrams = gTaskL3bfhsContext.wgtSnrPar.snrAutoZeroCaptureRangeGrams;
			snd.calZeroPar.WeightSensorFilterCutOffFreqHz = gTaskL3bfhsContext.wgtSnrPar.snrFilterCutOffFreqHz;
			snd.calZeroPar.WeightSensorMeasurementRangeNo = gTaskL3bfhsContext.wgtSnrPar.snrMeasurementRangeNo;
			snd.calZeroPar.WeightSensorPreloadComPensationPlacesAfterDecimalPoint = gTaskL3bfhsContext.wgtSnrPar.snrPreloadCompensationDataFormat;
			snd.calZeroPar.WeightSensorPreloadComPensationValuePercent = gTaskL3bfhsContext.wgtSnrPar.snrPreloadCompensationValue;
			snd.calZeroPar.WeightSensorRingBufTimeMs = gTaskL3bfhsContext.wgtSnrPar.snrRingBufTimeMs;
			snd.calZeroPar.WeightSensorStandstillRangeGrams = gTaskL3bfhsContext.wgtSnrPar.snrStandstillRangeGrams;
			snd.calZeroPar.WeightSensorStandstillTime = gTaskL3bfhsContext.wgtSnrPar.snrStandstillTime;
			snd.calZeroPar.WeightSensorStandstillTimeoutMs = gTaskL3bfhsContext.wgtSnrPar.snrStandstillTimeoutMs;
			snd.calZeroPar.spare1 = 0;
			snd.calZeroPar.spare2 = 0;
			snd.length = sizeof(msg_struct_l3bfhs_can_cal_zero_req_t);
			HCU_MSG_SEND_GENERNAL_PROCESS(MSG_ID_L3BFHS_CAN_CAL_ZERO_REQ, TASK_ID_CANALPHA, TASK_ID_L3BFHS);
			hcu_timer_start(TASK_ID_L3BFHS, HCU_TIMERID_WITH_DUR(TIMER_ID_1S_L3BFHS_CAL_ZERO_WAIT_FB), TIMER_TYPE_ONE_TIME, TIMER_RESOLUTION_1S);
		}
		else if(rcv.cmdValue == HCU_SYSMSG_BFHS_UICOMM_CMDVALUE_STATIC_CALI_FULL){
			msg_struct_l3bfhs_can_cal_full_req_t snd;
			memset(&snd, 0, sizeof(msg_struct_l3bfhs_can_cal_full_req_t));
			snd.boardBitmap[1] = 1;
			snd.calFullPar.WeightSensorAdjustingTolerancePercent = gTaskL3bfhsContext.calFullReqPar.WeightSensorAdjustingTolerancePercent;
			snd.calFullPar.WeightSensorAdjustingWeightGrams = gTaskL3bfhsContext.calFullReqPar.WeightSensorAdjustingWeightGrams;
			snd.length = sizeof(msg_struct_l3bfhs_can_cal_full_req_t);
			HCU_MSG_SEND_GENERNAL_PROCESS(MSG_ID_L3BFHS_CAN_CAL_FULL_REQ, TASK_ID_CANALPHA, TASK_ID_L3BFHS);
			hcu_timer_start(TASK_ID_L3BFHS, HCU_TIMERID_WITH_DUR(TIMER_ID_1S_L3BFHS_CAL_FULL_WAIT_FB), TIMER_TYPE_ONE_TIME, TIMER_RESOLUTION_1S);
		}
		else
			HCU_ERROR_PRINT_L3BFHS("L3BFHS: Receive cmdValue error for CMDID_STATIC_CALI!\n");
	}
	//DYNAMIC_CALI
	else if (rcv.cmdid == HCU_SYSMSG_BFHS_UICOMM_CMDID_DYNAMIC_CALI){
		if(rcv.cmdValue == HCU_SYSMSG_BFHS_UICOMM_CMDVALUE_DYNAMIC_CALI_ZERO){
			msg_struct_l3bfhs_can_dyn_zero_req_t snd;
			memset(&snd, 0, sizeof(msg_struct_l3bfhs_can_dyn_zero_req_t));
			snd.boardBitmap[1] = 1;
			snd.length = sizeof(msg_struct_l3bfhs_can_dyn_zero_req_t);
			HCU_MSG_SEND_GENERNAL_PROCESS(MSG_ID_L3BFHS_CAN_DYN_ZERO_REQ, TASK_ID_CANALPHA, TASK_ID_L3BFHS);
			//动态校准可以重复进行，一次request后用户重复放置砝码会有多次response返回
		}
		else if(rcv.cmdValue == HCU_SYSMSG_BFHS_UICOMM_CMDVALUE_DYNAMIC_CALI_FULL){
			msg_struct_l3bfhs_can_dyn_full_req_t snd;
			memset(&snd, 0, sizeof(msg_struct_l3bfhs_can_dyn_full_req_t));
			snd.boardBitmap[1] = 1;
			snd.adjustingWeight = gTaskL3bfhsContext.calFullReqPar.WeightSensorAdjustingWeightGrams;
			snd.motorSpeed = gTaskL3bfhsContext.motoCtrlPar.MotorSpeed;
			snd.motorDirection = gTaskL3bfhsContext.motoCtrlPar.MotorDirection;
			snd.length = sizeof(msg_struct_l3bfhs_can_dyn_full_req_t);
			HCU_MSG_SEND_GENERNAL_PROCESS(MSG_ID_L3BFHS_CAN_DYN_FULL_REQ, TASK_ID_CANALPHA, TASK_ID_L3BFHS);
			//动态校准可以重复进行，一次request后用户重复放置砝码会有多次response返回
		}
		else
			HCU_ERROR_PRINT_L3BFHS("L3BFHS: Receive cmdValue error for CMDID_DYNAMIC_CALI!\n");
	}

	//差错
	else HCU_ERROR_PRINT_L3BFHS("L3BFHS: Receive message error! Received Cmdid = %d, CmdValue=%d\n", rcv.cmdid, rcv.cmdValue);

	//返回
	return SUCCESS;
}

/***************************************************************************************************************************
 *
 * 　TIME_OUT处理过程
 *
 ***************************************************************************************************************************/
OPSTAT func_l3bfhs_time_out_sys_cfg_start_wait_fb_process(void)
{
	//发送反馈给UICOMM
	msg_struct_l3bfhs_uicomm_ctrl_cmd_resp_t snd;
	memset(&snd, 0, sizeof(msg_struct_l3bfhs_uicomm_ctrl_cmd_resp_t));
	snd.cmdid = HCU_SYSMSG_BFHS_UICOMM_CMDID_CFG_START;
	snd.validFlag = FALSE;
	snd.errCode = HCU_SYSMSG_BFHS_ERR_CODE_TIME_OUT;
	snd.length = sizeof(msg_struct_l3bfhs_uicomm_ctrl_cmd_resp_t);
	HCU_MSG_SEND_GENERNAL_PROCESS(MSG_ID_L3BFHS_UICOMM_CTRL_CMD_RESP, TASK_ID_BFHSUICOMM, TASK_ID_L3BFHS);
	FsmSetState(TASK_ID_L3BFHS, FSM_STATE_L3BFHS_ACTIVED);

	//返回
	return SUCCESS;
}

OPSTAT func_l3bfhs_time_out_sys_suspend_wait_fb_process(void)
{
	//发送反馈给UICOMM
	msg_struct_l3bfhs_uicomm_ctrl_cmd_resp_t snd;
	memset(&snd, 0, sizeof(msg_struct_l3bfhs_uicomm_ctrl_cmd_resp_t));
	snd.cmdid = HCU_SYSMSG_BFHS_UICOMM_CMDID_SUSPEND;
	snd.validFlag = FALSE;
	snd.errCode = HCU_SYSMSG_BFHS_ERR_CODE_TIME_OUT;
	snd.length = sizeof(msg_struct_l3bfhs_uicomm_ctrl_cmd_resp_t);
	HCU_MSG_SEND_GENERNAL_PROCESS(MSG_ID_L3BFHS_UICOMM_CTRL_CMD_RESP, TASK_ID_BFHSUICOMM, TASK_ID_L3BFHS);
	FsmSetState(TASK_ID_L3BFHS, FSM_STATE_L3BFHS_ACTIVED);

	//返回
	return SUCCESS;
}

OPSTAT func_l3bfhs_time_out_sys_resume_wait_fb_process(void)
{
	//发送反馈给UICOMM
	msg_struct_l3bfhs_uicomm_ctrl_cmd_resp_t snd;
	memset(&snd, 0, sizeof(msg_struct_l3bfhs_uicomm_ctrl_cmd_resp_t));
	snd.cmdid = HCU_SYSMSG_BFHS_UICOMM_CMDID_RESUME;
	snd.validFlag = FALSE;
	snd.errCode = HCU_SYSMSG_BFHS_ERR_CODE_TIME_OUT;
	snd.length = sizeof(msg_struct_l3bfhs_uicomm_ctrl_cmd_resp_t);
	HCU_MSG_SEND_GENERNAL_PROCESS(MSG_ID_L3BFHS_UICOMM_CTRL_CMD_RESP, TASK_ID_BFHSUICOMM, TASK_ID_L3BFHS);
	FsmSetState(TASK_ID_L3BFHS, FSM_STATE_L3BFHS_ACTIVED);

	//返回
	return SUCCESS;
}

OPSTAT func_l3bfhs_time_out_cal_zero_wait_fb_process(void)
{
	//发送反馈给UICOMM
	msg_struct_l3bfhs_uicomm_ctrl_cmd_resp_t snd;
	memset(&snd, 0, sizeof(msg_struct_l3bfhs_uicomm_ctrl_cmd_resp_t));
	snd.cmdid = HCU_SYSMSG_BFHS_UICOMM_CMDID_STATIC_CALI;
	snd.validFlag = FALSE;
	snd.errCode = HCU_SYSMSG_BFHS_ERR_CODE_TIME_OUT;
	snd.length = sizeof(msg_struct_l3bfhs_uicomm_ctrl_cmd_resp_t);
	HCU_MSG_SEND_GENERNAL_PROCESS(MSG_ID_L3BFHS_UICOMM_CTRL_CMD_RESP, TASK_ID_BFHSUICOMM, TASK_ID_L3BFHS);
	//不设置状态机

	//返回
	return SUCCESS;
}

OPSTAT func_l3bfhs_time_out_cal_full_wait_fb_process(void)
{
	//发送反馈给UICOMM
	msg_struct_l3bfhs_uicomm_ctrl_cmd_resp_t snd;
	memset(&snd, 0, sizeof(msg_struct_l3bfhs_uicomm_ctrl_cmd_resp_t));
	snd.cmdid = HCU_SYSMSG_BFHS_UICOMM_CMDID_STATIC_CALI;
	snd.validFlag = FALSE;
	snd.errCode = HCU_SYSMSG_BFHS_ERR_CODE_TIME_OUT;
	snd.length = sizeof(msg_struct_l3bfhs_uicomm_ctrl_cmd_resp_t);
	HCU_MSG_SEND_GENERNAL_PROCESS(MSG_ID_L3BFHS_UICOMM_CTRL_CMD_RESP, TASK_ID_BFHSUICOMM, TASK_ID_L3BFHS);
	//不设置状态机

	//返回
	return SUCCESS;
}

OPSTAT func_l3bfhs_send_out_sys_cfg_req(void)
{
	msg_struct_l3bfhs_can_sys_cfg_req_t snd;
	memset(&snd, 0, sizeof(msg_struct_l3bfhs_can_sys_cfg_req_t));
	memcpy(&snd.wgtSnrPar, &gTaskL3bfhsContext.wgtSnrPar, sizeof(gTaskL3bfhsContextWgtSnrParamaters_t));
	memcpy(&snd.motoCtrlPar, &gTaskL3bfhsContext.motoCtrlPar, sizeof(gTaskL3bfhsContextMotoCtrlParamaters_t));
	memcpy(&snd.armCtrlPar, &gTaskL3bfhsContext.armCtrlPar, sizeof(gTaskL3bfhsContextArmCtrlParamaters_t));
	snd.length = sizeof(msg_struct_l3bfhs_can_sys_cfg_req_t);
	HCU_MSG_SEND_GENERNAL_PROCESS(MSG_ID_L3BFHS_CAN_SYS_CFG_REQ, TASK_ID_CANALPHA, TASK_ID_L3BFHS);

	//返回
	return SUCCESS;
}




/***************************************************************************************************************************
 *
 * 　周期性扫描过程
 *
 ***************************************************************************************************************************/
//周期性统计扫描定时器
OPSTAT func_l3bfhs_time_out_statistic_scan_process(void)
{
	//int ret = 0;

	//首先增加时间流逝的计数器
	gTaskL3bfhsContext.elipseCnt++;
	gTaskL3bfhsContext.elipse24HourCnt++;

	//采取老化算法 x(n+1) = x(n) * (1-1/120) + latest，从而得到最新的数据，但该数据最好使用float，然后再转换为UINT32存入到数据库表单中
	memset(&(gTaskL3bfhsContext.staLocalUi), 0, sizeof(HcuSysMsgIeL3bfhsContextStaElement_t));
	//先存储临时数据
	gTaskL3bfhsContext.curAge.wsIncMatCntMid = gTaskL3bfhsContext.curAge.wsIncMatCntMid*(float)HCU_L3BFHS_STA_AGEING_COEF + (float)HCU_L3BFHS_STA_AGEING_COEF_ALPHA*gTaskL3bfhsContext.cur.wsIncMatCnt;
	gTaskL3bfhsContext.curAge.wsIncMatWgtMid = gTaskL3bfhsContext.curAge.wsIncMatWgtMid*(float)HCU_L3BFHS_STA_AGEING_COEF + (float)HCU_L3BFHS_STA_AGEING_COEF_ALPHA*gTaskL3bfhsContext.cur.wsIncMatWgt;
	gTaskL3bfhsContext.curAge.wsNormalCntMid = gTaskL3bfhsContext.curAge.wsNormalCntMid*(float)HCU_L3BFHS_STA_AGEING_COEF + (float)HCU_L3BFHS_STA_AGEING_COEF_ALPHA*gTaskL3bfhsContext.cur.wsNormalCnt;
	gTaskL3bfhsContext.curAge.wsNormalWgtMid = gTaskL3bfhsContext.curAge.wsNormalWgtMid*(float)HCU_L3BFHS_STA_AGEING_COEF + (float)HCU_L3BFHS_STA_AGEING_COEF_ALPHA*gTaskL3bfhsContext.cur.wsNormalWgt;
	gTaskL3bfhsContext.curAge.wsOverCntMid = gTaskL3bfhsContext.curAge.wsOverCntMid*(float)HCU_L3BFHS_STA_AGEING_COEF + (float)HCU_L3BFHS_STA_AGEING_COEF_ALPHA*gTaskL3bfhsContext.cur.wsOverCnt;
	gTaskL3bfhsContext.curAge.wsOverWgtMid = gTaskL3bfhsContext.curAge.wsOverWgtMid*(float)HCU_L3BFHS_STA_AGEING_COEF + (float)HCU_L3BFHS_STA_AGEING_COEF_ALPHA*gTaskL3bfhsContext.cur.wsOverWgt;
	gTaskL3bfhsContext.curAge.wsUnderTotalCntMid = gTaskL3bfhsContext.curAge.wsUnderTotalCntMid*(float)HCU_L3BFHS_STA_AGEING_COEF + (float)HCU_L3BFHS_STA_AGEING_COEF_ALPHA*gTaskL3bfhsContext.cur.wsUnderTotalCnt;
	gTaskL3bfhsContext.curAge.wsUnderTotalWgtMid = gTaskL3bfhsContext.curAge.wsUnderTotalWgtMid*(float)HCU_L3BFHS_STA_AGEING_COEF + (float)HCU_L3BFHS_STA_AGEING_COEF_ALPHA*gTaskL3bfhsContext.cur.wsUnderTotalWgt;
	gTaskL3bfhsContext.curAge.wsUnderTu1CntMid = gTaskL3bfhsContext.curAge.wsUnderTu1CntMid*(float)HCU_L3BFHS_STA_AGEING_COEF + (float)HCU_L3BFHS_STA_AGEING_COEF_ALPHA*gTaskL3bfhsContext.cur.wsUnderTu1Cnt;
	gTaskL3bfhsContext.curAge.wsUnderTu1WgtMid = gTaskL3bfhsContext.curAge.wsUnderTu1WgtMid*(float)HCU_L3BFHS_STA_AGEING_COEF + (float)HCU_L3BFHS_STA_AGEING_COEF_ALPHA*gTaskL3bfhsContext.cur.wsUnderTu1Wgt;
	gTaskL3bfhsContext.curAge.wsUnderTu2CntMid = gTaskL3bfhsContext.curAge.wsUnderTu2CntMid*(float)HCU_L3BFHS_STA_AGEING_COEF + (float)HCU_L3BFHS_STA_AGEING_COEF_ALPHA*gTaskL3bfhsContext.cur.wsUnderTu2Cnt;
	gTaskL3bfhsContext.curAge.wsUnderTu2WgtMid = gTaskL3bfhsContext.curAge.wsUnderTu2WgtMid*(float)HCU_L3BFHS_STA_AGEING_COEF + (float)HCU_L3BFHS_STA_AGEING_COEF_ALPHA*gTaskL3bfhsContext.cur.wsUnderTu2Wgt;
	gTaskL3bfhsContext.curAge.wsUnspecificCntMid = gTaskL3bfhsContext.curAge.wsUnspecificCntMid*(float)HCU_L3BFHS_STA_AGEING_COEF + (float)HCU_L3BFHS_STA_AGEING_COEF_ALPHA*gTaskL3bfhsContext.cur.wsUnspecificCnt;
	gTaskL3bfhsContext.curAge.wsUnspecificWgtMid = gTaskL3bfhsContext.curAge.wsUnspecificWgtMid*(float)HCU_L3BFHS_STA_AGEING_COEF + (float)HCU_L3BFHS_STA_AGEING_COEF_ALPHA*gTaskL3bfhsContext.cur.wsUnspecificWgt;
	gTaskL3bfhsContext.curAge.wsAvgMatTimesMid = gTaskL3bfhsContext.curAge.wsAvgMatTimesMid*(float)HCU_L3BFHS_STA_AGEING_COEF + (float)HCU_L3BFHS_STA_AGEING_COEF_ALPHA*gTaskL3bfhsContext.cur.wsAvgMatTimes;
	gTaskL3bfhsContext.curAge.wsAvgTttTimesMid = gTaskL3bfhsContext.curAge.wsAvgTttTimesMid*(float)HCU_L3BFHS_STA_AGEING_COEF + (float)HCU_L3BFHS_STA_AGEING_COEF_ALPHA*gTaskL3bfhsContext.cur.wsAvgTttTimes;
	gTaskL3bfhsContext.curAge.wsAvgTgvTimesMid = gTaskL3bfhsContext.curAge.wsAvgTgvTimesMid*(float)HCU_L3BFHS_STA_AGEING_COEF + (float)HCU_L3BFHS_STA_AGEING_COEF_ALPHA*gTaskL3bfhsContext.cur.wsAvgTgvTimes;

	//再输出结果
	gTaskL3bfhsContext.staLocalUi.wsIncMatCnt = (UINT32)(gTaskL3bfhsContext.curAge.wsIncMatCntMid + 0.5);
	gTaskL3bfhsContext.staLocalUi.wsIncMatWgt = (UINT32)(gTaskL3bfhsContext.curAge.wsIncMatWgtMid + 0.5);
	gTaskL3bfhsContext.staLocalUi.wsNormalCnt = (UINT32)(gTaskL3bfhsContext.curAge.wsNormalCntMid + 0.5);
	gTaskL3bfhsContext.staLocalUi.wsNormalWgt = (UINT32)(gTaskL3bfhsContext.curAge.wsNormalWgtMid + 0.5);
	gTaskL3bfhsContext.staLocalUi.wsOverCnt = (UINT32)(gTaskL3bfhsContext.curAge.wsOverCntMid + 0.5);
	gTaskL3bfhsContext.staLocalUi.wsOverWgt = (UINT32)(gTaskL3bfhsContext.curAge.wsOverWgtMid + 0.5);
	gTaskL3bfhsContext.staLocalUi.wsUnderTotalCnt = (UINT32)(gTaskL3bfhsContext.curAge.wsUnderTotalCntMid + 0.5);
	gTaskL3bfhsContext.staLocalUi.wsUnderTotalWgt = (UINT32)(gTaskL3bfhsContext.curAge.wsUnderTotalWgtMid + 0.5);
	gTaskL3bfhsContext.staLocalUi.wsUnderTu1Cnt = (UINT32)(gTaskL3bfhsContext.curAge.wsUnderTu1CntMid + 0.5);
	gTaskL3bfhsContext.staLocalUi.wsUnderTu1Wgt = (UINT32)(gTaskL3bfhsContext.curAge.wsUnderTu1WgtMid + 0.5);
	gTaskL3bfhsContext.staLocalUi.wsUnderTu2Cnt = (UINT32)(gTaskL3bfhsContext.curAge.wsUnderTu2CntMid + 0.5);
	gTaskL3bfhsContext.staLocalUi.wsUnderTu2Wgt = (UINT32)(gTaskL3bfhsContext.curAge.wsUnderTu2WgtMid + 0.5);
	gTaskL3bfhsContext.staLocalUi.wsUnspecificCnt = (UINT32)(gTaskL3bfhsContext.curAge.wsUnspecificCntMid + 0.5);
	gTaskL3bfhsContext.staLocalUi.wsUnspecificWgt = (UINT32)(gTaskL3bfhsContext.curAge.wsUnspecificWgtMid + 0.5);
	gTaskL3bfhsContext.staLocalUi.wsAvgMatTimes = (UINT32)(gTaskL3bfhsContext.curAge.wsAvgMatTimesMid + 0.5);
	gTaskL3bfhsContext.staLocalUi.wsAvgTttTimes = (UINT32)(gTaskL3bfhsContext.curAge.wsAvgTttTimesMid + 0.5);
	gTaskL3bfhsContext.staLocalUi.wsAvgTgvTimes = (UINT32)(gTaskL3bfhsContext.curAge.wsAvgTgvTimesMid + 0.5);

	//5s的LocalUI数据进行更新
	if ((gTaskL3bfhsContext.elipseCnt % HCU_L3BFHS_STA_5S_CYCLE) == 0){
		StrHlcIe_cui_hcu2uir_statistic_bfhs_report_t buf;
		memset(&buf, 0, sizeof(StrHlcIe_cui_hcu2uir_statistic_bfhs_report_t));
		buf.targetWeight = gTaskL3bfhsContext.wgtSnrPar.minAllowedWeight;
		buf.tu1LimitWeight = gTaskL3bfhsContext.wgtSnrPar.snrAlgoTu1Limit;
		buf.tu2LimitWeight = gTaskL3bfhsContext.wgtSnrPar.snrAlgoTu2Limit;
		buf.upLimitWeight = gTaskL3bfhsContext.wgtSnrPar.maxAllowedWeight;
		buf.totalPackage = gTaskL3bfhsContext.staUp2Now.wsIncMatCnt;
		buf.totalGoodPackage = gTaskL3bfhsContext.staUp2Now.wsNormalCnt;
		buf.totalOverReject = gTaskL3bfhsContext.staUp2Now.wsOverCnt;
		buf.totalUnderReject = gTaskL3bfhsContext.staUp2Now.wsUnderTotalCnt;
		buf.totalWeight = gTaskL3bfhsContext.staUp2Now.wsIncMatWgt;
		buf.throughputPerMin = gTaskL3bfhsContext.staLocalUi.wsAvgTttTimes;
		hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_statistic_bfhs_report(gTaskL3bfhsContext.configId, &buf);
	}

	//更新60Min各个统计表单
	gTaskL3bfhsContext.sta60Min.wsIncMatCnt += gTaskL3bfhsContext.cur.wsIncMatCnt;
	gTaskL3bfhsContext.sta60Min.wsIncMatWgt += gTaskL3bfhsContext.cur.wsIncMatWgt;
	gTaskL3bfhsContext.sta60Min.wsNormalCnt += gTaskL3bfhsContext.cur.wsNormalCnt;
	gTaskL3bfhsContext.sta60Min.wsNormalWgt += gTaskL3bfhsContext.cur.wsNormalWgt;
	gTaskL3bfhsContext.sta60Min.wsOverCnt += gTaskL3bfhsContext.cur.wsOverCnt;
	gTaskL3bfhsContext.sta60Min.wsOverWgt += gTaskL3bfhsContext.cur.wsOverWgt;
	gTaskL3bfhsContext.sta60Min.wsUnderTotalCnt += gTaskL3bfhsContext.cur.wsUnderTotalCnt;
	gTaskL3bfhsContext.sta60Min.wsUnderTotalWgt += gTaskL3bfhsContext.cur.wsUnderTotalWgt;
	gTaskL3bfhsContext.sta60Min.wsUnderTu1Cnt += gTaskL3bfhsContext.cur.wsUnderTu1Cnt;
	gTaskL3bfhsContext.sta60Min.wsUnderTu1Wgt += gTaskL3bfhsContext.cur.wsUnderTu1Wgt;
	gTaskL3bfhsContext.sta60Min.wsUnderTu2Cnt += gTaskL3bfhsContext.cur.wsUnderTu2Cnt;
	gTaskL3bfhsContext.sta60Min.wsUnderTu2Wgt += gTaskL3bfhsContext.cur.wsUnderTu2Wgt;
	gTaskL3bfhsContext.sta60Min.wsUnspecificCnt += gTaskL3bfhsContext.cur.wsUnspecificCnt;
	gTaskL3bfhsContext.sta60Min.wsUnspecificWgt += gTaskL3bfhsContext.cur.wsUnspecificWgt;
	float timeRun60MinRatio = (float) HCU_L3BFHS_STA_60M_CYCLE / (float)(((gTaskL3bfhsContext.elipseCnt%HCU_L3BFHS_STA_60M_CYCLE)==0)?HCU_L3BFHS_STA_60M_CYCLE:(gTaskL3bfhsContext.elipseCnt%HCU_L3BFHS_STA_60M_CYCLE));
	gTaskL3bfhsContext.sta60Min.wsAvgMatTimes = (UINT32)(gTaskL3bfhsContext.sta60Min.wsIncMatCnt*timeRun60MinRatio);
	gTaskL3bfhsContext.sta60Min.wsAvgTttTimes = (UINT32)(gTaskL3bfhsContext.sta60Min.wsNormalCnt*timeRun60MinRatio);
	gTaskL3bfhsContext.sta60Min.wsAvgTgvTimes = (UINT32)(gTaskL3bfhsContext.sta60Min.wsOverCnt*timeRun60MinRatio) + (UINT32)(gTaskL3bfhsContext.sta60Min.wsUnderTotalCnt*timeRun60MinRatio);

	//60分钟到了，发送统计报告到后台：发送后台只需要一种统计报告即可，重复发送意义不大
	if ((gTaskL3bfhsContext.elipseCnt % HCU_L3BFHS_STA_60M_CYCLE) == 0){
		//60分统计表更新数据库
		if (dbi_HcuBfhs_StaDatainfo_save(HCU_L3BFHS_STA_DBI_TABLE_60MIN, gTaskL3bfhsContext.configId, &(gTaskL3bfhsContext.sta60Min)) == FAILURE)
				HCU_ERROR_PRINT_L3BFHS("L3BFHS: Save data to DB error!\n");

		//再发送统计报告
		msg_struct_l3bfhs_cloudvela_statistic_report_t snd;
		memset(&snd, 0, sizeof(msg_struct_l3bfhs_cloudvela_statistic_report_t));

		//L2信息
		strncpy(snd.comHead.destUser, zHcuSysEngPar.cloud.svrNameDefault, strlen(zHcuSysEngPar.cloud.svrNameDefault)<sizeof(snd.comHead.destUser)?strlen(zHcuSysEngPar.cloud.svrNameDefault):sizeof(snd.comHead.destUser));
		strncpy(snd.comHead.srcUser, zHcuSysEngPar.hwBurnId.equLable, strlen(zHcuSysEngPar.hwBurnId.equLable)<sizeof(snd.comHead.srcUser)?strlen(zHcuSysEngPar.hwBurnId.equLable):sizeof(snd.comHead.srcUser));
		snd.comHead.timeStamp = time(0);
		snd.comHead.msgType = HUITP_MSG_HUIXML_MSGTYPE_DEVICE_REPORT_ID;
		strcpy(snd.comHead.funcFlag, "0");

		//CONTENT
		snd.dataFormat = HUITP_IEID_UNI_COM_FORMAT_TYPE_FLOAT_WITH_NF2;
		snd.baseReport = HUITP_IEID_UNI_COM_REPORT_YES;
		snd.staRepType = HUITP_IEID_UNI_SCALE_WEIGHT_STATISTIC_REPORT_60M;
		snd.sta.wsIncMatCnt = gTaskL3bfhsContext.sta60Min.wsIncMatCnt;
		snd.sta.wsIncMatWgt = gTaskL3bfhsContext.sta60Min.wsIncMatWgt;
		snd.sta.wsNormalCnt = gTaskL3bfhsContext.sta60Min.wsNormalCnt;
		snd.sta.wsNormalWgt = gTaskL3bfhsContext.sta60Min.wsNormalWgt;
		snd.sta.wsOverCnt = gTaskL3bfhsContext.sta60Min.wsOverCnt;
		snd.sta.wsOverWgt = gTaskL3bfhsContext.sta60Min.wsOverWgt;
		snd.sta.wsUnderTotalCnt = gTaskL3bfhsContext.sta60Min.wsUnderTotalCnt;
		snd.sta.wsUnderTotalWgt = gTaskL3bfhsContext.sta60Min.wsUnderTotalWgt;
		snd.sta.wsUnderTu1Cnt = gTaskL3bfhsContext.sta60Min.wsUnderTu1Cnt;
		snd.sta.wsUnderTu1Wgt = gTaskL3bfhsContext.sta60Min.wsUnderTu1Wgt;
		snd.sta.wsUnderTu2Cnt = gTaskL3bfhsContext.sta60Min.wsUnderTu2Cnt;
		snd.sta.wsUnderTu2Wgt = gTaskL3bfhsContext.sta60Min.wsUnderTu2Wgt;
		snd.sta.wsUnspecificCnt = gTaskL3bfhsContext.sta60Min.wsUnspecificCnt;
		snd.sta.wsUnspecificWgt = gTaskL3bfhsContext.sta60Min.wsUnspecificWgt;
		snd.sta.wsAvgMatTimes = gTaskL3bfhsContext.sta60Min.wsAvgMatTimes;
		snd.sta.wsAvgTttTimes = gTaskL3bfhsContext.sta60Min.wsAvgTttTimes;
		snd.sta.wsAvgTgvTimes = gTaskL3bfhsContext.sta60Min.wsAvgTgvTimes;

		snd.length = sizeof(msg_struct_l3bfhs_cloudvela_statistic_report_t);
		HCU_MSG_SEND_GENERNAL_PROCESS(MSG_ID_L3BFHS_CLOUDVELA_STATISTIC_REPORT, TASK_ID_CLOUDVELA, TASK_ID_L3BFHS);
		if (hcu_message_send(MSG_ID_L3BFHS_CLOUDVELA_STATISTIC_REPORT, TASK_ID_CLOUDVELA, TASK_ID_L3BFHS, &snd, snd.length) == FAILURE)
			HCU_ERROR_PRINT_L3BFHS("L3BFHS: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3BFHS].taskName, zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName);

		//然后将60分钟统计数据表单清零，以便再次计数
		memset(&(gTaskL3bfhsContext.sta60Min), 0, sizeof(HcuSysMsgIeL3bfhsContextStaElement_t));
	}

	//更新24小时统计表单
	gTaskL3bfhsContext.sta24H.wsIncMatCnt += gTaskL3bfhsContext.cur.wsIncMatCnt;
	gTaskL3bfhsContext.sta24H.wsIncMatWgt += gTaskL3bfhsContext.cur.wsIncMatWgt;
	gTaskL3bfhsContext.sta24H.wsNormalCnt += gTaskL3bfhsContext.cur.wsNormalCnt;
	gTaskL3bfhsContext.sta24H.wsNormalWgt += gTaskL3bfhsContext.cur.wsNormalWgt;
	gTaskL3bfhsContext.sta24H.wsOverCnt += gTaskL3bfhsContext.cur.wsOverCnt;
	gTaskL3bfhsContext.sta24H.wsOverWgt += gTaskL3bfhsContext.cur.wsOverWgt;
	gTaskL3bfhsContext.sta24H.wsUnderTotalCnt += gTaskL3bfhsContext.cur.wsUnderTotalCnt;
	gTaskL3bfhsContext.sta24H.wsUnderTotalWgt += gTaskL3bfhsContext.cur.wsUnderTotalWgt;
	gTaskL3bfhsContext.sta24H.wsUnderTu1Cnt += gTaskL3bfhsContext.cur.wsUnderTu1Cnt;
	gTaskL3bfhsContext.sta24H.wsUnderTu1Wgt += gTaskL3bfhsContext.cur.wsUnderTu1Wgt;
	gTaskL3bfhsContext.sta24H.wsUnderTu2Cnt += gTaskL3bfhsContext.cur.wsUnderTu2Cnt;
	gTaskL3bfhsContext.sta24H.wsUnderTu2Wgt += gTaskL3bfhsContext.cur.wsUnderTu2Wgt;
	gTaskL3bfhsContext.sta24H.wsUnspecificCnt += gTaskL3bfhsContext.cur.wsUnspecificCnt;
	gTaskL3bfhsContext.sta24H.wsUnspecificWgt += gTaskL3bfhsContext.cur.wsUnspecificWgt;
	//float timeRun24HourRatio = (float) HCU_L3BFHS_STA_24H_CYCLE / (float)(((gTaskL3bfhsContext.elipse24HourCnt%HCU_L3BFHS_STA_24H_CYCLE)==0)?HCU_L3BFHS_STA_24H_CYCLE:(gTaskL3bfhsContext.elipseCnt%HCU_L3BFHS_STA_24H_CYCLE));
	gTaskL3bfhsContext.sta24H.wsAvgMatTimes = (UINT32)(gTaskL3bfhsContext.sta24H.wsIncMatCnt*timeRun60MinRatio);
	gTaskL3bfhsContext.sta24H.wsAvgTttTimes = (UINT32)(gTaskL3bfhsContext.sta24H.wsNormalCnt*timeRun60MinRatio);
	gTaskL3bfhsContext.sta24H.wsAvgTgvTimes = (UINT32)(gTaskL3bfhsContext.sta24H.wsOverCnt*timeRun60MinRatio) + (UINT32)(gTaskL3bfhsContext.sta60Min.wsUnderTotalCnt*timeRun60MinRatio);

	//24小时统计表更新数据库
	if ((gTaskL3bfhsContext.elipseCnt % HCU_L3BFHS_STA_1M_CYCLE) == 0){
		if (dbi_HcuBfhs_StaDatainfo_save(HCU_L3BFHS_STA_DBI_TABLE_24HOUR, gTaskL3bfhsContext.configId, &(gTaskL3bfhsContext.sta24H)) == FAILURE)
				HCU_ERROR_PRINT_L3BFHS("L3BFHS: Save data to DB error!\n");
	}

	//24小时到了，然后将24小时统计数据表单清零，以便再次计数
	if (((time(0)-gTaskL3bfhsContext.start24hStaTimeInUnix) % HCU_L3BFHS_STA_24H_IN_SECOND) == 0){
		memset(&(gTaskL3bfhsContext.sta24H), 0, sizeof(HcuSysMsgIeL3bfhsContextStaElement_t));
		gTaskL3bfhsContext.elipse24HourCnt = 0;
	}

	//增加数据到连续统计数据中
	gTaskL3bfhsContext.staUp2Now.wsIncMatCnt += gTaskL3bfhsContext.cur.wsIncMatCnt;
	gTaskL3bfhsContext.staUp2Now.wsIncMatWgt += gTaskL3bfhsContext.cur.wsIncMatWgt;
	gTaskL3bfhsContext.staUp2Now.wsNormalCnt += gTaskL3bfhsContext.cur.wsNormalCnt;
	gTaskL3bfhsContext.staUp2Now.wsNormalWgt += gTaskL3bfhsContext.cur.wsNormalWgt;
	gTaskL3bfhsContext.staUp2Now.wsOverCnt += gTaskL3bfhsContext.cur.wsOverCnt;
	gTaskL3bfhsContext.staUp2Now.wsOverWgt += gTaskL3bfhsContext.cur.wsOverWgt;
	gTaskL3bfhsContext.staUp2Now.wsUnderTotalCnt += gTaskL3bfhsContext.cur.wsUnderTotalCnt;
	gTaskL3bfhsContext.staUp2Now.wsUnderTotalWgt += gTaskL3bfhsContext.cur.wsUnderTotalWgt;
	gTaskL3bfhsContext.staUp2Now.wsUnderTu1Cnt += gTaskL3bfhsContext.cur.wsUnderTu1Cnt;
	gTaskL3bfhsContext.staUp2Now.wsUnderTu1Wgt += gTaskL3bfhsContext.cur.wsUnderTu1Wgt;
	gTaskL3bfhsContext.staUp2Now.wsUnderTu2Cnt += gTaskL3bfhsContext.cur.wsUnderTu2Cnt;
	gTaskL3bfhsContext.staUp2Now.wsUnderTu2Wgt += gTaskL3bfhsContext.cur.wsUnderTu2Wgt;
	gTaskL3bfhsContext.staUp2Now.wsUnspecificCnt += gTaskL3bfhsContext.cur.wsUnspecificCnt;
	gTaskL3bfhsContext.staUp2Now.wsUnspecificWgt += gTaskL3bfhsContext.cur.wsUnspecificWgt;
	gTaskL3bfhsContext.staUp2Now.wsAvgMatTimes = gTaskL3bfhsContext.staUp2Now.wsIncMatCnt;
	gTaskL3bfhsContext.staUp2Now.wsAvgTttTimes = gTaskL3bfhsContext.staUp2Now.wsNormalCnt;
	gTaskL3bfhsContext.staUp2Now.wsAvgTgvTimes = gTaskL3bfhsContext.staUp2Now.wsOverCnt + gTaskL3bfhsContext.staUp2Now.wsUnderTotalCnt;

	//重要的统计功能挂载
	if ((gTaskL3bfhsContext.elipseCnt%HCU_L3BFHS_STATISTIC_PRINT_FREQUENCY) == 0)
	HCU_DEBUG_PRINT_CRT("L3BFHS: Control statistics, Total Up2Now Inc Cnt = [%d], Success Rate = [%5.2f\%], Give-up Rate = [%5.2f\%], Local UI Shows AvgSpeed of [TTT Times/MatCnt/MatWgt] = %d.\n",\
			gTaskL3bfhsContext.staUp2Now.wsIncMatCnt, \
			(float)(gTaskL3bfhsContext.staUp2Now.wsNormalCnt)/(float)((gTaskL3bfhsContext.staUp2Now.wsIncMatCnt==0)?0.01:gTaskL3bfhsContext.staUp2Now.wsIncMatCnt) * 100, \
			(float)(gTaskL3bfhsContext.staUp2Now.wsOverCnt + gTaskL3bfhsContext.staUp2Now.wsUnderTotalCnt)/(float)((gTaskL3bfhsContext.staUp2Now.wsIncMatCnt==0)?0.01:gTaskL3bfhsContext.staUp2Now.wsIncMatCnt) * 100, \
			gTaskL3bfhsContext.staLocalUi.wsAvgTttTimes);
	//将当前基础统计周期的数据清零
	memset(&(gTaskL3bfhsContext.cur), 0, sizeof(HcuSysMsgIeL3bfhsContextStaElement_t));

	//返回
	return SUCCESS;
}

/***************************************************************************************************************************
 *
 * 　高级简化技巧
 *
 ***************************************************************************************************************************/
//由于错误，直接从差错中转入扫描状态
//它提供了一种比RESTART更快更低级的方式，让L3状态机直接返回到扫描状态
void func_l3bfhs_stm_main_recovery_from_fault(void)
{
	//控制其它模块进入正常状态

	//状态转移到SLEEP状态
	FsmSetState(TASK_ID_L3BFHS, FSM_STATE_L3BFHS_OOS_SCAN);

	//初始化模块的任务资源
	//初始化定时器：暂时决定不做，除非该模块重新RESTART
	//初始化模块级全局变量：暂时决定不做，除非该模块重新RESTART

	return;
}




