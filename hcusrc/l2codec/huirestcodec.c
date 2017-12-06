/**
 ****************************************************************************************
 *
 * @file huirestcodec.c
 *
 * @brief HUIXMLCODEC
 *
 * BXXH team
 * Created by ZJL, 20160918
 *
 ****************************************************************************************
 */

#include "huirestcodec.h"
#include "../l0service/trace.h"

//发送和接收API
OPSTAT hcu_restful_HUIREST_ACTIONID_PRINTER_callcell_bfsc(StrRestMsgIn_HUIREST_ACTIONID_PRINTER_callcell_bfsc_t *in, StrRestMsgOut_HUIREST_ACTIONID_general_t *out)
{
    struct json_object *jsonobj = NULL;
    struct json_object *cont_jsonobj = NULL;
	char stmp[HCU_HUIRESTCODEC_BODY_MAX_LEN];
	UINT32 t=0;

	//固定头
    HCU_HUIREST_ENCODE_ACTIONHEAD_WITH_FIX_VALUE(HUIREST_ACCESS_CONST_SVRTAG_PRINTER_IN_STRING, HUIREST_ACTIONID_PRINTER_callcell_bfsc);
//    jsonobj = json_object_new_object();
//    cont_jsonobj = json_object_new_object();
//    if (jsonobj == NULL) HCU_ERROR_PRINT_TASK(TASK_ID_CLOUDVELA, "HUIRESTCODEC: Failed to create json object!\n");
//    if (cont_jsonobj == NULL){
//    	json_object_put(jsonobj);//free
//    	HCU_ERROR_PRINT_TASK(TASK_ID_CLOUDVELA, "HUIRESTCODEC: Failed to create json object!\n");
//    }
//    json_object_object_add(jsonobj, HUIREST_ACCESS_CONST_SERVICE_TAG, json_object_new_string(HUIREST_ACCESS_CONST_SVRTAG_PRINTER));
//    json_object_object_add(jsonobj, HUIREST_ACCESS_CONST_ACTION_ID, json_object_new_int(HUIREST_ACTIONID_PRINTER_callcell_bfsc));
//    json_object_object_add(jsonobj, HUIREST_ACCESS_CONST_PAR_FLAG, json_object_new_boolean(TRUE));


    //内容体
    json_object_object_add(cont_jsonobj, "cfgId", json_object_new_int(in->cfgId));
    json_object_object_add(cont_jsonobj, "matNbr", json_object_new_int(in->matNbr));
    json_object_object_add(cont_jsonobj, "wgtTarget", json_object_new_double(in->wgtTarget));
    json_object_object_add(cont_jsonobj, "wgtCurrent", json_object_new_double(in->wgtCurrent));
    json_object_object_add(cont_jsonobj, "wgtUnit", json_object_new_string(in->wgtUnit));
    json_object_object_add(cont_jsonobj, "oprName", json_object_new_string(in->oprName));
    json_object_object_add(cont_jsonobj, "sn", json_object_new_int(in->sn));
    json_object_object_add(cont_jsonobj, "timeStamp", json_object_new_int(in->timeStamp));
    json_object_object_add(cont_jsonobj, "barCode", json_object_new_string(in->barCode));
    json_object_object_add(cont_jsonobj, "qrCode", json_object_new_string(in->qrCode));
    json_object_object_add(cont_jsonobj, "barCodeFile", json_object_new_string(in->barCodeFile));
    json_object_object_add(cont_jsonobj, "qrCodeFile", json_object_new_string(in->qrCodeFile));

    //发送
    char sendBuf[HCU_HUIRESTCODEC_CONTENT_MAX_LEN];
	msg_struct_ethernet_cloudvela_data_rx_t receiveBuffer;
//    json_object_object_add(jsonobj, HUIREST_ACCESS_CONST_PAR_CONTENT, cont_jsonobj);
//    json_object_put(cont_jsonobj);//free
//    memset(buf, 0, sizeof(buf));
//    sprintf(buf, "%s", json_object_to_json_string(jsonobj));
//    json_object_put(jsonobj);//free
//	  memset(&receiveBuffer, 0, sizeof(msg_struct_ethernet_cloudvela_data_rx_t));
    HCU_HUIREST_ENCODE_ACTIONHEAD_TRANSFER_TO_STRING();

    //CURL过程
    if (func_huirestcodec_curl_data_send(HUIREST_ACCESS_CONST_SVRTAG_PRINTER_IN_NUMBER, sendBuf, &receiveBuffer) == FAILURE)
    	HCU_ERROR_PRINT_HUIRESTCODEC("HUIRESTCODEC: Error perform curl!\n");

    //解码反馈
    //HUIREST_ACCESS_CONST_SERVICE_TAG
    HCU_HUIREST_DECODE_ACTION_HEAD_SERVICE_TAG(HUIREST_ACCESS_CONST_SVRTAG_PRINTER_IN_STRING);
//	jsonobj = json_tokener_parse(receiveBuffer.buf);
//	if (jsonobj == NULL) HCU_ERROR_PRINT_TASK(TASK_ID_CLOUDVELA, "HUIRESTCODEC: Failed to decode json object!\n");
//	//HUIREST_ACCESS_CONST_SERVICE_TAG
//	cont_jsonobj = json_object_object_get(jsonobj, HUIREST_ACCESS_CONST_SERVICE_TAG);
//	if (cont_jsonobj == NULL)
//	{
//		json_object_put(jsonobj);
//		HCU_ERROR_PRINT_TASK(TASK_ID_CLOUDVELA, "HUIRESTCODEC: Failed to decode HUIREST_ACCESS_CONST_SERVICE_TAG object!\n");
//	}
//	t = strlen(json_object_get_string(cont_jsonobj));
//	memset(stmp, 0, sizeof(stmp));
//	strncpy(stmp, json_object_get_string(cont_jsonobj), t<HCU_HUIRESTCODEC_BODY_MAX_LEN?t:HCU_HUIRESTCODEC_BODY_MAX_LEN);
//	json_object_put(cont_jsonobj);
//	if (strcmp(stmp, HUIREST_ACCESS_CONST_SVRTAG_PRINTER) != 0){
//		json_object_put(jsonobj);
//		HCU_ERROR_PRINT_TASK(TASK_ID_CLOUDVELA, "HUIJSONCODEC: Failed to match receive SERVICE TAG!\n");
//	}

	//HUIREST_ACCESS_CONST_ACTION_ID
    HCU_HUIREST_DECODE_ACTION_HEAD_ACTIONID(HUIREST_ACTIONID_PRINTER_callcell_bfsc);
//	cont_jsonobj = json_object_object_get(jsonobj, HUIREST_ACCESS_CONST_ACTION_ID);
//	if (cont_jsonobj == NULL)
//	{
//		json_object_put(jsonobj);
//		HCU_ERROR_PRINT_TASK(TASK_ID_CLOUDVELA, "HUIRESTCODEC: Failed to decode HUIREST_ACCESS_CONST_ACTION_ID object!\n");
//	}
//	if (json_object_get_int(cont_jsonobj) != HUIREST_ACTIONID_PRINTER_callcell_bfsc){
//		json_object_put(jsonobj);
//		HCU_ERROR_PRINT_TASK(TASK_ID_CLOUDVELA, "HUIJSONCODEC: Failed to match receive ACTION ID!\n");
//	}

	//HUIREST_ACCESS_CONST_PAR_FLAG
    HCU_HUIREST_DECODE_ACTION_HEAD_PAR_FLAG(TRUE);
//	cont_jsonobj = json_object_object_get(jsonobj, HUIREST_ACCESS_CONST_PAR_FLAG);
//	if (cont_jsonobj == NULL)
//	{
//		json_object_put(jsonobj);
//		HCU_ERROR_PRINT_TASK(TASK_ID_CLOUDVELA, "HUIRESTCODEC: Failed to decode HUIREST_ACCESS_CONST_PAR_FLAG object!\n");
//	}
//	if (json_object_get_boolean(cont_jsonobj) != TRUE){
//		json_object_put(jsonobj);
//		HCU_ERROR_PRINT_TASK(TASK_ID_CLOUDVELA, "HUIJSONCODEC: Failed to match receive ACTION ID!\n");
//	}

	//HUIREST_ACCESS_CONST_PAR_CONTENT
    HCU_HUIREST_DECODE_ACTION_HEAD_PAR_CONTENT();
//	cont_jsonobj = json_object_object_get(jsonobj, HUIREST_ACCESS_CONST_PAR_CONTENT);
//	if (cont_jsonobj == NULL)
//	{
//		json_object_put(jsonobj);
//		HCU_ERROR_PRINT_TASK(TASK_ID_CLOUDVELA, "HUIRESTCODEC: Failed to decode HUIREST_ACCESS_CONST_PAR_CONTENT object!\n");
//	}
//	json_object_put(jsonobj);

	//解内容第二层内容
    HCU_HUIREST_DECODE_ACTION_HEAD_PAR_GENERAL_FB();
//	jsonobj = json_object_object_get(jsonobj, "sucFlag");
//	if (jsonobj == NULL)
//	{
//		json_object_put(cont_jsonobj);
//		HCU_ERROR_PRINT_TASK(TASK_ID_CLOUDVELA, "HUIRESTCODEC: Failed to decode HUIREST_ACCESS_CONST_PAR_CONTENT L2 object!\n");
//	}
//	out->sucFlag = json_object_get_boolean(jsonobj);
//	json_object_put(jsonobj);
//	jsonobj = json_object_object_get(jsonobj, "errCode");
//	if (jsonobj == NULL)
//	{
//		json_object_put(cont_jsonobj);
//		HCU_ERROR_PRINT_TASK(TASK_ID_CLOUDVELA, "HUIRESTCODEC: Failed to decode HUIREST_ACCESS_CONST_PAR_CONTENT L2 object!\n");
//	}
//	out->errCode = json_object_get_int(jsonobj);
//	json_object_put(jsonobj);
//	json_object_put(cont_jsonobj);

	//返回
	return SUCCESS;
}

//发送和接收API
OPSTAT hcu_restful_HUIREST_ACTIONID_PRINTER_callcell_bfdf(StrRestMsgIn_HUIREST_ACTIONID_PRINTER_callcell_bfdf_t *in, StrRestMsgOut_HUIREST_ACTIONID_general_t *out)
{
    struct json_object *jsonobj = NULL;
    struct json_object *cont_jsonobj = NULL;
	char stmp[HCU_HUIRESTCODEC_BODY_MAX_LEN];
	UINT32 t=0;
    char sendBuf[HCU_HUIRESTCODEC_CONTENT_MAX_LEN];
	msg_struct_ethernet_cloudvela_data_rx_t receiveBuffer;

	//固定头
    HCU_HUIREST_ENCODE_ACTIONHEAD_WITH_FIX_VALUE(HUIREST_ACCESS_CONST_SVRTAG_PRINTER_IN_STRING, HUIREST_ACTIONID_PRINTER_callcell_bfdf);

    //内容体
    json_object_object_add(cont_jsonobj, "cfgId", json_object_new_int(in->cfgId));
    json_object_object_add(cont_jsonobj, "wgtTarget", json_object_new_double(in->wgtTarget));
    json_object_object_add(cont_jsonobj, "wgtCurrent", json_object_new_double(in->wgtCurrent));
    json_object_object_add(cont_jsonobj, "wgtUnit", json_object_new_string(in->wgtUnit));
    json_object_object_add(cont_jsonobj, "oprName", json_object_new_string(in->oprName));
    json_object_object_add(cont_jsonobj, "sn", json_object_new_int(in->sn));
    json_object_object_add(cont_jsonobj, "timeStamp", json_object_new_int(in->timeStamp));
    json_object_object_add(cont_jsonobj, "barCode", json_object_new_string(in->barCode));
    json_object_object_add(cont_jsonobj, "qrCode", json_object_new_string(in->qrCode));
    json_object_object_add(cont_jsonobj, "barCodeFile", json_object_new_string(in->barCodeFile));
    json_object_object_add(cont_jsonobj, "qrCodeFile", json_object_new_string(in->qrCodeFile));

    //发送
    HCU_HUIREST_ENCODE_ACTIONHEAD_TRANSFER_TO_STRING();

    //CURL过程
    if (func_huirestcodec_curl_data_send(HUIREST_ACCESS_CONST_SVRTAG_PRINTER_IN_NUMBER, sendBuf, &receiveBuffer) == FAILURE)
    	HCU_ERROR_PRINT_HUIRESTCODEC("HUIRESTCODEC: Error perform curl!\n");

    //解码反馈
    //HUIREST_ACCESS_CONST_SERVICE_TAG
    HCU_HUIREST_DECODE_ACTION_HEAD_SERVICE_TAG(HUIREST_ACCESS_CONST_SVRTAG_PRINTER_IN_STRING);

	//HUIREST_ACCESS_CONST_ACTION_ID
    HCU_HUIREST_DECODE_ACTION_HEAD_ACTIONID(HUIREST_ACTIONID_PRINTER_callcell_bfdf);

	//HUIREST_ACCESS_CONST_PAR_FLAG
    HCU_HUIREST_DECODE_ACTION_HEAD_PAR_FLAG(TRUE);

    //HUIREST_ACCESS_CONST_PAR_CONTENT
    HCU_HUIREST_DECODE_ACTION_HEAD_PAR_CONTENT();

	//解内容第二层内容
    HCU_HUIREST_DECODE_ACTION_HEAD_PAR_GENERAL_FB();

	//返回
	return SUCCESS;
}

//发送和接收API
OPSTAT hcu_restful_HUIREST_ACTIONID_PRINTER_callcell_bfhs(StrRestMsgIn_HUIREST_ACTIONID_PRINTER_callcell_bfhs_t *in, StrRestMsgOut_HUIREST_ACTIONID_general_t *out)
{
    struct json_object *jsonobj = NULL;
    struct json_object *cont_jsonobj = NULL;
	char stmp[HCU_HUIRESTCODEC_BODY_MAX_LEN];
	UINT32 t=0;
    char sendBuf[HCU_HUIRESTCODEC_CONTENT_MAX_LEN];
	msg_struct_ethernet_cloudvela_data_rx_t receiveBuffer;

	//固定头
    HCU_HUIREST_ENCODE_ACTIONHEAD_WITH_FIX_VALUE(HUIREST_ACCESS_CONST_SVRTAG_PRINTER_IN_STRING, HUIREST_ACTIONID_PRINTER_callcell_bfhs);

    //内容体
    json_object_object_add(cont_jsonobj, "cfgId", json_object_new_int(in->cfgId));
    json_object_object_add(cont_jsonobj, "wgtTarget", json_object_new_double(in->wgtTarget));
    json_object_object_add(cont_jsonobj, "wgtCurrent", json_object_new_double(in->wgtCurrent));
    json_object_object_add(cont_jsonobj, "wgtUnit", json_object_new_string(in->wgtUnit));
    json_object_object_add(cont_jsonobj, "oprName", json_object_new_string(in->oprName));
    json_object_object_add(cont_jsonobj, "sn", json_object_new_int(in->sn));
    json_object_object_add(cont_jsonobj, "timeStamp", json_object_new_int(in->timeStamp));
    json_object_object_add(cont_jsonobj, "barCode", json_object_new_string(in->barCode));
    json_object_object_add(cont_jsonobj, "qrCode", json_object_new_string(in->qrCode));
    json_object_object_add(cont_jsonobj, "barCodeFile", json_object_new_string(in->barCodeFile));
    json_object_object_add(cont_jsonobj, "qrCodeFile", json_object_new_string(in->qrCodeFile));

    //发送
    HCU_HUIREST_ENCODE_ACTIONHEAD_TRANSFER_TO_STRING();

    //CURL过程
    if (func_huirestcodec_curl_data_send(HUIREST_ACCESS_CONST_SVRTAG_PRINTER_IN_NUMBER, sendBuf, &receiveBuffer) == FAILURE)
    	HCU_ERROR_PRINT_HUIRESTCODEC("HUIRESTCODEC: Error perform curl!\n");

    //解码反馈
    //HUIREST_ACCESS_CONST_SERVICE_TAG
    HCU_HUIREST_DECODE_ACTION_HEAD_SERVICE_TAG(HUIREST_ACCESS_CONST_SVRTAG_PRINTER_IN_STRING);

	//HUIREST_ACCESS_CONST_ACTION_ID
    HCU_HUIREST_DECODE_ACTION_HEAD_ACTIONID(HUIREST_ACTIONID_PRINTER_callcell_bfhs);

	//HUIREST_ACCESS_CONST_PAR_FLAG
    HCU_HUIREST_DECODE_ACTION_HEAD_PAR_FLAG(TRUE);

    //HUIREST_ACCESS_CONST_PAR_CONTENT
    HCU_HUIREST_DECODE_ACTION_HEAD_PAR_CONTENT();

	//解内容第二层内容
    HCU_HUIREST_DECODE_ACTION_HEAD_PAR_GENERAL_FB();

	//返回
	return SUCCESS;
}

//发送和接收API
OPSTAT hcu_restful_HUIREST_ACTIONID_PRINTER_fam_sdqx_md1(StrRestMsgIn_HUIREST_ACTIONID_PRINTER_fam_sdqx_md1_t *in, StrRestMsgOut_HUIREST_ACTIONID_general_t *out)
{
    struct json_object *jsonobj = NULL;
    struct json_object *cont_jsonobj = NULL;
	char stmp[HCU_HUIRESTCODEC_BODY_MAX_LEN];
	UINT32 t=0;
    char sendBuf[HCU_HUIRESTCODEC_CONTENT_MAX_LEN];
	msg_struct_ethernet_cloudvela_data_rx_t receiveBuffer;

	//固定头
    HCU_HUIREST_ENCODE_ACTIONHEAD_WITH_FIX_VALUE(HUIREST_ACCESS_CONST_SVRTAG_PRINTER_IN_STRING, HUIREST_ACTIONID_PRINTER_fam_sdqx_md1);

    //内容体
    json_object_object_add(cont_jsonobj, "oprName", json_object_new_string(in->oprName));
    json_object_object_add(cont_jsonobj, "sn", json_object_new_int(in->sn));
    json_object_object_add(cont_jsonobj, "devId", json_object_new_string(in->devId));
    json_object_object_add(cont_jsonobj, "timeStamp", json_object_new_int(in->timeStamp));
    json_object_object_add(cont_jsonobj, "barCode", json_object_new_string(in->barCode));
    json_object_object_add(cont_jsonobj, "qrCode", json_object_new_string(in->qrCode));

    //发送
    HCU_HUIREST_ENCODE_ACTIONHEAD_TRANSFER_TO_STRING();

    //CURL过程
    if (func_huirestcodec_curl_data_send(HUIREST_ACCESS_CONST_SVRTAG_PRINTER_IN_NUMBER, sendBuf, &receiveBuffer) == FAILURE)
    	HCU_ERROR_PRINT_HUIRESTCODEC("HUIRESTCODEC: Error perform curl!\n");

    //解码反馈
    //HUIREST_ACCESS_CONST_SERVICE_TAG
    HCU_HUIREST_DECODE_ACTION_HEAD_SERVICE_TAG(HUIREST_ACCESS_CONST_SVRTAG_PRINTER_IN_STRING);

	//HUIREST_ACCESS_CONST_ACTION_ID
    HCU_HUIREST_DECODE_ACTION_HEAD_ACTIONID(HUIREST_ACTIONID_PRINTER_fam_sdqx_md1);

	//HUIREST_ACCESS_CONST_PAR_FLAG
    HCU_HUIREST_DECODE_ACTION_HEAD_PAR_FLAG(TRUE);

    //HUIREST_ACCESS_CONST_PAR_CONTENT
    HCU_HUIREST_DECODE_ACTION_HEAD_PAR_CONTENT();

	//解内容第二层内容
    HCU_HUIREST_DECODE_ACTION_HEAD_PAR_GENERAL_FB();

	//返回
	return SUCCESS;
}

//发送和接收API
OPSTAT hcu_restful_HUIREST_ACTIONID_PRINTER_fam_sdqx_md2(StrRestMsgIn_HUIREST_ACTIONID_PRINTER_fam_sdqx_md2_t *in, StrRestMsgOut_HUIREST_ACTIONID_general_t *out)
{
    struct json_object *jsonobj = NULL;
    struct json_object *cont_jsonobj = NULL;
	char stmp[HCU_HUIRESTCODEC_BODY_MAX_LEN];
	UINT32 t=0;
    char sendBuf[HCU_HUIRESTCODEC_CONTENT_MAX_LEN];
	msg_struct_ethernet_cloudvela_data_rx_t receiveBuffer;

	//固定头
    HCU_HUIREST_ENCODE_ACTIONHEAD_WITH_FIX_VALUE(HUIREST_ACCESS_CONST_SVRTAG_PRINTER_IN_STRING, HUIREST_ACTIONID_PRINTER_fam_sdqx_md2);

    //内容体
    json_object_object_add(cont_jsonobj, "oprName", json_object_new_string(in->oprName));
    json_object_object_add(cont_jsonobj, "sn", json_object_new_int(in->sn));
    json_object_object_add(cont_jsonobj, "devId", json_object_new_string(in->devId));
    json_object_object_add(cont_jsonobj, "timeStamp", json_object_new_int(in->timeStamp));
    json_object_object_add(cont_jsonobj, "barCode", json_object_new_string(in->barCode));
    json_object_object_add(cont_jsonobj, "qrCode", json_object_new_string(in->qrCode));

    //发送
    HCU_HUIREST_ENCODE_ACTIONHEAD_TRANSFER_TO_STRING();

    //CURL过程
    if (func_huirestcodec_curl_data_send(HUIREST_ACCESS_CONST_SVRTAG_PRINTER_IN_NUMBER, sendBuf, &receiveBuffer) == FAILURE)
    	HCU_ERROR_PRINT_HUIRESTCODEC("HUIRESTCODEC: Error perform curl!\n");

    //解码反馈
    //HUIREST_ACCESS_CONST_SERVICE_TAG
    HCU_HUIREST_DECODE_ACTION_HEAD_SERVICE_TAG(HUIREST_ACCESS_CONST_SVRTAG_PRINTER_IN_STRING);

	//HUIREST_ACCESS_CONST_ACTION_ID
    HCU_HUIREST_DECODE_ACTION_HEAD_ACTIONID(HUIREST_ACTIONID_PRINTER_fam_sdqx_md2);

	//HUIREST_ACCESS_CONST_PAR_FLAG
    HCU_HUIREST_DECODE_ACTION_HEAD_PAR_FLAG(TRUE);

    //HUIREST_ACCESS_CONST_PAR_CONTENT
    HCU_HUIREST_DECODE_ACTION_HEAD_PAR_CONTENT();

	//解内容第二层内容
    HCU_HUIREST_DECODE_ACTION_HEAD_PAR_GENERAL_FB();

	//返回
	return SUCCESS;
}

//发送和接收API
OPSTAT hcu_restful_HUIREST_ACTIONID_DBA_yczx_temp_update(StrRestMsgIn_HUIREST_ACTIONID_DBA_yczx_temp_update_t *in, StrRestMsgOut_HUIREST_ACTIONID_general_t *out)
{
    struct json_object *jsonobj = NULL;
    struct json_object *cont_jsonobj = NULL;
	char stmp[HCU_HUIRESTCODEC_BODY_MAX_LEN];
	UINT32 t=0;
    char sendBuf[HCU_HUIRESTCODEC_CONTENT_MAX_LEN];
	msg_struct_ethernet_cloudvela_data_rx_t receiveBuffer;

	//固定头
    HCU_HUIREST_ENCODE_ACTIONHEAD_WITH_FIX_VALUE(HUIREST_ACCESS_CONST_SVRTAG_DBA_IN_STRING, HUIREST_ACTIONID_DBA_yczx_temp_update);

    //内容体
    json_object_object_add(cont_jsonobj, "tempCur", json_object_new_double(in->tempCur));
    json_object_object_add(cont_jsonobj, "tempMin", json_object_new_double(in->tempMin));
    json_object_object_add(cont_jsonobj, "tempMax", json_object_new_double(in->tempMax));
    json_object_object_add(cont_jsonobj, "tempAvg", json_object_new_double(in->tempAvg));
    json_object_object_add(cont_jsonobj, "uint", json_object_new_string(in->uint));
    json_object_object_add(cont_jsonobj, "timeStamp", json_object_new_int(in->timeStamp));

    //发送
    HCU_HUIREST_ENCODE_ACTIONHEAD_TRANSFER_TO_STRING();

    //CURL过程
    if (func_huirestcodec_curl_data_send(HUIREST_ACCESS_CONST_SVRTAG_DBA_IN_NUMBER, sendBuf, &receiveBuffer) == FAILURE)
    	HCU_ERROR_PRINT_HUIRESTCODEC("HUIRESTCODEC: Error perform curl!\n");

    //解码反馈
    //HUIREST_ACCESS_CONST_SERVICE_TAG
    HCU_HUIREST_DECODE_ACTION_HEAD_SERVICE_TAG(HUIREST_ACCESS_CONST_SVRTAG_DBA_IN_STRING);

	//HUIREST_ACCESS_CONST_ACTION_ID
    HCU_HUIREST_DECODE_ACTION_HEAD_ACTIONID(HUIREST_ACTIONID_DBA_yczx_temp_update);

	//HUIREST_ACCESS_CONST_PAR_FLAG
    HCU_HUIREST_DECODE_ACTION_HEAD_PAR_FLAG(TRUE);

    //HUIREST_ACCESS_CONST_PAR_CONTENT
    HCU_HUIREST_DECODE_ACTION_HEAD_PAR_CONTENT();

	//解内容第二层内容
    HCU_HUIREST_DECODE_ACTION_HEAD_PAR_GENERAL_FB();

	//返回
	return SUCCESS;
}

//发送和接收API
OPSTAT hcu_restful_HUIREST_ACTIONID_VISION_test1(StrRestMsgIn_HUIREST_ACTIONID_VISION_test1_t *in, StrRestMsgOut_HUIREST_ACTIONID_general_t *out)
{
    struct json_object *jsonobj = NULL;
    struct json_object *cont_jsonobj = NULL;
	char stmp[HCU_HUIRESTCODEC_BODY_MAX_LEN];
	UINT32 t=0;
    char sendBuf[HCU_HUIRESTCODEC_CONTENT_MAX_LEN];
	msg_struct_ethernet_cloudvela_data_rx_t receiveBuffer;

	//固定头
    HCU_HUIREST_ENCODE_ACTIONHEAD_WITH_FIX_VALUE(HUIREST_ACCESS_CONST_SVRTAG_VISION_IN_STRING, HUIREST_ACTIONID_VISION_test1);

    //内容体
    json_object_object_add(cont_jsonobj, "testData", json_object_new_double(in->testData));

    //发送
    HCU_HUIREST_ENCODE_ACTIONHEAD_TRANSFER_TO_STRING();

    //CURL过程
    if (func_huirestcodec_curl_data_send(HUIREST_ACCESS_CONST_SVRTAG_VISION_IN_NUMBER, sendBuf, &receiveBuffer) == FAILURE)
    	HCU_ERROR_PRINT_HUIRESTCODEC("HUIRESTCODEC: Error perform curl!\n");

    //解码反馈
    //HUIREST_ACCESS_CONST_SERVICE_TAG
    HCU_HUIREST_DECODE_ACTION_HEAD_SERVICE_TAG(HUIREST_ACCESS_CONST_SVRTAG_VISION_IN_STRING);

	//HUIREST_ACCESS_CONST_ACTION_ID
    HCU_HUIREST_DECODE_ACTION_HEAD_ACTIONID(HUIREST_ACTIONID_VISION_test1);

	//HUIREST_ACCESS_CONST_PAR_FLAG
    HCU_HUIREST_DECODE_ACTION_HEAD_PAR_FLAG(TRUE);

    //HUIREST_ACCESS_CONST_PAR_CONTENT
    HCU_HUIREST_DECODE_ACTION_HEAD_PAR_CONTENT();

	//解内容第二层内容
    HCU_HUIREST_DECODE_ACTION_HEAD_PAR_GENERAL_FB();

	//返回
	return SUCCESS;
}

//CURL发送给RESTFUL服务器
OPSTAT func_huirestcodec_curl_data_send(UINT8 serTag, char *buf, msg_struct_ethernet_cloudvela_data_rx_t* receiveBuffer)
{
	//int ret = 0;

	//初始化
	curl_global_init(CURL_GLOBAL_ALL);
	CURL *curl = curl_easy_init();
	if (curl == NULL) HCU_ERROR_PRINT_HUIRESTCODEC("HUIRESTCODEC: Init CURL error!\n");

	//也就是说，默认情况下libcurl完成一个任务以后，出于重用连接的考虑不会马上关闭如果没有新的TCP请求来重用这个连接，那么只能等到CLOSE_WAIT超时
	//这个时间默认在7200秒甚至更高，太多的CLOSE_WAIT连接会导致性能问题。这里设置为0就是为了重用
	//curl_easy_setopt(curl, CURLOPT_FORBID_REUSE, 0);

	//设置目标地址：未来再将这个地址改为可配置的，目前先做死
	if (serTag == HUIREST_ACCESS_CONST_SVRTAG_PRINTER_IN_NUMBER)
		curl_easy_setopt(curl, CURLOPT_URL, HCU_SYSCFG_RESTFUL_PRINTER);
	else if (serTag == HUIREST_ACCESS_CONST_SVRTAG_DBA_IN_NUMBER)
		curl_easy_setopt(curl, CURLOPT_URL, HCU_SYSCFG_RESTFUL_DBA);
	else if (serTag == HUIREST_ACCESS_CONST_SVRTAG_VISION_IN_NUMBER)
		curl_easy_setopt(curl, CURLOPT_URL, HCU_SYSCFG_RESTFUL_VISION);
	else
		HCU_ERROR_PRINT_HUIRESTCODEC("HUIRESTCODEC: Error set access target!\n");

	//设置超时时长，做为发送API，这个设置绝对必要，不然会阻塞在这儿
	curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, HCU_HUIRESTCODEC_INSTANCE_DATA_SEND_TIME_OUT_IN_MS);

	//就是当多个线程都使用超时处理的时候，同时主线程中有sleep或是wait等操作。如果不设置这个选项，libcurl将会发信号打断这个wait从而导致程序退出。
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);

	//设置发送数据的信息
	curl_easy_setopt(curl, CURLOPT_POST, 1);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, buf);

	//设置回调函数
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, func_huirestcodec_curl_write_callback);

	//设置接收buffer
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&receiveBuffer);

	//实施调用
	CURLcode curlRes = curl_easy_perform(curl);

	//Clean curl pointer
	curl_easy_cleanup(curl);
	curl_global_cleanup();
	if(curlRes != CURLE_OK) HCU_ERROR_PRINT_HUIRESTCODEC("HUIRESTCODEC: Curl perform error!\n");
	if (receiveBuffer->length > 1)
		return SUCCESS;
	else
		return FAILURE;
}

//This callback function gets called by libcurl as soon as there is  data received that needs to be saved
size_t func_huirestcodec_curl_write_callback(void *buffer, size_t size, size_t nmemb, void *userp)
{
	size_t realsize = size*nmemb;
	msg_struct_ethernet_cloudvela_data_rx_t *mem = (msg_struct_ethernet_cloudvela_data_rx_t *)userp;

	//For test
	memset(mem, 0, sizeof(msg_struct_ethernet_cloudvela_data_rx_t));
	if(realsize > HCU_SYSMSG_COM_MSG_BODY_LEN_MAX)
	{
		HcuErrorPrint("HUIRESTCODEC: No enough memory!\n");
		return 0;
	}
	memcpy(mem->buf, buffer, realsize); //mem->length
	mem->length = realsize;

	return realsize;
}





