/*
 * bfscuicomm.c
 *
 *  Created on: 2016年5月14日
 *      Author: hitpony
 */

#include "bfscuicomm.h"
#include  "jsoninotify.h"

#include "../l0service/timer.h"
#include "../l0service/trace.h"
#include "../l1com/l1comdef.h"
#include <json-c/json.h>
#include <json-c/json_object.h>

/*
** FSM of the BFSCUICOMM
*/
HcuFsmStateItem_t HcuFsmBfscuicomm[] =
{
    //MessageId                 //State                   		 		//Function
	//启始点，固定定义，不要改动, 使用ENTRY/END，意味者MSGID肯定不可能在某个高位区段中；考虑到所有任务共享MsgId，即使分段，也无法实现
	//完全是为了给任务一个初始化的机会，按照状态转移机制，该函数不具备启动的机会，因为任务初始化后自动到FSM_STATE_IDLE
	//如果没有必要进行初始化，可以设置为NULL
	{MSG_ID_ENTRY,       					FSM_STATE_ENTRY,            				fsm_bfscuicomm_task_entry}, //Starting

	//System level initialization, only controlled by HCU-MAIN
    {MSG_ID_COM_INIT,       				FSM_STATE_IDLE,            					fsm_bfscuicomm_init},
    {MSG_ID_COM_INIT_FEEDBACK,				FSM_STATE_IDLE,            					fsm_com_do_nothing},

	//Task level initialization
    {MSG_ID_COM_INIT,       				FSM_STATE_BFSCUICOMM_INITED,            	fsm_bfscuicomm_init},
    {MSG_ID_COM_INIT_FEEDBACK,				FSM_STATE_BFSCUICOMM_INITED,            	fsm_com_do_nothing},

	//ANY state entry
    {MSG_ID_COM_INIT_FEEDBACK,				FSM_STATE_COMMON,          					fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT,       			FSM_STATE_COMMON,          					fsm_com_heart_beat_rcv},
	{MSG_ID_COM_STOP,       				FSM_STATE_COMMON,          					fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT_FB,       		FSM_STATE_COMMON,          					fsm_com_do_nothing},
    {MSG_ID_COM_RESTART,					FSM_STATE_COMMON,            				fsm_bfscuicomm_restart},
	{MSG_ID_COM_TIME_OUT,       			FSM_STATE_COMMON,          					fsm_bfscuicomm_timeout},

    //Normal working status
    {MSG_ID_COM_INIT_FEEDBACK,				FSM_STATE_BFSCUICOMM_ACTIVED,            	fsm_com_do_nothing},
    {MSG_ID_INOTIFY_UICOMM_FILE_CHANGE_IND,	FSM_STATE_BFSCUICOMM_ACTIVED,            	fsm_bfscuicomm_scan_jason_callback},
	{MSG_ID_L3BFSC_UICOMM_CFG_RESP,      	FSM_STATE_BFSCUICOMM_ACTIVED,          		fsm_bfscuicomm_l3bfsc_cfg_resp},	//配置反馈
	{MSG_ID_L3BFSC_UICOMM_CMD_RESP,      	FSM_STATE_BFSCUICOMM_ACTIVED,          		fsm_bfscuicomm_l3bfsc_cmd_resp},	//人工控制反馈
	{MSG_ID_CAN_UICOMM_TEST_CMD_RESP,      	FSM_STATE_BFSCUICOMM_ACTIVED,          		fsm_bfscuicomm_can_test_cmd_resp},  //测试命令反馈

    //结束点，固定定义，不要改动
    {MSG_ID_END,            	FSM_STATE_END,             				NULL},  //Ending
};

//Global variables
#if (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFSC_CBU_ID)
	#include "../l3appctrl/l3bfsc.h"
	extern gTaskL3bfscContext_t gTaskL3bfscContext;
#endif

gTaskL3bfscuicommContext_t gTaskL3bfscuicommContext;

//Main Entry
//Input parameter would be useless, but just for similar structure purpose
OPSTAT fsm_bfscuicomm_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//除了对全局变量进行操作之外，尽量不要做其它操作，因为该函数将被主任务/线程调用，不是本任务/线程调用
	//该API就是给本任务一个提早介入的入口，可以帮着做些测试性操作
	if (FsmSetState(TASK_ID_BFSCUICOMM, FSM_STATE_IDLE) == FAILURE){
		HcuErrorPrint("BFSCUICOMM: Error Set FSM State at fsm_bfscuicomm_task_entry\n");}
	return SUCCESS;
}

OPSTAT fsm_bfscuicomm_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;

	if ((src_id > TASK_ID_MIN) &&(src_id < TASK_ID_MAX)){
		//Send back MSG_ID_COM_INIT_FEEDBACK to SVRCON
		msg_struct_com_init_feedback_t snd0;
		memset(&snd0, 0, sizeof(msg_struct_com_init_feedback_t));
		snd0.length = sizeof(msg_struct_com_init_feedback_t);

		//to avoid all task send out the init fb msg at the same time which lead to msgque get stuck
		hcu_usleep(dest_id*HCU_SYSCFG_DURATION_OF_INIT_FB_WAIT_MAX);

		ret = hcu_message_send(MSG_ID_COM_INIT_FEEDBACK, src_id, TASK_ID_BFSCUICOMM, &snd0, snd0.length);
		if (ret == FAILURE){
			HcuErrorPrint("BFSCUICOMM: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_BFSCUICOMM].taskName, zHcuVmCtrTab.task[src_id].taskName);
			return FAILURE;
		}
	}

	//收到初始化消息后，进入初始化状态
	if (FsmSetState(TASK_ID_BFSCUICOMM, FSM_STATE_BFSCUICOMM_INITED) == FAILURE){
		HcuErrorPrint("BFSCUICOMM: Error Set FSM State!\n");
		return FAILURE;
	}

	//Global Variables
	zHcuSysStaPm.taskRunErrCnt[TASK_ID_BFSCUICOMM] = 0;

	memset(&gTaskL3bfscuicommContext, 0, sizeof(gTaskL3bfscuicommContext_t));

	//Create necessary exchange files

	//在系统/temp目录下创建空的command.json文件，用于通知界面HCU及下位机已经准备好了
	FILE *fileStream;
	char cmdJson[] = "{\"start_cmd\":{\"flag\":0,\"value\":0,\"confindex\":0},\"calibration_cmd\":{\"flag\":0,\"value\":0,\"sensorid\":0},\"resume_cmd\":{\"flag\":0,\"value\":0},\"test_cmd\":{\"flag\":0,\"value\":0,\"sensorid\":0,\"testcmd\":0,\"testpara\":0}}";
	char str[100];
	memset(str, 0, sizeof(str));
	sprintf(str, "rm %s", zHcuCmdflagJsonFile);
	if (access(&str[3], F_OK) == 0) system(str);
	if ((fileStream = fopen( zHcuCmdflagJsonFile, "w+" )) != NULL ) {
		fputs(cmdJson, fileStream);
		fclose(fileStream);
	 }
	memset(str, 0, sizeof(str));
	sprintf(str, "chmod -R 777 %s", zHcuCmdflagJsonFile);
	system(str);

	//修改生成command.json link
	sprintf(str, "rm %s", zHcuCmdLinkJsonFile);
	if (access(&str[3], F_OK) == 0) system(str);
	sprintf(str, "ln -s %s %s", zHcuCmdflagJsonFile, zHcuCmdLinkJsonFile);
	system(str);
	sprintf(str, "chmod -R 777 /var/www/html/*");
	system(str);

	HCU_DEBUG_PRINT_INF("BFSCUICOMM: fsm_bfscuicomm_l3bfsc_cfg_resp: fileStream=%x, zHcuCmdflagJsonFile = %d\n", fileStream, zHcuCmdflagJsonFile);

	//启动周期性定时器
	ret = hcu_timer_start(TASK_ID_BFSCUICOMM, TIMER_ID_1S_BFSCUICOMM_PERIOD_READ, \
			zHcuSysEngPar.timer.array[TIMER_ID_1S_BFSCUICOMM_PERIOD_READ].dur, TIMER_TYPE_PERIOD, TIMER_RESOLUTION_1S);

	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_BFSCUICOMM, FSM_STATE_BFSCUICOMM_ACTIVED) == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_BFSCUICOMM]++;
		HcuErrorPrint("BFSCUICOMM: Error Set FSM State!\n");
		return FAILURE;
	}
	if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_FAT_ON) != FALSE){
		HcuDebugPrint("BFSCUICOMM: Enter FSM_STATE_BFSCUICOMM_ACTIVED status, Keeping refresh here!\n");
	}

	//返回
	return SUCCESS;
}

OPSTAT fsm_bfscuicomm_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HcuErrorPrint("BFSCUICOMM: Internal error counter reach DEAD level, SW-RESTART soon!\n");
	zHcuSysStaPm.statisCnt.restartCnt++;
	fsm_bfscuicomm_init(0, 0, NULL, 0);
	return SUCCESS;
}

//暂时啥都不干，定时到达后，还不明确是否需要支持定时工作
OPSTAT fsm_bfscuicomm_timeout(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret;

	//Receive message and copy to local variable
	msg_struct_com_time_out_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_com_time_out_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_com_time_out_t))){
		HcuErrorPrint("BFSCUICOMM: Receive message error!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_BFSCUICOMM]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//钩子在此处，检查zHcuRunErrCnt[TASK_ID_BFSCUICOMM]是否超限
	if (zHcuSysStaPm.taskRunErrCnt[TASK_ID_BFSCUICOMM] > HCU_RUN_ERROR_LEVEL_2_MAJOR){
		//减少重复RESTART的概率
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_BFSCUICOMM] = zHcuSysStaPm.taskRunErrCnt[TASK_ID_BFSCUICOMM] - HCU_RUN_ERROR_LEVEL_2_MAJOR;
		msg_struct_com_restart_t snd0;
		memset(&snd0, 0, sizeof(msg_struct_com_restart_t));
		snd0.length = sizeof(msg_struct_com_restart_t);
		ret = hcu_message_send(MSG_ID_COM_RESTART, TASK_ID_BFSCUICOMM, TASK_ID_BFSCUICOMM, &snd0, snd0.length);
		if (ret == FAILURE){
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_BFSCUICOMM]++;
			HcuErrorPrint("BFSCUICOMM: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_BFSCUICOMM].taskName, zHcuVmCtrTab.task[TASK_ID_BFSCUICOMM].taskName);
			return FAILURE;
		}
	}

	//Period time out received
	if ((rcv.timeId == TIMER_ID_1S_BFSCUICOMM_PERIOD_READ) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		//保护周期读数的优先级，强制抢占状态，并简化问题
		if (FsmGetState(TASK_ID_BFSCUICOMM) != FSM_STATE_BFSCUICOMM_ACTIVED){
			ret = FsmSetState(TASK_ID_BFSCUICOMM, FSM_STATE_BFSCUICOMM_ACTIVED);
			if (ret == FAILURE){
				zHcuSysStaPm.taskRunErrCnt[TASK_ID_BFSCUICOMM]++;
				HcuErrorPrint("BFSCUICOMM: Error Set FSM State!\n");
				return FAILURE;
			}//FsmSetState
		}

		//Do nothing
		func_bfscuicomm_time_out_period_read_process();
	}

	return SUCCESS;
}

//配置反馈
OPSTAT fsm_bfscuicomm_l3bfsc_cfg_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;

	msg_struct_l3bfsc_uicomm_cfg_resp_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_l3bfsc_uicomm_cfg_resp_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_l3bfsc_uicomm_cfg_resp_t)))
		HCU_ERROR_PRINT_BFSCUICOMM("BFSCUICOMM: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);

	//收到正确且所有秤台齐活的反馈
	HcuDebugPrint("BFSCUICOMM: fsm_bfscuicomm_l3bfsc_cfg_resp: rcv.validFlag = %d\n", rcv.validFlag);
	dbi_HcuBfsc_WmcCurComWgtUpdate(0);
	if((rcv.validFlag == TRUE) && (gTaskL3bfscuicommContext.bfscuiState == HCU_BFSCCOMM_JASON_CMD_START)){
		//Update databse, to let START menu turn state from grey to active!!!
		msg_struct_uicomm_l3bfsc_cmd_req_t snd;
		memset(&snd, 0, sizeof(msg_struct_uicomm_l3bfsc_cmd_req_t));
		snd.length = sizeof(msg_struct_uicomm_l3bfsc_cmd_req_t);
		snd.cmdid = HCU_SYSMSG_BFSC_UICOMM_CMDID_START;
		//发送命令给L3BFSC
		ret = hcu_message_send(MSG_ID_UICOMM_L3BFSC_CMD_REQ, TASK_ID_L3BFSC, TASK_ID_BFSCUICOMM, &snd, snd.length);
		if (ret == FAILURE){
			HCU_ERROR_PRINT_BFSCUICOMM("BFSCUICOMM: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_BFSCUICOMM].taskName, zHcuVmCtrTab.task[TASK_ID_L3BFSC].taskName);
			return FAILURE;
		}
	}
	//收到L3BFSC指示秤台配置错误的反馈
	else if ((rcv.validFlag == FALSE) && (gTaskL3bfscuicommContext.bfscuiState == HCU_BFSCCOMM_JASON_CMD_START)){
		hcu_sleep(2);
		msg_struct_uicomm_l3bfsc_cfg_req_t snd;
		memset(&snd, 0, sizeof(msg_struct_uicomm_l3bfsc_cfg_req_t));
		snd.length = sizeof(msg_struct_uicomm_l3bfsc_cfg_req_t);
		ret = hcu_message_send(MSG_ID_UICOMM_L3BFSC_CFG_REQ, TASK_ID_L3BFSC, TASK_ID_BFSCUICOMM, &snd, snd.length);
		if (ret == FAILURE){
			HCU_ERROR_PRINT_BFSCUICOMM("BFSCUICOMM: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_BFSCUICOMM].taskName, zHcuVmCtrTab.task[TASK_ID_L3BFSC].taskName);
			return FAILURE;
		}
	}
	//返回
	return SUCCESS;
}

//启动停止反馈
OPSTAT fsm_bfscuicomm_l3bfsc_cmd_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;
	UINT8  	sensorid = 0;
	UINT8	validFlag = 0;
	UINT8	cmdid = 0;

	msg_struct_l3bfsc_uicomm_cmd_resp_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_l3bfsc_uicomm_cmd_resp_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_l3bfsc_uicomm_cmd_resp_t)))
		HCU_ERROR_PRINT_BFSCUICOMM("BFSCUICOMM: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);

	sensorid = rcv.sensorid;
	validFlag = rcv.validFlag;
	cmdid = rcv.cmdid;

	if(sensorid < 1 || sensorid >= HCU_SYSCFG_BFSC_SNR_WS_NBR_MAX){
		HCU_ERROR_PRINT_BFSCUICOMM("BFSCUICOMM: fsm_bfscuicomm_l3bfsc_cmd_resp's sensorid out of range, [sensorid=%d]! \n", sensorid);
		return FAILURE;
	}

	//存入数据库表单，通知界面新的状态信息
	if((rcv.validFlag == TRUE) && (cmdid == HCU_SYSMSG_BFSC_UICOMM_CMDID_SUSPEND)){
		ret = dbi_HcuBfsc_WmcStatusForceSuspend(sensorid);
		if (ret == FAILURE) {
			HCU_ERROR_PRINT_TASK(TASK_ID_BFSCUICOMM, "TASK_ID_BFSCUICOMM: Save data error!\n");
			return FAILURE;
		}
	}
	else{
		HCU_ERROR_PRINT_TASK(TASK_ID_BFSCUICOMM, "TASK_ID_BFSCUICOMM: Invalid command response!\n");
		return FAILURE;
	}

	//返回
	return SUCCESS;
}

//一般性测试命令的反馈
OPSTAT fsm_bfscuicomm_can_test_cmd_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;
	UINT32  adcvalue = 0, weight = 0;
	UINT16 	errorcode;
	UINT8  	sensorid = 0;
	UINT8	validFlag = 0;

	msg_struct_can_uicomm_test_cmd_resp_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_can_uicomm_test_cmd_resp_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_can_uicomm_test_cmd_resp_t)))
		HCU_ERROR_PRINT_BFSCUICOMM("BFSCUICOMM: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);

	//Special process for calibration command, save result into table `hcubfsccalibration`
	if (rcv.cmdid == CMDID_SENSOR_COMMAND_CALIBRATION_ZERO){
		sensorid = rcv.sensorid;
		adcvalue = rcv.cmdvalue1;
		weight = rcv.cmdvalue2;
		if(sensorid < 1 || sensorid >= HCU_SYSCFG_BFSC_SNR_WS_NBR_MAX){
			HCU_ERROR_PRINT_BFSCUICOMM("BFSCUICOMM: fsm_bfscuicomm_can_test_cmd_resp's sensorid out of range, [sensorid=%d]! \n", sensorid);
			return FAILURE;
		}
		ret = dbi_HcuBfsc_CalibrationDataUpdate_adczero(adcvalue, weight, sensorid);
		if (ret == FAILURE) HCU_ERROR_PRINT_TASK(TASK_ID_BFSCUICOMM, "TASK_ID_BFSCUICOMM: Save data error!\n");
	}
	else if (rcv.cmdid == CMDID_SENSOR_COMMAND_CALIBRATION_FULL){
		sensorid = rcv.sensorid;
		adcvalue = rcv.cmdvalue1;
		weight = rcv.cmdvalue2;
		if(sensorid < 1 || sensorid >= HCU_SYSCFG_BFSC_SNR_WS_NBR_MAX){
			HCU_ERROR_PRINT_BFSCUICOMM("BFSCUICOMM: fsm_bfscuicomm_can_test_cmd_resp's sensorid out of range, [sensorid=%d]! \n", sensorid);
			return FAILURE;
		}
		ret = dbi_HcuBfsc_CalibrationDataUpdate_adcfull(adcvalue, weight, sensorid);
		if (ret == FAILURE) HCU_ERROR_PRINT_TASK(TASK_ID_BFSCUICOMM, "TASK_ID_BFSCUICOMM: Save data error!\n");
	}
	//Common process for other test command, save the response to table 'hcubfscfb2ui'
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
		ret = dbi_HcuBfsc_TestCmdRespUpdate(rcv.cmdid, validFlag, strResp);
		if (ret == FAILURE) HCU_ERROR_PRINT_TASK(TASK_ID_BFSCUICOMM, "TASK_ID_BFSCUICOMM: Save data error!\n");

	}

	//返回
	return SUCCESS;
}

//具体扫描文件改变的回调函数
OPSTAT  fsm_bfscuicomm_scan_jason_callback(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret = 0;
	UINT32  fileChangeContent = 0;
	UINT8  sensorid = 0;

	L3BfscuiJsonCmdParseResult_t parseResult;

	//分析命令标志文件command.json的变化
	memset(&parseResult, 0, sizeof(L3BfscuiJsonCmdParseResult_t));
	ret =  func_bfscuicomm_cmdfile_json_parse(zHcuCmdflagJsonFile, &parseResult );
	if (ret == FAILURE){
		HCU_ERROR_PRINT_BFSCUICOMM("BFSCUICOMM: Command Json file parse failure, [func = func_bfscuicomm_cmdfile_json_parse]");
		return FAILURE;
	}

	dbi_HcuBfsc_WmcCurComWgtUpdate(0);
	//开启命令标志位发生了变化，
	if (parseResult.cmdStartStop.cmdFlag != gTaskL3bfscuicommContext.cmdStartStopFlag)
	{
		gTaskL3bfscuicommContext.cmdStartStopFlag = parseResult.cmdStartStop.cmdFlag;
		fileChangeContent = parseResult.cmdStartStop.cmdValue;
		//依赖文件变化的内容，分类发送控制命令：START/STOP命令
		if (fileChangeContent == HCU_BFSCCOMM_JASON_CMD_START){
			UINT16 config_index = 0;
			gTaskL3bfscuicommContext.bfscuiState = HCU_BFSCCOMM_JASON_CMD_START;
			config_index = parseResult.cmdStartStop.confindex;
			if (func_bfscuicomm_read_cfg_file_into_ctrl_table(config_index) == SUCCESS){
				msg_struct_uicomm_l3bfsc_cfg_req_t snd;
				memset(&snd, 0, sizeof(msg_struct_uicomm_l3bfsc_cfg_req_t));
				snd.length = sizeof(msg_struct_uicomm_l3bfsc_cfg_req_t);
				ret = hcu_message_send(MSG_ID_UICOMM_L3BFSC_CFG_REQ, TASK_ID_L3BFSC, TASK_ID_BFSCUICOMM, &snd, snd.length);
				if (ret == FAILURE){
					HCU_ERROR_PRINT_BFSCUICOMM("BFSCUICOMM: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_BFSCUICOMM].taskName, zHcuVmCtrTab.task[TASK_ID_L3BFSC].taskName);
					return FAILURE;
				}
			}
		}
		else if (fileChangeContent == HCU_BFSCCOMM_JASON_CMD_STOP){
			gTaskL3bfscuicommContext.bfscuiState = HCU_BFSCCOMM_JASON_CMD_STOP;
			msg_struct_uicomm_l3bfsc_cmd_req_t snd;
			memset(&snd, 0, sizeof(msg_struct_uicomm_l3bfsc_cmd_req_t));
			snd.length = sizeof(msg_struct_uicomm_l3bfsc_cmd_req_t);
			snd.cmdid = HCU_SYSMSG_BFSC_UICOMM_CMDID_STOP;
			//发送命令给L3BFSC
			ret = hcu_message_send(MSG_ID_UICOMM_L3BFSC_CMD_REQ, TASK_ID_L3BFSC, TASK_ID_BFSCUICOMM, &snd, snd.length);
			if (ret == FAILURE){
				HCU_ERROR_PRINT_BFSCUICOMM("BFSCUICOMM: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_BFSCUICOMM].taskName, zHcuVmCtrTab.task[TASK_ID_L3BFSC].taskName);
				return FAILURE;
			}
		}
	}

	//校准命令标志位发生了变化
	if (parseResult.cmdCalibration.cmdFlag != gTaskL3bfscuicommContext.cmdCalibrationFlag)
	{
		gTaskL3bfscuicommContext.cmdCalibrationFlag = parseResult.cmdCalibration.cmdFlag;
		fileChangeContent = parseResult.cmdCalibration.cmdValue;
		//依赖文件变化的内容，分类发送控制命令：零值校准命令/满值校准命令
		if ((fileChangeContent == HCU_BFSCCOMM_JASON_CMD_CALZERO) || (fileChangeContent == HCU_BFSCCOMM_JASON_CMD_CALFULL)){
			sensorid = parseResult.cmdCalibration.sensorid;
			msg_struct_uicomm_can_test_cmd_req_t snd;
			memset(&snd, 0, sizeof(msg_struct_uicomm_can_test_cmd_req_t));
			snd.length = sizeof(msg_struct_uicomm_can_test_cmd_req_t);
			//check sensor id
			if(sensorid < 1 || sensorid >= HCU_SYSCFG_BFSC_SNR_WS_NBR_MAX){
				HCU_ERROR_PRINT_BFSCUICOMM("BFSCUICOMM: UI input sensorid out of range, [sensorid=%d]! \n", sensorid);
				return FAILURE;
			}

			if (fileChangeContent == HCU_BFSCCOMM_JASON_CMD_CALZERO)
			{
				snd.cmdid = CMDID_SENSOR_COMMAND_CALIBRATION_ZERO;
				snd.cmdvalue = 0;
			}
			else if (fileChangeContent == HCU_BFSCCOMM_JASON_CMD_CALFULL)
			{
				snd.cmdid = CMDID_SENSOR_COMMAND_CALIBRATION_FULL;
				snd.cmdvalue = gTaskL3bfscContext.wgtSnrPar.calibration[sensorid].WeightSensorCalibrationFullWeight;
			}

			snd.wsBitmap[sensorid] = 1;
			//发送命令给CANITFLEO
			ret = hcu_message_send(MSG_ID_UICOMM_CAN_TEST_CMD_REQ, TASK_ID_CANITFLEO, TASK_ID_BFSCUICOMM, &snd, snd.length);
			if (ret == FAILURE){
				HCU_ERROR_PRINT_BFSCUICOMM("BFSCUICOMM: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_BFSCUICOMM].taskName, zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName);
				return FAILURE;
			}
		}
	}

	//暂停命令标志位发生了变化
	if (parseResult.cmdResume.cmdFlag != gTaskL3bfscuicommContext.cmdResumeFlag)
	{
		gTaskL3bfscuicommContext.cmdResumeFlag = parseResult.cmdResume.cmdFlag;
		fileChangeContent = parseResult.cmdResume.cmdValue;
		//依赖文件变化的内容，分类发送控制命令：SUSPEND命令/RESUME命令
		if ((fileChangeContent == HCU_BFSCCOMM_JASON_CMD_RESUME) || (fileChangeContent == HCU_BFSCCOMM_JASON_CMD_SUSPEND)){
			msg_struct_uicomm_l3bfsc_cmd_req_t snd;
			memset(&snd, 0, sizeof(msg_struct_uicomm_l3bfsc_cmd_req_t));
			snd.length = sizeof(msg_struct_uicomm_l3bfsc_cmd_req_t);
			if (fileChangeContent == HCU_BFSCCOMM_JASON_CMD_RESUME)  snd.cmdid = HCU_SYSMSG_BFSC_UICOMM_CMDID_RESUME;
			else if (fileChangeContent == HCU_BFSCCOMM_JASON_CMD_SUSPEND)  snd.cmdid = HCU_SYSMSG_BFSC_UICOMM_CMDID_SUSPEND;

			ret = hcu_message_send(MSG_ID_UICOMM_L3BFSC_CMD_REQ, TASK_ID_L3BFSC, TASK_ID_BFSCUICOMM, &snd, snd.length);
			if (ret == FAILURE){
				HCU_ERROR_PRINT_BFSCUICOMM("BFSCUICOMM: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_BFSCUICOMM].taskName, zHcuVmCtrTab.task[TASK_ID_L3BFSC].taskName);
				return FAILURE;
			}
		}
	}

	//一般性测试命令标志位发生了变化
	if (parseResult.cmdTest.cmdFlag != gTaskL3bfscuicommContext.cmdTestFlag)
	{
		gTaskL3bfscuicommContext.cmdTestFlag = parseResult.cmdTest.cmdFlag;
		fileChangeContent = parseResult.cmdTest.cmdValue;

		//依赖文件变化的内容，分类发送控制命令：一般性控制命令
		if (fileChangeContent == HCU_BFSCCOMM_JASON_CMD_TEST){
			UINT8 testcmd = 0, testpara = 0;
			sensorid = parseResult.cmdTest.sensorid;
			testcmd = parseResult.cmdTest.testCmd;
			testpara = parseResult.cmdTest.testPara;
			msg_struct_uicomm_can_test_cmd_req_t snd;
			memset(&snd, 0, sizeof(msg_struct_uicomm_can_test_cmd_req_t));
			snd.length = sizeof(msg_struct_uicomm_can_test_cmd_req_t);
			//check sensor id
			if(sensorid < HCU_SYSCFG_BFSC_SNR_WS_NBR_MAX){
				snd.wsBitmap[sensorid] = 1;
			}
			else if (sensorid == HCU_SYSCFG_BFSC_SNR_WS_NBR_MAX){ //set all sensor
				memset(snd.wsBitmap, 1, sizeof(UINT8)*HCU_SYSCFG_BFSC_SNR_WS_NBR_MAX);
			}
			else{
				HCU_ERROR_PRINT_BFSCUICOMM("BFSCUICOMM: UI input sensorid out of range, [sensorid=%d]! \n", sensorid);
				return FAILURE;
			}

			snd.cmdid = testcmd;
			snd.cmdvalue = testpara;
			ret = hcu_message_send(MSG_ID_UICOMM_CAN_TEST_CMD_REQ, TASK_ID_CANITFLEO, TASK_ID_BFSCUICOMM, &snd, snd.length);
			if (ret == FAILURE){
				HCU_ERROR_PRINT_BFSCUICOMM("BFSCUICOMM: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_BFSCUICOMM].taskName, zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName);
				return FAILURE;
			}
		}
	}

	return SUCCESS;
}

OPSTAT func_bfscuicomm_time_out_period_read_process(void)
{
//	UINT8 state = FsmGetState(TASK_ID_L3BFSC);

//	if ((state == FSM_STATE_L3BFSC_ACTIVED) || (state == FSM_STATE_L3BFSC_OPR_CFG)) {
//		//启动完成以后，等待一小会儿，然后将缺省的参数读入到系统内存，并发送CFG_REQ给L3BFSC
//		//如果缺省参数读取不成功，等待人工干预并读取，然后再发送给L3BFSC
//		if (func_bfscuicomm_read_cfg_file_into_ctrl_table() == SUCCESS){
//			msg_struct_uicomm_l3bfsc_cfg_req_t snd;
//			memset(&snd, 0, sizeof(msg_struct_uicomm_l3bfsc_cfg_req_t));
//			snd.length = sizeof(msg_struct_uicomm_l3bfsc_cfg_req_t);
//			if (hcu_message_send(MSG_ID_UICOMM_L3BFSC_CFG_REQ, TASK_ID_L3BFSC, TASK_ID_BFSCUICOMM, &snd, snd.length) == FAILURE)
//				HCU_ERROR_PRINT_BFSCUICOMM("BFSCUICOMM: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_BFSCUICOMM].taskName, zHcuVmCtrTab.task[TASK_ID_L3BFSC].taskName);
//		}
//	}
//	else if (state == FSM_STATE_L3BFSC_OPR_GO) {
//		msg_struct_uicomm_l3bfsc_cmd_req_t snd_start_req;
//		memset(&snd_start_req, 0, sizeof(msg_struct_uicomm_l3bfsc_cmd_req_t))				HCU_ERROR_PRINT_BFSCUICOMM("BFSCUICOMM: UI input sensorid out of range, [sensorid=%d]! \n", sensorid);
	return FAILURE;;
//		snd_start_req.length = sizeof(msg_struct_uicomm_l3bfsc_cmd_req_t);
//		snd_start_req.cmdid = HCU_SYSMSG_BFSC_UICOMM_CMDID_START;
//		if (hcu_message_send(MSG_ID_UICOMM_L3BFSC_CMD_REQ, TASK_ID_L3BFSC, TASK_ID_BFSCUICOMM, &snd_start_req, snd_start_req.length) == FAILURE)
//			HCU_ERROR_PRINT_BFSCUICOMM("BFSCUICOMM: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_BFSCUICOMM].taskName, zHcuVmCtrTab.task[TASK_ID_L3BFSC].taskName);
//		}
//	else{
//
//	}
	return SUCCESS;
}

//命令标志command.Json文件解析
OPSTAT  func_bfscuicomm_cmdfile_json_parse(char *monitorJsonFile, L3BfscuiJsonCmdParseResult_t *parseResult )
{
	FILE *fileStream;
	char inotifyReadBuf[1000];
	UINT32  numread = 0;
	UINT32  flag = 0, value = 0, errid = 0;
	UINT8  sensorid = 0;

	if((NULL == monitorJsonFile) || (NULL == parseResult))
	{
		HCU_ERROR_PRINT_BFSCUICOMM("BFSCUICOMM: (NULL == monitorJsonFile) || (NULL == parseResult), return.\n");
		return FAILURE;
	}

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
					HCU_ERROR_PRINT_BFSCUICOMM("BFSCUICOMM: Read NULL command json file, [file=%s] [errid=%d] ! \n", monitorJsonFile, errid);
					fclose( fileStream );
					return FAILURE;
				}
			}
			fclose( fileStream );

			file_jsonobj = json_tokener_parse(inotifyReadBuf);
			if (file_jsonobj == NULL){
				HCU_ERROR_PRINT_BFSCUICOMM("BFSCUICOMM: Command file json_tokener_parse failure, [file=%s]  ! \n", monitorJsonFile);
				json_object_put(file_jsonobj);  //释放Json Object指针
				return FAILURE;
			}

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
				HCU_ERROR_PRINT_BFSCUICOMM("BFSCUICOMM: Field [start_cmd] dose not exist in command json file ! \n");

		   //解析Calibration命令
		  if (json_object_object_get_ex(file_jsonobj, "calibration_cmd", &cmd_jsonobj)){
			  flag_jsonobj = json_object_object_get(cmd_jsonobj, "flag");
			  value_jsonobj = json_object_object_get(cmd_jsonobj, "value");
			  sensorid_jsonobj =  json_object_object_get(cmd_jsonobj, "sensorid");
			  if ((flag_jsonobj != NULL) && (value_jsonobj != NULL) && (sensorid_jsonobj != NULL)){
				  flag = json_object_get_int(flag_jsonobj);
				  value = json_object_get_int(value_jsonobj);
				  sensorid = json_object_get_int(sensorid_jsonobj);
				  if ((sensorid >0 ) && (sensorid < HCU_SYSCFG_BFSC_SNR_WS_NBR_MAX)){
					  parseResult->cmdCalibration.cmdFlag = flag;
					  parseResult->cmdCalibration.cmdValue = value;
					  parseResult->cmdCalibration.sensorid = sensorid;
					  parseResult->cmdCalibration.weight = gTaskL3bfscContext.wgtSnrPar.calibration[sensorid].WeightSensorCalibrationFullWeight;
				  }
			  }
		  }
		 else
			 HCU_ERROR_PRINT_BFSCUICOMM("BFSCUICOMM: Field [calibration_cmd] dose not exist in command json file ! \n");

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
			HCU_ERROR_PRINT_BFSCUICOMM("BFSCUICOMM: Field [resume_cmd] dose not exist in command json file ! \n");

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
			HCU_ERROR_PRINT_BFSCUICOMM("BFSCUICOMM: Field [test_cmd] dose not exist in command json file ! \n");

		 //释放Json Object指针
		json_object_put(file_jsonobj);
		json_object_put(cmd_jsonobj);
		json_object_put(flag_jsonobj);
		json_object_put(value_jsonobj);
		json_object_put(sensorid_jsonobj);

        return SUCCESS;
    }
    else
    {
    	HCU_ERROR_PRINT_BFSCUICOMM("BFSCUICOMM: Open command json file failure, file=%s \n", monitorJsonFile);
        return FAILURE;
    }
}

//扫描文件是否有DEFAULT参数，并配置进入系统参数控制表
OPSTAT func_bfscuicomm_read_cfg_file_into_ctrl_table (UINT16 config_index)
{
	UINT32  calibrationdata[(HCU_SYSCFG_BFSC_SNR_WS_NBR_MAX-1)*3];
	UINT32 dynamicdata[HCU_SYSCFG_BFSC_DB_COLUMN_NUM_MAX], staticdata[HCU_SYSCFG_BFSC_DB_COLUMN_NUM_MAX];

	//Update config id to gTaskL3bfscContext
	gTaskL3bfscContext.configId = config_index;

	//查询用户动态配置参数
	if (dbi_HcuBfsc_DynamicConfigDataGet(config_index, dynamicdata) == FAILURE){
		HCU_ERROR_PRINT_BFSCUICOMM("BFSCUICOMM: Get DB algorithm data failed \n");
	}
	UINT8 i = 7;   //数据表单hcubfscconfigpara前7个参数为UI界面使用参数
	gTaskL3bfscContext.comAlgPar.MinScaleNumberCombination = dynamicdata[i++];
	gTaskL3bfscContext.comAlgPar.MaxScaleNumberCombination  = dynamicdata[i++];
	gTaskL3bfscContext.comAlgPar.MinScaleNumberStartCombination = dynamicdata[i++];
	gTaskL3bfscContext.comAlgPar.TargetCombinationWeight = dynamicdata[i++];
	gTaskL3bfscContext.comAlgPar.TargetCombinationUpperWeight = dynamicdata[i++];
	gTaskL3bfscContext.comAlgPar.IsProximitCombinationMode = dynamicdata[i++];
	gTaskL3bfscContext.comAlgPar.CombinationBias =  dynamicdata[i++];
	gTaskL3bfscContext.comAlgPar.IsRemainDetectionEnable = dynamicdata[i++];
	gTaskL3bfscContext.comAlgPar.RemainDetectionTimeSec  = dynamicdata[i++];
	gTaskL3bfscContext.comAlgPar.RemainScaleTreatment =  dynamicdata[i++];
	gTaskL3bfscContext.comAlgPar.IsPriorityScaleEnabled = dynamicdata[i++];
	gTaskL3bfscContext.comAlgPar.CombinationAutoMode = dynamicdata[i++];
	gTaskL3bfscContext.comAlgPar.MovingAvrageSpeedCount = dynamicdata[i++];
	gTaskL3bfscContext.wgtSnrPar.WeightSensorLoadDetectionTimeMs =  dynamicdata[i++];
	gTaskL3bfscContext.wgtSnrPar.WeightSensorLoadThread = dynamicdata[i++];
	gTaskL3bfscContext.wgtSnrPar.WeightSensorEmptyDetectionTimeMs = dynamicdata[i++];
	gTaskL3bfscContext.wgtSnrPar.WeightSensorEmptyThread = dynamicdata[i++];
	gTaskL3bfscContext.wgtSnrPar.StardardReadyTimeMs = dynamicdata[i++];
	gTaskL3bfscContext.motCtrPar.MotorSpeed = dynamicdata[i++];
	gTaskL3bfscContext.motCtrPar.MotorDirection = dynamicdata[i++];
	gTaskL3bfscContext.motCtrPar.MotorRollingStartMs = dynamicdata[i++];
	gTaskL3bfscContext.motCtrPar.MotorRollingStopMs  = dynamicdata[i++];
	gTaskL3bfscContext.motCtrPar.MotorRollingInveralMs = dynamicdata[i++];
	gTaskL3bfscContext.motCtrPar.MotorFailureDetectionVaration = dynamicdata[i++];
	gTaskL3bfscContext.motCtrPar.MotorFailureDetectionTimeMs = dynamicdata[i++];
	//重复参数
	gTaskL3bfscContext.wgtSnrPar.RemainDetectionTimeSec = gTaskL3bfscContext.comAlgPar.RemainDetectionTimeSec;

	//查询系统静态配置参数
	if (dbi_HcuBfsc_StaticConfigDataGet(staticdata) == FAILURE){
		HCU_ERROR_PRINT_BFSCUICOMM("BFSCUICOMM: Get DB algorithm data failed \n");
	}

	i =1;
	gTaskL3bfscContext.wgtSnrPar.MaxAllowedWeight = staticdata[i++];

	i++;  // calibration full weight
	gTaskL3bfscContext.wgtSnrPar.WeightSensorAdcGain = staticdata[i++];
	gTaskL3bfscContext.wgtSnrPar.WeightSensorAdcSampleFreq = staticdata[i++];
	gTaskL3bfscContext.wgtSnrPar.WeightSensorStaticZeroValue = staticdata[i++];
	gTaskL3bfscContext.wgtSnrPar.WeightSensorTailorValue = staticdata[i++];
	gTaskL3bfscContext.wgtSnrPar.WeightSensorDynamicZeroThreadValue = staticdata[i++];
	gTaskL3bfscContext.wgtSnrPar.WeightSensorDynamicZeroHysteresisMs = staticdata[i++];

	gTaskL3bfscContext.comAlgPar.CombinationSpeedMode = 0;
	gTaskL3bfscContext.wgtSnrPar.WeightSensorPickupThread = 300;
	gTaskL3bfscContext.wgtSnrPar.WeightSensorPickupDetectionTimeMs = 500;
	gTaskL3bfscContext.wgtSnrPar.WeightSensorInitOrNot = 0;
	gTaskL3bfscContext.wgtSnrPar.WeightSensorAdcBitwidth = 22;
	gTaskL3bfscContext.wgtSnrPar.WeightSensorAdcValue = 0;

	gTaskL3bfscContext.wgtSnrPar.WeightSensorFilterCoeff[0] = 0;				// NOT for GUI
	gTaskL3bfscContext.wgtSnrPar.WeightSensorFilterCoeff[1] = 0;				// NOT for GUI
	gTaskL3bfscContext.wgtSnrPar.WeightSensorFilterCoeff[2] = 0;				// NOT for GUI
	gTaskL3bfscContext.wgtSnrPar.WeightSensorFilterCoeff[3] = 0;				// NOT for GUI
	gTaskL3bfscContext.wgtSnrPar.WeightSensorOutputValue[0] = 1;				// NOT for GUI
	gTaskL3bfscContext.wgtSnrPar.WeightSensorOutputValue[1] = 0;				// NOT for GUI
	gTaskL3bfscContext.wgtSnrPar.WeightSensorOutputValue[2] = 0;				// NOT for GUI
	gTaskL3bfscContext.wgtSnrPar.WeightSensorOutputValue[3] = 0;				// NOT for GUI

	//查询校准数据
	if (dbi_HcuBfsc_CalibrationDataGet(calibrationdata) == FAILURE){
		HCU_ERROR_PRINT_BFSCUICOMM("BFSCUICOMM: Get DB sensor calibration data failed \n");
	}

	for (i = 1; i <= HCU_SYSCFG_BFSC_SNR_WS_NBR_MAX-1; i++){
		gTaskL3bfscContext.wgtSnrPar.calibration[i].WeightSensorCalibrationZeroAdcValue = calibrationdata[3*(i-1)];
		gTaskL3bfscContext.wgtSnrPar.calibration[i].WeightSensorCalibrationFullAdcValue = calibrationdata[3*(i-1)+1];
		gTaskL3bfscContext.wgtSnrPar.calibration[i].WeightSensorCalibrationFullWeight = calibrationdata[3*(i-1)+2];
	}

	return SUCCESS;
}
