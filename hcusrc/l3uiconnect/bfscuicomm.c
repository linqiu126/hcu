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


//本地全局变量，分别用于标识开启命令，校准命令，和配置命令的变化，系统启动初始化为0, UI界面修改后自动累加，HCU对比Flag变化判断用户修改了那个Json文件
UINT32  zCmdStartFlag = 0;
UINT32  zCmdCalibrationFlag = 0;
UINT32  zCmdConfigFlag = 0;
UINT32  zCmdTResumeFlag = 0;
UINT32  zCmdTestFlag = 0;

//Global variables
#if (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFSC_CBU_ID)
	#include "../l3appctrl/l3bfsc.h"
	extern gTaskL3bfscContext_t gTaskL3bfscContext;
#endif

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
	zCmdStartFlag = 0;
	zCmdCalibrationFlag = 0;
	zCmdConfigFlag = 0;
	zCmdTResumeFlag = 0;
	zCmdTestFlag = 0;

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
	//int ret=0;

	msg_struct_l3bfsc_uicomm_cfg_resp_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_l3bfsc_uicomm_cfg_resp_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_l3bfsc_uicomm_cfg_resp_t)))
		HCU_ERROR_PRINT_BFSCUICOMM("BFSCUICOMM: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);

	//存入数据库表单，通知界面新的状态信息

	//返回
	return SUCCESS;
}

//启动停止反馈
OPSTAT fsm_bfscuicomm_l3bfsc_cmd_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret=0;

	msg_struct_l3bfsc_uicomm_cmd_resp_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_l3bfsc_uicomm_cmd_resp_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_l3bfsc_uicomm_cmd_resp_t)))
		HCU_ERROR_PRINT_BFSCUICOMM("BFSCUICOMM: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);

	//存入数据库表单，通知界面新的状态信息

	//返回
	return SUCCESS;
}

//一般性测试命令的反馈
OPSTAT fsm_bfscuicomm_can_test_cmd_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret=0;
	UINT32  adcvalue = 0;
	UINT8  sensorid = 0;

	msg_struct_can_uicomm_test_cmd_resp_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_can_uicomm_test_cmd_resp_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_can_uicomm_test_cmd_resp_t)))
		HCU_ERROR_PRINT_BFSCUICOMM("BFSCUICOMM: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);

	//存入数据库表单，通知界面新的状态信息
	switch(rcv.cmdid)
	{
		case SESOR_COMMAND_ID_CALIBRATION_ZERO:
			adcvalue = rcv.sensor_weight;
			sensorid = rcv.sensorid;

			break;
		case SESOR_COMMAND_ID_CALIBRATION_FULL:
			adcvalue = rcv.sensor_weight;
			sensorid = rcv.sensorid;

			break;
		case SESOR_COMMAND_ID_WEITGH_READ:
			break;
		default:
			break;
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
	ret =  func_bfscuicomm_cmdfile_json_parse(zHcuCmdflagJsonFile, parseResult );
	if (ret == FAILURE){
		HCU_ERROR_PRINT_BFSCUICOMM("BFSCUICOMM: Command Json file parse failure, [func = func_bfscuicomm_cmdfile_json_parse]");
		return FAILURE;
	}

	//开启命令标志位发生了变化，
	if (parseResult.cmdStart.cmdFlag != zCmdStartFlag)
	{
		zCmdStartFlag = parseResult.cmdStart.cmdFlag;
		fileChangeContent = parseResult.cmdStart.cmdValue;
		//依赖文件变化的内容，分类发送控制命令：START/STOP命令
		if ((fileChangeContent == HCU_BFSCCOMM_JASON_CMD_START) || (fileChangeContent == HCU_BFSCCOMM_JASON_CMD_STOP)){
			msg_struct_uicomm_l3bfsc_cmd_req_t snd;
			memset(&snd, 0, sizeof(msg_struct_uicomm_l3bfsc_cmd_req_t));
			snd.length = sizeof(msg_struct_uicomm_l3bfsc_cmd_req_t);
			if (fileChangeContent == HCU_BFSCCOMM_JASON_CMD_START)  snd.cmdid = HCU_SYSMSG_BFSC_UICOMM_CMDID_START;
			else if (fileChangeContent == HCU_BFSCCOMM_JASON_CMD_STOP)  snd.cmdid = HCU_SYSMSG_BFSC_UICOMM_CMDID_STOP;

			//发送命令给L3BFSC
			ret = hcu_message_send(MSG_ID_UICOMM_L3BFSC_CMD_REQ, TASK_ID_L3BFSC, TASK_ID_BFSCUICOMM, &snd, snd.length);
			if (ret == FAILURE){
				HCU_ERROR_PRINT_BFSCUICOMM("BFSCUICOMM: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_BFSCUICOMM].taskName, zHcuVmCtrTab.task[TASK_ID_L3BFSC].taskName);
				return FAILURE;
			}
		}
	}

	//校准命令标志位发生了变化
	if (parseResult.cmdCalibration.cmdFlag != zCmdCalibrationFlag)
	{
		zCmdCalibrationFlag = parseResult.cmdCalibration.cmdFlag;
		fileChangeContent = parseResult.cmdCalibration.cmdValue;
		sensorid = parseResult.cmdCalibration.sensorid;
		//依赖文件变化的内容，分类发送控制命令：零值校准命令/满值校准命令
		if ((fileChangeContent == HCU_BFSCCOMM_JASON_CMD_CALZERO) || (fileChangeContent == HCU_BFSCCOMM_JASON_CMD_CALFULL)){
			msg_struct_uicomm_can_test_cmd_req_t snd;
			memset(&snd, 0, sizeof(msg_struct_uicomm_can_test_cmd_req_t));
			snd.length = sizeof(msg_struct_uicomm_can_test_cmd_req_t);
			if (fileChangeContent == HCU_BFSCCOMM_JASON_CMD_CALZERO)  snd.cmdid = SESOR_COMMAND_ID_CALIBRATION_ZERO;
			else if (fileChangeContent == HCU_BFSCCOMM_JASON_CMD_CALFULL)  snd.cmdid = SESOR_COMMAND_ID_CALIBRATION_FULL;

			if (sensorid >=0 && sensorid < HCU_SYSMSG_L3BFSC_MAX_SENSOR_NBR)  snd.wsBitmap[sensorid] = 1;
			//发送命令给CANITFLEO
			ret = hcu_message_send(MSG_ID_UICOMM_CAN_TEST_CMD_REQ, TASK_ID_CANITFLEO, TASK_ID_BFSCUICOMM, &snd, snd.length);
			if (ret == FAILURE){
				HCU_ERROR_PRINT_BFSCUICOMM("BFSCUICOMM: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_BFSCUICOMM].taskName, zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName);
				return FAILURE;
			}
		}
	}

	//业务配置命令标志位发生了变化
	if (parseResult.cmdConfig.cmdFlag != zCmdConfigFlag)
	{
		zCmdConfigFlag = parseResult.cmdConfig.cmdFlag;
		fileChangeContent = parseResult.cmdConfig.cmdValue;
		//依赖文件变化的内容，分类发送控制命令：CONFIG命令
		if (fileChangeContent == HCU_BFSCCOMM_JASON_CMD_CONFIG){
			if (func_bfscuicomm_read_cfg_file_into_ctrl_table() == SUCCESS){
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
	 }

	//暂停命令标志位发生了变化
	if (parseResult.cmdResume.cmdFlag != zCmdTResumeFlag)
	{
		zCmdTResumeFlag = parseResult.cmdResume.cmdFlag;
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
	if (parseResult.cmdTest.cmdFlag != zCmdTestFlag)
	{
		zCmdTestFlag = parseResult.cmdTest.cmdFlag;
		fileChangeContent = parseResult.cmdTest.cmdValue;
		//依赖文件变化的内容，分类发送控制命令：一般性控制命令
		if (fileChangeContent == HCU_BFSCCOMM_JASON_CMD_TEST){
			msg_struct_uicomm_can_test_cmd_req_t snd;
			memset(&snd, 0, sizeof(msg_struct_uicomm_can_test_cmd_req_t));
			snd.length = sizeof(msg_struct_uicomm_can_test_cmd_req_t);

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
	UINT8 state = FsmGetState(TASK_ID_L3BFSC);

	if ((state == FSM_STATE_L3BFSC_ACTIVED) || (state == FSM_STATE_L3BFSC_OPR_CFG)) {
		//启动完成以后，等待一小会儿，然后将缺省的参数读入到系统内存，并发送CFG_REQ给L3BFSC
		//如果缺省参数读取不成功，等待人工干预并读取，然后再发送给L3BFSC
		if (func_bfscuicomm_read_cfg_file_into_ctrl_table() == SUCCESS){
			msg_struct_uicomm_l3bfsc_cfg_req_t snd;
			memset(&snd, 0, sizeof(msg_struct_uicomm_l3bfsc_cfg_req_t));
			snd.length = sizeof(msg_struct_uicomm_l3bfsc_cfg_req_t);
			if (hcu_message_send(MSG_ID_UICOMM_L3BFSC_CFG_REQ, TASK_ID_L3BFSC, TASK_ID_BFSCUICOMM, &snd, snd.length) == FAILURE)
				HCU_ERROR_PRINT_BFSCUICOMM("BFSCUICOMM: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_BFSCUICOMM].taskName, zHcuVmCtrTab.task[TASK_ID_L3BFSC].taskName);
		}
	}
	else if (state == FSM_STATE_L3BFSC_OPR_GO) {
		msg_struct_uicomm_l3bfsc_cmd_req_t snd_start_req;
		memset(&snd_start_req, 0, sizeof(msg_struct_uicomm_l3bfsc_cmd_req_t));
		snd_start_req.length = sizeof(msg_struct_uicomm_l3bfsc_cmd_req_t);
		snd_start_req.cmdid = HCU_SYSMSG_BFSC_UICOMM_CMDID_START;
		if (hcu_message_send(MSG_ID_UICOMM_L3BFSC_CMD_REQ, TASK_ID_L3BFSC, TASK_ID_BFSCUICOMM, &snd_start_req, snd_start_req.length) == FAILURE)
			HCU_ERROR_PRINT_BFSCUICOMM("BFSCUICOMM: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_BFSCUICOMM].taskName, zHcuVmCtrTab.task[TASK_ID_L3BFSC].taskName);
		}
	else{

	}
	return SUCCESS;
}

//命令标志command.Json文件解析
OPSTAT  func_bfscuicomm_cmdfile_json_parse(char monitorJasonFile [], L3BfscuiJsonCmdParseResult_t parseResult )
{
	FILE *fileStream;
	char inotifyReadBuf[BUFSIZ];
	UINT32  numread = 0;
	UINT32  flag = 0, value = 0;
	UINT8  sensorid = 0;

	struct json_object *file_jsonobj = NULL;
	 struct json_object *cmd_jsonobj = NULL, *flag_jsonobj = NULL, *value_jsonobj = NULL, *sensorid_jsonobj = NULL;

    if ((fileStream = fopen( monitorJasonFile, "r" )) != NULL )  // 文件读取
    {
			numread = fread( inotifyReadBuf, sizeof( char ), BUFSIZ-1, fileStream );
			fclose( fileStream );
			if (numread == 0){
				HCU_ERROR_PRINT_BFSCUICOMM("BFSCUICOMM: Read NULL command json file, [file=%s]  ! \n", monitorJasonFile);
				return FAILURE;
			}

			file_jsonobj = json_tokener_parse(inotifyReadBuf);
			if (file_jsonobj == NULL){
				HCU_ERROR_PRINT_BFSCUICOMM("BFSCUICOMM: Command file json_tokener_parse failure, [file=%s]  ! \n", monitorJasonFile);
				json_object_put(file_jsonobj);  //释放Json Object指针
				return FAILURE;
			}

			//解析Start/Stop命令
		   if ( json_object_object_get_ex(file_jsonobj, "start_cmd", &cmd_jsonobj)){
			   flag_jsonobj = json_object_object_get(cmd_jsonobj, "flag");
			   value_jsonobj = json_object_object_get(cmd_jsonobj, "value");
			   if ((flag_jsonobj != NULL) && (value_jsonobj != NULL)){
				   flag = json_object_get_int(flag_jsonobj);
				   value = json_object_get_int(value_jsonobj);
				   parseResult.cmdStart.cmdFlag = flag;
				   parseResult.cmdStart.cmdValue = value;
			   }
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
				  parseResult.cmdCalibration.cmdFlag = flag;
				  parseResult.cmdCalibration.cmdValue = value;
				  parseResult.cmdCalibration.sensorid = sensorid;
			  }
		  }
		 else
			 HCU_ERROR_PRINT_BFSCUICOMM("BFSCUICOMM: Field [calibration_cmd] dose not exist in command json file ! \n");

		  //解析Config命令
		 if (json_object_object_get_ex(file_jsonobj, "config_cmd", &cmd_jsonobj)){
			 flag_jsonobj = json_object_object_get(cmd_jsonobj, "flag");
			 value_jsonobj = json_object_object_get(cmd_jsonobj, "value");
			 if ((flag_jsonobj != NULL) && (value_jsonobj != NULL)){
				 flag = json_object_get_int(flag_jsonobj);
				 value = json_object_get_int(value_jsonobj);
				 parseResult.cmdConfig.cmdFlag = flag;
				 parseResult.cmdConfig.cmdValue = value;
			 }
		 }
		 else
			 HCU_ERROR_PRINT_BFSCUICOMM("BFSCUICOMM: Field [config_cmd] dose not exist in command json file ! \n");

		 //解析Resume命令
		if (json_object_object_get_ex(file_jsonobj, "resume_cmd", &cmd_jsonobj)){
			flag_jsonobj = json_object_object_get(cmd_jsonobj, "flag");
			value_jsonobj = json_object_object_get(cmd_jsonobj, "value");
			if ((flag_jsonobj != NULL) && (value_jsonobj != NULL)){
				flag = json_object_get_int(flag_jsonobj);
				value = json_object_get_int(value_jsonobj);
				parseResult.cmdResume.cmdFlag = flag;
				parseResult.cmdResume.cmdValue = value;
			}
		}
		else
			HCU_ERROR_PRINT_BFSCUICOMM("BFSCUICOMM: Field [resume_cmd] dose not exist in command json file ! \n");

		 //解析Test命令
		if (json_object_object_get_ex(file_jsonobj, "test_cmd", &cmd_jsonobj)){
			flag_jsonobj = json_object_object_get(cmd_jsonobj, "flag");
			value_jsonobj = json_object_object_get(cmd_jsonobj, "value");
			if ((flag_jsonobj != NULL) && (value_jsonobj != NULL)){
				flag = json_object_get_int(flag_jsonobj);
				value = json_object_get_int(value_jsonobj);
				parseResult.cmdTest.cmdFlag = flag;
				parseResult.cmdTest.cmdValue = value;
			}
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
    	HCU_ERROR_PRINT_BFSCUICOMM("BFSCUICOMM: Open command json file failure, file=%s \n", monitorJasonFile);
        return FAILURE;
    }
}

//扫描文件是否有DEFAULT参数，并配置进入系统参数控制表
OPSTAT func_bfscuicomm_read_cfg_file_into_ctrl_table(void)
{

	//测试数据
	gTaskL3bfscContext.comAlgPar.MinScaleNumberStartCombination = 3;
	gTaskL3bfscContext.comAlgPar.MinScaleNumberCombination = 4;
	gTaskL3bfscContext.comAlgPar.MaxScaleNumberCombination = 10;
	gTaskL3bfscContext.comAlgPar.TargetCombinationWeight = 120000;
	gTaskL3bfscContext.comAlgPar.TargetCombinationUpperWeight = 100000;
	gTaskL3bfscContext.comAlgPar.IsPriorityScaleEnabled = 0;
	gTaskL3bfscContext.comAlgPar.IsProximitCombinationMode = 0;
	gTaskL3bfscContext.comAlgPar.CombinationBias = 1;
	gTaskL3bfscContext.comAlgPar.IsRemainDetectionEnable = 0;
	gTaskL3bfscContext.comAlgPar.IsRemainDetectionEnable = 0;
	gTaskL3bfscContext.comAlgPar.RemainDetectionTimeSec = 1;
	gTaskL3bfscContext.comAlgPar.RemainScaleTreatment = 1;
	gTaskL3bfscContext.comAlgPar.CombinationSpeedMode = 0;
	gTaskL3bfscContext.comAlgPar.CombinationAutoMode = 0;
	gTaskL3bfscContext.comAlgPar.MovingAvrageSpeedCount = 0;

	gTaskL3bfscContext.wgtSnrPar.WeightSensorLoadDetectionTimeMs = 1000;
	gTaskL3bfscContext.wgtSnrPar.WeightSensorLoadThread = 200;
	gTaskL3bfscContext.wgtSnrPar.WeightSensorEmptyThread = 200;
	gTaskL3bfscContext.wgtSnrPar.WeightSensorEmptyDetectionTimeMs = 1000;
	gTaskL3bfscContext.wgtSnrPar.WeightSensorPickupThread = 300;
	gTaskL3bfscContext.wgtSnrPar.WeightSensorPickupDetectionTimeMs = 500;
	gTaskL3bfscContext.wgtSnrPar.StardardReadyTimeMs = 300;
	gTaskL3bfscContext.wgtSnrPar.RemainDetectionTimeSec = 10;
	gTaskL3bfscContext.wgtSnrPar.MaxAllowedWeight = 1000000;
	gTaskL3bfscContext.wgtSnrPar.WeightSensorInitOrNot = 0;
	gTaskL3bfscContext.wgtSnrPar.WeightSensorAdcSampleFreq = 0;
	gTaskL3bfscContext.wgtSnrPar.WeightSensorAdcGain = 4;
	gTaskL3bfscContext.wgtSnrPar.WeightSensorAdcBitwidth = 22;
	gTaskL3bfscContext.wgtSnrPar.WeightSensorAdcValue = 0;
	gTaskL3bfscContext.wgtSnrPar.calibration[0].WeightSensorCalibrationZeroAdcValue = 100;
	gTaskL3bfscContext.wgtSnrPar.calibration[0].WeightSensorCalibrationFullAdcValue = 100000;
	gTaskL3bfscContext.wgtSnrPar.calibration[0].WeightSensorCalibrationFullWeight = 100000;
	gTaskL3bfscContext.wgtSnrPar.calibration[1].WeightSensorCalibrationZeroAdcValue = 100;
	gTaskL3bfscContext.wgtSnrPar.calibration[1].WeightSensorCalibrationFullAdcValue = 100000;
	gTaskL3bfscContext.wgtSnrPar.calibration[1].WeightSensorCalibrationFullWeight = 100000;
	gTaskL3bfscContext.wgtSnrPar.calibration[2].WeightSensorCalibrationZeroAdcValue = 100;
	gTaskL3bfscContext.wgtSnrPar.calibration[2].WeightSensorCalibrationFullAdcValue = 100000;
	gTaskL3bfscContext.wgtSnrPar.calibration[2].WeightSensorCalibrationFullWeight = 100000;
	gTaskL3bfscContext.wgtSnrPar.calibration[3].WeightSensorCalibrationZeroAdcValue = 100;
	gTaskL3bfscContext.wgtSnrPar.calibration[3].WeightSensorCalibrationFullAdcValue = 100000;
	gTaskL3bfscContext.wgtSnrPar.calibration[3].WeightSensorCalibrationFullWeight = 100000;
	gTaskL3bfscContext.wgtSnrPar.calibration[4].WeightSensorCalibrationZeroAdcValue = 100;
	gTaskL3bfscContext.wgtSnrPar.calibration[4].WeightSensorCalibrationFullAdcValue = 100000;
	gTaskL3bfscContext.wgtSnrPar.calibration[4].WeightSensorCalibrationFullWeight = 100000;
	gTaskL3bfscContext.wgtSnrPar.calibration[5].WeightSensorCalibrationZeroAdcValue = 100;
	gTaskL3bfscContext.wgtSnrPar.calibration[5].WeightSensorCalibrationFullAdcValue = 100000;
	gTaskL3bfscContext.wgtSnrPar.calibration[5].WeightSensorCalibrationFullWeight = 100000;
	gTaskL3bfscContext.wgtSnrPar.calibration[6].WeightSensorCalibrationZeroAdcValue = 100;
	gTaskL3bfscContext.wgtSnrPar.calibration[6].WeightSensorCalibrationFullAdcValue = 100000;
	gTaskL3bfscContext.wgtSnrPar.calibration[6].WeightSensorCalibrationFullWeight = 100000;
	gTaskL3bfscContext.wgtSnrPar.calibration[7].WeightSensorCalibrationZeroAdcValue = 100;
	gTaskL3bfscContext.wgtSnrPar.calibration[7].WeightSensorCalibrationFullAdcValue = 100000;
	gTaskL3bfscContext.wgtSnrPar.calibration[7].WeightSensorCalibrationFullWeight = 100000;
	gTaskL3bfscContext.wgtSnrPar.calibration[8].WeightSensorCalibrationZeroAdcValue = 100;
	gTaskL3bfscContext.wgtSnrPar.calibration[8].WeightSensorCalibrationFullAdcValue = 100000;
	gTaskL3bfscContext.wgtSnrPar.calibration[8].WeightSensorCalibrationFullWeight = 100000;
	gTaskL3bfscContext.wgtSnrPar.calibration[9].WeightSensorCalibrationZeroAdcValue = 100;
	gTaskL3bfscContext.wgtSnrPar.calibration[9].WeightSensorCalibrationFullAdcValue = 100000;
	gTaskL3bfscContext.wgtSnrPar.calibration[9].WeightSensorCalibrationFullWeight = 100000;
	gTaskL3bfscContext.wgtSnrPar.calibration[10].WeightSensorCalibrationZeroAdcValue = 100;
	gTaskL3bfscContext.wgtSnrPar.calibration[10].WeightSensorCalibrationFullAdcValue = 100000;
	gTaskL3bfscContext.wgtSnrPar.calibration[10].WeightSensorCalibrationFullWeight = 100000;
	gTaskL3bfscContext.wgtSnrPar.calibration[11].WeightSensorCalibrationZeroAdcValue = 100;
	gTaskL3bfscContext.wgtSnrPar.calibration[11].WeightSensorCalibrationFullAdcValue = 100000;
	gTaskL3bfscContext.wgtSnrPar.calibration[11].WeightSensorCalibrationFullWeight = 100000;
	gTaskL3bfscContext.wgtSnrPar.calibration[12].WeightSensorCalibrationZeroAdcValue = 100;
	gTaskL3bfscContext.wgtSnrPar.calibration[12].WeightSensorCalibrationFullAdcValue = 100000;
	gTaskL3bfscContext.wgtSnrPar.calibration[12].WeightSensorCalibrationFullWeight = 100000;
	gTaskL3bfscContext.wgtSnrPar.calibration[13].WeightSensorCalibrationZeroAdcValue = 100;
	gTaskL3bfscContext.wgtSnrPar.calibration[13].WeightSensorCalibrationFullAdcValue = 100000;
	gTaskL3bfscContext.wgtSnrPar.calibration[13].WeightSensorCalibrationFullWeight = 100000;
	gTaskL3bfscContext.wgtSnrPar.calibration[14].WeightSensorCalibrationZeroAdcValue = 100;
	gTaskL3bfscContext.wgtSnrPar.calibration[14].WeightSensorCalibrationFullAdcValue = 100000;
	gTaskL3bfscContext.wgtSnrPar.calibration[14].WeightSensorCalibrationFullWeight = 100000;
	gTaskL3bfscContext.wgtSnrPar.calibration[15].WeightSensorCalibrationZeroAdcValue = 100;
	gTaskL3bfscContext.wgtSnrPar.calibration[15].WeightSensorCalibrationFullAdcValue = 100000;
	gTaskL3bfscContext.wgtSnrPar.calibration[15].WeightSensorCalibrationFullWeight = 100000;
	gTaskL3bfscContext.wgtSnrPar.calibration[16].WeightSensorCalibrationZeroAdcValue = 100;
	gTaskL3bfscContext.wgtSnrPar.calibration[16].WeightSensorCalibrationFullAdcValue = 100000;
	gTaskL3bfscContext.wgtSnrPar.calibration[16].WeightSensorCalibrationFullWeight = 100000;
	gTaskL3bfscContext.wgtSnrPar.WeightSensorStaticZeroValue = 0;
	gTaskL3bfscContext.wgtSnrPar.WeightSensorTailorValue = 0;
	gTaskL3bfscContext.wgtSnrPar.WeightSensorDynamicZeroThreadValue = 0;
	gTaskL3bfscContext.wgtSnrPar.WeightSensorDynamicZeroHysteresisMs = 10000;
	gTaskL3bfscContext.wgtSnrPar.WeightSensorFilterCoeff[0] = 0;				// NOT for GUI
	gTaskL3bfscContext.wgtSnrPar.WeightSensorFilterCoeff[1] = 0;				// NOT for GUI
	gTaskL3bfscContext.wgtSnrPar.WeightSensorFilterCoeff[2] = 0;				// NOT for GUI
	gTaskL3bfscContext.wgtSnrPar.WeightSensorFilterCoeff[3] = 0;				// NOT for GUI
	gTaskL3bfscContext.wgtSnrPar.WeightSensorOutputValue[0] = 1;				// NOT for GUI
	gTaskL3bfscContext.wgtSnrPar.WeightSensorOutputValue[1] = 0;				// NOT for GUI
	gTaskL3bfscContext.wgtSnrPar.WeightSensorOutputValue[2] = 0;				// NOT for GUI
	gTaskL3bfscContext.wgtSnrPar.WeightSensorOutputValue[3] = 0;				// NOT for GUI

	gTaskL3bfscContext.motCtrPar.MotorSpeed = 0;
	gTaskL3bfscContext.motCtrPar.MotorDirection = 0;
	gTaskL3bfscContext.motCtrPar.MotorRollingStartMs = 0;
	gTaskL3bfscContext.motCtrPar.MotorRollingStopMs = 0;
	gTaskL3bfscContext.motCtrPar.MotorRollingInveralMs = 0;
	gTaskL3bfscContext.motCtrPar.MotorFailureDetectionVaration = 0;
	gTaskL3bfscContext.motCtrPar.MotorFailureDetectionTimeMs = 0;

	return SUCCESS;
}
