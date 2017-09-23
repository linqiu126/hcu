/*
 * l3bfdf.c
 *
 *  Created on: 2015年11月22日
 *      Author: test
 */


#include "l3bfdf.h"

#include "../l0service/timer.h"
#include "../l0service/trace.h"
#include "../l1com/hwinv.h"
#include "../l2frame/cloudvela.h"

/*
** FSM of the L3BFDF
*/
HcuFsmStateItem_t HcuFsmL3bfdf[] =
{
    //MessageId                 //State                   		 		//Function
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
	{MSG_ID_UICOMM_L3BFDF_CMD_REQ,       		FSM_STATE_COMMON,          			fsm_l3bfdf_uicomm_cmd_req},

	//只为出现ACTIVED状态，入口自动被COMMON屏蔽
	{MSG_ID_CAN_L3BFDF_SYS_CFG_RESP,       		FSM_STATE_L3BFDF_ACTIVED,          	fsm_l3bfdf_canitf_sys_config_resp},
	{MSG_ID_CAN_L3BFDF_SYS_SUSPEND_RESP,       	FSM_STATE_L3BFDF_ACTIVED,          	fsm_l3bfdf_canitf_sys_suspend_resp},//这个是先转移状态，再发送命令

	//进料组合态
	{MSG_ID_CAN_L3BFDF_WS_NEW_READY_EVENT,      FSM_STATE_L3BFDF_OOS_SCAN,          fsm_l3bfdf_canitf_ws_new_ready_event},
	{MSG_ID_CAN_L3BFDF_WS_COMB_OUT_FB,       	FSM_STATE_L3BFDF_OOS_SCAN,          fsm_l3bfdf_canitf_ws_comb_out_fb},
	{MSG_ID_CAN_L3BFDF_SNC_PULLIN_RESP,       	FSM_STATE_L3BFDF_OOS_SCAN,          fsm_l3bfdf_canitf_snc_pulliin_resp},
	{MSG_ID_CAN_L3BFDF_BASKET_CLEAN_IND,       	FSM_STATE_L3BFDF_OOS_SCAN,          fsm_l3bfdf_canitf_basket_clean_ind},

	//出料流程态
	//如果出现两个重叠出料情况，而且第一个出料的返回消息还未回来，没有影响，继续执行，定时器只是覆盖而已
	{MSG_ID_CAN_L3BFDF_WS_NEW_READY_EVENT,      FSM_STATE_L3BFDF_OOS_TTT,          	fsm_l3bfdf_canitf_ws_new_ready_event},
	{MSG_ID_CAN_L3BFDF_WS_COMB_OUT_FB,       	FSM_STATE_L3BFDF_OOS_TTT,          	fsm_l3bfdf_canitf_ws_comb_out_fb},
	{MSG_ID_CAN_L3BFDF_SNC_PULLIN_RESP,       	FSM_STATE_L3BFDF_OOS_TTT,           fsm_l3bfdf_canitf_snc_pulliin_resp},
	{MSG_ID_CAN_L3BFDF_BASKET_CLEAN_IND,       	FSM_STATE_L3BFDF_OOS_TTT,           fsm_l3bfdf_canitf_basket_clean_ind},

	//休眠状态：只允许收到RESUME指令，不更新界面广告牌以及数据库
	{MSG_ID_CAN_L3BFDF_SYS_RESUME_RESP,       	FSM_STATE_L3BFDF_SUSPEND,          	fsm_l3bfdf_canitf_sys_resume_resp},//这个是先发送命令，收齐后再转移状态

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
	//严格检查内部消息设置的大小，是否越界
	#if ((HCU_SYSMSG_L3BFDF_MAX_STREAM_NBR < HCU_SYSCFG_BFDF_EQU_FLOW_NBR_MAX) || (HCU_SYSMSG_L3BFDF_MAX_SNC_BOARD_NBR < HCU_SYSCFG_BFDF_SNC_BOARD_NBR_MAX))
		#error L3BFDF module level configuration number error!
	#endif

//	//严格防止HUITP消息跟内部消息在关键结构上定义的不一致
//	if ((sizeof(StrIe_HUITP_IEID_uni_scale_weight_sta_element_t)) != (sizeof(msgie_struct_bfdf_scale_weight_sta_element_t)))
//		HCU_ERROR_PRINT_L3BFDF("L3BFDF: module message definition on statistic element error!\n");
//	//严格保证统计周期的一致性
//	if (HCU_L3BFDF_STA_UNIT_DUR != (10*zHcuSysEngPar.timer.array[TIMER_ID_10MS_L3BFDF_PERIOD_STA_SCAN].dur))  //静态表是以10ms为单位的
//		HCU_ERROR_PRINT_L3BFDF("L3BFDF: module timer statistic parameter set error!\n");

	//秤盘数据表单控制表初始化
	memset(&gTaskL3bfdfContext, 0, sizeof(gTaskL3bfdfContext_t));


	//更新LocUI数据库，以便本地界面实时展示数据

	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_L3BFDF, FSM_STATE_L3BFDF_ACTIVED) == FAILURE)
		HCU_ERROR_PRINT_L3BFDF("L3BFDF: Error Set FSM State!\n");
	HCU_DEBUG_PRINT_FAT("L3BFDF: Enter FSM_STATE_L3BFDF_ACTIVED status, Keeping refresh here!\n");

	//测试组合程序

	//返回
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
/*
	char s[100];
	time_t lt;
	struct tm *cu;
	memset(s, 0, sizeof(s));

	//初始本地时间
	lt=time(NULL);
	cu = localtime(&lt);
	cu->tm_mon = cu->tm_mon + 1; //月份是从0-11的，+1是为了符合正常逻辑
	sprintf(s, "BOFENGZHINENG-%s-%4.2f-%04d.%02d.%02d.%02d:%02d:%02d", gTaskL3bfdfContext.configName, (float)gTaskL3bfdfContext.comAlgPar.TargetCombinationWeight, \
			(UINT16)(1900+cu->tm_year), (UINT8)cu->tm_mon, (UINT8)cu->tm_mday, (UINT8)cu->tm_hour, (UINT8)cu->tm_min, (UINT8)cu->tm_sec);
	hcu_sps232_send_char_to_ext_printer(s, strlen(s));
*/

	return SUCCESS;
}

//暂时啥都不干，定时到达后，还不明确是否需要支持定时工作
OPSTAT fsm_l3bfdf_time_out(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret;

	//Receive message and copy to local variable
	msg_struct_com_time_out_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_com_time_out_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_com_time_out_t))){
		HCU_ERROR_PRINT_L3BFDF("L3BFDF: Receive message error!\n");
	}
	memcpy(&rcv, param_ptr, param_len);

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
			HCU_ERROR_PRINT_L3BFDF("L3BFDF: Error process time out message!\n");
		}
	}

	//等待料斗出料反馈：一次性工作
	else if ((rcv.timeId == TIMER_ID_1S_L3BFDF_TTT_WAIT_FB) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		if (func_l3bfdf_time_out_comb_out_req_process() == FAILURE){
			HCU_ERROR_PRINT_L3BFDF("L3BFDF: Error process time out message!\n");
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
OPSTAT fsm_l3bfdf_uicomm_cmd_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret=0;
	int i = 0, j = 0;
	//UINT8 state = 0;
	int total=0;
	char s[200], tmp[20];

	msg_struct_uicomm_l3bfdf_cmd_req_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_uicomm_l3bfdf_cmd_req_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_uicomm_l3bfdf_cmd_req_t))){
		HCU_ERROR_PRINT_L3BFDF("L3BFDF: Receive message error!\n");
	}
	memcpy(&rcv, param_ptr, param_len);

	//根据收到的命令，分别进行控制
	//state = FsmGetState(TASK_ID_L3BFDF);

	//启动命令
	if (rcv.cmdid == HCU_SYSMSG_BFDF_UICOMM_CMDID_CFG_START){
		msg_struct_l3bfdf_can_sys_cfg_req_t snd;
		memset(&snd, 0, sizeof(msg_struct_l3bfdf_can_sys_cfg_req_t));
		snd.length = sizeof(msg_struct_l3bfdf_can_sys_cfg_req_t);
		total=0;
		for (j = 0; j< HCU_SYSCFG_BFDF_EQU_FLOW_NBR_MAX; j++){
			for (i = 0; i< HCU_SYSCFG_BFDF_NODE_BOARD_NBR_MAX; i++){
				//所有的配置过后的传感器
				if (gTaskL3bfdfContext.nodeDyn[j][i].nodeStatus > HCU_L3BFDF_NODE_BOARD_STATUS_INIT_MIN){
					snd.boardBitmap[j][i] = TRUE;
					gTaskL3bfdfContext.nodeDyn[j][i].nodeStatus = HCU_L3BFDF_NODE_BOARD_STATUS_CFG_START_REQ;
					gTaskL3bfdfContext.nodeDyn[j][i].cfgRcvFlag = FALSE;
					total++;
				}
				else snd.boardBitmap[j][i] = FALSE;
			}
		}

		memset(s, 0, sizeof(s));
		sprintf(s, "L3BFDF: Total start sensor number = %d, bitmap = ", total);
		for (j = 0; j< HCU_SYSCFG_BFDF_EQU_FLOW_NBR_MAX; j++){
			for (i=0; i<HCU_SYSCFG_BFDF_NODE_BOARD_NBR_MAX; i++){
				memset(tmp, 0, sizeof(tmp));
				sprintf(tmp, "%d/", snd.boardBitmap[j][i]);
				if ((strlen(s)+strlen(tmp)) < sizeof(s)) strcat(s, tmp);
			}
		}
		strcat(s, "\n");
		HCU_DEBUG_PRINT_CRT(s);

		HCU_DEBUG_PRINT_CRT("L3BFDF: Total sensor to be start = %d\n", total);
		if (hcu_message_send(MSG_ID_L3BFDF_CAN_SYS_CFG_REQ, TASK_ID_CANITFLEO, TASK_ID_L3BFDF, &snd, snd.length) == FAILURE)
			HCU_ERROR_PRINT_L3BFDF("L3BFDF: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3BFDF].taskName, zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName);

		//启动定时器
		hcu_timer_start(TASK_ID_L3BFDF, TIMER_ID_1S_L3BFDF_CFG_START_WAIT_FB, zHcuSysEngPar.timer.array[TIMER_ID_1S_L3BFDF_CFG_START_WAIT_FB].dur, TIMER_TYPE_PERIOD, TIMER_RESOLUTION_1S);
	}

	//SUSPEND
	else if (rcv.cmdid == HCU_SYSMSG_BFDF_UICOMM_CMDID_SUSPEND){

	}

	//RESUME
	else if (rcv.cmdid == HCU_SYSMSG_BFDF_UICOMM_CMDID_RESUME){

	}
	//停止命令：针对所有的传感器来说的
	else if (rcv.cmdid == HCU_SYSMSG_BFDF_UICOMM_CMDID_STOP){
/*		msg_struct_l3bfdf_can_sys_stop_req_t snd;
		memset(&snd, 0, sizeof(msg_struct_l3bfdf_can_sys_stop_req_t));
		snd.length = sizeof(msg_struct_l3bfdf_can_sys_stop_req_t);
		total=0;
		for (i = 0; i< HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX; i++){
			//所有的非离线传感器
			if (gTaskL3bfdfContext.sensorWs[i].sensorStatus > HCU_L3BFDF_SENSOR_WS_STATUS_OFFLINE){
				snd.wsBitmap[i] = TRUE;
				gTaskL3bfdfContext.sensorWs[i].sensorStatus = HCU_L3BFDF_SENSOR_WS_STATUS_STOP_REQ;
				gTaskL3bfdfContext.sensorWs[i].stopRcvFlag = FALSE;
				total++;
			}
			else snd.wsBitmap[i] = FALSE;
		}

		memset(s, 0, sizeof(s));
		sprintf(s, "L3BFDF: Total stop sensor number = %d, bitmap = ", total);
		for (i=0; i<HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX; i++){
			memset(tmp, 0, sizeof(tmp));
			sprintf(tmp, "%d/", snd.wsBitmap[i]);
			if ((strlen(s)+strlen(tmp)) < sizeof(s)) strcat(s, tmp);
		}
		strcat(s, "\n");
		HCU_DEBUG_PRINT_CRT(s);

		ret = hcu_message_send(MSG_ID_L3BFDF_CAN_SYS_STOP_REQ, TASK_ID_CANITFLEO, TASK_ID_L3BFDF, &snd, snd.length);
		if (ret == FAILURE) HCU_ERROR_PRINT_L3BFDF("L3BFDF: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3BFDF].taskName, zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName);

		//启动定时器
		ret = hcu_timer_start(TASK_ID_L3BFDF, TIMER_ID_1S_L3BFDF_SYS_STOP_WAIT_FB, \
				zHcuSysEngPar.timer.array[TIMER_ID_1S_L3BFDF_SYS_STOP_WAIT_FB].dur, TIMER_TYPE_PERIOD, TIMER_RESOLUTION_1S);
		if (ret == FAILURE){
			HCU_ERROR_PRINT_L3BFDF("L3BFDF: Error start timer!\n");
		}

		//设置新状态
		if (FsmSetState(TASK_ID_L3BFDF, FSM_STATE_L3BFDF_OPR_GO) == FAILURE){
			HCU_ERROR_PRINT_L3BFDF("L3BFDF: Error Set FSM State!\n");
		}*/
	}

	//差错
	else HCU_ERROR_PRINT_L3BFDF("L3BFDF: Receive message error!\n");

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
OPSTAT fsm_l3bfdf_cloudvela_data_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	return SUCCESS;
}

//由于是内部消息命令执行，为了简化整个执行，不设置超时状态，以简化整个状态机的设计
OPSTAT fsm_l3bfdf_cloudvela_data_confirm(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	return SUCCESS;
}

//由于是内部消息命令执行，为了简化整个执行，不设置超时状态，以简化整个状态机的设计
OPSTAT fsm_l3bfdf_cloudvela_event_confirm(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	return SUCCESS;
}

//由于是内部消息命令执行，为了简化整个执行，不设置超时状态，以简化整个状态机的设计
OPSTAT fsm_l3bfdf_cloudvela_ctrl_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret=0;

	//入参检查
	msg_struct_cloudvela_l3bfdf_ctrl_req_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_cloudvela_l3bfdf_ctrl_req_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_cloudvela_l3bfdf_ctrl_req_t))){
		HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Receive message error!\n");
	}
	memcpy(&rcv, param_ptr, param_len);
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
//	ret = hcu_message_send(MSG_ID_L3BFDF_CAN_GENERAL_CMD_REQ, TASK_ID_CANITFLEO, TASK_ID_L3BFDF, &snd, snd.length);
//	if (ret == FAILURE){
//		HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3BFDF].taskName, zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName);
//	}

	//状态不转移

	//短时钟不启动

	//返回
	return SUCCESS;
}

//由于是内部消息命令执行，为了简化整个执行，不设置超时状态，以简化整个状态机的设计
OPSTAT fsm_l3bfdf_cloudvela_statistic_confirm(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
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
	int i = 0;

	msg_struct_can_l3bfdf_sys_cfg_resp_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_can_l3bfdf_sys_cfg_resp_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_can_l3bfdf_sys_cfg_resp_t))){
		HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Receive message error!\n");
	}
	if ((rcv.streamId < 0) || (rcv.streamId > HCU_SYSCFG_BFDF_EQU_FLOW_NBR_MAX)\
			|| (rcv.boardId < 0) || (rcv.boardId > HCU_SYSCFG_BFDF_NODE_BOARD_NBR_MAX))
			HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);

	//先改本传感器的状态
	gTaskL3bfdfContext.nodeDyn[rcv.streamId][rcv.boardId].nodeStatus = HCU_L3BFDF_NODE_BOARD_STATUS_CFG_START_CMPL;
	gTaskL3bfdfContext.nodeDyn[rcv.streamId][rcv.boardId].cfgRcvFlag = TRUE;

	//收到错误的反馈，就回复差错给界面
	if (rcv.validFlag == FALSE){
		//发送反馈给UICOMM
		msg_struct_l3bfdf_uicomm_cmd_resp_t snd;
		memset(&snd, 0, sizeof(msg_struct_l3bfdf_uicomm_cmd_resp_t));
		snd.cmdid = HCU_SYSMSG_BFDF_UICOMM_CMDID_CFG_START;
		snd.validFlag = FALSE;
		snd.errCode = rcv.errCode;
		snd.streamId = rcv.streamId;
		snd.sensorid = rcv.boardId;
		snd.length = sizeof(msg_struct_l3bfdf_uicomm_cmd_resp_t);
		if (hcu_message_send(MSG_ID_L3BFDF_UICOMM_CMD_RESP, TASK_ID_BFDFUICOMM, TASK_ID_L3BFDF, &snd, snd.length) == FAILURE)
			HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3BFDF].taskName, zHcuVmCtrTab.task[TASK_ID_BFDFUICOMM].taskName);

		//停止定时器
		hcu_timer_stop(TASK_ID_L3BFDF, TIMER_ID_1S_L3BFDF_CFG_START_WAIT_FB, TIMER_RESOLUTION_1S);
		//设置状态机
		FsmSetState(TASK_ID_L3BFDF, FSM_STATE_L3BFDF_ACTIVED);
	}

	//收到正确以及齐活的反馈
	if (func_l3bfdf_cacluate_sensor_cfg_start_rcv_complete() == TRUE){
		//发送反馈给UICOMM
		msg_struct_l3bfdf_uicomm_cmd_resp_t snd;
		memset(&snd, 0, sizeof(msg_struct_l3bfdf_uicomm_cmd_resp_t));
		snd.validFlag = TRUE;
		snd.length = sizeof(msg_struct_l3bfdf_uicomm_cmd_resp_t);
		if (hcu_message_send(MSG_ID_L3BFDF_UICOMM_CMD_RESP, TASK_ID_BFDFUICOMM, TASK_ID_L3BFDF, &snd, snd.length) == FAILURE)
			HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3BFDF].taskName, zHcuVmCtrTab.task[TASK_ID_BFDFUICOMM].taskName);

		//停止定时器
		hcu_timer_stop(TASK_ID_L3BFDF, TIMER_ID_1S_L3BFDF_CFG_START_WAIT_FB, TIMER_RESOLUTION_1S);

		//设置状态机
		FsmSetState(TASK_ID_L3BFDF, FSM_STATE_L3BFDF_OOS_SCAN);

		//设置所有料斗状态到工作状态
		for (i=0; i<HCU_SYSCFG_BFDF_EQU_FLOW_NBR_MAX; i++){
			if (func_l3bfdf_hopper_state_set_valid(i) == FALSE)
				HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Init global parameter error!\n");
		}
	}

	//收到正确以及没有齐活的反馈：直接返回

	//返回
	return SUCCESS;
}

OPSTAT fsm_l3bfdf_canitf_sys_suspend_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret=0;
	msg_struct_can_l3bfdf_sys_suspend_resp_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_can_l3bfdf_sys_suspend_resp_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_can_l3bfdf_sys_suspend_resp_t))){
		HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Receive message error!\n");
	}
	/*	if ((rcv.sensorid < 0) || (rcv.sensorid > HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX))
			HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);

	//先改本传感器的状态
	gTaskL3bfdfContext.sensorWs[rcv.sensorid].sensorStatus = HCU_L3BFDF_SENSOR_WS_STATUS_STOP_CMPL;
	gTaskL3bfdfContext.sensorWs[rcv.sensorid].stopRcvFlag = TRUE;

	//收到错误的反馈，就回复差错给界面
	if (rcv.validFlag == FALSE){
		//发送反馈给UICOMM
		msg_struct_l3bfdf_uicomm_cmd_resp_t snd;
		memset(&snd, 0, sizeof(msg_struct_l3bfdf_uicomm_cmd_resp_t));
		snd.validFlag = FALSE;
		snd.errCode = rcv.errCode;
		snd.sensorid = rcv.sensorid;
		snd.cmdid = HCU_SYSMSG_BFDF_UICOMM_CMDID_STOP;
		snd.length = sizeof(msg_struct_l3bfdf_uicomm_cmd_resp_t);
		ret = hcu_message_send(MSG_ID_L3BFDF_UICOMM_CMD_RESP, TASK_ID_BFDFUICOMM, TASK_ID_L3BFDF, &snd, snd.length);
		if (ret == FAILURE){
			HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3BFDF].taskName, zHcuVmCtrTab.task[TASK_ID_BFDFUICOMM].taskName);
		}

		//将传感器强制设置为错误状态
		gTaskL3bfdfContext.sensorWs[rcv.sensorid].sensorStatus = HCU_L3BFDF_SENSOR_WS_STATUS_INIT_ERR;

		//停止定时器
		ret = hcu_timer_stop(TASK_ID_L3BFDF, TIMER_ID_1S_L3BFDF_SYS_STOP_WAIT_FB, TIMER_RESOLUTION_1S);
		if (ret == FAILURE){
			HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Error stop timer!\n");
		}
		//设置状态机
		if (FsmSetState(TASK_ID_L3BFDF, FSM_STATE_L3BFDF_OPR_GO) == FAILURE){
			HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Error Set FSM State!\n");
		}
	}

	//收到正确以及齐活的反馈
	if (func_l3bfdf_cacluate_sensor_stop_rcv_complete() == TRUE){
		//发送反馈给UICOMM
		msg_struct_l3bfdf_uicomm_cmd_resp_t snd;
		memset(&snd, 0, sizeof(msg_struct_l3bfdf_uicomm_cmd_resp_t));
		snd.cmdid = HCU_SYSMSG_BFDF_UICOMM_CMDID_STOP;
		snd.validFlag = TRUE;
		snd.length = sizeof(msg_struct_l3bfdf_uicomm_cmd_resp_t);
		ret = hcu_message_send(MSG_ID_L3BFDF_UICOMM_CMD_RESP, TASK_ID_BFDFUICOMM, TASK_ID_L3BFDF, &snd, snd.length);
		if (ret == FAILURE){
			HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3BFDF].taskName, zHcuVmCtrTab.task[TASK_ID_BFDFUICOMM].taskName);
		}

		//停止定时器
		ret = hcu_timer_stop(TASK_ID_L3BFDF, TIMER_ID_1S_L3BFDF_SYS_STOP_WAIT_FB, TIMER_RESOLUTION_1S);
		if (ret == FAILURE){
			HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Error stop timer!\n");
		}
		//设置状态机
		if (FsmSetState(TASK_ID_L3BFDF, FSM_STATE_L3BFDF_OPR_GO) == FAILURE){
			HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Error Set FSM State!\n");
		}
	}*/

	//收到正确以及没有齐活的反馈：直接返回

	//返回
	return SUCCESS;
}

OPSTAT fsm_l3bfdf_canitf_sys_resume_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret=0;
	msg_struct_can_l3bfdf_sys_resume_resp_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_can_l3bfdf_sys_resume_resp_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_can_l3bfdf_sys_resume_resp_t))){
		HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Receive message error!\n");
	}
/*
	if ((rcv.sensorid < 0) || (rcv.sensorid > HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX))
			HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);

	//先改本传感器的状态
	gTaskL3bfdfContext.sensorWs[rcv.sensorid].sensorStatus = HCU_L3BFDF_SENSOR_WS_STATUS_VALIID_EMPTY;
	gTaskL3bfdfContext.sensorWs[rcv.sensorid].startRcvFlag = TRUE;

	//收到错误的反馈，就回复差错给界面
	if (rcv.validFlag == FALSE){
		//发送反馈给UICOMM
		msg_struct_l3bfdf_uicomm_cmd_resp_t snd;
		memset(&snd, 0, sizeof(msg_struct_l3bfdf_uicomm_cmd_resp_t));
		snd.validFlag = FALSE;
		snd.errCode = rcv.errCode;
		snd.sensorid = rcv.sensorid;
		snd.cmdid = HCU_SYSMSG_BFDF_UICOMM_CMDID_START;
		snd.length = sizeof(msg_struct_l3bfdf_uicomm_cmd_resp_t);
		ret = hcu_message_send(MSG_ID_L3BFDF_UICOMM_CMD_RESP, TASK_ID_BFDFUICOMM, TASK_ID_L3BFDF, &snd, snd.length);
		if (ret == FAILURE){
			HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3BFDF].taskName, zHcuVmCtrTab.task[TASK_ID_BFDFUICOMM].taskName);
		}

		//停止定时器
		ret = hcu_timer_stop(TASK_ID_L3BFDF, TIMER_ID_1S_L3BFDF_SYS_START_WAIT_FB, TIMER_RESOLUTION_1S);
		if (ret == FAILURE){
			HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Error stop timer!\n");
		}
		//设置状态机
		if (FsmSetState(TASK_ID_L3BFDF, FSM_STATE_L3BFDF_OPR_GO) == FAILURE){
			HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Error Set FSM State!\n");
		}
	}

	//收到正确以及齐活的反馈
	if (func_l3bfdf_cacluate_sensor_start_rcv_complete() == TRUE){
		//发送反馈给UICOMM
		msg_struct_l3bfdf_uicomm_cmd_resp_t snd;
		memset(&snd, 0, sizeof(msg_struct_l3bfdf_uicomm_cmd_resp_t));
		snd.cmdid = HCU_SYSMSG_BFDF_UICOMM_CMDID_START;
		snd.validFlag = TRUE;
		snd.length = sizeof(msg_struct_l3bfdf_uicomm_cmd_resp_t);
		ret = hcu_message_send(MSG_ID_L3BFDF_UICOMM_CMD_RESP, TASK_ID_BFDFUICOMM, TASK_ID_L3BFDF, &snd, snd.length);
		if (ret == FAILURE){
			HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3BFDF].taskName, zHcuVmCtrTab.task[TASK_ID_BFDFUICOMM].taskName);
		}

		//停止定时器
		ret = hcu_timer_stop(TASK_ID_L3BFDF, TIMER_ID_1S_L3BFDF_SYS_START_WAIT_FB, TIMER_RESOLUTION_1S);
		if (ret == FAILURE){
			HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Error stop timer!\n");
		}

		//启动统计扫描定时器
		ret = hcu_timer_start(TASK_ID_L3BFDF, TIMER_ID_10MS_L3BFDF_PERIOD_STA_SCAN, \
			zHcuSysEngPar.timer.array[TIMER_ID_10MS_L3BFDF_PERIOD_STA_SCAN].dur, TIMER_TYPE_PERIOD, TIMER_RESOLUTION_10MS);
		if (ret == FAILURE)
			HCU_ERROR_PRINT_L3BFDF("L3BFDF: Error start period timer!\n");

		//设置工作启动时间
		gTaskL3bfdfContext.startWorkTimeInUnix = time(0);

		//设置状态机
		if (FsmSetState(TASK_ID_L3BFDF, FSM_STATE_L3BFDF_OOS_SCAN) == FAILURE){
			HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Error Set FSM State!\n");
		}
	}
*/

	//收到正确以及没有齐活的反馈：直接返回

	//返回
	return SUCCESS;
}


//触发组合算法
OPSTAT fsm_l3bfdf_canitf_ws_new_ready_event(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret=0;
	double weight = 0;
	UINT16 outHopperId = 0;

	//入参检查
	msg_struct_can_l3bfdf_new_ready_event_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_can_l3bfdf_new_ready_event_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_can_l3bfdf_new_ready_event_t))){
		HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Receive message error!\n");
	}
	memcpy(&rcv, param_ptr, param_len);

	//差错检测
	if (rcv.streamId >= HCU_SYSCFG_BFDF_EQU_FLOW_NBR_MAX){
		HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Receive message error, StreamId = %d, WsWeight=%d!\n", rcv.streamId, rcv.sensorWsValue);
	}

	//先做Audit
	int res = func_l3bfdf_hopper_dual_chain_audit();
	if (res <0) HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Audit error, errCode = %d\n", res);

	//手工浏览一遍双链表，进行打印
	//func_l3bfdf_print_all_hopper_status_by_chain(0);
	//func_l3bfdf_print_all_hopper_status_by_chain(1);

	//超界限，扔进垃圾桶
	UINT16 gidMin = 1;
	UINT16 gidMax = gTaskL3bfdfContext.totalGroupNbr[rcv.streamId];
	weight = (double)rcv.sensorWsValue / 100.0;
	if ((weight < gTaskL3bfdfContext.group[rcv.streamId][gidMin].rangeLow) || (weight > gTaskL3bfdfContext.group[rcv.streamId][gidMax].rangeHigh))
	{
		if (func_l3bfdf_new_ws_send_out_pullin_message(rcv.streamId, 0) == FALSE)
			HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3BFDF].taskName, zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName);
		return SUCCESS;
	}

	//先分组，分到具体的组别
	UINT16 gId = func_l3bfdf_new_ws_search_group(rcv.streamId, weight);
	UINT16 fillHopper = 0;
	if ((gId == 0) || (gId > gidMax))
		HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Can not find right group! StreamId=%d, rcv.weight=%fg\n", rcv.streamId, (float)rcv.sensorWsValue/100.00);

	//看看是否有满的
	outHopperId = func_l3bfdf_new_ws_search_hopper_full(rcv.streamId);
	if (outHopperId != 0){
		gTaskL3bfdfContext.hopper[rcv.streamId][outHopperId].matLackIndex = 0;
		//发送出料
		if (func_l3bfdf_new_ws_send_out_comb_out_message(rcv.streamId, outHopperId) == FALSE){
			gTaskL3bfdfContext.hopper[rcv.streamId][outHopperId].hopperStatus = HCU_L3BFDF_HOPPER_STATUS_VALID_ERR;
			HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Send Comb Out message error!\n");
		}
		//设置状态
		FsmSetState(TASK_ID_L3BFDF, FSM_STATE_L3BFDF_OOS_TTT);
		//还需要继续处理收到的称重事件
	}

	//先搜索是否有欠一满：而且还必须状态正常
	outHopperId = func_l3bfdf_new_ws_search_hopper_lack_one(rcv.streamId, gId, weight);
	if (outHopperId != 0){
		//先发送出料：出料的定时机制，如何处理？这是重入过程：使用状态机，不用定时器
		if (func_l3bfdf_new_ws_send_out_pullin_message(rcv.streamId, outHopperId) == FALSE)
			HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Send out pullin message error!\n");
		//等待入料成功后，状态设置为满，即可激活出料过程
		gTaskL3bfdfContext.hopper[rcv.streamId][outHopperId].hopperStatus = HCU_L3BFDF_HOPPER_STATUS_FULL_PRE;
		//留待入料成功后，加入到目标重量区
		gTaskL3bfdfContext.hopper[rcv.streamId][outHopperId].hopperLastMat = weight;
		//清零Index，等待COMB_OUT成功后再恢复最大
		gTaskL3bfdfContext.hopper[rcv.streamId][outHopperId].matLackIndex = 0;
		//循环将fillHopper后移
		fillHopper = gTaskL3bfdfContext.group[rcv.streamId][gId].fillHopperId;
		gTaskL3bfdfContext.group[rcv.streamId][gId].fillHopperId = gTaskL3bfdfContext.hopper[rcv.streamId][fillHopper].nextHopperId;
		return SUCCESS;
	}

	//通过fillHopper进行搜索：深水区，需要判定料斗的欠缺深度
	//从fillHopper入手，寻找第一个可以满足区间控制的目标，然后填入
	outHopperId = func_l3bfdf_new_ws_search_hopper_valid_normal(rcv.streamId, gId, weight);
	if (outHopperId == 0){
		//扔进垃圾桶
		if (func_l3bfdf_new_ws_send_out_pullin_message(rcv.streamId, 0) == FALSE)
			HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3BFDF].taskName, zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName);
		//更新统计数据

		return SUCCESS;
	}

	//正确找到
	else{
		//先发送出料：出料的定时机制，如何处理？这是重入过程：使用状态机，不用定时器
		if (func_l3bfdf_new_ws_send_out_pullin_message(rcv.streamId, outHopperId) == FALSE)
			HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Send out pullin message error!\n");
		//等待入料成功后，状态设置为出料，即可激活出料过程
		gTaskL3bfdfContext.hopper[rcv.streamId][outHopperId].hopperStatus = HCU_L3BFDF_HOPPER_STATUS_PULLIN_OUT;
		//留待入料成功后，加入到目标重量区
		gTaskL3bfdfContext.hopper[rcv.streamId][outHopperId].hopperLastMat = weight;
		//欠一满不可能，不然该料斗已经成功进入出料状态了
		if (gTaskL3bfdfContext.hopper[rcv.streamId][outHopperId].matLackIndex <= 1){
			gTaskL3bfdfContext.hopper[rcv.streamId][outHopperId].hopperStatus = HCU_L3BFDF_HOPPER_STATUS_VALID_ERR;
			HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Hopper Status error, Stream/HopperId=[%d/%d]!\n", rcv.streamId, outHopperId);
		}
		//减少Index
		gTaskL3bfdfContext.hopper[rcv.streamId][outHopperId].matLackIndex--;
		return SUCCESS;
	}

/*
	//正常处理
	gTaskL3bfdfContext.cur.wsIncMatCnt++;
	gTaskL3bfdfContext.cur.wsIncMatWgt += rcv.sensorWsValue;
	HCU_DEBUG_PRINT_FAT("L3BFDF: Sensor Input Id = %d, Status = %d\n", rcv.sensorid, gTaskL3bfdfContext.sensorWs[rcv.sensorid].sensorStatus);

	//将重复次数清零：这个参数用于增加该传感器被算法命中的概率
	gTaskL3bfdfContext.sensorWs[rcv.sensorid].sensorRepTimes = 0;

	//根据不同情况进行处理：正常空闲态
	if (gTaskL3bfdfContext.sensorWs[rcv.sensorid].sensorStatus == HCU_L3BFDF_SENSOR_WS_STATUS_VALIID_EMPTY){
		gTaskL3bfdfContext.sensorWs[rcv.sensorid].sensorValue = rcv.sensorWsValue;
		gTaskL3bfdfContext.sensorWs[rcv.sensorid].sensorStatus = HCU_L3BFDF_SENSOR_WS_STATUS_VALID_TO_COMB;
		if (gTaskL3bfdfContext.wsValueNbrFree>0) gTaskL3bfdfContext.wsValueNbrFree--;
		gTaskL3bfdfContext.wsValueNbrWeight++;
		gTaskL3bfdfContext.wsValueNbrWeight = gTaskL3bfdfContext.wsValueNbrWeight % (HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX + 1);
		HCU_DEBUG_PRINT_NOR("L3BFDF: Receive EVENT, Normally status!\n");
	}

	//重复来临
	else if (gTaskL3bfdfContext.sensorWs[rcv.sensorid].sensorStatus == HCU_L3BFDF_SENSOR_WS_STATUS_VALID_TO_COMB){
		gTaskL3bfdfContext.sensorWs[rcv.sensorid].sensorValue = rcv.sensorWsValue;
		HCU_DEBUG_PRINT_NOR("L3BFDF: Receive EVENT, Repeat status!\n");
	}

	//恢复错误
	else if (gTaskL3bfdfContext.sensorWs[rcv.sensorid].sensorStatus == HCU_L3BFDF_SENSOR_WS_STATUS_VALID_ERROR){
		gTaskL3bfdfContext.sensorWs[rcv.sensorid].sensorValue = rcv.sensorWsValue;
		gTaskL3bfdfContext.sensorWs[rcv.sensorid].sensorStatus = HCU_L3BFDF_SENSOR_WS_STATUS_VALID_TO_COMB;
		gTaskL3bfdfContext.wsValueNbrWeight++;
		gTaskL3bfdfContext.wsValueNbrWeight = gTaskL3bfdfContext.wsValueNbrWeight % (HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX + 1);
		HCU_DEBUG_PRINT_NOR("L3BFDF: Receive EVENT, Error recover status!\n");
	}

	//错误来临：进入ERROR_INQ流程 (TTT/TGU或者其它状态）
	else
	{
		HCU_DEBUG_PRINT_NOR("L3BFDF: Receive EVENT, Error status!\n");
		//为了全局控制表统计的正确性
		if (gTaskL3bfdfContext.sensorWs[rcv.sensorid].sensorStatus == HCU_L3BFDF_SENSOR_WS_STATUS_VALID_TO_TTT) {
			if (gTaskL3bfdfContext.wsValueNbrTtt > 0) gTaskL3bfdfContext.wsValueNbrTtt--;
		}
		else if (gTaskL3bfdfContext.sensorWs[rcv.sensorid].sensorStatus == HCU_L3BFDF_SENSOR_WS_STATUS_VALID_TO_TGU) {
			if (gTaskL3bfdfContext.wsValueNbrTgu > 0) gTaskL3bfdfContext.wsValueNbrTgu--;
		}
		gTaskL3bfdfContext.sensorWs[rcv.sensorid].sensorStatus = HCU_L3BFDF_SENSOR_WS_STATUS_VALID_ERROR;

		//启动定时器
		ret = hcu_timer_start(TASK_ID_L3BFDF, TIMER_ID_1S_L3BFDF_ERROR_INQ, \
				zHcuSysEngPar.timer.array[TIMER_ID_1S_L3BFDF_ERROR_INQ].dur, TIMER_TYPE_ONE_TIME, TIMER_RESOLUTION_1S);
		if (ret == FAILURE){
			HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Error start period timer!\n");
		}

		//发送查询命令
		msg_struct_l3bfdf_can_error_inq_cmd_req_t snd0;
		memset(&snd0, 0, sizeof(msg_struct_l3bfdf_can_error_inq_cmd_req_t));
		snd0.length = sizeof(msg_struct_l3bfdf_can_error_inq_cmd_req_t);
		snd0.sensorid = rcv.sensorid;
		ret = hcu_message_send(MSG_ID_L3BFDF_CAN_ERROR_INQ_CMD_REQ, TASK_ID_CANITFLEO, TASK_ID_L3BFDF, &snd0, snd0.length);
		if (ret == FAILURE) {
			HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3BFDF].taskName, zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName);
		}

		return SUCCESS;
	}

	//重新统计各个参数
	func_l3bfdf_cacluate_sensor_ws_valid_value();

	//这里不可能出现以下情况
	if ((gTaskL3bfdfContext.wsValueNbrTtt !=0) && (gTaskL3bfdfContext.wsValueNbrTgu !=0)){
		HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Wrong TTT/TGU figure during SCAN mode!\n");
	}

	//是否要进入搜索
	if (gTaskL3bfdfContext.wsValueNbrWeight >= gTaskL3bfdfContext.comAlgPar.MinScaleNumberStartCombination){
		//组合失败
		HCU_DEBUG_PRINT_NOR("L3BFDF: Enter Combination search!\n");
		if (func_l3bfdf_ws_sensor_search_combination() == -1){
			HCU_DEBUG_PRINT_NOR("L3BFDF: Combination search finished, None find!\n");
			if (gTaskL3bfdfContext.wsValueNbrWeight >= (HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX-1)){
				//得到抛弃的传感器
				func_l3bfdf_ws_sensor_search_give_up();

				gTaskL3bfdfContext.cur.wsTgvTimes++;
				gTaskL3bfdfContext.cur.wsTgvMatCnt += func_l3bfdf_cacluate_sensor_ws_bitmap_valid_number();
				gTaskL3bfdfContext.cur.wsTgvMatWgt += func_l3bfdf_cacluate_sensor_ws_bitmap_valid_weight();

				//发送抛弃命令
				msg_struct_l3bfdf_can_ws_give_up_t snd1;
				memset(&snd1, 0, sizeof(msg_struct_l3bfdf_can_ws_give_up_t));
				snd1.length = sizeof(msg_struct_l3bfdf_can_ws_give_up_t);
				memcpy(snd1.sensorBitmap, gTaskL3bfdfContext.wsBitmap, HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX);
				ret = hcu_message_send(MSG_ID_L3BFDF_CAN_WS_GIVE_UP, TASK_ID_CANITFLEO, TASK_ID_L3BFDF, &snd1, snd1.length);
				if (ret == FAILURE){
					HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3BFDF].taskName, zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName);
				}

				//更新传感器状态
				for (i=0; i<HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX; i++){
					if (gTaskL3bfdfContext.wsBitmap[i] == 1) gTaskL3bfdfContext.sensorWs[i].sensorStatus = HCU_L3BFDF_SENSOR_WS_STATUS_VALID_TGU_START;
				}

				//测试打印
				char targetStr[100];
				char tStr[10];
				sprintf(targetStr, "L3BFDF: Send out TGU_OUT bitmap [%d]=[", gTaskL3bfdfContext.wsValueNbrTgu);
				for (i=0; i<HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX; i++){
					sprintf(tStr, "%d", *(gTaskL3bfdfContext.wsBitmap+i));
					strcat(targetStr, tStr);
				}
				strcat(targetStr, "]\n");
				HCU_DEBUG_PRINT_CRT(targetStr);

				//启动定时器
				ret = hcu_timer_start(TASK_ID_L3BFDF, TIMER_ID_1S_L3BFDF_TGU_WAIT_FB, \
						zHcuSysEngPar.timer.array[TIMER_ID_1S_L3BFDF_TGU_WAIT_FB].dur, TIMER_TYPE_ONE_TIME, TIMER_RESOLUTION_1S);
				if (ret == FAILURE){
					HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Error start timer!\n");
				}

				//进入TGU状态，设置新状态
				if (FsmSetState(TASK_ID_L3BFDF, FSM_STATE_L3BFDF_OOS_TGU) == FAILURE){
					HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Error Set FSM State!\n");
				}
			} //== HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX
			//对于未到最大物料压秤的情形，不予理睬
		}//-1

		//返回有意义的数值
		else{
			HCU_DEBUG_PRINT_NOR("L3BFDF: Combination search finished, Success!\n");
			//发送出料命令
			gTaskL3bfdfContext.cur.wsTttTimes++;
			gTaskL3bfdfContext.cur.wsTttMatCnt += func_l3bfdf_cacluate_sensor_ws_bitmap_valid_number();
			gTaskL3bfdfContext.cur.wsTttMatWgt += func_l3bfdf_cacluate_sensor_ws_bitmap_valid_weight();

			msg_struct_l3bfdf_can_ws_comb_out_t snd2;
			memset(&snd2, 0, sizeof(msg_struct_l3bfdf_can_ws_comb_out_t));
			snd2.length = sizeof(msg_struct_l3bfdf_can_ws_comb_out_t);
			memcpy(snd2.sensorBitmap, gTaskL3bfdfContext.wsBitmap, HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX);
			snd2.combnbr = gTaskL3bfdfContext.wsValueNbrTtt;
			ret = hcu_message_send(MSG_ID_L3BFDF_CAN_WS_COMB_OUT, TASK_ID_CANITFLEO, TASK_ID_L3BFDF, &snd2, snd2.length);
			if (ret == FAILURE){
				HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3BFDF].taskName, zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName);
			}

			//更新传感器状态
			for (i=0; i<HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX; i++){
				if (gTaskL3bfdfContext.wsBitmap[i] == 1) gTaskL3bfdfContext.sensorWs[i].sensorStatus = HCU_L3BFDF_SENSOR_WS_STATUS_VALID_TTT_START;
			}

			//测试打印
			char targetStr[100];
			char tStr[10];
			sprintf(targetStr, "L3BFDF: Send out TTT_OUT bitmap [%d]=[", gTaskL3bfdfContext.wsValueNbrTtt);
			for (i=0; i<HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX; i++){
				sprintf(tStr, "%d", *(gTaskL3bfdfContext.wsBitmap+i));
				strcat(targetStr, tStr);
			}
			strcat(targetStr, "]\n");
			HCU_DEBUG_PRINT_CRT(targetStr);

			//GET comb total value and store into DB
			int totalWgt = 0;
			for (i=0; i<HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX; i++){
				if (gTaskL3bfdfContext.wsBitmap[i] == 1)
				totalWgt+= gTaskL3bfdfContext.sensorWs[i].sensorValue;
			}
			dbi_HcuBfsc_WmcCurComWgtUpdate(totalWgt);

			//启动定时器
			ret = hcu_timer_start(TASK_ID_L3BFDF, TIMER_ID_1S_L3BFDF_TTT_WAIT_FB, \
					zHcuSysEngPar.timer.array[TIMER_ID_1S_L3BFDF_TTT_WAIT_FB].dur, TIMER_TYPE_ONE_TIME, TIMER_RESOLUTION_1S);
			if (ret == FAILURE){
				HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Error start timer!\n");
			}

			//进入TTT状态
			if (FsmSetState(TASK_ID_L3BFDF, FSM_STATE_L3BFDF_OOS_TTT) == FAILURE){
				HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Error Set FSM State!\n");
			}
		}//返回有意义的数值
	} //>= gTaskL3bfdfContext.comAlgPar.MinScaleNumberStartCombination
	//对于不足以启动搜索的情形，不予理睬
*/

	//返回
	return SUCCESS;
}

//组合出料反馈
OPSTAT fsm_l3bfdf_canitf_ws_comb_out_fb(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret=0;
	msg_struct_can_l3bfdf_ws_comb_out_fb_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_can_l3bfdf_ws_comb_out_fb_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_can_l3bfdf_ws_comb_out_fb_t))){
		HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Receive message error!\n");
	}
	memcpy(&rcv, param_ptr, param_len);

	if ((rcv.streamId >= HCU_SYSCFG_BFDF_EQU_FLOW_NBR_MAX) || (rcv.hopperId == 0) || (rcv.hopperId >= HCU_SYSCFG_BFDF_HOPPER_NBR_MAX))
		HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Receive message error!\n");

	//停止定时器
	hcu_timer_stop(TASK_ID_L3BFDF, TIMER_ID_1S_L3BFDF_TTT_WAIT_FB, TIMER_RESOLUTION_1S);

	//设置状态机
	FsmSetState(TASK_ID_L3BFDF, FSM_STATE_L3BFDF_OOS_SCAN);

	//先处理错误情况
	if (rcv.validFlag == FALSE){
		//隔离该料斗
		gTaskL3bfdfContext.hopper[rcv.streamId][rcv.hopperId].hopperStatus = HCU_L3BFDF_NODE_BOARD_STATUS_VALID_ERROR;

		//返回
		return SUCCESS;
	}

	//篮子设置满
	gTaskL3bfdfContext.hopper[rcv.streamId][rcv.hopperId].basketStatus = HCU_L3BFDF_HOPPER_BASKET_FULL;

	//设置料斗状态
	gTaskL3bfdfContext.hopper[rcv.streamId][rcv.hopperId].hopperValue = 0;
	gTaskL3bfdfContext.hopper[rcv.streamId][rcv.hopperId].hopperStatus = HCU_L3BFDF_NODE_BOARD_STATUS_VALID;
	gTaskL3bfdfContext.hopper[rcv.streamId][rcv.hopperId].matLackIndex = gTaskL3bfdfContext.hopper[rcv.streamId][rcv.hopperId].matLackNbr;

	//打印二维码／条形码：二维码＋条形码的内容
	char s[100];
	time_t lt;
	struct tm *cu;
	memset(s, 0, sizeof(s));

	//初始本地时间
	lt=time(NULL);
	cu = localtime(&lt);
	cu->tm_mon = cu->tm_mon + 1; //月份是从0-11的，+1是为了符合正常逻辑
	sprintf(s, "BOFENGZHINENG-%s-%4.2fg-%04d.%02d.%02d.%02d:%02d:%02d", gTaskL3bfdfContext.configName, (float)gTaskL3bfdfContext.comAlgPar.TargetCombinationWeight, \
			(UINT16)(1900+cu->tm_year), (UINT8)cu->tm_mon, (UINT8)cu->tm_mday, (UINT8)cu->tm_hour, (UINT8)cu->tm_min, (UINT8)cu->tm_sec);
	hcu_sps232_send_char_to_ext_printer(s, strlen(s));

	//返回
	return SUCCESS;
}

//入料反馈消息处理
OPSTAT fsm_l3bfdf_canitf_snc_pulliin_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret=0;
	msg_struct_can_l3bfdf_snc_pullin_resp_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_can_l3bfdf_snc_pullin_resp_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_can_l3bfdf_snc_pullin_resp_t))){
		HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Receive message error!\n");
	}
	memcpy(&rcv, param_ptr, param_len);

	if ((rcv.streamId>=HCU_SYSCFG_BFDF_EQU_FLOW_NBR_MAX) || (rcv.hopperId == 0) || (rcv.hopperId >= HCU_SYSCFG_BFDF_HOPPER_NBR_MAX))
		HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Receive message error!\n");

	//如果不太正常，则隔离该料斗
	if (rcv.validFlag != TRUE){
		gTaskL3bfdfContext.hopper[rcv.streamId][rcv.hopperId].hopperStatus = HCU_L3BFDF_HOPPER_STATUS_VALID_ERR;
		HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: hopper error!, StreamId/HopperId=[%d/%d]\n", rcv.streamId, rcv.hopperId);
	}

	//将物料重量加到目标重量上去
	gTaskL3bfdfContext.hopper[rcv.streamId][rcv.hopperId].hopperValue += gTaskL3bfdfContext.hopper[rcv.streamId][rcv.hopperId].hopperLastMat;

	//判定是否满
	if (gTaskL3bfdfContext.hopper[rcv.streamId][rcv.hopperId].hopperStatus == HCU_L3BFDF_HOPPER_STATUS_FULL_PRE){
		//设置满
		gTaskL3bfdfContext.hopper[rcv.streamId][rcv.hopperId].hopperStatus = HCU_L3BFDF_HOPPER_STATUS_FULL;
		//判定篮子状态
		if (gTaskL3bfdfContext.hopper[rcv.streamId][rcv.hopperId].basketStatus != HCU_L3BFDF_HOPPER_BASKET_EMPTY) return SUCCESS;
		//发送出料
		if (func_l3bfdf_new_ws_send_out_comb_out_message(rcv.streamId, rcv.hopperId) == FALSE){
			gTaskL3bfdfContext.hopper[rcv.streamId][rcv.hopperId].hopperStatus = HCU_L3BFDF_HOPPER_STATUS_VALID_ERR;
			HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Send Comb Out message error!\n");
		}
		//设置状态
		FsmSetState(TASK_ID_L3BFDF, FSM_STATE_L3BFDF_OOS_TTT);
	}
	else{
		//设置Hopper状态
		gTaskL3bfdfContext.hopper[rcv.streamId][rcv.hopperId].hopperStatus = HCU_L3BFDF_HOPPER_STATUS_VALID;
	}

	//返回
	return SUCCESS;
}

OPSTAT fsm_l3bfdf_canitf_basket_clean_ind(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret=0;
	msg_struct_can_l3bfdf_basket_clean_ind_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_can_l3bfdf_basket_clean_ind_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_can_l3bfdf_basket_clean_ind_t))){
		HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Receive message error!\n");
	}
	memcpy(&rcv, param_ptr, param_len);

	if ((rcv.streamId >= HCU_SYSCFG_BFDF_EQU_FLOW_NBR_MAX) || (rcv.hopperId == 0) || (rcv.hopperId >= HCU_SYSCFG_BFDF_HOPPER_NBR_MAX))
		HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Receive message error!\n");

	gTaskL3bfdfContext.hopper[rcv.streamId][rcv.hopperId].basketStatus = HCU_L3BFDF_HOPPER_BASKET_EMPTY;

	//如果此时也条件允许，也产生出料
	if (gTaskL3bfdfContext.hopper[rcv.streamId][rcv.hopperId].hopperStatus == HCU_L3BFDF_HOPPER_STATUS_FULL){
		//发送出料
		if (func_l3bfdf_new_ws_send_out_comb_out_message(rcv.streamId, rcv.hopperId) == FALSE){
			gTaskL3bfdfContext.hopper[rcv.streamId][rcv.hopperId].hopperStatus = HCU_L3BFDF_HOPPER_STATUS_VALID_ERR;
			HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Send Comb Out message error!\n");
		}
		//设置状态
		FsmSetState(TASK_ID_L3BFDF, FSM_STATE_L3BFDF_OOS_TTT);
	}

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
	//int ret = 0, i=0;
	//int total = 0;

/*
	//先判定成功的数量是否达到最小数，如果是，就认为是成功了
	for (i=0; i<HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX; i++){
		if (gTaskL3bfdfContext.sensorWs[i].sensorStatus == HCU_L3BFDF_SENSOR_WS_STATUS_CFG_CMPL) total++;
	}
	if (total < gTaskL3bfdfContext.comAlgPar.MinScaleNumberCombination){
		//发送反馈给UICOMM
		msg_struct_l3bfdf_uicomm_cfg_resp_t snd;
		memset(&snd, 0, sizeof(msg_struct_l3bfdf_uicomm_cfg_resp_t));
		snd.validFlag = FALSE;
		snd.sensorid = HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX;
		snd.errCode = HCU_SYSMSG_BFDF_ERR_CODE_TIME_OUT;
		snd.length = sizeof(msg_struct_l3bfdf_uicomm_cfg_resp_t);
		ret = hcu_message_send(MSG_ID_L3BFDF_UICOMM_CFG_RESP, TASK_ID_BFDFUICOMM, TASK_ID_L3BFDF, &snd, snd.length);
		if (ret == FAILURE){
			HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3BFDF].taskName, zHcuVmCtrTab.task[TASK_ID_BFDFUICOMM].taskName);
		}
		//设置状态机
		if (FsmSetState(TASK_ID_L3BFDF, FSM_STATE_L3BFDF_ACTIVED) == FAILURE){
			HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Error Set FSM State!\n");
		}
	}

	//如果超过最小数量，则可以继续执行
	else{
		//发送反馈给UICOMM
		msg_struct_l3bfdf_uicomm_cfg_resp_t snd;
		memset(&snd, 0, sizeof(msg_struct_l3bfdf_uicomm_cfg_resp_t));
		snd.validFlag = TRUE;
		snd.length = sizeof(msg_struct_l3bfdf_uicomm_cfg_resp_t);
		ret = hcu_message_send(MSG_ID_L3BFDF_UICOMM_CFG_RESP, TASK_ID_BFDFUICOMM, TASK_ID_L3BFDF, &snd, snd.length);
		if (ret == FAILURE){
			HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3BFDF].taskName, zHcuVmCtrTab.task[TASK_ID_BFDFUICOMM].taskName);
		}
		//设置状态机
		if (FsmSetState(TASK_ID_L3BFDF, FSM_STATE_L3BFDF_OPR_GO) == FAILURE){
			HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Error Set FSM State!\n");
		}
	}
*/

	//返回
	return SUCCESS;
}

//出料超时处理过程
OPSTAT func_l3bfdf_time_out_comb_out_req_process(void)
{
	//int ret = 0, i=0;

/*
	//发送STOP_REQ给所有下位机
	msg_struct_l3bfsc_can_sys_stop_req_t snd1;
	memset(&snd1, 0, sizeof(msg_struct_l3bfsc_can_sys_stop_req_t));
	snd1.length = sizeof(msg_struct_l3bfsc_can_sys_stop_req_t);
	//所有传感器
	for (i = 0; i< HCU_SYSCFG_BFSC_SNR_WS_NBR_MAX; i++){
		snd1.wsBitmap[i] = TRUE;
	}
	ret = hcu_message_send(MSG_ID_L3BFSC_CAN_SYS_STOP_REQ, TASK_ID_CANITFLEO, TASK_ID_L3BFSC, &snd1, snd1.length);
	if (ret == FAILURE) HCU_ERROR_PRINT_L3BFSC("L3BFSC: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3BFSC].taskName, zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName);

	//设置状态机
	if (FsmSetState(TASK_ID_L3BFSC, FSM_STATE_L3BFSC_OPR_GO) == FAILURE){
		HCU_ERROR_PRINT_L3BFSC_RECOVERY("L3BFSC: Error Set FSM State!\n");
	}

	//延时发送SUSPEND给界面，以防止界面SUSPEND被STOP_RESP覆盖
	hcu_sleep(2);
	msg_struct_l3bfsc_uicomm_cmd_resp_t snd;
	memset(&snd, 0, sizeof(msg_struct_l3bfsc_uicomm_cmd_resp_t));
	snd.cmdid = HCU_SYSMSG_BFSC_UICOMM_CMDID_SUSPEND;
	snd.validFlag = TRUE;
	snd.length = sizeof(msg_struct_l3bfsc_uicomm_cmd_resp_t);
	ret = hcu_message_send(MSG_ID_L3BFSC_UICOMM_CMD_RESP, TASK_ID_BFSCUICOMM, TASK_ID_L3BFSC, &snd, snd.length);
	if (ret == FAILURE){
		HCU_ERROR_PRINT_L3BFSC_RECOVERY("L3BFSC: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3BFSC].taskName, zHcuVmCtrTab.task[TASK_ID_BFSCUICOMM].taskName);
	}

*/
	//返回
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
		for (j=0; j<HCU_SYSCFG_BFDF_NODE_BOARD_NBR_MAX; j++)
		{
			if ((gTaskL3bfdfContext.nodeDyn[i][j].nodeStatus == HCU_L3BFDF_NODE_BOARD_STATUS_CFG_START_REQ) && (gTaskL3bfdfContext.nodeDyn[i][j].cfgRcvFlag == FALSE))
				return FALSE;
		}
	}

	//返回
	return TRUE;
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
	if ((streamId >= HCU_SYSCFG_BFDF_EQU_FLOW_NBR_MAX) || (nbrGroup == 0) || (nbrGroup > HCU_SYSCFG_BFDF_HOPPER_NBR_MAX))
		return FALSE;

	gTaskL3bfdfContext.totalGroupNbr[streamId] = nbrGroup;

	//循环赋值
	for (i=1; i<=nbrGroup; i++){
		gTaskL3bfdfContext.group[streamId][i].groupId = 0;
		gTaskL3bfdfContext.group[streamId][i].groupStatus = HCU_L3BFDF_GROUP_STATUS_ACTIVE;
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
	for (i=1; i<HCU_SYSCFG_BFDF_HOPPER_NBR_MAX; i++){
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
	if (streamId >= HCU_SYSCFG_BFDF_EQU_FLOW_NBR_MAX)
		return FALSE;

	//状态设置, 初始化物料数量
	for (i=0; i<HCU_SYSCFG_BFDF_HOPPER_NBR_MAX; i++){
		gTaskL3bfdfContext.hopper[streamId][i].hopperStatus = HCU_L3BFDF_HOPPER_STATUS_VALID;
		gTaskL3bfdfContext.hopper[streamId][i].basketStatus = HCU_L3BFDF_HOPPER_BASKET_EMPTY;
		gTaskL3bfdfContext.hopper[streamId][i].matLackIndex = 0;
		gid = gTaskL3bfdfContext.hopper[streamId][i].groupId;
		targetWgt = gTaskL3bfdfContext.group[streamId][gid].targetWeight + gTaskL3bfdfContext.group[streamId][gid].targetUpLimit/2.0;
		avgWgt = gTaskL3bfdfContext.group[streamId][gid].rangeAvg;
		sigWgt = gTaskL3bfdfContext.group[streamId][gid].rangeSigma;
		gTaskL3bfdfContext.hopper[streamId][i].matLackNbr = (UINT32)(targetWgt/avgWgt==0?0.01:avgWgt)&0xFFFF;
		nbrDeep = (UINT32)(avgWgt/(sigWgt==0?0.01:sigWgt))&0xFFFF;

		//这里的目标，还要更好的进行比较
		if (gTaskL3bfdfContext.hopper[streamId][i].matLackNbr <= nbrDeep) return FALSE;
	}

	return TRUE;
}

//正常在某一个GroupId的末尾增加一个hopperId
//GroupId/HopperId都不得等于0
//整个增加函数采用强行格式化模式，而不是检测返回错误模式，从而简化和提升可靠性．数据不一致之后还有可能纠偏回来．
bool func_l3bfdf_hopper_add_by_tail(UINT8 streamId, UINT16 groupId, UINT16 hopperNewId)
{
	UINT16 fHopper = 0;

	//入参检查：注意起点和终点
	if ((streamId >= HCU_SYSCFG_BFDF_EQU_FLOW_NBR_MAX) || (groupId >HCU_SYSCFG_BFDF_HOPPER_NBR_MAX) || (hopperNewId >HCU_SYSCFG_BFDF_HOPPER_NBR_MAX))
		return FALSE;

	//强制增加GroupId到最大数量计量单位上
	if (groupId > gTaskL3bfdfContext.totalGroupNbr[streamId]) gTaskL3bfdfContext.totalGroupNbr[streamId] = groupId;

	//强制修改该GroupId数据
	gTaskL3bfdfContext.group[streamId][groupId].groupId = groupId;

	//强制修改该HopperId的GroupId
	fHopper = gTaskL3bfdfContext.group[streamId][groupId].firstHopperId;
	if (fHopper > HCU_SYSCFG_BFDF_HOPPER_NBR_MAX) return FAILURE;

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

bool func_l3bfdf_hopper_add_by_group(UINT8 streamId, UINT16 groupId, UINT16 nbrHopper)
{
	//入参检查：注意起点和终点
	if ((streamId >= HCU_SYSCFG_BFDF_EQU_FLOW_NBR_MAX) || (groupId > HCU_SYSCFG_BFDF_HOPPER_NBR_MAX) || (nbrHopper == 0)|| (nbrHopper > HCU_SYSCFG_BFDF_HOPPER_NBR_MAX))
		return FALSE;

	//强制增加GroupId到最大数量计量单位上
	if (groupId > gTaskL3bfdfContext.totalGroupNbr[streamId]) gTaskL3bfdfContext.totalGroupNbr[streamId] = groupId;

	int i=0;
	UINT16 tmpHopper = 0;
	int cnt = 0;
	for (i=0; i<nbrHopper; i++){
		cnt = 0;
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

bool func_l3bfdf_hopper_add_by_group_in_average_distribution(UINT8 streamId, UINT16 nbrGroup)
{
	//入参检查：注意起点和终点
	if ((streamId >= HCU_SYSCFG_BFDF_EQU_FLOW_NBR_MAX) || (nbrGroup > HCU_SYSCFG_BFDF_HOPPER_NBR_MAX))
		return FALSE;

	//强制增加GroupId到最大数量计量单位上
	if (nbrGroup > gTaskL3bfdfContext.totalGroupNbr[streamId]) gTaskL3bfdfContext.totalGroupNbr[streamId] = nbrGroup;

	int i=0;
	int cnt = HCU_SYSCFG_BFDF_HOPPER_NBR_MAX/nbrGroup;
	for (i=1; i<nbrGroup; i++){
		if (func_l3bfdf_hopper_add_by_group(streamId, i, cnt) == FALSE) return FALSE;
		//HcuDebugPrint("L3BFDF: GroupId = %d, Nbr of Hopper = %d\n", i, cnt);
	}
	cnt = HCU_SYSCFG_BFDF_HOPPER_NBR_MAX - cnt * (nbrGroup-1) -1; //第一个是固定的垃圾桶
	if (func_l3bfdf_hopper_add_by_group(streamId, nbrGroup, cnt) == FALSE) return FALSE;
	//HcuDebugPrint("L3BFDF: GroupId = %d, Nbr of Hopper = %d\n", nbrGroup, cnt);

	return TRUE;
}

bool func_l3bfdf_hopper_add_by_group_in_normal_distribution(UINT8 streamId, UINT16 nbrGroup)
{
	UINT16 cnt[HCU_SYSCFG_BFDF_HOPPER_NBR_MAX];
	int i=0;

	//入参检查：注意起点和终点
	if ((streamId >= HCU_SYSCFG_BFDF_EQU_FLOW_NBR_MAX) || (nbrGroup > HCU_SYSCFG_BFDF_HOPPER_NBR_MAX))
		return FALSE;

	//强制增加GroupId到最大数量计量单位上
	if (nbrGroup > gTaskL3bfdfContext.totalGroupNbr[streamId]) gTaskL3bfdfContext.totalGroupNbr[streamId] = nbrGroup;

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
		if (func_l3bfdf_hopper_add_by_group(streamId, i, cnt[i]) == FALSE) return FALSE;
	}

	return TRUE;
}

bool func_l3bfdf_hopper_remove_by_tail(UINT8 streamId, UINT16 groupId)
{
	UINT16 fHopper = 0;

	//入参检查：注意起点和终点
	if ((streamId >= HCU_SYSCFG_BFDF_EQU_FLOW_NBR_MAX) || (groupId >HCU_SYSCFG_BFDF_HOPPER_NBR_MAX))
		return FALSE;

	//GroupId
	if (groupId > gTaskL3bfdfContext.totalGroupNbr[streamId]) return FALSE;

	//HopperId -> GroupId
	if (gTaskL3bfdfContext.group[streamId][groupId].groupId != groupId) return FALSE;

	//First HopperId
	fHopper = gTaskL3bfdfContext.group[streamId][groupId].firstHopperId;
	if ((fHopper == 0) || (fHopper > HCU_SYSCFG_BFDF_HOPPER_NBR_MAX)) return FALSE;

	//Hopper总数
	if ((gTaskL3bfdfContext.group[streamId][groupId].totalHopperNbr == 0) || (gTaskL3bfdfContext.group[streamId][groupId].totalHopperNbr > HCU_SYSCFG_BFDF_HOPPER_NBR_MAX)) return FALSE;

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
	if ((streamId >= HCU_SYSCFG_BFDF_EQU_FLOW_NBR_MAX) || (groupId >HCU_SYSCFG_BFDF_HOPPER_NBR_MAX) || (hopperId >HCU_SYSCFG_BFDF_HOPPER_NBR_MAX) || (hopperNewId >HCU_SYSCFG_BFDF_HOPPER_NBR_MAX))
		return FALSE;

	//强制增加GroupId到最大数量计量单位上
	if (groupId > gTaskL3bfdfContext.totalGroupNbr[streamId]) gTaskL3bfdfContext.totalGroupNbr[streamId] = groupId;

	//强制修改该GroupId数据
	gTaskL3bfdfContext.group[streamId][groupId].groupId = groupId;

	//强制修改该HopperId的GroupId
	fHopper = gTaskL3bfdfContext.group[streamId][groupId].firstHopperId;
	if ((fHopper == 0) || (fHopper > HCU_SYSCFG_BFDF_HOPPER_NBR_MAX)) return FAILURE;

	//遍历到hopperId
	UINT16 curHopper = fHopper;
	UINT16 cnt = 0;
	while(curHopper != hopperId){
		cnt++;
		if (cnt > HCU_SYSCFG_BFDF_HOPPER_NBR_MAX) return FALSE;
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
	if ((streamId >= HCU_SYSCFG_BFDF_EQU_FLOW_NBR_MAX) || (groupId >HCU_SYSCFG_BFDF_HOPPER_NBR_MAX) || (hopperId >HCU_SYSCFG_BFDF_HOPPER_NBR_MAX))
		return FALSE;

	//GroupId
	if (groupId > gTaskL3bfdfContext.totalGroupNbr[streamId]) return FALSE;

	//HopperId -> GroupId
	if (gTaskL3bfdfContext.group[streamId][groupId].groupId != groupId) return FALSE;

	//First HopperId
	fHopper = gTaskL3bfdfContext.group[streamId][groupId].firstHopperId;
	if ((fHopper == 0) || (fHopper > HCU_SYSCFG_BFDF_HOPPER_NBR_MAX)) return FALSE;

	//Hopper总数
	if ((gTaskL3bfdfContext.group[streamId][groupId].totalHopperNbr == 0) || (gTaskL3bfdfContext.group[streamId][groupId].totalHopperNbr > HCU_SYSCFG_BFDF_HOPPER_NBR_MAX)) return FALSE;

	//遍历到hopperId
	UINT16 curHopper = fHopper;
	UINT16 cnt = 0;
	while(curHopper != hopperId){
		cnt++;
		if (cnt > HCU_SYSCFG_BFDF_HOPPER_NBR_MAX) return FALSE;
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

int func_l3bfdf_hopper_dual_chain_audit(void)
{
	UINT8  streamId = 0;
	UINT16 groupId = 0;
	UINT16 fHopper=0, nextHopper=0, tmpHopper=0, preHopper=0;
	UINT16 tmp=0;
	int cnt = 0;

	//STREAM
	for (streamId=0; streamId<HCU_SYSCFG_BFDF_EQU_FLOW_NBR_MAX; streamId++)
	{
		if (gTaskL3bfdfContext.totalGroupNbr[streamId] > HCU_SYSCFG_BFDF_HOPPER_NBR_MAX) return -1;
		if (gTaskL3bfdfContext.totalGroupNbr[streamId] == 0) continue;

		//检查总量
		tmp=0;
		for (groupId = 0; groupId <= gTaskL3bfdfContext.totalGroupNbr[streamId]; groupId++)
		{
			if (gTaskL3bfdfContext.group[streamId][groupId].totalHopperNbr > HCU_SYSCFG_BFDF_HOPPER_NBR_MAX) return -2;
			if (gTaskL3bfdfContext.group[streamId][groupId].totalHopperNbr == 0) return -3;
			//HcuDebugPrint("L3BFDF: S/G=%d/%d, Total Hopper=%d\n", streamId, groupId, gTaskL3bfdfContext.group[streamId][groupId].totalHopperNbr);
			tmp += gTaskL3bfdfContext.group[streamId][groupId].totalHopperNbr;
		}
		if (tmp != HCU_SYSCFG_BFDF_HOPPER_NBR_MAX) return -4;
		for (groupId = gTaskL3bfdfContext.totalGroupNbr[streamId]+1; groupId < HCU_SYSCFG_BFDF_HOPPER_NBR_MAX; groupId++)
		{
			if (gTaskL3bfdfContext.group[streamId][groupId].totalHopperNbr != 0) return -5;
		}

		//按照GroupId进行循环
		for (groupId = 0; groupId <= gTaskL3bfdfContext.totalGroupNbr[streamId]; groupId++)
		{
			if (gTaskL3bfdfContext.group[streamId][groupId].groupId != groupId) return -10;
			if (gTaskL3bfdfContext.group[streamId][groupId].firstHopperId > HCU_SYSCFG_BFDF_HOPPER_NBR_MAX) return -11;
			if (gTaskL3bfdfContext.group[streamId][groupId].fillHopperId > HCU_SYSCFG_BFDF_HOPPER_NBR_MAX) return -12;
			fHopper = gTaskL3bfdfContext.group[streamId][groupId].firstHopperId;
			if ((groupId == 0) && (fHopper == 0)){
				if (gTaskL3bfdfContext.group[streamId][0].totalHopperNbr != 1) return -13;
				if (gTaskL3bfdfContext.hopper[streamId][0].groupId != 0) return -14;
				if (gTaskL3bfdfContext.hopper[streamId][0].hopperId != 0) return -15;
				if (gTaskL3bfdfContext.hopper[streamId][0].nextHopperId != 0) return -16;
				if (gTaskL3bfdfContext.hopper[streamId][0].preHopperId != 0) return -17;
				continue;
			}
			if ((groupId != 0) && (fHopper == 0)){
				if (gTaskL3bfdfContext.group[streamId][groupId].totalHopperNbr != 0) return -18;
				continue;
			}
			nextHopper = fHopper;
			cnt = 1;
			if (gTaskL3bfdfContext.hopper[streamId][nextHopper].groupId != groupId) return -20;
			if (gTaskL3bfdfContext.hopper[streamId][nextHopper].hopperId != nextHopper) return -21;
			if (gTaskL3bfdfContext.hopper[streamId][nextHopper].nextHopperId > HCU_SYSCFG_BFDF_HOPPER_NBR_MAX) return -22;
			if (gTaskL3bfdfContext.hopper[streamId][nextHopper].preHopperId > HCU_SYSCFG_BFDF_HOPPER_NBR_MAX) return -23;
			//正向循环
			while ((cnt < HCU_SYSCFG_BFDF_HOPPER_NBR_MAX) && (gTaskL3bfdfContext.hopper[streamId][nextHopper].nextHopperId != fHopper))
			{
				cnt++;
				tmpHopper = gTaskL3bfdfContext.hopper[streamId][nextHopper].nextHopperId;
				if (gTaskL3bfdfContext.hopper[streamId][tmpHopper].groupId != groupId) return -30;
				if (gTaskL3bfdfContext.hopper[streamId][tmpHopper].hopperId != tmpHopper) return -31;
				if (gTaskL3bfdfContext.hopper[streamId][tmpHopper].nextHopperId > HCU_SYSCFG_BFDF_HOPPER_NBR_MAX) return -32;
				if (gTaskL3bfdfContext.hopper[streamId][tmpHopper].preHopperId > HCU_SYSCFG_BFDF_HOPPER_NBR_MAX) return -33;
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
				if (gTaskL3bfdfContext.hopper[streamId][tmpHopper].groupId != groupId) return -40;
				if (gTaskL3bfdfContext.hopper[streamId][tmpHopper].hopperId != tmpHopper) return -41;
				if (gTaskL3bfdfContext.hopper[streamId][tmpHopper].nextHopperId > HCU_SYSCFG_BFDF_HOPPER_NBR_MAX) return -42;
				if (gTaskL3bfdfContext.hopper[streamId][tmpHopper].preHopperId > HCU_SYSCFG_BFDF_HOPPER_NBR_MAX) return -43;
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
bool func_l3bfdf_group_auto_alloc_init_range_in_average(UINT8 streamId, UINT16 nbrGroup, double wgtMin, double wgtMax)
{
	int i=0;
	double gap = 0;

	//入参检查：注意起点和终点
	if ((streamId >= HCU_SYSCFG_BFDF_EQU_FLOW_NBR_MAX) || (nbrGroup > gTaskL3bfdfContext.totalGroupNbr[streamId]))
		return FALSE;
	if (wgtMin >= wgtMax) return FALSE;
	if (nbrGroup <=0) return FALSE;

	if (nbrGroup == 1) gap = 0;
	else gap = (wgtMax - wgtMin) / (nbrGroup - 1);

	for (i=1; i<=nbrGroup; i++){
		gTaskL3bfdfContext.group[streamId][i].rangeLow = wgtMin + (i-1)*gap;
		gTaskL3bfdfContext.group[streamId][i].rangeHigh = wgtMin + i*gap;
		gTaskL3bfdfContext.group[streamId][i].rangeAvg = (gTaskL3bfdfContext.group[streamId][i].rangeLow + gTaskL3bfdfContext.group[streamId][i].rangeHigh)/2.0;
		gTaskL3bfdfContext.group[streamId][i].rangeSigma = gap/2.0;
	}

	return TRUE;
}

//分配小组的重量目标
bool func_l3bfdf_group_auto_alloc_init_target_with_uplimit(UINT8 streamId, double targetWgt, double ulRatio)
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

	for (i=1; i<HCU_SYSCFG_BFDF_HOPPER_NBR_MAX; i++){
		gTaskL3bfdfContext.group[streamId][i].targetWeight = targetWgt * (rand()%100+100) / 100.0;
		gTaskL3bfdfContext.group[streamId][i].targetUpLimit = targetWgt * ratio;
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
	sprintf(s, "BFDFUICOMM: Stream[%d] Total Group number = %d, bitmap = ", streamId, gTaskL3bfdfContext.totalGroupNbr[streamId]);
	for (i = 0; i<HCU_SYSCFG_BFDF_HOPPER_NBR_MAX; i++){
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
	char s[800];
	char tmp[40];

	//入参检查：注意起点和终点
	if (streamId >= HCU_SYSCFG_BFDF_EQU_FLOW_NBR_MAX) return FALSE;

	sprintf(s, "BFDFUICOMM: Stream[%d] Total Group number = %d, Group[x-y/y/y] = ", streamId, gTaskL3bfdfContext.totalGroupNbr[streamId]);
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

//搜索是否存在欠一满：状态必须是VALID，而不能是其它状态
UINT16 func_l3bfdf_new_ws_search_hopper_full(UINT8 streamId)
{
	int i=0;

	//入参检查：注意起点和终点
	if (streamId >= HCU_SYSCFG_BFDF_EQU_FLOW_NBR_MAX) return 0;

	for (i=1; i<HCU_SYSCFG_BFDF_HOPPER_NBR_MAX; i++){
		if (gTaskL3bfdfContext.hopper[streamId][i].hopperStatus == HCU_L3BFDF_HOPPER_STATUS_FULL)
			return i;
	}

	//没找到，返回0
	return 0;
}

//搜索是否存在欠一满：状态必须是VALID，而不能是其它状态
UINT16 func_l3bfdf_new_ws_search_hopper_lack_one(UINT8 streamId, UINT16 gid, double weight)
{
	int cnt=0;
	UINT16 fHopper=0, nextHopper=0, tmpHopper=0;

	//入参检查：注意起点和终点
	if (streamId >= HCU_SYSCFG_BFDF_EQU_FLOW_NBR_MAX) return 0;
	if ((gid <=0) || (gid > gTaskL3bfdfContext.totalGroupNbr[streamId])) return 0;

	fHopper = gTaskL3bfdfContext.group[streamId][gid].fillHopperId;
	if (fHopper == 0) return 0;

	if ((gTaskL3bfdfContext.hopper[streamId][fHopper].hopperStatus == HCU_L3BFDF_HOPPER_STATUS_VALID) &&\
			((gTaskL3bfdfContext.hopper[streamId][fHopper].hopperValue + weight) >= gTaskL3bfdfContext.group[streamId][gid].targetWeight) &&\
			((gTaskL3bfdfContext.hopper[streamId][fHopper].hopperValue + weight) <= (gTaskL3bfdfContext.group[streamId][gid].targetWeight +\
					gTaskL3bfdfContext.group[streamId][gid].targetUpLimit)))
			return fHopper;

	cnt = 0;
	nextHopper = gTaskL3bfdfContext.hopper[streamId][fHopper].nextHopperId;

	while ((cnt < HCU_SYSCFG_BFDF_HOPPER_NBR_MAX) && (nextHopper != fHopper))
	{
		cnt++;
		if ((gTaskL3bfdfContext.hopper[streamId][nextHopper].hopperStatus == HCU_L3BFDF_HOPPER_STATUS_VALID) &&\
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
UINT16 func_l3bfdf_new_ws_search_hopper_valid_normal(UINT8 sid, UINT16 gid, double weight)
{
	int cnt=0;
	UINT16 fH=0, nextHopper=0, tmpHopper=0;
	double cWg=0, tWg=0, tLm=0;
	double tAvg=0, tSigma=0;
	double hMin=0, hMax=0;

	//入参检查：注意起点和终点
	if (sid >= HCU_SYSCFG_BFDF_EQU_FLOW_NBR_MAX) return 0;
	if ((gid <=0) || (gid > gTaskL3bfdfContext.totalGroupNbr[sid])) return 0;

	//入口寻找
	fH = gTaskL3bfdfContext.group[sid][gid].fillHopperId;
	if (fH == 0) return 0;

	tWg = gTaskL3bfdfContext.group[sid][gid].targetWeight;
	tLm = gTaskL3bfdfContext.group[sid][gid].targetUpLimit;
	tAvg = gTaskL3bfdfContext.group[sid][gid].rangeAvg;
	tSigma = gTaskL3bfdfContext.group[sid][gid].rangeSigma;

	//判定入口满足条件
	if (gTaskL3bfdfContext.hopper[sid][fH].hopperStatus == HCU_L3BFDF_HOPPER_STATUS_VALID)
	{
		cWg = gTaskL3bfdfContext.hopper[sid][fH].hopperValue + weight;
		hMin = tWg - (tAvg+tSigma)*gTaskL3bfdfContext.hopper[sid][fH].matLackIndex;
		hMax = tWg+tLm -(tAvg-tSigma)*gTaskL3bfdfContext.hopper[sid][fH].matLackIndex;
		if ((cWg >= hMin) && (cWg <= hMax)) return fH;
	}

	//顺着双链表，查找满足条件的料斗
	cnt = 0;
	nextHopper = gTaskL3bfdfContext.hopper[sid][fH].nextHopperId;
	while ((cnt < HCU_SYSCFG_BFDF_HOPPER_NBR_MAX) && (nextHopper != fH))
	{
		cnt++;
		if (gTaskL3bfdfContext.hopper[sid][nextHopper].hopperStatus == HCU_L3BFDF_HOPPER_STATUS_VALID)
		{
			cWg = gTaskL3bfdfContext.hopper[sid][nextHopper].hopperValue + weight;
			hMin = tWg - (tAvg+tSigma)*gTaskL3bfdfContext.hopper[sid][nextHopper].matLackIndex;
			hMax = tWg+tLm -(tAvg-tSigma)*gTaskL3bfdfContext.hopper[sid][nextHopper].matLackIndex;
			if ((cWg >= hMin) && (cWg <= hMax)) return nextHopper;
		}
		tmpHopper = gTaskL3bfdfContext.hopper[sid][nextHopper].nextHopperId;
		nextHopper = tmpHopper;
	}

	//没找到，返回0
	return 0;
}




//入料消息发送
bool func_l3bfdf_new_ws_send_out_pullin_message(UINT8 streamId, UINT16 hopperId)
{
	//入参检查：注意起点和终点
	if ((streamId >= HCU_SYSCFG_BFDF_EQU_FLOW_NBR_MAX) || (hopperId > HCU_SYSCFG_BFDF_HOPPER_NBR_MAX))
		return FALSE;

	//先检查hopper状态
	if (gTaskL3bfdfContext.hopper[streamId][hopperId].hopperStatus != HCU_L3BFDF_HOPPER_STATUS_VALID) return FALSE;

	//将临时物料重量清零
	gTaskL3bfdfContext.hopper[streamId][hopperId].hopperLastMat = 0;

	//发送
	msg_struct_l3bfdf_can_snc_pullin_req_t snd;
	memset(&snd, 0, sizeof(msg_struct_l3bfdf_can_snc_pullin_req_t));
	snd.streamId = streamId;
	snd.hopperId = hopperId;
	snd.length = sizeof(msg_struct_l3bfdf_can_snc_pullin_req_t);
	if (hcu_message_send(MSG_ID_L3BFDF_CAN_SNC_PULLIN_REQ, TASK_ID_CANITFLEO, TASK_ID_L3BFDF, &snd, snd.length) == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3BFDF]++;
		HcuErrorPrint("L3BFDF: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3BFDF].taskName, zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName);
		return FALSE;
	}

	//改变状态
	if (hopperId == 0){
		gTaskL3bfdfContext.hopper[streamId][hopperId].hopperStatus = HCU_L3BFDF_HOPPER_STATUS_VALID;
	}else{
		gTaskL3bfdfContext.hopper[streamId][hopperId].hopperStatus = HCU_L3BFDF_HOPPER_STATUS_PULLIN_OUT;
	}

	//没找到，返回0
	return TRUE;
}

//出料消息发送
bool func_l3bfdf_new_ws_send_out_comb_out_message(UINT8 streamId, UINT16 hopperId)
{
	//入参检查：注意起点和终点
	if ((streamId >= HCU_SYSCFG_BFDF_EQU_FLOW_NBR_MAX) || (hopperId == 0) || (hopperId > HCU_SYSCFG_BFDF_HOPPER_NBR_MAX))
		return FALSE;

	//先检查hopper状态
	if (gTaskL3bfdfContext.hopper[streamId][hopperId].hopperStatus != HCU_L3BFDF_HOPPER_STATUS_FULL) return FALSE;

	//发送
	msg_struct_l3bfdf_can_ws_comb_out_t snd;
	memset(&snd, 0, sizeof(msg_struct_l3bfdf_can_ws_comb_out_t));
	snd.streamId = streamId;
	snd.hopperId = hopperId;
	snd.length = sizeof(msg_struct_l3bfdf_can_ws_comb_out_t);
	if (hcu_message_send(MSG_ID_L3BFDF_CAN_WS_COMB_OUT, TASK_ID_CANITFLEO, TASK_ID_L3BFDF, &snd, snd.length) == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3BFDF]++;
		HcuErrorPrint("L3BFDF: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3BFDF].taskName, zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName);
		return FALSE;
	}

	//启动定时器
	gTaskL3bfdfContext.hopper[streamId][hopperId].hopperStatus = HCU_L3BFDF_HOPPER_STATUS_PULLIN_OUT;
	hcu_timer_start(TASK_ID_L3BFDF, TIMER_ID_1S_L3BFDF_TTT_WAIT_FB, zHcuSysEngPar.timer.array[TIMER_ID_1S_L3BFDF_TTT_WAIT_FB].dur, TIMER_TYPE_ONE_TIME, TIMER_RESOLUTION_1S);

	//没找到，返回0
	return TRUE;
}





