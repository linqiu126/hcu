/*
 * bfhsuicomm.c
 *
 *  Created on: 2016年5月14日
 *      Author: hitpony
 */

#include "bfhsuicomm.h"
//#include  "jsoninotify.h"

#include "../l0service/timer.h"
#include "../l0service/trace.h"
#include "../l1com/l1comdef.h"
//#include <json-c/json.h>
//#include <json-c/json_object.h>

#include "../l3appctrl/l3bfhs.h"

/*
** FSM of the BFHSUICOMM
*/
HcuFsmStateItem_t HcuFsmBfhsuicomm[] =
{
    //MessageId                 //State                   		 		//Function
	//启始点，固定定义，不要改动, 使用ENTRY/END，意味者MSGID肯定不可能在某个高位区段中；考虑到所有任务共享MsgId，即使分段，也无法实现
	//完全是为了给任务一个初始化的机会，按照状态转移机制，该函数不具备启动的机会，因为任务初始化后自动到FSM_STATE_IDLE
	//如果没有必要进行初始化，可以设置为NULL
	{MSG_ID_ENTRY,       					FSM_STATE_ENTRY,            				fsm_bfhsuicomm_task_entry}, //Starting

	//System level initialization, only controlled by HCU-MAIN
    {MSG_ID_COM_INIT,       				FSM_STATE_IDLE,            					fsm_bfhsuicomm_init},
    {MSG_ID_COM_INIT_FEEDBACK,				FSM_STATE_IDLE,            					fsm_com_do_nothing},

	//Task level initialization
    {MSG_ID_COM_INIT,       				FSM_STATE_BFHSUICOMM_INITED,            	fsm_bfhsuicomm_init},
    {MSG_ID_COM_INIT_FEEDBACK,				FSM_STATE_BFHSUICOMM_INITED,            	fsm_com_do_nothing},

	//ANY state entry
    {MSG_ID_COM_INIT_FEEDBACK,				FSM_STATE_COMMON,          					fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT,       			FSM_STATE_COMMON,          					fsm_com_heart_beat_rcv},
	{MSG_ID_COM_STOP,       				FSM_STATE_COMMON,          					fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT_FB,       		FSM_STATE_COMMON,          					fsm_com_do_nothing},
    {MSG_ID_COM_RESTART,					FSM_STATE_COMMON,            				fsm_bfhsuicomm_restart},
	{MSG_ID_COM_TIME_OUT,       			FSM_STATE_COMMON,          					fsm_bfhsuicomm_timeout},

    //Normal working status
	{MSG_ID_L3BFHS_UICOMM_CTRL_CMD_RESP,   		FSM_STATE_BFHSUICOMM_ACTIVED,      		fsm_bfhsuicomm_l3bfhs_ctrl_cmd_resp},	//人工控制反馈
	{MSG_ID_SUI_TEST_CMD_RESP,      			FSM_STATE_BFHSUICOMM_ACTIVED,      		fsm_bfhsuicomm_sui_test_cmd_resp},  //测试命令反馈

	//UIR2HCU MSG RCV
	{MSG_ID_HUICOBUS_UIR_INIT_REQ,      		FSM_STATE_BFHSUICOMM_ACTIVED,          	fsm_bfhsuicomm_huicobus_uir_init_req},
	{MSG_ID_HUICOBUS_UIR_START_RESUME_REQ, 		FSM_STATE_BFHSUICOMM_ACTIVED,          	fsm_bfhsuicomm_huicobus_uir_start_resume_req},
	{MSG_ID_HUICOBUS_UIR_STOP_SUSPEND_REQ, 		FSM_STATE_BFHSUICOMM_ACTIVED,          	fsm_bfhsuicomm_huicobus_uir_stop_suspend_req},
	{MSG_ID_HUICOBUS_UIR_STATIC_CALI_REQ,      	FSM_STATE_BFHSUICOMM_ACTIVED,          	fsm_bfhsuicomm_huicobus_uir_static_cali_req},
	{MSG_ID_HUICOBUS_UIR_DYNAMIC_CALI_REQ,    	FSM_STATE_BFHSUICOMM_ACTIVED,          	fsm_bfhsuicomm_huicobus_uir_dynamic_cali_req},
	{MSG_ID_HUICOBUS_UIR_TEST_CMD_REQ,      	FSM_STATE_BFHSUICOMM_ACTIVED,          	fsm_bfhsuicomm_huicobus_uir_test_cmd_req},
	{MSG_ID_HUICOBUS_UIR_ONE_KEY_ZERO_REQ, 		FSM_STATE_BFHSUICOMM_ACTIVED,         	fsm_bfhsuicomm_huicobus_uir_one_key_zero_req},

    //结束点，固定定义，不要改动
    {MSG_ID_END,            	FSM_STATE_END,             				NULL},  //Ending
};

//Global variables
extern gTaskL3bfhsContext_t gTaskL3bfhsContext;

//Main Entry
//Input parameter would be useless, but just for similar structure purpose
OPSTAT fsm_bfhsuicomm_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//除了对全局变量进行操作之外，尽量不要做其它操作，因为该函数将被主任务/线程调用，不是本任务/线程调用
	//该API就是给本任务一个提早介入的入口，可以帮着做些测试性操作
	if (FsmSetState(TASK_ID_BFHSUICOMM, FSM_STATE_IDLE) == FAILURE){
		HcuErrorPrint("BFHSUICOMM: Error Set FSM State at fsm_bfhsuicomm_task_entry\n");}
	return SUCCESS;
}

OPSTAT fsm_bfhsuicomm_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;

	if ((src_id > TASK_ID_MIN) &&(src_id < TASK_ID_MAX)){
		//Send back MSG_ID_COM_INIT_FEEDBACK to SVRCON
		msg_struct_com_init_feedback_t snd0;
		memset(&snd0, 0, sizeof(msg_struct_com_init_feedback_t));
		snd0.length = sizeof(msg_struct_com_init_feedback_t);

		//to avoid all task send out the init fb msg at the same time which lead to msgque get stuck
		hcu_usleep(dest_id*HCU_SYSCFG_DURATION_OF_INIT_FB_WAIT_MAX);

		ret = hcu_message_send(MSG_ID_COM_INIT_FEEDBACK, src_id, TASK_ID_BFHSUICOMM, &snd0, snd0.length);
		if (ret == FAILURE){
			HcuErrorPrint("BFHSUICOMM: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_BFHSUICOMM].taskName, zHcuVmCtrTab.task[src_id].taskName);
			return FAILURE;
		}
	}

	//收到初始化消息后，进入初始化状态
	if (FsmSetState(TASK_ID_BFHSUICOMM, FSM_STATE_BFHSUICOMM_INITED) == FAILURE){
		HcuErrorPrint("BFHSUICOMM: Error Set FSM State!\n");
		return FAILURE;
	}

	//Global Variables
	zHcuSysStaPm.taskRunErrCnt[TASK_ID_BFHSUICOMM] = 0;

	//启动MQTT服务内容

	//启动周期性定时器
/*
	ret = hcu_timer_start(TASK_ID_BFHSUICOMM, TIMER_ID_1S_BFHSUICOMM_PERIOD_READ, \
			zHcuSysEngPar.timer.array[TIMER_ID_1S_BFHSUICOMM_PERIOD_READ].dur, TIMER_TYPE_PERIOD, TIMER_RESOLUTION_1S);
*/

	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_BFHSUICOMM, FSM_STATE_BFHSUICOMM_ACTIVED) == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_BFHSUICOMM]++;
		HcuErrorPrint("BFHSUICOMM: Error Set FSM State!\n");
		return FAILURE;
	}
	if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_FAT_ON) != FALSE){
		HcuDebugPrint("BFHSUICOMM: Enter FSM_STATE_BFHSUICOMM_ACTIVED status, Keeping refresh here!\n");
	}

	//延迟并启动系统，进入测试模式
	hcu_sleep(4);
	//设置configIndex=1
	func_bfhsuicomm_read_cfg_db_into_ctrl_table(1);

	//发送启动消息给L3BFHS：有了界面后，这个就不需要了，以后需要删除
//	msg_struct_uicomm_l3bfhs_ctrl_cmd_req_t snd;
//	memset(&snd, 0, sizeof(msg_struct_uicomm_l3bfhs_ctrl_cmd_req_t));
//	snd.cmdid = HCU_SYSMSG_BFHS_UICOMM_CMDID_CFG_START;
//	snd.length = sizeof(msg_struct_uicomm_l3bfhs_ctrl_cmd_req_t);
//	ret = hcu_message_send(MSG_ID_UICOMM_L3BFHS_CTRL_CMD_REQ, TASK_ID_L3BFHS, TASK_ID_BFHSUICOMM, &snd, snd.length);
//	if (ret == FAILURE){
//		HcuErrorPrint("BFHSUICOMM: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_BFHSUICOMM].taskName, zHcuVmCtrTab.task[TASK_ID_L3BFHS].taskName);
//		return FAILURE;
//	}

	//TEST MQTT
//	while(1){
//		StrHlcIe_cui_hcu2uir_inswgt_bfhs_report_t buf;
//		buf.weight = rand()%500;
//		buf.wmcState = 1;
//		hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_inswgt_bfhs_report(buf.weight, &buf);

//		msg_struct_com_mqtt_send_t snd;
//		memset(&snd, 0, sizeof(msg_struct_com_mqtt_send_t));
//		snd.cmdId = HUICOBUS_CMDID_cui_hcu2uir_inswgt_bfhs_report;
//		snd.cmdValue = buf.weight;
//		snd.destId = HUICOBUS_MQTT_CLID_UIROUTER;
//		snd.topicId = HUICOBUS_MQTT_TPID_HCU2UIR;
//		snd.length = 11;
//		hcu_mqtt_msg_send_syn_mode(&snd);
//		hcu_sleep(2);
//	}

	//返回
	return SUCCESS;
}

OPSTAT fsm_bfhsuicomm_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HcuErrorPrint("BFHSUICOMM: Internal error counter reach DEAD level, SW-RESTART soon!\n");
	zHcuSysStaPm.statisCnt.restartCnt++;
	fsm_bfhsuicomm_init(0, 0, NULL, 0);
	return SUCCESS;
}

//暂时啥都不干，定时到达后，还不明确是否需要支持定时工作
OPSTAT fsm_bfhsuicomm_timeout(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret;

	//Receive message and copy to local variable
	msg_struct_com_time_out_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_com_time_out_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_com_time_out_t))){
		HcuErrorPrint("BFHSUICOMM: Receive message error!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_BFHSUICOMM]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//钩子在此处，检查zHcuRunErrCnt[TASK_ID_BFHSUICOMM]是否超限
	if (zHcuSysStaPm.taskRunErrCnt[TASK_ID_BFHSUICOMM] > HCU_RUN_ERROR_LEVEL_2_MAJOR){
		//减少重复RESTART的概率
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_BFHSUICOMM] = zHcuSysStaPm.taskRunErrCnt[TASK_ID_BFHSUICOMM] - HCU_RUN_ERROR_LEVEL_2_MAJOR;
		msg_struct_com_restart_t snd0;
		memset(&snd0, 0, sizeof(msg_struct_com_restart_t));
		snd0.length = sizeof(msg_struct_com_restart_t);
		ret = hcu_message_send(MSG_ID_COM_RESTART, TASK_ID_BFHSUICOMM, TASK_ID_BFHSUICOMM, &snd0, snd0.length);
		if (ret == FAILURE){
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_BFHSUICOMM]++;
			HcuErrorPrint("BFHSUICOMM: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_BFHSUICOMM].taskName, zHcuVmCtrTab.task[TASK_ID_BFHSUICOMM].taskName);
			return FAILURE;
		}
	}

	//Period time out received
	if ((rcv.timeId == TIMER_ID_1S_BFHSUICOMM_PERIOD_READ) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		//保护周期读数的优先级，强制抢占状态，并简化问题
		if (FsmGetState(TASK_ID_BFHSUICOMM) != FSM_STATE_BFHSUICOMM_ACTIVED){
			ret = FsmSetState(TASK_ID_BFHSUICOMM, FSM_STATE_BFHSUICOMM_ACTIVED);
			if (ret == FAILURE){
				zHcuSysStaPm.taskRunErrCnt[TASK_ID_BFHSUICOMM]++;
				HcuErrorPrint("BFHSUICOMM: Error Set FSM State!\n");
				return FAILURE;
			}//FsmSetState
		}


		//Do nothing
		func_bfhsuicomm_time_out_period_read_process();
	}
	return SUCCESS;
}

//启动停止反馈
OPSTAT fsm_bfhsuicomm_l3bfhs_ctrl_cmd_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HCU_MSG_RCV_CHECK_FOR_GEN_LOCAL(TASK_ID_BFHSUICOMM, msg_struct_l3bfhs_uicomm_ctrl_cmd_resp_t);

	if(rcv.cmdid == HCU_SYSMSG_BFHS_UICOMM_CMDID_CFG_START){
		//通知界面
		StrHlcIe_cui_hcu2uir_status_report_t status;
		memset(&status, 0, sizeof(StrHlcIe_cui_hcu2uir_status_report_t));
		if(rcv.validFlag == TRUE)
			status.boardStatus = HUICOBUS_CMDID_CUI_HCU2UIR_GENERAL_CMDVAL_CFG_ERR;
		else
			status.boardStatus = HUICOBUS_CMDID_CUI_HCU2UIR_GENERAL_CMDVAL_CFG_OK;

		hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_status_report(0, &status);
	}
	else if(rcv.cmdid == HCU_SYSMSG_BFHS_UICOMM_CMDID_STOP){
		//通知界面
		StrHlcIe_cui_hcu2uir_status_report_t status;
		memset(&status, 0, sizeof(StrHlcIe_cui_hcu2uir_status_report_t));
		status.boardStatus = HUICOBUS_CMDID_CUI_HCU2UIR_GENERAL_CMDVAL_STOP;

		hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_status_report(0, &status);
	}
	else if(rcv.cmdid == HCU_SYSMSG_BFHS_UICOMM_CMDID_SUSPEND){
		//通知界面
		StrHlcIe_cui_hcu2uir_status_report_t status;
		memset(&status, 0, sizeof(StrHlcIe_cui_hcu2uir_status_report_t));
		status.boardStatus = HUICOBUS_CMDID_CUI_HCU2UIR_GENERAL_CMDVAL_SUSPEND;
	}
	else if(rcv.cmdid == HCU_SYSMSG_BFHS_UICOMM_CMDID_RESUME){
		//通知界面
		StrHlcIe_cui_hcu2uir_status_report_t status;
		memset(&status, 0, sizeof(StrHlcIe_cui_hcu2uir_status_report_t));
		status.boardStatus = HUICOBUS_CMDID_CUI_HCU2UIR_GENERAL_CMDVAL_RESUME;
	}
	else if(rcv.cmdid == HCU_SYSMSG_BFHS_UICOMM_CMDID_STATIC_CALI){
		//通知界面
		StrHlcIe_cui_hcu2uir_static_cali_resp_t status;
		memset(&status, 0, sizeof(StrHlcIe_cui_hcu2uir_static_cali_resp_t));

		if (rcv.cmdValue == HCU_SYSMSG_BFHS_UICOMM_CMDVALUE_STATIC_CALI_ZERO){
			status.validFlag = rcv.validFlag;
			status.errCode = rcv.errCode;
			status.weight = 0;
		}
		else if (rcv.cmdValue == HCU_SYSMSG_BFHS_UICOMM_CMDVALUE_STATIC_CALI_FULL){
			status.validFlag = rcv.validFlag;
			status.errCode = rcv.errCode;
			status.weight = rcv.calFullRespPar.Weight;
			printf("BFHSUICOMM: STATIC_CALI_FULL weight = %d \n", status.weight);
		}
		hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_static_cali_resp(rcv.cmdValue, &status);
	}
	else if(rcv.cmdid == HCU_SYSMSG_BFHS_UICOMM_CMDID_DYNAMIC_CALI){
		StrHlcIe_cui_hcu2uir_dynamic_cali_resp_t status;
		memset(&status, 0, sizeof(StrHlcIe_cui_hcu2uir_dynamic_cali_resp_t));

		if (rcv.cmdValue == HCU_SYSMSG_BFHS_UICOMM_CMDVALUE_DYNAMIC_CALI_ZERO){
			status.validFlag = rcv.validFlag;
			status.errCode = rcv.errCode;
			status.weight = 0;
		}
		else if (rcv.cmdValue == HCU_SYSMSG_BFHS_UICOMM_CMDVALUE_DYNAMIC_CALI_FULL){
			status.validFlag = rcv.validFlag;
			status.errCode = rcv.errCode;
			status.weight = rcv.calFullRespPar.Weight;
		}
		hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_dynamic_cali_resp(rcv.cmdValue, &status);
	}
	else if (rcv.cmdid == HCU_SYSMSG_BFHS_UICOMM_CMDID_ONE_KEY_ZERO){
		//TBD
		printf ("fsm_bfhsuicomm_l3bfhs_ctrl_cmd_resp: rcv.cmdid = %d\n", rcv.cmdid);
	}
	else{
		HCU_ERROR_PRINT_TASK(TASK_ID_BFHSUICOMM, "TASK_ID_BFHSUICOMM: Invalid command response!\n");
		return FAILURE;
	}

	//返回
	return SUCCESS;
}

//一般性测试命令的反馈
OPSTAT fsm_bfhsuicomm_sui_test_cmd_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
//	int ret=0;
//	UINT32  adcvalue = 0;
//	UINT16 	errorcode;
//	UINT8  	sensorid = 0;
//	UINT8	validFlag = 0;
//
//	msg_struct_can_uicomm_test_cmd_resp_t rcv;
//	memset(&rcv, 0, sizeof(msg_struct_can_uicomm_test_cmd_resp_t));
//	if ((param_ptr == NULL || param_len > sizeof(msg_struct_can_uicomm_test_cmd_resp_t)))
//		HCU_ERROR_PRINT_BFHSUICOMM("BFHSUICOMM: Receive message error!\n");
//	memcpy(&rcv, param_ptr, param_len);
//
//	//Special process for calibration command, save result into table `hcubfhscalibration`
//	if (rcv.cmdid == CMDID_SENSOR_COMMAND_CALIBRATION_ZERO){
//		sensorid = rcv.sensorid;
//		adcvalue = rcv.cmdvalue1;
//		if(sensorid < 1 || sensorid >= HCU_SYSCFG_BFHS_SNR_WS_NBR_MAX)
//			HCU_ERROR_PRINT_BFHSUICOMM("BFHSUICOMM: fsm_bfhsuicomm_can_test_cmd_resp's sensorid out of range, [sensorid=%d]! \n", sensorid);
//		ret = dbi_HcuBfhs_CalibrationDataUpdate_adczero(adcvalue, sensorid);
//		if (ret == FAILURE) HCU_ERROR_PRINT_TASK(TASK_ID_BFHSUICOMM, "TASK_ID_BFHSUICOMM: Save data error!\n");
//	}
//	else if (rcv.cmdid == CMDID_SENSOR_COMMAND_CALIBRATION_FULL){
//		sensorid = rcv.sensorid;
//		adcvalue = rcv.cmdvalue1;
//		if(sensorid < 1 || sensorid >= HCU_SYSCFG_BFHS_SNR_WS_NBR_MAX)
//			HCU_ERROR_PRINT_BFHSUICOMM("BFHSUICOMM: fsm_bfhsuicomm_can_test_cmd_resp's sensorid out of range, [sensorid=%d]! \n", sensorid);
//		ret = dbi_HcuBfhs_CalibrationDataUpdate_adcfull(adcvalue, sensorid);
//		if (ret == FAILURE) HCU_ERROR_PRINT_TASK(TASK_ID_BFHSUICOMM, "TASK_ID_BFHSUICOMM: Save data error!\n");
//	}
//	//Common process for other test command, save the response to table 'hcubfhsfb2ui'
//	else{
//		UINT32 cmdValue1 = 0, cmdValue2 = 0;
//		char strResp[DBI_MAX_FBINFO_STRING_LENGTH];
//		validFlag = rcv.validFlag;
//		if (validFlag){
//			sensorid = rcv.sensorid;
//			errorcode = rcv.errCode;
//			cmdValue1 = rcv.cmdvalue1;
//			cmdValue2 = rcv.cmdvalue2;
//			sprintf(strResp, "Test command execute success! cmdValue1=%d, cmdValue2=%d", cmdValue1, cmdValue2);
//		}
//		else{
//			errorcode = rcv.errCode;
//			sprintf(strResp, "Test command execute failure! error code = %d", errorcode);
//		}
//
//		//Save command response result to DB
//		ret = dbi_HcuBfhs_TestCmdRespUpdate(rcv.cmdid, validFlag, strResp);
//		if (ret == FAILURE) HCU_ERROR_PRINT_TASK(TASK_ID_BFHSUICOMM, "TASK_ID_BFHSUICOMM: Save data error!\n");
//
//	}

	//返回
	return SUCCESS;
}

OPSTAT fsm_bfhsuicomm_huicobus_uir_init_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HCU_MSG_RCV_CHECK_FOR_GEN_LOCAL(TASK_ID_BFHSUICOMM, msg_struct_huicobus_uir_init_req_t);

	//查询gTaskL3bfhsContext状态，判定下位机状态
	if (gTaskL3bfhsContext.sensorWs[0].nodeStatus >= HCU_L3BFHS_NODE_BOARD_STATUS_STARTUP){
		hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_init_resp(HUICOBUS_CMDID_CUI_HCU2UIR_GENERAL_CMDVAL_STARTUP);
	}

	//if (wsState == INIT, DOWNLOAD) return SW LOADING

	//if (wsState == ACTIVE) return COMPL

	//假设状态正常
	hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_init_resp(HUICOBUS_CMDID_CUI_HCU2UIR_GENERAL_CMDVAL_OFFLINE);

	return SUCCESS;
}

OPSTAT fsm_bfhsuicomm_huicobus_uir_start_resume_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HCU_MSG_RCV_CHECK_FOR_GEN_LOCAL(TASK_ID_BFHSUICOMM, msg_struct_huicobus_uir_start_resume_req_t);
	msg_struct_uicomm_l3bfhs_ctrl_cmd_req_t snd;
	memset(&snd, 0, sizeof(msg_struct_uicomm_l3bfhs_ctrl_cmd_req_t));
	snd.length = sizeof(msg_struct_uicomm_l3bfhs_ctrl_cmd_req_t);
	snd.cmdid = HCU_SYSMSG_BFHS_UICOMM_CMDID_CFG_START;
	snd.cmdValue = rcv.cmdValue;
	HCU_MSG_SEND_GENERNAL_PROCESS(MSG_ID_UICOMM_L3BFHS_CTRL_CMD_REQ, TASK_ID_L3BFHS, TASK_ID_BFHSUICOMM);
	return SUCCESS;
}

OPSTAT fsm_bfhsuicomm_huicobus_uir_stop_suspend_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HCU_MSG_RCV_CHECK_FOR_GEN_LOCAL(TASK_ID_BFHSUICOMM, msg_struct_huicobus_uir_stop_suspend_req_t);
	msg_struct_uicomm_l3bfhs_ctrl_cmd_req_t snd;
	memset(&snd, 0, sizeof(msg_struct_uicomm_l3bfhs_ctrl_cmd_req_t));
	snd.length = sizeof(msg_struct_uicomm_l3bfhs_ctrl_cmd_req_t);
	snd.cmdid = HCU_SYSMSG_BFHS_UICOMM_CMDID_STOP;
	snd.cmdValue = rcv.cmdValue;
	HCU_MSG_SEND_GENERNAL_PROCESS(MSG_ID_UICOMM_L3BFHS_CTRL_CMD_REQ, TASK_ID_L3BFHS, TASK_ID_BFHSUICOMM);
	return SUCCESS;
}

OPSTAT fsm_bfhsuicomm_huicobus_uir_static_cali_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HCU_MSG_RCV_CHECK_FOR_GEN_LOCAL(TASK_ID_BFHSUICOMM, msg_struct_huicobus_uir_static_cali_req_t);
	msg_struct_uicomm_l3bfhs_ctrl_cmd_req_t snd;
	memset(&snd, 0, sizeof(msg_struct_uicomm_l3bfhs_ctrl_cmd_req_t));
	snd.length = sizeof(msg_struct_uicomm_l3bfhs_ctrl_cmd_req_t);
	snd.cmdid = HCU_SYSMSG_BFHS_UICOMM_CMDID_STATIC_CALI;
	if(rcv.cmdValue == HUICOBUS_CMDVALUE_static_cali_zero)
		snd.cmdValue = HCU_SYSMSG_BFHS_UICOMM_CMDVALUE_STATIC_CALI_ZERO;
	else if (rcv.cmdValue == HUICOBUS_CMDVALUE_static_cali_full)
		snd.cmdValue = HCU_SYSMSG_BFHS_UICOMM_CMDVALUE_STATIC_CALI_FULL;
	else
		snd.cmdValue = HCU_SYSMSG_BFHS_UICOMM_CMDVALUE_INVALID;

	HCU_MSG_SEND_GENERNAL_PROCESS(MSG_ID_UICOMM_L3BFHS_CTRL_CMD_REQ, TASK_ID_L3BFHS, TASK_ID_BFHSUICOMM);
	return SUCCESS;
}

OPSTAT fsm_bfhsuicomm_huicobus_uir_dynamic_cali_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HCU_MSG_RCV_CHECK_FOR_GEN_LOCAL(TASK_ID_BFHSUICOMM, msg_struct_huicobus_uir_dynamic_cali_req_t);
	msg_struct_uicomm_l3bfhs_ctrl_cmd_req_t snd;
	memset(&snd, 0, sizeof(msg_struct_uicomm_l3bfhs_ctrl_cmd_req_t));
	snd.length = sizeof(msg_struct_uicomm_l3bfhs_ctrl_cmd_req_t);
	snd.cmdid = HCU_SYSMSG_BFHS_UICOMM_CMDID_DYNAMIC_CALI;
	if(rcv.cmdValue == HUICOBUS_CMDVALUE_dynamic_cali_zero)
		snd.cmdValue = HCU_SYSMSG_BFHS_UICOMM_CMDVALUE_DYNAMIC_CALI_ZERO;
	else if (rcv.cmdValue == HUICOBUS_CMDVALUE_dynamic_cali_full)
		snd.cmdValue = HCU_SYSMSG_BFHS_UICOMM_CMDVALUE_DYNAMIC_CALI_FULL;
	else
		snd.cmdValue = HCU_SYSMSG_BFHS_UICOMM_CMDVALUE_INVALID;

	HCU_MSG_SEND_GENERNAL_PROCESS(MSG_ID_UICOMM_L3BFHS_CTRL_CMD_REQ, TASK_ID_L3BFHS, TASK_ID_BFHSUICOMM);
	return SUCCESS;
}

OPSTAT fsm_bfhsuicomm_huicobus_uir_test_cmd_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HCU_MSG_RCV_CHECK_FOR_GEN_LOCAL(TASK_ID_BFHSUICOMM, msg_struct_huicobus_uir_test_cmd_req_t);
	msg_struct_sui_test_cmd_req_t snd;
	memset(&snd, 0, sizeof(msg_struct_sui_test_cmd_req_t));
	snd.length = sizeof(msg_struct_sui_test_cmd_req_t);
	snd.cmdid = HCU_SYSMSG_BFHS_UICOMM_CMDID_TEST;
	memcpy(&snd.boardBitmap, &rcv.boardBitmap, sizeof(rcv.boardBitmap));
	snd.cmdValue1 = rcv.cmdValue1;
	snd.cmdValue2 = rcv.cmdValue2;
	snd.cmdValue3 = rcv.cmdValue3;
	snd.cmdValue4 = rcv.cmdValue4;
	memcpy(&snd.cmdBuf, &rcv.cmdBuf, sizeof(rcv.cmdBuf));

	HCU_MSG_SEND_GENERNAL_PROCESS(MSG_ID_SUI_TEST_CMD_REQ, TASK_ID_CANALPHA, TASK_ID_BFHSUICOMM);
	return SUCCESS;
}

OPSTAT fsm_bfhsuicomm_huicobus_uir_one_key_zero_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HCU_MSG_RCV_CHECK_FOR_GEN_LOCAL(TASK_ID_BFHSUICOMM, msg_struct_huicobus_uir_one_key_clean_zero_req_t);
	msg_struct_uicomm_l3bfhs_ctrl_cmd_req_t snd;
	memset(&snd, 0, sizeof(msg_struct_uicomm_l3bfhs_ctrl_cmd_req_t));
	snd.length = sizeof(msg_struct_uicomm_l3bfhs_ctrl_cmd_req_t);
	snd.cmdid = HCU_SYSMSG_BFHS_UICOMM_CMDID_ONE_KEY_ZERO;

	HCU_MSG_SEND_GENERNAL_PROCESS(MSG_ID_UICOMM_L3BFHS_CTRL_CMD_REQ, TASK_ID_L3BFHS, TASK_ID_BFHSUICOMM);
	return SUCCESS;
}

OPSTAT func_bfhsuicomm_time_out_period_read_process(void)
{
//	UINT8 state = FsmGetState(TASK_ID_L3BFHS);

//	if ((state == FSM_STATE_L3BFHS_ACTIVED) || (state == FSM_STATE_L3BFHS_OPR_CFG)) {
//		//启动完成以后，等待一小会儿，然后将缺省的参数读入到系统内存，并发送CFG_REQ给L3BFHS
//		//如果缺省参数读取不成功，等待人工干预并读取，然后再发送给L3BFHS
//		if (func_bfhsuicomm_read_cfg_file_into_ctrl_table() == SUCCESS){
//			msg_struct_uicomm_l3bfhs_cfg_req_t snd;
//			memset(&snd, 0, sizeof(msg_struct_uicomm_l3bfhs_cfg_req_t));
//			snd.length = sizeof(msg_struct_uicomm_l3bfhs_cfg_req_t);
//			if (hcu_message_send(MSG_ID_UICOMM_L3BFHS_CFG_REQ, TASK_ID_L3BFHS, TASK_ID_BFHSUICOMM, &snd, snd.length) == FAILURE)
//				HCU_ERROR_PRINT_BFHSUICOMM("BFHSUICOMM: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_BFHSUICOMM].taskName, zHcuVmCtrTab.task[TASK_ID_L3BFHS].taskName);
//		}
//	}
//	else if (state == FSM_STATE_L3BFHS_OPR_GO) {
//		msg_struct_uicomm_l3bfhs_cmd_req_t snd_start_req;
//		memset(&snd_start_req, 0, sizeof(msg_struct_uicomm_l3bfhs_cmd_req_t))				HCU_ERROR_PRINT_BFHSUICOMM("BFHSUICOMM: UI input sensorid out of range, [sensorid=%d]! \n", sensorid);
//	return FAILURE;;
//		snd_start_req.length = sizeof(msg_struct_uicomm_l3bfhs_cmd_req_t);
//		snd_start_req.cmdid = HCU_SYSMSG_BFHS_UICOMM_CMDID_START;
//		if (hcu_message_send(MSG_ID_UICOMM_L3BFHS_CMD_REQ, TASK_ID_L3BFHS, TASK_ID_BFHSUICOMM, &snd_start_req, snd_start_req.length) == FAILURE)
//			HCU_ERROR_PRINT_BFHSUICOMM("BFHSUICOMM: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_BFHSUICOMM].taskName, zHcuVmCtrTab.task[TASK_ID_L3BFHS].taskName);
//		}
//	else{
//
//	}
	return SUCCESS;
}

//扫描文件是否有DEFAULT参数，并配置进入系统参数控制表
OPSTAT func_bfhsuicomm_read_cfg_db_into_ctrl_table (UINT16 config_index)
{
	//Update config id to gTaskL3bfhsContext
	gTaskL3bfhsContext.configId = config_index;

	//称重传感器配置参数
	gTaskL3bfhsContext.wgtSnrPar.maxAllowedWeight = 1000;
	gTaskL3bfhsContext.wgtSnrPar.minAllowedWeight  = 0;
	gTaskL3bfhsContext.wgtSnrPar.snrAlgoSelect = 1;
	gTaskL3bfhsContext.wgtSnrPar.snrAutoZeroAutotaringTimeMs = 100;
	gTaskL3bfhsContext.wgtSnrPar.snrAutoZeroCaptureRangeGrams =  500;
	gTaskL3bfhsContext.wgtSnrPar.snrAutoZeroSwitch = 1;
	gTaskL3bfhsContext.wgtSnrPar.snrFilterCutOffFreqHz  = 200;
	gTaskL3bfhsContext.wgtSnrPar.snrMeasurementRangeNo =  1;
	gTaskL3bfhsContext.wgtSnrPar.snrPreloadCompensationDataFormat = 2;
	gTaskL3bfhsContext.wgtSnrPar.snrPreloadCompensationValue = 625;
	gTaskL3bfhsContext.wgtSnrPar.snrReadStartMs = 50;
	gTaskL3bfhsContext.wgtSnrPar.snrReadStopMs = 50;
	gTaskL3bfhsContext.wgtSnrPar.snrRingBufTimeMs = 100;
	gTaskL3bfhsContext.wgtSnrPar.snrStandstillRangeGrams = 500;
	gTaskL3bfhsContext.wgtSnrPar.snrStandstillTime = 100;
	gTaskL3bfhsContext.wgtSnrPar.snrStandstillTimeoutMs = 10000;
	gTaskL3bfhsContext.wgtSnrPar.snrTimeGrid = 100;

	//马达配置参数
	gTaskL3bfhsContext.motoCtrlPar.MotorDirection =  1;
	gTaskL3bfhsContext.motoCtrlPar.MotorSpeed = 1000;

	//摇臂配置参数
	gTaskL3bfhsContext.armCtrlPar.ArmFailureDetectionTimeMs = 100;
	gTaskL3bfhsContext.armCtrlPar.ArmFailureDetectionVaration = 1;
	gTaskL3bfhsContext.armCtrlPar.ArmRollingIntervalMs  = 100;
	gTaskL3bfhsContext.armCtrlPar.ArmRollingStartMs = 1;
	gTaskL3bfhsContext.armCtrlPar.ArmRollingStopMs = 1;
	gTaskL3bfhsContext.armCtrlPar.ArmStartActionMs = 1;

	//CAL ZERO REQ
	gTaskL3bfhsContext.calZeroPar.WeightSensorFilterCutOffFreqHz = 200;
	gTaskL3bfhsContext.calZeroPar.WeightSensorAutoZeroCaptureRangeGrams = 500;
	gTaskL3bfhsContext.calZeroPar.WeightSensorStandstillRangeGrams = 500;
	gTaskL3bfhsContext.calZeroPar.WeightSensorAutoZeroAutotaringTimeMs = 100;
	gTaskL3bfhsContext.calZeroPar.WeightSensorPreloadComPensationValuePercent = 625;
	gTaskL3bfhsContext.calZeroPar.WeightSensorPreloadComPensationPlacesAfterDecimalPoint = 2;
	gTaskL3bfhsContext.calZeroPar.WeightSensorStandstillTimeoutMs = 10000;
	gTaskL3bfhsContext.calZeroPar.WeightSensorStandstillTime = 100;
	gTaskL3bfhsContext.calZeroPar.WeightSensorRingBufTimeMs = 100;
	gTaskL3bfhsContext.calZeroPar.WeightSensorMeasurementRangeNo = 1;
	gTaskL3bfhsContext.calZeroPar.WeightSensorAutoZero = 1;

	//CAL FULL REQ
	gTaskL3bfhsContext.calFullReqPar.WeightSensorAdjustingTolerancePercent = 3000;
	gTaskL3bfhsContext.calFullReqPar.WeightSensorAdjustingWeightGrams = 22000;

	//读取数据库，更新批次数据
	gTaskL3bfhsContext.callCellId = 1;

	return SUCCESS;
}




