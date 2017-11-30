/**
 ****************************************************************************************
 *
 * @file huijsoncodec.c
 *
 * @brief HUIXMLCODEC
 *
 * BXXH team
 * Created by ZJL, 20160918
 *
 ****************************************************************************************
 */

#include "huijsoncodec.h"
#include "../l0service/trace.h"

/*
** FSM of the HUIJSONCODEC
*/
HcuFsmStateItem_t HcuFsmHuijsoncodec[] =
{
    //MessageId                 //State                   		 		//Function
	//启始点，固定定义，不要改动, 使用ENTRY/END，意味者MSGID肯定不可能在某个高位区段中；考虑到所有任务共享MsgId，即使分段，也无法实现
	//完全是为了给任务一个初始化的机会，按照状态转移机制，该函数不具备启动的机会，因为任务初始化后自动到FSM_STATE_IDLE
	//如果没有必要进行初始化，可以设置为NULL
	{MSG_ID_ENTRY,       		FSM_STATE_ENTRY,            			fsm_huijsoncodec_task_entry}, //Starting

	//System level initialization, only controlled by HCU-MAIN
    {MSG_ID_COM_INIT,       	FSM_STATE_IDLE,            				fsm_huijsoncodec_init},
    {MSG_ID_COM_RESTART,		FSM_STATE_IDLE,            				fsm_huijsoncodec_restart},
    {MSG_ID_COM_INIT_FEEDBACK,	FSM_STATE_IDLE,            				fsm_com_do_nothing},

    //Task level initialization
    {MSG_ID_COM_RESTART,        FSM_STATE_HUIJSONCODEC_ACTIVED,         fsm_huijsoncodec_restart},
    {MSG_ID_COM_INIT_FEEDBACK,	FSM_STATE_HUIJSONCODEC_ACTIVED,         fsm_com_do_nothing},
	{MSG_ID_COM_HEART_BEAT,     FSM_STATE_HUIJSONCODEC_ACTIVED,   		fsm_com_heart_beat_rcv},
	{MSG_ID_COM_HEART_BEAT_FB,  FSM_STATE_HUIJSONCODEC_ACTIVED,   		fsm_com_do_nothing},
	{MSG_ID_COM_MQTT_RCV,  		FSM_STATE_HUIJSONCODEC_ACTIVED,   		fsm_huijsoncodec_mqtt_rcv},

    //结束点，固定定义，不要改动
    {MSG_ID_END,            	FSM_STATE_END,             				NULL},  //Ending
};

//Global variables

//Main Entry
//Input parameter would be useless, but just for similar structure purpose
OPSTAT fsm_huijsoncodec_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//除了对全局变量进行操作之外，尽量不要做其它操作，因为该函数将被主任务/线程调用，不是本任务/线程调用
	//该API就是给本任务一个提早介入的入口，可以帮着做些测试性操作
	if (FsmSetState(TASK_ID_HUIJSONCODEC, FSM_STATE_IDLE) == FAILURE){
		HcuErrorPrint("HUIJSONCODEC: Error Set FSM State at fsm_timer_task_entry\n");}
	return SUCCESS;
}

OPSTAT fsm_huijsoncodec_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	int ret=0;

	if ((src_id > TASK_ID_MIN) &&(src_id < TASK_ID_MAX)){
		//Send back MSG_ID_COM_INIT_FEEDBACK to HCU-MAIN
		msg_struct_com_init_feedback_t snd0;
		memset(&snd0, 0, sizeof(msg_struct_com_init_feedback_t));
		snd0.length = sizeof(msg_struct_com_init_feedback_t);
		ret = hcu_message_send(MSG_ID_COM_INIT_FEEDBACK, src_id, TASK_ID_HUIJSONCODEC, &snd0, snd0.length);
		if (ret == FAILURE){
			HcuErrorPrint("HUIJSONCODEC: Send message error, TASK [%s] to TASK[%s]!\n", zHcuVmCtrTab.task[TASK_ID_HUIJSONCODEC].taskName, zHcuVmCtrTab.task[src_id].taskName);
			return FAILURE;
		}
	}

	//收到初始化消息后，进入初始化状态
	if (FsmSetState(TASK_ID_HUIJSONCODEC, FSM_STATE_HUIJSONCODEC_INITED) == FAILURE){
		HcuErrorPrint("HUIJSONCODEC: Error Set FSM State!\n");
		return FAILURE;
	}

	//INIT this task global variables
	zHcuSysStaPm.taskRunErrCnt[TASK_ID_HUIJSONCODEC] = 0;

	//进入等待反馈状态
	if (FsmSetState(TASK_ID_HUIJSONCODEC, FSM_STATE_HUIJSONCODEC_ACTIVED) == FAILURE){
		zHcuSysStaPm.taskRunErrCnt[TASK_ID_HUIJSONCODEC]++;
		HcuErrorPrint("HUIJSONCODEC: Error Set FSM State!\n");
		return FAILURE;
	}
	if ((zHcuSysEngPar.debugMode & HCU_SYSCFG_TRACE_DEBUG_FAT_ON) != FALSE){
		HcuDebugPrint("HUIJSONCODEC: Enter FSM_STATE_HUIJSONCODEC_ACTIVED status, Keeping loop of signal sending here!\n");
	}

	return SUCCESS;
}

OPSTAT fsm_huijsoncodec_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	HcuErrorPrint("MQTT: Internal error counter reach DEAD level, SW-RESTART soon!\n");
	zHcuSysStaPm.statisCnt.restartCnt++;
	fsm_huijsoncodec_init(0, 0, NULL, 0);
	return SUCCESS;
}

OPSTAT fsm_huijsoncodec_mqtt_rcv(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len)
{
	//int ret=0;
	msg_struct_com_mqtt_rcv_t rcv;
	HCU_MSG_RCV_CHECK_FOR_LOCALIZE(TASK_ID_HUIJSONCODEC, msg_struct_com_mqtt_rcv_t);

	//忽略srcNode/destNode/srcId/destId/topicId

	//针对cmdId进行处理
	switch(rcv.cmdId)
	{
	case HUITP_JSON_MSGID_uni_earthquake_ctrl_req:
	{
//		msg_struct_huijson_uir_init_req_t snd;
//		memset(&snd, 0, sizeof(msg_struct_huijson_uir_init_req_t));
//		snd.cmdValue = rcv.cmdValue;
//		snd.length = sizeof(msg_struct_huijson_uir_init_req_t);
//		HCU_HUIJSON_ENCODE_HCU2UIR_MSG_SND_UICOMM(MSG_ID_HUIJSON_UIR_INIT_REQ);
		break;
	}

	case HUITP_JSON_MSGID_uni_earthquake_data_confirm:
	{
		break;
	}

	default:
		HCU_ERROR_PRINT_HUIJSONCODEC("HUIJSON: Receive unsupported message! cmdId=0x%x, cmdValue=%d\n", rcv.cmdId, rcv.cmdValue);
		break;
	}

	return SUCCESS;
}

//FUNC
//OPSTAT func_huijsoncodec_HUIJSON_CMDID_cui_uir2hcu_test_cmd_req_received_handle(msg_struct_huijson_uir_test_cmd_req_t *snd, msg_struct_com_mqtt_rcv_t *rcv)
//{
//	#if ((HUIJSON_CMDID_CUI_HCU2UIR_TEST_CMD_BITMAP != HCU_SYSMSG_SUI_SENSOR_NBR) ||\
//			(HUIJSON_CMDID_CUI_HCU2UIR_TEST_CMD_BUF_LEN_MAX != HUIJSON_CMDID_CUI_HCU2UIR_TEST_CMD_BUF_LEN_MAX))
//	#error PARAMTER SET ERROR!
//	#endif
//	int i=0;
//	struct json_object *jsonobj = NULL;
//	struct json_object *decode_jsonobj = NULL;
//	jsonobj = json_tokener_parse(rcv->hlContent);
//	if (jsonobj == NULL) HCU_ERROR_PRINT_TASK(TASK_ID_HUIJSONCODEC, "HUIJSONCODEC: Failed to create json object!\n");
//
//	//解码snrBitmap ARRAY
//	decode_jsonobj = json_object_object_get(jsonobj, "snrBitmap");
//	if (decode_jsonobj != NULL){
//		for (i = 0 ; i < json_object_array_length(decode_jsonobj); i++){
//			json_object *val = json_object_array_get_idx(decode_jsonobj, i);
//			if (i<HUIJSON_CMDID_CUI_HCU2UIR_TEST_CMD_BITMAP) snd->boardBitmap[i] = json_object_get_int(val);
//		}
//	  json_object_put(decode_jsonobj);
//	}
//	//解码cmdvalue1
//	decode_jsonobj = json_object_object_get(jsonobj, "cmdTestValue1");
//	if (decode_jsonobj != NULL){
//		snd->cmdValue1 = json_object_get_int(decode_jsonobj);
//		json_object_put(decode_jsonobj);
//	}
//	//解码cmdvalue2
//	decode_jsonobj = json_object_object_get(jsonobj, "cmdTestValue2");
//	if (decode_jsonobj != NULL){
//		snd->cmdValue2 = json_object_get_int(decode_jsonobj);
//		json_object_put(decode_jsonobj);
//	}
//	//解码cmdvalue3
//	decode_jsonobj = json_object_object_get(jsonobj, "cmdTestValue3");
//	if (decode_jsonobj != NULL){
//		snd->cmdValue3 = json_object_get_int(decode_jsonobj);
//		json_object_put(decode_jsonobj);
//	}
//	//解码cmdvalue4
//	decode_jsonobj = json_object_object_get(jsonobj, "cmdTestValue4");
//	if (decode_jsonobj != NULL){
//		snd->cmdValue4 = json_object_get_int(decode_jsonobj);
//		json_object_put(decode_jsonobj);
//	}
//	//解码cmdBuf
//	decode_jsonobj = json_object_object_get(jsonobj, "cmdBuf");
//	if (decode_jsonobj != NULL){
//		for (i = 0 ; i < json_object_array_length(decode_jsonobj); i++){
//			json_object *val = json_object_array_get_idx(decode_jsonobj, i);
//			if (i<HUIJSON_CMDID_CUI_HCU2UIR_TEST_CMD_BUF_LEN_MAX) snd->cmdBuf[i] = json_object_get_int(val);
//		}
//	}
//	//全部解完了
//	json_object_put(jsonobj);
//	return SUCCESS;
//}

//发送API
OPSTAT hcu_encode_HUITP_JSON_MSGID_uni_earthquake_ctrl_resp(char *toUser, StrIe_HUITP_JSON_MSGID_uni_earthquake_ctrl_resp_t *buf)
{
    struct json_object *jsonobj = NULL;
    struct json_object *cont_jsonobj = NULL;
//    jsonobj = json_object_new_object();
//    cont_jsonobj = json_object_new_object();
//    if (jsonobj == NULL) HCU_ERROR_PRINT_TASK(TASK_ID_HUIJSONCODEC, "HUIJSONCODEC: Failed to create json object!\n");
//    if (cont_jsonobj == NULL) HCU_ERROR_PRINT_TASK(TASK_ID_HUIJSONCODEC, "HUIJSONCODEC: Failed to create json object!\n");
//    json_object_object_add(jsonobj, HUITP_MSG_HUIJSON_CONSTANT_TO_USER, json_object_new_string(toUser));
//    json_object_object_add(jsonobj, HUITP_MSG_HUIJSON_CONSTANT_FROM_USER, json_object_new_string(zHcuSysEngPar.hwBurnId.equLable));
//    json_object_object_add(jsonobj, HUITP_MSG_HUIJSON_CONSTANT_CREATE_TIME, json_object_new_string(HUITP_MSG_HUIXML_MSGTYPE_HUIJSON_STRING));
//    json_object_object_add(jsonobj, HUITP_MSG_HUIJSON_CONSTANT_MSG_ID, json_object_new_int(HUITP_JSON_MSGID_uni_earthquake_ctrl_resp));
    HCU_HUIJSON_ENCODE_MSGHEAD_WITH_FIX_VALUE(HUITP_JSON_MSGID_uni_earthquake_ctrl_resp);

    //内容体
    json_object_object_add(cont_jsonobj, "cmdId", json_object_new_int(buf->cmdId));
    json_object_object_add(cont_jsonobj, "result", json_object_new_int(buf->result));


    HCU_HUIJSON_ENCODE_MSGBUF_SEND_TO_BH();
//    //输出字符串
//    json_object_object_add(jsonobj, HUITP_MSG_HUIJSON_CONSTANT_IE_CONTENT, json_object_new_string(json_object_to_json_string(cont_jsonobj)));
//    json_object_object_add(jsonobj, HUITP_MSG_HUIJSON_CONSTANT_MSG_LEN, json_object_new_int(strlen(json_object_to_json_string(cont_jsonobj))));
//    json_object_put(cont_jsonobj);//free
//    json_object_object_add(jsonobj, HUITP_MSG_HUIJSON_CONSTANT_FUNC_FLAG, json_object_new_int(0));
//	  CloudDataSendBuf_t pMsgOutput;
//    memset(pMsgOutput.curBuf, 0, sizeof(pMsgOutput.curBuf));
//    sprintf((char*)pMsgOutput.curBuf, "%s", json_object_to_json_string(jsonobj));
//    pMsgOutput.curLen = strlen(pMsgOutput.curBuf);
//    json_object_put(cont_jsonobj);//free
//	//Send out
//	if (func_cloudvela_send_data_to_cloud(&pMsgOutput) == FAILURE)
//		HCU_ERROR_PRINT_TASK(TASK_ID_CLOUDVELA, "CLOUDVELA: Send message error!\n");

	return SUCCESS;
}

//发送API
OPSTAT hcu_encode_HUITP_JSON_MSGID_uni_earthquake_data_report(char *toUser, StrIe_HUITP_JSON_MSGID_uni_earthquake_data_report_t *buf)
{
    struct json_object *jsonobj = NULL;
    struct json_object *cont_jsonobj = NULL;
    HCU_HUIJSON_ENCODE_MSGHEAD_WITH_FIX_VALUE(HUITP_JSON_MSGID_uni_earthquake_data_report);

    //内容体
    json_object_object_add(cont_jsonobj, "cmdId", json_object_new_int(buf->cmdId));
    json_object_object_add(cont_jsonobj, "xData", json_object_new_int(buf->xData));
    json_object_object_add(cont_jsonobj, "yData", json_object_new_int(buf->yData));
    json_object_object_add(cont_jsonobj, "zData", json_object_new_int(buf->zData));

    HCU_HUIJSON_ENCODE_MSGBUF_SEND_TO_BH();
	return SUCCESS;
}



