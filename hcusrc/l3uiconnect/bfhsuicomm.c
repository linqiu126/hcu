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
/*
    {MSG_ID_COM_INIT_FEEDBACK,				FSM_STATE_BFHSUICOMM_ACTIVED,            	fsm_com_do_nothing},
    {MSG_ID_INOTIFY_UICOMM_FILE_CHANGE_IND,	FSM_STATE_BFHSUICOMM_ACTIVED,            	fsm_bfhsuicomm_scan_jason_callback},
	{MSG_ID_L3BFSC_UICOMM_CFG_RESP,      	FSM_STATE_BFHSUICOMM_ACTIVED,          		fsm_bfhsuicomm_l3bfsc_cfg_resp},	//配置反馈
	{MSG_ID_L3BFSC_UICOMM_CMD_RESP,      	FSM_STATE_BFHSUICOMM_ACTIVED,          		fsm_bfhsuicomm_l3bfsc_cmd_resp},	//人工控制反馈
	{MSG_ID_CAN_UICOMM_TEST_CMD_RESP,      	FSM_STATE_BFHSUICOMM_ACTIVED,          		fsm_bfhsuicomm_can_test_cmd_resp},  //测试命令反馈
*/

    //结束点，固定定义，不要改动
    {MSG_ID_END,            	FSM_STATE_END,             				NULL},  //Ending
};

//Global variables
#if (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFSC_CBU_ID)
	#include "../l3appctrl/l3bfsc.h"
	extern gTaskL3bfscContext_t gTaskL3bfscContext;
#endif

gTaskL3bfhsuicommContext_t gTaskL3bfhsuicommContext;

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

	memset(&gTaskL3bfhsuicommContext, 0, sizeof(gTaskL3bfhsuicommContext_t));

	//Create necessary exchange files

	//在系统/temp目录下创建空的command.json文件，用于通知界面HCU及下位机已经准备好了
/*
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
	if (system(str) < 0) printf("\n");
	sprintf(str, "chmod -R 777 /var/www/html/*");
	system(str);

	HCU_DEBUG_PRINT_INF("BFHSUICOMM: fsm_bfhsuicomm_l3bfsc_cfg_resp: fileStream=%x, zHcuCmdflagJsonFile = %d\n", fileStream, zHcuCmdflagJsonFile);
*/

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
/*
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
*/
	return SUCCESS;
}

//配置反馈
/*OPSTAT fsm_bfhsuicomm_l3bfsc_cfg_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret=0;

	msg_struct_l3bfsc_uicomm_cfg_resp_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_l3bfsc_uicomm_cfg_resp_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_l3bfsc_uicomm_cfg_resp_t)))
		HCU_ERROR_PRINT_BFHSUICOMM("BFHSUICOMM: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);

	//收到正确且所有秤台齐活的反馈
	HcuDebugPrint("BFHSUICOMM: fsm_bfhsuicomm_l3bfsc_cfg_resp: rcv.validFlag = %d\n", rcv.validFlag);
	dbi_HcuBfsc_WmcCurComWgtUpdate(0);
	if((rcv.validFlag == TRUE) && (gTaskL3bfhsuicommContext.bfscuiState == HCU_BFSCCOMM_JASON_CMD_START)){
		//Update databse, to let START menu turn state from grey to active!!!
		msg_struct_uicomm_l3bfsc_cmd_req_t snd;
		memset(&snd, 0, sizeof(msg_struct_uicomm_l3bfsc_cmd_req_t));
		snd.length = sizeof(msg_struct_uicomm_l3bfsc_cmd_req_t);
		snd.cmdid = HCU_SYSMSG_BFSC_UICOMM_CMDID_START;
		//发送命令给L3BFSC
		if (hcu_message_send(MSG_ID_UICOMM_L3BFSC_CMD_REQ, TASK_ID_L3BFSC, TASK_ID_BFHSUICOMM, &snd, snd.length) == FAILURE)
			HCU_ERROR_PRINT_BFHSUICOMM("BFHSUICOMM: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_BFHSUICOMM].taskName, zHcuVmCtrTab.task[TASK_ID_L3BFSC].taskName);
	}
	//收到L3BFSC指示秤台配置错误的反馈
	else if ((rcv.validFlag == FALSE) && (gTaskL3bfhsuicommContext.bfscuiState == HCU_BFSCCOMM_JASON_CMD_START)){
		hcu_sleep(2);
		msg_struct_uicomm_l3bfsc_cfg_req_t snd;
		memset(&snd, 0, sizeof(msg_struct_uicomm_l3bfsc_cfg_req_t));
		snd.length = sizeof(msg_struct_uicomm_l3bfsc_cfg_req_t);
		if (hcu_message_send(MSG_ID_UICOMM_L3BFSC_CFG_REQ, TASK_ID_L3BFSC, TASK_ID_BFHSUICOMM, &snd, snd.length) == FAILURE)
			HCU_ERROR_PRINT_BFHSUICOMM("BFHSUICOMM: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_BFHSUICOMM].taskName, zHcuVmCtrTab.task[TASK_ID_L3BFSC].taskName);
	}
	//返回
	return SUCCESS;
}

//启动停止反馈
OPSTAT fsm_bfhsuicomm_l3bfsc_cmd_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;
	UINT8	validFlag = 0;
	UINT8	cmdid = 0;

	msg_struct_l3bfsc_uicomm_cmd_resp_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_l3bfsc_uicomm_cmd_resp_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_l3bfsc_uicomm_cmd_resp_t)))
		HCU_ERROR_PRINT_BFHSUICOMM("BFHSUICOMM: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);

	validFlag = rcv.validFlag;
	cmdid = rcv.cmdid;
	//测试用的打印命令
	HCU_DEBUG_PRINT_NOR("TASK_ID_BFHSUICOMM: rcv.validFlag= %d, cmdid = %d!\n", validFlag,cmdid);

	//存入数据库表单，通知界面新的状态信息
	if ((rcv.validFlag == TRUE) && (cmdid == HCU_SYSMSG_BFSC_UICOMM_CMDID_START)){
		//do nothing
	}
	else if ((rcv.validFlag == TRUE) && (cmdid == HCU_SYSMSG_BFSC_UICOMM_CMDID_STOP)){
		//do nothing
	}
	else if((rcv.validFlag == TRUE) && (cmdid == HCU_SYSMSG_BFSC_UICOMM_CMDID_SUSPEND)){
		ret = dbi_HcuBfsc_WmcStatusForceSuspend();
		if (ret == FAILURE) {
			HCU_ERROR_PRINT_TASK(TASK_ID_BFHSUICOMM, "TASK_ID_BFHSUICOMM: Save data error!\n");
			return FAILURE;
		}
	}
	else if ((rcv.validFlag == TRUE) && (cmdid == HCU_SYSMSG_BFSC_UICOMM_CMDID_RESUME)){
		//do nothing
	}
	else{
		HCU_ERROR_PRINT_TASK(TASK_ID_BFHSUICOMM, "TASK_ID_BFHSUICOMM: Invalid command response!\n");
		return FAILURE;
	}

	//返回
	return SUCCESS;
}

//一般性测试命令的反馈
OPSTAT fsm_bfhsuicomm_can_test_cmd_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;
	UINT32  adcvalue = 0;
	UINT16 	errorcode;
	UINT8  	sensorid = 0;
	UINT8	validFlag = 0;

	msg_struct_can_uicomm_test_cmd_resp_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_can_uicomm_test_cmd_resp_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_can_uicomm_test_cmd_resp_t)))
		HCU_ERROR_PRINT_BFHSUICOMM("BFHSUICOMM: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);

	//Special process for calibration command, save result into table `hcubfsccalibration`
	if (rcv.cmdid == CMDID_SENSOR_COMMAND_CALIBRATION_ZERO){
		sensorid = rcv.sensorid;
		adcvalue = rcv.cmdvalue1;
		if(sensorid < 1 || sensorid >= HCU_SYSCFG_BFSC_SNR_WS_NBR_MAX)
			HCU_ERROR_PRINT_BFHSUICOMM("BFHSUICOMM: fsm_bfhsuicomm_can_test_cmd_resp's sensorid out of range, [sensorid=%d]! \n", sensorid);
		ret = dbi_HcuBfsc_CalibrationDataUpdate_adczero(adcvalue, sensorid);
		if (ret == FAILURE) HCU_ERROR_PRINT_TASK(TASK_ID_BFHSUICOMM, "TASK_ID_BFHSUICOMM: Save data error!\n");
	}
	else if (rcv.cmdid == CMDID_SENSOR_COMMAND_CALIBRATION_FULL){
		sensorid = rcv.sensorid;
		adcvalue = rcv.cmdvalue1;
		if(sensorid < 1 || sensorid >= HCU_SYSCFG_BFSC_SNR_WS_NBR_MAX)
			HCU_ERROR_PRINT_BFHSUICOMM("BFHSUICOMM: fsm_bfhsuicomm_can_test_cmd_resp's sensorid out of range, [sensorid=%d]! \n", sensorid);
		ret = dbi_HcuBfsc_CalibrationDataUpdate_adcfull(adcvalue, sensorid);
		if (ret == FAILURE) HCU_ERROR_PRINT_TASK(TASK_ID_BFHSUICOMM, "TASK_ID_BFHSUICOMM: Save data error!\n");
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
		if (ret == FAILURE) HCU_ERROR_PRINT_TASK(TASK_ID_BFHSUICOMM, "TASK_ID_BFHSUICOMM: Save data error!\n");

	}

	//返回
	return SUCCESS;
}

//具体扫描文件改变的回调函数
OPSTAT  fsm_bfhsuicomm_scan_jason_callback(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret = 0;
	UINT32  fileChangeContent = 0;
	UINT8  sensorid = 0;

	L3BfscuiJsonCmdParseResult_t parseResult;

	//分析命令标志文件command.json的变化
	memset(&parseResult, 0, sizeof(L3BfscuiJsonCmdParseResult_t));
	if (func_bfhsuicomm_cmdfile_json_parse(zHcuCmdflagJsonFile, &parseResult) == FAILURE)
		HCU_ERROR_PRINT_BFHSUICOMM("BFHSUICOMM: Command Json file parse failure, [func = func_bfhsuicomm_cmdfile_json_parse]");

	dbi_HcuBfsc_WmcCurComWgtUpdate(0);
	//开启命令标志位发生了变化，
	if (parseResult.cmdStartStop.cmdFlag != gTaskL3bfhsuicommContext.cmdStartStopFlag)
	{
		gTaskL3bfhsuicommContext.cmdStartStopFlag = parseResult.cmdStartStop.cmdFlag;
		fileChangeContent = parseResult.cmdStartStop.cmdValue;
		//依赖文件变化的内容，分类发送控制命令：START/STOP命令
		if (fileChangeContent == HCU_BFSCCOMM_JASON_CMD_START){
			UINT16 config_index = 0;
			gTaskL3bfhsuicommContext.bfscuiState = HCU_BFSCCOMM_JASON_CMD_START;
			config_index = parseResult.cmdStartStop.confindex;
			if (func_bfhsuicomm_read_cfg_file_into_ctrl_table(config_index) == SUCCESS){
				msg_struct_uicomm_l3bfsc_cfg_req_t snd;
				memset(&snd, 0, sizeof(msg_struct_uicomm_l3bfsc_cfg_req_t));
				snd.length = sizeof(msg_struct_uicomm_l3bfsc_cfg_req_t);
				if (hcu_message_send(MSG_ID_UICOMM_L3BFSC_CFG_REQ, TASK_ID_L3BFSC, TASK_ID_BFHSUICOMM, &snd, snd.length) == FAILURE)
					HCU_ERROR_PRINT_BFHSUICOMM("BFHSUICOMM: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_BFHSUICOMM].taskName, zHcuVmCtrTab.task[TASK_ID_L3BFSC].taskName);
			}
		}
		else if (fileChangeContent == HCU_BFSCCOMM_JASON_CMD_STOP){
			gTaskL3bfhsuicommContext.bfscuiState = HCU_BFSCCOMM_JASON_CMD_STOP;
			msg_struct_uicomm_l3bfsc_cmd_req_t snd;
			memset(&snd, 0, sizeof(msg_struct_uicomm_l3bfsc_cmd_req_t));
			snd.length = sizeof(msg_struct_uicomm_l3bfsc_cmd_req_t);
			snd.cmdid = HCU_SYSMSG_BFSC_UICOMM_CMDID_STOP;
			//发送命令给L3BFSC
			if (hcu_message_send(MSG_ID_UICOMM_L3BFSC_CMD_REQ, TASK_ID_L3BFSC, TASK_ID_BFHSUICOMM, &snd, snd.length) == FAILURE)
				HCU_ERROR_PRINT_BFHSUICOMM("BFHSUICOMM: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_BFHSUICOMM].taskName, zHcuVmCtrTab.task[TASK_ID_L3BFSC].taskName);
		}
	}

	//校准命令标志位发生了变化
	if (parseResult.cmdCalibration.cmdFlag != gTaskL3bfhsuicommContext.cmdCalibrationFlag)
	{
		gTaskL3bfhsuicommContext.cmdCalibrationFlag = parseResult.cmdCalibration.cmdFlag;
		fileChangeContent = parseResult.cmdCalibration.cmdValue;
		//依赖文件变化的内容，分类发送控制命令：零值校准命令/满值校准命令
		if ((fileChangeContent == HCU_BFSCCOMM_JASON_CMD_CALZERO) || (fileChangeContent == HCU_BFSCCOMM_JASON_CMD_CALFULL)){
			sensorid = parseResult.cmdCalibration.sensorid;
			msg_struct_uicomm_can_test_cmd_req_t snd;
			memset(&snd, 0, sizeof(msg_struct_uicomm_can_test_cmd_req_t));
			snd.length = sizeof(msg_struct_uicomm_can_test_cmd_req_t);
			//check sensor id
			if(sensorid < 1 || sensorid >= HCU_SYSCFG_BFSC_SNR_WS_NBR_MAX)
				HCU_ERROR_PRINT_BFHSUICOMM("BFHSUICOMM: UI input sensorid out of range, [sensorid=%d]! \n", sensorid);

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
			if (hcu_message_send(MSG_ID_UICOMM_CAN_TEST_CMD_REQ, TASK_ID_CANITFLEO, TASK_ID_BFHSUICOMM, &snd, snd.length) == FAILURE)
				HCU_ERROR_PRINT_BFHSUICOMM("BFHSUICOMM: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_BFHSUICOMM].taskName, zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName);
		}
	}

	//暂停命令标志位发生了变化
	if (parseResult.cmdResume.cmdFlag != gTaskL3bfhsuicommContext.cmdResumeFlag)
	{
		gTaskL3bfhsuicommContext.cmdResumeFlag = parseResult.cmdResume.cmdFlag;
		fileChangeContent = parseResult.cmdResume.cmdValue;
		//依赖文件变化的内容，分类发送控制命令：SUSPEND命令/RESUME命令
		if (fileChangeContent == HCU_BFSCCOMM_JASON_CMD_RESUME) {
			//Re-configure/start all weight sensor
			gTaskL3bfhsuicommContext.bfscuiState = HCU_BFSCCOMM_JASON_CMD_START;
			msg_struct_uicomm_l3bfsc_cfg_req_t snd;
			memset(&snd, 0, sizeof(msg_struct_uicomm_l3bfsc_cfg_req_t));
			snd.length = sizeof(msg_struct_uicomm_l3bfsc_cfg_req_t);
			if (hcu_message_send(MSG_ID_UICOMM_L3BFSC_CFG_REQ, TASK_ID_L3BFSC, TASK_ID_BFHSUICOMM, &snd, snd.length) == FAILURE)
				HCU_ERROR_PRINT_BFHSUICOMM("BFHSUICOMM: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_BFHSUICOMM].taskName, zHcuVmCtrTab.task[TASK_ID_L3BFSC].taskName);
		}
	}

	//一般性测试命令标志位发生了变化
	if (parseResult.cmdTest.cmdFlag != gTaskL3bfhsuicommContext.cmdTestFlag)
	{
		gTaskL3bfhsuicommContext.cmdTestFlag = parseResult.cmdTest.cmdFlag;
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
			else
				HCU_ERROR_PRINT_BFHSUICOMM("BFHSUICOMM: UI input sensorid out of range, [sensorid=%d]! \n", sensorid);

			snd.cmdid = testcmd;
			snd.cmdvalue = testpara;
			if (hcu_message_send(MSG_ID_UICOMM_CAN_TEST_CMD_REQ, TASK_ID_CANITFLEO, TASK_ID_BFHSUICOMM, &snd, snd.length) == FAILURE)
				HCU_ERROR_PRINT_BFHSUICOMM("BFHSUICOMM: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_BFHSUICOMM].taskName, zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName);
		}
	}

	return SUCCESS;
}

OPSTAT func_bfhsuicomm_time_out_period_read_process(void)
{
//	UINT8 state = FsmGetState(TASK_ID_L3BFSC);

//	if ((state == FSM_STATE_L3BFSC_ACTIVED) || (state == FSM_STATE_L3BFSC_OPR_CFG)) {
//		//启动完成以后，等待一小会儿，然后将缺省的参数读入到系统内存，并发送CFG_REQ给L3BFSC
//		//如果缺省参数读取不成功，等待人工干预并读取，然后再发送给L3BFSC
//		if (func_bfhsuicomm_read_cfg_file_into_ctrl_table() == SUCCESS){
//			msg_struct_uicomm_l3bfsc_cfg_req_t snd;
//			memset(&snd, 0, sizeof(msg_struct_uicomm_l3bfsc_cfg_req_t));
//			snd.length = sizeof(msg_struct_uicomm_l3bfsc_cfg_req_t);
//			if (hcu_message_send(MSG_ID_UICOMM_L3BFSC_CFG_REQ, TASK_ID_L3BFSC, TASK_ID_BFHSUICOMM, &snd, snd.length) == FAILURE)
//				HCU_ERROR_PRINT_BFHSUICOMM("BFHSUICOMM: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_BFHSUICOMM].taskName, zHcuVmCtrTab.task[TASK_ID_L3BFSC].taskName);
//		}
//	}
//	else if (state == FSM_STATE_L3BFSC_OPR_GO) {
//		msg_struct_uicomm_l3bfsc_cmd_req_t snd_start_req;
//		memset(&snd_start_req, 0, sizeof(msg_struct_uicomm_l3bfsc_cmd_req_t))				HCU_ERROR_PRINT_BFHSUICOMM("BFHSUICOMM: UI input sensorid out of range, [sensorid=%d]! \n", sensorid);
	return FAILURE;;
//		snd_start_req.length = sizeof(msg_struct_uicomm_l3bfsc_cmd_req_t);
//		snd_start_req.cmdid = HCU_SYSMSG_BFSC_UICOMM_CMDID_START;
//		if (hcu_message_send(MSG_ID_UICOMM_L3BFSC_CMD_REQ, TASK_ID_L3BFSC, TASK_ID_BFHSUICOMM, &snd_start_req, snd_start_req.length) == FAILURE)
//			HCU_ERROR_PRINT_BFHSUICOMM("BFHSUICOMM: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_BFHSUICOMM].taskName, zHcuVmCtrTab.task[TASK_ID_L3BFSC].taskName);
//		}
//	else{
//
//	}
	return SUCCESS;
}

//命令标志command.Json文件解析
OPSTAT  func_bfhsuicomm_cmdfile_json_parse(char *monitorJsonFile, L3BfscuiJsonCmdParseResult_t *parseResult )
{
	FILE *fileStream;
	char inotifyReadBuf[1000];
	UINT32  numread = 0;
	UINT32  flag = 0, value = 0, errid = 0;
	UINT8  sensorid = 0;

	if((NULL == monitorJsonFile) || (NULL == parseResult))
		HCU_ERROR_PRINT_BFHSUICOMM("BFHSUICOMM: (NULL == monitorJsonFile) || (NULL == parseResult), return.\n");

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
					HCU_ERROR_PRINT_BFHSUICOMM("BFHSUICOMM: Read NULL command json file, [file=%s] [errid=%d] ! \n", monitorJsonFile, errid);
				}
			}
			fclose( fileStream );

			file_jsonobj = json_tokener_parse(inotifyReadBuf);
			if (file_jsonobj == NULL)
				HCU_ERROR_PRINT_BFHSUICOMM("BFHSUICOMM: Command file json_tokener_parse failure, [file=%s]  ! \n", monitorJsonFile);

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
				HCU_ERROR_PRINT_BFHSUICOMM("BFHSUICOMM: Field [start_cmd] dose not exist in command json file ! \n");

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
			 HCU_ERROR_PRINT_BFHSUICOMM("BFHSUICOMM: Field [calibration_cmd] dose not exist in command json file ! \n");

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
			HCU_ERROR_PRINT_BFHSUICOMM("BFHSUICOMM: Field [resume_cmd] dose not exist in command json file ! \n");

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
			HCU_ERROR_PRINT_BFHSUICOMM("BFHSUICOMM: Field [test_cmd] dose not exist in command json file ! \n");

		 //释放Json Object指针
		json_object_put(file_jsonobj);
		json_object_put(cmd_jsonobj);
		json_object_put(flag_jsonobj);
		json_object_put(value_jsonobj);
		json_object_put(sensorid_jsonobj);

        return SUCCESS;
    }
    else
    	HCU_ERROR_PRINT_BFHSUICOMM("BFHSUICOMM: Open command json file failure, file=%s \n", monitorJsonFile);
}

//扫描文件是否有DEFAULT参数，并配置进入系统参数控制表
OPSTAT func_bfhsuicomm_read_cfg_file_into_ctrl_table (UINT16 config_index)
{
	UINT32  calibrationdata[(HCU_SYSCFG_BFSC_SNR_WS_NBR_MAX-1)*3];
	UINT32 dynamicdata[HCU_SYSCFG_BFSC_DB_COLUMN_NUM_MAX], staticdata[HCU_SYSCFG_BFSC_DB_COLUMN_NUM_MAX];

	//Update config id to gTaskL3bfscContext
	gTaskL3bfscContext.configId = config_index;
	memset(&(gTaskL3bfscContext.staLocalUi), 0, sizeof(HcuSysMsgIeL3bfscContextStaElement_t));
	memset(&(gTaskL3bfscContext.staOneMin), 0, sizeof(HcuSysMsgIeL3bfscContextStaElement_t));
	memset(&(gTaskL3bfscContext.sta15Min), 0, sizeof(HcuSysMsgIeL3bfscContextStaElement_t));
	memset(&(gTaskL3bfscContext.sta60Min), 0, sizeof(HcuSysMsgIeL3bfscContextStaElement_t));
	memset(&(gTaskL3bfscContext.sta2H), 0, sizeof(HcuSysMsgIeL3bfscContextStaElement_t));
	memset(&(gTaskL3bfscContext.sta8H), 0, sizeof(HcuSysMsgIeL3bfscContextStaElement_t));
	memset(&(gTaskL3bfscContext.sta24H), 0, sizeof(HcuSysMsgIeL3bfscContextStaElement_t));
	memset(&(gTaskL3bfscContext.staUp2Now), 0, sizeof(HcuSysMsgIeL3bfscContextStaElement_t));

	//查询用户动态配置参数
	if (dbi_HcuBfsc_DynamicConfigDataGet(config_index, dynamicdata) == FAILURE){
		HCU_ERROR_PRINT_BFHSUICOMM("BFHSUICOMM: Get DB algorithm data failed \n");
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
		HCU_ERROR_PRINT_BFHSUICOMM("BFHSUICOMM: Get DB algorithm data failed \n");
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
		HCU_ERROR_PRINT_BFHSUICOMM("BFHSUICOMM: Get DB sensor calibration data failed \n");
	}

	for (i = 1; i <= HCU_SYSCFG_BFSC_SNR_WS_NBR_MAX-1; i++){
		gTaskL3bfscContext.wgtSnrPar.calibration[i].WeightSensorCalibrationZeroAdcValue = calibrationdata[3*(i-1)];
		gTaskL3bfscContext.wgtSnrPar.calibration[i].WeightSensorCalibrationFullAdcValue = calibrationdata[3*(i-1)+1];
		gTaskL3bfscContext.wgtSnrPar.calibration[i].WeightSensorCalibrationFullWeight = calibrationdata[3*(i-1)+2];
	}

	return SUCCESS;
}*/



