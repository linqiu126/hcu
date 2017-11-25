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

    //Normal task status:任何项目都可能用到的部分
#if ((HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFSC_CBU_ID) \
		|| (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFDF_CBU_ID)\
		|| (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFHS_CBU_ID))
	{MSG_ID_SYSSWM_CANITFLEO_INVENTORY_CONFIRM, 	FSM_STATE_CANITFLEO_ACTIVED,     fsm_canitfleo_sysswm_inventory_confirm},
	{MSG_ID_SYSSWM_CANITFLEO_SW_PACKAGE_CONFIRM, 	FSM_STATE_CANITFLEO_ACTIVED,     fsm_canitfleo_sysswm_sw_package_confirm},
	{MSG_ID_SUI_TEST_CMD_REQ, 						FSM_STATE_CANITFLEO_ACTIVED,     fsm_canitfleo_sui_test_cmd_req},
	{MSG_ID_SUI_SUSPEND_REQ, 						FSM_STATE_CANITFLEO_ACTIVED,     fsm_canitfleo_sui_suspend_req},
	{MSG_ID_SUI_RESUME_REQ, 						FSM_STATE_CANITFLEO_ACTIVED,     fsm_canitfleo_sui_resume_req},
#endif

    //Normal task status：BFSC
#if (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFSC_CBU_ID)
	{MSG_ID_L3BFSC_CAN_SYS_CFG_REQ,      	FSM_STATE_CANITFLEO_ACTIVED,          		fsm_canitfleo_l3bfsc_sys_cfg_req},  	//初始化配置
	{MSG_ID_L3BFSC_CAN_SYS_START_REQ,      	FSM_STATE_CANITFLEO_ACTIVED,          		fsm_canitfleo_can_sys_start_req},   	//人工命令
	{MSG_ID_L3BFSC_CAN_SYS_STOP_REQ,      	FSM_STATE_CANITFLEO_ACTIVED,          		fsm_canitfleo_can_sys_stop_req},   		//人工命令
	{MSG_ID_UICOMM_CAN_TEST_CMD_REQ,      	FSM_STATE_CANITFLEO_ACTIVED,          		fsm_canitfleo_can_test_cmd_req},   		//测试性命令

	{MSG_ID_L3BFSC_CAN_WS_COMB_OUT,      	FSM_STATE_CANITFLEO_ACTIVED,          		fsm_canitfleo_l3bfsc_ws_comb_out},  	//组合指令
	{MSG_ID_L3BFSC_CAN_WS_GIVE_UP,      	FSM_STATE_CANITFLEO_ACTIVED,          		fsm_canitfleo_l3bfsc_ws_give_up},   	//剔除指令
	{MSG_ID_L3BFSC_CAN_ERROR_INQ_CMD_REQ,   FSM_STATE_CANITFLEO_ACTIVED,          		fsm_canitfleo_l3bfsc_error_inq_cmd_req},//差错状态下的发送指令

	{MSG_ID_USBCAN_L2FRAME_RCV,      		FSM_STATE_CANITFLEO_ACTIVED,          		fsm_canitfleo_usbcan_l2frame_receive},   //收到L2送过来的帧
#endif

	//Normal task status：BFDF
#if (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFDF_CBU_ID)
	{MSG_ID_L3BFDF_CAN_SYS_CFG_REQ,      	FSM_STATE_CANITFLEO_ACTIVED,          		fsm_canitfleo_l3bfdf_sys_cfg_req},  	//初始化配置
	//{MSG_ID_L3BFDF_CAN_SYS_SUSPEND_REQ,     FSM_STATE_CANITFLEO_ACTIVED,          		fsm_canitfleo_l3bfdf_suspend_req},      //人工命令
	//{MSG_ID_L3BFDF_CAN_SYS_RESUME_REQ,      FSM_STATE_CANITFLEO_ACTIVED,          		fsm_canitfleo_l3bfdf_resume_req},   	//人工命令
	//{MSG_ID_UICOMM_CAN_TEST_CMD_REQ,      	FSM_STATE_CANITFLEO_ACTIVED,          		fsm_canitfleo_l3bfdf_test_cmd_req},   	//测试性命令
	{MSG_ID_L3BFDF_CAN_SNC_PULLIN_REQ,      FSM_STATE_CANITFLEO_ACTIVED,          		fsm_canitfleo_l3bfdf_snc_pullin_req},  	//拨杆指令
	{MSG_ID_L3BFDF_CAN_WS_COMB_OUT,      	FSM_STATE_CANITFLEO_ACTIVED,          		fsm_canitfleo_l3bfdf_ws_comb_out},  	//组合指令
	{MSG_ID_USBCAN_L2FRAME_RCV,      		FSM_STATE_CANITFLEO_ACTIVED,          		fsm_canitfleo_usbcan_l2frame_receive},  //收到L2送过来的帧
#endif

	//Normal task status：BFDF
#if (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFHS_CBU_ID)
	{MSG_ID_L3BFHS_CAN_SYS_CFG_REQ,      	FSM_STATE_CANITFLEO_ACTIVED,          		fsm_canitfleo_l3bfhs_sys_cfg_req},  	//初始化配置
	//{MSG_ID_L3BFHS_CAN_SYS_SUSPEND_REQ,     FSM_STATE_CANITFLEO_ACTIVED,          		fsm_canitfleo_l3bfhs_suspend_req},      //人工命令
	//{MSG_ID_L3BFHS_CAN_SYS_RESUME_REQ,      FSM_STATE_CANITFLEO_ACTIVED,          		fsm_canitfleo_l3bfhs_resume_req},   	//人工命令
	{MSG_ID_L3BFHS_CAN_CAL_ZERO_REQ,      	FSM_STATE_CANITFLEO_ACTIVED,          		fsm_canitfleo_l3bfhs_cal_zero_req},   	//人工命令
	{MSG_ID_L3BFHS_CAN_CAL_FULL_REQ,      	FSM_STATE_CANITFLEO_ACTIVED,          		fsm_canitfleo_l3bfhs_cal_full_req},   	//人工命令
	//{MSG_ID_UICOMM_CAN_TEST_CMD_REQ,      	FSM_STATE_CANITFLEO_ACTIVED,          		fsm_canitfleo_l3bfhs_test_cmd_req},   	//测试性命令
	{MSG_ID_USBCAN_L2FRAME_RCV,      		FSM_STATE_CANITFLEO_ACTIVED,          		fsm_canitfleo_usbcan_l2frame_receive},  //收到L2送过来的帧
#endif

	//结束点，固定定义，不要改动
    {MSG_ID_END,            	FSM_STATE_END,             				NULL},  //Ending
};

//Global variables
#if (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFSC_CBU_ID)
	#include "../l3appctrl/l3bfsc.h"
	extern gTaskL3bfscContext_t gTaskL3bfscContext;
#endif
#if (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFDF_CBU_ID)
	#include "../l3appctrl/l3bfdf.h"
	extern gTaskL3bfdfContext_t gTaskL3bfdfContext;
#endif
#if (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFHS_CBU_ID)
	#include "../l3appctrl/l3bfhs.h"
	extern gTaskL3bfhsContext_t gTaskL3bfhsContext;
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
	//为了简化任务的执行，先禁止该定时器的启动
#if (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFDF_CBU_ID)
	ret = hcu_timer_start(TASK_ID_CANITFLEO, TIMER_ID_1S_CANITFLEO_WORKING_SCAN, zHcuSysEngPar.timer.array[TIMER_ID_1S_CANITFLEO_WORKING_SCAN].dur, TIMER_TYPE_ONE_TIME, TIMER_RESOLUTION_1S);
	if (ret == FAILURE) HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Error start timer!\n");
#endif
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

	//这一块的启动与否，关系到程序是否自动退出来
/*
	if(SUCCESS == ret){
		return SUCCESS;
	}
	else{
		exit(EXIT_FAILURE);
		return FAILURE;
	}
*/
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
#if (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFDF_CBU_ID)
		func_canitfleo_working_scan_process();
#endif
	}

#if (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFDF_CBU_ID)
	//测试目的，正式程序中可以去掉
	else if ((rcv.timeId == TIMER_ID_10MS_CANITFLEO_SIMULATION_DATA) &&(rcv.timeRes == TIMER_RESOLUTION_10MS)){
		func_canitfleo_bfdf_simulation_data_process();
	}
#endif

#if (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFHS_CBU_ID)
	//测试目的，正式程序中可以去掉
	else if ((rcv.timeId == TIMER_ID_10MS_CANITFLEO_SIMULATION_DATA) &&(rcv.timeRes == TIMER_RESOLUTION_10MS)){
		func_canitfleo_bfhs_simulation_data_process();
	}
#endif

	//返回
	return SUCCESS;
}

/*
 *
 *
 * //BFSC项目部分//以下均为BFSC项目所特有的
 *
 *
 *
 */


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
			pMsgProc.weight_sensor_param.RemainDetectionTimeSec = HUITP_ENDIAN_EXG32(gTaskL3bfscContext.wgtSnrPar.RemainDetectionTimeSec);
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
			pMsgProc.weight_sensor_param.WeightSensorFilterCoeff[0] = HUITP_ENDIAN_EXG32(gTaskL3bfscContext.wgtSnrPar.WeightSensorFilterCoeff[0]);
			pMsgProc.weight_sensor_param.WeightSensorFilterCoeff[1] = HUITP_ENDIAN_EXG32(gTaskL3bfscContext.wgtSnrPar.WeightSensorFilterCoeff[1]);
			pMsgProc.weight_sensor_param.WeightSensorFilterCoeff[2] = HUITP_ENDIAN_EXG32(gTaskL3bfscContext.wgtSnrPar.WeightSensorFilterCoeff[2]);
			pMsgProc.weight_sensor_param.WeightSensorFilterCoeff[3] = HUITP_ENDIAN_EXG32(gTaskL3bfscContext.wgtSnrPar.WeightSensorFilterCoeff[3]);
			pMsgProc.weight_sensor_param.WeightSensorOutputValue[0] = HUITP_ENDIAN_EXG32(gTaskL3bfscContext.wgtSnrPar.WeightSensorOutputValue[0]);
			pMsgProc.weight_sensor_param.WeightSensorOutputValue[1] = HUITP_ENDIAN_EXG32(gTaskL3bfscContext.wgtSnrPar.WeightSensorOutputValue[1]);
			pMsgProc.weight_sensor_param.WeightSensorOutputValue[2] = HUITP_ENDIAN_EXG32(gTaskL3bfscContext.wgtSnrPar.WeightSensorOutputValue[2]);
			pMsgProc.weight_sensor_param.WeightSensorOutputValue[3] = HUITP_ENDIAN_EXG32(gTaskL3bfscContext.wgtSnrPar.WeightSensorOutputValue[3]);
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
	pMsgProc.cmdid = HUITP_ENDIAN_EXG32(rcv.cmdid);
	pMsgProc.cmdvalue = HUITP_ENDIAN_EXG32(rcv.cmdvalue);

	//发送消息
	if (hcu_canitfleo_usbcan_l2frame_send((UINT8*)&pMsgProc, msgProcLen, bitmap) == FAILURE)
		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Send CAN frame error!\n");

	HCU_DEBUG_PRINT_CRT("CANITFLEO: Send TEST_CMD, CmdId=%d, CmdValue=%d, Node BitMap = 0x%X!\n", rcv.cmdid, rcv.cmdvalue, bitmap);
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

//本消息将消耗在L2
OPSTAT func_canitfleo_l2frame_msg_bfsc_startup_ind_received_handle(StrMsg_HUITP_MSGID_sui_bfsc_startup_ind_t *rcv, UINT8 nodeId)
{
	//因为没有标准的IE结构，所以这里不能再验证IEID/IELEN的大小段和长度问题

	//Firstly Register state
	gTaskL3bfscContext.sensorWs[nodeId].sensorStatus = HCU_L3BFSC_SENSOR_WS_STATUS_STARTUP;
	HCU_DEBUG_PRINT_CRT("CANITFLEO: Sensor ID = %d is set to be startup!\n", nodeId);

	dbi_HcuBfsc_WmcStatusUpdate(0, nodeId, DBI_BFSC_SNESOR_STATUS_STARTUP, 0);

	//系统初始化过程
	if (FsmGetState(TASK_ID_L3BFSC) == FSM_STATE_L3BFSC_ACTIVED){
		//Do nothing
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
		pMsgProc.weight_sensor_param.RemainDetectionTimeSec = HUITP_ENDIAN_EXG32(gTaskL3bfscContext.wgtSnrPar.RemainDetectionTimeSec);
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
		pMsgProc.weight_sensor_param.WeightSensorFilterCoeff[0] = HUITP_ENDIAN_EXG32(gTaskL3bfscContext.wgtSnrPar.WeightSensorFilterCoeff[0]);
		pMsgProc.weight_sensor_param.WeightSensorFilterCoeff[1] = HUITP_ENDIAN_EXG32(gTaskL3bfscContext.wgtSnrPar.WeightSensorFilterCoeff[1]);
		pMsgProc.weight_sensor_param.WeightSensorFilterCoeff[2] = HUITP_ENDIAN_EXG32(gTaskL3bfscContext.wgtSnrPar.WeightSensorFilterCoeff[2]);
		pMsgProc.weight_sensor_param.WeightSensorFilterCoeff[3] = HUITP_ENDIAN_EXG32(gTaskL3bfscContext.wgtSnrPar.WeightSensorFilterCoeff[3]);
		pMsgProc.weight_sensor_param.WeightSensorOutputValue[0] = HUITP_ENDIAN_EXG32(gTaskL3bfscContext.wgtSnrPar.WeightSensorOutputValue[0]);
		pMsgProc.weight_sensor_param.WeightSensorOutputValue[1] = HUITP_ENDIAN_EXG32(gTaskL3bfscContext.wgtSnrPar.WeightSensorOutputValue[1]);
		pMsgProc.weight_sensor_param.WeightSensorOutputValue[2] = HUITP_ENDIAN_EXG32(gTaskL3bfscContext.wgtSnrPar.WeightSensorOutputValue[2]);
		pMsgProc.weight_sensor_param.WeightSensorOutputValue[3] = HUITP_ENDIAN_EXG32(gTaskL3bfscContext.wgtSnrPar.WeightSensorOutputValue[3]);
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

	if (rcv->validFlag == FALSE) dbi_HcuBfsc_WmcStatusUpdate(0, nodeId, DBI_BFSC_SNESOR_STATUS_CONFIG_ERR, 0);
	else dbi_HcuBfsc_WmcStatusUpdate(0, nodeId, DBI_BFSC_SNESOR_STATUS_CONFIG_RCV, 0);

	//系统初始化过程
	if (FsmGetState(TASK_ID_L3BFSC) < FSM_STATE_L3BFSC_OOS_SCAN){
		msg_struct_can_l3bfsc_sys_cfg_resp_t snd;
		memset(&snd, 0, sizeof(msg_struct_can_l3bfsc_sys_cfg_resp_t));
		snd.validFlag = HUITP_ENDIAN_EXG8(rcv->validFlag);
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

	dbi_HcuBfsc_WmcStatusUpdate(0, nodeId, DBI_BFSC_SNESOR_STATUS_START_RCV, 0);

	//系统初始化过程
	if (FsmGetState(TASK_ID_L3BFSC) < FSM_STATE_L3BFSC_OOS_SCAN){
		msg_struct_can_l3bfsc_sys_start_resp_t snd;
		memset(&snd, 0, sizeof(msg_struct_can_l3bfsc_sys_start_resp_t));
		snd.validFlag = HUITP_ENDIAN_EXG8(rcv->validFlag);
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
	dbi_HcuBfsc_WmcStatusUpdate(0, nodeId, DBI_BFSC_SNESOR_STATUS_STOP_RCV, 0);

	msg_struct_can_l3bfsc_sys_stop_resp_t snd;
	memset(&snd, 0, sizeof(msg_struct_can_l3bfsc_sys_stop_resp_t));
	snd.validFlag = HUITP_ENDIAN_EXG8(rcv->validFlag);
	HCU_DEBUG_PRINT_INF("CANITFLEO: Receive STOP_RESP flag = %d\n", snd.validFlag);
	snd.errCode = HUITP_ENDIAN_EXG16(rcv->errCode);
	snd.sensorid = nodeId;
	snd.length = sizeof(msg_struct_can_l3bfsc_sys_stop_resp_t);
	if (hcu_message_send(MSG_ID_CAN_L3BFSC_SYS_STOP_RESP, TASK_ID_L3BFSC, TASK_ID_CANITFLEO, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName, zHcuVmCtrTab.task[TASK_ID_L3BFSC].taskName);

	//返回
	return SUCCESS;
}

//本消息或者在本地消耗，或者将发送给L3
OPSTAT func_canitfleo_l2frame_msg_bfsc_new_ws_event_received_handle(StrMsg_HUITP_MSGID_sui_bfsc_new_ws_event_t *rcv, UINT8 nodeId)
{
	//因为没有标准的IE结构，所以这里不能再验证IEID/IELEN的大小段和长度问题
	//将内容发送给目的模块，具体内容是否越界／合理，均由L3模块进行处理
	UINT32 comType = 0, wsEvent = 0;
	comType = HUITP_ENDIAN_EXG32(rcv->weight_combin_type.WeightCombineType);
	wsEvent = HUITP_ENDIAN_EXG32(rcv->weight_ind.weight_event);

	//先更新本地数据库表单
	dbi_HcuBfsc_WmcStatusUpdate(0, nodeId, DBI_BFSC_SNESOR_STATUS_DATA_VALID, HUITP_ENDIAN_EXG32(rcv->weight_ind.average_weight));
	//HCU_DEBUG_PRINT_FAT("CANITFLEP: NEW WS EVENT, Receiveing NodeId/Weight = [%d/%d]", rcv->weight_ind.average_weight);

	//先检查汇报事件/EMPTY_LOAD事件
	if (HUITP_ENDIAN_EXG32(rcv->weight_ind.weight_event) == WEIGHT_EVENT_ID_EMPTY){
		gTaskL3bfscContext.sensorWs[nodeId].sensorStatus = HCU_L3BFSC_SENSOR_WS_STATUS_VALIID_EMPTY;
		gTaskL3bfscContext.sensorWs[nodeId].sensorValue = HUITP_ENDIAN_EXG32(rcv->weight_ind.average_weight);
		gTaskL3bfscContext.sensorWs[nodeId].sensorRepTimes = 0;
		return SUCCESS;
	}

	else if (HUITP_ENDIAN_EXG32(rcv->weight_ind.weight_event) != WEIGHT_EVENT_ID_LOAD){
		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Receive parameters error, receive Weight Event = %d!\n", rcv->weight_ind.weight_event);
		return FAILURE;
	}

	//真的新事件
	if ((comType == HUITP_IEID_SUI_BFSC_COMINETYPE_NULL) && (wsEvent == WEIGHT_EVENT_ID_LOAD)){
		if (FsmGetState(TASK_ID_L3BFSC) == FSM_STATE_L3BFSC_ACTIVED){
			gTaskL3bfscContext.sensorWs[nodeId].sensorStatus = HCU_L3BFSC_SENSOR_WS_STATUS_STARTUP;
		}
		else if (FsmGetState(TASK_ID_L3BFSC) == FSM_STATE_L3BFSC_OPR_CFG){
			gTaskL3bfscContext.sensorWs[nodeId].sensorStatus = HCU_L3BFSC_SENSOR_WS_STATUS_CFG_CMPL;
			gTaskL3bfscContext.sensorWs[nodeId].cfgRcvFlag = TRUE;
		}
		else if (FsmGetState(TASK_ID_L3BFSC) == FSM_STATE_L3BFSC_OPR_GO){
			gTaskL3bfscContext.sensorWs[nodeId].sensorStatus = HCU_L3BFSC_SENSOR_WS_STATUS_VALIID_EMPTY;
			gTaskL3bfscContext.sensorWs[nodeId].startRcvFlag = TRUE;
		}
		else if (FsmGetState(TASK_ID_L3BFSC) == FSM_STATE_L3BFSC_OOS_SCAN){
			gTaskL3bfscContext.cur.wsIncMatCnt++;
			gTaskL3bfscContext.cur.wsIncMatWgt += HUITP_ENDIAN_EXG32(rcv->weight_ind.average_weight);
			//先存再发，是为了防止上层NEW_WS_EVENT重入被拒接，导致数据丢失
			gTaskL3bfscContext.sensorWs[nodeId].sensorStatus = HCU_L3BFSC_SENSOR_WS_STATUS_VALID_TO_COMB;
			gTaskL3bfscContext.sensorWs[nodeId].sensorValue = HUITP_ENDIAN_EXG32(rcv->weight_ind.average_weight);
			gTaskL3bfscContext.sensorWs[nodeId].sensorRepTimes = 0;
			msg_struct_can_l3bfsc_new_ready_event_t snd;
			memset(&snd, 0, sizeof(msg_struct_can_l3bfsc_new_ready_event_t));
			snd.sensorWsValue = HUITP_ENDIAN_EXG32(rcv->weight_ind.average_weight);
			//暂时不处理weight_event等不同类型事件
			snd.sensorid = nodeId;
			snd.length = sizeof(msg_struct_can_l3bfsc_new_ready_event_t);
			if (hcu_message_send(MSG_ID_CAN_L3BFSC_WS_NEW_READY_EVENT, TASK_ID_L3BFSC, TASK_ID_CANITFLEO, &snd, snd.length) == FAILURE)
				HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName, zHcuVmCtrTab.task[TASK_ID_L3BFSC].taskName);
		}
		else{
			gTaskL3bfscContext.cur.wsIncMatCnt++;
			gTaskL3bfscContext.cur.wsIncMatWgt += HUITP_ENDIAN_EXG32(rcv->weight_ind.average_weight);
			gTaskL3bfscContext.sensorWs[nodeId].sensorStatus = HCU_L3BFSC_SENSOR_WS_STATUS_VALID_TO_COMB;
			gTaskL3bfscContext.sensorWs[nodeId].sensorValue = HUITP_ENDIAN_EXG32(rcv->weight_ind.average_weight);
			gTaskL3bfscContext.sensorWs[nodeId].sensorRepTimes = 0;
		}
	}

//	//组合完成事件
//	else if ((comType == HUITP_IEID_SUI_BFSC_COMINETYPE_ROOLOUT) && (wsEvent == WEIGHT_EVENT_ID_EMPTY)){
//		msg_struct_can_l3bfsc_ws_comb_out_fb_t snd;
//		memset(&snd, 0, sizeof(msg_struct_can_l3bfsc_ws_comb_out_fb_t));
//		snd.sensorid = nodeId;
//		snd.length = sizeof(msg_struct_can_l3bfsc_ws_comb_out_fb_t);
//		if (hcu_message_send(MSG_ID_CAN_L3BFSC_WS_COMB_OUT_FB, TASK_ID_L3BFSC, TASK_ID_CANITFLEO, &snd, snd.length) == FAILURE)
//			HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName, zHcuVmCtrTab.task[TASK_ID_L3BFSC].taskName);
//	}
//
//	//抛弃完成事件
//	else if ((comType == HUITP_IEID_SUI_BFSC_COMINETYPE_DROP)  && (wsEvent == WEIGHT_EVENT_ID_EMPTY)){
//		msg_struct_can_l3bfsc_ws_give_up_fb_t snd;
//		memset(&snd, 0, sizeof(msg_struct_can_l3bfsc_ws_give_up_fb_t));
//		snd.sensorid = nodeId;
//		snd.length = sizeof(msg_struct_can_l3bfsc_ws_give_up_fb_t);
//		if (hcu_message_send(MSG_ID_CAN_L3BFSC_WS_GIVE_UP_FB, TASK_ID_L3BFSC, TASK_ID_CANITFLEO, &snd, snd.length) == FAILURE)
//			HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName, zHcuVmCtrTab.task[TASK_ID_L3BFSC].taskName);
//	}

	//差错：也不完全是差错，只是上层不关心
	else{
		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Receive parameters error!\n");
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

	//先更新本地数据库表单
	dbi_HcuBfsc_WmcStatusUpdate(0, nodeId, DBI_BFSC_SNESOR_STATUS_DATA_VALID, HUITP_ENDIAN_EXG32(rcv->weight_ind.average_weight));
	//HCU_DEBUG_PRINT_FAT("CANITFLEP: REPEAT WS EVENT, Receiveing NodeId/Weight = [%d/%d]", rcv->weight_ind.average_weight);

	if ((comType == HUITP_IEID_SUI_BFSC_COMINETYPE_NULL) && (wsEvent == WEIGHT_EVENT_ID_LOAD)){
		if (FsmGetState(TASK_ID_L3BFSC) == FSM_STATE_L3BFSC_ACTIVED){
			gTaskL3bfscContext.sensorWs[nodeId].sensorStatus = HCU_L3BFSC_SENSOR_WS_STATUS_STARTUP;
		}
		else if (FsmGetState(TASK_ID_L3BFSC) == FSM_STATE_L3BFSC_OPR_CFG){
			gTaskL3bfscContext.sensorWs[nodeId].sensorStatus = HCU_L3BFSC_SENSOR_WS_STATUS_CFG_CMPL;
			gTaskL3bfscContext.sensorWs[nodeId].cfgRcvFlag = TRUE;
		}
		else if (FsmGetState(TASK_ID_L3BFSC) == FSM_STATE_L3BFSC_OPR_GO){
			gTaskL3bfscContext.sensorWs[nodeId].sensorStatus = HCU_L3BFSC_SENSOR_WS_STATUS_VALIID_EMPTY;
			gTaskL3bfscContext.sensorWs[nodeId].startRcvFlag = TRUE;
		}
		else if (FsmGetState(TASK_ID_L3BFSC) >= FSM_STATE_L3BFSC_OOS_SCAN){
			//如果是所有传感器全部进入REPEAT状态，则将其翻译为NEW_WS_EVENT发送L3，以解锁其工作逻辑
			if (func_l3bfsc_judge_whether_all_valid_sensor_enter_repeat_status() == TRUE){
				msg_struct_can_l3bfsc_new_ready_event_t snd;
				memset(&snd, 0, sizeof(msg_struct_can_l3bfsc_new_ready_event_t));
				snd.sensorWsValue = HUITP_ENDIAN_EXG32(rcv->weight_ind.average_weight);
				//暂时不处理weight_event等不同类型事件
				snd.sensorid = nodeId;
				snd.length = sizeof(msg_struct_can_l3bfsc_new_ready_event_t);
				if (hcu_message_send(MSG_ID_CAN_L3BFSC_WS_NEW_READY_EVENT, TASK_ID_L3BFSC, TASK_ID_CANITFLEO, &snd, snd.length) == FAILURE)
					HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName, zHcuVmCtrTab.task[TASK_ID_L3BFSC].taskName);
			}else{
				gTaskL3bfscContext.sensorWs[nodeId].sensorStatus = HCU_L3BFSC_SENSOR_WS_STATUS_VALID_TO_COMB;
				gTaskL3bfscContext.sensorWs[nodeId].sensorValue = HUITP_ENDIAN_EXG32(rcv->weight_ind.average_weight);
				gTaskL3bfscContext.sensorWs[nodeId].sensorRepTimes++;
			}
			//为了支持组合之后的剩余物料重新进入组合态，这里将REPEAT消息翻译为NEW_WS发送给L3BFSC，以便重新进入组合态
			if (FsmGetState(TASK_ID_L3BFSC) == FSM_STATE_L3BFSC_OOS_SCAN){
				msg_struct_can_l3bfsc_new_ready_event_t snd;
				memset(&snd, 0, sizeof(msg_struct_can_l3bfsc_new_ready_event_t));
				snd.sensorWsValue = HUITP_ENDIAN_EXG32(rcv->weight_ind.average_weight);
				//暂时不处理weight_event等不同类型事件
				snd.sensorid = nodeId;
				snd.length = sizeof(msg_struct_can_l3bfsc_new_ready_event_t);
				if (hcu_message_send(MSG_ID_CAN_L3BFSC_WS_NEW_READY_EVENT, TASK_ID_L3BFSC, TASK_ID_CANITFLEO, &snd, snd.length) == FAILURE)
					HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName, zHcuVmCtrTab.task[TASK_ID_L3BFSC].taskName);
			}
		}
		else HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Receive parameters error under wrong state machine, Nodeid=%d, ComType=%d, Event=%d!\n", nodeId, comType, wsEvent);
	}
	else HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Receive parameters error under wrong sensor status, Nodeid=%d, ComType=%d, Event=%d!\n", nodeId, comType, wsEvent);

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

	//先更新本地数据库表单
	dbi_HcuBfsc_WmcStatusUpdate(0, nodeId, DBI_BFSC_SNESOR_STATUS_DATA_VALID, 0);	//数据清零

	if (comType == HUITP_IEID_SUI_BFSC_COMINETYPE_ROOLOUT){
		//gTaskL3bfscContext.sensorWs[nodeId].sensorStatus = HCU_L3BFSC_SENSOR_WS_STATUS_VALID_TTT_START;
		msg_struct_can_l3bfsc_ws_comb_out_fb_t snd;
		memset(&snd, 0, sizeof(msg_struct_can_l3bfsc_ws_comb_out_fb_t));
		snd.sensorid = nodeId;
		snd.validFlag = TRUE;
		snd.errCode = 0;
		snd.length = sizeof(msg_struct_can_l3bfsc_ws_comb_out_fb_t);
		if (hcu_message_send(MSG_ID_CAN_L3BFSC_WS_COMB_OUT_FB, TASK_ID_L3BFSC, TASK_ID_CANITFLEO, &snd, snd.length) == FAILURE)
			HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName, zHcuVmCtrTab.task[TASK_ID_L3BFSC].taskName);
	}
	else if (comType == HUITP_IEID_SUI_BFSC_COMINETYPE_DROP){
		//gTaskL3bfscContext.sensorWs[nodeId].sensorStatus = HCU_L3BFSC_SENSOR_WS_STATUS_VALID_TTT_START;
		msg_struct_can_l3bfsc_ws_give_up_fb_t snd;
		memset(&snd, 0, sizeof(msg_struct_can_l3bfsc_ws_give_up_fb_t));
		snd.sensorid = nodeId;
		snd.validFlag = TRUE;
		snd.errCode = 0;
		snd.length = sizeof(msg_struct_can_l3bfsc_ws_give_up_fb_t);
		if (hcu_message_send(MSG_ID_CAN_L3BFSC_WS_GIVE_UP_FB, TASK_ID_L3BFSC, TASK_ID_CANITFLEO, &snd, snd.length) == FAILURE)
			HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName, zHcuVmCtrTab.task[TASK_ID_L3BFSC].taskName);
	}
	else if ((comType == HUITP_IEID_SUI_BFSC_COMINETYPE_ERROR) && (FsmGetState(TASK_ID_L3BFSC) == FSM_STATE_L3BFSC_OOS_TTT)){
		//gTaskL3bfscContext.sensorWs[nodeId].sensorStatus = HCU_L3BFSC_SENSOR_WS_STATUS_VALID_TTT_START;
		msg_struct_can_l3bfsc_ws_comb_out_fb_t snd;
		memset(&snd, 0, sizeof(msg_struct_can_l3bfsc_ws_comb_out_fb_t));
		snd.sensorid = nodeId;
		snd.validFlag = FALSE;
		snd.errCode = 0;  //待完善
		snd.length = sizeof(msg_struct_can_l3bfsc_ws_comb_out_fb_t);
		if (hcu_message_send(MSG_ID_CAN_L3BFSC_WS_COMB_OUT_FB, TASK_ID_L3BFSC, TASK_ID_CANITFLEO, &snd, snd.length) == FAILURE)
			HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName, zHcuVmCtrTab.task[TASK_ID_L3BFSC].taskName);
	}
	else if ((comType == HUITP_IEID_SUI_BFSC_COMINETYPE_ERROR) && (FsmGetState(TASK_ID_L3BFSC) == FSM_STATE_L3BFSC_OOS_TGU)){
		//gTaskL3bfscContext.sensorWs[nodeId].sensorStatus = HCU_L3BFSC_SENSOR_WS_STATUS_VALID_TTT_START;
		msg_struct_can_l3bfsc_ws_give_up_fb_t snd;
		memset(&snd, 0, sizeof(msg_struct_can_l3bfsc_ws_give_up_fb_t));
		snd.sensorid = nodeId;
		snd.validFlag = FALSE;
		snd.errCode = 0;  //待完善
		snd.length = sizeof(msg_struct_can_l3bfsc_ws_give_up_fb_t);
		if (hcu_message_send(MSG_ID_CAN_L3BFSC_WS_GIVE_UP_FB, TASK_ID_L3BFSC, TASK_ID_CANITFLEO, &snd, snd.length) == FAILURE)
			HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName, zHcuVmCtrTab.task[TASK_ID_L3BFSC].taskName);
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
	dbi_HcuBfsc_WmcStatusUpdate(0, nodeId, DBI_BFSC_SNESOR_STATUS_TEST_CMD_RCV, 0);

	msg_struct_can_uicomm_test_cmd_resp_t snd;
	memset(&snd, 0, sizeof(msg_struct_can_uicomm_test_cmd_resp_t));
	snd.validFlag = HUITP_ENDIAN_EXG8(rcv->validFlag);
	snd.errCode = HUITP_ENDIAN_EXG16(rcv->result.error_code);
	snd.sensorid = nodeId;
	snd.cmdid = HUITP_ENDIAN_EXG32(rcv->cmdid);
	snd.cmdvalue1 = HUITP_ENDIAN_EXG32(rcv->cmdvalue1);
	snd.cmdvalue2 = HUITP_ENDIAN_EXG32(rcv->cmdvalue2);
	snd.length = sizeof(msg_struct_can_uicomm_test_cmd_resp_t);

	HCU_DEBUG_PRINT_CRT("CANITFLEO: Test Cmd received, NodeID=%d, CmdId=%d, Value1=%d, Value2=%d\n", nodeId, snd.cmdid, snd.cmdvalue1, snd.cmdvalue2);
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

	dbi_HcuBfsc_WmcStatusUpdate(0, nodeId, DBI_BFSC_SNESOR_STATUS_FAULT_RCV, 0);

	if (gTaskL3bfscContext.sensorWs[nodeId].sensorStatus < HCU_L3BFSC_SENSOR_WS_STATUS_INIT_MAX)
		gTaskL3bfscContext.sensorWs[nodeId].sensorStatus = HCU_L3BFSC_SENSOR_WS_STATUS_INIT_ERR;
	else
		//隔离该传感器
		gTaskL3bfscContext.sensorWs[nodeId].sensorStatus = HCU_L3BFSC_SENSOR_WS_STATUS_HW_ERROR; //HCU_L3BFSC_SENSOR_WS_STATUS_VALID_ERROR;

	//是否要根据ERR_CODE，赋予不同的差错情形，待定

	//返回
	return SUCCESS;
}

//本消息将发送给L3
OPSTAT func_canitfleo_l2frame_msg_bfsc_err_ind_cmd_resp_received_handle(StrMsg_HUITP_MSGID_sui_bfsc_err_inq_cmd_resp_t *rcv, UINT8 nodeId)
{
	//因为没有标准的IE结构，所以这里不能再验证IEID/IELEN的大小段和长度问题
	//将内容发送给目的模块，具体内容是否越界／合理，均由L3模块进行处理
	dbi_HcuBfsc_WmcStatusUpdate(0, nodeId, DBI_BFSC_SNESOR_STATUS_ERR_RESP_RCV, 0);

	msg_struct_can_l3bfsc_error_inq_cmd_resp_t snd;
	memset(&snd, 0, sizeof(msg_struct_can_l3bfsc_error_inq_cmd_resp_t));
	snd.sensorid = nodeId;
	snd.validFlag = HUITP_ENDIAN_EXG8(rcv->validFlag);
	snd.sensorWsValue = HUITP_ENDIAN_EXG32(rcv->average_weight);
	snd.length = sizeof(msg_struct_can_l3bfsc_error_inq_cmd_resp_t);
	if (hcu_message_send(MSG_ID_CAN_L3BFSC_ERROR_INQ_CMD_RESP, TASK_ID_L3BFSC, TASK_ID_CANITFLEO, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName, zHcuVmCtrTab.task[TASK_ID_L3BFSC].taskName);

	//返回
	return SUCCESS;
}

//本消息将消耗在L2
OPSTAT func_canitfleo_l2frame_msg_bfsc_heart_beat_report_received_handle(StrMsg_HUITP_MSGID_sui_bfsc_heart_beat_report_t *rcv, UINT8 nodeId)
{
	//因为没有标准的IE结构，所以这里不能再验证IEID/IELEN的大小段和长度问题
	//将内容发送给目的模块：暂无。基于目前的情况，等待下位机重启

	//准备组装发送消息
	StrMsg_HUITP_MSGID_sui_bfsc_heart_beat_confirm_t pMsgProc;
	UINT16 msgProcLen = sizeof(StrMsg_HUITP_MSGID_sui_bfsc_heart_beat_confirm_t);
	memset(&pMsgProc, 0, msgProcLen);
	pMsgProc.msgid = HUITP_ENDIAN_EXG16(HUITP_MSGID_sui_bfsc_heart_beat_confirm);
	pMsgProc.length = HUITP_ENDIAN_EXG16(msgProcLen - 4);
	pMsgProc.wmc_id.wmc_id = nodeId;
	if (gTaskL3bfscContext.sensorWs[nodeId].sensorStatus <= HCU_L3BFSC_SENSOR_WS_STATUS_OFFLINE_MAX)
		pMsgProc.wmcState = HUITP_IEID_SUI_BFSC_HEATT_BEAT_WMC_STATE_OFFLINE;
	else if (gTaskL3bfscContext.sensorWs[nodeId].sensorStatus <= HCU_L3BFSC_SENSOR_WS_STATUS_INIT_MAX)
		pMsgProc.wmcState = HUITP_IEID_SUI_BFSC_HEATT_BEAT_WMC_STATE_INIT;
	else if (gTaskL3bfscContext.sensorWs[nodeId].sensorStatus <= HCU_L3BFSC_SENSOR_WS_STATUS_WORK_MAX)
		pMsgProc.wmcState = HUITP_IEID_SUI_BFSC_HEATT_BEAT_WMC_STATE_WORKING;
	else
		pMsgProc.wmcState = HUITP_IEID_SUI_BFSC_HEATT_BEAT_WMC_STATE_INVALID;

	//生成bitmap
	UINT32 bitmap = ((UINT32)1<<nodeId);

	//发送消息
	if (hcu_canitfleo_usbcan_l2frame_send((UINT8*)&pMsgProc, msgProcLen, bitmap) == FAILURE)
		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Send CAN frame error!\n");

	//返回
	return SUCCESS;
}

#endif//BFSC项目部分



/*
 *
 *
 *  //BFDF项目部分
 *
 *
 *
 */


#if (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFDF_CBU_ID)
OPSTAT fsm_canitfleo_l3bfdf_sys_cfg_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret=0;
	//int i=0;

	msg_struct_l3bfdf_can_sys_cfg_req_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_l3bfdf_can_sys_cfg_req_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_l3bfdf_can_sys_cfg_req_t)))
		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);


	//为了测试目的，直接返回
	msg_struct_can_l3bfdf_sys_cfg_resp_t snd;
	memset(&snd, 0, sizeof(msg_struct_can_l3bfdf_sys_cfg_resp_t));
	snd.validFlag = TRUE;
	snd.length = sizeof(msg_struct_can_l3bfdf_sys_cfg_resp_t);
	int i=0, j=0;
	for (i = 0; i<HCU_SYSCFG_BFDF_EQU_FLOW_NBR_MAX; i++)
	{
		for (j = 0; j<HCU_SYSCFG_BFDF_NODE_BOARD_NBR_MAX; j++)
		{
			snd.streamId = i;
			snd.boardId = j;
			hcu_usleep(rand()%10000);
			if (hcu_message_send(MSG_ID_CAN_L3BFDF_SYS_CFG_RESP, TASK_ID_L3BFDF, TASK_ID_CANITFLEO, &snd, snd.length) == FAILURE)
				HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName, zHcuVmCtrTab.task[TASK_ID_L3BFDF].taskName);
		}
	}

	//启动测试定时器
	hcu_timer_start(TASK_ID_CANITFLEO, TIMER_ID_10MS_CANITFLEO_SIMULATION_DATA, zHcuSysEngPar.timer.array[TIMER_ID_10MS_CANITFLEO_SIMULATION_DATA].dur, TIMER_TYPE_PERIOD, TIMER_RESOLUTION_10MS);

	//返回
	return SUCCESS;
}

//OPSTAT fsm_canitfleo_l3bfdf_suspend_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
//{
//	//int ret=0;
//	//int i=0;
//
//	msg_struct_l3bfdf_can_sys_suspend_req_t rcv;
//	memset(&rcv, 0, sizeof(msg_struct_l3bfdf_can_sys_suspend_req_t));
//	if ((param_ptr == NULL || param_len > sizeof(msg_struct_l3bfdf_can_sys_suspend_req_t)))
//		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Receive message error!\n");
//	memcpy(&rcv, param_ptr, param_len);
//
//	//返回
//	return SUCCESS;
//}

//OPSTAT fsm_canitfleo_l3bfdf_resume_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
//{
//	//int ret=0;
//	//int i=0;
//
//	msg_struct_l3bfdf_can_sys_resume_req_t rcv;
//	memset(&rcv, 0, sizeof(msg_struct_l3bfdf_can_sys_resume_req_t));
//	if ((param_ptr == NULL || param_len > sizeof(msg_struct_l3bfdf_can_sys_resume_req_t)))
//		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Receive message error!\n");
//	memcpy(&rcv, param_ptr, param_len);
//
//	//返回
//	return SUCCESS;
//}

//OPSTAT fsm_canitfleo_l3bfdf_test_cmd_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
//{
//	//int ret=0;
//	//int i=0;
//
//	msg_struct_uicomm_can_test_cmd_req_t rcv;
//	memset(&rcv, 0, sizeof(msg_struct_uicomm_can_test_cmd_req_t));
//	if ((param_ptr == NULL || param_len > sizeof(msg_struct_uicomm_can_test_cmd_req_t)))
//		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Receive message error!\n");
//	memcpy(&rcv, param_ptr, param_len);
//
//	//返回
//	return SUCCESS;
//}

OPSTAT fsm_canitfleo_l3bfdf_ws_comb_out(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret=0;
	//int i=0;

	msg_struct_l3bfdf_can_ws_comb_out_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_l3bfdf_can_ws_comb_out_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_l3bfdf_can_ws_comb_out_t)))
		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);

	//测试反馈
	msg_struct_can_l3bfdf_ws_comb_out_fb_t snd;
	memset(&snd, 0, sizeof(msg_struct_can_l3bfdf_ws_comb_out_fb_t));
	snd.validFlag = TRUE;
	snd.streamId = rcv.streamId;
	snd.hopperId = rcv.hopperId;
	snd.length = sizeof(msg_struct_can_l3bfdf_ws_comb_out_fb_t);
	if (hcu_message_send(MSG_ID_CAN_L3BFDF_WS_COMB_OUT_FB, TASK_ID_L3BFDF, TASK_ID_CANITFLEO, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName, zHcuVmCtrTab.task[TASK_ID_L3BFDF].taskName);

	//返回
	return SUCCESS;
}

OPSTAT fsm_canitfleo_l3bfdf_snc_pullin_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret=0;
	//int i=0;

	msg_struct_l3bfdf_can_ws_comb_out_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_l3bfdf_can_ws_comb_out_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_l3bfdf_can_ws_comb_out_t)))
		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);

	//测试反馈
	if (rcv.hopperId == 0){
		//抛弃物料
		return SUCCESS;
	}

	msg_struct_can_l3bfdf_snc_pullin_resp_t snd;
	memset(&snd, 0, sizeof(msg_struct_can_l3bfdf_snc_pullin_resp_t));
	snd.validFlag = TRUE;
	snd.streamId = rcv.streamId;
	snd.hopperId = rcv.hopperId;
	snd.length = sizeof(msg_struct_can_l3bfdf_snc_pullin_resp_t);
	if (hcu_message_send(MSG_ID_CAN_L3BFDF_SNC_PULLIN_RESP, TASK_ID_L3BFDF, TASK_ID_CANITFLEO, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName, zHcuVmCtrTab.task[TASK_ID_L3BFDF].taskName);

	//返回
	return SUCCESS;
}

OPSTAT func_canitfleo_working_scan_process(void)
{
	gTaskL3bfdfContext.nodeDyn[0][0].nodeStatus = HCU_L3BFDF_NODE_BOARD_STATUS_STARTUP;
	gTaskL3bfdfContext.nodeDyn[0][1].nodeStatus = HCU_L3BFDF_NODE_BOARD_STATUS_STARTUP;
	gTaskL3bfdfContext.nodeDyn[0][2].nodeStatus = HCU_L3BFDF_NODE_BOARD_STATUS_STARTUP;
	gTaskL3bfdfContext.nodeDyn[0][3].nodeStatus = HCU_L3BFDF_NODE_BOARD_STATUS_STARTUP;
	gTaskL3bfdfContext.nodeDyn[0][4].nodeStatus = HCU_L3BFDF_NODE_BOARD_STATUS_STARTUP;
	gTaskL3bfdfContext.nodeDyn[1][0].nodeStatus = HCU_L3BFDF_NODE_BOARD_STATUS_STARTUP;
	gTaskL3bfdfContext.nodeDyn[1][1].nodeStatus = HCU_L3BFDF_NODE_BOARD_STATUS_STARTUP;
	gTaskL3bfdfContext.nodeDyn[1][2].nodeStatus = HCU_L3BFDF_NODE_BOARD_STATUS_STARTUP;
	gTaskL3bfdfContext.nodeDyn[1][3].nodeStatus = HCU_L3BFDF_NODE_BOARD_STATUS_STARTUP;
	gTaskL3bfdfContext.nodeDyn[1][4].nodeStatus = HCU_L3BFDF_NODE_BOARD_STATUS_STARTUP;

	return SUCCESS;
}

//发送测试数据给L3BFDF
OPSTAT func_canitfleo_bfdf_simulation_data_process(void)
{
//	int ret = 0;

	//这里的定时工作，是为了测试目标。
	//这里的定时已经启动，是为了定时产生假测试数据，喂给L3BFSC模块，以便测试算法
	msg_struct_can_l3bfdf_new_ready_event_t snd;
	memset(&snd, 0, sizeof(msg_struct_can_l3bfdf_new_ready_event_t));
	snd.length = sizeof(msg_struct_can_l3bfdf_new_ready_event_t);
	gTaskCanitfleoContext.sensorIdRoundBing++;
	gTaskCanitfleoContext.sensorIdRoundBing = (gTaskCanitfleoContext.sensorIdRoundBing % HCU_SYSCFG_BFDF_EQU_FLOW_NBR_MAX);
	//snd.streamId = gTaskCanitfleoContext.sensorIdRoundBing;
	snd.streamId = rand() % HCU_SYSCFG_BFDF_EQU_FLOW_NBR_MAX;
	snd.sensorWsValue = (rand()%900+100)*100;
	//HcuDebugPrint("CANITFLEO: Value = %d\n", snd.sensorWsValue);
	if (hcu_message_send(MSG_ID_CAN_L3BFDF_WS_NEW_READY_EVENT, TASK_ID_L3BFDF, TASK_ID_CANITFLEO, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName, zHcuVmCtrTab.task[TASK_ID_L3BFDF].taskName);

	return SUCCESS;
}

//OPSTAT func_canitfleo_l2frame_msg_bfdf_startup_ind_received_handle(StrMsg_HUITP_MSGID_sui_bfdf_startup_ind_t *rcv, UINT8 nodeId)
//{
//	//因为没有标准的IE结构，所以这里不能再验证IEID/IELEN的大小段和长度问题
//
//	//Firstly Register state
//	//STREAMID待完善
//	gTaskL3bfdfContext.nodeDyn[0][nodeId].nodeStatus = HCU_L3BFDF_NODE_BOARD_STATUS_STARTUP;
//	HCU_DEBUG_PRINT_CRT("CANITFLEO: Sensor ID = %d is set to be startup!\n", nodeId);
//
//	//dbi_HcuBfdf_WmcStatusUpdate(0, nodeId, DBI_BFDF_SNESOR_STATUS_STARTUP, 0);
//
//	//系统初始化过程
//	if (FsmGetState(TASK_ID_L3BFDF) == FSM_STATE_L3BFDF_ACTIVED){
//		//Do nothing
//	}
//
//	//系统已经在工作状态：发送CFG给IHU
//	else{
//		//生成bitmap
//		UINT32 bitmap = 0;
//		bitmap = (1<<nodeId);
//
//		//准备组装发送消息
//		StrMsg_HUITP_MSGID_sui_bfdf_set_config_req_t pMsgProc;
//		UINT16 msgProcLen = sizeof(StrMsg_HUITP_MSGID_sui_bfdf_set_config_req_t);
//		memset(&pMsgProc, 0, msgProcLen);
//		pMsgProc.msgid = HUITP_ENDIAN_EXG16(HUITP_MSGID_sui_bfdf_set_config_req);
//		pMsgProc.length = HUITP_ENDIAN_EXG16(msgProcLen - 4);
////		pMsgProc.weight_sensor_param.WeightSensorLoadDetectionTimeMs = HUITP_ENDIAN_EXG32(gTaskL3bfdfContext.wgtSnrPar.WeightSensorLoadDetectionTimeMs);
////		pMsgProc.weight_sensor_param.WeightSensorLoadThread = HUITP_ENDIAN_EXG32(gTaskL3bfdfContext.wgtSnrPar.WeightSensorLoadThread);
////		pMsgProc.weight_sensor_param.WeightSensorEmptyThread = HUITP_ENDIAN_EXG32(gTaskL3bfdfContext.wgtSnrPar.WeightSensorEmptyThread);
////		pMsgProc.weight_sensor_param.WeightSensorEmptyDetectionTimeMs = HUITP_ENDIAN_EXG32(gTaskL3bfdfContext.wgtSnrPar.WeightSensorEmptyDetectionTimeMs);
////		pMsgProc.weight_sensor_param.WeightSensorPickupThread = HUITP_ENDIAN_EXG32(gTaskL3bfdfContext.wgtSnrPar.WeightSensorPickupThread);
////		pMsgProc.weight_sensor_param.WeightSensorPickupDetectionTimeMs = HUITP_ENDIAN_EXG32(gTaskL3bfdfContext.wgtSnrPar.WeightSensorPickupDetectionTimeMs);
////		pMsgProc.weight_sensor_param.StardardReadyTimeMs = HUITP_ENDIAN_EXG32(gTaskL3bfdfContext.wgtSnrPar.StardardReadyTimeMs);
////		pMsgProc.weight_sensor_param.MaxAllowedWeight = HUITP_ENDIAN_EXG32(gTaskL3bfdfContext.wgtSnrPar.MaxAllowedWeight);
////		pMsgProc.weight_sensor_param.RemainDetectionTimeSec = HUITP_ENDIAN_EXG32(gTaskL3bfdfContext.wgtSnrPar.RemainDetectionTimeSec);
////		pMsgProc.weight_sensor_param.WeightSensorInitOrNot = HUITP_ENDIAN_EXG32(gTaskL3bfdfContext.wgtSnrPar.WeightSensorInitOrNot);
////		pMsgProc.weight_sensor_param.WeightSensorAdcSampleFreq = HUITP_ENDIAN_EXG32(gTaskL3bfdfContext.wgtSnrPar.WeightSensorAdcSampleFreq);
////		pMsgProc.weight_sensor_param.WeightSensorAdcGain = HUITP_ENDIAN_EXG32(gTaskL3bfdfContext.wgtSnrPar.WeightSensorAdcGain);
////		pMsgProc.weight_sensor_param.WeightSensorAdcBitwidth = HUITP_ENDIAN_EXG32(gTaskL3bfdfContext.wgtSnrPar.WeightSensorAdcBitwidth);
////		pMsgProc.weight_sensor_param.WeightSensorAdcValue = HUITP_ENDIAN_EXG32(gTaskL3bfdfContext.wgtSnrPar.WeightSensorAdcValue);
////		pMsgProc.weight_sensor_param.WeightSensorCalibrationZeroAdcValue = HUITP_ENDIAN_EXG32(gTaskL3bfdfContext.wgtSnrPar.calibration[nodeId].WeightSensorCalibrationZeroAdcValue);
////		pMsgProc.weight_sensor_param.WeightSensorCalibrationFullAdcValue = HUITP_ENDIAN_EXG32(gTaskL3bfdfContext.wgtSnrPar.calibration[nodeId].WeightSensorCalibrationFullAdcValue);
////		pMsgProc.weight_sensor_param.WeightSensorCalibrationFullWeight = HUITP_ENDIAN_EXG32(gTaskL3bfdfContext.wgtSnrPar.calibration[nodeId].WeightSensorCalibrationFullWeight);
////		pMsgProc.weight_sensor_param.WeightSensorStaticZeroValue = HUITP_ENDIAN_EXG32(gTaskL3bfdfContext.wgtSnrPar.WeightSensorStaticZeroValue);
////		pMsgProc.weight_sensor_param.WeightSensorTailorValue = HUITP_ENDIAN_EXG32(gTaskL3bfdfContext.wgtSnrPar.WeightSensorTailorValue);
////		pMsgProc.weight_sensor_param.WeightSensorDynamicZeroThreadValue = HUITP_ENDIAN_EXG32(gTaskL3bfdfContext.wgtSnrPar.WeightSensorDynamicZeroThreadValue);
////		pMsgProc.weight_sensor_param.WeightSensorDynamicZeroHysteresisMs = HUITP_ENDIAN_EXG32(gTaskL3bfdfContext.wgtSnrPar.WeightSensorDynamicZeroHysteresisMs);
////		pMsgProc.weight_sensor_param.WeightSensorFilterCoeff[0] = HUITP_ENDIAN_EXG32(gTaskL3bfdfContext.wgtSnrPar.WeightSensorFilterCoeff[0]);
////		pMsgProc.weight_sensor_param.WeightSensorFilterCoeff[1] = HUITP_ENDIAN_EXG32(gTaskL3bfdfContext.wgtSnrPar.WeightSensorFilterCoeff[1]);
////		pMsgProc.weight_sensor_param.WeightSensorFilterCoeff[2] = HUITP_ENDIAN_EXG32(gTaskL3bfdfContext.wgtSnrPar.WeightSensorFilterCoeff[2]);
////		pMsgProc.weight_sensor_param.WeightSensorFilterCoeff[3] = HUITP_ENDIAN_EXG32(gTaskL3bfdfContext.wgtSnrPar.WeightSensorFilterCoeff[3]);
////		pMsgProc.weight_sensor_param.WeightSensorOutputValue[0] = HUITP_ENDIAN_EXG32(gTaskL3bfdfContext.wgtSnrPar.WeightSensorOutputValue[0]);
////		pMsgProc.weight_sensor_param.WeightSensorOutputValue[1] = HUITP_ENDIAN_EXG32(gTaskL3bfdfContext.wgtSnrPar.WeightSensorOutputValue[1]);
////		pMsgProc.weight_sensor_param.WeightSensorOutputValue[2] = HUITP_ENDIAN_EXG32(gTaskL3bfdfContext.wgtSnrPar.WeightSensorOutputValue[2]);
////		pMsgProc.weight_sensor_param.WeightSensorOutputValue[3] = HUITP_ENDIAN_EXG32(gTaskL3bfdfContext.wgtSnrPar.WeightSensorOutputValue[3]);
////		pMsgProc.motor_control_param.MotorSpeed = HUITP_ENDIAN_EXG32(gTaskL3bfdfContext.motCtrPar.MotorSpeed);
////		pMsgProc.motor_control_param.MotorDirection = HUITP_ENDIAN_EXG32(gTaskL3bfdfContext.motCtrPar.MotorDirection);
////		pMsgProc.motor_control_param.MotorRollingStartMs = HUITP_ENDIAN_EXG32(gTaskL3bfdfContext.motCtrPar.MotorRollingStartMs);
////		pMsgProc.motor_control_param.MotorRollingStopMs = HUITP_ENDIAN_EXG32(gTaskL3bfdfContext.motCtrPar.MotorRollingStopMs);
////		pMsgProc.motor_control_param.MotorRollingInveralMs = HUITP_ENDIAN_EXG32(gTaskL3bfdfContext.motCtrPar.MotorRollingInveralMs);
////		pMsgProc.motor_control_param.MotorFailureDetectionVaration = HUITP_ENDIAN_EXG32(gTaskL3bfdfContext.motCtrPar.MotorFailureDetectionVaration);
////		pMsgProc.motor_control_param.MotorFailureDetectionTimeMs = HUITP_ENDIAN_EXG32(gTaskL3bfdfContext.motCtrPar.MotorFailureDetectionTimeMs);
//
//		//发送消息
//		if (hcu_canitfleo_usbcan_l2frame_send((UINT8*)&pMsgProc, msgProcLen, bitmap) == FAILURE)
//			HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Send CAN frame error!\n");
//
//		//更新传感器状态
//		gTaskL3bfdfContext.nodeDyn[0][nodeId].nodeStatus = HCU_L3BFDF_NODE_BOARD_STATUS_CFG_START_REQ;
//	}
//
//	return SUCCESS;
//}

OPSTAT func_canitfleo_l2frame_msg_bfdf_set_config_resp_received_handle(StrMsg_HUITP_MSGID_sui_bfdf_set_config_resp_t *rcv, UINT8 nodeId)
{
	//因为没有标准的IE结构，所以这里不能再验证IEID/IELEN的大小段和长度问题
	//将内容发送给目的模块，具体内容是否越界／合理，均由L3模块进行处理

//	if (rcv->validFlag == FALSE) dbi_HcuBfsc_WmcStatusUpdate(0, nodeId, DBI_BFDF_SNESOR_STATUS_CONFIG_ERR, 0);
//	else dbi_HcuBfdf_WmcStatusUpdate(0, nodeId, DBI_BFDF_SNESOR_STATUS_CONFIG_RCV, 0);

	//系统初始化过程
	msg_struct_can_l3bfdf_sys_cfg_resp_t snd;
	memset(&snd, 0, sizeof(msg_struct_can_l3bfdf_sys_cfg_resp_t));
//		snd.validFlag = HUITP_ENDIAN_EXG8(rcv->validFlag);
//		snd.errCode = HUITP_ENDIAN_EXG16(rcv->errCode);
//		snd.sensorid = nodeId;
	snd.length = sizeof(msg_struct_can_l3bfdf_sys_cfg_resp_t);
	if (hcu_message_send(MSG_ID_CAN_L3BFDF_SYS_CFG_RESP, TASK_ID_L3BFDF, TASK_ID_CANITFLEO, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName, zHcuVmCtrTab.task[TASK_ID_L3BFDF].taskName);

	return SUCCESS;
}

//OPSTAT func_canitfleo_l2frame_msg_bfdf_suspend_resp_received_handle(StrMsg_HUITP_MSGID_sui_bfdf_suspend_resp_t *rcv, UINT8 nodeId)
//{
//	//因为没有标准的IE结构，所以这里不能再验证IEID/IELEN的大小段和长度问题
//	//将内容发送给目的模块，具体内容是否越界／合理，均由L3模块进行处理
//
//	//dbi_HcuBfdf_WmcStatusUpdate(0, nodeId, DBI_BFDF_SNESOR_STATUS_SUSPEND_RCV, 0);
//
//	//系统初始化过程
//	msg_struct_can_l3bfdf_sys_suspend_resp_t snd;
//	memset(&snd, 0, sizeof(msg_struct_can_l3bfdf_sys_suspend_resp_t));
////		snd.validFlag = HUITP_ENDIAN_EXG8(rcv->validFlag);
////		snd.errCode = HUITP_ENDIAN_EXG16(rcv->errCode);
//	snd.length = sizeof(msg_struct_can_l3bfdf_sys_suspend_resp_t);
//	if (hcu_message_send(MSG_ID_CAN_L3BFDF_SYS_SUSPEND_RESP, TASK_ID_L3BFDF, TASK_ID_CANITFLEO, &snd, snd.length) == FAILURE)
//		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName, zHcuVmCtrTab.task[TASK_ID_L3BFDF].taskName);
//
//	//系统已经在工作状态：可以正式工作了
//	else{
//		gTaskL3bfdfContext.nodeDyn[0][nodeId].nodeStatus = HCU_L3BFDF_NODE_BOARD_STATUS_SUSPEND;
//	}
//
//	return SUCCESS;
//}
//
//OPSTAT func_canitfleo_l2frame_msg_bfdf_resume_resp_received_handle(StrMsg_HUITP_MSGID_sui_bfdf_resume_resp_t *rcv, UINT8 nodeId)
//{
//	//dbi_HcuBfdf_WmcStatusUpdate(0, nodeId, DBI_BFDF_SNESOR_STATUS_RESUME_RCV, 0);
//
//	//系统初始化过程
//	msg_struct_can_l3bfdf_sys_resume_resp_t snd;
//	memset(&snd, 0, sizeof(msg_struct_can_l3bfdf_sys_resume_resp_t));
////		snd.validFlag = HUITP_ENDIAN_EXG8(rcv->validFlag);
////		snd.errCode = HUITP_ENDIAN_EXG16(rcv->errCode);
//	snd.length = sizeof(msg_struct_can_l3bfdf_sys_resume_resp_t);
//	if (hcu_message_send(MSG_ID_CAN_L3BFDF_SYS_RESUME_RESP, TASK_ID_L3BFDF, TASK_ID_CANITFLEO, &snd, snd.length) == FAILURE)
//		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName, zHcuVmCtrTab.task[TASK_ID_L3BFDF].taskName);
//
//	//系统已经在工作状态：可以正式工作了
//	else{
//		gTaskL3bfdfContext.nodeDyn[0][nodeId].nodeStatus = HCU_L3BFDF_NODE_BOARD_STATUS_VALID;
//	}
//
//	return SUCCESS;
//}

OPSTAT func_canitfleo_l2frame_msg_bfdf_new_ws_event_received_handle(StrMsg_HUITP_MSGID_sui_bfdf_new_ws_event_t *rcv, UINT8 nodeId)
{
	//因为没有标准的IE结构，所以这里不能再验证IEID/IELEN的大小段和长度问题
	//将内容发送给目的模块，具体内容是否越界／合理，均由L3模块进行处理
//	UINT32 comType = 0, wsEvent = 0;
//	comType = HUITP_ENDIAN_EXG32(rcv->weight_combin_type.WeightCombineType);
//	wsEvent = HUITP_ENDIAN_EXG32(rcv->weight_ind.weight_event);

//	//先更新本地数据库表单
//	//dbi_HcuBfdf_WmcStatusUpdate(0, nodeId, DBI_BFDF_SNESOR_STATUS_DATA_VALID, HUITP_ENDIAN_EXG32(rcv->weight_ind.average_weight));
//
//	//先检查汇报事件/EMPTY_LOAD事件
//	if (HUITP_ENDIAN_EXG32(rcv->weight_ind.weight_event) == WEIGHT_EVENT_ID_EMPTY){
//		gTaskL3bfdfContext.sensorWs[0].sensorStatus = HCU_L3BFDF_SENSOR_WS_STATUS_VALIID_EMPTY;
//		gTaskL3bfdfContext.sensorWs[0].sensorValue = HUITP_ENDIAN_EXG32(rcv->weight_ind.average_weight);
//		gTaskL3bfdfContext.sensorWs[0].sensorRepTimes = 0;
//		return SUCCESS;
//	}
//
//	else if (HUITP_ENDIAN_EXG32(rcv->weight_ind.weight_event) != WEIGHT_EVENT_ID_LOAD){
//		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Receive parameters error, receive Weight Event = %d!\n", rcv->weight_ind.weight_event);
//		return FAILURE;
//	}
//
//	//真的新事件
//	if ((comType == HUITP_IEID_SUI_BFDF_COMINETYPE_NULL) && (wsEvent == WEIGHT_EVENT_ID_LOAD)){
//		if (FsmGetState(TASK_ID_L3BFDF) == FSM_STATE_L3BFDF_ACTIVED){
//			gTaskL3bfdfContext.sensorWs[0].sensorStatus = HCU_L3BFDF_SENSOR_WS_STATUS_STARTUP;
//		}
//		else if (FsmGetState(TASK_ID_L3BFDF) == FSM_STATE_L3BFDF_OPR_CFG){
//			gTaskL3bfdfContext.sensorWs[0].sensorStatus = HCU_L3BFDF_SENSOR_WS_STATUS_CFG_CMPL;
//			gTaskL3bfdfContext.sensorWs[0].cfgRcvFlag = TRUE;
//		}
//		else if (FsmGetState(TASK_ID_L3BFDF) == FSM_STATE_L3BFDF_OPR_GO){
//			gTaskL3bfdfContext.sensorWs[0].sensorStatus = HCU_L3BFDF_SENSOR_WS_STATUS_VALIID_EMPTY;
//			gTaskL3bfdfContext.sensorWs[0].startRcvFlag = TRUE;
//		}
//		else if (FsmGetState(TASK_ID_L3BFDF) == FSM_STATE_L3BFDF_OOS_SCAN){
//			gTaskL3bfdfContext.cur.wsIncMatCnt++;
//			gTaskL3bfdfContext.cur.wsIncMatWgt += HUITP_ENDIAN_EXG32(rcv->weight_ind.average_weight);
//			//先存再发，是为了防止上层NEW_WS_EVENT重入被拒接，导致数据丢失
//			gTaskL3bfdfContext.sensorWs[0].sensorStatus = HCU_L3BFDF_SENSOR_WS_STATUS_VALID_TO_COMB;
//			gTaskL3bfdfContext.sensorWs[0].sensorValue = HUITP_ENDIAN_EXG32(rcv->weight_ind.average_weight);
//			gTaskL3bfdfContext.sensorWs[0].sensorRepTimes = 0;
//			msg_struct_can_l3bfdf_new_ready_event_t snd;
//			memset(&snd, 0, sizeof(msg_struct_can_l3bfdf_new_ready_event_t));
//			snd.sensorWsValue = HUITP_ENDIAN_EXG32(rcv->weight_ind.average_weight);
//			//暂时不处理weight_event等不同类型事件
//			snd.sensorid = nodeId;
//			snd.length = sizeof(msg_struct_can_l3bfdf_new_ready_event_t);
//			if (hcu_message_send(MSG_ID_CAN_L3BFDF_WS_NEW_READY_EVENT, TASK_ID_L3BFDF, TASK_ID_CANITFLEO, &snd, snd.length) == FAILURE)
//				HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName, zHcuVmCtrTab.task[TASK_ID_L3BFDF].taskName);
//		}
//		else{
//			gTaskL3bfdfContext.cur.wsIncMatCnt++;
//			gTaskL3bfdfContext.cur.wsIncMatWgt += HUITP_ENDIAN_EXG32(rcv->weight_ind.average_weight);
//			gTaskL3bfdfContext.sensorWs[0].sensorStatus = HCU_L3BFDF_SENSOR_WS_STATUS_VALID_TO_COMB;
//			gTaskL3bfdfContext.sensorWs[0].sensorValue = HUITP_ENDIAN_EXG32(rcv->weight_ind.average_weight);
//			gTaskL3bfdfContext.sensorWs[0].sensorRepTimes = 0;
//		}
//	}
//
//	//差错：也不完全是差错，只是上层不关心
//	else{
//		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Receive parameters error!\n");
//	}

	return SUCCESS;
}

OPSTAT func_canitfleo_l2frame_msg_bfdf_ws_comb_out_received_handle(StrMsg_HUITP_MSGID_sui_bfdf_ws_comb_out_resp_t *rcv, UINT8 nodeId)
{
	//因为没有标准的IE结构，所以这里不能再验证IEID/IELEN的大小段和长度问题
	//更新状态
	//UINT32 comType = 0;
	//comType = HUITP_ENDIAN_EXG32(rcv->weight_combin_type.WeightCombineType);

	//先更新本地数据库表单
	//dbi_HcuBfdf_WmcStatusUpdate(0, nodeId, DBI_BFDF_SNESOR_STATUS_DATA_VALID, 0);	//数据清零

//	if (comType == HUITP_IEID_SUI_BFDF_COMINETYPE_ROOLOUT){
//		//gTaskL3bfdfContext.sensorWs[nodeId].sensorStatus = HCU_L3BFDF_SENSOR_WS_STATUS_VALID_TTT_START;
//		msg_struct_can_l3bfdf_ws_comb_out_fb_t snd;
//		memset(&snd, 0, sizeof(msg_struct_can_l3bfdf_ws_comb_out_fb_t));
//		snd.sensorid = nodeId;
//		snd.validFlag = TRUE;
//		snd.errCode = 0;
//		snd.length = sizeof(msg_struct_can_l3bfdf_ws_comb_out_fb_t);
//		if (hcu_message_send(MSG_ID_CAN_L3BFDF_WS_COMB_OUT_FB, TASK_ID_L3BFDF, TASK_ID_CANITFLEO, &snd, snd.length) == FAILURE)
//			HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName, zHcuVmCtrTab.task[TASK_ID_L3BFDF].taskName);
//	}
//	else if (comType == HUITP_IEID_SUI_BFDF_COMINETYPE_DROP){
//		//gTaskL3bfdfContext.sensorWs[nodeId].sensorStatus = HCU_L3BFDF_SENSOR_WS_STATUS_VALID_TTT_START;
//		msg_struct_can_l3bfdf_ws_give_up_fb_t snd;
//		memset(&snd, 0, sizeof(msg_struct_can_l3bfdf_ws_give_up_fb_t));
//		snd.sensorid = nodeId;
//		snd.validFlag = TRUE;
//		snd.errCode = 0;
//		snd.length = sizeof(msg_struct_can_l3bfdf_ws_give_up_fb_t);
//		if (hcu_message_send(MSG_ID_CAN_L3BFDF_WS_GIVE_UP_FB, TASK_ID_L3BFDF, TASK_ID_CANITFLEO, &snd, snd.length) == FAILURE)
//			HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName, zHcuVmCtrTab.task[TASK_ID_L3BFDF].taskName);
//	}
//	else if ((comType == HUITP_IEID_SUI_BFDF_COMINETYPE_ERROR) && (FsmGetState(TASK_ID_L3BFDF) == FSM_STATE_L3BFDF_OOS_TTT)){
//		//gTaskL3bfdfContext.sensorWs[nodeId].sensorStatus = HCU_L3BFDF_SENSOR_WS_STATUS_VALID_TTT_START;
//		msg_struct_can_l3bfdf_ws_comb_out_fb_t snd;
//		memset(&snd, 0, sizeof(msg_struct_can_l3bfdf_ws_comb_out_fb_t));
//		snd.sensorid = nodeId;
//		snd.validFlag = FALSE;
//		snd.errCode = 0;  //待完善
//		snd.length = sizeof(msg_struct_can_l3bfdf_ws_comb_out_fb_t);
//		if (hcu_message_send(MSG_ID_CAN_L3BFDF_WS_COMB_OUT_FB, TASK_ID_L3BFDF, TASK_ID_CANITFLEO, &snd, snd.length) == FAILURE)
//			HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName, zHcuVmCtrTab.task[TASK_ID_L3BFDF].taskName);
//	}
//	else if ((comType == HUITP_IEID_SUI_BFDF_COMINETYPE_ERROR) && (FsmGetState(TASK_ID_L3BFDF) == FSM_STATE_L3BFDF_OOS_TGU)){
//		//gTaskL3bfdfContext.sensorWs[nodeId].sensorStatus = HCU_L3BFDF_SENSOR_WS_STATUS_VALID_TTT_START;
//		msg_struct_can_l3bfdf_ws_give_up_fb_t snd;
//		memset(&snd, 0, sizeof(msg_struct_can_l3bfdf_ws_give_up_fb_t));
//		snd.sensorid = nodeId;
//		snd.validFlag = FALSE;
//		snd.errCode = 0;  //待完善
//		snd.length = sizeof(msg_struct_can_l3bfdf_ws_give_up_fb_t);
//		if (hcu_message_send(MSG_ID_CAN_L3BFDF_WS_GIVE_UP_FB, TASK_ID_L3BFDF, TASK_ID_CANITFLEO, &snd, snd.length) == FAILURE)
//			HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName, zHcuVmCtrTab.task[TASK_ID_L3BFDF].taskName);
//	}
//
//	else
//		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Receive parameters error!");

	return SUCCESS;
}

//OPSTAT func_canitfleo_l2frame_msg_bfdf_command_resp_received_handle(StrMsg_HUITP_MSGID_sui_bfdf_command_resp_t *rcv, UINT8 nodeId)
//{
//	//因为没有标准的IE结构，所以这里不能再验证IEID/IELEN的大小段和长度问题
//	//将内容发送给目的模块
//	//dbi_HcuBfdf_WmcStatusUpdate(0, nodeId, DBI_BFHS_SNESOR_STATUS_TEST_CMD_RCV, 0);
//
//	msg_struct_can_uicomm_test_cmd_resp_t snd;
//	memset(&snd, 0, sizeof(msg_struct_can_uicomm_test_cmd_resp_t));
////	snd.validFlag = HUITP_ENDIAN_EXG8(rcv->validFlag);
////	snd.errCode = HUITP_ENDIAN_EXG16(rcv->result.error_code);
////	snd.sensorid = nodeId;
////	snd.cmdid = HUITP_ENDIAN_EXG32(rcv->cmdid);
////	snd.cmdvalue1 = HUITP_ENDIAN_EXG32(rcv->cmdvalue1);
////	snd.cmdvalue2 = HUITP_ENDIAN_EXG32(rcv->cmdvalue2);
//	snd.length = sizeof(msg_struct_can_uicomm_test_cmd_resp_t);
//
//	HCU_DEBUG_PRINT_CRT("CANITFLEO: Test Cmd received, NodeID=%d, CmdId=%d, Value1=%d, Value2=%d\n", nodeId, snd.cmdid, snd.cmdvalue1, snd.cmdvalue2);
//	if (hcu_message_send(MSG_ID_CAN_UICOMM_TEST_CMD_RESP, TASK_ID_BFDFUICOMM, TASK_ID_CANITFLEO, &snd, snd.length) == FAILURE)
//		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName, zHcuVmCtrTab.task[TASK_ID_BFDFUICOMM].taskName);
//
//	return SUCCESS;
//}

//OPSTAT func_canitfleo_l2frame_msg_bfdf_fault_ind_received_handle(StrMsg_HUITP_MSGID_sui_bfdf_fault_ind_t *rcv, UINT8 nodeId)
//{
//	//因为没有标准的IE结构，所以这里不能再验证IEID/IELEN的大小段和长度问题
//	//只更新传感器的状态，不做其它处理
//
//	//dbi_HcuBfdf_WmcStatusUpdate(0, nodeId, DBI_BFDF_SNESOR_STATUS_FAULT_RCV, 0);
//
//	//STREAMID待完善
//	if (gTaskL3bfdfContext.nodeDyn[0][nodeId].nodeStatus < HCU_L3BFDF_NODE_BOARD_STATUS_INIT_MAX)
//		gTaskL3bfdfContext.nodeDyn[0][nodeId].nodeStatus = HCU_L3BFDF_NODE_BOARD_STATUS_INIT_ERR;
//	else
//		//隔离该传感器
//		gTaskL3bfdfContext.nodeDyn[0][nodeId].nodeStatus = HCU_L3BFDF_NODE_BOARD_STATUS_HW_ERROR;
//
//	//是否要根据ERR_CODE，赋予不同的差错情形，待定
//
//	return SUCCESS;
//}

//OPSTAT func_canitfleo_l2frame_msg_bfdf_heart_beat_report_received_handle(StrMsg_HUITP_MSGID_sui_bfdf_heart_beat_report_t *rcv, UINT8 nodeId)
//{
//	//因为没有标准的IE结构，所以这里不能再验证IEID/IELEN的大小段和长度问题
//	//将内容发送给目的模块：暂无。基于目前的情况，等待下位机重启
//
//	//准备组装发送消息
//	StrMsg_HUITP_MSGID_sui_bfdf_heart_beat_confirm_t pMsgProc;
//	UINT16 msgProcLen = sizeof(StrMsg_HUITP_MSGID_sui_bfdf_heart_beat_confirm_t);
//	memset(&pMsgProc, 0, msgProcLen);
//	pMsgProc.msgid = HUITP_ENDIAN_EXG16(HUITP_MSGID_sui_bfdf_heart_beat_confirm);
//	pMsgProc.length = HUITP_ENDIAN_EXG16(msgProcLen - 4);
//	//pMsgProc.wmc_id.wmc_id = nodeId;
////	if (gTaskL3bfdfContext.sensorWs[nodeId].nodeStatus <= HCU_L3BFDF_NODE_BOARD_STATUS_OFFLINE_MAX)
////		pMsgProc.wmcState = HUITP_IEID_SUI_BFDF_HEATT_BEAT_WMC_STATE_OFFLINE;
////	else if (gTaskL3bfdfContext.sensorWs[nodeId].nodeStatus <= HCU_L3BFDF_NODE_BOARD_STATUS_INIT_MAX)
////		pMsgProc.wmcState = HUITP_IEID_SUI_BFDF_HEATT_BEAT_WMC_STATE_INIT;
////	else if (gTaskL3bfdfContext.sensorWs[nodeId].nodeStatus <= HCU_L3BFDF_NODE_BOARD_STATUS_WORK_MAX)
////		pMsgProc.wmcState = HUITP_IEID_SUI_BFDF_HEATT_BEAT_WMC_STATE_WORKING;
////	else
////		pMsgProc.wmcState = HUITP_IEID_SUI_BFDF_HEATT_BEAT_WMC_STATE_INVALID;
//
//	//生成bitmap
//	UINT32 bitmap = ((UINT32)1<<nodeId);
//
//	//发送消息
//	if (hcu_canitfleo_usbcan_l2frame_send((UINT8*)&pMsgProc, msgProcLen, bitmap) == FAILURE)
//		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Send CAN frame error!\n");
//
//	return SUCCESS;
//}

OPSTAT func_canitfleo_l2frame_msg_bfdf_snc_pullin_resp_received_handle(StrMsg_HUITP_MSGID_sui_bfdf_snc_pullin_resp_t *rcv, UINT8 nodeId)
{
	return SUCCESS;
}

OPSTAT func_canitfleo_l2frame_msg_bfdf_basket_clean_ind_received_handle(StrMsg_HUITP_MSGID_sui_bfdf_basket_clean_ind_t *rcv, UINT8 nodeId)
{
	return SUCCESS;
}

#endif//BFDF项目部分


/*
 *
 *
 *  //BFHS项目部分
 *
 *
 *
 */


#if (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFHS_CBU_ID)
OPSTAT fsm_canitfleo_l3bfhs_sys_cfg_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret=0;
	int i=0;

	msg_struct_l3bfhs_can_sys_cfg_req_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_l3bfhs_can_sys_cfg_req_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_l3bfhs_can_sys_cfg_req_t)))
		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);

	//生成bitmap
	UINT32 bitmap = 0;
	for (i=0; i<HCU_SYSMSG_SUI_SENSOR_NBR; i++){
		if (rcv.boardBitmap[i] == TRUE){
			bitmap = ((UINT32)1<<i);
		}
	}

	//准备组装发送消息
	StrMsg_HUITP_MSGID_sui_bfhs_set_config_req_t pMsgProc;
	UINT16 msgProcLen = sizeof(StrMsg_HUITP_MSGID_sui_bfhs_set_config_req_t);
	memset(&pMsgProc, 0, msgProcLen);
	pMsgProc.msgid = HUITP_ENDIAN_EXG16(HUITP_MSGID_sui_bfhs_set_config_req);
	pMsgProc.length = HUITP_ENDIAN_EXG16(msgProcLen - 4);

	pMsgProc.weight_sensor_param.WeightSensorAutoZeroCaptureRangeGrams = HUITP_ENDIAN_EXG32(gTaskL3bfhsContext.wgtSnrPar.WeightSensorAutoZeroCaptureRangeGrams);
	pMsgProc.weight_sensor_param.WeightSensorStandstillRangeGrams = HUITP_ENDIAN_EXG32(gTaskL3bfhsContext.wgtSnrPar.WeightSensorStandstillRangeGrams);
	pMsgProc.weight_sensor_param.MaxAllowedWeight = HUITP_ENDIAN_EXG32(gTaskL3bfhsContext.wgtSnrPar.MaxAllowedWeight);
	pMsgProc.weight_sensor_param.MinAllowedWeight = HUITP_ENDIAN_EXG32(gTaskL3bfhsContext.wgtSnrPar.MinAllowedWeight);
	pMsgProc.weight_sensor_param.WeightSensorFilterCutOffFreqHz = HUITP_ENDIAN_EXG32(gTaskL3bfhsContext.wgtSnrPar.WeightSensorFilterCutOffFreqHz);
	pMsgProc.weight_sensor_param.WeightSensorRingBufTimeMs = HUITP_ENDIAN_EXG16(gTaskL3bfhsContext.wgtSnrPar.WeightSensorRingBufTimeMs);
	pMsgProc.weight_sensor_param.WeightSensorAutoZeroAutotaringTimeMs = HUITP_ENDIAN_EXG16(gTaskL3bfhsContext.wgtSnrPar.WeightSensorAutoZeroAutotaringTimeMs);
	pMsgProc.weight_sensor_param.WeightSensorPreloadComPensationValuePercent = HUITP_ENDIAN_EXG16(gTaskL3bfhsContext.wgtSnrPar.WeightSensorPreloadComPensationValuePercent);
	pMsgProc.weight_sensor_param.WeightSensorPreloadComPensationPlacesAfterDecimalPoint = HUITP_ENDIAN_EXG16(gTaskL3bfhsContext.wgtSnrPar.WeightSensorPreloadComPensationPlacesAfterDecimalPoint);
	pMsgProc.weight_sensor_param.WeightSensorStandstillTimeoutMs = HUITP_ENDIAN_EXG16(gTaskL3bfhsContext.wgtSnrPar.WeightSensorStandstillTimeoutMs);
	pMsgProc.weight_sensor_param.WeightSensorStandstillTime = HUITP_ENDIAN_EXG16(gTaskL3bfhsContext.wgtSnrPar.WeightSensorStandstillTime);
	pMsgProc.weight_sensor_param.WeightSensorMeasurementRangeNo = HUITP_ENDIAN_EXG8(gTaskL3bfhsContext.wgtSnrPar.WeightSensorMeasurementRangeNo);
	pMsgProc.weight_sensor_param.WeightSensorAutoZero = HUITP_ENDIAN_EXG8(gTaskL3bfhsContext.wgtSnrPar.WeightSensorAutoZero);
	pMsgProc.weight_sensor_param.WeightSensorTimeGrid = HUITP_ENDIAN_EXG8(gTaskL3bfhsContext.wgtSnrPar.WeightSensorTimeGrid);
	pMsgProc.weight_sensor_param.WeightSensorAlgoSelect = HUITP_ENDIAN_EXG8(gTaskL3bfhsContext.wgtSnrPar.WeightSensorAlgoSelect);
	pMsgProc.motor_control_param.MotorSpeed = HUITP_ENDIAN_EXG32(gTaskL3bfhsContext.motoCtrlPar.MotorSpeed);
	pMsgProc.motor_control_param.MotorDirection = HUITP_ENDIAN_EXG32(gTaskL3bfhsContext.motoCtrlPar.MotorDirection);
	pMsgProc.arm_control_param.ArmRollingStartMs = HUITP_ENDIAN_EXG32(gTaskL3bfhsContext.armCtrlPar.ArmRollingStartMs);
	pMsgProc.arm_control_param.ArmRollingStopMs = HUITP_ENDIAN_EXG32(gTaskL3bfhsContext.armCtrlPar.ArmRollingStopMs);
	pMsgProc.arm_control_param.ArmRollingInveralMs = HUITP_ENDIAN_EXG32(gTaskL3bfhsContext.armCtrlPar.ArmRollingInveralMs);
	pMsgProc.arm_control_param.ArmFailureDetectionVaration = HUITP_ENDIAN_EXG32(gTaskL3bfhsContext.armCtrlPar.ArmFailureDetectionVaration);
	pMsgProc.arm_control_param.ArmFailureDetectionTimeMs = HUITP_ENDIAN_EXG32(gTaskL3bfhsContext.armCtrlPar.ArmFailureDetectionTimeMs);

	//更新传感器状态
	gTaskL3bfhsContext.sensorWs[0].nodeStatus = HCU_L3BFHS_NODE_BOARD_STATUS_CFG_START_REQ;

	//发送消息：配置消息分成多个分别发送，因为校准参数对于每一个下位机不一样
	if (hcu_canitfleo_usbcan_l2frame_send((UINT8*)&pMsgProc, msgProcLen, bitmap) == FAILURE)
		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Send CAN frame error!\n");

//	//为了测试目的，直接返回
//	msg_struct_can_l3bfhs_sys_cfg_resp_t snd;
//	memset(&snd, 0, sizeof(msg_struct_can_l3bfhs_sys_cfg_resp_t));
//	snd.validFlag = TRUE;
//	snd.length = sizeof(msg_struct_can_l3bfhs_sys_cfg_resp_t);
//	if (hcu_message_send(MSG_ID_CAN_L3BFHS_SYS_CFG_RESP, TASK_ID_L3BFHS, TASK_ID_CANITFLEO, &snd, snd.length) == FAILURE)
//		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName, zHcuVmCtrTab.task[TASK_ID_L3BFHS].taskName);
//
//	//启动测试定时器：暂时不启动，所以也不测试了
//	hcu_timer_start(TASK_ID_CANITFLEO, TIMER_ID_10MS_CANITFLEO_SIMULATION_DATA, zHcuSysEngPar.timer.array[TIMER_ID_10MS_CANITFLEO_SIMULATION_DATA].dur, TIMER_TYPE_PERIOD, TIMER_RESOLUTION_10MS);

	//返回
	return SUCCESS;
}

////往外发送
//OPSTAT fsm_canitfleo_l3bfhs_suspend_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
//{
//	//int ret=0;
//	int i=0;
//
//	msg_struct_l3bfhs_can_sys_suspend_req_t rcv;
//	memset(&rcv, 0, sizeof(msg_struct_l3bfhs_can_sys_suspend_req_t));
//	if ((param_ptr == NULL || param_len > sizeof(msg_struct_l3bfhs_can_sys_suspend_req_t)))
//		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Receive message error!\n");
//	memcpy(&rcv, param_ptr, param_len);
//
//	//生成bitmap
//	UINT32 bitmap = 0;
//	for (i=0; i<HCU_SYSMSG_SUI_SENSOR_NBR; i++){
//		if (rcv.boardBitmap[i] == TRUE){
//			bitmap = ((UINT32)1<<i);
//		}
//	}
//
//	//准备组装发送消息
//	StrMsg_HUITP_MSGID_sui_bfhs_suspend_req_t pMsgProc;
//	UINT16 msgProcLen = sizeof(StrMsg_HUITP_MSGID_sui_bfhs_suspend_req_t);
//	memset(&pMsgProc, 0, msgProcLen);
//	pMsgProc.msgid = HUITP_ENDIAN_EXG16(HUITP_MSGID_sui_bfhs_suspend_req);
//	pMsgProc.length = HUITP_ENDIAN_EXG16(msgProcLen - 4);
//
//	//发送消息
//	if (hcu_canitfleo_usbcan_l2frame_send((UINT8*)&pMsgProc, msgProcLen, bitmap) == FAILURE)
//		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Send CAN frame error!\n");
//
//	//返回
//	return SUCCESS;
//}
//
////往外发送
//OPSTAT fsm_canitfleo_l3bfhs_resume_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
//{
//	//int ret=0;
//	int i=0;
//
//	msg_struct_l3bfhs_can_sys_resume_req_t rcv;
//	memset(&rcv, 0, sizeof(msg_struct_l3bfhs_can_sys_resume_req_t));
//	if ((param_ptr == NULL || param_len > sizeof(msg_struct_l3bfhs_can_sys_resume_req_t)))
//		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Receive message error!\n");
//	memcpy(&rcv, param_ptr, param_len);
//
//	//生成bitmap
//	UINT32 bitmap = 0;
//	for (i=0; i<HCU_SYSMSG_SUI_SENSOR_NBR; i++){
//		if (rcv.boardBitmap[i] == TRUE){
//			bitmap = ((UINT32)1<<i);
//		}
//	}
//
//	//准备组装发送消息
//	StrMsg_HUITP_MSGID_sui_bfhs_resume_req_t pMsgProc;
//	UINT16 msgProcLen = sizeof(StrMsg_HUITP_MSGID_sui_bfhs_resume_req_t);
//	memset(&pMsgProc, 0, msgProcLen);
//	pMsgProc.msgid = HUITP_ENDIAN_EXG16(HUITP_MSGID_sui_bfhs_resume_req);
//	pMsgProc.length = HUITP_ENDIAN_EXG16(msgProcLen - 4);
//
//	//发送消息
//	if (hcu_canitfleo_usbcan_l2frame_send((UINT8*)&pMsgProc, msgProcLen, bitmap) == FAILURE)
//		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Send CAN frame error!\n");
//
//	//返回
//	return SUCCESS;
//}

//往外发送
OPSTAT fsm_canitfleo_l3bfhs_cal_zero_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret=0;
	int i=0;

	msg_struct_l3bfhs_can_cal_zero_req_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_l3bfhs_can_cal_zero_req_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_l3bfhs_can_cal_zero_req_t)))
		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);

	//生成bitmap
	UINT32 bitmap = 0;
	for (i=0; i<HCU_SYSMSG_SUI_SENSOR_NBR; i++){
		if (rcv.boardBitmap[i] == TRUE){
			bitmap = ((UINT32)1<<i);
		}
	}

	//准备组装发送消息
	StrMsg_HUITP_MSGID_sui_bfhs_calibration_zero_req_t pMsgProc;
	UINT16 msgProcLen = sizeof(StrMsg_HUITP_MSGID_sui_bfhs_calibration_zero_req_t);
	memset(&pMsgProc, 0, msgProcLen);
	pMsgProc.msgid = HUITP_ENDIAN_EXG16(HUITP_MSGID_sui_bfhs_calibration_zero_req);
	pMsgProc.length = HUITP_ENDIAN_EXG16(msgProcLen - 4);
	pMsgProc.weight_sensor_calibration_zero.WeightSensorFilterCutOffFreqHz = HUITP_ENDIAN_EXG32(rcv.calZeroPar.WeightSensorFilterCutOffFreqHz);
	pMsgProc.weight_sensor_calibration_zero.WeightSensorAutoZeroCaptureRangeGrams = HUITP_ENDIAN_EXG32(rcv.calZeroPar.WeightSensorAutoZeroCaptureRangeGrams);
	pMsgProc.weight_sensor_calibration_zero.WeightSensorStandstillRangeGrams = HUITP_ENDIAN_EXG32(rcv.calZeroPar.WeightSensorStandstillRangeGrams);
	pMsgProc.weight_sensor_calibration_zero.WeightSensorAutoZeroAutotaringTimeMs = HUITP_ENDIAN_EXG16(rcv.calZeroPar.WeightSensorAutoZeroAutotaringTimeMs);
	pMsgProc.weight_sensor_calibration_zero.WeightSensorPreloadComPensationValuePercent = HUITP_ENDIAN_EXG16(rcv.calZeroPar.WeightSensorPreloadComPensationValuePercent);
	pMsgProc.weight_sensor_calibration_zero.WeightSensorPreloadComPensationPlacesAfterDecimalPoint = HUITP_ENDIAN_EXG16(rcv.calZeroPar.WeightSensorPreloadComPensationPlacesAfterDecimalPoint);
	pMsgProc.weight_sensor_calibration_zero.WeightSensorStandstillTimeoutMs = HUITP_ENDIAN_EXG16(rcv.calZeroPar.WeightSensorStandstillTimeoutMs);
	pMsgProc.weight_sensor_calibration_zero.WeightSensorStandstillTime = HUITP_ENDIAN_EXG16(rcv.calZeroPar.WeightSensorStandstillTime);
	pMsgProc.weight_sensor_calibration_zero.WeightSensorRingBufTimeMs = HUITP_ENDIAN_EXG16(rcv.calZeroPar.WeightSensorRingBufTimeMs);
	pMsgProc.weight_sensor_calibration_zero.WeightSensorMeasurementRangeNo = HUITP_ENDIAN_EXG8(rcv.calZeroPar.WeightSensorMeasurementRangeNo);
	pMsgProc.weight_sensor_calibration_zero.WeightSensorAutoZero = HUITP_ENDIAN_EXG8(rcv.calZeroPar.WeightSensorAutoZero);

	//发送消息
	if (hcu_canitfleo_usbcan_l2frame_send((UINT8*)&pMsgProc, msgProcLen, bitmap) == FAILURE)
		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Send CAN frame error!\n");

	//返回
	return SUCCESS;
}

//往外发送
OPSTAT fsm_canitfleo_l3bfhs_cal_full_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret=0;
	int i=0;

	msg_struct_l3bfhs_can_cal_full_req_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_l3bfhs_can_cal_full_req_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_l3bfhs_can_cal_full_req_t)))
		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);

	//生成bitmap
	UINT32 bitmap = 0;
	for (i=0; i<HCU_SYSMSG_SUI_SENSOR_NBR; i++){
		if (rcv.boardBitmap[i] == TRUE){
			bitmap = ((UINT32)1<<i);
		}
	}

	//准备组装发送消息
	StrMsg_HUITP_MSGID_sui_bfhs_calibration_full_req_t pMsgProc;
	UINT16 msgProcLen = sizeof(StrMsg_HUITP_MSGID_sui_bfhs_calibration_full_req_t);
	memset(&pMsgProc, 0, msgProcLen);
	pMsgProc.msgid = HUITP_ENDIAN_EXG16(HUITP_MSGID_sui_bfhs_calibration_full_req);
	pMsgProc.length = HUITP_ENDIAN_EXG16(msgProcLen - 4);
	pMsgProc.weight_sensor_calibration_full.WeightSensorAdjustingWeightGrams = HUITP_ENDIAN_EXG32(rcv.calFullPar.WeightSensorAdjustingWeightGrams);
	pMsgProc.weight_sensor_calibration_full.WeightSensorAdjustingTolerancePercent = HUITP_ENDIAN_EXG32(rcv.calFullPar.WeightSensorAdjustingTolerancePercent);

	//发送消息
	if (hcu_canitfleo_usbcan_l2frame_send((UINT8*)&pMsgProc, msgProcLen, bitmap) == FAILURE)
		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Send CAN frame error!\n");

	//返回
	return SUCCESS;
}

//往外发送
//OPSTAT fsm_canitfleo_l3bfhs_test_cmd_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
//{
//	//int ret=0;
//	//int i=0;
//
//	msg_struct_uicomm_can_test_cmd_req_t rcv;
//	memset(&rcv, 0, sizeof(msg_struct_uicomm_can_test_cmd_req_t));
//	if ((param_ptr == NULL || param_len > sizeof(msg_struct_uicomm_can_test_cmd_req_t)))
//		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Receive message error!\n");
//	memcpy(&rcv, param_ptr, param_len);
//
//	//返回
//	return SUCCESS;
//}

//发送测试数据给L3BFHS
OPSTAT func_canitfleo_bfhs_simulation_data_process(void)
{
//	int ret = 0;

	//这里的定时工作，是为了测试目标。
	//这里的定时已经启动，是为了定时产生假测试数据，喂给L3BFSC模块，以便测试算法
	msg_struct_can_l3bfhs_new_ready_event_t snd;
	memset(&snd, 0, sizeof(msg_struct_can_l3bfhs_new_ready_event_t));
	snd.length = sizeof(msg_struct_can_l3bfhs_new_ready_event_t);
	snd.snrWsValue = (rand()%900+100)*100;
	if (hcu_message_send(MSG_ID_CAN_L3BFHS_WS_NEW_READY_EVENT, TASK_ID_L3BFHS, TASK_ID_CANITFLEO, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName, zHcuVmCtrTab.task[TASK_ID_L3BFHS].taskName);

	return SUCCESS;
}

//接收消息处理部分
//OPSTAT func_canitfleo_l2frame_msg_bfhs_startup_ind_received_handle(StrMsg_HUITP_MSGID_sui_bfhs_startup_ind_t *rcv, UINT8 nodeId)
//{
//	//Firstly Register state
//	gTaskL3bfhsContext.sensorWs[0].nodeStatus = HCU_L3BFHS_NODE_BOARD_STATUS_STARTUP;
//	HCU_DEBUG_PRINT_CRT("CANITFLEO: Sensor ID = %d is set to be startup!\n", nodeId);
//
//	//通知界面
//	func_canitfleo_huicobus_trigger_uir(0, 0);
//	//dbi_HcuBfsc_WmcStatusUpdate(0, nodeId, DBI_BFHS_SNESOR_STATUS_STARTUP, 0);
//
//	//系统已经过了初始化过程
//	if ((FsmGetState(TASK_ID_L3BFHS) == FSM_STATE_L3BFHS_ACTIVED) || (FsmGetState(TASK_ID_L3BFHS) == FSM_STATE_L3BFHS_OOS_SCAN))
//	{
//		//生成bitmap
//		UINT32 bitmap = 0;
//		bitmap = (1<<nodeId);
//
//		//准备组装发送消息
//		StrMsg_HUITP_MSGID_sui_bfhs_set_config_req_t pMsgProc;
//		UINT16 msgProcLen = sizeof(StrMsg_HUITP_MSGID_sui_bfhs_set_config_req_t);
//		memset(&pMsgProc, 0, msgProcLen);
//		pMsgProc.msgid = HUITP_ENDIAN_EXG16(HUITP_MSGID_sui_bfhs_set_config_req);
//		pMsgProc.length = HUITP_ENDIAN_EXG16(msgProcLen - 4);
//
//		pMsgProc.weight_sensor_param.WeightSensorAutoZeroCaptureRangeGrams = HUITP_ENDIAN_EXG32(gTaskL3bfhsContext.wgtSnrPar.WeightSensorAutoZeroCaptureRangeGrams);
//		pMsgProc.weight_sensor_param.WeightSensorStandstillRangeGrams = HUITP_ENDIAN_EXG32(gTaskL3bfhsContext.wgtSnrPar.WeightSensorStandstillRangeGrams);
//		pMsgProc.weight_sensor_param.MaxAllowedWeight = HUITP_ENDIAN_EXG32(gTaskL3bfhsContext.wgtSnrPar.MaxAllowedWeight);
//		pMsgProc.weight_sensor_param.MinAllowedWeight = HUITP_ENDIAN_EXG32(gTaskL3bfhsContext.wgtSnrPar.MinAllowedWeight);
//		pMsgProc.weight_sensor_param.WeightSensorFilterCutOffFreqHz = HUITP_ENDIAN_EXG32(gTaskL3bfhsContext.wgtSnrPar.WeightSensorFilterCutOffFreqHz);
//		pMsgProc.weight_sensor_param.WeightSensorRingBufTimeMs = HUITP_ENDIAN_EXG16(gTaskL3bfhsContext.wgtSnrPar.WeightSensorRingBufTimeMs);
//		pMsgProc.weight_sensor_param.WeightSensorAutoZeroAutotaringTimeMs = HUITP_ENDIAN_EXG16(gTaskL3bfhsContext.wgtSnrPar.WeightSensorAutoZeroAutotaringTimeMs);
//		pMsgProc.weight_sensor_param.WeightSensorPreloadComPensationValuePercent = HUITP_ENDIAN_EXG16(gTaskL3bfhsContext.wgtSnrPar.WeightSensorPreloadComPensationValuePercent);
//		pMsgProc.weight_sensor_param.WeightSensorPreloadComPensationPlacesAfterDecimalPoint = HUITP_ENDIAN_EXG16(gTaskL3bfhsContext.wgtSnrPar.WeightSensorPreloadComPensationPlacesAfterDecimalPoint);
//		pMsgProc.weight_sensor_param.WeightSensorStandstillTimeoutMs = HUITP_ENDIAN_EXG16(gTaskL3bfhsContext.wgtSnrPar.WeightSensorStandstillTimeoutMs);
//		pMsgProc.weight_sensor_param.WeightSensorStandstillTime = HUITP_ENDIAN_EXG16(gTaskL3bfhsContext.wgtSnrPar.WeightSensorStandstillTime);
//		pMsgProc.weight_sensor_param.WeightSensorMeasurementRangeNo = HUITP_ENDIAN_EXG8(gTaskL3bfhsContext.wgtSnrPar.WeightSensorMeasurementRangeNo);
//		pMsgProc.weight_sensor_param.WeightSensorAutoZero = HUITP_ENDIAN_EXG8(gTaskL3bfhsContext.wgtSnrPar.WeightSensorAutoZero);
//		pMsgProc.weight_sensor_param.WeightSensorTimeGrid = HUITP_ENDIAN_EXG8(gTaskL3bfhsContext.wgtSnrPar.WeightSensorTimeGrid);
//		pMsgProc.weight_sensor_param.WeightSensorAlgoSelect = HUITP_ENDIAN_EXG8(gTaskL3bfhsContext.wgtSnrPar.WeightSensorAlgoSelect);
//		pMsgProc.motor_control_param.MotorSpeed = HUITP_ENDIAN_EXG32(gTaskL3bfhsContext.motoCtrlPar.MotorSpeed);
//		pMsgProc.motor_control_param.MotorDirection = HUITP_ENDIAN_EXG32(gTaskL3bfhsContext.motoCtrlPar.MotorDirection);
//		pMsgProc.arm_control_param.ArmRollingStartMs = HUITP_ENDIAN_EXG32(gTaskL3bfhsContext.armCtrlPar.ArmRollingStartMs);
//		pMsgProc.arm_control_param.ArmRollingStopMs = HUITP_ENDIAN_EXG32(gTaskL3bfhsContext.armCtrlPar.ArmRollingStopMs);
//		pMsgProc.arm_control_param.ArmRollingInveralMs = HUITP_ENDIAN_EXG32(gTaskL3bfhsContext.armCtrlPar.ArmRollingInveralMs);
//		pMsgProc.arm_control_param.ArmFailureDetectionVaration = HUITP_ENDIAN_EXG32(gTaskL3bfhsContext.armCtrlPar.ArmFailureDetectionVaration);
//		pMsgProc.arm_control_param.ArmFailureDetectionTimeMs = HUITP_ENDIAN_EXG32(gTaskL3bfhsContext.armCtrlPar.ArmFailureDetectionTimeMs);
//
//		//发送消息
//		if (hcu_canitfleo_usbcan_l2frame_send((UINT8*)&pMsgProc, msgProcLen, bitmap) == FAILURE)
//			HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Send CAN frame error!\n");
//
//		//更新传感器状态
//		gTaskL3bfhsContext.sensorWs[0].nodeStatus = HCU_L3BFHS_NODE_BOARD_STATUS_CFG_START_REQ;
//	}
//
//	return SUCCESS;
//}

//接收消息处理部分
OPSTAT func_canitfleo_l2frame_msg_bfhs_calibration_zero_resp_received_handle(StrMsg_HUITP_MSGID_sui_bfhs_calibration_zero_resp_t *rcv, UINT8 nodeId)
{
	//通知界面
	func_canitfleo_huicobus_trigger_uir(0, 0);
	//dbi_HcuBfsc_WmcStatusUpdate(0, nodeId, DBI_BFHS_SNESOR_STATUS_STARTUP, 0);

	//发送到L3BFHS
	msg_struct_can_l3bfhs_cal_zero_resp_t snd;
	memset(&snd, 0, sizeof(msg_struct_can_l3bfhs_cal_zero_resp_t));
	snd.validFlag = HUITP_ENDIAN_EXG8(rcv->validFlag);
	snd.errCode = HUITP_ENDIAN_EXG16(rcv->errCode);
	snd.length = sizeof(msg_struct_can_l3bfhs_cal_zero_resp_t);
	if (hcu_message_send(MSG_ID_CAN_L3BFHS_CAL_ZERO_RESP, TASK_ID_L3BFHS, TASK_ID_CANITFLEO, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName, zHcuVmCtrTab.task[TASK_ID_L3BFHS].taskName);

	return SUCCESS;
}

//接收消息处理部分
OPSTAT func_canitfleo_l2frame_msg_bfhs_calibration_full_resp_received_handle(StrMsg_HUITP_MSGID_sui_bfhs_calibration_full_resp_t *rcv, UINT8 nodeId)
{
	//通知界面
	func_canitfleo_huicobus_trigger_uir(0, 0);
	//dbi_HcuBfsc_WmcStatusUpdate(0, nodeId, DBI_BFHS_SNESOR_STATUS_STARTUP, 0);

	//发送到L3BFHS
	msg_struct_can_l3bfhs_cal_full_resp_t snd;
	memset(&snd, 0, sizeof(msg_struct_can_l3bfhs_cal_full_resp_t));
	snd.validFlag = HUITP_ENDIAN_EXG8(rcv->validFlag);
	snd.errCode = HUITP_ENDIAN_EXG16(rcv->errCode);
	snd.calFullRespPar.WeightSensorFilterCutOffFreqHz = HUITP_ENDIAN_EXG32(rcv->weight_sensor_calibration_full.WeightSensorFilterCutOffFreqHz);
	snd.calFullRespPar.WeightSensorCurrentZeroPointGrams = HUITP_ENDIAN_EXG32(rcv->weight_sensor_calibration_full.WeightSensorCurrentZeroPointGrams);
	snd.calFullRespPar.WeightSensorReferenceZeroPointGrams = HUITP_ENDIAN_EXG32(rcv->weight_sensor_calibration_full.WeightSensorReferenceZeroPointGrams);
	snd.calFullRespPar.WeightSensorNegativeZeroSettingRangeGrams = HUITP_ENDIAN_EXG32(rcv->weight_sensor_calibration_full.WeightSensorNegativeZeroSettingRangeGrams);
	snd.calFullRespPar.WeightSensorPositiveZeroSettingRangeGrams = HUITP_ENDIAN_EXG32(rcv->weight_sensor_calibration_full.WeightSensorPositiveZeroSettingRangeGrams);
	snd.calFullRespPar.WeightSensorMeasurementRange = HUITP_ENDIAN_EXG32(rcv->weight_sensor_calibration_full.WeightSensorMeasurementRange);
	snd.calFullRespPar.WeightSensorScaleIntervalValue = HUITP_ENDIAN_EXG32(rcv->weight_sensor_calibration_full.WeightSensorScaleIntervalValue);
	snd.calFullRespPar.WeightSensorCalibrationValue = HUITP_ENDIAN_EXG32(rcv->weight_sensor_calibration_full.WeightSensorCalibrationValue);
	snd.calFullRespPar.WeightSensorAutoZeroCaptureRangeGrams = HUITP_ENDIAN_EXG32(rcv->weight_sensor_calibration_full.WeightSensorAutoZeroCaptureRangeGrams);
	snd.calFullRespPar.WeightSensorAdjustingWeightGrams = HUITP_ENDIAN_EXG32(rcv->weight_sensor_calibration_full.WeightSensorAdjustingWeightGrams);
	snd.calFullRespPar.WeightSensorAdjustingFactor = HUITP_ENDIAN_EXG32(rcv->weight_sensor_calibration_full.WeightSensorAdjustingFactor);
	snd.calFullRespPar.WeightSensorAdjustingTolerancePercent = HUITP_ENDIAN_EXG32(rcv->weight_sensor_calibration_full.WeightSensorAdjustingTolerancePercent);
	snd.calFullRespPar.WeightSensorStandstillRangeGrams = HUITP_ENDIAN_EXG32(rcv->weight_sensor_calibration_full.WeightSensorStandstillRangeGrams);
	snd.calFullRespPar.WeightSensorTemperatureInMagnetSystem = HUITP_ENDIAN_EXG16(rcv->weight_sensor_calibration_full.WeightSensorTemperatureInMagnetSystem);
	snd.calFullRespPar.WeightSensorTemperatureAtMeasuringShunt = HUITP_ENDIAN_EXG16(rcv->weight_sensor_calibration_full.WeightSensorTemperatureAtMeasuringShunt);
	snd.calFullRespPar.WeightSensorSamplingFreqHz = HUITP_ENDIAN_EXG16(rcv->weight_sensor_calibration_full.WeightSensorSamplingFreqHz);
	snd.calFullRespPar.WeightSensorRingBufTimeMs = HUITP_ENDIAN_EXG16(rcv->weight_sensor_calibration_full.WeightSensorRingBufTimeMs);
	snd.calFullRespPar.WeightSensorAutoZeroAutotaringTimeMs = HUITP_ENDIAN_EXG16(rcv->weight_sensor_calibration_full.WeightSensorAutoZeroAutotaringTimeMs);
	snd.calFullRespPar.WeightSensorPreloadComPensationValuePercent = HUITP_ENDIAN_EXG16(rcv->weight_sensor_calibration_full.WeightSensorPreloadComPensationValuePercent);
	snd.calFullRespPar.WeightSensorPreloadComPensationPlacesAfterDecimalPoint = HUITP_ENDIAN_EXG16(rcv->weight_sensor_calibration_full.WeightSensorPreloadComPensationPlacesAfterDecimalPoint);
	snd.calFullRespPar.WeightSensorStandstillTimeoutMs = HUITP_ENDIAN_EXG16(rcv->weight_sensor_calibration_full.WeightSensorStandstillTimeoutMs);
	snd.calFullRespPar.WeightSensorStandstillTime = HUITP_ENDIAN_EXG16(rcv->weight_sensor_calibration_full.WeightSensorStandstillTime);
	snd.calFullRespPar.WeightSensorMeasurementRangeNo = HUITP_ENDIAN_EXG8(rcv->weight_sensor_calibration_full.WeightSensorMeasurementRangeNo);
	snd.calFullRespPar.WeightSensorPlacesAfterDecimalPoint = HUITP_ENDIAN_EXG8(rcv->weight_sensor_calibration_full.WeightSensorPlacesAfterDecimalPoint);
	memcpy(snd.calFullRespPar.WeightSensorUintString,  rcv->weight_sensor_calibration_full.WeightSensorUintString, 64);
	snd.calFullRespPar.WeightSensorAutoZero = HUITP_ENDIAN_EXG8(rcv->weight_sensor_calibration_full.WeightSensorAutoZero);
	snd.calFullRespPar.WeightSensorCellAddress = HUITP_ENDIAN_EXG8(rcv->weight_sensor_calibration_full.WeightSensorCellAddress);
	snd.calFullRespPar.WeightSensorTimeGrid = HUITP_ENDIAN_EXG8(rcv->weight_sensor_calibration_full.WeightSensorTimeGrid);
	snd.calFullRespPar.spare2 = HUITP_ENDIAN_EXG8(rcv->weight_sensor_calibration_full.spare2);

	snd.length = sizeof(msg_struct_can_l3bfhs_cal_full_resp_t);
	if (hcu_message_send(MSG_ID_CAN_L3BFHS_CAL_FULL_RESP, TASK_ID_L3BFHS, TASK_ID_CANITFLEO, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName, zHcuVmCtrTab.task[TASK_ID_L3BFHS].taskName);

	return SUCCESS;
}

OPSTAT func_canitfleo_l2frame_msg_bfhs_set_config_resp_received_handle(StrMsg_HUITP_MSGID_sui_bfhs_set_config_resp_t *rcv, UINT8 nodeId)
{
	//通知界面
	if (rcv->validFlag == FALSE){
		func_canitfleo_huicobus_trigger_uir(0, 0);
		//dbi_HcuBfhs_WmcStatusUpdate(0, nodeId, DBI_BFSC_SNESOR_STATUS_CONFIG_ERR, 0);
	}
	else{
		func_canitfleo_huicobus_trigger_uir(0, 0);
		//dbi_HcuBfhs_WmcStatusUpdate(0, nodeId, DBI_BFSC_SNESOR_STATUS_CONFIG_RCV, 0);
	}

	//进入工作过程
	msg_struct_can_l3bfhs_sys_cfg_resp_t snd;
	memset(&snd, 0, sizeof(msg_struct_can_l3bfhs_sys_cfg_resp_t));
	snd.validFlag = HUITP_ENDIAN_EXG8(rcv->validFlag);
	snd.errCode = HUITP_ENDIAN_EXG16(rcv->errCode);
	snd.length = sizeof(msg_struct_can_l3bfhs_sys_cfg_resp_t);
	if (hcu_message_send(MSG_ID_CAN_L3BFHS_SYS_CFG_RESP, TASK_ID_L3BFHS, TASK_ID_CANITFLEO, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName, zHcuVmCtrTab.task[TASK_ID_L3BFHS].taskName);

	//更新状态
	gTaskL3bfhsContext.sensorWs[0].nodeStatus = HCU_L3BFHS_NODE_BOARD_STATUS_CFG_START_CMPL;

	return SUCCESS;
}

//OPSTAT func_canitfleo_l2frame_msg_bfhs_suspend_resp_received_handle(StrMsg_HUITP_MSGID_sui_bfhs_suspend_resp_t *rcv, UINT8 nodeId)
//{
//	//通知界面
//	func_canitfleo_huicobus_trigger_uir(0, 0);
//	//dbi_HcuBfhs_WmcStatusUpdate(0, nodeId, DBI_BFHS_SNESOR_STATUS_SUSPEND_RCV, 0);
//
//	//系统初始化过程
//	msg_struct_can_l3bfhs_sys_suspend_resp_t snd;
//	memset(&snd, 0, sizeof(msg_struct_can_l3bfhs_sys_suspend_resp_t));
//	snd.validFlag = HUITP_ENDIAN_EXG8(rcv->validFlag);
//	snd.errCode = HUITP_ENDIAN_EXG16(rcv->errCode);
//	snd.length = sizeof(msg_struct_can_l3bfhs_sys_suspend_resp_t);
//	if (hcu_message_send(MSG_ID_CAN_L3BFHS_SYS_SUSPEND_RESP, TASK_ID_L3BFHS, TASK_ID_CANITFLEO, &snd, snd.length) == FAILURE)
//		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName, zHcuVmCtrTab.task[TASK_ID_L3BFHS].taskName);
//
//	//系统已经在工作状态：可以正式工作了
//	else{
//		gTaskL3bfhsContext.sensorWs[0].nodeStatus = HCU_L3BFHS_NODE_BOARD_STATUS_SUSPEND;
//	}
//
//	return SUCCESS;
//}
//
//OPSTAT func_canitfleo_l2frame_msg_bfhs_resume_resp_received_handle(StrMsg_HUITP_MSGID_sui_bfhs_resume_resp_t *rcv, UINT8 nodeId)
//{
//	//通知界面
//	func_canitfleo_huicobus_trigger_uir(0, 0);
//	//dbi_HcuBfhs_WmcStatusUpdate(0, nodeId, DBI_BFHS_SNESOR_STATUS_RESUME_RCV, 0);
//
//	//系统初始化过程
//	msg_struct_can_l3bfhs_sys_resume_resp_t snd;
//	memset(&snd, 0, sizeof(msg_struct_can_l3bfhs_sys_resume_resp_t));
//	snd.validFlag = HUITP_ENDIAN_EXG8(rcv->validFlag);
//	snd.length = sizeof(msg_struct_can_l3bfhs_sys_resume_resp_t);
//	if (hcu_message_send(MSG_ID_CAN_L3BFHS_SYS_RESUME_RESP, TASK_ID_L3BFHS, TASK_ID_CANITFLEO, &snd, snd.length) == FAILURE)
//		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName, zHcuVmCtrTab.task[TASK_ID_L3BFHS].taskName);
//
//	//系统已经在工作状态：可以正式工作了
//	else{
//		gTaskL3bfhsContext.sensorWs[0].nodeStatus = HCU_L3BFHS_NODE_BOARD_STATUS_VALID;
//	}
//
//	return SUCCESS;
//}

OPSTAT func_canitfleo_l2frame_msg_bfhs_new_ws_event_received_handle(StrMsg_HUITP_MSGID_sui_bfhs_new_ws_event_t *rcv, UINT8 nodeId)
{
	//因为没有标准的IE结构，所以这里不能再验证IEID/IELEN的大小段和长度问题
	//将内容发送给目的模块，具体内容是否越界／合理，均由L3模块进行处理

	//通知界面
	func_canitfleo_huicobus_trigger_uir(0, 0);
	//dbi_HcuBfhs_WmcStatusUpdate(0, nodeId, DBI_BFHS_SNESOR_STATUS_DATA_VALID, HUITP_ENDIAN_EXG32(rcv->weight_ind.average_weight));

	//发送到L3BFHS
	msg_struct_can_l3bfhs_new_ready_event_t snd;
	memset(&snd, 0, sizeof(msg_struct_can_l3bfhs_new_ready_event_t));
	snd.snrWsValue = HUITP_ENDIAN_EXG32(rcv->Weight);
	snd.snrState = HUITP_ENDIAN_EXG8(rcv->weight_state);
	snd.length = sizeof(msg_struct_can_l3bfhs_new_ready_event_t);
	if (hcu_message_send(MSG_ID_CAN_L3BFHS_WS_NEW_READY_EVENT, TASK_ID_L3BFHS, TASK_ID_CANITFLEO, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName, zHcuVmCtrTab.task[TASK_ID_L3BFHS].taskName);

	return SUCCESS;
}

//OPSTAT func_canitfleo_l2frame_msg_bfhs_command_resp_received_handle(StrMsg_HUITP_MSGID_sui_bfhs_command_resp_t *rcv, UINT8 nodeId)
//{
//	//因为没有标准的IE结构，所以这里不能再验证IEID/IELEN的大小段和长度问题
//	//将内容发送给目的模块
//	//通知界面
//	func_canitfleo_huicobus_trigger_uir(0, 0);
//	//dbi_HcuBfhs_WmcStatusUpdate(0, nodeId, DBI_BFHS_SNESOR_STATUS_TEST_CMD_RCV, 0);
//
//	msg_struct_can_uicomm_test_cmd_resp_t snd;
//	memset(&snd, 0, sizeof(msg_struct_can_uicomm_test_cmd_resp_t));
//	snd.validFlag = HUITP_ENDIAN_EXG8(rcv->validFlag);
//	snd.errCode = HUITP_ENDIAN_EXG16(rcv->result.error_code);
//	snd.sensorid = nodeId;
//	snd.cmdid = HUITP_ENDIAN_EXG32(rcv->cmdid);
//	snd.cmdvalue1 = HUITP_ENDIAN_EXG32(rcv->cmdvalue1);
//	snd.cmdvalue2 = HUITP_ENDIAN_EXG32(rcv->cmdvalue2);
//	snd.length = sizeof(msg_struct_can_uicomm_test_cmd_resp_t);
//
//	HCU_DEBUG_PRINT_CRT("CANITFLEO: Test Cmd received, NodeID=%d, CmdId=%d, Value1=%d, Value2=%d\n", nodeId, snd.cmdid, snd.cmdvalue1, snd.cmdvalue2);
//	if (hcu_message_send(MSG_ID_CAN_UICOMM_TEST_CMD_RESP, TASK_ID_BFDFUICOMM, TASK_ID_CANITFLEO, &snd, snd.length) == FAILURE)
//		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName, zHcuVmCtrTab.task[TASK_ID_BFDFUICOMM].taskName);
//
//	return SUCCESS;
//}

//OPSTAT func_canitfleo_l2frame_msg_bfhs_fault_ind_received_handle(StrMsg_HUITP_MSGID_sui_bfhs_fault_ind_t *rcv, UINT8 nodeId)
//{
//	//因为没有标准的IE结构，所以这里不能再验证IEID/IELEN的大小段和长度问题
//	//只更新传感器的状态，不做其它处理
//
//	//通知界面
//	func_canitfleo_huicobus_trigger_uir(0, 0);
//	//dbi_HcuBfhs_WmcStatusUpdate(0, nodeId, DBI_BFDF_SNESOR_STATUS_FAULT_RCV, 0);
//
//	if (gTaskL3bfhsContext.sensorWs[0].nodeStatus < HCU_L3BFHS_NODE_BOARD_STATUS_INIT_MAX)
//		gTaskL3bfhsContext.sensorWs[0].nodeStatus = HCU_L3BFHS_NODE_BOARD_STATUS_INIT_ERR;
//	else
//		//隔离该传感器
//		gTaskL3bfhsContext.sensorWs[0].nodeStatus = HCU_L3BFHS_NODE_BOARD_STATUS_HW_ERROR;
//
//	//是否要根据ERR_CODE，赋予不同的差错情形，待定
//
//	return SUCCESS;
//}

//OPSTAT func_canitfleo_l2frame_msg_bfhs_heart_beat_report_received_handle(StrMsg_HUITP_MSGID_sui_bfhs_heart_beat_report_t *rcv, UINT8 nodeId)
//{
//	//因为没有标准的IE结构，所以这里不能再验证IEID/IELEN的大小段和长度问题
//	//将内容发送给目的模块：暂无。基于目前的情况，等待下位机重启
//
//	//准备组装发送消息
//	StrMsg_HUITP_MSGID_sui_bfhs_heart_beat_confirm_t pMsgProc;
//	UINT16 msgProcLen = sizeof(StrMsg_HUITP_MSGID_sui_bfhs_heart_beat_confirm_t);
//	memset(&pMsgProc, 0, msgProcLen);
//	pMsgProc.msgid = HUITP_ENDIAN_EXG16(HUITP_MSGID_sui_bfhs_heart_beat_confirm);
//	pMsgProc.length = HUITP_ENDIAN_EXG16(msgProcLen - 4);
//	//pMsgProc.wmc_id.wmc_id = nodeId;
////	if (gTaskL3bfhsContext.sensorWs[nodeId].nodeStatus <= HCU_L3BFHS_NODE_BOARD_STATUS_OFFLINE_MAX)
////		pMsgProc.wmcState = HUITP_IEID_SUI_BFHS_HEATT_BEAT_WMC_STATE_OFFLINE;
////	else if (gTaskL3bfhsContext.sensorWs[nodeId].nodeStatus <= HCU_L3BFHS_NODE_BOARD_STATUS_INIT_MAX)
////		pMsgProc.wmcState = HUITP_IEID_SUI_BFHS_HEATT_BEAT_WMC_STATE_INIT;
////	else if (gTaskL3bfhsContext.sensorWs[nodeId].nodeStatus <= HCU_L3BFHS_NODE_BOARD_STATUS_WORK_MAX)
////		pMsgProc.wmcState = HUITP_IEID_SUI_BFHS_HEATT_BEAT_WMC_STATE_WORKING;
////	else
////		pMsgProc.wmcState = HUITP_IEID_SUI_BFHS_HEATT_BEAT_WMC_STATE_INVALID;
//
//	//生成bitmap
//	UINT32 bitmap = ((UINT32)1<<nodeId);
//
//	//发送消息
//	if (hcu_canitfleo_usbcan_l2frame_send((UINT8*)&pMsgProc, msgProcLen, bitmap) == FAILURE)
//		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Send CAN frame error!\n");
//
//	return SUCCESS;
//}

#endif//BFHS项目部分


/*
 *
 *
 *  //多项目共享部分
 *
 *
 *
 */

#if ((HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFSC_CBU_ID) || \
		(HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFDF_CBU_ID) || \
		(HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFHS_CBU_ID))
//通用过程
OPSTAT fsm_canitfleo_sysswm_inventory_confirm(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret=0;

	msg_struct_sysswm_canitfleo_inventory_confirm_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_sysswm_canitfleo_inventory_confirm_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_sysswm_canitfleo_inventory_confirm_t)))
		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);

	//入参检查
#if (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFSC_CBU_ID)
	if (rcv.nodeId >= HCU_SYSCFG_BFSC_SNR_WS_NBR_MAX)  HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Error parameter set!\n");
#endif
	UINT32 bitmap = ((UINT32)1<<rcv.nodeId);

	//准备组装发送消息
	StrMsg_HUITP_MSGID_sui_inventory_confirm_t pMsgProc;
	UINT16 msgProcLen = sizeof(StrMsg_HUITP_MSGID_sui_inventory_confirm_t);
	memset(&pMsgProc, 0, msgProcLen);
	pMsgProc.msgid = HUITP_ENDIAN_EXG16(HUITP_MSGID_sui_inventory_confirm);
	pMsgProc.length = HUITP_ENDIAN_EXG16(msgProcLen - 4);

	pMsgProc.swRel = HUITP_ENDIAN_EXG16(rcv.swRel);
	pMsgProc.swVer = HUITP_ENDIAN_EXG16(rcv.swVer);
	pMsgProc.upgradeFlag = rcv.upgradeFlag;
	pMsgProc.swCheckSum = HUITP_ENDIAN_EXG16(rcv.swCheckSum);
	pMsgProc.swTotalLengthInBytes = HUITP_ENDIAN_EXG32(rcv.swTotalLengthInBytes);

	//发送消息：配置消息分成多个分别发送，因为校准参数对于每一个下位机不一样
	if (hcu_canitfleo_usbcan_l2frame_send((UINT8*)&pMsgProc, msgProcLen, bitmap) == FAILURE)
		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Send CAN frame error!\n");

	HCU_DEBUG_PRINT_INF("CANITFLEO: Send INVERNTORY CONFIRM, REL/VER/UFLAG/CHKSUM/TLEN=[%d/%d/%d/%d/%d]\n", pMsgProc.swRel, pMsgProc.swVer, pMsgProc.upgradeFlag, pMsgProc.upgradeFlag, pMsgProc.swTotalLengthInBytes);
	//返回
	return SUCCESS;
}

OPSTAT fsm_canitfleo_sysswm_sw_package_confirm(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret=0;

	msg_struct_sysswm_canitfleo_sw_package_confirm_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_sysswm_canitfleo_sw_package_confirm_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_sysswm_canitfleo_sw_package_confirm_t)))
		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);

	//入参检查
//#if (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFSC_CBU_ID)
//	if (rcv.nodeId >= HCU_SYSCFG_BFSC_SNR_WS_NBR_MAX)  HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Error parameter set!\n");
//#endif
	UINT32 bitmap = ((UINT32)1<<rcv.nodeId);

	//准备组装发送消息
	StrMsg_HUITP_MSGID_sui_sw_package_confirm_t pMsgProc;
	UINT16 msgProcLen = sizeof(StrMsg_HUITP_MSGID_sui_sw_package_confirm_t);
	memset(&pMsgProc, 0, msgProcLen);
	pMsgProc.msgid = HUITP_ENDIAN_EXG16(HUITP_MSGID_sui_sw_package_confirm);
	pMsgProc.length = HUITP_ENDIAN_EXG16(msgProcLen - 4);

	pMsgProc.swRelId = HUITP_ENDIAN_EXG16(rcv.swRelId);
	pMsgProc.swVerId = HUITP_ENDIAN_EXG16(rcv.swVerId);
	pMsgProc.upgradeFlag = rcv.upgradeFlag;
	pMsgProc.segIndex = HUITP_ENDIAN_EXG16(rcv.segIndex);
	pMsgProc.segTotal = HUITP_ENDIAN_EXG16(rcv.segTotal);
	pMsgProc.segSplitLen = HUITP_ENDIAN_EXG16(rcv.segSplitLen);
	pMsgProc.segValidLen = HUITP_ENDIAN_EXG16(rcv.segValidLen);
	pMsgProc.segCheckSum = HUITP_ENDIAN_EXG16(rcv.segCheckSum);
	memcpy(pMsgProc.swPkgBody, rcv.swPkgBody, rcv.segValidLen);

	//发送消息：配置消息分成多个分别发送，因为校准参数对于每一个下位机不一样
	if (hcu_canitfleo_usbcan_l2frame_send((UINT8*)&pMsgProc, msgProcLen, bitmap) == FAILURE)
		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Send CAN frame error!\n");

	//返回
	return SUCCESS;
}

OPSTAT fsm_canitfleo_sui_test_cmd_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret=0;
	int i=0;

	msg_struct_sui_test_cmd_req_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_sui_test_cmd_req_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_sui_test_cmd_req_t)))
		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);

	//生成bitmap
	UINT32 bitmap = 0;
	UINT32 temp =0;
	for (i=0; i<HCU_SYSMSG_SUI_SENSOR_NBR; i++){
		if (rcv.snrBitmap[i] == TRUE){
			temp = ((UINT32)1<<i);
			bitmap |= temp;
		}
	}

	//准备组装发送消息
	StrMsg_HUITP_MSGID_sui_com_test_command_req_t pMsgProc;
	UINT16 msgProcLen = sizeof(StrMsg_HUITP_MSGID_sui_com_test_command_req_t);
	memset(&pMsgProc, 0, msgProcLen);
	pMsgProc.msgid = HUITP_ENDIAN_EXG16(HUITP_MSGID_sui_com_test_command_req);
	pMsgProc.length = HUITP_ENDIAN_EXG16(msgProcLen - 4);
	pMsgProc.cmdid = HUITP_ENDIAN_EXG32(rcv.cmdid);
	pMsgProc.cmdvalue1 = HUITP_ENDIAN_EXG32(rcv.cmdvalue1);
	pMsgProc.cmdvalue2 = HUITP_ENDIAN_EXG32(rcv.cmdvalue2);
	pMsgProc.cmdvalue3 = HUITP_ENDIAN_EXG32(rcv.cmdvalue3);
	pMsgProc.cmdvalue4 = HUITP_ENDIAN_EXG32(rcv.cmdvalue4);
#if (HUITP_IEID_SUI_COM_TEST_CMD_BUF_LEN_MAX != HCU_SYSMSG_SUI_COM_TEST_CMD_BUF)
	#error HUITP_IEID_SUI_COM_TEST_CMD_BUF_LEN_MAX define error!
#endif
	memcpy(pMsgProc.cmdBuf, rcv.cmdBuf, HUITP_IEID_SUI_COM_TEST_CMD_BUF_LEN_MAX);
	//发送消息
	if (hcu_canitfleo_usbcan_l2frame_send((UINT8*)&pMsgProc, msgProcLen, bitmap) == FAILURE)
		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Send CAN frame error!\n");

	//返回
	return SUCCESS;
}

OPSTAT fsm_canitfleo_sui_suspend_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret=0;
	int i=0;

	msg_struct_sui_suspend_req_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_sui_suspend_req_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_sui_suspend_req_t)))
		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);

	//生成bitmap
	UINT32 bitmap = 0;
	for (i=0; i<HCU_SYSMSG_SUI_SENSOR_NBR; i++){
		if (rcv.boardBitmap[i] == TRUE){
			bitmap = ((UINT32)1<<i);
		}
	}

	//准备组装发送消息
	StrMsg_HUITP_MSGID_sui_com_suspend_req_t pMsgProc;
	UINT16 msgProcLen = sizeof(StrMsg_HUITP_MSGID_sui_com_suspend_req_t);
	memset(&pMsgProc, 0, msgProcLen);
	pMsgProc.msgid = HUITP_ENDIAN_EXG16(HUITP_MSGID_sui_com_suspend_req);
	pMsgProc.length = HUITP_ENDIAN_EXG16(msgProcLen - 4);

	//发送消息
	if (hcu_canitfleo_usbcan_l2frame_send((UINT8*)&pMsgProc, msgProcLen, bitmap) == FAILURE)
		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Send CAN frame error!\n");

	//返回
	return SUCCESS;
}

OPSTAT fsm_canitfleo_sui_resume_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret=0;
	int i=0;

	msg_struct_sui_resume_req_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_sui_resume_req_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_sui_resume_req_t)))
		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);

	//生成bitmap
	UINT32 bitmap = 0;
	for (i=0; i<HCU_SYSMSG_SUI_SENSOR_NBR; i++){
		if (rcv.boardBitmap[i] == TRUE){
			bitmap = ((UINT32)1<<i);
		}
	}

	//准备组装发送消息
	StrMsg_HUITP_MSGID_sui_com_resume_req_t pMsgProc;
	UINT16 msgProcLen = sizeof(StrMsg_HUITP_MSGID_sui_com_resume_req_t);
	memset(&pMsgProc, 0, msgProcLen);
	pMsgProc.msgid = HUITP_ENDIAN_EXG16(HUITP_MSGID_sui_com_resume_req);
	pMsgProc.length = HUITP_ENDIAN_EXG16(msgProcLen - 4);

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

	msg_struct_usbcan_l2frame_rcv_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_usbcan_l2frame_rcv_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_usbcan_l2frame_rcv_t)))
		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);
//#if (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFSC_CBU_ID)
//	if ((rcv.nodeId >= HCU_SYSCFG_BFSC_SNR_WS_NBR_MAX) || (rcv.validDataLen > HCU_SYSMSG_BFSC_USBCAN_MAX_RX_BUF_SIZE))
//		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Receive message error!\n");
//#endif

	//解码MSGID/MSGLEN
	UINT16 msgId = 0, msgLen = 0;
	StrMsg_HUITP_MSGID_sui_common_msg_header_t *pComMsg = (StrMsg_HUITP_MSGID_sui_common_msg_header_t *)(rcv.databuf);
	msgId = HUITP_ENDIAN_EXG16(pComMsg->msgid);
	msgLen = HUITP_ENDIAN_EXG16(pComMsg->length);
	if (msgLen != (rcv.validDataLen-4))
		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Decode message error on length, decoded msgLen=%d, in buffer len=%d!\n", msgLen, rcv.validDataLen-4);

	//按照消息类型进行分类处理
	switch(msgId){

#if (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFSC_CBU_ID)
	//BFSC消息
	case HUITP_MSGID_sui_bfsc_startup_ind:
	{
		HCU_DEBUG_PRINT_INF("CANITFLEO: Receive L3 MSG = HUITP_MSGID_sui_bfsc_startup_ind \n");
		StrMsg_HUITP_MSGID_sui_bfsc_startup_ind_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_sui_bfsc_startup_ind_t) - 4))
			HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_sui_bfsc_startup_ind_t*)(rcv.databuf);
		ret = func_canitfleo_l2frame_msg_bfsc_startup_ind_received_handle(snd, rcv.nodeId);
	}
	break;

	//BFSC消息
	case HUITP_MSGID_sui_bfsc_set_config_resp:
	{
		HCU_DEBUG_PRINT_INF("CANITFLEO: Receive L3 MSG = HUITP_MSGID_sui_bfsc_set_config_resp \n");
		StrMsg_HUITP_MSGID_sui_bfsc_set_config_resp_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_sui_bfsc_set_config_resp_t) - 4))
			HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_sui_bfsc_set_config_resp_t*)(rcv.databuf);
		ret = func_canitfleo_l2frame_msg_bfsc_set_config_resp_received_handle(snd, rcv.nodeId);
	}
	break;

	//BFSC消息
	case HUITP_MSGID_sui_bfsc_start_resp:
	{
		HCU_DEBUG_PRINT_INF("CANITFLEO: Receive L3 MSG = HUITP_MSGID_sui_bfsc_start_resp \n");
		StrMsg_HUITP_MSGID_sui_bfsc_start_resp_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_sui_bfsc_start_resp_t) - 4))
			HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_sui_bfsc_start_resp_t*)(rcv.databuf);
		ret = func_canitfleo_l2frame_msg_bfsc_start_resp_received_handle(snd, rcv.nodeId);
	}
	break;

	//BFSC消息
	case HUITP_MSGID_sui_bfsc_stop_resp:
	{
		HCU_DEBUG_PRINT_INF("CANITFLEO: Receive L3 MSG = HUITP_MSGID_sui_bfsc_stop_resp \n");
		StrMsg_HUITP_MSGID_sui_bfsc_stop_resp_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_sui_bfsc_stop_resp_t) - 4))
			HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_sui_bfsc_stop_resp_t*)(rcv.databuf);
		ret = func_canitfleo_l2frame_msg_bfsc_stop_resp_received_handle(snd, rcv.nodeId);
	}
	break;

	//BFSC消息
	case HUITP_MSGID_sui_bfsc_new_ws_event:
	{
		HCU_DEBUG_PRINT_INF("CANITFLEO: Receive L3 MSG = HUITP_MSGID_sui_bfsc_new_ws_event \n");
		StrMsg_HUITP_MSGID_sui_bfsc_new_ws_event_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_sui_bfsc_new_ws_event_t) - 4))
			HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_sui_bfsc_new_ws_event_t*)(rcv.databuf);
		ret = func_canitfleo_l2frame_msg_bfsc_new_ws_event_received_handle(snd, rcv.nodeId);
	}
	break;

	//BFSC消息
	case HUITP_MSGID_sui_bfsc_repeat_ws_event:
	{
		HCU_DEBUG_PRINT_INF("CANITFLEO: Receive L3 MSG = HUITP_MSGID_sui_bfsc_repeat_ws_event \n");
		StrMsg_HUITP_MSGID_sui_bfsc_repeat_ws_event_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_sui_bfsc_repeat_ws_event_t) - 4))
			HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_sui_bfsc_repeat_ws_event_t*)(rcv.databuf);
		ret = func_canitfleo_l2frame_msg_bfsc_repeat_ws_received_handle(snd, rcv.nodeId);
	}
	break;

	//BFSC消息
	case HUITP_MSGID_sui_bfsc_ws_comb_out_resp:
	{
		HCU_DEBUG_PRINT_INF("CANITFLEO: Receive L3 MSG = HUITP_MSGID_sui_bfsc_ws_comb_out_resp \n");
		StrMsg_HUITP_MSGID_sui_bfsc_ws_comb_out_resp_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_sui_bfsc_ws_comb_out_resp_t) - 4))
			HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_sui_bfsc_ws_comb_out_resp_t*)(rcv.databuf);
		ret = func_canitfleo_l2frame_msg_bfsc_ws_comb_out_received_handle(snd, rcv.nodeId);
	}
	break;

	//BFSC消息
	case HUITP_MSGID_sui_bfsc_command_resp:
	{
		HCU_DEBUG_PRINT_INF("CANITFLEO: Receive L3 MSG = HUITP_MSGID_sui_bfsc_command_resp \n");
		StrMsg_HUITP_MSGID_sui_bfsc_command_resp_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_sui_bfsc_command_resp_t) - 4))
			HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_sui_bfsc_command_resp_t*)(rcv.databuf);
		ret = func_canitfleo_l2frame_msg_bfsc_command_resp_received_handle(snd, rcv.nodeId);
	}
	break;

	//BFSC消息
	case HUITP_MSGID_sui_bfsc_fault_ind:
	{
		HCU_DEBUG_PRINT_INF("CANITFLEO: Receive L3 MSG = HUITP_MSGID_sui_bfsc_fault_ind \n");
		StrMsg_HUITP_MSGID_sui_bfsc_fault_ind_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_sui_bfsc_fault_ind_t) - 4))
			HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_sui_bfsc_fault_ind_t*)(rcv.databuf);
		ret = func_canitfleo_l2frame_msg_bfsc_fault_ind_received_handle(snd, rcv.nodeId);
	}
	break;

	//BFSC消息
	case HUITP_MSGID_sui_bfsc_err_inq_cmd_resp:
	{
		HCU_DEBUG_PRINT_INF("CANITFLEO: Receive L3 MSG = HUITP_MSGID_sui_bfsc_err_inq_cmd_resp \n");
		StrMsg_HUITP_MSGID_sui_bfsc_err_inq_cmd_resp_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_sui_bfsc_err_inq_cmd_resp_t) - 4))
			HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_sui_bfsc_err_inq_cmd_resp_t*)(rcv.databuf);
		ret = func_canitfleo_l2frame_msg_bfsc_err_ind_cmd_resp_received_handle(snd, rcv.nodeId);
	}
	break;

	//BFSC消息
	case HUITP_MSGID_sui_bfsc_heart_beat_report:
	{
		HCU_DEBUG_PRINT_INF("CANITFLEO: Receive L3 MSG = HUITP_MSGID_sui_bfsc_heart_beat_report \n");
		StrMsg_HUITP_MSGID_sui_bfsc_heart_beat_report_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_sui_bfsc_heart_beat_report_t) - 4))
			HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_sui_bfsc_heart_beat_report_t*)(rcv.databuf);
		ret = func_canitfleo_l2frame_msg_bfsc_heart_beat_report_received_handle(snd, rcv.nodeId);
	}
	break;
#endif

#if (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFDF_CBU_ID)
	//BFDF消息
//	case HUITP_MSGID_sui_bfdf_startup_ind:
//	{
//		HCU_DEBUG_PRINT_INF("CANITFLEO: Receive L3 MSG = HUITP_MSGID_sui_bfdf_startup_ind \n");
//		StrMsg_HUITP_MSGID_sui_bfdf_startup_ind_t *snd;
//		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_sui_bfdf_startup_ind_t) - 4))
//			HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Error unpack message on length!\n");
//		snd = (StrMsg_HUITP_MSGID_sui_bfdf_startup_ind_t*)(rcv.databuf);
//		ret = func_canitfleo_l2frame_msg_bfdf_startup_ind_received_handle(snd, rcv.nodeId);
//	}
//	break;

	//BFDF消息
	case HUITP_MSGID_sui_bfdf_set_config_resp:
	{
		HCU_DEBUG_PRINT_INF("CANITFLEO: Receive L3 MSG = HUITP_MSGID_sui_bfdf_set_config_resp \n");
		StrMsg_HUITP_MSGID_sui_bfdf_set_config_resp_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_sui_bfdf_set_config_resp_t) - 4))
			HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_sui_bfdf_set_config_resp_t*)(rcv.databuf);
		ret = func_canitfleo_l2frame_msg_bfdf_set_config_resp_received_handle(snd, rcv.nodeId);
	}
	break;

	//BFDF消息
//	case HUITP_MSGID_sui_bfdf_suspend_resp:
//	{
//		HCU_DEBUG_PRINT_INF("CANITFLEO: Receive L3 MSG = HUITP_MSGID_sui_bfdf_start_resp \n");
//		StrMsg_HUITP_MSGID_sui_bfdf_suspend_resp_t *snd;
//		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_sui_bfdf_suspend_resp_t) - 4))
//			HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Error unpack message on length!\n");
//		snd = (StrMsg_HUITP_MSGID_sui_bfdf_suspend_resp_t*)(rcv.databuf);
//		ret = func_canitfleo_l2frame_msg_bfdf_suspend_resp_received_handle(snd, rcv.nodeId);
//	}
//	break;
//
//	//BFDF消息
//	case HUITP_MSGID_sui_bfdf_resume_resp:
//	{
//		HCU_DEBUG_PRINT_INF("CANITFLEO: Receive L3 MSG = HUITP_MSGID_sui_bfdf_stop_resp \n");
//		StrMsg_HUITP_MSGID_sui_bfdf_resume_resp_t *snd;
//		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_sui_bfdf_resume_resp_t) - 4))
//			HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Error unpack message on length!\n");
//		snd = (StrMsg_HUITP_MSGID_sui_bfdf_resume_resp_t*)(rcv.databuf);
//		ret = func_canitfleo_l2frame_msg_bfdf_resume_resp_received_handle(snd, rcv.nodeId);
//	}
//	break;

	//BFDF消息
	case HUITP_MSGID_sui_bfdf_new_ws_event:
	{
		HCU_DEBUG_PRINT_INF("CANITFLEO: Receive L3 MSG = HUITP_MSGID_sui_bfdf_new_ws_event \n");
		StrMsg_HUITP_MSGID_sui_bfdf_new_ws_event_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_sui_bfdf_new_ws_event_t) - 4))
			HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_sui_bfdf_new_ws_event_t*)(rcv.databuf);
		ret = func_canitfleo_l2frame_msg_bfdf_new_ws_event_received_handle(snd, rcv.nodeId);
	}
	break;

	//BFDF消息
	case HUITP_MSGID_sui_bfdf_ws_comb_out_resp:
	{
		HCU_DEBUG_PRINT_INF("CANITFLEO: Receive L3 MSG = HUITP_MSGID_sui_bfdf_ws_comb_out_resp \n");
		StrMsg_HUITP_MSGID_sui_bfdf_ws_comb_out_resp_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_sui_bfdf_ws_comb_out_resp_t) - 4))
			HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_sui_bfdf_ws_comb_out_resp_t*)(rcv.databuf);
		ret = func_canitfleo_l2frame_msg_bfdf_ws_comb_out_received_handle(snd, rcv.nodeId);
	}
	break;

	//BFDF消息
//	case HUITP_MSGID_sui_bfdf_command_resp:
//	{
//		HCU_DEBUG_PRINT_INF("CANITFLEO: Receive L3 MSG = HUITP_MSGID_sui_bfdf_command_resp \n");
//		StrMsg_HUITP_MSGID_sui_bfdf_command_resp_t *snd;
//		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_sui_bfdf_command_resp_t) - 4))
//			HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Error unpack message on length!\n");
//		snd = (StrMsg_HUITP_MSGID_sui_bfdf_command_resp_t*)(rcv.databuf);
//		ret = func_canitfleo_l2frame_msg_bfdf_command_resp_received_handle(snd, rcv.nodeId);
//	}
//	break;

	//BFDF消息
//	case HUITP_MSGID_sui_bfdf_fault_ind:
//	{
//		HCU_DEBUG_PRINT_INF("CANITFLEO: Receive L3 MSG = HUITP_MSGID_sui_bfdf_fault_ind \n");
//		StrMsg_HUITP_MSGID_sui_bfdf_fault_ind_t *snd;
//		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_sui_bfdf_fault_ind_t) - 4))
//			HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Error unpack message on length!\n");
//		snd = (StrMsg_HUITP_MSGID_sui_bfdf_fault_ind_t*)(rcv.databuf);
//		ret = func_canitfleo_l2frame_msg_bfdf_fault_ind_received_handle(snd, rcv.nodeId);
//	}
//	break;

	//BFDF消息
//	case HUITP_MSGID_sui_bfdf_heart_beat_report:
//	{
//		HCU_DEBUG_PRINT_INF("CANITFLEO: Receive L3 MSG = HUITP_MSGID_sui_bfdf_heart_beat_report \n");
//		StrMsg_HUITP_MSGID_sui_bfdf_heart_beat_report_t *snd;
//		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_sui_bfdf_heart_beat_report_t) - 4))
//			HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Error unpack message on length!\n");
//		snd = (StrMsg_HUITP_MSGID_sui_bfdf_heart_beat_report_t*)(rcv.databuf);
//		ret = func_canitfleo_l2frame_msg_bfdf_heart_beat_report_received_handle(snd, rcv.nodeId);
//	}
//	break;

	//BFDF消息
	case HUITP_MSGID_sui_bfdf_snc_pullin_resp:
	{
		HCU_DEBUG_PRINT_INF("CANITFLEO: Receive L3 MSG = HUITP_MSGID_sui_bfdf_heart_beat_report \n");
		StrMsg_HUITP_MSGID_sui_bfdf_snc_pullin_resp_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_sui_bfdf_snc_pullin_resp_t) - 4))
			HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_sui_bfdf_snc_pullin_resp_t*)(rcv.databuf);
		ret = func_canitfleo_l2frame_msg_bfdf_snc_pullin_resp_received_handle(snd, rcv.nodeId);
	}
	break;

	//BFDF消息
	case HUITP_MSGID_sui_bfdf_basket_clean_ind:
	{
		HCU_DEBUG_PRINT_INF("CANITFLEO: Receive L3 MSG = HUITP_MSGID_sui_bfdf_heart_beat_report \n");
		StrMsg_HUITP_MSGID_sui_bfdf_basket_clean_ind_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_sui_bfdf_basket_clean_ind_t) - 4))
			HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_sui_bfdf_basket_clean_ind_t*)(rcv.databuf);
		ret = func_canitfleo_l2frame_msg_bfdf_basket_clean_ind_received_handle(snd, rcv.nodeId);
	}
	break;

#endif

#if (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFHS_CBU_ID)
	//BFHS消息
//	case HUITP_MSGID_sui_bfhs_startup_ind:
//	{
//		HCU_DEBUG_PRINT_INF("CANITFLEO: Receive L3 MSG = HUITP_MSGID_sui_bfhs_startup_ind \n");
//		StrMsg_HUITP_MSGID_sui_bfhs_startup_ind_t *snd;
//		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_sui_bfhs_startup_ind_t) - 4))
//			HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Error unpack message on length!\n");
//		snd = (StrMsg_HUITP_MSGID_sui_bfhs_startup_ind_t*)(rcv.databuf);
//		ret = func_canitfleo_l2frame_msg_bfhs_startup_ind_received_handle(snd, rcv.nodeId);
//	}
//	break;

	//BFHS消息
	case HUITP_MSGID_sui_bfhs_calibration_zero_resp:
	{
		HCU_DEBUG_PRINT_INF("CANITFLEO: Receive L3 MSG = HUITP_MSGID_sui_bfhs_calibration_zero_resp \n");
		StrMsg_HUITP_MSGID_sui_bfhs_calibration_zero_resp_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_sui_bfhs_calibration_zero_resp_t) - 4))
			HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_sui_bfhs_calibration_zero_resp_t*)(rcv.databuf);
		ret = func_canitfleo_l2frame_msg_bfhs_calibration_zero_resp_received_handle(snd, rcv.nodeId);
	}
	break;

	//BFHS消息
	case HUITP_MSGID_sui_bfhs_calibration_full_resp:
	{
		HCU_DEBUG_PRINT_INF("CANITFLEO: Receive L3 MSG = HUITP_MSGID_sui_bfhs_calibration_full_resp \n");
		StrMsg_HUITP_MSGID_sui_bfhs_calibration_full_resp_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_sui_bfhs_calibration_full_resp_t) - 4))
			HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_sui_bfhs_calibration_full_resp_t*)(rcv.databuf);
		ret = func_canitfleo_l2frame_msg_bfhs_calibration_full_resp_received_handle(snd, rcv.nodeId);
	}
	break;

	//BFHS消息
	case HUITP_MSGID_sui_bfhs_set_config_resp:
	{
		HCU_DEBUG_PRINT_INF("CANITFLEO: Receive L3 MSG = HUITP_MSGID_sui_bfhs_set_config_resp \n");
		StrMsg_HUITP_MSGID_sui_bfhs_set_config_resp_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_sui_bfhs_set_config_resp_t) - 4))
			HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_sui_bfhs_set_config_resp_t*)(rcv.databuf);
		ret = func_canitfleo_l2frame_msg_bfhs_set_config_resp_received_handle(snd, rcv.nodeId);
	}
	break;

//	//BFHS消息
//	case HUITP_MSGID_sui_bfhs_suspend_resp:
//	{
//		HCU_DEBUG_PRINT_INF("CANITFLEO: Receive L3 MSG = HUITP_MSGID_sui_bfhs_start_resp \n");
//		StrMsg_HUITP_MSGID_sui_bfhs_suspend_resp_t *snd;
//		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_sui_bfhs_suspend_resp_t) - 4))
//			HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Error unpack message on length!\n");
//		snd = (StrMsg_HUITP_MSGID_sui_bfhs_suspend_resp_t*)(rcv.databuf);
//		ret = func_canitfleo_l2frame_msg_bfhs_suspend_resp_received_handle(snd, rcv.nodeId);
//	}
//	break;
//
//	//BFHS消息
//	case HUITP_MSGID_sui_bfhs_resume_resp:
//	{
//		HCU_DEBUG_PRINT_INF("CANITFLEO: Receive L3 MSG = HUITP_MSGID_sui_bfhs_stop_resp \n");
//		StrMsg_HUITP_MSGID_sui_bfhs_resume_resp_t *snd;
//		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_sui_bfhs_resume_resp_t) - 4))
//			HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Error unpack message on length!\n");
//		snd = (StrMsg_HUITP_MSGID_sui_bfhs_resume_resp_t*)(rcv.databuf);
//		ret = func_canitfleo_l2frame_msg_bfhs_resume_resp_received_handle(snd, rcv.nodeId);
//	}
//	break;

	//BFHS消息
	case HUITP_MSGID_sui_bfhs_new_ws_event:
	{
		HCU_DEBUG_PRINT_INF("CANITFLEO: Receive L3 MSG = HUITP_MSGID_sui_bfhs_new_ws_event \n");
		StrMsg_HUITP_MSGID_sui_bfhs_new_ws_event_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_sui_bfhs_new_ws_event_t) - 4))
			HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_sui_bfhs_new_ws_event_t*)(rcv.databuf);
		ret = func_canitfleo_l2frame_msg_bfhs_new_ws_event_received_handle(snd, rcv.nodeId);
	}
	break;

//	case HUITP_MSGID_sui_bfhs_command_resp:
//	{
//		HCU_DEBUG_PRINT_INF("CANITFLEO: Receive L3 MSG = HUITP_MSGID_sui_bfhs_command_resp \n");
//		StrMsg_HUITP_MSGID_sui_bfhs_command_resp_t *snd;
//		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_sui_bfhs_command_resp_t) - 4))
//			HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Error unpack message on length!\n");
//		snd = (StrMsg_HUITP_MSGID_sui_bfhs_command_resp_t*)(rcv.databuf);
//		ret = func_canitfleo_l2frame_msg_bfhs_command_resp_received_handle(snd, rcv.nodeId);
//	}
//	break;

//	case HUITP_MSGID_sui_bfhs_fault_ind:
//	{
//		HCU_DEBUG_PRINT_INF("CANITFLEO: Receive L3 MSG = HUITP_MSGID_sui_bfhs_fault_ind \n");
//		StrMsg_HUITP_MSGID_sui_bfhs_fault_ind_t *snd;
//		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_sui_bfhs_fault_ind_t) - 4))
//			HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Error unpack message on length!\n");
//		snd = (StrMsg_HUITP_MSGID_sui_bfhs_fault_ind_t*)(rcv.databuf);
//		ret = func_canitfleo_l2frame_msg_bfhs_fault_ind_received_handle(snd, rcv.nodeId);
//	}
//	break;

//	case HUITP_MSGID_sui_bfhs_heart_beat_report:
//	{
//		HCU_DEBUG_PRINT_INF("CANITFLEO: Receive L3 MSG = HUITP_MSGID_sui_bfhs_heart_beat_report \n");
//		StrMsg_HUITP_MSGID_sui_bfhs_heart_beat_report_t *snd;
//		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_sui_bfhs_heart_beat_report_t) - 4))
//			HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Error unpack message on length!\n");
//		snd = (StrMsg_HUITP_MSGID_sui_bfhs_heart_beat_report_t*)(rcv.databuf);
//		ret = func_canitfleo_l2frame_msg_bfhs_heart_beat_report_received_handle(snd, rcv.nodeId);
//	}
//	break;

#endif //BFHS

	//公共消息部分
	case HUITP_MSGID_sui_inventory_report:
	{
		HCU_DEBUG_PRINT_INF("CANITFLEO: Receive L3 MSG = HUITP_MSGID_sui_inventory_report \n");
		StrMsg_HUITP_MSGID_sui_inventory_report_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_sui_inventory_report_t) - 4))
			HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_sui_inventory_report_t*)(rcv.databuf);
		ret = func_canitfleo_l2frame_msg_inventory_report_received_handle(snd, rcv.nodeId);
	}
	break;

	//公共消息部分
	case HUITP_MSGID_sui_sw_package_report:
	{
		HCU_DEBUG_PRINT_INF("CANITFLEO: Receive L3 MSG = HUITP_MSGID_sui_sw_package_report \n");
		StrMsg_HUITP_MSGID_sui_sw_package_report_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_sui_sw_package_report_t) - 4))
			HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_sui_sw_package_report_t*)(rcv.databuf);
		ret = func_canitfleo_l2frame_msg_sw_package_report_received_handle(snd, rcv.nodeId);
	}
	break;

	//公共消息部分
	case HUITP_MSGID_sui_com_test_command_resp:
	{
		HCU_DEBUG_PRINT_INF("CANITFLEO: Receive L3 MSG = HUITP_MSGID_sui_com_test_command_resp \n");
		StrMsg_HUITP_MSGID_sui_com_test_command_resp_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_sui_com_test_command_resp_t) - 4))
			HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_sui_com_test_command_resp_t*)(rcv.databuf);
		ret = func_canitfleo_l2frame_msg_com_test_command_resp_received_handle(snd, rcv.nodeId);
	}
	break;

	//公共消息部分
	case HUITP_MSGID_sui_com_heart_beat_report:
	{
		HCU_DEBUG_PRINT_INF("CANITFLEO: Receive L3 MSG = HUITP_MSGID_sui_com_heart_beat_report \n");
		StrMsg_HUITP_MSGID_sui_com_heart_beat_report_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_sui_com_heart_beat_report_t) - 4))
			HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_sui_com_heart_beat_report_t*)(rcv.databuf);
		ret = func_canitfleo_l2frame_msg_com_heart_beat_report_received_handle(snd, rcv.nodeId);
	}
	break;

	//公共消息部分
	case HUITP_MSGID_sui_com_startup_ind:
	{
		HCU_DEBUG_PRINT_INF("CANITFLEO: Receive L3 MSG = HUITP_MSGID_sui_com_startup_ind \n");
		StrMsg_HUITP_MSGID_sui_com_startup_ind_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_sui_com_startup_ind_t) - 4))
			HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_sui_com_startup_ind_t*)(rcv.databuf);
		ret = func_canitfleo_l2frame_msg_com_startup_ind_received_handle(snd, rcv.nodeId);
	}
	break;

	//公共消息部分
	case HUITP_MSGID_sui_com_fault_ind:
	{
		HCU_DEBUG_PRINT_INF("CANITFLEO: Receive L3 MSG = HUITP_MSGID_sui_com_fault_ind \n");
		StrMsg_HUITP_MSGID_sui_com_fault_ind_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_sui_com_fault_ind_t) - 4))
			HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_sui_com_fault_ind_t*)(rcv.databuf);
		ret = func_canitfleo_l2frame_msg_com_fault_ind_received_handle(snd, rcv.nodeId);
	}
	break;

	//公共消息部分
	case HUITP_MSGID_sui_com_suspend_resp:
	{
		HCU_DEBUG_PRINT_INF("CANITFLEO: Receive L3 MSG = HUITP_MSGID_sui_com_suspend_resp \n");
		StrMsg_HUITP_MSGID_sui_com_suspend_resp_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_sui_com_suspend_resp_t) - 4))
			HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_sui_com_suspend_resp_t*)(rcv.databuf);
		ret = func_canitfleo_l2frame_msg_com_suspend_resp_received_handle(snd, rcv.nodeId);
	}
	break;

	//公共消息部分
	case HUITP_MSGID_sui_com_resume_resp:
	{
		HCU_DEBUG_PRINT_INF("CANITFLEO: Receive L3 MSG = HUITP_MSGID_sui_com_resume_resp \n");
		StrMsg_HUITP_MSGID_sui_com_resume_resp_t *snd;
		if (msgLen != (sizeof(StrMsg_HUITP_MSGID_sui_com_resume_resp_t) - 4))
			HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Error unpack message on length!\n");
		snd = (StrMsg_HUITP_MSGID_sui_com_resume_resp_t*)(rcv.databuf);
		ret = func_canitfleo_l2frame_msg_com_resume_resp_received_handle(snd, rcv.nodeId);
	}
	break;

	default:
		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Receive unsupported message! Msgid=0x%x, NodeId=%d\n", msgId, rcv.nodeId);
	break;
	}

	//返回
	return ret;
}

//本消息将送到SYSSWM模块
OPSTAT func_canitfleo_l2frame_msg_inventory_report_received_handle(StrMsg_HUITP_MSGID_sui_inventory_report_t *rcv, UINT8 nodeId)
{
	//因为没有标准的IE结构，所以这里不能再验证IEID/IELEN的大小段和长度问题
	//将内容发送给目的模块：暂无。基于目前的情况，等待下位机重启

#if (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFSC_CBU_ID)
	dbi_HcuBfsc_WmcStatusUpdate(0, nodeId, DBI_BFSC_SNESOR_STATUS_INV_RPT_RCV, 0);
#endif
#if (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFDF_CBU_ID)
#endif
#if (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFHS_CBU_ID)
#endif

	//准备组装发送消息
	msg_struct_canitfleo_sysswm_inventory_report_t snd;
	memset(&snd, 0, sizeof(msg_struct_canitfleo_sysswm_inventory_report_t));

	snd.hwType = HUITP_ENDIAN_EXG16(rcv->hwType);
	snd.hwId = HUITP_ENDIAN_EXG16(rcv->hwId);
	snd.swRel = HUITP_ENDIAN_EXG16(rcv->swRel);
	snd.swVer = HUITP_ENDIAN_EXG16(rcv->swVer);
	snd.upgradeFlag = rcv->upgradeFlag;
	snd.nodeId = nodeId;
	snd.length = sizeof(msg_struct_canitfleo_sysswm_inventory_report_t);
	if (hcu_message_send(MSG_ID_CANITFLEO_SYSSWM_INVENTORY_REPORT, TASK_ID_SYSSWM, TASK_ID_CANITFLEO, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName, zHcuVmCtrTab.task[TASK_ID_SYSSWM].taskName);

	//刷界面中IHU版本信息：已经移动到SYSSWM模块中进行处理了
//	char input[50];
//	memset(input, 0, 50);
//	if (zHcuSysEngPar.hwBurnId.nodeHwType != 0){
//		memset(input, 0, 50);
//		sprintf(input, "IHU-SW-R%d.V%d.", snd.swRel, snd.swVer);
//		if (snd.upgradeFlag == HUITP_IEID_UNI_FW_UPGRADE_NO) strcat(input, "UPG_NO");
//		else if (snd.upgradeFlag == HUITP_IEID_UNI_FW_UPGRADE_YES_STABLE) strcat(input, "STABLE");
//		else if (snd.upgradeFlag == HUITP_IEID_UNI_FW_UPGRADE_YES_TRIAL) strcat(input, "TRIAL");
//		else if (snd.upgradeFlag == HUITP_IEID_UNI_FW_UPGRADE_YES_PATCH) strcat(input, "PATCH");
//		else strcat(input, "UPG_ERROR");
//		dbi_HcuBfsc_ihusw_ver_Update(input, strlen(input));
//	}

	//返回
	return SUCCESS;
}

OPSTAT func_canitfleo_l2frame_msg_sw_package_report_received_handle(StrMsg_HUITP_MSGID_sui_sw_package_report_t *rcv, UINT8 nodeId)
{
	//因为没有标准的IE结构，所以这里不能再验证IEID/IELEN的大小段和长度问题
	//将内容发送给目的模块：暂无。基于目前的情况，等待下位机重启
#if (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFSC_CBU_ID)
	dbi_HcuBfsc_WmcStatusUpdate(0, nodeId, DBI_BFSC_SNESOR_STATUS_SW_PKG_RCV, HUITP_ENDIAN_EXG16(rcv->segIndex));
#endif
#if (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFDF_CBU_ID)
#endif
#if (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFHS_CBU_ID)
#endif

	//准备组装发送消息
	msg_struct_canitfleo_sysswm_sw_package_report_t snd;
	memset(&snd, 0, sizeof(msg_struct_canitfleo_sysswm_sw_package_report_t));

	snd.swRelId = HUITP_ENDIAN_EXG16(rcv->swRelId);
	snd.swVerId = HUITP_ENDIAN_EXG16(rcv->swVerId);
	snd.upgradeFlag = rcv->upgradeFlag;
	snd.segIndex = HUITP_ENDIAN_EXG16(rcv->segIndex);
	snd.segTotal = HUITP_ENDIAN_EXG16(rcv->segTotal);
	snd.segSplitLen = HUITP_ENDIAN_EXG16(rcv->segSplitLen);
	snd.nodeId = nodeId;

	snd.length = sizeof(msg_struct_canitfleo_sysswm_sw_package_report_t);
	if (hcu_message_send(MSG_ID_CANITFLEO_SYSSWM_SW_PACKAGE_REPORT, TASK_ID_SYSSWM, TASK_ID_CANITFLEO, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName, zHcuVmCtrTab.task[TASK_ID_SYSSWM].taskName);

	//返回
	return SUCCESS;
}

OPSTAT func_canitfleo_l2frame_msg_com_test_command_resp_received_handle(StrMsg_HUITP_MSGID_sui_com_test_command_resp_t *rcv, UINT8 nodeId)
{
	//因为没有标准的IE结构，所以这里不能再验证IEID/IELEN的大小段和长度问题
	//将内容发送给目的模块：暂无。基于目前的情况，等待下位机重启

	//准备组装发送消息
	msg_struct_sui_test_cmd_resp_t snd;
	memset(&snd, 0, sizeof(msg_struct_sui_test_cmd_resp_t));
	snd.snrId = nodeId;
	snd.cmdid = HUITP_ENDIAN_EXG32(rcv->cmdid);
	snd.validFlag = HUITP_ENDIAN_EXG8(rcv->validFlag);
	snd.errCode = HUITP_ENDIAN_EXG16(rcv->error_code);
	snd.cmdvalue1 = HUITP_ENDIAN_EXG32(rcv->cmdvalue1);
	snd.cmdvalue2 = HUITP_ENDIAN_EXG32(rcv->cmdvalue2);
	snd.cmdvalue3 = HUITP_ENDIAN_EXG32(rcv->cmdvalue3);
	snd.cmdvalue4 = HUITP_ENDIAN_EXG32(rcv->cmdvalue4);

	snd.length = sizeof(msg_struct_sui_test_cmd_resp_t);
#if (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFDF_CBU_ID)
	if (hcu_message_send(MSG_ID_SUI_TEST_CMD_RESP, TASK_ID_BFDFUICOMM, TASK_ID_CANITFLEO, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName, zHcuVmCtrTab.task[TASK_ID_BFDFUICOMM].taskName);
#endif
#if (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFHS_CBU_ID)
	if (hcu_message_send(MSG_ID_SUI_TEST_CMD_RESP, TASK_ID_BFHSUICOMM, TASK_ID_CANITFLEO, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName, zHcuVmCtrTab.task[TASK_ID_BFHSUICOMM].taskName);
#endif

	//返回
	return SUCCESS;
}

OPSTAT func_canitfleo_l2frame_msg_com_heart_beat_report_received_handle(StrMsg_HUITP_MSGID_sui_com_heart_beat_report_t *rcv, UINT8 nodeId)
{
	//因为没有标准的IE结构，所以这里不能再验证IEID/IELEN的大小段和长度问题
	//将内容发送给目的模块：暂无。基于目前的情况，等待下位机重启

	//准备组装发送消息
	StrMsg_HUITP_MSGID_sui_com_heart_beat_confirm_t pMsgProc;
	UINT16 msgProcLen = sizeof(StrMsg_HUITP_MSGID_sui_com_heart_beat_confirm_t);
	memset(&pMsgProc, 0, msgProcLen);
	pMsgProc.msgid = HUITP_ENDIAN_EXG16(HUITP_MSGID_sui_com_heart_beat_confirm);
	pMsgProc.length = HUITP_ENDIAN_EXG16(msgProcLen - 4);
	pMsgProc.wmc_id = nodeId;
#if (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFDF_CBU_ID)
#endif
#if (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFHS_CBU_ID)
	if (gTaskL3bfhsContext.sensorWs[nodeId].nodeStatus <= HCU_L3BFHS_NODE_BOARD_STATUS_OFFLINE_MAX)
		pMsgProc.wmcState = HUITP_IEID_SUI_COM_HEATT_BEAT_WMC_STATE_OFFLINE;
	else if (gTaskL3bfhsContext.sensorWs[nodeId].nodeStatus <= HCU_L3BFHS_NODE_BOARD_STATUS_INIT_MAX)
		pMsgProc.wmcState = HUITP_IEID_SUI_COM_HEATT_BEAT_WMC_STATE_INIT;
	else if (gTaskL3bfhsContext.sensorWs[nodeId].nodeStatus <= HCU_L3BFHS_NODE_BOARD_STATUS_WORK_MAX)
		pMsgProc.wmcState = HUITP_IEID_SUI_COM_HEATT_BEAT_WMC_STATE_WORKING;
	else
		pMsgProc.wmcState = HUITP_IEID_SUI_COM_HEATT_BEAT_WMC_STATE_INVALID;
	pMsgProc.timeStamp = HUITP_ENDIAN_EXG32(time(0));
#endif

	//生成bitmap
	UINT32 bitmap = ((UINT32)1<<nodeId);

	//发送消息
	if (hcu_canitfleo_usbcan_l2frame_send((UINT8*)&pMsgProc, msgProcLen, bitmap) == FAILURE)
		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Send CAN frame error!\n");

	//返回
	return SUCCESS;
}

OPSTAT func_canitfleo_l2frame_msg_com_startup_ind_received_handle(StrMsg_HUITP_MSGID_sui_com_startup_ind_t *rcv, UINT8 nodeId)
{
	//因为没有标准的IE结构，所以这里不能再验证IEID/IELEN的大小段和长度问题
	//将内容发送给目的模块：暂无。基于目前的情况，等待下位机重启

	//准备组装发送消息
	msg_struct_sui_startup_ind_t snd;
	memset(&snd, 0, sizeof(msg_struct_sui_startup_ind_t));
	snd.snrId = nodeId; //HUITP_ENDIAN_EXG8(rcv->wmc_inventory.wmc_id);
	snd.hwId = HUITP_ENDIAN_EXG32(rcv->wmc_inventory.hw_inventory_id);
	snd.swId = HUITP_ENDIAN_EXG32(rcv->wmc_inventory.sw_inventory_id);
	snd.cpuId = HUITP_ENDIAN_EXG32(rcv->wmc_inventory.stm32_cpu_id);
	snd.wgtType = HUITP_ENDIAN_EXG32(rcv->wmc_inventory.weight_sensor_type);
	snd.motoType = HUITP_ENDIAN_EXG32(rcv->wmc_inventory.motor_type);
	snd.length = sizeof(msg_struct_sui_startup_ind_t);
#if (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFDF_CBU_ID)
	if (hcu_message_send(MSG_ID_SUI_STARTUP_IND, TASK_ID_L3BFDF, TASK_ID_CANITFLEO, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName, zHcuVmCtrTab.task[TASK_ID_L3BFDF].taskName);
#endif
#if (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFHS_CBU_ID)
	if (hcu_message_send(MSG_ID_SUI_STARTUP_IND, TASK_ID_L3BFHS, TASK_ID_CANITFLEO, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName, zHcuVmCtrTab.task[TASK_ID_L3BFHS].taskName);
#endif

	//返回
	return SUCCESS;
}

OPSTAT func_canitfleo_l2frame_msg_com_fault_ind_received_handle(StrMsg_HUITP_MSGID_sui_com_fault_ind_t *rcv, UINT8 nodeId)
{
	//因为没有标准的IE结构，所以这里不能再验证IEID/IELEN的大小段和长度问题
	//将内容发送给目的模块：暂无。基于目前的情况，等待下位机重启

	//准备组装发送消息
	msg_struct_sui_fault_ind_t snd;
	memset(&snd, 0, sizeof(msg_struct_sui_fault_ind_t));
	snd.snrId = nodeId;  //HUITP_ENDIAN_EXG8(rcv->wmc_id);
	snd.error_code = HUITP_ENDIAN_EXG16(rcv->error_code);
	snd.length = sizeof(msg_struct_sui_fault_ind_t);
#if (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFDF_CBU_ID)
	if (hcu_message_send(MSG_ID_SUI_FAULT_IND, TASK_ID_L3BFDF, TASK_ID_CANITFLEO, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName, zHcuVmCtrTab.task[TASK_ID_L3BFDF].taskName);
#endif
#if (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFHS_CBU_ID)
	if (hcu_message_send(MSG_ID_SUI_FAULT_IND, TASK_ID_L3BFHS, TASK_ID_CANITFLEO, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName, zHcuVmCtrTab.task[TASK_ID_L3BFHS].taskName);
#endif

	//返回
	return SUCCESS;
}

OPSTAT func_canitfleo_l2frame_msg_com_suspend_resp_received_handle(StrMsg_HUITP_MSGID_sui_com_suspend_resp_t *rcv, UINT8 nodeId)
{
	//因为没有标准的IE结构，所以这里不能再验证IEID/IELEN的大小段和长度问题
	//将内容发送给目的模块：暂无。基于目前的情况，等待下位机重启

	//准备组装发送消息
	msg_struct_sui_suspend_resp_t snd;
	memset(&snd, 0, sizeof(msg_struct_sui_suspend_resp_t));
	snd.errCode = HUITP_ENDIAN_EXG16(rcv->errCode);
	snd.validFlag = HUITP_ENDIAN_EXG8(rcv->validFlag);
	snd.snrId = nodeId;
	snd.length = sizeof(msg_struct_sui_suspend_resp_t);
#if (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFDF_CBU_ID)
	if (hcu_message_send(MSG_ID_SUI_SUSPEND_RESP, TASK_ID_L3BFDF, TASK_ID_CANITFLEO, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName, zHcuVmCtrTab.task[TASK_ID_L3BFDF].taskName);
#endif
#if (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFHS_CBU_ID)
	if (hcu_message_send(MSG_ID_SUI_SUSPEND_RESP, TASK_ID_L3BFHS, TASK_ID_CANITFLEO, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName, zHcuVmCtrTab.task[TASK_ID_L3BFHS].taskName);
#endif

	//返回
	return SUCCESS;
}

OPSTAT func_canitfleo_l2frame_msg_com_resume_resp_received_handle(StrMsg_HUITP_MSGID_sui_com_resume_resp_t *rcv, UINT8 nodeId)
{
	//因为没有标准的IE结构，所以这里不能再验证IEID/IELEN的大小段和长度问题
	//将内容发送给目的模块：暂无。基于目前的情况，等待下位机重启

	//准备组装发送消息
	msg_struct_sui_resume_resp_t snd;
	memset(&snd, 0, sizeof(msg_struct_sui_resume_resp_t));
	snd.errCode = HUITP_ENDIAN_EXG16(rcv->errCode);
	snd.validFlag = HUITP_ENDIAN_EXG8(rcv->validFlag);
	snd.snrId = nodeId;
	snd.length = sizeof(msg_struct_sui_resume_resp_t);
#if (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFDF_CBU_ID)
	if (hcu_message_send(MSG_ID_SUI_RESUME_RESP, TASK_ID_L3BFDF, TASK_ID_CANITFLEO, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName, zHcuVmCtrTab.task[TASK_ID_L3BFDF].taskName);
#endif
#if (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFHS_CBU_ID)
	if (hcu_message_send(MSG_ID_SUI_RESUME_RESP, TASK_ID_L3BFHS, TASK_ID_CANITFLEO, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName, zHcuVmCtrTab.task[TASK_ID_L3BFHS].taskName);
#endif

	//返回
	return SUCCESS;
}

/*
 *  USBCAN BSP函数映射：硬件在两个CAN接口之间跳动，待固定住哪个CAN接口
 *
 */
OPSTAT hcu_canitfleo_usbcan_interface_init(void)
{
	int ret = FAILURE;
	ret = hcu_bsp_usbcan_init(&(gTaskCanitfleoContext.can1), CAN_DEVICE_TYPE_PCI9820I, \
					CAN_DEVIDE_IDX_CARD0, CAN_DEVIDE_CHANNEL_CAN0, \
					CAN_BANDRATE_500KBPS, 0, CAN_L2_FRAME_FORWARD_YES);
	if (ret == FAILURE){
		HcuErrorPrint("CANITFLEO: Init CAN interface CAN_DEVIDE_IDX_CARD0 failure!\n");
		ret = hcu_bsp_usbcan_init(&(gTaskCanitfleoContext.can1), CAN_DEVICE_TYPE_PCI9820I, \
							CAN_DEVIDE_IDX_CARD1, CAN_DEVIDE_CHANNEL_CAN1, \
							CAN_BANDRATE_500KBPS, 0, CAN_L2_FRAME_FORWARD_YES);
	}

	if (ret == FAILURE) HcuErrorPrint("CANITFLEO: Init CAN interface CAN_DEVIDE_IDX_CARD1 failure!\n");

	return ret;
}

//发送API
OPSTAT hcu_canitfleo_usbcan_l2frame_send(UINT8 *buffer, UINT32 length, UINT32 wmc_id_bitmap)
{
	//HATE测试环境
#ifdef HATE_TRIGGER_ENABLE
	msg_struct_l3hate_can_frame_rcv_t snd;
	memset(&snd, 0, sizeof(msg_struct_l3hate_can_frame_rcv_t));
	memcpy(snd.dataBuf, buffer, length);
	snd.bufValidLen = length;
	snd.length = sizeof(msg_struct_l3hate_can_frame_rcv_t);
	if (hcu_message_send(MSG_ID_CAN_L3HATE_FRAME_RCV, TASK_ID_L3HATE, TASK_ID_CANITFLEO, &snd, snd.length) == FAILURE){
		HcuErrorPrint("L3HATE: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName, zHcuVmCtrTab.task[TASK_ID_L3HATE].taskName);
		return FAILURE;
	}
	return SUCCESS;
#else
	//业务执行
	return hcu_bsp_usbcan_l2frame_transmit(&(gTaskCanitfleoContext.can1), buffer, length, wmc_id_bitmap);
#endif
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
#endif //多项目共享部分


OPSTAT hcu_canitfleo_hate_send_data(UINT8 *buf, int len, UINT8 node)
{
	//发送给CANITFLEO模块
	msg_struct_usbcan_l2frame_rcv_t snd;
	memset(&snd, 0, sizeof(msg_struct_usbcan_l2frame_rcv_t));
	snd.nodeId = node;
	snd.validDataLen = len;
	memcpy(snd.databuf, buf, len);

	snd.length = sizeof(msg_struct_usbcan_l2frame_rcv_t);
	if (hcu_message_send(MSG_ID_USBCAN_L2FRAME_RCV, TASK_ID_CANITFLEO, TASK_ID_CANITFLEO, &snd, snd.length) == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_CANITFLEO]++;
		HcuErrorPrint("CANITFLEO: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName, zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName);
		return FAILURE;
	}

	return SUCCESS;
}

//发送给界面的通知消息
void func_canitfleo_huicobus_trigger_uir(UINT32 cmdId, UINT32 cmdValue)
{
}


