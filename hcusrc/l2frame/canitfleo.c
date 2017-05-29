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

	//{MSG_ID_L3BFSC_CAN_WS_READ_REQ,      	FSM_STATE_CANITFLEO_ACTIVED,          		fsm_canitfleo_l3bfsc_ws_read_req},   //周期性读取
	//{MSG_ID_L3BFSC_CAN_GENERAL_CMD_REQ,     FSM_STATE_CANITFLEO_ACTIVED,          	fsm_canitfleo_l3bfsc_general_cmd_req}, //后台的指令

	{MSG_ID_L3BFSC_CAN_SYS_CFG_REQ,      	FSM_STATE_CANITFLEO_ACTIVED,          		fsm_canitfleo_l3bfsc_sys_cfg_req},  	//初始化配置
	{MSG_ID_L3BFSC_CAN_SYS_START_REQ,      	FSM_STATE_CANITFLEO_ACTIVED,          		fsm_canitfleo_can_sys_start_req},   	//人工命令
	{MSG_ID_L3BFSC_CAN_SYS_STOP_REQ,      	FSM_STATE_CANITFLEO_ACTIVED,          		fsm_canitfleo_can_sys_stop_req},   		//人工命令
	{MSG_ID_UICOMM_CAN_TEST_CMD_REQ,      	FSM_STATE_CANITFLEO_ACTIVED,          		fsm_canitfleo_can_test_cmd_req},   		//测试性命令

	{MSG_ID_L3BFSC_CAN_WS_COMB_OUT,      	FSM_STATE_CANITFLEO_ACTIVED,          		fsm_canitfleo_l3bfsc_ws_comb_out},  	//组合指令
	{MSG_ID_L3BFSC_CAN_WS_GIVE_UP,      	FSM_STATE_CANITFLEO_ACTIVED,          		fsm_canitfleo_l3bfsc_ws_give_up},   	//剔除指令
	{MSG_ID_L3BFSC_CAN_ERROR_INQ_CMD_REQ,   FSM_STATE_CANITFLEO_ACTIVED,          		fsm_canitfleo_l3bfsc_error_inq_cmd_req},//差错状态下的发送指令

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

//const char anichar[MAXANI] = {'|', '/', '-', '\\'};
//const char col_on [MAXCOL][19] = {BLUE, RED, GREEN, BOLD, MAGENTA, CYAN};
//const char col_off []; //ATTRESET;
//static char *cmdlinename[MAXSOCK];
//static __u32 dropcnt[MAXSOCK];
//static __u32 last_dropcnt[MAXSOCK];
//static char devname[MAXIFNAMES][IFNAMSIZ+1];
//static int  dindex[MAXIFNAMES];
//static int  max_devname_len; /* to prevent frazzled device name output */


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

	//初始化硬件接口
	if (func_canitfleo_int_init() == FAILURE){
		HcuErrorPrint("CANITFLEO: Error initialize interface!\n");
		return FAILURE;
	}

	//Global Variables
	zHcuSysStaPm.taskRunErrCnt[TASK_ID_CANITFLEO] = 0;
	memset(&gTaskCanitfleoContext, 0, sizeof(gTaskCanitfleoContext_t));

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
	//MYC
/*
#ifdef TARGET_RASPBERRY_PI3B
	INT32 ret;
	ret = func_canitfleo_can_init("can0", &gTaskCanitfleoContext.can_socket_id);

	if(0 == ret)
		return SUCCESS;
	else
		return FAILURE;
#endif
*/

#ifdef TARGET_LINUX_X86_ADVANTECH //Added by MYC 2017/05/15
	INT32 ret;
	ret = usb_can_init(&(gTaskCanitfleoContext.can1), CAN_DEVICE_TYPE_PCI9820I, \
			CAN_DEVIDE_IDX_CARD1, CAN_DEVIDE_CHANNEL_CAN0, \
			CAN_BANDRATE_500KBPS, 0, CAN_L2_FRAME_FORWARD_YES);

	HcuDebugPrint("CANITFLEO: usb_can_init() called, ret = %d\r\n", ret);

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
		//func_canitfleo_working_scan_process(); //编译扬尘不过，暂注释掉
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
	int ret=0;
	int i=0;

	//入参检查
	msg_struct_l3bfsc_can_sys_cfg_req_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_l3bfsc_can_sys_cfg_req_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_l3bfsc_can_sys_cfg_req_t)))
		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);

	//生成命令分别发送命令给各个下位机
	strHcuCanitfleoCmdFrame_t p;
	for (i=0; i<HCU_SYSCFG_BFSC_SNR_WS_NBR_MAX; i++){
		//生成CAN命令
		memset(&p, 0, sizeof(strHcuCanitfleoCmdFrame_t));
		if (func_canitfleo_frame_encode(HCU_CANITFLEO_PREFIXH_ws_ctrl, HCU_CANITFLEO_OPTID_weight_scale_calibration, HCU_CANITFLEO_OPTPAR_weight_scale_calibration_0, 0, &p) == FAILURE)
			HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Generate CAN Frame error!\n");
		//发送CAN命令出去

	}

	//本来没有立即返回的结果，这里为了测试，立即返回所有12个秤盘的初始化结果
	for (i=0; i<HCU_SYSCFG_BFSC_SNR_WS_NBR_MAX; i++){
		msg_struct_can_l3bfsc_sys_cfg_resp_t snd;
		memset(&snd, 0, sizeof(msg_struct_can_l3bfsc_sys_cfg_resp_t));
		snd.length = sizeof(msg_struct_can_l3bfsc_sys_cfg_resp_t);
		//snd.sensorid = i;
		//snd.initFlag = 1; //标明是初始化成功了

		//等待一会儿
		hcu_usleep(10 + rand()%10);
		if (hcu_message_send(MSG_ID_CAN_L3BFSC_SYS_CFG_RESP, TASK_ID_L3BFSC, TASK_ID_CANITFLEO, &snd, snd.length) == FAILURE)
			HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName, zHcuVmCtrTab.task[TASK_ID_L3BFSC].taskName);
	}

	//启动定时器：更快的毫秒级定时器
	ret = hcu_timer_start(TASK_ID_CANITFLEO, TIMER_ID_10MS_CANITFLEO_SIMULATION_DATA, \
			zHcuSysEngPar.timer.array[TIMER_ID_10MS_CANITFLEO_SIMULATION_DATA].dur, TIMER_TYPE_PERIOD, TIMER_RESOLUTION_10MS);
	if (ret == FAILURE) HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Error start timer!\n");

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
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_l3bfsc_can_error_inq_cmd_req_t)))
		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);

	//应该生成命令发送给下位机

	//为了测试目的
	if (rcv.sensorid < HCU_SYSCFG_BFSC_SNR_WS_NBR_MAX){
		hcu_usleep(10 + rand()%10);
		msg_struct_can_l3bfsc_error_inq_cmd_resp_t snd;
		memset(&snd, 0, sizeof(msg_struct_can_l3bfsc_error_inq_cmd_resp_t));
		snd.length = sizeof(msg_struct_can_l3bfsc_error_inq_cmd_resp_t);
		snd.sensorid = rcv.sensorid;
		snd.sensorWsValue = 400 + rand()%100;
		snd.flag = 1;
		if (hcu_message_send(MSG_ID_CAN_L3BFSC_ERROR_INQ_CMD_RESP, TASK_ID_L3BFSC, TASK_ID_CANITFLEO, &snd, snd.length) == FAILURE)
			HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName, zHcuVmCtrTab.task[TASK_ID_L3BFSC].taskName);
	}

	//独特的技巧，用于复用同一个消息结构
	//使用了BITMAP的方式
	else{
		for (i=0; i<HCU_SYSCFG_BFSC_SNR_WS_NBR_MAX; i++){
			if (rcv.sensorBitmap[i] ==1){
				hcu_usleep(10 + rand()%10);
				msg_struct_can_l3bfsc_error_inq_cmd_resp_t snd;
				memset(&snd, 0, sizeof(msg_struct_can_l3bfsc_error_inq_cmd_resp_t));
				snd.length = sizeof(msg_struct_can_l3bfsc_error_inq_cmd_resp_t);
				snd.sensorid = rcv.sensorid;
				snd.sensorWsValue = 400 + rand()%100;
				snd.flag = 1;
				if (hcu_message_send(MSG_ID_CAN_L3BFSC_ERROR_INQ_CMD_RESP, TASK_ID_L3BFSC, TASK_ID_CANITFLEO, &snd, snd.length) == FAILURE)
					HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName, zHcuVmCtrTab.task[TASK_ID_L3BFSC].taskName);
			}
		}
	}
	//返回
	return SUCCESS;
}

//组合算法的输出结果
OPSTAT fsm_canitfleo_l3bfsc_ws_comb_out(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0, i=0;

	msg_struct_l3bfsc_can_ws_comb_out_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_l3bfsc_can_ws_comb_out_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_l3bfsc_can_ws_comb_out_t)))
		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);

	//将收到的传感器发送到下位机

	//等待返回命令

	//测试命令，发送结果给L3BFSC
	for (i=0; i<HCU_SYSCFG_BFSC_SNR_WS_NBR_MAX; i++){
		if (rcv.sensorBitmap[i] == 1){
			hcu_usleep(10 + rand()%10);
			msg_struct_can_l3bfsc_ws_comb_out_fb_t snd;
			memset(&snd, 0, sizeof(msg_struct_can_l3bfsc_ws_comb_out_fb_t));
			snd.sensorid = i;
			snd.length = sizeof(msg_struct_can_l3bfsc_ws_comb_out_fb_t);

			if (hcu_message_send(MSG_ID_CAN_L3BFSC_WS_COMB_OUT_FB, TASK_ID_L3BFSC, TASK_ID_CANITFLEO, &snd, snd.length) == FAILURE)
				HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName, zHcuVmCtrTab.task[TASK_ID_L3BFSC].taskName);
		}
	}

	//返回
	return SUCCESS;
}

//剔除算法
OPSTAT fsm_canitfleo_l3bfsc_ws_give_up(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0, i=0;

	msg_struct_l3bfsc_can_ws_give_up_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_l3bfsc_can_ws_give_up_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_l3bfsc_can_ws_give_up_t)))
		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);

	//将收到的传感器发送到下位机

	//等待返回命令

	//测试命令，发送结果给L3BFSC
	for (i=0; i<HCU_SYSCFG_BFSC_SNR_WS_NBR_MAX; i++){
		if (rcv.sensorBitmap[i] == 1){
			hcu_usleep(10 + rand()%10);
			msg_struct_can_l3bfsc_ws_give_up_fb_t snd;
			memset(&snd, 0, sizeof(msg_struct_can_l3bfsc_ws_give_up_fb_t));
			snd.sensorid = i;
			snd.length = sizeof(msg_struct_can_l3bfsc_ws_give_up_fb_t);

			if (hcu_message_send(MSG_ID_CAN_L3BFSC_WS_GIVE_UP_FB, TASK_ID_L3BFSC, TASK_ID_CANITFLEO, &snd, snd.length) == FAILURE)
				HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName, zHcuVmCtrTab.task[TASK_ID_L3BFSC].taskName);
		}
	}

	//返回
	return SUCCESS;
}

//L3BFSC启动的定时器，周期性强制读取并汇报给L3BFSC
//OPSTAT fsm_canitfleo_l3bfsc_ws_read_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
//{
//	int ret=0, i=0;
//
//	msg_struct_l3bfsc_can_ws_read_req_t rcv;
//	memset(&rcv, 0, sizeof(msg_struct_l3bfsc_can_ws_read_req_t));
//	if ((param_ptr == NULL || param_len > sizeof(msg_struct_l3bfsc_can_ws_read_req_t)))
//		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Receive message error!\n");
//	memcpy(&rcv, param_ptr, param_len);
//
//	//将收到的传感器发送到下位机
//
//	/* MYC ADDED FOR BF DEMO */
////	canid_t canid = 0x0000;
////	char buf[sizeof(MAX_CANFRAME)+1]; /* max length */
////	char canframe_hex[256];
////	int weight;
////		if (func_canitfleo_can_send(gTaskCanitfleoContext.can_socket_id, "602#4000650200000000") == FAILURE)
////			HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: func_canitfleo_can_send(sock, 602#4000650200000000.\n");
////
////		memset(buf, 0, sizeof(MAX_CANFRAME)+1);
////		do{
////			if (func_canitfleo_can_receive(gTaskCanitfleoContext.can_socket_id, &canid, canframe_hex, buf) == FAILURE)
////				HcuErrorPrint("CANITFLEO: can_receive(sock, &canid, &canframe_hex, &buf);\r\n");
////			if(canid == 0x702)
////				HcuDebugPrint("CANITFLEO: can_receive(sock, &canid, &canframe_hex, &buf), CANOpen Heartbeat [%s}\r\n", buf);
////		}while(canid == 0x702);
////		weight = canframe_hex[4] + (canframe_hex[5] << 8) + (canframe_hex[6] << 16) + (canframe_hex[7] << 24);
////		HCU_DEBUG_PRINT_INF("CANITFLEO: can_receive(sock, &canid, &canframe_hex, &buf), canid =0x%X, buf = %s, weight = %d\r\n", (UINT32)canid, buf, weight);
//
//	//等待返回命令
//
//	//测试命令，发送结果给L3BFSC
//	hcu_usleep(10 + rand()%10);
//	msg_struct_can_l3bfsc_ws_read_resp_t snd;
//	memset(&snd, 0, sizeof(msg_struct_can_l3bfsc_ws_read_resp_t));
//	for (i=0; i<HCU_SYSCFG_BFSC_SNR_WS_NBR_MAX; i++){
//		snd.sensorWsValue[i] = ++gTaskCanitfleoContext.sensorIdRoundBing;
//		//if ( i == (canid & 0xF))    //MYC
//		//	snd.sensorWsValue[i] = weight;
//		if (rcv.wsBitmap[i] == 1){
//			snd.sensorWsValue[i] = rand()%1000;
//		}
//	}
//	snd.length = sizeof(msg_struct_can_l3bfsc_ws_read_resp_t);
//	if (hcu_message_send(MSG_ID_CAN_L3BFSC_WS_READ_RESP, TASK_ID_L3BFSC, TASK_ID_CANITFLEO, &snd, snd.length) == FAILURE)
//		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName, zHcuVmCtrTab.task[TASK_ID_L3BFSC].taskName);
//
//	//返回
//	return SUCCESS;
//}

//后台的控制指令：启动/停止组合秤
OPSTAT fsm_canitfleo_can_sys_start_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret=0, i=0;

	msg_struct_l3bfsc_can_sys_start_req_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_l3bfsc_can_sys_start_req_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_l3bfsc_can_sys_start_req_t)))
		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);

	//调用CAN的API，将命令发送给每一个目标秤台
	//反馈由上层控制，这里不做超时处理

	//返回
	return SUCCESS;
}

//后台的控制指令：启动/停止组合秤
OPSTAT fsm_canitfleo_can_sys_stop_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret=0, i=0;

	msg_struct_l3bfsc_can_sys_stop_req_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_l3bfsc_can_sys_stop_req_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_l3bfsc_can_sys_stop_req_t)))
		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);

	//调用CAN的API，将命令发送给每一个目标秤台
	//反馈由上层控制，这里不做超时处理

	//返回
	return SUCCESS;
}



//后台的控制指令：启动/停止组合秤
//OPSTAT fsm_canitfleo_l3bfsc_general_cmd_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
//{
//	int ret=0;
//
//	canid_t canid = 0x0000;
//	char buf[sizeof(MAX_CANFRAME)+1]; /* max length */
//	char canframe_hex[256];
//	//int weight;
//
//	msg_struct_l3bfsc_can_general_cmd_req_t rcv;
//	memset(&rcv, 0, sizeof(msg_struct_l3bfsc_can_general_cmd_req_t));
//	if ((param_ptr == NULL || param_len > sizeof(msg_struct_l3bfsc_can_general_cmd_req_t)))
//		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Receive message error!\n");
//	memcpy(&rcv, param_ptr, param_len);

	//将收到的传感器发送到下位机
	//这里需要控制发送所有的命令，也就是连续发送12个CAN命令到每一个传感器

	/* MYC ADDED FOR BF DEMO */
//	if (L3PO_bfsc_start_cmd == rcv.optid)
//	{
//		if (func_canitfleo_can_send(gTaskCanitfleoContext.can_socket_id, "602#2300652002000000") == FAILURE)
//			HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: func_canitfleo_can_send(sock, 602#2300652002000000.\n");
//		memset(buf, 0, sizeof(MAX_CANFRAME)+1);
//		do{
//			if (func_canitfleo_can_receive(gTaskCanitfleoContext.can_socket_id, &canid, canframe_hex, buf) == FAILURE)
//				HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: can_receive(sock, &canid, &canframe_hex, &buf);\r\n");
//			if(canid == 0x702)
//				HCU_DEBUG_PRINT_INF("CANITFLEO: can_receive(sock, &canid, &canframe_hex, &buf), CANOpen Heartbeat [%s}\r\n", buf);
//		}while(canid == 0x702);
//	}
//	else if (L3PO_bfsc_stop_cmd == rcv.optid)
//	{
//		if (func_canitfleo_can_send(gTaskCanitfleoContext.can_socket_id, "602#2300652003000000") == FAILURE)
//			HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: func_canitfleo_can_send(sock, 602#2300652003000000.\r\n");
//		memset(buf, 0, sizeof(MAX_CANFRAME)+1);
//		do{
//			if (func_canitfleo_can_receive(gTaskCanitfleoContext.can_socket_id, &canid, canframe_hex, buf) == FAILURE)
//				HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: can_receive(sock, &canid, &canframe_hex, &buf);\r\n");
//			if(canid == 0x702)
//				HCU_DEBUG_PRINT_INF("CANITFLEO: can_receive(sock, &canid, &canframe_hex, &buf), CANOpen Heartbeat [%s}\r\n", buf);
//		}while(canid == 0x702);
//	}
//	else
//	{
//
//	}
	/* MYC ADDED FOR BF DEMO */

//	//等待返回命令
//
//	//测试命令，发送结果给L3BFSC
//	hcu_usleep(10 + rand()%10);
//	msg_struct_can_l3bfsc_general_cmd_resp_t snd;
//	memset(&snd, 0, sizeof(msg_struct_can_l3bfsc_general_cmd_resp_t));
//	snd.optid = rcv.optid;
//	snd.optpar = rcv.optpar;
//	snd.sensorid = rcv.sensorid;
//
//	snd.length = sizeof(msg_struct_can_l3bfsc_general_cmd_resp_t);
//	if (hcu_message_send(MSG_ID_CAN_L3BFSC_GENERAL_CMD_RESP, TASK_ID_L3BFSC, TASK_ID_CANITFLEO, &snd, snd.length) == FAILURE)
//		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName, zHcuVmCtrTab.task[TASK_ID_L3BFSC].taskName);
//
//	//返回
//	return SUCCESS;
//}

//收到人工测试命令
OPSTAT fsm_canitfleo_can_test_cmd_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//返回
	return SUCCESS;
}


//发送测试数据给L3BFSC
OPSTAT func_canitfleo_bfsc_simulation_data_process(void)
{
	int ret = 0;

	canid_t canid = 0x0000;
	char buf[sizeof(MAX_CANFRAME)+1]; /* max length */
	char canframe_hex[256];
	int weight;

	//这里的定时工作，是为了测试目标。
	//这里的定时已经启动，是为了定时产生假测试数据，喂给L3BFSC模块，以便测试算法
	msg_struct_can_l3bfsc_new_ready_event_t snd;
	memset(&snd, 0, sizeof(msg_struct_can_l3bfsc_new_ready_event_t));
	snd.length = sizeof(msg_struct_can_l3bfsc_new_ready_event_t);
	gTaskCanitfleoContext.sensorIdRoundBing++;
	gTaskCanitfleoContext.sensorIdRoundBing = (gTaskCanitfleoContext.sensorIdRoundBing % HCU_SYSCFG_BFSC_SNR_WS_NBR_MAX);
	snd.sensorid = gTaskCanitfleoContext.sensorIdRoundBing;

	/* MYC ADDED FOR BF DEMO */
//	ret = func_canitfleo_can_send(gTaskCanitfleoContext.can_socket_id, "602#4000650200000000");
//	if(1 == ret)
//	{
//		HcuErrorPrint("CANITFLEO: func_canitfleo_can_send(sock, 602#4000650200000000.\r\n");
//	}
//
//	memset(buf, 0, sizeof(MAX_CANFRAME)+1);
//	do{
//		ret = func_canitfleo_can_receive(gTaskCanitfleoContext.can_socket_id, &canid, canframe_hex, buf);
//		if(1 == ret)
//		{
//			HcuErrorPrint("CANITFLEO: can_receive(sock, &canid, &canframe_hex, &buf);\r\n");
//		}
//		if(canid == 0x702)
//			HcuDebugPrint("CANITFLEO: can_receive(sock, &canid, &canframe_hex, &buf), CANOpen Heartbeat [%s}\r\n", buf);
//
//	}while(canid == 0x702);
//	weight = canframe_hex[4] + (canframe_hex[5] << 8) + (canframe_hex[6] << 16) + (canframe_hex[7] << 24);
//	HcuErrorPrint("CANITFLEO: can_receive(sock, &canid, &canframe_hex, &buf), canid =0x%X, buf = %s, weight = %d\r\n", (UINT32)canid, buf, weight);
//
	/* MYC ADDED FOR BF DEMO */

	snd.sensorid = rand() % HCU_SYSCFG_BFSC_SNR_WS_NBR_MAX;
	snd.sensorWsValue = 500 + (rand()%50);
//	if (snd.sensorid == (canid & 0xF))
//		snd.sensorWsValue = weight;

	if (hcu_message_send(MSG_ID_CAN_L3BFSC_WS_NEW_READY_EVENT, TASK_ID_L3BFSC, TASK_ID_CANITFLEO, &snd, snd.length) == FAILURE)
		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName, zHcuVmCtrTab.task[TASK_ID_L3BFSC].taskName);

	return SUCCESS;
}


//生成FRAME函数
OPSTAT func_canitfleo_frame_encode(UINT8 prefixcmdid, UINT8 optid, UINT8 optpar, UINT32 modbusval, strHcuCanitfleoCmdFrame_t *pframe)
{
	//先检查输入参数
	if ((optid <= HCU_CANITFLEO_OPTID_min) || (optid >= HCU_CANITFLEO_OPTID_max) || (pframe == NULL))
		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Input parameters error!\n");

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
		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Input parameters error!\n");
		break;
	}

	//返回
	return SUCCESS;
}

//解码FRAME的函数
OPSTAT func_canitfleo_frame_decode(strHcuCanitfleoCmdFrame_t *pframe, UINT8 prefixcmdid, UINT8 optid, UINT8 optpar, UINT32 modbusval)
{
	//入参检查
	if (pframe == NULL) HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Input parameters error!\n");

	//检查最重要的参数
	if ((pframe->bfscCmdPrefixH <= HCU_CANITFLEO_PREFIXH_min) || (pframe->bfscCmdPrefixH >= HCU_CANITFLEO_PREFIXH_max) || (pframe->bfscCmdPrefixL != HCU_CANITFLEO_CMD_PREFIXL) || (pframe->bfscCmdId != HCU_CANITFLEO_CMD_BFSC_ID))
		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Frame header error!\n");

	prefixcmdid= pframe->bfscCmdId;
	optid = pframe->bfscOptId;
	if ((optid <=HCU_CANITFLEO_OPTID_min) || (optid >=HCU_CANITFLEO_OPTID_max))
		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Encoding Canitf frame OptId error!\n");

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
		HCU_ERROR_PRINT_CANITFLEO("CANITFLEO: Input parameters error!\n");
		break;
	}

	//返回
	return SUCCESS;
}



OPSTAT func_canitfleo_working_scan_process(void)
{
	return SUCCESS;
}


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
OPSTAT func_canitfleo_can_send(int socket, char *canid_canframe)
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

OPSTAT func_canitfleo_can_receive(int socket, canid_t *canid, char *canframe_hex, char *canid_canframe_char)
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


/*
 * =============================== BFSC MESSAGE PROCESS ===========================
 */

/* THESE ARE ONLY FOR TEST */

void TestSetConfigReq(uint8_t wmc_id)
{
	static uint8_t SetConfigReq[] = {
	0xfe, 0x7a, 0x84, 0x00, 0x11, 0x3B, 0x80, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x10, 0x27, 0x00, 0x00, 0x10, 0x27, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xe8, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00};

	int len;
	len = 16 * 8 + 4;

	bsp_can_l2_frame_transmit(&(gTaskCanitfleoContext.can1), SetConfigReq, len, (0x00100000 | (1 << wmc_id)));
}

void TestCommandReq()
{
	static uint8_t CommandReq[] = {
	0xfe, 0xe6, 0x18, 0x00, 0x17, 0x3B, 0x14, 0x00,
	0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x88, 0x13, 0xe8, 0x03, 0x01, 0x00, 0x00, 0x00,
	};

	int len = 24;

	bsp_can_l2_frame_transmit(&(gTaskCanitfleoContext.can1), CommandReq, len, 0xFFFF);
}

void TestStartReq(uint8_t wmc_id)
{
	static uint8_t StartReq[] = {0xfe, 0xf6, 0x08, 0x00, 0x12, 0x3B, 0x04, 0x00};

	int len = 8;

	bsp_can_l2_frame_transmit(&(gTaskCanitfleoContext.can1), StartReq, len, (0x00100000 | (1 << wmc_id)));
}

/*
 * MESAGE ID MAPPING IN AWS/WMC
 */
//	HUITP_MSGID_sui_bfsc_startup_ind                 = 0x3B90,
//	//配置过程
//	HUITP_MSGID_sui_bfsc_set_config_req              = 0x3B11,
//	HUITP_MSGID_sui_bfsc_set_config_resp             = 0x3B91,
//	//启动过程
//	HUITP_MSGID_sui_bfsc_start_req                   = 0x3B12,
//	HUITP_MSGID_sui_bfsc_start_resp                  = 0x3B92,
//	HUITP_MSGID_sui_bfsc_stop_req                    = 0x3B13,
//	HUITP_MSGID_sui_bfsc_stop_resp                   = 0x3B93,
//	//重量汇报过程
//	HUITP_MSGID_sui_bfsc_new_ws_event                = 0x3B94,
//	HUITP_MSGID_sui_bfsc_repeat_ws_event             = 0x3B94,
//	//组合出料过程
//	HUITP_MSGID_sui_bfsc_ws_comb_out_req             = 0x3B15,
//	HUITP_MSGID_sui_bfsc_ws_comb_out_resp            = 0x3B95,
//	//抛料过程
//	HUITP_MSGID_sui_bfsc_ws_give_up_req              = 0x3B16,
//	HUITP_MSGID_sui_bfsc_ws_give_up_resp             = 0x3B96,
//	//特殊命令过程（测试等过程）
//	HUITP_MSGID_sui_bfsc_command_req                 = 0x3B17,
//	HUITP_MSGID_sui_bfsc_command_resp                = 0x3B97,
//	//差错过程
//	HUITP_MSGID_sui_bfsc_fault_ind                   = 0x3B98,
//	HUITP_MSGID_sui_bfsc_err_inq_cmd_req             = 0x3B19,
//	HUITP_MSGID_sui_bfsc_err_inq_cmd_resp            = 0x3B99,

uint16_t HuitpMsgIdMapToInternalMsgId(uint16_t huitp_msgid)
{
	switch (huitp_msgid)
	{
		case HUITP_MSGID_sui_bfsc_startup_ind:
			return MSG_ID_L3BFSC_WMC_STARTUP_IND;

		case HUITP_MSGID_sui_bfsc_set_config_req:
			return MSG_ID_L3BFSC_WMC_SET_CONFIG_REQ;
		case HUITP_MSGID_sui_bfsc_set_config_resp:
		   return MSG_ID_L3BFSC_WMC_SET_CONFIG_RESP;

		case HUITP_MSGID_sui_bfsc_start_req:
			return MSG_ID_L3BFSC_WMC_START_RESP;
		case HUITP_MSGID_sui_bfsc_start_resp:
			return MSG_ID_L3BFSC_WMC_START_RESP;

		case HUITP_MSGID_sui_bfsc_stop_req:
			return MSG_ID_L3BFSC_WMC_STOP_RESP;
		case HUITP_MSGID_sui_bfsc_stop_resp:
			return MSG_ID_L3BFSC_WMC_STOP_RESP;

		case HUITP_MSGID_sui_bfsc_new_ws_event:
			return MSG_ID_L3BFSC_WMC_NEW_WS_EVENT;

		case HUITP_MSGID_sui_bfsc_ws_comb_out_req:
			return MSG_ID_L3BFSC_WMC_WS_COMB_OUT_REQ;
		case HUITP_MSGID_sui_bfsc_ws_comb_out_resp:
			return MSG_ID_L3BFSC_WMC_WS_COMB_OUT_RESP;

		case HUITP_MSGID_sui_bfsc_command_req:
			return MSG_ID_L3BFSC_WMC_COMMAND_REQ;
		case HUITP_MSGID_sui_bfsc_command_resp:
			return MSG_ID_L3BFSC_WMC_COMMAND_RESP;

		case HUITP_MSGID_sui_bfsc_fault_ind:
			return MSG_ID_L3BFSC_WMC_FAULT_IND;

		case HUITP_MSGID_sui_bfsc_err_inq_cmd_req:
			return MSG_ID_L3BFSC_WMC_ERR_INQ_CMD_RESP;
		case HUITP_MSGID_sui_bfsc_err_inq_cmd_resp:
			return MSG_ID_L3BFSC_WMC_ERR_INQ_CMD_RESP;

		default:
			return 0xFFFF;
	}
}

uint16_t InternalMsgIdMapToHuitpMsgId(uint16_t internal_msgid)
{
	switch (internal_msgid)
	{
		case MSG_ID_L3BFSC_WMC_STARTUP_IND:
			return HUITP_MSGID_sui_bfsc_startup_ind;

		case MSG_ID_L3BFSC_WMC_SET_CONFIG_REQ:
		   return HUITP_MSGID_sui_bfsc_set_config_req;
		case MSG_ID_L3BFSC_WMC_SET_CONFIG_RESP:
		   return HUITP_MSGID_sui_bfsc_set_config_resp;

		case MSG_ID_L3BFSC_WMC_START_REQ:
			return HUITP_MSGID_sui_bfsc_start_req;
		case MSG_ID_L3BFSC_WMC_START_RESP:
			return HUITP_MSGID_sui_bfsc_start_resp;

		case MSG_ID_L3BFSC_WMC_STOP_REQ:
			return HUITP_MSGID_sui_bfsc_stop_req;
		case MSG_ID_L3BFSC_WMC_STOP_RESP:
			return HUITP_MSGID_sui_bfsc_stop_resp;

		case MSG_ID_L3BFSC_WMC_NEW_WS_EVENT:
			return HUITP_MSGID_sui_bfsc_new_ws_event;

		case MSG_ID_L3BFSC_WMC_WS_COMB_OUT_REQ:
			return HUITP_MSGID_sui_bfsc_ws_comb_out_req;
		case MSG_ID_L3BFSC_WMC_WS_COMB_OUT_RESP:
			return HUITP_MSGID_sui_bfsc_ws_comb_out_resp;

		case MSG_ID_L3BFSC_WMC_COMMAND_REQ:
			return HUITP_MSGID_sui_bfsc_command_req;
		case MSG_ID_L3BFSC_WMC_COMMAND_RESP:
			return HUITP_MSGID_sui_bfsc_command_resp;

		case MSG_ID_L3BFSC_WMC_FAULT_IND:
			return HUITP_MSGID_sui_bfsc_fault_ind;

		case MSG_ID_L3BFSC_WMC_ERR_INQ_CMD_REQ:
			return HUITP_MSGID_sui_bfsc_err_inq_cmd_req;
		case MSG_ID_L3BFSC_WMC_ERR_INQ_CMD_RESP:
			return HUITP_MSGID_sui_bfsc_err_inq_cmd_resp;

		default:
			return 0xFFFF;
	}
}

OPSTAT BfscMessageLengthCheck(uint16_t msgid, uint16_t length)
{
	HcuDebugPrint("CANITFLEO: BfscMessageLengthCheck, msgid=%d, length=%d\r\n", msgid, length);

	switch (msgid)
	{
		case MSG_ID_L3BFSC_WMC_STARTUP_IND:
			if (MSG_SIZE_L3BFSC_WMC_STARTUP_IND != length)
			{
				HcuErrorPrint("BfscMessageLengthCheck: MSG_SIZE_L3BFSC_WMC_STARTUP_IND != length\r\n");
				return FAILURE;
			}
			break;
		case MSG_ID_L3BFSC_WMC_SET_CONFIG_RESP:
			if (MSG_SIZE_L3BFSC_WMC_SET_CONFIG_RESP != length)
			{
				HcuErrorPrint("BfscMessageLengthCheck: MSG_SIZE_L3BFSC_WMC_SET_CONFIG_RESP(%d) != length\r\n", MSG_SIZE_L3BFSC_WMC_SET_CONFIG_RESP);
				return FAILURE;
			}
			break;
		case MSG_ID_L3BFSC_WMC_START_RESP:
			if (MSG_SIZE_L3BFSC_WMC_START_RESP != length)
			{
				HcuErrorPrint("BfscMessageLengthCheck: MSG_SIZE_L3BFSC_WMC_START_RESP != length\r\n");
				return FAILURE;
			}
			break;
		case MSG_ID_L3BFSC_WMC_STOP_RESP:
			if (MSG_SIZE_L3BFSC_WMC_STOP_RESP != length)
			{
				HcuErrorPrint("BfscMessageLengthCheck: MSG_SIZE_L3BFSC_WMC_STOP_RESP != length\r\n");
				return FAILURE;
			}
			break;
		case MSG_ID_L3BFSC_WMC_NEW_WS_EVENT:
			if (MSG_SIZE_L3BFSC_WMC_NEW_WS_EVENT != length)
			{
				HcuErrorPrint("BfscMessageLengthCheck: MSG_SIZE_L3BFSC_WMC_NEW_WS_EVENT != length\r\n");
				return FAILURE;
			}
			break;
		case MSG_ID_L3BFSC_WMC_WS_COMB_OUT_RESP:
			if (MSG_SIZE_L3BFSC_WMC_WS_COMB_OUT_RESP != length)
			{
				HcuErrorPrint("BfscMessageLengthCheck: MSG_SIZE_L3BFSC_WMC_WS_COMB_OUT_RESP != length\r\n");
				return FAILURE;
			}
			break;
		case MSG_ID_L3BFSC_WMC_COMMAND_RESP:
			if (MSG_SIZE_L3BFSC_WMC_COMMAND_RESP != length)
			{
				HcuErrorPrint("BfscMessageLengthCheck: MSG_SIZE_L3BFSC_WMC_COMMAND_RESP != length\r\n");
				return FAILURE;
			}
			break;
		case MSG_ID_L3BFSC_WMC_FAULT_IND:
			if (MSG_SIZE_L3BFSC_WMC_FAULT_IND != length)
			{
				HcuErrorPrint("BfscMessageLengthCheck: MSG_SIZE_L3BFSC_WMC_FAULT_IND != length\r\n");
				return FAILURE;
			}
			break;
		case MSG_ID_L3BFSC_WMC_ERR_INQ_CMD_RESP:
			if (MSG_SIZE_L3BFSC_WMC_ERR_INQ_CMD_RESP != length)
			{
				HcuErrorPrint("BfscMessageLengthCheck: MSG_SIZE_L3BFSC_WMC_ERR_INQ_CMD_RESP != length\r\n");
				return FAILURE;
			}
			break;
		default:
			return SUCCESS;
	}

	return SUCCESS;
}

/*
 * Message Processing function for START_IND
 */
void canitfleo_message_process_bfsc_wmc_start_ind(uint8_t *ptr)
{
	msg_struct_l3bfsc_wmc_startup_ind_t snd;

	memcpy((uint8_t *)&snd, ptr, sizeof(msg_struct_l3bfsc_wmc_startup_ind_t));
	uint32_t wmc_id_received = snd.wmc_inventory.wmc_id.wmc_id;

	/* TO DO: TO SEND TO BFSC TASK */
	//if (hcu_message_send(MSG_ID_L3BFSC_STARTUP_IND, TASK_ID_L3BFSC, TASK_ID_CANITFLEO, &snd, sizeof(msg_struct_l3bfsc_startup_ind_t)) == FAILURE)
	//	HcuErrorPrint("CANITFLEO: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName, zHcuVmCtrTab.task[TASK_ID_L3BFSC].taskName);

#define HCU_L3BFSC_SENSOR_WS_STATUS_ONLINE 1
	dbi_HcuBfsc_WmcStatusUpdate(0, wmc_id_received + 1, HCU_L3BFSC_SENSOR_WS_STATUS_ONLINE, -1);
	HcuDebugPrint("CANITFLEO: dbi_HcuBfsc_WmcStatusUpdate, wmc_id=%d, wmc_weight_value=%d\r\n", wmc_id_received, -1);

	/* THIS IS FOR TEST ONLY, NEED TO MOVE TO BFSC */
	HcuDebugPrint("CANITFLEO: canitfleo_can_l2frame_receive_process_bfsc_start_ind (%d),  Send SetConfigReq to WMC\r\n", wmc_id_received);
	TestSetConfigReq(wmc_id_received);
}

/*
 * Message Processing function for SET_CONFIG_RESP
 */
void canitfleo_message_process_bfsc_wmc_set_config_resp(uint8_t *ptr)
{
	msg_struct_l3bfsc_wmc_resp_t snd;

	memcpy((uint8_t *)&snd, ptr, sizeof(msg_struct_l3bfsc_wmc_resp_t));
	uint32_t wmc_id_received = snd.wmc_id.wmc_id;

	/* THIS IS WHERE NEED TO SEND INTERNAL MESSAGE TO BFSC, TO DO */
	//if (hcu_message_send(MSG_ID_L3BFSC_STARTUP_IND, TASK_ID_L3BFSC, TASK_ID_CANITFLEO, &snd, sizeof(msg_struct_l3bfsc_startup_ind_t)) == FAILURE)
	//	HcuErrorPrint("CANITFLEO: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName, zHcuVmCtrTab.task[TASK_ID_L3BFSC].taskName);

	/* Send for Test Only, NEED TO MOVE THE PROCESSING TO BFSC TASK, VIA MESSAGE */
	HcuDebugPrint("CANITFLEO: canitfleo_can_l2frame_receive_process_bfsc_set_config_resp(%)\r\n", wmc_id_received);
	TestStartReq(wmc_id_received);
}

/*
 * Message Processing function for NEW_WS_EVENT
 */
static long count_ws_event[16] = {0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0};

void canitfleo_message_process_bfsc_wmc_new_ws_event(uint8_t *ptr)
{

	msg_struct_l3bfsc_wmc_ws_event_t snd;
	memcpy((uint8_t *)&snd, ptr, sizeof(msg_struct_l3bfsc_wmc_ws_event_t));

	uint32_t weight_received = snd.weight_ind.average_weight;
	uint32_t wmc_id_received = snd.wmc_id.wmc_id;
	count_ws_event[wmc_id_received]++;

	/* THIS IS ONLY FOR TEST -> NEED TO MOVE TO BFSC AS BELOW */
	dbi_HcuBfsc_WmcStatusUpdate(0, wmc_id_received + 1, -1, weight_received);
	HcuDebugPrint("CANITFLEO: dbi_HcuBfsc_WmcStatusUpdate, wmc_id=%d, wmc_weight_value=%d\r\n", wmc_id_received, weight_received);

	HcuDebugPrint("CANITFLEO: WS EVENT: [%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d]\r\n",
			count_ws_event[0], count_ws_event[1], count_ws_event[2], count_ws_event[3],
			count_ws_event[4], count_ws_event[5], count_ws_event[6], count_ws_event[7],
			count_ws_event[8], count_ws_event[9], count_ws_event[10], count_ws_event[11],
			count_ws_event[12], count_ws_event[13], count_ws_event[14], count_ws_event[15]);

	/* TO BE UN-COMMENT IF BFSC STATE MACHINE IS OK */
	//if (hcu_message_send(MSG_ID_L3BFSC_NEW_WS_EVENT, TASK_ID_L3BFSC, TASK_ID_CANITFLEO, &snd, sizeof(msg_struct_l3bfsc_startup_ind_t)) == FAILURE)
	//	HcuErrorPrint("CANITFLEO: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_CANITFLEO].taskName, zHcuVmCtrTab.task[TASK_ID_L3BFSC].taskName);
}

OPSTAT canitfleo_can_l2frame_receive_process(uint8_t *p_l2_frame, uint32_t l2_frame_len)
{

	IHU_HUITP_L2FRAME_STD_frame_header_t *p = (IHU_HUITP_L2FRAME_STD_frame_header_t  *)p_l2_frame;
	StrMsg_HUITP_MSGID_sui_bfsc_wmc_msg_header_t *pBfscMsg = (StrMsg_HUITP_MSGID_sui_bfsc_wmc_msg_header_t *)(p + 1);

	if (NULL == p_l2_frame)
	{
		HcuErrorPrint("CANITFLEO: canitfleo_can_l2frame_receive_process, NULL == p_l2_frame\r\n");
		return FAILURE;
	}

	if (l2_frame_len > BFSC_CAN_MAX_RX_BUF_SIZE)
	{
		HcuErrorPrint("CANITFLEO: canitfleo_can_l2frame_receive_process, l2_frame_len(%d) > BFSC_CAN_MAX_RX_BUF_SIZE(256)\r\n", l2_frame_len);
		return FAILURE;
	}

	uint8_t start = p->start;
	uint8_t chksum = p->chksum;
	uint16_t len = p->len;

	/* Header Checksum */
	uint8_t calc_chksum = l2packet_gen_chksum(p);
	HcuDebugPrint("CANITFLEO: canitfleo_can_l2frame_receive_process: l2frame: start=0x%02X, chksum=0x%02X, calc_chksum=0x%02X, len=%d\r\n", start, chksum, calc_chksum, len);

	if(chksum != calc_chksum)
	{
		HcuErrorPrint("CANITFLEO: canitfleo_can_l2frame_receive_process, L2 CAN Frame checksum NOK\r\n");
		return FAILURE;
	}

	uint16_t msgid_huitp =  pBfscMsg->msgid;
	uint16_t msgid = HuitpMsgIdMapToInternalMsgId(msgid_huitp);
	uint16_t length =  pBfscMsg->length;
	HcuDebugPrint("CANITFLEO: canitfleo_can_l2frame_receive_process: l3bfsc: msgid_huitp=0x%04X, msgid=0x%02X, length=%d\r\n", msgid_huitp, msgid, length);

	if(FAILURE == BfscMessageLengthCheck(msgid, length))
	{
		HcuErrorPrint("CANITFLEO: canitfleo_can_l2frame_receive_process, BfscMessageLengthCheck failure\r\n");
		return FAILURE;
	}

	switch (msgid)
	{
		case MSG_ID_L3BFSC_WMC_STARTUP_IND:
			if (MSG_SIZE_L3BFSC_WMC_STARTUP_IND == length)
			{
				HcuDebugPrint("CANITFLEO: call canitfleo_can_l2frame_receive_process_bfsc_start_ind()\r\n");
				canitfleo_message_process_bfsc_wmc_start_ind((uint8_t *)pBfscMsg);
			}
			break;
		case MSG_ID_L3BFSC_WMC_SET_CONFIG_RESP:
			if (MSG_SIZE_L3BFSC_WMC_SET_CONFIG_RESP == length)
			{
				HcuDebugPrint("CANITFLEO: call canitfleo_can_l2frame_receive_process_bfsc_set_config_resp()\r\n");
				canitfleo_message_process_bfsc_wmc_set_config_resp((uint8_t *)pBfscMsg);
			}
			break;
		case MSG_ID_L3BFSC_WMC_START_RESP:
			if (MSG_SIZE_L3BFSC_WMC_START_RESP != length)
			{
				HcuErrorPrint("CANITFLEO: BfscMessageLengthCheck: MSG_SIZE_L3BFSC_START_RESP != length\r\n");
				return FAILURE;
			}
			break;
		case MSG_ID_L3BFSC_WMC_STOP_RESP:
			if (MSG_SIZE_L3BFSC_WMC_STOP_RESP != length)
			{
				HcuErrorPrint("CANITFLEO: BfscMessageLengthCheck: MSG_SIZE_L3BFSC_STOP_RESP != length\r\n");
				return FAILURE;
			}
			break;
		case MSG_ID_L3BFSC_WMC_NEW_WS_EVENT:
			if (MSG_SIZE_L3BFSC_WMC_NEW_WS_EVENT == length)
			{
				HcuDebugPrint("CANITFLEO: call canitfleo_can_l2frame_receive_process_bfsc_new_ws_event()\r\n");
				canitfleo_message_process_bfsc_wmc_new_ws_event((uint8_t *)pBfscMsg);
			}
			break;
		case MSG_ID_L3BFSC_WMC_WS_COMB_OUT_RESP:
			if (MSG_SIZE_L3BFSC_WMC_WS_COMB_OUT_RESP != length)
			{
				HcuErrorPrint("CANITFLEO: BfscMessageLengthCheck: MSG_SIZE_L3BFSC_WS_COMB_OUT_RESP != length\r\n");
				return FAILURE;
			}
			break;
		case MSG_ID_L3BFSC_WMC_COMMAND_RESP:
			if (MSG_SIZE_L3BFSC_WMC_COMMAND_RESP != length)
			{
				HcuErrorPrint("CANITFLEO: BfscMessageLengthCheck: MSG_SIZE_L3BFSC_COMMAND_RESP != length\r\n");
				return FAILURE;
			}
			break;
		case MSG_ID_L3BFSC_WMC_FAULT_IND:
			if (MSG_SIZE_L3BFSC_WMC_FAULT_IND != length)
			{
				HcuErrorPrint("CANITFLEO: BfscMessageLengthCheck: MSG_SIZE_L3BFSC_FAULT_IND != length\r\n");
				return FAILURE;
			}
			break;
		case MSG_ID_L3BFSC_WMC_ERR_INQ_CMD_RESP:
			if (MSG_SIZE_L3BFSC_WMC_ERR_INQ_CMD_RESP != length)
			{
				HcuErrorPrint("CANITFLEO: BfscMessageLengthCheck: MSG_SIZE_L3BFSC_ERR_INQ_CMD_RESP != length\r\n");
				return FAILURE;
			}
			break;
		default:
			return SUCCESS;
	}
	return SUCCESS;
}

/*
 * CANITFLEO STATE MACHINE PROCESS FUNCTION, MAIN ENTRY FOR MESAGE RECEIVED
 */
OPSTAT fsm_canitfleo_can_l2frame_receive(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	/* PAY ATTENTION THAT the CAN Thread use can_l2frame_itf_t to transfer form/to with CANITFLEO task */
	can_l2frame_itf_t *p = (can_l2frame_itf_t *)param_ptr;
	uint8_t *p_l2_frame = p->can_l2frame;
	uint32_t l2_frame_len = p->can_l2frame_len;

	HcuDebugPrint("CANITFLEO: fsm_canitfleo_can_l2frame_receive, start to process message\r\n");

	/* process the L2 frame message */
	canitfleo_can_l2frame_receive_process(p_l2_frame, l2_frame_len);
}

#endif

