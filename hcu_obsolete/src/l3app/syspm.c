/*
 * syspm.c
 *
 *  Created on: 2016年5月10日
 *      Author: hitpony
 */

#include "syspm.h"

#include "../l0service/timer.h"
#include "../l0service/trace.h"
#include "../l2frame/cloudvela.h"

/*
** FSM of the SYSPM
*/
HcuFsmStateItem_t FsmSyspm[] =
{
    //MessageId                 //State                   		 		//Function
	//启始点，固定定义，不要改动, 使用ENTRY/END，意味者MSGID肯定不可能在某个高位区段中；考虑到所有任务共享MsgId，即使分段，也无法实现
	//完全是为了给任务一个初始化的机会，按照状态转移机制，该函数不具备启动的机会，因为任务初始化后自动到FSM_STATE_IDLE
	//如果没有必要进行初始化，可以设置为NULL
	{MSG_ID_ENTRY,       		FSM_STATE_ENTRY,            			fsm_syspm_task_entry}, //Starting

	//System level initialization, only controlled by HCU-MAIN
    {MSG_ID_COM_INIT,       	FSM_STATE_IDLE,            				fsm_syspm_init},
    {MSG_ID_COM_RESTART,		FSM_STATE_IDLE,            				fsm_syspm_restart},
    {MSG_ID_COM_INIT_FEEDBACK,	FSM_STATE_IDLE,            				fsm_com_do_nothing},

	//Task level initialization
    {MSG_ID_COM_INIT,       	FSM_STATE_SYSPM_INITIED,            	fsm_syspm_init},
    {MSG_ID_COM_RESTART,		FSM_STATE_SYSPM_INITIED,            	fsm_syspm_restart},
    {MSG_ID_COM_INIT_FEEDBACK,	FSM_STATE_SYSPM_INITIED,            	fsm_com_do_nothing},

    //Task level initialization
    {MSG_ID_COM_RESTART,        FSM_STATE_SYSPM_ACTIVED,            	fsm_syspm_restart},
    {MSG_ID_COM_INIT_FEEDBACK,	FSM_STATE_SYSPM_ACTIVED,            	fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT,     FSM_STATE_SYSPM_ACTIVED,       			fsm_com_heart_beat_rcv},
	{MSG_ID_COM_HEART_BEAT_FB,  FSM_STATE_SYSPM_ACTIVED,       			fsm_com_do_nothing},
	{MSG_ID_COM_TIME_OUT,       FSM_STATE_SYSPM_ACTIVED,          		fsm_syspm_time_out},

    //结束点，固定定义，不要改动
    {MSG_ID_END,            	FSM_STATE_END,             				NULL},  //Ending
};

//Global variables
extern HcuSysEngParTablet_t zHcuSysEngPar; //全局工程参数控制表
extern HcuGlobalCounter_t zHcuSysStaPm.statisCnt;

//Main Entry
//Input parameter would be useless, but just for similar structure purpose
OPSTAT fsm_syspm_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//除了对全局变量进行操作之外，尽量不要做其它操作，因为该函数将被主任务/线程调用，不是本任务/线程调用
	//该API就是给本任务一个提早介入的入口，可以帮着做些测试性操作
	if (FsmSetState(TASK_ID_SYSPM, FSM_STATE_IDLE) == FAILURE){
		HcuErrorPrint("SYSPM: Error Set FSM State at fsm_syspm_task_entry\n");}
	return SUCCESS;
}

OPSTAT fsm_syspm_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;

	if ((src_id > TASK_ID_MIN) &&(src_id < TASK_ID_MAX)){
		//Send back MSG_ID_COM_INIT_FEEDBACK to SVRCON
		msg_struct_com_init_feedback_t snd0;
		memset(&snd0, 0, sizeof(msg_struct_com_init_feedback_t));
		snd0.length = sizeof(msg_struct_com_init_feedback_t);

		//to avoid all task send out the init fb msg at the same time which lead to msgque get stuck
		hcu_usleep(dest_id*HCU_DURATION_OF_INIT_FB_WAIT_MAX);

		ret = hcu_message_send(MSG_ID_COM_INIT_FEEDBACK, src_id, TASK_ID_SYSPM, &snd0, snd0.length);
		if (ret == FAILURE){
			HcuErrorPrint("SYSPM: Send message error, TASK [%s] to TASK[%s]!\n", zHcuSysCrlTab.taskRun.taskName[TASK_ID_SYSPM], zHcuSysCrlTab.taskRun.taskName[src_id]);
			return FAILURE;
		}
	}

	//收到初始化消息后，进入初始化状态
	if (FsmSetState(TASK_ID_SYSPM, FSM_STATE_SYSPM_INITED) == FAILURE){
		HcuErrorPrint("SYSPM: Error Set FSM State!\n");
		return FAILURE;
	}

	//初始化硬件接口
	if (func_syspm_int_init() == FAILURE){
		HcuErrorPrint("SYSPM: Error initialize interface!\n");
		return FAILURE;
	}

	//Global Variables
	zHcuSysStaPm.taskRunErrCnt[TASK_ID_SYSPM] = 0;
	memset(&zHcuSysStaPm.statisCnt, 0, sizeof(HcuGlobalCounter_t));

	//启动周期性定时器
	ret = hcu_timer_start(TASK_ID_SYSPM, TIMER_ID_1S_SYSPM_PERIOD_WORKING, zHcuSysEngPar.timer.syspmWorkingTimer, TIMER_TYPE_PERIOD, TIMER_RESOLUTION_1S);
	if (ret == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_SYSPM]++;
		HcuErrorPrint("SYSPM: Error start period timer!\n");
		return FAILURE;
	}

	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_SYSPM, FSM_STATE_SYSPM_ACTIVED) == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_SYSPM]++;
		HcuErrorPrint("SYSPM: Error Set FSM State!\n");
		return FAILURE;
	}
	if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_FAT_ON) != FALSE){
		HcuDebugPrint("SYSPM: Enter FSM_STATE_SYSPM_ACTIVED status, Keeping refresh here!\n");
	}
	/*

	//进入阻塞式接收数据状态，然后继续发送
	while(1){
		//接收数据
		int dataLen=0;
		if (dataLen > 1){
			//发送数据给HSMMP
			msg_struct_syspm_hsmmp_data_rx_t snd;
			memset(&snd, 0, sizeof(msg_struct_syspm_hsmmp_data_rx_t));
			snd.length = sizeof(msg_struct_syspm_hsmmp_data_rx_t);
			ret = hcu_message_send(MSG_ID_SYSPM_HSMMP_DATA_RX, TASK_ID_HSMMP, TASK_ID_SYSPM, &snd, snd.length);
			if (ret == FAILURE){
				zHcuRunErrCnt[TASK_ID_SYSPM]++;
				HcuErrorPrint("SYSPM: Send message error, TASK [%s] to TASK[%s]!\n", zHcuTaskNameList[TASK_ID_SYSPM], zHcuTaskNameList[TASK_ID_HSMMP]);
				return FAILURE;
			}
		}

		hcu_sleep(5);
	}
	*/

	return SUCCESS;
}

OPSTAT fsm_syspm_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HcuErrorPrint("SYSPM: Internal error counter reach DEAD level, SW-RESTART soon!\n");
	zHcuSysStaPm.statisCnt.restartCnt++;
	fsm_syspm_init(0, 0, NULL, 0);
	return SUCCESS;
}

OPSTAT func_syspm_int_init(void)
{
	return SUCCESS;
}

OPSTAT fsm_syspm_time_out(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret;

	//Receive message and copy to local variable
	msg_struct_com_time_out_t rcv;
	memset(&rcv, 0, sizeof(msg_struct_com_time_out_t));
	if ((param_ptr == NULL || param_len > sizeof(msg_struct_com_time_out_t))){
		HcuErrorPrint("SYSPM: Receive message error!\n");
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_SYSPM]++;
		return FAILURE;
	}
	memcpy(&rcv, param_ptr, param_len);

	//钩子在此处，检查zHcuRunErrCnt[TASK_ID_SYSPM]是否超限
	if (zHcuSysStaPm.taskRunErrCnt[TASK_ID_SYSPM] > HCU_RUN_ERROR_LEVEL_2_MAJOR){
		//减少重复RESTART的概率
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_SYSPM] = zHcuSysStaPm.taskRunErrCnt[TASK_ID_SYSPM] - HCU_RUN_ERROR_LEVEL_2_MAJOR;
		msg_struct_com_restart_t snd0;
		memset(&snd0, 0, sizeof(msg_struct_com_restart_t));
		snd0.length = sizeof(msg_struct_com_restart_t);
		ret = hcu_message_send(MSG_ID_COM_RESTART, TASK_ID_SYSPM, TASK_ID_SYSPM, &snd0, snd0.length);
		if (ret == FAILURE){
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_SYSPM]++;
			HcuErrorPrint("SYSPM: Send message error, TASK [%s] to TASK[%s]!\n", zHcuSysCrlTab.taskRun.taskName[TASK_ID_SYSPM], zHcuSysCrlTab.taskRun.taskName[TASK_ID_SYSPM]);
			return FAILURE;
		}
	}

	//Period time out received
	if ((rcv.timeId == TIMER_ID_1S_SYSPM_PERIOD_WORKING) &&(rcv.timeRes == TIMER_RESOLUTION_1S)){
		//保护周期读数的优先级，强制抢占状态，并简化问题
		if (FsmGetState(TASK_ID_SYSPM) != FSM_STATE_SYSPM_ACTIVED){
			ret = FsmSetState(TASK_ID_SYSPM, FSM_STATE_SYSPM_ACTIVED);
			if (ret == FAILURE){
				zHcuSysStaPm.taskRunErrCnt[TASK_ID_SYSPM]++;
				HcuErrorPrint("SYSPM: Error Set FSM State!\n");
				return FAILURE;
			}//FsmSetState
		}
		//干活的这儿！
		//拷贝zHcuRunErrCnt到目标全局变量中
		memcpy(&zHcuSysStaPm.statisCnt.errCnt[0], &zHcuSysStaPm.taskRunErrCnt[0], (sizeof(UINT32)*MAX_TASK_NUM_IN_ONE_HCU));
		//检查COUNTER的情况，并生成相应的事件。这里暂时空着
		//存储事件到数据库中，形成报告
		if (dbi_HcuSyspmGlobalDataInfo_save() == FAILURE) zHcuSysStaPm.taskRunErrCnt[TASK_ID_SYSPM]++;

		//PM report to Cloud added by ZSC
		if (FsmGetState(TASK_ID_CLOUDVELA) == FSM_STATE_CLOUDVELA_ONLINE){
			msg_struct_com_pm_report_t snd;
			memset(&snd, 0, sizeof(msg_struct_com_pm_report_t));

			snd.length = sizeof(msg_struct_com_pm_report_t);
			snd.usercmdid = L3CI_performance_info;
			snd.timeStamp = time(0);
			snd.PmRestartCnt = zHcuSysStaPm.statisCnt.restartCnt;
			snd.PmCloudVelaDiscCnt = zHcuSysStaPm.statisCnt.cloudVelaDiscCnt;
			snd.PmSocketDiscCnt = zHcuSysStaPm.statisCnt.SocketDiscCnt;

			ret = hcu_message_send(MSG_ID_COM_PM_REPORT, TASK_ID_CLOUDVELA, TASK_ID_SYSPM, &snd, snd.length);
			if (ret == FAILURE){
				zHcuSysStaPm.taskRunErrCnt[TASK_ID_SYSPM]++;
				HcuErrorPrint("SYSPM: Send message error, TASK [%s] to TASK[%s]!\n", zHcuSysCrlTab.taskRun.taskName[TASK_ID_SYSPM], zHcuSysCrlTab.taskRun.taskName[TASK_ID_CLOUDVELA]);
				return FAILURE;
			}

		}

		//差错情形
		else{
			HcuErrorPrint("SYSPM: Wrong state of CLOUDVELA when data need send out!\n");
			zHcuSysStaPm.taskRunErrCnt[TASK_ID_SYSPM]++;
			return FAILURE;
		}


	}

	return SUCCESS;
}
