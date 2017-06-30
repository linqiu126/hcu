/*
 * sysswm.c
 *
 *  Created on: 2016年5月10日
 *      Author: hitpony
 */

#include "sysswm.h"

#include "../l0service/timer.h"
#include "../l0service/trace.h"
#include "../l1com/l1comdef.h"
#include "../l2frame/cloudvela.h"


/*
** FSM of the SYSSWM
*/
HcuFsmStateItem_t HcuFsmSysswm[] =
{
    //MessageId                 //State                   		 		//Function
	//启始点，固定定义，不要改动, 使用ENTRY/END，意味者MSGID肯定不可能在某个高位区段中；考虑到所有任务共享MsgId，即使分段，也无法实现
	//完全是为了给任务一个初始化的机会，按照状态转移机制，该函数不具备启动的机会，因为任务初始化后自动到FSM_STATE_IDLE
	//如果没有必要进行初始化，可以设置为NULL
	{MSG_ID_ENTRY,       						FSM_STATE_ENTRY,            			fsm_sysswm_task_entry}, //Starting

	//System level initialization, only controlled by HCU-MAIN
    {MSG_ID_COM_INIT,       					FSM_STATE_IDLE,            				fsm_sysswm_init},
    {MSG_ID_COM_INIT_FEEDBACK,					FSM_STATE_IDLE,            				fsm_com_do_nothing},

	//Task level initialization
    {MSG_ID_COM_INIT,       					FSM_STATE_SYSSWM_INITED,            	fsm_sysswm_init},
    {MSG_ID_COM_INIT_FEEDBACK,					FSM_STATE_SYSSWM_INITED,            	fsm_com_do_nothing},

	//ANY state entry
    {MSG_ID_COM_INIT_FEEDBACK,					FSM_STATE_COMMON,          				fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT,       				FSM_STATE_COMMON,          				fsm_com_heart_beat_rcv},
	{MSG_ID_COM_STOP,       					FSM_STATE_COMMON,          				fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT_FB,       			FSM_STATE_COMMON,          				fsm_com_do_nothing},
    {MSG_ID_COM_RESTART,						FSM_STATE_COMMON,            			fsm_sysswm_restart},
	{MSG_ID_COM_TIME_OUT,       				FSM_STATE_COMMON,          				fsm_sysswm_time_out},

    //Task level functions
	{MSG_ID_CLOUDVELA_SYSSWM_INVENTORY_REQ,     	FSM_STATE_SYSSWM_ACTIVED,                fsm_sysswm_cloudvela_inventory_req},
	{MSG_ID_CLOUDVELA_SYSSWM_INVENTORY_CONFIRM, 	FSM_STATE_SYSSWM_ACTIVED,                fsm_sysswm_cloudvela_inventory_confirm},
	{MSG_ID_CLOUDVELA_SYSSWM_SW_PACKAGE_REQ,        FSM_STATE_SYSSWM_ACTIVED,                fsm_sysswm_cloudvela_sw_package_req},
	{MSG_ID_CLOUDVELA_SYSSWM_SW_PACKAGE_CONFIRM,    FSM_STATE_SYSSWM_ACTIVED,                fsm_sysswm_cloudvela_sw_package_confirm},

    //结束点，固定定义，不要改动
    {MSG_ID_END,            	FSM_STATE_END,             				NULL},  //Ending
};

//Global variables

//Task Global variables
gTaskSysswmContext_t gTaskSysswmContext;

//Main Entry
//Input parameter would be useless, but just for similar structure purpose
OPSTAT fsm_sysswm_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//除了对全局变量进行操作之外，尽量不要做其它操作，因为该函数将被主任务/线程调用，不是本任务/线程调用
	//该API就是给本任务一个提早介入的入口，可以帮着做些测试性操作
	if (FsmSetState(TASK_ID_SYSSWM, FSM_STATE_IDLE) == FAILURE){
		HcuErrorPrint("SYSSWM: Error Set FSM State at fsm_sysswm_task_entry\n");}
	return SUCCESS;
}

OPSTAT fsm_sysswm_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;

	if ((src_id > TASK_ID_MIN) &&(src_id < TASK_ID_MAX)){
		//Send back MSG_ID_COM_INIT_FEEDBACK to SVRCON
		msg_struct_com_init_feedback_t snd0;
		memset(&snd0, 0, sizeof(msg_struct_com_init_feedback_t));
		snd0.length = sizeof(msg_struct_com_init_feedback_t);

		//to avoid all task send out the init fb msg at the same time which lead to msgque get stuck
		hcu_usleep(dest_id*HCU_SYSCFG_DURATION_OF_INIT_FB_WAIT_MAX);

		ret = hcu_message_send(MSG_ID_COM_INIT_FEEDBACK, src_id, TASK_ID_SYSSWM, &snd0, snd0.length);
		if (ret == FAILURE){
			HcuErrorPrint("SYSSWM: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_SYSSWM].taskName, zHcuVmCtrTab.task[src_id].taskName);
			return FAILURE;
		}
	}

	//收到初始化消息后，进入初始化状态
	if (FsmSetState(TASK_ID_SYSSWM, FSM_STATE_SYSSWM_INITED) == FAILURE){
		HcuErrorPrint("SYSSWM: Error Set FSM State!\n");
		return FAILURE;
	}

	//初始化硬件接口
	if (func_sysswm_int_init() == FAILURE){
		HcuErrorPrint("SYSSWM: Error initialize interface!\n");
		return FAILURE;
	}

	//Global Variables
	zHcuSysStaPm.taskRunErrCnt[TASK_ID_SYSSWM] = 0;
	memset(&gTaskSysswmContext, 0, sizeof(gTaskSysswmContext_t));

	//启动周期性定时器
	ret = hcu_timer_start(TASK_ID_SYSSWM, TIMER_ID_1S_SYSSWM_PERIOD_WORKING, \
	zHcuSysEngPar.timer.array[TIMER_ID_1S_SYSSWM_PERIOD_WORKING].dur, TIMER_TYPE_PERIOD, TIMER_RESOLUTION_1S);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_SYSSWM]++;
		HcuErrorPrint("SYSSWM: Error start period timer!\n");
		return FAILURE;
	}

	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_SYSSWM, FSM_STATE_SYSSWM_ACTIVED) == FAILURE)
		HCU_ERROR_PRINT_TASK(TASK_ID_SYSSWM, "SYSSWM: Error Set FSM State!\n");
	HCU_DEBUG_PRINT_FAT("SYSSWM: Enter FSM_STATE_SYSSWM_ACTIVED status, Keeping refresh here!\n");

	//返回
	return SUCCESS;
}

OPSTAT fsm_sysswm_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HcuErrorPrint("SYSSWM: Internal error counter reach DEAD level, SW-RESTART soon!\n");
	zHcuSysStaPm.statisCnt.restartCnt++;
	fsm_sysswm_init(0, 0, NULL, 0);
	return SUCCESS;
}

OPSTAT func_sysswm_int_init(void)
{
	return SUCCESS;
}

OPSTAT fsm_sysswm_time_out(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;

	//Receive message and copy to local variable
	msg_struct_com_time_out_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_com_time_out_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_com_time_out_t)))
		HCU_ERROR_PRINT_TASK(TASK_ID_SYSSWM, "SYSSWM: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);

	//钩子在此处，检查zHcuSysStaPm.taskRunErrCnt[TASK_ID_SYSSWM]是否超限
	if (zHcuSysStaPm.taskRunErrCnt[TASK_ID_SYSSWM] > HCU_RUN_ERROR_LEVEL_3_CRITICAL){
		//减少重复RESTART的概率
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_SYSSWM] = zHcuSysStaPm.taskRunErrCnt[TASK_ID_SYSSWM] - HCU_RUN_ERROR_LEVEL_3_CRITICAL;
		msg_struct_com_restart_t snd0;
		memset(&snd0, 0, sizeof(msg_struct_com_restart_t));
		snd0.length = sizeof(msg_struct_com_restart_t);
		if (hcu_message_send(MSG_ID_COM_RESTART, TASK_ID_SYSSWM, TASK_ID_SYSSWM, &snd0, snd0.length) == FAILURE)
			HCU_ERROR_PRINT_TASK(TASK_ID_SYSSWM, "SYSSWM: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_SYSSWM].taskName, zHcuVmCtrTab.task[TASK_ID_SYSSWM].taskName);
	}

	//PERIOD WORKING TIMER
	else if ((rcv.timeId == TIMER_ID_1S_SYSSWM_PERIOD_WORKING) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		ret = func_sysswm_time_out_period_working_scan();
	}

	return ret;
}

OPSTAT fsm_sysswm_cloudvela_inventory_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret=0;

	//Receive message and copy to local variable
	msg_struct_cloudvela_spspm_inventory_req_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_cloudvela_spspm_inventory_req_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_cloudvela_spspm_inventory_req_t)))
		HCU_ERROR_PRINT_TASK(TASK_ID_SYSSWM, "SYSSWM: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);

	//生成消息并发送给后台
	msg_struct_spspm_cloudvela_inventory_resp_t snd;
	memset(&snd, 0, sizeof(msg_struct_spspm_cloudvela_inventory_resp_t));

	//L2信息
	strncpy(snd.comHead.destUser, zHcuSysEngPar.cloud.svrNameHome, strlen(zHcuSysEngPar.cloud.svrNameHome)<\
		sizeof(snd.comHead.destUser)?strlen(zHcuSysEngPar.cloud.svrNameHome):sizeof(snd.comHead.destUser));
	strncpy(snd.comHead.srcUser, zHcuSysEngPar.hwBurnId.equLable, strlen(zHcuSysEngPar.hwBurnId.equLable)<\
			sizeof(snd.comHead.srcUser)?strlen(zHcuSysEngPar.hwBurnId.equLable):sizeof(snd.comHead.srcUser));
	snd.comHead.timeStamp = time(0);
	snd.comHead.msgType = HUITP_MSG_HUIXML_MSGTYPE_COMMON_ID;
	strcpy(snd.comHead.funcFlag, "0");

	snd.baseResp = HUITP_IEID_UNI_COM_RESPONSE_YES;
	snd.hwType = zHcuSysEngPar.hwBurnId.hwType;
	snd.hwId = zHcuSysEngPar.hwBurnId.hwPemId;
	snd.swRel = zHcuSysEngPar.hwBurnId.swRelId;
	snd.swVer = zHcuSysEngPar.hwBurnId.swVerId;
	snd.upgradeFlag = zHcuSysEngPar.hwBurnId.swUpgradeFlag;
	strcpy(snd.desc, "");
	snd.length = sizeof(msg_struct_spspm_cloudvela_inventory_resp_t);
	if (hcu_message_send(MSG_ID_SYSSWM_CLOUDVELA_INVENTORY_RESP, TASK_ID_CLOUDVELA, TASK_ID_SYSSWM, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_SYSSWM("SYSSWM: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_SYSSWM].taskName, zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName);

	//返回
	return SUCCESS;
}

OPSTAT fsm_sysswm_cloudvela_inventory_confirm(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret=0;

	//Receive message and copy to local variable
	msg_struct_cloudvela_spspm_inventory_confirm_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_cloudvela_spspm_inventory_confirm_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_cloudvela_spspm_inventory_confirm_t)))
		HCU_ERROR_PRINT_TASK(TASK_ID_SYSSWM, "SYSSWM: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);

	//处理接收到的反馈，并进行合理的处理
	if (rcv.baseConfirm != HUITP_IEID_UNI_COM_CONFIRM_POSITIVE){
		HCU_DEBUG_PRINT_IPT("SYSSWM: Receive none-positive inventory confirm message, so no handle.\n");
		return SUCCESS;
	}
	if ((rcv.hwType != zHcuSysEngPar.hwBurnId.hwType) || (rcv.hwId != zHcuSysEngPar.hwBurnId.hwPemId) || (rcv.upgradeFlag != zHcuSysEngPar.hwBurnId.swUpgradeFlag)){
		HCU_DEBUG_PRINT_IPT("SYSSWM: Receive none-compliance hardware info, so no handle.\n");
		return SUCCESS;
	}
	if ((rcv.upgradeFlag != HCU_SYSCFG_HBB_FW_UPGRADE_YES_STABLE) && (rcv.upgradeFlag != HCU_SYSCFG_HBB_FW_UPGRADE_YES_TRAIL) && (rcv.upgradeFlag != HCU_SYSCFG_HBB_FW_UPGRADE_YES_PATCH)){
		HCU_DEBUG_PRINT_IPT("SYSSWM: Receive none-compliance upgrade info, so no handle.\n");
		return SUCCESS;
	}

	//处理软件升级过程：每一次都会重置下载过程
	memset(&(gTaskSysswmContext.bhSw), 0, sizeof(gTaskSysswmContextBody_t));
	gTaskSysswmContext.bhSw.targetSwRel = rcv.swRel;
	gTaskSysswmContext.bhSw.targetSwVer = rcv.swVer;

	//生成消息并发送给后台
	msg_struct_spspm_cloudvela_sw_package_report_t snd;
	memset(&snd, 0, sizeof(msg_struct_spspm_cloudvela_sw_package_report_t));

	//L2信息
	strncpy(snd.comHead.destUser, zHcuSysEngPar.cloud.svrNameHome, strlen(zHcuSysEngPar.cloud.svrNameHome)<\
		sizeof(snd.comHead.destUser)?strlen(zHcuSysEngPar.cloud.svrNameHome):sizeof(snd.comHead.destUser));
	strncpy(snd.comHead.srcUser, zHcuSysEngPar.hwBurnId.equLable, strlen(zHcuSysEngPar.hwBurnId.equLable)<\
			sizeof(snd.comHead.srcUser)?strlen(zHcuSysEngPar.hwBurnId.equLable):sizeof(snd.comHead.srcUser));
	snd.comHead.timeStamp = time(0);
	snd.comHead.msgType = HUITP_MSG_HUIXML_MSGTYPE_COMMON_ID;
	strcpy(snd.comHead.funcFlag, "0");

	snd.baseReport = HUITP_IEID_UNI_COM_REPORT_YES;
	snd.segIndex = 0;
	snd.segTotal = 0;
	snd.segLen = 0;
	snd.length = sizeof(msg_struct_spspm_cloudvela_sw_package_report_t);
	if (hcu_message_send(MSG_ID_SYSSWM_CLOUDVELA_SW_PACKAGE_REPORT, TASK_ID_CLOUDVELA, TASK_ID_SYSSWM, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_SYSSWM("SYSSWM: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_SYSSWM].taskName, zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName);

	//返回
	return SUCCESS;
}

//暂时不处理
OPSTAT fsm_sysswm_cloudvela_sw_package_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret=0;

	//Receive message and copy to local variable
	msg_struct_cloudvela_spspm_sw_package_req_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_cloudvela_spspm_sw_package_req_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_cloudvela_spspm_sw_package_req_t)))
		HCU_ERROR_PRINT_TASK(TASK_ID_SYSSWM, "SYSSWM: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);



	return SUCCESS;
}

//完全依赖于这个过程进行软件体的传输
OPSTAT fsm_sysswm_cloudvela_sw_package_confirm(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret=0;

	//Receive message and copy to local variable
	msg_struct_cloudvela_spspm_sw_packag_confirm_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_cloudvela_spspm_sw_packag_confirm_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_cloudvela_spspm_sw_packag_confirm_t)))
		HCU_ERROR_PRINT_TASK(TASK_ID_SYSSWM, "SYSSWM: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);

	//具体的处理过程
	if (rcv.baseConfirm != HUITP_IEID_UNI_COM_CONFIRM_POSITIVE){
		HCU_DEBUG_PRINT_IPT("SYSSWM: Receive none-positive sw package confirm message, so no handle.\n");
		return SUCCESS;
	}
	//接下来的这一段：第一段index=1，是必须的
	if ((rcv.validLen > rcv.segLen) || (rcv.segIndex != (gTaskSysswmContext.bhSw.cfmSegIndex+1)) || (rcv.validLen > sizeof(rcv.body))){
		HCU_DEBUG_PRINT_IPT("SYSSWM: Receive invalid segment sw package, so no handle.\n");
		return SUCCESS;
	}

	//重新覆盖关键参数，包括第一次写入
	gTaskSysswmContext.bhSw.totalSeg = rcv.segTotal;
	gTaskSysswmContext.bhSw.segLen = rcv.segLen;

	//处理数据并写到硬盘
	FILE *fp;
	char stmp[200];
	memset(stmp, 0, sizeof(stmp));
	strcpy(stmp, zHcuSysEngPar.swm.hcuSwDownloadDir);
	sprintf(stmp, "%shcusw_r%d_v%d.zip", zHcuSysEngPar.swm.hcuSwDownloadDir, gTaskSysswmContext.bhSw.targetSwRel, gTaskSysswmContext.bhSw.targetSwVer);
	//打开源文件：不存在就创建
	if((fp=fopen(stmp, "wb+"))== NULL){
		fclose(fp);
		HCU_ERROR_PRINT_SYSSWM("SYSSWM: Open file %s Error!\n", stmp);
	}
	fseek(fp, 0L, SEEK_END);
	if (fwrite(rcv.body, 1, rcv.validLen, fp) != rcv.validLen){
		fclose(fp);
		HCU_ERROR_PRINT_SYSSWM("SYSSWM: Write file %s Error!\n", stmp);
	}

	//如果检查一切正确，则证实需要＋１
	gTaskSysswmContext.bhSw.cfmSegIndex++;
	gTaskSysswmContext.bhSw.retransTimes = 0;

	//重发，则cfm不变，retransTimes++

	//如果达到最大重传次数，则结束
	if (gTaskSysswmContext.bhSw.retransTimes >= HCU_SYSSWM_SW_PACKAGE_RETRANS_MAX_TIMES){
		memset(&(gTaskSysswmContext.bhSw), 0, sizeof(gTaskSysswmContextBody_t));
		HCU_DEBUG_PRINT_IPT("SYSSWM: Retransmit sw package reach max times, not success.\n");
		//处理硬盘上已经接收到的数据：暂时忽略

		//返回
		return SUCCESS;
	}

	//如果还未收全：暂时无限接收重传
	if (gTaskSysswmContext.bhSw.cfmSegIndex < gTaskSysswmContext.bhSw.totalSeg){
		//继续接收下一段
		msg_struct_spspm_cloudvela_sw_package_report_t snd;
		memset(&snd, 0, sizeof(msg_struct_spspm_cloudvela_sw_package_report_t));

		//L2信息
		strncpy(snd.comHead.destUser, zHcuSysEngPar.cloud.svrNameHome, strlen(zHcuSysEngPar.cloud.svrNameHome)<\
			sizeof(snd.comHead.destUser)?strlen(zHcuSysEngPar.cloud.svrNameHome):sizeof(snd.comHead.destUser));
		strncpy(snd.comHead.srcUser, zHcuSysEngPar.hwBurnId.equLable, strlen(zHcuSysEngPar.hwBurnId.equLable)<\
				sizeof(snd.comHead.srcUser)?strlen(zHcuSysEngPar.hwBurnId.equLable):sizeof(snd.comHead.srcUser));
		snd.comHead.timeStamp = time(0);
		snd.comHead.msgType = HUITP_MSG_HUIXML_MSGTYPE_COMMON_ID;
		strcpy(snd.comHead.funcFlag, "0");

		snd.baseReport = HUITP_IEID_UNI_COM_REPORT_YES;
		snd.segIndex = gTaskSysswmContext.bhSw.cfmSegIndex;  //证实的作用，以便让后台发送下一段
		snd.segTotal = rcv.segTotal;
		snd.segLen = rcv.segLen;
		snd.length = sizeof(msg_struct_spspm_cloudvela_sw_package_report_t);
		if (hcu_message_send(MSG_ID_SYSSWM_CLOUDVELA_SW_PACKAGE_REPORT, TASK_ID_CLOUDVELA, TASK_ID_SYSSWM, &snd, snd.length) == FAILURE)
			HCU_ERROR_PRINT_SYSSWM("SYSSWM: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_SYSSWM].taskName, zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName);
	}

	//数据收齐活了
	else{
		//硬盘文件，并实施升级过程
		//执行过程，未来待完善

		//升级系统区的软件版本
		zHcuSysEngPar.hwBurnId.swRelId = gTaskSysswmContext.bhSw.targetSwRel;
		zHcuSysEngPar.hwBurnId.swVerId = gTaskSysswmContext.bhSw.targetSwVer;

		//升级BOOT区参数
		if (hcu_vm_engpar_update_phy_boot_sw_ver(zHcuSysEngPar.hwBurnId.swRelId, zHcuSysEngPar.hwBurnId.swVerId) == FAILURE)
			HCU_ERROR_PRINT_SYSSWM("SYSSWM: Update local configure file REL/VER ID error!\n");
	}

	//返回
	return SUCCESS;
}

OPSTAT func_sysswm_time_out_period_working_scan(void)
{
	//int ret=0;

	//发送数据给后台
	if ((FsmGetState(TASK_ID_CLOUDVELA) == FSM_STATE_CLOUDVELA_ONLINE) || (FsmGetState(TASK_ID_CLOUDVELA) == FSM_STATE_CLOUDVELA_OFFLINE) ){
		msg_struct_spspm_cloudvela_inventory_report_t snd;
		memset(&snd, 0, sizeof(msg_struct_spspm_cloudvela_inventory_report_t));

		//L2信息
		strncpy(snd.comHead.destUser, zHcuSysEngPar.cloud.svrNameHome, strlen(zHcuSysEngPar.cloud.svrNameHome)<\
			sizeof(snd.comHead.destUser)?strlen(zHcuSysEngPar.cloud.svrNameHome):sizeof(snd.comHead.destUser));
		strncpy(snd.comHead.srcUser, zHcuSysEngPar.hwBurnId.equLable, strlen(zHcuSysEngPar.hwBurnId.equLable)<\
				sizeof(snd.comHead.srcUser)?strlen(zHcuSysEngPar.hwBurnId.equLable):sizeof(snd.comHead.srcUser));
		snd.comHead.timeStamp = time(0);
		snd.comHead.msgType = HUITP_MSG_HUIXML_MSGTYPE_COMMON_ID;
		strcpy(snd.comHead.funcFlag, "0");

		//CONTENT
		snd.baseReport = HUITP_IEID_UNI_COM_REPORT_YES;
		snd.hwType = zHcuSysEngPar.hwBurnId.hwType;
		snd.hwId = zHcuSysEngPar.hwBurnId.hwPemId;
		snd.swRel = zHcuSysEngPar.hwBurnId.swRelId;
		snd.swVer = zHcuSysEngPar.hwBurnId.swVerId;
		snd.upgradeFlag = zHcuSysEngPar.hwBurnId.swUpgradeFlag;
		snd.timeStamp = time(0);
		strcpy(snd.desc, "");
		snd.length = sizeof(msg_struct_spspm_cloudvela_inventory_report_t);
		if (hcu_message_send(MSG_ID_SYSSWM_CLOUDVELA_INVENTORY_REPORT, TASK_ID_CLOUDVELA, TASK_ID_SYSSWM, &snd, snd.length) == FAILURE)
			HCU_ERROR_PRINT_TASK(TASK_ID_SYSSWM, "SYSSWM: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_SYSSWM].taskName, zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName);
	}

	//State no change
	return SUCCESS;
}


