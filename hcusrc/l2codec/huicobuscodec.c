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
		msg_struct_huicobus_uir_start_resume_req_t snd;
		memset(&snd, 0, sizeof(msg_struct_huicobus_uir_start_resume_req_t));
		snd.cmdValue = rcv.cmdValue;
		snd.length = sizeof(msg_struct_huicobus_uir_start_resume_req_t);
		HCU_HUICOBUS_ENCODE_HCU2UIR_MSG_SND_UICOMM(MSG_ID_HUICOBUS_UIR_START_RESUME_REQ);
		break;
	}

	case HUICOBUS_CMDID_cui_uir2hcu_stop_suspend_req:
	{
		msg_struct_huicobus_uir_stop_suspend_req_t snd;
		memset(&snd, 0, sizeof(msg_struct_huicobus_uir_stop_suspend_req_t));
		snd.cmdValue = rcv.cmdValue;
		snd.length = sizeof(msg_struct_huicobus_uir_stop_suspend_req_t);
		HCU_HUICOBUS_ENCODE_HCU2UIR_MSG_SND_UICOMM(MSG_ID_HUICOBUS_UIR_STOP_SUSPEND_REQ);
		break;
	}

	case HUICOBUS_CMDID_cui_uir2hcu_static_cali_req:
	{
		msg_struct_huicobus_uir_static_cali_req_t snd;
		memset(&snd, 0, sizeof(msg_struct_huicobus_uir_static_cali_req_t));
		snd.cmdValue = rcv.cmdValue;
		snd.length = sizeof(msg_struct_huicobus_uir_static_cali_req_t);
		HCU_HUICOBUS_ENCODE_HCU2UIR_MSG_SND_UICOMM(MSG_ID_HUICOBUS_UIR_STATIC_CALI_REQ);
		break;
	}

	case HUICOBUS_CMDID_cui_uir2hcu_dynamic_cali_req:
	{
		msg_struct_huicobus_uir_dynamic_cali_req_t snd;
		memset(&snd, 0, sizeof(msg_struct_huicobus_uir_dynamic_cali_req_t));
		snd.cmdValue = rcv.cmdValue;
		snd.length = sizeof(msg_struct_huicobus_uir_dynamic_cali_req_t);
		HCU_HUICOBUS_ENCODE_HCU2UIR_MSG_SND_UICOMM(MSG_ID_HUICOBUS_UIR_DYNAMIC_CALI_REQ);
		break;
	}

	case HUICOBUS_CMDID_cui_uir2hcu_test_cmd_req:
	{
		msg_struct_huicobus_uir_test_cmd_req_t snd;
		memset(&snd, 0, sizeof(msg_struct_huicobus_uir_test_cmd_req_t));
		snd.cmdValue1 = rcv.cmdValue;
		snd.length = sizeof(msg_struct_huicobus_uir_test_cmd_req_t);
		//带HLC结构
		if ((rcv.hlcLen == 0) || (func_huicobuscodec_HUICOBUS_CMDID_cui_uir2hcu_test_cmd_req_received_handle(&snd, &rcv) == FAILURE))
			HCU_ERROR_PRINT_HUICOBUSCODEC("HUICOBUSDECODE: Decoded HUICOBUS_CMDID_cui_uir2hcu_test_cmd_req error!\n");
		HCU_HUICOBUS_ENCODE_HCU2UIR_MSG_SND_UICOMM(MSG_ID_HUICOBUS_UIR_TEST_CMD_REQ);
		break;
	}

	case HUICOBUS_CMDID_cui_uir2hcu_one_key_zero_req:
	{
		msg_struct_huicobus_uir_one_key_clean_zero_req_t snd;
		memset(&snd, 0, sizeof(msg_struct_huicobus_uir_one_key_clean_zero_req_t));
		snd.cmdValue = rcv.cmdValue;
		snd.length = sizeof(msg_struct_huicobus_uir_one_key_clean_zero_req_t);
		HCU_HUICOBUS_ENCODE_HCU2UIR_MSG_SND_UICOMM(MSG_ID_HUICOBUS_UIR_ONE_KEY_ZERO_REQ);
		break;
	}

	default:
		HCU_ERROR_PRINT_HUICOBUSCODEC("HUICOBUS: Receive unsupported message! cmdId=0x%x, cmdValue=%d\n", rcv.cmdId, rcv.cmdValue);
		break;
	}

	return SUCCESS;
}

//FUNC
OPSTAT func_huicobuscodec_HUICOBUS_CMDID_cui_uir2hcu_test_cmd_req_received_handle(msg_struct_huicobus_uir_test_cmd_req_t *snd, msg_struct_com_mqtt_rcv_t *rcv)
{
	#if ((HUICOBUS_CMDID_CUI_HCU2UIR_TEST_CMD_BITMAP != HCU_SYSMSG_SUI_SENSOR_NBR) ||\
			(HUICOBUS_CMDID_CUI_HCU2UIR_TEST_CMD_BUF_LEN_MAX != HUICOBUS_CMDID_CUI_HCU2UIR_TEST_CMD_BUF_LEN_MAX))
	#error PARAMTER SET ERROR!
	#endif
	int i=0;
	struct json_object *jsonobj = NULL;
	struct json_object *decode_jsonobj = NULL;
	jsonobj = json_tokener_parse(rcv->hlContent);
	if (jsonobj == NULL) HCU_ERROR_PRINT_TASK(TASK_ID_HUICOBUSCODEC, "HUICOBUSCODEC: Failed to create json object!\n");

	//解码snrBitmap ARRAY
	json_object_object_get_ex(jsonobj, "snrBitmap", &decode_jsonobj);
	if (decode_jsonobj != NULL){
//		int tt = json_object_array_length(decode_jsonobj);
//		for (i = 0 ; i < json_object_array_length(decode_jsonobj); i++){
//			json_object *val = json_object_array_get_idx(decode_jsonobj, i);
//			if (i<HUICOBUS_CMDID_CUI_HCU2UIR_TEST_CMD_BITMAP)
//				snd->boardBitmap[i] = json_object_get_int(val);
//		}
	  snd->boardBitmap[0] = json_object_get_int(decode_jsonobj);
	  json_object_put(decode_jsonobj);
	}
	//解码cmdvalue1
	json_object_object_get_ex(jsonobj, "cmdTestValue1", &decode_jsonobj);
	if (decode_jsonobj != NULL){
		snd->cmdValue1 = json_object_get_int(decode_jsonobj);
		json_object_put(decode_jsonobj);
	}
	//解码cmdvalue2
	json_object_object_get_ex(jsonobj, "cmdTestValue2", &decode_jsonobj);
	if (decode_jsonobj != NULL){
		snd->cmdValue2 = json_object_get_int(decode_jsonobj);
		json_object_put(decode_jsonobj);
	}
	//解码cmdvalue3
	json_object_object_get_ex(jsonobj, "cmdTestValue3", &decode_jsonobj);
	if (decode_jsonobj != NULL){
		snd->cmdValue3 = json_object_get_int(decode_jsonobj);
		json_object_put(decode_jsonobj);
	}
	//解码cmdvalue4
	json_object_object_get_ex(jsonobj, "cmdTestValue4", &decode_jsonobj);
	if (decode_jsonobj != NULL){
		snd->cmdValue4 = json_object_get_int(decode_jsonobj);
		json_object_put(decode_jsonobj);
	}
	//解码cmdBuf
	json_object_object_get_ex(jsonobj, "cmdBuf", &decode_jsonobj);
	if (decode_jsonobj != NULL){
		for (i = 0 ; i < json_object_array_length(decode_jsonobj); i++){
			json_object *val = json_object_array_get_idx(decode_jsonobj, i);
			if (i<HUICOBUS_CMDID_CUI_HCU2UIR_TEST_CMD_BUF_LEN_MAX) snd->cmdBuf[i] = json_object_get_int(val);
		}
	}
	//全部解完了
	json_object_put(jsonobj);
	return SUCCESS;
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

	return SUCCESS;
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

	return SUCCESS;
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

	return SUCCESS;
}

//发送API
OPSTAT hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_static_cali_resp(INT32 cmdValue, StrHlcIe_cui_hcu2uir_static_cali_resp_t *buf)
{
	msg_struct_com_mqtt_send_t pMsgProc;
	HCU_HUICOBUS_ENCODE_HCU2UIR_MSGHEAD_WITH_FIX_VALUE();
	pMsgProc.cmdId = HUICOBUS_CMDID_cui_hcu2uir_static_cali_resp;

	//HLC part
	struct json_object *jsonobj = NULL;
	jsonobj = json_object_new_object();
    if (jsonobj == NULL) HCU_ERROR_PRINT_TASK(TASK_ID_HUICOBUSCODEC, "HUICOBUSCODEC: Failed to create json object!\n");

    json_object_object_add(jsonobj, "validFlag", json_object_new_int(buf->validFlag));
    json_object_object_add(jsonobj, "errCode", json_object_new_int(buf->errCode));
    json_object_object_add(jsonobj, "weight", json_object_new_int(buf->weight));
    sprintf(pMsgProc.hlContent, "%s", json_object_to_json_string(jsonobj));
    json_object_put(jsonobj);//free
    pMsgProc.hlcLen = strlen(pMsgProc.hlContent);

	//Call MQTT APIs
	HCU_HUICOBUS_ENCODE_HCU2UIR_CALL_API_MQTT_SYN_MODE();

	return SUCCESS;
}

//发送API
OPSTAT hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_dynamic_cali_resp(INT32 cmdValue, StrHlcIe_cui_hcu2uir_dynamic_cali_resp_t *buf)
{
	msg_struct_com_mqtt_send_t pMsgProc;
	HCU_HUICOBUS_ENCODE_HCU2UIR_MSGHEAD_WITH_FIX_VALUE();
	pMsgProc.cmdId = HUICOBUS_CMDID_cui_hcu2uir_dynamic_cali_resp;

	//HLC part
	struct json_object *jsonobj = NULL;
	jsonobj = json_object_new_object();
    if (jsonobj == NULL) HCU_ERROR_PRINT_TASK(TASK_ID_HUICOBUSCODEC, "HUICOBUSCODEC: Failed to create json object!\n");

    json_object_object_add(jsonobj, "validFlag", json_object_new_int(buf->validFlag));
    json_object_object_add(jsonobj, "errCode", json_object_new_int(buf->errCode));
    json_object_object_add(jsonobj, "weight", json_object_new_int(buf->weight));
    sprintf(pMsgProc.hlContent, "%s", json_object_to_json_string(jsonobj));
    json_object_put(jsonobj);//free
    pMsgProc.hlcLen = strlen(pMsgProc.hlContent);

	//Call MQTT APIs
	HCU_HUICOBUS_ENCODE_HCU2UIR_CALL_API_MQTT_SYN_MODE();

	return SUCCESS;
}

//发送API
OPSTAT hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_test_cmd_resp(INT32 cmdValue, StrHlcIe_cui_hcu2uir_test_cmd_resp_t *buf)
{
	msg_struct_com_mqtt_send_t pMsgProc;
	HCU_HUICOBUS_ENCODE_HCU2UIR_MSGHEAD_WITH_FIX_VALUE();
	pMsgProc.cmdId = HUICOBUS_CMDID_cui_hcu2uir_test_cmd_resp;

	//HLC part
	struct json_object *jsonobj = NULL;
	jsonobj = json_object_new_object();
    if (jsonobj == NULL) HCU_ERROR_PRINT_TASK(TASK_ID_HUICOBUSCODEC, "HUICOBUSCODEC: Failed to create json object!\n");

    json_object_object_add(jsonobj, "snrId", json_object_new_int(buf->snrId));
    json_object_object_add(jsonobj, "validFlag", json_object_new_int(buf->validFlag));
    json_object_object_add(jsonobj, "errCode", json_object_new_int(buf->errCode));
    json_object_object_add(jsonobj, "cmdTestValue1", json_object_new_int(buf->cmdTestValue1));
    json_object_object_add(jsonobj, "cmdTestValue2", json_object_new_int(buf->cmdTestValue2));
    json_object_object_add(jsonobj, "cmdTestValue3", json_object_new_int(buf->cmdTestValue3));
    json_object_object_add(jsonobj, "cmdTestValue4", json_object_new_int(buf->cmdTestValue4));

    sprintf(pMsgProc.hlContent, "%s", json_object_to_json_string(jsonobj));
    json_object_put(jsonobj);//free
    pMsgProc.hlcLen = strlen(pMsgProc.hlContent);

	//Call MQTT APIs
	HCU_HUICOBUS_ENCODE_HCU2UIR_CALL_API_MQTT_SYN_MODE();

	return SUCCESS;
}

//发送API
OPSTAT hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_version_report(INT32 cmdValue, StrHlcIe_cui_hcu2uir_version_report_t *buf)
{
	msg_struct_com_mqtt_send_t pMsgProc;
	HCU_HUICOBUS_ENCODE_HCU2UIR_MSGHEAD_WITH_FIX_VALUE();
	pMsgProc.cmdId = HUICOBUS_CMDID_cui_hcu2uir_version_report;

	//HLC part
	struct json_object *jsonobj = NULL;
	jsonobj = json_object_new_object();
    if (jsonobj == NULL) HCU_ERROR_PRINT_TASK(TASK_ID_HUICOBUSCODEC, "HUICOBUSCODEC: Failed to create json object!\n");

    json_object_object_add(jsonobj, "swRel_hcu", json_object_new_int(buf->swRel_hcu));
    json_object_object_add(jsonobj, "swVer_hcu", json_object_new_int(buf->swVer_hcu));
    json_object_object_add(jsonobj, "dbVer_hcu", json_object_new_int(buf->dbVer_hcu));
    json_object_object_add(jsonobj, "devCode_hcu", json_object_new_string(buf->devCode_hcu));
    json_object_object_add(jsonobj, "upgradeFlag_hcu", json_object_new_int(buf->upgradeFlag_hcu));
    json_object_object_add(jsonobj, "swRel_ihu", json_object_new_int(buf->swRel_ihu));
    json_object_object_add(jsonobj, "swVer_ihu", json_object_new_int(buf->swVer_ihu));

    sprintf(pMsgProc.hlContent, "%s", json_object_to_json_string(jsonobj));
    json_object_put(jsonobj);//free
    pMsgProc.hlcLen = strlen(pMsgProc.hlContent);

	//Call MQTT APIs
	HCU_HUICOBUS_ENCODE_HCU2UIR_CALL_API_MQTT_SYN_MODE();

	return SUCCESS;
}

//发送API
OPSTAT hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_status_report(INT32 cmdValue, StrHlcIe_cui_hcu2uir_status_report_t *buf)
{
	msg_struct_com_mqtt_send_t pMsgProc;
	HCU_HUICOBUS_ENCODE_HCU2UIR_MSGHEAD_WITH_FIX_VALUE();
	pMsgProc.cmdId = HUICOBUS_CMDID_cui_hcu2uir_status_report;

	//HLC part
	struct json_object *jsonobj = NULL;
	jsonobj = json_object_new_object();
    if (jsonobj == NULL) HCU_ERROR_PRINT_TASK(TASK_ID_HUICOBUSCODEC, "HUICOBUSCODEC: Failed to create json object!\n");

    json_object_object_add(jsonobj, "boardStatus", json_object_new_int(buf->boardStatus));

    //测试Array
//    struct json_object *array_object = NULL;
//    array_object = json_object_new_array();
//    /*添加json值类型到数组中*/
//    json_object_array_add(array_object, json_object_new_int(256));
//    json_object_array_add(array_object, json_object_new_int(257));
//    json_object_array_add(array_object, json_object_new_int(258));
//    json_object_object_add(jsonobj, "array", array_object);
    //TEST END
    sprintf(pMsgProc.hlContent, "%s", json_object_to_json_string(jsonobj));
    json_object_put(jsonobj);//free
    pMsgProc.hlcLen = strlen(pMsgProc.hlContent);

	//Call MQTT APIs
	HCU_HUICOBUS_ENCODE_HCU2UIR_CALL_API_MQTT_SYN_MODE();

	return SUCCESS;
}

//发送API
OPSTAT hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_alarm_report(INT32 cmdValue, StrHlcIe_cui_hcu2uir_alarm_report_t *buf)
{
	msg_struct_com_mqtt_send_t pMsgProc;
	HCU_HUICOBUS_ENCODE_HCU2UIR_MSGHEAD_WITH_FIX_VALUE();
	pMsgProc.cmdId = HUICOBUS_CMDID_cui_hcu2uir_alarm_report;

	//HLC part
	struct json_object *jsonobj = NULL;
	jsonobj = json_object_new_object();
    if (jsonobj == NULL) HCU_ERROR_PRINT_TASK(TASK_ID_HUICOBUSCODEC, "HUICOBUSCODEC: Failed to create json object!\n");

    json_object_object_add(jsonobj, "desc", json_object_new_string(buf->desc));

    sprintf(pMsgProc.hlContent, "%s", json_object_to_json_string(jsonobj));
    json_object_put(jsonobj);//free
    pMsgProc.hlcLen = strlen(pMsgProc.hlContent);

	//Call MQTT APIs
	HCU_HUICOBUS_ENCODE_HCU2UIR_CALL_API_MQTT_SYN_MODE();

	return SUCCESS;
}

//发送API
OPSTAT hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_error_report(INT32 cmdValue, StrHlcIe_cui_hcu2uir_error_report_t *buf)
{
	msg_struct_com_mqtt_send_t pMsgProc;
	HCU_HUICOBUS_ENCODE_HCU2UIR_MSGHEAD_WITH_FIX_VALUE();
	pMsgProc.cmdId = HUICOBUS_CMDID_cui_hcu2uir_error_report;

	//HLC part
	struct json_object *jsonobj = NULL;
	jsonobj = json_object_new_object();
    if (jsonobj == NULL) HCU_ERROR_PRINT_TASK(TASK_ID_HUICOBUSCODEC, "HUICOBUSCODEC: Failed to create json object!\n");

    json_object_object_add(jsonobj, "desc", json_object_new_string(buf->desc));

    sprintf(pMsgProc.hlContent, "%s", json_object_to_json_string(jsonobj));
    json_object_put(jsonobj);//free
    pMsgProc.hlcLen = strlen(pMsgProc.hlContent);

	//Call MQTT APIs
	HCU_HUICOBUS_ENCODE_HCU2UIR_CALL_API_MQTT_SYN_MODE();

	return SUCCESS;
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

	return SUCCESS;
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

	return SUCCESS;
}

//发送API
OPSTAT hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_inswgt_bfsc_report(INT32 cmdValue, StrHlcIe_cui_hcu2uir_inswgt_bfsc_report_t *buf)
{
	msg_struct_com_mqtt_send_t pMsgProc;
	HCU_HUICOBUS_ENCODE_HCU2UIR_MSGHEAD_WITH_FIX_VALUE();
	pMsgProc.cmdId = HUICOBUS_CMDID_cui_hcu2uir_inswgt_bfsc_report;

	//HLC part
	struct json_object *jsonobj = NULL;
	jsonobj = json_object_new_object();
    if (jsonobj == NULL) HCU_ERROR_PRINT_TASK(TASK_ID_HUICOBUSCODEC, "HUICOBUSCODEC: Failed to create json object!\n");

    json_object_object_add(jsonobj, "weight", json_object_new_int(buf->weight));

    sprintf(pMsgProc.hlContent, "%s", json_object_to_json_string(jsonobj));
    json_object_put(jsonobj);//free
    pMsgProc.hlcLen = strlen(pMsgProc.hlContent);

	//Call MQTT APIs
	HCU_HUICOBUS_ENCODE_HCU2UIR_CALL_API_MQTT_SYN_MODE();

	return SUCCESS;
}

//发送API
OPSTAT hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_inswgt_bfdf_report(INT32 cmdValue, StrHlcIe_cui_hcu2uir_inswgt_bfdf_report_t *buf)
{
	msg_struct_com_mqtt_send_t pMsgProc;
	HCU_HUICOBUS_ENCODE_HCU2UIR_MSGHEAD_WITH_FIX_VALUE();
	pMsgProc.cmdId = HUICOBUS_CMDID_cui_hcu2uir_inswgt_bfdf_report;

	//HLC part
	struct json_object *jsonobj = NULL;
	jsonobj = json_object_new_object();
    if (jsonobj == NULL) HCU_ERROR_PRINT_TASK(TASK_ID_HUICOBUSCODEC, "HUICOBUSCODEC: Failed to create json object!\n");

    json_object_object_add(jsonobj, "weight", json_object_new_int(buf->weight));
    json_object_object_add(jsonobj, "lineId", json_object_new_int(buf->lineId));
    json_object_object_add(jsonobj, "hopperId", json_object_new_int(buf->hopperId));
    json_object_object_add(jsonobj, "groupId", json_object_new_int(buf->groupId));
    json_object_object_add(jsonobj, "validFlag", json_object_new_int(buf->validFlag));
    json_object_object_add(jsonobj, "curWgt", json_object_new_int(buf->curWgt));
    json_object_object_add(jsonobj, "bufWgt", json_object_new_int(buf->bufWgt));
    json_object_object_add(jsonobj, "curRatio", json_object_new_int(buf->curRatio));
    json_object_object_add(jsonobj, "bufRatio", json_object_new_int(buf->bufRatio));

    sprintf(pMsgProc.hlContent, "%s", json_object_to_json_string(jsonobj));
    json_object_put(jsonobj);//free
    pMsgProc.hlcLen = strlen(pMsgProc.hlContent);

	//Call MQTT APIs
	HCU_HUICOBUS_ENCODE_HCU2UIR_CALL_API_MQTT_SYN_MODE();

	return SUCCESS;
}

//发送API
OPSTAT hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_inswgt_bfhs_report(INT32 cmdValue, StrHlcIe_cui_hcu2uir_inswgt_bfhs_report_t *buf)
{
	msg_struct_com_mqtt_send_t pMsgProc;
	HCU_HUICOBUS_ENCODE_HCU2UIR_MSGHEAD_WITH_FIX_VALUE();
	pMsgProc.cmdId = HUICOBUS_CMDID_cui_hcu2uir_inswgt_bfhs_report;

	//HLC part
	struct json_object *jsonobj = NULL;
	jsonobj = json_object_new_object();
    if (jsonobj == NULL) HCU_ERROR_PRINT_TASK(TASK_ID_HUICOBUSCODEC, "HUICOBUSCODEC: Failed to create json object!\n");

    json_object_object_add(jsonobj, "weight", json_object_new_int(buf->weight));
    json_object_object_add(jsonobj, "wmcState", json_object_new_int(buf->wmcState));

    sprintf(pMsgProc.hlContent, "%s", json_object_to_json_string(jsonobj));
    json_object_put(jsonobj);//free
    pMsgProc.hlcLen = strlen(pMsgProc.hlContent);

	//Call MQTT APIs
	HCU_HUICOBUS_ENCODE_HCU2UIR_CALL_API_MQTT_SYN_MODE();

	return SUCCESS;
}

//发送API
OPSTAT hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_callcell_bfsc_report(INT32 cmdValue, StrHlcIe_cui_hcu2uir_callcell_bfsc_report_t *buf)
{
	msg_struct_com_mqtt_send_t pMsgProc;
	HCU_HUICOBUS_ENCODE_HCU2UIR_MSGHEAD_WITH_FIX_VALUE();
	pMsgProc.cmdId = HUICOBUS_CMDID_cui_hcu2uir_callcell_bfsc_report;

	//HLC part
	struct json_object *jsonobj = NULL;
	jsonobj = json_object_new_object();
    if (jsonobj == NULL) HCU_ERROR_PRINT_TASK(TASK_ID_HUICOBUSCODEC, "HUICOBUSCODEC: Failed to create json object!\n");

    json_object_object_add(jsonobj, "combNbr", json_object_new_int(buf->combNbr));
    json_object_object_add(jsonobj, "targetWeight", json_object_new_int(buf->targetWeight));
    json_object_object_add(jsonobj, "upLimitWeight", json_object_new_int(buf->upLimitWeight));
    json_object_object_add(jsonobj, "combWeight", json_object_new_int(buf->combWeight));

    sprintf(pMsgProc.hlContent, "%s", json_object_to_json_string(jsonobj));
    json_object_put(jsonobj);//free
    pMsgProc.hlcLen = strlen(pMsgProc.hlContent);

	//Call MQTT APIs
	HCU_HUICOBUS_ENCODE_HCU2UIR_CALL_API_MQTT_SYN_MODE();

	return SUCCESS;
}

//发送API
OPSTAT hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_callcell_bfdf_report(INT32 cmdValue, StrHlcIe_cui_hcu2uir_callcell_bfdf_report_t *buf)
{
	msg_struct_com_mqtt_send_t pMsgProc;
	HCU_HUICOBUS_ENCODE_HCU2UIR_MSGHEAD_WITH_FIX_VALUE();
	pMsgProc.cmdId = HUICOBUS_CMDID_cui_hcu2uir_callcell_bfdf_report;

	//HLC part
	struct json_object *jsonobj = NULL;
	jsonobj = json_object_new_object();
    if (jsonobj == NULL) HCU_ERROR_PRINT_TASK(TASK_ID_HUICOBUSCODEC, "HUICOBUSCODEC: Failed to create json object!\n");

    json_object_object_add(jsonobj, "hopperId", json_object_new_int(buf->hopperId));
    json_object_object_add(jsonobj, "targetWeight", json_object_new_int(buf->targetWeight));
    json_object_object_add(jsonobj, "upLimitWeight", json_object_new_int(buf->upLimitWeight));
    json_object_object_add(jsonobj, "combWeight", json_object_new_int(buf->combWeight));

    sprintf(pMsgProc.hlContent, "%s", json_object_to_json_string(jsonobj));
    json_object_put(jsonobj);//free
    pMsgProc.hlcLen = strlen(pMsgProc.hlContent);

	//Call MQTT APIs
	HCU_HUICOBUS_ENCODE_HCU2UIR_CALL_API_MQTT_SYN_MODE();

	return SUCCESS;
}

//发送API
OPSTAT hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_callcell_bfhs_report(INT32 cmdValue, StrHlcIe_cui_hcu2uir_callcell_bfhs_report_t *buf)
{
	msg_struct_com_mqtt_send_t pMsgProc;
	HCU_HUICOBUS_ENCODE_HCU2UIR_MSGHEAD_WITH_FIX_VALUE();
	pMsgProc.cmdId = HUICOBUS_CMDID_cui_hcu2uir_callcell_bfhs_report;

	//HLC part
	struct json_object *jsonobj = NULL;
	jsonobj = json_object_new_object();
    if (jsonobj == NULL) HCU_ERROR_PRINT_TASK(TASK_ID_HUICOBUSCODEC, "HUICOBUSCODEC: Failed to create json object!\n");

    json_object_object_add(jsonobj, "targetWeight", json_object_new_int(buf->targetWeight));
    json_object_object_add(jsonobj, "tu1Limit", json_object_new_int(buf->tu1Limit));
    json_object_object_add(jsonobj, "tu2Limit", json_object_new_int(buf->tu2Limit));
    json_object_object_add(jsonobj, "upperLimit", json_object_new_int(buf->upperLimit));
    json_object_object_add(jsonobj, "totalWeight", json_object_new_int(buf->totalWeight));
    json_object_object_add(jsonobj, "totalPackage", json_object_new_int(buf->totalPackage));
    json_object_object_add(jsonobj, "throughput", json_object_new_int(buf->throughput));
    json_object_object_add(jsonobj, "goodPackage", json_object_new_int(buf->goodPackage));
    json_object_object_add(jsonobj, "overWeight", json_object_new_int(buf->overWeight));
    json_object_object_add(jsonobj, "underWeight", json_object_new_int(buf->underWeight));

    sprintf(pMsgProc.hlContent, "%s", json_object_to_json_string(jsonobj));
    json_object_put(jsonobj);//free
    pMsgProc.hlcLen = strlen(pMsgProc.hlContent);

	//Call MQTT APIs
	HCU_HUICOBUS_ENCODE_HCU2UIR_CALL_API_MQTT_SYN_MODE();

	return SUCCESS;
}

//发送API
OPSTAT hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_one_key_clean_zero_resp(INT32 cmdValue)
{
	msg_struct_com_mqtt_send_t pMsgProc;
	HCU_HUICOBUS_ENCODE_HCU2UIR_MSGHEAD_WITH_FIX_VALUE();
	pMsgProc.cmdId = HUICOBUS_CMDID_cui_hcu2uir_one_key_clean_zero_resp;

	//HLC part
	pMsgProc.hlcLen = 0;

	//Call MQTT APIs
	HCU_HUICOBUS_ENCODE_HCU2UIR_CALL_API_MQTT_SYN_MODE();

	return SUCCESS;
}

//发送API
OPSTAT hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_statistic_bfsc_report(INT32 cmdValue, StrHlcIe_cui_hcu2uir_statistic_bfsc_report_t *buf)
{
	msg_struct_com_mqtt_send_t pMsgProc;
	HCU_HUICOBUS_ENCODE_HCU2UIR_MSGHEAD_WITH_FIX_VALUE();
	pMsgProc.cmdId = HUICOBUS_CMDID_cui_hcu2uir_statistic_bfsc_report;

	//HLC part
	struct json_object *jsonobj = NULL;
	jsonobj = json_object_new_object();
    if (jsonobj == NULL) HCU_ERROR_PRINT_TASK(TASK_ID_HUICOBUSCODEC, "HUICOBUSCODEC: Failed to create json object!\n");

    json_object_object_add(jsonobj, "combNbr", json_object_new_int(buf->combNbr));
    json_object_object_add(jsonobj, "targetWeight", json_object_new_int(buf->targetWeight));
    json_object_object_add(jsonobj, "upLimitWeight", json_object_new_int(buf->upLimitWeight));
    json_object_object_add(jsonobj, "combWeight", json_object_new_int(buf->combWeight));

    sprintf(pMsgProc.hlContent, "%s", json_object_to_json_string(jsonobj));
    json_object_put(jsonobj);//free
    pMsgProc.hlcLen = strlen(pMsgProc.hlContent);

	//Call MQTT APIs
	HCU_HUICOBUS_ENCODE_HCU2UIR_CALL_API_MQTT_SYN_MODE();

	return SUCCESS;
}

//发送API
OPSTAT hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_statistic_bfdf_report(INT32 cmdValue, StrHlcIe_cui_hcu2uir_statistic_bfdf_report_t *buf)
{
	msg_struct_com_mqtt_send_t pMsgProc;
	HCU_HUICOBUS_ENCODE_HCU2UIR_MSGHEAD_WITH_FIX_VALUE();
	pMsgProc.cmdId = HUICOBUS_CMDID_cui_hcu2uir_statistic_bfdf_report;

	//HLC part
	struct json_object *jsonobj = NULL;
	jsonobj = json_object_new_object();
    if (jsonobj == NULL) HCU_ERROR_PRINT_TASK(TASK_ID_HUICOBUSCODEC, "HUICOBUSCODEC: Failed to create json object!\n");

    json_object_object_add(jsonobj, "lineId", json_object_new_int(buf->lineId));
    json_object_object_add(jsonobj, "targetWeight", json_object_new_int(buf->targetWeight));
    json_object_object_add(jsonobj, "upLimitWeight", json_object_new_int(buf->upLimitWeight));
    json_object_object_add(jsonobj, "throughputPerMin", json_object_new_int(buf->throughputPerMin));
    json_object_object_add(jsonobj, "totalReject", json_object_new_int(buf->totalReject));
    json_object_object_add(jsonobj, "totalWeight", json_object_new_int(buf->totalWeight));
    json_object_object_add(jsonobj, "totalPackage", json_object_new_int(buf->totalPackage));

    sprintf(pMsgProc.hlContent, "%s", json_object_to_json_string(jsonobj));
    json_object_put(jsonobj);//free
    pMsgProc.hlcLen = strlen(pMsgProc.hlContent);

	//Call MQTT APIs
	HCU_HUICOBUS_ENCODE_HCU2UIR_CALL_API_MQTT_SYN_MODE();

	return SUCCESS;
}

//发送API
OPSTAT hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_statistic_bfhs_report(INT32 cmdValue, StrHlcIe_cui_hcu2uir_statistic_bfhs_report_t *buf)
{
	msg_struct_com_mqtt_send_t pMsgProc;
	HCU_HUICOBUS_ENCODE_HCU2UIR_MSGHEAD_WITH_FIX_VALUE();
	pMsgProc.cmdId = HUICOBUS_CMDID_cui_hcu2uir_statistic_bfhs_report;

	//HLC part
	struct json_object *jsonobj = NULL;
	jsonobj = json_object_new_object();
    if (jsonobj == NULL) HCU_ERROR_PRINT_TASK(TASK_ID_HUICOBUSCODEC, "HUICOBUSCODEC: Failed to create json object!\n");

    json_object_object_add(jsonobj, "targetWeight", json_object_new_int(buf->targetWeight));
    json_object_object_add(jsonobj, "tu1Limit", json_object_new_int(buf->tu1LimitWeight));
    json_object_object_add(jsonobj, "tu2Limit", json_object_new_int(buf->tu2LimitWeight));
    json_object_object_add(jsonobj, "upperLimit", json_object_new_int(buf->upLimitWeight));
    json_object_object_add(jsonobj, "totalWeight", json_object_new_int(buf->totalWeight));
    json_object_object_add(jsonobj, "totalPackage", json_object_new_int(buf->totalPackage));
    json_object_object_add(jsonobj, "throughput", json_object_new_int(buf->throughputPerMin));
    json_object_object_add(jsonobj, "goodPackage", json_object_new_int(buf->totalGoodPackage));
    json_object_object_add(jsonobj, "overWeight", json_object_new_int(buf->totalOverReject));
    json_object_object_add(jsonobj, "underWeight", json_object_new_int(buf->totalUnderReject));

    sprintf(pMsgProc.hlContent, "%s", json_object_to_json_string(jsonobj));
    json_object_put(jsonobj);//free
    pMsgProc.hlcLen = strlen(pMsgProc.hlContent);

	//Call MQTT APIs
	HCU_HUICOBUS_ENCODE_HCU2UIR_CALL_API_MQTT_SYN_MODE();

	return SUCCESS;
}

//发送API
OPSTAT hcu_encode_HUICOBUS_CMDID_cui_hcu2uir_pullin_confirm(INT32 cmdValue, StrHlcIe_cui_hcu2uir_pullin_confirm_t *buf)
{
	msg_struct_com_mqtt_send_t pMsgProc;
	HCU_HUICOBUS_ENCODE_HCU2UIR_MSGHEAD_WITH_FIX_VALUE();
	pMsgProc.cmdId = HUICOBUS_CMDID_cui_hcu2uir_pullin_confirm;

	//HLC part
	struct json_object *jsonobj = NULL;
	jsonobj = json_object_new_object();
    if (jsonobj == NULL) HCU_ERROR_PRINT_TASK(TASK_ID_HUICOBUSCODEC, "HUICOBUSCODEC: Failed to create json object!\n");

    json_object_object_add(jsonobj, "lineId", json_object_new_int(buf->lineId));
    json_object_object_add(jsonobj, "hopperId", json_object_new_int(buf->hopperId));
    json_object_object_add(jsonobj, "groupId", json_object_new_int(buf->groupId));
    json_object_object_add(jsonobj, "validFlag", json_object_new_int(buf->validFlag));
    json_object_object_add(jsonobj, "curWeight", json_object_new_int(buf->curWeight));
    json_object_object_add(jsonobj, "bufWeight", json_object_new_int(buf->bufWeight));
    json_object_object_add(jsonobj, "curRatio", json_object_new_int(buf->curRatio));
    json_object_object_add(jsonobj, "bufRatio", json_object_new_int(buf->bufRatio));

    sprintf(pMsgProc.hlContent, "%s", json_object_to_json_string(jsonobj));
    json_object_put(jsonobj);//free
    pMsgProc.hlcLen = strlen(pMsgProc.hlContent);

	//Call MQTT APIs
	HCU_HUICOBUS_ENCODE_HCU2UIR_CALL_API_MQTT_SYN_MODE();

	return SUCCESS;
}


