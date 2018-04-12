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
	{MSG_ID_L3BFDF_UICOMM_CTRL_CMD_RESP,    FSM_STATE_BFDFUICOMM_ACTIVED,          		fsm_bfdfuicomm_l3bfdf_ctrl_cmd_resp},	//人工控制反馈
	{MSG_ID_SUI_TEST_CMD_RESP,      		FSM_STATE_BFDFUICOMM_ACTIVED,          		fsm_bfdfuicomm_sui_test_cmd_resp},      //测试命令反馈

	//UIR2HCU MSG RCV
	{MSG_ID_HUICOBUS_UIR_INIT_REQ,      		FSM_STATE_BFDFUICOMM_ACTIVED,          	fsm_bfdfuicomm_huicobus_uir_init_req},
	{MSG_ID_HUICOBUS_UIR_START_REQ, 			FSM_STATE_BFDFUICOMM_ACTIVED,          	fsm_bfdfuicomm_huicobus_uir_start_req},
	{MSG_ID_HUICOBUS_UIR_STOP_REQ, 				FSM_STATE_BFDFUICOMM_ACTIVED,          	fsm_bfdfuicomm_huicobus_uir_stop_req},
	{MSG_ID_HUICOBUS_UIR_RESUME_REQ, 			FSM_STATE_BFDFUICOMM_ACTIVED,          	fsm_bfdfuicomm_huicobus_uir_resume_req},
	{MSG_ID_HUICOBUS_UIR_SUSPEND_REQ, 			FSM_STATE_BFDFUICOMM_ACTIVED,          	fsm_bfdfuicomm_huicobus_uir_suspend_req},
	{MSG_ID_HUICOBUS_UIR_DYNAMIC_CALI_REQ,     	FSM_STATE_BFDFUICOMM_ACTIVED,          	fsm_bfdfuicomm_huicobus_uir_dynamic_cali_req},
	{MSG_ID_HUICOBUS_UIR_TEST_CMD_REQ,      	FSM_STATE_BFDFUICOMM_ACTIVED,          	fsm_bfdfuicomm_huicobus_uir_test_cmd_req},

    //结束点，固定定义，不要改动
    {MSG_ID_END,            	FSM_STATE_END,             				NULL},  //Ending
};

//Global variables
extern gTaskL3bfdfContext_t gTaskL3bfdfContext;

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

	//秤盘数据表单控制表初始化
	memset(&gTaskL3bfdfContext, 0, sizeof(gTaskL3bfdfContext_t));
	//初始化系统参数
	if (func_bfdfuicomm_read_system_config_into_ctrl_table () == FAILURE)
		HcuErrorPrint("BFDFUICOMM: read system config into ctrl_table failure!\n");

	//初始化sessionId
	gTaskL3bfdfContext.sessionId = dbi_HcuBfdf_CallCellMaxIdGet() + 1;

	//算法性能评估测试模式
#if (HCU_SYSCFG_L3BFDF_PMAS_SET == HCU_SYSCFG_L3BFDF_PMAS_ENABLE)
	//延迟并启动系统，进入测试模式
	hcu_sleep(2);
	func_bfdfuicomm_algo_pmas_load_config_into_ctrl_table();

	msg_struct_uicomm_l3bfdf_ctrl_cmd_req_t snd;
	memset(&snd, 0, sizeof(msg_struct_uicomm_l3bfdf_ctrl_cmd_req_t));
	snd.length = sizeof(msg_struct_uicomm_l3bfdf_ctrl_cmd_req_t);
	snd.cmdid = HCU_SYSMSG_BFDF_UICOMM_CMDID_CFG_START;
	snd.cmdValue = 0;
	HCU_MSG_SEND_GENERNAL_PROCESS(MSG_ID_UICOMM_L3BFDF_CTRL_CMD_REQ, TASK_ID_L3BFDF, TASK_ID_BFDFUICOMM);
#endif

	//检查参数设置情况
	func_bfdfuicomm_algo_parameter_set_check();

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

//控制命令的反馈
OPSTAT fsm_bfdfuicomm_l3bfdf_ctrl_cmd_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HCU_MSG_RCV_CHECK_FOR_GEN_LOCAL(TASK_ID_BFDFUICOMM, msg_struct_l3bfdf_uicomm_ctrl_cmd_resp_t);

	if(rcv.cmdid == HCU_SYSMSG_BFDF_UICOMM_CMDID_CFG_START){
		//通知界面
		StrHlcIe_cui_hcu2uir_status_report_t status;
		memset(&status, 0, sizeof(StrHlcIe_cui_hcu2uir_status_report_t));
		if(rcv.validFlag == TRUE)
			status.boardStatus = HUICOBUS_CMDID_CUI_HCU2UIR_GENERAL_CMDVAL_CFG_OK;
		else
			status.boardStatus = HUICOBUS_CMDID_CUI_HCU2UIR_GENERAL_CMDVAL_CFG_ERR;

		hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_status_report(rcv.sensorid, &status);
	}
	else if(rcv.cmdid == HCU_SYSMSG_BFDF_UICOMM_CMDID_STOP){
		StrHlcIe_cui_hcu2uir_status_report_t status;
		memset(&status, 0, sizeof(StrHlcIe_cui_hcu2uir_status_report_t));
		status.boardStatus = HUICOBUS_CMDID_CUI_HCU2UIR_GENERAL_CMDVAL_STOP;
		//通知界面
		hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_status_report(rcv.sensorid, &status);
	}
	else if(rcv.cmdid == HCU_SYSMSG_BFDF_UICOMM_CMDID_SUSPEND){
		StrHlcIe_cui_hcu2uir_status_report_t status;
		memset(&status, 0, sizeof(StrHlcIe_cui_hcu2uir_status_report_t));
		status.boardStatus = HUICOBUS_CMDID_CUI_HCU2UIR_GENERAL_CMDVAL_SUSPEND;
		//通知界面
		hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_status_report(rcv.sensorid, &status);
	}
	else if(rcv.cmdid == HCU_SYSMSG_BFDF_UICOMM_CMDID_RESUME){
		StrHlcIe_cui_hcu2uir_status_report_t status;
		memset(&status, 0, sizeof(StrHlcIe_cui_hcu2uir_status_report_t));
		status.boardStatus = HUICOBUS_CMDID_CUI_HCU2UIR_GENERAL_CMDVAL_RESUME;
		//通知界面
		hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_status_report(rcv.sensorid, &status);
	}
	else if(rcv.cmdid == HCU_SYSMSG_BFDF_UICOMM_CMDID_DYNAMIC_CALI){
		UINT32 cmdValue = 0;
		char debugInfo[HUICOBUS_CALI_RESP_DEBUG_INFO_LEN_MAX];
		StrHlcIe_cui_hcu2uir_dynamic_cali_resp_t status;
		StrHlcIe_cui_hcu2uir_dynamic_cali_finish_t finish;
		memset(&status, 0, sizeof(StrHlcIe_cui_hcu2uir_dynamic_cali_resp_t));
		if (rcv.dynCalResp.calibration_zero_or_full == 1){        /* 1 for ZERO, 2 for FULL */
			if (rcv.sensorid == 1)  //LINE0
				cmdValue = HCU_SYSMSG_BFDF_UICOMM_CMDVALUE_DYNAMIC_CALI_ZERO_LINE0;
			else if (rcv.sensorid == 9) //LINE1
				cmdValue = HCU_SYSMSG_BFDF_UICOMM_CMDVALUE_DYNAMIC_CALI_ZERO_LINE1;
			else{
				HCU_ERROR_PRINT_TASK(TASK_ID_BFDFUICOMM, "TASK_ID_BFDFUICOMM: Invalid sensorId of dynamic calibration zero response, sensorId = %d!\n", rcv.sensorid);
				return FAILURE;
			}
			status.weight = 0;
			if (rcv.dynCalResp.calibration_cur_iteration == gTaskL3bfdfContext.dynCalPar.zero_cal_iteration){
				finish.result = rcv.dynCalResp.calibration_result;
				//通知界面finish
				hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_dynamic_cali_finish(cmdValue, &finish);
			}
			if (rcv.validFlag == FALSE){
				finish.result = rcv.errCode;
				//通知界面finish
				hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_dynamic_cali_finish(cmdValue, &finish);
			}
		}
		else if (rcv.dynCalResp.calibration_zero_or_full == 2){
			if (rcv.sensorid == 1)  //LINE0
				cmdValue = HCU_SYSMSG_BFDF_UICOMM_CMDVALUE_DYNAMIC_CALI_FULL_LINE0;
			else if (rcv.sensorid == 9) //LINE1
				cmdValue = HCU_SYSMSG_BFDF_UICOMM_CMDVALUE_DYNAMIC_CALI_FULL_LINE1;
			else{
				HCU_ERROR_PRINT_TASK(TASK_ID_BFDFUICOMM, "TASK_ID_BFDFUICOMM: Invalid sensorId of dynamic calibration full response, sensorId = %d!\n", rcv.sensorid);
				return FAILURE;
			}
			status.weight = rcv.dynCalResp.full_weight;
			if (rcv.dynCalResp.calibration_cur_iteration == gTaskL3bfdfContext.dynCalPar.full_cal_iteration){
				finish.result = rcv.dynCalResp.calibration_result;
				//通知界面finish
				hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_dynamic_cali_finish(cmdValue, &finish);
			}
			if (rcv.validFlag == FALSE){
				finish.result = rcv.errCode;
				//通知界面finish
				hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_dynamic_cali_finish(cmdValue, &finish);
			}
		}
		else{
			HCU_ERROR_PRINT_TASK(TASK_ID_BFDFUICOMM, "TASK_ID_BFDFUICOMM: Invalid parameter of dynamic calibration response!\n");
			return FAILURE;
		}
		status.engModeSwitch = gTaskL3bfdfContext.engModeSwitch;
		status.iteration = rcv.dynCalResp.calibration_cur_iteration;
		status.validFlag = rcv.validFlag;
		status.errCode = rcv.errCode;
		sprintf(debugInfo, "cali_cur_itera=%d; cali_result=%d; nf_period_10ms=%d; nf_period_10ms_max=%d; wgt_offset_wrt_infra=%d; \
nf_sd_mean=%d; nf_sd_max=%d; nf_sd_min=%d; nf_sd_sd=%d; nf_mean_mean=%d; nf_mean_max=%d; nf_mean_min=%d; nf_mean_sd=%d; offset_peak_wrt_infra[0]=%d; \
offset_peak_wrt_infra[1]=%d; offset_peak_wrt_infra[2]=%d; offset_peak_wrt_infra[3]=%d; full_coeff_average=%d; estimate_err_itera[0]=%d; \
estimate_err_itera[1]=%d; estimate_err_itera[2]=%d; estimate_err_itera[3]=%d; estimate_err_max_possible=%d; estimate_err_average=%d",\
				rcv.dynCalResp.calibration_cur_iteration, rcv.dynCalResp.calibration_result, rcv.dynCalResp.noise_floor_period_10ms, rcv.dynCalResp.noise_floor_period_10ms_max, \
				rcv.dynCalResp.weight_report_offset_wrt_infra, rcv.dynCalResp.noise_floor_sd_mean, rcv.dynCalResp.noise_floor_sd_max, rcv.dynCalResp.noise_floor_sd_min, rcv.dynCalResp.noise_floor_sd_sd, \
				rcv.dynCalResp.noise_floor_mean_mean, rcv.dynCalResp.noise_floor_mean_max, rcv.dynCalResp.noise_floor_mean_min, rcv.dynCalResp.noise_floor_mean_sd, \
				rcv.dynCalResp.full_offset_peak_wrt_infra[0], rcv.dynCalResp.full_offset_peak_wrt_infra[1], rcv.dynCalResp.full_offset_peak_wrt_infra[2], rcv.dynCalResp.full_offset_peak_wrt_infra[3], \
				rcv.dynCalResp.full_coefficiency_average, rcv.dynCalResp.estimated_error_iteration[0], rcv.dynCalResp.estimated_error_iteration[1], rcv.dynCalResp.estimated_error_iteration[2], \
				rcv.dynCalResp.estimated_error_iteration[3], rcv.dynCalResp.estimated_error_max_possible, rcv.dynCalResp.estimated_error_average);
		strncpy(status.debugInfo, debugInfo, sizeof(debugInfo)<HUICOBUS_CALI_RESP_DEBUG_INFO_LEN_MAX?sizeof(debugInfo):HUICOBUS_CALI_RESP_DEBUG_INFO_LEN_MAX);

		//通知界面
		hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_dynamic_cali_resp(cmdValue, &status);
	}
	else if (rcv.cmdid == HCU_SYSMSG_BFDF_UICOMM_CMDID_ONE_KEY_ZERO){
		//TBD
		printf ("fsm_bfdfuicomm_l3bfdf_ctrl_cmd_resp: rcv.cmdid = %d\n", rcv.cmdid);
	}
	else{
		HCU_ERROR_PRINT_TASK(TASK_ID_BFDFUICOMM, "TASK_ID_BFDFUICOMM: Invalid command response!\n");
		return FAILURE;
	}

	//返回
	return SUCCESS;
}

//一般性测试命令的反馈
OPSTAT fsm_bfdfuicomm_sui_test_cmd_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
//	int ret=0;
//	UINT32  adcvalue = 0;
//	UINT16 	errorcode;
//	UINT8  	sensorid = 0;
//	UINT8	validFlag = 0;

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

OPSTAT fsm_bfdfuicomm_huicobus_uir_start_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	UINT16 configId = 0;
	HCU_MSG_RCV_CHECK_FOR_GEN_LOCAL(TASK_ID_BFDFUICOMM, msg_struct_huicobus_uir_start_req_t);

	msg_struct_uicomm_l3bfdf_ctrl_cmd_req_t snd;
	memset(&snd, 0, sizeof(msg_struct_uicomm_l3bfdf_ctrl_cmd_req_t));
	snd.length = sizeof(msg_struct_uicomm_l3bfdf_ctrl_cmd_req_t);
	snd.cmdid = HCU_SYSMSG_BFDF_UICOMM_CMDID_CFG_START;
	snd.cmdValue = rcv.cmdValue;
	configId = rcv.cmdValue;

	if(func_bfdfuicomm_read_product_config_into_ctrl_table(configId) == FAILURE)
		HCU_ERROR_PRINT_TASK(TASK_ID_BFDFUICOMM, "TASK_ID_BFDFUICOMM: Load global context table failure!\n");
	//检查参数设置情况
	func_bfdfuicomm_algo_parameter_set_check();

	HCU_MSG_SEND_GENERNAL_PROCESS(MSG_ID_UICOMM_L3BFDF_CTRL_CMD_REQ, TASK_ID_L3BFDF, TASK_ID_BFDFUICOMM);
	return SUCCESS;
}

OPSTAT fsm_bfdfuicomm_huicobus_uir_stop_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HCU_MSG_RCV_CHECK_FOR_GEN_LOCAL(TASK_ID_BFDFUICOMM, msg_struct_huicobus_uir_stop_req_t);

	msg_struct_uicomm_l3bfdf_ctrl_cmd_req_t snd;
	memset(&snd, 0, sizeof(msg_struct_uicomm_l3bfdf_ctrl_cmd_req_t));
	snd.length = sizeof(msg_struct_uicomm_l3bfdf_ctrl_cmd_req_t);
	snd.cmdid = HCU_SYSMSG_BFDF_UICOMM_CMDID_STOP;
	snd.cmdValue = rcv.cmdValue;

	HCU_MSG_SEND_GENERNAL_PROCESS(MSG_ID_UICOMM_L3BFDF_CTRL_CMD_REQ, TASK_ID_L3BFDF, TASK_ID_BFDFUICOMM);
	return SUCCESS;
}


OPSTAT fsm_bfdfuicomm_huicobus_uir_resume_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HCU_MSG_RCV_CHECK_FOR_GEN_LOCAL(TASK_ID_BFDFUICOMM, msg_struct_huicobus_uir_resume_req_t);

	msg_struct_uicomm_l3bfdf_ctrl_cmd_req_t snd;
	memset(&snd, 0, sizeof(msg_struct_uicomm_l3bfdf_ctrl_cmd_req_t));
	snd.length = sizeof(msg_struct_uicomm_l3bfdf_ctrl_cmd_req_t);
	snd.cmdid = HCU_SYSMSG_BFDF_UICOMM_CMDID_RESUME;
	snd.cmdValue = rcv.cmdValue;

	HCU_MSG_SEND_GENERNAL_PROCESS(MSG_ID_UICOMM_L3BFDF_CTRL_CMD_REQ, TASK_ID_L3BFDF, TASK_ID_BFDFUICOMM);
	return SUCCESS;
}

OPSTAT fsm_bfdfuicomm_huicobus_uir_suspend_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HCU_MSG_RCV_CHECK_FOR_GEN_LOCAL(TASK_ID_BFDFUICOMM, msg_struct_huicobus_uir_suspend_req_t);

	msg_struct_uicomm_l3bfdf_ctrl_cmd_req_t snd;
	memset(&snd, 0, sizeof(msg_struct_uicomm_l3bfdf_ctrl_cmd_req_t));
	snd.length = sizeof(msg_struct_uicomm_l3bfdf_ctrl_cmd_req_t);
	snd.cmdid = HCU_SYSMSG_BFDF_UICOMM_CMDID_SUSPEND;
	snd.cmdValue = rcv.cmdValue;

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
	snd.cmdValue = rcv.cmdValue;

	//重新初始化全局系统参数
	if (func_bfdfuicomm_read_system_config_into_ctrl_table () == FAILURE)
		HcuErrorPrint("BFDFUICOMM: read system config into ctrl_table failure!\n");

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

UINT32 func_bfdfuicomm_time_out_period_read_process(void)
{
//	UINT8 state = FsmGetState(TASK_ID_L3BFDF);

//	if ((state == FSM_STATE_L3BFDF_ACTIVED) || (state == FSM_STATE_L3BFDF_OPR_CFG)) {
//		//启动完成以后，等待一小会儿，然后将缺省的参数读入到系统内存，并发送CFG_REQ给L3BFDF
//		//如果缺省参数读取不成功，等待人工干预并读取，然后再发送给L3BFDF
//		if (func_bfdfuicomm_read_product_config_into_ctrl_table() == SUCCESS){
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
//	return FAILURE;
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

bool func_bfdfuicomm_hopper_bitmap_validate(UINT64 hopperBitmap, UINT8 hopperNum, UINT8 hopperArr[HCU_SYSCFG_BFDF_HOPPER_NBR_MAX])
{
	int i, j=0;
	memset(hopperArr, 0, HCU_SYSCFG_BFDF_HOPPER_NBR_MAX);
	for (i=0; i<HCU_SYSMSG_SUI_SENSOR_NBR; i++){ //bitmap: 0~31
		if (((hopperBitmap>>i) & 1) == 1){
			hopperArr[j+1] = i+1;
			j++;
		}
	}
	if (j != hopperNum){
		HCU_ERROR_PRINT_BFDFUICOMM("BFDFUICOMM: hcubfdfgrouppara DB data error, hopperBitmap mismatch [hopperBitmap = %d]/[hopperNum = %d]!\n", hopperBitmap, hopperNum);
		return FALSE;
	}

	return TRUE;
}

//初始化所有料斗状态
bool func_bfdfuicomm_hopper_state_set_init(UINT8 streamId)
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

UINT32 func_bfdfuicomm_read_system_config_into_ctrl_table(void)
{
	UINT8 line, hopper;
	DbiL3BfdfSystemPara_t sysConfigData;
	DbiL3BfdfCalibrationPara_t calConfigData;

	/*** Initialize SYSTEM configuration, these parameters are common for all products ***/

	if (dbi_HcuBfdf_sysConfigData_read(&sysConfigData, &calConfigData) == FAILURE){
		HCU_ERROR_PRINT_BFDFUICOMM("BFDFUICOMM: Get DB System and Calibration configuration data failed \n");
		return FALSE;
	}
	gTaskL3bfdfContext.engModeSwitch = sysConfigData.engModeSwitch;

	//配置系统的DIMENSIONING
	gTaskL3bfdfContext.nbrStreamLine = sysConfigData.lineNum;
	gTaskL3bfdfContext.nbrIoBoardPerLine = sysConfigData.boardNumPerLine;

	//如果系统配置了该全局算法参数，则可以调整该参数，可以更大程度的由人工来调优
	gTaskL3bfdfContext.combAlgoSpaceCtrlRatio = HCU_SYSCFG_ALGO_SPACE_CTRL_RATIO_DEFAULT;

	//Hopper初始化
	for (line = 0; line< gTaskL3bfdfContext.nbrStreamLine; line++){
		func_bfdfuicomm_hopper_state_set_init(line);
	}
	for (line = gTaskL3bfdfContext.nbrStreamLine; line< HCU_SYSCFG_BFDF_EQU_FLOW_NBR_MAX; line++){
		for (hopper=0; hopper<HCU_SYSCFG_BFDF_HOPPER_NBR_MAX; hopper++){
			gTaskL3bfdfContext.hopper[line][hopper].hopperStatus = HCU_L3BFDF_HOPPER_STATUS_OFFLINE;
		}
	}

	//查询系统配置参数
	memcpy(&gTaskL3bfdfContext.motMainPar, &sysConfigData.motMainPar, sizeof(gTaskL3bfdfContextMotorControlParamaters_t));
	memcpy(&gTaskL3bfdfContext.motSecondPar, &sysConfigData.motSecondPar, sizeof(gTaskL3bfdfContextMotorControlParamaters_t));
	memcpy(&gTaskL3bfdfContext.actionCtrlPar, &sysConfigData.armCtrlPar, sizeof(gTaskL3bfdfContextActionControlParamaters_t));
	memcpy(&gTaskL3bfdfContext.dynCalPar, &calConfigData, sizeof(DbiL3BfdfCalibrationPara_t));
//	gTaskL3bfdfContext.nbrStreamLine = 1;
//	gTaskL3bfdfContext.nbrIoBoardPerLine = 1;
//	gTaskL3bfdfContext.wgtSnrPar.WeightSensorLoadDetectionTimeMs =  1;
//	gTaskL3bfdfContext.wgtSnrPar.WeightSensorLoadThread = 1;
//	gTaskL3bfdfContext.wgtSnrPar.WeightSensorEmptyDetectionTimeMs = 1;
//	gTaskL3bfdfContext.wgtSnrPar.WeightSensorEmptyThread = 1;
//	gTaskL3bfdfContext.wgtSnrPar.StardardReadyTimeMs = 1;
//
//	gTaskL3bfdfContext.wgtSnrPar.MaxAllowedWeight = 1;
//	gTaskL3bfdfContext.wgtSnrPar.WeightSensorStaticZeroValue = 1;
//	gTaskL3bfdfContext.wgtSnrPar.WeightSensorTailorValue = 1;
//	gTaskL3bfdfContext.wgtSnrPar.WeightSensorDynamicZeroThreadValue = 1;
//	gTaskL3bfdfContext.wgtSnrPar.WeightSensorDynamicZeroHysteresisMs = 1;
//
//	gTaskL3bfdfContext.wgtSnrPar.WeightSensorPickupThread = 300;
//	gTaskL3bfdfContext.wgtSnrPar.WeightSensorPickupDetectionTimeMs = 500;

	return SUCCESS;
}

//配置系统参数控制表
UINT32 func_bfdfuicomm_read_product_config_into_ctrl_table(UINT16 configId)
{
	UINT8 line, index, hopper;
	UINT8 groupPerLine = 0, groupTotal = 0;
	DbiL3BfdfProductPara_t productConfigData;
	DbiL3BfdfGroupPara_t groupConfigData[HCU_SYSCFG_BFDF_HOPPER_NBR_MAX*2];

	//重新初始化全局系统参数
	if (func_bfdfuicomm_read_system_config_into_ctrl_table () == FAILURE)
		HCU_ERROR_PRINT_BFDFUICOMM("BFDFUICOMM: read system config into ctrl_table failure!\n");

	/*** Initialize PRODUCT configuration, these parameters are different for each product ***/
	//Read productPara table
	if (dbi_HcuBfdf_productConfigData_read(configId, &productConfigData) == FAILURE)
		HCU_ERROR_PRINT_BFDFUICOMM("BFDFUICOMM: Get DB PRODUCT configuration data failed, configId = %d \n", configId);

	gTaskL3bfdfContext.configId = configId;
	groupPerLine = productConfigData.groupPerLine;
	groupTotal = groupPerLine * gTaskL3bfdfContext.nbrStreamLine;
	for(line = 0; line < gTaskL3bfdfContext.nbrStreamLine; line++)
		gTaskL3bfdfContext.totalGroupNbr[line] = groupPerLine;

	strncpy(gTaskL3bfdfContext.configName, productConfigData.configName, HCU_L3BFDF_CONTEXT_CONFIG_NAME_LEN_MAX-1);
	strncpy(gTaskL3bfdfContext.operatorName, productConfigData.operatorName, HCU_L3BFDF_CONTEXT_OPERATOR_NAME_LEN_MAX-1);

	//Read groupPara table
	if (dbi_HcuBfdf_groupConfigData_read(configId, groupTotal, groupConfigData) == FAILURE)
		HCU_ERROR_PRINT_BFDFUICOMM("BFDFUICOMM: Get DB GROUP configuration data failed, groupTotal=%d \n", groupTotal);

	UINT16 hopperId = 0;
	UINT16 groupId = 0;
	UINT16 lineId = 0;
	UINT8 groupHooperNum;
	UINT64 groupHopperBitmap;
	UINT8 hopperArr[HCU_SYSCFG_BFDF_HOPPER_NBR_MAX];
	for(index = 1; index <= groupTotal; index++){
		groupId = groupConfigData[index].groupId;
		lineId = groupConfigData[index].lineId;
		if (configId != groupConfigData[index].configId)
			HCU_ERROR_PRINT_BFDFUICOMM("BFDFUICOMM: hcubfdfgrouppara DB data error, configId[=%d] groupId[=%d] mismatch!\n", configId, groupId);

		groupHooperNum = groupConfigData[index].hopperNum;
		groupHopperBitmap = groupConfigData[index].hopperBitmap;
		gTaskL3bfdfContext.group[lineId][groupId].targetWeight = groupConfigData[index].targetWeight;
		gTaskL3bfdfContext.group[lineId][groupId].targetUpLimit = groupConfigData[index].targetUpLimit;
		gTaskL3bfdfContext.group[lineId][groupId].bufWgtTarget = groupConfigData[index].bufWgtTarget;
		gTaskL3bfdfContext.group[lineId][groupId].rangeLow = groupConfigData[index].rangeLow;
		gTaskL3bfdfContext.group[lineId][groupId].rangeHigh = groupConfigData[index].rangeHigh;
		gTaskL3bfdfContext.group[lineId][groupId].rangeAvg = (gTaskL3bfdfContext.group[lineId][groupId].rangeLow + gTaskL3bfdfContext.group[lineId][groupId].rangeHigh)/2;
		gTaskL3bfdfContext.group[lineId][groupId].rangeSigma = (gTaskL3bfdfContext.group[lineId][groupId].rangeHigh - gTaskL3bfdfContext.group[lineId][groupId].rangeLow)/2;
		gTaskL3bfdfContext.group[lineId][groupId].groupId = groupId;
		gTaskL3bfdfContext.group[lineId][groupId].groupStatus = HCU_L3BFDF_GROUP_STATUS_ACTIVE;
		gTaskL3bfdfContext.group[lineId][groupId].totalHopperNbr = groupHooperNum;

		if (func_bfdfuicomm_hopper_bitmap_validate(groupHopperBitmap, groupHooperNum, hopperArr) == FALSE)
			HCU_ERROR_PRINT_BFDFUICOMM("BFDFUICOMM: hcubfdfgrouppara DB data error, configId[=%d] hopperBitmap[=%d] mismatch!\n", configId, groupHopperBitmap);

		for(hopper = 1; hopper <= groupHooperNum; hopper++){
			hopperId = hopperArr[hopper];
			gTaskL3bfdfContext.hopper[lineId][hopperId].groupId = groupId;
			gTaskL3bfdfContext.hopper[lineId][hopperId].hopperId = hopperId;
			gTaskL3bfdfContext.hopper[lineId][hopperId].hopperStatus = HCU_L3BFDF_HOPPER_STATUS_INIT_ALLOC;

			if(hopper == 1){
				gTaskL3bfdfContext.group[lineId][groupId].firstHopperId = hopperId;
				gTaskL3bfdfContext.group[lineId][groupId].fillHopperId = hopperId;
				if (groupHooperNum <= 1){
					gTaskL3bfdfContext.hopper[lineId][hopperId].nextHopperId = hopperId;
					gTaskL3bfdfContext.hopper[lineId][hopperId].preHopperId = hopperId;
				}
				else{
					gTaskL3bfdfContext.hopper[lineId][hopperId].nextHopperId = hopperArr[hopper+1];
					gTaskL3bfdfContext.hopper[lineId][hopperId].preHopperId = hopperArr[groupHooperNum];
				}
			}
			else if(hopper >1 && hopper < groupHooperNum){
				gTaskL3bfdfContext.hopper[lineId][hopperId].nextHopperId = hopperArr[hopper+1];
				gTaskL3bfdfContext.hopper[lineId][hopperId].preHopperId = hopperArr[hopper-1];
			}
			else if(hopper == groupHooperNum){
				gTaskL3bfdfContext.hopper[lineId][hopperId].nextHopperId = hopperArr[1];
				gTaskL3bfdfContext.hopper[lineId][hopperId].preHopperId = hopperArr[hopper-1];
			}
		}
	}

	//printf("BFDFUI: MotorSpeed = %d, WeightSensorTailorValue = %d\n", gTaskL3bfdfContext.motMainPar.MotorSpeed, gTaskL3bfdfContext.dynCalPar.WeightSensorTailorValue);

//	//Hopper初始化
//	for (line = 0; line< gTaskL3bfdfContext.nbrStreamLine; line++){
//		func_bfdfuicomm_hopper_state_set_init(line);
//	}
//	int nbrGroup = 0;
//	//第0个流水线，分配组别
//	nbrGroup = rand()%3+1;
//	//For test purpose, fix to be 1 group
//	nbrGroup = 1;
//	func_l3bfdf_group_allocation(0, nbrGroup);
//	func_l3bfdf_hopper_add_by_grp_in_average_distribution(0, nbrGroup);
//	//设置小组重量范围：数据均为NF2进行设置
//	func_l3bfdf_group_auto_alloc_init_range_in_average(0, nbrGroup, 10000, 100000);
//	//设置重量目标
//	func_l3bfdf_group_auto_alloc_init_target_with_uplimit(0, 300000, 0.1);
//
//	//第1个流水线，分配组别
//	if (gTaskL3bfdfContext.nbrIoBoardPerLine >= 2){
//		nbrGroup = rand()%3+1;
//		func_l3bfdf_group_allocation(1, nbrGroup);
//		func_l3bfdf_hopper_add_by_grp_in_average_distribution(1, nbrGroup);
//		func_l3bfdf_group_auto_alloc_init_range_in_average(1, nbrGroup, 20000, 200000);
//		func_l3bfdf_group_auto_alloc_init_target_with_uplimit(1, 2000000, 0.2);
//	}

	//打印
	for (line = 0; line< gTaskL3bfdfContext.nbrStreamLine; line++){
		func_l3bfdf_print_all_hopper_status_by_id(line);
	}

	//手工浏览一遍双链表
	for (line = 0; line< gTaskL3bfdfContext.nbrStreamLine; line++){
		func_l3bfdf_print_all_hopper_status_by_chain(line);
	}

	//自动Audit过程
	int res = func_l3bfdf_hopper_dual_chain_audit();
	if (res < 0) HCU_ERROR_PRINT_BFDFUICOMM("BSDFUICOMM: Audit error, errCode = %d\n", res);

	//批次数据更新
	//读取数据库，更新批次数据
	gTaskL3bfdfContext.sessionId = 1;

	return SUCCESS;
}


//算法性能评估所使用的测试模式，装载参数
UINT32 func_bfdfuicomm_algo_pmas_load_config_into_ctrl_table(void)
{
	UINT8 line, hopper;
	int i = 0, j = 0;
	int nbrGroup = 0;

	//每块板子设置为1个IO板
	gTaskL3bfdfContext.nbrStreamLine = 1;
	gTaskL3bfdfContext.nbrIoBoardPerLine = 1;

	//Hopper初始化
	for (line = 0; line< gTaskL3bfdfContext.nbrStreamLine; line++){
		func_bfdfuicomm_hopper_state_set_init(line);
	}
	for (line = gTaskL3bfdfContext.nbrStreamLine; line< HCU_SYSCFG_BFDF_EQU_FLOW_NBR_MAX; line++){
		for (hopper=0; hopper<HCU_SYSCFG_BFDF_HOPPER_NBR_MAX; hopper++){
			gTaskL3bfdfContext.hopper[line][hopper].hopperStatus = HCU_L3BFDF_HOPPER_STATUS_OFFLINE;
		}
	}

	//第0#流水线，分配组别
	nbrGroup = rand()%3+1;
	//For test purpose, fix to be 1 group
	nbrGroup = 1;
	func_l3bfdf_group_allocation(0, nbrGroup);
	func_l3bfdf_hopper_add_by_grp_in_average_distribution(0, nbrGroup);
	//设置小组重量范围：数据均为NF2进行设置
	func_l3bfdf_group_auto_alloc_init_range_in_average(0, nbrGroup, 10000, 100000);
	//设置重量目标
	func_l3bfdf_group_auto_alloc_init_target_with_uplimit(0, 1000000, 0.01);

	//第1#流水线，分配组别
	if (gTaskL3bfdfContext.nbrIoBoardPerLine >= 2){
		nbrGroup = rand()%10+1;
		nbrGroup = 1;
		func_l3bfdf_group_allocation(1, nbrGroup);
		func_l3bfdf_hopper_add_by_grp_in_average_distribution(1, nbrGroup);
		func_l3bfdf_group_auto_alloc_init_range_in_average(1, nbrGroup, 20000, 200000);
		func_l3bfdf_group_auto_alloc_init_target_with_uplimit(1, 1000000, 0.2);
	}

	//第2#流水线，分配组别
	if (gTaskL3bfdfContext.nbrIoBoardPerLine >= 3)
	{
		nbrGroup = rand()%10+1;
		func_l3bfdf_group_allocation(2, nbrGroup);
		func_l3bfdf_hopper_add_by_grp_in_average_distribution(2, nbrGroup);
		func_l3bfdf_group_auto_alloc_init_range_in_average(2, nbrGroup, 20000, 200000);
		func_l3bfdf_group_auto_alloc_init_target_with_uplimit(2, 1000000, 0.2);
	}

	//第3#流水线，分配组别
	if (gTaskL3bfdfContext.nbrIoBoardPerLine >= 4)
	{
		nbrGroup = rand()%10+1;
		func_l3bfdf_group_allocation(3, nbrGroup);
		func_l3bfdf_hopper_add_by_grp_in_average_distribution(3, nbrGroup);
		func_l3bfdf_group_auto_alloc_init_range_in_average(3, nbrGroup, 20000, 200000);
		func_l3bfdf_group_auto_alloc_init_target_with_uplimit(3, 1000000, 0.2);
	}

	//打印
	for (line = 0; line< gTaskL3bfdfContext.nbrStreamLine; line++){
		func_l3bfdf_print_all_hopper_status_by_id(line);
	}

	//手工浏览一遍双链表
	for (line = 0; line< gTaskL3bfdfContext.nbrStreamLine; line++){
		func_l3bfdf_print_all_hopper_status_by_chain(line);
	}

	//自动Audit过程
	int res = func_l3bfdf_hopper_dual_chain_audit();
	if (res < 0) HCU_ERROR_PRINT_BFDFUICOMM("BSDFUICOMM: Audit error, errCode = %d\n", res);

	//批次数据更新
	//读取数据库，更新批次数据
	gTaskL3bfdfContext.sessionId = 1;

	//设置各个板子启动的情况，模拟STARTUP收到
	for (i = 0; i < HCU_L3BFDF_MAX_STREAM_LINE_ACTUAL; i++){
		for (j = 1; j < HCU_L3BFDF_MAX_IO_BOARD_NBR_ACTUAL; j++){
			gTaskL3bfdfContext.nodeDyn[i][j].nodeStatus = HCU_L3BFDF_NODE_BOARD_STATUS_STARTUP;
		}
	}

	return TRUE;
}

//配置参数检查
void func_bfdfuicomm_algo_parameter_set_check(void)
{
	int lineId = 0, gId = 0;
	int maxGroup;
	double gRange = 0, tRange = 0, ratio;

	for (lineId = 0; lineId < HCU_L3BFDF_MAX_STREAM_LINE_ACTUAL; lineId++)
	{
		maxGroup = gTaskL3bfdfContext.totalGroupNbr[lineId];
		for (gId = 1; gId <= maxGroup; gId++)
		{
			gRange = (double)(gTaskL3bfdfContext.group[lineId][gId].rangeHigh - gTaskL3bfdfContext.group[lineId][gId].rangeLow);
			tRange = (double)gTaskL3bfdfContext.group[lineId][gId].targetUpLimit;
			gRange = ((gRange==0)?0.01:gRange);
			ratio = tRange / gRange;
			if (ratio < HCU_SYSCFG_UPLIMIT_VS_GRP_DISTR_RATIO_MAX){
				HcuErrorPrint("BFDFUICOMM: Parameter set Grp range or Uplimit range too small, potential risk to high rejection rate. Ratio/Line/Gid=%6.2f%/%d/%d\n", ratio*100, lineId, gId);
			}
		}
	}
}



