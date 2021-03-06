/*
 * l3bfdf.c
 *
 *  Created on: 2015年11月22日
 *      Author: test
 */
#include "l3bfdf.h"

/*
** FSM of the L3BFDF
*/
HcuFsmStateItem_t HcuFsmL3bfdf[] =
{
    //MessageId                 				//State                   		 	//Function
	//启始点，固定定义，不要改动, 使用ENTRY/END，意味者MSGID肯定不可能在某个高位区段中；考虑到所有任务共享MsgId，即使分段，也无法实现
	//完全是为了给任务一个初始化的机会，按照状态转移机制，该函数不具备启动的机会，因为任务初始化后自动到FSM_STATE_IDLE
	//如果没有必要进行初始化，可以设置为NULL
	{MSG_ID_ENTRY,       						FSM_STATE_ENTRY,            		fsm_l3bfdf_task_entry}, //Starting

	//System level initialization, only controlled by HCU-MAIN
    {MSG_ID_COM_INIT,       					FSM_STATE_IDLE,            			fsm_l3bfdf_init},
    {MSG_ID_COM_INIT_FEEDBACK,					FSM_STATE_IDLE,            			fsm_com_do_nothing},

	//Task level initialization
    {MSG_ID_COM_INIT,       					FSM_STATE_L3BFDF_INITED,            fsm_l3bfdf_init},
    {MSG_ID_COM_INIT_FEEDBACK,					FSM_STATE_L3BFDF_INITED,            fsm_com_do_nothing},

	//ANY state entry
    {MSG_ID_COM_INIT_FEEDBACK,					FSM_STATE_COMMON,          			fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT,       				FSM_STATE_COMMON,          			fsm_com_heart_beat_rcv},
	{MSG_ID_COM_STOP,       					FSM_STATE_COMMON,          			fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT_FB,       			FSM_STATE_COMMON,          			fsm_com_do_nothing},
    {MSG_ID_COM_RESTART,						FSM_STATE_COMMON,            		fsm_l3bfdf_restart},
	{MSG_ID_COM_TIME_OUT,       				FSM_STATE_COMMON,          			fsm_l3bfdf_time_out},
	{MSG_ID_CLOUDVELA_L3BFDF_DATA_REQ,          FSM_STATE_COMMON,                   fsm_l3bfdf_cloudvela_data_req},
	{MSG_ID_CLOUDVELA_L3BFDF_DATA_CONFIRM,      FSM_STATE_COMMON,                   fsm_l3bfdf_cloudvela_data_confirm},
	{MSG_ID_CLOUDVELA_L3BFDF_EVENT_CONFIRM,     FSM_STATE_COMMON,                   fsm_l3bfdf_cloudvela_event_confirm},
	{MSG_ID_CLOUDVELA_L3BFDF_CTRL_REQ,          FSM_STATE_COMMON,                   fsm_l3bfdf_cloudvela_ctrl_req},
	{MSG_ID_CLOUDVELA_L3BFDF_STATISTIC_CONFIRM, FSM_STATE_COMMON,                   fsm_l3bfdf_cloudvela_statistic_confirm},

	//任意状态下的命令执行
	{MSG_ID_UICOMM_L3BFDF_CTRL_CMD_REQ,       	FSM_STATE_COMMON,          			fsm_l3bfdf_uicomm_ctrl_cmd_req},
	{MSG_ID_SUI_STARTUP_IND,       				FSM_STATE_COMMON,          			fsm_l3bfdf_canitf_startup_ind},
	{MSG_ID_SUI_FAULT_IND,       				FSM_STATE_COMMON,          			fsm_l3bfdf_canitf_fault_ind},
	{MSG_ID_SUI_HEART_BEAT_REPORT,       		FSM_STATE_COMMON,          			fsm_l3bfdf_canitf_heart_beat_report},
	{MSG_ID_CAN_L3BFDF_WS_COMB_OUT_FB,       	FSM_STATE_COMMON,      			    fsm_l3bfdf_canitf_ws_comb_out_fb},
	{MSG_ID_SUI_STOP_RESP,       				FSM_STATE_COMMON,          			fsm_l3bfdf_canitf_sys_stop_resp},

	//只为出现ACTIVED状态，入口自动被COMMON屏蔽
	{MSG_ID_CAN_L3BFDF_SYS_CFG_RESP,       		FSM_STATE_L3BFDF_ACTIVED,          	fsm_l3bfdf_canitf_sys_config_resp},
	{MSG_ID_CAN_L3BFDF_DYN_CAL_RESP,       		FSM_STATE_L3BFDF_ACTIVED,          	fsm_l3bfdf_canitf_dyn_cal_resp},

	//进料组合态
	{MSG_ID_CAN_L3BFDF_WS_NEW_READY_EVENT,      FSM_STATE_L3BFDF_OOS_SCAN,          fsm_l3bfdf_canitf_ws_new_ready_event},
	{MSG_ID_CAN_L3BFDF_BASKET_CLEAN_IND,       	FSM_STATE_L3BFDF_OOS_SCAN,          fsm_l3bfdf_canitf_basket_clean_ind},
	{MSG_ID_SUI_SUSPEND_RESP,       			FSM_STATE_L3BFDF_OOS_SCAN,          fsm_l3bfdf_canitf_sys_suspend_resp},//这个是先转移状态，再发送命令

	//休眠状态：只允许收到RESUME指令，不更新界面广告牌以及数据库
	{MSG_ID_SUI_RESUME_RESP,       				FSM_STATE_L3BFDF_SUSPEND,          	fsm_l3bfdf_canitf_sys_resume_resp},//这个是先发送命令，收齐后再转移状态

    //结束点，固定定义，不要改动
    {MSG_ID_END,            					FSM_STATE_END,             			NULL},  //Ending
};

//System Level Global variables

//Task Level Global variables
gTaskL3bfdfContext_t gTaskL3bfdfContext; //总控表


//Main Entry
/***************************************************************************************************************************
 *
 * 　FSM初始化部分
 *
 ***************************************************************************************************************************/
//Input parameter would be useless, but just for similar structure purpose
OPSTAT fsm_l3bfdf_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//除了对全局变量进行操作之外，尽量不要做其它操作，因为该函数将被主任务/线程调用，不是本任务/线程调用
	//该API就是给本任务一个提早介入的入口，可以帮着做些测试性操作
	if (FsmSetState(TASK_ID_L3BFDF, FSM_STATE_IDLE) == FAILURE){
		HcuErrorPrint("L3BFDF: Error Set FSM State at fsm_l3bfdf_task_entry\n");}
	return SUCCESS;
}

OPSTAT fsm_l3bfdf_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;
	int i=0, j=0, boardId=0;

	if ((src_id > TASK_ID_MIN) &&(src_id < TASK_ID_MAX)){
		//Send back MSG_ID_COM_INIT_FEEDBACK to SVRCON
		msg_struct_com_init_feedback_t snd0;
		memset(&snd0, 0, sizeof(msg_struct_com_init_feedback_t));
		snd0.length = sizeof(msg_struct_com_init_feedback_t);

		//to avoid all task send out the init fb msg at the same time which lead to msgque get stuck
		hcu_usleep(dest_id*HCU_SYSCFG_DURATION_OF_INIT_FB_WAIT_MAX);

		ret = hcu_message_send(MSG_ID_COM_INIT_FEEDBACK, src_id, TASK_ID_L3BFDF, &snd0, snd0.length);
		if (ret == FAILURE){
			HcuErrorPrint("L3BFDF: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3BFDF].taskName, zHcuVmCtrTab.task[src_id].taskName);
			return FAILURE;
		}
	}

	//收到初始化消息后，进入初始化状态
	if (FsmSetState(TASK_ID_L3BFDF, FSM_STATE_L3BFDF_INITED) == FAILURE){
		HcuErrorPrint("L3BFDF: Error Set FSM State!\n");
		return FAILURE;
	}

	//初始化硬件接口
	if (func_l3bfdf_int_init() == FAILURE){
		HcuErrorPrint("L3BFDF: Error initialize interface!\n");
		return FAILURE;
	}

	//Global Variables
	zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3BFDF] = 0;

	/*
	 * 严格检查内部消息设置的大小，是否越界：
	 *
	 */

#if ((HCU_SYSMSG_BFDF_SET_CFG_HOPPER_MAX < HCU_SYSCFG_BFDF_HOPPER_NBR_MAX) ||\
		(HCU_SYSMSG_BFDF_SET_CFG_HOPPER_MAX != HUITP_IEID_SUI_BFDF_MAX_GLOBAL_AP_NUM+1) ||\
		(HCU_SYSMSG_BFDF_SET_CFG_HOP_IN_BOARD_MAX != HCU_SYSCFG_BFDF_HOPPER_IN_ONE_BOARD)||\
		(HCU_SYSMSG_BFDF_SET_CFG_HOP_IN_BOARD_MAX != HUITP_IEID_SUI_BFDF_MAX_LOCAL_AP_NUM))
	#error BFDF KEY PARMETER SET ERROR!
#endif

	//严格防止HUITP消息跟内部消息在关键结构上定义的不一致
	if ((sizeof(gTaskL3bfdfContextWeightSensorParamaters_t) != sizeof(strMsgIe_l3bfdf_WeightSensorParamaters_t))||\
			(sizeof(gTaskL3bfdfContextMotorControlParamaters_t) != sizeof(strMsgIe_l3bfdf_MotorControlParamaters_t))||\
			(sizeof(gTaskL3bfdfContextActionControlParamaters_t) > sizeof(strMsgIe_l3bfdf_ActionControlParamaters_t)))
		HCU_ERROR_PRINT_L3BFDF("L3BFDF: module message definition on statistic element error! GlobalPar/MsgStr WP=%d/%d, CP=%d/%d, AP=%d/%d\n",\
				sizeof(gTaskL3bfdfContextWeightSensorParamaters_t), sizeof(strMsgIe_l3bfdf_WeightSensorParamaters_t), \
				sizeof(gTaskL3bfdfContextMotorControlParamaters_t), sizeof(strMsgIe_l3bfdf_MotorControlParamaters_t), \
				sizeof(gTaskL3bfdfContextActionControlParamaters_t), sizeof(strMsgIe_l3bfdf_ActionControlParamaters_t));
	//严格保证统计周期的一致性
	if (HCU_L3BFDF_STA_UNIT_DUR != (10*zHcuSysEngPar.timer.array[TIMER_ID_10MS_L3BFDF_PERIOD_STA_SCAN].dur))  //静态表是以10ms为单位的
		HCU_ERROR_PRINT_L3BFDF("L3BFDF: module timer statistic parameter set error!\n");

	//秤盘数据表单控制表初始化：留给BFSDUICOMM初始化，不然LOAD表的时候会出问题
//	memset(&gTaskL3bfdfContext, 0, sizeof(gTaskL3bfdfContext_t));
	//第0号板子一定是垃圾桶：跟后面的逻辑是否相配？
	for (i=0; i<HCU_SYSCFG_BFDF_EQU_FLOW_NBR_MAX; i++){
		gTaskL3bfdfContext.nodeDyn[i][0].nodeStatus = HCU_L3BFDF_NODE_BOARD_STATUS_VALID;
	}

	//初始化界面交互数据
	for (i=0; i<HCU_SYSCFG_BFDF_EQU_FLOW_NBR_MAX; i++){
		for (j=1; j < (HCU_SYSCFG_BFDF_NODE_BOARD_NBR_MAX); j++){
			boardId = (i<<3) + j;
			HCU_L3BFDF_TRIGGER_UI_STATUS_REPORT(boardId, HUICOBUS_CMDID_CUI_HCU2UIR_GENERAL_CMDVAL_NULL);
		}
	}
	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_L3BFDF, FSM_STATE_L3BFDF_ACTIVED) == FAILURE)
		HCU_ERROR_PRINT_L3BFDF("L3BFDF: Error Set FSM State!\n");
	HCU_DEBUG_PRINT_FAT("L3BFDF: Enter FSM_STATE_L3BFDF_ACTIVED status, Keeping refresh here!\n");

	return SUCCESS;
}

OPSTAT fsm_l3bfdf_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HcuErrorPrint("L3BFDF: Internal error counter reach DEAD level, SW-RESTART soon!\n");
	zHcuSysStaPm.statisCnt.restartCnt++;
	fsm_l3bfdf_init(0, 0, NULL, 0);
	return SUCCESS;
}

OPSTAT func_l3bfdf_int_init(void)
{
	if (hcu_sps232_qr_printer_init() == FAILURE)
		HcuDebugPrint("L3BFDF: Init QR Printer not success, but not impact whole system working!\n");

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
	sprintf(s, "BOFENGZHINENG-%s-%5.2f-%04d.%02d.%02d.%02d:%02d:%02d", gTaskL3bfdfContext.configName, (float)gTaskL3bfdfContext.wgtSnrPar.MaxAllowedWeight, \
			(UINT16)(1900+cu->tm_year), (UINT8)cu->tm_mon, (UINT8)cu->tm_mday, (UINT8)cu->tm_hour, (UINT8)cu->tm_min, (UINT8)cu->tm_sec);
	hcu_sps232_send_char_to_ext_printer(s, strlen(s));

	return SUCCESS;
}

//暂时啥都不干，定时到达后，还不明确是否需要支持定时工作
OPSTAT fsm_l3bfdf_time_out(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret;

	HCU_MSG_RCV_CHECK_FOR_GEN_LOCAL(TASK_ID_L3BFDF, msg_struct_com_time_out_t);

	//钩子在此处，检查zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3BFDF]是否超限
	if (zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3BFDF] > HCU_RUN_ERROR_LEVEL_2_MAJOR){
		//减少重复RESTART的概率
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3BFDF] = zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3BFDF] - HCU_RUN_ERROR_LEVEL_2_MAJOR;
		msg_struct_com_restart_t snd0;
		memset(&snd0, 0, sizeof(msg_struct_com_restart_t));
		snd0.length = sizeof(msg_struct_com_restart_t);
		ret = hcu_message_send(MSG_ID_COM_RESTART, TASK_ID_L3BFDF, TASK_ID_L3BFDF, &snd0, snd0.length);
		if (ret == FAILURE){
			HCU_ERROR_PRINT_L3BFDF("L3BFDF: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3BFDF].taskName, zHcuVmCtrTab.task[TASK_ID_L3BFDF].taskName);
		}
	}

	//配置初始化：一次性工作
	else if ((rcv.timeId == TIMER_ID_1S_L3BFDF_CFG_START_WAIT_FB) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		if (func_l3bfdf_time_out_sys_cfg_req_process() == FAILURE){
			HCU_ERROR_PRINT_L3BFDF("L3BFDF: Error process cfg_start time out message!\n");
		}
	}

	else if ((rcv.timeId == TIMER_ID_1S_L3BFDF_SUSPEND_WAIT_FB) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		if (func_l3bfdf_time_out_sys_resp_common_process() == FAILURE){
			HCU_ERROR_PRINT_L3BFDF("L3BFDF: Error process suspend time out message!\n");
		}
	}

	else if ((rcv.timeId == TIMER_ID_1S_L3BFDF_STOP_WAIT_FB) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		if (func_l3bfdf_time_out_sys_resp_common_process() == FAILURE){
			HCU_ERROR_PRINT_L3BFDF("L3BFDF: Error process stop time out message!\n");
		}
	}

	else if ((rcv.timeId == TIMER_ID_1S_L3BFDF_RESUME_WAIT_FB) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		if (func_l3bfdf_time_out_sys_resp_common_process() == FAILURE){
			HCU_ERROR_PRINT_L3BFDF("L3BFDF: Error process resume time out message!\n");
		}
	}

	//L3BFDF_TTT_WAIT_FB定时器，也没啥用，用不上

	//周期性统计扫描定时器
	else if ((rcv.timeId == TIMER_ID_10MS_L3BFDF_PERIOD_STA_SCAN) &&(rcv.timeRes == TIMER_RESOLUTION_10MS)){
		if (func_l3bfdf_time_out_statistic_scan_process() == FAILURE){
			HCU_ERROR_PRINT_L3BFDF("L3BFDF: Error process time out message!\n");
		}
		//Pure test code, to be deleted.
		//func_l3bfdf_send_out_suspend_message_to_all();
	}

	//返回
	return SUCCESS;
}

/***************************************************************************************************************************
 *
 * 　核心状态机处理过程
 *
 ***************************************************************************************************************************/
OPSTAT fsm_l3bfdf_uicomm_ctrl_cmd_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret=0;
	HCU_MSG_RCV_CHECK_FOR_GEN_LOCAL(TASK_ID_L3BFDF, msg_struct_uicomm_l3bfdf_ctrl_cmd_req_t);

	//启动命令
	if (rcv.cmdid == HCU_SYSMSG_BFDF_UICOMM_CMDID_CFG_START)
	{
		//判定不合法: to resume this function later, as all boards not yet installed.
		if (func_l3bfdf_is_there_any_board_not_yet_startup()==FALSE){
			HCU_L3BFDF_FEEDBACK_CTRL_RESP_MESSAGE(HCU_SYSMSG_BFDF_UICOMM_CMDID_CFG_START);
			return SUCCESS;
		}
		if (func_l3bfdf_send_out_cfg_start_message_to_all() == FAILURE) return FAILURE;
		//一旦收到配置命令即进入ACTIVE状态
		FsmSetState(TASK_ID_L3BFDF, FSM_STATE_L3BFDF_ACTIVED);
	}

	//STOP
	else if (rcv.cmdid == HCU_SYSMSG_BFDF_UICOMM_CMDID_STOP)
	{
		if (func_l3bfdf_send_out_stop_message_to_all() == FAILURE) return FAILURE;

		//统计报告部分初始化
		memset(&gTaskL3bfdfContext.sta15Min, 0, sizeof(HcuSysMsgIeL3bfdfContextStaElement_t));
		memset(&gTaskL3bfdfContext.sta24H, 0, sizeof(HcuSysMsgIeL3bfdfContextStaElement_t));
		memset(&gTaskL3bfdfContext.sta2H, 0, sizeof(HcuSysMsgIeL3bfdfContextStaElement_t));
		memset(&gTaskL3bfdfContext.sta60Min, 0, sizeof(HcuSysMsgIeL3bfdfContextStaElement_t));
		memset(&gTaskL3bfdfContext.sta8H, 0, sizeof(HcuSysMsgIeL3bfdfContextStaElement_t));
		memset(&gTaskL3bfdfContext.staLocalUi, 0, sizeof(HcuSysMsgIeL3bfdfContextStaElement_t));
		memset(&gTaskL3bfdfContext.staOneMin, 0, sizeof(HcuSysMsgIeL3bfdfContextStaElement_t));
		memset(&gTaskL3bfdfContext.staUp2Now, 0, sizeof(HcuSysMsgIeL3bfdfContextStaElement_t));
		//实时统计部分初始化
		gTaskL3bfdfContext.elipseCnt = 0;
		gTaskL3bfdfContext.elipse24HourCnt = 0;
		memset(&gTaskL3bfdfContext.cur, 0, sizeof(HcuSysMsgIeL3bfdfContextStaElement_t));
		memset(&gTaskL3bfdfContext.curAge, 0, sizeof(gTaskL3bfdfContextStaEleMid_t));
		//停止周期统计报告定时器
		hcu_timer_stop(TASK_ID_L3BFDF, TIMER_ID_10MS_L3BFDF_PERIOD_STA_SCAN, TIMER_RESOLUTION_10MS);
	}

	//RESUME
	else if (rcv.cmdid == HCU_SYSMSG_BFDF_UICOMM_CMDID_RESUME)
	{
		//如果当前状态不在SUSPEND状态，判定不合法，直接返回
		if (FsmGetState(TASK_ID_L3BFDF) != FSM_STATE_L3BFDF_SUSPEND) return SUCCESS;

		//发送RESUME：从板号１开始一直到HCU_SYSCFG_BFDF_NODE_BOARD_NBR_MAX-1
		if (func_l3bfdf_send_out_resume_message_to_all() == FAILURE) return FAILURE;
	}

	//SUSPEND
	else if (rcv.cmdid == HCU_SYSMSG_BFDF_UICOMM_CMDID_SUSPEND)
	{
		//如果当前状态不在进料组合状态，判定不合法，直接返回
		if (FsmGetState(TASK_ID_L3BFDF) != FSM_STATE_L3BFDF_OOS_SCAN) return SUCCESS;
		//发送SUSPEND
		if (func_l3bfdf_send_out_suspend_message_to_all() == FAILURE) return FAILURE;
	}

	//DYNAMIC_CALI
	else if (rcv.cmdid == HCU_SYSMSG_BFDF_UICOMM_CMDID_DYNAMIC_CALI){
		msg_struct_l3bfdf_can_dyn_cal_req_t snd;
		memset(&snd, 0, sizeof(msg_struct_l3bfdf_can_dyn_cal_req_t));
		snd.dynCalReq.TWeightInd = gTaskL3bfdfContext.dynCalPar.TWeightInd;
		snd.dynCalReq.WeightSensorTailorValue = gTaskL3bfdfContext.dynCalPar.WeightSensorTailorValue;
		snd.dynCalReq.adc_gain = gTaskL3bfdfContext.dynCalPar.adc_gain;
		snd.dynCalReq.adc_sample_freq = gTaskL3bfdfContext.dynCalPar.adc_sample_freq;
		snd.dynCalReq.full_weight = gTaskL3bfdfContext.dynCalPar.full_weight;
		snd.dynCalReq.max_allowed_weight = gTaskL3bfdfContext.dynCalPar.max_allowed_weight;
		snd.dynCalReq.motor_speed = gTaskL3bfdfContext.dynCalPar.motor_speed;
		snd.dynCalReq.noise_floor_filter_factor = gTaskL3bfdfContext.dynCalPar.noise_floor_filter_factor;

		int boardId = 0, lineId = 0;
		if(rcv.cmdValue == HCU_SYSMSG_BFDF_UICOMM_CMDVALUE_DYNAMIC_CALI_ZERO_LINE0){
			snd.dynCalReq.calibration_zero_or_full = 1; /* 1 for ZERO, 2 for FULL */
			snd.dynCalReq.calibration_iteration = gTaskL3bfdfContext.dynCalPar.zero_cal_iteration;
			lineId = 0;
			boardId = (lineId<<3) + 1;
			snd.boardBitmap[boardId] = 1;
			snd.length = sizeof(msg_struct_l3bfdf_can_dyn_cal_req_t);
			HCU_MSG_SEND_GENERNAL_PROCESS(MSG_ID_L3BFDF_CAN_DYN_CAL_REQ, TASK_ID_CANALPHA, TASK_ID_L3BFDF);
		}
		else if(rcv.cmdValue == HCU_SYSMSG_BFDF_UICOMM_CMDVALUE_DYNAMIC_CALI_FULL_LINE0){
			snd.dynCalReq.calibration_zero_or_full = 2;
			snd.dynCalReq.calibration_iteration = gTaskL3bfdfContext.dynCalPar.full_cal_iteration;
			lineId = 0;
			boardId = (lineId<<3) + 1;
			snd.boardBitmap[boardId] = 1;
			snd.length = sizeof(msg_struct_l3bfdf_can_dyn_cal_req_t);
			HCU_MSG_SEND_GENERNAL_PROCESS(MSG_ID_L3BFDF_CAN_DYN_CAL_REQ, TASK_ID_CANALPHA, TASK_ID_L3BFDF);
			//动态校准可以重复进行，一次request后用户重复放置砝码会有多次response返回
		}
		else if(rcv.cmdValue == HCU_SYSMSG_BFDF_UICOMM_CMDVALUE_DYNAMIC_CALI_ZERO_LINE1){
			snd.dynCalReq.calibration_zero_or_full = 1; /* 1 for ZERO, 2 for FULL */
			snd.dynCalReq.calibration_iteration = gTaskL3bfdfContext.dynCalPar.zero_cal_iteration;
			lineId = 1;
			boardId = (lineId<<3) + 1;
			snd.boardBitmap[boardId] = 1;
			snd.length = sizeof(msg_struct_l3bfdf_can_dyn_cal_req_t);
			HCU_MSG_SEND_GENERNAL_PROCESS(MSG_ID_L3BFDF_CAN_DYN_CAL_REQ, TASK_ID_CANALPHA, TASK_ID_L3BFDF);
		}
		else if(rcv.cmdValue == HCU_SYSMSG_BFDF_UICOMM_CMDVALUE_DYNAMIC_CALI_FULL_LINE1){
			snd.dynCalReq.calibration_zero_or_full = 2;
			snd.dynCalReq.calibration_iteration = gTaskL3bfdfContext.dynCalPar.full_cal_iteration;
			lineId = 1;
			boardId = (lineId<<3) + 1;
			snd.boardBitmap[boardId] = 1;
			snd.length = sizeof(msg_struct_l3bfdf_can_dyn_cal_req_t);
			HCU_MSG_SEND_GENERNAL_PROCESS(MSG_ID_L3BFDF_CAN_DYN_CAL_REQ, TASK_ID_CANALPHA, TASK_ID_L3BFDF);
			//动态校准可以重复进行，一次request后用户重复放置砝码会有多次response返回
		}
		else
			HCU_ERROR_PRINT_L3BFDF("L3BFDF: Receive cmdValue error for CMDID_DYNAMIC_CALI!\n");
	}

	//差错
	else HCU_ERROR_PRINT_L3BFDF("L3BFDF: Receive message error! Rcv.cmdId=%x, Rcv.value=%x.\n", rcv.cmdid, rcv.cmdValue);

	//返回
	return SUCCESS;
}

//单个板子重启，将重启整个系统：无法做到单个板子的重启，但整个系统还能继续工作的，因为料斗的状态是需要人工干预的，并不能依赖点击驱动的
OPSTAT fsm_l3bfdf_canitf_startup_ind(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret=0;
	int line=0, boardId=0;

	HCU_MSG_RCV_CHECK_FOR_GEN_LOCAL(TASK_ID_L3BFDF, msg_struct_sui_startup_ind_t);
	HCU_L3BFDF_INCOMING_MESSAGE_KEY_PARAMETERS_CHECK();

	line = rcv.snrId>>3;
	boardId = rcv.snrId & 0x07;
	//存储本地
	gTaskL3bfdfContext.nodeDyn[line][boardId].nodeStatus = HCU_L3BFDF_NODE_BOARD_STATUS_STARTUP;
	//printf("L3BFDF: Sensor ID = %d is set to be startup, line = %d, boardId = %d!\n", rcv.snrId, line, boardId);

	//通知界面
	HCU_L3BFDF_TRIGGER_UI_STATUS_REPORT(rcv.snrId, HUICOBUS_CMDID_CUI_HCU2UIR_GENERAL_CMDVAL_STARTUP);

	//为了简化流程，运行过程中任何板子重启导致收到STARTUP，将状态强制转到ACTIVE，等待界面重新开始
	if (FsmGetState(TASK_ID_L3BFDF) > FSM_STATE_L3BFDF_ACTIVED)
	{
		FsmSetState(TASK_ID_L3BFDF, FSM_STATE_L3BFDF_ACTIVED);
	}

	//Update latest boards status
	func_l3bfdf_print_all_board_status();

	//返回
	return SUCCESS;
}

OPSTAT fsm_l3bfdf_canitf_fault_ind(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret=0;
	int line=0, boardId=0;

	HCU_MSG_RCV_CHECK_FOR_GEN_LOCAL(TASK_ID_L3BFDF, msg_struct_sui_fault_ind_t);
	HCU_L3BFDF_INCOMING_MESSAGE_KEY_PARAMETERS_CHECK();

	//本地存储
	if (gTaskL3bfdfContext.nodeDyn[line][boardId].nodeStatus < HCU_L3BFDF_NODE_BOARD_STATUS_INIT_MAX)
		gTaskL3bfdfContext.nodeDyn[line][boardId].nodeStatus = HCU_L3BFDF_NODE_BOARD_STATUS_INIT_ERR;
	else
		gTaskL3bfdfContext.nodeDyn[line][boardId].nodeStatus = HCU_L3BFDF_NODE_BOARD_STATUS_VALID_ERROR;

	//通知界面
	UINT32 boardStatus;
	if (gTaskL3bfdfContext.nodeDyn[line][boardId].nodeStatus == HCU_L3BFDF_NODE_BOARD_STATUS_INIT_ERR)
		boardStatus = HUICOBUS_CMDID_CUI_HCU2UIR_GENERAL_CMDVAL_CFG_ERR;
	else
		boardStatus = HUICOBUS_CMDID_CUI_HCU2UIR_GENERAL_CMDVAL_ERROR;

	HCU_L3BFDF_TRIGGER_UI_STATUS_REPORT(rcv.snrId, boardStatus);

	//是否要根据ERR_CODE，赋予不同的差错情形，待定
	//称重传感器如果出错，则需要SUSPEND整个系统，以免损坏系统的持续运行．仅仅通知界面是不够的．
	if (func_l3bfdf_send_out_suspend_message_to_all() == FAILURE) return FAILURE;

	//返回
	return SUCCESS;
}

//static uint32_t counter_l3bfdf_canitf_heart_beat_report = 0;
OPSTAT fsm_l3bfdf_canitf_heart_beat_report(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret=0;
	int line=0, boardId=0;

	HCU_MSG_RCV_CHECK_FOR_GEN_LOCAL(TASK_ID_L3BFDF, msg_struct_sui_heart_beat_report_t);
	HCU_L3BFDF_INCOMING_MESSAGE_KEY_PARAMETERS_CHECK();

	//本地存储：只是为了防止下位机重启
	if (gTaskL3bfdfContext.nodeDyn[line][boardId].nodeStatus < HCU_L3BFDF_NODE_BOARD_STATUS_STARTUP){
		gTaskL3bfdfContext.nodeDyn[line][boardId].nodeStatus = HCU_L3BFDF_NODE_BOARD_STATUS_STARTUP;
		//通知界面
		HCU_L3BFDF_TRIGGER_UI_STATUS_REPORT(rcv.snrId, HUICOBUS_CMDID_CUI_HCU2UIR_GENERAL_CMDVAL_STARTUP);
	}

	//回送
	msg_struct_sui_heart_beat_confirm_t snd;
	memset(&snd, 0, sizeof(msg_struct_sui_heart_beat_confirm_t));
	snd.snrId = rcv.snrId;
	/*
	 *  2018/4/13, Update by ZHANG Jianlin
	 *  If heart beat received during node offline status, it shall set this node to be STARTUP status, iso keeping OFFLINE status.
	 *  In this way, heart-beat will take as STARTUP message function. This is indeed why we design heart-beat function, to take back
	 *  node when HCU restart or broken.
	 *  一言以蔽之：HEAT-BEAT可以将坏的，重启ＨＣＵ的状态拉回到正常状态．
	 */
	if (gTaskL3bfdfContext.nodeDyn[line][boardId].nodeStatus <= HCU_L3BFDF_NODE_BOARD_STATUS_OFFLINE_MAX){
		gTaskL3bfdfContext.nodeDyn[line][boardId].nodeStatus = HCU_L3BFDF_NODE_BOARD_STATUS_STARTUP;
		snd.state = HUITP_IEID_SUI_COM_HEATT_BEAT_WMC_STATE_INIT;
	}
	else if (gTaskL3bfdfContext.nodeDyn[line][boardId].nodeStatus < HCU_L3BFDF_NODE_BOARD_STATUS_VALID)
		snd.state = HUITP_IEID_SUI_COM_HEATT_BEAT_WMC_STATE_INIT;
	else if(gTaskL3bfdfContext.nodeDyn[line][boardId].nodeStatus <= HCU_L3BFDF_NODE_BOARD_STATUS_WORK_MAX)
		snd.state = HUITP_IEID_SUI_COM_HEATT_BEAT_WMC_STATE_WORKING;
	else
		snd.state = HUITP_IEID_SUI_COM_HEATT_BEAT_WMC_STATE_INVALID;
	snd.timeStamp = time(0);
	snd.length = sizeof(msg_struct_sui_heart_beat_confirm_t);
	HCU_MSG_SEND_GENERNAL_PROCESS(MSG_ID_SUI_HEART_BEAT_CONFIRM, TASK_ID_CANALPHA, TASK_ID_L3BFDF);

//	extern uint32_t counter_canalpha_sui_heart_beat_confirm;
//	extern uint32_t counter_canalpha_sui_heart_beat_confirm_ko;

//	if(rcv.snrId == 1)
//	{
//	    counter_l3bfdf_canitf_heart_beat_report++;
//	    printf("L3APP HEARTBEAT REPORT/CONFIRM %d times, CAN TX %d times, CAN TX KO %d times\r\n", counter_l3bfdf_canitf_heart_beat_report, counter_canalpha_sui_heart_beat_confirm, counter_canalpha_sui_heart_beat_confirm_ko);
//	}

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
OPSTAT fsm_l3bfdf_cloudvela_data_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HCU_MSG_RCV_CHECK_FOR_GEN_LOCAL(TASK_ID_L3BFDF, msg_struct_cloudvela_l3bfdf_data_req_t);

	return SUCCESS;
}

//由于是内部消息命令执行，为了简化整个执行，不设置超时状态，以简化整个状态机的设计
OPSTAT fsm_l3bfdf_cloudvela_data_confirm(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HCU_MSG_RCV_CHECK_FOR_GEN_LOCAL(TASK_ID_L3BFDF, msg_struct_cloudvela_l3bfdf_data_confirm_t);

	return SUCCESS;
}

//由于是内部消息命令执行，为了简化整个执行，不设置超时状态，以简化整个状态机的设计
OPSTAT fsm_l3bfdf_cloudvela_event_confirm(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HCU_MSG_RCV_CHECK_FOR_GEN_LOCAL(TASK_ID_L3BFDF, msg_struct_cloudvela_l3bfdf_event_confirm_t);

	return SUCCESS;
}

//由于是内部消息命令执行，为了简化整个执行，不设置超时状态，以简化整个状态机的设计
OPSTAT fsm_l3bfdf_cloudvela_ctrl_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HCU_MSG_RCV_CHECK_FOR_GEN_LOCAL(TASK_ID_L3BFDF, msg_struct_cloudvela_l3bfdf_ctrl_req_t);
//	if (rcv.cmdid != L3CI_bfdf_comb_scale){
//		HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Receive message error!\n");
//	}

	//将命令发送到传感器下位机
//	msg_struct_l3bfdf_can_general_cmd_req_t snd;
//	memset(&snd, 0, sizeof(msg_struct_l3bfdf_can_general_cmd_req_t));
//	snd.length = sizeof(msg_struct_l3bfdf_can_general_cmd_req_t);

	//这里只支持一种启动和停止命令，其它的暂时不支持
	//if ((rcv.optid != L3PO_bfdf_start_cmd) && (rcv.optid != L3PO_bfdf_stop_cmd)){
//	if ((rcv.scaleWeightCmd != L3PO_bfdf_start_cmd) && (rcv.scaleWeightCmd != L3PO_bfdf_stop_cmd)){
//				HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Can not supported command coming from cloud!\n");
//	}

//	snd.optid = rcv.optid;
//	snd.optpar = rcv.optopr;
	//这里如此设置，表示是为了全局所有的传感器
//	snd.sensorid = HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX;
//	HCU_MSG_SEND_GENERNAL_PROCESS(MSG_ID_L3BFDF_CAN_GENERAL_CMD_REQ, TASK_ID_CANALPHA, TASK_ID_L3BFDF);

	//状态不转移

	//短时钟不启动

	//返回
	return SUCCESS;
}

//由于是内部消息命令执行，为了简化整个执行，不设置超时状态，以简化整个状态机的设计
OPSTAT fsm_l3bfdf_cloudvela_statistic_confirm(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HCU_MSG_RCV_CHECK_FOR_GEN_LOCAL(TASK_ID_L3BFDF, msg_struct_cloudvela_l3bfdf_statistic_confirm_t);

	return SUCCESS;
}


/***************************************************************************************************************************
 *
 *  CANITF下位机传感器处理的结果
 *
 ***************************************************************************************************************************/
OPSTAT fsm_l3bfdf_canitf_sys_config_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret=0;
	int i = 0, j = 0;
	int line=0, boardId=0;

	HCU_MSG_RCV_CHECK_FOR_GEN_LOCAL(TASK_ID_L3BFDF, msg_struct_can_l3bfdf_sys_cfg_resp_t);
	HCU_L3BFDF_INCOMING_MESSAGE_KEY_PARAMETERS_CHECK();

	//printf("L3BFDF: Receiving CFG_RESP, Flag/SnrId=%d/%d\n", rcv.validFlag, rcv.snrId);

	//收到错误的反馈，就回复差错给界面
	if (rcv.validFlag == FALSE){
		//先改本传感器的状态
		gTaskL3bfdfContext.nodeDyn[line][boardId].nodeStatus = HCU_L3BFDF_NODE_BOARD_STATUS_STARTUP;
		gTaskL3bfdfContext.nodeDyn[line][boardId].respRcvFlag = TRUE;
		//通知界面
		HCU_L3BFDF_TRIGGER_UI_STATUS_REPORT(rcv.snrId, HUICOBUS_CMDID_CUI_HCU2UIR_GENERAL_CMDVAL_CFG_ERR);
		//发送反馈给UICOMM
		msg_struct_l3bfdf_uicomm_ctrl_cmd_resp_t snd;
		memset(&snd, 0, sizeof(msg_struct_l3bfdf_uicomm_ctrl_cmd_resp_t));
		snd.cmdid = HCU_SYSMSG_BFDF_UICOMM_CMDID_CFG_START;
		snd.validFlag = FALSE;
		snd.errCode = rcv.errCode;
		snd.sensorid = rcv.snrId;
		snd.length = sizeof(msg_struct_l3bfdf_uicomm_ctrl_cmd_resp_t);
		HCU_MSG_SEND_GENERNAL_PROCESS(MSG_ID_L3BFDF_UICOMM_CTRL_CMD_RESP, TASK_ID_BFDFUICOMM, TASK_ID_L3BFDF);
		hcu_timer_stop(TASK_ID_L3BFDF, TIMER_ID_1S_L3BFDF_CFG_START_WAIT_FB, TIMER_RESOLUTION_1S);
		FsmSetState(TASK_ID_L3BFDF, FSM_STATE_L3BFDF_ACTIVED);
	}

	//先改本传感器的状态
	gTaskL3bfdfContext.nodeDyn[line][boardId].nodeStatus = HCU_L3BFDF_NODE_BOARD_STATUS_CFG_START_CMPL;
	gTaskL3bfdfContext.nodeDyn[line][boardId].respRcvFlag = TRUE;
	//通知界面
	HCU_L3BFDF_TRIGGER_UI_STATUS_REPORT(rcv.snrId, HUICOBUS_CMDID_CUI_HCU2UIR_GENERAL_CMDVAL_CFG_OK);

	//收到正确以及齐活的反馈
	//这里有漏洞：如果是单个板子重启，可能造成整个系统重新刷新料斗状态．那到底是否要支持单个板子重启？
	//合理的逻辑是：单个板子重启，将重启整个系统
	if (func_l3bfdf_cacluate_sensor_cfg_start_rcv_complete() == TRUE){
		//发送反馈给UICOMM
		msg_struct_l3bfdf_uicomm_ctrl_cmd_resp_t snd;
		memset(&snd, 0, sizeof(msg_struct_l3bfdf_uicomm_ctrl_cmd_resp_t));
		snd.cmdid = HCU_SYSMSG_BFDF_UICOMM_CMDID_CFG_START;
		snd.sensorid = rcv.snrId;
		snd.validFlag = TRUE;
		snd.length = sizeof(msg_struct_l3bfdf_uicomm_ctrl_cmd_resp_t);
		HCU_MSG_SEND_GENERNAL_PROCESS(MSG_ID_L3BFDF_UICOMM_CTRL_CMD_RESP, TASK_ID_BFDFUICOMM, TASK_ID_L3BFDF);
		hcu_timer_stop(TASK_ID_L3BFDF, TIMER_ID_1S_L3BFDF_CFG_START_WAIT_FB, TIMER_RESOLUTION_1S);
		FsmSetState(TASK_ID_L3BFDF, FSM_STATE_L3BFDF_OOS_SCAN);

		//设置所有料斗状态到工作状态
		for (i=0; i<HCU_L3BFDF_MAX_STREAM_LINE_ACTUAL; i++){
			if (func_l3bfdf_hopper_state_set_valid(i) == FALSE)
				//To enable configure continuously work, temparily disable RECOVERY work mechanism
				//HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Init global parameter error! Index = %d.\n", i);
				HCU_DEBUG_PRINT_CRT("L3BFDF: Init global parameter error, but still temp. working continuously! Index = %d.\n", i);
		}

		//批次Session+1：它的初始化应该存入数据库表单
		gTaskL3bfdfContext.sessionId++;
		//将session+1的结果存入数据库

		//启动统计扫描定时器
		hcu_timer_start(TASK_ID_L3BFDF, HCU_TIMERID_WITH_DUR(TIMER_ID_10MS_L3BFDF_PERIOD_STA_SCAN), TIMER_TYPE_PERIOD, TIMER_RESOLUTION_10MS);
		//设置工作启动时间
		gTaskL3bfdfContext.startWorkTimeInUnix = time(0);

		//状态修改为业务态
		for (i=0; i<HCU_SYSCFG_BFDF_EQU_FLOW_NBR_MAX; i++)
		{
			for (j=1; j<HCU_SYSCFG_BFDF_NODE_BOARD_NBR_MAX; j++)
			{
				if (gTaskL3bfdfContext.nodeDyn[i][j].nodeStatus == HCU_L3BFDF_NODE_BOARD_STATUS_CFG_START_CMPL)
					gTaskL3bfdfContext.nodeDyn[i][j].nodeStatus = HCU_L3BFDF_NODE_BOARD_STATUS_VALID;
			}
		}
	}

	//返回
	return SUCCESS;
}

OPSTAT fsm_l3bfdf_canitf_sys_suspend_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret=0;
	int line=0, boardId=0;

	HCU_MSG_RCV_CHECK_FOR_GEN_LOCAL(TASK_ID_L3BFDF, msg_struct_sui_suspend_resp_t);
	HCU_L3BFDF_INCOMING_MESSAGE_KEY_PARAMETERS_CHECK();

	//printf("L3BFDF: Suspend resp rcv, SnrId=%d, flag=%d\n", rcv.snrId, rcv.validFlag);

	//收到错误的反馈，就回复差错给界面
	if (rcv.validFlag == FALSE){
		//先改本传感器的状态
		gTaskL3bfdfContext.nodeDyn[line][boardId].nodeStatus = HCU_L3BFDF_NODE_BOARD_STATUS_VALID_ERROR;
		gTaskL3bfdfContext.nodeDyn[line][boardId].respRcvFlag = FALSE;
		//发送反馈给UICOMM
		msg_struct_l3bfdf_uicomm_ctrl_cmd_resp_t snd;
		memset(&snd, 0, sizeof(msg_struct_l3bfdf_uicomm_ctrl_cmd_resp_t));
		snd.validFlag = FALSE;
		snd.errCode = rcv.errCode;
		snd.sensorid = rcv.snrId;
		snd.cmdid = HCU_SYSMSG_BFDF_UICOMM_CMDID_SUSPEND;
		snd.length = sizeof(msg_struct_l3bfdf_uicomm_ctrl_cmd_resp_t);
		HCU_MSG_SEND_GENERNAL_PROCESS(MSG_ID_L3BFDF_UICOMM_CTRL_CMD_RESP, TASK_ID_BFDFUICOMM, TASK_ID_L3BFDF);
		//停止定时器
		hcu_timer_stop(TASK_ID_L3BFDF, TIMER_ID_1S_L3BFDF_SUSPEND_WAIT_FB, TIMER_RESOLUTION_1S);
		FsmSetState(TASK_ID_L3BFDF, FSM_STATE_L3BFDF_ACTIVED);
		return SUCCESS;
	}

	//先改本传感器的状态
	gTaskL3bfdfContext.nodeDyn[line][boardId].nodeStatus = HCU_L3BFDF_NODE_BOARD_STATUS_SUSPEND;
	gTaskL3bfdfContext.nodeDyn[line][boardId].respRcvFlag = TRUE;
	//通知界面
	HCU_L3BFDF_TRIGGER_UI_STATUS_REPORT(rcv.snrId, HUICOBUS_CMDID_CUI_HCU2UIR_GENERAL_CMDVAL_SUSPEND);

	//收到正确以及齐活的反馈
	if (func_l3bfdf_cacluate_sensor_suspend_rcv_complete() == TRUE){
		//发送反馈给UICOMM
		msg_struct_l3bfdf_uicomm_ctrl_cmd_resp_t snd;
		memset(&snd, 0, sizeof(msg_struct_l3bfdf_uicomm_ctrl_cmd_resp_t));
		snd.cmdid = HCU_SYSMSG_BFDF_UICOMM_CMDID_SUSPEND;
		snd.validFlag = TRUE;
		snd.length = sizeof(msg_struct_l3bfdf_uicomm_ctrl_cmd_resp_t);
		HCU_MSG_SEND_GENERNAL_PROCESS(MSG_ID_L3BFDF_UICOMM_CTRL_CMD_RESP, TASK_ID_BFDFUICOMM, TASK_ID_L3BFDF);
		//停止定时器
		hcu_timer_stop(TASK_ID_L3BFDF, TIMER_ID_1S_L3BFDF_SUSPEND_WAIT_FB, TIMER_RESOLUTION_1S);
		FsmSetState(TASK_ID_L3BFDF, FSM_STATE_L3BFDF_SUSPEND);
	}

	//返回
	return SUCCESS;
}

OPSTAT fsm_l3bfdf_canitf_sys_stop_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret=0;
	int line=0, boardId=0;

	HCU_MSG_RCV_CHECK_FOR_GEN_LOCAL(TASK_ID_L3BFDF, msg_struct_sui_stop_resp_t);
	HCU_L3BFDF_INCOMING_MESSAGE_KEY_PARAMETERS_CHECK();

	printf("L3BFDF: Stop resp rcv, SnrId=%d, flag=%d\n", rcv.snrId, rcv.validFlag);

	//收到错误的反馈，就回复差错给界面
	if (rcv.validFlag == FALSE){
		//先改本传感器的状态
		gTaskL3bfdfContext.nodeDyn[line][boardId].nodeStatus = HCU_L3BFDF_NODE_BOARD_STATUS_VALID_ERROR;
		gTaskL3bfdfContext.nodeDyn[line][boardId].respRcvFlag = FALSE;
		//发送反馈给UICOMM
		msg_struct_l3bfdf_uicomm_ctrl_cmd_resp_t snd;
		memset(&snd, 0, sizeof(msg_struct_l3bfdf_uicomm_ctrl_cmd_resp_t));
		snd.validFlag = FALSE;
		snd.errCode = rcv.errCode;
		snd.sensorid = rcv.snrId;
		snd.cmdid = HCU_SYSMSG_BFDF_UICOMM_CMDID_STOP;
		snd.length = sizeof(msg_struct_l3bfdf_uicomm_ctrl_cmd_resp_t);
		HCU_MSG_SEND_GENERNAL_PROCESS(MSG_ID_L3BFDF_UICOMM_CTRL_CMD_RESP, TASK_ID_BFDFUICOMM, TASK_ID_L3BFDF);
		//停止定时器
		hcu_timer_stop(TASK_ID_L3BFDF, TIMER_ID_1S_L3BFDF_STOP_WAIT_FB, TIMER_RESOLUTION_1S);
		FsmSetState(TASK_ID_L3BFDF, FSM_STATE_L3BFDF_ACTIVED);
		return SUCCESS;
	}

	//先改本传感器的状态
	gTaskL3bfdfContext.nodeDyn[line][boardId].nodeStatus = HCU_L3BFDF_NODE_BOARD_STATUS_STOP;
	gTaskL3bfdfContext.nodeDyn[line][boardId].respRcvFlag = TRUE;
	//通知界面
	HCU_L3BFDF_TRIGGER_UI_STATUS_REPORT(rcv.snrId, HUICOBUS_CMDID_CUI_HCU2UIR_GENERAL_CMDVAL_STOP);

	//收到正确以及齐活的反馈
	if (func_l3bfdf_cacluate_sensor_stop_rcv_complete() == TRUE){
		//发送反馈给UICOMM
		msg_struct_l3bfdf_uicomm_ctrl_cmd_resp_t snd;
		memset(&snd, 0, sizeof(msg_struct_l3bfdf_uicomm_ctrl_cmd_resp_t));
		snd.cmdid = HCU_SYSMSG_BFDF_UICOMM_CMDID_STOP;
		snd.validFlag = TRUE;
		snd.length = sizeof(msg_struct_l3bfdf_uicomm_ctrl_cmd_resp_t);
		HCU_MSG_SEND_GENERNAL_PROCESS(MSG_ID_L3BFDF_UICOMM_CTRL_CMD_RESP, TASK_ID_BFDFUICOMM, TASK_ID_L3BFDF);
		//停止定时器
		hcu_timer_stop(TASK_ID_L3BFDF, TIMER_ID_1S_L3BFDF_STOP_WAIT_FB, TIMER_RESOLUTION_1S);
		FsmSetState(TASK_ID_L3BFDF, FSM_STATE_L3BFDF_ACTIVED);
	}

	//返回
	return SUCCESS;
}

OPSTAT fsm_l3bfdf_canitf_dyn_cal_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HCU_MSG_RCV_CHECK_FOR_GEN_LOCAL(TASK_ID_L3BFDF, msg_struct_can_l3bfdf_dyn_cal_resp_t);

	msg_struct_l3bfdf_uicomm_ctrl_cmd_resp_t snd;
	memset(&snd, 0, sizeof(msg_struct_l3bfdf_uicomm_ctrl_cmd_resp_t));

	snd.cmdid = HCU_SYSMSG_BFDF_UICOMM_CMDID_DYNAMIC_CALI;
	snd.validFlag = rcv.validFlag;
	snd.sensorid = rcv.snrId;
	snd.errCode = rcv.errCode;
	memcpy(&snd.dynCalResp, &rcv.dynCalResp, sizeof(strMsgIe_bfdf_calibration_resp_t));
	snd.length = sizeof(msg_struct_l3bfdf_uicomm_ctrl_cmd_resp_t);
	HCU_MSG_SEND_GENERNAL_PROCESS(MSG_ID_L3BFDF_UICOMM_CTRL_CMD_RESP, TASK_ID_BFDFUICOMM, TASK_ID_L3BFDF);

	//返回
	return SUCCESS;
}

OPSTAT fsm_l3bfdf_canitf_sys_resume_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret=0;
	int line=0, boardId=0;

	HCU_MSG_RCV_CHECK_FOR_GEN_LOCAL(TASK_ID_L3BFDF, msg_struct_sui_resume_resp_t);
	HCU_L3BFDF_INCOMING_MESSAGE_KEY_PARAMETERS_CHECK();

	//printf("L3BFDF: Resume resp rcv, SnrId=%d, flag=%d\n", rcv.snrId, rcv.validFlag);

	//收到错误的反馈，就回复差错给界面
	if (rcv.validFlag == FALSE){
		//先改本传感器的状态
		gTaskL3bfdfContext.nodeDyn[line][boardId].nodeStatus = HCU_L3BFDF_NODE_BOARD_STATUS_VALID_ERROR;
		gTaskL3bfdfContext.nodeDyn[line][boardId].respRcvFlag = FALSE;
		//发送反馈给UICOMM
		msg_struct_l3bfdf_uicomm_ctrl_cmd_resp_t snd;
		memset(&snd, 0, sizeof(msg_struct_l3bfdf_uicomm_ctrl_cmd_resp_t));
		snd.validFlag = FALSE;
		snd.errCode = rcv.errCode;
		snd.sensorid = rcv.snrId;
		snd.cmdid = HCU_SYSMSG_BFDF_UICOMM_CMDID_RESUME;
		snd.length = sizeof(msg_struct_l3bfdf_uicomm_ctrl_cmd_resp_t);
		HCU_MSG_SEND_GENERNAL_PROCESS(MSG_ID_L3BFDF_UICOMM_CTRL_CMD_RESP, TASK_ID_BFDFUICOMM, TASK_ID_L3BFDF);
		//停止定时器
		hcu_timer_stop(TASK_ID_L3BFDF, TIMER_ID_1S_L3BFDF_SUSPEND_WAIT_FB, TIMER_RESOLUTION_1S);
		FsmSetState(TASK_ID_L3BFDF, FSM_STATE_L3BFDF_ACTIVED);
		return SUCCESS;
	}

	//先改本传感器的状态
	gTaskL3bfdfContext.nodeDyn[line][boardId].nodeStatus = HCU_L3BFDF_NODE_BOARD_STATUS_RESUME;
	gTaskL3bfdfContext.nodeDyn[line][boardId].respRcvFlag = TRUE;
	//通知界面
	HCU_L3BFDF_TRIGGER_UI_STATUS_REPORT(rcv.snrId, HUICOBUS_CMDID_CUI_HCU2UIR_GENERAL_CMDVAL_RESUME);

	//收到正确以及齐活的反馈
	if (func_l3bfdf_cacluate_sensor_resume_rcv_complete() == TRUE){
		//发送反馈给UICOMM
		msg_struct_l3bfdf_uicomm_ctrl_cmd_resp_t snd;
		memset(&snd, 0, sizeof(msg_struct_l3bfdf_uicomm_ctrl_cmd_resp_t));
		snd.cmdid = HCU_SYSMSG_BFDF_UICOMM_CMDID_RESUME;
		snd.validFlag = TRUE;
		snd.length = sizeof(msg_struct_l3bfdf_uicomm_ctrl_cmd_resp_t);
		HCU_MSG_SEND_GENERNAL_PROCESS(MSG_ID_L3BFDF_UICOMM_CTRL_CMD_RESP, TASK_ID_BFDFUICOMM, TASK_ID_L3BFDF);
		//停止定时器
		hcu_timer_stop(TASK_ID_L3BFDF, TIMER_ID_1S_L3BFDF_SUSPEND_WAIT_FB, TIMER_RESOLUTION_1S);
		FsmSetState(TASK_ID_L3BFDF, FSM_STATE_L3BFDF_OOS_SCAN);
	}

	//返回
	return SUCCESS;
}


//触发组合算法
//进来的重量是NF2，在整个计算过程中，全部以NF2进行表达
//为了让单个料斗可以无限制的增加物料，必须不断的接受物料的进入：LastMat控制机制就不能用了，这相当于多HARQ机制，需要滑动窗口，单个HARQ和缓冲区搞不定了
OPSTAT fsm_l3bfdf_canitf_ws_new_ready_event(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret=0;
	INT32 weight = 0;
	UINT16 outHopperId = 0;
	int i=0;
	int line=0, boardId=0;

	HCU_MSG_RCV_CHECK_FOR_GEN_LOCAL(TASK_ID_L3BFDF, msg_struct_can_l3bfdf_new_ready_event_t);
	HCU_L3BFDF_INCOMING_MESSAGE_KEY_PARAMETERS_CHECK();
	if (boardId != 1) HCU_ERROR_PRINT_L3BFDF("L3BFDF: Receiving message error!\n");

//	//UPDATE UI
//	HCU_L3BFDF_TRIGGER_UI_STATUS_REPORT(rcv.snrId, HUICOBUS_CMDID_CUI_HCU2UIR_GENERAL_CMDVAL_DATA_VALID);
//
	//Update latest boards status
	if ((rand()%HCU_L3BFDF_STATISTIC_PRINT_FREQUENCY) == 0){
		func_l3bfdf_print_all_board_status();
	}

	//通知界面, should be put after algo result
	StrHlcIe_cui_hcu2uir_inswgt_bfdf_report_t inswgt_report;
	memset(&inswgt_report, 0, sizeof(StrHlcIe_cui_hcu2uir_inswgt_bfdf_report_t));
	inswgt_report.weight = rcv.sensorWsValue;
	inswgt_report.lineId = line;
	inswgt_report.hopperId = 0;

	//统计更新
	gTaskL3bfdfContext.cur.wsIncMatCnt++;
	gTaskL3bfdfContext.cur.wsIncMatWgt += rcv.sensorWsValue;

	//先做Audit
	int res = func_l3bfdf_hopper_dual_chain_audit();
	if (res <0) HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Audit error, errCode = %d\n", res);

	//打印统计信息
	if ((rand()%HCU_L3BFDF_STATISTIC_PRINT_FREQUENCY) == 0){
		//手工浏览一遍双链表，进行打印
		for (i=0; i<HCU_L3BFDF_MAX_STREAM_LINE_ACTUAL; i++){
			func_l3bfdf_print_all_hopper_status_by_chain(i);
		}

		//打印所有Hopper的仓位情况
		for (i=0; i<HCU_L3BFDF_MAX_STREAM_LINE_ACTUAL; i++){
			func_l3bfdf_print_all_hopper_ratio_by_weight(i);
		}
	}

	//超界限错误，扔进垃圾桶
	//UINT16 gidMin = 1;
	UINT16 gidMax = gTaskL3bfdfContext.totalGroupNbr[line];
	weight = rcv.sensorWsValue;
	//if ((weight < gTaskL3bfdfContext.group[line][gidMin].rangeLow) || (weight > gTaskL3bfdfContext.group[line][gidMax].rangeHigh))
	if (func_l3bfdf_judge_material_in_official_range(weight, line) == FALSE)
	{
		//HCU_DEBUG_PRINT_CRT("L3BFDF: NEW EVENT S/W=%d/%f, Low/High=%f/%f\n", line, weight, gTaskL3bfdfContext.group[line][gidMin].rangeLow, gTaskL3bfdfContext.group[line][gidMax].rangeHigh);

		//UPDATE UI
		HCU_L3BFDF_TRIGGER_UI_STATUS_REPORT(rcv.snrId, HUICOBUS_CMDID_CUI_HCU2UIR_GENERAL_CMDVAL_DATA_VALID);

		//更新目标料斗重量 in NF2
		gTaskL3bfdfContext.hopper[line][0].hopperValue += rcv.sensorWsValue;

		//更新统计信息
		gTaskL3bfdfContext.cur.wsCombTimes++;
		gTaskL3bfdfContext.cur.wsTgvTimes++;
		gTaskL3bfdfContext.cur.wsTgvMatCnt++;
		gTaskL3bfdfContext.cur.wsTgvMatWgt += weight;

		//打印状态
		HCU_DEBUG_PRINT_FAT("L3BFDF: Send un-range mat to rubbish!\n");

		//送往垃圾桶并通知界面
		inswgt_report.hopperId = 0;
		inswgt_report.groupId = 0;
		inswgt_report.validFlag = TRUE;
		inswgt_report.curWgt = 0;
		inswgt_report.curRatio = 0;
		inswgt_report.bufWgt = 0;
		inswgt_report.bufRatio = 0;
		hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_inswgt_bfdf_report(rcv.snrId, &inswgt_report);

		return SUCCESS;
	}

	//先分组，分到具体的组别
	UINT16 gId = func_l3bfdf_new_ws_search_group(line, weight);
	UINT16 fillHopper = 0;
	if ((gId == 0) || (gId > gidMax))
		HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Can not find right group! StreamId=%d, rcv.weight=%dg\n", line, rcv.sensorWsValue);

	//STEP0: 如果已经双满的，则不管了
	//STEP1: 先找加上本物料之后就双满的
	outHopperId = func_l3bfdf_new_ws_search_hopper_buffer_lack_one(line, gId, weight);
	if (outHopperId !=0){
		HCU_DEBUG_PRINT_INF("L3BFDF: Buffer full, Hopperid=%d\n", outHopperId);
		//发送出料
		if (func_l3bfdf_new_ws_send_out_comb_out_message_w_double_full(line, outHopperId) == FALSE){
			gTaskL3bfdfContext.hopper[line][outHopperId].hopperStatus = HCU_L3BFDF_HOPPER_STATUS_VALID_ERR;
			HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Send Comb Out message error!\n");
		}

		//UPDATE UI
		HCU_L3BFDF_TRIGGER_UI_STATUS_REPORT(rcv.snrId, HUICOBUS_CMDID_CUI_HCU2UIR_GENERAL_CMDVAL_DATA_VALID);

		//本料斗信息更新
		gTaskL3bfdfContext.hopper[line][outHopperId].hopperStatus = HCU_L3BFDF_HOPPER_STATUS_BUF_FULL;
		gTaskL3bfdfContext.hopper[line][outHopperId].buferValue += weight;

		//动态更新调控数量
		double gap = (double)gTaskL3bfdfContext.group[line][gId].targetWeight - (double)gTaskL3bfdfContext.hopper[line][outHopperId].buferValue;
		double avg = gTaskL3bfdfContext.group[line][gId].rangeAvg==0?0.01:gTaskL3bfdfContext.group[line][gId].rangeAvg;
		gTaskL3bfdfContext.hopper[line][outHopperId].matLackIndex = ((UINT32)(gap/avg))&0xFFFF;

		//循环将fillHopper后移
		fillHopper = gTaskL3bfdfContext.group[line][gId].fillHopperId;
		gTaskL3bfdfContext.group[line][gId].fillHopperId = gTaskL3bfdfContext.hopper[line][fillHopper].nextHopperId;

		//更新统计信息
		gTaskL3bfdfContext.cur.wsCombTimes++;
		gTaskL3bfdfContext.cur.wsTttTimes++;
		gTaskL3bfdfContext.cur.wsTttMatCnt++;
		gTaskL3bfdfContext.cur.wsTttMatWgt += weight;

		//打印状态
		HCU_DEBUG_PRINT_INF("L3BFDF: Find full buffer! Line = %d, HopperId = %d\n", line, outHopperId);

		//送往明确的满buf并通知界面
		inswgt_report.hopperId = outHopperId;
		inswgt_report.groupId = gId;
		inswgt_report.validFlag = TRUE;
		inswgt_report.curWgt = gTaskL3bfdfContext.hopper[line][outHopperId].hopperValue;
		inswgt_report.curRatio = (UINT8)((float)gTaskL3bfdfContext.hopper[line][outHopperId].hopperValue/\
				(float)gTaskL3bfdfContext.group[line][gId].targetWeight*100.0)&0xFF;
		inswgt_report.bufWgt = gTaskL3bfdfContext.hopper[line][outHopperId].buferValue;
		inswgt_report.bufRatio = (UINT8)((float)gTaskL3bfdfContext.hopper[line][outHopperId].buferValue/\
				(float)gTaskL3bfdfContext.group[line][gId].bufWgtTarget*100.0)&0xFF;
		hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_inswgt_bfdf_report(rcv.snrId, &inswgt_report);

		return SUCCESS;
	}

	//STEP2: 再寻找BASKET已经满的，但BUF不是满的
	outHopperId = func_l3bfdf_new_ws_search_hopper_buffer_normal(line, gId, weight);
	if (outHopperId !=0){
		HCU_DEBUG_PRINT_INF("L3BFDF: Buffer Cont, Hopperid=%d\n", outHopperId);
		//发送出料
		if (func_l3bfdf_new_ws_send_out_comb_out_message_w_basket_full(line, outHopperId) == FALSE){
			gTaskL3bfdfContext.hopper[line][outHopperId].hopperStatus = HCU_L3BFDF_HOPPER_STATUS_VALID_ERR;
			HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Send Comb Out message error!\n");
		}

		//UPDATE UI
		HCU_L3BFDF_TRIGGER_UI_STATUS_REPORT(rcv.snrId, HUICOBUS_CMDID_CUI_HCU2UIR_GENERAL_CMDVAL_DATA_VALID);

		//本料斗信息更新
		gTaskL3bfdfContext.hopper[line][outHopperId].hopperStatus = HCU_L3BFDF_HOPPER_STATUS_BUF_CONT;
		gTaskL3bfdfContext.hopper[line][outHopperId].buferValue += weight;

		//动态更新调控数量
		double gap = (double)gTaskL3bfdfContext.group[line][gId].targetWeight - (double)gTaskL3bfdfContext.hopper[line][outHopperId].buferValue;
		double avg = gTaskL3bfdfContext.group[line][gId].rangeAvg==0?0.01:gTaskL3bfdfContext.group[line][gId].rangeAvg;
		gTaskL3bfdfContext.hopper[line][outHopperId].matLackIndex = ((UINT32)(gap/avg))&0xFFFF;

		//更新统计信息
		gTaskL3bfdfContext.cur.wsCombTimes++;
		gTaskL3bfdfContext.cur.wsTttTimes++;
		gTaskL3bfdfContext.cur.wsTttMatCnt++;
		gTaskL3bfdfContext.cur.wsTttMatWgt += weight;

		//打印状态
		HCU_DEBUG_PRINT_INF("L3BFDF: Find continuous working buffer! Line = %d, HopperId = %d\n", line, outHopperId);

		//送往明确的满buf并通知界面
		inswgt_report.hopperId = outHopperId;
		inswgt_report.groupId = gId;
		inswgt_report.validFlag = TRUE;
		inswgt_report.curWgt = gTaskL3bfdfContext.hopper[line][outHopperId].hopperValue;
		inswgt_report.curRatio = (UINT8)((float)gTaskL3bfdfContext.hopper[line][outHopperId].hopperValue/\
				(float)gTaskL3bfdfContext.group[line][gId].targetWeight*100.0)&0xFF;
		inswgt_report.bufWgt = gTaskL3bfdfContext.hopper[line][outHopperId].buferValue;
		inswgt_report.bufRatio = (UINT8)((float)gTaskL3bfdfContext.hopper[line][outHopperId].buferValue/\
				(float)gTaskL3bfdfContext.group[line][gId].bufWgtTarget*100.0)&0xFF;
		hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_inswgt_bfdf_report(rcv.snrId, &inswgt_report);

		return SUCCESS;
	}

	//STEP3: 寻找BASKET欠1满，然后物料框就满了
	outHopperId = func_l3bfdf_new_ws_search_hopper_basket_lack_one(line, gId, weight);
	if (outHopperId != 0){
		HCU_DEBUG_PRINT_INF("L3BFDF: Lacking one, Hopperid=%d, Hopper status=%d\n", outHopperId, gTaskL3bfdfContext.hopper[line][outHopperId].hopperStatus);
		//先发送出料：出料的定时机制，如何处理？这是重入过程：使用状态机，不用定时器
		if (func_l3bfdf_new_ws_send_out_comb_out_message_w_basket_full(line, outHopperId) == FALSE){
			gTaskL3bfdfContext.hopper[line][outHopperId].hopperStatus = HCU_L3BFDF_HOPPER_STATUS_VALID_ERR;
			HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Send out pullin message error!\n");
		}

		//UPDATE UI
		HCU_L3BFDF_TRIGGER_UI_STATUS_REPORT(rcv.snrId, HUICOBUS_CMDID_CUI_HCU2UIR_GENERAL_CMDVAL_DATA_VALID);

		//等待入料成功后，状态设置为满，即可激活出料过程
		gTaskL3bfdfContext.hopper[line][outHopperId].hopperStatus = HCU_L3BFDF_HOPPER_STATUS_BASKET_FULL;
		gTaskL3bfdfContext.hopper[line][outHopperId].hopperValue += weight;

		//动态更新调控数量
		gTaskL3bfdfContext.hopper[line][outHopperId].matLackIndex = gTaskL3bfdfContext.hopper[line][outHopperId].matLackNbr;

		//更新统计信息
		gTaskL3bfdfContext.cur.wsCombTimes++;
		gTaskL3bfdfContext.cur.wsTttTimes++;
		gTaskL3bfdfContext.cur.wsTttMatCnt++;
		gTaskL3bfdfContext.cur.wsTttMatWgt += weight;

		//打印状态
		HCU_DEBUG_PRINT_INF("L3BFDF: Find Lacking-one basket! Line = %d, HopperId = %d\n", line, outHopperId);

		//送往明确的满桶并通知界面
		inswgt_report.hopperId = outHopperId;
		inswgt_report.groupId = gId;
		inswgt_report.validFlag = TRUE;
		inswgt_report.curWgt = gTaskL3bfdfContext.hopper[line][outHopperId].hopperValue;
		inswgt_report.curRatio = (UINT8)((float)gTaskL3bfdfContext.hopper[line][outHopperId].hopperValue/\
				(float)gTaskL3bfdfContext.group[line][gId].targetWeight*100.0)&0xFF;
		inswgt_report.bufWgt = gTaskL3bfdfContext.hopper[line][outHopperId].buferValue;
		inswgt_report.bufRatio = (UINT8)((float)gTaskL3bfdfContext.hopper[line][outHopperId].buferValue/\
				(float)gTaskL3bfdfContext.group[line][gId].bufWgtTarget*100.0)&0xFF;
		hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_inswgt_bfdf_report(rcv.snrId, &inswgt_report);

		return SUCCESS;
	}

	//通过fillHopper进行搜索：深水区，需要判定料斗的欠缺深度
	//从fillHopper入手，寻找第一个可以满足区间控制的目标，然后填入
	//STEP4: 正常状态
	outHopperId = func_l3bfdf_new_ws_search_hopper_valid_normal(line, gId, weight);
	if (outHopperId == 0){
		//UPDATE UI
		HCU_L3BFDF_TRIGGER_UI_STATUS_REPORT(rcv.snrId, HUICOBUS_CMDID_CUI_HCU2UIR_GENERAL_CMDVAL_DATA_VALID);

		//更新统计数据
		gTaskL3bfdfContext.cur.wsCombTimes++;
		gTaskL3bfdfContext.cur.wsTgvTimes++;
		gTaskL3bfdfContext.cur.wsTgvMatCnt++;
		gTaskL3bfdfContext.cur.wsTgvMatWgt += weight;

		//更新目标料斗重量 in NF2
		gTaskL3bfdfContext.hopper[line][0].hopperValue += rcv.sensorWsValue;

		//打印状态
		HCU_DEBUG_PRINT_FAT("L3BFDF: Normal basket, but not find right hopper, so throw into rubbish! Line = %d, HopperId = 0\n", line);

		//送往明确的满桶并通知界面
		inswgt_report.hopperId = 0;
		inswgt_report.groupId = 0;
		inswgt_report.validFlag = TRUE;
		inswgt_report.curWgt = 0;
		inswgt_report.curRatio = 0;
		inswgt_report.bufWgt = 0;
		inswgt_report.bufRatio = 0;
		hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_inswgt_bfdf_report(rcv.snrId, &inswgt_report);

		return SUCCESS;
	}

	//正确找到
	else{
		HCU_DEBUG_PRINT_INF("L3BFDF: Normal status, Hopperid=%d, Hopper status=%d\n", outHopperId, gTaskL3bfdfContext.hopper[line][outHopperId].hopperStatus);
		//先发送出料：出料的定时机制，如何处理？这是重入过程：使用状态机，不用定时器
		if (func_l3bfdf_new_ws_send_out_comb_out_message_by_pullin(line, outHopperId) == FALSE){
			gTaskL3bfdfContext.hopper[line][outHopperId].hopperStatus = HCU_L3BFDF_HOPPER_STATUS_VALID_ERR;
			HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Send out pullin message error!\n");
		}

		//UPDATE UI
		HCU_L3BFDF_TRIGGER_UI_STATUS_REPORT(rcv.snrId, HUICOBUS_CMDID_CUI_HCU2UIR_GENERAL_CMDVAL_DATA_VALID);

		//等待入料成功后，状态设置为出料，即可激活出料过程
		gTaskL3bfdfContext.hopper[line][outHopperId].hopperStatus = HCU_L3BFDF_HOPPER_STATUS_PULLIN_OUT;
		gTaskL3bfdfContext.hopper[line][outHopperId].hopperValue += weight;

		//动态更新调控数量
		double gap = (double)gTaskL3bfdfContext.group[line][gId].targetWeight - (double)gTaskL3bfdfContext.hopper[line][outHopperId].hopperValue;
		double avg = gTaskL3bfdfContext.group[line][gId].rangeAvg==0?0.01:gTaskL3bfdfContext.group[line][gId].rangeAvg;
		gTaskL3bfdfContext.hopper[line][outHopperId].matLackIndex = ((UINT32)(gap/avg))&0xFFFF;

		//欠一满不可能，不然该料斗已经成功进入出料状态了
		//这里可能有潜在的问题，需要再进一步测试和实验
		//核心问题与思路是：计算最大的倒推N，得到负数，但每一次并没有落入真正的n区间，而是因为小于0被归零了，这样，可能造成最终聚合成总重量，并不是N个，而是比N大
		//但这里的计算还是以N进行计算的．所以在第一次跨越0时，可能需要重新计算最大N值，而且还要重新将Index赋给N
		//每一次均自动刷新N进行迭代，这个风险就没有了
		//给一个发现BUG的地方：其实是可以不用的，但考虑到调测的需求，还是放在这儿
		//最恶劣的情况：还剩下1.9个，所以没法直接满，matLackIndex=1了，所以不能跟1进行比较，不然有虚警
//		if (gTaskL3bfdfContext.hopper[line][outHopperId].matLackIndex <= 0){
//			gTaskL3bfdfContext.hopper[line][outHopperId].hopperStatus = HCU_L3BFDF_HOPPER_STATUS_VALID_ERR;
//			HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Hopper Status error, Stream/HopperId=[%d/%d]!\n", line, outHopperId);
//		}

		//更新统计信息
		gTaskL3bfdfContext.cur.wsCombTimes++;
		gTaskL3bfdfContext.cur.wsTttTimes++;
		gTaskL3bfdfContext.cur.wsTttMatCnt++;
		gTaskL3bfdfContext.cur.wsTttMatWgt += weight;

		//打印状态
		HCU_DEBUG_PRINT_INF("L3BFDF: Find other normal basket! Line = %d, HopperId = %d\n", line, outHopperId);

		//送往明确的满桶并通知界面
		inswgt_report.hopperId = outHopperId;
		inswgt_report.groupId = gId;
		inswgt_report.validFlag = TRUE;
		inswgt_report.curWgt = gTaskL3bfdfContext.hopper[line][outHopperId].hopperValue;
		inswgt_report.curRatio = (UINT8)((float)gTaskL3bfdfContext.hopper[line][outHopperId].hopperValue/\
				(float)gTaskL3bfdfContext.group[line][gId].targetWeight*100.0)&0xFF;
		inswgt_report.bufWgt = gTaskL3bfdfContext.hopper[line][outHopperId].buferValue;
		inswgt_report.bufRatio = (UINT8)((float)gTaskL3bfdfContext.hopper[line][outHopperId].buferValue/\
				(float)gTaskL3bfdfContext.group[line][gId].bufWgtTarget*100.0)&0xFF;
		hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_inswgt_bfdf_report(rcv.snrId, &inswgt_report);

		return SUCCESS;
	}

	HCU_DEBUG_PRINT_FAT("L3BFDF: WS EVENT Processing, Can not get here, keep warning!");

	//返回
	return SUCCESS;
}


//组合出料反馈
OPSTAT fsm_l3bfdf_canitf_ws_comb_out_fb(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret=0;
	int line=0, boardId=0, locHopperId=0;

	HCU_MSG_RCV_CHECK_FOR_GEN_LOCAL(TASK_ID_L3BFDF, msg_struct_can_l3bfdf_ws_comb_out_fb_t);
	HCU_L3BFDF_INCOMING_MESSAGE_KEY_PARAMETERS_CHECK();
	locHopperId = rcv.hopperId;

	//printf("L3BFDF: LocalHopperId=%d, rcv.Hopper=%d, SnrId=%d\n", locHopperId, rcv.hopperId, rcv.snrId);

	if ((locHopperId <= 0) || (locHopperId >= HCU_SYSCFG_BFDF_HOPPER_NBR_MAX))
		HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Receive message error! Rcv HopperId = %d, localHopperId=%d\n", rcv.hopperId, locHopperId);

	//先处理错误情况
	if (rcv.validFlag == FALSE){
		//隔离该料斗
		gTaskL3bfdfContext.hopper[line][locHopperId].hopperStatus = HCU_L3BFDF_NODE_BOARD_STATUS_VALID_ERROR;
		return SUCCESS;
	}

	//Protection
	if ((gTaskL3bfdfContext.hopper[line][locHopperId].hopperStatus <= HCU_L3BFDF_HOPPER_STATUS_WORK_MIN) ||\
			(gTaskL3bfdfContext.hopper[line][locHopperId].hopperStatus >= HCU_L3BFDF_HOPPER_STATUS_WORK_MAX)){
		//Send error to UI
		HCU_L3BFDF_TRIGGER_UI_STATUS_REPORT(boardId, HUICOBUS_CMDID_CUI_HCU2UIR_GENERAL_CMDVAL_ERROR);
	}else{
		//PullIn confirm message
		StrHlcIe_cui_hcu2uir_pullin_confirm_t buf;
		memset(&buf, 0, sizeof(StrHlcIe_cui_hcu2uir_pullin_confirm_t));
		buf.validFlag = 1;
		buf.lineId = line;
		buf.hopperId = locHopperId;
		buf.groupId = gTaskL3bfdfContext.hopper[line][locHopperId].groupId;
		buf.curWeight = gTaskL3bfdfContext.hopper[line][locHopperId].hopperValue;
		buf.bufWeight = gTaskL3bfdfContext.hopper[line][locHopperId].buferValue;
		buf.curRatio = (UINT8)((float)gTaskL3bfdfContext.hopper[line][locHopperId].hopperValue/\
				(float)gTaskL3bfdfContext.group[line][buf.groupId].targetWeight*100.0)&0xFF;
		buf.bufRatio = (UINT8)((float)gTaskL3bfdfContext.hopper[line][locHopperId].buferValue/\
				(float)gTaskL3bfdfContext.group[line][buf.groupId].bufWgtTarget*100.0)&0xFF;
		hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_pullin_confirm(gTaskL3bfdfContext.configId, &buf);
	}

	//料斗重量更新：将最后一个物料增加到统计数据中
	//由于有可能出现多个物料进入同一个料斗的情况，如果一直要等待FB消息之后才更新，则可能出现下一次的计算不准确：为啥？因为下一次还是按照物料进桶之前为基数做的计算
	//设计方案只能如此：LastMat记录重量，如果失败了，则扣除，如果成功了，则不管。但这里又会出现N个证实-buffer深度的问题，对吧？
	//完整的进行证实的确可以，但也会遇到复杂度大大上升的问题！所以简单的方式是，一旦有一个物料在路上，则下一次不再给它分配。
	//如何判定该料斗还在物料回收状态？判定hopperLastMat是否归零。不再使用其他更为很杂的状态机去操控。
	//逻辑上，这属于单线程的HARQ机制
#if 0
	UINT16 gId = gTaskL3bfdfContext.hopper[line][locHopperId].groupId;
	if (gTaskL3bfdfContext.hopper[line][locHopperId].hopperStatus == HCU_L3BFDF_HOPPER_STATUS_BASKET_FULL){
		//设置料斗状态：操控余量控制数量
		gTaskL3bfdfContext.hopper[line][locHopperId].matLackIndex = gTaskL3bfdfContext.hopper[line][locHopperId].matLackNbr;
		//料斗实时重量刷新
		gTaskL3bfdfContext.hopper[line][locHopperId].hopperValue += gTaskL3bfdfContext.hopper[line][locHopperId].hopperLastMat;
		gTaskL3bfdfContext.hopper[line][locHopperId].hopperLastMat = 0;
	}
	else if ((gTaskL3bfdfContext.hopper[line][locHopperId].hopperStatus == HCU_L3BFDF_HOPPER_STATUS_BUF_CONT) ||\
			(gTaskL3bfdfContext.hopper[line][locHopperId].hopperStatus == HCU_L3BFDF_HOPPER_STATUS_BUF_FULL)){
		//料斗实时重量刷新
		gTaskL3bfdfContext.hopper[line][locHopperId].buferValue += gTaskL3bfdfContext.hopper[line][locHopperId].hopperLastMat;
		gTaskL3bfdfContext.hopper[line][locHopperId].hopperLastMat = 0;
		//动态更新调控数量
		double gap = (double)gTaskL3bfdfContext.group[line][gId].targetWeight - (double)gTaskL3bfdfContext.hopper[line][locHopperId].buferValue;
		double avg = gTaskL3bfdfContext.group[line][gId].rangeAvg==0?0.01:gTaskL3bfdfContext.group[line][gId].rangeAvg;
		gTaskL3bfdfContext.hopper[line][locHopperId].matLackIndex = ((UINT32)(gap/avg))&0xFFFF;
	}
	else if ((gTaskL3bfdfContext.hopper[line][locHopperId].hopperStatus == HCU_L3BFDF_HOPPER_STATUS_VALID) ||\
			(gTaskL3bfdfContext.hopper[line][locHopperId].hopperStatus == HCU_L3BFDF_HOPPER_STATUS_PULLIN_OUT)){
		//料斗实时重量刷新
		gTaskL3bfdfContext.hopper[line][locHopperId].hopperValue += gTaskL3bfdfContext.hopper[line][locHopperId].hopperLastMat;
		gTaskL3bfdfContext.hopper[line][locHopperId].hopperLastMat = 0;
		//动态更新调控数量
		double gap = (double)gTaskL3bfdfContext.group[line][gId].targetWeight - (double)gTaskL3bfdfContext.hopper[line][locHopperId].hopperValue;
		double avg = gTaskL3bfdfContext.group[line][gId].rangeAvg==0?0.01:gTaskL3bfdfContext.group[line][gId].rangeAvg;
		gTaskL3bfdfContext.hopper[line][locHopperId].matLackIndex = ((UINT32)(gap/avg))&0xFFFF;
	}
	else{
		gTaskL3bfdfContext.hopper[line][locHopperId].hopperLastMat = 0;
		HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Rcv COMB FB but relevant hopperId status error. Stream/HopperId/Status=%d/%d/%d!\n", line, locHopperId, gTaskL3bfdfContext.hopper[line][locHopperId].hopperStatus);
	}
#endif

	//打印二维码／条形码：二维码＋条形码的内容
#if 0
	char s[100];
	time_t lt;
	struct tm *cu;
	memset(s, 0, sizeof(s));
	//初始本地时间
	lt=time(NULL);
	cu = localtime(&lt);
	cu->tm_mon = cu->tm_mon + 1; //月份是从0-11的，+1是为了符合正常逻辑
	sprintf(s, "BOFENGZHINENG-%s-%5.2fg-%04d.%02d.%02d.%02d:%02d:%02d", gTaskL3bfdfContext.configName, (float)gTaskL3bfdfContext.wgtSnrPar.MaxAllowedWeight, \
			(UINT16)(1900+cu->tm_year), (UINT8)cu->tm_mon, (UINT8)cu->tm_mday, (UINT8)cu->tm_hour, (UINT8)cu->tm_min, (UINT8)cu->tm_sec);
	hcu_sps232_send_char_to_ext_printer(s, strlen(s));
#endif

	//返回
	return SUCCESS;
}

/*  几个假设：
 *  1) 篮筐清零，必然导致篮筐中的物料=0
 *  2) 篮筐进料的门被打开，buffer中的物料倒入篮筐
 *  3) buffer中的重量进入到篮筐
 *  4) 组合算法变得更加复杂，还需要考虑buffer的高度，但buffer的高度只是一个门限，最终策略还是以篮筐为准
 *
 */
OPSTAT fsm_l3bfdf_canitf_basket_clean_ind(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret=0;
	int line=0, boardId=0, locHopperId=0;

	HCU_MSG_RCV_CHECK_FOR_GEN_LOCAL(TASK_ID_L3BFDF, msg_struct_can_l3bfdf_basket_clean_ind_t);
	HCU_L3BFDF_INCOMING_MESSAGE_KEY_PARAMETERS_CHECK();
	locHopperId = rcv.hopperId;

	if ((locHopperId <= 0) || (locHopperId >= HCU_SYSCFG_BFDF_HOPPER_NBR_MAX))
		HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Receive message error!\n");

	//如果此时也条件允许，也产生出料：不太可能。一般情况下，只有BUF的深度=篮筐的深度，才有这个可能性。
	//另外，HCU_L3BFDF_HOPPER_STATUS_BASKET_FULL也表达不了这个，需要HCU_L3BFDF_HOPPER_STATUS_BUF_FULL以及两者深度相同才行。本项目中不存在。


	//本地的CallCell数据存储更新，表示一个完整的料箱搞完成了
	HcuSysMsgIeL3bfdfCallcellElement_t data;
	memset(&data, 0, sizeof(HcuSysMsgIeL3bfdfCallcellElement_t));
	data.sid = gTaskL3bfdfContext.sessionId;
	data.configId = gTaskL3bfdfContext.configId;
	data.groupId = gTaskL3bfdfContext.hopper[line][locHopperId].groupId;;
	data.groupRangeHigh = gTaskL3bfdfContext.group[line][data.groupId].rangeHigh;
	data.groupRangeLow = gTaskL3bfdfContext.group[line][data.groupId].rangeLow;
	data.targetWeight = gTaskL3bfdfContext.group[line][data.groupId].targetWeight;
	data.upLimitWeight = gTaskL3bfdfContext.group[line][data.groupId].targetUpLimit;
	data.hopperId = locHopperId;
	data.streamLine = line;
	data.actualWeight = gTaskL3bfdfContext.hopper[line][locHopperId].hopperValue;
	data.timestamp = time(0);
	strncpy(data.operator, gTaskL3bfdfContext.configName, sizeof(gTaskL3bfdfContext.configName)<sizeof(data.operator)?sizeof(gTaskL3bfdfContext.configName):sizeof(data.operator));
	dbi_HcuBfdf_callcell_save(&data);

	//99.9%的条件下，篮筐大于BUFFER，门自动打开，BUF中的物料自动掉落到篮筐，继续进行组合
	gTaskL3bfdfContext.hopper[line][locHopperId].hopperStatus = HCU_L3BFDF_HOPPER_STATUS_PULLIN_OUT;
	gTaskL3bfdfContext.hopper[line][locHopperId].hopperValue = gTaskL3bfdfContext.hopper[line][locHopperId].buferValue;
	gTaskL3bfdfContext.hopper[line][locHopperId].buferValue = 0;

	//通知界面：CallCell报告
	StrHlcIe_cui_hcu2uir_callcell_bfdf_report_t callcell;
	memset(&callcell, 0, sizeof(StrHlcIe_cui_hcu2uir_callcell_bfdf_report_t));
	int groupId = gTaskL3bfdfContext.hopper[line][locHopperId].groupId;
	callcell.validFlag = 1;
	callcell.lineId = line;
	callcell.hopperId = locHopperId;
	callcell.groupId = gTaskL3bfdfContext.hopper[line][locHopperId].groupId;
	callcell.curWgt = gTaskL3bfdfContext.hopper[line][locHopperId].hopperValue;
	callcell.bufWgt = gTaskL3bfdfContext.hopper[line][locHopperId].buferValue;
	callcell.curRatio = (UINT8)((float)gTaskL3bfdfContext.hopper[line][locHopperId].hopperValue/\
			(float)gTaskL3bfdfContext.group[line][groupId].targetWeight*100.0)&0xFF;
	callcell.bufRatio = (UINT8)((float)gTaskL3bfdfContext.hopper[line][locHopperId].buferValue/\
			(float)gTaskL3bfdfContext.group[line][groupId].bufWgtTarget*100.0)&0xFF;

	hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_callcell_bfdf_report(rcv.snrId, &callcell);

	//更新统计信息
	gTaskL3bfdfContext.cur.wsCallCellTimes++;
	gTaskL3bfdfContext.sessionId++;

	//返回
	return SUCCESS;
}

/***************************************************************************************************************************
 *
 * 　TIME_OUT处理过程
 *
 ***************************************************************************************************************************/
OPSTAT func_l3bfdf_time_out_sys_cfg_req_process(void)
{
	FsmSetState(TASK_ID_L3BFDF, FSM_STATE_L3BFDF_ACTIVED);
	HCU_L3BFDF_FEEDBACK_CTRL_RESP_MESSAGE(HCU_SYSMSG_BFDF_UICOMM_CMDID_CFG_START);

	//返回
	return SUCCESS;
}

OPSTAT func_l3bfdf_time_out_sys_resp_common_process(void)
{
	int i, j;
	for (j = 0; j< HCU_SYSCFG_BFDF_EQU_FLOW_NBR_MAX; j++){
		for (i = 1; i< HCU_SYSCFG_BFDF_NODE_BOARD_NBR_MAX; i++){
			if (gTaskL3bfdfContext.nodeDyn[j][i].respRcvFlag == FALSE)
				gTaskL3bfdfContext.nodeDyn[j][i].nodeStatus = HCU_L3BFDF_NODE_BOARD_STATUS_OFFLINE;
		}
	}

	//Response消息超时都强行退回到ACTIVE状态
	FsmSetState(TASK_ID_L3BFDF, FSM_STATE_L3BFDF_ACTIVED);
	return SUCCESS;
}

/***************************************************************************************************************************
 *
 * 　超时处理过程
 *
 ***************************************************************************************************************************/
//判定是否所有的CFG_RESP收齐了
bool func_l3bfdf_cacluate_sensor_cfg_start_rcv_complete(void)
{
	int i=0, j=0;
	for (i=0; i<HCU_SYSCFG_BFDF_EQU_FLOW_NBR_MAX; i++)
	{
		for (j=1; j<HCU_SYSCFG_BFDF_NODE_BOARD_NBR_MAX; j++)
		{
			if ((gTaskL3bfdfContext.nodeDyn[i][j].nodeStatus == HCU_L3BFDF_NODE_BOARD_STATUS_CFG_START_REQ) && (gTaskL3bfdfContext.nodeDyn[i][j].respRcvFlag == FALSE))
				return FALSE;
		}
	}

	//返回
	return TRUE;
}

bool func_l3bfdf_cacluate_sensor_suspend_rcv_complete(void)
{
	int i=0, j=0;
	for (i=0; i<HCU_SYSCFG_BFDF_EQU_FLOW_NBR_MAX; i++)
	{
		for (j=1; j<HCU_SYSCFG_BFDF_NODE_BOARD_NBR_MAX; j++)
		{
			if ((gTaskL3bfdfContext.nodeDyn[i][j].nodeStatus == HCU_L3BFDF_NODE_BOARD_STATUS_SUSPEND) && (gTaskL3bfdfContext.nodeDyn[i][j].respRcvFlag == FALSE))
				return FALSE;
		}
	}

	return TRUE;
}

bool func_l3bfdf_cacluate_sensor_resume_rcv_complete(void)
{
	int i=0, j=0;
	for (i=0; i<HCU_SYSCFG_BFDF_EQU_FLOW_NBR_MAX; i++)
	{
		for (j=1; j<HCU_SYSCFG_BFDF_NODE_BOARD_NBR_MAX; j++)
		{
			if ((gTaskL3bfdfContext.nodeDyn[i][j].nodeStatus == HCU_L3BFDF_NODE_BOARD_STATUS_RESUME) && (gTaskL3bfdfContext.nodeDyn[i][j].respRcvFlag == FALSE))
				return FALSE;
		}
	}

	return TRUE;
}

bool func_l3bfdf_cacluate_sensor_stop_rcv_complete(void)
{
	int i=0, j=0;
	for (i=0; i<HCU_SYSCFG_BFDF_EQU_FLOW_NBR_MAX; i++)
	{
		for (j=1; j<HCU_SYSCFG_BFDF_NODE_BOARD_NBR_MAX; j++)
		{
			if ((gTaskL3bfdfContext.nodeDyn[i][j].nodeStatus == HCU_L3BFDF_NODE_BOARD_STATUS_STOP) && (gTaskL3bfdfContext.nodeDyn[i][j].respRcvFlag == FALSE))
				return FALSE;
		}
	}

	return TRUE;
}

//判定各个板子的状态是否合法
//TRUE：全部都启动起来了
//FALSE：还有板子未启动完成
bool func_l3bfdf_is_there_any_board_not_yet_startup(void)
{
	int i = 0, j = 0;
	for (i = 0; i < HCU_L3BFDF_MAX_STREAM_LINE_ACTUAL; i++){
		for (j = 1; j < HCU_L3BFDF_MAX_IO_BOARD_NBR_ACTUAL; j++){
			if (gTaskL3bfdfContext.nodeDyn[i][j].nodeStatus < HCU_L3BFDF_NODE_BOARD_STATUS_STARTUP){
				return FALSE;
			}
		}
	}
	//全部启动完成
	return TRUE;
}

//Special TREATMENT for bitmap of boards
OPSTAT func_l3bfdf_send_out_suspend_message_to_all(void)
{
	int i = 0, j = 0, boardId = 0, total = 0;
	char s[200], tmp[20];
	msg_struct_sui_suspend_req_t snd;
	memset(&snd, 0, sizeof(msg_struct_sui_suspend_req_t));
	HCU_L3BFDF_FILL_ALL_BOARD_BITMAP(HCU_L3BFDF_NODE_BOARD_STATUS_SUSPEND);
	HCU_L3BFDF_PRINT_ALL_BOARD_BITMAP();
	snd.length = sizeof(msg_struct_sui_suspend_req_t);
	HCU_MSG_SEND_GENERNAL_PROCESS(MSG_ID_SUI_SUSPEND_REQ, TASK_ID_CANALPHA, TASK_ID_L3BFDF);
	hcu_timer_start(TASK_ID_L3BFDF, HCU_TIMERID_WITH_DUR(TIMER_ID_1S_L3BFDF_SUSPEND_WAIT_FB), TIMER_TYPE_ONE_TIME, TIMER_RESOLUTION_1S);
	FsmSetState(TASK_ID_L3BFDF, FSM_STATE_L3BFDF_SUSPEND);
	return SUCCESS;
}

OPSTAT func_l3bfdf_send_out_resume_message_to_all(void)
{
	int i = 0, j = 0, boardId = 0, total = 0;
	char s[200], tmp[20];
	msg_struct_sui_resume_req_t snd;
	memset(&snd, 0, sizeof(msg_struct_sui_resume_req_t));
	HCU_L3BFDF_FILL_ALL_BOARD_BITMAP(HCU_L3BFDF_NODE_BOARD_STATUS_RESUME);
	HCU_L3BFDF_PRINT_ALL_BOARD_BITMAP();
	snd.length = sizeof(msg_struct_sui_resume_req_t);
	HCU_MSG_SEND_GENERNAL_PROCESS(MSG_ID_SUI_RESUME_REQ, TASK_ID_CANALPHA, TASK_ID_L3BFDF);
	hcu_timer_start(TASK_ID_L3BFDF, HCU_TIMERID_WITH_DUR(TIMER_ID_1S_L3BFDF_RESUME_WAIT_FB), TIMER_TYPE_ONE_TIME, TIMER_RESOLUTION_1S);
	return SUCCESS;
}

OPSTAT func_l3bfdf_send_out_stop_message_to_all(void)
{
	int i = 0, j = 0, boardId = 0, total = 0;
	char s[200], tmp[20];
	msg_struct_sui_stop_req_t snd;
	memset(&snd, 0, sizeof(msg_struct_sui_stop_req_t));
	HCU_L3BFDF_FILL_ALL_BOARD_BITMAP(HCU_L3BFDF_NODE_BOARD_STATUS_STOP);
	HCU_L3BFDF_PRINT_ALL_BOARD_BITMAP();
	snd.length = sizeof(msg_struct_sui_stop_req_t);
	HCU_MSG_SEND_GENERNAL_PROCESS(MSG_ID_SUI_STOP_REQ, TASK_ID_CANALPHA, TASK_ID_L3BFDF);
	hcu_timer_start(TASK_ID_L3BFDF, HCU_TIMERID_WITH_DUR(TIMER_ID_1S_L3BFDF_STOP_WAIT_FB), TIMER_TYPE_ONE_TIME, TIMER_RESOLUTION_1S);
	return SUCCESS;
}

//不改变状态：因为可能是在业务态来的某个板子重启，所以状态不变．
OPSTAT func_l3bfdf_send_out_cfg_start_message_to_all(void)
{
	int i = 0, j = 0, boardId = 0, total = 0;
	char s[200], tmp[20];
	msg_struct_l3bfdf_can_sys_cfg_req_t snd;
	memset(&snd, 0, sizeof(msg_struct_l3bfdf_can_sys_cfg_req_t));
	HCU_L3BFDF_FILL_ALL_BOARD_BITMAP(HCU_L3BFDF_NODE_BOARD_STATUS_CFG_START_REQ);
	memcpy(&snd.wgtSnrPar, &gTaskL3bfdfContext.wgtSnrPar, sizeof(gTaskL3bfdfContextWeightSensorParamaters_t));
	memcpy(&snd.motMainPar, &gTaskL3bfdfContext.motMainPar, sizeof(gTaskL3bfdfContextMotorControlParamaters_t));
	memcpy(&snd.motSecondPar, &gTaskL3bfdfContext.motSecondPar, sizeof(gTaskL3bfdfContextMotorControlParamaters_t));
	memcpy(&snd.actionCtrlPar, &gTaskL3bfdfContext.actionCtrlPar, sizeof(gTaskL3bfdfContextActionControlParamaters_t));
	HCU_L3BFDF_PRINT_ALL_BOARD_BITMAP();
	snd.length = sizeof(msg_struct_l3bfdf_can_sys_cfg_req_t);
	HCU_MSG_SEND_GENERNAL_PROCESS(MSG_ID_L3BFDF_CAN_SYS_CFG_REQ, TASK_ID_CANALPHA, TASK_ID_L3BFDF);
	hcu_timer_start(TASK_ID_L3BFDF, HCU_TIMERID_WITH_DUR(TIMER_ID_1S_L3BFDF_CFG_START_WAIT_FB), TIMER_TYPE_ONE_TIME, TIMER_RESOLUTION_1S);
	FsmSetState(TASK_ID_L3BFDF, FSM_STATE_L3BFDF_ACTIVED);
	return SUCCESS;
}

//判定是否在规定范围内
bool func_l3bfdf_judge_material_in_official_range(INT32 weight, UINT8 line)
{
	int i = 0;
	UINT16 gidMin = 1;
	UINT16 gidMax = gTaskL3bfdfContext.totalGroupNbr[line];

	for (i = gidMin; i<=gidMax; i++){
		if ((weight >= gTaskL3bfdfContext.group[line][i].rangeLow) && (weight <= gTaskL3bfdfContext.group[line][i].rangeHigh))
			return TRUE;
	}

	//不在范围内
	return FALSE;
}




/***************************************************************************************************************************
 *
 * 　核心本地算法
 *
 ***************************************************************************************************************************/

//streamId是0开始计数的
//groupId是从1开始计数的
bool func_l3bfdf_group_allocation(UINT8 streamId, UINT16 nbrGroup)
{
	int i = 0;
	//入参检查
	if ((streamId >= HCU_SYSCFG_BFDF_EQU_FLOW_NBR_MAX) || (nbrGroup <= 0) || (nbrGroup >= HCU_SYSCFG_BFDF_HOPPER_NBR_MAX))
		return FALSE;

	gTaskL3bfdfContext.totalGroupNbr[streamId] = nbrGroup;

	//循环赋值
	for (i=0; i<HCU_SYSCFG_BFDF_HOPPER_NBR_MAX; i++){
		gTaskL3bfdfContext.group[streamId][i].groupId = i;
		if (i<=nbrGroup) gTaskL3bfdfContext.group[streamId][i].groupStatus = HCU_L3BFDF_GROUP_STATUS_ACTIVE;
		else gTaskL3bfdfContext.group[streamId][i].groupStatus = HCU_L3BFDF_GROUP_STATUS_DISABLE;
	}
	return TRUE;
}

//初始化所有料斗状态
bool func_l3bfdf_hopper_state_set_init(UINT8 streamId)
{
	int i = 0;
	//入参检查
	if (streamId >= HCU_SYSCFG_BFDF_EQU_FLOW_NBR_MAX)
		return FALSE;

	//循环赋值
	for (i=1; i< HCU_L3BFDF_MAX_HOOPER_PER_LINE_ACTUAL; i++){
		gTaskL3bfdfContext.hopper[streamId][i].groupId = 0;
		gTaskL3bfdfContext.hopper[streamId][i].hopperId = i;
		gTaskL3bfdfContext.hopper[streamId][i].hopperStatus = HCU_L3BFDF_HOPPER_STATUS_INIT_UNALLOC;
	}

	//#0号HOPPER是固定留给垃圾料斗的
	gTaskL3bfdfContext.hopper[streamId][0].groupId = 0;
	gTaskL3bfdfContext.hopper[streamId][0].hopperId = 0;
	gTaskL3bfdfContext.hopper[streamId][0].hopperStatus = HCU_L3BFDF_HOPPER_STATUS_INIT_ALLOC;
	gTaskL3bfdfContext.hopper[streamId][0].nextHopperId = 0;
	gTaskL3bfdfContext.hopper[streamId][0].preHopperId = 0;
	gTaskL3bfdfContext.group[streamId][0].totalHopperNbr = 1;
	gTaskL3bfdfContext.group[streamId][0].firstHopperId = 0;
	gTaskL3bfdfContext.group[streamId][0].fillHopperId = 0;

	//其他未分配的料斗，均为离线状态
	for (i=HCU_L3BFDF_MAX_HOOPER_PER_LINE_ACTUAL; i<HCU_SYSCFG_BFDF_HOPPER_NBR_MAX; i++){
		gTaskL3bfdfContext.hopper[streamId][i].hopperStatus = HCU_L3BFDF_HOPPER_STATUS_OFFLINE;
	}

	return TRUE;
}

//设置所有料斗状态
bool func_l3bfdf_hopper_state_set_valid(UINT8 streamId)
{
	int i = 0;
	UINT16 gid = 0;
	double targetWgt = 0;
	double avgWgt=0;
	double sigWgt=0;
	UINT16 nbrDeep=0;

	//入参检查
	if (streamId >= HCU_SYSCFG_BFDF_EQU_FLOW_NBR_MAX){
		HCU_DEBUG_PRINT_CRT("L3BFDF: Stream=%d\n", streamId);
		return FALSE;
	}

	//0号特别
	gTaskL3bfdfContext.hopper[streamId][0].hopperStatus = HCU_L3BFDF_HOPPER_STATUS_VALID;

	//状态设置, 初始化物料数量
	for (i=1; i<HCU_L3BFDF_MAX_HOOPER_PER_LINE_ACTUAL; i++){
		gTaskL3bfdfContext.hopper[streamId][i].hopperStatus = HCU_L3BFDF_HOPPER_STATUS_VALID;

		gid = gTaskL3bfdfContext.hopper[streamId][i].groupId;
		targetWgt = gTaskL3bfdfContext.group[streamId][gid].targetWeight + gTaskL3bfdfContext.group[streamId][gid].targetUpLimit/2.0;
		avgWgt = gTaskL3bfdfContext.group[streamId][gid].rangeAvg;
		sigWgt = gTaskL3bfdfContext.group[streamId][gid].rangeSigma;
		gTaskL3bfdfContext.hopper[streamId][i].matLackNbr = (UINT32)(targetWgt/(avgWgt==0?0.01:avgWgt))&0xFFFF;
		nbrDeep = (UINT32)(avgWgt/(sigWgt==0?0.01:sigWgt))&0xFFFF;
		if ((sigWgt == 0) || (avgWgt == 0)){
			HCU_DEBUG_PRINT_CRT("L3BFDF: Zero sigWgt/avgWgt, Stream/Gid/Hopper=[%d/%d/%d], RangAvg/Sig/Min/Max=[%f/%f/%f/%f]\n", streamId, gid, i, avgWgt, sigWgt, \
					gTaskL3bfdfContext.group[streamId][gid].rangeLow, gTaskL3bfdfContext.group[streamId][gid].rangeHigh);
			return FALSE;
		}

		//这里的目标，还要更好的进行比较
		if (gTaskL3bfdfContext.hopper[streamId][i].matLackNbr <= nbrDeep){
			HCU_DEBUG_PRINT_CRT("L3BFDF: Stream/Hopper=[%d/%d], LackNbr=%d, nbrDeep=%d\n", streamId, i, gTaskL3bfdfContext.hopper[streamId][i].matLackNbr, nbrDeep);
			return FALSE;
		}

		//初始化控制参数
		gTaskL3bfdfContext.hopper[streamId][i].matLackIndex = gTaskL3bfdfContext.hopper[streamId][i].matLackNbr;
	}

	return TRUE;
}

//设置单个料斗状态
bool func_l3bfdf_hopper_update_N_coef(UINT8 streamId, UINT16 hid)
{
	UINT16 gid = 0;
	double targetWgt = 0;
	double avgWgt=0;

	//入参检查
	if ((streamId >= HCU_SYSCFG_BFDF_EQU_FLOW_NBR_MAX) || (hid==0) || (hid >= HCU_SYSCFG_BFDF_HOPPER_NBR_MAX))
		return FALSE;

	//状态设置, 初始化物料数量
	gid = gTaskL3bfdfContext.hopper[streamId][hid].groupId;
	targetWgt = gTaskL3bfdfContext.group[streamId][gid].targetWeight + gTaskL3bfdfContext.group[streamId][gid].targetUpLimit - gTaskL3bfdfContext.hopper[streamId][hid].hopperValue;
	avgWgt = gTaskL3bfdfContext.group[streamId][gid].rangeAvg;
	if (avgWgt==0){
		gTaskL3bfdfContext.hopper[streamId][hid].matLackIndex = (UINT32)(targetWgt)&0xFFFF;
		HcuErrorPrint("L3BFDF: Taking care, data set error!\n");
	}
	else
		gTaskL3bfdfContext.hopper[streamId][hid].matLackIndex = (UINT32)((double)targetWgt/(double)avgWgt)&0xFFFF;
	if (avgWgt == 0) HcuErrorPrint("L3BFDF: Max Weight caculation error!\n");

/*
	HcuDebugPrint("L3BFDF: S/G/H/High/Low=%d/%d/%d/%f/%f, Max/Min=%d/%d\n", streamId, gid, hid, \
			gTaskL3bfdfContext.group[streamId][gid].rangeHigh, gTaskL3bfdfContext.group[streamId][gid].rangeLow, \
			gTaskL3bfdfContext.hopper[streamId][hid].matLackIndexMax, gTaskL3bfdfContext.hopper[streamId][hid].matLackIndexMin);
*/

	return TRUE;
}

//正常在某一个GroupId的末尾增加一个hopperId
//GroupId/HopperId都不得等于0
//整个增加函数采用强行格式化模式，而不是检测返回错误模式，从而简化和提升可靠性．数据不一致之后还有可能纠偏回来．
bool func_l3bfdf_hopper_add_by_tail(UINT8 streamId, UINT16 groupId, UINT16 hopperNewId)
{
	UINT16 fHopper = 0;

	//入参检查：注意起点和终点
	if ((streamId >= HCU_SYSCFG_BFDF_EQU_FLOW_NBR_MAX) || (groupId >=HCU_SYSCFG_BFDF_HOPPER_NBR_MAX) || (hopperNewId >=HCU_SYSCFG_BFDF_HOPPER_NBR_MAX))
		return FALSE;

	//强制增加GroupId到最大数量计量单位上
	if (groupId > gTaskL3bfdfContext.totalGroupNbr[streamId]){
		HCU_DEBUG_PRINT_FAT("ERROR: Add group by tail, groupid = %d\n", groupId);
		gTaskL3bfdfContext.totalGroupNbr[streamId] = groupId;
	}

	//强制修改该GroupId数据
	gTaskL3bfdfContext.group[streamId][groupId].groupId = groupId;

	//强制修改该HopperId的GroupId
	fHopper = gTaskL3bfdfContext.group[streamId][groupId].firstHopperId;
	if (fHopper >= HCU_SYSCFG_BFDF_HOPPER_NBR_MAX) return FAILURE;

	//如果是第一次填入：指针指向自己
	if (fHopper == 0){
		gTaskL3bfdfContext.group[streamId][groupId].firstHopperId = hopperNewId;
		gTaskL3bfdfContext.group[streamId][groupId].fillHopperId = hopperNewId;
		gTaskL3bfdfContext.hopper[streamId][hopperNewId].nextHopperId = hopperNewId;
		gTaskL3bfdfContext.hopper[streamId][hopperNewId].preHopperId = hopperNewId;
		gTaskL3bfdfContext.hopper[streamId][hopperNewId].groupId = groupId;
		gTaskL3bfdfContext.hopper[streamId][hopperNewId].hopperStatus = HCU_L3BFDF_HOPPER_STATUS_INIT_ALLOC;
	}
	else{
		UINT16 tailHopper = gTaskL3bfdfContext.hopper[streamId][fHopper].preHopperId;
		//尾巴指针
		gTaskL3bfdfContext.hopper[streamId][tailHopper].nextHopperId = hopperNewId;
		//新增指针
		gTaskL3bfdfContext.hopper[streamId][hopperNewId].nextHopperId = fHopper;
		gTaskL3bfdfContext.hopper[streamId][hopperNewId].preHopperId = tailHopper;
		gTaskL3bfdfContext.hopper[streamId][hopperNewId].groupId = groupId;
		gTaskL3bfdfContext.hopper[streamId][hopperNewId].hopperStatus = HCU_L3BFDF_HOPPER_STATUS_INIT_ALLOC;
		//首指针
		gTaskL3bfdfContext.hopper[streamId][fHopper].preHopperId = hopperNewId;
	}
	gTaskL3bfdfContext.group[streamId][groupId].totalHopperNbr++;
	//HcuDebugPrint("L3BFDF: Allocation by Tail, S/G/H = %d/%d/%d, Status=%d, Next/Pre=%d/%d\n", streamId, groupId, hopperNewId, gTaskL3bfdfContext.hopper[streamId][hopperNewId].hopperStatus, gTaskL3bfdfContext.hopper[streamId][hopperNewId].nextHopperId, gTaskL3bfdfContext.hopper[streamId][hopperNewId].preHopperId);

	return TRUE;
}

bool func_l3bfdf_hopper_add_by_group_element(UINT8 streamId, UINT16 groupId, UINT16 nbrHopper)
{
	//入参检查：注意起点和终点
	if ((streamId >= HCU_SYSCFG_BFDF_EQU_FLOW_NBR_MAX) || (groupId >= HCU_SYSCFG_BFDF_HOPPER_NBR_MAX) || (nbrHopper <= 0)|| (nbrHopper >= HCU_SYSCFG_BFDF_HOPPER_NBR_MAX))
		return FALSE;

	//强制增加GroupId到最大数量计量单位上
	if (groupId > gTaskL3bfdfContext.totalGroupNbr[streamId]){
		HCU_DEBUG_PRINT_FAT("ERROR: Add group by group, groupid = %d\n", groupId);
		gTaskL3bfdfContext.totalGroupNbr[streamId] = groupId;
	}

	int i=0;
	UINT16 tmpHopper = 0;
	int cnt = 0;
	for (i=0; i<nbrHopper; i++){
		cnt = 0;
		//HCU_SYSCFG_BFDF_HOPPER_NBR_MAX本身就有+1的特性在里面
		tmpHopper = rand()%(HCU_SYSCFG_BFDF_HOPPER_NBR_MAX);
		while ((cnt < 1000) && (gTaskL3bfdfContext.hopper[streamId][tmpHopper].hopperStatus != HCU_L3BFDF_HOPPER_STATUS_INIT_UNALLOC)){
			cnt++;
			tmpHopper = rand()%(HCU_SYSCFG_BFDF_HOPPER_NBR_MAX);
		}
		if (cnt >= 1000) return FALSE;
		//HcuDebugPrint("L3BFDF: streamId = %d, GroupId = %d, Hopper Allocation = %d\n", streamId, groupId, tmpHopper);
		if (func_l3bfdf_hopper_add_by_tail(streamId, groupId, tmpHopper) == FALSE) return FALSE;
	}

	return TRUE;
}

//这个正在使用的分布
bool func_l3bfdf_hopper_add_by_grp_in_average_distribution(UINT8 streamId, UINT16 nbrGroup)
{
	//入参检查：注意起点和终点
	if ((streamId >= HCU_SYSCFG_BFDF_EQU_FLOW_NBR_MAX) || (nbrGroup >= HCU_SYSCFG_BFDF_HOPPER_NBR_MAX))
		return FALSE;

	//强制增加GroupId到最大数量计量单位上
	if (nbrGroup > gTaskL3bfdfContext.totalGroupNbr[streamId]){
		HCU_DEBUG_PRINT_FAT("ERROR: Add group by average distribution, groupid = %d\n", nbrGroup);
		gTaskL3bfdfContext.totalGroupNbr[streamId] = nbrGroup;
	}

	int i=0;
	//int cnt = HCU_SYSCFG_BFDF_HOPPER_NBR_MAX/nbrGroup;
	int cnt = HCU_L3BFDF_MAX_HOOPER_PER_LINE_ACTUAL/nbrGroup;
	for (i=1; i<nbrGroup; i++){
		if (func_l3bfdf_hopper_add_by_group_element(streamId, i, cnt) == FALSE) return FALSE;
		//HcuDebugPrint("L3BFDF: GroupId = %d, Nbr of Hopper = %d\n", i, cnt);
	}
	cnt = HCU_L3BFDF_MAX_HOOPER_PER_LINE_ACTUAL - cnt * (nbrGroup-1) -1; //第0#是固定的垃圾桶
	if (func_l3bfdf_hopper_add_by_group_element(streamId, nbrGroup, cnt) == FALSE) return FALSE;
	//HcuDebugPrint("L3BFDF: GroupId = %d, Nbr of Hopper = %d\n", nbrGroup, cnt);

	return TRUE;
}

//这个分布比较复杂，暂时没用的
bool func_l3bfdf_hopper_add_by_grp_in_normal_distribution(UINT8 streamId, UINT16 nbrGroup)
{
	UINT16 cnt[HCU_SYSCFG_BFDF_HOPPER_NBR_MAX];
	int i=0;

	//入参检查：注意起点和终点
	if ((streamId >= HCU_SYSCFG_BFDF_EQU_FLOW_NBR_MAX) || (nbrGroup >= HCU_SYSCFG_BFDF_HOPPER_NBR_MAX))
		return FALSE;

	//强制增加GroupId到最大数量计量单位上
	if (nbrGroup > gTaskL3bfdfContext.totalGroupNbr[streamId]){
		HCU_DEBUG_PRINT_FAT("ERROR: Group allocation by group in normal distribution, groupd = %d\n", nbrGroup);
		gTaskL3bfdfContext.totalGroupNbr[streamId] = nbrGroup;
	}

	//斜线分布
	for (i = 0; i< HCU_SYSCFG_BFDF_HOPPER_NBR_MAX; i++){
		cnt[i] = 0;
	}
	for (i = 1; i< nbrGroup/2; i++){
		cnt[i] = i;
	}
	for (i = nbrGroup/2; i< nbrGroup; i++){
		cnt[i] = nbrGroup  - i;
	}
	cnt[nbrGroup] = 1;
	UINT32 total = 0;
	for (i = 1; i<= nbrGroup; i++){
		total += cnt[i];
	}
	for (i = 1; i<= nbrGroup; i++){
		cnt[i] = (cnt[i] * HCU_SYSCFG_BFDF_HOPPER_NBR_MAX)/total;
	}
	total = 0;
	for (i = 1; i<= nbrGroup; i++){
		total += cnt[i];
		//HcuDebugPrint("TEST: Cnt[%d] = %d, total = %d\n", i, cnt[i], total);
	}
	cnt[nbrGroup/2] = HCU_SYSCFG_BFDF_HOPPER_NBR_MAX - 1 - (total - cnt[nbrGroup/2]);
	//HcuDebugPrint("TEST: Cnt[%d] = %d\n", nbrGroup/2, cnt[nbrGroup/2]);

	//异常情况的处理
	if (nbrGroup == 1) cnt[1] = HCU_SYSCFG_BFDF_HOPPER_NBR_MAX - 1;

	//循环调用
	for (i=1; i<=nbrGroup; i++){
		if (func_l3bfdf_hopper_add_by_group_element(streamId, i, cnt[i]) == FALSE) return FALSE;
	}

	return TRUE;
}

bool func_l3bfdf_hopper_remove_by_tail(UINT8 streamId, UINT16 groupId)
{
	UINT16 fHopper = 0;

	//入参检查：注意起点和终点
	if ((streamId >= HCU_SYSCFG_BFDF_EQU_FLOW_NBR_MAX) || (groupId >= HCU_SYSCFG_BFDF_HOPPER_NBR_MAX))
		return FALSE;

	//GroupId
	if (groupId > gTaskL3bfdfContext.totalGroupNbr[streamId]) return FALSE;

	//HopperId -> GroupId
	if (gTaskL3bfdfContext.group[streamId][groupId].groupId != groupId) return FALSE;

	//First HopperId
	fHopper = gTaskL3bfdfContext.group[streamId][groupId].firstHopperId;
	if ((fHopper <= 0) || (fHopper >= HCU_SYSCFG_BFDF_HOPPER_NBR_MAX)) return FALSE;

	//Hopper总数
	if ((gTaskL3bfdfContext.group[streamId][groupId].totalHopperNbr == 0) || (gTaskL3bfdfContext.group[streamId][groupId].totalHopperNbr >= HCU_SYSCFG_BFDF_HOPPER_NBR_MAX)) return FALSE;

	//最后一个指针
	if (gTaskL3bfdfContext.group[streamId][groupId].totalHopperNbr == 1){
		gTaskL3bfdfContext.group[streamId][groupId].totalHopperNbr = 0;
		gTaskL3bfdfContext.group[streamId][groupId].firstHopperId = 0;
		gTaskL3bfdfContext.group[streamId][groupId].fillHopperId = 0;
		gTaskL3bfdfContext.hopper[streamId][fHopper].groupId = 0;
		gTaskL3bfdfContext.hopper[streamId][fHopper].nextHopperId = 0;
		gTaskL3bfdfContext.hopper[streamId][fHopper].preHopperId = 0;
		gTaskL3bfdfContext.hopper[streamId][fHopper].hopperStatus = HCU_L3BFDF_HOPPER_STATUS_INIT_UNALLOC;
	}
	else{
		UINT16 tailHooper = gTaskL3bfdfContext.hopper[streamId][fHopper].preHopperId;
		UINT16 tailHooper2 = gTaskL3bfdfContext.hopper[streamId][tailHooper].preHopperId;
		//尾巴指针清零
		gTaskL3bfdfContext.hopper[streamId][tailHooper].nextHopperId = 0;
		gTaskL3bfdfContext.hopper[streamId][tailHooper].preHopperId = 0;
		gTaskL3bfdfContext.hopper[streamId][tailHooper].groupId = 0;
		gTaskL3bfdfContext.hopper[streamId][tailHooper].hopperStatus = HCU_L3BFDF_HOPPER_STATUS_INIT_UNALLOC;
		//尾巴前指针
		gTaskL3bfdfContext.hopper[streamId][tailHooper2].nextHopperId = fHopper;
		//首指针
		gTaskL3bfdfContext.hopper[streamId][fHopper].preHopperId = tailHooper2;
		//数量更新
		gTaskL3bfdfContext.group[streamId][groupId].totalHopperNbr--;
	}

	return TRUE;
}

//不存在GroupId中是空的情况，因为hopperId必须要从GroupId中找到，不然就算错误
bool func_l3bfdf_hopper_insert_by_middle(UINT8 streamId, UINT16 groupId, UINT16 hopperId, UINT16 hopperNewId)
{
	UINT16 fHopper = 0;

	//入参检查：注意起点和终点
	if ((streamId >= HCU_SYSCFG_BFDF_EQU_FLOW_NBR_MAX) || (groupId >= HCU_SYSCFG_BFDF_HOPPER_NBR_MAX) || (hopperId >= HCU_SYSCFG_BFDF_HOPPER_NBR_MAX) || (hopperNewId >= HCU_SYSCFG_BFDF_HOPPER_NBR_MAX))
		return FALSE;

	//强制增加GroupId到最大数量计量单位上
	if (groupId > gTaskL3bfdfContext.totalGroupNbr[streamId]){
		HCU_DEBUG_PRINT_FAT("ERROR: func_l3bfdf_hopper_insert_by_middle, groupd = %d\n", groupId);
		gTaskL3bfdfContext.totalGroupNbr[streamId] = groupId;
	}

	//强制修改该GroupId数据
	gTaskL3bfdfContext.group[streamId][groupId].groupId = groupId;

	//强制修改该HopperId的GroupId
	fHopper = gTaskL3bfdfContext.group[streamId][groupId].firstHopperId;
	if ((fHopper == 0) || (fHopper >= HCU_SYSCFG_BFDF_HOPPER_NBR_MAX)) return FAILURE;

	//遍历到hopperId
	UINT16 curHopper = fHopper;
	UINT16 cnt = 0;
	while(curHopper != hopperId){
		cnt++;
		if (cnt >= HCU_SYSCFG_BFDF_HOPPER_NBR_MAX) return FALSE;
		curHopper = gTaskL3bfdfContext.hopper[streamId][curHopper].nextHopperId;
	}

	//添加到curHopper后面
	UINT16 nextHopper = gTaskL3bfdfContext.hopper[streamId][curHopper].nextHopperId;
	gTaskL3bfdfContext.hopper[streamId][curHopper].nextHopperId = hopperNewId;
	gTaskL3bfdfContext.hopper[streamId][hopperNewId].nextHopperId = nextHopper;
	gTaskL3bfdfContext.hopper[streamId][hopperNewId].preHopperId = curHopper;
	gTaskL3bfdfContext.hopper[streamId][hopperNewId].groupId = groupId;
	gTaskL3bfdfContext.hopper[streamId][hopperNewId].hopperStatus = HCU_L3BFDF_HOPPER_STATUS_INIT_ALLOC;
	gTaskL3bfdfContext.hopper[streamId][nextHopper].preHopperId = hopperNewId;

	gTaskL3bfdfContext.group[streamId][groupId].totalHopperNbr++;
	return TRUE;
}

bool func_l3bfdf_hopper_del_by_middle(UINT8 streamId, UINT16 groupId, UINT16 hopperId)
{
	UINT16 fHopper = 0;

	//入参检查：注意起点和终点
	if ((streamId >= HCU_SYSCFG_BFDF_EQU_FLOW_NBR_MAX) || (groupId >= HCU_SYSCFG_BFDF_HOPPER_NBR_MAX) || (hopperId >= HCU_SYSCFG_BFDF_HOPPER_NBR_MAX))
		return FALSE;

	//GroupId
	if (groupId > gTaskL3bfdfContext.totalGroupNbr[streamId]) return FALSE;

	//HopperId -> GroupId
	if (gTaskL3bfdfContext.group[streamId][groupId].groupId != groupId) return FALSE;

	//First HopperId
	fHopper = gTaskL3bfdfContext.group[streamId][groupId].firstHopperId;
	if ((fHopper == 0) || (fHopper >= HCU_SYSCFG_BFDF_HOPPER_NBR_MAX)) return FALSE;

	//Hopper总数
	if ((gTaskL3bfdfContext.group[streamId][groupId].totalHopperNbr == 0) || (gTaskL3bfdfContext.group[streamId][groupId].totalHopperNbr >= HCU_SYSCFG_BFDF_HOPPER_NBR_MAX)) return FALSE;

	//遍历到hopperId
	UINT16 curHopper = fHopper;
	UINT16 cnt = 0;
	while(curHopper != hopperId){
		cnt++;
		if (cnt >= HCU_SYSCFG_BFDF_HOPPER_NBR_MAX) return FALSE;
		curHopper = gTaskL3bfdfContext.hopper[streamId][curHopper].nextHopperId;
	}

	//最后一个指针
	if (gTaskL3bfdfContext.group[streamId][groupId].totalHopperNbr == 1){
		gTaskL3bfdfContext.group[streamId][groupId].totalHopperNbr = 0;
		gTaskL3bfdfContext.group[streamId][groupId].firstHopperId = 0;
		gTaskL3bfdfContext.group[streamId][groupId].fillHopperId = 0;
		gTaskL3bfdfContext.hopper[streamId][fHopper].groupId = 0;
		gTaskL3bfdfContext.hopper[streamId][fHopper].nextHopperId = 0;
		gTaskL3bfdfContext.hopper[streamId][fHopper].preHopperId = 0;
		gTaskL3bfdfContext.hopper[streamId][fHopper].hopperStatus = HCU_L3BFDF_HOPPER_STATUS_INIT_UNALLOC;
	}
	else{
		UINT16 preHopper = gTaskL3bfdfContext.hopper[streamId][curHopper].preHopperId;
		UINT16 nextHopper = gTaskL3bfdfContext.hopper[streamId][curHopper].nextHopperId;
		//前一个，后一个
		gTaskL3bfdfContext.hopper[streamId][preHopper].nextHopperId = nextHopper;
		gTaskL3bfdfContext.hopper[streamId][nextHopper].preHopperId = preHopper;
		//本身清理
		gTaskL3bfdfContext.hopper[streamId][curHopper].nextHopperId = 0;
		gTaskL3bfdfContext.hopper[streamId][curHopper].preHopperId = 0;
		gTaskL3bfdfContext.hopper[streamId][curHopper].groupId = 0;
		gTaskL3bfdfContext.hopper[streamId][curHopper].hopperStatus = HCU_L3BFDF_HOPPER_STATUS_INIT_UNALLOC;

		//数量更新
		gTaskL3bfdfContext.group[streamId][groupId].totalHopperNbr--;
	}

	return TRUE;
}

//按照实际数量进行AUDIT
int func_l3bfdf_hopper_dual_chain_audit(void)
{
	UINT8  streamId = 0;
	UINT16 gid = 0;
	UINT16 fHopper=0, nextHopper=0, tmpHopper=0, preHopper=0;
	UINT16 tmp=0;
	int cnt = 0;

	//STREAM
	for (streamId=0; streamId<HCU_L3BFDF_MAX_STREAM_LINE_ACTUAL; streamId++)
	{
		if (gTaskL3bfdfContext.totalGroupNbr[streamId] >= HCU_SYSCFG_BFDF_HOPPER_NBR_MAX) return -1;
		if (gTaskL3bfdfContext.totalGroupNbr[streamId] == 0) continue;

		//检查总量
		tmp=0;
		for (gid = 0; gid <= gTaskL3bfdfContext.totalGroupNbr[streamId]; gid++)
		{
			if (gTaskL3bfdfContext.group[streamId][gid].totalHopperNbr >= HCU_SYSCFG_BFDF_HOPPER_NBR_MAX) return -2;
			if (gTaskL3bfdfContext.group[streamId][gid].totalHopperNbr == 0) return -3;
			//HcuDebugPrint("L3BFDF: S/G=%d/%d, Total Hopper=%d\n", streamId, gid, gTaskL3bfdfContext.group[streamId][gid].totalHopperNbr);
			tmp += gTaskL3bfdfContext.group[streamId][gid].totalHopperNbr;
		}
		//if (tmp != HCU_SYSCFG_BFDF_HOPPER_NBR_MAX) return -4;
		if (tmp != HCU_L3BFDF_MAX_HOOPER_PER_LINE_ACTUAL) return -4;
		for (gid = gTaskL3bfdfContext.totalGroupNbr[streamId]+1; gid < HCU_SYSCFG_BFDF_HOPPER_NBR_MAX; gid++)
		{
			if (gTaskL3bfdfContext.group[streamId][gid].totalHopperNbr != 0) return -5;
		}

		//按照GroupId进行循环
		for (gid = 0; gid <= gTaskL3bfdfContext.totalGroupNbr[streamId]; gid++)
		{
			//HcuDebugPrint("L3BFDF: groupId Index = %d, access = %d\n", gid, gTaskL3bfdfContext.group[streamId][gid].groupId);
			if (gTaskL3bfdfContext.group[streamId][gid].groupId != gid) return -10;
			if (gTaskL3bfdfContext.group[streamId][gid].firstHopperId >= HCU_SYSCFG_BFDF_HOPPER_NBR_MAX) return -11;
			if (gTaskL3bfdfContext.group[streamId][gid].fillHopperId >= HCU_SYSCFG_BFDF_HOPPER_NBR_MAX) return -12;
			fHopper = gTaskL3bfdfContext.group[streamId][gid].firstHopperId;
			if ((gid == 0) && (fHopper == 0)){
				if (gTaskL3bfdfContext.group[streamId][0].totalHopperNbr != 1) return -13;
				if (gTaskL3bfdfContext.hopper[streamId][0].groupId != 0) return -14;
				if (gTaskL3bfdfContext.hopper[streamId][0].hopperId != 0) return -15;
				if (gTaskL3bfdfContext.hopper[streamId][0].nextHopperId != 0) return -16;
				if (gTaskL3bfdfContext.hopper[streamId][0].preHopperId != 0) return -17;
				continue;
			}
			if ((gid != 0) && (fHopper == 0)){
				if (gTaskL3bfdfContext.group[streamId][gid].totalHopperNbr != 0) return -18;
				continue;
			}
			nextHopper = fHopper;
			cnt = 1;
			if (gTaskL3bfdfContext.hopper[streamId][nextHopper].groupId != gid) return -20;
			if (gTaskL3bfdfContext.hopper[streamId][nextHopper].hopperId != nextHopper) return -21;
			if (gTaskL3bfdfContext.hopper[streamId][nextHopper].nextHopperId >= HCU_SYSCFG_BFDF_HOPPER_NBR_MAX) return -22;
			if (gTaskL3bfdfContext.hopper[streamId][nextHopper].preHopperId >= HCU_SYSCFG_BFDF_HOPPER_NBR_MAX) return -23;
			//正向循环
			while ((cnt < HCU_SYSCFG_BFDF_HOPPER_NBR_MAX) && (gTaskL3bfdfContext.hopper[streamId][nextHopper].nextHopperId != fHopper))
			{
				cnt++;
				tmpHopper = gTaskL3bfdfContext.hopper[streamId][nextHopper].nextHopperId;
				if (gTaskL3bfdfContext.hopper[streamId][tmpHopper].groupId != gid) return -30;
				if (gTaskL3bfdfContext.hopper[streamId][tmpHopper].hopperId != tmpHopper) return -31;
				if (gTaskL3bfdfContext.hopper[streamId][tmpHopper].nextHopperId >= HCU_SYSCFG_BFDF_HOPPER_NBR_MAX) return -32;
				if (gTaskL3bfdfContext.hopper[streamId][tmpHopper].preHopperId >= HCU_SYSCFG_BFDF_HOPPER_NBR_MAX) return -33;
				nextHopper = tmpHopper;
			}
			if (cnt >= HCU_SYSCFG_BFDF_HOPPER_NBR_MAX) return -34;
			//逆向循环
			cnt = 1;
			preHopper = fHopper;
			while ((cnt < HCU_SYSCFG_BFDF_HOPPER_NBR_MAX) && (gTaskL3bfdfContext.hopper[streamId][preHopper].preHopperId != fHopper))
			{
				cnt++;
				tmpHopper = gTaskL3bfdfContext.hopper[streamId][preHopper].preHopperId;
				if (gTaskL3bfdfContext.hopper[streamId][tmpHopper].groupId != gid) return -40;
				if (gTaskL3bfdfContext.hopper[streamId][tmpHopper].hopperId != tmpHopper) return -41;
				if (gTaskL3bfdfContext.hopper[streamId][tmpHopper].nextHopperId >= HCU_SYSCFG_BFDF_HOPPER_NBR_MAX) return -42;
				if (gTaskL3bfdfContext.hopper[streamId][tmpHopper].preHopperId >= HCU_SYSCFG_BFDF_HOPPER_NBR_MAX) return -43;
				preHopper = tmpHopper;
			}
			if (cnt >= HCU_SYSCFG_BFDF_HOPPER_NBR_MAX) return -44;

		}//按照GroupId进行循环
	}//STREAM

	return TRUE;
}

//用Box_Muller算法产生高斯分布的随机数
//u/N是均值和方差
double gaussian(double u, double n)
{
	double r,t,z,x;
	double s1,s2;
	s1=(1.0+rand())/(RAND_MAX+1.0);
	s2=(1.0+rand())/(RAND_MAX+1.0);
	r=sqrt(-2*log(s2)/log(2.71828));
	t=2*3.14*s1;
	z=r*cos(t);
	x=u+z*n;
	return x;
}

//分配小组的重量范围
bool func_l3bfdf_group_auto_alloc_init_range_in_average(UINT8 streamId, UINT16 nbrGroup, UINT32 wgtMin, UINT32 wgtMax)
{
	int i=0;
	double gap = 0;

	//入参检查：注意起点和终点
	if ((streamId >= HCU_SYSCFG_BFDF_EQU_FLOW_NBR_MAX) || (nbrGroup > gTaskL3bfdfContext.totalGroupNbr[streamId]))
		return FALSE;
	if (wgtMin >= wgtMax) return FALSE;
	if (nbrGroup <=0) return FALSE;
	gap = (double) (wgtMax - wgtMin) / (double)nbrGroup;

	for (i=1; i<=nbrGroup; i++){
		gTaskL3bfdfContext.group[streamId][i].rangeLow = wgtMin + (i-1)*gap;
		gTaskL3bfdfContext.group[streamId][i].rangeHigh = wgtMin + i*gap;
		gTaskL3bfdfContext.group[streamId][i].rangeAvg = (gTaskL3bfdfContext.group[streamId][i].rangeLow + gTaskL3bfdfContext.group[streamId][i].rangeHigh)/2.0;
		gTaskL3bfdfContext.group[streamId][i].rangeSigma = gap/2;
		//HcuDebugPrint("L3BFDF: S/G/Low/High = %d/%d/%f/%f, gap = %f\n", streamId, i, gTaskL3bfdfContext.group[streamId][i].rangeLow, gTaskL3bfdfContext.group[streamId][i].rangeHigh, gap);
	}

	return TRUE;
}

//分配小组的重量目标
bool func_l3bfdf_group_auto_alloc_init_target_with_uplimit(UINT8 streamId, UINT32 targetWgt, double ulRatio)
{
	int i=0;
	double ratio;

	//入参检查：注意起点和终点
	if (streamId >= HCU_SYSCFG_BFDF_EQU_FLOW_NBR_MAX) return FALSE;

	//处理Ratio
	if (ulRatio < 0) return FALSE;
	else if (ulRatio < 1) ratio = ulRatio;
	else if (ulRatio < 10) ratio = 1 + ulRatio/10;
	else ratio = 2;

	for (i=1; i<= gTaskL3bfdfContext.totalGroupNbr[streamId]; i++){
		gTaskL3bfdfContext.group[streamId][i].targetWeight = targetWgt * (rand()%100+100) / 100.0 / sqrt(1+gTaskL3bfdfContext.totalGroupNbr[streamId]-i);
		gTaskL3bfdfContext.group[streamId][i].targetUpLimit = targetWgt * ratio;
		gTaskL3bfdfContext.group[streamId][i].bufWgtTarget = gTaskL3bfdfContext.group[streamId][i].targetWeight/2; //暂时按照50%固定分配，未来待改进
	}

	return TRUE;
}

//打印信息
bool func_l3bfdf_print_all_hopper_status_by_id(UINT8 streamId)
{
	int i = 0;
	char s[800];
	char tmp[40];

	//入参检查：注意起点和终点
	if (streamId >= HCU_SYSCFG_BFDF_EQU_FLOW_NBR_MAX) return FALSE;

	memset(s, 0, sizeof(s));
	sprintf(s, "L3BFDF: Stream[%d] Total Group = %d, Hopper bitmap Gid/Hid/nHid/pHid/Stat = ", streamId, gTaskL3bfdfContext.totalGroupNbr[streamId]);
	for (i = 0; i<HCU_L3BFDF_MAX_HOOPER_PER_LINE_ACTUAL; i++){
		memset(tmp, 0, sizeof(tmp));
		sprintf(tmp, "[%d/%d/%d/%d/%d]", gTaskL3bfdfContext.hopper[streamId][i].groupId, gTaskL3bfdfContext.hopper[streamId][i].hopperId, gTaskL3bfdfContext.hopper[streamId][i].nextHopperId, gTaskL3bfdfContext.hopper[streamId][i].preHopperId, gTaskL3bfdfContext.hopper[streamId][i].hopperStatus);
		if ((strlen(s)+strlen(tmp)) < sizeof(s)) strcat(s, tmp);
	}
	strcat(s, "\n");
	HCU_DEBUG_PRINT_CRT(s);

	return TRUE;
}

//打印信息
bool func_l3bfdf_print_all_hopper_status_by_chain(UINT8 streamId)
{
	int i = 0;
	int fHopper = 0;
	int nextHopper = 0;
	int tmpHopper = 0;
	char s[900];
	char tmp[100];

	//入参检查：注意起点和终点
	if (streamId >= HCU_SYSCFG_BFDF_EQU_FLOW_NBR_MAX) return FALSE;

	//Output basic info
	memset(s, 0, sizeof(s));
	sprintf(s, "L3BFDF: PART-A Group profile Stream[%d] Total Group/Hopper=%d/%d, Group set [Index-tWgt/Buf/Uplim/High/Low] as: ", streamId, gTaskL3bfdfContext.totalGroupNbr[streamId], HCU_L3BFDF_MAX_HOOPER_PER_LINE_ACTUAL-1);
	for (i = 0; i<= gTaskL3bfdfContext.totalGroupNbr[streamId]; i++)
	{
		memset(tmp, 0, sizeof(tmp));
		sprintf(tmp, "[I:%d-T:%d/B:%d/U:%d/H:%d/L:%d] ", i, gTaskL3bfdfContext.group[streamId][i].targetWeight, gTaskL3bfdfContext.group[streamId][i].bufWgtTarget, \
				gTaskL3bfdfContext.group[streamId][i].targetUpLimit, gTaskL3bfdfContext.group[streamId][i].rangeHigh, gTaskL3bfdfContext.group[streamId][i].rangeLow);
		if ((strlen(s)+strlen(tmp)) < sizeof(s)) strcat(s, tmp);
	}
	strcat(s, "\n");
	HCU_DEBUG_PRINT_CRT(s);

	//Output chain status
	memset(s, 0, sizeof(s));
	sprintf(s, "L3BFDF: PART-B Hopper profile Stream[%d] Total Group=%d, Hopper chain set [Gid-ToHo-Hid/nHid/nHid] as: ", streamId, gTaskL3bfdfContext.totalGroupNbr[streamId]);
	for (i = 0; i<= gTaskL3bfdfContext.totalGroupNbr[streamId]; i++)
	{
		memset(tmp, 0, sizeof(tmp));
		sprintf(tmp, "[%d-%d-", i, gTaskL3bfdfContext.group[streamId][i].totalHopperNbr);
		if ((strlen(s)+strlen(tmp)) < sizeof(s)) strcat(s, tmp);
		fHopper = gTaskL3bfdfContext.group[streamId][i].firstHopperId;
		if (fHopper == 0){
			strcat(s, "] ");
			continue;
		}
		tmpHopper = fHopper;
		sprintf(tmp, "%d/", fHopper);
		if ((strlen(s)+strlen(tmp)) < sizeof(s)) strcat(s, tmp);
		nextHopper = gTaskL3bfdfContext.hopper[streamId][tmpHopper].nextHopperId;
		while(nextHopper!=fHopper){
			sprintf(tmp, "%d/", nextHopper);
			if ((strlen(s)+strlen(tmp)) < sizeof(s)) strcat(s, tmp);
			tmpHopper = gTaskL3bfdfContext.hopper[streamId][nextHopper].nextHopperId;
			nextHopper = tmpHopper;
			//HcuDebugPrint("TEST: nextHopperid = %d, fHopper=%d\n", nextHopper, fHopper);
		}
		strcat(s, "] ");
	}
	strcat(s, "\n");
	HCU_DEBUG_PRINT_CRT(s);

	return TRUE;
}

//打印信息
bool func_l3bfdf_print_all_hopper_ratio_by_weight(UINT8 streamId)
{
	int i = 0;
	int fHopper = 0;
	int nextHopper = 0;
	int tmpHopper = 0;
	char s[800];
	char tmp[40];

	//入参检查：注意起点和终点
	if (streamId >= HCU_SYSCFG_BFDF_EQU_FLOW_NBR_MAX) return FALSE;

	sprintf(s, "L3BFDF: Weight status Stream[%d] Total Group number = %d. Hopper/Buffer Ratio = ", streamId, gTaskL3bfdfContext.totalGroupNbr[streamId]);
	for (i = 1; i<= gTaskL3bfdfContext.totalGroupNbr[streamId]; i++)
	{
		memset(tmp, 0, sizeof(tmp));
		sprintf(tmp, "[Gid=%d, Hnbr=%d", i, gTaskL3bfdfContext.group[streamId][i].totalHopperNbr);
		if ((strlen(s)+strlen(tmp)) < sizeof(s)) strcat(s, tmp);
		fHopper = gTaskL3bfdfContext.group[streamId][i].firstHopperId;
		if (fHopper == 0){
			strcat(s, "] ");
			continue;
		}
		tmpHopper = fHopper;
		sprintf(tmp, "  (Hid:%d, Hv:%5.2f%%, Bv:%5.2f%%, Nbr:%d)", fHopper, (double)gTaskL3bfdfContext.hopper[streamId][fHopper].hopperValue/(double)gTaskL3bfdfContext.group[streamId][gTaskL3bfdfContext.hopper[streamId][fHopper].groupId].targetWeight*100.0,\
				(double)gTaskL3bfdfContext.hopper[streamId][fHopper].buferValue/(double)gTaskL3bfdfContext.group[streamId][gTaskL3bfdfContext.hopper[streamId][fHopper].groupId].targetWeight*100.0, gTaskL3bfdfContext.hopper[streamId][fHopper].matLackIndex);
		if ((strlen(s)+strlen(tmp)) < sizeof(s)) strcat(s, tmp);
		nextHopper = gTaskL3bfdfContext.hopper[streamId][tmpHopper].nextHopperId;
		while(nextHopper!=fHopper){
			//sprintf(tmp, "%5.2f%%/", 100*(double)gTaskL3bfdfContext.hopper[streamId][nextHopper].hopperValue/(double)gTaskL3bfdfContext.group[streamId][gTaskL3bfdfContext.hopper[streamId][nextHopper].groupId].targetWeight);
			sprintf(tmp, "  (Hid:%d, Hv:%5.2f%%, Bv:%5.2f%%, Nbr:%d)", nextHopper, (double)gTaskL3bfdfContext.hopper[streamId][nextHopper].hopperValue/(double)gTaskL3bfdfContext.group[streamId][gTaskL3bfdfContext.hopper[streamId][nextHopper].groupId].targetWeight*100.0,\
					(double)gTaskL3bfdfContext.hopper[streamId][nextHopper].buferValue/(double)gTaskL3bfdfContext.group[streamId][gTaskL3bfdfContext.hopper[streamId][nextHopper].groupId].targetWeight*100.0, gTaskL3bfdfContext.hopper[streamId][nextHopper].matLackIndex);
			if ((strlen(s)+strlen(tmp)) < sizeof(s)) strcat(s, tmp);
			tmpHopper = gTaskL3bfdfContext.hopper[streamId][nextHopper].nextHopperId;
			nextHopper = tmpHopper;
			//HcuDebugPrint("TEST: nextHopperid = %d, fHopper=%d\n", nextHopper, fHopper);
		}
		strcat(s, "] ");
	}
	strcat(s, "\n");
	HCU_DEBUG_PRINT_CRT(s);
	return TRUE;
}

//打印信息
bool func_l3bfdf_print_all_board_status(void)
{
	int i = 0, j=0;
	char s[900];
	char tmp[100];

	//Output basic info
	memset(s, 0, sizeof(s));
	sprintf(s, "L3BFDF: Boards startup refresh, conf. with Line/Boards = %d/%d, ", HCU_L3BFDF_MAX_STREAM_LINE_ACTUAL, HCU_L3BFDF_MAX_IO_BOARD_NBR_ACTUAL-1);
	for (i = 0; i< HCU_SYSCFG_BFDF_EQU_FLOW_NBR_MAX; i++)
	{
		memset(tmp, 0, sizeof(tmp));
		sprintf(tmp, "[Line(%d)=", i);
		strcat(s, tmp);
		for (j = 1; j< HCU_SYSCFG_BFDF_NODE_BOARD_NBR_MAX; j++)
		{
			sprintf(tmp, "%d/", gTaskL3bfdfContext.nodeDyn[i][j].nodeStatus);
			if ((strlen(s)+strlen(tmp)) < sizeof(s)) strcat(s, tmp);
		}
		strcat(s, "] ");
	}
	strcat(s, "\n");
	HCU_DEBUG_PRINT_CRT(s);

	memset(s, 0, sizeof(s));
	sprintf(s, "L3BFDF: Hopper status refresh, conf. with Line/Hop = %d/%d, ", HCU_L3BFDF_MAX_STREAM_LINE_ACTUAL, HCU_L3BFDF_MAX_HOOPER_PER_LINE_ACTUAL-1);
	for (i = 0; i< HCU_SYSCFG_BFDF_EQU_FLOW_NBR_MAX; i++)
	{
		memset(tmp, 0, sizeof(tmp));
		sprintf(tmp, "[Line(%d)=", i);
		strcat(s, tmp);
		for (j = 1; j< HCU_SYSCFG_BFDF_HOPPER_NBR_MAX; j++)
		{
			sprintf(tmp, "%d/", gTaskL3bfdfContext.hopper[i][j].hopperStatus);
			if ((strlen(s)+strlen(tmp)) < sizeof(s)) strcat(s, tmp);
		}
		strcat(s, "] ");
	}
	strcat(s, "\n");
	HCU_DEBUG_PRINT_CRT(s);

	return TRUE;
}

//寻找重物在小组的组别
UINT16 func_l3bfdf_new_ws_search_group(UINT8 streamId, double weight)
{
	int i=0;

	//入参检查：注意起点和终点
	if (streamId >= HCU_SYSCFG_BFDF_EQU_FLOW_NBR_MAX) return 0;

	for (i=1; i<=gTaskL3bfdfContext.totalGroupNbr[streamId] ; i++){
		if ((weight>=gTaskL3bfdfContext.group[streamId][i].rangeLow) && (weight<=gTaskL3bfdfContext.group[streamId][i].rangeHigh))
			return i;
	}

	//没找到，返回0
	return 0;
}

//搜索双满的第一个
UINT16 func_l3bfdf_new_ws_search_hopper_double_full(UINT8 streamId)
{
	int i=0;

	//入参检查：注意起点和终点
	if (streamId >= HCU_SYSCFG_BFDF_EQU_FLOW_NBR_MAX) return 0;

	for (i=1; i<HCU_SYSCFG_BFDF_HOPPER_NBR_MAX; i++){
		if (gTaskL3bfdfContext.hopper[streamId][i].hopperStatus == HCU_L3BFDF_HOPPER_STATUS_BUF_FULL)
			return i;
	}

	//没找到，返回0
	return 0;
}

//搜索篮筐满的第一个
UINT16 func_l3bfdf_new_ws_search_hopper_basket_full(UINT8 streamId)
{
	int i=0;

	//入参检查：注意起点和终点
	if (streamId >= HCU_SYSCFG_BFDF_EQU_FLOW_NBR_MAX) return 0;

	for (i=1; i<HCU_SYSCFG_BFDF_HOPPER_NBR_MAX; i++){
		if (gTaskL3bfdfContext.hopper[streamId][i].hopperStatus == HCU_L3BFDF_HOPPER_STATUS_BASKET_FULL)
			return i;
	}

	//没找到，返回0
	return 0;
}

//搜索BUF是否存在欠一满
UINT16 func_l3bfdf_new_ws_search_hopper_buffer_lack_one(UINT8 streamId, UINT16 gid, UINT32 weight)
{
	int cnt=0;
	UINT16 fHopper=0, nextHopper=0, tmpHopper=0;

	//入参检查：注意起点和终点
	if (streamId >= HCU_SYSCFG_BFDF_EQU_FLOW_NBR_MAX) return 0;
	if ((gid <=0) || (gid > gTaskL3bfdfContext.totalGroupNbr[streamId])) return 0;

	fHopper = gTaskL3bfdfContext.group[streamId][gid].fillHopperId;
	if (fHopper == 0) return 0;

	//Secure the range
	if (((gTaskL3bfdfContext.hopper[streamId][fHopper].buferValue + weight) <= gTaskL3bfdfContext.group[streamId][gid].bufWgtTarget) && \
			(gTaskL3bfdfContext.hopper[streamId][fHopper].hopperStatus >= HCU_L3BFDF_HOPPER_STATUS_BASKET_FULL) && \
			(gTaskL3bfdfContext.hopper[streamId][fHopper].hopperStatus < HCU_L3BFDF_HOPPER_STATUS_BUF_FULL) && \
			(func_l3bfdf_hopper_judge_buffer_is_lack_one_full(streamId, fHopper, weight) == TRUE))
			return fHopper;

	cnt = 0;
	nextHopper = gTaskL3bfdfContext.hopper[streamId][fHopper].nextHopperId;
	//while ((cnt < HCU_SYSCFG_BFDF_HOPPER_NBR_MAX) && (nextHopper != fHopper) && (gTaskL3bfdfContext.hopper[streamId][nextHopper].hopperStatus >= HCU_L3BFDF_HOPPER_STATUS_BASKET_FULL))
	while ((cnt < HCU_SYSCFG_BFDF_HOPPER_NBR_MAX) && (nextHopper != fHopper))
	{
		cnt++;
		if (((gTaskL3bfdfContext.hopper[streamId][nextHopper].buferValue + weight) <= gTaskL3bfdfContext.group[streamId][gid].bufWgtTarget) && \
				(gTaskL3bfdfContext.hopper[streamId][nextHopper].hopperStatus >= HCU_L3BFDF_HOPPER_STATUS_BASKET_FULL) && \
				(gTaskL3bfdfContext.hopper[streamId][nextHopper].hopperStatus < HCU_L3BFDF_HOPPER_STATUS_BUF_FULL) && \
				(func_l3bfdf_hopper_judge_buffer_is_lack_one_full(streamId, nextHopper, weight) == TRUE))
				return nextHopper;
		tmpHopper = gTaskL3bfdfContext.hopper[streamId][nextHopper].nextHopperId;
		nextHopper = tmpHopper;
	}

	//没找到，返回0
	return 0;
}

//搜索BUF常态的框
UINT16 func_l3bfdf_new_ws_search_hopper_buffer_normal(UINT8 streamId, UINT16 gid, UINT32 weight)
{
	int cnt=0;
	UINT16 fHopper=0, nextHopper=0, tmpHopper=0;

	//printf("test1! streamId/Gid/Weight=%d/%d/%d\n", streamId, gid, weight);

	//入参检查：注意起点和终点
	if (streamId >= HCU_SYSCFG_BFDF_EQU_FLOW_NBR_MAX) return 0;
	if ((gid <=0) || (gid > gTaskL3bfdfContext.totalGroupNbr[streamId])) return 0;

	fHopper = gTaskL3bfdfContext.group[streamId][gid].fillHopperId;
	if (fHopper == 0) return 0;

	//Secure the range
	if (((gTaskL3bfdfContext.hopper[streamId][fHopper].buferValue + weight) <= gTaskL3bfdfContext.group[streamId][gid].bufWgtTarget) && \
			(gTaskL3bfdfContext.hopper[streamId][fHopper].hopperStatus >= HCU_L3BFDF_HOPPER_STATUS_BASKET_FULL) && \
			(gTaskL3bfdfContext.hopper[streamId][fHopper].hopperStatus < HCU_L3BFDF_HOPPER_STATUS_BUF_FULL) && \
			(func_l3bfdf_hopper_judge_cur_mat_is_in_buffer_space(streamId, fHopper, weight)) == TRUE)
			return fHopper;

	cnt = 0;
	nextHopper = gTaskL3bfdfContext.hopper[streamId][fHopper].nextHopperId;
	//printf("Test3.2, NextHooperId=%d, fHopper=%d, HopStatus=%d\n", nextHopper, fHopper, gTaskL3bfdfContext.hopper[streamId][nextHopper].hopperStatus);

	//while ((cnt < HCU_SYSCFG_BFDF_HOPPER_NBR_MAX) && (nextHopper != fHopper) && (gTaskL3bfdfContext.hopper[streamId][nextHopper].hopperStatus >= HCU_L3BFDF_HOPPER_STATUS_BASKET_FULL))
	while ((cnt < HCU_SYSCFG_BFDF_HOPPER_NBR_MAX) && (nextHopper != fHopper))
	{
		cnt++;
		//printf("test3.5, bufferValue/Weight/BufTarget=%d/%d/%d, HopStatus=%d\n", gTaskL3bfdfContext.hopper[streamId][nextHopper].buferValue, weight, gTaskL3bfdfContext.group[streamId][gid].bufWgtTarget, gTaskL3bfdfContext.hopper[streamId][nextHopper].hopperStatus);
		if (((gTaskL3bfdfContext.hopper[streamId][nextHopper].buferValue + weight) <= gTaskL3bfdfContext.group[streamId][gid].bufWgtTarget) && \
				(gTaskL3bfdfContext.hopper[streamId][nextHopper].hopperStatus >= HCU_L3BFDF_HOPPER_STATUS_BASKET_FULL) && \
				(gTaskL3bfdfContext.hopper[streamId][nextHopper].hopperStatus < HCU_L3BFDF_HOPPER_STATUS_BUF_FULL) && \
				(func_l3bfdf_hopper_judge_cur_mat_is_in_buffer_space(streamId, nextHopper, weight) == TRUE))
				return nextHopper;
		tmpHopper = gTaskL3bfdfContext.hopper[streamId][nextHopper].nextHopperId;
		nextHopper = tmpHopper;
	}

	//没找到，返回0
	return 0;
}


//搜索篮筐是否存在欠一满
UINT16 func_l3bfdf_new_ws_search_hopper_basket_lack_one(UINT8 streamId, UINT16 gid, UINT32 weight)
{
	int cnt=0;
	UINT16 fHopper=0, nextHopper=0, tmpHopper=0;

	//入参检查：注意起点和终点
	if (streamId >= HCU_SYSCFG_BFDF_EQU_FLOW_NBR_MAX) return 0;
	if ((gid <=0) || (gid > gTaskL3bfdfContext.totalGroupNbr[streamId])) return 0;

	fHopper = gTaskL3bfdfContext.group[streamId][gid].fillHopperId;
	if (fHopper == 0) return 0;

	if ((gTaskL3bfdfContext.hopper[streamId][fHopper].hopperStatus >= HCU_L3BFDF_HOPPER_STATUS_VALID) &&\
			(gTaskL3bfdfContext.hopper[streamId][fHopper].hopperStatus < HCU_L3BFDF_HOPPER_STATUS_BASKET_FULL) &&\
			((gTaskL3bfdfContext.hopper[streamId][fHopper].hopperValue + weight) >= gTaskL3bfdfContext.group[streamId][gid].targetWeight) &&\
			((gTaskL3bfdfContext.hopper[streamId][fHopper].hopperValue + weight) <= (gTaskL3bfdfContext.group[streamId][gid].targetWeight +\
					gTaskL3bfdfContext.group[streamId][gid].targetUpLimit)))
			return fHopper;

	cnt = 0;
	nextHopper = gTaskL3bfdfContext.hopper[streamId][fHopper].nextHopperId;

	while ((cnt < HCU_SYSCFG_BFDF_HOPPER_NBR_MAX) && (nextHopper != fHopper))
	{
		cnt++;
		if ((gTaskL3bfdfContext.hopper[streamId][nextHopper].hopperStatus >= HCU_L3BFDF_HOPPER_STATUS_VALID) &&\
				(gTaskL3bfdfContext.hopper[streamId][nextHopper].hopperStatus < HCU_L3BFDF_HOPPER_STATUS_BASKET_FULL) &&\
				((gTaskL3bfdfContext.hopper[streamId][nextHopper].hopperValue + weight) >= gTaskL3bfdfContext.group[streamId][gid].targetWeight) &&\
				((gTaskL3bfdfContext.hopper[streamId][nextHopper].hopperValue + weight) <= (gTaskL3bfdfContext.group[streamId][gid].targetWeight +\
						gTaskL3bfdfContext.group[streamId][gid].targetUpLimit)))
				return nextHopper;
		tmpHopper = gTaskL3bfdfContext.hopper[streamId][nextHopper].nextHopperId;
		nextHopper = tmpHopper;
	}

	//没找到，返回0
	return 0;
}

//搜索正常的目标区间
UINT16 func_l3bfdf_new_ws_search_hopper_valid_normal(UINT8 sid, UINT16 gid, UINT32 weight)
{
	int cnt=0;
	UINT16 fH=0, nextHopper=0, tmpHopper=0;
	double cWg=0, tWg=0, tLm=0;
	//double tAvg=0;
//	double tSigma=0;
//	double hMin=0;
//	double hMax=0;
	double hFinal = 0;

	//入参检查：注意起点和终点
	if (sid >= HCU_SYSCFG_BFDF_EQU_FLOW_NBR_MAX) return 0;
	if ((gid <=0) || (gid > gTaskL3bfdfContext.totalGroupNbr[sid])) return 0;

	//入口寻找
	fH = gTaskL3bfdfContext.group[sid][gid].fillHopperId;
	if (fH == 0) return 0;

	tWg = gTaskL3bfdfContext.group[sid][gid].targetWeight;
	tLm = gTaskL3bfdfContext.group[sid][gid].targetUpLimit;
	//tAvg = gTaskL3bfdfContext.group[sid][gid].rangeAvg;
	//tSigma = gTaskL3bfdfContext.group[sid][gid].rangeSigma;
	hFinal = tWg+tLm;

	//判定入口满足条件
	if ((gTaskL3bfdfContext.hopper[sid][fH].hopperStatus >= HCU_L3BFDF_HOPPER_STATUS_VALID) && \
			(gTaskL3bfdfContext.hopper[sid][fH].hopperStatus < HCU_L3BFDF_HOPPER_STATUS_BUF_FULL))
	{
		//在每一次反馈中，已经更新过最新的N系数了，不需要再更新
		cWg = gTaskL3bfdfContext.hopper[sid][fH].hopperValue + weight;
		if ((func_l3bfdf_hopper_judge_cur_mat_is_in_right_space(sid, fH, weight) == TRUE) && (cWg <= hFinal)) return fH;
	}

	//顺着双链表，查找满足条件的料斗
	cnt = 0;
	nextHopper = gTaskL3bfdfContext.hopper[sid][fH].nextHopperId;
	while ((cnt < HCU_SYSCFG_BFDF_HOPPER_NBR_MAX) && (nextHopper != fH))
	{
		cnt++;
		if ((gTaskL3bfdfContext.hopper[sid][nextHopper].hopperStatus >= HCU_L3BFDF_HOPPER_STATUS_VALID) &&\
			(gTaskL3bfdfContext.hopper[sid][nextHopper].hopperStatus < HCU_L3BFDF_HOPPER_STATUS_BUF_FULL))
		{
			//在每一次反馈中，已经更新过最新的N系数了，不需要再更新
			cWg = gTaskL3bfdfContext.hopper[sid][nextHopper].hopperValue + weight;
			if ((func_l3bfdf_hopper_judge_cur_mat_is_in_right_space(sid, nextHopper, weight) == TRUE) && (cWg <= hFinal)) return nextHopper;
		}
		tmpHopper = gTaskL3bfdfContext.hopper[sid][nextHopper].nextHopperId;
		nextHopper = tmpHopper;
	}

	//没找到，返回0
	return 0;
}

//寻找目标空间是否为空，就是是否一定会在理论上组合出来
//最核心的算法：整数解空间算法
bool func_l3bfdf_hopper_judge_cur_mat_is_in_right_space(UINT8 streamId, UINT16 hid, UINT32 weight)
{
	UINT16 gid = 0;
	double targetWgtH = 0, targetWgtL = 0;
	double min=0, max=0;
	UINT16 umin=0, umax=0;
	//UINT16 index = 0;

	//入参检查
	if ((streamId >= HCU_SYSCFG_BFDF_EQU_FLOW_NBR_MAX) || (hid==0) || (hid >= HCU_SYSCFG_BFDF_HOPPER_NBR_MAX))
		return FALSE;

	//状态设置, 初始化物料数量
	gid = gTaskL3bfdfContext.hopper[streamId][hid].groupId;
	if (gTaskL3bfdfContext.group[streamId][gid].rangeHigh == 0) return FALSE;
	if (gTaskL3bfdfContext.group[streamId][gid].rangeLow == 0) gTaskL3bfdfContext.group[streamId][gid].rangeLow = 500;  //5g是动态归零值

	//目标重量
	if (gTaskL3bfdfContext.group[streamId][gid].targetWeight >= (gTaskL3bfdfContext.hopper[streamId][hid].hopperValue + weight)){
		targetWgtL = gTaskL3bfdfContext.group[streamId][gid].targetWeight - gTaskL3bfdfContext.hopper[streamId][hid].hopperValue - weight;
	}else{
		targetWgtL = 0;
	}
	targetWgtH = targetWgtL + gTaskL3bfdfContext.group[streamId][gid].targetUpLimit;

	//求最大最小范围
	max = targetWgtH/(double)((gTaskL3bfdfContext.group[streamId][gid].rangeLow==0)?0.01:gTaskL3bfdfContext.group[streamId][gid].rangeLow);
	min = targetWgtL/(double)((gTaskL3bfdfContext.group[streamId][gid].rangeHigh==0)?0.01:gTaskL3bfdfContext.group[streamId][gid].rangeHigh);
	umax = (UINT32)max;
	umin = (UINT32)min;

	//不相等的情况下，是否覆盖整数
#if 1
	HCU_DEBUG_PRINT_FAT("L3BFDF: Judge mat in basket space: max/min/targetWgtH/L = %6.2f/%6.2f/%6.2f/%6.2f, weight/RangeL/H/Cur/Target/Up=%d/%d/%d/%d/%d/%d\n", \
			max, min, targetWgtH, targetWgtL, \
			weight, gTaskL3bfdfContext.group[streamId][gid].rangeLow, gTaskL3bfdfContext.group[streamId][gid].rangeHigh, \
			gTaskL3bfdfContext.hopper[streamId][hid].hopperValue, gTaskL3bfdfContext.group[streamId][gid].targetWeight, \
			gTaskL3bfdfContext.group[streamId][gid].targetWeight+gTaskL3bfdfContext.group[streamId][gid].targetUpLimit);
#endif
	//当最后两个Lack1/2的时候
//	index = gTaskL3bfdfContext.hopper[streamId][hid].matLackIndex;
//	if ((index > 2) && (index <= 4) && ((umax - umin) >=1) && ((max-min)>0.05)) return TRUE;
//	if ((index == 2) && ((umax - umin) >=1) && ((max-min) > 0.05)) return TRUE;
//	if (((index > 4) || (index <= 1)) && ((umax - umin) >=1)) return TRUE;

	if ((umax-umin)>=1 && ((max-min) >= gTaskL3bfdfContext.combAlgoSpaceCtrlRatio)) return TRUE;

	//最终不成功
	return FALSE;
}

//为了buffer而建立的搜索算法
bool func_l3bfdf_hopper_judge_cur_mat_is_in_buffer_space(UINT8 streamId, UINT16 hid, UINT32 weight)
{
	UINT16 gid = 0;
	double targetWgtH = 0, targetWgtL = 0;
	double min=0, max=0;
	UINT16 umin=0, umax=0;
	//UINT16 index = 0;

	//printf("BUFFER ENTER, LineId/Hid/Weight=%d/%d/%d\n\n", streamId, hid, weight);
	//入参检查
	if ((streamId >= HCU_SYSCFG_BFDF_EQU_FLOW_NBR_MAX) || (hid==0) || (hid >= HCU_SYSCFG_BFDF_HOPPER_NBR_MAX))
		return FALSE;

	//状态设置, 初始化物料数量
	gid = gTaskL3bfdfContext.hopper[streamId][hid].groupId;
	if (gTaskL3bfdfContext.group[streamId][gid].rangeHigh == 0) return FALSE;
	if (gTaskL3bfdfContext.group[streamId][gid].rangeLow == 0) gTaskL3bfdfContext.group[streamId][gid].rangeLow = 500;  //5g是动态归零值

	//目标重量
	if (gTaskL3bfdfContext.group[streamId][gid].targetWeight >= (gTaskL3bfdfContext.hopper[streamId][hid].buferValue + weight)){
		targetWgtL = gTaskL3bfdfContext.group[streamId][gid].targetWeight - gTaskL3bfdfContext.hopper[streamId][hid].buferValue - weight;
	}else{
		targetWgtL = 0;
	}
	targetWgtH = targetWgtL + gTaskL3bfdfContext.group[streamId][gid].targetUpLimit;

	//求最大最小范围
	max = targetWgtH/(double)((gTaskL3bfdfContext.group[streamId][gid].rangeLow==0)?0.01:gTaskL3bfdfContext.group[streamId][gid].rangeLow);
	min = targetWgtL/(double)((gTaskL3bfdfContext.group[streamId][gid].rangeHigh==0)?0.01:gTaskL3bfdfContext.group[streamId][gid].rangeHigh);
	umax = (UINT32)max;
	umin = (UINT32)min;

	//不相等的情况下，是否覆盖整数
#if 1
	HCU_DEBUG_PRINT_FAT("L3BFDF: Judge cur mat in buffer space: max/min/targetWgtH/L = %6.2f/%6.2f/%6.2f/%6.2f, weight/RangeL/H/Cur/Target/Up=%d/%d/%d/%d/%d/%d\n", \
			max, min, targetWgtH, targetWgtL,\
			weight, gTaskL3bfdfContext.group[streamId][gid].rangeLow, gTaskL3bfdfContext.group[streamId][gid].rangeHigh, \
			gTaskL3bfdfContext.hopper[streamId][hid].buferValue, gTaskL3bfdfContext.group[streamId][gid].targetWeight, \
			gTaskL3bfdfContext.group[streamId][gid].targetWeight+gTaskL3bfdfContext.group[streamId][gid].targetUpLimit);
#endif

	//当最后两个Lack1/2的时候
//	index = gTaskL3bfdfContext.hopper[streamId][hid].matLackIndex;
//	if ((index > 2) && (index <= 4) && ((umax - umin) >=1) && ((max-min)>0.05)) return TRUE;
//	if ((index == 2) && ((umax - umin) >=1) && ((max-min) > 0.05)) return TRUE;
//	if (((index > 4) || (index <= 1)) && ((umax - umin) >=1)) return TRUE;

	if ((umax-umin)>=1 && ((max-min) >= gTaskL3bfdfContext.combAlgoSpaceCtrlRatio)) return TRUE;

	//最终不成功
	return FALSE;
}


//判定BUFFER是不是欠一满，而且还没有已经满了的
bool func_l3bfdf_hopper_judge_buffer_is_lack_one_full(UINT8 sid, UINT16 hid, UINT32 weight)
{
	UINT16 gid = 0;
	UINT32 tAvg=0;
	//UINT32 tSigma=0;
	UINT32 cWg=0;
	UINT32 gap=0;

	//入参检查
	if ((sid >= HCU_SYSCFG_BFDF_EQU_FLOW_NBR_MAX) || (hid==0) || (hid >= HCU_SYSCFG_BFDF_HOPPER_NBR_MAX))
		return FALSE;

	//状态判定
	if ((gTaskL3bfdfContext.hopper[sid][hid].hopperStatus != HCU_L3BFDF_HOPPER_STATUS_BASKET_FULL) &&\
			(gTaskL3bfdfContext.hopper[sid][hid].hopperStatus != HCU_L3BFDF_HOPPER_STATUS_BUF_CONT))
		return FALSE;

	//获取基础信息
	gid = gTaskL3bfdfContext.hopper[sid][hid].groupId;
	tAvg = gTaskL3bfdfContext.group[sid][gid].rangeAvg;
	//tSigma = gTaskL3bfdfContext.group[sid][gid].rangeSigma;
	cWg = gTaskL3bfdfContext.hopper[sid][hid].buferValue + weight;

	if (cWg > gTaskL3bfdfContext.group[sid][gid].bufWgtTarget) return FALSE;

	//计算范围一次范围
	if (func_l3bfdf_hopper_judge_cur_mat_is_in_buffer_space(sid, hid, weight) == FALSE)
		return FALSE;

	//计算二次范围：计算GAP比范围还小，则认为是最后一个
	gap = gTaskL3bfdfContext.group[sid][gid].bufWgtTarget - cWg;
	if (gap < tAvg) return TRUE;

	//其他情况，均认为不是最后一个
	return FALSE;
}

//正常篮筐未满条件下的出料发送，物料肯定直接进入篮筐
bool func_l3bfdf_new_ws_send_out_comb_out_message_by_pullin(UINT8 streamId, UINT16 hopperId)
{
	//入参检查：注意起点和终点
	if ((streamId >= HCU_SYSCFG_BFDF_EQU_FLOW_NBR_MAX) || (hopperId >= HCU_SYSCFG_BFDF_HOPPER_NBR_MAX))
		return FALSE;

	//先检查hopper状态
	if ((gTaskL3bfdfContext.hopper[streamId][hopperId].hopperStatus != HCU_L3BFDF_HOPPER_STATUS_VALID) &&\
			(gTaskL3bfdfContext.hopper[streamId][hopperId].hopperStatus != HCU_L3BFDF_HOPPER_STATUS_PULLIN_OUT)){
		HCU_DEBUG_PRINT_FAT("L3BFDF: Hopper status error, Line/HopId=%d/%d, Status=%d\n", streamId, hopperId, gTaskL3bfdfContext.hopper[streamId][hopperId].hopperStatus);
		return FALSE;
	}

	//发送
	msg_struct_l3bfdf_can_ws_comb_out_t snd;
	memset(&snd, 0, sizeof(msg_struct_l3bfdf_can_ws_comb_out_t));
	//HopperId就是按照HUITP的定义来进行
	snd.hopperId = hopperId;
	snd.basketFullStatus = FALSE;
	snd.bufferFullStatus = FALSE;
	snd.snrId = ((streamId<<3) + ((2 + (hopperId-1)/HCU_SYSCFG_BFDF_HOPPER_IN_ONE_BOARD)&0x07));
	snd.length = sizeof(msg_struct_l3bfdf_can_ws_comb_out_t);
	HCU_MSG_SEND_GENERNAL_PROCESS(MSG_ID_L3BFDF_CAN_WS_COMB_OUT, TASK_ID_CANALPHA, TASK_ID_L3BFDF);

	//改变状态
	if (hopperId == 0){
		gTaskL3bfdfContext.hopper[streamId][hopperId].hopperStatus = HCU_L3BFDF_HOPPER_STATUS_VALID;
	}else{
		gTaskL3bfdfContext.hopper[streamId][hopperId].hopperStatus = HCU_L3BFDF_HOPPER_STATUS_PULLIN_OUT;
	}

	//更新统计信息

	//没找到，返回0
	return TRUE;
}

//正常篮筐未满条件下的出料发送，物料最后一次进入篮筐，或者进入BUFFER
//HOPPER肯定不会出现满的情况
bool func_l3bfdf_new_ws_send_out_comb_out_message_w_basket_full(UINT8 streamId, UINT16 hopperId)
{
	//入参检查：注意起点和终点
	if ((streamId >= HCU_SYSCFG_BFDF_EQU_FLOW_NBR_MAX) || (hopperId == 0) || (hopperId >= HCU_SYSCFG_BFDF_HOPPER_NBR_MAX))
		return FALSE;

	//先检查hopper状态
	//printf("Send comb-out msg out, shall be full basket status, actual=%d\n", gTaskL3bfdfContext.hopper[streamId][hopperId].hopperStatus);
	if ((gTaskL3bfdfContext.hopper[streamId][hopperId].hopperStatus < HCU_L3BFDF_HOPPER_STATUS_VALID) ||\
			(gTaskL3bfdfContext.hopper[streamId][hopperId].hopperStatus >= HCU_L3BFDF_HOPPER_STATUS_BUF_FULL))
	{
		HCU_DEBUG_PRINT_IPT("L3BFDF: Hopper status error, Line/HopId=%d/%d, Status=%d\n", streamId, hopperId, gTaskL3bfdfContext.hopper[streamId][hopperId].hopperStatus);
		return FALSE;
	}

	//发送
	msg_struct_l3bfdf_can_ws_comb_out_t snd;
	memset(&snd, 0, sizeof(msg_struct_l3bfdf_can_ws_comb_out_t));
	//HopperId就是按照HUITP的定义来进行
	snd.hopperId = hopperId;
	snd.basketFullStatus = TRUE;
	snd.bufferFullStatus = FALSE;
	snd.snrId = ((streamId<<3) + ((2 + hopperId/HCU_SYSCFG_BFDF_HOPPER_NBR_MAX)&0x07));
	snd.length = sizeof(msg_struct_l3bfdf_can_ws_comb_out_t);
	HCU_MSG_SEND_GENERNAL_PROCESS(MSG_ID_L3BFDF_CAN_WS_COMB_OUT, TASK_ID_CANALPHA, TASK_ID_L3BFDF);

	gTaskL3bfdfContext.hopper[streamId][hopperId].hopperStatus = HCU_L3BFDF_HOPPER_STATUS_PULLIN_OUT;

	//没找到，返回0
	return TRUE;
}

//双满，最后一次发送出料消息
bool func_l3bfdf_new_ws_send_out_comb_out_message_w_double_full(UINT8 streamId, UINT16 hopperId)
{
	//入参检查：注意起点和终点
	if ((streamId >= HCU_SYSCFG_BFDF_EQU_FLOW_NBR_MAX) || (hopperId == 0) || (hopperId >= HCU_SYSCFG_BFDF_HOPPER_NBR_MAX))
		return FALSE;

	//先检查hopper状态
	if (gTaskL3bfdfContext.hopper[streamId][hopperId].hopperStatus < HCU_L3BFDF_HOPPER_STATUS_BASKET_FULL) return FALSE;

	//发送
	msg_struct_l3bfdf_can_ws_comb_out_t snd;
	memset(&snd, 0, sizeof(msg_struct_l3bfdf_can_ws_comb_out_t));
	//HopperId就是按照HUITP的定义来进行
	snd.hopperId = hopperId;
	snd.basketFullStatus = TRUE;
	snd.bufferFullStatus = TRUE;
	snd.snrId = ((streamId<<3) + ((2 + hopperId/HCU_SYSCFG_BFDF_HOPPER_NBR_MAX)&0x07));
	snd.length = sizeof(msg_struct_l3bfdf_can_ws_comb_out_t);
	HCU_MSG_SEND_GENERNAL_PROCESS(MSG_ID_L3BFDF_CAN_WS_COMB_OUT, TASK_ID_CANALPHA, TASK_ID_L3BFDF);

	gTaskL3bfdfContext.hopper[streamId][hopperId].hopperStatus = HCU_L3BFDF_HOPPER_STATUS_PULLIN_OUT;

	//没找到，返回0
	return TRUE;
}

/***************************************************************************************************************************
 *
 * 　周期性扫描过程
 *
 ***************************************************************************************************************************/
//周期性统计扫描定时器
OPSTAT func_l3bfdf_time_out_statistic_scan_process(void)
{
	//int ret = 0;

	//首先增加时间流逝的计数器
	gTaskL3bfdfContext.elipseCnt++;
	gTaskL3bfdfContext.elipse24HourCnt++;

	//采取老化算法 x(n+1) = x(n) * (1-1/120) + latest，从而得到最新的数据，但该数据最好使用float，然后再转换为UINT32存入到数据库表单中
	memset(&(gTaskL3bfdfContext.staLocalUi), 0, sizeof(HcuSysMsgIeL3bfdfContextStaElement_t));
	//先存储临时数据
	gTaskL3bfdfContext.curAge.wsIncMatCntMid = gTaskL3bfdfContext.curAge.wsIncMatCntMid*(float)HCU_L3BFDF_STA_AGEING_COEF + (float)HCU_L3BFDF_STA_AGEING_COEF_ALPHA*gTaskL3bfdfContext.cur.wsIncMatCnt;
	gTaskL3bfdfContext.curAge.wsIncMatWgtMid = gTaskL3bfdfContext.curAge.wsIncMatWgtMid*(float)HCU_L3BFDF_STA_AGEING_COEF + (float)HCU_L3BFDF_STA_AGEING_COEF_ALPHA*gTaskL3bfdfContext.cur.wsIncMatWgt;
	gTaskL3bfdfContext.curAge.wsCombTimesMid = gTaskL3bfdfContext.curAge.wsCombTimesMid*(float)HCU_L3BFDF_STA_AGEING_COEF + (float)HCU_L3BFDF_STA_AGEING_COEF_ALPHA*gTaskL3bfdfContext.cur.wsCombTimes;
	gTaskL3bfdfContext.curAge.wsTttTimesMid = gTaskL3bfdfContext.curAge.wsTttTimesMid*(float)HCU_L3BFDF_STA_AGEING_COEF + (float)HCU_L3BFDF_STA_AGEING_COEF_ALPHA*gTaskL3bfdfContext.cur.wsTttTimes;
	gTaskL3bfdfContext.curAge.wsTgvTimesMid = gTaskL3bfdfContext.curAge.wsTgvTimesMid*(float)HCU_L3BFDF_STA_AGEING_COEF + (float)HCU_L3BFDF_STA_AGEING_COEF_ALPHA*gTaskL3bfdfContext.cur.wsTgvTimes;
	gTaskL3bfdfContext.curAge.wsTttMatCntMid = gTaskL3bfdfContext.curAge.wsTttMatCntMid*(float)HCU_L3BFDF_STA_AGEING_COEF + (float)HCU_L3BFDF_STA_AGEING_COEF_ALPHA*gTaskL3bfdfContext.cur.wsTttMatCnt;
	gTaskL3bfdfContext.curAge.wsCallCellTimesMid = gTaskL3bfdfContext.curAge.wsCallCellTimesMid*(float)HCU_L3BFDF_STA_AGEING_COEF + (float)HCU_L3BFDF_STA_AGEING_COEF_ALPHA*gTaskL3bfdfContext.cur.wsCallCellTimes;
	gTaskL3bfdfContext.curAge.wsTgvMatCntMid = gTaskL3bfdfContext.curAge.wsTgvMatCntMid*(float)HCU_L3BFDF_STA_AGEING_COEF + (float)HCU_L3BFDF_STA_AGEING_COEF_ALPHA*gTaskL3bfdfContext.cur.wsTgvMatCnt;
	gTaskL3bfdfContext.curAge.wsTttMatWgtMid = gTaskL3bfdfContext.curAge.wsTttMatWgtMid*(float)HCU_L3BFDF_STA_AGEING_COEF + (float)HCU_L3BFDF_STA_AGEING_COEF_ALPHA*gTaskL3bfdfContext.cur.wsTttMatWgt;
	gTaskL3bfdfContext.curAge.wsTgvMatWgtMid = gTaskL3bfdfContext.curAge.wsTgvMatWgtMid*(float)HCU_L3BFDF_STA_AGEING_COEF + (float)HCU_L3BFDF_STA_AGEING_COEF_ALPHA*gTaskL3bfdfContext.cur.wsTgvMatWgt;
	//再输出结果
	gTaskL3bfdfContext.staLocalUi.wsIncMatCnt = (UINT32)(gTaskL3bfdfContext.curAge.wsIncMatCntMid + 0.5);
	gTaskL3bfdfContext.staLocalUi.wsIncMatWgt = (UINT64)(gTaskL3bfdfContext.curAge.wsIncMatWgtMid + 0.5);
	gTaskL3bfdfContext.staLocalUi.wsCombTimes = (UINT32)(gTaskL3bfdfContext.curAge.wsCombTimesMid + 0.5);
	gTaskL3bfdfContext.staLocalUi.wsTttTimes  = (UINT32)(gTaskL3bfdfContext.curAge.wsTttTimesMid + 0.5);
	gTaskL3bfdfContext.staLocalUi.wsTgvTimes  = (UINT32)(gTaskL3bfdfContext.curAge.wsTgvTimesMid + 0.5);
	gTaskL3bfdfContext.staLocalUi.wsTttMatCnt = (UINT32)(gTaskL3bfdfContext.curAge.wsTttMatCntMid + 0.5);
	gTaskL3bfdfContext.staLocalUi.wsTgvMatCnt = (UINT32)(gTaskL3bfdfContext.curAge.wsTgvMatCntMid + 0.5);
	gTaskL3bfdfContext.staLocalUi.wsTttMatWgt = (UINT64)(gTaskL3bfdfContext.curAge.wsTttMatWgtMid + 0.5);
	gTaskL3bfdfContext.staLocalUi.wsTgvMatWgt = (UINT64)(gTaskL3bfdfContext.curAge.wsTgvMatWgtMid + 0.5);
	gTaskL3bfdfContext.staLocalUi.wsCallCellTimes = (UINT32)(gTaskL3bfdfContext.curAge.wsCallCellTimesMid + 0.5);
	gTaskL3bfdfContext.staLocalUi.wsAvgTttTimes = gTaskL3bfdfContext.staLocalUi.wsTttTimes;
	gTaskL3bfdfContext.staLocalUi.wsAvgTttMatCnt = gTaskL3bfdfContext.staLocalUi.wsTttMatCnt;
	gTaskL3bfdfContext.staLocalUi.wsAvgTttMatWgt = gTaskL3bfdfContext.staLocalUi.wsTttMatWgt;

	//5s的LocalUI数据进行更新
	if ((gTaskL3bfdfContext.elipseCnt % HCU_L3BFDF_STA_5S_CYCLE) == 0){
		StrHlcIe_cui_hcu2uir_statistic_bfdf_report_t buf;
		memset(&buf, 0, sizeof(StrHlcIe_cui_hcu2uir_statistic_bfdf_report_t));
		int i = 0;
		for (i = 0; i<HCU_L3BFDF_MAX_STREAM_LINE_ACTUAL; i++){
			buf.lineId = i;
			buf.targetWeight = gTaskL3bfdfContext.group[i][1].targetWeight;
			buf.upLimitWeight = gTaskL3bfdfContext.group[i][1].targetUpLimit;
			buf.totalPackage = gTaskL3bfdfContext.staUp2Now.wsIncMatCnt;
			buf.totalReject = gTaskL3bfdfContext.staUp2Now.wsTgvMatCnt;
			buf.totalWeight = gTaskL3bfdfContext.staUp2Now.wsIncMatWgt;\
			buf.throughputPerMin = gTaskL3bfdfContext.staLocalUi.wsAvgTttTimes;
			hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_statistic_bfdf_report(gTaskL3bfdfContext.configId, &buf);
		}
	}

	//更新1Min各个统计表单
	gTaskL3bfdfContext.staOneMin.wsIncMatCnt += gTaskL3bfdfContext.cur.wsIncMatCnt;
	gTaskL3bfdfContext.staOneMin.wsIncMatWgt += gTaskL3bfdfContext.cur.wsIncMatWgt;
	gTaskL3bfdfContext.staOneMin.wsCombTimes += gTaskL3bfdfContext.cur.wsCombTimes;
	gTaskL3bfdfContext.staOneMin.wsTttTimes  += gTaskL3bfdfContext.cur.wsTttTimes;
	gTaskL3bfdfContext.staOneMin.wsTgvTimes  += gTaskL3bfdfContext.cur.wsTgvTimes;
	gTaskL3bfdfContext.staOneMin.wsTttMatCnt += gTaskL3bfdfContext.cur.wsTttMatCnt;
	gTaskL3bfdfContext.staOneMin.wsTgvMatCnt += gTaskL3bfdfContext.cur.wsTgvMatCnt;
	gTaskL3bfdfContext.staOneMin.wsTttMatWgt += gTaskL3bfdfContext.cur.wsTttMatWgt;
	gTaskL3bfdfContext.staOneMin.wsTgvMatWgt += gTaskL3bfdfContext.cur.wsTgvMatWgt;
	gTaskL3bfdfContext.staOneMin.wsCallCellTimes += gTaskL3bfdfContext.cur.wsCallCellTimes;
	float timeRun1MinRatio = (float) HCU_L3BFDF_STA_1M_CYCLE / (float)(((gTaskL3bfdfContext.elipseCnt%HCU_L3BFDF_STA_1M_CYCLE)==0)?HCU_L3BFDF_STA_1M_CYCLE:(gTaskL3bfdfContext.elipseCnt%HCU_L3BFDF_STA_1M_CYCLE));
	gTaskL3bfdfContext.staOneMin.wsAvgTttTimes = (UINT32)(gTaskL3bfdfContext.staOneMin.wsTttTimes*timeRun1MinRatio);
	gTaskL3bfdfContext.staOneMin.wsAvgTttMatCnt = (UINT32)(gTaskL3bfdfContext.staOneMin.wsTttMatCnt*timeRun1MinRatio);
	gTaskL3bfdfContext.staOneMin.wsAvgTttMatWgt = (UINT32)(gTaskL3bfdfContext.staOneMin.wsTttMatWgt*timeRun1MinRatio);

#if 0  //使用了5s的更新方式，不再需要1分钟的频度
	//1分钟：是否需要分为2条线路：明确是按照整个系统为单位
	if ((gTaskL3bfdfContext.elipseCnt % HCU_L3BFDF_STA_1M_CYCLE) == 0){
		StrHlcIe_cui_hcu2uir_statistic_bfdf_report_t buf;
		memset(&buf, 0, sizeof(StrHlcIe_cui_hcu2uir_statistic_bfdf_report_t));
		int i = 0;
		for (i = 0; i<HCU_L3BFDF_MAX_STREAM_LINE_ACTUAL; i++){
			buf.lineId = i;
			buf.targetWeight = gTaskL3bfdfContext.group[i][1].targetWeight;
			buf.upLimitWeight = gTaskL3bfdfContext.group[i][1].targetUpLimit;
			buf.totalPackage = gTaskL3bfdfContext.staUp2Now.wsIncMatCnt;
			buf.totalReject = gTaskL3bfdfContext.staUp2Now.wsTgvMatCnt;
			buf.totalWeight = gTaskL3bfdfContext.staUp2Now.wsIncMatWgt;\
			buf.throughputPerMin = gTaskL3bfdfContext.staOneMin.wsAvgTttTimes;
			hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_statistic_bfdf_report(gTaskL3bfdfContext.configId, &buf);
		}
		//然后将1分钟统计数据表单清零，以便再次计数
		memset(&(gTaskL3bfdfContext.staOneMin), 0, sizeof(HcuSysMsgIeL3bfdfContextStaElement_t));
	}
#endif

	//更新60Min各个统计表单
	gTaskL3bfdfContext.sta60Min.wsIncMatCnt += gTaskL3bfdfContext.cur.wsIncMatCnt;
	gTaskL3bfdfContext.sta60Min.wsIncMatWgt += gTaskL3bfdfContext.cur.wsIncMatWgt;
	gTaskL3bfdfContext.sta60Min.wsCombTimes += gTaskL3bfdfContext.cur.wsCombTimes;
	gTaskL3bfdfContext.sta60Min.wsTttTimes  += gTaskL3bfdfContext.cur.wsTttTimes;
	gTaskL3bfdfContext.sta60Min.wsTgvTimes  += gTaskL3bfdfContext.cur.wsTgvTimes;
	gTaskL3bfdfContext.sta60Min.wsTttMatCnt += gTaskL3bfdfContext.cur.wsTttMatCnt;
	gTaskL3bfdfContext.sta60Min.wsTgvMatCnt += gTaskL3bfdfContext.cur.wsTgvMatCnt;
	gTaskL3bfdfContext.sta60Min.wsTttMatWgt += gTaskL3bfdfContext.cur.wsTttMatWgt;
	gTaskL3bfdfContext.sta60Min.wsTgvMatWgt += gTaskL3bfdfContext.cur.wsTgvMatWgt;
	gTaskL3bfdfContext.sta60Min.wsCallCellTimes += gTaskL3bfdfContext.cur.wsCallCellTimes;
	float timeRun60MinRatio = (float) HCU_L3BFDF_STA_60M_CYCLE / (float)(((gTaskL3bfdfContext.elipseCnt%HCU_L3BFDF_STA_60M_CYCLE)==0)?HCU_L3BFDF_STA_60M_CYCLE:(gTaskL3bfdfContext.elipseCnt%HCU_L3BFDF_STA_60M_CYCLE));
	gTaskL3bfdfContext.sta60Min.wsAvgTttTimes = (UINT32)(gTaskL3bfdfContext.sta60Min.wsTttTimes*timeRun60MinRatio);
	gTaskL3bfdfContext.sta60Min.wsAvgTttMatCnt = (UINT32)(gTaskL3bfdfContext.sta60Min.wsTttMatCnt*timeRun60MinRatio);
	gTaskL3bfdfContext.sta60Min.wsAvgTttMatWgt = (UINT32)(gTaskL3bfdfContext.sta60Min.wsTttMatWgt*timeRun60MinRatio);

	//60分钟到了，发送统计报告到后台：发送后台只需要一种统计报告即可，重复发送意义不大
	if ((gTaskL3bfdfContext.elipseCnt % HCU_L3BFDF_STA_60M_CYCLE) == 0){
		//60分统计表更新数据库
		if (dbi_HcuBfdf_StaDatainfo_save(HCU_L3BFDF_STA_DBI_TABLE_60MIN, gTaskL3bfdfContext.configId, &(gTaskL3bfdfContext.sta60Min)) == FAILURE)
				HCU_ERROR_PRINT_L3BFDF("L3BFDF: Save data to DB error!\n");

		//再发送统计报告
		msg_struct_l3bfdf_cloudvela_statistic_report_t snd;
		memset(&snd, 0, sizeof(msg_struct_l3bfdf_cloudvela_statistic_report_t));

		//L2信息
		strncpy(snd.comHead.destUser, zHcuSysEngPar.cloud.svrNameDefault, strlen(zHcuSysEngPar.cloud.svrNameDefault)<\
			sizeof(snd.comHead.destUser)?strlen(zHcuSysEngPar.cloud.svrNameDefault):sizeof(snd.comHead.destUser));
		strncpy(snd.comHead.srcUser, zHcuSysEngPar.hwBurnId.equLable, strlen(zHcuSysEngPar.hwBurnId.equLable)<\
				sizeof(snd.comHead.srcUser)?strlen(zHcuSysEngPar.hwBurnId.equLable):sizeof(snd.comHead.srcUser));
		snd.comHead.timeStamp = time(0);
		snd.comHead.msgType = HUITP_MSG_HUIXML_MSGTYPE_DEVICE_REPORT_ID;
		strcpy(snd.comHead.funcFlag, "0");

		//CONTENT
		snd.dataFormat = HUITP_IEID_UNI_COM_FORMAT_TYPE_FLOAT_WITH_NF2;
		snd.baseReport = HUITP_IEID_UNI_COM_REPORT_YES;
		snd.staRepType = HUITP_IEID_UNI_SCALE_WEIGHT_STATISTIC_REPORT_60M;
		snd.sta.combTimes = gTaskL3bfdfContext.sta60Min.wsCombTimes;
		snd.sta.tttTimes = gTaskL3bfdfContext.sta60Min.wsTttTimes;
		snd.sta.tgvTimes = gTaskL3bfdfContext.sta60Min.wsTgvTimes;
		snd.sta.combMatNbr = gTaskL3bfdfContext.sta60Min.wsIncMatCnt;
		snd.sta.tttMatNbr = gTaskL3bfdfContext.sta60Min.wsTttMatCnt;
		snd.sta.tgvMatNbr = gTaskL3bfdfContext.sta60Min.wsTgvMatCnt;
		snd.sta.combAvgSpeed = gTaskL3bfdfContext.sta60Min.wsAvgTttTimes;
		snd.sta.totalWeight = gTaskL3bfdfContext.sta60Min.wsIncMatWgt;
		snd.sta.totalMatNbr = gTaskL3bfdfContext.sta60Min.wsIncMatCnt;
		snd.sta.totalWorkMin = (time(0) - gTaskL3bfdfContext.startWorkTimeInUnix)/60;
		snd.sta.timeInUnix = time(0);
		snd.sta.errNbr = 0;

		snd.length = sizeof(msg_struct_l3bfdf_cloudvela_statistic_report_t);
		HCU_MSG_SEND_GENERNAL_PROCESS(MSG_ID_L3BFDF_CLOUDVELA_STATISTIC_REPORT, TASK_ID_CLOUDVELA, TASK_ID_L3BFDF);

		//然后将60分钟统计数据表单清零，以便再次计数
		memset(&(gTaskL3bfdfContext.sta60Min), 0, sizeof(HcuSysMsgIeL3bfdfContextStaElement_t));
	}

	//更新24小时统计表单
	gTaskL3bfdfContext.sta24H.wsIncMatCnt += gTaskL3bfdfContext.cur.wsIncMatCnt;
	gTaskL3bfdfContext.sta24H.wsIncMatWgt += gTaskL3bfdfContext.cur.wsIncMatWgt;
	gTaskL3bfdfContext.sta24H.wsCombTimes += gTaskL3bfdfContext.cur.wsCombTimes;
	gTaskL3bfdfContext.sta24H.wsTttTimes  += gTaskL3bfdfContext.cur.wsTttTimes;
	gTaskL3bfdfContext.sta24H.wsTgvTimes  += gTaskL3bfdfContext.cur.wsTgvTimes;
	gTaskL3bfdfContext.sta24H.wsTttMatCnt += gTaskL3bfdfContext.cur.wsTttMatCnt;
	gTaskL3bfdfContext.sta24H.wsTgvMatCnt += gTaskL3bfdfContext.cur.wsTgvMatCnt;
	gTaskL3bfdfContext.sta24H.wsTttMatWgt += gTaskL3bfdfContext.cur.wsTttMatWgt;
	gTaskL3bfdfContext.sta24H.wsTgvMatWgt += gTaskL3bfdfContext.cur.wsTgvMatWgt;
	gTaskL3bfdfContext.sta24H.wsCallCellTimes += gTaskL3bfdfContext.cur.wsCallCellTimes;
	float timeRun24HourRatio = (float) HCU_L3BFDF_STA_24H_CYCLE / (float)(((gTaskL3bfdfContext.elipse24HourCnt%HCU_L3BFDF_STA_24H_CYCLE)==0)?HCU_L3BFDF_STA_24H_CYCLE:(gTaskL3bfdfContext.elipseCnt%HCU_L3BFDF_STA_24H_CYCLE));
	gTaskL3bfdfContext.sta24H.wsAvgTttTimes = (UINT32)(gTaskL3bfdfContext.sta24H.wsTttTimes*timeRun24HourRatio);
	gTaskL3bfdfContext.sta24H.wsAvgTttMatCnt = (UINT32)(gTaskL3bfdfContext.sta24H.wsTttMatCnt*timeRun24HourRatio);
	gTaskL3bfdfContext.sta24H.wsAvgTttMatWgt = (UINT32)(gTaskL3bfdfContext.sta24H.wsTttMatWgt*timeRun24HourRatio);

	//24小时统计表更新数据库
	if ((gTaskL3bfdfContext.elipseCnt % HCU_L3BFDF_STA_1M_CYCLE) == 0){
		if (dbi_HcuBfdf_StaDatainfo_save(HCU_L3BFDF_STA_DBI_TABLE_24HOUR, gTaskL3bfdfContext.configId, &(gTaskL3bfdfContext.sta24H)) == FAILURE)
				HCU_ERROR_PRINT_L3BFDF("L3BFDF: Save data to DB error!\n");
	}

	//24小时到了，然后将24小时统计数据表单清零，以便再次计数
	if (((time(0)-gTaskL3bfdfContext.start24hStaTimeInUnix) % HCU_L3BFDF_STA_24H_IN_SECOND) == 0){
		memset(&(gTaskL3bfdfContext.sta24H), 0, sizeof(HcuSysMsgIeL3bfdfContextStaElement_t));
		gTaskL3bfdfContext.elipse24HourCnt = 0;
	}

	//增加数据到连续统计数据中
	gTaskL3bfdfContext.staUp2Now.wsIncMatCnt += gTaskL3bfdfContext.cur.wsIncMatCnt;
	gTaskL3bfdfContext.staUp2Now.wsIncMatWgt += gTaskL3bfdfContext.cur.wsIncMatWgt;
	gTaskL3bfdfContext.staUp2Now.wsCombTimes += gTaskL3bfdfContext.cur.wsCombTimes;
	gTaskL3bfdfContext.staUp2Now.wsTttTimes  += gTaskL3bfdfContext.cur.wsTttTimes;
	gTaskL3bfdfContext.staUp2Now.wsTgvTimes  += gTaskL3bfdfContext.cur.wsTgvTimes;
	gTaskL3bfdfContext.staUp2Now.wsTttMatCnt += gTaskL3bfdfContext.cur.wsTttMatCnt;
	gTaskL3bfdfContext.staUp2Now.wsTgvMatCnt += gTaskL3bfdfContext.cur.wsTgvMatCnt;
	gTaskL3bfdfContext.staUp2Now.wsTttMatWgt += gTaskL3bfdfContext.cur.wsTttMatWgt;
	gTaskL3bfdfContext.staUp2Now.wsTgvMatWgt += gTaskL3bfdfContext.cur.wsTgvMatWgt;
	gTaskL3bfdfContext.staUp2Now.wsCallCellTimes += gTaskL3bfdfContext.cur.wsCallCellTimes;
	gTaskL3bfdfContext.staUp2Now.wsAvgTttTimes = gTaskL3bfdfContext.staUp2Now.wsTttTimes;
	gTaskL3bfdfContext.staUp2Now.wsAvgTttMatCnt = gTaskL3bfdfContext.staUp2Now.wsTttMatCnt;
	gTaskL3bfdfContext.staUp2Now.wsAvgTttMatWgt = gTaskL3bfdfContext.staUp2Now.wsTttMatWgt;

	//重要的统计功能挂载
	if ((gTaskL3bfdfContext.elipseCnt%HCU_L3BFDF_STATISTIC_PRINT_FREQUENCY) == 0){
		HCU_DEBUG_PRINT_FAT("L3BFDF: Control statistics, Total Up2Now Inc/Comb/TTT/TGV/CallCell-Cnt = [%d/%d/%d/%d/%d], TotalWgt=%lu, Comb/TTT/TGV Rate = [%5.2f%%/%5.2f%%/%5.2f%%], Local UI Shows AvgSpeed of [TTT Times/MatCnt/MatWgt] = %d/%d/%lu.\n",\
				gTaskL3bfdfContext.staUp2Now.wsIncMatCnt, \
				gTaskL3bfdfContext.staUp2Now.wsCombTimes, \
				gTaskL3bfdfContext.staUp2Now.wsTttTimes, \
				gTaskL3bfdfContext.staUp2Now.wsTgvTimes, \
				gTaskL3bfdfContext.staUp2Now.wsCallCellTimes, \
				gTaskL3bfdfContext.staUp2Now.wsIncMatWgt, \
				(float)(gTaskL3bfdfContext.staUp2Now.wsCombTimes)/(float)((gTaskL3bfdfContext.staUp2Now.wsIncMatCnt==0)?0.01:gTaskL3bfdfContext.staUp2Now.wsIncMatCnt) * 100.0, \
				(float)(gTaskL3bfdfContext.staUp2Now.wsTttMatCnt)/(float)((gTaskL3bfdfContext.staUp2Now.wsIncMatCnt==0)?0.01:gTaskL3bfdfContext.staUp2Now.wsIncMatCnt) * 100.0, \
				(float)(gTaskL3bfdfContext.staUp2Now.wsTgvMatCnt)/(float)((gTaskL3bfdfContext.staUp2Now.wsIncMatCnt==0)?0.01:gTaskL3bfdfContext.staUp2Now.wsIncMatCnt) * 100.0, \
				gTaskL3bfdfContext.staLocalUi.wsAvgTttTimes, \
				gTaskL3bfdfContext.staLocalUi.wsAvgTttMatCnt, \
				gTaskL3bfdfContext.staLocalUi.wsAvgTttMatWgt);
	}

	//将当前基础统计周期的数据清零
	memset(&(gTaskL3bfdfContext.cur), 0, sizeof(HcuSysMsgIeL3bfdfContextStaElement_t));

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
void func_l3bfdf_stm_main_recovery_from_fault(void)
{
	//控制其它模块进入正常状态

	//状态转移到SLEEP状态
	FsmSetState(TASK_ID_L3BFDF, FSM_STATE_L3BFDF_OOS_SCAN);

	//初始化模块的任务资源
	//初始化定时器：暂时决定不做，除非该模块重新RESTART
	//初始化模块级全局变量：暂时决定不做，除非该模块重新RESTART

	return;
}







