/*
 * sysswm.c
 *
 *  Created on: 2016年5月10日
 *      Author: hitpony
 */

#include "sysswm.h"

#include "../l0service/timer.h"
#include "../l0service/trace.h"
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

    //Task level functions: CLOUD BH handler
	{MSG_ID_CLOUDVELA_SYSSWM_INVENTORY_REQ,     	FSM_STATE_SYSSWM_ACTIVED,           fsm_sysswm_cloudvela_inventory_req},
	{MSG_ID_CLOUDVELA_SYSSWM_INVENTORY_CONFIRM, 	FSM_STATE_SYSSWM_ACTIVED,           fsm_sysswm_cloudvela_inventory_confirm},
	{MSG_ID_CLOUDVELA_SYSSWM_SW_PACKAGE_REQ,        FSM_STATE_SYSSWM_ACTIVED,           fsm_sysswm_cloudvela_sw_package_req},
	{MSG_ID_CLOUDVELA_SYSSWM_SW_PACKAGE_CONFIRM,    FSM_STATE_SYSSWM_ACTIVED,           fsm_sysswm_cloudvela_sw_package_confirm},

	//Task level functions: IHU CAN handler
	{MSG_ID_CANITFLEO_SYSSWM_INVENTORY_REPORT,     	FSM_STATE_SYSSWM_ACTIVED,           fsm_sysswm_canitfleo_inventory_report},
	{MSG_ID_CANITFLEO_SYSSWM_SW_PACKAGE_REPORT,     FSM_STATE_SYSSWM_ACTIVED,           fsm_sysswm_canitfleo_sw_package_report},

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
	gTaskSysswmContext.swDlMaxTimes = HCU_SYSSWM_SW_DOWNLOAD_MAX_TIMES;

	//启动周期性定时器：第一次将时钟降低到15秒
	ret = hcu_timer_start(TASK_ID_SYSSWM, TIMER_ID_1S_SYSSWM_PERIOD_WORKING, 15, TIMER_TYPE_PERIOD, TIMER_RESOLUTION_1S);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_SYSSWM]++;
		HcuErrorPrint("SYSSWM: Error start period timer!\n");
		return FAILURE;
	}

	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_SYSSWM, FSM_STATE_SYSSWM_ACTIVED) == FAILURE)
		HCU_ERROR_PRINT_TASK(TASK_ID_SYSSWM, "SYSSWM: Error Set FSM State!\n");
	HCU_DEBUG_PRINT_FAT("SYSSWM: Enter FSM_STATE_SYSSWM_ACTIVED status, Keeping refresh here!\n");

	//刷新右下角当前版本信息
	func_sysswm_sw_init_info_trigger_ui();

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
		//先判定是否要干活
		gTaskSysswmContext.swDlCntTimes++;
		if (gTaskSysswmContext.swDlCntTimes > gTaskSysswmContext.swDlMaxTimes){
			hcu_timer_stop(TASK_ID_SYSSWM, TIMER_ID_1S_SYSSWM_PERIOD_WORKING, TIMER_RESOLUTION_1S);
			return SUCCESS;
		}

		//先清理孤儿文件
		if (dbi_HcuSysSwm_SwPkg_orphane_file_delete() == FAILURE)
			HCU_ERROR_PRINT_TASK(TASK_ID_SYSSWM, "SYSSWM: Delete orphane file failure!\n");

		//清理所有下载到一半的表单和文件
		if (dbi_HcuSysSwm_SwPkg_download_incomplete_file_and_table_delete() == FAILURE)
			HCU_ERROR_PRINT_TASK(TASK_ID_SYSSWM, "SYSSWM: Delete incomplete download file failure!\n");

		//清理上下文
		memset(&gTaskSysswmContext.cloudSwPkg, 0, sizeof(HcuSysMsgIeL3SysSwmSwPkgElement_t));
		memset(&gTaskSysswmContext.cloudSwDl, 0, sizeof(HcuSysMsgIeL3SysSwmSwDlElement_t));
		gTaskSysswmContext.reTransTimes = 0;
		gTaskSysswmContext.swDlSession++;
		gTaskSysswmContext.swDlSession = (gTaskSysswmContext.swDlSession % HCU_SYSSWM_SW_DOWNLOAD_SESSION_MAX_NBR);

		//分类处理
		if ((gTaskSysswmContext.swDlSession == HCU_SYSSWM_SW_DOWNLOAD_SESSION_HCU_CLIENT) && ((zHcuSysEngPar.hwBurnId.swUpgradeFlag == HCU_SYSMSG_SYSSWM_FW_UPGRADE_YES_STABLE) || \
				(zHcuSysEngPar.hwBurnId.swUpgradeFlag == HCU_SYSMSG_SYSSWM_FW_UPGRADE_YES_TRIAL) || (zHcuSysEngPar.hwBurnId.swUpgradeFlag == HCU_SYSMSG_SYSSWM_FW_UPGRADE_YES_PATCH)))
			ret = func_sysswm_time_out_period_working_scan_hcu_client();
		else if ((gTaskSysswmContext.swDlSession == HCU_SYSSWM_SW_DOWNLOAD_SESSION_IHU_STABLE) && (zHcuSysEngPar.hwBurnId.nodeHwType != 0))
			ret = func_sysswm_time_out_period_working_scan_ihu_stable();
		else if ((gTaskSysswmContext.swDlSession == HCU_SYSSWM_SW_DOWNLOAD_SESSION_IHU_TRIAL) && (zHcuSysEngPar.hwBurnId.nodeHwType != 0))
			ret = func_sysswm_time_out_period_working_scan_ihu_trial();
		else if ((gTaskSysswmContext.swDlSession == HCU_SYSSWM_SW_DOWNLOAD_SESSION_IHU_PATCH) && (zHcuSysEngPar.hwBurnId.nodeHwType != 0)){
			ret = func_sysswm_time_out_period_working_scan_ihu_patch();
			//重新设置定时器
			hcu_timer_start(TASK_ID_SYSSWM, TIMER_ID_1S_SYSSWM_PERIOD_WORKING, zHcuSysEngPar.timer.array[TIMER_ID_1S_SYSSWM_PERIOD_WORKING].dur, TIMER_TYPE_PERIOD, TIMER_RESOLUTION_1S);
		}
	}

	//下载短定时
	else if ((rcv.timeId == TIMER_ID_1S_SYSSWM_SEG_DL_WAIT) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		ret = func_sysswm_time_out_segment_download_wait_process();
	}

	return ret;
}

OPSTAT fsm_sysswm_cloudvela_inventory_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret=0;

	//Receive message and copy to local variable
	msg_struct_cloudvela_sysswm_inventory_req_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_cloudvela_sysswm_inventory_req_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_cloudvela_sysswm_inventory_req_t)))
		HCU_ERROR_PRINT_TASK(TASK_ID_SYSSWM, "SYSSWM: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);

	//生成消息并发送给后台
	msg_struct_sysswm_cloudvela_inventory_resp_t snd;
	memset(&snd, 0, sizeof(msg_struct_sysswm_cloudvela_inventory_resp_t));

	//L2信息
	strncpy(snd.comHead.destUser, zHcuSysEngPar.cloud.svrNameHome, strlen(zHcuSysEngPar.cloud.svrNameHome)<\
		sizeof(snd.comHead.destUser)?strlen(zHcuSysEngPar.cloud.svrNameHome):sizeof(snd.comHead.destUser));
	strncpy(snd.comHead.srcUser, zHcuSysEngPar.hwBurnId.equLable, strlen(zHcuSysEngPar.hwBurnId.equLable)<\
			sizeof(snd.comHead.srcUser)?strlen(zHcuSysEngPar.hwBurnId.equLable):sizeof(snd.comHead.srcUser));
	snd.comHead.timeStamp = time(0);
	snd.comHead.msgType = HUITP_MSG_HUIXML_MSGTYPE_COMMON_ID;
	strncpy(snd.comHead.funcFlag, zHcuSysEngPar.hwBurnId.hw_mac, strlen(zHcuSysEngPar.hwBurnId.hw_mac)<IHU_SYSMSG_BH_COM_HEAD_NAME_LEN_MAX?strlen(zHcuSysEngPar.hwBurnId.hw_mac):IHU_SYSMSG_BH_COM_HEAD_NAME_LEN_MAX);

	snd.baseResp = HUITP_IEID_UNI_COM_RESPONSE_YES;
	snd.equEntry = HCU_SYSMSG_SYSSWM_EQU_ENTRY_HCU_CLIENT_SW;
	snd.hwType = zHcuSysEngPar.hwBurnId.hwType;
	snd.hwId = zHcuSysEngPar.hwBurnId.hwPemId;
	snd.swRel = zHcuSysEngPar.hwBurnId.swRelId;
	snd.swVer = zHcuSysEngPar.hwBurnId.swVerId;
	snd.dbVer = zHcuSysEngPar.hwBurnId.dbVerId;
	snd.upgradeFlag = zHcuSysEngPar.hwBurnId.swUpgradeFlag;

	snd.length = sizeof(msg_struct_sysswm_cloudvela_inventory_resp_t);
	if (hcu_message_send(MSG_ID_SYSSWM_CLOUDVELA_INVENTORY_RESP, TASK_ID_CLOUDVELA, TASK_ID_SYSSWM, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_SYSSWM("SYSSWM: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_SYSSWM].taskName, zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName);

	//返回
	return SUCCESS;
}

OPSTAT fsm_sysswm_cloudvela_inventory_confirm(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;
	char fname[HCU_SYSMSG_SYSSWM_SW_PKG_FILE_NAME_MAX_LEN];

	//Receive message and copy to local variable
	msg_struct_cloudvela_sysswm_inventory_confirm_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_cloudvela_sysswm_inventory_confirm_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_cloudvela_sysswm_inventory_confirm_t)))
		HCU_ERROR_PRINT_TASK(TASK_ID_SYSSWM, "SYSSWM: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);

	//处理接收到的反馈，并进行合理的处理
	if (rcv.baseConfirm != HUITP_IEID_UNI_COM_CONFIRM_POSITIVE){
		HcuErrorPrint("SYSSWM: Receive none-positive inventory confirm message, so no handle, Session=%d.\n", gTaskSysswmContext.swDlSession);
		return SUCCESS;
	}

	//刷新版本界面
	func_sysswm_sw_inventory_confirm_info_trigger_ui(gTaskSysswmContext.swDlSession, rcv.swRel, rcv.swVer, rcv.dbVer);

	//一次INVENTORY都会重来
	ret = SUCCESS;

	//1) 如果收到HCU_CLIENT
	if (gTaskSysswmContext.swDlSession == HCU_SYSSWM_SW_DOWNLOAD_SESSION_HCU_CLIENT){
		if ((rcv.equEntry != HCU_SYSMSG_SYSSWM_EQU_ENTRY_HCU_CLIENT_SW) || (rcv.hwType != zHcuSysEngPar.hwBurnId.hwType) || (rcv.hwId < zHcuSysEngPar.hwBurnId.hwPemId) || (rcv.upgradeFlag != zHcuSysEngPar.hwBurnId.swUpgradeFlag)){
			HCU_ERROR_PRINT_TASK(TASK_ID_SYSSWM, "SYSSWM: Receive none-compliance hardware info, so no handle.\n");
		}
		if ((rcv.upgradeFlag != HCU_SYSCFG_HBB_FW_UPGRADE_YES_STABLE) && (rcv.upgradeFlag != HCU_SYSCFG_HBB_FW_UPGRADE_YES_TRIAL) && (rcv.upgradeFlag != HCU_SYSCFG_HBB_FW_UPGRADE_YES_PATCH)){
			HCU_ERROR_PRINT_TASK(TASK_ID_SYSSWM, "SYSSWM: Receive none-compliance upgrade info, so no handle.\n");
		}

		//如果版本号不适合
		HCU_DEBUG_PRINT_CRT("SYSSWM: Session=%d, HCU Rcv REL/VER/DBVER=[%d/%d/%d], BurnId REL/VER/DBVER=[%d/%d/%d]\n", gTaskSysswmContext.swDlSession, rcv.swRel, rcv.swVer, rcv.dbVer, zHcuSysEngPar.hwBurnId.swRelId, zHcuSysEngPar.hwBurnId.swVerId, zHcuSysEngPar.hwBurnId.dbVerId);
		if ((rcv.swRel < zHcuSysEngPar.hwBurnId.swRelId) || ((rcv.swRel == zHcuSysEngPar.hwBurnId.swRelId) && (rcv.swVer <= zHcuSysEngPar.hwBurnId.swVerId))){
			return SUCCESS;
		}

		//将新版本存入数据库
		gTaskSysswmContext.cloudSwPkg.equEntry = HCU_SYSMSG_SYSSWM_EQU_ENTRY_HCU_CLIENT_SW;
		gTaskSysswmContext.cloudSwPkg.hwType = rcv.hwType;
		gTaskSysswmContext.cloudSwPkg.hwPem = rcv.hwId;
		gTaskSysswmContext.cloudSwPkg.swRel = rcv.swRel;
		gTaskSysswmContext.cloudSwPkg.swVer = rcv.swVer;
		gTaskSysswmContext.cloudSwPkg.dbVer = rcv.dbVer;
		gTaskSysswmContext.cloudSwPkg.upgradeFlag = rcv.upgradeFlag;
		gTaskSysswmContext.cloudSwPkg.swCksum = rcv.swCheckSum;
		gTaskSysswmContext.cloudSwPkg.swTotalLen = rcv.swTotalLen;
		gTaskSysswmContext.cloudSwPkg.dbCksum = rcv.dbCheckSum;
		gTaskSysswmContext.cloudSwPkg.dbTotalLen = rcv.dbTotalLen;
		strncpy(gTaskSysswmContext.cloudSwPkg.currentActive, HCU_SYSMSG_SYSSWM_CUR_ACTIVE_START, sizeof(HCU_SYSMSG_SYSSWM_CUR_ACTIVE_START));
		gTaskSysswmContext.cloudSwPkg.updateTime = time(0);
		memset(fname, 0, sizeof(fname));
		if (zHcuSysEngPar.hwBurnId.swUpgradeFlag == HCU_SYSCFG_HBB_FW_UPGRADE_YES_STABLE)
			sprintf(fname, "HCU_HPT%d_PEM%d_REL%d_VER%d_STABLE.HEX", rcv.hwType, rcv.hwId, rcv.swRel, rcv.swVer);
		else if (zHcuSysEngPar.hwBurnId.swUpgradeFlag == HCU_SYSCFG_HBB_FW_UPGRADE_YES_TRIAL)
			sprintf(fname, "HCU_HPT%d_PEM%d_REL%d_VER%d_TRAIL.HEX", rcv.hwType, rcv.hwId, rcv.swRel, rcv.swVer);
		else if (zHcuSysEngPar.hwBurnId.swUpgradeFlag == HCU_SYSCFG_HBB_FW_UPGRADE_YES_PATCH)
			sprintf(fname, "HCU_HPT%d_PEM%d_REL%d_VER%d_PATCH.HEX", rcv.hwType, rcv.hwId, rcv.swRel, rcv.swVer);
		strncpy(gTaskSysswmContext.cloudSwPkg.fileName, fname, HCU_SYSMSG_SYSSWM_SW_PKG_FILE_NAME_MAX_LEN-1);
		sprintf(fname, "HCU_HPT%d_PEM%d_REL%d_VER%d_MYSQL.SQL", rcv.hwType, rcv.hwId, rcv.swRel, rcv.dbVer);
		strncpy(gTaskSysswmContext.cloudSwPkg.dbName, fname, HCU_SYSMSG_SYSSWM_SW_PKG_FILE_NAME_MAX_LEN-1);
		if (dbi_HcuSysSwm_SwPkg_save(&(gTaskSysswmContext.cloudSwPkg)) == FAILURE)
			HCU_ERROR_PRINT_TASK(TASK_ID_SYSSWM, "SYSSWM: Save new version into database error!\n");

		//刷新下载初始化Context
		memset(&(gTaskSysswmContext.cloudSwDl), 0, sizeof(HcuSysMsgIeL3SysSwmSwDlElement_t));
		gTaskSysswmContext.cloudSwDl.equEntry = HCU_SYSMSG_SYSSWM_EQU_ENTRY_HCU_CLIENT_SW;
		gTaskSysswmContext.cloudSwDl.hwType = rcv.hwType;
		gTaskSysswmContext.cloudSwDl.hwPem = rcv.hwId;
		gTaskSysswmContext.cloudSwDl.swRel = rcv.swRel;
		gTaskSysswmContext.cloudSwDl.verId = rcv.swVer;
		gTaskSysswmContext.cloudSwDl.upgradeFlag = rcv.upgradeFlag;
		gTaskSysswmContext.cloudSwDl.checksum = rcv.swCheckSum;
		gTaskSysswmContext.cloudSwDl.totalLen = rcv.swTotalLen;
		gTaskSysswmContext.cloudSwDl.dlTime = time(0);
		gTaskSysswmContext.cloudSwDl.segIndex = 1; //第一段
		gTaskSysswmContext.cloudSwDl.segSplitLen = HUITP_IEID_UNI_SW_PACKAGE_BODY_MAX_LEN;
		strncpy(gTaskSysswmContext.cloudSwDl.fileName, gTaskSysswmContext.cloudSwPkg.fileName, strlen(gTaskSysswmContext.cloudSwPkg.fileName));

		UINT32 segTotal = 0;
		segTotal = gTaskSysswmContext.cloudSwPkg.swTotalLen / HUITP_IEID_UNI_SW_PACKAGE_BODY_MAX_LEN;
		if (segTotal * HUITP_IEID_UNI_SW_PACKAGE_BODY_MAX_LEN < gTaskSysswmContext.cloudSwPkg.swTotalLen) segTotal++;
		gTaskSysswmContext.cloudSwDl.segTotal = segTotal;

		//如果文件过大，就直接使用FTP下载
		if (gTaskSysswmContext.cloudSwPkg.swTotalLen > HCU_SYSSWM_SW_PACKAGE_FTP_DOWNLOAD_FILE_SIZE_THREADHOLD){
			return func_sysswm_ftp_file_big_size_process_hcu_sw_and_db();
		}//如果文件过大，就直接使用FTP下载

		//生成消息并发送给后台
		ret = func_sysswm_send_cloudvela_sw_package_report();
	}

	//2) 如果收到IHU_CLIENT.STABLE / TRIAL / PATCH
	else if ((gTaskSysswmContext.swDlSession == HCU_SYSSWM_SW_DOWNLOAD_SESSION_IHU_STABLE) || (gTaskSysswmContext.swDlSession == HCU_SYSSWM_SW_DOWNLOAD_SESSION_IHU_TRIAL) || (gTaskSysswmContext.swDlSession == HCU_SYSSWM_SW_DOWNLOAD_SESSION_IHU_PATCH)){
		if ((gTaskSysswmContext.cloudSwPkg.equEntry != HCU_SYSMSG_SYSSWM_EQU_ENTRY_IHU_CLIENT) || (rcv.equEntry != HCU_SYSMSG_SYSSWM_EQU_ENTRY_IHU_CLIENT) || (rcv.hwType != gTaskSysswmContext.cloudSwPkg.hwType) \
				|| (rcv.hwId < gTaskSysswmContext.cloudSwPkg.hwPem) || (rcv.upgradeFlag != gTaskSysswmContext.cloudSwPkg.upgradeFlag)){
			HCU_ERROR_PRINT_TASK(TASK_ID_SYSSWM, "SYSSWM: Receive none-compliance hardware info, so no handle. Entry=%d, hwType=%d, hwId=%d, Flag=%d. Context Entry=%d, heType=%d, heId=%d, Flag=%d\n", \
					rcv.equEntry, rcv.hwType, rcv.hwId, rcv.upgradeFlag, gTaskSysswmContext.cloudSwPkg.equEntry, \
					gTaskSysswmContext.cloudSwPkg.hwType, gTaskSysswmContext.cloudSwPkg.hwPem, gTaskSysswmContext.cloudSwPkg.upgradeFlag);
		}

		//如果版本号不适合
		HCU_DEBUG_PRINT_CRT("SYSSWM: Session=%d, IHU Rcv REL/VER=[%d/%d], Original REL/VER=[%d/%d]\n", gTaskSysswmContext.swDlSession, rcv.swRel, rcv.swVer, gTaskSysswmContext.cloudSwPkg.swRel, gTaskSysswmContext.cloudSwPkg.swVer);
		if ((rcv.swRel < gTaskSysswmContext.cloudSwPkg.swRel) || ((rcv.swRel == gTaskSysswmContext.cloudSwPkg.swRel) && (rcv.swVer <= gTaskSysswmContext.cloudSwPkg.swVer))){
			return SUCCESS;
		}

		//将新版本存入数据库
		gTaskSysswmContext.cloudSwPkg.swRel = rcv.swRel;
		gTaskSysswmContext.cloudSwPkg.swVer = rcv.swVer;
		gTaskSysswmContext.cloudSwPkg.swCksum = rcv.swCheckSum;
		gTaskSysswmContext.cloudSwPkg.swTotalLen = rcv.swTotalLen;
		strncpy(gTaskSysswmContext.cloudSwPkg.currentActive, HCU_SYSMSG_SYSSWM_CUR_ACTIVE_START, sizeof(HCU_SYSMSG_SYSSWM_CUR_ACTIVE_START));
		gTaskSysswmContext.cloudSwPkg.updateTime = time(0);
		memset(fname, 0, sizeof(fname));
		if (gTaskSysswmContext.cloudSwPkg.upgradeFlag == HCU_SYSCFG_HBB_FW_UPGRADE_YES_STABLE)
			sprintf(fname, "IHU_HPT%d_PEM%d_REL%d_VER%d_STABLE.BIN", rcv.hwType, rcv.hwId, rcv.swRel, rcv.swVer);
		else if (gTaskSysswmContext.cloudSwPkg.upgradeFlag == HCU_SYSCFG_HBB_FW_UPGRADE_YES_TRIAL)
			sprintf(fname, "IHU_HPT%d_PEM%d_REL%d_VER%d_TRAIL.BIN", rcv.hwType, rcv.hwId, rcv.swRel, rcv.swVer);
		else if (gTaskSysswmContext.cloudSwPkg.upgradeFlag == HCU_SYSCFG_HBB_FW_UPGRADE_YES_PATCH)
			sprintf(fname, "IHU_HPT%d_PEM%d_REL%d_VER%d_PATCH.BIN", rcv.hwType, rcv.hwId, rcv.swRel, rcv.swVer);
		strncpy(gTaskSysswmContext.cloudSwPkg.fileName, fname, HCU_SYSMSG_SYSSWM_SW_PKG_FILE_NAME_MAX_LEN-1);
		if (dbi_HcuSysSwm_SwPkg_save(&(gTaskSysswmContext.cloudSwPkg)) == FAILURE)
			HCU_ERROR_PRINT_TASK(TASK_ID_SYSSWM, "SYSSWM: Save new version into database error!\n");

		//刷新下载初始化Context
		memset(&(gTaskSysswmContext.cloudSwDl), 0, sizeof(HcuSysMsgIeL3SysSwmSwDlElement_t));
		gTaskSysswmContext.cloudSwDl.equEntry = rcv.equEntry;
		gTaskSysswmContext.cloudSwDl.hwType = rcv.hwType;
		gTaskSysswmContext.cloudSwDl.hwPem = rcv.hwId;
		gTaskSysswmContext.cloudSwDl.swRel = rcv.swRel;
		gTaskSysswmContext.cloudSwDl.verId = rcv.swVer;
		gTaskSysswmContext.cloudSwDl.upgradeFlag = rcv.upgradeFlag;
		gTaskSysswmContext.cloudSwDl.checksum = rcv.swCheckSum;
		gTaskSysswmContext.cloudSwDl.totalLen = rcv.swTotalLen;
		gTaskSysswmContext.cloudSwDl.segIndex = 1;  //第一段
		strncpy(gTaskSysswmContext.cloudSwDl.fileName, fname, HCU_SYSMSG_SYSSWM_SW_PKG_FILE_NAME_MAX_LEN-1);
		gTaskSysswmContext.cloudSwDl.segSplitLen = HUITP_IEID_UNI_SW_PACKAGE_BODY_MAX_LEN;
		UINT32 segTotal = 0;
		segTotal = gTaskSysswmContext.cloudSwPkg.swTotalLen / HUITP_IEID_UNI_SW_PACKAGE_BODY_MAX_LEN;
		if (segTotal * HUITP_IEID_UNI_SW_PACKAGE_BODY_MAX_LEN < gTaskSysswmContext.cloudSwPkg.swTotalLen) segTotal++;
		gTaskSysswmContext.cloudSwDl.segTotal = segTotal;
		gTaskSysswmContext.cloudSwDl.equEntry = HCU_SYSMSG_SYSSWM_EQU_ENTRY_IHU_CLIENT;

		//如果文件过大，就直接使用FTP下载
		if (gTaskSysswmContext.cloudSwPkg.swTotalLen > HCU_SYSSWM_SW_PACKAGE_FTP_DOWNLOAD_FILE_SIZE_THREADHOLD){
			return func_sysswm_ftp_file_big_size_process_ihu_sw();
		}//如果文件过大，就直接使用FTP下载

		//生成消息并发送给后台
		ret = func_sysswm_send_cloudvela_sw_package_report();
	}

	else{
		HCU_ERROR_PRINT_SYSSWM("SYSSWM: Error equEntry field filled!\n");
	}

	//返回
	return ret;
}

//暂时不处理
OPSTAT fsm_sysswm_cloudvela_sw_package_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret=0;

	//Receive message and copy to local variable
	msg_struct_cloudvela_sysswm_sw_package_req_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_cloudvela_sysswm_sw_package_req_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_cloudvela_sysswm_sw_package_req_t)))
		HCU_ERROR_PRINT_TASK(TASK_ID_SYSSWM, "SYSSWM: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);



	return SUCCESS;
}

//完全依赖于这个过程进行软件体的传输
OPSTAT fsm_sysswm_cloudvela_sw_package_confirm(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;
	int i =0;
	UINT16 res = 0;

	//Receive message and copy to local variable
	msg_struct_cloudvela_sysswm_sw_package_confirm_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_cloudvela_sysswm_sw_package_confirm_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_cloudvela_sysswm_sw_package_confirm_t)))
		HCU_ERROR_PRINT_TASK(TASK_ID_SYSSWM, "SYSSWM: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);

	//停止定时器
	hcu_timer_stop(TASK_ID_SYSSWM, TIMER_ID_1S_SYSSWM_SEG_DL_WAIT, TIMER_RESOLUTION_1S);

	//判定状态
	if ((rcv.equEntry != gTaskSysswmContext.cloudSwDl.equEntry) || (rcv.hwType != gTaskSysswmContext.cloudSwDl.hwType) \
			|| (rcv.hwPem != gTaskSysswmContext.cloudSwDl.hwPem) || (rcv.swRelId != gTaskSysswmContext.cloudSwDl.swRel) \
			|| (rcv.verId != gTaskSysswmContext.cloudSwDl.verId) || (rcv.upgradeFlag != gTaskSysswmContext.cloudSwDl.upgradeFlag)){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_SYSSWM]++;
		HcuErrorPrint("SYSSWM: Receive invalid key element, so no handle.\n");
		ret = FAILURE;
	}

	//具体的处理过程
	if (rcv.baseConfirm != HUITP_IEID_UNI_COM_CONFIRM_POSITIVE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_SYSSWM]++;
		HcuErrorPrint("SYSSWM: Receive none-positive sw package confirm message, so no handle. SegTotal Received=%d, UpgradeFlag=%d, Ver=%d\n", rcv.segTotal, rcv.upgradeFlag, rcv.verId);
		return SUCCESS;
	}

	//接下来的这一段：第一段index=1，是必须的
	if ((rcv.segValidLen > rcv.segSplitLen) || (rcv.segIndex == 0) || (rcv.segIndex != gTaskSysswmContext.cloudSwDl.segIndex) \
			|| (rcv.segValidLen > sizeof(rcv.body)) || (rcv.segTotal != gTaskSysswmContext.cloudSwDl.segTotal) \
			|| (rcv.segSplitLen != gTaskSysswmContext.cloudSwDl.segSplitLen) || (rcv.segIndex > rcv.segTotal)){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_SYSSWM]++;
		HcuErrorPrint("SYSSWM: Receive invalid segment sw package, so no handle.\n");
		ret = FAILURE;
	}

	//重新覆盖关键参数，包括第一次写入
	if (rcv.segIndex < rcv.segTotal){
		if (rcv.segValidLen != rcv.segSplitLen){
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_SYSSWM]++;
			HcuErrorPrint("SYSSWM: Receive invalid seg valid len sw package, so no handle. Rcc ValidLen=%d, SplitLen=%d\n", rcv.segValidLen, rcv.segSplitLen);
			ret = FAILURE;
		}
		gTaskSysswmContext.cloudSwDl.segValidLen = rcv.segValidLen;
	}
	else{
		gTaskSysswmContext.cloudSwDl.segValidLen = rcv.segValidLen;
		int totalLen = rcv.segSplitLen * (rcv.segTotal-1) + rcv.segValidLen;
		if (rcv.equEntry == HCU_SYSMSG_SYSSWM_EQU_ENTRY_HCU_CLIENT_DB){
			 if (totalLen != gTaskSysswmContext.cloudSwPkg.dbTotalLen){
				zHcuSysStaPm.taskRunErrCnt[TASK_ID_SYSSWM]++;
				HcuErrorPrint("SYSSWM: Receive invalid seg total len db package, so no handle.  Caculated Total Len=%d, Exptected Len=%d.\n", totalLen, gTaskSysswmContext.cloudSwPkg.dbTotalLen);
				ret = FAILURE;
			 }
		}
		else if (totalLen != gTaskSysswmContext.cloudSwPkg.swTotalLen){
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_SYSSWM]++;
			HcuErrorPrint("SYSSWM: Receive invalid seg total len sw package, so no handle. Caculated Total Len=%d, Exptected Len=%d.\n", totalLen, gTaskSysswmContext.cloudSwPkg.swTotalLen);
			ret = FAILURE;
		}
	}

	//检查segCkSum
	res = 0;
	if (rcv.segIndex == rcv.segTotal){
		for (i=0; i<rcv.segValidLen; i++){
			res+= (UINT8)rcv.body[i];
		}
	}else{
		for (i=0; i<rcv.segSplitLen; i++){
			res+= (UINT8)rcv.body[i];
		}
	}
	HCU_DEBUG_PRINT_FAT("SYSSWM: Download progressing: segIndex=%d, segTotal=%d, splitLen=%d, validLen=%d, Caculated Result = 0x%x, Received Cksum =0x%x\n", rcv.segIndex, rcv.segTotal, rcv.segSplitLen, rcv.segValidLen, res, rcv.segCheckSum);
	if (res != rcv.segCheckSum){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_SYSSWM]++;
		HcuErrorPrint("SYSSWM: Receive invalid segment sw package, so no handle. segIndex=%d, segTotal=%d, validLen=%d, Caculated Result = 0x%x, Received Cksum =0x%x\n", rcv.segIndex, rcv.segTotal, rcv.segValidLen, res, rcv.segCheckSum);
		ret = FAILURE;
	}

	//重传
	if (ret == FAILURE){
		gTaskSysswmContext.reTransTimes++;
		if (gTaskSysswmContext.reTransTimes < HCU_SYSSWM_SW_PACKAGE_RETRANS_MAX_TIMES){
			return func_sysswm_send_cloudvela_sw_package_report();
		}else{
			return ret;
		}
	}

	//存入分段数据库
	gTaskSysswmContext.cloudSwDl.segCkSum = rcv.segCheckSum;
	gTaskSysswmContext.cloudSwDl.dlTime = time(0);
	if (dbi_HcuSysSwm_SwDownLoad_save(&(gTaskSysswmContext.cloudSwDl)) == FAILURE)
		HCU_ERROR_PRINT_TASK(TASK_ID_SYSSWM, "SYSSWM: Save SW_DOWNLOAD Seg into database error!\n");

	//处理数据并写到硬盘
	FILE *fp;
	char stmp[200];
	memset(stmp, 0, sizeof(stmp));
	strcpy(stmp, zHcuSysEngPar.swm.hcuSwActiveDir);
	strcat(stmp, gTaskSysswmContext.cloudSwDl.fileName);

	//如果是第一段，果断删掉已经存在的文件
	if (rcv.segIndex == 1){
		if (access(stmp, F_OK) == 0){
			char stmp2[200];
			memset(stmp2, 0, sizeof(stmp2));
			sprintf(stmp2, "rm %s", stmp);
			HCU_DEBUG_PRINT_FAT("SYSSWM: Rm file = [%s]\n", stmp2);
			system(stmp2);
		}
	}

	//打开源文件：不存在就创建
	//at+, ab+，代表TEXT/BIN文件格式
	//本地统一使用ab+，不同的方式将导致checksum的不一致
	if (rcv.equEntry == HCU_SYSMSG_SYSSWM_EQU_ENTRY_HCU_CLIENT_DB){
		if((fp=fopen(stmp, "at+"))== NULL){
			HCU_ERROR_PRINT_SYSSWM("SYSSWM: Open file %s Error!\n", stmp);
		}
	}
	else{
		if((fp=fopen(stmp, "ab+"))== NULL){
			HCU_ERROR_PRINT_SYSSWM("SYSSWM: Open file %s Error!\n", stmp);
		}
	}
	fseek(fp, 0L, SEEK_END);

	//最后一段
	if (rcv.segIndex == rcv.segTotal){
		if (fwrite(rcv.body, 1, rcv.segValidLen, fp) != rcv.segValidLen){
			fclose(fp);
			HCU_ERROR_PRINT_SYSSWM("SYSSWM: Write file %s Error!\n", stmp);
		}

		//测得文件大小
		fseek(fp, 0L, SEEK_END);
		int file_len = 0;
		file_len = ftell(fp);
		if (rcv.equEntry == HCU_SYSMSG_SYSSWM_EQU_ENTRY_HCU_CLIENT_DB){
			if (file_len != gTaskSysswmContext.cloudSwPkg.dbTotalLen){
				fclose(fp);
				HCU_ERROR_PRINT_SYSSWM("SYSSWM: Not correct file len, Actual download FileLen = %d, Inventory receive len = %d!\n", file_len, gTaskSysswmContext.cloudSwPkg.swTotalLen);
			}
		}else{
			if (file_len != gTaskSysswmContext.cloudSwPkg.swTotalLen){
				fclose(fp);
				HCU_ERROR_PRINT_SYSSWM("SYSSWM: Not correct file len, Actual download FileLen = %d, Inventory receive len = %d!\n", file_len, gTaskSysswmContext.cloudSwPkg.swTotalLen);
			}
		}
	}
	//中间段
	else{
		if (fwrite(rcv.body, 1, rcv.segSplitLen, fp) != rcv.segSplitLen){
			fclose(fp);
			HCU_ERROR_PRINT_SYSSWM("SYSSWM: Write file %s Error!\n", stmp);
		}
	}
	//关闭文件
	fclose(fp);

	//如果是最后一段，检查整体Checksum
	//最后一段HCU_SW
	if ((rcv.segIndex == rcv.segTotal) && (rcv.equEntry == HCU_SYSMSG_SYSSWM_EQU_ENTRY_HCU_CLIENT_SW)){
		if (func_sysswm_swpkg_last_seg_process_hcu_sw(stmp) == FAILURE) return FAILURE;
	}

	//最后一段HCU_DB
	else if ((rcv.segIndex == rcv.segTotal) && (rcv.equEntry == HCU_SYSMSG_SYSSWM_EQU_ENTRY_HCU_CLIENT_DB)){
		if (func_sysswm_swpkg_last_seg_process_hcu_db(stmp) == FAILURE) return FAILURE;
	}

	//为下位机下载的软件
	else if ((rcv.segIndex == rcv.segTotal) && (rcv.equEntry == HCU_SYSMSG_SYSSWM_EQU_ENTRY_IHU_CLIENT)){
		if (func_sysswm_swpkg_last_seg_process_ihu_sw(stmp) == FAILURE) return FAILURE;
	}

	else{
		gTaskSysswmContext.cloudSwDl.segIndex++;
		gTaskSysswmContext.reTransTimes = 0;
		return func_sysswm_send_cloudvela_sw_package_report();
	}

	//返回
	return SUCCESS;
}

OPSTAT func_sysswm_time_out_period_working_scan_hcu_client(void)
{
	//int ret=0;

	//停止定时器，防止上一次的操作还未结束
	hcu_timer_stop(TASK_ID_SYSSWM, TIMER_ID_1S_SYSSWM_SEG_DL_WAIT, TIMER_RESOLUTION_1S);

	//发送数据给后台
	if ((FsmGetState(TASK_ID_CLOUDVELA) == FSM_STATE_CLOUDVELA_ONLINE) || (FsmGetState(TASK_ID_CLOUDVELA) == FSM_STATE_CLOUDVELA_OFFLINE) ){
		msg_struct_sysswm_cloudvela_inventory_report_t snd;
		memset(&snd, 0, sizeof(msg_struct_sysswm_cloudvela_inventory_report_t));
		//L2信息
		strncpy(snd.comHead.destUser, zHcuSysEngPar.cloud.svrNameHome, strlen(zHcuSysEngPar.cloud.svrNameHome)<\
			sizeof(snd.comHead.destUser)?strlen(zHcuSysEngPar.cloud.svrNameHome):sizeof(snd.comHead.destUser));
		strncpy(snd.comHead.srcUser, zHcuSysEngPar.hwBurnId.equLable, strlen(zHcuSysEngPar.hwBurnId.equLable)<\
				sizeof(snd.comHead.srcUser)?strlen(zHcuSysEngPar.hwBurnId.equLable):sizeof(snd.comHead.srcUser));
		snd.comHead.timeStamp = time(0);
		snd.comHead.msgType = HUITP_MSG_HUIXML_MSGTYPE_COMMON_ID;
		strncpy(snd.comHead.funcFlag, zHcuSysEngPar.hwBurnId.hw_mac, strlen(zHcuSysEngPar.hwBurnId.hw_mac)<IHU_SYSMSG_BH_COM_HEAD_NAME_LEN_MAX?strlen(zHcuSysEngPar.hwBurnId.hw_mac):IHU_SYSMSG_BH_COM_HEAD_NAME_LEN_MAX);

		//CONTENT
		//这里假设，每一次发现新的版本，必然更新到新版本，所以内存中的版本将跟数据库最新版本保持同步状态
		snd.baseReport = HUITP_IEID_UNI_COM_REPORT_YES;
		snd.equEntry = HCU_SYSMSG_SYSSWM_EQU_ENTRY_HCU_CLIENT_SW;
		snd.hwType = zHcuSysEngPar.hwBurnId.hwType;
		snd.hwId = zHcuSysEngPar.hwBurnId.hwPemId;
		snd.swRel = zHcuSysEngPar.hwBurnId.swRelId;
		snd.swVer = zHcuSysEngPar.hwBurnId.swVerId;
		snd.dbVer = zHcuSysEngPar.hwBurnId.dbVerId;
		snd.upgradeFlag = zHcuSysEngPar.hwBurnId.swUpgradeFlag;
		snd.timeStamp = time(0);
		snd.length = sizeof(msg_struct_sysswm_cloudvela_inventory_report_t);
		if (hcu_message_send(MSG_ID_SYSSWM_CLOUDVELA_INVENTORY_REPORT, TASK_ID_CLOUDVELA, TASK_ID_SYSSWM, &snd, snd.length) == FAILURE)
			HCU_ERROR_PRINT_TASK(TASK_ID_SYSSWM, "SYSSWM: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_SYSSWM].taskName, zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName);

		//刷新版本界面
		func_sysswm_sw_inventory_req_info_trigger_ui(HCU_SYSSWM_SW_DOWNLOAD_SESSION_HCU_CLIENT, snd.swRel, snd.swVer, snd.dbVer);
	} //if ((FsmGetState(TASK_ID_CLOUDVELA) == FSM_STATE_CLOUDVELA_ONLINE) || (FsmGetState(TASK_ID_CLOUDVELA) == FSM_STATE_CLOUDVELA_OFFLINE) )

	//State no change
	return SUCCESS;
}

OPSTAT func_sysswm_time_out_period_working_scan_ihu_stable(void)
{
	//int ret=0;

	//停止定时器，防止上一次的操作还未结束
	hcu_timer_stop(TASK_ID_SYSSWM, TIMER_ID_1S_SYSSWM_SEG_DL_WAIT, TIMER_RESOLUTION_1S);

	//发送数据给后台
	if ((FsmGetState(TASK_ID_CLOUDVELA) == FSM_STATE_CLOUDVELA_ONLINE) || (FsmGetState(TASK_ID_CLOUDVELA) == FSM_STATE_CLOUDVELA_OFFLINE) ){
		msg_struct_sysswm_cloudvela_inventory_report_t snd;
		memset(&snd, 0, sizeof(msg_struct_sysswm_cloudvela_inventory_report_t));
		//L2信息
		strncpy(snd.comHead.destUser, zHcuSysEngPar.cloud.svrNameHome, strlen(zHcuSysEngPar.cloud.svrNameHome)<\
			sizeof(snd.comHead.destUser)?strlen(zHcuSysEngPar.cloud.svrNameHome):sizeof(snd.comHead.destUser));
		strncpy(snd.comHead.srcUser, zHcuSysEngPar.hwBurnId.equLable, strlen(zHcuSysEngPar.hwBurnId.equLable)<\
				sizeof(snd.comHead.srcUser)?strlen(zHcuSysEngPar.hwBurnId.equLable):sizeof(snd.comHead.srcUser));
		snd.comHead.timeStamp = time(0);
		snd.comHead.msgType = HUITP_MSG_HUIXML_MSGTYPE_COMMON_ID;
		strncpy(snd.comHead.funcFlag, zHcuSysEngPar.hwBurnId.hw_mac, strlen(zHcuSysEngPar.hwBurnId.hw_mac)<IHU_SYSMSG_BH_COM_HEAD_NAME_LEN_MAX?strlen(zHcuSysEngPar.hwBurnId.hw_mac):IHU_SYSMSG_BH_COM_HEAD_NAME_LEN_MAX);

		//CONTENT
		snd.baseReport = HUITP_IEID_UNI_COM_REPORT_YES;
		snd.equEntry = HCU_SYSMSG_SYSSWM_EQU_ENTRY_IHU_CLIENT;
		snd.hwType = zHcuSysEngPar.hwBurnId.nodeHwType;
		snd.hwId = zHcuSysEngPar.hwBurnId.nodeHwPemId;
		//数据库中搜寻
		memset(&(gTaskSysswmContext.cloudSwPkg), 0, sizeof(HcuSysMsgIeL3SysSwmSwPkgElement_t));
		if (dbi_HcuSysSwm_SwPkg_inquery_max_sw_ver(snd.equEntry, snd.hwType, snd.hwId, HCU_SYSCFG_HBB_FW_UPGRADE_YES_STABLE, &(gTaskSysswmContext.cloudSwPkg)) == FAILURE)
			HCU_ERROR_PRINT_TASK(TASK_ID_SYSSWM, "SYSSWM: Inquery max REL/VER ID error from database!\n");
		snd.hwId = gTaskSysswmContext.cloudSwPkg.hwPem; //找到的最新的PEMID对应的软件版本
		snd.swRel = gTaskSysswmContext.cloudSwPkg.swRel;
		snd.swVer = gTaskSysswmContext.cloudSwPkg.swVer;
		snd.dbVer = gTaskSysswmContext.cloudSwPkg.dbVer;
		snd.upgradeFlag = HCU_SYSCFG_HBB_FW_UPGRADE_YES_STABLE;
		snd.timeStamp = time(0);
		snd.length = sizeof(msg_struct_sysswm_cloudvela_inventory_report_t);
		if (hcu_message_send(MSG_ID_SYSSWM_CLOUDVELA_INVENTORY_REPORT, TASK_ID_CLOUDVELA, TASK_ID_SYSSWM, &snd, snd.length) == FAILURE)
			HCU_ERROR_PRINT_TASK(TASK_ID_SYSSWM, "SYSSWM: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_SYSSWM].taskName, zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName);

		//刷新版本界面
		func_sysswm_sw_inventory_req_info_trigger_ui(HCU_SYSSWM_SW_DOWNLOAD_SESSION_IHU_STABLE, snd.swRel, snd.swVer, snd.dbVer);
	}

	//State no change
	return SUCCESS;
}

OPSTAT func_sysswm_time_out_period_working_scan_ihu_trial(void)
{
	//int ret=0;

	//停止定时器，防止上一次的操作还未结束
	hcu_timer_stop(TASK_ID_SYSSWM, TIMER_ID_1S_SYSSWM_SEG_DL_WAIT, TIMER_RESOLUTION_1S);

	//发送数据给后台
	if ((FsmGetState(TASK_ID_CLOUDVELA) == FSM_STATE_CLOUDVELA_ONLINE) || (FsmGetState(TASK_ID_CLOUDVELA) == FSM_STATE_CLOUDVELA_OFFLINE) ){
		msg_struct_sysswm_cloudvela_inventory_report_t snd;
		memset(&snd, 0, sizeof(msg_struct_sysswm_cloudvela_inventory_report_t));
		//L2信息
		strncpy(snd.comHead.destUser, zHcuSysEngPar.cloud.svrNameHome, strlen(zHcuSysEngPar.cloud.svrNameHome)<\
			sizeof(snd.comHead.destUser)?strlen(zHcuSysEngPar.cloud.svrNameHome):sizeof(snd.comHead.destUser));
		strncpy(snd.comHead.srcUser, zHcuSysEngPar.hwBurnId.equLable, strlen(zHcuSysEngPar.hwBurnId.equLable)<\
				sizeof(snd.comHead.srcUser)?strlen(zHcuSysEngPar.hwBurnId.equLable):sizeof(snd.comHead.srcUser));
		snd.comHead.timeStamp = time(0);
		snd.comHead.msgType = HUITP_MSG_HUIXML_MSGTYPE_COMMON_ID;
		strncpy(snd.comHead.funcFlag, zHcuSysEngPar.hwBurnId.hw_mac, strlen(zHcuSysEngPar.hwBurnId.hw_mac)<IHU_SYSMSG_BH_COM_HEAD_NAME_LEN_MAX?strlen(zHcuSysEngPar.hwBurnId.hw_mac):IHU_SYSMSG_BH_COM_HEAD_NAME_LEN_MAX);

		//CONTENT
		snd.baseReport = HUITP_IEID_UNI_COM_REPORT_YES;
		snd.equEntry = HCU_SYSMSG_SYSSWM_EQU_ENTRY_IHU_CLIENT;
		snd.hwType = zHcuSysEngPar.hwBurnId.nodeHwType;
		snd.hwId = zHcuSysEngPar.hwBurnId.nodeHwPemId;
		//数据库中搜寻
		memset(&(gTaskSysswmContext.cloudSwPkg), 0, sizeof(HcuSysMsgIeL3SysSwmSwPkgElement_t));
		if (dbi_HcuSysSwm_SwPkg_inquery_max_sw_ver(snd.equEntry, snd.hwType, snd.hwId, HCU_SYSCFG_HBB_FW_UPGRADE_YES_TRIAL, &(gTaskSysswmContext.cloudSwPkg)) == FAILURE)
			HCU_ERROR_PRINT_TASK(TASK_ID_SYSSWM, "SYSSWM: Inquery max REL/VER ID error from database!\n");
		snd.hwId = gTaskSysswmContext.cloudSwPkg.hwPem; //找到的最新的PEMID对应的软件版本
		snd.swRel = gTaskSysswmContext.cloudSwPkg.swRel;
		snd.swVer = gTaskSysswmContext.cloudSwPkg.swVer;
		snd.dbVer = gTaskSysswmContext.cloudSwPkg.dbVer;
		snd.upgradeFlag = HCU_SYSCFG_HBB_FW_UPGRADE_YES_TRIAL;
		snd.timeStamp = time(0);
		snd.length = sizeof(msg_struct_sysswm_cloudvela_inventory_report_t);
		if (hcu_message_send(MSG_ID_SYSSWM_CLOUDVELA_INVENTORY_REPORT, TASK_ID_CLOUDVELA, TASK_ID_SYSSWM, &snd, snd.length) == FAILURE)
			HCU_ERROR_PRINT_TASK(TASK_ID_SYSSWM, "SYSSWM: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_SYSSWM].taskName, zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName);

		//刷新版本界面
		func_sysswm_sw_inventory_req_info_trigger_ui(HCU_SYSSWM_SW_DOWNLOAD_SESSION_IHU_TRIAL, snd.swRel, snd.swVer, snd.dbVer);
	}

	//State no change
	return SUCCESS;
}

OPSTAT func_sysswm_time_out_period_working_scan_ihu_patch(void)
{
	//int ret=0;

	//停止定时器，防止上一次的操作还未结束
	hcu_timer_stop(TASK_ID_SYSSWM, TIMER_ID_1S_SYSSWM_SEG_DL_WAIT, TIMER_RESOLUTION_1S);

	//发送数据给后台
	if ((FsmGetState(TASK_ID_CLOUDVELA) == FSM_STATE_CLOUDVELA_ONLINE) || (FsmGetState(TASK_ID_CLOUDVELA) == FSM_STATE_CLOUDVELA_OFFLINE) ){
		msg_struct_sysswm_cloudvela_inventory_report_t snd;
		memset(&snd, 0, sizeof(msg_struct_sysswm_cloudvela_inventory_report_t));
		//L2信息
		strncpy(snd.comHead.destUser, zHcuSysEngPar.cloud.svrNameHome, strlen(zHcuSysEngPar.cloud.svrNameHome)<\
			sizeof(snd.comHead.destUser)?strlen(zHcuSysEngPar.cloud.svrNameHome):sizeof(snd.comHead.destUser));
		strncpy(snd.comHead.srcUser, zHcuSysEngPar.hwBurnId.equLable, strlen(zHcuSysEngPar.hwBurnId.equLable)<\
				sizeof(snd.comHead.srcUser)?strlen(zHcuSysEngPar.hwBurnId.equLable):sizeof(snd.comHead.srcUser));
		snd.comHead.timeStamp = time(0);
		snd.comHead.msgType = HUITP_MSG_HUIXML_MSGTYPE_COMMON_ID;
		strncpy(snd.comHead.funcFlag, zHcuSysEngPar.hwBurnId.hw_mac, strlen(zHcuSysEngPar.hwBurnId.hw_mac)<IHU_SYSMSG_BH_COM_HEAD_NAME_LEN_MAX?strlen(zHcuSysEngPar.hwBurnId.hw_mac):IHU_SYSMSG_BH_COM_HEAD_NAME_LEN_MAX);

		//CONTENT
		snd.baseReport = HUITP_IEID_UNI_COM_REPORT_YES;
		snd.equEntry = HCU_SYSMSG_SYSSWM_EQU_ENTRY_IHU_CLIENT;
		snd.hwType = zHcuSysEngPar.hwBurnId.nodeHwType;
		snd.hwId = zHcuSysEngPar.hwBurnId.nodeHwPemId;
		//数据库中搜寻
		memset(&(gTaskSysswmContext.cloudSwPkg), 0, sizeof(HcuSysMsgIeL3SysSwmSwPkgElement_t));
		if (dbi_HcuSysSwm_SwPkg_inquery_max_sw_ver(snd.equEntry, snd.hwType, snd.hwId, HCU_SYSCFG_HBB_FW_UPGRADE_YES_PATCH, &(gTaskSysswmContext.cloudSwPkg)) == FAILURE)
			HCU_ERROR_PRINT_TASK(TASK_ID_SYSSWM, "SYSSWM: Inquery max REL/VER ID error from database!\n");
		snd.hwId = gTaskSysswmContext.cloudSwPkg.hwPem; //找到的最新的PEMID对应的软件版本
		snd.swRel = gTaskSysswmContext.cloudSwPkg.swRel;
		snd.swVer = gTaskSysswmContext.cloudSwPkg.swVer;
		snd.dbVer = gTaskSysswmContext.cloudSwPkg.dbVer;
		snd.upgradeFlag = HCU_SYSCFG_HBB_FW_UPGRADE_YES_PATCH;
		snd.timeStamp = time(0);
		snd.length = sizeof(msg_struct_sysswm_cloudvela_inventory_report_t);
		if (hcu_message_send(MSG_ID_SYSSWM_CLOUDVELA_INVENTORY_REPORT, TASK_ID_CLOUDVELA, TASK_ID_SYSSWM, &snd, snd.length) == FAILURE)
			HCU_ERROR_PRINT_TASK(TASK_ID_SYSSWM, "SYSSWM: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_SYSSWM].taskName, zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName);

		//刷新版本界面
		func_sysswm_sw_inventory_req_info_trigger_ui(HCU_SYSSWM_SW_DOWNLOAD_SESSION_IHU_PATCH, snd.swRel, snd.swVer, snd.dbVer);
	}

	//State no change
	return SUCCESS;
}

OPSTAT func_sysswm_time_out_segment_download_wait_process(void)
{
	int ret = SUCCESS;

	gTaskSysswmContext.reTransTimes++;
	if (gTaskSysswmContext.reTransTimes < HCU_SYSSWM_SW_PACKAGE_RETRANS_MAX_TIMES){
		ret = func_sysswm_send_cloudvela_sw_package_report();
		hcu_timer_start(TASK_ID_SYSSWM, TIMER_ID_1S_SYSSWM_SEG_DL_WAIT, zHcuSysEngPar.timer.array[TIMER_ID_1S_SYSSWM_SEG_DL_WAIT].dur, TIMER_TYPE_ONE_TIME, TIMER_RESOLUTION_1S);
	}

	return ret;
}

OPSTAT fsm_sysswm_canitfleo_inventory_report(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret=0;
	strTaskSysswmSwpkgLable_t input;

	//Receive message and copy to local variable
	msg_struct_canitfleo_sysswm_inventory_report_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_canitfleo_sysswm_inventory_report_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_canitfleo_sysswm_inventory_report_t)))
		HCU_ERROR_PRINT_TASK(TASK_ID_SYSSWM, "SYSSWM: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);

	//入参检查
	if ((rcv.upgradeFlag != HUITP_IEID_SUI_FW_UPGRADE_YES_STABLE) && (rcv.upgradeFlag != HUITP_IEID_SUI_FW_UPGRADE_YES_TRIAL) \
			&& (rcv.upgradeFlag != HUITP_IEID_SUI_FW_UPGRADE_YES_PATCH))
		HCU_ERROR_PRINT_TASK(TASK_ID_SYSSWM, "SYSSWM: Receive message error!\n");

	//返回消息
	msg_struct_sysswm_canitfleo_inventory_confirm_t snd;
	memset(&snd, 0, sizeof(msg_struct_sysswm_canitfleo_inventory_confirm_t));
	memset(&input, 0, sizeof(strTaskSysswmSwpkgLable_t));
	input.fileNameLen = sizeof(input.fPathName);

	//搜索
	if (func_sysswm_analysis_ihu_sw_package(rcv.hwType, rcv.hwId, rcv.swRel, rcv.swVer, rcv.upgradeFlag, &input) == SUCCESS){
		if (strlen(input.fPathName) != 0){
			//分析数据
			snd.swRel = input.swRel;
			snd.swVer = input.swVer;
			snd.swCheckSum = func_sysswm_caculate_file_whole_checksum(input.fPathName);
			snd.swTotalLengthInBytes = func_sysswm_caculate_file_length_in_bytes(input.fPathName);
		}
		//不成功
		else{
			snd.swRel = 0;
			snd.swVer = 0;
			snd.swCheckSum = 0;
			snd.swTotalLengthInBytes = 0;
		}
	}
	//不成功
	else{
		snd.swRel = 0;
		snd.swVer = 0;
		snd.swCheckSum = 0;
		snd.swTotalLengthInBytes = 0;
	}

/*	//采用数据库的方式进行操纵
	//数据库中搜寻
	memset(&(gTaskSysswmContext.cloudSwPkg), 0, sizeof(HcuSysMsgIeL3SysSwmSwPkgElement_t));
	if (dbi_HcuSysSwm_SwPkg_inquery_max_sw_ver(snd.equEntry, snd.hwType, snd.hwId, HCU_SYSCFG_HBB_FW_UPGRADE_YES_PATCH, &(gTaskSysswmContext.cloudSwPkg)) == FAILURE)
		HCU_ERROR_PRINT_TASK(TASK_ID_SYSSWM, "SYSSWM: Inquery max REL/VER ID error from database!\n");
	snd.hwId = gTaskSysswmContext.cloudSwPkg.hwPem; //找到的最新的PEMID对应的软件版本
	snd.swRel = gTaskSysswmContext.cloudSwPkg.swRel;
	snd.swVer = gTaskSysswmContext.cloudSwPkg.swVer;
	snd.dbVer = gTaskSysswmContext.cloudSwPkg.dbVer;
	snd.upgradeFlag = HCU_SYSCFG_HBB_FW_UPGRADE_YES_PATCH;
	snd.timeStamp = time(0);*/

	//固定填入的信息
	snd.upgradeFlag = rcv.upgradeFlag;
	snd.timeStamp = time(0);
	snd.nodeId = rcv.nodeId;
	snd.length = sizeof(msg_struct_sysswm_canitfleo_inventory_confirm_t);
	if (hcu_message_send(MSG_ID_SYSSWM_CANITFLEO_INVENTORY_CONFIRM, TASK_ID_CANITFLEO, TASK_ID_SYSSWM, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_TASK(TASK_ID_SYSSWM, "SYSSWM: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_SYSSWM].taskName, zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName);

	return SUCCESS;
}


//软件下载方案，打算采用一种硬件版本，只存储STABLE/TRIAL/PATCH的最新版本，不存储中间版本的情形，从而简化升级路径
//本地将根据硬件信息，读取文件名字，然后分段下载
//为了简化，并借助于淘宝文件的技巧，所有下位机软件版本包都存在目录下，版本信息全部使用文件名的方式进行隔离
//比如，采用IHU_HPT32769_PEM11_REL03_VER218_PATCH.BIN
OPSTAT fsm_sysswm_canitfleo_sw_package_report(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret=0;
	strTaskSysswmSwpkgSegment_t input;

	//Receive message and copy to local variable
	msg_struct_canitfleo_sysswm_sw_package_report_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_canitfleo_sysswm_sw_package_report_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_canitfleo_sysswm_sw_package_report_t)))
		HCU_ERROR_PRINT_TASK(TASK_ID_SYSSWM, "SYSSWM: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);

	//入参检查
	if ((rcv.segIndex > rcv.segTotal) || (rcv.segIndex == 0) || (rcv.segSplitLen == 0) || (rcv.segSplitLen > HCU_SYSMSG_CANITFLEO_SYSSWM_SW_PACKAGE_BODY_MAX_LEN)\
			|| (rcv.segTotal == 0) || (rcv.segSplitLen > HUITP_IEID_SUI_SW_PACKAGE_BODY_MAX_LEN))
		HCU_ERROR_PRINT_TASK(TASK_ID_SYSSWM, "SYSSWM: Receive message error!\n");

	//返回消息
	msg_struct_sysswm_canitfleo_sw_package_confirm_t snd;
	memset(&snd, 0, sizeof(msg_struct_sysswm_canitfleo_sw_package_confirm_t));
	memset(&input, 0, sizeof(strTaskSysswmSwpkgSegment_t));
	input.swRel = rcv.swRelId;
	input.swVer = rcv.swVerId;
	input.upgradeFlag = rcv.upgradeFlag;
	input.segIndex = rcv.segIndex;
	input.segTotal = rcv.segTotal;
	input.segSplitLen = rcv.segSplitLen;
	input.buf = snd.swPkgBody;
	if (func_sysswm_read_ihu_sw_package_segment(&input) == SUCCESS){
		snd.swRelId = rcv.swRelId;
		snd.swVerId = rcv.swVerId;
		snd.upgradeFlag = rcv.upgradeFlag;
		snd.segIndex = rcv.segIndex;
		snd.segTotal = rcv.segTotal;
		snd.segSplitLen = rcv.segSplitLen;
		snd.segValidLen = input.segValidLen;
		snd.segCheckSum = input.segCheckSum;
	}
	//分段读取失败
	else{
		snd.swRelId = 0;
		snd.swVerId = 0;
		snd.upgradeFlag = 0;
		snd.segIndex = 0;
		snd.segTotal = 0;
		snd.segSplitLen = 0;
		snd.segValidLen = 0;
		snd.segCheckSum = 0;
	}

	//固定填入的信息
	snd.nodeId = rcv.nodeId;
	snd.length = sizeof(msg_struct_sysswm_canitfleo_sw_package_confirm_t);
	if (hcu_message_send(MSG_ID_SYSSWM_CANITFLEO_SW_PACKAGE_CONFIRM, TASK_ID_CANITFLEO, TASK_ID_SYSSWM, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_TASK(TASK_ID_SYSSWM, "SYSSWM: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_SYSSWM].taskName, zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName);

	HCU_DEBUG_PRINT_CRT("SYSSWM: Snd pkg REL=%d, VER=%d, FLAG=%d, INDEX=%d, SegTotal=%d, SegSplitLen=%d, SegValidLen=%d, ChkSum=%d, NodeId = %d\n",\
			snd.swRelId, snd.swVerId, snd.upgradeFlag, snd.segIndex, snd.segTotal, snd.segSplitLen, snd.segValidLen, snd.segCheckSum, snd.nodeId);
	return SUCCESS;
}

//用来分析是否存在IHU软件体，是否存在
//分析的结果，填入fname中
//文件名字格式：IHU_HPT32769_PEM11_REL03_VER218_PATCH.BIN
//下载的软件版本，必须按照hwType+ugradeFlag进行搜索，全部删除后，再进行下载，不然会出现重复软件版本的问题
//返回错误，或者strlen(fname)==0，表示没找到
OPSTAT func_sysswm_analysis_ihu_sw_package(UINT16 hwType, UINT16 hwId, UINT16 swRel, UINT16 swVer, UINT8 upgradeFlag, strTaskSysswmSwpkgLable_t* output)
{
	//zHcuSysEngPar.swm.hcuSwActiveDir
	//遍历文件目录下的所有文件，获取文件名字
	DIR *dir;
	struct dirent *ptr;
	char *p1, *p2, *p3, *p4, *p5, *p6, *p7;
	UINT32 res = 0;
	char s[6];
	UINT16 tmpRel=0;
	UINT16 tmpVer=0;

	//首先强制赋值
	output->swRel = 0;
	output->swVer = 0;

	//继续搜寻最大的
	if ((dir=opendir(zHcuSysEngPar.swm.hcuSwActiveDir)) == NULL)
		HCU_ERROR_PRINT_TASK(TASK_ID_SYSSWM, "SYSSWM: Open dir error!\n");
	while ((ptr=readdir(dir)) != NULL)
	{
	    if(strcmp(ptr->d_name,".")==0 || strcmp(ptr->d_name,"..")==0){    ///current dir OR parrent dir
	        continue;
	    }
	    else if ((ptr->d_type == 8) || (ptr->d_type == 10)){    ///File or Link file
	    	//判定关键字
	        p1 = strstr(ptr->d_name, "IHU");
	        p2 = strstr(ptr->d_name, "_HPT");
	        p3 = strstr(ptr->d_name, "_PEM");
	        p4 = strstr(ptr->d_name, "_REL");
	        p5 = strstr(ptr->d_name, "_VER");
	        p6 = strstr(ptr->d_name, ".BIN");
	        if ((p1 == NULL) || (p2 == NULL) || (p3 == NULL) || (p4 == NULL) || (p5 == NULL) || (p6 == NULL)){
	        	continue;
	        }
	        //判定升级标签
	        if (upgradeFlag == HCU_SYSMSG_SYSSWM_FW_UPGRADE_YES_STABLE)  p7 = strstr(ptr->d_name, "_STABLE");
	        else if (upgradeFlag == HCU_SYSMSG_SYSSWM_FW_UPGRADE_YES_TRIAL)  p7 = strstr(ptr->d_name, "_TRIAL");
	        else if (upgradeFlag == HCU_SYSMSG_SYSSWM_FW_UPGRADE_YES_PATCH)  p7 = strstr(ptr->d_name, "_PATCH");
	        else continue;
	        if (p7 == NULL) continue;

	        //读取标识hwType
	        p2 = p2 + strlen("_HPT");
	        if (((p3-p2) <=0) || ((p3-p2) > 5)) continue;
	        memset(s, 0, sizeof(s));
	        strncpy(s, p2, p3-p2);
	        res = strtoul(s, NULL, 10);
	        if (res != hwType) continue;
	        output->hwType = res & 0xFFFF;
	        //读取标识hwId
	        p3 = p3 + strlen("_PEM");
	        if (((p4-p3) <=0) || ((p4-p3) > 5)) continue;
	        memset(s, 0, sizeof(s));
	        strncpy(s, p3, p4-p3);
	        res = strtoul(s, NULL, 10);
	        if (res < hwId) continue; //这里的软件版本，如果目标PEM版本高于实际硬件，是可以前向兼容的，不允许后项兼容的
	        output->hwId = res  & 0xFFFF;
	        //读取标识swRel
	        p4 = p4 + strlen("_REL");
	        if (((p5-p4) <=0) || ((p5-p4) > 5)) continue;
	        memset(s, 0, sizeof(s));
	        strncpy(s, p4, p5-p4);
	        res = strtoul(s, NULL, 10);
	        if (res < swRel) continue;  //这里的软件版本，如果目标PEM版本高于实际硬件，是可以前向兼容的，不允许后项兼容的
	        tmpRel = res & 0xFFFF;
	        //读取标识swVer
	        p5 = p5 + strlen("_VER");
	        if (((p7-p5) <=0) || ((p7-p5) > 5)) continue;
	        memset(s, 0, sizeof(s));
	        strncpy(s, p5, p7-p5);
	        res = strtoul(s, NULL, 10);
	        if (res < swVer) continue;  //这里的软件版本，如果目标PEM版本高于实际硬件，是可以前向兼容的，不允许后项兼容的
	        tmpVer = res  & 0xFFFF;

	        //找到合适的目标
	        if (tmpRel > output->swRel)
	        {
	        	output->swRel = tmpRel;
	        	output->swVer = tmpVer;
			    //找到了，干活，返回完整的文件目录和文件名字
	        	memset(output->fPathName, 0, sizeof(output->fPathName));
			    if (zHcuSysEngPar.swm.hcuSwActiveDir[strlen(zHcuSysEngPar.swm.hcuSwActiveDir)-1] == '/'){
			    	strncpy(output->fPathName, zHcuSysEngPar.swm.hcuSwActiveDir, (strlen(zHcuSysEngPar.swm.hcuSwActiveDir)<output->fileNameLen)?strlen(zHcuSysEngPar.swm.hcuSwActiveDir):output->fileNameLen);
			    	strncat(output->fPathName, ptr->d_name, (strlen(ptr->d_name) < output->fileNameLen - strlen(output->fPathName))?strlen(ptr->d_name):output->fileNameLen - strlen(output->fPathName));
			    }
			    else{
			    	strncpy(output->fPathName, zHcuSysEngPar.swm.hcuSwActiveDir, (strlen(zHcuSysEngPar.swm.hcuSwActiveDir)<output->fileNameLen)?strlen(zHcuSysEngPar.swm.hcuSwActiveDir):output->fileNameLen);
			    	strcat(output->fPathName, "/");
			    	strncat(output->fPathName, ptr->d_name, (strlen(ptr->d_name) < output->fileNameLen - strlen(output->fPathName))?strlen(ptr->d_name):output->fileNameLen - strlen(output->fPathName));
			    }
			    //恢复0值
	        	tmpRel=0;
	        	tmpVer=0;
	        }
	        else if ((tmpRel == output->swRel) && (tmpVer > output->swVer))
	        {
	        	output->swVer = tmpVer;
			    //找到了，干活，返回完整的文件目录和文件名字
		        memset(output->fPathName, 0, sizeof(output->fPathName));
			    if (zHcuSysEngPar.swm.hcuSwActiveDir[strlen(zHcuSysEngPar.swm.hcuSwActiveDir)-1] == '/'){
			    	strncpy(output->fPathName, zHcuSysEngPar.swm.hcuSwActiveDir, (strlen(zHcuSysEngPar.swm.hcuSwActiveDir)<output->fileNameLen)?strlen(zHcuSysEngPar.swm.hcuSwActiveDir):output->fileNameLen);
			    	strncat(output->fPathName, ptr->d_name, (strlen(ptr->d_name) < output->fileNameLen - strlen(output->fPathName))?strlen(ptr->d_name):output->fileNameLen - strlen(output->fPathName));
			    }
			    else{
			    	strncpy(output->fPathName, zHcuSysEngPar.swm.hcuSwActiveDir, (strlen(zHcuSysEngPar.swm.hcuSwActiveDir)<output->fileNameLen)?strlen(zHcuSysEngPar.swm.hcuSwActiveDir):output->fileNameLen);
			    	strcat(output->fPathName, "/");
			    	strncat(output->fPathName, ptr->d_name, (strlen(ptr->d_name) < output->fileNameLen - strlen(output->fPathName))?strlen(ptr->d_name):output->fileNameLen - strlen(output->fPathName));
			    }
			    //恢复0值
	        	tmpRel=0;
	        	tmpVer=0;
	        }
	    }
	    else if(ptr->d_type == 4)    ///dir
	    {
	    	//循环调用本函数，这个技巧这里暂时不用，但保留
	    	//char base[1000];
	        //memset(base,'\0',sizeof(base));
	        //strcpy(base,zHcuSysEngPar.swm.hcuSwActiveDir);
	        //strcat(base,"/");
	        //strcat(base,ptr->d_name);
	        //readFileList(base);
	    }
	}

	//结束
	closedir(dir);
	//找到了合法的
	if ((output->swRel > 0) || (output->swVer > 0))
	{
	    return SUCCESS;
	}
	else{
		return FAILURE;
	}
}

//具体读取某一个文件的一个分段
OPSTAT func_sysswm_read_ihu_sw_package_segment(strTaskSysswmSwpkgSegment_t *input)
{
	//zHcuSysEngPar.swm.hcuSwActiveDir
	//遍历文件目录下的所有文件，获取文件名字
	DIR *dir;
	struct dirent *ptr;
	char *p1, *p2, *p3, *p4, *p5, *p6, *p7;
	UINT32 res = 0;
	char s[6];
	char fname[200];

	if ((dir=opendir(zHcuSysEngPar.swm.hcuSwActiveDir)) == NULL)
		HCU_ERROR_PRINT_TASK(TASK_ID_SYSSWM, "SYSSWM: Open dir error!\n");

	while ((ptr=readdir(dir)) != NULL)
	{
	    if(strcmp(ptr->d_name,".")==0 || strcmp(ptr->d_name,"..")==0)    ///current dir OR parrent dir
	        continue;
	    else if ((ptr->d_type == 8) || (ptr->d_type == 10)){    ///File or Link file
	    	//判定关键字
	        p1 = strstr(ptr->d_name, "IHU");
	        p2 = strstr(ptr->d_name, "_HPT");
	        p3 = strstr(ptr->d_name, "_PEM");
	        p4 = strstr(ptr->d_name, "_REL");
	        p5 = strstr(ptr->d_name, "_VER");
	        p6 = strstr(ptr->d_name, ".BIN");
	        if ((p1 == NULL) || (p2 == NULL) || (p3 == NULL) || (p4 == NULL) || (p5 == NULL) || (p6 == NULL))
	        	continue;

	        //判定升级标签
	        if (input->upgradeFlag == HCU_SYSMSG_SYSSWM_FW_UPGRADE_YES_STABLE)  p7 = strstr(ptr->d_name, "_STABLE");
	        else if (input->upgradeFlag == HCU_SYSMSG_SYSSWM_FW_UPGRADE_YES_TRIAL)  p7 = strstr(ptr->d_name, "_TRIAL");
	        else if (input->upgradeFlag == HCU_SYSMSG_SYSSWM_FW_UPGRADE_YES_PATCH)  p7 = strstr(ptr->d_name, "_PATCH");
	        else continue;
	        if (p7 == NULL) continue;

	        //读取标识swRel
	        p4 = p4 + strlen("_REL");
	        if (((p5-p4) <=0) || ((p5-p4) > 5)) continue;
	        memset(s, 0, sizeof(s));
	        strncpy(s, p4, p5-p4);
	        res = strtoul(s, NULL, 10);
	        if (res != input->swRel) continue;
	        //读取标识swVer
	        p5 = p5 + strlen("_VER");
	        if (((p7-p5) <=0) || ((p7-p5) > 5)) continue;
	        memset(s, 0, sizeof(s));
	        strncpy(s, p5, p7-p5);
	        res = strtoul(s, NULL, 10);
	        if (res != input->swVer) continue;

	        //找到了，干活，返回完整的文件目录和文件名字
	        memset(fname, 0, sizeof(fname));
	        if (zHcuSysEngPar.swm.hcuSwActiveDir[strlen(zHcuSysEngPar.swm.hcuSwActiveDir)-1] == '/'){
	        	sprintf(fname, "%s%s", zHcuSysEngPar.swm.hcuSwActiveDir, ptr->d_name);
	        }
	        else{
	        	sprintf(fname, "%s/%s", zHcuSysEngPar.swm.hcuSwActiveDir, ptr->d_name);
	        }

	    	FILE *fp;
	    	int bytes_read = 0, i = 0;
	    	int file_len=0;
	    	UINT8 *pRecord;
	    	UINT8 *ptr;
	    	UINT8 tmp = 0;

	    	//打开源文件
	    	if((fp=fopen(fname, "rb"))== NULL){
	    		closedir(dir);
	    		HCU_ERROR_PRINT_TASK(TASK_ID_SYSSWM, "SYSSWM: Open %s Error!\n", fname);
	    	}

	    	//测得文件大小
	    	fseek(fp, 0L, SEEK_END);
	    	file_len = ftell(fp);
	    	fseek(fp, 0L, SEEK_SET);
	    	if (file_len <=0){
	    		fclose(fp);
	    		closedir(dir);
	    		HCU_ERROR_PRINT_TASK(TASK_ID_SYSSWM, "SYSSWM: No file content, file_len =%d!\n", file_len);
	    	}

	    	//读取文件
	    	pRecord = malloc(file_len);
	    	if (pRecord==NULL){
	    		fclose(fp);
	    		closedir(dir);
	    		HCU_ERROR_PRINT_TASK(TASK_ID_SYSSWM, "SYSSWM: Allocate memory fail!\n");
	    	}
	    	ptr = pRecord;
	    	bytes_read=fread(pRecord, 1, file_len, fp);
	    	if (bytes_read != file_len){
	    		free(pRecord);
	    		fclose(fp);
	    		closedir(dir);
	    		HCU_ERROR_PRINT_TASK(TASK_ID_SYSSWM, "SYSSWM: Read file fail, byte_read = %d, file_len = %d!\n", bytes_read, file_len);
	    	}

	    	//分段计算
	    	res = 0;
	    	if (((input->segTotal-1) * input->segSplitLen > file_len) || (input->segTotal * input->segSplitLen < file_len) || (input->segIndex > input->segTotal) || (input->segIndex ==0)){
	    		free(pRecord);
	    		fclose(fp);
	    		closedir(dir);
	    		HCU_ERROR_PRINT_TASK(TASK_ID_SYSSWM, "SYSSWM: File split error, segTotal = %d, Split len = %d, File len = %d!\n", input->segTotal, input->segSplitLen, file_len);
	    	}

	    	//访问数据：分段segIndex必须从１－Ｎ
	    	ptr += input->segSplitLen * (input->segIndex-1);
	    	//最后一段
	    	if (input->segIndex == input->segTotal){
	    		input->segValidLen = file_len - ((input->segTotal-1) * input->segSplitLen);
	    		memcpy(input->buf, ptr, input->segValidLen);
	    	}
	    	//正常段
	    	else{
	    		input->segValidLen = input->segSplitLen;
	    		memcpy(input->buf, ptr, input->segValidLen);
	    	}
	    	//计算分段CHECKSUM
	    	input->segCheckSum = 0;
	    	for (i=0; i<input->segValidLen; i++){
	    		memcpy(&tmp, input->buf+i, 1);
	    		input->segCheckSum += tmp;
	    	}
	        //找到第一个就结束，不再去寻找第二个，所以必须进行定期扫描
	    	free(pRecord);
	    	fclose(fp);
	        closedir(dir);
	        return SUCCESS;
	    }
	    else if(ptr->d_type == 4)    ///dir
	    {
	    	//循环调用本函数，这个技巧这里暂时不用，但保留
	    	//char base[1000];
	        //memset(base,'\0',sizeof(base));
	        //strcpy(base,zHcuSysEngPar.swm.hcuSwActiveDir);
	        //strcat(base,"/");
	        //strcat(base,ptr->d_name);
	        //readFileList(base);
	    }
	}
	closedir(dir);
	return FAILURE;
}

//下载的软件版本，必须按照hwType+ugradeFlag进行搜索，全部删除后，再进行下载，不然会出现重复软件版本的问题
//下载之前调用该函数
OPSTAT func_sysswm_delete_ihu_redundance_sw_package(UINT16 hwType, UINT8 upgradeFlag)
{
	//zHcuSysEngPar.swm.hcuSwActiveDir
	//遍历文件目录下的所有文件，获取文件名字
	DIR *dir;
	struct dirent *ptr;
	char *p1, *p2, *p3, *p4, *p5, *p6, *p7;
	UINT32 res = 0;
	char s[6];

	if ((dir=opendir(zHcuSysEngPar.swm.hcuSwActiveDir)) == NULL)
		HCU_ERROR_PRINT_TASK(TASK_ID_SYSSWM, "SYSSWM: Open dir error!\n");

	while ((ptr=readdir(dir)) != NULL)
	{
	    if(strcmp(ptr->d_name,".")==0 || strcmp(ptr->d_name,"..")==0)    ///current dir OR parrent dir
	        continue;
	    else if ((ptr->d_type == 8) || (ptr->d_type == 10)){    ///File or Link file
	    	//判定关键字
	    	if (ptr->d_name == NULL) continue;
	        p1 = strstr(ptr->d_name, "IHU");
	        p2 = strstr(ptr->d_name, "_HPT");
	        p3 = strstr(ptr->d_name, "_PEM");
	        p4 = strstr(ptr->d_name, "_REL");
	        p5 = strstr(ptr->d_name, "_VER");
	        p6 = strstr(ptr->d_name, ".BIN");
	        if ((p1 == NULL) || (p2 == NULL) || (p3 == NULL) || (p4 == NULL) || (p5 == NULL) || (p6 == NULL))
	        	continue;
	        //判定升级标签
	        if (upgradeFlag == HCU_SYSMSG_SYSSWM_FW_UPGRADE_YES_STABLE)  p7 = strstr(ptr->d_name, "_STABLE");
	        else if (upgradeFlag == HCU_SYSMSG_SYSSWM_FW_UPGRADE_YES_TRIAL)  p7 = strstr(ptr->d_name, "_TRIAL");
	        else if (upgradeFlag == HCU_SYSMSG_SYSSWM_FW_UPGRADE_YES_PATCH)  p7 = strstr(ptr->d_name, "_PATCH");
	        else continue;
	        if (p7 == NULL) continue;

	        //读取标识hwType
	        p2 = p2 + strlen("_HPT");
	        if (((p3-p2) <=0) || ((p3-p2) > 5)) continue;
	        memset(s, 0, sizeof(s));
	        strncpy(s, p2, p3-p2);
	        res = strtoul(s, NULL, 10);
	        if (res != hwType) continue;

	        //找到了，干活
	        char str[200];
	        memset(str, 0, sizeof(str));
	        if (strlen(zHcuSysEngPar.swm.hcuSwActiveDir) == 0){
	        	closedir(dir);
	        	HCU_ERROR_PRINT_TASK(TASK_ID_SYSSWM, "SYSSWM: System HCU SW DIR set error!\n");
	        }
	        if (zHcuSysEngPar.swm.hcuSwActiveDir[strlen(zHcuSysEngPar.swm.hcuSwActiveDir)-1] == '/'){
		        sprintf(str, "rm %s%s", zHcuSysEngPar.swm.hcuSwActiveDir, ptr->d_name);
	        }
	        else{
		        sprintf(str, "rm %s/%s", zHcuSysEngPar.swm.hcuSwActiveDir, ptr->d_name);
	        }
	        HCU_DEBUG_PRINT_CRT("SYSSWM: Rm files = %s\n", str);
	        system(str);
	    }
	    else if(ptr->d_type == 4)    ///dir
	    {
	    	//循环调用本函数，这个技巧这里暂时不用，但保留
	    	//char base[1000];
	        //memset(base,'\0',sizeof(base));
	        //strcpy(base,zHcuSysEngPar.swm.hcuSwActiveDir);
	        //strcat(base,"/");
	        //strcat(base,ptr->d_name);
	        //readFileList(base);
	    }
	}
	closedir(dir);
	return SUCCESS;
}

UINT16 func_sysswm_caculate_file_whole_checksum(char *fname)
{
	FILE *fp;
	int bytes_read = 0, i = 0;
	int file_len=0;
	UINT8 *pRecord;
	UINT16 res = 0;
	UINT8 tmp = 0;

	//打开源文件
	if((fp=fopen(fname, "rb"))== NULL){
		HcuErrorPrint("SYSSWM: Open %s Error!\n", fname);
		return 0;
	}

	//测得文件大小
	fseek(fp, 0L, SEEK_END);
	file_len = ftell(fp);
	fseek(fp, 0L, SEEK_SET);
	if (file_len <=0){
		HcuErrorPrint("SYSSWM: No file content, file len = %d!\n", file_len);
		fclose(fp);
		return 0;
	}

	//读取文件
	pRecord = malloc(file_len);
	if (pRecord==NULL){
		HcuErrorPrint("SYSSWM: Allocate memory fail!\n");
		fclose(fp);
		return 0;
	}
	bytes_read=fread(pRecord, 1, file_len, fp);
	if (bytes_read != file_len){
		HcuErrorPrint("SYSSWM: Read file fail, byte_read = %d, file_len = %d!\n", bytes_read, file_len);
		free(pRecord);
		fclose(fp);
		return 0;
	}

	res = 0;
	for (i=0; i<file_len; i++){
		memcpy(&tmp, pRecord+i, 1);
		res += tmp;
	}

	free(pRecord);
	fclose(fp);
	return res;
}

UINT32 func_sysswm_caculate_file_length_in_bytes(char *fname)
{
	FILE *fp;
	int file_len=0;

	//打开源文件
	if((fp=fopen(fname, "rb"))== NULL){
		HcuErrorPrint("SYSSWM: Open %s Error!\n", fname);
		return 0;
	}

	//测得文件大小
	fseek(fp, 0L, SEEK_END);
	file_len = ftell(fp);
	fseek(fp, 0L, SEEK_SET);
	if (file_len <=0){
		HcuErrorPrint("SYSSWM: No file content, file len = %d!\n", file_len);
		fclose(fp);
		return 0;
	}

	fclose(fp);
	return file_len;
}

OPSTAT func_sysswm_send_cloudvela_sw_package_report(void)
{
	//ret = 0;

	if ((FsmGetState(TASK_ID_CLOUDVELA) == FSM_STATE_CLOUDVELA_ONLINE) || (FsmGetState(TASK_ID_CLOUDVELA) == FSM_STATE_CLOUDVELA_OFFLINE) ){
		//生成消息并发送给后台
		msg_struct_sysswm_cloudvela_sw_package_report_t snd;
		memset(&snd, 0, sizeof(msg_struct_sysswm_cloudvela_sw_package_report_t));

		//L2信息
		strncpy(snd.comHead.destUser, zHcuSysEngPar.cloud.svrNameHome, strlen(zHcuSysEngPar.cloud.svrNameHome)<\
			sizeof(snd.comHead.destUser)?strlen(zHcuSysEngPar.cloud.svrNameHome):sizeof(snd.comHead.destUser));
		strncpy(snd.comHead.srcUser, zHcuSysEngPar.hwBurnId.equLable, strlen(zHcuSysEngPar.hwBurnId.equLable)<\
				sizeof(snd.comHead.srcUser)?strlen(zHcuSysEngPar.hwBurnId.equLable):sizeof(snd.comHead.srcUser));

		snd.comHead.timeStamp = time(0);
		snd.comHead.msgType = HUITP_MSG_HUIXML_MSGTYPE_COMMON_ID;
		strncpy(snd.comHead.funcFlag, zHcuSysEngPar.hwBurnId.hw_mac, strlen(zHcuSysEngPar.hwBurnId.hw_mac)<IHU_SYSMSG_BH_COM_HEAD_NAME_LEN_MAX?strlen(zHcuSysEngPar.hwBurnId.hw_mac):IHU_SYSMSG_BH_COM_HEAD_NAME_LEN_MAX);

		snd.baseReport = HUITP_IEID_UNI_COM_REPORT_YES;
		snd.equEntry = gTaskSysswmContext.cloudSwDl.equEntry;
		snd.hwType = gTaskSysswmContext.cloudSwDl.hwType;
		snd.hwPem = gTaskSysswmContext.cloudSwDl.hwPem;
		snd.swRelId = gTaskSysswmContext.cloudSwDl.swRel;
		snd.verId = gTaskSysswmContext.cloudSwDl.verId;
		snd.upgradeFlag = gTaskSysswmContext.cloudSwDl.upgradeFlag;
		snd.segIndex = gTaskSysswmContext.cloudSwDl.segIndex;
		snd.segTotal = gTaskSysswmContext.cloudSwDl.segTotal;
		snd.segSplitLen = gTaskSysswmContext.cloudSwDl.segSplitLen;

		snd.length = sizeof(msg_struct_sysswm_cloudvela_sw_package_report_t);
		if (hcu_message_send(MSG_ID_SYSSWM_CLOUDVELA_SW_PACKAGE_REPORT, TASK_ID_CLOUDVELA, TASK_ID_SYSSWM, &snd, snd.length) == FAILURE)
			HCU_ERROR_PRINT_SYSSWM("SYSSWM: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_SYSSWM].taskName, zHcuVmCtrTab.task[TASK_ID_CLOUDVELA].taskName);
	}

	//启动定时器
	hcu_timer_start(TASK_ID_SYSSWM, TIMER_ID_1S_SYSSWM_SEG_DL_WAIT, zHcuSysEngPar.timer.array[TIMER_ID_1S_SYSSWM_SEG_DL_WAIT].dur, TIMER_TYPE_ONE_TIME, TIMER_RESOLUTION_1S);

	//返回
	return SUCCESS;
}

//删去hcu_new, hcu两个执行文件
//脚本程序将专注执行：CRON定时执行，监控hcu是否存在，如果不存在，cp hew_new hcu，stop hcu, start hcu
//这样脚本程序将成为环境的一部分，而且是万能的，不需要根据不同项目进行复杂的配置
void func_sysswm_copy_exe_to_target_dir_and_restart(void)
{
	char strOpr[200];

	//先删掉hcu_new执行该文件
	memset(strOpr, 0, sizeof(strOpr));
	//不能删去hcu文件
	//sprintf(strOpr, "rm %shcu", zHcuSysEngPar.swm.hcuSwRunDir);
	if (access(&strOpr[3], F_OK) == 0) system(strOpr);
	sprintf(strOpr, "rm %shcu_new", zHcuSysEngPar.swm.hcuSwRunDir);
	if (access(&strOpr[3], F_OK) == 0) system(strOpr);
	//拷贝当前文件到目标目录
	sprintf(strOpr, "cp %s%s %shcu_new", zHcuSysEngPar.swm.hcuSwActiveDir, gTaskSysswmContext.cloudSwPkg.fileName, zHcuSysEngPar.swm.hcuSwRunDir);
	system(strOpr);

	return;
}

void func_sysswm_copy_db_and_exe_to_target_dir_and_restart(void)
{
	char strOpr[200];

	//先删掉hcu_new执行该文件
	memset(strOpr, 0, sizeof(strOpr));
	//hcu文件不能被删掉
	//sprintf(strOpr, "rm %shcu", zHcuSysEngPar.swm.hcuSwRunDir);
	if (access(&strOpr[3], F_OK) == 0) system(strOpr);
	sprintf(strOpr, "rm %shcu_new", zHcuSysEngPar.swm.hcuSwRunDir);
	if (access(&strOpr[3], F_OK) == 0) system(strOpr);
	sprintf(strOpr, "rm %shcu_new.sql", zHcuSysEngPar.swm.hcuSwRunDir);
	if (access(&strOpr[3], F_OK) == 0) system(strOpr);
	//拷贝当前文件到目标目录
	sprintf(strOpr, "cp %s%s %shcu_new", zHcuSysEngPar.swm.hcuSwActiveDir, gTaskSysswmContext.cloudSwPkg.fileName, zHcuSysEngPar.swm.hcuSwRunDir);
	system(strOpr);
	sprintf(strOpr, "cp %s%s %shcu_new.sql", zHcuSysEngPar.swm.hcuSwActiveDir, gTaskSysswmContext.cloudSwPkg.dbName, zHcuSysEngPar.swm.hcuSwRunDir);
	system(strOpr);

	return;
}

OPSTAT func_sysswm_sw_download_process_swpkg_db_refresh(char *stmp)
{
	if (func_sysswm_caculate_file_whole_checksum(stmp) != gTaskSysswmContext.cloudSwPkg.swCksum)
		HCU_ERROR_PRINT_SYSSWM("SYSSWM: SW package checksum error! Caculated Cksum=0x%x, RecvChsum=0x%x\n", func_sysswm_caculate_file_whole_checksum(stmp), gTaskSysswmContext.cloudSwPkg.swCksum);
	if (gTaskSysswmContext.cloudSwPkg.dbVer > zHcuSysEngPar.hwBurnId.dbVerId){
		strncpy(gTaskSysswmContext.cloudSwPkg.currentActive, HCU_SYSMSG_SYSSWM_CUR_ACTIVE_HALF_COMP, sizeof(HCU_SYSMSG_SYSSWM_CUR_ACTIVE_HALF_COMP));  //Complete of download
	}else{
		strncpy(gTaskSysswmContext.cloudSwPkg.currentActive, HCU_SYSMSG_SYSSWM_CUR_ACTIVE_COMPLETE, sizeof(HCU_SYSMSG_SYSSWM_CUR_ACTIVE_COMPLETE));  //Complete of download
	}
	gTaskSysswmContext.cloudSwPkg.updateTime = time(0);
	//更新软件数据库
	if (dbi_HcuSysSwm_SwPkg_save(&(gTaskSysswmContext.cloudSwPkg)) == FAILURE)
		HCU_ERROR_PRINT_SYSSWM("SYSSWM: After download complete, save sw pakcage error!\n");

	return SUCCESS;
}

OPSTAT func_sysswm_db_download_process_swpkg_db_refresh(char *stmp)
{
	if (func_sysswm_caculate_file_whole_checksum(stmp) != gTaskSysswmContext.cloudSwPkg.dbCksum)
		HCU_ERROR_PRINT_SYSSWM("SYSSWM: db file checksum error!, Caculated=0x%x, Exptected=0x%x\n", func_sysswm_caculate_file_whole_checksum(stmp), gTaskSysswmContext.cloudSwPkg.dbCksum);
	strncpy(gTaskSysswmContext.cloudSwPkg.currentActive, HCU_SYSMSG_SYSSWM_CUR_ACTIVE_COMPLETE, sizeof(HCU_SYSMSG_SYSSWM_CUR_ACTIVE_COMPLETE));  //Complete of download
	gTaskSysswmContext.cloudSwPkg.updateTime = time(0);
	//更新软件数据库
	if (dbi_HcuSysSwm_SwPkg_save(&(gTaskSysswmContext.cloudSwPkg)) == FAILURE)
		HCU_ERROR_PRINT_SYSSWM("SYSSWM: After download complete, save sw pakcage error!\n");

	return SUCCESS;
}

OPSTAT func_sysswm_swpkg_last_seg_process_hcu_sw(char *stmp)
{
	char s[200];
	bool flag = FALSE;
	//先判定是否需要升级DB
	flag = ((gTaskSysswmContext.cloudSwPkg.swRel == zHcuSysEngPar.hwBurnId.swRelId) && (gTaskSysswmContext.cloudSwPkg.dbVer <= zHcuSysEngPar.hwBurnId.dbVerId));

	//swpkg数据表单刷新
	if (func_sysswm_sw_download_process_swpkg_db_refresh(stmp) == FAILURE) return FAILURE;

	//修改下载后的文件属性
	sprintf(s, "chmod -R 777 %s", zHcuSysEngPar.swm.hcuSwActiveDir);
	system(s);

	//保留老旧数据一般ＤＢ出错时恢复
	gTaskSysswmContext.swRelSave = zHcuSysEngPar.hwBurnId.swRelId;
	gTaskSysswmContext.swVerSave = zHcuSysEngPar.hwBurnId.swVerId;

	//升级系统区的软件版本
	zHcuSysEngPar.hwBurnId.swRelId = gTaskSysswmContext.cloudSwPkg.swRel;
	zHcuSysEngPar.hwBurnId.swVerId = gTaskSysswmContext.cloudSwPkg.swVer;

	//升级BOOT区参数
	if (hcu_vm_engpar_update_phy_boot_sw_ver(zHcuSysEngPar.hwBurnId.swRelId, zHcuSysEngPar.hwBurnId.swVerId) == FAILURE)
		HCU_ERROR_PRINT_SYSSWM("SYSSWM: Update local configure file REL/VER ID error!\n");

	//更新版本按钮
	func_sysswm_hcusw_upgrade_info_trigger_ui();

	//拷贝文件到目标区并执行重启任务
	if (flag == TRUE)
	{
		func_sysswm_copy_exe_to_target_dir_and_restart();
	}
	//继续传输DB文件
	else
	{
		//刷新下载初始化Context
		memset(&(gTaskSysswmContext.cloudSwDl), 0, sizeof(HcuSysMsgIeL3SysSwmSwDlElement_t));
		gTaskSysswmContext.cloudSwDl.equEntry = HCU_SYSMSG_SYSSWM_EQU_ENTRY_HCU_CLIENT_DB;
		gTaskSysswmContext.cloudSwDl.hwType = gTaskSysswmContext.cloudSwPkg.hwType;
		gTaskSysswmContext.cloudSwDl.hwPem = gTaskSysswmContext.cloudSwPkg.hwPem;
		gTaskSysswmContext.cloudSwDl.swRel = gTaskSysswmContext.cloudSwPkg.swRel;
		gTaskSysswmContext.cloudSwDl.verId = gTaskSysswmContext.cloudSwPkg.dbVer;
		gTaskSysswmContext.cloudSwDl.upgradeFlag = gTaskSysswmContext.cloudSwPkg.upgradeFlag;
		gTaskSysswmContext.cloudSwDl.checksum = gTaskSysswmContext.cloudSwPkg.dbCksum;
		gTaskSysswmContext.cloudSwDl.totalLen = gTaskSysswmContext.cloudSwPkg.dbTotalLen;
		gTaskSysswmContext.cloudSwDl.dlTime = time(0);
		gTaskSysswmContext.cloudSwDl.segIndex = 1; //第一段

		strncpy(gTaskSysswmContext.cloudSwDl.fileName, gTaskSysswmContext.cloudSwPkg.dbName, HCU_SYSMSG_SYSSWM_SW_PKG_FILE_NAME_MAX_LEN-1);
		gTaskSysswmContext.cloudSwDl.segSplitLen = HUITP_IEID_UNI_SW_PACKAGE_BODY_MAX_LEN;
		UINT32 segTotal = 0;
		segTotal = gTaskSysswmContext.cloudSwPkg.dbTotalLen / HUITP_IEID_UNI_SW_PACKAGE_BODY_MAX_LEN;
		if (segTotal * HUITP_IEID_UNI_SW_PACKAGE_BODY_MAX_LEN < gTaskSysswmContext.cloudSwPkg.swTotalLen) segTotal++;
		gTaskSysswmContext.cloudSwDl.segTotal = segTotal;

		//生成消息并发送给后台
		return func_sysswm_send_cloudvela_sw_package_report();
	}//继续传输DB文件

	return SUCCESS;
}

OPSTAT func_sysswm_swpkg_last_seg_process_hcu_db(char *stmp)
{
	char s[200];

	//swpkg数据表单刷新
	if (func_sysswm_db_download_process_swpkg_db_refresh(stmp) == FAILURE){
		//将软件版本信息恢复
		zHcuSysEngPar.hwBurnId.swRelId = gTaskSysswmContext.swRelSave;
		zHcuSysEngPar.hwBurnId.swVerId = gTaskSysswmContext.swVerSave;
		//恢复BOOT区老旧参数
		hcu_vm_engpar_update_phy_boot_sw_ver(zHcuSysEngPar.hwBurnId.swRelId, zHcuSysEngPar.hwBurnId.swVerId);
		return FAILURE;
	}

	//修改下载后的文件属性
	sprintf(s, "chmod -R 777 %s", zHcuSysEngPar.swm.hcuSwActiveDir);
	system(s);

	//升级系统区的软件版本
	zHcuSysEngPar.hwBurnId.swRelId = gTaskSysswmContext.cloudSwPkg.swRel;
	zHcuSysEngPar.hwBurnId.dbVerId = gTaskSysswmContext.cloudSwPkg.dbVer;

	//升级BOOT区参数
	if (hcu_vm_engpar_update_phy_boot_db_ver(zHcuSysEngPar.hwBurnId.swRelId, zHcuSysEngPar.hwBurnId.dbVerId) == FAILURE)
		HCU_ERROR_PRINT_SYSSWM("SYSSWM: Update local configure file REL/VER ID error!\n");

	//更新版本按钮
	func_sysswm_hcusw_upgrade_info_trigger_ui();

	//拷贝文件到目标区并执行重启任务
	func_sysswm_copy_db_and_exe_to_target_dir_and_restart();

	return SUCCESS;
}

OPSTAT func_sysswm_swpkg_last_seg_process_ihu_sw(char *stmp)
{
	char s[200];

	if (func_sysswm_caculate_file_whole_checksum(stmp) != gTaskSysswmContext.cloudSwPkg.swCksum)
		HCU_ERROR_PRINT_SYSSWM("SYSSWM: Check whole file checksum error!\n");
	strncpy(gTaskSysswmContext.cloudSwPkg.currentActive, HCU_SYSMSG_SYSSWM_CUR_ACTIVE_COMPLETE, sizeof(HCU_SYSMSG_SYSSWM_CUR_ACTIVE_COMPLETE));  //Complete of download
	gTaskSysswmContext.cloudSwPkg.updateTime = time(0);
	//更新软件数据库
	if (dbi_HcuSysSwm_SwPkg_save(&(gTaskSysswmContext.cloudSwPkg)) == FAILURE)
		HCU_ERROR_PRINT_SYSSWM("SYSSWM: After download complete, save sw pakcage error!\n");

	//修改下载后的文件属性
	sprintf(s, "chmod -R 777 %s", zHcuSysEngPar.swm.hcuSwActiveDir);
	system(s);

	//刷新界面
	func_sysswm_ihusw_upgrade_info_trigger_ui(gTaskSysswmContext.swDlSession, gTaskSysswmContext.cloudSwPkg.swRel, gTaskSysswmContext.cloudSwPkg.swVer, gTaskSysswmContext.cloudSwPkg.dbVer);

	//返回
	return SUCCESS;
}

OPSTAT func_sysswm_ftp_file_big_size_process_hcu_sw_and_db(void)
{
	char stmp[200];
	bool flag = FALSE;
	//先判定是否需要升级DB
	flag = ((gTaskSysswmContext.cloudSwPkg.swRel == zHcuSysEngPar.hwBurnId.swRelId) && (gTaskSysswmContext.cloudSwPkg.dbVer <= zHcuSysEngPar.hwBurnId.dbVerId));

	memset(stmp, 0, sizeof(stmp));
	//File Name Only
	strcat(stmp, gTaskSysswmContext.cloudSwPkg.fileName);
	if (hcu_service_ftp_sw_download_by_ftp(stmp) == FAILURE)
		HCU_ERROR_PRINT_SYSSWM("SYSSWM: File Download Error! Filename=[%s]\n", stmp);

	//修改下载后的文件属性
	sprintf(stmp, "chmod -R 777 %s", zHcuSysEngPar.swm.hcuSwActiveDir);
	system(stmp);

	//swpkg数据表单刷新
	memset(stmp, 0, sizeof(stmp));
	strcpy(stmp, zHcuSysEngPar.swm.hcuSwActiveDir);
	strcat(stmp, gTaskSysswmContext.cloudSwPkg.fileName);
	if (func_sysswm_sw_download_process_swpkg_db_refresh(stmp) == FAILURE) return FAILURE;

	//临时存储老旧数据，一旦DB不成功，可以恢复回来
	gTaskSysswmContext.swRelSave = zHcuSysEngPar.hwBurnId.swRelId;
	gTaskSysswmContext.swVerSave = zHcuSysEngPar.hwBurnId.swVerId;

	//升级系统区的软件版本
	zHcuSysEngPar.hwBurnId.swRelId = gTaskSysswmContext.cloudSwPkg.swRel;
	zHcuSysEngPar.hwBurnId.swVerId = gTaskSysswmContext.cloudSwPkg.swVer;

	//升级BOOT区参数
	if (hcu_vm_engpar_update_phy_boot_sw_ver(zHcuSysEngPar.hwBurnId.swRelId, zHcuSysEngPar.hwBurnId.swVerId) == FAILURE)
		HCU_ERROR_PRINT_SYSSWM("SYSSWM: Update local configure file REL/VER ID error!\n");

	//更新版本按钮
	func_sysswm_hcusw_upgrade_info_trigger_ui();

	//拷贝文件到目标区并执行重启任务
	if (flag == TRUE)
	{
		func_sysswm_copy_exe_to_target_dir_and_restart();
		HCU_DEBUG_PRINT_CRT("SYSSWM: HCU File FTP download and process SW_PKG only success!\n");
	}
	//继续传输DB文件
	else{
		//刷新下载初始化Context
		memset(&(gTaskSysswmContext.cloudSwDl), 0, sizeof(HcuSysMsgIeL3SysSwmSwDlElement_t));
		gTaskSysswmContext.cloudSwDl.equEntry = HCU_SYSMSG_SYSSWM_EQU_ENTRY_HCU_CLIENT_DB;
		gTaskSysswmContext.cloudSwDl.hwType = gTaskSysswmContext.cloudSwPkg.hwType;
		gTaskSysswmContext.cloudSwDl.hwPem = gTaskSysswmContext.cloudSwPkg.hwPem;
		gTaskSysswmContext.cloudSwDl.swRel = gTaskSysswmContext.cloudSwPkg.swRel;
		gTaskSysswmContext.cloudSwDl.verId = gTaskSysswmContext.cloudSwPkg.dbVer;
		gTaskSysswmContext.cloudSwDl.upgradeFlag = gTaskSysswmContext.cloudSwPkg.upgradeFlag;
		gTaskSysswmContext.cloudSwDl.checksum = gTaskSysswmContext.cloudSwPkg.dbCksum;
		gTaskSysswmContext.cloudSwDl.totalLen = gTaskSysswmContext.cloudSwPkg.dbTotalLen;
		gTaskSysswmContext.cloudSwDl.dlTime = time(0);
		gTaskSysswmContext.cloudSwDl.segIndex = 1; //第一段

		strncpy(gTaskSysswmContext.cloudSwDl.fileName, gTaskSysswmContext.cloudSwPkg.dbName, HCU_SYSMSG_SYSSWM_SW_PKG_FILE_NAME_MAX_LEN-1);
		gTaskSysswmContext.cloudSwDl.segSplitLen = HUITP_IEID_UNI_SW_PACKAGE_BODY_MAX_LEN;
		UINT32 segTotal = 0;
		segTotal = gTaskSysswmContext.cloudSwPkg.dbTotalLen / HUITP_IEID_UNI_SW_PACKAGE_BODY_MAX_LEN;
		if (segTotal * HUITP_IEID_UNI_SW_PACKAGE_BODY_MAX_LEN < gTaskSysswmContext.cloudSwPkg.swTotalLen) segTotal++;
		gTaskSysswmContext.cloudSwDl.segTotal = segTotal;

		//执行HCU_DB软件下载
		memset(stmp, 0, sizeof(stmp));
		//File Name Only
		strcat(stmp, gTaskSysswmContext.cloudSwDl.fileName);
		if (hcu_service_ftp_sw_download_by_ftp(stmp) == FAILURE){
			//将软件版本信息恢复
			zHcuSysEngPar.hwBurnId.swRelId = gTaskSysswmContext.swRelSave;
			zHcuSysEngPar.hwBurnId.swVerId = gTaskSysswmContext.swVerSave;
			//恢复BOOT区老旧参数
			hcu_vm_engpar_update_phy_boot_sw_ver(zHcuSysEngPar.hwBurnId.swRelId, zHcuSysEngPar.hwBurnId.swVerId);
			HCU_ERROR_PRINT_SYSSWM("SYSSWM: File Download Error!\n");
		}

		//修改下载后的文件属性
		sprintf(stmp, "chmod -R 777 %s", zHcuSysEngPar.swm.hcuSwActiveDir);
		system(stmp);

		//swpkg数据表单刷新
		memset(stmp, 0, sizeof(stmp));
		strcpy(stmp, zHcuSysEngPar.swm.hcuSwActiveDir);
		strcat(stmp, gTaskSysswmContext.cloudSwDl.fileName);
		if (func_sysswm_db_download_process_swpkg_db_refresh(stmp) == FAILURE) return FAILURE;

		//升级系统区的软件版本
		zHcuSysEngPar.hwBurnId.swRelId = gTaskSysswmContext.cloudSwPkg.swRel;
		zHcuSysEngPar.hwBurnId.dbVerId = gTaskSysswmContext.cloudSwPkg.dbVer;

		//升级BOOT区参数
		if (hcu_vm_engpar_update_phy_boot_db_ver(zHcuSysEngPar.hwBurnId.swRelId, zHcuSysEngPar.hwBurnId.dbVerId) == FAILURE)
			HCU_ERROR_PRINT_SYSSWM("SYSSWM: Update local configure file REL/VER ID error!\n");

		//更新版本按钮
		func_sysswm_hcusw_upgrade_info_trigger_ui();

		//拷贝文件到目标区并执行重启任务
		func_sysswm_copy_db_and_exe_to_target_dir_and_restart();
		HCU_DEBUG_PRINT_CRT("SYSSWM: HCU File FTP download and process SW_PKG+DB_VER success!\n");
	}//继续传输DB文件

	return SUCCESS;
}

OPSTAT func_sysswm_ftp_file_big_size_process_ihu_sw(void)
{
	char stmp[200];
	memset(stmp, 0, sizeof(stmp));
	strcat(stmp, gTaskSysswmContext.cloudSwPkg.fileName);
	if (hcu_service_ftp_sw_download_by_ftp(stmp) == FAILURE)
		HCU_ERROR_PRINT_SYSSWM("SYSSWM: File Download Error!\n");

	//修改下载后的文件属性
	sprintf(stmp, "chmod -R 777 %s", zHcuSysEngPar.swm.hcuSwActiveDir);
	system(stmp);

	//检查checksum
	memset(stmp, 0, sizeof(stmp));
	strcpy(stmp, zHcuSysEngPar.swm.hcuSwActiveDir);
	strcat(stmp, gTaskSysswmContext.cloudSwPkg.fileName);
	if (func_sysswm_caculate_file_whole_checksum(stmp) != gTaskSysswmContext.cloudSwPkg.swCksum)
		HCU_ERROR_PRINT_SYSSWM("SYSSWM: Check whole file checksum error!\n");
	strncpy(gTaskSysswmContext.cloudSwPkg.currentActive, HCU_SYSMSG_SYSSWM_CUR_ACTIVE_COMPLETE, sizeof(HCU_SYSMSG_SYSSWM_CUR_ACTIVE_COMPLETE));  //Complete of download
	gTaskSysswmContext.cloudSwPkg.updateTime = time(0);
	//更新软件数据库
	if (dbi_HcuSysSwm_SwPkg_save(&(gTaskSysswmContext.cloudSwPkg)) == FAILURE)
		HCU_ERROR_PRINT_SYSSWM("SYSSWM: After download complete, save sw pakcage error!\n");

	HCU_DEBUG_PRINT_CRT("SYSSWM: IHU File FTP download and process SW_PKG only success!\n");

	//刷新界面
	func_sysswm_ihusw_upgrade_info_trigger_ui(gTaskSysswmContext.swDlSession, gTaskSysswmContext.cloudSwPkg.swRel, gTaskSysswmContext.cloudSwPkg.swVer, gTaskSysswmContext.cloudSwPkg.dbVer);

	//返回
	return SUCCESS;
}

//刷入初始化软件版本信息
void func_sysswm_sw_init_info_trigger_ui(void)
{
	//当前HCU版本
	char input[50];
	memset(input, 0, 50);
	sprintf(input, "NOW: HCU-SW-R%d.V%d.DB%d.", zHcuSysEngPar.hwBurnId.swRelId, zHcuSysEngPar.hwBurnId.swVerId, zHcuSysEngPar.hwBurnId.dbVerId);
	if (zHcuSysEngPar.hwBurnId.swUpgradeFlag == HUITP_IEID_UNI_FW_UPGRADE_NO) strcat(input, "UPG_NO!");
	else if (zHcuSysEngPar.hwBurnId.swUpgradeFlag == HUITP_IEID_UNI_FW_UPGRADE_YES_STABLE) strcat(input, "STABLE!");
	else if (zHcuSysEngPar.hwBurnId.swUpgradeFlag == HUITP_IEID_UNI_FW_UPGRADE_YES_TRIAL) strcat(input, "TRIAL!");
	else if (zHcuSysEngPar.hwBurnId.swUpgradeFlag == HUITP_IEID_UNI_FW_UPGRADE_YES_PATCH) strcat(input, "PATCH!");
	else strcat(input, "UPG_ERROR!");
#if (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFSC_CBU_ID)
	dbi_HcuBfsc_hcusw_ver_Update(input, strlen(input));
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFDF_CBU_ID)
	//dbi_HcuBfdf_hcusw_ver_Update(input, strlen(input));
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFHS_CBU_ID)
	//dbi_HcuBfhs_hcusw_ver_Update(input, strlen(input));
#endif
}

//刷入更新软件版本信息
void func_sysswm_hcusw_upgrade_info_trigger_ui(void)
{
	//升级HCU版本
	char input[50];
	memset(input, 0, 50);
	sprintf(input, "NEW: HCU-SW-R%d.V%d.DB%d.", zHcuSysEngPar.hwBurnId.swRelId, zHcuSysEngPar.hwBurnId.swVerId, zHcuSysEngPar.hwBurnId.dbVerId);
	if (zHcuSysEngPar.hwBurnId.swUpgradeFlag == HUITP_IEID_UNI_FW_UPGRADE_NO) strcat(input, "UPG_NO!");
	else if (zHcuSysEngPar.hwBurnId.swUpgradeFlag == HUITP_IEID_UNI_FW_UPGRADE_YES_STABLE) strcat(input, "STABLE!");
	else if (zHcuSysEngPar.hwBurnId.swUpgradeFlag == HUITP_IEID_UNI_FW_UPGRADE_YES_TRIAL) strcat(input, "TRIAL!");
	else if (zHcuSysEngPar.hwBurnId.swUpgradeFlag == HUITP_IEID_UNI_FW_UPGRADE_YES_PATCH) strcat(input, "PATCH!");
	else strcat(input, "UPG_ERROR!");
#if (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFSC_CBU_ID)
	dbi_HcuBfsc_hcusw_ver_Update(input, strlen(input));
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFDF_CBU_ID)
	//dbi_HcuBfdf_hcusw_ver_Update(input, strlen(input));
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFHS_CBU_ID)
	//dbi_HcuBfhs_hcusw_ver_Update(input, strlen(input));
#endif
}

//刷入更新软件版本信息
void func_sysswm_ihusw_upgrade_info_trigger_ui(UINT8 session, UINT16 swrel, UINT16 swver, UINT16 dbver)
{
	//升级HCU版本
	char input[50];
	memset(input, 0, 50);
	sprintf(input, "NEW: IHU-SW-R%d.V%d.DB%d.", swrel, swver, dbver);
	if (session == HCU_SYSSWM_SW_DOWNLOAD_SESSION_IHU_STABLE) strcat(input, "STABLE!");
		else if (session == HCU_SYSSWM_SW_DOWNLOAD_SESSION_IHU_TRIAL) strcat(input, "TRIAL!");
		else if (session == HCU_SYSSWM_SW_DOWNLOAD_SESSION_IHU_PATCH) strcat(input, "PATCH!");
		else strcat(input, "ERROR!");

#if (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFSC_CBU_ID)
	if (HCU_SYSSWM_UPGRADE_FLAG_MATCH == TRUE) dbi_HcuBfsc_ihusw_ver_Update(input, strlen(input));
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFDF_CBU_ID)
	//dbi_HcuBfdf_ihusw_ver_Update(input, strlen(input));
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFHS_CBU_ID)
	//dbi_HcuBfhs_ihusw_ver_Update(input, strlen(input));
#endif
}

//刷入inventory request软件版本信息
void func_sysswm_sw_inventory_req_info_trigger_ui(UINT8 session, UINT16 swrel, UINT16 swver, UINT16 dbver)
{
	//当前HCU版本
	char input[50];
	memset(input, 0, 50);
	if (session == HCU_SYSSWM_SW_DOWNLOAD_SESSION_HCU_CLIENT)
	{
		sprintf(input, "NOW: Inventory Req HCU-SW-R%d.V%d.DB%d.", swrel, swver, dbver);
		if (zHcuSysEngPar.hwBurnId.swUpgradeFlag == HUITP_IEID_UNI_FW_UPGRADE_NO) strcat(input, "UPG_NO!");
		else if (zHcuSysEngPar.hwBurnId.swUpgradeFlag == HUITP_IEID_UNI_FW_UPGRADE_YES_STABLE) strcat(input, "STABLE!");
		else if (zHcuSysEngPar.hwBurnId.swUpgradeFlag == HUITP_IEID_UNI_FW_UPGRADE_YES_TRIAL) strcat(input, "TRIAL!");
		else if (zHcuSysEngPar.hwBurnId.swUpgradeFlag == HUITP_IEID_UNI_FW_UPGRADE_YES_PATCH) strcat(input, "PATCH!");
		else strcat(input, "UPG_ERROR!");
	}
	else if (session == HCU_SYSSWM_SW_DOWNLOAD_SESSION_IHU_STABLE) sprintf(input, "NOW: Inventory Req IHU-SW-R%d.V%d.DB%d.STABLE!", swrel, swver, dbver);
	else if (session == HCU_SYSSWM_SW_DOWNLOAD_SESSION_IHU_TRIAL) sprintf(input, "NOW: Inventory Req IHU-SW-R%d.V%d.DB%d.TRIAL!", swrel, swver, dbver);
	else if (session == HCU_SYSSWM_SW_DOWNLOAD_SESSION_IHU_PATCH) sprintf(input, "NOW: Inventory Req IHU-SW-R%d.V%d.DB%d.PATCH!", swrel, swver, dbver);
	else sprintf(input, "NOW: Inventory Req ERROR-SW-R%d.V%d.DB%d.PATCH!", swrel, swver, dbver);

	if (session == HCU_SYSSWM_SW_DOWNLOAD_SESSION_HCU_CLIENT){
#if (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFSC_CBU_ID)
	dbi_HcuBfsc_hcusw_ver_Update(input, strlen(input));
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFDF_CBU_ID)
	//dbi_HcuBfdf_hcusw_ver_Update(input, strlen(input));
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFHS_CBU_ID)
	//dbi_HcuBfhs_hcusw_ver_Update(input, strlen(input));
#endif
	}

	else{
#if (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFSC_CBU_ID)
		if (HCU_SYSSWM_UPGRADE_FLAG_MATCH == TRUE) dbi_HcuBfsc_ihusw_ver_Update(input, strlen(input));
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFDF_CBU_ID)
	//dbi_HcuBfdf_ihusw_ver_Update(input, strlen(input));
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFHS_CBU_ID)
	//dbi_HcuBfhs_ihusw_ver_Update(input, strlen(input));
#endif
	}
}

//刷入inventory confirm软件版本信息
void func_sysswm_sw_inventory_confirm_info_trigger_ui(UINT8 session, UINT16 swrel, UINT16 swver, UINT16 dbver)
{
	//当前HCU版本
	char input[50];
	memset(input, 0, 50);
	if (session == HCU_SYSSWM_SW_DOWNLOAD_SESSION_HCU_CLIENT)
	{
		sprintf(input, "NOW: Inventory confirm HCU-SW-R%d.V%d.DB%d.", swrel, swver, dbver);
		if (zHcuSysEngPar.hwBurnId.swUpgradeFlag == HUITP_IEID_UNI_FW_UPGRADE_NO) strcat(input, "UPG_NO!");
		else if (zHcuSysEngPar.hwBurnId.swUpgradeFlag == HUITP_IEID_UNI_FW_UPGRADE_YES_STABLE) strcat(input, "STABLE!");
		else if (zHcuSysEngPar.hwBurnId.swUpgradeFlag == HUITP_IEID_UNI_FW_UPGRADE_YES_TRIAL) strcat(input, "TRIAL!");
		else if (zHcuSysEngPar.hwBurnId.swUpgradeFlag == HUITP_IEID_UNI_FW_UPGRADE_YES_PATCH) strcat(input, "PATCH!");
		else strcat(input, "UPG_ERROR!");
	}
	else if (session == HCU_SYSSWM_SW_DOWNLOAD_SESSION_IHU_STABLE) sprintf(input, "NOW: Inventory confirm IHU-SW-R%d.V%d.DB%d.STABLE!", swrel, swver, dbver);
	else if (session == HCU_SYSSWM_SW_DOWNLOAD_SESSION_IHU_TRIAL) sprintf(input, "NOW: Inventory confirm IHU-SW-R%d.V%d.DB%d.TRIAL!", swrel, swver, dbver);
	else if (session == HCU_SYSSWM_SW_DOWNLOAD_SESSION_IHU_PATCH) sprintf(input, "NOW: Inventory confirm IHU-SW-R%d.V%d.DB%d.PATCH!", swrel, swver, dbver);
	else sprintf(input, "NOW: Inventory confirm ERROR-SW-R%d.V%d.DB%d.PATCH!", swrel, swver, dbver);

	if (session == HCU_SYSSWM_SW_DOWNLOAD_SESSION_HCU_CLIENT){
#if (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFSC_CBU_ID)
	dbi_HcuBfsc_hcusw_ver_Update(input, strlen(input));
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFDF_CBU_ID)
	//dbi_HcuBfdf_hcusw_ver_Update(input, strlen(input));
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFHS_CBU_ID)
	//dbi_HcuBfhs_hcusw_ver_Update(input, strlen(input));
#endif
	}

	else{
#if (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFSC_CBU_ID)
		if (HCU_SYSSWM_UPGRADE_FLAG_MATCH == TRUE) dbi_HcuBfsc_ihusw_ver_Update(input, strlen(input));
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFDF_CBU_ID)
	//dbi_HcuBfdf_ihusw_ver_Update(input, strlen(input));
#elif (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFHS_CBU_ID)
	//dbi_HcuBfhs_ihusw_ver_Update(input, strlen(input));
#endif
	}
}




