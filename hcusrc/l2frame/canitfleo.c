/*
 * canitfleo.c
 *
 *  Created on: 2016年5月14日
 *      Author: hitpony
 */

#include "canitfleo.h"

#include "../l0service/timer.h"
#include "../l0service/trace.h"
#include "../l1com/l1comdef.h"
#include "../l2frame/l2usbcan.h"   //Added by MYC 2017/05/15
#include "../l2frame/l2packet.h"   //Added by MYC 2017/05/15
#include "../l0comvm/commsg.h"   //Added by MYC 2017/05/15
#include "../l2codec/huitp.h"   //Added by MYC 2017/05/30
#include "../l2codec/huixmlcodec.h"

/*
** FSM of the CANITFLEO
*/
HcuFsmStateItem_t HcuFsmCanitfleo[] =
{
    //MessageId                 //State                   		 		//Function
	//启始点，固定定义，不要改动, 使用ENTRY/END，意味者MSGID肯定不可能在某个高位区段中；考虑到所有任务共享MsgId，即使分段，也无法实现
	//完全是为了给任务一个初始化的机会，按照状态转移机制，该函数不具备启动的机会，因为任务初始化后自动到FSM_STATE_IDLE
	//如果没有必要进行初始化，可以设置为NULL
	{MSG_ID_ENTRY,       					FSM_STATE_ENTRY,            				fsm_canitfleo_task_entry}, //Starting

	//System level initialization, only controlled by HCU-MAIN
    {MSG_ID_COM_INIT,       				FSM_STATE_IDLE,            					fsm_canitfleo_init},
    {MSG_ID_COM_INIT_FEEDBACK,				FSM_STATE_IDLE,            					fsm_com_do_nothing},

	//Task level initialization
    {MSG_ID_COM_INIT,       				FSM_STATE_CANITFLEO_INITED,            		fsm_canitfleo_init},
    {MSG_ID_COM_INIT_FEEDBACK,				FSM_STATE_CANITFLEO_INITED,            		fsm_com_do_nothing},

	//ANY state entry
    {MSG_ID_COM_INIT_FEEDBACK,				FSM_STATE_COMMON,          				fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT,       			FSM_STATE_COMMON,          				fsm_com_heart_beat_rcv},
	{MSG_ID_COM_STOP,       				FSM_STATE_COMMON,          				fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT_FB,       		FSM_STATE_COMMON,          				fsm_com_do_nothing},
    {MSG_ID_COM_RESTART,					FSM_STATE_COMMON,            			fsm_canitfleo_restart},
	{MSG_ID_COM_TIME_OUT,       			FSM_STATE_COMMON,          				fsm_canitfleo_timeout},

    //Normal task status
#if (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFSC_CBU_ID)

	//{MSG_ID_L3BFSC_CAN_GENERAL_CMD_REQ,     FSM_STATE_CANITFLEO_ACTIVED,          	fsm_canitfleo_l3bfsc_general_cmd_req}, //后台的指令

	{MSG_ID_L3BFSC_CAN_SYS_CFG_REQ,      	FSM_STATE_CANITFLEO_ACTIVED,          		fsm_canitfleo_l3bfsc_sys_cfg_req},  	//初始化配置
	{MSG_ID_L3BFSC_CAN_SYS_START_REQ,      	FSM_STATE_CANITFLEO_ACTIVED,          		fsm_canitfleo_can_sys_start_req},   	//人工命令
	{MSG_ID_L3BFSC_CAN_SYS_STOP_REQ,      	FSM_STATE_CANITFLEO_ACTIVED,          		fsm_canitfleo_can_sys_stop_req},   		//人工命令
	{MSG_ID_UICOMM_CAN_TEST_CMD_REQ,      	FSM_STATE_CANITFLEO_ACTIVED,          		fsm_canitfleo_can_test_cmd_req},   		//测试性命令

	{MSG_ID_L3BFSC_CAN_WS_COMB_OUT,      	FSM_STATE_CANITFLEO_ACTIVED,          		fsm_canitfleo_l3bfsc_ws_comb_out},  	//组合指令
	{MSG_ID_L3BFSC_CAN_WS_GIVE_UP,      	FSM_STATE_CANITFLEO_ACTIVED,          		fsm_canitfleo_l3bfsc_ws_give_up},   	//剔除指令
	{MSG_ID_L3BFSC_CAN_ERROR_INQ_CMD_REQ,   FSM_STATE_CANITFLEO_ACTIVED,          		fsm_canitfleo_l3bfsc_error_inq_cmd_req},//差错状态下的发送指令

	{MSG_ID_USBCAN_L2FRAME_RCV,      		FSM_STATE_CANITFLEO_ACTIVED,          		fsm_canitfleo_usbcan_l2frame_receive},   //收到L2送过来的帧

#endif
	//结束点，固定定义，不要改动
    {MSG_ID_END,            	FSM_STATE_END,             				NULL},  //Ending
};

//Global variables
#if (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFSC_CBU_ID)
	#include "../l3appctrl/l3bfsc.h"
	extern gTaskL3bfscContext_t gTaskL3bfscContext;
#endif

//Task Global variables
gTaskCanitfleoContext_t gTaskCanitfleoContext;

//Main Entry
//Input parameter would be useless, but just for similar structure purpose
OPSTAT fsm_canitfleo_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//除了对全局变量进行操作之外，尽量不要做其它操作，因为该函数将被主任务/线程调用，不是本任务/线程调用
	//该API就是给本任务一个提早介入的入口，可以帮着做些测试性操作
	if (FsmSetState(TASK_ID_CANITFLEO, FSM_STATE_IDLE) == FAILURE){
		HcuErrorPrint("CANITFLEO: Error Set FSM State at fsm_canitfleo_task_entry\n");}
	return SUCCESS;
}

//纯粹初始化
OPSTAT fsm_canitfleo_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;

	if ((src_id > TASK_ID_MIN) &&(src_id < TASK_ID_MAX)){
		//Send back MSG_ID_COM_INIT_FEEDBACK to SVRCON
		msg_struct_com_init_feedback_t snd0;
		memset(&snd0, 0, sizeof(msg_struct_com_init_feedback_t));
		snd0.length = sizeof(msg_struct_com_init_feedback_t);

		//to avoid all task send out the init fb msg at the same time which lead to msgque get stuck
		hcu_usleep(dest_id*HCU_SYSCFG_DURATION_OF_INIT_FB_WAIT_MAX);

		ret = hcu_message_send(MSG_ID_COM_INIT_FEEDBACK, src_id, TASK_ID_CANITFLEO, &snd0, snd0.length);
		if (ret == FAILURE){
			HcuErrorPrint("CANITFLEO: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName, zHcuVmCtrTab.task[src_id].taskName);
			return FAILURE;
		}
	}

	//收到初始化消息后，进入初始化状态
	if (FsmSetState(TASK_ID_CANITFLEO, FSM_STATE_CANITFLEO_INITED) == FAILURE){
		HcuErrorPrint("CANITFLEO: Error Set FSM State!\n");
		return FAILURE;
	}

	//Global Variables
	zHcuSysStaPm.taskRunErrCnt[TASK_ID_CANITFLEO] = 0;
	memset(&gTaskCanitfleoContext, 0, sizeof(gTaskCanitfleoContext_t));

	//初始化硬件接口
	if (func_canitfleo_int_init() == FAILURE){
		HcuErrorPrint("CANITFLEO: Error initialize interface!\n");
		return FAILURE;
	}

	//启动定时器：放在初始化完成之后再启动，仅仅是为了测试目的
	ret = hcu_timer_start(TASK_ID_CANITFLEO, TIMER_ID_1S_CANITFLEO_WORKING_SCAN, zHcuSysEngPar.timer.array[TIMER_ID_1S_CANITFLEO_WORKING_SCAN].dur,\
			TIMER_TYPE_PERIOD, TIMER_RESOLUTION_1S);
	if (ret == FAILURE) HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Error start timer!\n");

	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_CANITFLEO, FSM_STATE_CANITFLEO_ACTIVED) == FAILURE)
		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Error Set FSM State!\n");

	HCU_DEBUG_PRINT_FAT("CANITFLEO: Enter FSM_STATE_CANITFLEO_ACTIVED status, Keeping refresh here!\n");

	return SUCCESS;
}

//RESTART重启动
OPSTAT fsm_canitfleo_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HcuErrorPrint("CANITFLEO: Internal error counter reach DEAD level, SW-RESTART soon!\n");
	zHcuSysStaPm.statisCnt.restartCnt++;
	fsm_canitfleo_init(0, 0, NULL, 0);
	return SUCCESS;
}

//本模块初始化的机会
OPSTAT func_canitfleo_int_init(void)
{
#ifdef TARGET_LINUX_X86_ADVANTECH //Added by MYC 2017/05/15
	INT32 ret;
	ret = hcu_canitfleo_usbcan_interface_init();
	HCU_DEBUG_PRINT_INF("CANITFLEO: hcu_canitfleo_usbcan_interface_init() called, ret = %d\n", ret);

	if(SUCCESS == ret)
		return SUCCESS;
	else
		return FAILURE;
#endif
		return SUCCESS;
}

//暂时啥都不干，定时到达后，还不明确是否需要支持定时工作
//业务逻辑还不明确
//已经启动，用于产生假的传感器称重数据，用于参数L3BFSC组合算法
OPSTAT fsm_canitfleo_timeout(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret;

	//Receive message and copy to local variable
	msg_struct_com_time_out_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_com_time_out_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_com_time_out_t)))
		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);

	//钩子在此处，检查zHcuRunErrCnt[TASK_ID_CANITFLEO]是否超限
	if (zHcuSysStaPm.taskRunErrCnt[TASK_ID_CANITFLEO] > HCU_RUN_ERROR_LEVEL_2_MAJOR){
		//减少重复RESTART的概率
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CANITFLEO] = zHcuSysStaPm.taskRunErrCnt[TASK_ID_CANITFLEO] - HCU_RUN_ERROR_LEVEL_2_MAJOR;
		msg_struct_com_restart_t snd0;
		memset(&snd0, 0, sizeof(msg_struct_com_restart_t));
		snd0.length = sizeof(msg_struct_com_restart_t);
		if (hcu_message_send(MSG_ID_COM_RESTART, TASK_ID_CANITFLEO, TASK_ID_CANITFLEO, &snd0, snd0.length) == FAILURE)
			HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName, zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName);
	}

	//Period time out received
	if ((rcv.timeId == TIMER_ID_1S_CANITFLEO_WORKING_SCAN) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		//保护周期读数的优先级，强制抢占状态，并简化问题
		if (FsmGetState(TASK_ID_CANITFLEO) != FSM_STATE_CANITFLEO_ACTIVED){
			if (FsmSetState(TASK_ID_CANITFLEO, FSM_STATE_CANITFLEO_ACTIVED) == FAILURE) HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Error Set FSM State!\n");
		}
#if (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFSC_CBU_ID)
		func_canitfleo_working_scan_process();
#endif
	}

#if (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFSC_CBU_ID)
	//测试目的，正式程序中可以去掉
	else if ((rcv.timeId == TIMER_ID_10MS_CANITFLEO_SIMULATION_DATA) &&(rcv.timeRes == TIMER_RESOLUTION_10MS)){
		//func_canitfleo_bfsc_simulation_data_process();
	}
#endif
	return SUCCESS;
}

//以下均为BFSC项目所特有的
#if (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFSC_CBU_ID)
//整个传感器系统的初始化
OPSTAT fsm_canitfleo_l3bfsc_sys_cfg_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret=0;
	int i=0;

	//入参检查
	msg_struct_l3bfsc_can_sys_cfg_req_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_l3bfsc_can_sys_cfg_req_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_l3bfsc_can_sys_cfg_req_t)) || (HCU_SYSCFG_BFSC_SNR_WS_NBR_MAX > 32))
		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);

	//生成bitmap
	UINT32 bitmap = 0;
	for (i=0; i<HCU_SYSCFG_BFSC_SNR_WS_NBR_MAX; i++){
		if (rcv.wsBitmap[i] == TRUE){
			bitmap = ((UINT32)1<<i);

			//准备组装发送消息
			StrMsg_HUITP_MSGID_sui_bfsc_set_config_req_t pMsgProc;
			UINT16 msgProcLen = sizeof(StrMsg_HUITP_MSGID_sui_bfsc_set_config_req_t);
			memset(&pMsgProc, 0, msgProcLen);
			pMsgProc.msgid = HUITP_ENDIAN_EXG16(MSG_ID_L3BFSC_WMC_SET_CONFIG_REQ);
			pMsgProc.length = HUITP_ENDIAN_EXG16(msgProcLen - 4);
			pMsgProc.weight_sensor_param.WeightSensorLoadDetectionTimeMs = HUITP_ENDIAN_EXG32(gTaskL3bfscContext.wgtSnrPar.WeightSensorLoadDetectionTimeMs);
			pMsgProc.weight_sensor_param.WeightSensorLoadThread = HUITP_ENDIAN_EXG32(gTaskL3bfscContext.wgtSnrPar.WeightSensorLoadThread);
			pMsgProc.weight_sensor_param.WeightSensorEmptyThread = HUITP_ENDIAN_EXG32(gTaskL3bfscContext.wgtSnrPar.WeightSensorEmptyThread);
			pMsgProc.weight_sensor_param.WeightSensorEmptyDetectionTimeMs = HUITP_ENDIAN_EXG32(gTaskL3bfscContext.wgtSnrPar.WeightSensorEmptyDetectionTimeMs);
			pMsgProc.weight_sensor_param.WeightSensorPickupThread = HUITP_ENDIAN_EXG32(gTaskL3bfscContext.wgtSnrPar.WeightSensorPickupThread);
			pMsgProc.weight_sensor_param.WeightSensorPickupDetectionTimeMs = HUITP_ENDIAN_EXG32(gTaskL3bfscContext.wgtSnrPar.WeightSensorPickupDetectionTimeMs);
			pMsgProc.weight_sensor_param.StardardReadyTimeMs = HUITP_ENDIAN_EXG32(gTaskL3bfscContext.wgtSnrPar.StardardReadyTimeMs);
			pMsgProc.weight_sensor_param.MaxAllowedWeight = HUITP_ENDIAN_EXG32(gTaskL3bfscContext.wgtSnrPar.MaxAllowedWeight);
			pMsgProc.weight_sensor_param.WeightSensorInitOrNot = HUITP_ENDIAN_EXG32(gTaskL3bfscContext.wgtSnrPar.WeightSensorInitOrNot);
			pMsgProc.weight_sensor_param.WeightSensorAdcSampleFreq = HUITP_ENDIAN_EXG32(gTaskL3bfscContext.wgtSnrPar.WeightSensorAdcSampleFreq);
			pMsgProc.weight_sensor_param.WeightSensorAdcGain = HUITP_ENDIAN_EXG32(gTaskL3bfscContext.wgtSnrPar.WeightSensorAdcGain);
			pMsgProc.weight_sensor_param.WeightSensorAdcBitwidth = HUITP_ENDIAN_EXG32(gTaskL3bfscContext.wgtSnrPar.WeightSensorAdcBitwidth);
			pMsgProc.weight_sensor_param.WeightSensorAdcValue = HUITP_ENDIAN_EXG32(gTaskL3bfscContext.wgtSnrPar.WeightSensorAdcValue);
			pMsgProc.weight_sensor_param.WeightSensorCalibrationZeroAdcValue = HUITP_ENDIAN_EXG32(gTaskL3bfscContext.wgtSnrPar.calibration[i].WeightSensorCalibrationZeroAdcValue);
			pMsgProc.weight_sensor_param.WeightSensorCalibrationFullAdcValue = HUITP_ENDIAN_EXG32(gTaskL3bfscContext.wgtSnrPar.calibration[i].WeightSensorCalibrationFullAdcValue);
			pMsgProc.weight_sensor_param.WeightSensorCalibrationFullWeight = HUITP_ENDIAN_EXG32(gTaskL3bfscContext.wgtSnrPar.calibration[i].WeightSensorCalibrationFullWeight);
			pMsgProc.weight_sensor_param.WeightSensorStaticZeroValue = HUITP_ENDIAN_EXG32(gTaskL3bfscContext.wgtSnrPar.WeightSensorStaticZeroValue);
			pMsgProc.weight_sensor_param.WeightSensorTailorValue = HUITP_ENDIAN_EXG32(gTaskL3bfscContext.wgtSnrPar.WeightSensorTailorValue);
			pMsgProc.weight_sensor_param.WeightSensorDynamicZeroThreadValue = HUITP_ENDIAN_EXG32(gTaskL3bfscContext.wgtSnrPar.WeightSensorDynamicZeroThreadValue);
			pMsgProc.weight_sensor_param.WeightSensorDynamicZeroHysteresisMs = HUITP_ENDIAN_EXG32(gTaskL3bfscContext.wgtSnrPar.WeightSensorDynamicZeroHysteresisMs);
			pMsgProc.motor_control_param.MotorSpeed = HUITP_ENDIAN_EXG32(gTaskL3bfscContext.motCtrPar.MotorSpeed);
			pMsgProc.motor_control_param.MotorDirection = HUITP_ENDIAN_EXG32(gTaskL3bfscContext.motCtrPar.MotorDirection);
			pMsgProc.motor_control_param.MotorRollingStartMs = HUITP_ENDIAN_EXG32(gTaskL3bfscContext.motCtrPar.MotorRollingStartMs);
			pMsgProc.motor_control_param.MotorRollingStopMs = HUITP_ENDIAN_EXG32(gTaskL3bfscContext.motCtrPar.MotorRollingStopMs);
			pMsgProc.motor_control_param.MotorRollingInveralMs = HUITP_ENDIAN_EXG32(gTaskL3bfscContext.motCtrPar.MotorRollingInveralMs);
			pMsgProc.motor_control_param.MotorFailureDetectionVaration = HUITP_ENDIAN_EXG32(gTaskL3bfscContext.motCtrPar.MotorFailureDetectionVaration);
			pMsgProc.motor_control_param.MotorFailureDetectionTimeMs = HUITP_ENDIAN_EXG32(gTaskL3bfscContext.motCtrPar.MotorFailureDetectionTimeMs);

			//发送消息：配置消息分成多个分别发送，因为校准参数对于每一个下位机不一样
			if (hcu_canitfleo_usbcan_l2frame_send((UINT8*)&pMsgProc, msgProcLen, bitmap) == FAILURE)
				HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Send CAN frame error!\n");
		}
	}


	//返回
	return SUCCESS;
}

//后台的控制指令：启动/停止组合秤
OPSTAT fsm_canitfleo_can_sys_start_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret=0;
	int i=0;

	msg_struct_l3bfsc_can_sys_start_req_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_l3bfsc_can_sys_start_req_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_l3bfsc_can_sys_start_req_t)))
		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);

	//生成bitmap
	UINT32 bitmap = 0;
	UINT32 temp =0;
	for (i=0; i<HCU_SYSCFG_BFSC_SNR_WS_NBR_MAX; i++){
		if (rcv.wsBitmap[i] == TRUE){
			temp = ((UINT32)1<<i);
			bitmap |= temp;
		}
	}

	//准备组装发送消息
	StrMsg_HUITP_MSGID_sui_bfsc_start_req_t pMsgProc;
	UINT16 msgProcLen = sizeof(StrMsg_HUITP_MSGID_sui_bfsc_start_req_t);
	memset(&pMsgProc, 0, msgProcLen);
	pMsgProc.msgid = HUITP_ENDIAN_EXG16(HUITP_MSGID_sui_bfsc_start_req);
	pMsgProc.length = HUITP_ENDIAN_EXG16(msgProcLen - 4);

	//发送消息
	if (hcu_canitfleo_usbcan_l2frame_send((UINT8*)&pMsgProc, msgProcLen, bitmap) == FAILURE)
		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Send CAN frame error!\n");

	//返回
	return SUCCESS;
}

//后台的控制指令：启动/停止组合秤
OPSTAT fsm_canitfleo_can_sys_stop_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret=0;
	int i=0;

	msg_struct_l3bfsc_can_sys_stop_req_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_l3bfsc_can_sys_stop_req_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_l3bfsc_can_sys_stop_req_t)))
		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);

	//生成bitmap
	UINT32 bitmap = 0;
	UINT32 temp =0;
	for (i=0; i<HCU_SYSCFG_BFSC_SNR_WS_NBR_MAX; i++){
		if (rcv.wsBitmap[i] == TRUE){
			temp = ((UINT32)1<<i);
			bitmap |= temp;
		}
	}

	//准备组装发送消息
	StrMsg_HUITP_MSGID_sui_bfsc_stop_req_t pMsgProc;
	UINT16 msgProcLen = sizeof(StrMsg_HUITP_MSGID_sui_bfsc_stop_req_t);
	memset(&pMsgProc, 0, msgProcLen);
	pMsgProc.msgid = HUITP_ENDIAN_EXG16(HUITP_MSGID_sui_bfsc_stop_req);
	pMsgProc.length = HUITP_ENDIAN_EXG16(msgProcLen - 4);

	//发送消息
	if (hcu_canitfleo_usbcan_l2frame_send((UINT8*)&pMsgProc, msgProcLen, bitmap) == FAILURE)
		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Send CAN frame error!\n");

	//返回
	return SUCCESS;
}

//收到人工测试命令
OPSTAT fsm_canitfleo_can_test_cmd_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret=0;
	int i=0;

	msg_struct_uicomm_can_test_cmd_req_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_uicomm_can_test_cmd_req_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_uicomm_can_test_cmd_req_t)))
		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);

	//生成bitmap
	UINT32 bitmap = 0;
	UINT32 temp =0;
	for (i=0; i<HCU_SYSCFG_BFSC_SNR_WS_NBR_MAX; i++){
		if (rcv.wsBitmap[i] == TRUE){
			temp = ((UINT32)1<<i);
			bitmap |= temp;
		}
	}

	//准备组装发送消息
	StrMsg_HUITP_MSGID_sui_bfsc_command_req_t pMsgProc;
	UINT16 msgProcLen = sizeof(StrMsg_HUITP_MSGID_sui_bfsc_command_req_t);
	memset(&pMsgProc, 0, msgProcLen);
	pMsgProc.msgid = HUITP_ENDIAN_EXG16(HUITP_MSGID_sui_bfsc_command_req);
	pMsgProc.length = HUITP_ENDIAN_EXG16(msgProcLen - 4);

	pMsgProc.comand_flags = HUITP_ENDIAN_EXG32(rcv.comand_flags);
	pMsgProc.led1_command = rcv.led1_command;
	pMsgProc.led2_command = rcv.led2_command;
	pMsgProc.led3_command = rcv.led3_command;
	pMsgProc.led4_command = rcv.led4_command;
	pMsgProc.motor_command = HUITP_ENDIAN_EXG32(rcv.motor_command);
	pMsgProc.sensor_command = HUITP_ENDIAN_EXG32(rcv.sensor_command);

	//发送消息
	if (hcu_canitfleo_usbcan_l2frame_send((UINT8*)&pMsgProc, msgProcLen, bitmap) == FAILURE)
		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Send CAN frame error!\n");

	//返回
	return SUCCESS;
}


//组合算法的输出结果
OPSTAT fsm_canitfleo_l3bfsc_ws_comb_out(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret=0;
	int i=0;

	msg_struct_l3bfsc_can_ws_comb_out_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_l3bfsc_can_ws_comb_out_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_l3bfsc_can_ws_comb_out_t)))
		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);

	//生成bitmap
	UINT32 bitmap = 0;
	UINT32 temp =0;
	for (i=0; i<HCU_SYSCFG_BFSC_SNR_WS_NBR_MAX; i++){
		if (rcv.sensorBitmap[i] == TRUE){
			temp = ((UINT32)1<<i);
			bitmap |= temp;
		}
	}

	//准备组装发送消息
	StrMsg_HUITP_MSGID_sui_bfsc_ws_comb_out_req_t pMsgProc;
	UINT16 msgProcLen = sizeof(StrMsg_HUITP_MSGID_sui_bfsc_ws_comb_out_req_t);
	memset(&pMsgProc, 0, msgProcLen);
	pMsgProc.msgid = HUITP_ENDIAN_EXG16(HUITP_MSGID_sui_bfsc_ws_comb_out_req);
	pMsgProc.length = HUITP_ENDIAN_EXG16(msgProcLen - 4);
	pMsgProc.weight_combin_type.WeightCombineType = HUITP_ENDIAN_EXG32(HUITP_IEID_SUI_BFSC_COMINETYPE_ROOLOUT);

	//发送消息
	if (hcu_canitfleo_usbcan_l2frame_send((UINT8*)&pMsgProc, msgProcLen, bitmap) == FAILURE)
		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Send CAN frame error!\n");

	//返回
	return SUCCESS;
}

//剔除算法
OPSTAT fsm_canitfleo_l3bfsc_ws_give_up(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret=0;
	int i=0;

	msg_struct_l3bfsc_can_ws_give_up_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_l3bfsc_can_ws_give_up_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_l3bfsc_can_ws_give_up_t)))
		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);

	//生成bitmap
	UINT32 bitmap = 0;
	UINT32 temp =0;
	for (i=0; i<HCU_SYSCFG_BFSC_SNR_WS_NBR_MAX; i++){
		if (rcv.sensorBitmap[i] == TRUE){
			temp = ((UINT32)1<<i);
			bitmap |= temp;
		}
	}

	//准备组装发送消息
	StrMsg_HUITP_MSGID_sui_bfsc_ws_comb_out_req_t pMsgProc;
	UINT16 msgProcLen = sizeof(StrMsg_HUITP_MSGID_sui_bfsc_ws_comb_out_req_t);
	memset(&pMsgProc, 0, msgProcLen);
	pMsgProc.msgid = HUITP_ENDIAN_EXG16(HUITP_MSGID_sui_bfsc_ws_comb_out_req);
	pMsgProc.length = HUITP_ENDIAN_EXG16(msgProcLen - 4);
	pMsgProc.weight_combin_type.WeightCombineType = HUITP_ENDIAN_EXG32(HUITP_IEID_SUI_BFSC_COMINETYPE_DROP);

	//发送消息
	if (hcu_canitfleo_usbcan_l2frame_send((UINT8*)&pMsgProc, msgProcLen, bitmap) == FAILURE)
		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Send CAN frame error!\n");

	//返回
	return SUCCESS;
}

//这本来就是L3BFSC专用的消息及处理函数，由于简化系统设计，消息的统一化以及VMTASK的统一化，造成所有这些函数必须在消息路由表中，必须在各个项目中都可以出现
//这必然造成了处理的具体内容需要根据不同项目进行分类处理
OPSTAT fsm_canitfleo_l3bfsc_error_inq_cmd_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret=0;
	int i=0;

	msg_struct_l3bfsc_can_error_inq_cmd_req_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_l3bfsc_can_error_inq_cmd_req_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_l3bfsc_can_error_inq_cmd_req_t)))
		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);

	//生成bitmap
	UINT32 bitmap = 0;
	UINT32 temp =0;
	for (i=0; i<HCU_SYSCFG_BFSC_SNR_WS_NBR_MAX; i++){
		if (rcv.sensorBitmap[i] == TRUE){
			temp = ((UINT32)1<<i);
			bitmap |= temp;
		}
	}

	//准备组装发送消息
	StrMsg_HUITP_MSGID_sui_bfsc_err_inq_cmd_req_t pMsgProc;
	UINT16 msgProcLen = sizeof(StrMsg_HUITP_MSGID_sui_bfsc_err_inq_cmd_req_t);
	memset(&pMsgProc, 0, msgProcLen);
	pMsgProc.msgid = HUITP_ENDIAN_EXG16(HUITP_MSGID_sui_bfsc_err_inq_cmd_req);
	pMsgProc.length = HUITP_ENDIAN_EXG16(msgProcLen - 4);
	pMsgProc.wmc_id.wmc_id = rcv.sensorid;
	pMsgProc.error_code = HUITP_ENDIAN_EXG16(rcv.errCode);

	//发送消息
	if (hcu_canitfleo_usbcan_l2frame_send((UINT8*)&pMsgProc, msgProcLen, bitmap) == FAILURE)
		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Send CAN frame error!\n");

	//返回
	return SUCCESS;
}

//收到底层驱动USBCAN送过来的数据帧
OPSTAT fsm_canitfleo_usbcan_l2frame_receive(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;

	msg_struct_bfsc_usbcan_l2frame_rcv_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_bfsc_usbcan_l2frame_rcv_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_bfsc_usbcan_l2frame_rcv_t)))
		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);
	if ((rcv.nodeId >= HCU_SYSCFG_BFSC_SNR_WS_NBR_MAX) || (rcv.validDataLen > HCU_SYSMSG_BFSC_USBCAN_MAX_RX_BUF_SIZE))
		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Receive message error!\n");

	//解码MSGID/MSGLEN
	UINT16 msgId = 0, msgLen = 0;
	StrMsg_HUITP_MSGID_sui_bfsc_wmc_msg_header_t *pBfscMsg = (StrMsg_HUITP_MSGID_sui_bfsc_wmc_msg_header_t *)(rcv.databuf);
	msgId = HUITP_ENDIAN_EXG16(pBfscMsg->msgid);
	msgLen = HUITP_ENDIAN_EXG16(pBfscMsg->length);
	if (msgLen != (rcv.validDataLen-4))
		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Decode message error on length!\n");

	//按照消息类型进行分类处理
	switch(msgId){

	case HUITP_MSGID_sui_bfsc_startup_ind:
	{
		HCU_DEBUG_PRINT_CRT("CANITFLEO: Receive L3 MSG = HUITP_MSGID_sui_bfsc_startup_ind \n");
		StrMsg_HUITP_MSGID_sui_bfsc_startup_ind_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_sui_bfsc_startup_ind_t) - 4))
			HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_sui_bfsc_startup_ind_t*)(rcv.databuf);
		ret = func_canitfleo_l2frame_msg_bfsc_startup_ind_received_handle(snd, rcv.nodeId);
	}
	break;

	case HUITP_MSGID_sui_bfsc_set_config_resp:
	{
		HCU_DEBUG_PRINT_CRT("CANITFLEO: Receive L3 MSG = HUITP_MSGID_sui_bfsc_set_config_resp \n");
		StrMsg_HUITP_MSGID_sui_bfsc_set_config_resp_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_sui_bfsc_set_config_resp_t) - 4))
			HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_sui_bfsc_set_config_resp_t*)(rcv.databuf);
		ret = func_canitfleo_l2frame_msg_bfsc_set_config_resp_received_handle(snd, rcv.nodeId);
	}
	break;

	case HUITP_MSGID_sui_bfsc_start_resp:
	{
		HCU_DEBUG_PRINT_CRT("CANITFLEO: Receive L3 MSG = HUITP_MSGID_sui_bfsc_start_resp \n");
		StrMsg_HUITP_MSGID_sui_bfsc_start_resp_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_sui_bfsc_start_resp_t) - 4))
			HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_sui_bfsc_start_resp_t*)(rcv.databuf);
		ret = func_canitfleo_l2frame_msg_bfsc_start_resp_received_handle(snd, rcv.nodeId);
	}
	break;

	case HUITP_MSGID_sui_bfsc_stop_resp:
	{
		HCU_DEBUG_PRINT_CRT("CANITFLEO: Receive L3 MSG = HUITP_MSGID_sui_bfsc_stop_resp \n");
		StrMsg_HUITP_MSGID_sui_bfsc_stop_resp_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_sui_bfsc_stop_resp_t) - 4))
			HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_sui_bfsc_stop_resp_t*)(rcv.databuf);
		ret = func_canitfleo_l2frame_msg_bfsc_stop_resp_received_handle(snd, rcv.nodeId);
	}
	break;

	case HUITP_MSGID_sui_bfsc_new_ws_event:
	{
		HCU_DEBUG_PRINT_CRT("CANITFLEO: Receive L3 MSG = HUITP_MSGID_sui_bfsc_new_ws_event \n");
		StrMsg_HUITP_MSGID_sui_bfsc_new_ws_event_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_sui_bfsc_new_ws_event_t) - 4))
			HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_sui_bfsc_new_ws_event_t*)(rcv.databuf);
		ret = func_canitfleo_l2frame_msg_bfsc_new_ws_event_received_handle(snd, rcv.nodeId);
	}
	break;

	case HUITP_MSGID_sui_bfsc_repeat_ws_event:
	{
		HCU_DEBUG_PRINT_CRT("CANITFLEO: Receive L3 MSG = HUITP_MSGID_sui_bfsc_repeat_ws_event \n");
		StrMsg_HUITP_MSGID_sui_bfsc_repeat_ws_event_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_sui_bfsc_repeat_ws_event_t) - 4))
			HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_sui_bfsc_repeat_ws_event_t*)(rcv.databuf);
		ret = func_canitfleo_l2frame_msg_bfsc_repeat_ws_received_handle(snd, rcv.nodeId);
	}
	break;

	case HUITP_MSGID_sui_bfsc_ws_comb_out_resp:
	{
		HCU_DEBUG_PRINT_CRT("CANITFLEO: Receive L3 MSG = HUITP_MSGID_sui_bfsc_ws_comb_out_resp \n");
		StrMsg_HUITP_MSGID_sui_bfsc_ws_comb_out_resp_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_sui_bfsc_ws_comb_out_resp_t) - 4))
			HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_sui_bfsc_ws_comb_out_resp_t*)(rcv.databuf);
		ret = func_canitfleo_l2frame_msg_bfsc_ws_comb_out_received_handle(snd, rcv.nodeId);
	}
	break;

	case HUITP_MSGID_sui_bfsc_command_resp:
	{
		HCU_DEBUG_PRINT_CRT("CANITFLEO: Receive L3 MSG = HUITP_MSGID_sui_bfsc_command_resp \n");
		StrMsg_HUITP_MSGID_sui_bfsc_command_resp_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_sui_bfsc_command_resp_t) - 4))
			HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_sui_bfsc_command_resp_t*)(rcv.databuf);
		ret = func_canitfleo_l2frame_msg_bfsc_command_resp_received_handle(snd, rcv.nodeId);
	}
	break;

	case HUITP_MSGID_sui_bfsc_fault_ind:
	{
		HCU_DEBUG_PRINT_CRT("CANITFLEO: Receive L3 MSG = HUITP_MSGID_sui_bfsc_fault_ind \n");
		StrMsg_HUITP_MSGID_sui_bfsc_fault_ind_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_sui_bfsc_fault_ind_t) - 4))
			HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_sui_bfsc_fault_ind_t*)(rcv.databuf);
		ret = func_canitfleo_l2frame_msg_bfsc_fault_ind_received_handle(snd, rcv.nodeId);
	}
	break;

	case HUITP_MSGID_sui_bfsc_err_inq_cmd_resp:
	{
		HCU_DEBUG_PRINT_CRT("CANITFLEO: Receive L3 MSG = HUITP_MSGID_sui_bfsc_err_inq_cmd_resp \n");
		StrMsg_HUITP_MSGID_sui_bfsc_err_inq_cmd_resp_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_sui_bfsc_err_inq_cmd_resp_t) - 4))
			HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_sui_bfsc_err_inq_cmd_resp_t*)(rcv.databuf);
		ret = func_canitfleo_l2frame_msg_bfsc_err_ind_cmd_resp_received_handle(snd, rcv.nodeId);
	}
	break;

	default:
		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Receive unsupported message!\n");
	break;
	}

	//返回
	return ret;
}

//发送测试数据给L3BFSC
OPSTAT func_canitfleo_bfsc_simulation_data_process(void)
{
//	int ret = 0;

	//这里的定时工作，是为了测试目标。
	//这里的定时已经启动，是为了定时产生假测试数据，喂给L3BFSC模块，以便测试算法
	msg_struct_can_l3bfsc_new_ready_event_t snd;
	memset(&snd, 0, sizeof(msg_struct_can_l3bfsc_new_ready_event_t));
	snd.length = sizeof(msg_struct_can_l3bfsc_new_ready_event_t);
	gTaskCanitfleoContext.sensorIdRoundBing++;
	gTaskCanitfleoContext.sensorIdRoundBing = (gTaskCanitfleoContext.sensorIdRoundBing % HCU_SYSCFG_BFSC_SNR_WS_NBR_MAX);
	snd.sensorid = gTaskCanitfleoContext.sensorIdRoundBing;
	snd.sensorid = rand() % HCU_SYSCFG_BFSC_SNR_WS_NBR_MAX;
	snd.sensorWsValue = 500 + (rand()%50);

	if (hcu_message_send(MSG_ID_CAN_L3BFSC_WS_NEW_READY_EVENT, TASK_ID_L3BFSC, TASK_ID_CANITFLEO, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName, zHcuVmCtrTab.task[TASK_ID_L3BFSC].taskName);

	return SUCCESS;
}


OPSTAT func_canitfleo_working_scan_process(void)
{
	return SUCCESS;
}

//本消息将消耗在L2
OPSTAT func_canitfleo_l2frame_msg_bfsc_startup_ind_received_handle(StrMsg_HUITP_MSGID_sui_bfsc_startup_ind_t *rcv, UINT8 nodeId)
{
	//因为没有标准的IE结构，所以这里不能再验证IEID/IELEN的大小段和长度问题

	//系统初始化过程
	if (FsmGetState(TASK_ID_L3BFSC) < FSM_STATE_L3BFSC_OOS_SCAN){
		gTaskL3bfscContext.sensorWs[nodeId].sensorStatus = HCU_L3BFSC_SENSOR_WS_STATUS_STARTUP;
	}

	//系统已经在工作状态：发送CFG给IHU
	else{
		//生成bitmap
		UINT32 bitmap = 0;
		bitmap = (1<<nodeId);

		//准备组装发送消息
		StrMsg_HUITP_MSGID_sui_bfsc_set_config_req_t pMsgProc;
		UINT16 msgProcLen = sizeof(StrMsg_HUITP_MSGID_sui_bfsc_set_config_req_t);
		memset(&pMsgProc, 0, msgProcLen);
		pMsgProc.msgid = HUITP_ENDIAN_EXG16(HUITP_MSGID_sui_bfsc_set_config_req);
		pMsgProc.length = HUITP_ENDIAN_EXG16(msgProcLen - 4);
		pMsgProc.weight_sensor_param.WeightSensorLoadDetectionTimeMs = HUITP_ENDIAN_EXG32(gTaskL3bfscContext.wgtSnrPar.WeightSensorLoadDetectionTimeMs);
		pMsgProc.weight_sensor_param.WeightSensorLoadThread = HUITP_ENDIAN_EXG32(gTaskL3bfscContext.wgtSnrPar.WeightSensorLoadThread);
		pMsgProc.weight_sensor_param.WeightSensorEmptyThread = HUITP_ENDIAN_EXG32(gTaskL3bfscContext.wgtSnrPar.WeightSensorEmptyThread);
		pMsgProc.weight_sensor_param.WeightSensorEmptyDetectionTimeMs = HUITP_ENDIAN_EXG32(gTaskL3bfscContext.wgtSnrPar.WeightSensorEmptyDetectionTimeMs);
		pMsgProc.weight_sensor_param.WeightSensorPickupThread = HUITP_ENDIAN_EXG32(gTaskL3bfscContext.wgtSnrPar.WeightSensorPickupThread);
		pMsgProc.weight_sensor_param.WeightSensorPickupDetectionTimeMs = HUITP_ENDIAN_EXG32(gTaskL3bfscContext.wgtSnrPar.WeightSensorPickupDetectionTimeMs);
		pMsgProc.weight_sensor_param.StardardReadyTimeMs = HUITP_ENDIAN_EXG32(gTaskL3bfscContext.wgtSnrPar.StardardReadyTimeMs);
		pMsgProc.weight_sensor_param.MaxAllowedWeight = HUITP_ENDIAN_EXG32(gTaskL3bfscContext.wgtSnrPar.MaxAllowedWeight);
		pMsgProc.weight_sensor_param.WeightSensorInitOrNot = HUITP_ENDIAN_EXG32(gTaskL3bfscContext.wgtSnrPar.WeightSensorInitOrNot);
		pMsgProc.weight_sensor_param.WeightSensorAdcSampleFreq = HUITP_ENDIAN_EXG32(gTaskL3bfscContext.wgtSnrPar.WeightSensorAdcSampleFreq);
		pMsgProc.weight_sensor_param.WeightSensorAdcGain = HUITP_ENDIAN_EXG32(gTaskL3bfscContext.wgtSnrPar.WeightSensorAdcGain);
		pMsgProc.weight_sensor_param.WeightSensorAdcBitwidth = HUITP_ENDIAN_EXG32(gTaskL3bfscContext.wgtSnrPar.WeightSensorAdcBitwidth);
		pMsgProc.weight_sensor_param.WeightSensorAdcValue = HUITP_ENDIAN_EXG32(gTaskL3bfscContext.wgtSnrPar.WeightSensorAdcValue);
		pMsgProc.weight_sensor_param.WeightSensorCalibrationZeroAdcValue = HUITP_ENDIAN_EXG32(gTaskL3bfscContext.wgtSnrPar.calibration[nodeId].WeightSensorCalibrationZeroAdcValue);
		pMsgProc.weight_sensor_param.WeightSensorCalibrationFullAdcValue = HUITP_ENDIAN_EXG32(gTaskL3bfscContext.wgtSnrPar.calibration[nodeId].WeightSensorCalibrationFullAdcValue);
		pMsgProc.weight_sensor_param.WeightSensorCalibrationFullWeight = HUITP_ENDIAN_EXG32(gTaskL3bfscContext.wgtSnrPar.calibration[nodeId].WeightSensorCalibrationFullWeight);
		pMsgProc.weight_sensor_param.WeightSensorStaticZeroValue = HUITP_ENDIAN_EXG32(gTaskL3bfscContext.wgtSnrPar.WeightSensorStaticZeroValue);
		pMsgProc.weight_sensor_param.WeightSensorTailorValue = HUITP_ENDIAN_EXG32(gTaskL3bfscContext.wgtSnrPar.WeightSensorTailorValue);
		pMsgProc.weight_sensor_param.WeightSensorDynamicZeroThreadValue = HUITP_ENDIAN_EXG32(gTaskL3bfscContext.wgtSnrPar.WeightSensorDynamicZeroThreadValue);
		pMsgProc.weight_sensor_param.WeightSensorDynamicZeroHysteresisMs = HUITP_ENDIAN_EXG32(gTaskL3bfscContext.wgtSnrPar.WeightSensorDynamicZeroHysteresisMs);
		pMsgProc.motor_control_param.MotorSpeed = HUITP_ENDIAN_EXG32(gTaskL3bfscContext.motCtrPar.MotorSpeed);
		pMsgProc.motor_control_param.MotorDirection = HUITP_ENDIAN_EXG32(gTaskL3bfscContext.motCtrPar.MotorDirection);
		pMsgProc.motor_control_param.MotorRollingStartMs = HUITP_ENDIAN_EXG32(gTaskL3bfscContext.motCtrPar.MotorRollingStartMs);
		pMsgProc.motor_control_param.MotorRollingStopMs = HUITP_ENDIAN_EXG32(gTaskL3bfscContext.motCtrPar.MotorRollingStopMs);
		pMsgProc.motor_control_param.MotorRollingInveralMs = HUITP_ENDIAN_EXG32(gTaskL3bfscContext.motCtrPar.MotorRollingInveralMs);
		pMsgProc.motor_control_param.MotorFailureDetectionVaration = HUITP_ENDIAN_EXG32(gTaskL3bfscContext.motCtrPar.MotorFailureDetectionVaration);
		pMsgProc.motor_control_param.MotorFailureDetectionTimeMs = HUITP_ENDIAN_EXG32(gTaskL3bfscContext.motCtrPar.MotorFailureDetectionTimeMs);

		//发送消息
		if (hcu_canitfleo_usbcan_l2frame_send((UINT8*)&pMsgProc, msgProcLen, bitmap) == FAILURE)
			HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Send CAN frame error!\n");

		//更新传感器状态
		gTaskL3bfscContext.sensorWs[nodeId].sensorStatus = HCU_L3BFSC_SENSOR_WS_STATUS_CFG_REQ;
	}

	return SUCCESS;
}

//本消息将发送给L3
OPSTAT func_canitfleo_l2frame_msg_bfsc_set_config_resp_received_handle(StrMsg_HUITP_MSGID_sui_bfsc_set_config_resp_t *rcv, UINT8 nodeId)
{
	//因为没有标准的IE结构，所以这里不能再验证IEID/IELEN的大小段和长度问题
	//将内容发送给目的模块，具体内容是否越界／合理，均由L3模块进行处理

	//系统初始化过程
	if (FsmGetState(TASK_ID_L3BFSC) < FSM_STATE_L3BFSC_OOS_SCAN){
		msg_struct_can_l3bfsc_sys_cfg_resp_t snd;
		memset(&snd, 0, sizeof(msg_struct_can_l3bfsc_sys_cfg_resp_t));
		snd.validFlag = rcv->validFlag;
		snd.errCode = HUITP_ENDIAN_EXG16(rcv->errCode);
		snd.sensorid = nodeId;
		snd.length = sizeof(msg_struct_can_l3bfsc_sys_cfg_resp_t);
		if (hcu_message_send(MSG_ID_CAN_L3BFSC_SYS_CFG_RESP, TASK_ID_L3BFSC, TASK_ID_CANITFLEO, &snd, snd.length) == FAILURE)
			HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName, zHcuVmCtrTab.task[TASK_ID_L3BFSC].taskName);
	}

	//系统已经在工作状态：发送START给IHU
	else{
		//生成bitmap
		UINT32 bitmap = 0;
		bitmap = (1<<nodeId);

		//准备组装发送消息
		StrMsg_HUITP_MSGID_sui_bfsc_start_req_t pMsgProc;
		UINT16 msgProcLen = sizeof(StrMsg_HUITP_MSGID_sui_bfsc_start_req_t);
		memset(&pMsgProc, 0, msgProcLen);
		pMsgProc.msgid = HUITP_ENDIAN_EXG16(HUITP_MSGID_sui_bfsc_start_req);
		pMsgProc.length = HUITP_ENDIAN_EXG16(msgProcLen - 4);

		//发送消息
		if (hcu_canitfleo_usbcan_l2frame_send((UINT8*)&pMsgProc, msgProcLen, bitmap) == FAILURE)
			HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Send CAN frame error!\n");

		//更新状态
		gTaskL3bfscContext.sensorWs[nodeId].sensorStatus = HCU_L3BFSC_SENSOR_WS_STATUS_START_REQ;
	}

	//返回
	return SUCCESS;
}

//本消息将发送给L3
OPSTAT func_canitfleo_l2frame_msg_bfsc_start_resp_received_handle(StrMsg_HUITP_MSGID_sui_bfsc_start_resp_t *rcv, UINT8 nodeId)
{
	//因为没有标准的IE结构，所以这里不能再验证IEID/IELEN的大小段和长度问题
	//将内容发送给目的模块，具体内容是否越界／合理，均由L3模块进行处理
	//系统初始化过程
	if (FsmGetState(TASK_ID_L3BFSC) < FSM_STATE_L3BFSC_OOS_SCAN){
		msg_struct_can_l3bfsc_sys_start_resp_t snd;
		memset(&snd, 0, sizeof(msg_struct_can_l3bfsc_sys_start_resp_t));
		snd.validFlag = rcv->validFlag;
		snd.errCode = HUITP_ENDIAN_EXG16(rcv->errCode);
		snd.sensorid = nodeId;
		snd.length = sizeof(msg_struct_can_l3bfsc_sys_start_resp_t);
		if (hcu_message_send(MSG_ID_CAN_L3BFSC_SYS_START_RESP, TASK_ID_L3BFSC, TASK_ID_CANITFLEO, &snd, snd.length) == FAILURE)
			HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName, zHcuVmCtrTab.task[TASK_ID_L3BFSC].taskName);
	}

	//系统已经在工作状态：可以正式工作了
	else{
		gTaskL3bfscContext.sensorWs[nodeId].sensorStatus = HCU_L3BFSC_SENSOR_WS_STATUS_VALIID_EMPTY;
	}

	//返回
	return SUCCESS;
}

//本消息将发送给L3
OPSTAT func_canitfleo_l2frame_msg_bfsc_stop_resp_received_handle(StrMsg_HUITP_MSGID_sui_bfsc_stop_resp_t *rcv, UINT8 nodeId)
{
	//因为没有标准的IE结构，所以这里不能再验证IEID/IELEN的大小段和长度问题
	//将内容发送给目的模块，具体内容是否越界／合理，均由L3模块进行处理
	msg_struct_can_l3bfsc_sys_stop_resp_t snd;
	memset(&snd, 0, sizeof(msg_struct_can_l3bfsc_sys_stop_resp_t));
	snd.validFlag = rcv->validFlag;
	snd.errCode = HUITP_ENDIAN_EXG16(rcv->errCode);
	snd.sensorid = nodeId;
	snd.length = sizeof(msg_struct_can_l3bfsc_sys_stop_resp_t);
	if (hcu_message_send(MSG_ID_CAN_L3BFSC_SYS_STOP_RESP, TASK_ID_L3BFSC, TASK_ID_CANITFLEO, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName, zHcuVmCtrTab.task[TASK_ID_L3BFSC].taskName);

	//返回
	return SUCCESS;
}

//本消息将发送给L3
OPSTAT func_canitfleo_l2frame_msg_bfsc_new_ws_event_received_handle(StrMsg_HUITP_MSGID_sui_bfsc_new_ws_event_t *rcv, UINT8 nodeId)
{
	//因为没有标准的IE结构，所以这里不能再验证IEID/IELEN的大小段和长度问题
	//将内容发送给目的模块，具体内容是否越界／合理，均由L3模块进行处理
	UINT32 comType = 0, wsEvent = 0;
	comType = HUITP_ENDIAN_EXG32(rcv->weight_combin_type.WeightCombineType);
	wsEvent = HUITP_ENDIAN_EXG32(rcv->weight_ind.weight_event);

	//真的新事件
	if ((comType == HUITP_IEID_SUI_BFSC_COMINETYPE_NULL) && (wsEvent == WEIGHT_EVENT_ID_LOAD)){
		msg_struct_can_l3bfsc_new_ready_event_t snd;
		memset(&snd, 0, sizeof(msg_struct_can_l3bfsc_new_ready_event_t));
		snd.sensorWsValue = HUITP_ENDIAN_EXG32(rcv->weight_ind.average_weight);
		//暂时不处理weight_event等不同类型事件
		snd.sensorid = nodeId;
		snd.length = sizeof(msg_struct_can_l3bfsc_new_ready_event_t);
		if (hcu_message_send(MSG_ID_CAN_L3BFSC_WS_NEW_READY_EVENT, TASK_ID_L3BFSC, TASK_ID_CANITFLEO, &snd, snd.length) == FAILURE)
			HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName, zHcuVmCtrTab.task[TASK_ID_L3BFSC].taskName);
	}

	//组合完成事件
	else if ((comType == HUITP_IEID_SUI_BFSC_COMINETYPE_ROOLOUT) && (wsEvent == WEIGHT_EVENT_ID_EMPTY)){
		msg_struct_can_l3bfsc_ws_comb_out_fb_t snd;
		memset(&snd, 0, sizeof(msg_struct_can_l3bfsc_ws_comb_out_fb_t));
		snd.sensorid = nodeId;
		snd.length = sizeof(msg_struct_can_l3bfsc_ws_comb_out_fb_t);
		if (hcu_message_send(MSG_ID_CAN_L3BFSC_WS_COMB_OUT_FB, TASK_ID_L3BFSC, TASK_ID_CANITFLEO, &snd, snd.length) == FAILURE)
			HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName, zHcuVmCtrTab.task[TASK_ID_L3BFSC].taskName);
	}

	//抛弃完成事件
	else if ((comType == HUITP_IEID_SUI_BFSC_COMINETYPE_DROP)  && (wsEvent == WEIGHT_EVENT_ID_EMPTY)){
		msg_struct_can_l3bfsc_ws_give_up_fb_t snd;
		memset(&snd, 0, sizeof(msg_struct_can_l3bfsc_ws_give_up_fb_t));
		snd.sensorid = nodeId;
		snd.length = sizeof(msg_struct_can_l3bfsc_ws_give_up_fb_t);
		if (hcu_message_send(MSG_ID_CAN_L3BFSC_WS_GIVE_UP_FB, TASK_ID_L3BFSC, TASK_ID_CANITFLEO, &snd, snd.length) == FAILURE)
			HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName, zHcuVmCtrTab.task[TASK_ID_L3BFSC].taskName);
	}

	//差错：也不完全是差错，只是上层不关心
	else{
		//HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Receive parameters error!");
	}
	//返回
	return SUCCESS;
}

//本消息将消耗在L2
OPSTAT func_canitfleo_l2frame_msg_bfsc_repeat_ws_received_handle(StrMsg_HUITP_MSGID_sui_bfsc_repeat_ws_event_t *rcv, UINT8 nodeId)
{
	//因为没有标准的IE结构，所以这里不能再验证IEID/IELEN的大小段和长度问题
	UINT32 comType = 0, wsEvent = 0;
	comType = HUITP_ENDIAN_EXG32(rcv->weight_combin_type.WeightCombineType);
	wsEvent = HUITP_ENDIAN_EXG32(rcv->weight_ind.weight_event);

	//这个事件，只允许在空闲态下汇报的
	if ((comType == HUITP_IEID_SUI_BFSC_COMINETYPE_NULL) && (wsEvent == WEIGHT_EVENT_ID_LOAD) && \
			(gTaskL3bfscContext.sensorWs[nodeId].sensorStatus = HCU_L3BFSC_SENSOR_WS_STATUS_VALID_TO_COMB)){
		gTaskL3bfscContext.sensorWs[nodeId].sensorRepTimes++;
	}
	else
		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Receive parameters error!");

	//返回
	return SUCCESS;
}

//本消息将消耗在L2
OPSTAT func_canitfleo_l2frame_msg_bfsc_ws_comb_out_received_handle(StrMsg_HUITP_MSGID_sui_bfsc_ws_comb_out_resp_t *rcv, UINT8 nodeId)
{
	//因为没有标准的IE结构，所以这里不能再验证IEID/IELEN的大小段和长度问题
	//更新状态
	UINT32 comType = 0;
	comType = HUITP_ENDIAN_EXG32(rcv->weight_combin_type.WeightCombineType);

	if (comType == HUITP_IEID_SUI_BFSC_COMINETYPE_ROOLOUT){
		gTaskL3bfscContext.sensorWs[nodeId].sensorStatus = HCU_L3BFSC_SENSOR_WS_STATUS_VALID_TTT_START;
	}
	else if (comType == HUITP_IEID_SUI_BFSC_COMINETYPE_DROP){
		gTaskL3bfscContext.sensorWs[nodeId].sensorStatus = HCU_L3BFSC_SENSOR_WS_STATUS_VALID_TTT_START;
	}
	else
		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Receive parameters error!");

	return SUCCESS;
}

//本消息将发送给UICOMM
OPSTAT func_canitfleo_l2frame_msg_bfsc_command_resp_received_handle(StrMsg_HUITP_MSGID_sui_bfsc_command_resp_t *rcv, UINT8 nodeId)
{
	//因为没有标准的IE结构，所以这里不能再验证IEID/IELEN的大小段和长度问题
	//将内容发送给目的模块
	msg_struct_can_uicomm_test_cmd_resp_t snd;
	memset(&snd, 0, sizeof(msg_struct_can_uicomm_test_cmd_resp_t));
	snd.validFlag = rcv->validFlag;
	snd.errCode = HUITP_ENDIAN_EXG16(rcv->result.error_code);
	snd.sensorid = nodeId;
	snd.motor_speed = HUITP_ENDIAN_EXG32(rcv->motor_speed);
	snd.sensor_weight = HUITP_ENDIAN_EXG32(rcv->sensor_weight);
	snd.length = sizeof(msg_struct_can_uicomm_test_cmd_resp_t);
	if (hcu_message_send(MSG_ID_CAN_UICOMM_TEST_CMD_RESP, TASK_ID_BFSCUICOMM, TASK_ID_CANITFLEO, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName, zHcuVmCtrTab.task[TASK_ID_BFSCUICOMM].taskName);

	//返回
	return SUCCESS;
}

//本消息将消耗在L2
OPSTAT func_canitfleo_l2frame_msg_bfsc_fault_ind_received_handle(StrMsg_HUITP_MSGID_sui_bfsc_fault_ind_t *rcv, UINT8 nodeId)
{
	//因为没有标准的IE结构，所以这里不能再验证IEID/IELEN的大小段和长度问题
	//只更新传感器的状态，不做其它处理
	if (gTaskL3bfscContext.sensorWs[nodeId].sensorStatus < HCU_L3BFSC_SENSOR_WS_STATUS_INIT_MAX)
		gTaskL3bfscContext.sensorWs[nodeId].sensorStatus = HCU_L3BFSC_SENSOR_WS_STATUS_INIT_ERR;
	else
		gTaskL3bfscContext.sensorWs[nodeId].sensorStatus = HCU_L3BFSC_SENSOR_WS_STATUS_VALID_ERROR;

	//返回
	return SUCCESS;
}

//本消息将发送给L3
OPSTAT func_canitfleo_l2frame_msg_bfsc_err_ind_cmd_resp_received_handle(StrMsg_HUITP_MSGID_sui_bfsc_err_inq_cmd_resp_t *rcv, UINT8 nodeId)
{
	//因为没有标准的IE结构，所以这里不能再验证IEID/IELEN的大小段和长度问题
	//将内容发送给目的模块，具体内容是否越界／合理，均由L3模块进行处理
	msg_struct_can_l3bfsc_error_inq_cmd_resp_t snd;
	memset(&snd, 0, sizeof(msg_struct_can_l3bfsc_error_inq_cmd_resp_t));
	snd.sensorid = nodeId;
	snd.validFlag = rcv->validFlag;
	snd.sensorWsValue = HUITP_ENDIAN_EXG32(rcv->average_weight);
	snd.length = sizeof(msg_struct_can_l3bfsc_error_inq_cmd_resp_t);
	if (hcu_message_send(MSG_ID_CAN_L3BFSC_ERROR_INQ_CMD_RESP, TASK_ID_L3BFSC, TASK_ID_CANITFLEO, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName, zHcuVmCtrTab.task[TASK_ID_L3BFSC].taskName);

	//返回
	return SUCCESS;
}



/*
 *  USBCAN BSP函数映射
 *
 */
OPSTAT hcu_canitfleo_usbcan_interface_init(void)
{
	return hcu_bsp_usbcan_init(&(gTaskCanitfleoContext.can1), CAN_DEVICE_TYPE_PCI9820I, \
				CAN_DEVIDE_IDX_CARD0, CAN_DEVIDE_CHANNEL_CAN0, \
				CAN_BANDRATE_500KBPS, 0, CAN_L2_FRAME_FORWARD_YES);
}

//发送API
OPSTAT hcu_canitfleo_usbcan_l2frame_send(UINT8 *buffer, UINT32 length, UINT32 wmc_id_bitmap)
{
	return hcu_bsp_usbcan_l2frame_transmit(&(gTaskCanitfleoContext.can1), buffer, length, wmc_id_bitmap);
}


/*
 *  以下函数暂时不用的，它是使用了socket CAN的函数形式
 *
 */

/*
 *
 * Initialize the SocketCAN
 * In: char *canitfname, like "can0"
 * Out: int *sock, sick id after initialized
 * Return: 1 Failure, 0 Success
 * MAKE SURE "sudo /sbin/ip link set can0 up type can bitrate 500000"
 *
 */
OPSTAT func_canitfleo_socketcan_init(char *canitfname, int *sock)
{

	int s; /* can raw socket */
	//int nbytes;
	struct sockaddr_can addr;
	//struct can_frame frame;
	struct ifreq ifr;

	/* check command line options */
	if (NULL == canitfname) HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: canitfname == NULL!\n");

	/* open socket */
	if ((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: socket open error!\n");

	addr.can_family = AF_CAN;

	strcpy(ifr.ifr_name, canitfname);
	if (ioctl(s, SIOCGIFINDEX, &ifr) < 0) HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: SIOCGIFINDEX error!\n");
	addr.can_ifindex = ifr.ifr_ifindex;

	/* disable default receive filter on this RAW socket */
	/* This is obsolete as we do not read from the socket at all, but for */
	/* this reason we can remove the receive list in the Kernel to save a */
	/* little (really a very little!) CPU usage.                          */
	//setsockopt(s, SOL_CAN_RAW, CAN_RAW_FILTER, NULL, 0);
	/* comment this will be able to disable the filter */

	if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: bind!\n");

	/* Return / Get back the socket ID */
	*sock = s;

	return SUCCESS;
}

/*
 *
 * Send out a CAN frame with CAN ID
 * In: int socket, socket which has been initialized already
 * In: char *canid_canframe, can ID + can frame in the string format of "CANID#CANFRAME", like "602#0201030400000000"
 * Return: 1 Failure, 0 Success
 *
 */
OPSTAT func_canitfleo_socketcan_send(int socket, char *canid_canframe)
{
	int s; /* can raw socket */
	int nbytes;
	struct can_frame frame;

	if (0 == socket || -1 == socket) HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Invalid can socket ID.\n");

	s = socket;

	/* check command line options */
	if (NULL == canid_canframe) HCU_ERROR_PRINT_CANITFLEO("canid_canframe == NULL\n");

	/* send frame */
	if ((nbytes = write(s, &frame, sizeof(frame))) != sizeof(frame)) HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Write error.\n");

	return SUCCESS;
}

/*
 *
 * To block receive a CAN frame
 * In: int socket, socket which has been initialized already
 * Out: canid_t *canid, CAN ID
 * Out: char *canid_canframe, binary array for the CAN frame
 * Out: char *canid_canframe_char, string format of "CANID#CANFRAME", like "602#0201030400000000"
 * Return: 1 Failure, 0 Success
 *
 */

OPSTAT func_canitfleo_socketcan_receive(int socket, canid_t *canid, char *canframe_hex, char *canid_canframe_char)
{
	int s; /* can raw socket */
	int nbytes;
	struct sockaddr_can addr;
	struct can_frame frame;

	char ctrlmsg[CMSG_SPACE(sizeof(struct timeval)) + CMSG_SPACE(sizeof(__u32))];
	struct iovec iov;
	struct msghdr msg;

	if (0 == socket || -1 == socket) HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Invalid can socket ID.\n");

	s = socket;

	/* check command line options */
	if (NULL == canframe_hex || NULL == canid_canframe_char) HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: canid_canframe == NULL\n");

	/* these settings are static and can be held out of the hot path */
	iov.iov_base = &frame;
	msg.msg_name = &addr;
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	msg.msg_control = &ctrlmsg;

	/* these settings may be modified by recvmsg() */
	iov.iov_len = sizeof(frame);
	msg.msg_namelen = sizeof(addr);
	msg.msg_controllen = sizeof(ctrlmsg);
	msg.msg_flags = 0;

	nbytes = recvmsg(s, &msg, 0);

	if (nbytes < 0) HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: read error\n");

	if (nbytes < sizeof(struct can_frame)) HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: read: incomplete CAN frame\n");

	*canid = frame.can_id;
	memcpy(canframe_hex, frame.data, frame.can_dlc);

	return SUCCESS;
}

int func_canitfleo_socketcan_test_main(int argc, char **argv)
{
	int sock;
	int ret;
	canid_t canid = 0x0000;
	char buf[sizeof(MAX_CANFRAME)+1]; /* max length */
	char canframe_hex[256];

	int weight;
	int loop = 10, i = 0;
	int sleep_second = 2;

	HCU_DEBUG_PRINT_INF("pi_cantest_bf [loop] [sleep_second]\n");
	HCU_DEBUG_PRINT_INF("pi_cantest_bf 10 2, loop 10 time, wait for 2 seconds between each of the action\n");

	if(2 == argc) loop = atoi(argv[1]);
	if(3 == argc) {loop = atoi(argv[1]); sleep_second = atoi(argv[2]);}

	HCU_DEBUG_PRINT_INF("pi_cantest_bf loop = %d sleep_second = %d\n", loop, sleep_second);

	/****** MAKE SURE "sudo /sbin/ip link set can0 up type can bitrate 500000" ********/
	ret = func_canitfleo_socketcan_init("can0", &sock);
	if(1 == ret)
	{
		HCU_DEBUG_PRINT_INF("can_init failure.\n");
	}
	HCU_DEBUG_PRINT_INF("can_init successful.\n");

	/* int func_canitfleo_can_send(int socket, char *canid_canframe) */
	/* int func_canitfleo_can_receive(int socket, canid_t *canid, char *canframe_hex, char *canid_canframe_char) */
	/*  ALL THESE COMMAND ARE DEFINED BY BOFENG

	CAN COMMAND:
	TX FrameID: 0x0602
	RX FrameID: 0x0582

	MOTOR START   CLOCKWISE: 23 00 65 20 01 00 00 00, Response: 60 00 65 20 00 00 00 00
	MOTOR START C-CLOCKWISE: 23 00 65 20 02 00 00 00, Response: 60 00 65 20 00 00 00 00
	MOTOR STOP             : 23 00 65 20 03 00 00 00, Response: 60 00 65 20 00 00 00 00
	WEIGHT READ            : 40 00 65 02 00 00 00 00, Response: 43 00 65 02 DD CC BB AA

	HEART BEAT
	FrameID : 0x0702
	        : 7F

	cansend can0 602#2300652001000000
	candump can0

	*/
	for(i = 0; i < loop; i++)
	{
		/* =============================================================*/
		/* ========================= clockwise =========================*/
		/* =============================================================*/
		sleep(sleep_second);
		ret = func_canitfleo_socketcan_send(sock, "602#2300652001000000");
		if(1 == ret)
		{
			HCU_DEBUG_PRINT_INF("func_canitfleo_can_send(sock, 602#2300652001000000.\n");
		}
		HCU_DEBUG_PRINT_INF("func_canitfleo_can_send(sock, 602#2300652001000000 successful.\n");

		memset(buf, 0, sizeof(MAX_CANFRAME)+1);
		do{
			ret = func_canitfleo_socketcan_receive(sock, &canid, canframe_hex, buf);
			if(1 == ret)
			{
				HCU_DEBUG_PRINT_INF("can_receive(sock, &canid, &canframe_hex, &buf);\n");
			}
			if(canid == 0x702)
				HCU_DEBUG_PRINT_INF("can_receive(sock, &canid, &canframe_hex, &buf), CANOpen Heartbeat [%s}\n", buf);
		}while(canid == 0x702);

		HCU_DEBUG_PRINT_INF("can_receive(sock, &canid, &canframe_hex, &buf), buf = %s\n", buf);


		/* =============================================================*/
		/* ======================== r-clockwise ========================*/
		/* =============================================================*/
		sleep(sleep_second);
		ret = func_canitfleo_socketcan_send(sock, "602#2300652002000000");
		if(1 == ret)
		{
			HCU_DEBUG_PRINT_INF("func_canitfleo_can_send(sock, 602#2300652002000000.\n");
		}
		HCU_DEBUG_PRINT_INF("func_canitfleo_can_send(sock, 602#2300652002000000 successful.\n");

		memset(buf, 0, sizeof(MAX_CANFRAME)+1);
		do{
			ret = func_canitfleo_socketcan_receive(sock, &canid, canframe_hex, buf);
			if(1 == ret)
			{
				HCU_DEBUG_PRINT_INF("func_canitfleo_can_receive(sock, &canid, &canframe_hex, &buf);\n");
			}
			if(canid == 0x702)
				HCU_DEBUG_PRINT_INF("func_canitfleo_can_receive(sock, &canid, &canframe_hex, &buf), CANOpen Heartbeat [%s}\n", buf);
		}while(canid == 0x702);
		HCU_DEBUG_PRINT_INF("func_canitfleo_can_receive(sock, &canid, &canframe_hex, &buf), buf = %s\n", buf);

		/* =============================================================*/
		/* ======================== stop motor  ========================*/
		/* =============================================================*/
		sleep(sleep_second);
		ret = func_canitfleo_socketcan_send(sock, "602#2300652003000000");
		if(1 == ret)
		{
			HCU_DEBUG_PRINT_INF("func_canitfleo_can_send(sock, 602#2300652003000000.\n");
		}
		HCU_DEBUG_PRINT_INF("func_canitfleo_can_send(sock, 602#2300652003000000 successful.\n");

		//usleep(2000);
		memset(buf, 0, sizeof(MAX_CANFRAME)+1);
		do{
			ret = func_canitfleo_socketcan_receive(sock, &canid, canframe_hex, buf);
			if(1 == ret)
			{
				HCU_DEBUG_PRINT_INF("func_canitfleo_can_receive(sock, &canid, &canframe_hex, &buf);\n");
			}
			if(canid == 0x702)
				HCU_DEBUG_PRINT_INF("func_canitfleo_can_receive(sock, &canid, &canframe_hex, &buf), CANOpen Heartbeat [%s}\n", buf);

		}while(canid == 0x702);
		HCU_DEBUG_PRINT_INF("func_canitfleo_can_receive(sock, &canid, &canframe_hex, &buf), buf = %s\n", buf);

		/* =============================================================*/
		/* ======================== Weight Get  ========================*/
		/* =============================================================*/
		sleep(sleep_second);
		ret = func_canitfleo_socketcan_send(sock, "602#4000650200000000");
		if(1 == ret)
		{
			HCU_DEBUG_PRINT_INF("func_canitfleo_can_send(sock, 602#4000650200000000.\n");
		}
		HCU_DEBUG_PRINT_INF("func_canitfleo_can_send(sock, 602#4000650200000000 successful.\n");

		//usleep(2000);
		memset(buf, 0, sizeof(MAX_CANFRAME)+1);
		do{
			ret = func_canitfleo_socketcan_receive(sock, &canid, canframe_hex, buf);
			if(1 == ret)
			{
				HCU_DEBUG_PRINT_INF("can_receive(sock, &canid, &canframe_hex, &buf);\n");
			}
			if(canid == 0x702)
				HCU_DEBUG_PRINT_INF("can_receive(sock, &canid, &canframe_hex, &buf), CANOpen Heartbeat [%s}\n", buf);

		}while(canid == 0x702);
		weight = canframe_hex[4] + (canframe_hex[5] << 8) + (canframe_hex[6] << 16) + (canframe_hex[7] << 24);
		HCU_DEBUG_PRINT_INF("can_receive(sock, &canid, &canframe_hex, &buf), buf = %s, weight = %d\n", buf, weight);

	}

	return EXIT_SUCCESS;
}


#endif

