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

/*
** FSM of the CANITFLEO
*/
FsmStateItem_t FsmCanitfleo[] =
{
    //MessageId                 //State                   		 		//Function
	//启始点，固定定义，不要改动, 使用ENTRY/END，意味者MSGID肯定不可能在某个高位区段中；考虑到所有任务共享MsgId，即使分段，也无法实现
	//完全是为了给任务一个初始化的机会，按照状态转移机制，该函数不具备启动的机会，因为任务初始化后自动到FSM_STATE_IDLE
	//如果没有必要进行初始化，可以设置为NULL
	{MSG_ID_ENTRY,       					FSM_STATE_ENTRY,            				fsm_canitfleo_task_entry}, //Starting

	//System level initialization, only controlled by HCU-MAIN
    {MSG_ID_COM_INIT,       				FSM_STATE_IDLE,            					fsm_canitfleo_init},
    {MSG_ID_COM_RESTART,					FSM_STATE_IDLE,            					fsm_canitfleo_restart},
    {MSG_ID_COM_INIT_FEEDBACK,				FSM_STATE_IDLE,            					fsm_com_do_nothing},

	//Task level initialization
    {MSG_ID_COM_INIT,       				FSM_STATE_CANITFLEO_INITED,            		fsm_canitfleo_init},
    {MSG_ID_COM_RESTART,					FSM_STATE_CANITFLEO_INITED,            		fsm_canitfleo_restart},
    {MSG_ID_COM_INIT_FEEDBACK,				FSM_STATE_CANITFLEO_INITED,            		fsm_com_do_nothing},

    //Normal task status
    {MSG_ID_COM_RESTART,        			FSM_STATE_CANITFLEO_ACTIVED,            	fsm_canitfleo_restart},
    {MSG_ID_COM_INIT_FEEDBACK,				FSM_STATE_CANITFLEO_ACTIVED,            	fsm_com_do_nothing},
	{MSG_ID_COM_TIME_OUT,      				FSM_STATE_CANITFLEO_ACTIVED,          		fsm_canitfleo_timeout},
	{MSG_ID_L3BFSC_CAN_WS_INIT_REQ,      	FSM_STATE_CANITFLEO_ACTIVED,          		fsm_canitfleo_l3bfsc_ws_init_req},  //称重传感器初始化
	{MSG_ID_L3BFSC_CAN_ERROR_INQ_CMD_REQ,   FSM_STATE_CANITFLEO_ACTIVED,          		fsm_canitfleo_l3bfsc_error_inq_cmd_req},  //发送指令
	{MSG_ID_L3BFSC_CAN_WS_COMB_OUT,      	FSM_STATE_CANITFLEO_ACTIVED,          		fsm_canitfleo_l3bfsc_ws_comb_out},  //组合指令
	{MSG_ID_L3BFSC_CAN_WS_GIVE_UP,      	FSM_STATE_CANITFLEO_ACTIVED,          		fsm_canitfleo_l3bfsc_ws_give_up},   //剔除指令
	{MSG_ID_L3BFSC_CAN_WS_READ_REQ,      	FSM_STATE_CANITFLEO_ACTIVED,          		fsm_canitfleo_l3bfsc_ws_read_req},   //周期性读取
	{MSG_ID_L3BFSC_CAN_GENERAL_CMD_REQ,     FSM_STATE_CANITFLEO_ACTIVED,          		fsm_canitfleo_l3bfsc_general_cmd_req}, //后台的指令

	//结束点，固定定义，不要改动
    {MSG_ID_END,            	FSM_STATE_END,             				NULL},  //Ending
};

//Global variables
extern HcuSysEngParTablet_t zHcuSysEngPar; //全局工程参数控制表
UINT32 HcuSensorIdRoundBing;

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
		hcu_usleep(dest_id*HCU_DURATION_OF_INIT_FB_WAIT_MAX);

		ret = hcu_message_send(MSG_ID_COM_INIT_FEEDBACK, src_id, TASK_ID_CANITFLEO, &snd0, snd0.length);
		if (ret == FAILURE){
			HcuErrorPrint("CANITFLEO: Send message error, TASK [%s] to TASK[%s]!\n", zHcuTaskNameList[TASK_ID_CANITFLEO], zHcuTaskNameList[src_id]);
			return FAILURE;
		}
	}

	//收到初始化消息后，进入初始化状态
	if (FsmSetState(TASK_ID_CANITFLEO, FSM_STATE_CANITFLEO_INITED) == FAILURE){
		HcuErrorPrint("CANITFLEO: Error Set FSM State!\n");
		return FAILURE;
	}

	//初始化硬件接口
	if (func_canitfleo_int_init() == FAILURE){
		HcuErrorPrint("CANITFLEO: Error initialize interface!\n");
		return FAILURE;
	}

	//Global Variables
	zHcuRunErrCnt[TASK_ID_CANITFLEO] = 0;
	HcuSensorIdRoundBing = 0;

	//启动定时器：放在初始化完成之后再启动，仅仅是为了测试目的
/*	ret = hcu_timer_start(TASK_ID_CANITFLEO, TIMER_ID_1S_CANITFLEO_WORKING_SCAN, HCU_CANITFLEO_TIMER_WORKING_SCAN_DURATION, TIMER_TYPE_PERIOD, TIMER_RESOLUTION_1S);
	if (ret == FAILURE){
		zHcuRunErrCnt[TASK_ID_CANITFLEO]++;
		HcuErrorPrint("CANITFLEO: Error start timer!\n");
		return FAILURE;
	}*/

	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_CANITFLEO, FSM_STATE_CANITFLEO_ACTIVED) == FAILURE){
		zHcuRunErrCnt[TASK_ID_CANITFLEO]++;
		HcuErrorPrint("CANITFLEO: Error Set FSM State!\n");
		return FAILURE;
	}
	if ((zHcuSysEngPar.debugMode & HCU_TRACE_DEBUG_FAT_ON) != FALSE){
		HcuDebugPrint("CANITFLEO: Enter FSM_STATE_CANITFLEO_ACTIVED status, Keeping refresh here!\n");
	}

	return SUCCESS;
}

//RESTART重启动
OPSTAT fsm_canitfleo_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HcuErrorPrint("CANITFLEO: Internal error counter reach DEAD level, SW-RESTART soon!\n");
	zHcuGlobalCounter.restartCnt++;
	fsm_canitfleo_init(0, 0, NULL, 0);
	return SUCCESS;
}

//本模块初始化的机会
OPSTAT func_canitfleo_int_init(void)
{
	return SUCCESS;
}

//暂时啥都不干，定时到达后，还不明确是否需要支持定时工作
//业务逻辑还不明确
//已经启动，用于产生假的传感器称重数据，用于参数L3BFSC组合算法
OPSTAT fsm_canitfleo_timeout(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret;

	//Receive message and copy to local variable
	msg_struct_com_time_out_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_com_time_out_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_com_time_out_t))){
		HcuErrorPrint("CANITFLEO: Receive message error!\n");
		zHcuRunErrCnt[TASK_ID_CANITFLEO]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//钩子在此处，检查zHcuRunErrCnt[TASK_ID_CANITFLEO]是否超限
	if (zHcuRunErrCnt[TASK_ID_CANITFLEO] > HCU_RUN_ERROR_LEVEL_2_MAJOR){
		//减少重复RESTART的概率
		zHcuRunErrCnt[TASK_ID_CANITFLEO] = zHcuRunErrCnt[TASK_ID_CANITFLEO] - HCU_RUN_ERROR_LEVEL_2_MAJOR;
		msg_struct_com_restart_t snd0;
		memset(&snd0, 0, sizeof(msg_struct_com_restart_t));
		snd0.length = sizeof(msg_struct_com_restart_t);
		ret = hcu_message_send(MSG_ID_COM_RESTART, TASK_ID_CANITFLEO, TASK_ID_CANITFLEO, &snd0, snd0.length);
		if (ret == FAILURE){
			zHcuRunErrCnt[TASK_ID_CANITFLEO]++;
			HcuErrorPrint("CANITFLEO: Send message error, TASK [%s] to TASK[%s]!\n", zHcuTaskNameList[TASK_ID_CANITFLEO], zHcuTaskNameList[TASK_ID_CANITFLEO]);
			return FAILURE;
		}
	}

	//Period time out received
	if ((rcv.timeId == TIMER_ID_1S_CANITFLEO_WORKING_SCAN) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		//保护周期读数的优先级，强制抢占状态，并简化问题
		if (FsmGetState(TASK_ID_CANITFLEO) != FSM_STATE_CANITFLEO_ACTIVED){
			ret = FsmSetState(TASK_ID_CANITFLEO, FSM_STATE_CANITFLEO_ACTIVED);
			if (ret == FAILURE){
				zHcuRunErrCnt[TASK_ID_CANITFLEO]++;
				HcuErrorPrint("CANITFLEO: Error Set FSM State!\n");
				return FAILURE;
			}//FsmSetState
		}
		func_canitfleo_working_scan_process();
	}

#if (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFSC_CBU_ID)
	//测试目的，正式程序中可以去掉
	else if ((rcv.timeId == TIMER_ID_10MS_CANITFLEO_SIMULATION_DATA) &&(rcv.timeRes == TIMER_RESOLUTION_10MS)){
		//保护周期读数的优先级，强制抢占状态，并简化问题
		if (FsmGetState(TASK_ID_CANITFLEO) != FSM_STATE_CANITFLEO_ACTIVED){
			ret = FsmSetState(TASK_ID_CANITFLEO, FSM_STATE_CANITFLEO_ACTIVED);
			if (ret == FAILURE){
				zHcuRunErrCnt[TASK_ID_CANITFLEO]++;
				HcuErrorPrint("CANITFLEO: Error Set FSM State!\n");
				return FAILURE;
			}//FsmSetState
		}
		func_canitfleo_working_scan_process();
	}
#endif
	return SUCCESS;
}

//整个传感器系统的初始化
OPSTAT fsm_canitfleo_l3bfsc_ws_init_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;
	int i=0;

	//入参检查
	msg_struct_l3bfsc_can_ws_init_req_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_l3bfsc_can_ws_init_req_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_l3bfsc_can_ws_init_req_t))){
		HcuErrorPrint("CANITFLEO: Receive message error!\n");
		zHcuRunErrCnt[TASK_ID_CANITFLEO]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

#if (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFSC_CBU_ID)
	//生成命令分别发送命令给各个下位机
	strHcuCanitfleoCmdFrame_t p;
	for (i=0; i<HCU_BFSC_SENSOR_WS_NBR_MAX; i++){
		if (rcv.wsBitmap[i] ==1){
			//生成CAN命令
			memset(&p, 0, sizeof(strHcuCanitfleoCmdFrame_t));
			if (func_canitfleo_frame_encode(HCU_CANITFLEO_PREFIXH_ws_ctrl, HCU_CANITFLEO_OPTID_weight_scale_calibration, HCU_CANITFLEO_OPTPAR_weight_scale_calibration_0, 0, &p) == FAILURE){
				zHcuRunErrCnt[TASK_ID_CANITFLEO]++;
				HcuErrorPrint("CANITFLEO: Generate CAN Frame error!\n");
				return FAILURE;
			}
			//发送CAN命令出去

		}
	}

	//本来没有立即返回的结果，这里为了测试，立即返回所有12个秤盘的初始化结果
	for (i=0; i<HCU_BFSC_SENSOR_WS_NBR_MAX; i++){
		msg_struct_can_l3bfsc_ws_init_fb_t snd;
		memset(&snd, 0, sizeof(msg_struct_can_l3bfsc_ws_init_fb_t));
		snd.length = sizeof(msg_struct_can_l3bfsc_ws_init_fb_t);
		snd.sensorid = i;
		snd.initFlag = 1; //标明是初始化成功了

		//等待一会儿
		hcu_usleep(10 + rand()%10);
		ret = hcu_message_send(MSG_ID_CAN_L3BFSC_WS_INIT_FB, TASK_ID_L3BFSC, TASK_ID_CANITFLEO, &snd, snd.length);
		if (ret == FAILURE){
			zHcuRunErrCnt[TASK_ID_CANITFLEO]++;
			HcuErrorPrint("CANITFLEO: Send message error, TASK [%s] to TASK[%s]!\n", zHcuTaskNameList[TASK_ID_CANITFLEO], zHcuTaskNameList[TASK_ID_L3BFSC]);
			return FAILURE;
		}
	}

	//启动定时器：放在初始化完成之后再启动，仅仅是为了测试目的
/*	ret = hcu_timer_start(TASK_ID_CANITFLEO, TIMER_ID_1S_CANITFLEO_WORKING_SCAN, HCU_CANITFLEO_TIMER_WORKING_SCAN_DURATION, TIMER_TYPE_PERIOD, TIMER_RESOLUTION_1S);
	if (ret == FAILURE){
		zHcuRunErrCnt[TASK_ID_CANITFLEO]++;
		HcuErrorPrint("CANITFLEO: Error start timer!\n");
		return FAILURE;
	}*/

	//启动定时器：更快的毫秒级定时器
	ret = hcu_timer_start(TASK_ID_CANITFLEO, TIMER_ID_10MS_CANITFLEO_SIMULATION_DATA, HCU_CANITFLEO_TIMER_10MS_SIM_DATA, TIMER_TYPE_PERIOD, TIMER_RESOLUTION_10MS);
	if (ret == FAILURE){
		zHcuRunErrCnt[TASK_ID_CANITFLEO]++;
		HcuErrorPrint("CANITFLEO: Error start timer!\n");
		return FAILURE;
	}
#endif
	//返回
	return SUCCESS;
}


//这本来就是L3BFSC专用的消息及处理函数，由于简化系统设计，消息的统一化以及VMTASK的统一化，造成所有这些函数必须在消息路由表中，必须在各个项目中都可以出现
//这必然造成了处理的具体内容需要根据不同项目进行分类处理
OPSTAT fsm_canitfleo_l3bfsc_error_inq_cmd_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int i=0, ret=0;

	msg_struct_l3bfsc_can_error_inq_cmd_req_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_l3bfsc_can_error_inq_cmd_req_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_l3bfsc_can_error_inq_cmd_req_t))){
		HcuErrorPrint("CANITFLEO: Receive message error!\n");
		zHcuRunErrCnt[TASK_ID_CANITFLEO]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

#if (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFSC_CBU_ID)
	//应该生成命令发送给下位机

	//为了测试目的
	if (rcv.sensorid < HCU_BFSC_SENSOR_WS_NBR_MAX){
		hcu_usleep(10 + rand()%10);
		msg_struct_can_l3bfsc_error_inq_cmd_resp_t snd;
		memset(&snd, 0, sizeof(msg_struct_can_l3bfsc_error_inq_cmd_resp_t));
		snd.length = sizeof(msg_struct_can_l3bfsc_error_inq_cmd_resp_t);
		snd.sensorid = rcv.sensorid;
		snd.sensorWsValue = 400 + rand()%100;
		snd.flag = 1;
		ret = hcu_message_send(MSG_ID_CAN_L3BFSC_ERROR_INQ_CMD_RESP, TASK_ID_L3BFSC, TASK_ID_CANITFLEO, &snd, snd.length);
		if (ret == FAILURE){
			zHcuRunErrCnt[TASK_ID_CANITFLEO]++;
			HcuErrorPrint("CANITFLEO: Send message error, TASK [%s] to TASK[%s]!\n", zHcuTaskNameList[TASK_ID_CANITFLEO], zHcuTaskNameList[TASK_ID_L3BFSC]);
			return FAILURE;
		}
	}

	//独特的技巧，用于复用同一个消息结构
	//使用了BITMAP的方式
	else{
		for (i=0; i<HCU_BFSC_SENSOR_WS_NBR_MAX; i++){
			if (rcv.sensorBitmap[i] ==1){
				hcu_usleep(10 + rand()%10);
				msg_struct_can_l3bfsc_error_inq_cmd_resp_t snd;
				memset(&snd, 0, sizeof(msg_struct_can_l3bfsc_error_inq_cmd_resp_t));
				snd.length = sizeof(msg_struct_can_l3bfsc_error_inq_cmd_resp_t);
				snd.sensorid = rcv.sensorid;
				snd.sensorWsValue = 400 + rand()%100;
				snd.flag = 1;
				ret = hcu_message_send(MSG_ID_CAN_L3BFSC_ERROR_INQ_CMD_RESP, TASK_ID_L3BFSC, TASK_ID_CANITFLEO, &snd, snd.length);
				if (ret == FAILURE){
					zHcuRunErrCnt[TASK_ID_CANITFLEO]++;
					HcuErrorPrint("CANITFLEO: Send message error, TASK [%s] to TASK[%s]!\n", zHcuTaskNameList[TASK_ID_CANITFLEO], zHcuTaskNameList[TASK_ID_L3BFSC]);
					return FAILURE;
				}
			}
		}
	}
#endif
	//返回
	return SUCCESS;
}

//组合算法的输出结果
OPSTAT fsm_canitfleo_l3bfsc_ws_comb_out(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0, i=0;

	msg_struct_l3bfsc_can_ws_comb_out_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_l3bfsc_can_ws_comb_out_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_l3bfsc_can_ws_comb_out_t))){
		HcuErrorPrint("CANITFLEO: Receive message error!\n");
		zHcuRunErrCnt[TASK_ID_CANITFLEO]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//将收到的传感器发送到下位机

	//等待返回命令

#if (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFSC_CBU_ID)
	//测试命令，发送结果给L3BFSC
	for (i=0; i<HCU_BFSC_SENSOR_WS_NBR_MAX; i++){
		if (rcv.sensorBitmap[i] == 1){
			hcu_usleep(10 + rand()%10);
			msg_struct_can_l3bfsc_ws_comb_out_fb_t snd;
			memset(&snd, 0, sizeof(msg_struct_can_l3bfsc_ws_comb_out_fb_t));
			snd.sensorid = i;
			snd.length = sizeof(msg_struct_can_l3bfsc_ws_comb_out_fb_t);

			ret = hcu_message_send(MSG_ID_CAN_L3BFSC_WS_COMB_OUT_FB, TASK_ID_L3BFSC, TASK_ID_CANITFLEO, &snd, snd.length);
			if (ret == FAILURE){
				zHcuRunErrCnt[TASK_ID_CANITFLEO]++;
				HcuErrorPrint("CANITFLEO: Send message error, TASK [%s] to TASK[%s]!\n", zHcuTaskNameList[TASK_ID_CANITFLEO], zHcuTaskNameList[TASK_ID_L3BFSC]);
				return FAILURE;
			}
		}
	}
#endif

	//返回
	return SUCCESS;
}

//剔除算法
OPSTAT fsm_canitfleo_l3bfsc_ws_give_up(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
#if (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFSC_CBU_ID)
	int ret=0, i=0;

	msg_struct_l3bfsc_can_ws_give_up_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_l3bfsc_can_ws_give_up_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_l3bfsc_can_ws_give_up_t))){
		HcuErrorPrint("CANITFLEO: Receive message error!\n");
		zHcuRunErrCnt[TASK_ID_CANITFLEO]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//将收到的传感器发送到下位机

	//等待返回命令

	//测试命令，发送结果给L3BFSC
	for (i=0; i<HCU_BFSC_SENSOR_WS_NBR_MAX; i++){
		if (rcv.sensorBitmap[i] == 1){
			hcu_usleep(10 + rand()%10);
			msg_struct_can_l3bfsc_ws_give_up_fb_t snd;
			memset(&snd, 0, sizeof(msg_struct_can_l3bfsc_ws_give_up_fb_t));
			snd.sensorid = i;
			snd.length = sizeof(msg_struct_can_l3bfsc_ws_give_up_fb_t);

			ret = hcu_message_send(MSG_ID_CAN_L3BFSC_WS_GIVE_UP_FB, TASK_ID_L3BFSC, TASK_ID_CANITFLEO, &snd, snd.length);
			if (ret == FAILURE){
				zHcuRunErrCnt[TASK_ID_CANITFLEO]++;
				HcuErrorPrint("CANITFLEO: Send message error, TASK [%s] to TASK[%s]!\n", zHcuTaskNameList[TASK_ID_CANITFLEO], zHcuTaskNameList[TASK_ID_L3BFSC]);
				return FAILURE;
			}
		}
	}
#endif

	//返回
	return SUCCESS;
}

//周期性读取
OPSTAT fsm_canitfleo_l3bfsc_ws_read_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
#if (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFSC_CBU_ID)
	int ret=0, i=0;

	msg_struct_l3bfsc_can_ws_read_req_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_l3bfsc_can_ws_read_req_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_l3bfsc_can_ws_read_req_t))){
		HcuErrorPrint("CANITFLEO: Receive message error!\n");
		zHcuRunErrCnt[TASK_ID_CANITFLEO]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//将收到的传感器发送到下位机

	//等待返回命令

	//测试命令，发送结果给L3BFSC
	hcu_usleep(10 + rand()%10);
	msg_struct_can_l3bfsc_ws_read_resp_t snd;
	memset(&snd, 0, sizeof(msg_struct_can_l3bfsc_ws_read_resp_t));
	for (i=0; i<HCU_BFSC_SENSOR_WS_NBR_MAX; i++){
		snd.sensorWsValue[i] = ++HcuSensorIdRoundBing;
		//if (rcv.wsBitmap[i] == 1){
		//	snd.sensorWsValue[i] = rand()%1000;
		//}
	}
	snd.length = sizeof(msg_struct_can_l3bfsc_ws_read_resp_t);
	ret = hcu_message_send(MSG_ID_CAN_L3BFSC_WS_READ_RESP, TASK_ID_L3BFSC, TASK_ID_CANITFLEO, &snd, snd.length);
	if (ret == FAILURE){
		zHcuRunErrCnt[TASK_ID_CANITFLEO]++;
		HcuErrorPrint("CANITFLEO: Send message error, TASK [%s] to TASK[%s]!\n", zHcuTaskNameList[TASK_ID_CANITFLEO], zHcuTaskNameList[TASK_ID_L3BFSC]);
		return FAILURE;
	}
#endif

	//返回
	return SUCCESS;
}

//后台的控制指令：启动/停止组合秤
OPSTAT fsm_canitfleo_l3bfsc_general_cmd_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
#if (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFSC_CBU_ID)
	int ret=0;

	msg_struct_l3bfsc_can_general_cmd_req_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_l3bfsc_can_general_cmd_req_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_l3bfsc_can_general_cmd_req_t))){
		HcuErrorPrint("CANITFLEO: Receive message error!\n");
		zHcuRunErrCnt[TASK_ID_CANITFLEO]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//将收到的传感器发送到下位机
	//这里需要控制发送所有的命令，也就是连续发送12个CAN命令到每一个传感器

	//等待返回命令

	//测试命令，发送结果给L3BFSC
	hcu_usleep(10 + rand()%10);
	msg_struct_can_l3bfsc_general_cmd_resp_t snd;
	memset(&snd, 0, sizeof(msg_struct_can_l3bfsc_general_cmd_resp_t));
	snd.optid = rcv.optid;
	snd.optpar = rcv.optpar;
	snd.sensorid = rcv.sensorid;
	snd.length = sizeof(msg_struct_can_l3bfsc_general_cmd_resp_t);
	ret = hcu_message_send(MSG_ID_CAN_L3BFSC_GENERAL_CMD_RESP, TASK_ID_L3BFSC, TASK_ID_CANITFLEO, &snd, snd.length);
	if (ret == FAILURE){
		zHcuRunErrCnt[TASK_ID_CANITFLEO]++;
		HcuErrorPrint("CANITFLEO: Send message error, TASK [%s] to TASK[%s]!\n", zHcuTaskNameList[TASK_ID_CANITFLEO], zHcuTaskNameList[TASK_ID_L3BFSC]);
		return FAILURE;
	}
#endif

	//返回
	return SUCCESS;
}


//发送测试数据给L3BFSC
void func_canitfleo_working_scan_process(void)
{
#if (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFSC_CBU_ID)
	int ret = 0;

	//这里的定时工作，是为了测试目标。
	//这里的定时已经启动，是为了定时产生假测试数据，喂给L3BFSC模块，以便测试算法
	msg_struct_can_l3bfsc_new_ready_event_t snd;
	memset(&snd, 0, sizeof(msg_struct_can_l3bfsc_new_ready_event_t));
	snd.length = sizeof(msg_struct_can_l3bfsc_new_ready_event_t);
/*	HcuSensorIdRoundBing++;
	HcuSensorIdRoundBing = (HcuSensorIdRoundBing % HCU_BFSC_SENSOR_WS_NBR_MAX);
	snd.sensorid = HcuSensorIdRoundBing;*/
	snd.sensorid = rand() % HCU_BFSC_SENSOR_WS_NBR_MAX;
	snd.sensorWsValue = 500 + (rand()%50);

	ret = hcu_message_send(MSG_ID_CAN_L3BFSC_WS_NEW_READY_EVENT, TASK_ID_L3BFSC, TASK_ID_CANITFLEO, &snd, snd.length);
	if (ret == FAILURE){
		zHcuRunErrCnt[TASK_ID_CANITFLEO]++;
		HcuErrorPrint("CANITFLEO: Send message error, TASK [%s] to TASK[%s]!\n", zHcuTaskNameList[TASK_ID_CANITFLEO], zHcuTaskNameList[TASK_ID_L3BFSC]);
		return;
	}
#endif

}

//生成FRAME函数
OPSTAT func_canitfleo_frame_encode(UINT8 prefixcmdid, UINT8 optid, UINT8 optpar, UINT32 modbusval, strHcuCanitfleoCmdFrame_t *pframe)
{
	//先检查输入参数
	if ((optid <= HCU_CANITFLEO_OPTID_min) || (optid >= HCU_CANITFLEO_OPTID_max) || (pframe == NULL)){
		zHcuRunErrCnt[TASK_ID_CANITFLEO]++;
		HcuErrorPrint("CANITFLEO: Input parameters error!\n");
		return FAILURE;
	}

	//按字节，生成消息结构帧
	pframe->bfscCmdPrefixH = prefixcmdid;
	pframe->bfscCmdPrefixL = HCU_CANITFLEO_CMD_PREFIXL;
	pframe->bfscCmdId = HCU_CANITFLEO_CMD_BFSC_ID;
	pframe->bfscOptId = optid;

	//分别针对不同的OPTID进行帧的分类处理
	switch(optid){
	case HCU_CANITFLEO_OPTID_wegith_read:
		//do nothing
		break;

	case HCU_CANITFLEO_OPTID_auto_zero_track:
		//Modbus值起作用
		pframe->bfscOptPar = modbusval & 0xFF;
		pframe->bfscPar1 = ((modbusval & 0xFF00) >> 8) & 0xFF;
		pframe->bfscPar2 = ((modbusval & 0xFF0000) >> 16) & 0xFF;
		pframe->bfscPar3 = ((modbusval & 0xFF000000) >> 24) & 0xFF;
		break;

	case HCU_CANITFLEO_OPTID_min_sensitivity:
		//Modbus值起作用
		pframe->bfscOptPar = modbusval & 0xFF;
		pframe->bfscPar1 = ((modbusval & 0xFF00) >> 8) & 0xFF;
		pframe->bfscPar2 = ((modbusval & 0xFF0000) >> 16) & 0xFF;
		pframe->bfscPar3 = ((modbusval & 0xFF000000) >> 24) & 0xFF;
		break;

	case HCU_CANITFLEO_OPTID_manual_set_zero:
		//Modbus值起作用
		pframe->bfscOptPar = modbusval & 0xFF;
		pframe->bfscPar1 = ((modbusval & 0xFF00) >> 8) & 0xFF;
		pframe->bfscPar2 = ((modbusval & 0xFF0000) >> 16) & 0xFF;
		pframe->bfscPar3 = ((modbusval & 0xFF000000) >> 24) & 0xFF;
		break;

	case HCU_CANITFLEO_OPTID_static_detect_range:
		//Modbus值起作用
		pframe->bfscOptPar = modbusval & 0xFF;
		pframe->bfscPar1 = ((modbusval & 0xFF00) >> 8) & 0xFF;
		pframe->bfscPar2 = ((modbusval & 0xFF0000) >> 16) & 0xFF;
		pframe->bfscPar3 = ((modbusval & 0xFF000000) >> 24) & 0xFF;
		break;

	case HCU_CANITFLEO_OPTID_static_detect_duration:
		//Modbus值起作用
		pframe->bfscOptPar = modbusval & 0xFF;
		pframe->bfscPar1 = ((modbusval & 0xFF00) >> 8) & 0xFF;
		pframe->bfscPar2 = ((modbusval & 0xFF0000) >> 16) & 0xFF;
		pframe->bfscPar3 = ((modbusval & 0xFF000000) >> 24) & 0xFF;
		break;

	case HCU_CANITFLEO_OPTID_weight_scale_calibration:
		pframe->bfscOptPar = optpar;
		break;

	case HCU_CANITFLEO_OPTID_motor_turn_around:
		pframe->bfscOptPar = optpar;
		break;

	case HCU_CANITFLEO_OPTID_motor_speed:
		//do nothing
		break;

	case HCU_CANITFLEO_OPTID_scale_range:
		//do nothing
		break;

	default:
		zHcuRunErrCnt[TASK_ID_CANITFLEO]++;
		HcuErrorPrint("CANITFLEO: Input parameters error!\n");
		return FAILURE;
		break;
	}

	//返回
	return SUCCESS;
}

//解码FRAME的函数
OPSTAT func_canitfleo_frame_decode(strHcuCanitfleoCmdFrame_t *pframe, UINT8 prefixcmdid, UINT8 optid, UINT8 optpar, UINT32 modbusval)
{
	//入参检查
	if (pframe == NULL){
		zHcuRunErrCnt[TASK_ID_CANITFLEO]++;
		HcuErrorPrint("CANITFLEO: Input parameters error!\n");
		return FAILURE;
	}

	//检查最重要的参数
	if ((pframe->bfscCmdPrefixH <= HCU_CANITFLEO_PREFIXH_min) || (pframe->bfscCmdPrefixH >= HCU_CANITFLEO_PREFIXH_max) || (pframe->bfscCmdPrefixL != HCU_CANITFLEO_CMD_PREFIXL) || (pframe->bfscCmdId != HCU_CANITFLEO_CMD_BFSC_ID)){
		zHcuRunErrCnt[TASK_ID_CANITFLEO]++;
		HcuErrorPrint("CANITFLEO: Frame header error!\n");
		return FAILURE;
	}
	prefixcmdid= pframe->bfscCmdId;
	optid = pframe->bfscOptId;
	if ((optid <=HCU_CANITFLEO_OPTID_min) || (optid >=HCU_CANITFLEO_OPTID_max)){
		zHcuRunErrCnt[TASK_ID_CANITFLEO]++;
		HcuErrorPrint("CANITFLEO: Encoding Canitf frame OptId error!\n");
		return FAILURE;
	}

	//分类处理不同的情况
	switch(optid){
	case HCU_CANITFLEO_OPTID_wegith_read:
		//do nothing
		break;

	case HCU_CANITFLEO_OPTID_auto_zero_track:
		//Modbus值起作用
		modbusval = pframe->bfscOptPar + (pframe->bfscPar1 << 8) + (pframe->bfscPar2 << 16) + (pframe->bfscPar3 << 24);
		break;

	case HCU_CANITFLEO_OPTID_min_sensitivity:
		//Modbus值起作用
		modbusval = pframe->bfscOptPar + (pframe->bfscPar1 << 8) + (pframe->bfscPar2 << 16) + (pframe->bfscPar3 << 24);
		break;

	case HCU_CANITFLEO_OPTID_manual_set_zero:
		//Modbus值起作用
		modbusval = pframe->bfscOptPar + (pframe->bfscPar1 << 8) + (pframe->bfscPar2 << 16) + (pframe->bfscPar3 << 24);
		break;

	case HCU_CANITFLEO_OPTID_static_detect_range:
		//Modbus值起作用
		modbusval = pframe->bfscOptPar + (pframe->bfscPar1 << 8) + (pframe->bfscPar2 << 16) + (pframe->bfscPar3 << 24);
		break;

	case HCU_CANITFLEO_OPTID_static_detect_duration:
		//Modbus值起作用
		modbusval = pframe->bfscOptPar + (pframe->bfscPar1 << 8) + (pframe->bfscPar2 << 16) + (pframe->bfscPar3 << 24);
		break;

	case HCU_CANITFLEO_OPTID_weight_scale_calibration:
		optpar = pframe->bfscOptPar;
		break;

	case HCU_CANITFLEO_OPTID_motor_turn_around:
		optpar = pframe->bfscOptPar;
		break;

	case HCU_CANITFLEO_OPTID_motor_speed:
		//do nothing
		break;

	case HCU_CANITFLEO_OPTID_scale_range:
		//do nothing
		break;

	default:
		zHcuRunErrCnt[TASK_ID_CANITFLEO]++;
		HcuErrorPrint("CANITFLEO: Input parameters error!\n");
		return FAILURE;
		break;
	}

	//返回
	return SUCCESS;
}



#define MAXSOCK 16    /* max. number of CAN interfaces given on the cmdline */
#define MAXIFNAMES 30 /* size of receive name index to omit ioctls */
#define MAXCOL 6      /* number of different colors for colorized output */
#define ANYDEV "any"  /* name of interface to receive from any CAN interface */
#define ANL "\r\n"    /* newline in ASC mode */

#define SILENT_INI 42 /* detect user setting on commandline */
#define SILENT_OFF 0  /* no silent mode */
#define SILENT_ANI 1  /* silent mode with animation */
#define SILENT_ON  2  /* silent mode (completely silent) */

//#define BOLD    ATTBOLD
//#define RED     ATTBOLD FGRED
//#define GREEN   ATTBOLD FGGREEN
//#define YELLOW  ATTBOLD FGYELLOW
//#define BLUE    ATTBOLD FGBLUE
//#define MAGENTA ATTBOLD FGMAGENTA
//#define CYAN    ATTBOLD FGCYAN

//const char col_on [MAXCOL][19] = {BLUE, RED, GREEN, BOLD, MAGENTA, CYAN};
const char col_off []; //ATTRESET;

static char *cmdlinename[MAXSOCK];
static __u32 dropcnt[MAXSOCK];
static __u32 last_dropcnt[MAXSOCK];
static char devname[MAXIFNAMES][IFNAMSIZ+1];
static int  dindex[MAXIFNAMES];
static int  max_devname_len; /* to prevent frazzled device name output */

#define MAXANI 4
const char anichar[MAXANI] = {'|', '/', '-', '\\'};

#define MAX_CANFRAME      "12345678#01.23.45.67.89.AB.CD.EF"

extern int optind, opterr, optopt;

static volatile int running = 1;

/*
 *
 * Initialize the SocketCAN
 * In: char *canitfname, like "can0"
 * Out: int *sock, sick id after initialized
 * Return: 1 Failure, 0 Success
 * MAKE SURE "sudo /sbin/ip link set can0 up type can bitrate 500000"
 *
 */
OPSTAT func_canitfleo_can_init(char *canitfname, int *sock)
{

	int s; /* can raw socket */
	//int nbytes;
	struct sockaddr_can addr;
	//struct can_frame frame;
	struct ifreq ifr;

	/* check command line options */
	if (NULL == canitfname) {
		fprintf(stderr, "canitfname == NULL\n");
		return 1;
	}

	/* open socket */
	if ((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
		perror("socket open error");
		return 1;
	}

	addr.can_family = AF_CAN;

	strcpy(ifr.ifr_name, canitfname);
	if (ioctl(s, SIOCGIFINDEX, &ifr) < 0) {
		perror("SIOCGIFINDEX");
		return 1;
	}
	addr.can_ifindex = ifr.ifr_ifindex;

	/* disable default receive filter on this RAW socket */
	/* This is obsolete as we do not read from the socket at all, but for */
	/* this reason we can remove the receive list in the Kernel to save a */
	/* little (really a very little!) CPU usage.                          */
	//setsockopt(s, SOL_CAN_RAW, CAN_RAW_FILTER, NULL, 0);
	/* comment this will be able to disable the filter */

	if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		perror("bind");
		return 1;
	}

	/* Return / Get back the socket ID */
	*sock = s;

	return 0;
}

/*
 *
 * Send out a CAN frame with CAN ID
 * In: int socket, socket which has been initialized already
 * In: char *canid_canframe, can ID + can frame in the string format of "CANID#CANFRAME", like "602#0201030400000000"
 * Return: 1 Failure, 0 Success
 *
 */
OPSTAT func_canitfleo_can_send(int socket, char *canid_canframe)
{

	int s; /* can raw socket */
	int nbytes;
	struct can_frame frame;

	if (0 == socket || -1 == socket) {
		fprintf(stderr, "Invalid Socket ID.\n");
		return 1;
	}

	s = socket;

	/* check command line options */
	if (NULL == canid_canframe) {
		fprintf(stderr, "canid_canframe == NULL\n");
		return 1;
	}

	/* parse CAN frame */
	if (parse_canframe(canid_canframe, &frame)){
		fprintf(stderr, "\nWrong CAN-frame format!\n\n");
		fprintf(stderr, "Try: <can_id>#{R|data}\n");
		fprintf(stderr, "can_id can have 3 (SFF) or 8 (EFF) hex chars\n");
		fprintf(stderr, "data has 0 to 8 hex-values that can (optionally)");
		fprintf(stderr, " be seperated by '.'\n\n");
		fprintf(stderr, "e.g. 5A1#11.2233.44556677.88 / 123#DEADBEEF / ");
		fprintf(stderr, "5AA# /\n     1F334455#1122334455667788 / 123#R ");
		fprintf(stderr, "for remote transmission request.\n\n");
		return 1;
	}

	/* send frame */
	if ((nbytes = write(s, &frame, sizeof(frame))) != sizeof(frame)) {
		perror("write");
		return 1;
	}

	return 0;
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

OPSTAT func_canitfleo_can_receive(int socket, canid_t *canid, char *canframe_hex, char *canid_canframe_char)
{

	int s; /* can raw socket */
	int nbytes;
	struct sockaddr_can addr;
	struct can_frame frame;


	char ctrlmsg[CMSG_SPACE(sizeof(struct timeval)) + CMSG_SPACE(sizeof(__u32))];
	struct iovec iov;
	struct msghdr msg;

	if (0 == socket || -1 == socket) {
		fprintf(stderr, "Invalid Socket ID.\n");
		return 1;
	}

	s = socket;

	/* check command line options */
	if (NULL == canframe_hex || NULL == canid_canframe_char) {
		fprintf(stderr, "canid_canframe == NULL\n");
		return 1;
	}

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
	//nbytes = read(s, &frame, sizeof(frame));

	if (nbytes < 0) {
		perror("read");
		return 1;
	}

	if (nbytes < sizeof(struct can_frame)) {
		fprintf(stderr, "read: incomplete CAN frame\n");
		return 1;
	}

	*canid = frame.can_id;
	memcpy(canframe_hex, frame.data, frame.can_dlc);

	//fprint_canframe(stdout, &frame, "\n", 0);
	sprint_canframe(canid_canframe_char, &frame, 0);

	return 0;

}

int func_canitfleo_test_main(int argc, char **argv)
{
	//puts("!!!Hello World!!!"); /* prints !!!Hello World!!! */
    //can_send_receive(argc, argv);

	int sock;
	int ret;
	canid_t canid = 0x0000;
	char buf[sizeof(MAX_CANFRAME)+1]; /* max length */
	char canframe_hex[256];

	int weight;
	int loop = 10, i = 0;
	int sleep_second = 2;

	printf("pi_cantest_bf [loop] [sleep_second]\r\n");
	printf("pi_cantest_bf 10 2, loop 10 time, wait for 2 seconds between each of the action\r\n");

	if(2 == argc) loop = atoi(argv[1]);
	if(3 == argc) {loop = atoi(argv[1]); sleep_second = atoi(argv[2]);}

	printf("pi_cantest_bf loop = %d sleep_second = %d\r\n", loop, sleep_second);

	/****** MAKE SURE "sudo /sbin/ip link set can0 up type can bitrate 500000" ********/
	ret = func_canitfleo_can_init("can0", &sock);
	if(1 == ret)
	{
		printf("can_init failure.\r\n");
	}
	printf("can_init successful.\r\n");

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
		ret = func_canitfleo_can_send(sock, "602#2300652001000000");
		if(1 == ret)
		{
			printf("func_canitfleo_can_send(sock, 602#2300652001000000.\r\n");
		}
		printf("func_canitfleo_can_send(sock, 602#2300652001000000 successful.\r\n");

		memset(buf, 0, sizeof(MAX_CANFRAME)+1);
		do{
			ret = func_canitfleo_can_receive(sock, &canid, canframe_hex, buf);
			if(1 == ret)
			{
				printf("can_receive(sock, &canid, &canframe_hex, &buf);\r\n");
			}
			if(canid == 0x702)
				printf("can_receive(sock, &canid, &canframe_hex, &buf), CANOpen Heartbeat [%s}\r\n", buf);
		}while(canid == 0x702);

		printf("can_receive(sock, &canid, &canframe_hex, &buf), buf = %s\r\n", buf);


		/* =============================================================*/
		/* ======================== r-clockwise ========================*/
		/* =============================================================*/
		sleep(sleep_second);
		ret = func_canitfleo_can_send(sock, "602#2300652002000000");
		if(1 == ret)
		{
			printf("func_canitfleo_can_send(sock, 602#2300652002000000.\r\n");
		}
		printf("func_canitfleo_can_send(sock, 602#2300652002000000 successful.\r\n");

		memset(buf, 0, sizeof(MAX_CANFRAME)+1);
		do{
			ret = func_canitfleo_can_receive(sock, &canid, canframe_hex, buf);
			if(1 == ret)
			{
				printf("func_canitfleo_can_receive(sock, &canid, &canframe_hex, &buf);\r\n");
			}
			if(canid == 0x702)
				printf("func_canitfleo_can_receive(sock, &canid, &canframe_hex, &buf), CANOpen Heartbeat [%s}\r\n", buf);
		}while(canid == 0x702);
		printf("func_canitfleo_can_receive(sock, &canid, &canframe_hex, &buf), buf = %s\r\n", buf);

		/* =============================================================*/
		/* ======================== stop motor  ========================*/
		/* =============================================================*/
		sleep(sleep_second);
		ret = func_canitfleo_can_send(sock, "602#2300652003000000");
		if(1 == ret)
		{
			printf("func_canitfleo_can_send(sock, 602#2300652003000000.\r\n");
		}
		printf("func_canitfleo_can_send(sock, 602#2300652003000000 successful.\r\n");

		//usleep(2000);
		memset(buf, 0, sizeof(MAX_CANFRAME)+1);
		do{
			ret = func_canitfleo_can_receive(sock, &canid, canframe_hex, buf);
			if(1 == ret)
			{
				printf("func_canitfleo_can_receive(sock, &canid, &canframe_hex, &buf);\r\n");
			}
			if(canid == 0x702)
				printf("func_canitfleo_can_receive(sock, &canid, &canframe_hex, &buf), CANOpen Heartbeat [%s}\r\n", buf);

		}while(canid == 0x702);
		printf("func_canitfleo_can_receive(sock, &canid, &canframe_hex, &buf), buf = %s\r\n", buf);

		/* =============================================================*/
		/* ======================== Weight Get  ========================*/
		/* =============================================================*/
		sleep(sleep_second);
		ret = func_canitfleo_can_send(sock, "602#4000650200000000");
		if(1 == ret)
		{
			printf("func_canitfleo_can_send(sock, 602#4000650200000000.\r\n");
		}
		printf("func_canitfleo_can_send(sock, 602#4000650200000000 successful.\r\n");

		//usleep(2000);
		memset(buf, 0, sizeof(MAX_CANFRAME)+1);
		do{
			ret = func_canitfleo_can_receive(sock, &canid, canframe_hex, buf);
			if(1 == ret)
			{
				printf("can_receive(sock, &canid, &canframe_hex, &buf);\r\n");
			}
			if(canid == 0x702)
				printf("can_receive(sock, &canid, &canframe_hex, &buf), CANOpen Heartbeat [%s}\r\n", buf);

		}while(canid == 0x702);
		weight = canframe_hex[4] + (canframe_hex[5] << 8) + (canframe_hex[6] << 16) + (canframe_hex[7] << 24);
		printf("can_receive(sock, &canid, &canframe_hex, &buf), buf = %s, weight = %d\r\n", buf, weight);

	}

	return EXIT_SUCCESS;
}

