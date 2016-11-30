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
	{MSG_ID_L3BFSC_CAN_INQ_CMD_REQ,      	FSM_STATE_CANITFLEO_ACTIVED,          		fsm_canitfleo_l3bfsc_inq_cmd_req},  //发送指令
	{MSG_ID_L3BFSC_CAN_WS_COMB_OUT,      	FSM_STATE_CANITFLEO_ACTIVED,          		fsm_canitfleo_l3bfsc_ws_comb_out},  //组合指令
	{MSG_ID_L3BFSC_CAN_WS_GIVE_UP,      	FSM_STATE_CANITFLEO_ACTIVED,          		fsm_canitfleo_l3bfsc_ws_give_up},   //剔除指令

	//结束点，固定定义，不要改动
    {MSG_ID_END,            	FSM_STATE_END,             				NULL},  //Ending
};

//Global variables
extern HcuSysEngParTablet_t zHcuSysEngPar; //全局工程参数控制表
UINT8 HcuSensorIdRoundBing;

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
			if (func_canitfleo_frame_encode(HCU_CANITFLEO_OPTID_weight_scale_calibration, HCU_CANITFLEO_OPTPAR_weight_scale_calibration_0, 0, &p) == FAILURE){
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
		ret = hcu_message_send(MSG_ID_CAN_L3FSC_WS_INIT_FB, TASK_ID_L3BFSC, TASK_ID_CANITFLEO, &snd, snd.length);
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
OPSTAT fsm_canitfleo_l3bfsc_inq_cmd_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int i=0, ret=0;

	msg_struct_l3bfsc_can_inq_cmd_req_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_l3bfsc_can_inq_cmd_req_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_l3bfsc_can_inq_cmd_req_t))){
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
		msg_struct_can_l3bfsc_inq_cmd_resp_t snd;
		memset(&snd, 0, sizeof(msg_struct_can_l3bfsc_inq_cmd_resp_t));
		snd.length = sizeof(msg_struct_can_l3bfsc_inq_cmd_resp_t);
		snd.sensorid = rcv.sensorid;
		snd.sensorWsValue = 400 + rand()%100;
		snd.flag = 1;
		ret = hcu_message_send(MSG_ID_CAN_L3BFSC_INQ_CMD_RESP, TASK_ID_L3BFSC, TASK_ID_CANITFLEO, &snd, snd.length);
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
				msg_struct_can_l3bfsc_inq_cmd_resp_t snd;
				memset(&snd, 0, sizeof(msg_struct_can_l3bfsc_inq_cmd_resp_t));
				snd.length = sizeof(msg_struct_can_l3bfsc_inq_cmd_resp_t);
				snd.sensorid = rcv.sensorid;
				snd.sensorWsValue = 400 + rand()%100;
				snd.flag = 1;
				ret = hcu_message_send(MSG_ID_CAN_L3BFSC_INQ_CMD_RESP, TASK_ID_L3BFSC, TASK_ID_CANITFLEO, &snd, snd.length);
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

#if (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFSC_CBU_ID)
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

//发送测试数据给L3BFSC
void func_canitfleo_working_scan_process(void)
{
	int ret = 0;

	//这里的定时工作，是为了测试目标。
	//这里的定时已经启动，是为了定时产生假测试数据，喂给L3BFSC模块，以便测试算法
#if (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFSC_CBU_ID)
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
OPSTAT func_canitfleo_frame_encode(UINT8 optid, UINT8 optpar, UINT32 modbusval, strHcuCanitfleoCmdFrame_t *pframe)
{
	//先检查输入参数
	if ((optid <= HCU_CANITFLEO_OPTID_min) || (optid >= HCU_CANITFLEO_OPTID_max) || (pframe == NULL)){
		zHcuRunErrCnt[TASK_ID_CANITFLEO]++;
		HcuErrorPrint("CANITFLEO: Input parameters error!\n");
		return FAILURE;
	}

	//按字节，生成消息结构帧
	pframe->bfscCmdPrefixH =HCU_CANITFLEO_CMD_PREFIXH;
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
OPSTAT func_canitfleo_frame_decode(strHcuCanitfleoCmdFrame_t *pframe, UINT8 optid, UINT8 optpar, UINT32 modbusval)
{
	//入参检查
	if (pframe == NULL){
		zHcuRunErrCnt[TASK_ID_CANITFLEO]++;
		HcuErrorPrint("CANITFLEO: Input parameters error!\n");
		return FAILURE;
	}

	//检查最重要的参数
	if ((pframe->bfscCmdPrefixH != HCU_CANITFLEO_CMD_PREFIXH) || (pframe->bfscCmdPrefixL != HCU_CANITFLEO_CMD_PREFIXL) || (pframe->bfscCmdId != HCU_CANITFLEO_CMD_BFSC_ID)){
		zHcuRunErrCnt[TASK_ID_CANITFLEO]++;
		HcuErrorPrint("CANITFLEO: Frame header error!\n");
		return FAILURE;
	}
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



