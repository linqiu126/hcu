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
	//这个命令，有些命令可以在任何状态下执行：比如STOP，但有些命令必须在配置状态下，比如START
	{MSG_ID_UICOMM_L3BFDF_CMD_REQ,       		FSM_STATE_COMMON,          			fsm_l3bfdf_uicomm_cmd_req},

//	//进料组合态：等待正常的MSG_ID_CAN_L3BFDF_WS_NEW_READY_EVENT，每一次进来均触发一次组合算法。结果无动作，或发送MSG_ID_L3BFDF_CAN_WS_COMB_OUT/MSG_ID_L3BFDF_CAN_WS_GIVE_UP
//	{MSG_ID_CAN_L3BFDF_WS_NEW_READY_EVENT,      FSM_STATE_L3BFDF_OOS_SCAN,          fsm_l3bfdf_canitf_ws_new_ready_event},  //只能触发数据存储，不进入组合算法的执行
//
//	//出料流程态：单纯等待MSG_ID_CAN_L3BFDF_WS_COMB_OUT_FB，一旦收到无误后进入FSM_STATE_L3BFDF_OOS_SCAN
//	{MSG_ID_CAN_L3BFDF_WS_COMB_OUT_FB,       	FSM_STATE_L3BFDF_OOS_TTT,          	fsm_l3bfdf_canitf_ws_comb_out_fb},
//
//	//放弃物料态：单纯等待MSG_ID_CAN_L3BFDF_WS_GIVE_UP_FB，一旦收到无误后进入FSM_STATE_L3BFDF_OOS_SCAN
//	{MSG_ID_CAN_L3BFDF_WS_GIVE_UP_FB,       	FSM_STATE_L3BFDF_OOS_TGU,          	fsm_l3bfdf_canitf_ws_give_up_fb},

    //结束点，固定定义，不要改动
    {MSG_ID_END,            					FSM_STATE_END,             			NULL},  //Ending
};

//System Level Global variables


//Task Level Global variables
gTaskL3bfdfContext_t gTaskL3bfdfContext; //波峰秤盘的总控表

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
//	#if ((HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX > HUITP_SCALE_WEIGHT_SENSOR_NBR_MAX) || (HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX > HCU_SYSMSG_L3BFDF_MAX_SENSOR_NBR))
//		#error L3BFDF module level configuration number error!
//	#endif
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

//	//配置初始化：一次性工作
//	else if ((rcv.timeId == TIMER_ID_1S_L3BFDF_SYS_CFG_WAIT_FB) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
//		if (func_l3bfdf_time_out_sys_cfg_req_process() == FAILURE){
//			HCU_ERROR_PRINT_L3BFDF("L3BFDF: Error process time out message!\n");
//		}
//	}
//	}

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
	//int ret=0, i = 0;
	UINT8 state = 0;
	//int total=0;
	//char s[200], tmp[20];

	msg_struct_uicomm_l3bfdf_cmd_req_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_uicomm_l3bfdf_cmd_req_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_uicomm_l3bfdf_cmd_req_t))){
		HCU_ERROR_PRINT_L3BFDF("L3BFDF: Receive message error!\n");
	}
	memcpy(&rcv, param_ptr, param_len);

	//根据收到的命令，分别进行控制
	state = FsmGetState(TASK_ID_L3BFDF);

	//启动命令
	if ((rcv.cmdid == HCU_SYSMSG_BFDF_UICOMM_CMDID_CFG_START) || (rcv.cmdid == HCU_SYSMSG_BFDF_UICOMM_CMDID_RESUME)){
/*		msg_struct_l3bfdf_can_sys_start_req_t snd;
		memset(&snd, 0, sizeof(msg_struct_l3bfdf_can_sys_start_req_t));
		snd.length = sizeof(msg_struct_l3bfdf_can_sys_start_req_t);
		total=0;
		for (i = 0; i< HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX; i++){
			//所有的配置过后的传感器
			if (gTaskL3bfdfContext.sensorWs[i].sensorStatus >= HCU_L3BFDF_SENSOR_WS_STATUS_CFG_CMPL){
				snd.wsBitmap[i] = TRUE;
				gTaskL3bfdfContext.sensorWs[i].sensorStatus = HCU_L3BFDF_SENSOR_WS_STATUS_START_REQ;
				gTaskL3bfdfContext.sensorWs[i].startRcvFlag = FALSE;
				total++;
			}
			else snd.wsBitmap[i] = FALSE;
		}

		memset(s, 0, sizeof(s));
		sprintf(s, "L3BFDF: Total start sensor number = %d, bitmap = ", total);
		for (i=0; i<HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX; i++){
			memset(tmp, 0, sizeof(tmp));
			sprintf(tmp, "%d/", snd.wsBitmap[i]);
			if ((strlen(s)+strlen(tmp)) < sizeof(s)) strcat(s, tmp);
		}
		strcat(s, "\n");
		HCU_DEBUG_PRINT_CRT(s);

		HCU_DEBUG_PRINT_CRT("L3BFDF: Total sensor to be start = %d\n", total);
		ret = hcu_message_send(MSG_ID_L3BFDF_CAN_SYS_START_REQ, TASK_ID_CANITFLEO, TASK_ID_L3BFDF, &snd, snd.length);
		if (ret == FAILURE) HCU_ERROR_PRINT_L3BFDF("L3BFDF: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3BFDF].taskName, zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName);

		//启动定时器
		ret = hcu_timer_start(TASK_ID_L3BFDF, TIMER_ID_1S_L3BFDF_SYS_START_WAIT_FB, \
				zHcuSysEngPar.timer.array[TIMER_ID_1S_L3BFDF_SYS_START_WAIT_FB].dur, TIMER_TYPE_PERIOD, TIMER_RESOLUTION_1S);
		if (ret == FAILURE){
			HCU_ERROR_PRINT_L3BFDF("L3BFDF: Error start timer!\n");
		}

		//设置新状态
		if (FsmSetState(TASK_ID_L3BFDF, FSM_STATE_L3BFDF_OPR_GO) == FAILURE){
			HCU_ERROR_PRINT_L3BFDF("L3BFDF: Error Set FSM State!\n");
		}*/

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



//streamId是0开始计数的
//groupId是从1开始计数的
bool func_l3bfdf_group_allocation(UINT8 streamId, UINT16 nbrGroup)
{
	int i = 0;
	//入参检查
	if ((streamId >= HCU_SYSCFG_BFDF_SNC_BOARD_NBR_MAX) || (nbrGroup == 0) || (nbrGroup > HCU_SYSCFG_BFDF_HOPPER_NBR_MAX))
		return FALSE;

	gTaskL3bfdfContext.totalGroupNbr[streamId] = nbrGroup;

	//循环赋值
	for (i=1; i<=nbrGroup; i++){
		gTaskL3bfdfContext.group[streamId][i].groupId = 0;
		gTaskL3bfdfContext.group[streamId][i].groupStatus = HCU_L3BFDF_GROUP_STATUS_ACTIVE;
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
	if ((streamId >= HCU_SYSCFG_BFDF_SNC_BOARD_NBR_MAX) || (groupId >HCU_SYSCFG_BFDF_HOPPER_NBR_MAX) || (hopperNewId >HCU_SYSCFG_BFDF_HOPPER_NBR_MAX))
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
		gTaskL3bfdfContext.hopper[streamId][fHopper].nextHopperId = hopperNewId;
		gTaskL3bfdfContext.hopper[streamId][fHopper].preHopperId = hopperNewId;
		gTaskL3bfdfContext.hopper[streamId][fHopper].groupId = groupId;
		gTaskL3bfdfContext.hopper[streamId][fHopper].hopperStatus = HCU_L3BFDF_HOPPER_STATUS_INIT_ALLOC;
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

	return TRUE;
}

bool func_l3bfdf_hopper_remove_by_tail(UINT8 streamId, UINT16 groupId)
{
	UINT16 fHopper = 0;

	//入参检查：注意起点和终点
	if ((streamId >= HCU_SYSCFG_BFDF_SNC_BOARD_NBR_MAX) || (groupId >HCU_SYSCFG_BFDF_HOPPER_NBR_MAX))
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
	if ((streamId >= HCU_SYSCFG_BFDF_SNC_BOARD_NBR_MAX) || (groupId >HCU_SYSCFG_BFDF_HOPPER_NBR_MAX) || (hopperId >HCU_SYSCFG_BFDF_HOPPER_NBR_MAX) || (hopperNewId >HCU_SYSCFG_BFDF_HOPPER_NBR_MAX))
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
	if ((streamId >= HCU_SYSCFG_BFDF_SNC_BOARD_NBR_MAX) || (groupId >HCU_SYSCFG_BFDF_HOPPER_NBR_MAX) || (hopperId >HCU_SYSCFG_BFDF_HOPPER_NBR_MAX))
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






