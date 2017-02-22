/*
 * llczhb.c
 *
 *  Created on: 2016年5月14日
 *      Author: hitpony
 */

#include "llczhb.h"

#include "../l0service/timer.h"
#include "../l0service/trace.h"
#include "../l1com/l1comdef.h"
#include "../l2codec/zhbhjt212codec.h"

/*
** FSM of the LLCZHB
*/
HcuFsmStateItem_t HcuFsmLlczhb[] =
{
    //MessageId                 			//State                   		 		//Function
	//启始点，固定定义，不要改动, 使用ENTRY/END，意味者MSGID肯定不可能在某个高位区段中；考虑到所有任务共享MsgId，即使分段，也无法实现
	//完全是为了给任务一个初始化的机会，按照状态转移机制，该函数不具备启动的机会，因为任务初始化后自动到FSM_STATE_IDLE
	//如果没有必要进行初始化，可以设置为NULL
	{MSG_ID_ENTRY,       					FSM_STATE_ENTRY,            			fsm_llczhb_task_entry}, //Starting

	//System level initialization, only controlled by HCU-MAIN
    {MSG_ID_COM_INIT,       				FSM_STATE_IDLE,            				fsm_llczhb_init},
    {MSG_ID_COM_INIT_FEEDBACK,				FSM_STATE_IDLE,            				fsm_com_do_nothing},

	//Task level initialization
    {MSG_ID_COM_INIT,       				FSM_STATE_LLCZHB_INITIED,            	fsm_llczhb_init},
    {MSG_ID_COM_INIT_FEEDBACK,				FSM_STATE_LLCZHB_INITIED,            	fsm_com_do_nothing},

	//ANY state entry
    {MSG_ID_COM_INIT_FEEDBACK,				FSM_STATE_COMMON,          				fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT,       			FSM_STATE_COMMON,          				fsm_com_heart_beat_rcv},
	{MSG_ID_COM_STOP,       				FSM_STATE_COMMON,          				fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT_FB,       		FSM_STATE_COMMON,          				fsm_com_do_nothing},
    {MSG_ID_COM_RESTART,					FSM_STATE_COMMON,            			fsm_llczhb_restart},
	{MSG_ID_COM_TIME_OUT,       			FSM_STATE_COMMON,          				fsm_llczhb_time_out},

    //Task level initialization
    {MSG_ID_CLOUDVELA_LLCZHB_FRAME_REQ,		FSM_STATE_LLCZHB_ACTIVED,            	fsm_llczhb_cloudvela_l2frame_req},
    {MSG_ID_L3MOD_LLCZHB_CTRL_RESP,			FSM_STATE_LLCZHB_ACTIVED,            	fsm_llczhb_l3mod_llczhb_ctrl_resp},
    {MSG_ID_L3MOD_LLCZHB_DATA_REPORT,		FSM_STATE_LLCZHB_ACTIVED,            	fsm_llczhb_l3mod_llczhb_data_report},

    //结束点，固定定义，不要改动
    {MSG_ID_END,            	FSM_STATE_END,             				NULL},  //Ending
};

//Global variables

//Task Global variables
gTaskLlczhbContext_t gTaskLlczhbContext;

//Main Entry
//Input parameter would be useless, but just for similar structure purpose
OPSTAT fsm_llczhb_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//除了对全局变量进行操作之外，尽量不要做其它操作，因为该函数将被主任务/线程调用，不是本任务/线程调用
	//该API就是给本任务一个提早介入的入口，可以帮着做些测试性操作
	if (FsmSetState(TASK_ID_LLCZHB, FSM_STATE_IDLE) == FAILURE){
		HcuErrorPrint("LLCZHB: Error Set FSM State at fsm_llczhb_task_entry\n");}
	return SUCCESS;
}

OPSTAT fsm_llczhb_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;

	if ((src_id > TASK_ID_MIN) &&(src_id < TASK_ID_MAX)){
		//Send back MSG_ID_COM_INIT_FEEDBACK to SVRCON
		msg_struct_com_init_feedback_t snd0;
		memset(&snd0, 0, sizeof(msg_struct_com_init_feedback_t));
		snd0.length = sizeof(msg_struct_com_init_feedback_t);

		//to avoid all task send out the init fb msg at the same time which lead to msgque get stuck
		hcu_usleep(dest_id*HCU_SYSCFG_DURATION_OF_INIT_FB_WAIT_MAX);

		ret = hcu_message_send(MSG_ID_COM_INIT_FEEDBACK, src_id, TASK_ID_LLCZHB, &snd0, snd0.length);
		if (ret == FAILURE){
			HcuErrorPrint("LLCZHB: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_LLCZHB].taskName, zHcuVmCtrTab.task[src_id].taskName);
			return FAILURE;
		}
	}

	//收到初始化消息后，进入初始化状态
	if (FsmSetState(TASK_ID_LLCZHB, FSM_STATE_LLCZHB_INITED) == FAILURE){
		HcuErrorPrint("LLCZHB: Error Set FSM State!\n");
		return FAILURE;
	}

	//初始化硬件接口
	if (func_llczhb_int_init() == FAILURE){
		HcuErrorPrint("LLCZHB: Error initialize interface!\n");
		return FAILURE;
	}

	//Global Variables
	zHcuSysStaPm.taskRunErrCnt[TASK_ID_LLCZHB] = 0;
	memset(&gTaskLlczhbContext, 0, sizeof(gTaskLlczhbContext_t));
	gTaskLlczhbContext.envSd.overTimeDur = LLCZHB_CFG_STATIC_LLC_OVERTIME_DUR;
	gTaskLlczhbContext.envSd.resndTimesSet = LLCZHB_CFG_STATIC_LLC_RESND_CNT;
	//因为减少消息的使用，必须使用比特位来控制状态
	gTaskLlczhbContext.llcState = LLCZHB_STATE_CTRL_DEACTIVE;

	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_LLCZHB, FSM_STATE_LLCZHB_ACTIVED) == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_LLCZHB]++;
		HcuErrorPrint("LLCZHB: Error Set FSM State!\n");
		return FAILURE;
	}
	HCU_DEBUG_PRINT_FAT("LLCZHB: Enter FSM_STATE_LLCZHB_ACTIVED status, Keeping refresh here!\n");

	return SUCCESS;
}

OPSTAT fsm_llczhb_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HcuErrorPrint("LLCZHB: Internal error counter reach DEAD level, SW-RESTART soon!\n");
	zHcuSysStaPm.statisCnt.restartCnt++;
	fsm_llczhb_init(0, 0, NULL, 0);
	return SUCCESS;
}

OPSTAT func_llczhb_int_init(void)
{
	return SUCCESS;
}

OPSTAT fsm_llczhb_time_out(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;

	//Receive message and copy to local variable
	msg_struct_com_time_out_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_com_time_out_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_com_time_out_t)))
		HCU_ERROR_PRINT_CLOUDVELA("LLCZHB: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);

	//钩子在此处，检查zHcuSysStaPm.taskRunErrCnt[TASK_ID_LLCZHB]是否超限
	if (zHcuSysStaPm.taskRunErrCnt[TASK_ID_LLCZHB] > HCU_RUN_ERROR_LEVEL_3_CRITICAL){
		//减少重复RESTART的概率
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_LLCZHB] = zHcuSysStaPm.taskRunErrCnt[TASK_ID_LLCZHB] - HCU_RUN_ERROR_LEVEL_3_CRITICAL;
		msg_struct_com_restart_t snd0;
		memset(&snd0, 0, sizeof(msg_struct_com_restart_t));
		snd0.length = sizeof(msg_struct_com_restart_t);
		if (hcu_message_send(MSG_ID_COM_RESTART, TASK_ID_LLCZHB, TASK_ID_LLCZHB, &snd0, snd0.length) == FAILURE)
			HCU_ERROR_PRINT_LLCZHB("LLCZHB: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_LLCZHB].taskName, zHcuVmCtrTab.task[TASK_ID_LLCZHB].taskName);
	}

	//定时长时钟进行链路检测的
	if ((rcv.timeId == TIMER_ID_1S_LLCZHB_LINK_CTRL) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		ret = func_llczhb_time_out_link_control_process();
	}

	return ret;
}




//从MAC层收到解码好的帧
OPSTAT fsm_llczhb_cloudvela_l2frame_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret=0;
	msg_struct_cloudvela_llczhb_frame_req_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_cloudvela_llczhb_frame_req_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_cloudvela_llczhb_frame_req_t)))
		HCU_ERROR_PRINT_LLCZHB("LLCZHB: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);

	//链路MN在编解码过程中直接进行检查

	//检查链路系统码
	if ((rcv.head.st != HCU_SYSCFG_CLOUD_SVR_DEFAULT_ST_CODE) && (rcv.head.st != ZHBHJT_IE_uni_STcode_system_interaction))
		HCU_ERROR_PRINT_LLCZHB("LLCZHB: Receive frame error!\n");

	//检查链路密码：只有在正常业务操作的情况下才有意义，因为在ZHBHJT_IE_uni_STcode_system_interaction情况下本来就不存在
	if ((rcv.head.cn != ZHBHJT_IE_uni_CNcode_par_set_access_pswd_req_1072) && (rcv.head.st !=ZHBHJT_IE_uni_STcode_system_interaction) && (strcmp(rcv.head.pw, gTaskLlczhbContext.envSd.pswd) != 0))
		HCU_ERROR_PRINT_LLCZHB("LLCZHB: Receive frame error!\n");

	//检查链路FLAG，待定

	//将链路层数据存入到静态上下文，以便后续继续使用
	if (rcv.head.qn !=0) gTaskLlczhbContext.frameHead.qn = rcv.head.qn;
	if (rcv.head.st !=0) gTaskLlczhbContext.frameHead.st = rcv.head.st;
	if (rcv.head.cn != 0) gTaskLlczhbContext.frameHead.cn = rcv.head.cn;
	if (rcv.head.pw != NULL) strncpy(gTaskLlczhbContext.frameHead.pw, rcv.head.pw, strlen(rcv.head.pw));
	if (rcv.head.pno !=0) gTaskLlczhbContext.frameHead.pno = rcv.head.pno;
	if (rcv.head.pnum !=0) gTaskLlczhbContext.frameHead.pnum = rcv.head.pnum;
	if (rcv.head.ansFlag !=0) gTaskLlczhbContext.frameHead.ansFlag = rcv.head.ansFlag;

	//申明变量
	msg_struct_llczhb_l3mod_ctrl_req_t snd;

	//根据不同的接收命令进行判定
	switch (rcv.head.cn){
	//初始化超时时间与重发次数
	case ZHBHJT_IE_uni_CNcode_init_set_resend_time_out_req_1000:
		memset(&gTaskLlczhbContext.llcSn, 0, sizeof(gTaskLlczhbContextLinkDynamic_t));
		gTaskLlczhbContext.llcState = LLCZHB_STATE_CTRL_ACTIVE;

		//发送L2_LLC反馈给后台
		if (fsm_llczhb_send_to_cloud_ctrl_req_answer_directly_in_l2llc_9011(ZHBHJT_IE_uni_QNRTN_PREPARE_REQ) == FAILURE)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Send L2frame to cloud error!\n");

		//链路超时与重发次数设置
		gTaskLlczhbContext.envSd.overTimeDur = rcv.dl2Self.OverTime;
		gTaskLlczhbContext.envSd.resndTimesSet = rcv.dl2Self.ReCount;

		//发送完成终结函数到后台
		if (fsm_llczhb_send_to_cloud_ctrl_execute_operation_result_9012(ZHBHJT_IE_uni_EXETRN_EXE_SUCCESS) == FAILURE)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Send L2frame to cloud error!\n");
		gTaskLlczhbContext.llcState = LLCZHB_STATE_CTRL_DEACTIVE;
		break;

	//现场超限报警时间
	case ZHBHJT_IE_uni_CNcode_init_set_uplimit_duration_req_1001:
		memset(&gTaskLlczhbContext.llcSn, 0, sizeof(gTaskLlczhbContextLinkDynamic_t));
		gTaskLlczhbContext.llcState = LLCZHB_STATE_CTRL_ACTIVE;

		//发送L2_LLC反馈给后台
		if (fsm_llczhb_send_to_cloud_ctrl_req_answer_directly_in_l2llc_9011(ZHBHJT_IE_uni_QNRTN_PREPARE_REQ) == FAILURE)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Send L2frame to cloud error!\n");

		//将请求发往L3MOD
		memset(&snd, 0, sizeof(msg_struct_llczhb_l3mod_ctrl_req_t));
		snd.length = sizeof(msg_struct_llczhb_l3mod_ctrl_req_t);
		snd.actionId = HCU_SYSMSG_ZHBHJT_ACTION_SET_WARN_TIME_1001;
		snd.dl2Self.WarnTime = rcv.dl2Self.WarnTime;
		if (hcu_message_send(MSG_ID_LLCZHB_L3MOD_CTRL_REQ, TASK_ID_L3AQYCG20, TASK_ID_LLCZHB, &snd, snd.length) == FAILURE)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_LLCZHB].taskName, zHcuVmCtrTab.task[TASK_ID_L3AQYCG20].taskName);
		gTaskLlczhbContext.llcState = LLCZHB_STATE_CTRL_WFFB_FROM_L3MOD_SET_WARN_TIME_1001;
		break;

	//获取时间
	case ZHBHJT_IE_uni_CNcode_par_get_field_time_req_1011:
		memset(&gTaskLlczhbContext.llcSn, 0, sizeof(gTaskLlczhbContextLinkDynamic_t));
		gTaskLlczhbContext.llcState = LLCZHB_STATE_CTRL_ACTIVE;

		//发送L2_LLC反馈给后台
		if (fsm_llczhb_send_to_cloud_ctrl_req_answer_directly_in_l2llc_9011(ZHBHJT_IE_uni_QNRTN_PREPARE_REQ) == FAILURE)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Send L2frame to cloud error!\n");

		//执行上传现场时间到后台
		if (fsm_llczhb_send_to_cloud_data_get_field_time_1011() == FAILURE)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Send L2frame to cloud error!\n");

		//发送完成终结函数到后台
		if (fsm_llczhb_send_to_cloud_ctrl_execute_operation_result_9012(ZHBHJT_IE_uni_EXETRN_EXE_SUCCESS) == FAILURE)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Send L2frame to cloud error!\n");
		gTaskLlczhbContext.llcState = LLCZHB_STATE_CTRL_DEACTIVE;
		break;

	//设置本地时间
	case ZHBHJT_IE_uni_CNcode_par_set_field_time_req_1012:
		memset(&gTaskLlczhbContext.llcSn, 0, sizeof(gTaskLlczhbContextLinkDynamic_t));
		gTaskLlczhbContext.llcState = LLCZHB_STATE_CTRL_ACTIVE;

		//发送L2_LLC反馈给后台
		if (fsm_llczhb_send_to_cloud_ctrl_req_answer_directly_in_l2llc_9011(ZHBHJT_IE_uni_QNRTN_PREPARE_REQ) == FAILURE)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Send L2frame to cloud error!\n");

		//将收到的时间设置到本地
		hcu_vm_set_local_time(rcv.dl2Self.SystemTime);

		//发送完成终结函数到后台
		if (fsm_llczhb_send_to_cloud_ctrl_execute_operation_result_9012(ZHBHJT_IE_uni_EXETRN_EXE_SUCCESS) == FAILURE)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Send L2frame to cloud error!\n");
		gTaskLlczhbContext.llcState = LLCZHB_STATE_CTRL_DEACTIVE;
		break;

	//污染物门限
	case ZHBHJT_IE_uni_CNcode_par_get_pollution_limitaion_req_1021:
		memset(&gTaskLlczhbContext.llcSn, 0, sizeof(gTaskLlczhbContextLinkDynamic_t));
		gTaskLlczhbContext.llcState = LLCZHB_STATE_CTRL_ACTIVE;

		//发送L2_LLC反馈给后台
		if (fsm_llczhb_send_to_cloud_ctrl_req_answer_directly_in_l2llc_9011(ZHBHJT_IE_uni_QNRTN_PREPARE_REQ) == FAILURE)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Send L2frame to cloud error!\n");

		//将请求发往L3MOD
		memset(&snd, 0, sizeof(msg_struct_llczhb_l3mod_ctrl_req_t));
		snd.length = sizeof(msg_struct_llczhb_l3mod_ctrl_req_t);
		snd.actionId = HCU_SYSMSG_ZHBHJT_ACTION_GET_ALMLIM_1021;
		snd.dl2Self.nbrOfPolId = rcv.dl2Self.nbrOfPolId;
		memcpy(&(rcv.dl2Self.multiPolid), &(rcv.dl2Self.multiPolid), sizeof(UINT8)*HCU_SYSMSG_ZHBHJT_POLID_NBR_MAX);
		if (hcu_message_send(MSG_ID_LLCZHB_L3MOD_CTRL_REQ, TASK_ID_L3AQYCG20, TASK_ID_LLCZHB, &snd, snd.length) == FAILURE)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_LLCZHB].taskName, zHcuVmCtrTab.task[TASK_ID_L3AQYCG20].taskName);
		gTaskLlczhbContext.llcState = LLCZHB_STATE_CTRL_WFFB_FROM_L3MOD_GET_ALARM_LIMUTATION_1021;

		break;

	//设置污染物汇报门限值
	case ZHBHJT_IE_uni_CNcode_par_set_pollution_limitaion_req_1022:
		memset(&gTaskLlczhbContext.llcSn, 0, sizeof(gTaskLlczhbContextLinkDynamic_t));
		gTaskLlczhbContext.llcState = LLCZHB_STATE_CTRL_ACTIVE;

		//发送L2_LLC反馈给后台
		if (fsm_llczhb_send_to_cloud_ctrl_req_answer_directly_in_l2llc_9011(ZHBHJT_IE_uni_QNRTN_PREPARE_REQ) == FAILURE)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Send L2frame to cloud error!\n");

		//将请求发往L3MOD
		memset(&snd, 0, sizeof(msg_struct_llczhb_l3mod_ctrl_req_t));
		snd.length = sizeof(msg_struct_llczhb_l3mod_ctrl_req_t);
		snd.actionId = HCU_SYSMSG_ZHBHJT_ACTION_SET_ALMLIM_1022;
		snd.dl2Self.nbrOfLimitation = rcv.dl2Self.nbrOfLimitation;
		memcpy(&(rcv.dl2Self.limitation), &(rcv.dl2Self.limitation), sizeof(msgie_struct_zhbhjt_frame_data_low_upvalue_t)*HCU_SYSMSG_ZHBHJT_POLID_NBR_MAX);
		if (hcu_message_send(MSG_ID_LLCZHB_L3MOD_CTRL_REQ, TASK_ID_L3AQYCG20, TASK_ID_LLCZHB, &snd, snd.length) == FAILURE)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_LLCZHB].taskName, zHcuVmCtrTab.task[TASK_ID_L3AQYCG20].taskName);
		gTaskLlczhbContext.llcState = LLCZHB_STATE_CTRL_WFFB_FROM_L3MOD_SET_ALARM_LIMITATION_1022;

		//等待上面的传感器设置的结果L3MOD_LLCZHB_CTRL_RESP
		break;

	//提取上位机地址
	case ZHBHJT_IE_uni_CNcode_par_get_field_addr_req_1031:
		memset(&gTaskLlczhbContext.llcSn, 0, sizeof(gTaskLlczhbContextLinkDynamic_t));
		gTaskLlczhbContext.llcState = LLCZHB_STATE_CTRL_ACTIVE;

		//发送L2_LLC反馈给后台
		if (fsm_llczhb_send_to_cloud_ctrl_req_answer_directly_in_l2llc_9011(ZHBHJT_IE_uni_QNRTN_PREPARE_REQ) == FAILURE)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Send L2frame to cloud error!\n");

		//读取上位机地址并发送到后台
		if (fsm_llczhb_send_to_cloud_data_get_field_addr_1031() == FAILURE)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Send L2frame to cloud error!\n");

		//发送完成终结函数到后台
		if (fsm_llczhb_send_to_cloud_ctrl_execute_operation_result_9012(ZHBHJT_IE_uni_EXETRN_EXE_SUCCESS) == FAILURE)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Send L2frame to cloud error!\n");
		gTaskLlczhbContext.llcState = LLCZHB_STATE_CTRL_DEACTIVE;
		break;

	//设置上位机地址
	case ZHBHJT_IE_uni_CNcode_par_set_field_addr_req_1032:
		memset(&gTaskLlczhbContext.llcSn, 0, sizeof(gTaskLlczhbContextLinkDynamic_t));
		gTaskLlczhbContext.llcState = LLCZHB_STATE_CTRL_ACTIVE;

		//发送L2_LLC反馈给后台
		if (fsm_llczhb_send_to_cloud_ctrl_req_answer_directly_in_l2llc_9011(ZHBHJT_IE_uni_QNRTN_PREPARE_REQ) == FAILURE)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Send L2frame to cloud error!\n");

		//设置上位机地址
		gTaskLlczhbContext.envSd.AlarmTarget = rcv.dl2Self.AlarmTarget;

		//发送完成终结函数到后台
		if (fsm_llczhb_send_to_cloud_ctrl_execute_operation_result_9012(ZHBHJT_IE_uni_EXETRN_EXE_SUCCESS) == FAILURE)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Send L2frame to cloud error!\n");
		gTaskLlczhbContext.llcState = LLCZHB_STATE_CTRL_DEACTIVE;
		break;

	//设置数据上报时间
	case ZHBHJT_IE_uni_CNcode_par_get_data_report_time_req_1041:
		memset(&gTaskLlczhbContext.llcSn, 0, sizeof(gTaskLlczhbContextLinkDynamic_t));
		gTaskLlczhbContext.llcState = LLCZHB_STATE_CTRL_ACTIVE;

		//发送L2_LLC反馈给后台
		if (fsm_llczhb_send_to_cloud_ctrl_req_answer_directly_in_l2llc_9011(ZHBHJT_IE_uni_QNRTN_PREPARE_REQ) == FAILURE)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Send L2frame to cloud error!\n");

		//将请求发往L3MOD
		memset(&snd, 0, sizeof(msg_struct_llczhb_l3mod_ctrl_req_t));
		snd.length = sizeof(msg_struct_llczhb_l3mod_ctrl_req_t);
		snd.actionId = HCU_SYSMSG_ZHBHJT_ACTION_GET_RPT_TIME_1041;
		if (hcu_message_send(MSG_ID_LLCZHB_L3MOD_CTRL_REQ, TASK_ID_L3AQYCG20, TASK_ID_LLCZHB, &snd, snd.length) == FAILURE)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_LLCZHB].taskName, zHcuVmCtrTab.task[TASK_ID_L3AQYCG20].taskName);
		gTaskLlczhbContext.llcState = LLCZHB_STATE_CTRL_WFFB_FROM_L3MOD_GET_DATA_RPT_TIME_1041;
		break;

	//设置数据上报时间
	case ZHBHJT_IE_uni_CNcode_par_set_data_report_time_req_1042:
		memset(&gTaskLlczhbContext.llcSn, 0, sizeof(gTaskLlczhbContextLinkDynamic_t));
		gTaskLlczhbContext.llcState = LLCZHB_STATE_CTRL_ACTIVE;

		//发送L2_LLC反馈给后台
		if (fsm_llczhb_send_to_cloud_ctrl_req_answer_directly_in_l2llc_9011(ZHBHJT_IE_uni_QNRTN_PREPARE_REQ) == FAILURE)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Send L2frame to cloud error!\n");

		//将请求发往L3MOD
		memset(&snd, 0, sizeof(msg_struct_llczhb_l3mod_ctrl_req_t));
		snd.length = sizeof(msg_struct_llczhb_l3mod_ctrl_req_t);
		snd.actionId = HCU_SYSMSG_ZHBHJT_ACTION_SET_RPT_TIME_1042;
		snd.dl2Self.ReportTime = rcv.dl2Self.ReportTime;
		if (hcu_message_send(MSG_ID_LLCZHB_L3MOD_CTRL_REQ, TASK_ID_L3AQYCG20, TASK_ID_LLCZHB, &snd, snd.length) == FAILURE)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_LLCZHB].taskName, zHcuVmCtrTab.task[TASK_ID_L3AQYCG20].taskName);
		gTaskLlczhbContext.llcState = LLCZHB_STATE_CTRL_WFFB_FROM_L3MOD_SET_DATA_RPT_TIME_1042;
		break;

	//提取实时采样数据上报间隔
	case ZHBHJT_IE_uni_CNcode_par_get_rtdi_interval_req_1061:
		memset(&gTaskLlczhbContext.llcSn, 0, sizeof(gTaskLlczhbContextLinkDynamic_t));
		gTaskLlczhbContext.llcState = LLCZHB_STATE_CTRL_ACTIVE;

		//发送L2_LLC反馈给后台
		if (fsm_llczhb_send_to_cloud_ctrl_req_answer_directly_in_l2llc_9011(ZHBHJT_IE_uni_QNRTN_PREPARE_REQ) == FAILURE)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Send L2frame to cloud error!\n");

		//将请求发往L3MOD
		memset(&snd, 0, sizeof(msg_struct_llczhb_l3mod_ctrl_req_t));
		snd.length = sizeof(msg_struct_llczhb_l3mod_ctrl_req_t);
		snd.actionId = HCU_SYSMSG_ZHBHJT_ACTION_GET_RTDI_1061;
		if (hcu_message_send(MSG_ID_LLCZHB_L3MOD_CTRL_REQ, TASK_ID_L3AQYCG20, TASK_ID_LLCZHB, &snd, snd.length) == FAILURE)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_LLCZHB].taskName, zHcuVmCtrTab.task[TASK_ID_L3AQYCG20].taskName);
		gTaskLlczhbContext.llcState = LLCZHB_STATE_CTRL_WFFB_FROM_L3MOD_GET_RTDI_1061;
		break;

	//设置实时采样数据上报间隔
	case ZHBHJT_IE_uni_CNcode_par_set_rtdi_interval_req_1062:
		memset(&gTaskLlczhbContext.llcSn, 0, sizeof(gTaskLlczhbContextLinkDynamic_t));
		gTaskLlczhbContext.llcState = LLCZHB_STATE_CTRL_ACTIVE;

		//发送L2_LLC反馈给后台
		if (fsm_llczhb_send_to_cloud_ctrl_req_answer_directly_in_l2llc_9011(ZHBHJT_IE_uni_QNRTN_PREPARE_REQ) == FAILURE)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Send L2frame to cloud error!\n");

		//将请求发往L3MOD
		memset(&snd, 0, sizeof(msg_struct_llczhb_l3mod_ctrl_req_t));
		snd.length = sizeof(msg_struct_llczhb_l3mod_ctrl_req_t);
		snd.actionId = HCU_SYSMSG_ZHBHJT_ACTION_SET_RTDI_1062;
		snd.dl2Self.RtdInterval = rcv.dl2Self.RtdInterval;
		if (hcu_message_send(MSG_ID_LLCZHB_L3MOD_CTRL_REQ, TASK_ID_L3AQYCG20, TASK_ID_LLCZHB, &snd, snd.length) == FAILURE)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_LLCZHB].taskName, zHcuVmCtrTab.task[TASK_ID_L3AQYCG20].taskName);
		gTaskLlczhbContext.llcState = LLCZHB_STATE_CTRL_WFFB_FROM_L3MOD_SET_RTDI_1062;
		break;

	//设置新的密码
	case ZHBHJT_IE_uni_CNcode_par_set_access_pswd_req_1072:
		memset(&gTaskLlczhbContext.llcSn, 0, sizeof(gTaskLlczhbContextLinkDynamic_t));
		gTaskLlczhbContext.llcState = LLCZHB_STATE_CTRL_ACTIVE;

		//设置新的密码
		strncpy(gTaskLlczhbContext.envSd.pswd, rcv.setpw, strlen(rcv.setpw));

		//发送L2_LLC反馈给后台
		if (fsm_llczhb_send_to_cloud_ctrl_req_answer_directly_in_l2llc_9011(ZHBHJT_IE_uni_QNRTN_PREPARE_REQ) == FAILURE)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Send L2frame to cloud error!\n");

		//发送完成终结函数到后台
		if (fsm_llczhb_send_to_cloud_ctrl_execute_operation_result_9012(ZHBHJT_IE_uni_EXETRN_EXE_SUCCESS) == FAILURE)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Send L2frame to cloud error!\n");
		gTaskLlczhbContext.llcState = LLCZHB_STATE_CTRL_DEACTIVE;
		break;

	//很可能是超标告警，其它的流程没来过
	case ZHBHJT_IE_uni_CNcode_cmd_notification_req_9013:
		if (gTaskLlczhbContext.llcState != LLCZHB_STATE_CTRL_WFFB_FROM_CLOUD_REPORT_ALA_TYPE_2072)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Wrong state entered!\n");

		//STOP TIMER
		hcu_timer_stop(TASK_ID_LLCZHB, TIMER_ID_1S_LLCZHB_LINK_CTRL, TIMER_RESOLUTION_1S);

		//检查QN
		if (gTaskLlczhbContext.frameHead.qn != rcv.cfmQn) HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Received wrong parameter!\n");
		gTaskLlczhbContext.llcState = LLCZHB_STATE_CTRL_DEACTIVE;
		break;

	case ZHBHJT_IE_uni_CNcode_cmd_data_req_9014:
		HCU_ERROR_PRINT_LLCZHB("LLCZHB: Error action received!\n");
		break;

	//实时数据采集
	case ZHBHJT_IE_uni_CNcode_dat_get_rt_data_req_2011:
		memset(&gTaskLlczhbContext.llcSn, 0, sizeof(gTaskLlczhbContextLinkDynamic_t));
		gTaskLlczhbContext.llcState = LLCZHB_STATE_CTRL_ACTIVE;

		//发送L2_LLC反馈给后台
		if (fsm_llczhb_send_to_cloud_ctrl_req_answer_directly_in_l2llc_9011(ZHBHJT_IE_uni_QNRTN_PREPARE_REQ) == FAILURE)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Send L2frame to cloud error!\n");

		//将请求发往L3MOD
		memset(&snd, 0, sizeof(msg_struct_llczhb_l3mod_ctrl_req_t));
		snd.length = sizeof(msg_struct_llczhb_l3mod_ctrl_req_t);
		snd.actionId = HCU_SYSMSG_ZHBHJT_ACTION_GET_POL_RTD_2011;
		if (hcu_message_send(MSG_ID_LLCZHB_L3MOD_CTRL_REQ, TASK_ID_L3AQYCG20, TASK_ID_LLCZHB, &snd, snd.length) == FAILURE)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_LLCZHB].taskName, zHcuVmCtrTab.task[TASK_ID_L3AQYCG20].taskName);
		gTaskLlczhbContext.llcState = LLCZHB_STATE_CTRL_WFFB_FROM_L3MOD_GET_RTD_2011;
		break;

	//停止查看数据
	case ZHBHJT_IE_uni_CNcode_dat_stop_monitor_rt_data_req_2012:
		if (gTaskLlczhbContext.llcState != LLCZHB_STATE_CTRL_WFFB_FROM_CLOUD_GET_RTD_2011)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Wrong state of receive info!\n");

		//STOP TIMER
		hcu_timer_stop(TASK_ID_LLCZHB, TIMER_ID_1S_LLCZHB_LINK_CTRL, TIMER_RESOLUTION_1S);

		//发送CTRL_CMD给L3MOD
		memset(&snd, 0, sizeof(msg_struct_llczhb_l3mod_ctrl_req_t));
		snd.length = sizeof(msg_struct_llczhb_l3mod_ctrl_req_t);
		snd.actionId = HCU_SYSMSG_ZHBHJT_ACTION_STOP_RTD_2012;
		if (hcu_message_send(MSG_ID_LLCZHB_L3MOD_CTRL_REQ, TASK_ID_L3AQYCG20, TASK_ID_LLCZHB, &snd, snd.length) == FAILURE)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_LLCZHB].taskName, zHcuVmCtrTab.task[TASK_ID_L3AQYCG20].taskName);

		//通知应答
		if (fsm_llczhb_send_to_cloud_ctrl_notify_answer_9013() == FAILURE)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Send L2frame to cloud error!\n");
		gTaskLlczhbContext.llcState = LLCZHB_STATE_CTRL_DEACTIVE;

		break;

	//取污染治理设备运行状态
	case ZHBHJT_IE_uni_CNcode_dat_get_equ_status_req_2021:
		memset(&gTaskLlczhbContext.llcSn, 0, sizeof(gTaskLlczhbContextLinkDynamic_t));
		gTaskLlczhbContext.llcState = LLCZHB_STATE_CTRL_ACTIVE;

		//发送L2_LLC反馈给后台
		if (fsm_llczhb_send_to_cloud_ctrl_req_answer_directly_in_l2llc_9011(ZHBHJT_IE_uni_QNRTN_PREPARE_REQ) == FAILURE)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Send L2frame to cloud error!\n");

		//将请求发往L3MOD
		memset(&snd, 0, sizeof(msg_struct_llczhb_l3mod_ctrl_req_t));
		snd.length = sizeof(msg_struct_llczhb_l3mod_ctrl_req_t);
		snd.actionId = HCU_SYSMSG_ZHBHJT_ACTION_GET_EQU_RUN_2021;
		if (hcu_message_send(MSG_ID_LLCZHB_L3MOD_CTRL_REQ, TASK_ID_L3AQYCG20, TASK_ID_LLCZHB, &snd, snd.length) == FAILURE)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_LLCZHB].taskName, zHcuVmCtrTab.task[TASK_ID_L3AQYCG20].taskName);
		gTaskLlczhbContext.llcState = LLCZHB_STATE_CTRL_WFFB_FROM_L3MOD_GET_EQU_RUN_2021;

		break;

	//停止查看设备运行状态
	case ZHBHJT_IE_uni_CNcode_dat_stop_monitor_equ_status_req_2022:
		if (gTaskLlczhbContext.llcState != LLCZHB_STATE_CTRL_WFFB_FROM_CLOUD_GET_EQU_RUN_2021)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Wrong state of receive info!\n");

		//STOP TIMER
		hcu_timer_stop(TASK_ID_LLCZHB, TIMER_ID_1S_LLCZHB_LINK_CTRL, TIMER_RESOLUTION_1S);
		//发送CTRL_CMD给L3MOD
		msg_struct_llczhb_l3mod_ctrl_req_t snd;
		memset(&snd, 0, sizeof(msg_struct_llczhb_l3mod_ctrl_req_t));
		snd.length = sizeof(msg_struct_llczhb_l3mod_ctrl_req_t);
		snd.actionId = HCU_SYSMSG_ZHBHJT_ACTION_STOP_EQU_RUN_2022;
		if (hcu_message_send(MSG_ID_LLCZHB_L3MOD_CTRL_REQ, TASK_ID_L3AQYCG20, TASK_ID_LLCZHB, &snd, snd.length) == FAILURE)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_LLCZHB].taskName, zHcuVmCtrTab.task[TASK_ID_L3AQYCG20].taskName);

		//通知应答
		if (fsm_llczhb_send_to_cloud_ctrl_notify_answer_9013() == FAILURE)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Send L2frame to cloud error!\n");
		gTaskLlczhbContext.llcState = LLCZHB_STATE_CTRL_DEACTIVE;
		break;

	//获取污染物日数据
	case ZHBHJT_IE_uni_CNcode_dat_get_pollution_his_data_req_2031:
		memset(&gTaskLlczhbContext.llcSn, 0, sizeof(gTaskLlczhbContextLinkDynamic_t));
		gTaskLlczhbContext.llcState = LLCZHB_STATE_CTRL_ACTIVE;

		//发送L2_LLC反馈给后台
		if (fsm_llczhb_send_to_cloud_ctrl_req_answer_directly_in_l2llc_9011(ZHBHJT_IE_uni_QNRTN_PREPARE_REQ) == FAILURE)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Send L2frame to cloud error!\n");

		//将请求发往L3MOD
		memset(&snd, 0, sizeof(msg_struct_llczhb_l3mod_ctrl_req_t));
		snd.length = sizeof(msg_struct_llczhb_l3mod_ctrl_req_t);
		snd.actionId = HCU_SYSMSG_ZHBHJT_ACTION_GET_POL_DAY_RPT_2031;
		snd.dl2Self.gapTime.BeginTime = rcv.dl2Self.gapTime.BeginTime;
		snd.dl2Self.gapTime.EndTime = rcv.dl2Self.gapTime.EndTime;
		if (hcu_message_send(MSG_ID_LLCZHB_L3MOD_CTRL_REQ, TASK_ID_L3AQYCG20, TASK_ID_LLCZHB, &snd, snd.length) == FAILURE)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_LLCZHB].taskName, zHcuVmCtrTab.task[TASK_ID_L3AQYCG20].taskName);
		gTaskLlczhbContext.llcState = LLCZHB_STATE_CTRL_WFFB_FROM_L3MOD_GET_POL_DAY_2031;

		break;

	//获取今天的运行的日数据
	case ZHBHJT_IE_uni_CNcode_dat_get_today_his_data_req_2041:
		memset(&gTaskLlczhbContext.llcSn, 0, sizeof(gTaskLlczhbContextLinkDynamic_t));
		gTaskLlczhbContext.llcState = LLCZHB_STATE_CTRL_ACTIVE;

		//发送L2_LLC反馈给后台
		if (fsm_llczhb_send_to_cloud_ctrl_req_answer_directly_in_l2llc_9011(ZHBHJT_IE_uni_QNRTN_PREPARE_REQ) == FAILURE)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Send L2frame to cloud error!\n");

		//将请求发往L3MOD
		memset(&snd, 0, sizeof(msg_struct_llczhb_l3mod_ctrl_req_t));
		snd.length = sizeof(msg_struct_llczhb_l3mod_ctrl_req_t);
		snd.actionId = HCU_SYSMSG_ZHBHJT_ACTION_GET_POL_RT_2041;
		snd.dl2Self.gapTime.BeginTime = rcv.dl2Self.gapTime.BeginTime;
		snd.dl2Self.gapTime.EndTime = rcv.dl2Self.gapTime.EndTime;
		if (hcu_message_send(MSG_ID_LLCZHB_L3MOD_CTRL_REQ, TASK_ID_L3AQYCG20, TASK_ID_LLCZHB, &snd, snd.length) == FAILURE)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_LLCZHB].taskName, zHcuVmCtrTab.task[TASK_ID_L3AQYCG20].taskName);
		gTaskLlczhbContext.llcState = LLCZHB_STATE_CTRL_WFFB_FROM_L3MOD_GET_POL_TODAY_RT_2041;
		break;

	//暂时只支持无应答／无拆分包的流程，其它的两个流程，未来再完善
	//fsm_llczhb_send_to_cloud_req_answer_directly_in_l2llc()中FLAG=0,表示无应答，FLAG=1，表示有应答但不拆包，FLAG=3标识有应答且拆包
	case ZHBHJT_IE_uni_CNcode_dat_get_pollution_min_data_req_2051:
		memset(&gTaskLlczhbContext.llcSn, 0, sizeof(gTaskLlczhbContextLinkDynamic_t));
		gTaskLlczhbContext.llcState = LLCZHB_STATE_CTRL_ACTIVE;

		//发送L2_LLC反馈给后台
		if (fsm_llczhb_send_to_cloud_ctrl_req_answer_directly_in_l2llc_9011(ZHBHJT_IE_uni_QNRTN_PREPARE_REQ) == FAILURE)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Send L2frame to cloud error!\n");

		//将请求发往L3MOD
		memset(&snd, 0, sizeof(msg_struct_llczhb_l3mod_ctrl_req_t));
		snd.length = sizeof(msg_struct_llczhb_l3mod_ctrl_req_t);
		snd.actionId = HCU_SYSMSG_ZHBHJT_ACTION_GET_POL_MIN_RPT_2051;
		snd.dl2Self.gapTime.BeginTime = rcv.dl2Self.gapTime.BeginTime;
		snd.dl2Self.gapTime.EndTime = rcv.dl2Self.gapTime.EndTime;
		if (hcu_message_send(MSG_ID_LLCZHB_L3MOD_CTRL_REQ, TASK_ID_L3AQYCG20, TASK_ID_LLCZHB, &snd, snd.length) == FAILURE)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_LLCZHB].taskName, zHcuVmCtrTab.task[TASK_ID_L3AQYCG20].taskName);
		gTaskLlczhbContext.llcState = LLCZHB_STATE_CTRL_WFFB_FROM_L3MOD_GET_POL_MIN_2051;

		break;

	case ZHBHJT_IE_uni_CNcode_dat_get_pollution_hour_data_req_2061:
		HCU_ERROR_PRINT_LLCZHB("LLCZHB: Error action received!\n");
		break;

	//取污染物报警记录
	case ZHBHJT_IE_uni_CNcode_dat_get_pollution_alarm_record_req_2071:
		memset(&gTaskLlczhbContext.llcSn, 0, sizeof(gTaskLlczhbContextLinkDynamic_t));
		gTaskLlczhbContext.llcState = LLCZHB_STATE_CTRL_ACTIVE;

		//发送L2_LLC反馈给后台
		if (fsm_llczhb_send_to_cloud_ctrl_req_answer_directly_in_l2llc_9011(ZHBHJT_IE_uni_QNRTN_PREPARE_REQ) == FAILURE)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Send L2frame to cloud error!\n");

		//将请求发往L3MOD
		memset(&snd, 0, sizeof(msg_struct_llczhb_l3mod_ctrl_req_t));
		snd.length = sizeof(msg_struct_llczhb_l3mod_ctrl_req_t);
		snd.actionId = HCU_SYSMSG_ZHBHJT_ACTION_GET_POL_ALA_2071;
		snd.dl2Self.gapTime.BeginTime = rcv.dl2Self.gapTime.BeginTime;
		snd.dl2Self.gapTime.EndTime = rcv.dl2Self.gapTime.EndTime;
		if (hcu_message_send(MSG_ID_LLCZHB_L3MOD_CTRL_REQ, TASK_ID_L3AQYCG20, TASK_ID_LLCZHB, &snd, snd.length) == FAILURE)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_LLCZHB].taskName, zHcuVmCtrTab.task[TASK_ID_L3AQYCG20].taskName);
		gTaskLlczhbContext.llcState = LLCZHB_STATE_CTRL_WFFB_FROM_L3MOD_GET_POL_ALA_2071;
		break;

	//下端设备校零校满
	case ZHBHJT_IE_uni_CNcode_ctr_calabration_req_3011:
		memset(&gTaskLlczhbContext.llcSn, 0, sizeof(gTaskLlczhbContextLinkDynamic_t));
		gTaskLlczhbContext.llcState = LLCZHB_STATE_CTRL_ACTIVE;

		//发送L2_LLC反馈给后台
		if (fsm_llczhb_send_to_cloud_ctrl_req_answer_directly_in_l2llc_9011(ZHBHJT_IE_uni_QNRTN_PREPARE_REQ) == FAILURE)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Send L2frame to cloud error!\n");

		//将请求发往L3MOD
		memset(&snd, 0, sizeof(msg_struct_llczhb_l3mod_ctrl_req_t));
		snd.length = sizeof(msg_struct_llczhb_l3mod_ctrl_req_t);
		snd.actionId = HCU_SYSMSG_ZHBHJT_ACTION_SET_ZERO_3011;
		snd.dl2Self.singlePolId = rcv.dl2Self.singlePolId;
		if (hcu_message_send(MSG_ID_LLCZHB_L3MOD_CTRL_REQ, TASK_ID_L3AQYCG20, TASK_ID_LLCZHB, &snd, snd.length) == FAILURE)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_LLCZHB].taskName, zHcuVmCtrTab.task[TASK_ID_L3AQYCG20].taskName);
		gTaskLlczhbContext.llcState = LLCZHB_STATE_CTRL_WFFB_FROM_L3MOD_SET_CALIBRATION_ZERO_3011;
		break;

	//立即启动设备采样
	case ZHBHJT_IE_uni_CNcode_ctr_start_sample_req_3012:
		memset(&gTaskLlczhbContext.llcSn, 0, sizeof(gTaskLlczhbContextLinkDynamic_t));
		gTaskLlczhbContext.llcState = LLCZHB_STATE_CTRL_ACTIVE;

		//发送L2_LLC反馈给后台
		if (fsm_llczhb_send_to_cloud_ctrl_req_answer_directly_in_l2llc_9011(ZHBHJT_IE_uni_QNRTN_PREPARE_REQ) == FAILURE)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Send L2frame to cloud error!\n");

		//将请求发往L3MOD
		memset(&snd, 0, sizeof(msg_struct_llczhb_l3mod_ctrl_req_t));
		snd.length = sizeof(msg_struct_llczhb_l3mod_ctrl_req_t);
		snd.actionId = HCU_SYSMSG_ZHBHJT_ACTION_SET_INST_SAMPLE_3012;
		snd.dl2Self.singlePolId = rcv.dl2Self.singlePolId;
		if (hcu_message_send(MSG_ID_LLCZHB_L3MOD_CTRL_REQ, TASK_ID_L3AQYCG20, TASK_ID_LLCZHB, &snd, snd.length) == FAILURE)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_LLCZHB].taskName, zHcuVmCtrTab.task[TASK_ID_L3AQYCG20].taskName);
		gTaskLlczhbContext.llcState = LLCZHB_STATE_CTRL_WFFB_FROM_L3MOD_SET_INSTANCE_SAMPLE_3012;
		break;

	case ZHBHJT_IE_uni_CNcode_ctr_equ_operation_req_3013:
		HCU_ERROR_PRINT_LLCZHB("LLCZHB: Error action received!\n");
		break;

	//设置设备采样周期
	case ZHBHJT_IE_uni_CNcode_ctr_set_sample_tti_req_3014:
		memset(&gTaskLlczhbContext.llcSn, 0, sizeof(gTaskLlczhbContextLinkDynamic_t));
		gTaskLlczhbContext.llcState = LLCZHB_STATE_CTRL_ACTIVE;

		//发送L2_LLC反馈给后台
		if (fsm_llczhb_send_to_cloud_ctrl_req_answer_directly_in_l2llc_9011(ZHBHJT_IE_uni_QNRTN_PREPARE_REQ) == FAILURE)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Send L2frame to cloud error!\n");

		//将请求发往L3MOD
		memset(&snd, 0, sizeof(msg_struct_llczhb_l3mod_ctrl_req_t));
		snd.length = sizeof(msg_struct_llczhb_l3mod_ctrl_req_t);
		snd.actionId = HCU_SYSMSG_ZHBHJT_ACTION_SET_SAMPLE_CTIME_3014;
		snd.dl2Self.nbrOfCTime = rcv.dl2Self.nbrOfCTime;
		memcpy(&(snd.dl2Self.ctime), &(rcv.dl2Self.ctime), sizeof(msgie_struct_zhbhjt_frame_data_ctime_t));
		snd.dl2Self.singlePolId = rcv.dl2Self.singlePolId;
		if (hcu_message_send(MSG_ID_LLCZHB_L3MOD_CTRL_REQ, TASK_ID_L3AQYCG20, TASK_ID_LLCZHB, &snd, snd.length) == FAILURE)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_LLCZHB].taskName, zHcuVmCtrTab.task[TASK_ID_L3AQYCG20].taskName);
		gTaskLlczhbContext.llcState = LLCZHB_STATE_CTRL_WFFB_FROM_L3MOD_SET_SAMPLE_CYCLE_3014;
		break;

	default:
		HCU_ERROR_PRINT_LLCZHB("LLCZHB: Error action received!\n");
		break;
	}



	return SUCCESS;
}

//控制命令，用于纯粹传递控制信息，如果需要反馈给上层，则不认为是控制命令，而在内部使用数据报告方式
OPSTAT fsm_llczhb_l3mod_llczhb_ctrl_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret=0;
	msg_struct_l3mod_llczhb_ctrl_resp rcv;
	memset(&rcv, 0, sizeof(msg_struct_l3mod_llczhb_ctrl_resp));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_l3mod_llczhb_ctrl_resp)))
		HCU_ERROR_PRINT_LLCZHB("LLCZHB: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);

	switch (rcv.actionId){
	case HCU_SYSMSG_ZHBHJT_ACTION_EXECUTE_FINISH_9012:
		HCU_ERROR_PRINT_LLCZHB("LLCZHB: Error action received!\n");
		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_DATA_CFM_9014:
		HCU_ERROR_PRINT_LLCZHB("LLCZHB: Error action received!\n");
		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_SET_FIELD_TIME_1012:
		HCU_ERROR_PRINT_LLCZHB("LLCZHB: Error action received!\n");
		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_STOP_RTD_2012:
		HCU_ERROR_PRINT_LLCZHB("LLCZHB: Error action received!\n");
		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_STOP_EQU_RUN_2022:
		HCU_ERROR_PRINT_LLCZHB("LLCZHB: Error action received!\n");
		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_SET_ALMLIM_1022:
		if (gTaskLlczhbContext.llcState != LLCZHB_STATE_CTRL_WFFB_FROM_L3MOD_SET_ALARM_LIMITATION_1022)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Not in the right state!\n");

		//发送完成终结函数到后台：实际上将根据操作的结果来定，结果写在ExeRtn中吧，省得再多费一个新的域
		if (fsm_llczhb_send_to_cloud_ctrl_execute_operation_result_9012(rcv.ul2Cloud.ExeRtn) == FAILURE)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Send L2frame to cloud error!\n");
		gTaskLlczhbContext.llcState = LLCZHB_STATE_CTRL_DEACTIVE;
		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_GET_ALMLIM_1021:
		if (gTaskLlczhbContext.llcState != LLCZHB_STATE_CTRL_WFFB_FROM_L3MOD_GET_ALARM_LIMUTATION_1021)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Not in the right state!\n");

		//发送数据到后台
		gTaskLlczhbContext.llcSn.ulTx.nbrOfAlmLim = rcv.ul2Cloud.nbrOfAlmLim;
		memcpy(&(gTaskLlczhbContext.llcSn.ulTx.limitation), &(rcv.ul2Cloud.limitation), sizeof(msgie_struct_zhbhjt_frame_data_low_upvalue_t)*HCU_SYSMSG_ZHBHJT_POLID_NBR_MAX);
		gTaskLlczhbContext.llcSn.ulTx.Ala = rcv.ul2Cloud.Ala;
		if (fsm_llczhb_send_to_cloud_data_get_alarm_limitation_1021() == FAILURE)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Send L2frame to cloud error!\n");

		//发送完成终结函数到后台：实际上将根据操作的结果来定，结果写在ExeRtn中吧，省得再多费一个新的域
		if (fsm_llczhb_send_to_cloud_ctrl_execute_operation_result_9012(rcv.ul2Cloud.ExeRtn) == FAILURE)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Send L2frame to cloud error!\n");
		gTaskLlczhbContext.llcState = LLCZHB_STATE_CTRL_DEACTIVE;
		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_SET_MNADDR_1032:
		HCU_ERROR_PRINT_LLCZHB("LLCZHB: Error action received!\n");
		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_SET_RPT_TIME_1042:
		if (gTaskLlczhbContext.llcState != LLCZHB_STATE_CTRL_WFFB_FROM_L3MOD_SET_DATA_RPT_TIME_1042)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Not in the right state!\n");

		//发送完成终结函数到后台：实际上将根据操作的结果来定，结果写在ExeRtn中吧，省得再多费一个新的域
		if (fsm_llczhb_send_to_cloud_ctrl_execute_operation_result_9012(rcv.ul2Cloud.ExeRtn) == FAILURE)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Send L2frame to cloud error!\n");
		gTaskLlczhbContext.llcState = LLCZHB_STATE_CTRL_DEACTIVE;
		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_SET_ZERO_3011:
		if (gTaskLlczhbContext.llcState != LLCZHB_STATE_CTRL_WFFB_FROM_L3MOD_SET_CALIBRATION_ZERO_3011)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Not in the right state!\n");

		//发送完成终结函数到后台：实际上将根据操作的结果来定，结果写在ExeRtn中吧，省得再多费一个新的域
		if (fsm_llczhb_send_to_cloud_ctrl_execute_operation_result_9012(rcv.ul2Cloud.ExeRtn) == FAILURE)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Send L2frame to cloud error!\n");
		gTaskLlczhbContext.llcState = LLCZHB_STATE_CTRL_DEACTIVE;
		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_SET_RTDI_1062:
		if (gTaskLlczhbContext.llcState != LLCZHB_STATE_CTRL_WFFB_FROM_L3MOD_SET_RTDI_1062)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Not in the right state!\n");

		//发送完成终结函数到后台：实际上将根据操作的结果来定，结果写在ExeRtn中吧，省得再多费一个新的域
		if (fsm_llczhb_send_to_cloud_ctrl_execute_operation_result_9012(rcv.ul2Cloud.ExeRtn) == FAILURE)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Send L2frame to cloud error!\n");
		gTaskLlczhbContext.llcState = LLCZHB_STATE_CTRL_DEACTIVE;
		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_SET_WARN_TIME_1001:
		if (gTaskLlczhbContext.llcState != LLCZHB_STATE_CTRL_WFFB_FROM_L3MOD_SET_WARN_TIME_1001)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Not in the right state!\n");

		//发送完成终结函数到后台：实际上将根据操作的结果来定，结果写在ExeRtn中吧，省得再多费一个新的域
		if (fsm_llczhb_send_to_cloud_ctrl_execute_operation_result_9012(rcv.ul2Cloud.ExeRtn) == FAILURE)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Send L2frame to cloud error!\n");
		gTaskLlczhbContext.llcState = LLCZHB_STATE_CTRL_DEACTIVE;
		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_SET_INST_SAMPLE_3012:
		if (gTaskLlczhbContext.llcState != LLCZHB_STATE_CTRL_WFFB_FROM_L3MOD_SET_INSTANCE_SAMPLE_3012)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Not in the right state!\n");

		//发送完成终结函数到后台：实际上将根据操作的结果来定，结果写在ExeRtn中吧，省得再多费一个新的域
		if (fsm_llczhb_send_to_cloud_ctrl_execute_operation_result_9012(rcv.ul2Cloud.ExeRtn) == FAILURE)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Send L2frame to cloud error!\n");
		gTaskLlczhbContext.llcState = LLCZHB_STATE_CTRL_DEACTIVE;
		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_SET_SAMPLE_CTIME_3014:
		if (gTaskLlczhbContext.llcState != LLCZHB_STATE_CTRL_WFFB_FROM_L3MOD_SET_SAMPLE_CYCLE_3014)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Not in the right state!\n");

		//发送完成终结函数到后台：实际上将根据操作的结果来定，结果写在ExeRtn中吧，省得再多费一个新的域
		if (fsm_llczhb_send_to_cloud_ctrl_execute_operation_result_9012(rcv.ul2Cloud.ExeRtn) == FAILURE)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Send L2frame to cloud error!\n");
		gTaskLlczhbContext.llcState = LLCZHB_STATE_CTRL_DEACTIVE;
		break;

	default:
		HCU_ERROR_PRINT_LLCZHB("LLCZHB: Error action received!\n");
		break;
	}


	return SUCCESS;
}


//带数据回报给后台的，都认为是数据报告格式
OPSTAT fsm_llczhb_l3mod_llczhb_data_report(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret=0;
	msg_struct_l3mod_llczhb_data_report_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_l3mod_llczhb_data_report_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_l3mod_llczhb_data_report_t)))
		HCU_ERROR_PRINT_LLCZHB("LLCZHB: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);

	switch (rcv.actionId){
	case HCU_SYSMSG_ZHBHJT_ACTION_GET_POL_RTD_2011:
		if (gTaskLlczhbContext.llcState != LLCZHB_STATE_CTRL_WFFB_FROM_L3MOD_GET_RTD_2011)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Not in the right state!\n");
		//发送数据到后台
		gTaskLlczhbContext.llcSn.ulTx.nbrOfCRtd = rcv.ul2Cloud.nbrOfCRtd;
		memcpy(&gTaskLlczhbContext.llcSn.ulTx.rtd, &rcv.ul2Cloud.rtd, sizeof(msgie_struct_zhbhjt_frame_data_pol_rtd_t)*HCU_SYSMSG_ZHBHJT_POLID_NBR_MAX);
		gTaskLlczhbContext.llcSn.ulTx.DataTime = rcv.ul2Cloud.DataTime;
		if (fsm_llczhb_send_to_cloud_data_data_get_pol_rtd_2011() == FAILURE)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Send L2frame to cloud error!\n");
		gTaskLlczhbContext.llcState = LLCZHB_STATE_CTRL_WFFB_FROM_CLOUD_GET_RTD_2011;
		gTaskLlczhbContext.resndCnt = gTaskLlczhbContext.envSd.resndTimesSet;
		//启动定时器
		if (hcu_timer_start(TASK_ID_LLCZHB, TIMER_ID_1S_LLCZHB_LINK_CTRL, gTaskLlczhbContext.envSd.overTimeDur, TIMER_TYPE_ONE_TIME, TIMER_RESOLUTION_1S) == FAILURE)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Start timer error!\n");
		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_STOP_RTD_2012:
		HCU_ERROR_PRINT_LLCZHB("LLCZHB: Error action received!\n");
		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_GET_EQU_RUN_2021:
		if (gTaskLlczhbContext.llcState != LLCZHB_STATE_CTRL_WFFB_FROM_L3MOD_GET_EQU_RUN_2021)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Not in the right state!\n");
		//发送数据到后台
		gTaskLlczhbContext.llcSn.ulTx.nbrOfRS = rcv.ul2Cloud.nbrOfRS;
		memcpy(&(gTaskLlczhbContext.llcSn.ulTx.RS), &(rcv.ul2Cloud.RS), sizeof(msgie_struct_zhbhjt_frame_data_RS_value_t)*HCU_SYSMSG_ZHBHJT_POLID_NBR_MAX);
		gTaskLlczhbContext.llcSn.ulTx.DataTime = rcv.ul2Cloud.DataTime;
		if (fsm_llczhb_send_to_cloud_data_get_equ_run_2021() == FAILURE)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Send L2frame to cloud error!\n");
		gTaskLlczhbContext.llcState = LLCZHB_STATE_CTRL_WFFB_FROM_CLOUD_GET_EQU_RUN_2021;
		gTaskLlczhbContext.resndCnt = gTaskLlczhbContext.envSd.resndTimesSet;
		//启动定时器
		if (hcu_timer_start(TASK_ID_LLCZHB, TIMER_ID_1S_LLCZHB_LINK_CTRL, gTaskLlczhbContext.envSd.overTimeDur, TIMER_TYPE_ONE_TIME, TIMER_RESOLUTION_1S) == FAILURE)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Start timer error!\n");
		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_STOP_EQU_RUN_2022:
		HCU_ERROR_PRINT_LLCZHB("LLCZHB: Error action received!\n");
		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_GET_POL_MIN_RPT_2051:
		if (gTaskLlczhbContext.llcState != LLCZHB_STATE_CTRL_WFFB_FROM_L3MOD_GET_POL_MIN_2051)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Not in the right state!\n");
		//发送数据到后台
		gTaskLlczhbContext.llcSn.ulTx.nbrOfCMinRpt = rcv.ul2Cloud.nbrOfCMinRpt;
		memcpy(&(gTaskLlczhbContext.llcSn.ulTx.min), &(rcv.ul2Cloud.min), sizeof(msgie_struct_zhbhjt_frame_data_pol_min_hour_t)*HCU_SYSMSG_ZHBHJT_POLID_NBR_MAX);
		gTaskLlczhbContext.llcSn.ulTx.DataTime = rcv.ul2Cloud.DataTime;
		if (fsm_llczhb_send_to_cloud_data_get_pol_min_rpt_2051() == FAILURE)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Send L2frame to cloud error!\n");

		//发送完成终结函数到后台
		if (fsm_llczhb_send_to_cloud_ctrl_execute_operation_result_9012(ZHBHJT_IE_uni_EXETRN_EXE_SUCCESS) == FAILURE)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Send L2frame to cloud error!\n");
		gTaskLlczhbContext.llcState = LLCZHB_STATE_CTRL_DEACTIVE;
		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_GET_POL_DAY_RPT_2031:
		if (gTaskLlczhbContext.llcState != LLCZHB_STATE_CTRL_WFFB_FROM_L3MOD_GET_POL_DAY_2031)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Not in the right state!\n");
		//发送数据到后台
		gTaskLlczhbContext.llcSn.ulTx.nbrOfCMinRpt = rcv.ul2Cloud.nbrOfCMinRpt;
		memcpy(&(gTaskLlczhbContext.llcSn.ulTx.min), &(rcv.ul2Cloud.min), sizeof(msgie_struct_zhbhjt_frame_data_pol_min_hour_t)*HCU_SYSMSG_ZHBHJT_POLID_NBR_MAX);
		gTaskLlczhbContext.llcSn.ulTx.DataTime = rcv.ul2Cloud.DataTime;
		if (fsm_llczhb_send_to_cloud_data_get_pol_his_day_rpt_2031() == FAILURE)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Send L2frame to cloud error!\n");

		//发送完成终结函数到后台
		if (fsm_llczhb_send_to_cloud_ctrl_execute_operation_result_9012(ZHBHJT_IE_uni_EXETRN_EXE_SUCCESS) == FAILURE)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Send L2frame to cloud error!\n");
		gTaskLlczhbContext.llcState = LLCZHB_STATE_CTRL_DEACTIVE;
		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_GET_POL_RT_2041:
		if (gTaskLlczhbContext.llcState != LLCZHB_STATE_CTRL_WFFB_FROM_L3MOD_GET_POL_TODAY_RT_2041)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Not in the right state!\n");
		//发送数据到后台
		gTaskLlczhbContext.llcSn.ulTx.nbrOfRT = rcv.ul2Cloud.nbrOfRT;
		memcpy(&(gTaskLlczhbContext.llcSn.ulTx.RT), &(rcv.ul2Cloud.RT), sizeof(msgie_struct_zhbhjt_frame_data_RT_value_t)*HCU_SYSMSG_ZHBHJT_POLID_NBR_MAX);
		gTaskLlczhbContext.llcSn.ulTx.DataTime = rcv.ul2Cloud.DataTime;
		if (fsm_llczhb_send_to_cloud_data_get_pol_today_rt_2041() == FAILURE)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Send L2frame to cloud error!\n");

		//发送完成终结函数到后台
		if (fsm_llczhb_send_to_cloud_ctrl_execute_operation_result_9012(ZHBHJT_IE_uni_EXETRN_EXE_SUCCESS) == FAILURE)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Send L2frame to cloud error!\n");
		gTaskLlczhbContext.llcState = LLCZHB_STATE_CTRL_DEACTIVE;
		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_GET_POL_ALA_2071:
		if (gTaskLlczhbContext.llcState != LLCZHB_STATE_CTRL_WFFB_FROM_L3MOD_GET_POL_ALA_2071)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Not in the right state!\n");
		//发送数据到后台
		gTaskLlczhbContext.llcSn.ulTx.Ala = rcv.ul2Cloud.Ala;
		gTaskLlczhbContext.llcSn.ulTx.DataTime = rcv.ul2Cloud.DataTime;
		if (fsm_llczhb_send_to_cloud_data_get_pol_ala_record_2071() == FAILURE)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Send L2frame to cloud error!\n");

		//发送完成终结函数到后台
		if (fsm_llczhb_send_to_cloud_ctrl_execute_operation_result_9012(ZHBHJT_IE_uni_EXETRN_EXE_SUCCESS) == FAILURE)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Send L2frame to cloud error!\n");
		gTaskLlczhbContext.llcState = LLCZHB_STATE_CTRL_DEACTIVE;
		break;

	//超标报警，这个是主动发送的，没有任何之前遗存的动态上下文
	case HCU_SYSMSG_ZHBHJT_ACTION_SND_ALM_EVT_2072:
		//发送数据到后台
		memset(&gTaskLlczhbContext.llcSn, 0, sizeof(gTaskLlczhbContextLinkDynamic_t));
		gTaskLlczhbContext.llcSn.ulTx.Ala = rcv.ul2Cloud.Ala;
		gTaskLlczhbContext.llcSn.ulTx.AlarmTime = rcv.ul2Cloud.AlarmTime;
		gTaskLlczhbContext.llcSn.ulTx.AlarmType = rcv.ul2Cloud.AlarmType;
		if (fsm_llczhb_send_to_cloud_data_report_ala_event_2072() == FAILURE)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Send L2frame to cloud error!\n");
		gTaskLlczhbContext.llcState = LLCZHB_STATE_CTRL_WFFB_FROM_CLOUD_REPORT_ALA_TYPE_2072;

		//启动定时器，等待通知应答
		gTaskLlczhbContext.resndCnt = gTaskLlczhbContext.envSd.resndTimesSet;
		//启动定时器
		if (hcu_timer_start(TASK_ID_LLCZHB, TIMER_ID_1S_LLCZHB_LINK_CTRL, gTaskLlczhbContext.envSd.overTimeDur, TIMER_TYPE_ONE_TIME, TIMER_RESOLUTION_1S) == FAILURE)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Start timer error!\n");

		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_GET_ALMLIM_1021:
		HCU_ERROR_PRINT_LLCZHB("LLCZHB: Error action received!\n");
		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_GET_MNADDR_1031:
		HCU_ERROR_PRINT_LLCZHB("LLCZHB: Error action received!\n");
		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_GET_RPT_TIME_1041:
		if (gTaskLlczhbContext.llcState != LLCZHB_STATE_CTRL_WFFB_FROM_L3MOD_GET_DATA_RPT_TIME_1041)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Not in the right state!\n");
		//发送数据到后台
		gTaskLlczhbContext.llcSn.ulTx.ReportTime = rcv.ul2Cloud.ReportTime;
		if (fsm_llczhb_send_to_cloud_data_get_report_time_1041() == FAILURE)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Send L2frame to cloud error!\n");

		//发送完成终结函数到后台
		if (fsm_llczhb_send_to_cloud_ctrl_execute_operation_result_9012(ZHBHJT_IE_uni_EXETRN_EXE_SUCCESS) == FAILURE)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Send L2frame to cloud error!\n");
		gTaskLlczhbContext.llcState = LLCZHB_STATE_CTRL_DEACTIVE;
		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_GET_RTDI_1061:
		if (gTaskLlczhbContext.llcState != LLCZHB_STATE_CTRL_WFFB_FROM_L3MOD_GET_RTDI_1061)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Not in the right state!\n");
		//发送数据到后台
		gTaskLlczhbContext.llcSn.ulTx.RtdInterval = rcv.ul2Cloud.RtdInterval;
		if (fsm_llczhb_send_to_cloud_data_get_rtdi_interval_1061() == FAILURE)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Send L2frame to cloud error!\n");

		//发送完成终结函数到后台
		if (fsm_llczhb_send_to_cloud_ctrl_execute_operation_result_9012(ZHBHJT_IE_uni_EXETRN_EXE_SUCCESS) == FAILURE)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Send L2frame to cloud error!\n");
		gTaskLlczhbContext.llcState = LLCZHB_STATE_CTRL_DEACTIVE;
		break;

	default:
		HCU_ERROR_PRINT_LLCZHB("LLCZHB: Error action received!\n");
		break;
	}
	return SUCCESS;
}

//发送L2FRAME给后台
OPSTAT fsm_llczhb_send_to_cloud_ctrl_req_answer_directly_in_l2llc_9011(UINT8 qnRtn)
{
	msg_struct_llczhb_cloudvela_frame_resp_t snd;
	memset(&snd, 0, sizeof(msg_struct_llczhb_cloudvela_frame_resp_t));

	snd.head.st = ZHBHJT_IE_uni_STcode_system_interaction;
	snd.head.cn = ZHBHJT_IE_uni_CNcode_cmd_answer_resp_9011;
	strncpy(snd.head.pw, gTaskLlczhbContext.envSd.pswd, strlen(gTaskLlczhbContext.envSd.pswd));
	snd.head.ansFlag = 0;
	//表示不需要分段
	snd.head.pnum = 0xFFFF;
	snd.head.pno = 0xFFFF;
	snd.cfmQn = gTaskLlczhbContext.frameHead.qn;
	snd.ul2Cloud.QnRtn = qnRtn;
	snd.length = sizeof(msg_struct_llczhb_cloudvela_frame_resp_t);
	if (hcu_message_send(MSG_ID_LLCZHB_CLOUDVELA_FRAME_RESP, TASK_ID_CLOUDVELA, TASK_ID_LLCZHB, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_LLCZHB("LLCZHB: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_LLCZHB].taskName, zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName);
	return SUCCESS;
}

//通用API，将执行结果函数发送给后台
OPSTAT fsm_llczhb_send_to_cloud_ctrl_execute_operation_result_9012(UINT8 exeRtn)
{
	msg_struct_llczhb_cloudvela_frame_resp_t snd;
	memset(&snd, 0, sizeof(msg_struct_llczhb_cloudvela_frame_resp_t));

	snd.head.st = ZHBHJT_IE_uni_STcode_system_interaction;
	snd.head.cn = ZHBHJT_IE_uni_CNcode_cmd_operation_result_resp_9012;
	strncpy(snd.head.pw, gTaskLlczhbContext.envSd.pswd, strlen(gTaskLlczhbContext.envSd.pswd));
	//表示不需要回复Flag标识
	snd.head.ansFlag = 0xFF;
	//表示不需要分段
	snd.head.pnum = 0xFFFF;
	snd.head.pno = 0xFFFF;
	snd.cfmQn = gTaskLlczhbContext.frameHead.qn;
	snd.ul2Cloud.ExeRtn = exeRtn;
	snd.length = sizeof(msg_struct_llczhb_cloudvela_frame_resp_t);
	if (hcu_message_send(MSG_ID_LLCZHB_CLOUDVELA_FRAME_RESP, TASK_ID_CLOUDVELA, TASK_ID_LLCZHB, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_LLCZHB("LLCZHB: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_LLCZHB].taskName, zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName);
	return SUCCESS;
}

//通知应答
OPSTAT fsm_llczhb_send_to_cloud_ctrl_notify_answer_9013(void)
{
	msg_struct_llczhb_cloudvela_frame_resp_t snd;
	memset(&snd, 0, sizeof(msg_struct_llczhb_cloudvela_frame_resp_t));

	snd.head.st = ZHBHJT_IE_uni_STcode_system_interaction;
	snd.head.cn = ZHBHJT_IE_uni_CNcode_cmd_notification_resp_9013;
	strncpy(snd.head.pw, gTaskLlczhbContext.envSd.pswd, strlen(gTaskLlczhbContext.envSd.pswd));
	//表示不需要回复Flag标识
	snd.head.ansFlag = 0xFF;
	//表示不需要分段
	snd.head.pnum = 0xFFFF;
	snd.head.pno = 0xFFFF;
	snd.cfmQn = gTaskLlczhbContext.frameHead.qn;
	snd.length = sizeof(msg_struct_llczhb_cloudvela_frame_resp_t);
	if (hcu_message_send(MSG_ID_LLCZHB_CLOUDVELA_FRAME_RESP, TASK_ID_CLOUDVELA, TASK_ID_LLCZHB, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_LLCZHB("LLCZHB: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_LLCZHB].taskName, zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName);
	return SUCCESS;
}

//将新时间发给后台
OPSTAT fsm_llczhb_send_to_cloud_data_get_field_time_1011(void)
{
	msg_struct_llczhb_cloudvela_frame_resp_t snd;
	memset(&snd, 0, sizeof(msg_struct_llczhb_cloudvela_frame_resp_t));

	snd.head.st = ZHBHJT_IE_uni_STcode_system_interaction;
	snd.head.cn = ZHBHJT_IE_uni_CNcode_par_get_field_time_resp_1011;
	strncpy(snd.head.pw, gTaskLlczhbContext.envSd.pswd, strlen(gTaskLlczhbContext.envSd.pswd));
	//表示不需要回复Flag标识
	snd.head.ansFlag = 0xFF;
	//表示不需要分段
	snd.head.pnum = 0xFFFF;
	snd.head.pno = 0xFFFF;
	snd.cfmQn = gTaskLlczhbContext.frameHead.qn;
	snd.ul2Cloud.SystemTime = time(0);
	snd.length = sizeof(msg_struct_llczhb_cloudvela_frame_resp_t);
	if (hcu_message_send(MSG_ID_LLCZHB_CLOUDVELA_FRAME_RESP, TASK_ID_CLOUDVELA, TASK_ID_LLCZHB, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_LLCZHB("LLCZHB: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_LLCZHB].taskName, zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName);
	return SUCCESS;
}

//提取上位机地址并发送到后台
OPSTAT fsm_llczhb_send_to_cloud_data_get_field_addr_1031(void)
{
	msg_struct_llczhb_cloudvela_frame_resp_t snd;
	memset(&snd, 0, sizeof(msg_struct_llczhb_cloudvela_frame_resp_t));

	snd.head.st = ZHBHJT_IE_uni_STcode_system_interaction;
	snd.head.cn = ZHBHJT_IE_uni_CNcode_par_get_field_addr_resp_1031;
	strncpy(snd.head.pw, gTaskLlczhbContext.envSd.pswd, strlen(gTaskLlczhbContext.envSd.pswd));
	//表示不需要回复Flag标识
	snd.head.ansFlag = 0xFF;
	//表示不需要分段
	snd.head.pnum = 0xFFFF;
	snd.head.pno = 0xFFFF;
	snd.cfmQn = gTaskLlczhbContext.frameHead.qn;
	snd.ul2Cloud.AlarmTarget = gTaskLlczhbContext.envSd.AlarmTarget;
	snd.length = sizeof(msg_struct_llczhb_cloudvela_frame_resp_t);
	if (hcu_message_send(MSG_ID_LLCZHB_CLOUDVELA_FRAME_RESP, TASK_ID_CLOUDVELA, TASK_ID_LLCZHB, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_LLCZHB("LLCZHB: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_LLCZHB].taskName, zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName);
	return SUCCESS;
}

//发送数据到后台GET_POL_RTD
OPSTAT fsm_llczhb_send_to_cloud_data_data_get_pol_rtd_2011(void)
{
	msg_struct_llczhb_cloudvela_frame_resp_t snd;
	memset(&snd, 0, sizeof(msg_struct_llczhb_cloudvela_frame_resp_t));

	snd.head.st = ZHBHJT_IE_uni_STcode_system_interaction;
	snd.head.cn = ZHBHJT_IE_uni_CNcode_dat_get_rt_data_resp_2011;
	strncpy(snd.head.pw, gTaskLlczhbContext.envSd.pswd, strlen(gTaskLlczhbContext.envSd.pswd));
	//表示不需要回复Flag标识
	snd.head.ansFlag = 0xFF;
	//表示不需要分段
	snd.head.pnum = 0xFFFF;
	snd.head.pno = 0xFFFF;
	snd.ul2Cloud.DataTime = gTaskLlczhbContext.llcSn.ulTx.DataTime;
	snd.ul2Cloud.nbrOfCRtd = gTaskLlczhbContext.llcSn.ulTx.nbrOfCRtd;
	memcpy(&(snd.ul2Cloud.rtd), &(gTaskLlczhbContext.llcSn.ulTx.rtd), sizeof(msgie_struct_zhbhjt_frame_data_pol_rtd_t)*HCU_SYSMSG_ZHBHJT_POLID_NBR_MAX);
	snd.length = sizeof(msg_struct_llczhb_cloudvela_frame_resp_t);
	if (hcu_message_send(MSG_ID_LLCZHB_CLOUDVELA_FRAME_RESP, TASK_ID_CLOUDVELA, TASK_ID_LLCZHB, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_LLCZHB("LLCZHB: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_LLCZHB].taskName, zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName);
	return SUCCESS;
}

//发送数据到后台GET_EAU_RUN
OPSTAT fsm_llczhb_send_to_cloud_data_get_equ_run_2021(void)
{
	msg_struct_llczhb_cloudvela_frame_resp_t snd;
	memset(&snd, 0, sizeof(msg_struct_llczhb_cloudvela_frame_resp_t));

	snd.head.st = HCU_SYSCFG_CLOUD_SVR_DEFAULT_ST_CODE;
	snd.head.cn = ZHBHJT_IE_uni_CNcode_dat_get_equ_status_resp_2021;
	strncpy(snd.head.pw, gTaskLlczhbContext.envSd.pswd, strlen(gTaskLlczhbContext.envSd.pswd));
	//表示不需要回复Flag标识
	snd.head.ansFlag = 0xFF;
	//表示不需要分段
	snd.head.pnum = 0xFFFF;
	snd.head.pno = 0xFFFF;
	snd.ul2Cloud.DataTime = gTaskLlczhbContext.llcSn.ulTx.DataTime;
	snd.ul2Cloud.nbrOfRS = gTaskLlczhbContext.llcSn.ulTx.nbrOfRS;
	memcpy(&(snd.ul2Cloud.RS), &(gTaskLlczhbContext.llcSn.ulTx.RS), sizeof(msgie_struct_zhbhjt_frame_data_RS_value_t)*HCU_SYSMSG_ZHBHJT_POLID_NBR_MAX);
	snd.length = sizeof(msg_struct_llczhb_cloudvela_frame_resp_t);
	if (hcu_message_send(MSG_ID_LLCZHB_CLOUDVELA_FRAME_RESP, TASK_ID_CLOUDVELA, TASK_ID_LLCZHB, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_LLCZHB("LLCZHB: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_LLCZHB].taskName, zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName);
	return SUCCESS;
}

//发送数据到后台GET_POL_MIN_REPORT
OPSTAT fsm_llczhb_send_to_cloud_data_get_pol_min_rpt_2051(void)
{
	msg_struct_llczhb_cloudvela_frame_resp_t snd;
	memset(&snd, 0, sizeof(msg_struct_llczhb_cloudvela_frame_resp_t));

	snd.head.st = HCU_SYSCFG_CLOUD_SVR_DEFAULT_ST_CODE;
	snd.head.cn = ZHBHJT_IE_uni_CNcode_dat_get_pollution_min_data_resp_2051;
	strncpy(snd.head.pw, gTaskLlczhbContext.envSd.pswd, strlen(gTaskLlczhbContext.envSd.pswd));
	//表示不需要回复Flag标识
	snd.head.ansFlag = 0xFF;
	//表示不需要分段
	snd.head.pnum = 0xFFFF;
	snd.head.pno = 0xFFFF;
	snd.ul2Cloud.DataTime = gTaskLlczhbContext.llcSn.ulTx.DataTime;
	snd.ul2Cloud.nbrOfCMinRpt = gTaskLlczhbContext.llcSn.ulTx.nbrOfCMinRpt;
	memcpy(&(snd.ul2Cloud.min), &(gTaskLlczhbContext.llcSn.ulTx.min), sizeof(msgie_struct_zhbhjt_frame_data_pol_min_hour_t)*HCU_SYSMSG_ZHBHJT_POLID_NBR_MAX);
	snd.length = sizeof(msg_struct_llczhb_cloudvela_frame_resp_t);
	if (hcu_message_send(MSG_ID_LLCZHB_CLOUDVELA_FRAME_RESP, TASK_ID_CLOUDVELA, TASK_ID_LLCZHB, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_LLCZHB("LLCZHB: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_LLCZHB].taskName, zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName);
	return SUCCESS;
}

//发送数据到后台GET_POL_DAY_REPORT
OPSTAT fsm_llczhb_send_to_cloud_data_get_pol_his_day_rpt_2031(void)
{
	msg_struct_llczhb_cloudvela_frame_resp_t snd;
	memset(&snd, 0, sizeof(msg_struct_llczhb_cloudvela_frame_resp_t));

	snd.head.st = HCU_SYSCFG_CLOUD_SVR_DEFAULT_ST_CODE;
	snd.head.cn = ZHBHJT_IE_uni_CNcode_dat_get_pollution_his_data_resp_2031;
	strncpy(snd.head.pw, gTaskLlczhbContext.envSd.pswd, strlen(gTaskLlczhbContext.envSd.pswd));
	//表示不需要回复Flag标识
	snd.head.ansFlag = 0xFF;
	//表示不需要分段
	snd.head.pnum = 0xFFFF;
	snd.head.pno = 0xFFFF;
	snd.ul2Cloud.DataTime = gTaskLlczhbContext.llcSn.ulTx.DataTime;
	snd.ul2Cloud.nbrOfCMinRpt = gTaskLlczhbContext.llcSn.ulTx.nbrOfCMinRpt;
	memcpy(&(snd.ul2Cloud.min), &(gTaskLlczhbContext.llcSn.ulTx.min), sizeof(msgie_struct_zhbhjt_frame_data_pol_min_hour_t)*HCU_SYSMSG_ZHBHJT_POLID_NBR_MAX);
	snd.length = sizeof(msg_struct_llczhb_cloudvela_frame_resp_t);
	if (hcu_message_send(MSG_ID_LLCZHB_CLOUDVELA_FRAME_RESP, TASK_ID_CLOUDVELA, TASK_ID_LLCZHB, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_LLCZHB("LLCZHB: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_LLCZHB].taskName, zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName);
	return SUCCESS;
}

//发送数据到后台GET_POL_TODAY_RT
OPSTAT fsm_llczhb_send_to_cloud_data_get_pol_today_rt_2041(void)
{
	msg_struct_llczhb_cloudvela_frame_resp_t snd;
	memset(&snd, 0, sizeof(msg_struct_llczhb_cloudvela_frame_resp_t));

	snd.head.st = HCU_SYSCFG_CLOUD_SVR_DEFAULT_ST_CODE;
	snd.head.cn = ZHBHJT_IE_uni_CNcode_dat_get_today_his_data_resp_2041;
	strncpy(snd.head.pw, gTaskLlczhbContext.envSd.pswd, strlen(gTaskLlczhbContext.envSd.pswd));
	//表示不需要回复Flag标识
	snd.head.ansFlag = 0xFF;
	//表示不需要分段
	snd.head.pnum = 0xFFFF;
	snd.head.pno = 0xFFFF;
	snd.ul2Cloud.DataTime = gTaskLlczhbContext.llcSn.ulTx.DataTime;
	snd.ul2Cloud.nbrOfRT = gTaskLlczhbContext.llcSn.ulTx.nbrOfRT;
	memcpy(&(snd.ul2Cloud.RT), &(gTaskLlczhbContext.llcSn.ulTx.RT), sizeof(msgie_struct_zhbhjt_frame_data_RT_value_t)*HCU_SYSMSG_ZHBHJT_POLID_NBR_MAX);
	snd.length = sizeof(msg_struct_llczhb_cloudvela_frame_resp_t);
	if (hcu_message_send(MSG_ID_LLCZHB_CLOUDVELA_FRAME_RESP, TASK_ID_CLOUDVELA, TASK_ID_LLCZHB, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_LLCZHB("LLCZHB: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_LLCZHB].taskName, zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName);
	return SUCCESS;
}

//发送数据到后台GET_POL_ALA_RECORD
OPSTAT fsm_llczhb_send_to_cloud_data_get_pol_ala_record_2071(void)
{
	msg_struct_llczhb_cloudvela_frame_resp_t snd;
	memset(&snd, 0, sizeof(msg_struct_llczhb_cloudvela_frame_resp_t));

	snd.head.st = HCU_SYSCFG_CLOUD_SVR_DEFAULT_ST_CODE;
	snd.head.cn = ZHBHJT_IE_uni_CNcode_dat_get_pollution_alarm_record_resp_2071;
	strncpy(snd.head.pw, gTaskLlczhbContext.envSd.pswd, strlen(gTaskLlczhbContext.envSd.pswd));
	//表示不需要回复Flag标识
	snd.head.ansFlag = 0xFF;
	//表示不需要分段
	snd.head.pnum = 0xFFFF;
	snd.head.pno = 0xFFFF;
	snd.ul2Cloud.DataTime = gTaskLlczhbContext.llcSn.ulTx.DataTime;
	snd.ul2Cloud.Ala = gTaskLlczhbContext.llcSn.ulTx.Ala;
	snd.length = sizeof(msg_struct_llczhb_cloudvela_frame_resp_t);
	if (hcu_message_send(MSG_ID_LLCZHB_CLOUDVELA_FRAME_RESP, TASK_ID_CLOUDVELA, TASK_ID_LLCZHB, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_LLCZHB("LLCZHB: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_LLCZHB].taskName, zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName);
	return SUCCESS;
}

//发送数据到后台REPORT_ALA_EVENT
OPSTAT fsm_llczhb_send_to_cloud_data_report_ala_event_2072(void)
{
	msg_struct_llczhb_cloudvela_frame_resp_t snd;
	memset(&snd, 0, sizeof(msg_struct_llczhb_cloudvela_frame_resp_t));

	//初次发送出去，需要带QN信息
	snd.head.st = HCU_SYSCFG_CLOUD_SVR_DEFAULT_ST_CODE;
	snd.head.cn = ZHBHJT_IE_uni_CNcode_dat_get_pollution_alarm_event_resp_2072;
	snd.head.qn = time(0) * 1000;
	gTaskLlczhbContext.frameHead.qn = snd.head.qn; //存下来用于比对。当收到消息时，这个域是空的，但会包含在消息体中
	strncpy(snd.head.pw, gTaskLlczhbContext.envSd.pswd, strlen(gTaskLlczhbContext.envSd.pswd));
	//表示不需要回复Flag标识
	snd.head.ansFlag = 0xFF;
	//表示不需要分段
	snd.head.pnum = 0xFFFF;
	snd.head.pno = 0xFFFF;
	snd.ul2Cloud.Ala = gTaskLlczhbContext.llcSn.ulTx.Ala;
	snd.ul2Cloud.AlarmTime = gTaskLlczhbContext.llcSn.ulTx.AlarmTime;
	snd.ul2Cloud.AlarmType = gTaskLlczhbContext.llcSn.ulTx.AlarmType;
	snd.length = sizeof(msg_struct_llczhb_cloudvela_frame_resp_t);
	if (hcu_message_send(MSG_ID_LLCZHB_CLOUDVELA_FRAME_RESP, TASK_ID_CLOUDVELA, TASK_ID_LLCZHB, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_LLCZHB("LLCZHB: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_LLCZHB].taskName, zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName);
	return SUCCESS;
}

//发送数据到后台SET_UP_LIMITATION
OPSTAT fsm_llczhb_send_to_cloud_data_get_alarm_limitation_1021(void)
{
	msg_struct_llczhb_cloudvela_frame_resp_t snd;
	memset(&snd, 0, sizeof(msg_struct_llczhb_cloudvela_frame_resp_t));

	//初次发送出去，需要带QN信息
	snd.head.st = HCU_SYSCFG_CLOUD_SVR_DEFAULT_ST_CODE;
	snd.head.cn = ZHBHJT_IE_uni_CNcode_par_get_pollution_limitaion_resp_1021;
	strncpy(snd.head.pw, gTaskLlczhbContext.envSd.pswd, strlen(gTaskLlczhbContext.envSd.pswd));
	//表示不需要回复Flag标识
	snd.head.ansFlag = 0xFF;
	//表示不需要分段
	snd.head.pnum = 0xFFFF;
	snd.head.pno = 0xFFFF;
	snd.cfmQn = gTaskLlczhbContext.frameHead.qn;
	snd.ul2Cloud.nbrOfAlmLim = gTaskLlczhbContext.llcSn.ulTx.nbrOfAlmLim;
	memcpy(&(snd.ul2Cloud.limitation), &(gTaskLlczhbContext.llcSn.ulTx.limitation), sizeof(msgie_struct_zhbhjt_frame_data_low_upvalue_t)*HCU_SYSMSG_ZHBHJT_POLID_NBR_MAX);
	snd.length = sizeof(msg_struct_llczhb_cloudvela_frame_resp_t);
	if (hcu_message_send(MSG_ID_LLCZHB_CLOUDVELA_FRAME_RESP, TASK_ID_CLOUDVELA, TASK_ID_LLCZHB, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_LLCZHB("LLCZHB: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_LLCZHB].taskName, zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName);
	return SUCCESS;
}


//发送数据到后台GET_REPORT_TIME
OPSTAT fsm_llczhb_send_to_cloud_data_get_report_time_1041(void)
{
	msg_struct_llczhb_cloudvela_frame_resp_t snd;
	memset(&snd, 0, sizeof(msg_struct_llczhb_cloudvela_frame_resp_t));

	//初次发送出去，需要带QN信息
	snd.head.st = HCU_SYSCFG_CLOUD_SVR_DEFAULT_ST_CODE;
	snd.head.cn = ZHBHJT_IE_uni_CNcode_par_get_data_report_time_resp_1041;
	strncpy(snd.head.pw, gTaskLlczhbContext.envSd.pswd, strlen(gTaskLlczhbContext.envSd.pswd));
	//表示不需要回复Flag标识
	snd.head.ansFlag = 0xFF;
	//表示不需要分段
	snd.head.pnum = 0xFFFF;
	snd.head.pno = 0xFFFF;
	snd.cfmQn = gTaskLlczhbContext.frameHead.qn;
	snd.ul2Cloud.ReportTime = gTaskLlczhbContext.llcSn.ulTx.ReportTime;
	snd.length = sizeof(msg_struct_llczhb_cloudvela_frame_resp_t);
	if (hcu_message_send(MSG_ID_LLCZHB_CLOUDVELA_FRAME_RESP, TASK_ID_CLOUDVELA, TASK_ID_LLCZHB, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_LLCZHB("LLCZHB: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_LLCZHB].taskName, zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName);
	return SUCCESS;
}


//发送数据到后台GET_RTDI_TIME
OPSTAT fsm_llczhb_send_to_cloud_data_get_rtdi_interval_1061(void)
{
	msg_struct_llczhb_cloudvela_frame_resp_t snd;
	memset(&snd, 0, sizeof(msg_struct_llczhb_cloudvela_frame_resp_t));

	//初次发送出去，需要带QN信息
	snd.head.st = HCU_SYSCFG_CLOUD_SVR_DEFAULT_ST_CODE;
	snd.head.cn = ZHBHJT_IE_uni_CNcode_par_get_rtdi_interval_resp_1061;
	strncpy(snd.head.pw, gTaskLlczhbContext.envSd.pswd, strlen(gTaskLlczhbContext.envSd.pswd));
	//表示不需要回复Flag标识
	snd.head.ansFlag = 0xFF;
	//表示不需要分段
	snd.head.pnum = 0xFFFF;
	snd.head.pno = 0xFFFF;
	snd.cfmQn = gTaskLlczhbContext.frameHead.qn;
	snd.ul2Cloud.RtdInterval = gTaskLlczhbContext.llcSn.ulTx.RtdInterval;
	snd.length = sizeof(msg_struct_llczhb_cloudvela_frame_resp_t);
	if (hcu_message_send(MSG_ID_LLCZHB_CLOUDVELA_FRAME_RESP, TASK_ID_CLOUDVELA, TASK_ID_LLCZHB, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_LLCZHB("LLCZHB: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_LLCZHB].taskName, zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName);
	return SUCCESS;
}


//链路超时重发的控制过程
OPSTAT func_llczhb_time_out_link_control_process(void)
{
	if (gTaskLlczhbContext.llcSn.resndCnt <=0) return SUCCESS;

	gTaskLlczhbContext.llcSn.resndCnt--;
	if ((gTaskLlczhbContext.llcSn.resndCnt >0) && (gTaskLlczhbContext.llcState == LLCZHB_STATE_CTRL_WFFB_FROM_CLOUD_GET_RTD_2011)){
		if (fsm_llczhb_send_to_cloud_data_data_get_pol_rtd_2011() == FAILURE)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Send L2frame to cloud error!\n");
		if (hcu_timer_start(TASK_ID_LLCZHB, TIMER_ID_1S_LLCZHB_LINK_CTRL, gTaskLlczhbContext.envSd.overTimeDur, TIMER_TYPE_ONE_TIME, TIMER_RESOLUTION_1S) == FAILURE)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Start timer error!\n");
	}

	else if ((gTaskLlczhbContext.llcSn.resndCnt >0) && (gTaskLlczhbContext.llcState == LLCZHB_STATE_CTRL_WFFB_FROM_CLOUD_GET_EQU_RUN_2021)){
		if (fsm_llczhb_send_to_cloud_data_get_equ_run_2021() == FAILURE)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Send L2frame to cloud error!\n");
		if (hcu_timer_start(TASK_ID_LLCZHB, TIMER_ID_1S_LLCZHB_LINK_CTRL, gTaskLlczhbContext.envSd.overTimeDur, TIMER_TYPE_ONE_TIME, TIMER_RESOLUTION_1S) == FAILURE)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Start timer error!\n");
	}

	else if ((gTaskLlczhbContext.llcSn.resndCnt >0) && (gTaskLlczhbContext.llcState == LLCZHB_STATE_CTRL_WFFB_FROM_CLOUD_REPORT_ALA_TYPE_2072)){
		if (fsm_llczhb_send_to_cloud_data_report_ala_event_2072() == FAILURE)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Send L2frame to cloud error!\n");
		if (hcu_timer_start(TASK_ID_LLCZHB, TIMER_ID_1S_LLCZHB_LINK_CTRL, gTaskLlczhbContext.envSd.overTimeDur, TIMER_TYPE_ONE_TIME, TIMER_RESOLUTION_1S) == FAILURE)
			HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Start timer error!\n");
	}

	else{
		HCU_ERROR_PRINT_LLCZHB_RECOVERY("LLCZHB: Wrong counter set or state entered!\n");
	}

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
void func_llczhb_stm_main_recovery_from_fault(void)
{
	//控制其它模块进入正常状态

	//状态转移到SLEEP状态
	FsmSetState(TASK_ID_LLCZHB, FSM_STATE_LLCZHB_ACTIVED);
	memset(&gTaskLlczhbContext.llcSn, 0, sizeof(gTaskLlczhbContextLinkDynamic_t));
	gTaskLlczhbContext.llcState = LLCZHB_STATE_CTRL_DEACTIVE;

	//初始化模块的任务资源
	//初始化定时器：暂时决定不做，除非该模块重新RESTART
	//初始化模块级全局变量：暂时决定不做，除非该模块重新RESTART

	return;
}


