/*
 * jsoninotify.c
 *
 *  Created on: 2017年06月23日
 *      Author: LZH
 */

#include "jsoninotify.h"
#include "../l0service/trace.h"

#include <sys/inotify.h>

/*
** FSM of the JSONINOTIFY
*/
HcuFsmStateItem_t HcuFsmJsoninotify[] =
{
    //MessageId                 //State                   		 		//Function
	//启始点，固定定义，不要改动, 使用ENTRY/END，意味者MSGID肯定不可能在某个高位区段中；考虑到所有任务共享MsgId，即使分段，也无法实现
	//完全是为了给任务一个初始化的机会，按照状态转移机制，该函数不具备启动的机会，因为任务初始化后自动到FSM_STATE_IDLE
	//如果没有必要进行初始化，可以设置为NULL
	{MSG_ID_ENTRY,       		FSM_STATE_ENTRY,            			fsm_jsoninotify_task_entry}, //Starting

	//System level initialization, only controlled by HCU-MAIN
    {MSG_ID_COM_INIT,       	FSM_STATE_IDLE,            				fsm_jsoninotify_init},
    {MSG_ID_COM_INIT_FEEDBACK,	FSM_STATE_IDLE,            				fsm_com_do_nothing},

	//ANY state entry
    {MSG_ID_COM_INIT_FEEDBACK,	FSM_STATE_COMMON,          				fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT,     FSM_STATE_COMMON,          				fsm_com_heart_beat_rcv},
	{MSG_ID_COM_STOP,       	FSM_STATE_COMMON,          				fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT_FB,  FSM_STATE_COMMON,          				fsm_com_do_nothing},
    {MSG_ID_COM_RESTART,		FSM_STATE_COMMON,            			fsm_jsoninotify_restart},
	{MSG_ID_COM_TIME_OUT,       FSM_STATE_COMMON,          				fsm_com_do_nothing},

	//Task level initialization
    {MSG_ID_COM_INIT_FEEDBACK,	FSM_STATE_JSONINOTIFY_ACTIVIED,            		fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT,     FSM_STATE_JSONINOTIFY_ACTIVIED,       			fsm_com_heart_beat_rcv},
	{MSG_ID_COM_HEART_BEAT_FB,  FSM_STATE_JSONINOTIFY_ACTIVIED,       			fsm_com_do_nothing},

    //结束点，固定定义，不要改动
    {MSG_ID_END,            	FSM_STATE_END,             				NULL},  //Ending
};

char zHcuCmdflagJsonFile[]  = "/tmp/command.json";

//Main Entry
//Input parameter would be useless, but just for similar structure purpose
OPSTAT fsm_jsoninotify_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//除了对全局变量进行操作之外，尽量不要做其它操作，因为该函数将被主任务/线程调用，不是本任务/线程调用
	//该API就是给本任务一个提早介入的入口，可以帮着做些测试性操作
	if (FsmSetState(TASK_ID_JSONINOTIFY, FSM_STATE_IDLE) == FAILURE){
		HcuErrorPrint("JSONINOTIFY: Error Set FSM State at fsm_jsoninotify_task_entry\n");}
	return SUCCESS;
}

OPSTAT fsm_jsoninotify_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;

	if ((src_id > TASK_ID_MIN) &&(src_id < TASK_ID_MAX)){
		//Send back MSG_ID_COM_INIT_FEEDBACK to SVRCON
		msg_struct_com_init_feedback_t snd0;
		memset(&snd0, 0, sizeof(msg_struct_com_init_feedback_t));
		snd0.length = sizeof(msg_struct_com_init_feedback_t);

		//to avoid all task send out the init fb msg at the same time which lead to msgque get stuck
		hcu_usleep(dest_id*HCU_SYSCFG_DURATION_OF_INIT_FB_WAIT_MAX);

		ret = hcu_message_send(MSG_ID_COM_INIT_FEEDBACK, src_id, TASK_ID_JSONINOTIFY, &snd0, snd0.length);
		if (ret == FAILURE){
			HcuErrorPrint("JSONINOTIFY: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_JSONINOTIFY].taskName, zHcuVmCtrTab.task[src_id].taskName);
			return FAILURE;
		}
	}

	//收到初始化消息后，进入初始化状态
	if (FsmSetState(TASK_ID_JSONINOTIFY, FSM_STATE_JSONINOTIFY_INITED) == FAILURE){
		HcuErrorPrint("JSONINOTIFY: Error Set FSM State!\n");
		return FAILURE;
	}

	//Global Variables
	zHcuSysStaPm.taskRunErrCnt[TASK_ID_JSONINOTIFY] = 0;

	//设置状态机到目标状态
	if (FsmSetState(TASK_ID_JSONINOTIFY, FSM_STATE_JSONINOTIFY_ACTIVIED) == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_JSONINOTIFY]++;
		HcuErrorPrint("JSONINOTIFY: Error Set FSM State!\n");
		return FAILURE;
	}
	if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_FAT_ON) != FALSE){
		HcuDebugPrint("JSONINOTIFY: Enter FSM_STATE_JSONINOTIFY_ACTIVED status, Keeping refresh here!\n");
	}

	//进入阻塞式文件读取监控
	if((func_jsoninotify_add_watch(zHcuCmdflagJsonFile)) ==FAILURE){
		HcuDebugPrint("JSONINOTIFY: Add json file watch failure!\n");
	}

	return SUCCESS;
}

OPSTAT fsm_jsoninotify_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HcuErrorPrint("JSONINOTIFY: Internal error counter reach DEAD level, SW-RESTART soon!\n");
	zHcuSysStaPm.statisCnt.restartCnt++;
	fsm_jsoninotify_init(0, 0, NULL, 0);
	return SUCCESS;
}

OPSTAT func_jsoninotify_add_watch(char *monitorJsonFile )
{
	int ret = 0;
	int retInotifyInit;
	int retInotifyWatch;
	char inotifyReadBuf[HCU_SYSCFG_BFSC_CMDJSON_FILE_SIZE_MAX];
	int retReadLen;

	if(NULL == monitorJsonFile) 	{
		HcuErrorPrint("JSONINOTIFY: func_jsoninotify_add_watch function input pointer (monitorJsonFile == NULL), return.\n");
	}

	msg_struct_inotify_uicomm_file_change_ind_t snd0;
	memset(&snd0, 0, sizeof(msg_struct_inotify_uicomm_file_change_ind_t));
	snd0.length = sizeof(msg_struct_inotify_uicomm_file_change_ind_t);

	//创建初始化一个文件Inotify实例
	retInotifyInit = inotify_init();
	 if( retInotifyInit < 0 )
	 {
		 HcuErrorPrint("JSONINOTIFY: initialize json file inotify instance failure!\n");
		 return FAILURE;
	 }

	 //添加要监控的文件到Inotify实例
	 retInotifyWatch =  inotify_add_watch(retInotifyInit, monitorJsonFile, IN_MODIFY);
	 if( retInotifyWatch < 0 )
	 {
		 HcuErrorPrint("JSONINOTIFY: Inotify add watch failure, [file= %s]!\n", monitorJsonFile);
		 return FAILURE;
	 }

	 //监控文件修改事件
	 while(1)
	  {
		 retReadLen = read(retInotifyInit, inotifyReadBuf, HCU_SYSCFG_BFSC_CMDJSON_FILE_SIZE_MAX- 1) ;
	 	 if(retReadLen > 0)
	 	 {
	 		    //JSON文件有修改，发送消息通知BFSCUI模块
				ret = hcu_message_send(MSG_ID_INOTIFY_UICOMM_FILE_CHANGE_IND, TASK_ID_BFSCUICOMM, TASK_ID_JSONINOTIFY, &snd0, snd0.length);
				if (ret == FAILURE){
					HcuErrorPrint("JSONINOTIFY: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_JSONINOTIFY].taskName, zHcuVmCtrTab.task[TASK_ID_BFSCUICOMM].taskName);
					close(retInotifyInit);
					return FAILURE;
				}

	 		    /** Remove watch. */
	 		    if (inotify_rm_watch(retInotifyInit, retInotifyWatch) < 0) {
	 		    	 HcuErrorPrint("TASK_ID_JSONINOTIFY: remove inotify watch failure, [file= %s!\n", monitorJsonFile);
	 		    }
	 		    close(retInotifyInit);
	 	 }

		  //Inotify创建初始化一个文件实例在捕获一次watch事件后需要重新初始化
	 	retInotifyInit = inotify_init();
		 if( retInotifyInit < 0 )
		 {
			 HcuErrorPrint("TASK_ID_JSONINOTIFY: initialize json file inotify instance failure!\n");
			 close(retInotifyInit);
			 return FAILURE;
		 }
		 //重新添加要监控的文件到Inotify实例
		 retInotifyWatch =  inotify_add_watch(retInotifyInit, monitorJsonFile, IN_MODIFY);
		 if( retInotifyWatch < 0 )
		 {
			 HcuErrorPrint("TASK_ID_JSONINOTIFY: Inotify add watch failure, [file= %s]!\n", monitorJsonFile);
			 close(retInotifyInit);
			 return FAILURE;
		 }
	  }

	return SUCCESS;
}

