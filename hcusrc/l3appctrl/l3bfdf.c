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
//	{MSG_ID_CLOUDVELA_L3BFDF_DATA_REQ,          FSM_STATE_COMMON,                   fsm_l3bfdf_cloudvela_data_req},
//	{MSG_ID_CLOUDVELA_L3BFDF_DATA_CONFIRM,      FSM_STATE_COMMON,                   fsm_l3bfdf_cloudvela_data_confirm},
//	{MSG_ID_CLOUDVELA_L3BFDF_EVENT_CONFIRM,     FSM_STATE_COMMON,                   fsm_l3bfdf_cloudvela_event_confirm},
//	{MSG_ID_CLOUDVELA_L3BFDF_CTRL_REQ,          FSM_STATE_COMMON,                   fsm_l3bfdf_cloudvela_ctrl_req},
//	{MSG_ID_CLOUDVELA_L3BFDF_STATISTIC_CONFIRM, FSM_STATE_COMMON,                   fsm_l3bfdf_cloudvela_statistic_confirm},
//	//这个命令，有些命令可以在任何状态下执行：比如STOP，但有些命令必须在配置状态下，比如START
//	{MSG_ID_UICOMM_L3BFDF_CMD_REQ,       		FSM_STATE_COMMON,          			fsm_l3bfdf_uicomm_cmd_req},
//	//考虑到单个错误的查询不能影响大部队的组合，所以差错不采用独立的状态机控制，而做为公共事件进行处理
//	{MSG_ID_CAN_L3BFDF_ERROR_INQ_CMD_RESP,      FSM_STATE_COMMON,         			fsm_l3bfdf_canitf_error_inq_cmd_resp},  //只能触发数据存储，不进入组合算法的执行

//	//Normal working status：等待人工干预-登录触发
//	{MSG_ID_UICOMM_L3BFDF_CFG_REQ,     			FSM_STATE_L3BFDF_ACTIVED,          	fsm_l3bfdf_uicomm_config_req},
//
//	//配置状态：等待所有的下危机配置完成
//	{MSG_ID_CAN_L3BFDF_SYS_CFG_RESP,     		FSM_STATE_L3BFDF_OPR_CFG,          	fsm_l3bfdf_canitf_config_resp},
//	{MSG_ID_UICOMM_L3BFDF_CFG_REQ,     			FSM_STATE_L3BFDF_OPR_CFG,          	fsm_l3bfdf_uicomm_config_req},
//
//	//等待启动反馈结果：等待MSG_ID_CAN_L3BFDF_SYS_START_RESP，完成后进入FSM_STATE_L3BFDF_OOS_SCAN
//	{MSG_ID_UICOMM_L3BFDF_CFG_REQ,     			FSM_STATE_L3BFDF_OPR_GO,          	fsm_l3bfdf_uicomm_config_req},
//	{MSG_ID_CAN_L3BFDF_SYS_CFG_RESP,     		FSM_STATE_L3BFDF_OPR_GO,          	fsm_l3bfdf_canitf_config_resp},
//	{MSG_ID_CAN_L3BFDF_SYS_START_RESP,       	FSM_STATE_L3BFDF_OPR_GO,          	fsm_l3bfdf_canitf_sys_start_resp},
//	{MSG_ID_CAN_L3BFDF_SYS_STOP_RESP,     		FSM_STATE_L3BFDF_OPR_GO,          	fsm_l3bfdf_canitf_sys_stop_resp},
//
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
	int i=0;
	for (i=0; i<HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX; i++){
		gTaskL3bfdfContext.sensorWs[i].sensorWsId = i;
		gTaskL3bfdfContext.sensorWs[i].sensorStatus = HCU_L3BFDF_SENSOR_WS_STATUS_OFFLINE;
	}

//	//为搜索空间申请内存
//	gTaskL3bfdfContext.searchSpaceTotalNbr = pow(2, HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX);
//	gTaskL3bfdfContext.SearchCoefficientPointer = malloc(gTaskL3bfdfContext.searchSpaceTotalNbr * HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX * sizeof(UINT8));
//	if (gTaskL3bfdfContext.SearchCoefficientPointer == NULL)
//		HCU_ERROR_PRINT_L3BFDF("L3BFDF: Init parameter error!\n");
//	memset(gTaskL3bfdfContext.SearchCoefficientPointer, 0, gTaskL3bfdfContext.searchSpaceTotalNbr * HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX  * sizeof(UINT8));
//
//	//初始化搜索系数表
//	UINT8 *p = NULL;
//	UINT8 j=0;
//	UINT32 k=0, t=0;
//	UINT32 kUp = 0;
//	UINT32 tUp = 0, Index=0;
//	for (j=0; j<HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX; j++){
//		kUp = pow(2, HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX-j-1);
//		for (k=0; k< kUp; k++){
//			tUp = pow(2, j);
//			for (t=0; t<tUp; t++){
//				Index = j + (k * pow(2, j+1) + t) * HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX;
//				*(gTaskL3bfdfContext.SearchCoefficientPointer + Index) = 1;
//			}
//		}
//	}
//	p = gTaskL3bfdfContext.SearchCoefficientPointer;
//	char targetStr[100];
//	char tStr[10];
//	for (i=0; i<gTaskL3bfdfContext.searchSpaceTotalNbr; i++){
//		//这个长度是按照HCU_BFDF_SENSOR_WS_NBR_MAX=12来固定设定的，如果改变，改参数打印函数需要从新改变，不然会导致程序死机
//		sprintf(targetStr, "L3BFDF: Matrix Table[%d]=[", i);
//		for (j=0; j<HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX; j++){
//			sprintf(tStr, "%d", *(p+HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX*i+j));
//			strcat(targetStr, tStr);
//		}
//		strcat(targetStr, "]\n");
//		//HCU_DEBUG_PRINT_FAT(targetStr);
//	}

	//更新LocUI数据库，以便本地界面实时展示数据
//	if (dbi_HcuBfdf_StaDatainfo_save(HCU_L3BFDF_STA_DBI_TABLE_LOCALUI, gTaskL3bfdfContext.configId, &(gTaskL3bfdfContext.staLocalUi)) == FAILURE)
//			HCU_ERROR_PRINT_L3BFDF("L3BFDF: Save data to DB error!\n");

	//初始化界面交互数据
//	dbi_HcuBfdf_WmcStatusForceInvalid();
//	dbi_HcuBfdf_WmcCurComWgtUpdate(0);
//	HCU_DEBUG_PRINT_INF("L3BFDF: dbi_HcuBfdf_WmcStatusForceInvalid() set.\n");

	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_L3BFDF, FSM_STATE_L3BFDF_ACTIVED) == FAILURE)
		HCU_ERROR_PRINT_L3BFDF("L3BFDF: Error Set FSM State!\n");
	HCU_DEBUG_PRINT_FAT("L3BFDF: Enter FSM_STATE_L3BFDF_ACTIVED status, Keeping refresh here!\n");

	//测试组合程序
	//hcu_sleep(2);
	//func_l3bfdf_test_combine();

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
//
//	//启动命令：一次性工作
//	else if ((rcv.timeId == TIMER_ID_1S_L3BFDF_SYS_START_WAIT_FB) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
//		if (func_l3bfdf_time_out_sys_start_req_process() == FAILURE){
//			HCU_ERROR_PRINT_L3BFDF("L3BFDF: Error process time out message!\n");
//		}
//	}
//
//	//停止命令：一次性工作
//	else if ((rcv.timeId == TIMER_ID_1S_L3BFDF_SYS_STOP_WAIT_FB) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
//		if (func_l3bfdf_time_out_sys_stop_req_process() == FAILURE){
//			HCU_ERROR_PRINT_L3BFDF("L3BFDF: Error process time out message!\n");
//		}
//	}
//
//	//出料操作：一次性工作
//	else if ((rcv.timeId == TIMER_ID_1S_L3BFDF_TTT_WAIT_FB) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
//		if (func_l3bfdf_time_out_ttt_wait_fb_process() == FAILURE){
//			HCU_ERROR_PRINT_L3BFDF("L3BFDF: Error prceoss time out message!\n");
//		}
//	}
//
//	//抛弃料操作：一次性工作
//	else if ((rcv.timeId == TIMER_ID_1S_L3BFDF_TGU_WAIT_FB) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
//		if (func_l3bfdf_time_out_tgu_wait_fb_process() == FAILURE){
//			HCU_ERROR_PRINT_L3BFDF("L3BFDF: Error prceoss time out message!\n");
//		}
//	}
//
//	//差错查询：一次性工作
//	else if ((rcv.timeId == TIMER_ID_1S_L3BFDF_ERROR_INQ) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
//		if (func_l3bfdf_time_out_error_inq_process() == FAILURE){
//			HCU_ERROR_PRINT_L3BFDF("L3BFDF: Error prceoss time out message!\n");
//		}
//	}
//
//	//周期性统计扫描定时器
//	else if ((rcv.timeId == TIMER_ID_10MS_L3BFDF_PERIOD_STA_SCAN) &&(rcv.timeRes == TIMER_RESOLUTION_10MS)){
//		if (func_l3bfdf_time_out_statistic_scan_process() == FAILURE){
//			HCU_ERROR_PRINT_L3BFDF("L3BFDF: Error prceoss time out message!\n");
//		}
//	}

	//返回
	return SUCCESS;
}

///***************************************************************************************************************************
// *
// * 　核心状态机处理过程
// *
// ***************************************************************************************************************************/
////专门用于处理在差错状态下的状态查询
//OPSTAT fsm_l3bfdf_canitf_error_inq_cmd_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
//{
//	int ret=0;
//	int i = 0, cnt = 0;
//
//	msg_struct_can_l3bfdf_error_inq_cmd_resp_t rcv;
//	memset(&rcv, 0, sizeof(msg_struct_can_l3bfdf_error_inq_cmd_resp_t));
//	if ((param_ptr == NULL || param_len > sizeof(msg_struct_can_l3bfdf_error_inq_cmd_resp_t))){
//		HCU_ERROR_PRINT_L3BFDF("L3BFDF: Receive message error!\n");
//	}
//	memcpy(&rcv, param_ptr, param_len);
//
//	//入参检查
//	if ((rcv.sensorid > HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX) || (rcv.sensorWsValue > (gTaskL3bfdfContext.comAlgPar.TargetCombinationWeight + gTaskL3bfdfContext.comAlgPar.TargetCombinationUpperWeight))){
//		HCU_ERROR_PRINT_L3BFDF("L3BFDF: Receive message error!\n");
//	}
//
//	//先改本传感器的状态
//	if (rcv.validFlag == 1){
//		//正常处理，并回复到SCAN状态
//		if (rcv.sensorWsValue == 0) {
//			gTaskL3bfdfContext.sensorWs[rcv.sensorid].sensorStatus = HCU_L3BFDF_SENSOR_WS_STATUS_VALIID_EMPTY;
//			gTaskL3bfdfContext.wsValueNbrFree++;
//		}
//		else{
//			gTaskL3bfdfContext.sensorWs[rcv.sensorid].sensorValue = rcv.sensorWsValue;
//			gTaskL3bfdfContext.sensorWs[rcv.sensorid].sensorStatus = HCU_L3BFDF_SENSOR_WS_STATUS_VALID_TO_COMB;
//			gTaskL3bfdfContext.wsValueNbrWeight++;
//		}
//		//停止定时器
//		cnt = 0;
//		for (i=0; i<HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX; i++){
//			if(gTaskL3bfdfContext.sensorWs[rcv.sensorid].sensorStatus == HCU_L3BFDF_SENSOR_WS_STATUS_VALID_ERROR) cnt++;
//		}
//		if (cnt == 0){
//			hcu_timer_stop(TASK_ID_L3BFDF, TIMER_ID_1S_L3BFDF_ERROR_INQ, TIMER_RESOLUTION_1S);
//		}
//	}
//
//	//重发INQ_CMD_REQ
//	else{
//		gTaskL3bfdfContext.sensorWs[rcv.sensorid].sensorStatus = HCU_L3BFDF_SENSOR_WS_STATUS_VALID_ERROR;
//
//		//重新启动定时器
//		ret = hcu_timer_start(TASK_ID_L3BFDF, TIMER_ID_1S_L3BFDF_ERROR_INQ, \
//				zHcuSysEngPar.timer.array[TIMER_ID_1S_L3BFDF_ERROR_INQ].dur, TIMER_TYPE_ONE_TIME, TIMER_RESOLUTION_1S);
//		if (ret == FAILURE){
//			HCU_ERROR_PRINT_L3BFDF("L3BFDF: Error start period timer!\n");
//		}
//
//		//重新发送查询命令
//		msg_struct_l3bfdf_can_error_inq_cmd_req_t snd;
//		memset(&snd, 0, sizeof(msg_struct_l3bfdf_can_error_inq_cmd_req_t));
//		snd.length = sizeof(msg_struct_l3bfdf_can_error_inq_cmd_req_t);
//		snd.sensorid = rcv.sensorid;
//		ret = hcu_message_send(MSG_ID_L3BFDF_CAN_ERROR_INQ_CMD_REQ, TASK_ID_CANITFLEO, TASK_ID_L3BFDF, &snd, snd.length);
//		if (ret == FAILURE){
//			HCU_ERROR_PRINT_L3BFDF("L3BFDF: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3BFDF].taskName, zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName);
//		}
//
//		//返回
//		return SUCCESS;
//	}
//
//	//如果达到了条件，可以转移：这里的条件是TTT和TGU必须完成
//	//因为ERR_IND不再参与状态转移的任务，所以不需要再考虑这个问题
////	func_l3bfdf_cacluate_sensor_ws_valid_value();
////	if ((gTaskL3bfdfContext.wsValueNbrFree + gTaskL3bfdfContext.wsValueNbrWeight) == gTaskL3bfdfContext.wsValueNbrActive){
////		//停止定时器
////		ret = hcu_timer_stop(TASK_ID_L3BFDF, TIMER_ID_1S_L3BFDF_ERROR_INQ, TIMER_RESOLUTION_1S);
////		if (ret == FAILURE){
////			HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Error start timer!\n");
////		}
////
////		//设置状态机到目标状态
////		if (FsmSetState(TASK_ID_L3BFDF, FSM_STATE_L3BFDF_OOS_SCAN) == FAILURE){
////			HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Error Set FSM State!\n");
////		}
////	}
//
//	//返回
//	return SUCCESS;
//}
//
//
//OPSTAT fsm_l3bfdf_canitf_config_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
//{
//	int ret=0;
//	msg_struct_can_l3bfdf_sys_cfg_resp_t rcv;
//	memset(&rcv, 0, sizeof(msg_struct_can_l3bfdf_sys_cfg_resp_t));
//	if ((param_ptr == NULL || param_len > sizeof(msg_struct_can_l3bfdf_sys_cfg_resp_t))){
//		HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Receive message error!\n");
//	}
//	if ((rcv.sensorid < 0) || (rcv.sensorid > HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX))
//			HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Receive message error!\n");
//	memcpy(&rcv, param_ptr, param_len);
//
//	//先改本传感器的状态
//	gTaskL3bfdfContext.sensorWs[rcv.sensorid].sensorStatus = HCU_L3BFDF_SENSOR_WS_STATUS_CFG_CMPL;
//	gTaskL3bfdfContext.sensorWs[rcv.sensorid].cfgRcvFlag = TRUE;
//
//	//收到错误的反馈，就回复差错给界面
//	if (rcv.validFlag == FALSE){
//		//发送反馈给UICOMM
//		msg_struct_l3bfdf_uicomm_cfg_resp_t snd;
//		memset(&snd, 0, sizeof(msg_struct_l3bfdf_uicomm_cfg_resp_t));
//		snd.validFlag = FALSE;
//		snd.errCode = rcv.errCode;
//		snd.sensorid = rcv.sensorid;
//		snd.length = sizeof(msg_struct_l3bfdf_uicomm_cfg_resp_t);
//		ret = hcu_message_send(MSG_ID_L3BFDF_UICOMM_CFG_RESP, TASK_ID_BFDFUICOMM, TASK_ID_L3BFDF, &snd, snd.length);
//		if (ret == FAILURE){
//			HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3BFDF].taskName, zHcuVmCtrTab.task[TASK_ID_BFDFUICOMM].taskName);
//		}
//
//		//停止定时器
//		ret = hcu_timer_stop(TASK_ID_L3BFDF, TIMER_ID_1S_L3BFDF_SYS_CFG_WAIT_FB, TIMER_RESOLUTION_1S);
//		if (ret == FAILURE){
//			HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Error stop timer!\n");
//		}
//		//设置状态机
//		if (FsmSetState(TASK_ID_L3BFDF, FSM_STATE_L3BFDF_ACTIVED) == FAILURE){
//			HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Error Set FSM State!\n");
//		}
//	}
//
//	//收到正确以及齐活的反馈
//	if (func_l3bfdf_cacluate_sensor_cfg_rcv_complete() == TRUE){
//		//发送反馈给UICOMM
//		msg_struct_l3bfdf_uicomm_cfg_resp_t snd;
//		memset(&snd, 0, sizeof(msg_struct_l3bfdf_uicomm_cfg_resp_t));
//		snd.validFlag = TRUE;
//		snd.length = sizeof(msg_struct_l3bfdf_uicomm_cfg_resp_t);
//		ret = hcu_message_send(MSG_ID_L3BFDF_UICOMM_CFG_RESP, TASK_ID_BFDFUICOMM, TASK_ID_L3BFDF, &snd, snd.length);
//		if (ret == FAILURE){
//			HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3BFDF].taskName, zHcuVmCtrTab.task[TASK_ID_BFDFUICOMM].taskName);
//		}
//
//		//停止定时器
//		ret = hcu_timer_stop(TASK_ID_L3BFDF, TIMER_ID_1S_L3BFDF_SYS_CFG_WAIT_FB, TIMER_RESOLUTION_1S);
//		if (ret == FAILURE){
//			HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Error stop timer!\n");
//		}
//		//设置状态机
//		if (FsmSetState(TASK_ID_L3BFDF, FSM_STATE_L3BFDF_OPR_GO) == FAILURE){
//			HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Error Set FSM State!\n");
//		}
//	}
//
//	//收到正确以及没有齐活的反馈：直接返回
//
//	//返回
//	return SUCCESS;
//}
//
//OPSTAT fsm_l3bfdf_canitf_sys_start_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
//{
//	int ret=0;
//	msg_struct_can_l3bfdf_sys_start_resp_t rcv;
//	memset(&rcv, 0, sizeof(msg_struct_can_l3bfdf_sys_start_resp_t));
//	if ((param_ptr == NULL || param_len > sizeof(msg_struct_can_l3bfdf_sys_start_resp_t))){
//		HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Receive message error!\n");
//	}
//	if ((rcv.sensorid < 0) || (rcv.sensorid > HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX))
//			HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Receive message error!\n");
//	memcpy(&rcv, param_ptr, param_len);
//
//	//先改本传感器的状态
//	gTaskL3bfdfContext.sensorWs[rcv.sensorid].sensorStatus = HCU_L3BFDF_SENSOR_WS_STATUS_VALIID_EMPTY;
//	gTaskL3bfdfContext.sensorWs[rcv.sensorid].startRcvFlag = TRUE;
//
//	//收到错误的反馈，就回复差错给界面
//	if (rcv.validFlag == FALSE){
//		//发送反馈给UICOMM
//		msg_struct_l3bfdf_uicomm_cmd_resp_t snd;
//		memset(&snd, 0, sizeof(msg_struct_l3bfdf_uicomm_cmd_resp_t));
//		snd.validFlag = FALSE;
//		snd.errCode = rcv.errCode;
//		snd.sensorid = rcv.sensorid;
//		snd.cmdid = HCU_SYSMSG_BFDF_UICOMM_CMDID_START;
//		snd.length = sizeof(msg_struct_l3bfdf_uicomm_cmd_resp_t);
//		ret = hcu_message_send(MSG_ID_L3BFDF_UICOMM_CMD_RESP, TASK_ID_BFDFUICOMM, TASK_ID_L3BFDF, &snd, snd.length);
//		if (ret == FAILURE){
//			HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3BFDF].taskName, zHcuVmCtrTab.task[TASK_ID_BFDFUICOMM].taskName);
//		}
//
//		//停止定时器
//		ret = hcu_timer_stop(TASK_ID_L3BFDF, TIMER_ID_1S_L3BFDF_SYS_START_WAIT_FB, TIMER_RESOLUTION_1S);
//		if (ret == FAILURE){
//			HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Error stop timer!\n");
//		}
//		//设置状态机
//		if (FsmSetState(TASK_ID_L3BFDF, FSM_STATE_L3BFDF_OPR_GO) == FAILURE){
//			HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Error Set FSM State!\n");
//		}
//	}
//
//	//收到正确以及齐活的反馈
//	if (func_l3bfdf_cacluate_sensor_start_rcv_complete() == TRUE){
//		//发送反馈给UICOMM
//		msg_struct_l3bfdf_uicomm_cmd_resp_t snd;
//		memset(&snd, 0, sizeof(msg_struct_l3bfdf_uicomm_cmd_resp_t));
//		snd.cmdid = HCU_SYSMSG_BFDF_UICOMM_CMDID_START;
//		snd.validFlag = TRUE;
//		snd.length = sizeof(msg_struct_l3bfdf_uicomm_cmd_resp_t);
//		ret = hcu_message_send(MSG_ID_L3BFDF_UICOMM_CMD_RESP, TASK_ID_BFDFUICOMM, TASK_ID_L3BFDF, &snd, snd.length);
//		if (ret == FAILURE){
//			HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3BFDF].taskName, zHcuVmCtrTab.task[TASK_ID_BFDFUICOMM].taskName);
//		}
//
//		//停止定时器
//		ret = hcu_timer_stop(TASK_ID_L3BFDF, TIMER_ID_1S_L3BFDF_SYS_START_WAIT_FB, TIMER_RESOLUTION_1S);
//		if (ret == FAILURE){
//			HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Error stop timer!\n");
//		}
//
//		//启动统计扫描定时器
//		ret = hcu_timer_start(TASK_ID_L3BFDF, TIMER_ID_10MS_L3BFDF_PERIOD_STA_SCAN, \
//			zHcuSysEngPar.timer.array[TIMER_ID_10MS_L3BFDF_PERIOD_STA_SCAN].dur, TIMER_TYPE_PERIOD, TIMER_RESOLUTION_10MS);
//		if (ret == FAILURE)
//			HCU_ERROR_PRINT_L3BFDF("L3BFDF: Error start period timer!\n");
//
//		//设置工作启动时间
//		gTaskL3bfdfContext.startWorkTimeInUnix = time(0);
//
//		//设置状态机
//		if (FsmSetState(TASK_ID_L3BFDF, FSM_STATE_L3BFDF_OOS_SCAN) == FAILURE){
//			HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Error Set FSM State!\n");
//		}
//	}
//
//	//收到正确以及没有齐活的反馈：直接返回
//
//	//返回
//	return SUCCESS;
//}
//
//OPSTAT fsm_l3bfdf_canitf_sys_stop_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
//{
//	int ret=0;
//	msg_struct_can_l3bfdf_sys_stop_resp_t rcv;
//	memset(&rcv, 0, sizeof(msg_struct_can_l3bfdf_sys_stop_resp_t));
//	if ((param_ptr == NULL || param_len > sizeof(msg_struct_can_l3bfdf_sys_stop_resp_t))){
//		HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Receive message error!\n");
//	}
//	if ((rcv.sensorid < 0) || (rcv.sensorid > HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX))
//			HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Receive message error!\n");
//	memcpy(&rcv, param_ptr, param_len);
//
//	//先改本传感器的状态
//	gTaskL3bfdfContext.sensorWs[rcv.sensorid].sensorStatus = HCU_L3BFDF_SENSOR_WS_STATUS_STOP_CMPL;
//	gTaskL3bfdfContext.sensorWs[rcv.sensorid].stopRcvFlag = TRUE;
//
//	//收到错误的反馈，就回复差错给界面
//	if (rcv.validFlag == FALSE){
//		//发送反馈给UICOMM
//		msg_struct_l3bfdf_uicomm_cmd_resp_t snd;
//		memset(&snd, 0, sizeof(msg_struct_l3bfdf_uicomm_cmd_resp_t));
//		snd.validFlag = FALSE;
//		snd.errCode = rcv.errCode;
//		snd.sensorid = rcv.sensorid;
//		snd.cmdid = HCU_SYSMSG_BFDF_UICOMM_CMDID_STOP;
//		snd.length = sizeof(msg_struct_l3bfdf_uicomm_cmd_resp_t);
//		ret = hcu_message_send(MSG_ID_L3BFDF_UICOMM_CMD_RESP, TASK_ID_BFDFUICOMM, TASK_ID_L3BFDF, &snd, snd.length);
//		if (ret == FAILURE){
//			HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3BFDF].taskName, zHcuVmCtrTab.task[TASK_ID_BFDFUICOMM].taskName);
//		}
//
//		//将传感器强制设置为错误状态
//		gTaskL3bfdfContext.sensorWs[rcv.sensorid].sensorStatus = HCU_L3BFDF_SENSOR_WS_STATUS_INIT_ERR;
//
//		//停止定时器
//		ret = hcu_timer_stop(TASK_ID_L3BFDF, TIMER_ID_1S_L3BFDF_SYS_STOP_WAIT_FB, TIMER_RESOLUTION_1S);
//		if (ret == FAILURE){
//			HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Error stop timer!\n");
//		}
//		//设置状态机
//		if (FsmSetState(TASK_ID_L3BFDF, FSM_STATE_L3BFDF_OPR_GO) == FAILURE){
//			HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Error Set FSM State!\n");
//		}
//	}
//
//	//收到正确以及齐活的反馈
//	if (func_l3bfdf_cacluate_sensor_stop_rcv_complete() == TRUE){
//		//发送反馈给UICOMM
//		msg_struct_l3bfdf_uicomm_cmd_resp_t snd;
//		memset(&snd, 0, sizeof(msg_struct_l3bfdf_uicomm_cmd_resp_t));
//		snd.cmdid = HCU_SYSMSG_BFDF_UICOMM_CMDID_STOP;
//		snd.validFlag = TRUE;
//		snd.length = sizeof(msg_struct_l3bfdf_uicomm_cmd_resp_t);
//		ret = hcu_message_send(MSG_ID_L3BFDF_UICOMM_CMD_RESP, TASK_ID_BFDFUICOMM, TASK_ID_L3BFDF, &snd, snd.length);
//		if (ret == FAILURE){
//			HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3BFDF].taskName, zHcuVmCtrTab.task[TASK_ID_BFDFUICOMM].taskName);
//		}
//
//		//停止定时器
//		ret = hcu_timer_stop(TASK_ID_L3BFDF, TIMER_ID_1S_L3BFDF_SYS_STOP_WAIT_FB, TIMER_RESOLUTION_1S);
//		if (ret == FAILURE){
//			HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Error stop timer!\n");
//		}
//		//设置状态机
//		if (FsmSetState(TASK_ID_L3BFDF, FSM_STATE_L3BFDF_OPR_GO) == FAILURE){
//			HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Error Set FSM State!\n");
//		}
//	}
//
//	//收到正确以及没有齐活的反馈：直接返回
//
//	//返回
//	return SUCCESS;
//}
//
////组合反馈
//OPSTAT fsm_l3bfdf_canitf_ws_comb_out_fb(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
//{
//	int ret=0;
//	msg_struct_can_l3bfdf_ws_comb_out_fb_t rcv;
//	memset(&rcv, 0, sizeof(msg_struct_can_l3bfdf_ws_comb_out_fb_t));
//	if ((param_ptr == NULL || param_len > sizeof(msg_struct_can_l3bfdf_ws_comb_out_fb_t))){
//		HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Receive message error!\n");
//	}
//	memcpy(&rcv, param_ptr, param_len);
//
//	//先处理错误情况
//	if (rcv.validFlag == FALSE){
//		//发送命令给界面
//		msg_struct_l3bfdf_uicomm_cmd_resp_t snd;
//		memset(&snd, 0, sizeof(msg_struct_l3bfdf_uicomm_cmd_resp_t));
//		snd.cmdid = HCU_SYSMSG_BFDF_UICOMM_CMDID_SUSPEND;
//		snd.validFlag = TRUE;
//		snd.length = sizeof(msg_struct_l3bfdf_uicomm_cmd_resp_t);
//		ret = hcu_message_send(MSG_ID_L3BFDF_UICOMM_CMD_RESP, TASK_ID_BFDFUICOMM, TASK_ID_L3BFDF, &snd, snd.length);
//		if (ret == FAILURE){
//			HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3BFDF].taskName, zHcuVmCtrTab.task[TASK_ID_BFDFUICOMM].taskName);
//		}
//
//		//停止定时器
//		ret = hcu_timer_stop(TASK_ID_L3BFDF, TIMER_ID_1S_L3BFDF_TTT_WAIT_FB, TIMER_RESOLUTION_1S);
//		if (ret == FAILURE){
//			HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Error stop timer!\n");
//		}
//
//		//设置状态机
//		if (FsmSetState(TASK_ID_L3BFDF, FSM_STATE_L3BFDF_OPR_GO) == FAILURE){
//			HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Error Set FSM State!\n");
//		}
//
//		//发送STOP_REQ给所有下位机
//		msg_struct_l3bfdf_can_sys_stop_req_t snd1;
//		memset(&snd1, 0, sizeof(msg_struct_l3bfdf_can_sys_stop_req_t));
//		snd1.length = sizeof(msg_struct_l3bfdf_can_sys_stop_req_t);
//		int i = 0;
//		//所有传感器
//		for (i = 0; i< HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX; i++){
//			snd1.wsBitmap[i] = TRUE;
//		}
//		ret = hcu_message_send(MSG_ID_L3BFDF_CAN_SYS_STOP_REQ, TASK_ID_CANITFLEO, TASK_ID_L3BFDF, &snd1, snd1.length);
//		if (ret == FAILURE) HCU_ERROR_PRINT_L3BFDF("L3BFDF: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3BFDF].taskName, zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName);
//
//		//返回
//		return SUCCESS;
//	}
//
//	//恢复
//	if (gTaskL3bfdfContext.wsBitmap[rcv.sensorid] == 1){
//		gTaskL3bfdfContext.sensorWs[rcv.sensorid].sensorValue = 0;
//		gTaskL3bfdfContext.sensorWs[rcv.sensorid].sensorStatus = HCU_L3BFDF_SENSOR_WS_STATUS_VALIID_EMPTY;
//		gTaskL3bfdfContext.wsBitmap[rcv.sensorid] = 0;
//		gTaskL3bfdfContext.wsValueNbrFree++;
//		if (gTaskL3bfdfContext.wsValueNbrTtt>0) gTaskL3bfdfContext.wsValueNbrTtt--;
//	}
//
//	//检查是否收到所有传感器的反馈
//	if (gTaskL3bfdfContext.wsValueNbrTtt == 0){
//		//停止定时器
//		ret = hcu_timer_stop(TASK_ID_L3BFDF, TIMER_ID_1S_L3BFDF_TTT_WAIT_FB, TIMER_RESOLUTION_1S);
//		if (ret == FAILURE){
//			HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Error stop timer!\n");
//		}
//		//设置状态机
//		if (FsmSetState(TASK_ID_L3BFDF, FSM_STATE_L3BFDF_OOS_SCAN) == FAILURE){
//			HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Error Set FSM State!\n");
//		}
//		//打印二维码／条形码：二维码＋条形码的内容
//		char s[100];
//		time_t lt;
//		struct tm *cu;
//		memset(s, 0, sizeof(s));
//
//		//初始本地时间
//		lt=time(NULL);
//		cu = localtime(&lt);
//		cu->tm_mon = cu->tm_mon + 1; //月份是从0-11的，+1是为了符合正常逻辑
//		sprintf(s, "BOFENGZHINENG-%s-%4.2fg-%04d.%02d.%02d.%02d:%02d:%02d", gTaskL3bfdfContext.configName, (float)gTaskL3bfdfContext.comAlgPar.TargetCombinationWeight, \
//				(UINT16)(1900+cu->tm_year), (UINT8)cu->tm_mon, (UINT8)cu->tm_mday, (UINT8)cu->tm_hour, (UINT8)cu->tm_min, (UINT8)cu->tm_sec);
//		hcu_sps232_send_char_to_ext_printer(s, strlen(s));
//	}
//
//	//返回
//	return SUCCESS;
//}
//
//OPSTAT fsm_l3bfdf_canitf_ws_give_up_fb(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
//{
//	int ret=0;
//	msg_struct_can_l3bfdf_ws_give_up_fb_t rcv;
//	memset(&rcv, 0, sizeof(msg_struct_can_l3bfdf_ws_give_up_fb_t));
//	if ((param_ptr == NULL || param_len > sizeof(msg_struct_can_l3bfdf_ws_give_up_fb_t))){
//		HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Receive message error!\n");
//	}
//	memcpy(&rcv, param_ptr, param_len);
//
//	//先处理错误情况
//	if (rcv.validFlag == FALSE){
//		//发送命令给界面
//		msg_struct_l3bfdf_uicomm_cmd_resp_t snd;
//		memset(&snd, 0, sizeof(msg_struct_l3bfdf_uicomm_cmd_resp_t));
//		snd.cmdid = HCU_SYSMSG_BFDF_UICOMM_CMDID_SUSPEND;
//		snd.validFlag = TRUE;
//		snd.length = sizeof(msg_struct_l3bfdf_uicomm_cmd_resp_t);
//		ret = hcu_message_send(MSG_ID_L3BFDF_UICOMM_CMD_RESP, TASK_ID_BFDFUICOMM, TASK_ID_L3BFDF, &snd, snd.length);
//		if (ret == FAILURE){
//			HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3BFDF].taskName, zHcuVmCtrTab.task[TASK_ID_BFDFUICOMM].taskName);
//		}
//
//		//停止定时器
//		ret = hcu_timer_stop(TASK_ID_L3BFDF, TIMER_ID_1S_L3BFDF_TGU_WAIT_FB, TIMER_RESOLUTION_1S);
//		if (ret == FAILURE){
//			HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Error stop timer!\n");
//		}
//
//		//设置状态机
//		if (FsmSetState(TASK_ID_L3BFDF, FSM_STATE_L3BFDF_OPR_GO) == FAILURE){
//			HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Error Set FSM State!\n");
//		}
//
//		//发送STOP_REQ给所有下位机
//		msg_struct_l3bfdf_can_sys_stop_req_t snd1;
//		memset(&snd1, 0, sizeof(msg_struct_l3bfdf_can_sys_stop_req_t));
//		snd1.length = sizeof(msg_struct_l3bfdf_can_sys_stop_req_t);
//		int i = 0;
//		//所有传感器
//		for (i = 0; i< HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX; i++){
//			snd1.wsBitmap[i] = TRUE;
//		}
//		ret = hcu_message_send(MSG_ID_L3BFDF_CAN_SYS_STOP_REQ, TASK_ID_CANITFLEO, TASK_ID_L3BFDF, &snd1, snd1.length);
//		if (ret == FAILURE) HCU_ERROR_PRINT_L3BFDF("L3BFDF: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3BFDF].taskName, zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName);
//
//		//返回
//		return SUCCESS;
//	}
//
//	//恢复
//	if (gTaskL3bfdfContext.wsBitmap[rcv.sensorid] == 1){
//		gTaskL3bfdfContext.sensorWs[rcv.sensorid].sensorValue = 0;
//		gTaskL3bfdfContext.sensorWs[rcv.sensorid].sensorStatus = HCU_L3BFDF_SENSOR_WS_STATUS_VALIID_EMPTY;
//		gTaskL3bfdfContext.wsBitmap[rcv.sensorid] = 0;
//		gTaskL3bfdfContext.wsValueNbrFree++;
//		if (gTaskL3bfdfContext.wsValueNbrTgu>0) gTaskL3bfdfContext.wsValueNbrTgu--;
//	}
//
//	//检查是否收到所有传感器的反馈
//	if (gTaskL3bfdfContext.wsValueNbrTgu == 0){
//		//停止定时器
//		ret = hcu_timer_stop(TASK_ID_L3BFDF, TIMER_ID_1S_L3BFDF_TGU_WAIT_FB, TIMER_RESOLUTION_1S);
//		if (ret == FAILURE){
//			HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Error stop timer!\n");
//		}
//		//设置状态机
//		if (FsmSetState(TASK_ID_L3BFDF, FSM_STATE_L3BFDF_OOS_SCAN) == FAILURE){
//			HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Error Set FSM State!\n");
//		}
//	}
//
//	//返回
//	return SUCCESS;
//}
//
///***************************************************************************************************************************
// *
// *  CLOUDVELA部分的消息处理
// *
// ***************************************************************************************************************************/
////后台来的命令，发送到CANITFLEO模块，执行命令后，再返回给后台
////由于是内部消息命令执行，为了简化整个执行，不设置超时状态，以简化整个状态机的设计
//OPSTAT fsm_l3bfdf_cloudvela_data_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
//{
//	return SUCCESS;
//}
//
////由于是内部消息命令执行，为了简化整个执行，不设置超时状态，以简化整个状态机的设计
//OPSTAT fsm_l3bfdf_cloudvela_data_confirm(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
//{
//	return SUCCESS;
//}
//
////由于是内部消息命令执行，为了简化整个执行，不设置超时状态，以简化整个状态机的设计
//OPSTAT fsm_l3bfdf_cloudvela_event_confirm(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
//{
//	return SUCCESS;
//}
//
////由于是内部消息命令执行，为了简化整个执行，不设置超时状态，以简化整个状态机的设计
//OPSTAT fsm_l3bfdf_cloudvela_ctrl_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
//{
//	//int ret=0;
//
//	//入参检查
//	msg_struct_cloudvela_l3bfdf_ctrl_req_t rcv;
//	memset(&rcv, 0, sizeof(msg_struct_cloudvela_l3bfdf_ctrl_req_t));
//	if ((param_ptr == NULL || param_len > sizeof(msg_struct_cloudvela_l3bfdf_ctrl_req_t))){
//		HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Receive message error!\n");
//	}
//	memcpy(&rcv, param_ptr, param_len);
////	if (rcv.cmdid != L3CI_bfdf_comb_scale){
////		HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Receive message error!\n");
////	}
//
//	//将命令发送到传感器下位机
////	msg_struct_l3bfdf_can_general_cmd_req_t snd;
////	memset(&snd, 0, sizeof(msg_struct_l3bfdf_can_general_cmd_req_t));
////	snd.length = sizeof(msg_struct_l3bfdf_can_general_cmd_req_t);
//
//	//这里只支持一种启动和停止命令，其它的暂时不支持
//	//if ((rcv.optid != L3PO_bfdf_start_cmd) && (rcv.optid != L3PO_bfdf_stop_cmd)){
////	if ((rcv.scaleWeightCmd != L3PO_bfdf_start_cmd) && (rcv.scaleWeightCmd != L3PO_bfdf_stop_cmd)){
////				HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Can not supported command coming from cloud!\n");
////	}
//
////	snd.optid = rcv.optid;
////	snd.optpar = rcv.optopr;
//	//这里如此设置，表示是为了全局所有的传感器
////	snd.sensorid = HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX;
////	ret = hcu_message_send(MSG_ID_L3BFDF_CAN_GENERAL_CMD_REQ, TASK_ID_CANITFLEO, TASK_ID_L3BFDF, &snd, snd.length);
////	if (ret == FAILURE){
////		HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3BFDF].taskName, zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName);
////	}
//
//	//状态不转移
//
//	//短时钟不启动
//
//	//返回
//	return SUCCESS;
//}
//
////由于是内部消息命令执行，为了简化整个执行，不设置超时状态，以简化整个状态机的设计
//OPSTAT fsm_l3bfdf_cloudvela_statistic_confirm(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
//{
//	return SUCCESS;
//}
//
//
//
///***************************************************************************************************************************
// *
// *  CANITF下位机传感器处理的结果
// *
// ***************************************************************************************************************************/
////触发组合算法
//OPSTAT fsm_l3bfdf_canitf_ws_new_ready_event(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
//{
//	int ret=0, i=0;
//
//	//入参检查
//	msg_struct_can_l3bfdf_new_ready_event_t rcv;
//	memset(&rcv, 0, sizeof(msg_struct_can_l3bfdf_new_ready_event_t));
//	if ((param_ptr == NULL || param_len > sizeof(msg_struct_can_l3bfdf_new_ready_event_t))){
//		HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Receive message error!\n");
//	}
//	memcpy(&rcv, param_ptr, param_len);
//
//	if ((rcv.sensorid > HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX) || (rcv.sensorWsValue > (gTaskL3bfdfContext.comAlgPar.TargetCombinationWeight + gTaskL3bfdfContext.comAlgPar.TargetCombinationUpperWeight))){
//		HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Receive message error, SensorId = %d, WsWeight=%d!\n", rcv.sensorid, rcv.sensorWsValue);
//	}
//
//	//Test Print
//	char s[200], tmp[20];
//	memset(s, 0, sizeof(s));
//	sprintf(s, "L3BFDF: All sensor running state = [");
//	for (i=0; i<HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX; i++){
//		memset(tmp, 0, sizeof(tmp));
//		sprintf(tmp, "%d/", gTaskL3bfdfContext.sensorWs[i].sensorStatus);
//		if ((strlen(s)+strlen(tmp)) < sizeof(s)) strcat(s, tmp);
//	}
//	strcat(s, "]\n");
//	HCU_DEBUG_PRINT_CRT(s);
//
//	//正常处理
//	gTaskL3bfdfContext.cur.wsIncMatCnt++;
//	gTaskL3bfdfContext.cur.wsIncMatWgt += rcv.sensorWsValue;
//	HCU_DEBUG_PRINT_FAT("L3BFDF: Sensor Input Id = %d, Status = %d\n", rcv.sensorid, gTaskL3bfdfContext.sensorWs[rcv.sensorid].sensorStatus);
//
//	//将重复次数清零：这个参数用于增加该传感器被算法命中的概率
//	gTaskL3bfdfContext.sensorWs[rcv.sensorid].sensorRepTimes = 0;
//
//	//根据不同情况进行处理：正常空闲态
//	if (gTaskL3bfdfContext.sensorWs[rcv.sensorid].sensorStatus == HCU_L3BFDF_SENSOR_WS_STATUS_VALIID_EMPTY){
//		gTaskL3bfdfContext.sensorWs[rcv.sensorid].sensorValue = rcv.sensorWsValue;
//		gTaskL3bfdfContext.sensorWs[rcv.sensorid].sensorStatus = HCU_L3BFDF_SENSOR_WS_STATUS_VALID_TO_COMB;
//		if (gTaskL3bfdfContext.wsValueNbrFree>0) gTaskL3bfdfContext.wsValueNbrFree--;
//		gTaskL3bfdfContext.wsValueNbrWeight++;
//		gTaskL3bfdfContext.wsValueNbrWeight = gTaskL3bfdfContext.wsValueNbrWeight % (HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX + 1);
//		HCU_DEBUG_PRINT_NOR("L3BFDF: Receive EVENT, Normally status!\n");
//	}
//
//	//重复来临
//	else if (gTaskL3bfdfContext.sensorWs[rcv.sensorid].sensorStatus == HCU_L3BFDF_SENSOR_WS_STATUS_VALID_TO_COMB){
//		gTaskL3bfdfContext.sensorWs[rcv.sensorid].sensorValue = rcv.sensorWsValue;
//		HCU_DEBUG_PRINT_NOR("L3BFDF: Receive EVENT, Repeat status!\n");
//	}
//
//	//恢复错误
//	else if (gTaskL3bfdfContext.sensorWs[rcv.sensorid].sensorStatus == HCU_L3BFDF_SENSOR_WS_STATUS_VALID_ERROR){
//		gTaskL3bfdfContext.sensorWs[rcv.sensorid].sensorValue = rcv.sensorWsValue;
//		gTaskL3bfdfContext.sensorWs[rcv.sensorid].sensorStatus = HCU_L3BFDF_SENSOR_WS_STATUS_VALID_TO_COMB;
//		gTaskL3bfdfContext.wsValueNbrWeight++;
//		gTaskL3bfdfContext.wsValueNbrWeight = gTaskL3bfdfContext.wsValueNbrWeight % (HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX + 1);
//		HCU_DEBUG_PRINT_NOR("L3BFDF: Receive EVENT, Error recover status!\n");
//	}
//
//	//错误来临：进入ERROR_INQ流程 (TTT/TGU或者其它状态）
//	else
//	{
//		HCU_DEBUG_PRINT_NOR("L3BFDF: Receive EVENT, Error status!\n");
//		//为了全局控制表统计的正确性
//		if (gTaskL3bfdfContext.sensorWs[rcv.sensorid].sensorStatus == HCU_L3BFDF_SENSOR_WS_STATUS_VALID_TO_TTT) {
//			if (gTaskL3bfdfContext.wsValueNbrTtt > 0) gTaskL3bfdfContext.wsValueNbrTtt--;
//		}
//		else if (gTaskL3bfdfContext.sensorWs[rcv.sensorid].sensorStatus == HCU_L3BFDF_SENSOR_WS_STATUS_VALID_TO_TGU) {
//			if (gTaskL3bfdfContext.wsValueNbrTgu > 0) gTaskL3bfdfContext.wsValueNbrTgu--;
//		}
//		gTaskL3bfdfContext.sensorWs[rcv.sensorid].sensorStatus = HCU_L3BFDF_SENSOR_WS_STATUS_VALID_ERROR;
//
//		//启动定时器
//		ret = hcu_timer_start(TASK_ID_L3BFDF, TIMER_ID_1S_L3BFDF_ERROR_INQ, \
//				zHcuSysEngPar.timer.array[TIMER_ID_1S_L3BFDF_ERROR_INQ].dur, TIMER_TYPE_ONE_TIME, TIMER_RESOLUTION_1S);
//		if (ret == FAILURE){
//			HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Error start period timer!\n");
//		}
//
//		//发送查询命令
//		msg_struct_l3bfdf_can_error_inq_cmd_req_t snd0;
//		memset(&snd0, 0, sizeof(msg_struct_l3bfdf_can_error_inq_cmd_req_t));
//		snd0.length = sizeof(msg_struct_l3bfdf_can_error_inq_cmd_req_t);
//		snd0.sensorid = rcv.sensorid;
//		ret = hcu_message_send(MSG_ID_L3BFDF_CAN_ERROR_INQ_CMD_REQ, TASK_ID_CANITFLEO, TASK_ID_L3BFDF, &snd0, snd0.length);
//		if (ret == FAILURE) {
//			HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3BFDF].taskName, zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName);
//		}
//
//		return SUCCESS;
//	}
//
//	//重新统计各个参数
//	func_l3bfdf_cacluate_sensor_ws_valid_value();
//
//	//这里不可能出现以下情况
//	if ((gTaskL3bfdfContext.wsValueNbrTtt !=0) && (gTaskL3bfdfContext.wsValueNbrTgu !=0)){
//		HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Wrong TTT/TGU figure during SCAN mode!\n");
//	}
//
//	//是否要进入搜索
//	if (gTaskL3bfdfContext.wsValueNbrWeight >= gTaskL3bfdfContext.comAlgPar.MinScaleNumberStartCombination){
//		//组合失败
//		HCU_DEBUG_PRINT_NOR("L3BFDF: Enter Combination search!\n");
//		if (func_l3bfdf_ws_sensor_search_combination() == -1){
//			HCU_DEBUG_PRINT_NOR("L3BFDF: Combination search finished, None find!\n");
//			if (gTaskL3bfdfContext.wsValueNbrWeight >= (HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX-1)){
//				//得到抛弃的传感器
//				func_l3bfdf_ws_sensor_search_give_up();
//
//				gTaskL3bfdfContext.cur.wsTgvTimes++;
//				gTaskL3bfdfContext.cur.wsTgvMatCnt += func_l3bfdf_cacluate_sensor_ws_bitmap_valid_number();
//				gTaskL3bfdfContext.cur.wsTgvMatWgt += func_l3bfdf_cacluate_sensor_ws_bitmap_valid_weight();
//
//				//发送抛弃命令
//				msg_struct_l3bfdf_can_ws_give_up_t snd1;
//				memset(&snd1, 0, sizeof(msg_struct_l3bfdf_can_ws_give_up_t));
//				snd1.length = sizeof(msg_struct_l3bfdf_can_ws_give_up_t);
//				memcpy(snd1.sensorBitmap, gTaskL3bfdfContext.wsBitmap, HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX);
//				ret = hcu_message_send(MSG_ID_L3BFDF_CAN_WS_GIVE_UP, TASK_ID_CANITFLEO, TASK_ID_L3BFDF, &snd1, snd1.length);
//				if (ret == FAILURE){
//					HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3BFDF].taskName, zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName);
//				}
//
//				//更新传感器状态
//				for (i=0; i<HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX; i++){
//					if (gTaskL3bfdfContext.wsBitmap[i] == 1) gTaskL3bfdfContext.sensorWs[i].sensorStatus = HCU_L3BFDF_SENSOR_WS_STATUS_VALID_TGU_START;
//				}
//
//				//测试打印
//				char targetStr[100];
//				char tStr[10];
//				sprintf(targetStr, "L3BFDF: Send out TGU_OUT bitmap [%d]=[", gTaskL3bfdfContext.wsValueNbrTgu);
//				for (i=0; i<HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX; i++){
//					sprintf(tStr, "%d", *(gTaskL3bfdfContext.wsBitmap+i));
//					strcat(targetStr, tStr);
//				}
//				strcat(targetStr, "]\n");
//				HCU_DEBUG_PRINT_CRT(targetStr);
//
//				//启动定时器
//				ret = hcu_timer_start(TASK_ID_L3BFDF, TIMER_ID_1S_L3BFDF_TGU_WAIT_FB, \
//						zHcuSysEngPar.timer.array[TIMER_ID_1S_L3BFDF_TGU_WAIT_FB].dur, TIMER_TYPE_ONE_TIME, TIMER_RESOLUTION_1S);
//				if (ret == FAILURE){
//					HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Error start timer!\n");
//				}
//
//				//进入TGU状态，设置新状态
//				if (FsmSetState(TASK_ID_L3BFDF, FSM_STATE_L3BFDF_OOS_TGU) == FAILURE){
//					HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Error Set FSM State!\n");
//				}
//			} //== HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX
//			//对于未到最大物料压秤的情形，不予理睬
//		}//-1
//
//		//返回有意义的数值
//		else{
//			HCU_DEBUG_PRINT_NOR("L3BFDF: Combination search finished, Success!\n");
//			//发送出料命令
//			gTaskL3bfdfContext.cur.wsTttTimes++;
//			gTaskL3bfdfContext.cur.wsTttMatCnt += func_l3bfdf_cacluate_sensor_ws_bitmap_valid_number();
//			gTaskL3bfdfContext.cur.wsTttMatWgt += func_l3bfdf_cacluate_sensor_ws_bitmap_valid_weight();
//
//			msg_struct_l3bfdf_can_ws_comb_out_t snd2;
//			memset(&snd2, 0, sizeof(msg_struct_l3bfdf_can_ws_comb_out_t));
//			snd2.length = sizeof(msg_struct_l3bfdf_can_ws_comb_out_t);
//			memcpy(snd2.sensorBitmap, gTaskL3bfdfContext.wsBitmap, HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX);
//			snd2.combnbr = gTaskL3bfdfContext.wsValueNbrTtt;
//			ret = hcu_message_send(MSG_ID_L3BFDF_CAN_WS_COMB_OUT, TASK_ID_CANITFLEO, TASK_ID_L3BFDF, &snd2, snd2.length);
//			if (ret == FAILURE){
//				HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3BFDF].taskName, zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName);
//			}
//
//			//更新传感器状态
//			for (i=0; i<HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX; i++){
//				if (gTaskL3bfdfContext.wsBitmap[i] == 1) gTaskL3bfdfContext.sensorWs[i].sensorStatus = HCU_L3BFDF_SENSOR_WS_STATUS_VALID_TTT_START;
//			}
//
//			//测试打印
//			char targetStr[100];
//			char tStr[10];
//			sprintf(targetStr, "L3BFDF: Send out TTT_OUT bitmap [%d]=[", gTaskL3bfdfContext.wsValueNbrTtt);
//			for (i=0; i<HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX; i++){
//				sprintf(tStr, "%d", *(gTaskL3bfdfContext.wsBitmap+i));
//				strcat(targetStr, tStr);
//			}
//			strcat(targetStr, "]\n");
//			HCU_DEBUG_PRINT_CRT(targetStr);
//
//			//GET comb total value and store into DB
//			int totalWgt = 0;
//			for (i=0; i<HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX; i++){
//				if (gTaskL3bfdfContext.wsBitmap[i] == 1)
//				totalWgt+= gTaskL3bfdfContext.sensorWs[i].sensorValue;
//			}
//			dbi_HcuBfdf_WmcCurComWgtUpdate(totalWgt);
//
//			//启动定时器
//			ret = hcu_timer_start(TASK_ID_L3BFDF, TIMER_ID_1S_L3BFDF_TTT_WAIT_FB, \
//					zHcuSysEngPar.timer.array[TIMER_ID_1S_L3BFDF_TTT_WAIT_FB].dur, TIMER_TYPE_ONE_TIME, TIMER_RESOLUTION_1S);
//			if (ret == FAILURE){
//				HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Error start timer!\n");
//			}
//
//			//进入TTT状态
//			if (FsmSetState(TASK_ID_L3BFDF, FSM_STATE_L3BFDF_OOS_TTT) == FAILURE){
//				HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Error Set FSM State!\n");
//			}
//		}//返回有意义的数值
//	} //>= gTaskL3bfdfContext.comAlgPar.MinScaleNumberStartCombination
//	//对于不足以启动搜索的情形，不予理睬
//
//	//返回
//	return SUCCESS;
//}
//
///***************************************************************************************************************************
// *
// *  本地界面UI通信部分
// *
// ***************************************************************************************************************************/
////从BFDFUICOMM来的消息和命令
//OPSTAT fsm_l3bfdf_uicomm_config_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
//{
//	int ret=0, i=0;
//	int total=0;
//	msg_struct_uicomm_l3bfdf_cfg_req_t rcv;
//	memset(&rcv, 0, sizeof(msg_struct_uicomm_l3bfdf_cfg_req_t));
//	if ((param_ptr == NULL || param_len > sizeof(msg_struct_uicomm_l3bfdf_cfg_req_t))){
//		HCU_ERROR_PRINT_L3BFDF("L3BFDF: Receive message error!\n");
//	}
//	memcpy(&rcv, param_ptr, param_len);
//
//	//先数一数传感器的启动状态
//	if (func_l3bfdf_count_numbers_of_startup_ws_sensors() == 0){
//		HCU_ERROR_PRINT_L3BFDF("L3BFDF: No any weight scale sensor startup yet!\n");
//	}
//
//	//设置完成后，发送初始化命令给各个传感器
//	msg_struct_l3bfdf_can_sys_cfg_req_t snd;
//	memset(&snd, 0, sizeof(msg_struct_l3bfdf_can_sys_cfg_req_t));
//	total=0;
//	for (i=0; i<HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX; i++){
//		//将所有启动的传感器全部归化到配置状态
//		if (gTaskL3bfdfContext.sensorWs[i].sensorStatus > HCU_L3BFDF_SENSOR_WS_STATUS_INIT_MIN){
//			snd.wsBitmap[i] = TRUE;
//			gTaskL3bfdfContext.sensorWs[i].sensorStatus = HCU_L3BFDF_SENSOR_WS_STATUS_CFG_REQ;
//			gTaskL3bfdfContext.sensorWs[i].cfgRcvFlag = FALSE;
//			total++;
//		}
//		else{
//			snd.wsBitmap[i] = FALSE;
//		}
//	}
//
//	char s[200], tmp[20];
//	memset(s, 0, sizeof(s));
//	sprintf(s, "L3BFDF: Total config sensor number = %d, bitmap = ", total);
//	for (i=0; i<HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX; i++){
//		memset(tmp, 0, sizeof(tmp));
//		sprintf(tmp, "%d/", snd.wsBitmap[i]);
//		if ((strlen(s)+strlen(tmp)) < sizeof(s)) strcat(s, tmp);
//	}
//	strcat(s, "\n");
//	HCU_DEBUG_PRINT_CRT(s);
//
//	snd.length = sizeof(msg_struct_l3bfdf_can_sys_cfg_req_t);
//	ret = hcu_message_send(MSG_ID_L3BFDF_CAN_SYS_CFG_REQ, TASK_ID_CANITFLEO, TASK_ID_L3BFDF, &snd, snd.length);
//	if (ret == FAILURE){
//		HCU_ERROR_PRINT_L3BFDF("L3BFDF: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3BFDF].taskName, zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName);
//	}
//
//	//启动定时器
//	ret = hcu_timer_start(TASK_ID_L3BFDF, TIMER_ID_1S_L3BFDF_SYS_CFG_WAIT_FB, \
//			zHcuSysEngPar.timer.array[TIMER_ID_1S_L3BFDF_SYS_CFG_WAIT_FB].dur, TIMER_TYPE_ONE_TIME, TIMER_RESOLUTION_1S);
//	if (ret == FAILURE){
//		HCU_ERROR_PRINT_L3BFDF("L3BFDF: Error start timer!\n");
//	}
//
//	//设置新状态
//	if (FsmSetState(TASK_ID_L3BFDF, FSM_STATE_L3BFDF_OPR_CFG) == FAILURE){
//		HCU_ERROR_PRINT_L3BFDF("L3BFDF: Error Set FSM State!\n");
//	}
//
//	//返回
//	return SUCCESS;
//}
//
//OPSTAT fsm_l3bfdf_uicomm_cmd_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
//{
//	int ret=0, i = 0;
//	UINT8 state = 0;
//	int total=0;
//	char s[200], tmp[20];
//
//	msg_struct_uicomm_l3bfdf_cmd_req_t rcv;
//	memset(&rcv, 0, sizeof(msg_struct_uicomm_l3bfdf_cmd_req_t));
//	if ((param_ptr == NULL || param_len > sizeof(msg_struct_uicomm_l3bfdf_cmd_req_t))){
//		HCU_ERROR_PRINT_L3BFDF("L3BFDF: Receive message error!\n");
//	}
//	memcpy(&rcv, param_ptr, param_len);
//
//	//根据收到的命令，分别进行控制
//	state = FsmGetState(TASK_ID_L3BFDF);
//
//	//启动命令
//	if (((state == FSM_STATE_L3BFDF_OPR_GO) && (rcv.cmdid == HCU_SYSMSG_BFDF_UICOMM_CMDID_START)) ||\
//			((state == FSM_STATE_L3BFDF_OPR_GO) && (rcv.cmdid == HCU_SYSMSG_BFDF_UICOMM_CMDID_RESUME))){
//		msg_struct_l3bfdf_can_sys_start_req_t snd;
//		memset(&snd, 0, sizeof(msg_struct_l3bfdf_can_sys_start_req_t));
//		snd.length = sizeof(msg_struct_l3bfdf_can_sys_start_req_t);
//		total=0;
//		for (i = 0; i< HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX; i++){
//			//所有的配置过后的传感器
//			if (gTaskL3bfdfContext.sensorWs[i].sensorStatus >= HCU_L3BFDF_SENSOR_WS_STATUS_CFG_CMPL){
//				snd.wsBitmap[i] = TRUE;
//				gTaskL3bfdfContext.sensorWs[i].sensorStatus = HCU_L3BFDF_SENSOR_WS_STATUS_START_REQ;
//				gTaskL3bfdfContext.sensorWs[i].startRcvFlag = FALSE;
//				total++;
//			}
//			else snd.wsBitmap[i] = FALSE;
//		}
//
//		memset(s, 0, sizeof(s));
//		sprintf(s, "L3BFDF: Total start sensor number = %d, bitmap = ", total);
//		for (i=0; i<HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX; i++){
//			memset(tmp, 0, sizeof(tmp));
//			sprintf(tmp, "%d/", snd.wsBitmap[i]);
//			if ((strlen(s)+strlen(tmp)) < sizeof(s)) strcat(s, tmp);
//		}
//		strcat(s, "\n");
//		HCU_DEBUG_PRINT_CRT(s);
//
//		HCU_DEBUG_PRINT_CRT("L3BFDF: Total sensor to be start = %d\n", total);
//		ret = hcu_message_send(MSG_ID_L3BFDF_CAN_SYS_START_REQ, TASK_ID_CANITFLEO, TASK_ID_L3BFDF, &snd, snd.length);
//		if (ret == FAILURE) HCU_ERROR_PRINT_L3BFDF("L3BFDF: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3BFDF].taskName, zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName);
//
//		//启动定时器
//		ret = hcu_timer_start(TASK_ID_L3BFDF, TIMER_ID_1S_L3BFDF_SYS_START_WAIT_FB, \
//				zHcuSysEngPar.timer.array[TIMER_ID_1S_L3BFDF_SYS_START_WAIT_FB].dur, TIMER_TYPE_PERIOD, TIMER_RESOLUTION_1S);
//		if (ret == FAILURE){
//			HCU_ERROR_PRINT_L3BFDF("L3BFDF: Error start timer!\n");
//		}
//
//		//设置新状态
//		if (FsmSetState(TASK_ID_L3BFDF, FSM_STATE_L3BFDF_OPR_GO) == FAILURE){
//			HCU_ERROR_PRINT_L3BFDF("L3BFDF: Error Set FSM State!\n");
//		}
//
//	}
//
//	//停止命令：针对所有的传感器来说的
//	else if (rcv.cmdid == HCU_SYSMSG_BFDF_UICOMM_CMDID_STOP){
//		msg_struct_l3bfdf_can_sys_stop_req_t snd;
//		memset(&snd, 0, sizeof(msg_struct_l3bfdf_can_sys_stop_req_t));
//		snd.length = sizeof(msg_struct_l3bfdf_can_sys_stop_req_t);
//		total=0;
//		for (i = 0; i< HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX; i++){
//			//所有的非离线传感器
//			if (gTaskL3bfdfContext.sensorWs[i].sensorStatus > HCU_L3BFDF_SENSOR_WS_STATUS_OFFLINE){
//				snd.wsBitmap[i] = TRUE;
//				gTaskL3bfdfContext.sensorWs[i].sensorStatus = HCU_L3BFDF_SENSOR_WS_STATUS_STOP_REQ;
//				gTaskL3bfdfContext.sensorWs[i].stopRcvFlag = FALSE;
//				total++;
//			}
//			else snd.wsBitmap[i] = FALSE;
//		}
//
//		memset(s, 0, sizeof(s));
//		sprintf(s, "L3BFDF: Total stop sensor number = %d, bitmap = ", total);
//		for (i=0; i<HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX; i++){
//			memset(tmp, 0, sizeof(tmp));
//			sprintf(tmp, "%d/", snd.wsBitmap[i]);
//			if ((strlen(s)+strlen(tmp)) < sizeof(s)) strcat(s, tmp);
//		}
//		strcat(s, "\n");
//		HCU_DEBUG_PRINT_CRT(s);
//
//		ret = hcu_message_send(MSG_ID_L3BFDF_CAN_SYS_STOP_REQ, TASK_ID_CANITFLEO, TASK_ID_L3BFDF, &snd, snd.length);
//		if (ret == FAILURE) HCU_ERROR_PRINT_L3BFDF("L3BFDF: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3BFDF].taskName, zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName);
//
//		//启动定时器
//		ret = hcu_timer_start(TASK_ID_L3BFDF, TIMER_ID_1S_L3BFDF_SYS_STOP_WAIT_FB, \
//				zHcuSysEngPar.timer.array[TIMER_ID_1S_L3BFDF_SYS_STOP_WAIT_FB].dur, TIMER_TYPE_PERIOD, TIMER_RESOLUTION_1S);
//		if (ret == FAILURE){
//			HCU_ERROR_PRINT_L3BFDF("L3BFDF: Error start timer!\n");
//		}
//
//		//设置新状态
//		if (FsmSetState(TASK_ID_L3BFDF, FSM_STATE_L3BFDF_OPR_GO) == FAILURE){
//			HCU_ERROR_PRINT_L3BFDF("L3BFDF: Error Set FSM State!\n");
//		}
//	}
//
//	//差错
//	else HCU_ERROR_PRINT_L3BFDF("L3BFDF: Receive message error!\n");
//
//	//返回
//	return SUCCESS;
//}
//
//
///***************************************************************************************************************************
// *
// * 　组合秤核心算法
// *
// ***************************************************************************************************************************/
////系统参数的合法性检查，均在参数初始化中完成，后面不再检查
//INT32 func_l3bfdf_ws_sensor_search_combination(void)
//{
//	UINT32 WsSensorStart = 0;
//	int i=0;
//
//	UINT32 result=0, result_Ul=0, result_Dl=0;
//	int promoxityUl=-1, promoxityDl=-1;
//
//	UINT8 resBitmap[HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX];
//	UINT8 resBitmap_Ul[HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX];
//	UINT8 resBitmap_Dl[HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX];
//
//	UINT8 searchNbr = 0;
//
//	//选取起始位置
//	WsSensorStart = gTaskL3bfdfContext.wsRrSearchStart % gTaskL3bfdfContext.searchSpaceTotalNbr;
//	memset(resBitmap_Ul, 0, sizeof(resBitmap_Ul));
//	memset(resBitmap_Dl, 0, sizeof(resBitmap_Dl));
//
//	//组合搜索
//	result_Ul=0; result_Dl=0; promoxityUl=0; promoxityDl=0;
//
//	//HCU_DEBUG_PRINT_FAT("L3BFDF: Combine Search START, Index=%d, SearchSpace=%d, Start Point = %d\n", i, gTaskL3bfdfContext.searchSpaceTotalNbr, WsSensorStart);
//	for (i=WsSensorStart; i< (gTaskL3bfdfContext.searchSpaceTotalNbr + WsSensorStart); i++){
//		//先计算单个矢量乘积结果
//		memset(resBitmap, 0, sizeof(resBitmap));
//		result = func_l3bfdf_caculate_vector_multipy_result((i % gTaskL3bfdfContext.searchSpaceTotalNbr), resBitmap);
//
//		//再计算矢量的有效非零数
//		searchNbr = func_l3bfdf_caculate_bitmap_valid_number(resBitmap, HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX);
//		//HCU_DEBUG_PRINT_INF("L3BFDF: WsSensorStart=%d, Index = %d, Result = %d, ComTarget/Max=[%d/%d], Nbr Min/Max = [%d/%d], SearchNbr=%d\n", WsSensorStart, i, result, gTaskL3bfdfContext.comAlgPar.TargetCombinationWeight, gTaskL3bfdfContext.comAlgPar.TargetCombinationWeight + gTaskL3bfdfContext.comAlgPar.TargetCombinationUpperWeight, gTaskL3bfdfContext.comAlgPar.MinScaleNumberCombination, gTaskL3bfdfContext.comAlgPar.MaxScaleNumberCombination, searchNbr);
//
//		//在目标组合数量之内，寻求大中最小的
//		if ((searchNbr >= gTaskL3bfdfContext.comAlgPar.MinScaleNumberCombination) &&  (searchNbr <= gTaskL3bfdfContext.comAlgPar.MaxScaleNumberCombination)){
//			//寻求更大的
//			if ((result > (gTaskL3bfdfContext.comAlgPar.TargetCombinationWeight + gTaskL3bfdfContext.comAlgPar.TargetCombinationUpperWeight)) && (result < result_Ul))
//			{
//				result_Ul = result;
//				promoxityUl = i;
//				memcpy(resBitmap_Ul, resBitmap, sizeof(resBitmap_Ul));
//			}
//			//寻求小中最大的
//			else if ((result < gTaskL3bfdfContext.comAlgPar.TargetCombinationWeight) && (result > result_Dl))
//			{
//				result_Dl = result;
//				promoxityDl = i;
//				memcpy(resBitmap_Dl, resBitmap, sizeof(resBitmap_Dl));
//			}
//		}
//
//		//再判定搜索结果是否满足条件
//		if (func_l3bfdf_caculate_judge_search_result(result, searchNbr, (i % gTaskL3bfdfContext.searchSpaceTotalNbr)) == TRUE)
//		{
//			func_l3bfdf_caculate_execute_search_result(i, resBitmap);
//			//HCU_DEBUG_PRINT_FAT("L3BFDF: Combine SUCCESS one time, Index=%d, SearchSpace=%d\n", i, gTaskL3bfdfContext.searchSpaceTotalNbr);
//			return i;
//		}
//	}//整个搜索空间循环结束
//	//HCU_DEBUG_PRINT_FAT("L3BFDF: Combine Search Accomplish one round, Index=%d, SearchSpace=%d, StartPoint = %d\n", i, gTaskL3bfdfContext.searchSpaceTotalNbr, WsSensorStart);
//
//	//没找到，但激活了上近似组合
//	if (gTaskL3bfdfContext.comAlgPar.IsProximitCombinationMode == HCU_L3BFDF_COMB_ALG_PAR_PROXIMITY_ABOVE_UP_LIMIT)
//	{
//		//HCU_DEBUG_PRINT_FAT("L3BFDF: Combine Proximity UP One time, Index=%d, SearchSpace=%d\n", i, gTaskL3bfdfContext.searchSpaceTotalNbr);
//		if ((result_Ul != 0) && (promoxityUl != -1)){
//			func_l3bfdf_caculate_execute_search_result(promoxityUl, resBitmap_Ul);
//			return promoxityUl;
//		}
//	}
//
//	//没找到，但激活了下近似组合
//	else if (gTaskL3bfdfContext.comAlgPar.IsProximitCombinationMode == HCU_L3BFDF_COMB_ALG_PAR_PROXIMITY_BELOW_DN_LIMIT)
//	{
//		//HCU_DEBUG_PRINT_FAT("L3BFDF: Combine Proximity DL One time, Index=%d, SearchSpace=%d\n", i, gTaskL3bfdfContext.searchSpaceTotalNbr);
//		if ((result_Dl != 0) && (promoxityDl != -1)){
//			func_l3bfdf_caculate_execute_search_result(promoxityDl, resBitmap_Dl);
//			return promoxityDl;
//		}
//	}
//
//	//没找到，收场
//	//HCU_DEBUG_PRINT_FAT("L3BFDF: Combine Search UNSUCCESS and Return, Index=%d, SearchSpace=%d\n", i, gTaskL3bfdfContext.searchSpaceTotalNbr);
//	gTaskL3bfdfContext.wsRrSearchStart = (i+1) % (gTaskL3bfdfContext.searchSpaceTotalNbr);
//	return -1;
//}
//
//
//UINT8 func_l3bfdf_caculate_bitmap_valid_number(UINT8 *bitmap, UINT8 len)
//{
//	int i = 0;
//	UINT8 temp = 0;
//
//	for (i=0; i<len; i++){
//		temp += (UINT8)*bitmap;
//		bitmap++;
//	}
//	return temp;
//}
//
//
////计算搜索一个矢量内的乘积之和
////result = 0;
////memset(resBitmap, 0, sizeof(resBitmap));
////for (j=0; j<HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX; j++){
////	if (gTaskL3bfdfContext.sensorWs[j].sensorStatus == HCU_L3BFDF_SENSOR_WS_STATUS_VALID_TO_COMB){
////		//HCU_DEBUG_PRINT_FAT("L3BFDF: Search Algo => Index=%d, SensorId = %d\n", i, j);
////		t = ((i%gTaskL3bfdfContext.searchSpaceTotalNbr) * HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX + j);// % Cycle;
////		result = result + gTaskL3bfdfContext.sensorWs[j].sensorValue * (*(gTaskL3bfdfContext.SearchCoefficientPointer + t));
////		if (*(gTaskL3bfdfContext.SearchCoefficientPointer + t) == 1) resBitmap[j] = 1;
////	}
////}
//
//UINT32 func_l3bfdf_caculate_vector_multipy_result(UINT32 WsSensorStart, UINT8 *resBitmap)
//{
//	UINT32 j=0, t=0, result=0;
//
//	result = 0;
//	memset(resBitmap, 0, HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX);
//	for (j=0; j<HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX; j++){
//		if (gTaskL3bfdfContext.sensorWs[j].sensorStatus == HCU_L3BFDF_SENSOR_WS_STATUS_VALID_TO_COMB){
//			//HCU_DEBUG_PRINT_FAT("L3BFDF: Search Algo => Index=%d, SensorId = %d\n", i, j);
//			t = ((WsSensorStart%gTaskL3bfdfContext.searchSpaceTotalNbr) * HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX + j);// % Cycle;
//			result = result + gTaskL3bfdfContext.sensorWs[j].sensorValue * (*(gTaskL3bfdfContext.SearchCoefficientPointer + t));
//			if (*(gTaskL3bfdfContext.SearchCoefficientPointer + t) == 1) resBitmap[j] = 1;
//		}
//	}
//
//	return result;
//}
//
////判定搜索到的结果是否成功
//bool func_l3bfdf_caculate_judge_search_result(UINT32 result, UINT8 searchNbr, UINT32 WsSensorStart)
//{
//	int t=0, j=0;
//
//	//优先级算法没有被激活
//	if (gTaskL3bfdfContext.comAlgPar.IsPriorityScaleEnabled == HCU_L3BFDF_COMB_ALG_PAR_PRIORITY_DISABLE)
//	{
//		if ((result >= gTaskL3bfdfContext.comAlgPar.TargetCombinationWeight) && \
//				(result <= (gTaskL3bfdfContext.comAlgPar.TargetCombinationWeight + gTaskL3bfdfContext.comAlgPar.TargetCombinationUpperWeight)) && \
//				(searchNbr >= gTaskL3bfdfContext.comAlgPar.MinScaleNumberCombination) && \
//				(searchNbr <= gTaskL3bfdfContext.comAlgPar.MaxScaleNumberCombination))
//			return TRUE;
//		else
//			return FALSE;
//	}
//
//	//优先级算法被激活
//	//具体的优先级配置，还未定义参数，未来待完善
//	else{
//		//HCU_DEBUG_PRINT_FAT("L3BFDF: Priority Algo enter!\n");
//		//先寻找优先级是否为否
//		for (j=0; j<HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX; j++)
//		{
//			t = ((WsSensorStart%gTaskL3bfdfContext.searchSpaceTotalNbr) * HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX + j);// % Cycle;
//			result = result + gTaskL3bfdfContext.sensorWs[j].sensorValue * (*(gTaskL3bfdfContext.SearchCoefficientPointer + t));
//			if ((gTaskL3bfdfContext.comAlgParPriority[j] == 1) && (*(gTaskL3bfdfContext.SearchCoefficientPointer + t) == 0))
//				return FALSE;
//		}
//		if ((result >= gTaskL3bfdfContext.comAlgPar.TargetCombinationWeight) && \
//				(result <= (gTaskL3bfdfContext.comAlgPar.TargetCombinationWeight + gTaskL3bfdfContext.comAlgPar.TargetCombinationUpperWeight)) && \
//				(searchNbr >= gTaskL3bfdfContext.comAlgPar.MinScaleNumberCombination) && \
//				(searchNbr <= gTaskL3bfdfContext.comAlgPar.MaxScaleNumberCombination))
//			return TRUE;
//		else
//			return FALSE;
//
//		return TRUE;
//	}
//}
//
//
////对于搜寻到的结果，执行记录工作
//void func_l3bfdf_caculate_execute_search_result(UINT32 i, UINT8* resBitmap)
//{
//	UINT32 j=0;
//
//	gTaskL3bfdfContext.wsRrSearchStart = ((i+1) % gTaskL3bfdfContext.searchSpaceTotalNbr);
//	memcpy(gTaskL3bfdfContext.wsBitmap, resBitmap, HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX);
//	gTaskL3bfdfContext.wsValueNbrTtt = 0;
//	for (j = 0; j<HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX; j++){
//		gTaskL3bfdfContext.wsValueNbrTtt += gTaskL3bfdfContext.wsBitmap[j];
//	}
//	gTaskL3bfdfContext.cur.wsCombTimes++;
//	//不值得再打印
//	//HCU_DEBUG_PRINT_INF("L3BFDF: ComTarget/Max=[%d/%d], Nbr Min/Max = [%d/%d], SearchNbr=%d\n", gTaskL3bfdfContext.comAlgPar.TargetCombinationWeight, gTaskL3bfdfContext.comAlgPar.TargetCombinationWeight + gTaskL3bfdfContext.comAlgPar.TargetCombinationUpperWeight, gTaskL3bfdfContext.comAlgPar.MinScaleNumberCombination, gTaskL3bfdfContext.comAlgPar.MaxScaleNumberCombination, searchNbr);
//	//HCU_DEBUG_PRINT_INF("L3BFDF: 1=%d/2=%d/3=%d/4=%d/5=%d/6=%d/7=%d/8=%d/9=%d/10=%d\n", gTaskL3bfdfContext.sensorWs[1].sensorValue, gTaskL3bfdfContext.sensorWs[2].sensorValue, \
//	//		gTaskL3bfdfContext.sensorWs[3].sensorValue, gTaskL3bfdfContext.sensorWs[4].sensorValue, gTaskL3bfdfContext.sensorWs[5].sensorValue, gTaskL3bfdfContext.sensorWs[6].sensorValue,\
//	//		gTaskL3bfdfContext.sensorWs[7].sensorValue, gTaskL3bfdfContext.sensorWs[8].sensorValue, gTaskL3bfdfContext.sensorWs[9].sensorValue, gTaskL3bfdfContext.sensorWs[10].sensorValue);
//}
//
//
//
//
//
//
//
//
////丢弃必然成功
////先使用随机方式
//void func_l3bfdf_ws_sensor_search_give_up(void)
//{
//	UINT32 i=0;
//	memset(gTaskL3bfdfContext.wsBitmap, 0, HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX);
//	for (i=0; i<HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX; i++){
//		if (((rand()%11) < 6) &&(gTaskL3bfdfContext.sensorWs[i].sensorStatus == HCU_L3BFDF_SENSOR_WS_STATUS_VALID_TO_COMB)){
//			gTaskL3bfdfContext.wsBitmap[i]=1;
//			gTaskL3bfdfContext.wsValueNbrTgu++;
//		}
//
//	}
//	return;
//}
//
//void func_l3bfdf_cacluate_sensor_ws_valid_value(void)
//{
//	int i=0, resFree = 0, resWeight=0, resTtt =0, resTgu = 0, resActive = 0;
//	for (i=0; i<HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX; i++)
//	{
//		if (gTaskL3bfdfContext.sensorWs[i].sensorStatus == HCU_L3BFDF_SENSOR_WS_STATUS_VALIID_EMPTY){
//			resFree++;
//			resActive++;
//		}
//		else if (gTaskL3bfdfContext.sensorWs[i].sensorStatus == HCU_L3BFDF_SENSOR_WS_STATUS_VALID_TO_COMB){
//			resWeight++;
//			resActive++;
//		}
//		else if (gTaskL3bfdfContext.sensorWs[i].sensorStatus == HCU_L3BFDF_SENSOR_WS_STATUS_VALID_TO_TTT){
//			resTtt++;
//			resActive++;
//		}
//		else if (gTaskL3bfdfContext.sensorWs[i].sensorStatus == HCU_L3BFDF_SENSOR_WS_STATUS_VALID_TO_TGU){
//			resTgu++;
//			resActive++;
//		}
//	}
//
//	//存入全局控制表
//	gTaskL3bfdfContext.wsValueNbrFree = resFree;
//	gTaskL3bfdfContext.wsValueNbrWeight = resWeight;
//	gTaskL3bfdfContext.wsValueNbrTtt = resTtt;
//	gTaskL3bfdfContext.wsValueNbrTgu = resTgu;
//	gTaskL3bfdfContext.wsValueNbrActive = resActive;
//	//返回
//	return;
//}
//
////计算表中有多少个有效的bitmap传感器数量
//UINT32 func_l3bfdf_cacluate_sensor_ws_bitmap_valid_number(void)
//{
//	int i=0, total = 0;
//	for (i=0; i<HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX; i++)
//	{
//		total += gTaskL3bfdfContext.wsBitmap[i];
//	}
//
//	//返回
//	return total;
//}
//
////计算表中有多少个有效的bitmap传感器下的重量值
//float func_l3bfdf_cacluate_sensor_ws_bitmap_valid_weight(void)
//{
//	int i=0;
//	float weight = 0;
//	for (i=0; i<HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX; i++)
//	{
//		if (gTaskL3bfdfContext.wsBitmap[i] == 1)
//		weight += gTaskL3bfdfContext.sensorWs[i].sensorValue;
//	}
//
//	//返回
//	return weight;
//}
//
////计算有多少个传感器已经STARTUP了
//UINT8 func_l3bfdf_count_numbers_of_startup_ws_sensors(void)
//{
//	int i=0, count = 0;
//	for (i=0; i<HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX; i++)
//	{
//		if (gTaskL3bfdfContext.sensorWs[i].sensorStatus >= HCU_L3BFDF_SENSOR_WS_STATUS_STARTUP)
//			count++;
//	}
//
//	//返回
//	return count;
//}
//
////判定是否所有传感器都进入REPEAT状态
////TRUE:　所有传感器都进入REPEAT状态
//BOOL func_l3bfdf_judge_whether_all_valid_sensor_enter_repeat_status(void)
//{
//	int i=0;
//	for (i=0; i<HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX; i++)
//	{
//		if (gTaskL3bfdfContext.sensorWs[i].sensorStatus == HCU_L3BFDF_SENSOR_WS_STATUS_VALIID_EMPTY){
//			gTaskL3bfdfContext.sensorWs[i].sensorRepTimes = 0;
//			return FAILURE;
//		}
//	}
//
//	//返回
//	return TRUE;
//}
//
//
///***************************************************************************************************************************
// *
// * 　超时处理过程
// *
// ***************************************************************************************************************************/
////判定是否所有的CFG_RESP收齐了
//bool func_l3bfdf_cacluate_sensor_cfg_rcv_complete(void)
//{
//	int i=0;
//	for (i=0; i<HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX; i++)
//	{
//		if ((gTaskL3bfdfContext.sensorWs[i].sensorStatus == HCU_L3BFDF_SENSOR_WS_STATUS_CFG_REQ) && (gTaskL3bfdfContext.sensorWs[i].cfgRcvFlag == FALSE))
//			return FALSE;
//	}
//
//	//返回
//	return TRUE;
//}
//
////判定是否所有的START_RESP收齐了
//bool func_l3bfdf_cacluate_sensor_start_rcv_complete(void)
//{
//	int i=0;
//	for (i=0; i<HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX; i++)
//	{
//		if ((gTaskL3bfdfContext.sensorWs[i].sensorStatus == HCU_L3BFDF_SENSOR_WS_STATUS_START_REQ) && (gTaskL3bfdfContext.sensorWs[i].startRcvFlag == FALSE))
//			return FALSE;
//	}
//
//	//返回
//	return TRUE;
//}
//
////判定是否所有的STOP_RESP收齐了
//bool func_l3bfdf_cacluate_sensor_stop_rcv_complete(void)
//{
//	int i=0;
//	for (i=0; i<HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX; i++)
//	{
//		if ((gTaskL3bfdfContext.sensorWs[i].sensorStatus == HCU_L3BFDF_SENSOR_WS_STATUS_STOP_REQ) && (gTaskL3bfdfContext.sensorWs[i].stopRcvFlag == FALSE))
//			return FALSE;
//	}
//
//	//返回
//	return TRUE;
//}
//
//
////TIME_OUT处理过程
//OPSTAT func_l3bfdf_time_out_sys_cfg_req_process(void)
//{
//	int ret = 0, i=0;
//	int total = 0;
//
//	//先判定成功的数量是否达到最小数，如果是，就认为是成功了
//	for (i=0; i<HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX; i++){
//		if (gTaskL3bfdfContext.sensorWs[i].sensorStatus == HCU_L3BFDF_SENSOR_WS_STATUS_CFG_CMPL) total++;
//	}
//	if (total < gTaskL3bfdfContext.comAlgPar.MinScaleNumberCombination){
//		//发送反馈给UICOMM
//		msg_struct_l3bfdf_uicomm_cfg_resp_t snd;
//		memset(&snd, 0, sizeof(msg_struct_l3bfdf_uicomm_cfg_resp_t));
//		snd.validFlag = FALSE;
//		snd.sensorid = HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX;
//		snd.errCode = HCU_SYSMSG_BFDF_ERR_CODE_TIME_OUT;
//		snd.length = sizeof(msg_struct_l3bfdf_uicomm_cfg_resp_t);
//		ret = hcu_message_send(MSG_ID_L3BFDF_UICOMM_CFG_RESP, TASK_ID_BFDFUICOMM, TASK_ID_L3BFDF, &snd, snd.length);
//		if (ret == FAILURE){
//			HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3BFDF].taskName, zHcuVmCtrTab.task[TASK_ID_BFDFUICOMM].taskName);
//		}
//		//设置状态机
//		if (FsmSetState(TASK_ID_L3BFDF, FSM_STATE_L3BFDF_ACTIVED) == FAILURE){
//			HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Error Set FSM State!\n");
//		}
//	}
//
//	//如果超过最小数量，则可以继续执行
//	else{
//		//发送反馈给UICOMM
//		msg_struct_l3bfdf_uicomm_cfg_resp_t snd;
//		memset(&snd, 0, sizeof(msg_struct_l3bfdf_uicomm_cfg_resp_t));
//		snd.validFlag = TRUE;
//		snd.length = sizeof(msg_struct_l3bfdf_uicomm_cfg_resp_t);
//		ret = hcu_message_send(MSG_ID_L3BFDF_UICOMM_CFG_RESP, TASK_ID_BFDFUICOMM, TASK_ID_L3BFDF, &snd, snd.length);
//		if (ret == FAILURE){
//			HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3BFDF].taskName, zHcuVmCtrTab.task[TASK_ID_BFDFUICOMM].taskName);
//		}
//		//设置状态机
//		if (FsmSetState(TASK_ID_L3BFDF, FSM_STATE_L3BFDF_OPR_GO) == FAILURE){
//			HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Error Set FSM State!\n");
//		}
//	}
//
//	//返回
//	return SUCCESS;
//}
//
//OPSTAT func_l3bfdf_time_out_sys_start_req_process(void)
//{
//	int ret = 0, i=0;
//	int total = 0;
//
//	//先判定成功的数量是否达到最小数，如果是，就认为是成功了
//	for (i=0; i<HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX; i++){
//		if (gTaskL3bfdfContext.sensorWs[i].sensorStatus == HCU_L3BFDF_SENSOR_WS_STATUS_VALIID_EMPTY) total++;
//	}
//	if (total < gTaskL3bfdfContext.comAlgPar.MinScaleNumberCombination){
//		//发送反馈给UICOMM
//		msg_struct_l3bfdf_uicomm_cmd_resp_t snd;
//		memset(&snd, 0, sizeof(msg_struct_l3bfdf_uicomm_cmd_resp_t));
//		snd.validFlag = FALSE;
//		snd.cmdid = HCU_SYSMSG_BFDF_UICOMM_CMDID_START;
//		snd.sensorid = HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX;
//		snd.errCode = HCU_SYSMSG_BFDF_ERR_CODE_TIME_OUT;
//		snd.length = sizeof(msg_struct_l3bfdf_uicomm_cmd_resp_t);
//		ret = hcu_message_send(MSG_ID_L3BFDF_UICOMM_CMD_RESP, TASK_ID_BFDFUICOMM, TASK_ID_L3BFDF, &snd, snd.length);
//		if (ret == FAILURE){
//			HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3BFDF].taskName, zHcuVmCtrTab.task[TASK_ID_BFDFUICOMM].taskName);
//		}
//		//设置状态机
//		if (FsmSetState(TASK_ID_L3BFDF, FSM_STATE_L3BFDF_OPR_GO) == FAILURE){
//			HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Error Set FSM State!\n");
//		}
//	}
//
//	//如果超过最小数量，则可以继续执行
//	else{
//		//发送反馈给UICOMM
//		msg_struct_l3bfdf_uicomm_cmd_resp_t snd;
//		memset(&snd, 0, sizeof(msg_struct_l3bfdf_uicomm_cmd_resp_t));
//		snd.cmdid = HCU_SYSMSG_BFDF_UICOMM_CMDID_START;
//		snd.validFlag = TRUE;
//		snd.length = sizeof(msg_struct_l3bfdf_uicomm_cmd_resp_t);
//		ret = hcu_message_send(MSG_ID_L3BFDF_UICOMM_CMD_RESP, TASK_ID_BFDFUICOMM, TASK_ID_L3BFDF, &snd, snd.length);
//		if (ret == FAILURE){
//			HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3BFDF].taskName, zHcuVmCtrTab.task[TASK_ID_BFDFUICOMM].taskName);
//		}
//
//		//启动统计扫描定时器
//		ret = hcu_timer_start(TASK_ID_L3BFDF, TIMER_ID_10MS_L3BFDF_PERIOD_STA_SCAN, \
//			zHcuSysEngPar.timer.array[TIMER_ID_10MS_L3BFDF_PERIOD_STA_SCAN].dur, TIMER_TYPE_PERIOD, TIMER_RESOLUTION_10MS);
//		if (ret == FAILURE)
//			HCU_ERROR_PRINT_L3BFDF("L3BFDF: Error start period timer!\n");
//
//		//设置工作启动时间
//		gTaskL3bfdfContext.startWorkTimeInUnix = time(0);
//
//		//设置状态机
//		if (FsmSetState(TASK_ID_L3BFDF, FSM_STATE_L3BFDF_OOS_SCAN) == FAILURE){
//			HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Error Set FSM State!\n");
//		}
//	}
//	//返回
//	return SUCCESS;
//}
//
//OPSTAT func_l3bfdf_time_out_sys_stop_req_process(void)
//{
//	int ret = 0;
//
//	//发送反馈给UICOMM
//	msg_struct_l3bfdf_uicomm_cmd_resp_t snd;
//	memset(&snd, 0, sizeof(msg_struct_l3bfdf_uicomm_cmd_resp_t));
//	snd.validFlag = FALSE;
//	snd.cmdid = HCU_SYSMSG_BFDF_UICOMM_CMDID_STOP;
//	snd.sensorid = HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX;
//	snd.errCode = HCU_SYSMSG_BFDF_ERR_CODE_TIME_OUT;
//	snd.length = sizeof(msg_struct_l3bfdf_uicomm_cmd_resp_t);
//	ret = hcu_message_send(MSG_ID_L3BFDF_UICOMM_CMD_RESP, TASK_ID_BFDFUICOMM, TASK_ID_L3BFDF, &snd, snd.length);
//	if (ret == FAILURE){
//		HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3BFDF].taskName, zHcuVmCtrTab.task[TASK_ID_BFDFUICOMM].taskName);
//	}
//	//设置状态机
//	if (FsmSetState(TASK_ID_L3BFDF, FSM_STATE_L3BFDF_OPR_GO) == FAILURE){
//		HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Error Set FSM State!\n");
//	}
//
//	//返回
//	return SUCCESS;
//}
//
////出料超时的情况：隔离秤盘，并等待下一次下位机自动恢复
//OPSTAT func_l3bfdf_time_out_ttt_wait_fb_process(void)
//{
//	int ret = 0, i=0;
//
//	//发送STOP_REQ给所有下位机
//	msg_struct_l3bfdf_can_sys_stop_req_t snd1;
//	memset(&snd1, 0, sizeof(msg_struct_l3bfdf_can_sys_stop_req_t));
//	snd1.length = sizeof(msg_struct_l3bfdf_can_sys_stop_req_t);
//	//所有传感器
//	for (i = 0; i< HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX; i++){
//		snd1.wsBitmap[i] = TRUE;
//	}
//	ret = hcu_message_send(MSG_ID_L3BFDF_CAN_SYS_STOP_REQ, TASK_ID_CANITFLEO, TASK_ID_L3BFDF, &snd1, snd1.length);
//	if (ret == FAILURE) HCU_ERROR_PRINT_L3BFDF("L3BFDF: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3BFDF].taskName, zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName);
//
//	//设置状态机
//	if (FsmSetState(TASK_ID_L3BFDF, FSM_STATE_L3BFDF_OPR_GO) == FAILURE){
//		HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Error Set FSM State!\n");
//	}
//
//	//延时发送SUSPEND给界面，以防止界面SUSPEND被STOP_RESP覆盖
//	hcu_sleep(2);
//	msg_struct_l3bfdf_uicomm_cmd_resp_t snd;
//	memset(&snd, 0, sizeof(msg_struct_l3bfdf_uicomm_cmd_resp_t));
//	snd.cmdid = HCU_SYSMSG_BFDF_UICOMM_CMDID_SUSPEND;
//	snd.validFlag = TRUE;
//	snd.length = sizeof(msg_struct_l3bfdf_uicomm_cmd_resp_t);
//	ret = hcu_message_send(MSG_ID_L3BFDF_UICOMM_CMD_RESP, TASK_ID_BFDFUICOMM, TASK_ID_L3BFDF, &snd, snd.length);
//	if (ret == FAILURE){
//		HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3BFDF].taskName, zHcuVmCtrTab.task[TASK_ID_BFDFUICOMM].taskName);
//	}
//
//	//返回
//	return SUCCESS;
//}
//
//OPSTAT func_l3bfdf_time_out_tgu_wait_fb_process(void)
//{
//	int ret = 0, i=0;
//
//	//发送STOP_REQ给所有下位机
//	msg_struct_l3bfdf_can_sys_stop_req_t snd1;
//	memset(&snd1, 0, sizeof(msg_struct_l3bfdf_can_sys_stop_req_t));
//	snd1.length = sizeof(msg_struct_l3bfdf_can_sys_stop_req_t);
//	//所有传感器
//	for (i = 0; i< HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX; i++){
//		snd1.wsBitmap[i] = TRUE;
//	}
//	ret = hcu_message_send(MSG_ID_L3BFDF_CAN_SYS_STOP_REQ, TASK_ID_CANITFLEO, TASK_ID_L3BFDF, &snd1, snd1.length);
//	if (ret == FAILURE) HCU_ERROR_PRINT_L3BFDF("L3BFDF: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3BFDF].taskName, zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName);
//
//	//设置状态机
//	if (FsmSetState(TASK_ID_L3BFDF, FSM_STATE_L3BFDF_OPR_GO) == FAILURE){
//		HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Error Set FSM State!\n");
//	}
//
//	//延时发送SUSPEND给界面，以防止界面SUSPEND被STOP_RESP覆盖
//	hcu_sleep(2);
//	msg_struct_l3bfdf_uicomm_cmd_resp_t snd;
//	memset(&snd, 0, sizeof(msg_struct_l3bfdf_uicomm_cmd_resp_t));
//	snd.cmdid = HCU_SYSMSG_BFDF_UICOMM_CMDID_SUSPEND;
//	snd.validFlag = TRUE;
//	snd.length = sizeof(msg_struct_l3bfdf_uicomm_cmd_resp_t);
//	ret = hcu_message_send(MSG_ID_L3BFDF_UICOMM_CMD_RESP, TASK_ID_BFDFUICOMM, TASK_ID_L3BFDF, &snd, snd.length);
//	if (ret == FAILURE){
//		HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3BFDF].taskName, zHcuVmCtrTab.task[TASK_ID_BFDFUICOMM].taskName);
//	}
//
//	//返回
//	return SUCCESS;
//}
//
////差错情形下的处理过程：这个过程是永恒的，一直到系统重启
////暂时不考虑Ｎ次以后放弃的情形：我们将它当做是系统启动的是否还正常，为啥工作到一半就不正常了呢
//OPSTAT func_l3bfdf_time_out_error_inq_process(void)
//{
//	int ret = 0, i=0;
//
//	//发送扫描需求
//	msg_struct_l3bfdf_can_error_inq_cmd_req_t snd;
//	memset(&snd, 0, sizeof(msg_struct_l3bfdf_can_error_inq_cmd_req_t));
//	snd.length = sizeof(msg_struct_l3bfdf_can_error_inq_cmd_req_t);
//	snd.sensorid = HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX;
//	for (i=0; i<HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX; i++){
//		if (gTaskL3bfdfContext.sensorWs[i].sensorStatus == HCU_L3BFDF_SENSOR_WS_STATUS_VALID_ERROR) snd.sensorBitmap[i] = 1;
//	}
//
//	ret = hcu_message_send(MSG_ID_L3BFDF_CAN_ERROR_INQ_CMD_REQ, TASK_ID_CANITFLEO, TASK_ID_L3BFDF, &snd, snd.length);
//	if (ret == FAILURE){
//		HCU_ERROR_PRINT_L3BFDF_RECOVERY("L3BFDF: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3BFDF].taskName, zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName);
//	}
//
//	//返回
//	return SUCCESS;
//}
//
//
///***************************************************************************************************************************
// *
// * 　周期性扫描过程
// *
// ***************************************************************************************************************************/
////周期性统计扫描定时器
//OPSTAT func_l3bfdf_time_out_statistic_scan_process(void)
//{
//	int ret = 0;
//
//	//首先增加时间流逝的计数器
//	gTaskL3bfdfContext.elipseCnt++;
//	gTaskL3bfdfContext.elipse24HourCnt++;
//
//	//采取老化算法 x(n+1) = x(n) * (1-1/120) + latest，从而得到最新的数据，但该数据最好使用float，然后再转换为UINT32存入到数据库表单中
//	memset(&(gTaskL3bfdfContext.staLocalUi), 0, sizeof(HcuSysMsgIeL3bfdfContextStaElement_t));
//	//先存储临时数据
//	gTaskL3bfdfContext.curAge.wsIncMatCntMid = gTaskL3bfdfContext.curAge.wsIncMatCntMid*(float)HCU_L3BFDF_STA_AGEING_COEF + (float)HCU_L3BFDF_STA_AGEING_COEF_ALPHA*gTaskL3bfdfContext.cur.wsIncMatCnt;
//	gTaskL3bfdfContext.curAge.wsIncMatWgtMid = gTaskL3bfdfContext.curAge.wsIncMatWgtMid*(float)HCU_L3BFDF_STA_AGEING_COEF + (float)HCU_L3BFDF_STA_AGEING_COEF_ALPHA*gTaskL3bfdfContext.cur.wsIncMatWgt;
//	gTaskL3bfdfContext.curAge.wsCombTimesMid = gTaskL3bfdfContext.curAge.wsCombTimesMid*(float)HCU_L3BFDF_STA_AGEING_COEF + (float)HCU_L3BFDF_STA_AGEING_COEF_ALPHA*gTaskL3bfdfContext.cur.wsCombTimes;
//	gTaskL3bfdfContext.curAge.wsTttTimesMid = gTaskL3bfdfContext.curAge.wsTttTimesMid*(float)HCU_L3BFDF_STA_AGEING_COEF + (float)HCU_L3BFDF_STA_AGEING_COEF_ALPHA*gTaskL3bfdfContext.cur.wsTttTimes;
//	gTaskL3bfdfContext.curAge.wsTgvTimesMid = gTaskL3bfdfContext.curAge.wsTgvTimesMid*(float)HCU_L3BFDF_STA_AGEING_COEF + (float)HCU_L3BFDF_STA_AGEING_COEF_ALPHA*gTaskL3bfdfContext.cur.wsTgvTimes;
//	gTaskL3bfdfContext.curAge.wsTttMatCntMid = gTaskL3bfdfContext.curAge.wsTttMatCntMid*(float)HCU_L3BFDF_STA_AGEING_COEF + (float)HCU_L3BFDF_STA_AGEING_COEF_ALPHA*gTaskL3bfdfContext.cur.wsTttMatCnt;
//	gTaskL3bfdfContext.curAge.wsTgvMatCntMid = gTaskL3bfdfContext.curAge.wsTgvMatCntMid*(float)HCU_L3BFDF_STA_AGEING_COEF + (float)HCU_L3BFDF_STA_AGEING_COEF_ALPHA*gTaskL3bfdfContext.cur.wsTgvMatCnt;
//	gTaskL3bfdfContext.curAge.wsTttMatWgtMid = gTaskL3bfdfContext.curAge.wsTttMatWgtMid*(float)HCU_L3BFDF_STA_AGEING_COEF + (float)HCU_L3BFDF_STA_AGEING_COEF_ALPHA*gTaskL3bfdfContext.cur.wsTttMatWgt;
//	gTaskL3bfdfContext.curAge.wsTgvMatWgtMid = gTaskL3bfdfContext.curAge.wsTgvMatWgtMid*(float)HCU_L3BFDF_STA_AGEING_COEF + (float)HCU_L3BFDF_STA_AGEING_COEF_ALPHA*gTaskL3bfdfContext.cur.wsTgvMatWgt;
//	//再输出结果
//	gTaskL3bfdfContext.staLocalUi.wsIncMatCnt = (UINT32)(gTaskL3bfdfContext.curAge.wsIncMatCntMid + 0.5);
//	gTaskL3bfdfContext.staLocalUi.wsIncMatWgt = (UINT32)(gTaskL3bfdfContext.curAge.wsIncMatWgtMid + 0.5);
//	gTaskL3bfdfContext.staLocalUi.wsCombTimes = (UINT32)(gTaskL3bfdfContext.curAge.wsCombTimesMid + 0.5);
//	gTaskL3bfdfContext.staLocalUi.wsTttTimes  = (UINT32)(gTaskL3bfdfContext.curAge.wsTttTimesMid + 0.5);
//	gTaskL3bfdfContext.staLocalUi.wsTgvTimes  = (UINT32)(gTaskL3bfdfContext.curAge.wsTgvTimesMid + 0.5);
//	gTaskL3bfdfContext.staLocalUi.wsTttMatCnt = (UINT32)(gTaskL3bfdfContext.curAge.wsTttMatCntMid + 0.5);
//	gTaskL3bfdfContext.staLocalUi.wsTgvMatCnt = (UINT32)(gTaskL3bfdfContext.curAge.wsTgvMatCntMid + 0.5);
//	gTaskL3bfdfContext.staLocalUi.wsTttMatWgt = (UINT32)(gTaskL3bfdfContext.curAge.wsTttMatWgtMid + 0.5);
//	gTaskL3bfdfContext.staLocalUi.wsTgvMatWgt = (UINT32)(gTaskL3bfdfContext.curAge.wsTgvMatWgtMid + 0.5);
//	gTaskL3bfdfContext.staLocalUi.wsAvgTttTimes = gTaskL3bfdfContext.staLocalUi.wsTttTimes;
//	gTaskL3bfdfContext.staLocalUi.wsAvgTttMatCnt = gTaskL3bfdfContext.staLocalUi.wsTttMatCnt;
//	gTaskL3bfdfContext.staLocalUi.wsAvgTttMatWgt = gTaskL3bfdfContext.staLocalUi.wsTttMatWgt;
//
//	//更新LocUI数据库，以便本地界面实时展示数据
//	if (dbi_HcuBfdf_StaDatainfo_save(HCU_L3BFDF_STA_DBI_TABLE_LOCALUI, gTaskL3bfdfContext.configId, &(gTaskL3bfdfContext.staLocalUi)) == FAILURE)
//			HCU_ERROR_PRINT_L3BFDF("L3BFDF: Save data to DB error!\n");
//
//	//更新60Min各个统计表单
//	gTaskL3bfdfContext.sta60Min.wsIncMatCnt += gTaskL3bfdfContext.cur.wsIncMatCnt;
//	gTaskL3bfdfContext.sta60Min.wsIncMatWgt += gTaskL3bfdfContext.cur.wsIncMatWgt;
//	gTaskL3bfdfContext.sta60Min.wsCombTimes += gTaskL3bfdfContext.cur.wsCombTimes;
//	gTaskL3bfdfContext.sta60Min.wsTttTimes  += gTaskL3bfdfContext.cur.wsTttTimes;
//	gTaskL3bfdfContext.sta60Min.wsTgvTimes  += gTaskL3bfdfContext.cur.wsTgvTimes;
//	gTaskL3bfdfContext.sta60Min.wsTttMatCnt += gTaskL3bfdfContext.cur.wsTttMatCnt;
//	gTaskL3bfdfContext.sta60Min.wsTgvMatCnt += gTaskL3bfdfContext.cur.wsTgvMatCnt;
//	gTaskL3bfdfContext.sta60Min.wsTttMatWgt += gTaskL3bfdfContext.cur.wsTttMatWgt;
//	gTaskL3bfdfContext.sta60Min.wsTgvMatWgt += gTaskL3bfdfContext.cur.wsTgvMatWgt;
//	float timeRun60MinRatio = (float) HCU_L3BFDF_STA_60M_CYCLE / (float)(((gTaskL3bfdfContext.elipseCnt%HCU_L3BFDF_STA_60M_CYCLE)==0)?HCU_L3BFDF_STA_60M_CYCLE:(gTaskL3bfdfContext.elipseCnt%HCU_L3BFDF_STA_60M_CYCLE));
//	gTaskL3bfdfContext.sta60Min.wsAvgTttTimes = (UINT32)(gTaskL3bfdfContext.sta60Min.wsTttTimes*timeRun60MinRatio);
//	gTaskL3bfdfContext.sta60Min.wsAvgTttMatCnt = (UINT32)(gTaskL3bfdfContext.sta60Min.wsTttMatCnt*timeRun60MinRatio);
//	gTaskL3bfdfContext.sta60Min.wsAvgTttMatWgt = (UINT32)(gTaskL3bfdfContext.sta60Min.wsTttMatWgt*timeRun60MinRatio);
//
//	//60分钟到了，发送统计报告到后台：发送后台只需要一种统计报告即可，重复发送意义不大
//	if ((gTaskL3bfdfContext.elipseCnt % HCU_L3BFDF_STA_60M_CYCLE) == 0){
//		//60分统计表更新数据库
//		if (dbi_HcuBfdf_StaDatainfo_save(HCU_L3BFDF_STA_DBI_TABLE_60MIN, gTaskL3bfdfContext.configId, &(gTaskL3bfdfContext.sta60Min)) == FAILURE)
//				HCU_ERROR_PRINT_L3BFDF("L3BFDF: Save data to DB error!\n");
//
//		//再发送统计报告
//		msg_struct_l3bfdf_cloudvela_statistic_report_t snd;
//		memset(&snd, 0, sizeof(msg_struct_l3bfdf_cloudvela_statistic_report_t));
//
//		//L2信息
//		strncpy(snd.comHead.destUser, zHcuSysEngPar.cloud.svrNameDefault, strlen(zHcuSysEngPar.cloud.svrNameDefault)<\
//			sizeof(snd.comHead.destUser)?strlen(zHcuSysEngPar.cloud.svrNameDefault):sizeof(snd.comHead.destUser));
//		strncpy(snd.comHead.srcUser, zHcuSysEngPar.hwBurnId.equLable, strlen(zHcuSysEngPar.hwBurnId.equLable)<\
//				sizeof(snd.comHead.srcUser)?strlen(zHcuSysEngPar.hwBurnId.equLable):sizeof(snd.comHead.srcUser));
//		snd.comHead.timeStamp = time(0);
//		snd.comHead.msgType = HUITP_MSG_HUIXML_MSGTYPE_DEVICE_REPORT_ID;
//		strcpy(snd.comHead.funcFlag, "0");
//
//		//CONTENT
//		snd.dataFormat = HUITP_IEID_UNI_COM_FORMAT_TYPE_FLOAT_WITH_NF2;
//		snd.baseReport = HUITP_IEID_UNI_COM_REPORT_YES;
//		snd.staRepType = HUITP_IEID_UNI_SCALE_WEIGHT_STATISTIC_REPORT_60M;
//		snd.sta.combTimes = gTaskL3bfdfContext.sta60Min.wsCombTimes;
//		snd.sta.tttTimes = gTaskL3bfdfContext.sta60Min.wsTttTimes;
//		snd.sta.tgvTimes = gTaskL3bfdfContext.sta60Min.wsTgvTimes;
//		snd.sta.combMatNbr = gTaskL3bfdfContext.sta60Min.wsIncMatCnt;
//		snd.sta.tttMatNbr = gTaskL3bfdfContext.sta60Min.wsTttMatCnt;
//		snd.sta.tgvMatNbr = gTaskL3bfdfContext.sta60Min.wsTgvMatCnt;
//		snd.sta.combAvgSpeed = gTaskL3bfdfContext.sta60Min.wsAvgTttTimes;
//		snd.sta.totalWeight = (UINT32)(gTaskL3bfdfContext.sta60Min.wsIncMatWgt*100);
//		snd.sta.totalMatNbr = gTaskL3bfdfContext.sta60Min.wsIncMatCnt;
//		snd.sta.totalWorkMin = (time(0) - gTaskL3bfdfContext.startWorkTimeInUnix)/60;
//		snd.sta.timeInUnix = time(0);
//		snd.sta.errNbr = 0;
//
//		snd.length = sizeof(msg_struct_l3bfdf_cloudvela_statistic_report_t);
//		ret = hcu_message_send(MSG_ID_L3BFDF_CLOUDVELA_STATISTIC_REPORT, TASK_ID_CLOUDVELA, TASK_ID_L3BFDF, &snd, snd.length);
//		if (ret == FAILURE){
//			HCU_ERROR_PRINT_L3BFDF("L3BFDF: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3BFDF].taskName, zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName);
//		}
//
//		//然后将60分钟统计数据表单清零，以便再次计数
//		memset(&(gTaskL3bfdfContext.sta60Min), 0, sizeof(HcuSysMsgIeL3bfdfContextStaElement_t));
//	}
//
//	//更新24小时统计表单
//	gTaskL3bfdfContext.sta24H.wsIncMatCnt += gTaskL3bfdfContext.cur.wsIncMatCnt;
//	gTaskL3bfdfContext.sta24H.wsIncMatWgt += gTaskL3bfdfContext.cur.wsIncMatWgt;
//	gTaskL3bfdfContext.sta24H.wsCombTimes += gTaskL3bfdfContext.cur.wsCombTimes;
//	gTaskL3bfdfContext.sta24H.wsTttTimes  += gTaskL3bfdfContext.cur.wsTttTimes;
//	gTaskL3bfdfContext.sta24H.wsTgvTimes  += gTaskL3bfdfContext.cur.wsTgvTimes;
//	gTaskL3bfdfContext.sta24H.wsTttMatCnt += gTaskL3bfdfContext.cur.wsTttMatCnt;
//	gTaskL3bfdfContext.sta24H.wsTgvMatCnt += gTaskL3bfdfContext.cur.wsTgvMatCnt;
//	gTaskL3bfdfContext.sta24H.wsTttMatWgt += gTaskL3bfdfContext.cur.wsTttMatWgt;
//	gTaskL3bfdfContext.sta24H.wsTgvMatWgt += gTaskL3bfdfContext.cur.wsTgvMatWgt;
//	float timeRun24HourRatio = (float) HCU_L3BFDF_STA_24H_CYCLE / (float)(((gTaskL3bfdfContext.elipse24HourCnt%HCU_L3BFDF_STA_24H_CYCLE)==0)?HCU_L3BFDF_STA_24H_CYCLE:(gTaskL3bfdfContext.elipseCnt%HCU_L3BFDF_STA_24H_CYCLE));
//	gTaskL3bfdfContext.sta24H.wsAvgTttTimes = (UINT32)(gTaskL3bfdfContext.sta24H.wsTttTimes*timeRun24HourRatio);
//	gTaskL3bfdfContext.sta24H.wsAvgTttMatCnt = (UINT32)(gTaskL3bfdfContext.sta24H.wsTttMatCnt*timeRun24HourRatio);
//	gTaskL3bfdfContext.sta24H.wsAvgTttMatWgt = (UINT32)(gTaskL3bfdfContext.sta24H.wsTttMatWgt*timeRun24HourRatio);
//
//	//24小时统计表更新数据库
//	if ((gTaskL3bfdfContext.elipseCnt % HCU_L3BFDF_STA_1M_CYCLE) == 0){
//		if (dbi_HcuBfdf_StaDatainfo_save(HCU_L3BFDF_STA_DBI_TABLE_24HOUR, gTaskL3bfdfContext.configId, &(gTaskL3bfdfContext.sta24H)) == FAILURE)
//				HCU_ERROR_PRINT_L3BFDF("L3BFDF: Save data to DB error!\n");
//	}
//
//	//24小时到了，然后将24小时统计数据表单清零，以便再次计数
//	if (((time(0)-gTaskL3bfdfContext.start24hStaTimeInUnix) % HCU_L3BFDF_STA_24H_IN_SECOND) == 0){
//		memset(&(gTaskL3bfdfContext.sta24H), 0, sizeof(HcuSysMsgIeL3bfdfContextStaElement_t));
//		gTaskL3bfdfContext.elipse24HourCnt = 0;
//		printf("this is a test, to show enter into wrong loop!\n");
//	}
//
//	//24小时到了，应该发送单独的统计报告，未来完善
//
//	//增加数据到连续统计数据中
//	gTaskL3bfdfContext.staUp2Now.wsIncMatCnt += gTaskL3bfdfContext.cur.wsIncMatCnt;
//	gTaskL3bfdfContext.staUp2Now.wsIncMatWgt += gTaskL3bfdfContext.cur.wsIncMatWgt;
//	gTaskL3bfdfContext.staUp2Now.wsCombTimes += gTaskL3bfdfContext.cur.wsCombTimes;
//	gTaskL3bfdfContext.staUp2Now.wsTttTimes  += gTaskL3bfdfContext.cur.wsTttTimes;
//	gTaskL3bfdfContext.staUp2Now.wsTgvTimes  += gTaskL3bfdfContext.cur.wsTgvTimes;
//	gTaskL3bfdfContext.staUp2Now.wsTttMatCnt += gTaskL3bfdfContext.cur.wsTttMatCnt;
//	gTaskL3bfdfContext.staUp2Now.wsTgvMatCnt += gTaskL3bfdfContext.cur.wsTgvMatCnt;
//	gTaskL3bfdfContext.staUp2Now.wsTttMatWgt += gTaskL3bfdfContext.cur.wsTttMatWgt;
//	gTaskL3bfdfContext.staUp2Now.wsTgvMatWgt += gTaskL3bfdfContext.cur.wsTgvMatWgt;
//	gTaskL3bfdfContext.staUp2Now.wsAvgTttTimes = gTaskL3bfdfContext.staUp2Now.wsTttTimes;
//	gTaskL3bfdfContext.staUp2Now.wsAvgTttMatCnt = gTaskL3bfdfContext.staUp2Now.wsTttMatCnt;
//	gTaskL3bfdfContext.staUp2Now.wsAvgTttMatWgt = gTaskL3bfdfContext.staUp2Now.wsTttMatWgt;
//
//	//更新连续数据库数据库：每5秒存储一次，不然数据库操作过于频繁
//	if ((gTaskL3bfdfContext.elipseCnt % HCU_L3BFDF_STA_5S_CYCLE) == 0){
//		if (dbi_HcuBfdf_StaDatainfo_save(HCU_L3BFDF_STA_DBI_TABLE_UP2NOW, gTaskL3bfdfContext.configId, &(gTaskL3bfdfContext.staUp2Now)) == FAILURE)
//				HCU_ERROR_PRINT_L3BFDF("L3BFDF: Save data to DB error!\n");
//	}
//
//	//重要的统计功能挂载
//	if ((gTaskL3bfdfContext.elipseCnt%HCU_L3BFDF_STATISTIC_PRINT_FREQUENCY) == 0)
//	HCU_DEBUG_PRINT_CRT("L3BFDF: Control statistics, Running Free/Weight/Ttt/Tgu = [%d, %d, %d, %d], Total Up2Now Inc Cnt = [%d], Combination Rate = [%5.2f\%], Give-up Rate = [%5.2f\%], Local UI Shows AvgSpeed of [TTT Times/MatCnt/MatWgt] = %d/%d/%5.2f.\n",\
//			gTaskL3bfdfContext.wsValueNbrFree, gTaskL3bfdfContext.wsValueNbrWeight,\
//			gTaskL3bfdfContext.wsValueNbrTtt, gTaskL3bfdfContext.wsValueNbrTgu, gTaskL3bfdfContext.staUp2Now.wsIncMatCnt, \
//			(float)(gTaskL3bfdfContext.staUp2Now.wsTttMatCnt)/(float)((gTaskL3bfdfContext.staUp2Now.wsIncMatCnt==0)?0.01:gTaskL3bfdfContext.staUp2Now.wsIncMatCnt) * 100, \
//			(float)(gTaskL3bfdfContext.staUp2Now.wsTgvMatCnt)/(float)((gTaskL3bfdfContext.staUp2Now.wsIncMatCnt==0)?0.01:gTaskL3bfdfContext.staUp2Now.wsIncMatCnt) * 100, \
//			gTaskL3bfdfContext.staLocalUi.wsAvgTttTimes, gTaskL3bfdfContext.staLocalUi.wsAvgTttMatCnt, gTaskL3bfdfContext.staLocalUi.wsAvgTttMatWgt);
//	//将当前基础统计周期的数据清零
//	memset(&(gTaskL3bfdfContext.cur), 0, sizeof(HcuSysMsgIeL3bfdfContextStaElement_t));
//
//	//返回
//	return SUCCESS;
//}
//
//
//
///***************************************************************************************************************************
// *
// * 　高级简化技巧
// *
// ***************************************************************************************************************************/
////由于错误，直接从差错中转入扫描状态
////它提供了一种比RESTART更快更低级的方式，让L3状态机直接返回到扫描状态
//void func_l3bfdf_stm_main_recovery_from_fault(void)
//{
//	//控制其它模块进入正常状态
//
//	//状态转移到SLEEP状态
//	FsmSetState(TASK_ID_L3BFDF, FSM_STATE_L3BFDF_OOS_SCAN);
//
//	//初始化模块的任务资源
//	//初始化定时器：暂时决定不做，除非该模块重新RESTART
//	//初始化模块级全局变量：暂时决定不做，除非该模块重新RESTART
//
//	return;
//}
//
//
//void func_l3bfdf_test_combine(void)
//{
//	int res = 0;
//
//	//func_bfdfuicomm_read_cfg_file_into_ctrl_table();
//
//	gTaskL3bfdfContext.sensorWs[9].sensorStatus = HCU_L3BFDF_SENSOR_WS_STATUS_VALID_TO_COMB;
//	gTaskL3bfdfContext.sensorWs[9].sensorValue = 5000;
//	gTaskL3bfdfContext.sensorWs[10].sensorStatus = HCU_L3BFDF_SENSOR_WS_STATUS_VALID_TO_COMB;
//	gTaskL3bfdfContext.sensorWs[10].sensorValue = 6000;
//
//	res = func_l3bfdf_ws_sensor_search_combination();
//
//	HCU_DEBUG_PRINT_FAT("L3BFDF: Test Combination result = %d\n", res);
//}


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






