/*
 * bfdfuicomm.c
 *
 *  Created on: 2016年5月14日
 *      Author: hitpony
 */

#include "bfdfuicomm.h"
//#include  "jsoninotify.h"

#include "../l0service/timer.h"
#include "../l0service/trace.h"
#include "../l1com/l1comdef.h"
//#include <json-c/json.h>
//#include <json-c/json_object.h>

#include "../l3appctrl/l3bfdf.h"

/*
** FSM of the BFDFUICOMM
*/
HcuFsmStateItem_t HcuFsmBfdfuicomm[] =
{
    //MessageId                 //State                   		 		//Function
	//启始点，固定定义，不要改动, 使用ENTRY/END，意味者MSGID肯定不可能在某个高位区段中；考虑到所有任务共享MsgId，即使分段，也无法实现
	//完全是为了给任务一个初始化的机会，按照状态转移机制，该函数不具备启动的机会，因为任务初始化后自动到FSM_STATE_IDLE
	//如果没有必要进行初始化，可以设置为NULL
	{MSG_ID_ENTRY,       					FSM_STATE_ENTRY,            				fsm_bfdfuicomm_task_entry}, //Starting

	//System level initialization, only controlled by HCU-MAIN
    {MSG_ID_COM_INIT,       				FSM_STATE_IDLE,            					fsm_bfdfuicomm_init},
    {MSG_ID_COM_INIT_FEEDBACK,				FSM_STATE_IDLE,            					fsm_com_do_nothing},

	//Task level initialization
    {MSG_ID_COM_INIT,       				FSM_STATE_BFDFUICOMM_INITED,            	fsm_bfdfuicomm_init},
    {MSG_ID_COM_INIT_FEEDBACK,				FSM_STATE_BFDFUICOMM_INITED,            	fsm_com_do_nothing},

	//ANY state entry
    {MSG_ID_COM_INIT_FEEDBACK,				FSM_STATE_COMMON,          					fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT,       			FSM_STATE_COMMON,          					fsm_com_heart_beat_rcv},
	{MSG_ID_COM_STOP,       				FSM_STATE_COMMON,          					fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT_FB,       		FSM_STATE_COMMON,          					fsm_com_do_nothing},
    {MSG_ID_COM_RESTART,					FSM_STATE_COMMON,            				fsm_bfdfuicomm_restart},
	{MSG_ID_COM_TIME_OUT,       			FSM_STATE_COMMON,          					fsm_bfdfuicomm_timeout},

    //Normal working status
    {MSG_ID_COM_INIT_FEEDBACK,				FSM_STATE_BFDFUICOMM_ACTIVED,            	fsm_com_do_nothing},
	{MSG_ID_L3BFDF_UICOMM_CTRL_CMD_RESP,    FSM_STATE_BFDFUICOMM_ACTIVED,          		fsm_bfdfuicomm_l3bfdf_cmd_resp},	//人工控制反馈
	{MSG_ID_SUI_TEST_CMD_RESP,      		FSM_STATE_BFDFUICOMM_ACTIVED,          		fsm_bfdfuicomm_sui_test_cmd_resp},  //测试命令反馈

/*
    {MSG_ID_INOTIFY_UICOMM_FILE_CHANGE_IND,	FSM_STATE_BFDFUICOMM_ACTIVED,            	fsm_bfdfuicomm_scan_jason_callback},
	{MSG_ID_L3BFDF_UICOMM_CFG_RESP,      	FSM_STATE_BFDFUICOMM_ACTIVED,          		fsm_bfdfuicomm_l3bfdf_cfg_resp},	//配置反馈
*/

	//UIR2HCU MSG RCV
	{MSG_ID_HUICOBUS_UIR_INIT_REQ,      		FSM_STATE_BFDFUICOMM_ACTIVED,          	fsm_bfdfuicomm_huicobus_uir_init_req},
	{MSG_ID_HUICOBUS_UIR_START_RESUME_REQ, 		FSM_STATE_BFDFUICOMM_ACTIVED,          	fsm_bfdfuicomm_huicobus_uir_start_resume_req},
	{MSG_ID_HUICOBUS_UIR_STOP_SUSPEND_REQ, 		FSM_STATE_BFDFUICOMM_ACTIVED,          	fsm_bfdfuicomm_huicobus_uir_stop_suspend_req},
	{MSG_ID_HUICOBUS_UIR_STATIC_CALI_REQ,      	FSM_STATE_BFDFUICOMM_ACTIVED,          	fsm_bfdfuicomm_huicobus_uir_static_cali_req},
	{MSG_ID_HUICOBUS_UIR_DYNAMIC_CALI_REQ,     	FSM_STATE_BFDFUICOMM_ACTIVED,          	fsm_bfdfuicomm_huicobus_uir_dynamic_cali_req},
	{MSG_ID_HUICOBUS_UIR_TEST_CMD_REQ,      	FSM_STATE_BFDFUICOMM_ACTIVED,          	fsm_bfdfuicomm_huicobus_uir_test_cmd_req},
	{MSG_ID_HUICOBUS_UIR_ONE_KEY_ZERO_REQ, 		FSM_STATE_BFDFUICOMM_ACTIVED,         	fsm_bfdfuicomm_huicobus_uir_one_key_zero_req},

    //结束点，固定定义，不要改动
    {MSG_ID_END,            	FSM_STATE_END,             				NULL},  //Ending
};

//Global variables
extern gTaskL3bfdfContext_t gTaskL3bfdfContext;
gTaskL3bfdfuicommContext_t gTaskL3bfdfuicommContext;

//Main Entry
//Input parameter would be useless, but just for similar structure purpose
OPSTAT fsm_bfdfuicomm_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//除了对全局变量进行操作之外，尽量不要做其它操作，因为该函数将被主任务/线程调用，不是本任务/线程调用
	//该API就是给本任务一个提早介入的入口，可以帮着做些测试性操作
	if (FsmSetState(TASK_ID_BFDFUICOMM, FSM_STATE_IDLE) == FAILURE){
		HcuErrorPrint("BFDFUICOMM: Error Set FSM State at fsm_bfdfuicomm_task_entry\n");}
	return SUCCESS;
}

OPSTAT fsm_bfdfuicomm_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;

	if ((src_id > TASK_ID_MIN) &&(src_id < TASK_ID_MAX)){
		//Send back MSG_ID_COM_INIT_FEEDBACK to SVRCON
		msg_struct_com_init_feedback_t snd0;
		memset(&snd0, 0, sizeof(msg_struct_com_init_feedback_t));
		snd0.length = sizeof(msg_struct_com_init_feedback_t);

		//to avoid all task send out the init fb msg at the same time which lead to msgque get stuck
		hcu_usleep(dest_id*HCU_SYSCFG_DURATION_OF_INIT_FB_WAIT_MAX);

		ret = hcu_message_send(MSG_ID_COM_INIT_FEEDBACK, src_id, TASK_ID_BFDFUICOMM, &snd0, snd0.length);
		if (ret == FAILURE){
			HcuErrorPrint("BFDFUICOMM: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_BFDFUICOMM].taskName, zHcuVmCtrTab.task[src_id].taskName);
			return FAILURE;
		}
	}

	//收到初始化消息后，进入初始化状态
	if (FsmSetState(TASK_ID_BFDFUICOMM, FSM_STATE_BFDFUICOMM_INITED) == FAILURE){
		HcuErrorPrint("BFDFUICOMM: Error Set FSM State!\n");
		return FAILURE;
	}

	//Global Variables
	zHcuSysStaPm.taskRunErrCnt[TASK_ID_BFDFUICOMM] = 0;
	memset(&gTaskL3bfdfuicommContext, 0, sizeof(gTaskL3bfdfuicommContext_t));

	//启动MQTT服务内容
//		while(1){
//			StrHlcIe_cui_hcu2uir_inswgt_bfdf_report_t buf;
//			buf.weight = rand()%500;
//			INT32 boardId = rand()%2;
//
//			hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_inswgt_bfdf_report(boardId, &buf);
//			usleep(10);
//		}


	//启动周期性定时器
/*
	ret = hcu_timer_start(TASK_ID_BFDFUICOMM, TIMER_ID_1S_BFDFUICOMM_PERIOD_READ, \
			zHcuSysEngPar.timer.array[TIMER_ID_1S_BFDFUICOMM_PERIOD_READ].dur, TIMER_TYPE_PERIOD, TIMER_RESOLUTION_1S);
*/

	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_BFDFUICOMM, FSM_STATE_BFDFUICOMM_ACTIVED) == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_BFDFUICOMM]++;
		HcuErrorPrint("BFDFUICOMM: Error Set FSM State!\n");
		return FAILURE;
	}
	if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_FAT_ON) != FALSE){
		HcuDebugPrint("BFDFUICOMM: Enter FSM_STATE_BFDFUICOMM_ACTIVED status, Keeping refresh here!\n");
	}

	//延迟并启动系统，进入测试模式
	hcu_sleep(2);
	//设置configIndex=1
	func_bfdfuicomm_read_cfg_file_into_ctrl_table(1);
	//发送启动消息给L3BFDF
//	msg_struct_uicomm_l3bfdf_ctrl_cmd_req_t snd;
//	memset(&snd, 0, sizeof(msg_struct_uicomm_l3bfdf_ctrl_cmd_req_t));
//	snd.cmdid = HCU_SYSMSG_BFDF_UICOMM_CMDID_CFG_START;
//	snd.length = sizeof(msg_struct_uicomm_l3bfdf_ctrl_cmd_req_t);
//	ret = hcu_message_send(MSG_ID_UICOMM_L3BFDF_CTRL_CMD_REQ, TASK_ID_L3BFDF, TASK_ID_BFDFUICOMM, &snd, snd.length);
//	if (ret == FAILURE){
//		HcuErrorPrint("BFDFUICOMM: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_BFDFUICOMM].taskName, zHcuVmCtrTab.task[TASK_ID_L3BFDF].taskName);
//		return FAILURE;
//	}

	//初始化sessionId
	gTaskL3bfdfContext.sessionId = dbi_HcuBfdf_CallCellMaxIdGet() + 1;

	//返回
	return SUCCESS;
}

OPSTAT fsm_bfdfuicomm_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HcuErrorPrint("BFDFUICOMM: Internal error counter reach DEAD level, SW-RESTART soon!\n");
	zHcuSysStaPm.statisCnt.restartCnt++;
	fsm_bfdfuicomm_init(0, 0, NULL, 0);
	return SUCCESS;
}

//暂时啥都不干，定时到达后，还不明确是否需要支持定时工作
OPSTAT fsm_bfdfuicomm_timeout(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret;

	HCU_MSG_RCV_CHECK_FOR_GEN_LOCAL(TASK_ID_BFDFUICOMM, msg_struct_com_time_out_t);

	//钩子在此处，检查zHcuRunErrCnt[TASK_ID_BFDFUICOMM]是否超限
	if (zHcuSysStaPm.taskRunErrCnt[TASK_ID_BFDFUICOMM] > HCU_RUN_ERROR_LEVEL_2_MAJOR){
		//减少重复RESTART的概率
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_BFDFUICOMM] = zHcuSysStaPm.taskRunErrCnt[TASK_ID_BFDFUICOMM] - HCU_RUN_ERROR_LEVEL_2_MAJOR;
		msg_struct_com_restart_t snd0;
		memset(&snd0, 0, sizeof(msg_struct_com_restart_t));
		snd0.length = sizeof(msg_struct_com_restart_t);
		ret = hcu_message_send(MSG_ID_COM_RESTART, TASK_ID_BFDFUICOMM, TASK_ID_BFDFUICOMM, &snd0, snd0.length);
		if (ret == FAILURE){
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_BFDFUICOMM]++;
			HcuErrorPrint("BFDFUICOMM: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_BFDFUICOMM].taskName, zHcuVmCtrTab.task[TASK_ID_BFDFUICOMM].taskName);
			return FAILURE;
		}
	}

	//Period time out received
/*
	if ((rcv.timeId == TIMER_ID_1S_BFDFUICOMM_PERIOD_READ) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		//保护周期读数的优先级，强制抢占状态，并简化问题
		if (FsmGetState(TASK_ID_BFDFUICOMM) != FSM_STATE_BFDFUICOMM_ACTIVED){
			ret = FsmSetState(TASK_ID_BFDFUICOMM, FSM_STATE_BFDFUICOMM_ACTIVED);
			if (ret == FAILURE){
				zHcuSysStaPm.taskRunErrCnt[TASK_ID_BFDFUICOMM]++;
				HcuErrorPrint("BFDFUICOMM: Error Set FSM State!\n");
				return FAILURE;
			}//FsmSetState
		}


		//Do nothing
		func_bfdfuicomm_time_out_period_read_process();
	}
*/
	return SUCCESS;
}

//配置反馈
/*OPSTAT fsm_bfdfuicomm_l3bfdf_cfg_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret=0;

	msg_struct_l3bfdf_uicomm_cfg_resp_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_l3bfdf_uicomm_cfg_resp_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_l3bfdf_uicomm_cfg_resp_t)))
		HCU_ERROR_PRINT_BFDFUICOMM("BFDFUICOMM: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);

	//收到正确且所有秤台齐活的反馈
	HcuDebugPrint("BFDFUICOMM: fsm_bfdfuicomm_l3bfdf_cfg_resp: rcv.validFlag = %d\n", rcv.validFlag);
	dbi_HcuBfdf_WmcCurComWgtUpdate(0);
	if((rcv.validFlag == TRUE) && (gTaskL3bfdfuicommContext.bfdfuiState == HCU_BFDFCOMM_JASON_CMD_START)){
		//Update databse, to let START menu turn state from grey to active!!!
		msg_struct_uicomm_l3bfdf_cmd_req_t snd;
		memset(&snd, 0, sizeof(msg_struct_uicomm_l3bfdf_cmd_req_t));
		snd.length = sizeof(msg_struct_uicomm_l3bfdf_cmd_req_t);
		snd.cmdid = HCU_SYSMSG_BFDF_UICOMM_CMDID_START;
		//发送命令给L3BFDF
		if (hcu_message_send(MSG_ID_UICOMM_L3BFDF_CMD_REQ, TASK_ID_L3BFDF, TASK_ID_BFDFUICOMM, &snd, snd.length) == FAILURE)
			HCU_ERROR_PRINT_BFDFUICOMM("BFDFUICOMM: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_BFDFUICOMM].taskName, zHcuVmCtrTab.task[TASK_ID_L3BFDF].taskName);
	}
	//收到L3BFDF指示秤台配置错误的反馈
	else if ((rcv.validFlag == FALSE) && (gTaskL3bfdfuicommContext.bfdfuiState == HCU_BFDFCOMM_JASON_CMD_START)){
		hcu_sleep(2);
		msg_struct_uicomm_l3bfdf_cfg_req_t snd;
		memset(&snd, 0, sizeof(msg_struct_uicomm_l3bfdf_cfg_req_t));
		snd.length = sizeof(msg_struct_uicomm_l3bfdf_cfg_req_t);
		if (hcu_message_send(MSG_ID_UICOMM_L3BFDF_CFG_REQ, TASK_ID_L3BFDF, TASK_ID_BFDFUICOMM, &snd, snd.length) == FAILURE)
			HCU_ERROR_PRINT_BFDFUICOMM("BFDFUICOMM: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_BFDFUICOMM].taskName, zHcuVmCtrTab.task[TASK_ID_L3BFDF].taskName);
	}
	//返回
	return SUCCESS;
}
*/


//启动停止反馈
OPSTAT fsm_bfdfuicomm_l3bfdf_cmd_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;
	UINT8	validFlag = 0;
	UINT8	cmdid = 0;

	HCU_MSG_RCV_CHECK_FOR_GEN_LOCAL(TASK_ID_BFDFUICOMM, msg_struct_l3bfdf_uicomm_ctrl_cmd_resp_t);

	validFlag = rcv.validFlag;
	cmdid = rcv.cmdid;
	//测试用的打印命令
	HCU_DEBUG_PRINT_INF("BFDFUICOMM: rcv.validFlag= %d, cmdid = %d!\n", validFlag, cmdid);

/*
	//存入数据库表单，通知界面新的状态信息
	if ((rcv.validFlag == TRUE) && (cmdid == HCU_SYSMSG_BFDF_UICOMM_CMDID_START)){
		//do nothing
	}
	else if ((rcv.validFlag == TRUE) && (cmdid == HCU_SYSMSG_BFDF_UICOMM_CMDID_STOP)){
		//do nothing
	}
	else if((rcv.validFlag == TRUE) && (cmdid == HCU_SYSMSG_BFDF_UICOMM_CMDID_SUSPEND)){
		ret = dbi_HcuBfdf_WmcStatusForceSuspend();
		if (ret == FAILURE) {
			HCU_ERROR_PRINT_TASK(TASK_ID_BFDFUICOMM, "TASK_ID_BFDFUICOMM: Save data error!\n");
			return FAILURE;
		}
	}
	else if ((rcv.validFlag == TRUE) && (cmdid == HCU_SYSMSG_BFDF_UICOMM_CMDID_RESUME)){
		//do nothing
	}
	else{
		HCU_ERROR_PRINT_TASK(TASK_ID_BFDFUICOMM, "TASK_ID_BFDFUICOMM: Invalid command response!\n");
		return FAILURE;
	}
*/

	//返回
	return SUCCESS;
}

//一般性测试命令的反馈
OPSTAT fsm_bfdfuicomm_sui_test_cmd_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;
	UINT32  adcvalue = 0;
	UINT16 	errorcode;
	UINT8  	sensorid = 0;
	UINT8	validFlag = 0;

	HCU_MSG_RCV_CHECK_FOR_GEN_LOCAL(TASK_ID_BFDFUICOMM, msg_struct_can_uicomm_test_cmd_resp_t);

/*	//Special process for calibration command, save result into table `hcubfdfcalibration`
	if (rcv.cmdid == CMDID_SENSOR_COMMAND_CALIBRATION_ZERO){
		sensorid = rcv.sensorid;
		adcvalue = rcv.cmdvalue1;
		if(sensorid < 1 || sensorid >= HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX)
			HCU_ERROR_PRINT_BFDFUICOMM("BFDFUICOMM: fsm_bfdfuicomm_can_test_cmd_resp's sensorid out of range, [sensorid=%d]! \n", sensorid);
		ret = dbi_HcuBfdf_CalibrationDataUpdate_adczero(adcvalue, sensorid);
		if (ret == FAILURE) HCU_ERROR_PRINT_TASK(TASK_ID_BFDFUICOMM, "TASK_ID_BFDFUICOMM: Save data error!\n");
	}
	else if (rcv.cmdid == CMDID_SENSOR_COMMAND_CALIBRATION_FULL){
		sensorid = rcv.sensorid;
		adcvalue = rcv.cmdvalue1;
		if(sensorid < 1 || sensorid >= HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX)
			HCU_ERROR_PRINT_BFDFUICOMM("BFDFUICOMM: fsm_bfdfuicomm_can_test_cmd_resp's sensorid out of range, [sensorid=%d]! \n", sensorid);
		ret = dbi_HcuBfdf_CalibrationDataUpdate_adcfull(adcvalue, sensorid);
		if (ret == FAILURE) HCU_ERROR_PRINT_TASK(TASK_ID_BFDFUICOMM, "TASK_ID_BFDFUICOMM: Save data error!\n");
	}
	//Common process for other test command, save the response to table 'hcubfdffb2ui'
	else{
		UINT32 cmdValue1 = 0, cmdValue2 = 0;
		char strResp[DBI_MAX_FBINFO_STRING_LENGTH];
		validFlag = rcv.validFlag;
		if (validFlag){
			sensorid = rcv.sensorid;
			errorcode = rcv.errCode;
			cmdValue1 = rcv.cmdvalue1;
			cmdValue2 = rcv.cmdvalue2;
			sprintf(strResp, "Test command execute success! cmdValue1=%d, cmdValue2=%d", cmdValue1, cmdValue2);
		}
		else{
			errorcode = rcv.errCode;
			sprintf(strResp, "Test command execute failure! error code = %d", errorcode);
		}

		//Save command response result to DB
		ret = dbi_HcuBfdf_TestCmdRespUpdate(rcv.cmdid, validFlag, strResp);
		if (ret == FAILURE) HCU_ERROR_PRINT_TASK(TASK_ID_BFDFUICOMM, "TASK_ID_BFDFUICOMM: Save data error!\n");

	}*/

	//返回
	return SUCCESS;
}

OPSTAT fsm_bfdfuicomm_huicobus_uir_init_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HCU_MSG_RCV_CHECK_FOR_GEN_LOCAL(TASK_ID_BFDFUICOMM, msg_struct_huicobus_uir_init_req_t);

	//查询gTaskL3bfdfContext状态，判定下位机状态

	//if (wsState == INIT, DOWNLOAD) return SW LOADING

	//if (wsState == ACTIVE) return COMPL

	//假设状态正常
	hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_init_resp(HUICOBUS_CMDID_CUI_HCU2UIR_GENERAL_CMDVAL_OFFLINE);

	return SUCCESS;
}

OPSTAT fsm_bfdfuicomm_huicobus_uir_start_resume_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HCU_MSG_RCV_CHECK_FOR_GEN_LOCAL(TASK_ID_BFDFUICOMM, msg_struct_huicobus_uir_start_resume_req_t);
	msg_struct_uicomm_l3bfdf_ctrl_cmd_req_t snd;
	memset(&snd, 0, sizeof(msg_struct_uicomm_l3bfdf_ctrl_cmd_req_t));
	snd.length = sizeof(msg_struct_uicomm_l3bfdf_ctrl_cmd_req_t);
	snd.cmdid = HCU_SYSMSG_BFDF_UICOMM_CMDID_CFG_START;
	snd.cmdValue = rcv.cmdValue;
	HCU_MSG_SEND_GENERNAL_PROCESS(MSG_ID_UICOMM_L3BFDF_CTRL_CMD_REQ, TASK_ID_L3BFDF, TASK_ID_BFDFUICOMM);

	return SUCCESS;
}

OPSTAT fsm_bfdfuicomm_huicobus_uir_stop_suspend_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HCU_MSG_RCV_CHECK_FOR_GEN_LOCAL(TASK_ID_BFDFUICOMM, msg_struct_huicobus_uir_stop_suspend_req_t);
	msg_struct_uicomm_l3bfdf_ctrl_cmd_req_t snd;
	memset(&snd, 0, sizeof(msg_struct_uicomm_l3bfdf_ctrl_cmd_req_t));
	snd.length = sizeof(msg_struct_uicomm_l3bfdf_ctrl_cmd_req_t);
	snd.cmdid = HCU_SYSMSG_BFDF_UICOMM_CMDID_STOP;
	snd.cmdValue = rcv.cmdValue;
	HCU_MSG_SEND_GENERNAL_PROCESS(MSG_ID_UICOMM_L3BFDF_CTRL_CMD_REQ, TASK_ID_L3BFDF, TASK_ID_BFDFUICOMM);

	return SUCCESS;
}

OPSTAT fsm_bfdfuicomm_huicobus_uir_static_cali_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HCU_MSG_RCV_CHECK_FOR_GEN_LOCAL(TASK_ID_BFDFUICOMM, msg_struct_huicobus_uir_static_cali_req_t);
	msg_struct_uicomm_l3bfdf_ctrl_cmd_req_t snd;
	memset(&snd, 0, sizeof(msg_struct_uicomm_l3bfdf_ctrl_cmd_req_t));
	snd.length = sizeof(msg_struct_uicomm_l3bfdf_ctrl_cmd_req_t);
	snd.cmdid = HCU_SYSMSG_BFDF_UICOMM_CMDID_STATIC_CALI;
	if(rcv.cmdValue == HUICOBUS_CMDVALUE_static_cali_zero)
		snd.cmdValue = HCU_SYSMSG_BFDF_UICOMM_CMDVALUE_STATIC_CALI_ZERO;
	else if (rcv.cmdValue == HUICOBUS_CMDVALUE_static_cali_full)
		snd.cmdValue = HCU_SYSMSG_BFDF_UICOMM_CMDVALUE_STATIC_CALI_FULL;
	else
		snd.cmdValue = HCU_SYSMSG_BFDF_UICOMM_CMDVALUE_INVALID;

	HCU_MSG_SEND_GENERNAL_PROCESS(MSG_ID_UICOMM_L3BFDF_CTRL_CMD_REQ, TASK_ID_L3BFDF, TASK_ID_BFDFUICOMM);

	return SUCCESS;
}

OPSTAT fsm_bfdfuicomm_huicobus_uir_dynamic_cali_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HCU_MSG_RCV_CHECK_FOR_GEN_LOCAL(TASK_ID_BFDFUICOMM, msg_struct_huicobus_uir_dynamic_cali_req_t);
	msg_struct_uicomm_l3bfdf_ctrl_cmd_req_t snd;
	memset(&snd, 0, sizeof(msg_struct_uicomm_l3bfdf_ctrl_cmd_req_t));
	snd.length = sizeof(msg_struct_uicomm_l3bfdf_ctrl_cmd_req_t);
	snd.cmdid = HCU_SYSMSG_BFDF_UICOMM_CMDID_DYNAMIC_CALI;
	if(rcv.cmdValue == HUICOBUS_CMDVALUE_dynamic_cali_zero)
		snd.cmdValue = HCU_SYSMSG_BFDF_UICOMM_CMDVALUE_DYNAMIC_CALI_ZERO;
	else if (rcv.cmdValue == HUICOBUS_CMDVALUE_dynamic_cali_full)
		snd.cmdValue = HCU_SYSMSG_BFDF_UICOMM_CMDVALUE_DYNAMIC_CALI_FULL;
	else
		snd.cmdValue = HCU_SYSMSG_BFDF_UICOMM_CMDVALUE_INVALID;

	HCU_MSG_SEND_GENERNAL_PROCESS(MSG_ID_UICOMM_L3BFDF_CTRL_CMD_REQ, TASK_ID_L3BFDF, TASK_ID_BFDFUICOMM);

	return SUCCESS;
}

OPSTAT fsm_bfdfuicomm_huicobus_uir_test_cmd_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HCU_MSG_RCV_CHECK_FOR_GEN_LOCAL(TASK_ID_BFDFUICOMM, msg_struct_huicobus_uir_test_cmd_req_t);
	msg_struct_sui_test_cmd_req_t snd;
	memset(&snd, 0, sizeof(msg_struct_sui_test_cmd_req_t));
	snd.length = sizeof(msg_struct_sui_test_cmd_req_t);
	snd.cmdid = HCU_SYSMSG_BFDF_UICOMM_CMDID_TEST;
	memcpy(&snd.boardBitmap, &rcv.boardBitmap, sizeof(rcv.boardBitmap));
	snd.cmdValue1 = rcv.cmdValue1;
	snd.cmdValue2 = rcv.cmdValue2;
	snd.cmdValue3 = rcv.cmdValue3;
	snd.cmdValue4 = rcv.cmdValue4;
	memcpy(&snd.cmdBuf, &rcv.cmdBuf, sizeof(rcv.cmdBuf));
	HCU_MSG_SEND_GENERNAL_PROCESS(MSG_ID_SUI_TEST_CMD_REQ, TASK_ID_CANALPHA, TASK_ID_BFDFUICOMM);

	return SUCCESS;
}

OPSTAT fsm_bfdfuicomm_huicobus_uir_one_key_zero_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HCU_MSG_RCV_CHECK_FOR_GEN_LOCAL(TASK_ID_BFDFUICOMM, msg_struct_huicobus_uir_one_key_clean_zero_req_t);
	msg_struct_uicomm_l3bfdf_ctrl_cmd_req_t snd;
	memset(&snd, 0, sizeof(msg_struct_uicomm_l3bfdf_ctrl_cmd_req_t));
	snd.length = sizeof(msg_struct_uicomm_l3bfdf_ctrl_cmd_req_t);
	snd.cmdid = HCU_SYSMSG_BFDF_UICOMM_CMDID_ONE_KEY_ZERO;

	HCU_MSG_SEND_GENERNAL_PROCESS(MSG_ID_UICOMM_L3BFDF_CTRL_CMD_REQ, TASK_ID_L3BFDF, TASK_ID_BFDFUICOMM);

	return SUCCESS;
}

/*
//具体扫描文件改变的回调函数
OPSTAT  fsm_bfdfuicomm_scan_jason_callback(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret = 0;
	UINT32  fileChangeContent = 0;
	UINT8  sensorid = 0;

	L3BfdfuiJsonCmdParseResult_t parseResult;

	//分析命令标志文件command.json的变化
	memset(&parseResult, 0, sizeof(L3BfdfuiJsonCmdParseResult_t));
	if (func_bfdfuicomm_cmdfile_json_parse(zHcuCmdflagJsonFile, &parseResult) == FAILURE)
		HCU_ERROR_PRINT_BFDFUICOMM("BFDFUICOMM: Command Json file parse failure, [func = func_bfdfuicomm_cmdfile_json_parse]");

	dbi_HcuBfdf_WmcCurComWgtUpdate(0);
	//开启命令标志位发生了变化，
	if (parseResult.cmdStartStop.cmdFlag != gTaskL3bfdfuicommContext.cmdStartStopFlag)
	{
		gTaskL3bfdfuicommContext.cmdStartStopFlag = parseResult.cmdStartStop.cmdFlag;
		fileChangeContent = parseResult.cmdStartStop.cmdValue;
		//依赖文件变化的内容，分类发送控制命令：START/STOP命令
		if (fileChangeContent == HCU_BFDFCOMM_JASON_CMD_START){
			UINT16 config_index = 0;
			gTaskL3bfdfuicommContext.bfdfuiState = HCU_BFDFCOMM_JASON_CMD_START;
			config_index = parseResult.cmdStartStop.confindex;
			if (func_bfdfuicomm_read_cfg_file_into_ctrl_table(config_index) == SUCCESS){
				msg_struct_uicomm_l3bfdf_cfg_req_t snd;
				memset(&snd, 0, sizeof(msg_struct_uicomm_l3bfdf_cfg_req_t));
				snd.length = sizeof(msg_struct_uicomm_l3bfdf_cfg_req_t);
				if (hcu_message_send(MSG_ID_UICOMM_L3BFDF_CFG_REQ, TASK_ID_L3BFDF, TASK_ID_BFDFUICOMM, &snd, snd.length) == FAILURE)
					HCU_ERROR_PRINT_BFDFUICOMM("BFDFUICOMM: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_BFDFUICOMM].taskName, zHcuVmCtrTab.task[TASK_ID_L3BFDF].taskName);
			}
		}
		else if (fileChangeContent == HCU_BFDFCOMM_JASON_CMD_STOP){
			gTaskL3bfdfuicommContext.bfdfuiState = HCU_BFDFCOMM_JASON_CMD_STOP;
			msg_struct_uicomm_l3bfdf_cmd_req_t snd;
			memset(&snd, 0, sizeof(msg_struct_uicomm_l3bfdf_cmd_req_t));
			snd.length = sizeof(msg_struct_uicomm_l3bfdf_cmd_req_t);
			snd.cmdid = HCU_SYSMSG_BFDF_UICOMM_CMDID_STOP;
			//发送命令给L3BFDF
			if (hcu_message_send(MSG_ID_UICOMM_L3BFDF_CMD_REQ, TASK_ID_L3BFDF, TASK_ID_BFDFUICOMM, &snd, snd.length) == FAILURE)
				HCU_ERROR_PRINT_BFDFUICOMM("BFDFUICOMM: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_BFDFUICOMM].taskName, zHcuVmCtrTab.task[TASK_ID_L3BFDF].taskName);
		}
	}

	//校准命令标志位发生了变化
	if (parseResult.cmdCalibration.cmdFlag != gTaskL3bfdfuicommContext.cmdCalibrationFlag)
	{
		gTaskL3bfdfuicommContext.cmdCalibrationFlag = parseResult.cmdCalibration.cmdFlag;
		fileChangeContent = parseResult.cmdCalibration.cmdValue;
		//依赖文件变化的内容，分类发送控制命令：零值校准命令/满值校准命令
		if ((fileChangeContent == HCU_BFDFCOMM_JASON_CMD_CALZERO) || (fileChangeContent == HCU_BFDFCOMM_JASON_CMD_CALFULL)){
			sensorid = parseResult.cmdCalibration.sensorid;
			msg_struct_uicomm_can_test_cmd_req_t snd;
			memset(&snd, 0, sizeof(msg_struct_uicomm_can_test_cmd_req_t));
			snd.length = sizeof(msg_struct_uicomm_can_test_cmd_req_t);
			//check sensor id
			if(sensorid < 1 || sensorid >= HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX)
				HCU_ERROR_PRINT_BFDFUICOMM("BFDFUICOMM: UI input sensorid out of range, [sensorid=%d]! \n", sensorid);

			if (fileChangeContent == HCU_BFDFCOMM_JASON_CMD_CALZERO)
			{
				snd.cmdid = CMDID_SENSOR_COMMAND_CALIBRATION_ZERO;
				snd.cmdvalue = 0;
			}
			else if (fileChangeContent == HCU_BFDFCOMM_JASON_CMD_CALFULL)
			{
				snd.cmdid = CMDID_SENSOR_COMMAND_CALIBRATION_FULL;
				snd.cmdvalue = gTaskL3bfdfContext.wgtSnrPar.calibration[sensorid].WeightSensorCalibrationFullWeight;
			}

			snd.wsBitmap[sensorid] = 1;
			//发送命令给CANITFLEO
			if (hcu_message_send(MSG_ID_UICOMM_CAN_TEST_CMD_REQ, TASK_ID_CANITFLEO, TASK_ID_BFDFUICOMM, &snd, snd.length) == FAILURE)
				HCU_ERROR_PRINT_BFDFUICOMM("BFDFUICOMM: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_BFDFUICOMM].taskName, zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName);
		}
	}

	//暂停命令标志位发生了变化
	if (parseResult.cmdResume.cmdFlag != gTaskL3bfdfuicommContext.cmdResumeFlag)
	{
		gTaskL3bfdfuicommContext.cmdResumeFlag = parseResult.cmdResume.cmdFlag;
		fileChangeContent = parseResult.cmdResume.cmdValue;
		//依赖文件变化的内容，分类发送控制命令：SUSPEND命令/RESUME命令
		if (fileChangeContent == HCU_BFDFCOMM_JASON_CMD_RESUME) {
			//Re-configure/start all weight sensor
			gTaskL3bfdfuicommContext.bfdfuiState = HCU_BFDFCOMM_JASON_CMD_START;
			msg_struct_uicomm_l3bfdf_cfg_req_t snd;
			memset(&snd, 0, sizeof(msg_struct_uicomm_l3bfdf_cfg_req_t));
			snd.length = sizeof(msg_struct_uicomm_l3bfdf_cfg_req_t);
			if (hcu_message_send(MSG_ID_UICOMM_L3BFDF_CFG_REQ, TASK_ID_L3BFDF, TASK_ID_BFDFUICOMM, &snd, snd.length) == FAILURE)
				HCU_ERROR_PRINT_BFDFUICOMM("BFDFUICOMM: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_BFDFUICOMM].taskName, zHcuVmCtrTab.task[TASK_ID_L3BFDF].taskName);
		}
	}

	//一般性测试命令标志位发生了变化
	if (parseResult.cmdTest.cmdFlag != gTaskL3bfdfuicommContext.cmdTestFlag)
	{
		gTaskL3bfdfuicommContext.cmdTestFlag = parseResult.cmdTest.cmdFlag;
		fileChangeContent = parseResult.cmdTest.cmdValue;

		//依赖文件变化的内容，分类发送控制命令：一般性控制命令
		if (fileChangeContent == HCU_BFDFCOMM_JASON_CMD_TEST){
			UINT8 testcmd = 0, testpara = 0;
			sensorid = parseResult.cmdTest.sensorid;
			testcmd = parseResult.cmdTest.testCmd;
			testpara = parseResult.cmdTest.testPara;
			msg_struct_uicomm_can_test_cmd_req_t snd;
			memset(&snd, 0, sizeof(msg_struct_uicomm_can_test_cmd_req_t));
			snd.length = sizeof(msg_struct_uicomm_can_test_cmd_req_t);
			//check sensor id
			if(sensorid < HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX){
				snd.wsBitmap[sensorid] = 1;
			}
			else if (sensorid == HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX){ //set all sensor
				memset(snd.wsBitmap, 1, sizeof(UINT8)*HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX);
			}
			else
				HCU_ERROR_PRINT_BFDFUICOMM("BFDFUICOMM: UI input sensorid out of range, [sensorid=%d]! \n", sensorid);

			snd.cmdid = testcmd;
			snd.cmdvalue = testpara;
			if (hcu_message_send(MSG_ID_UICOMM_CAN_TEST_CMD_REQ, TASK_ID_CANITFLEO, TASK_ID_BFDFUICOMM, &snd, snd.length) == FAILURE)
				HCU_ERROR_PRINT_BFDFUICOMM("BFDFUICOMM: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_BFDFUICOMM].taskName, zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName);
		}
	}

	return SUCCESS;
}

OPSTAT func_bfdfuicomm_time_out_period_read_process(void)
{
//	UINT8 state = FsmGetState(TASK_ID_L3BFDF);

//	if ((state == FSM_STATE_L3BFDF_ACTIVED) || (state == FSM_STATE_L3BFDF_OPR_CFG)) {
//		//启动完成以后，等待一小会儿，然后将缺省的参数读入到系统内存，并发送CFG_REQ给L3BFDF
//		//如果缺省参数读取不成功，等待人工干预并读取，然后再发送给L3BFDF
//		if (func_bfdfuicomm_read_cfg_file_into_ctrl_table() == SUCCESS){
//			msg_struct_uicomm_l3bfdf_cfg_req_t snd;
//			memset(&snd, 0, sizeof(msg_struct_uicomm_l3bfdf_cfg_req_t));
//			snd.length = sizeof(msg_struct_uicomm_l3bfdf_cfg_req_t);
//			if (hcu_message_send(MSG_ID_UICOMM_L3BFDF_CFG_REQ, TASK_ID_L3BFDF, TASK_ID_BFDFUICOMM, &snd, snd.length) == FAILURE)
//				HCU_ERROR_PRINT_BFDFUICOMM("BFDFUICOMM: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_BFDFUICOMM].taskName, zHcuVmCtrTab.task[TASK_ID_L3BFDF].taskName);
//		}
//	}
//	else if (state == FSM_STATE_L3BFDF_OPR_GO) {
//		msg_struct_uicomm_l3bfdf_cmd_req_t snd_start_req;
//		memset(&snd_start_req, 0, sizeof(msg_struct_uicomm_l3bfdf_cmd_req_t))				HCU_ERROR_PRINT_BFDFUICOMM("BFDFUICOMM: UI input sensorid out of range, [sensorid=%d]! \n", sensorid);
	return FAILURE;;
//		snd_start_req.length = sizeof(msg_struct_uicomm_l3bfdf_cmd_req_t);
//		snd_start_req.cmdid = HCU_SYSMSG_BFDF_UICOMM_CMDID_START;
//		if (hcu_message_send(MSG_ID_UICOMM_L3BFDF_CMD_REQ, TASK_ID_L3BFDF, TASK_ID_BFDFUICOMM, &snd_start_req, snd_start_req.length) == FAILURE)
//			HCU_ERROR_PRINT_BFDFUICOMM("BFDFUICOMM: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_BFDFUICOMM].taskName, zHcuVmCtrTab.task[TASK_ID_L3BFDF].taskName);
//		}
//	else{
//
//	}
	return SUCCESS;
}

//命令标志command.Json文件解析
OPSTAT  func_bfdfuicomm_cmdfile_json_parse(char *monitorJsonFile, L3BfdfuiJsonCmdParseResult_t *parseResult )
{
	FILE *fileStream;
	char inotifyReadBuf[1000];
	UINT32  numread = 0;
	UINT32  flag = 0, value = 0, errid = 0;
	UINT8  sensorid = 0;

	if((NULL == monitorJsonFile) || (NULL == parseResult))
		HCU_ERROR_PRINT_BFDFUICOMM("BFDFUICOMM: (NULL == monitorJsonFile) || (NULL == parseResult), return.\n");

	struct json_object *file_jsonobj = NULL;
	struct json_object *cmd_jsonobj = NULL, *flag_jsonobj = NULL, *value_jsonobj = NULL, *sensorid_jsonobj = NULL;

    if ((fileStream = fopen( monitorJsonFile, "rt" )) != NULL )  // 文件读取
    {
			numread = fread( inotifyReadBuf, sizeof( char ), 1000-1, fileStream );
			if (numread == 0){
				hcu_sleep(1);
				numread = fread( inotifyReadBuf, sizeof( char ), 1000-1, fileStream );
				if (numread == 0){
					errid = ferror(fileStream);
					fclose( fileStream );
					HCU_ERROR_PRINT_BFDFUICOMM("BFDFUICOMM: Read NULL command json file, [file=%s] [errid=%d] ! \n", monitorJsonFile, errid);
				}
			}
			fclose( fileStream );

			file_jsonobj = json_tokener_parse(inotifyReadBuf);
			if (file_jsonobj == NULL)
				HCU_ERROR_PRINT_BFDFUICOMM("BFDFUICOMM: Command file json_tokener_parse failure, [file=%s]  ! \n", monitorJsonFile);

			//解析Start/Stop命令
		   if ( json_object_object_get_ex(file_jsonobj, "start_cmd", &cmd_jsonobj)){
			   UINT16 config_index = 0;
			   struct json_object *confindex_jsonobj = NULL;

			   flag_jsonobj = json_object_object_get(cmd_jsonobj, "flag");
			   value_jsonobj = json_object_object_get(cmd_jsonobj, "value");
			   confindex_jsonobj = json_object_object_get(cmd_jsonobj, "confindex");

			   if ((flag_jsonobj != NULL) && (value_jsonobj != NULL)){
				   flag = json_object_get_int(flag_jsonobj);
				   value = json_object_get_int(value_jsonobj);
				   config_index = json_object_get_int(confindex_jsonobj);
				   parseResult->cmdStartStop.cmdFlag = flag;
				   parseResult->cmdStartStop.cmdValue = value;
				   parseResult->cmdStartStop.confindex = config_index;
			   }
			   json_object_put(confindex_jsonobj);  //释放Json Object指针
		   }
		  else
				HCU_ERROR_PRINT_BFDFUICOMM("BFDFUICOMM: Field [start_cmd] dose not exist in command json file ! \n");

		   //解析Calibration命令
		  if (json_object_object_get_ex(file_jsonobj, "calibration_cmd", &cmd_jsonobj)){
			  flag_jsonobj = json_object_object_get(cmd_jsonobj, "flag");
			  value_jsonobj = json_object_object_get(cmd_jsonobj, "value");
			  sensorid_jsonobj =  json_object_object_get(cmd_jsonobj, "sensorid");
			  if ((flag_jsonobj != NULL) && (value_jsonobj != NULL) && (sensorid_jsonobj != NULL)){
				  flag = json_object_get_int(flag_jsonobj);
				  value = json_object_get_int(value_jsonobj);
				  sensorid = json_object_get_int(sensorid_jsonobj);
				  if ((sensorid >0 ) && (sensorid < HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX)){
					  parseResult->cmdCalibration.cmdFlag = flag;
					  parseResult->cmdCalibration.cmdValue = value;
					  parseResult->cmdCalibration.sensorid = sensorid;
					  parseResult->cmdCalibration.weight = gTaskL3bfdfContext.wgtSnrPar.calibration[sensorid].WeightSensorCalibrationFullWeight;
				  }
			  }
		  }
		 else
			 HCU_ERROR_PRINT_BFDFUICOMM("BFDFUICOMM: Field [calibration_cmd] dose not exist in command json file ! \n");

		 //解析Resume命令
		if (json_object_object_get_ex(file_jsonobj, "resume_cmd", &cmd_jsonobj)){
			flag_jsonobj = json_object_object_get(cmd_jsonobj, "flag");
			value_jsonobj = json_object_object_get(cmd_jsonobj, "value");
			if ((flag_jsonobj != NULL) && (value_jsonobj != NULL)){
				flag = json_object_get_int(flag_jsonobj);
				value = json_object_get_int(value_jsonobj);
				parseResult->cmdResume.cmdFlag = flag;
				parseResult->cmdResume.cmdValue = value;
			}
		}
		else
			HCU_ERROR_PRINT_BFDFUICOMM("BFDFUICOMM: Field [resume_cmd] dose not exist in command json file ! \n");

		 //解析Test命令
		if (json_object_object_get_ex(file_jsonobj, "test_cmd", &cmd_jsonobj)){
			UINT32 	testcmd = 0, testpara = 0;
			struct json_object *testcmd_jsonobj = NULL, *testpara_jsonobj = NULL;

			flag_jsonobj = json_object_object_get(cmd_jsonobj, "flag");
			value_jsonobj = json_object_object_get(cmd_jsonobj, "value");
			sensorid_jsonobj =  json_object_object_get(cmd_jsonobj, "sensorid");
			testcmd_jsonobj =  json_object_object_get(cmd_jsonobj, "testcmd");
			testpara_jsonobj =  json_object_object_get(cmd_jsonobj, "testpara");
			if ((flag_jsonobj != NULL) && (value_jsonobj != NULL) && (sensorid_jsonobj != NULL)){
				flag = json_object_get_int(flag_jsonobj);
				value = json_object_get_int(value_jsonobj);
				sensorid = json_object_get_int(sensorid_jsonobj);
				testcmd = json_object_get_int(testcmd_jsonobj);
				testpara = json_object_get_int(testpara_jsonobj);
				parseResult->cmdTest.cmdFlag = flag;
				parseResult->cmdTest.cmdValue = value;
				parseResult->cmdTest.sensorid = sensorid;
				parseResult->cmdTest.testCmd = testcmd;
				parseResult->cmdTest.testPara = testpara;
			}
			//free json object
			json_object_put(testcmd_jsonobj);
			json_object_put(testpara_jsonobj);
		}
		else
			HCU_ERROR_PRINT_BFDFUICOMM("BFDFUICOMM: Field [test_cmd] dose not exist in command json file ! \n");

		 //释放Json Object指针
		json_object_put(file_jsonobj);
		json_object_put(cmd_jsonobj);
		json_object_put(flag_jsonobj);
		json_object_put(value_jsonobj);
		json_object_put(sensorid_jsonobj);

        return SUCCESS;
    }
    else
    	HCU_ERROR_PRINT_BFDFUICOMM("BFDFUICOMM: Open command json file failure, file=%s \n", monitorJsonFile);
}

//扫描文件是否有DEFAULT参数，并配置进入系统参数控制表
OPSTAT func_bfdfuicomm_read_cfg_file_into_ctrl_table (UINT16 config_index)
{
	UINT32  calibrationdata[(HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX-1)*3];
	UINT32 dynamicdata[HCU_SYSCFG_BFDF_DB_COLUMN_NUM_MAX], staticdata[HCU_SYSCFG_BFDF_DB_COLUMN_NUM_MAX];

	//Update config id to gTaskL3bfdfContext
	gTaskL3bfdfContext.configId = config_index;
	memset(&(gTaskL3bfdfContext.staLocalUi), 0, sizeof(HcuSysMsgIeL3bfdfContextStaElement_t));
	memset(&(gTaskL3bfdfContext.staOneMin), 0, sizeof(HcuSysMsgIeL3bfdfContextStaElement_t));
	memset(&(gTaskL3bfdfContext.sta15Min), 0, sizeof(HcuSysMsgIeL3bfdfContextStaElement_t));
	memset(&(gTaskL3bfdfContext.sta60Min), 0, sizeof(HcuSysMsgIeL3bfdfContextStaElement_t));
	memset(&(gTaskL3bfdfContext.sta2H), 0, sizeof(HcuSysMsgIeL3bfdfContextStaElement_t));
	memset(&(gTaskL3bfdfContext.sta8H), 0, sizeof(HcuSysMsgIeL3bfdfContextStaElement_t));
	memset(&(gTaskL3bfdfContext.sta24H), 0, sizeof(HcuSysMsgIeL3bfdfContextStaElement_t));
	memset(&(gTaskL3bfdfContext.staUp2Now), 0, sizeof(HcuSysMsgIeL3bfdfContextStaElement_t));

	//查询用户动态配置参数
	if (dbi_HcuBfdf_DynamicConfigDataGet(config_index, dynamicdata) == FAILURE){
		HCU_ERROR_PRINT_BFDFUICOMM("BFDFUICOMM: Get DB algorithm data failed \n");
	}
	UINT8 i = 7;   //数据表单hcubfdfconfigpara前7个参数为UI界面使用参数
	gTaskL3bfdfContext.comAlgPar.MinScaleNumberCombination = dynamicdata[i++];
	gTaskL3bfdfContext.comAlgPar.MaxScaleNumberCombination  = dynamicdata[i++];
	gTaskL3bfdfContext.comAlgPar.MinScaleNumberStartCombination = dynamicdata[i++];
	gTaskL3bfdfContext.comAlgPar.TargetCombinationWeight = dynamicdata[i++];
	gTaskL3bfdfContext.comAlgPar.TargetCombinationUpperWeight = dynamicdata[i++];
	gTaskL3bfdfContext.comAlgPar.IsProximitCombinationMode = dynamicdata[i++];
	gTaskL3bfdfContext.comAlgPar.CombinationBias =  dynamicdata[i++];
	gTaskL3bfdfContext.comAlgPar.IsRemainDetectionEnable = dynamicdata[i++];
	gTaskL3bfdfContext.comAlgPar.RemainDetectionTimeSec  = dynamicdata[i++];
	gTaskL3bfdfContext.comAlgPar.RemainScaleTreatment =  dynamicdata[i++];
	gTaskL3bfdfContext.comAlgPar.IsPriorityScaleEnabled = dynamicdata[i++];
	gTaskL3bfdfContext.comAlgPar.CombinationAutoMode = dynamicdata[i++];
	gTaskL3bfdfContext.comAlgPar.MovingAvrageSpeedCount = dynamicdata[i++];
	gTaskL3bfdfContext.wgtSnrPar.WeightSensorLoadDetectionTimeMs =  dynamicdata[i++];
	gTaskL3bfdfContext.wgtSnrPar.WeightSensorLoadThread = dynamicdata[i++];
	gTaskL3bfdfContext.wgtSnrPar.WeightSensorEmptyDetectionTimeMs = dynamicdata[i++];
	gTaskL3bfdfContext.wgtSnrPar.WeightSensorEmptyThread = dynamicdata[i++];
	gTaskL3bfdfContext.wgtSnrPar.StardardReadyTimeMs = dynamicdata[i++];
	gTaskL3bfdfContext.motCtrPar.MotorSpeed = dynamicdata[i++];
	gTaskL3bfdfContext.motCtrPar.MotorDirection = dynamicdata[i++];
	gTaskL3bfdfContext.motCtrPar.MotorRollingStartMs = dynamicdata[i++];
	gTaskL3bfdfContext.motCtrPar.MotorRollingStopMs  = dynamicdata[i++];
	gTaskL3bfdfContext.motCtrPar.MotorRollingInveralMs = dynamicdata[i++];
	gTaskL3bfdfContext.motCtrPar.MotorFailureDetectionVaration = dynamicdata[i++];
	gTaskL3bfdfContext.motCtrPar.MotorFailureDetectionTimeMs = dynamicdata[i++];
	//重复参数
	gTaskL3bfdfContext.wgtSnrPar.RemainDetectionTimeSec = gTaskL3bfdfContext.comAlgPar.RemainDetectionTimeSec;

	//查询系统静态配置参数
	if (dbi_HcuBfdf_StaticConfigDataGet(staticdata) == FAILURE){
		HCU_ERROR_PRINT_BFDFUICOMM("BFDFUICOMM: Get DB algorithm data failed \n");
	}

	i =1;
	gTaskL3bfdfContext.wgtSnrPar.MaxAllowedWeight = staticdata[i++];

	i++;  // calibration full weight
	gTaskL3bfdfContext.wgtSnrPar.WeightSensorAdcGain = staticdata[i++];
	gTaskL3bfdfContext.wgtSnrPar.WeightSensorAdcSampleFreq = staticdata[i++];
	gTaskL3bfdfContext.wgtSnrPar.WeightSensorStaticZeroValue = staticdata[i++];
	gTaskL3bfdfContext.wgtSnrPar.WeightSensorTailorValue = staticdata[i++];
	gTaskL3bfdfContext.wgtSnrPar.WeightSensorDynamicZeroThreadValue = staticdata[i++];
	gTaskL3bfdfContext.wgtSnrPar.WeightSensorDynamicZeroHysteresisMs = staticdata[i++];

	gTaskL3bfdfContext.comAlgPar.CombinationSpeedMode = 0;
	gTaskL3bfdfContext.wgtSnrPar.WeightSensorPickupThread = 300;
	gTaskL3bfdfContext.wgtSnrPar.WeightSensorPickupDetectionTimeMs = 500;
	gTaskL3bfdfContext.wgtSnrPar.WeightSensorInitOrNot = 0;
	gTaskL3bfdfContext.wgtSnrPar.WeightSensorAdcBitwidth = 22;
	gTaskL3bfdfContext.wgtSnrPar.WeightSensorAdcValue = 0;

	gTaskL3bfdfContext.wgtSnrPar.WeightSensorFilterCoeff[0] = 0;				// NOT for GUI
	gTaskL3bfdfContext.wgtSnrPar.WeightSensorFilterCoeff[1] = 0;				// NOT for GUI
	gTaskL3bfdfContext.wgtSnrPar.WeightSensorFilterCoeff[2] = 0;				// NOT for GUI
	gTaskL3bfdfContext.wgtSnrPar.WeightSensorFilterCoeff[3] = 0;				// NOT for GUI
	gTaskL3bfdfContext.wgtSnrPar.WeightSensorOutputValue[0] = 1;				// NOT for GUI
	gTaskL3bfdfContext.wgtSnrPar.WeightSensorOutputValue[1] = 0;				// NOT for GUI
	gTaskL3bfdfContext.wgtSnrPar.WeightSensorOutputValue[2] = 0;				// NOT for GUI
	gTaskL3bfdfContext.wgtSnrPar.WeightSensorOutputValue[3] = 0;				// NOT for GUI

	//查询校准数据
	if (dbi_HcuBfdf_CalibrationDataGet(calibrationdata) == FAILURE){
		HCU_ERROR_PRINT_BFDFUICOMM("BFDFUICOMM: Get DB sensor calibration data failed \n");
	}

	for (i = 1; i <= HCU_SYSCFG_BFDF_SNR_WS_NBR_MAX-1; i++){
		gTaskL3bfdfContext.wgtSnrPar.calibration[i].WeightSensorCalibrationZeroAdcValue = calibrationdata[3*(i-1)];
		gTaskL3bfdfContext.wgtSnrPar.calibration[i].WeightSensorCalibrationFullAdcValue = calibrationdata[3*(i-1)+1];
		gTaskL3bfdfContext.wgtSnrPar.calibration[i].WeightSensorCalibrationFullWeight = calibrationdata[3*(i-1)+2];
	}

	return SUCCESS;
}*/

//扫描文件是否有DEFAULT参数，并配置进入系统参数控制表
OPSTAT func_bfdfuicomm_read_cfg_file_into_ctrl_table (UINT16 config_index)
{
	//Update config id to gTaskL3bfdfContext
	gTaskL3bfdfContext.configId = config_index;

	//查询用户动态配置参数
	gTaskL3bfdfContext.wgtSnrPar.WeightSensorLoadDetectionTimeMs =  1;
	gTaskL3bfdfContext.wgtSnrPar.WeightSensorLoadThread = 1;
	gTaskL3bfdfContext.wgtSnrPar.WeightSensorEmptyDetectionTimeMs = 1;
	gTaskL3bfdfContext.wgtSnrPar.WeightSensorEmptyThread = 1;
	gTaskL3bfdfContext.wgtSnrPar.StardardReadyTimeMs = 1;

	gTaskL3bfdfContext.wgtSnrPar.MaxAllowedWeight = 1;
	gTaskL3bfdfContext.wgtSnrPar.WeightSensorStaticZeroValue = 1;
	gTaskL3bfdfContext.wgtSnrPar.WeightSensorTailorValue = 1;
	gTaskL3bfdfContext.wgtSnrPar.WeightSensorDynamicZeroThreadValue = 1;
	gTaskL3bfdfContext.wgtSnrPar.WeightSensorDynamicZeroHysteresisMs = 1;

	gTaskL3bfdfContext.wgtSnrPar.WeightSensorPickupThread = 300;
	gTaskL3bfdfContext.wgtSnrPar.WeightSensorPickupDetectionTimeMs = 500;

	//查询校准数据

	int i =0;
	//分配Hooper数据：暂时没有考虑相应板子的启动状态
	//Hopper初始化
	for (i = 0; i< HCU_SYSCFG_BFDF_EQU_FLOW_NBR_MAX; i++){
		func_l3bfdf_hopper_state_set_init(i);
	}

	int nbrGroup = 0;
	//第0个流水线，分配组别
	nbrGroup = rand()%3+1;
	func_l3bfdf_group_allocation(0, nbrGroup);
	func_l3bfdf_hopper_add_by_grp_in_average_distribution(0, nbrGroup);
	//设置小组重量范围：数据均为NF2进行设置
	func_l3bfdf_group_auto_alloc_init_range_in_average(0, nbrGroup, 10000, 100000);
	//设置重量目标
	func_l3bfdf_group_auto_alloc_init_target_with_uplimit(0, 1000000, 3);

	//第1个流水线，分配组别
/*	nbrGroup = rand()%3+1;
	func_l3bfdf_group_allocation(1, nbrGroup);
	func_l3bfdf_hopper_add_by_grp_in_average_distribution(1, nbrGroup);
	func_l3bfdf_group_auto_alloc_init_range_in_average(1, nbrGroup, 20000, 200000);
	func_l3bfdf_group_auto_alloc_init_target_with_uplimit(1, 2000000, 40);*/

	//打印
	for (i = 0; i< HCU_SYSCFG_BFDF_EQU_FLOW_NBR_MAX; i++){
		func_l3bfdf_print_all_hopper_status_by_id(i);
	}

	//手工浏览一遍双链表
	for (i = 0; i< HCU_SYSCFG_BFDF_EQU_FLOW_NBR_MAX; i++){
		func_l3bfdf_print_all_hopper_status_by_chain(i);
	}

	//自动Audit过程
	int res = func_l3bfdf_hopper_dual_chain_audit();
	if (res < 0) HCU_ERROR_PRINT_BFDFUICOMM("BSDFUICOMM: Audit error, errCode = %d\n", res);

	//批次数据更新
	//读取数据库，更新批次数据
	gTaskL3bfdfContext.sessionId = 1;

	return SUCCESS;
}


