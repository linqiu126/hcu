/*
 * l3bfhs.c
 *
 *  Created on: 2015年11月22日
 *      Author: test
 */


#include "l3bfhs.h"

#include "../l0service/timer.h"
#include "../l0service/trace.h"
#include "../l1com/hwinv.h"
#include "../l2frame/cloudvela.h"


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
	{MSG_ID_UICOMM_L3BFHS_CMD_REQ,       		FSM_STATE_COMMON,          			fsm_l3bfhs_uicomm_cmd_req},
	{MSG_ID_CAN_L3BFHS_CAL_ZERO_RESP,      		FSM_STATE_COMMON,          			fsm_l3bfhs_canitf_cal_zero_resp},
	{MSG_ID_CAN_L3BFHS_CAL_FULL_RESP,       	FSM_STATE_COMMON,          			fsm_l3bfhs_canitf_cal_full_resp},

	//只为出现ACTIVED状态，入口自动被COMMON屏蔽
	{MSG_ID_CAN_L3BFHS_SYS_CFG_RESP,       		FSM_STATE_L3BFHS_ACTIVED,          	fsm_l3bfhs_canitf_sys_config_resp},
	{MSG_ID_CAN_L3BFHS_SYS_SUSPEND_RESP,       	FSM_STATE_L3BFHS_ACTIVED,          	fsm_l3bfhs_canitf_sys_suspend_resp},

	//进料组合态
	{MSG_ID_CAN_L3BFHS_WS_NEW_READY_EVENT,      FSM_STATE_L3BFHS_OOS_SCAN,          fsm_l3bfhs_canitf_ws_new_ready_event},

	//休眠状态：只允许收到RESUME指令
	{MSG_ID_CAN_L3BFHS_SYS_RESUME_RESP,       	FSM_STATE_L3BFHS_SUSPEND,          	fsm_l3bfhs_canitf_sys_resume_resp},

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
//	#if ((HCU_SYSCFG_BFHS_SNR_WS_NBR_MAX > HUITP_SCALE_WEIGHT_SENSOR_NBR_MAX) || (HCU_SYSCFG_BFHS_SNR_WS_NBR_MAX > HCU_SYSMSG_L3BFHS_MAX_SENSOR_NBR))
//		#error L3BFHS module level configuration number error!
//	#endif
//	//严格防止HUITP消息跟内部消息在关键结构上定义的不一致
//	if ((sizeof(StrIe_HUITP_IEID_uni_scale_weight_sta_element_t)) != (sizeof(msgie_struct_bfhs_scale_weight_sta_element_t)))
//		HCU_ERROR_PRINT_L3BFHS("L3BFHS: module message definition on statistic element error!\n");
	//严格保证统计周期的一致性
	if (HCU_L3BFHS_STA_UNIT_DUR != (10*zHcuSysEngPar.timer.array[TIMER_ID_10MS_L3BFHS_PERIOD_STA_SCAN].dur))  //静态表是以10ms为单位的
		HCU_ERROR_PRINT_L3BFHS("L3BFHS: module timer statistic parameter set error!\n");

	//秤盘数据表单控制表初始化
	memset(&gTaskL3bfhsContext, 0, sizeof(gTaskL3bfhsContext_t));

	//更新LocUI数据库，以便本地界面实时展示数据
//	if (dbi_HcuBfhs_StaDatainfo_save(HCU_L3BFHS_STA_DBI_TABLE_LOCALUI, gTaskL3bfhsContext.configId, &(gTaskL3bfhsContext.staLocalUi)) == FAILURE)
//			HCU_ERROR_PRINT_L3BFHS("L3BFHS: Save data to DB error!\n");

	//初始化界面交互数据
//	dbi_HcuBfhs_WmcStatusForceInvalid();
//	dbi_HcuBfhs_WmcCurComWgtUpdate(0);
//	HCU_DEBUG_PRINT_INF("L3BFHS: dbi_HcuBfhs_WmcStatusForceInvalid() set.\n");

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
	sprintf(s, "BOFENGZHINENG-%s-%4.2f-%04d.%02d.%02d.%02d:%02d:%02d", gTaskL3bfhsContext.configName, (float)gTaskL3bfhsContext.wgtSnrPar.MinAllowedWeight, \
			(UINT16)(1900+cu->tm_year), (UINT8)cu->tm_mon, (UINT8)cu->tm_mday, (UINT8)cu->tm_hour, (UINT8)cu->tm_min, (UINT8)cu->tm_sec);
	hcu_sps232_send_char_to_ext_printer(s, strlen(s));

	return SUCCESS;
}

//暂时啥都不干，定时到达后，还不明确是否需要支持定时工作
OPSTAT fsm_l3bfhs_time_out(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret;

	//Receive message and copy to local variable
	msg_struct_com_time_out_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_com_time_out_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_com_time_out_t))){
		HCU_ERROR_PRINT_L3BFHS("L3BFHS: Receive message error!\n");
	}
	memcpy(&rcv, param_ptr, param_len);

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
	//int ret=0;

	msg_struct_can_l3bfhs_sys_cfg_resp_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_can_l3bfhs_sys_cfg_resp_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_can_l3bfhs_sys_cfg_resp_t))){
		HCU_ERROR_PRINT_L3BFHS_RECOVERY("L3BFhs: Receive message error!\n");
	}
	memcpy(&rcv, param_ptr, param_len);

	//收到错误的反馈，就回复差错给界面
	if (rcv.validFlag == FALSE){
		//发送反馈给UICOMM
		msg_struct_l3bfhs_uicomm_cmd_resp_t snd;
		memset(&snd, 0, sizeof(msg_struct_l3bfhs_uicomm_cmd_resp_t));
		snd.cmdid = HCU_SYSMSG_BFHS_UICOMM_CMDID_CFG_START;
		snd.validFlag = FALSE;
		snd.errCode = rcv.errCode;
		snd.length = sizeof(msg_struct_l3bfhs_uicomm_cmd_resp_t);
		if (hcu_message_send(MSG_ID_L3BFHS_UICOMM_CMD_RESP, TASK_ID_BFHSUICOMM, TASK_ID_L3BFHS, &snd, snd.length) == FAILURE)
			HCU_ERROR_PRINT_L3BFHS_RECOVERY("L3BFHS: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3BFHS].taskName, zHcuVmCtrTab.task[TASK_ID_BFHSUICOMM].taskName);

		//停止定时器
		hcu_timer_stop(TASK_ID_L3BFHS, TIMER_ID_1S_L3BFHS_CFG_START_WAIT_FB, TIMER_RESOLUTION_1S);
		//设置状态机
		FsmSetState(TASK_ID_L3BFHS, FSM_STATE_L3BFHS_ACTIVED);
	}

	//收到正确的反馈
	else{
		//发送反馈给UICOMM
		msg_struct_l3bfhs_uicomm_cmd_resp_t snd;
		memset(&snd, 0, sizeof(msg_struct_l3bfhs_uicomm_cmd_resp_t));
		snd.cmdid = HCU_SYSMSG_BFHS_UICOMM_CMDID_CFG_START;
		snd.validFlag = TRUE;
		snd.length = sizeof(msg_struct_l3bfhs_uicomm_cmd_resp_t);
		if (hcu_message_send(MSG_ID_L3BFHS_UICOMM_CMD_RESP, TASK_ID_BFHSUICOMM, TASK_ID_L3BFHS, &snd, snd.length) == FAILURE)
			HCU_ERROR_PRINT_L3BFHS_RECOVERY("L3BFHS: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3BFHS].taskName, zHcuVmCtrTab.task[TASK_ID_BFHSUICOMM].taskName);

		//停止定时器
		hcu_timer_stop(TASK_ID_L3BFHS, TIMER_ID_1S_L3BFHS_CFG_START_WAIT_FB, TIMER_RESOLUTION_1S);
		//设置状态机
		FsmSetState(TASK_ID_L3BFHS, FSM_STATE_L3BFHS_OOS_SCAN);

		//批次Session+1：它的初始化应该存入数据库表单
		gTaskL3bfhsContext.callCellId++;
		//将session+1的结果存入数据库

		//启动统计扫描定时器
		hcu_timer_start(TASK_ID_L3BFHS, TIMER_ID_10MS_L3BFHS_PERIOD_STA_SCAN, zHcuSysEngPar.timer.array[TIMER_ID_10MS_L3BFHS_PERIOD_STA_SCAN].dur, TIMER_TYPE_PERIOD, TIMER_RESOLUTION_10MS);
		//设置工作启动时间
		gTaskL3bfhsContext.startWorkTimeInUnix = time(0);
	}

	//返回
	return SUCCESS;
}

OPSTAT fsm_l3bfhs_canitf_sys_suspend_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret=0;
	msg_struct_can_l3bfhs_sys_suspend_resp_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_can_l3bfhs_sys_suspend_resp_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_can_l3bfhs_sys_suspend_resp_t))){
		HCU_ERROR_PRINT_L3BFHS_RECOVERY("L3BFHS: Receive message error!\n");
	}
	memcpy(&rcv, param_ptr, param_len);

	//收到错误的反馈，就回复差错给界面
	if (rcv.validFlag == FALSE){
		//发送反馈给UICOMM
		msg_struct_l3bfhs_uicomm_cmd_resp_t snd;
		memset(&snd, 0, sizeof(msg_struct_l3bfhs_uicomm_cmd_resp_t));
		snd.validFlag = FALSE;
		snd.errCode = rcv.errCode;
		snd.cmdid = HCU_SYSMSG_BFHS_UICOMM_CMDID_SUSPEND;
		snd.length = sizeof(msg_struct_l3bfhs_uicomm_cmd_resp_t);
		if (hcu_message_send(MSG_ID_L3BFHS_UICOMM_CMD_RESP, TASK_ID_BFHSUICOMM, TASK_ID_L3BFHS, &snd, snd.length) == FAILURE)
			HCU_ERROR_PRINT_L3BFHS_RECOVERY("L3BFHS: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3BFHS].taskName, zHcuVmCtrTab.task[TASK_ID_BFHSUICOMM].taskName);

		//停止定时器
		hcu_timer_stop(TASK_ID_L3BFHS, TIMER_ID_1S_L3BFHS_SUSPEND_WAIT_FB, TIMER_RESOLUTION_1S);
		//设置状态机
		FsmSetState(TASK_ID_L3BFHS, FSM_STATE_L3BFHS_ACTIVED);
	}

	//收到正确的反馈
	else{
		//发送反馈给UICOMM
		msg_struct_l3bfhs_uicomm_cmd_resp_t snd;
		memset(&snd, 0, sizeof(msg_struct_l3bfhs_uicomm_cmd_resp_t));
		snd.cmdid = HCU_SYSMSG_BFHS_UICOMM_CMDID_SUSPEND;
		snd.validFlag = TRUE;
		snd.length = sizeof(msg_struct_l3bfhs_uicomm_cmd_resp_t);
		if (hcu_message_send(MSG_ID_L3BFHS_UICOMM_CMD_RESP, TASK_ID_BFHSUICOMM, TASK_ID_L3BFHS, &snd, snd.length) == FAILURE)
			HCU_ERROR_PRINT_L3BFHS_RECOVERY("L3BFHS: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3BFHS].taskName, zHcuVmCtrTab.task[TASK_ID_BFHSUICOMM].taskName);

		//停止定时器
		hcu_timer_stop(TASK_ID_L3BFHS, TIMER_ID_1S_L3BFHS_SUSPEND_WAIT_FB, TIMER_RESOLUTION_1S);
		//设置状态机
		FsmSetState(TASK_ID_L3BFHS, FSM_STATE_L3BFHS_SUSPEND);
	}

	//返回
	return SUCCESS;
}

OPSTAT fsm_l3bfhs_canitf_sys_resume_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret=0;
	msg_struct_can_l3bfhs_sys_resume_resp_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_can_l3bfhs_sys_resume_resp_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_can_l3bfhs_sys_resume_resp_t))){
		HCU_ERROR_PRINT_L3BFHS_RECOVERY("L3BFHS: Receive message error!\n");
	}

	//收到错误的反馈，就回复差错给界面
	if (rcv.validFlag == FALSE){
		//发送反馈给UICOMM
		msg_struct_l3bfhs_uicomm_cmd_resp_t snd;
		memset(&snd, 0, sizeof(msg_struct_l3bfhs_uicomm_cmd_resp_t));
		snd.validFlag = FALSE;
		snd.errCode = rcv.errCode;
		snd.cmdid = HCU_SYSMSG_BFHS_UICOMM_CMDID_RESUME;
		snd.length = sizeof(msg_struct_l3bfhs_uicomm_cmd_resp_t);
		if (hcu_message_send(MSG_ID_L3BFHS_UICOMM_CMD_RESP, TASK_ID_BFHSUICOMM, TASK_ID_L3BFHS, &snd, snd.length) == FAILURE)
			HCU_ERROR_PRINT_L3BFHS_RECOVERY("L3BFHS: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3BFHS].taskName, zHcuVmCtrTab.task[TASK_ID_BFHSUICOMM].taskName);

		//停止定时器
		hcu_timer_stop(TASK_ID_L3BFHS, TIMER_ID_1S_L3BFHS_RESUME_WAIT_FB, TIMER_RESOLUTION_1S);
		//设置状态机
		if (FsmSetState(TASK_ID_L3BFHS, FSM_STATE_L3BFHS_ACTIVED) == FAILURE){
			HCU_ERROR_PRINT_L3BFHS_RECOVERY("L3BFHS: Error Set FSM State!\n");
		}
	}

	//收到正确的反馈
	else{
		//发送反馈给UICOMM
		msg_struct_l3bfhs_uicomm_cmd_resp_t snd;
		memset(&snd, 0, sizeof(msg_struct_l3bfhs_uicomm_cmd_resp_t));
		snd.cmdid = HCU_SYSMSG_BFHS_UICOMM_CMDID_RESUME;
		snd.validFlag = TRUE;
		snd.length = sizeof(msg_struct_l3bfhs_uicomm_cmd_resp_t);
		if (hcu_message_send(MSG_ID_L3BFHS_UICOMM_CMD_RESP, TASK_ID_BFHSUICOMM, TASK_ID_L3BFHS, &snd, snd.length) == FAILURE)
			HCU_ERROR_PRINT_L3BFHS_RECOVERY("L3BFHS: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3BFHS].taskName, zHcuVmCtrTab.task[TASK_ID_BFHSUICOMM].taskName);

		//停止定时器
		hcu_timer_stop(TASK_ID_L3BFHS, TIMER_ID_1S_L3BFHS_RESUME_WAIT_FB, TIMER_RESOLUTION_1S);

		//设置状态机
		if (FsmSetState(TASK_ID_L3BFHS, FSM_STATE_L3BFHS_OOS_SCAN) == FAILURE){
			HCU_ERROR_PRINT_L3BFHS_RECOVERY("L3BFHS: Error Set FSM State!\n");
		}
	}

	//返回
	return SUCCESS;
}

OPSTAT fsm_l3bfhs_canitf_cal_zero_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret=0;
	msg_struct_can_l3bfhs_cal_zero_resp_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_can_l3bfhs_cal_zero_resp_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_can_l3bfhs_cal_zero_resp_t))){
		HCU_ERROR_PRINT_L3BFHS_RECOVERY("L3BFHS: Receive message error!\n");
	}

	//收到错误的反馈，就回复差错给界面
	if (rcv.validFlag == FALSE){
		//发送反馈给UICOMM
		msg_struct_l3bfhs_uicomm_cmd_resp_t snd;
		memset(&snd, 0, sizeof(msg_struct_l3bfhs_uicomm_cmd_resp_t));
		snd.validFlag = FALSE;
		snd.errCode = rcv.errCode;
		snd.cmdid = HCU_SYSMSG_BFHS_UICOMM_CMDID_CAL_ZERO;
		snd.length = sizeof(msg_struct_l3bfhs_uicomm_cmd_resp_t);
		if (hcu_message_send(MSG_ID_L3BFHS_UICOMM_CMD_RESP, TASK_ID_BFHSUICOMM, TASK_ID_L3BFHS, &snd, snd.length) == FAILURE)
			HCU_ERROR_PRINT_L3BFHS_RECOVERY("L3BFHS: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3BFHS].taskName, zHcuVmCtrTab.task[TASK_ID_BFHSUICOMM].taskName);

		//停止定时器
		hcu_timer_stop(TASK_ID_L3BFHS, TIMER_ID_1S_L3BFHS_CAL_ZERO_WAIT_FB, TIMER_RESOLUTION_1S);
	}

	//收到正确的反馈
	else{
		//发送反馈给UICOMM
		msg_struct_l3bfhs_uicomm_cmd_resp_t snd;
		memset(&snd, 0, sizeof(msg_struct_l3bfhs_uicomm_cmd_resp_t));
		snd.cmdid = HCU_SYSMSG_BFHS_UICOMM_CMDID_CAL_ZERO;
		snd.validFlag = TRUE;
		snd.length = sizeof(msg_struct_l3bfhs_uicomm_cmd_resp_t);
		if (hcu_message_send(MSG_ID_L3BFHS_UICOMM_CMD_RESP, TASK_ID_BFHSUICOMM, TASK_ID_L3BFHS, &snd, snd.length) == FAILURE)
			HCU_ERROR_PRINT_L3BFHS_RECOVERY("L3BFHS: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3BFHS].taskName, zHcuVmCtrTab.task[TASK_ID_BFHSUICOMM].taskName);

		//停止定时器
		hcu_timer_stop(TASK_ID_L3BFHS, TIMER_ID_1S_L3BFHS_CAL_ZERO_WAIT_FB, TIMER_RESOLUTION_1S);
	}

	//返回
	return SUCCESS;
}

OPSTAT fsm_l3bfhs_canitf_cal_full_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret=0;
	msg_struct_can_l3bfhs_cal_full_resp_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_can_l3bfhs_cal_full_resp_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_can_l3bfhs_cal_full_resp_t))){
		HCU_ERROR_PRINT_L3BFHS_RECOVERY("L3BFHS: Receive message error!\n");
	}

	//收到错误的反馈，就回复差错给界面
	if (rcv.validFlag == FALSE){
		//发送反馈给UICOMM
		msg_struct_l3bfhs_uicomm_cmd_resp_t snd;
		memset(&snd, 0, sizeof(msg_struct_l3bfhs_uicomm_cmd_resp_t));
		snd.validFlag = FALSE;
		snd.errCode = rcv.errCode;
		snd.cmdid = HCU_SYSMSG_BFHS_UICOMM_CMDID_CAL_FULL;
		memcpy(&snd.calFullRespPar, &rcv.calFullRespPar, sizeof(StrMsgIe_WeightSensorBfhsCalibrationFullRespParamaters_t));
		snd.length = sizeof(msg_struct_l3bfhs_uicomm_cmd_resp_t);
		if (hcu_message_send(MSG_ID_L3BFHS_UICOMM_CMD_RESP, TASK_ID_BFHSUICOMM, TASK_ID_L3BFHS, &snd, snd.length) == FAILURE)
			HCU_ERROR_PRINT_L3BFHS_RECOVERY("L3BFHS: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3BFHS].taskName, zHcuVmCtrTab.task[TASK_ID_BFHSUICOMM].taskName);

		//停止定时器
		hcu_timer_stop(TASK_ID_L3BFHS, TIMER_ID_1S_L3BFHS_CAL_ZERO_WAIT_FB, TIMER_RESOLUTION_1S);
	}

	//收到正确的反馈
	else{
		//发送反馈给UICOMM
		msg_struct_l3bfhs_uicomm_cmd_resp_t snd;
		memset(&snd, 0, sizeof(msg_struct_l3bfhs_uicomm_cmd_resp_t));
		snd.cmdid = HCU_SYSMSG_BFHS_UICOMM_CMDID_CAL_FULL;
		snd.validFlag = TRUE;
		memcpy(&snd.calFullRespPar, &rcv.calFullRespPar, sizeof(StrMsgIe_WeightSensorBfhsCalibrationFullRespParamaters_t));
		snd.length = sizeof(msg_struct_l3bfhs_uicomm_cmd_resp_t);
		if (hcu_message_send(MSG_ID_L3BFHS_UICOMM_CMD_RESP, TASK_ID_BFHSUICOMM, TASK_ID_L3BFHS, &snd, snd.length) == FAILURE)
			HCU_ERROR_PRINT_L3BFHS_RECOVERY("L3BFHS: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3BFHS].taskName, zHcuVmCtrTab.task[TASK_ID_BFHSUICOMM].taskName);

		//停止定时器
		hcu_timer_stop(TASK_ID_L3BFHS, TIMER_ID_1S_L3BFHS_CAL_FULL_WAIT_FB, TIMER_RESOLUTION_1S);
	}

	//返回
	return SUCCESS;
}

/***************************************************************************************************************************
 *
 *  CLOUDVELA部分的消息处理
 *
 ***************************************************************************************************************************/
//后台来的命令，发送到CANITFLEO模块，执行命令后，再返回给后台
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
	memset(&rcv, 0, sizeof(msg_struct_cloudvela_l3bfhs_ctrl_req_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_cloudvela_l3bfhs_ctrl_req_t))){
		HCU_ERROR_PRINT_L3BFHS_RECOVERY("L3BFHS: Receive message error!\n");
	}
	memcpy(&rcv, param_ptr, param_len);
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
//	ret = hcu_message_send(MSG_ID_L3BFHS_CAN_GENERAL_CMD_REQ, TASK_ID_CANITFLEO, TASK_ID_L3BFHS, &snd, snd.length);
//	if (ret == FAILURE){
//		HCU_ERROR_PRINT_L3BFHS_RECOVERY("L3BFHS: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3BFHS].taskName, zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName);
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
	//int ret=0;

	//入参检查
	msg_struct_can_l3bfhs_new_ready_event_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_can_l3bfhs_new_ready_event_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_can_l3bfhs_new_ready_event_t))){
		HCU_ERROR_PRINT_L3BFHS_RECOVERY("L3BFHS: Receive message error!\n");
	}
	memcpy(&rcv, param_ptr, param_len);

	//正常处理
	//存入数据库表单

	//返回
	return SUCCESS;
}

/***************************************************************************************************************************
 *
 *  本地界面UI通信部分
 *
 ***************************************************************************************************************************/
OPSTAT fsm_l3bfhs_uicomm_cmd_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret=0;

	msg_struct_uicomm_l3bfhs_cmd_req_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_uicomm_l3bfhs_cmd_req_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_uicomm_l3bfhs_cmd_req_t))){
		HCU_ERROR_PRINT_L3BFHS("L3BFHS: Receive message error!\n");
	}
	memcpy(&rcv, param_ptr, param_len);

	//根据收到的命令，分别进行控制
	//state = FsmGetState(TASK_ID_L3BFHS);

	//启动命令
	if (rcv.cmdid == HCU_SYSMSG_BFHS_UICOMM_CMDID_CFG_START){
		msg_struct_l3bfhs_can_sys_cfg_req_t snd;
		memset(&snd, 0, sizeof(msg_struct_l3bfhs_can_sys_cfg_req_t));
		snd.length = sizeof(msg_struct_l3bfhs_can_sys_cfg_req_t);
		if (hcu_message_send(MSG_ID_L3BFHS_CAN_SYS_CFG_REQ, TASK_ID_CANITFLEO, TASK_ID_L3BFHS, &snd, snd.length) == FAILURE)
			HCU_ERROR_PRINT_L3BFHS("L3BFHS: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3BFHS].taskName, zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName);
		//启动定时器
		hcu_timer_start(TASK_ID_L3BFHS, TIMER_ID_1S_L3BFHS_CFG_START_WAIT_FB, zHcuSysEngPar.timer.array[TIMER_ID_1S_L3BFHS_CFG_START_WAIT_FB].dur, TIMER_TYPE_ONE_TIME, TIMER_RESOLUTION_1S);
	}

	//SUSPEND
	else if (rcv.cmdid == HCU_SYSMSG_BFHS_UICOMM_CMDID_SUSPEND){
		msg_struct_l3bfhs_can_sys_suspend_req_t snd;
		memset(&snd, 0, sizeof(msg_struct_l3bfhs_can_sys_suspend_req_t));
		snd.length = sizeof(msg_struct_l3bfhs_can_sys_suspend_req_t);
		if (hcu_message_send(MSG_ID_L3BFHS_CAN_SYS_SUSPEND_REQ, TASK_ID_CANITFLEO, TASK_ID_L3BFHS, &snd, snd.length) == FAILURE)
			HCU_ERROR_PRINT_L3BFHS("L3BFHS: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3BFHS].taskName, zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName);
		//启动定时器
		hcu_timer_start(TASK_ID_L3BFHS, TIMER_ID_1S_L3BFHS_SUSPEND_WAIT_FB, zHcuSysEngPar.timer.array[TIMER_ID_1S_L3BFHS_SUSPEND_WAIT_FB].dur, TIMER_TYPE_ONE_TIME, TIMER_RESOLUTION_1S);
	}

	//RESUME
	else if (rcv.cmdid == HCU_SYSMSG_BFHS_UICOMM_CMDID_RESUME){
		msg_struct_l3bfhs_can_sys_resume_req_t snd;
		memset(&snd, 0, sizeof(msg_struct_l3bfhs_can_sys_resume_req_t));
		snd.length = sizeof(msg_struct_l3bfhs_can_sys_resume_req_t);
		if (hcu_message_send(MSG_ID_L3BFHS_CAN_SYS_RESUME_REQ, TASK_ID_CANITFLEO, TASK_ID_L3BFHS, &snd, snd.length) == FAILURE)
			HCU_ERROR_PRINT_L3BFHS("L3BFHS: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3BFHS].taskName, zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName);
		//启动定时器
		hcu_timer_start(TASK_ID_L3BFHS, TIMER_ID_1S_L3BFHS_RESUME_WAIT_FB, zHcuSysEngPar.timer.array[TIMER_ID_1S_L3BFHS_RESUME_WAIT_FB].dur, TIMER_TYPE_ONE_TIME, TIMER_RESOLUTION_1S);
	}

	//CAL_ZERO
	else if (rcv.cmdid == HCU_SYSMSG_BFHS_UICOMM_CMDID_CAL_ZERO){
		msg_struct_l3bfhs_can_cal_zero_req_t snd;
		memset(&snd, 0, sizeof(msg_struct_l3bfhs_can_cal_zero_req_t));
		memcpy(&snd.calZeroPar, &rcv.calZeroPar, sizeof(StrMsgIe_WeightSensorBfhsCalibrationZeroParamaters_t));
		snd.length = sizeof(msg_struct_l3bfhs_can_cal_zero_req_t);
		if (hcu_message_send(MSG_ID_L3BFHS_CAN_CAL_ZERO_REQ, TASK_ID_CANITFLEO, TASK_ID_L3BFHS, &snd, snd.length) == FAILURE)
			HCU_ERROR_PRINT_L3BFHS("L3BFHS: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3BFHS].taskName, zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName);
		//启动定时器
		hcu_timer_start(TASK_ID_L3BFHS, TIMER_ID_1S_L3BFHS_CAL_ZERO_WAIT_FB, zHcuSysEngPar.timer.array[TIMER_ID_1S_L3BFHS_CAL_ZERO_WAIT_FB].dur, TIMER_TYPE_ONE_TIME, TIMER_RESOLUTION_1S);
	}

	//CAL_FULL
	else if (rcv.cmdid == HCU_SYSMSG_BFHS_UICOMM_CMDID_CAL_FULL){
		msg_struct_l3bfhs_can_cal_full_req_t snd;
		memset(&snd, 0, sizeof(msg_struct_l3bfhs_can_cal_full_req_t));
		memcpy(&snd.calFullPar, &rcv.calFullPar, sizeof(StrMsgIe_WeightSensorBfhsCalibrationFullParamaters_t));
		snd.length = sizeof(msg_struct_l3bfhs_can_cal_full_req_t);
		if (hcu_message_send(MSG_ID_L3BFHS_CAN_CAL_FULL_REQ, TASK_ID_CANITFLEO, TASK_ID_L3BFHS, &snd, snd.length) == FAILURE)
			HCU_ERROR_PRINT_L3BFHS("L3BFHS: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3BFHS].taskName, zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName);
		//启动定时器
		hcu_timer_start(TASK_ID_L3BFHS, TIMER_ID_1S_L3BFHS_CAL_FULL_WAIT_FB, zHcuSysEngPar.timer.array[TIMER_ID_1S_L3BFHS_CAL_FULL_WAIT_FB].dur, TIMER_TYPE_ONE_TIME, TIMER_RESOLUTION_1S);
	}

	//差错
	else HCU_ERROR_PRINT_L3BFHS("L3BFHS: Receive message error!\n");

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
	msg_struct_l3bfhs_uicomm_cmd_resp_t snd;
	memset(&snd, 0, sizeof(msg_struct_l3bfhs_uicomm_cmd_resp_t));
	snd.cmdid = HCU_SYSMSG_BFHS_UICOMM_CMDID_CFG_START;
	snd.validFlag = FALSE;
	snd.errCode = HCU_SYSMSG_BFHS_ERR_CODE_TIME_OUT;
	snd.length = sizeof(msg_struct_l3bfhs_uicomm_cmd_resp_t);
	if (hcu_message_send(MSG_ID_L3BFHS_UICOMM_CMD_RESP, TASK_ID_BFHSUICOMM, TASK_ID_L3BFHS, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_L3BFHS_RECOVERY("L3BFHS: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3BFHS].taskName, zHcuVmCtrTab.task[TASK_ID_BFHSUICOMM].taskName);

	//设置状态机
	FsmSetState(TASK_ID_L3BFHS, FSM_STATE_L3BFHS_ACTIVED);

	//返回
	return SUCCESS;
}

OPSTAT func_l3bfhs_time_out_sys_suspend_wait_fb_process(void)
{
	//发送反馈给UICOMM
	msg_struct_l3bfhs_uicomm_cmd_resp_t snd;
	memset(&snd, 0, sizeof(msg_struct_l3bfhs_uicomm_cmd_resp_t));
	snd.cmdid = HCU_SYSMSG_BFHS_UICOMM_CMDID_SUSPEND;
	snd.validFlag = FALSE;
	snd.errCode = HCU_SYSMSG_BFHS_ERR_CODE_TIME_OUT;
	snd.length = sizeof(msg_struct_l3bfhs_uicomm_cmd_resp_t);
	if (hcu_message_send(MSG_ID_L3BFHS_UICOMM_CMD_RESP, TASK_ID_BFHSUICOMM, TASK_ID_L3BFHS, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_L3BFHS_RECOVERY("L3BFHS: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3BFHS].taskName, zHcuVmCtrTab.task[TASK_ID_BFHSUICOMM].taskName);

	//设置状态机
	FsmSetState(TASK_ID_L3BFHS, FSM_STATE_L3BFHS_ACTIVED);

	//返回
	return SUCCESS;
}

OPSTAT func_l3bfhs_time_out_sys_resume_wait_fb_process(void)
{
	//发送反馈给UICOMM
	msg_struct_l3bfhs_uicomm_cmd_resp_t snd;
	memset(&snd, 0, sizeof(msg_struct_l3bfhs_uicomm_cmd_resp_t));
	snd.cmdid = HCU_SYSMSG_BFHS_UICOMM_CMDID_RESUME;
	snd.validFlag = FALSE;
	snd.errCode = HCU_SYSMSG_BFHS_ERR_CODE_TIME_OUT;
	snd.length = sizeof(msg_struct_l3bfhs_uicomm_cmd_resp_t);
	if (hcu_message_send(MSG_ID_L3BFHS_UICOMM_CMD_RESP, TASK_ID_BFHSUICOMM, TASK_ID_L3BFHS, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_L3BFHS_RECOVERY("L3BFHS: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3BFHS].taskName, zHcuVmCtrTab.task[TASK_ID_BFHSUICOMM].taskName);

	//设置状态机
	FsmSetState(TASK_ID_L3BFHS, FSM_STATE_L3BFHS_ACTIVED);

	//返回
	return SUCCESS;
}

OPSTAT func_l3bfhs_time_out_cal_zero_wait_fb_process(void)
{
	//发送反馈给UICOMM
	msg_struct_l3bfhs_uicomm_cmd_resp_t snd;
	memset(&snd, 0, sizeof(msg_struct_l3bfhs_uicomm_cmd_resp_t));
	snd.cmdid = HCU_SYSMSG_BFHS_UICOMM_CMDID_CAL_ZERO;
	snd.validFlag = FALSE;
	snd.errCode = HCU_SYSMSG_BFHS_ERR_CODE_TIME_OUT;
	snd.length = sizeof(msg_struct_l3bfhs_uicomm_cmd_resp_t);
	if (hcu_message_send(MSG_ID_L3BFHS_UICOMM_CMD_RESP, TASK_ID_BFHSUICOMM, TASK_ID_L3BFHS, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_L3BFHS_RECOVERY("L3BFHS: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3BFHS].taskName, zHcuVmCtrTab.task[TASK_ID_BFHSUICOMM].taskName);

	//不设置状态机

	//返回
	return SUCCESS;
}

OPSTAT func_l3bfhs_time_out_cal_full_wait_fb_process(void)
{
	//发送反馈给UICOMM
	msg_struct_l3bfhs_uicomm_cmd_resp_t snd;
	memset(&snd, 0, sizeof(msg_struct_l3bfhs_uicomm_cmd_resp_t));
	snd.cmdid = HCU_SYSMSG_BFHS_UICOMM_CMDID_CAL_FULL;
	snd.validFlag = FALSE;
	snd.errCode = HCU_SYSMSG_BFHS_ERR_CODE_TIME_OUT;
	snd.length = sizeof(msg_struct_l3bfhs_uicomm_cmd_resp_t);
	if (hcu_message_send(MSG_ID_L3BFHS_UICOMM_CMD_RESP, TASK_ID_BFHSUICOMM, TASK_ID_L3BFHS, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_L3BFHS_RECOVERY("L3BFHS: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3BFHS].taskName, zHcuVmCtrTab.task[TASK_ID_BFHSUICOMM].taskName);

	//不设置状态机

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
	int ret = 0;

	//首先增加时间流逝的计数器
	gTaskL3bfhsContext.elipseCnt++;
	gTaskL3bfhsContext.elipse24HourCnt++;

	//采取老化算法 x(n+1) = x(n) * (1-1/120) + latest，从而得到最新的数据，但该数据最好使用float，然后再转换为UINT32存入到数据库表单中
	memset(&(gTaskL3bfhsContext.staLocalUi), 0, sizeof(HcuSysMsgIeL3bfhsContextStaElement_t));
	//先存储临时数据
	gTaskL3bfhsContext.curAge.wsIncMatCntMid = gTaskL3bfhsContext.curAge.wsIncMatCntMid*(float)HCU_L3BFHS_STA_AGEING_COEF + (float)HCU_L3BFHS_STA_AGEING_COEF_ALPHA*gTaskL3bfhsContext.cur.wsIncMatCnt;
	gTaskL3bfhsContext.curAge.wsIncMatWgtMid = gTaskL3bfhsContext.curAge.wsIncMatWgtMid*(float)HCU_L3BFHS_STA_AGEING_COEF + (float)HCU_L3BFHS_STA_AGEING_COEF_ALPHA*gTaskL3bfhsContext.cur.wsIncMatWgt;
	gTaskL3bfhsContext.curAge.wsCombTimesMid = gTaskL3bfhsContext.curAge.wsCombTimesMid*(float)HCU_L3BFHS_STA_AGEING_COEF + (float)HCU_L3BFHS_STA_AGEING_COEF_ALPHA*gTaskL3bfhsContext.cur.wsCombTimes;
	gTaskL3bfhsContext.curAge.wsTttTimesMid = gTaskL3bfhsContext.curAge.wsTttTimesMid*(float)HCU_L3BFHS_STA_AGEING_COEF + (float)HCU_L3BFHS_STA_AGEING_COEF_ALPHA*gTaskL3bfhsContext.cur.wsTttTimes;
	gTaskL3bfhsContext.curAge.wsTgvTimesMid = gTaskL3bfhsContext.curAge.wsTgvTimesMid*(float)HCU_L3BFHS_STA_AGEING_COEF + (float)HCU_L3BFHS_STA_AGEING_COEF_ALPHA*gTaskL3bfhsContext.cur.wsTgvTimes;
	gTaskL3bfhsContext.curAge.wsTttMatCntMid = gTaskL3bfhsContext.curAge.wsTttMatCntMid*(float)HCU_L3BFHS_STA_AGEING_COEF + (float)HCU_L3BFHS_STA_AGEING_COEF_ALPHA*gTaskL3bfhsContext.cur.wsTttMatCnt;
	gTaskL3bfhsContext.curAge.wsTgvMatCntMid = gTaskL3bfhsContext.curAge.wsTgvMatCntMid*(float)HCU_L3BFHS_STA_AGEING_COEF + (float)HCU_L3BFHS_STA_AGEING_COEF_ALPHA*gTaskL3bfhsContext.cur.wsTgvMatCnt;
	gTaskL3bfhsContext.curAge.wsTttMatWgtMid = gTaskL3bfhsContext.curAge.wsTttMatWgtMid*(float)HCU_L3BFHS_STA_AGEING_COEF + (float)HCU_L3BFHS_STA_AGEING_COEF_ALPHA*gTaskL3bfhsContext.cur.wsTttMatWgt;
	gTaskL3bfhsContext.curAge.wsTgvMatWgtMid = gTaskL3bfhsContext.curAge.wsTgvMatWgtMid*(float)HCU_L3BFHS_STA_AGEING_COEF + (float)HCU_L3BFHS_STA_AGEING_COEF_ALPHA*gTaskL3bfhsContext.cur.wsTgvMatWgt;
	//再输出结果
	gTaskL3bfhsContext.staLocalUi.wsIncMatCnt = (UINT32)(gTaskL3bfhsContext.curAge.wsIncMatCntMid + 0.5);
	gTaskL3bfhsContext.staLocalUi.wsIncMatWgt = (UINT32)(gTaskL3bfhsContext.curAge.wsIncMatWgtMid + 0.5);
	gTaskL3bfhsContext.staLocalUi.wsCombTimes = (UINT32)(gTaskL3bfhsContext.curAge.wsCombTimesMid + 0.5);
	gTaskL3bfhsContext.staLocalUi.wsTttTimes  = (UINT32)(gTaskL3bfhsContext.curAge.wsTttTimesMid + 0.5);
	gTaskL3bfhsContext.staLocalUi.wsTgvTimes  = (UINT32)(gTaskL3bfhsContext.curAge.wsTgvTimesMid + 0.5);
	gTaskL3bfhsContext.staLocalUi.wsTttMatCnt = (UINT32)(gTaskL3bfhsContext.curAge.wsTttMatCntMid + 0.5);
	gTaskL3bfhsContext.staLocalUi.wsTgvMatCnt = (UINT32)(gTaskL3bfhsContext.curAge.wsTgvMatCntMid + 0.5);
	gTaskL3bfhsContext.staLocalUi.wsTttMatWgt = (UINT32)(gTaskL3bfhsContext.curAge.wsTttMatWgtMid + 0.5);
	gTaskL3bfhsContext.staLocalUi.wsTgvMatWgt = (UINT32)(gTaskL3bfhsContext.curAge.wsTgvMatWgtMid + 0.5);
	gTaskL3bfhsContext.staLocalUi.wsAvgTttTimes = gTaskL3bfhsContext.staLocalUi.wsTttTimes;
	gTaskL3bfhsContext.staLocalUi.wsAvgTttMatCnt = gTaskL3bfhsContext.staLocalUi.wsTttMatCnt;
	gTaskL3bfhsContext.staLocalUi.wsAvgTttMatWgt = gTaskL3bfhsContext.staLocalUi.wsTttMatWgt;

	//更新LocUI数据库，以便本地界面实时展示数据
//	if (dbi_HcuBfhs_StaDatainfo_save(HCU_L3BFHS_STA_DBI_TABLE_LOCALUI, gTaskL3bfhsContext.configId, &(gTaskL3bfhsContext.staLocalUi)) == FAILURE)
//			HCU_ERROR_PRINT_L3BFHS("L3BFHS: Save data to DB error!\n");

	//更新60Min各个统计表单
	gTaskL3bfhsContext.sta60Min.wsIncMatCnt += gTaskL3bfhsContext.cur.wsIncMatCnt;
	gTaskL3bfhsContext.sta60Min.wsIncMatWgt += gTaskL3bfhsContext.cur.wsIncMatWgt;
	gTaskL3bfhsContext.sta60Min.wsCombTimes += gTaskL3bfhsContext.cur.wsCombTimes;
	gTaskL3bfhsContext.sta60Min.wsTttTimes  += gTaskL3bfhsContext.cur.wsTttTimes;
	gTaskL3bfhsContext.sta60Min.wsTgvTimes  += gTaskL3bfhsContext.cur.wsTgvTimes;
	gTaskL3bfhsContext.sta60Min.wsTttMatCnt += gTaskL3bfhsContext.cur.wsTttMatCnt;
	gTaskL3bfhsContext.sta60Min.wsTgvMatCnt += gTaskL3bfhsContext.cur.wsTgvMatCnt;
	gTaskL3bfhsContext.sta60Min.wsTttMatWgt += gTaskL3bfhsContext.cur.wsTttMatWgt;
	gTaskL3bfhsContext.sta60Min.wsTgvMatWgt += gTaskL3bfhsContext.cur.wsTgvMatWgt;
	float timeRun60MinRatio = (float) HCU_L3BFHS_STA_60M_CYCLE / (float)(((gTaskL3bfhsContext.elipseCnt%HCU_L3BFHS_STA_60M_CYCLE)==0)?HCU_L3BFHS_STA_60M_CYCLE:(gTaskL3bfhsContext.elipseCnt%HCU_L3BFHS_STA_60M_CYCLE));
	gTaskL3bfhsContext.sta60Min.wsAvgTttTimes = (UINT32)(gTaskL3bfhsContext.sta60Min.wsTttTimes*timeRun60MinRatio);
	gTaskL3bfhsContext.sta60Min.wsAvgTttMatCnt = (UINT32)(gTaskL3bfhsContext.sta60Min.wsTttMatCnt*timeRun60MinRatio);
	gTaskL3bfhsContext.sta60Min.wsAvgTttMatWgt = (UINT32)(gTaskL3bfhsContext.sta60Min.wsTttMatWgt*timeRun60MinRatio);

	//60分钟到了，发送统计报告到后台：发送后台只需要一种统计报告即可，重复发送意义不大
	if ((gTaskL3bfhsContext.elipseCnt % HCU_L3BFHS_STA_60M_CYCLE) == 0){
		//60分统计表更新数据库
//		if (dbi_HcuBfhs_StaDatainfo_save(HCU_L3BFHS_STA_DBI_TABLE_60MIN, gTaskL3bfhsContext.configId, &(gTaskL3bfhsContext.sta60Min)) == FAILURE)
//				HCU_ERROR_PRINT_L3BFHS("L3BFHS: Save data to DB error!\n");

		//再发送统计报告
		msg_struct_l3bfhs_cloudvela_statistic_report_t snd;
		memset(&snd, 0, sizeof(msg_struct_l3bfhs_cloudvela_statistic_report_t));

		//L2信息
//		strncpy(snd.comHead.destUser, zHcuSysEngPar.cloud.svrNameDefault, strlen(zHcuSysEngPar.cloud.svrNameDefault)<sizeof(snd.comHead.destUser)?strlen(zHcuSysEngPar.cloud.svrNameDefault):sizeof(snd.comHead.destUser));
//		strncpy(snd.comHead.srcUser, zHcuSysEngPar.hwBurnId.equLable, strlen(zHcuSysEngPar.hwBurnId.equLable)<sizeof(snd.comHead.srcUser)?strlen(zHcuSysEngPar.hwBurnId.equLable):sizeof(snd.comHead.srcUser));
		snd.comHead.timeStamp = time(0);
		snd.comHead.msgType = HUITP_MSG_HUIXML_MSGTYPE_DEVICE_REPORT_ID;
		strcpy(snd.comHead.funcFlag, "0");

		//CONTENT
		snd.dataFormat = HUITP_IEID_UNI_COM_FORMAT_TYPE_FLOAT_WITH_NF2;
		snd.baseReport = HUITP_IEID_UNI_COM_REPORT_YES;
		snd.staRepType = HUITP_IEID_UNI_SCALE_WEIGHT_STATISTIC_REPORT_60M;
//		snd.sta.combTimes = gTaskL3bfhsContext.sta60Min.wsCombTimes;
//		snd.sta.tttTimes = gTaskL3bfhsContext.sta60Min.wsTttTimes;
//		snd.sta.tgvTimes = gTaskL3bfhsContext.sta60Min.wsTgvTimes;
//		snd.sta.combMatNbr = gTaskL3bfhsContext.sta60Min.wsIncMatCnt;
//		snd.sta.tttMatNbr = gTaskL3bfhsContext.sta60Min.wsTttMatCnt;
//		snd.sta.tgvMatNbr = gTaskL3bfhsContext.sta60Min.wsTgvMatCnt;
//		snd.sta.combAvgSpeed = gTaskL3bfhsContext.sta60Min.wsAvgTttTimes;
//		snd.sta.totalWeight = (UINT32)(gTaskL3bfhsContext.sta60Min.wsIncMatWgt*100);
//		snd.sta.totalMatNbr = gTaskL3bfhsContext.sta60Min.wsIncMatCnt;
//		snd.sta.totalWorkMin = (time(0) - gTaskL3bfhsContext.startWorkTimeInUnix)/60;
//		snd.sta.timeInUnix = time(0);
//		snd.sta.errNbr = 0;

		snd.length = sizeof(msg_struct_l3bfhs_cloudvela_statistic_report_t);
		ret = hcu_message_send(MSG_ID_L3BFHS_CLOUDVELA_STATISTIC_REPORT, TASK_ID_CLOUDVELA, TASK_ID_L3BFHS, &snd, snd.length);
		if (ret == FAILURE){
			HCU_ERROR_PRINT_L3BFHS("L3BFHS: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3BFHS].taskName, zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName);
		}

		//然后将60分钟统计数据表单清零，以便再次计数
		memset(&(gTaskL3bfhsContext.sta60Min), 0, sizeof(HcuSysMsgIeL3bfhsContextStaElement_t));
	}

	//更新24小时统计表单
	gTaskL3bfhsContext.sta24H.wsIncMatCnt += gTaskL3bfhsContext.cur.wsIncMatCnt;
	gTaskL3bfhsContext.sta24H.wsIncMatWgt += gTaskL3bfhsContext.cur.wsIncMatWgt;
	gTaskL3bfhsContext.sta24H.wsCombTimes += gTaskL3bfhsContext.cur.wsCombTimes;
	gTaskL3bfhsContext.sta24H.wsTttTimes  += gTaskL3bfhsContext.cur.wsTttTimes;
	gTaskL3bfhsContext.sta24H.wsTgvTimes  += gTaskL3bfhsContext.cur.wsTgvTimes;
	gTaskL3bfhsContext.sta24H.wsTttMatCnt += gTaskL3bfhsContext.cur.wsTttMatCnt;
	gTaskL3bfhsContext.sta24H.wsTgvMatCnt += gTaskL3bfhsContext.cur.wsTgvMatCnt;
	gTaskL3bfhsContext.sta24H.wsTttMatWgt += gTaskL3bfhsContext.cur.wsTttMatWgt;
	gTaskL3bfhsContext.sta24H.wsTgvMatWgt += gTaskL3bfhsContext.cur.wsTgvMatWgt;
	float timeRun24HourRatio = (float) HCU_L3BFHS_STA_24H_CYCLE / (float)(((gTaskL3bfhsContext.elipse24HourCnt%HCU_L3BFHS_STA_24H_CYCLE)==0)?HCU_L3BFHS_STA_24H_CYCLE:(gTaskL3bfhsContext.elipseCnt%HCU_L3BFHS_STA_24H_CYCLE));
	gTaskL3bfhsContext.sta24H.wsAvgTttTimes = (UINT32)(gTaskL3bfhsContext.sta24H.wsTttTimes*timeRun24HourRatio);
	gTaskL3bfhsContext.sta24H.wsAvgTttMatCnt = (UINT32)(gTaskL3bfhsContext.sta24H.wsTttMatCnt*timeRun24HourRatio);
	gTaskL3bfhsContext.sta24H.wsAvgTttMatWgt = (UINT32)(gTaskL3bfhsContext.sta24H.wsTttMatWgt*timeRun24HourRatio);

	//24小时统计表更新数据库
	if ((gTaskL3bfhsContext.elipseCnt % HCU_L3BFHS_STA_1M_CYCLE) == 0){
//		if (dbi_HcuBfhs_StaDatainfo_save(HCU_L3BFHS_STA_DBI_TABLE_24HOUR, gTaskL3bfhsContext.configId, &(gTaskL3bfhsContext.sta24H)) == FAILURE)
//				HCU_ERROR_PRINT_L3BFHS("L3BFHS: Save data to DB error!\n");
	}

	//24小时到了，然后将24小时统计数据表单清零，以便再次计数
	if (((time(0)-gTaskL3bfhsContext.start24hStaTimeInUnix) % HCU_L3BFHS_STA_24H_IN_SECOND) == 0){
		memset(&(gTaskL3bfhsContext.sta24H), 0, sizeof(HcuSysMsgIeL3bfhsContextStaElement_t));
		gTaskL3bfhsContext.elipse24HourCnt = 0;
		printf("this is a test, to show enter into wrong loop!\n");
	}

	//24小时到了，应该发送单独的统计报告，未来完善

	//增加数据到连续统计数据中
	gTaskL3bfhsContext.staUp2Now.wsIncMatCnt += gTaskL3bfhsContext.cur.wsIncMatCnt;
	gTaskL3bfhsContext.staUp2Now.wsIncMatWgt += gTaskL3bfhsContext.cur.wsIncMatWgt;
	gTaskL3bfhsContext.staUp2Now.wsCombTimes += gTaskL3bfhsContext.cur.wsCombTimes;
	gTaskL3bfhsContext.staUp2Now.wsTttTimes  += gTaskL3bfhsContext.cur.wsTttTimes;
	gTaskL3bfhsContext.staUp2Now.wsTgvTimes  += gTaskL3bfhsContext.cur.wsTgvTimes;
	gTaskL3bfhsContext.staUp2Now.wsTttMatCnt += gTaskL3bfhsContext.cur.wsTttMatCnt;
	gTaskL3bfhsContext.staUp2Now.wsTgvMatCnt += gTaskL3bfhsContext.cur.wsTgvMatCnt;
	gTaskL3bfhsContext.staUp2Now.wsTttMatWgt += gTaskL3bfhsContext.cur.wsTttMatWgt;
	gTaskL3bfhsContext.staUp2Now.wsTgvMatWgt += gTaskL3bfhsContext.cur.wsTgvMatWgt;
	gTaskL3bfhsContext.staUp2Now.wsAvgTttTimes = gTaskL3bfhsContext.staUp2Now.wsTttTimes;
	gTaskL3bfhsContext.staUp2Now.wsAvgTttMatCnt = gTaskL3bfhsContext.staUp2Now.wsTttMatCnt;
	gTaskL3bfhsContext.staUp2Now.wsAvgTttMatWgt = gTaskL3bfhsContext.staUp2Now.wsTttMatWgt;

	//更新连续数据库数据库：每5秒存储一次，不然数据库操作过于频繁
	if ((gTaskL3bfhsContext.elipseCnt % HCU_L3BFHS_STA_5S_CYCLE) == 0){
//		if (dbi_HcuBfhs_StaDatainfo_save(HCU_L3BFHS_STA_DBI_TABLE_UP2NOW, gTaskL3bfhsContext.configId, &(gTaskL3bfhsContext.staUp2Now)) == FAILURE)
//				HCU_ERROR_PRINT_L3BFHS("L3BFHS: Save data to DB error!\n");
	}

	//重要的统计功能挂载
	if ((gTaskL3bfhsContext.elipseCnt%HCU_L3BFHS_STATISTIC_PRINT_FREQUENCY) == 0)
//	HCU_DEBUG_PRINT_CRT("L3BFHS: Control statistics, Running Free/Weight/Ttt/Tgu = [%d, %d, %d, %d], Total Up2Now Inc Cnt = [%d], Combination Rate = [%5.2f\%], Give-up Rate = [%5.2f\%], Local UI Shows AvgSpeed of [TTT Times/MatCnt/MatWgt] = %d/%d/%5.2f.\n",\
//			gTaskL3bfhsContext.wsValueNbrFree, gTaskL3bfhsContext.wsValueNbrWeight,\
//			gTaskL3bfhsContext.wsValueNbrTtt, gTaskL3bfhsContext.wsValueNbrTgu, gTaskL3bfhsContext.staUp2Now.wsIncMatCnt, \
//			(float)(gTaskL3bfhsContext.staUp2Now.wsTttMatCnt)/(float)((gTaskL3bfhsContext.staUp2Now.wsIncMatCnt==0)?0.01:gTaskL3bfhsContext.staUp2Now.wsIncMatCnt) * 100, \
//			(float)(gTaskL3bfhsContext.staUp2Now.wsTgvMatCnt)/(float)((gTaskL3bfhsContext.staUp2Now.wsIncMatCnt==0)?0.01:gTaskL3bfhsContext.staUp2Now.wsIncMatCnt) * 100, \
//			gTaskL3bfhsContext.staLocalUi.wsAvgTttTimes, gTaskL3bfhsContext.staLocalUi.wsAvgTttMatCnt, gTaskL3bfhsContext.staLocalUi.wsAvgTttMatWgt);
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




