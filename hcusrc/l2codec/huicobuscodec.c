/**
 ****************************************************************************************
 *
 * @file huicobuscodec.c
 *
 * @brief HUIXMLCODEC
 *
 * BXXH team
 * Created by ZJL, 20160918
 *
 ****************************************************************************************
 */

#include "huicobuscodec.h"
#include "../l0service/trace.h"

/*
** FSM of the HUICOBUSCODEC
*/
HcuFsmStateItem_t HcuFsmHuicobuscodec[] =
{
    //MessageId                 //State                   		 		//Function
	//启始点，固定定义，不要改动, 使用ENTRY/END，意味者MSGID肯定不可能在某个高位区段中；考虑到所有任务共享MsgId，即使分段，也无法实现
	//完全是为了给任务一个初始化的机会，按照状态转移机制，该函数不具备启动的机会，因为任务初始化后自动到FSM_STATE_IDLE
	//如果没有必要进行初始化，可以设置为NULL
	{MSG_ID_ENTRY,       		FSM_STATE_ENTRY,            			fsm_huicobuscodec_task_entry}, //Starting

	//System level initialization, only controlled by HCU-MAIN
    {MSG_ID_COM_INIT,       	FSM_STATE_IDLE,            				fsm_huicobuscodec_init},
    {MSG_ID_COM_RESTART,		FSM_STATE_IDLE,            				fsm_huicobuscodec_restart},
    {MSG_ID_COM_INIT_FEEDBACK,	FSM_STATE_IDLE,            				fsm_com_do_nothing},

    //Task level initialization
    {MSG_ID_COM_RESTART,        FSM_STATE_HUICOBUSCODEC_ACTIVED,        fsm_huicobuscodec_restart},
    {MSG_ID_COM_INIT_FEEDBACK,	FSM_STATE_HUICOBUSCODEC_ACTIVED,        fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT,     FSM_STATE_HUICOBUSCODEC_ACTIVED,   		fsm_com_heart_beat_rcv},
	{MSG_ID_COM_HEART_BEAT_FB,  FSM_STATE_HUICOBUSCODEC_ACTIVED,   		fsm_com_do_nothing},
	{MSG_ID_COM_MQTT_RCV,  		FSM_STATE_HUICOBUSCODEC_ACTIVED,   		fsm_huicobuscodec_mqtt_rcv},

    //结束点，固定定义，不要改动
    {MSG_ID_END,            	FSM_STATE_END,             				NULL},  //Ending
};

//Global variables

//Main Entry
//Input parameter would be useless, but just for similar structure purpose
OPSTAT fsm_huicobuscodec_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//除了对全局变量进行操作之外，尽量不要做其它操作，因为该函数将被主任务/线程调用，不是本任务/线程调用
	//该API就是给本任务一个提早介入的入口，可以帮着做些测试性操作
	if (FsmSetState(TASK_ID_HUICOBUSCODEC, FSM_STATE_IDLE) == FAILURE){
		HcuErrorPrint("HUICOBUSCODEC: Error Set FSM State at fsm_timer_task_entry\n");}
	return SUCCESS;
}

OPSTAT fsm_huicobuscodec_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;

	if ((src_id > TASK_ID_MIN) &&(src_id < TASK_ID_MAX)){
		//Send back MSG_ID_COM_INIT_FEEDBACK to HCU-MAIN
		msg_struct_com_init_feedback_t snd0;
		memset(&snd0, 0, sizeof(msg_struct_com_init_feedback_t));
		snd0.length = sizeof(msg_struct_com_init_feedback_t);
		ret = hcu_message_send(MSG_ID_COM_INIT_FEEDBACK, src_id, TASK_ID_HUICOBUSCODEC, &snd0, snd0.length);
		if (ret == FAILURE){
			HcuErrorPrint("HUICOBUSCODEC: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_HUICOBUSCODEC].taskName, zHcuVmCtrTab.task[src_id].taskName);
			return FAILURE;
		}
	}

	//收到初始化消息后，进入初始化状态
	if (FsmSetState(TASK_ID_HUICOBUSCODEC, FSM_STATE_HUICOBUSCODEC_INITED) == FAILURE){
		HcuErrorPrint("HUICOBUSCODEC: Error Set FSM State!\n");
		return FAILURE;
	}

	//INIT this task global variables
	zHcuSysStaPm.taskRunErrCnt[TASK_ID_HUICOBUSCODEC] = 0;

	//进入等待反馈状态
	if (FsmSetState(TASK_ID_HUICOBUSCODEC, FSM_STATE_HUICOBUSCODEC_ACTIVED) == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_HUICOBUSCODEC]++;
		HcuErrorPrint("HUICOBUSCODEC: Error Set FSM State!\n");
		return FAILURE;
	}
	if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_FAT_ON) != FALSE){
		HcuDebugPrint("HUICOBUSCODEC: Enter FSM_STATE_HUICOBUSCODEC_ACTIVED status, Keeping loop of signal sending here!\n");
	}

	return SUCCESS;
}

OPSTAT fsm_huicobuscodec_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HcuErrorPrint("MQTT: Internal error counter reach DEAD level, SW-RESTART soon!\n");
	zHcuSysStaPm.statisCnt.restartCnt++;
	fsm_huicobuscodec_init(0, 0, NULL, 0);
	return SUCCESS;
}

OPSTAT fsm_huicobuscodec_mqtt_rcv(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret=0;
	msg_struct_com_mqtt_rcv_t rcv;
	HCU_MSG_RCV_CHECK_FOR_LOCALIZE(TASK_ID_HUICOBUSCODEC, msg_struct_com_mqtt_rcv_t);

	//忽略srcNode/destNode/srcId/destId/topicId

	//针对cmdId进行处理
	switch(rcv.cmdId)
	{
	case HUICOBUS_CMDID_cui_uir2hcu_init_req:
	{
		msg_struct_huicobus_uir_init_req_t snd;
		memset(&snd, 0, sizeof(msg_struct_huicobus_uir_init_req_t));
		snd.cmdValue = rcv.cmdValue;
		snd.length = sizeof(msg_struct_huicobus_uir_init_req_t);
		HCU_HUICOBUS_ENCODE_HCU2UIR_MSG_SND_UICOMM(MSG_ID_HUICOBUS_UIR_INIT_REQ);
		break;
	}
	case HUICOBUS_CMDID_cui_uir2hcu_start_resume_req:
	{
		msg_struct_huicobus_uir_resume_req_t snd;
		memset(&snd, 0, sizeof(msg_struct_huicobus_uir_resume_req_t));
		snd.cmdValue = rcv.cmdValue;
		snd.length = sizeof(msg_struct_huicobus_uir_resume_req_t);
		HCU_HUICOBUS_ENCODE_HCU2UIR_MSG_SND_UICOMM(MSG_ID_HUICOBUS_UIR_RESUME_REQ);
		break;
	}
	case HUICOBUS_CMDID_cui_uir2hcu_stop_suspend_req:
	{
		msg_struct_huicobus_uir_suspend_req_t snd;
		memset(&snd, 0, sizeof(msg_struct_huicobus_uir_suspend_req_t));
		snd.cmdValue = rcv.cmdValue;
		snd.length = sizeof(msg_struct_huicobus_uir_suspend_req_t);
		HCU_HUICOBUS_ENCODE_HCU2UIR_MSG_SND_UICOMM(MSG_ID_HUICOBUS_UIR_RESUME_REQ);
		break;
	}
	case HUICOBUS_CMDID_cui_uir2hcu_cali_zero_req:
	{
		msg_struct_huicobus_uir_cali_zero_req_t snd;
		memset(&snd, 0, sizeof(msg_struct_huicobus_uir_cali_zero_req_t));
		snd.cmdValue = rcv.cmdValue;
		snd.length = sizeof(msg_struct_huicobus_uir_cali_zero_req_t);
		HCU_HUICOBUS_ENCODE_HCU2UIR_MSG_SND_UICOMM(MSG_ID_HUICOBUS_UIR_CALI_ZERO_REQ);
		break;
	}
	case HUICOBUS_CMDID_cui_uir2hcu_cali_full_req:
	{
		msg_struct_huicobus_uir_cali_full_req_t snd;
		memset(&snd, 0, sizeof(msg_struct_huicobus_uir_cali_full_req_t));
		snd.cmdValue = rcv.cmdValue;
		snd.length = sizeof(msg_struct_huicobus_uir_cali_full_req_t);
		HCU_HUICOBUS_ENCODE_HCU2UIR_MSG_SND_UICOMM(MSG_ID_HUICOBUS_UIR_CALI_FULL_REQ);
		break;
	}
	case HUICOBUS_CMDID_cui_uir2hcu_study_start_req:
	{
		msg_struct_huicobus_uir_study_start_req_t snd;
		memset(&snd, 0, sizeof(msg_struct_huicobus_uir_study_start_req_t));
		snd.cmdValue = rcv.cmdValue;
		snd.length = sizeof(msg_struct_huicobus_uir_study_start_req_t);
		HCU_HUICOBUS_ENCODE_HCU2UIR_MSG_SND_UICOMM(MSG_ID_HUICOBUS_UIR_STUDY_START_REQ);
		break;
	}
	case HUICOBUS_CMDID_cui_uir2hcu_study_stop_req:
	{
		msg_struct_huicobus_uir_study_stop_req_t snd;
		memset(&snd, 0, sizeof(msg_struct_huicobus_uir_study_stop_req_t));
		snd.cmdValue = rcv.cmdValue;
		snd.length = sizeof(msg_struct_huicobus_uir_study_stop_req_t);
		HCU_HUICOBUS_ENCODE_HCU2UIR_MSG_SND_UICOMM(MSG_ID_HUICOBUS_UIR_STUDY_STOP_REQ);
		break;
	}
	case HUICOBUS_CMDID_cui_uir2hcu_test_cmd_req:
	{
		msg_struct_huicobus_uir_test_cmd_req_t snd;
		memset(&snd, 0, sizeof(msg_struct_huicobus_uir_test_cmd_req_t));
		snd.cmdValue = rcv.cmdValue;
		snd.length = sizeof(msg_struct_huicobus_uir_test_cmd_req_t);
		HCU_HUICOBUS_ENCODE_HCU2UIR_MSG_SND_UICOMM(MSG_ID_HUICOBUS_UIR_TEST_CMD_REQ);
		break;
	}
	default:
		HCU_ERROR_PRINT_HUICOBUSCODEC("HUICOBUS: Receive unsupported message! cmdId=0x%x, cmdValue=%d\n", rcv.cmdId, rcv.cmdValue);
		break;
	}

	return SUCCESS;
}


//发送给界面的通知消息
void func_huicobus_codec_trigger_uir(UINT32 cmdId, INT32 cmdValue)
{
	//
}

//发送API
OPSTAT hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_init_resp(INT32 cmdValue)
{
	msg_struct_com_mqtt_send_t pMsgProc;
	HCU_HUICOBUS_ENCODE_HCU2UIR_MSGHEAD_WITH_FIX_VALUE();
	pMsgProc.cmdId = HUICOBUS_CMDID_cui_hcu2uir_init_resp;

	//HLC part
	pMsgProc.hlcLen = 0;

	//Call MQTT APIs
	HCU_HUICOBUS_ENCODE_HCU2UIR_CALL_API_MQTT_SYN_MODE();
}

//发送API
OPSTAT hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_start_resume_resp(INT32 cmdValue)
{
	msg_struct_com_mqtt_send_t pMsgProc;
	HCU_HUICOBUS_ENCODE_HCU2UIR_MSGHEAD_WITH_FIX_VALUE();
	pMsgProc.cmdId = HUICOBUS_CMDID_cui_hcu2uir_start_resume_resp;

	//HLC part
	pMsgProc.hlcLen = 0;

	//Call MQTT APIs
	HCU_HUICOBUS_ENCODE_HCU2UIR_CALL_API_MQTT_SYN_MODE();
}

//发送API
OPSTAT hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_stop_suspend_resp(INT32 cmdValue)
{
	msg_struct_com_mqtt_send_t pMsgProc;
	HCU_HUICOBUS_ENCODE_HCU2UIR_MSGHEAD_WITH_FIX_VALUE();
	pMsgProc.cmdId = HUICOBUS_CMDID_cui_hcu2uir_stop_suspend_resp;

	//HLC part
	pMsgProc.hlcLen = 0;

	//Call MQTT APIs
	HCU_HUICOBUS_ENCODE_HCU2UIR_CALL_API_MQTT_SYN_MODE();
}

//发送API
OPSTAT hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_cali_zero_resp(INT32 cmdValue)
{
	msg_struct_com_mqtt_send_t pMsgProc;
	HCU_HUICOBUS_ENCODE_HCU2UIR_MSGHEAD_WITH_FIX_VALUE();
	pMsgProc.cmdId = HUICOBUS_CMDID_cui_hcu2uir_cali_zero_resp;

	//HLC part
	pMsgProc.hlcLen = 0;

	//Call MQTT APIs
	HCU_HUICOBUS_ENCODE_HCU2UIR_CALL_API_MQTT_SYN_MODE();
}

//发送API
OPSTAT hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_cali_full_resp(INT32 cmdValue)
{
	msg_struct_com_mqtt_send_t pMsgProc;
	HCU_HUICOBUS_ENCODE_HCU2UIR_MSGHEAD_WITH_FIX_VALUE();
	pMsgProc.cmdId = HUICOBUS_CMDID_cui_hcu2uir_cali_full_resp;

	//HLC part
	pMsgProc.hlcLen = 0;

	//Call MQTT APIs
	HCU_HUICOBUS_ENCODE_HCU2UIR_CALL_API_MQTT_SYN_MODE();
}

//发送API
OPSTAT hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_study_start_resp(INT32 cmdValue)
{
	msg_struct_com_mqtt_send_t pMsgProc;
	HCU_HUICOBUS_ENCODE_HCU2UIR_MSGHEAD_WITH_FIX_VALUE();
	pMsgProc.cmdId = HUICOBUS_CMDID_cui_hcu2uir_study_start_resp;

	//HLC part
	pMsgProc.hlcLen = 0;

	//Call MQTT APIs
	HCU_HUICOBUS_ENCODE_HCU2UIR_CALL_API_MQTT_SYN_MODE();
}

//发送API
OPSTAT hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_study_stop_resp(INT32 cmdValue)
{
	msg_struct_com_mqtt_send_t pMsgProc;
	HCU_HUICOBUS_ENCODE_HCU2UIR_MSGHEAD_WITH_FIX_VALUE();
	pMsgProc.cmdId = HUICOBUS_CMDID_cui_hcu2uir_study_stop_resp;

	//HLC part
	pMsgProc.hlcLen = 0;

	//Call MQTT APIs
	HCU_HUICOBUS_ENCODE_HCU2UIR_CALL_API_MQTT_SYN_MODE();
}

//发送API
OPSTAT hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_test_cmd_resp(INT32 cmdValue)
{
	msg_struct_com_mqtt_send_t pMsgProc;
	HCU_HUICOBUS_ENCODE_HCU2UIR_MSGHEAD_WITH_FIX_VALUE();
	pMsgProc.cmdId = HUICOBUS_CMDID_cui_hcu2uir_test_cmd_resp;

	//HLC part
	pMsgProc.hlcLen = 0;

	//Call MQTT APIs
	HCU_HUICOBUS_ENCODE_HCU2UIR_CALL_API_MQTT_SYN_MODE();
}

//发送API
OPSTAT hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_version_report(INT32 cmdValue)
{
	msg_struct_com_mqtt_send_t pMsgProc;
	HCU_HUICOBUS_ENCODE_HCU2UIR_MSGHEAD_WITH_FIX_VALUE();
	pMsgProc.cmdId = HUICOBUS_CMDID_cui_hcu2uir_version_report;

	//HLC part
	pMsgProc.hlcLen = 0;

	//Call MQTT APIs
	HCU_HUICOBUS_ENCODE_HCU2UIR_CALL_API_MQTT_SYN_MODE();
}

//发送API
OPSTAT hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_status_report(INT32 cmdValue)
{
	msg_struct_com_mqtt_send_t pMsgProc;
	HCU_HUICOBUS_ENCODE_HCU2UIR_MSGHEAD_WITH_FIX_VALUE();
	pMsgProc.cmdId = HUICOBUS_CMDID_cui_hcu2uir_status_report;

	//HLC part
	pMsgProc.hlcLen = 0;

	//Call MQTT APIs
	HCU_HUICOBUS_ENCODE_HCU2UIR_CALL_API_MQTT_SYN_MODE();
}

//发送API
OPSTAT hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_alarm_report(INT32 cmdValue)
{
	msg_struct_com_mqtt_send_t pMsgProc;
	HCU_HUICOBUS_ENCODE_HCU2UIR_MSGHEAD_WITH_FIX_VALUE();
	pMsgProc.cmdId = HUICOBUS_CMDID_cui_hcu2uir_alarm_report;

	//HLC part
	pMsgProc.hlcLen = 0;

	//Call MQTT APIs
	HCU_HUICOBUS_ENCODE_HCU2UIR_CALL_API_MQTT_SYN_MODE();
}

//发送API
OPSTAT hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_error_report(INT32 cmdValue)
{
	msg_struct_com_mqtt_send_t pMsgProc;
	HCU_HUICOBUS_ENCODE_HCU2UIR_MSGHEAD_WITH_FIX_VALUE();
	pMsgProc.cmdId = HUICOBUS_CMDID_cui_hcu2uir_error_report;

	//HLC part
	pMsgProc.hlcLen = 0;

	//Call MQTT APIs
	HCU_HUICOBUS_ENCODE_HCU2UIR_CALL_API_MQTT_SYN_MODE();
}

//发送API
OPSTAT hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_restart_report(INT32 cmdValue)
{
	msg_struct_com_mqtt_send_t pMsgProc;
	HCU_HUICOBUS_ENCODE_HCU2UIR_MSGHEAD_WITH_FIX_VALUE();
	pMsgProc.cmdId = HUICOBUS_CMDID_cui_hcu2uir_restart_report;

	//HLC part
	pMsgProc.hlcLen = 0;

	//Call MQTT APIs
	HCU_HUICOBUS_ENCODE_HCU2UIR_CALL_API_MQTT_SYN_MODE();
}

//发送API
OPSTAT hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_logout_report(INT32 cmdValue)
{
	msg_struct_com_mqtt_send_t pMsgProc;
	HCU_HUICOBUS_ENCODE_HCU2UIR_MSGHEAD_WITH_FIX_VALUE();
	pMsgProc.cmdId = HUICOBUS_CMDID_cui_hcu2uir_logout_report;

	//HLC part
	pMsgProc.hlcLen = 0;

	//Call MQTT APIs
	HCU_HUICOBUS_ENCODE_HCU2UIR_CALL_API_MQTT_SYN_MODE();
}

//发送API
OPSTAT hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_inswgt_bfsc_report(INT32 cmdValue)
{
	msg_struct_com_mqtt_send_t pMsgProc;
	HCU_HUICOBUS_ENCODE_HCU2UIR_MSGHEAD_WITH_FIX_VALUE();
	pMsgProc.cmdId = HUICOBUS_CMDID_cui_hcu2uir_inswgt_bfsc_report;

	//HLC part
	pMsgProc.hlcLen = 0;

	//Call MQTT APIs
	HCU_HUICOBUS_ENCODE_HCU2UIR_CALL_API_MQTT_SYN_MODE();
}

//发送API
OPSTAT hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_inswgt_bfdf_report(INT32 cmdValue)
{
	msg_struct_com_mqtt_send_t pMsgProc;
	HCU_HUICOBUS_ENCODE_HCU2UIR_MSGHEAD_WITH_FIX_VALUE();
	pMsgProc.cmdId = HUICOBUS_CMDID_cui_hcu2uir_inswgt_bfdf_report;

	//HLC part
	pMsgProc.hlcLen = 0;

	//Call MQTT APIs
	HCU_HUICOBUS_ENCODE_HCU2UIR_CALL_API_MQTT_SYN_MODE();
}

//发送API
OPSTAT hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_inswgt_bfhs_report(INT32 cmdValue)
{
	msg_struct_com_mqtt_send_t pMsgProc;
	HCU_HUICOBUS_ENCODE_HCU2UIR_MSGHEAD_WITH_FIX_VALUE();
	pMsgProc.cmdId = HUICOBUS_CMDID_cui_hcu2uir_inswgt_bfhs_report;

	//HLC part
	pMsgProc.hlcLen = 0;

	//Call MQTT APIs
	HCU_HUICOBUS_ENCODE_HCU2UIR_CALL_API_MQTT_SYN_MODE();
}

//发送API
OPSTAT hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_callcell_bfsc_report(INT32 cmdValue)
{
	msg_struct_com_mqtt_send_t pMsgProc;
	HCU_HUICOBUS_ENCODE_HCU2UIR_MSGHEAD_WITH_FIX_VALUE();
	pMsgProc.cmdId = HUICOBUS_CMDID_cui_hcu2uir_callcell_bfsc_report;

	//HLC part
	pMsgProc.hlcLen = 0;

	//Call MQTT APIs
	HCU_HUICOBUS_ENCODE_HCU2UIR_CALL_API_MQTT_SYN_MODE();
}

//发送API
OPSTAT hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_callcell_bfdf_report(INT32 cmdValue)
{
	msg_struct_com_mqtt_send_t pMsgProc;
	HCU_HUICOBUS_ENCODE_HCU2UIR_MSGHEAD_WITH_FIX_VALUE();
	pMsgProc.cmdId = HUICOBUS_CMDID_cui_hcu2uir_callcell_bfdf_report;

	//HLC part
	pMsgProc.hlcLen = 0;

	//Call MQTT APIs
	HCU_HUICOBUS_ENCODE_HCU2UIR_CALL_API_MQTT_SYN_MODE();
}

//发送API
OPSTAT hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_callcell_bfhs_report(INT32 cmdValue)
{
	msg_struct_com_mqtt_send_t pMsgProc;
	HCU_HUICOBUS_ENCODE_HCU2UIR_MSGHEAD_WITH_FIX_VALUE();
	pMsgProc.cmdId = HUICOBUS_CMDID_cui_hcu2uir_callcell_bfhs_report;

	//HLC part
	pMsgProc.hlcLen = 0;

	//Call MQTT APIs
	HCU_HUICOBUS_ENCODE_HCU2UIR_CALL_API_MQTT_SYN_MODE();
}





