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

OPSTAT ihu_huijsoncodec_decode_HUITP_JSON_message(char *input)
{
	//int ret=0;
	UINT32 t=0;
	UINT32 msgId=0, msgLen=0;
	struct json_object *jsonobj = NULL;
	struct json_object *Cont_jsonobj = NULL;
	char stmp[HCU_HUIJSONCODEC_CONTENT_MAX_LEN];

	//分解对象
	jsonobj = json_tokener_parse(input);
	if (jsonobj == NULL) HCU_ERROR_PRINT_TASK(TASK_ID_CLOUDVELA, "HUIJSONCODEC: Failed to decode json object!\n");

	//ToUsr_jsonobj
	Cont_jsonobj = json_object_object_get(jsonobj, HUITP_MSG_HUIJSON_CONSTANT_TO_USER);
	if (Cont_jsonobj == NULL)
	{
		json_object_put(jsonobj);
		HCU_ERROR_PRINT_TASK(TASK_ID_CLOUDVELA, "HUIJSONCODEC: Failed to decode ToUsr_jsonobj object!\n");
	}
	t = strlen(json_object_get_string(Cont_jsonobj));
	memset(stmp, 0, sizeof(stmp));
	strncpy(stmp, json_object_get_string(Cont_jsonobj), t<HCU_HUIJSONCODEC_CONTENT_MAX_LEN?t:HCU_HUIJSONCODEC_CONTENT_MAX_LEN);
	json_object_put(Cont_jsonobj);
	if (strcmp(stmp, zHcuSysEngPar.hwBurnId.equLable) != 0){
		json_object_put(jsonobj);
		HCU_ERROR_PRINT_TASK(TASK_ID_CLOUDVELA, "HUIJSONCODEC: Failed to match receive deviceId!\n");
	}

	//FrUsr_jsonobj
	Cont_jsonobj = json_object_object_get(jsonobj, HUITP_MSG_HUIJSON_CONSTANT_FROM_USER);;
	if (Cont_jsonobj == NULL)
	{
		json_object_put(jsonobj);
		HCU_ERROR_PRINT_TASK(TASK_ID_CLOUDVELA, "HUIJSONCODEC: Failed to decode FrUsr_jsonobj object!\n");
	}
	json_object_put(Cont_jsonobj);

	//CrTim_jsonobj
	Cont_jsonobj = json_object_object_get(jsonobj, HUITP_MSG_HUIJSON_CONSTANT_CREATE_TIME);;
	if (Cont_jsonobj == NULL)
	{
		json_object_put(jsonobj);
		HCU_ERROR_PRINT_TASK(TASK_ID_CLOUDVELA, "HUIJSONCODEC: Failed to decode CrTim_jsonobj object!\n");
	}
	json_object_put(Cont_jsonobj);

	//MsgTp_jsonobj
	Cont_jsonobj = json_object_object_get(jsonobj, HUITP_MSG_HUIJSON_CONSTANT_MSG_TYPE);
	if (Cont_jsonobj == NULL)
	{
		json_object_put(jsonobj);
		HCU_ERROR_PRINT_TASK(TASK_ID_CLOUDVELA, "HUIJSONCODEC: Failed to decode MsgTp_jsonobj object!\n");
	}
	t = strlen(json_object_get_string(Cont_jsonobj));
	memset(stmp, 0, sizeof(stmp));
	strncpy(stmp, json_object_get_string(Cont_jsonobj), t<HCU_HUIJSONCODEC_CONTENT_MAX_LEN?t:HCU_HUIJSONCODEC_CONTENT_MAX_LEN);
	json_object_put(Cont_jsonobj);
	if (strcmp(stmp, HUITP_MSG_HUIXML_MSGTYPE_HUIJSON_STRING) != 0)
	{
		json_object_put(jsonobj);
		HCU_ERROR_PRINT_TASK(TASK_ID_CLOUDVELA, "HUIJSONCODEC: Failed to match receive deviceId!\n");
	}

	//MsgId_jsonobj
	Cont_jsonobj = json_object_object_get(jsonobj, HUITP_MSG_HUIJSON_CONSTANT_MSG_ID);;
	if (Cont_jsonobj == NULL)
	{
		json_object_put(jsonobj);
		HCU_ERROR_PRINT_TASK(TASK_ID_CLOUDVELA, "HUIJSONCODEC: Failed to decode MsgId_jsonobj object!\n");
	}
	msgId=json_object_get_int(Cont_jsonobj);
	json_object_put(Cont_jsonobj);

	//MsgLn_jsonobj
	Cont_jsonobj = json_object_object_get(jsonobj, HUITP_MSG_HUIJSON_CONSTANT_MSG_LEN);;
	if (Cont_jsonobj == NULL)
	{
		json_object_put(jsonobj);
		HCU_ERROR_PRINT_TASK(TASK_ID_CLOUDVELA, "HUIJSONCODEC: Failed to decode MsgLn_jsonobj object!\n");
	}
	msgId=json_object_get_int(Cont_jsonobj);
	json_object_put(Cont_jsonobj);

	//FnFlg_jsonobj
	Cont_jsonobj = json_object_object_get(jsonobj, HUITP_MSG_HUIJSON_CONSTANT_FUNC_FLAG);;
	if (Cont_jsonobj == NULL)
	{
		json_object_put(jsonobj);
		HCU_ERROR_PRINT_TASK(TASK_ID_CLOUDVELA, "HUIJSONCODEC: Failed to decode FnFlg_jsonobj object!\n");
	}
	json_object_put(Cont_jsonobj);

	//IeCnt_jsonobj
	Cont_jsonobj = json_object_object_get(jsonobj, HUITP_MSG_HUIJSON_CONSTANT_IE_CONTENT);
	json_object_put(jsonobj);
	if (msgLen==0){
		if (Cont_jsonobj != NULL){
			json_object_put(Cont_jsonobj);
			HCU_ERROR_PRINT_TASK(TASK_ID_CLOUDVELA, "HUIJSONCODEC: Failed to decode Cont_jsonobj object!\n");
		}
	}
	else{
		if (Cont_jsonobj == NULL) HCU_ERROR_PRINT_TASK(TASK_ID_CLOUDVELA, "HUIJSONCODEC: Failed to decode IeCnt_jsonobj object!\n");
		else{
			memset(stmp, 0, sizeof(stmp));
			strncpy(stmp, json_object_get_string(Cont_jsonobj), t<HCU_HUIJSONCODEC_CONTENT_MAX_LEN?t:HCU_HUIJSONCODEC_CONTENT_MAX_LEN);
			json_object_put(Cont_jsonobj);
		}
	}

	//针对cmdId进行处理
	switch(msgId)
	{
		case HUITP_JSON_MSGID_uni_earthquake_ctrl_req:
		{
			jsonobj = json_tokener_parse(stmp);
			if (jsonobj == NULL) HCU_ERROR_PRINT_TASK(TASK_ID_CLOUDVELA, "HUIJSONCODEC: Failed to decode json object!\n");
			Cont_jsonobj = json_object_object_get(jsonobj, "cmdId");
			if (Cont_jsonobj != NULL){
				//a = json_object_get_int(Cont_jsonobj); //赋值
				json_object_put(Cont_jsonobj);
			}
			json_object_put(jsonobj);
			//发送消息给L3
	//		msg_struct_huijson_uir_init_req_t snd;
	//		memset(&snd, 0, sizeof(msg_struct_huijson_uir_init_req_t));
	//		snd.cmdValue = rcv.cmdValue;
	//		snd.length = sizeof(msg_struct_huijson_uir_init_req_t);
	//		HCU_HUIJSON_ENCODE_HCU2UIR_MSG_SND_UICOMM(MSG_ID_HUIJSON_UIR_INIT_REQ);
			break;
		}

		case HUITP_JSON_MSGID_uni_earthquake_data_confirm:
		{
			jsonobj = json_tokener_parse(stmp);
			if (jsonobj == NULL) HCU_ERROR_PRINT_TASK(TASK_ID_CLOUDVELA, "HUIJSONCODEC: Failed to decode json object!\n");
			Cont_jsonobj = json_object_object_get(jsonobj, "cmdId");
			if (Cont_jsonobj != NULL){
				//a = json_object_get_int(Cont_jsonobj); //赋值
				json_object_put(Cont_jsonobj);
			}
			Cont_jsonobj = json_object_object_get(jsonobj, "result");
			if (Cont_jsonobj != NULL){
				//a = json_object_get_int(Cont_jsonobj); //赋值
				json_object_put(Cont_jsonobj);
			}
			json_object_put(jsonobj);
			//发送消息给L3
			break;
		}

		case HUITP_JSON_MSGID_uni_heart_beat_confirm:
		{
			jsonobj = json_tokener_parse(stmp);
			if (jsonobj == NULL) HCU_ERROR_PRINT_TASK(TASK_ID_CLOUDVELA, "HUIJSONCODEC: Failed to decode json object!\n");
			Cont_jsonobj = json_object_object_get(jsonobj, "rand");
			if (Cont_jsonobj != NULL){
				//a = json_object_get_int(Cont_jsonobj); //赋值
				json_object_put(Cont_jsonobj);
			}
			json_object_put(jsonobj);
			//发送消息给L3
			break;
		}

		default:
		{
			HCU_ERROR_PRINT_HUIJSONCODEC("HUIJSON: Receive unsupported message! msgId=0x%x, msgLen=%d\n", msgId, msgLen);
			break;
		}
	}

	return SUCCESS;
}

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

OPSTAT hcu_encode_HUITP_JSON_MSGID_uni_heart_beat_report(char *toUser, StrIe_HUITP_JSON_MSGID_uni_heart_beat_report_t *buf)
{
    struct json_object *jsonobj = NULL;
    struct json_object *cont_jsonobj = NULL;
    HCU_HUIJSON_ENCODE_MSGHEAD_WITH_FIX_VALUE(HUITP_JSON_MSGID_uni_heart_beat_report);

    //内容体
    json_object_object_add(cont_jsonobj, "rand", json_object_new_int(buf->rand));

    HCU_HUIJSON_ENCODE_MSGBUF_SEND_TO_BH();
	return SUCCESS;
}



