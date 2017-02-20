/*
 * l3aqycg20.c
 *
 *  Created on: 2015年11月22日
 *      Author: test
 */


#include "l3aqycg20.h"

#include "../l0service/timer.h"
#include "../l0service/trace.h"
#include "../l1com/hwinv.h"
#include "../l1com/l1comdef.h"
#include "../l2frame/cloudvela.h"

/*
** FSM of the L3AQYCG20
*/
HcuFsmStateItem_t HcuFsmL3aqycg20[] =
{
    //MessageId                 //State                   		 		//Function
	//启始点，固定定义，不要改动, 使用ENTRY/END，意味者MSGID肯定不可能在某个高位区段中；考虑到所有任务共享MsgId，即使分段，也无法实现
	//完全是为了给任务一个初始化的机会，按照状态转移机制，该函数不具备启动的机会，因为任务初始化后自动到FSM_STATE_IDLE
	//如果没有必要进行初始化，可以设置为NULL
	{MSG_ID_ENTRY,       						FSM_STATE_ENTRY,            			fsm_l3aqycg20_task_entry}, //Starting

	//System level initialization, only controlled by HCU-MAIN
    {MSG_ID_COM_INIT,       					FSM_STATE_IDLE,            				fsm_l3aqycg20_init},
    {MSG_ID_COM_INIT_FEEDBACK,					FSM_STATE_IDLE,            				fsm_com_do_nothing},

	//Task level initialization
    {MSG_ID_COM_INIT,       					FSM_STATE_L3AQYCG20_INITED,            		fsm_l3aqycg20_init},
    {MSG_ID_COM_INIT_FEEDBACK,					FSM_STATE_L3AQYCG20_INITED,            		fsm_com_do_nothing},

	//ANY state entry
    {MSG_ID_COM_INIT_FEEDBACK,					FSM_STATE_COMMON,          					fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT,       				FSM_STATE_COMMON,          					fsm_com_heart_beat_rcv},
	{MSG_ID_COM_STOP,       					FSM_STATE_COMMON,          					fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT_FB,       			FSM_STATE_COMMON,          					fsm_com_do_nothing},
    {MSG_ID_COM_RESTART,						FSM_STATE_COMMON,            				fsm_l3aqycg20_restart},
	{MSG_ID_COM_TIME_OUT,       				FSM_STATE_COMMON,          					fsm_l3aqycg20_time_out},

    //Normal working status
	//YCJK HUITP L2：发自底层来的请求
	{MSG_ID_CLOUDVELA_YCJK_DATA_REQ,    	FSM_STATE_L3AQYCG20_ACTIVED,      			fsm_l3aqycg20_cloudvela_data_req},
	{MSG_ID_CLOUDVELA_YCJK_DATA_CONFIRM,   	FSM_STATE_L3AQYCG20_ACTIVED,      			fsm_l3aqycg20_cloudvela_data_confirm},
	{MSG_ID_CLOUDVELA_YCJK_CTRL_REQ,    	FSM_STATE_L3AQYCG20_ACTIVED,          		fsm_l3aqycg20_cloudvela_ctrl_req},
	//ZHBHJT212 L2：底层命令
	{MSG_ID_LLCZHB_L3MOD_CTRL_REQ,    		FSM_STATE_L3AQYCG20_ACTIVED,          		fsm_l3aqycg20_llczhb_ctrl_req},

	//等待上层模块的控制反馈
	//YCJK HUITP L3：各个L3模块收集数据
	{MSG_ID_L3AQYC_EXG_CTRL_RESP,    		FSM_STATE_L3AQYCG20_WFFB_CTRL,          	fsm_l3aqycg20_huil3mod_exg_ctrl_resp},
	//ZHBHJT212 L3：从L3模块收集数据
	{MSG_ID_ZHBL3MOD_EXG_CTRL_RESP,    		FSM_STATE_L3AQYCG20_WFFB_CTRL,          	fsm_l3aqycg20_zhbl3mod_exg_ctrl_resp},

	//等待上层模块的控制反馈
	//YCJK HUITP L3：各个L3模块收集数据
	{MSG_ID_L3AQYC_EXG_DATA_REPORT,    		FSM_STATE_L3AQYCG20_WFFB_DATA,          	fsm_l3aqycg20_huil3mod_exg_data_report},
	//ZHBHJT212 L3：从L3模块收集数据
	{MSG_ID_ZHBL3MOD_EXG_DATA_REPORT,    	FSM_STATE_L3AQYCG20_WFFB_DATA,          	fsm_l3aqycg20_zhbl3mod_exg_data_report},

    //结束点，固定定义，不要改动
    {MSG_ID_END,            	FSM_STATE_END,             				NULL},  //Ending
};

//Global variables


//Task Global variables
gTaskL3aqycq20Context_t gTaskL3aqycq20Context;



//Main Entry
//Input parameter would be useless, but just for similar structure purpose
OPSTAT fsm_l3aqycg20_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//除了对全局变量进行操作之外，尽量不要做其它操作，因为该函数将被主任务/线程调用，不是本任务/线程调用
	//该API就是给本任务一个提早介入的入口，可以帮着做些测试性操作
	if (FsmSetState(TASK_ID_L3AQYCG20, FSM_STATE_IDLE) == FAILURE){
		HcuErrorPrint("L3AQYCG20: Error Set FSM State at fsm_l3aqycg20_task_entry\n");}
	return SUCCESS;
}

OPSTAT fsm_l3aqycg20_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;

	if ((src_id > TASK_ID_MIN) &&(src_id < TASK_ID_MAX)){
		//Send back MSG_ID_COM_INIT_FEEDBACK to SVRCON
		msg_struct_com_init_feedback_t snd0;
		memset(&snd0, 0, sizeof(msg_struct_com_init_feedback_t));
		snd0.length = sizeof(msg_struct_com_init_feedback_t);

		//to avoid all task send out the init fb msg at the same time which lead to msgque get stuck
		hcu_usleep(dest_id*HCU_SYSCFG_DURATION_OF_INIT_FB_WAIT_MAX);

		ret = hcu_message_send(MSG_ID_COM_INIT_FEEDBACK, src_id, TASK_ID_L3AQYCG20, &snd0, snd0.length);
		if (ret == FAILURE){
			HcuErrorPrint("L3AQYCG20: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3AQYCG20].taskName, zHcuVmCtrTab.task[src_id].taskName);
			return FAILURE;
		}
	}

	//收到初始化消息后，进入初始化状态
	if (FsmSetState(TASK_ID_L3AQYCG20, FSM_STATE_L3AQYCG20_INITED) == FAILURE){
		HcuErrorPrint("L3AQYCG20: Error Set FSM State!\n");
		return FAILURE;
	}

	//初始化硬件接口
	if (func_l3aqycg20_int_init() == FAILURE){
		HcuErrorPrint("L3AQYCG20: Error initialize interface!\n");
		return FAILURE;
	}

	//Global Variables
	zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20] = 0;

	//启动周期性定时器
	ret = hcu_timer_start(TASK_ID_L3AQYCG20, TIMER_ID_1S_L3AQYCG20_PERIOD_READ, \
			zHcuSysEngPar.timer.array[TIMER_ID_1S_L3AQYCG20_PERIOD_READ].dur, TIMER_TYPE_PERIOD, TIMER_RESOLUTION_1S);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		HcuErrorPrint("L3AQYCG20: Error start period timer!\n");
		return FAILURE;
	}

	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_L3AQYCG20, FSM_STATE_L3AQYCG20_ACTIVED) == FAILURE)
		HCU_ERROR_PRINT_L3AQYCG20("L3AQYCG20: Error Set FSM State!\n");
	HCU_DEBUG_PRINT_FAT("L3AQYCG20: Enter FSM_STATE_L3AQYCG20_ACTIVED status, Keeping refresh here!\n");

	//返回
	return SUCCESS;
}

OPSTAT fsm_l3aqycg20_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HcuErrorPrint("L3AQYCG20: Internal error counter reach DEAD level, SW-RESTART soon!\n");
	zHcuSysStaPm.statisCnt.restartCnt++;
	fsm_l3aqycg20_init(0, 0, NULL, 0);
	return SUCCESS;
}

OPSTAT func_l3aqycg20_int_init(void)
{
	return SUCCESS;
}

//暂时啥都不干，定时到达后，还不明确是否需要支持定时工作
OPSTAT fsm_l3aqycg20_time_out(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret;

	//Receive message and copy to local variable
	msg_struct_com_time_out_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_com_time_out_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_com_time_out_t))){
		HcuErrorPrint("L3AQYCG20: Receive message error!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//钩子在此处，检查zHcuRunErrCnt[TASK_ID_L3AQYCG20]是否超限
	if (zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20] > HCU_RUN_ERROR_LEVEL_2_MAJOR){
		//减少重复RESTART的概率
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20] = zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20] - HCU_RUN_ERROR_LEVEL_2_MAJOR;
		msg_struct_com_restart_t snd0;
		memset(&snd0, 0, sizeof(msg_struct_com_restart_t));
		snd0.length = sizeof(msg_struct_com_restart_t);
		ret = hcu_message_send(MSG_ID_COM_RESTART, TASK_ID_L3AQYCG20, TASK_ID_L3AQYCG20, &snd0, snd0.length);
		if (ret == FAILURE){
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
			HcuErrorPrint("L3AQYCG20: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_L3AQYCG20].taskName, zHcuVmCtrTab.task[TASK_ID_L3AQYCG20].taskName);
			return FAILURE;
		}
	}

	//Period time out received
	if ((rcv.timeId == TIMER_ID_1S_L3AQYCG20_PERIOD_READ) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		//保护周期读数的优先级，强制抢占状态，并简化问题
		if (FsmGetState(TASK_ID_L3AQYCG20) != FSM_STATE_L3AQYCG20_ACTIVED){
			ret = FsmSetState(TASK_ID_L3AQYCG20, FSM_STATE_L3AQYCG20_ACTIVED);
			if (ret == FAILURE){
				zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
				HcuErrorPrint("L3AQYCG20: Error Set FSM State!\n");
				return FAILURE;
			}//FsmSetState
		}

		//Do nothing

	}

	return SUCCESS;
}

//这个模块收到这个命令后，需要跟更多的模块进行交互
OPSTAT fsm_l3aqycg20_cloudvela_data_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret=0;
	msg_struct_cloudvela_ycjk_data_req_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_cloudvela_ycjk_data_req_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_cloudvela_ycjk_data_req_t))){
		HcuErrorPrint("NOISE: Receive message error!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_L3AQYCG20]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);



	return SUCCESS;
}

OPSTAT fsm_l3aqycg20_cloudvela_ctrl_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	return SUCCESS;
}

OPSTAT fsm_l3aqycg20_cloudvela_data_confirm(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	return SUCCESS;
}

OPSTAT fsm_l3aqycg20_huil3mod_exg_ctrl_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	return SUCCESS;
}

OPSTAT fsm_l3aqycg20_huil3mod_exg_data_report(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	return SUCCESS;
}

OPSTAT fsm_l3aqycg20_llczhb_ctrl_req(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret=0;
	msg_struct_llczhb_l3mod_ctrl_req_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_llczhb_l3mod_ctrl_req_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_llczhb_l3mod_ctrl_req_t)))
		HCU_ERROR_PRINT_L3AQYCG20("AQYC: Receive message error!\n");
	memcpy(&rcv, param_ptr, param_len);

	switch (rcv.actionId){
	case HCU_SYSMSG_ZHBHJT_ACTION_EXECUTE_FINISH_9012:
	break;

	case HCU_SYSMSG_ZHBHJT_ACTION_DATA_CFM_9014:
		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_SET_PSWD_1072:
		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_GET_FIELD_TIME_1011:
		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_SET_FIELD_TIME_1012:
		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_GET_POL_RTD_2011:
		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_STOP_RTD_2012:
		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_GET_EQU_RUN_2021:
		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_STOP_EQU_RUN_2022:
		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_GET_POL_MIN_RPT_2051:
		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_GET_POL_DAY_RPT_2031:
		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_GET_POL_RT_2041:
		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_GET_POL_ALA_2071:
		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_SND_ALM_EVT_2072:
		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_SET_ALMLIM_1022:
		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_GET_ALMLIM_1021:
		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_SET_MNADDR_1032:
		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_GET_MNADDR_1031:
		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_SET_RPT_TIME_1042:
		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_GET_RPT_TIME_1041:
		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_SET_ZERO_3011:
		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_SET_RTDI_1062:
		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_GET_RTDI_1061:
		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_SET_OT_RC_1000:
		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_SET_WARN_TIME_1001:
		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_SET_INST_SAMPLE_3012:
		break;

	case HCU_SYSMSG_ZHBHJT_ACTION_SET_SAMPLE_CTIME_3014:
		break;

	default:
		HCU_ERROR_PRINT_L3AQYCG20("L3AQYCG20: Error action received!\n");
		break;
	}

	//设置状态机到目标状态
	//根据情况，决定是希望等待控制命令的反馈，还是数据汇报的反馈。如果不需要对方反馈，也可以保持在ACTIVE状态
	//设置多状态，就是为了更好的控制数据与控制命令的反馈。目前各种控制命令都聚合在一起，必须通过状态机将情况很清楚，不然很容易漏掉某些组合情况。
	if (FsmSetState(TASK_ID_L3AQYCG20, FSM_STATE_L3AQYCG20_WFFB_CTRL) == FAILURE)
		HCU_ERROR_PRINT_L3AQYCG20("L3AQYCG20: Error Set FSM State!\n");

	return SUCCESS;
}

OPSTAT fsm_l3aqycg20_zhbl3mod_exg_ctrl_resp(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//根据不同的命令，判定是否需要再次接受到反馈，从而决定状态是直接转移到ACTIVE，还是继续留在WFFB
	//根据发送控制命令的多寡，可能需要判定是否接收齐活了所有传感器的反馈
	//注意启动和停止定时器

	//收集齐活了，发送MSG_ID_L3MOD_LLCZHB_CTRL_RESP给LLCZHB模块
	//因为是内部消息，所有内容一次性全部发给LLCZHB模块，让其决定是否分段。L3MOD不考虑数据分段问题，只控制并管理所有传感器的数据完整性

	//状态转移


	return SUCCESS;
}

OPSTAT fsm_l3aqycg20_zhbl3mod_exg_data_report(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//根据不同的命令，判定是否需要再次接受到反馈，从而决定状态是直接转移到ACTIVE，还是继续留在WFFB
	//根据发送控制命令的多寡，可能需要判定是否接收齐活了所有传感器的反馈
	//注意启动和停止定时器

	//收集齐活了，发送MSG_ID_L3MOD_LLCZHB_DATA_REPORT给LLCZHB模块
	//因为是内部消息，所有内容一次性全部发给LLCZHB模块，让其决定是否分段。L3MOD不考虑数据分段问题，只控制并管理所有传感器的数据完整性

	//状态转移

	return SUCCESS;
}




