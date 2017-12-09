/**
 ****************************************************************************************
 *
 * @file huirestcodec.h
 *
 * @brief HUIRESTCODEC
 *
 * BXXH team
 * Created by ZJL, 20171125
 *
 ****************************************************************************************
 */

#ifndef L2FRAME_HUIRESTCODEC_H_
#define L2FRAME_HUIRESTCODEC_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <json-c/json.h>
#include <json-c/json_object.h>
#include "../l1com/l1comdef.h"
#include "../l2codec/huirest.h"

//CONST DEFINATION
#define HCU_HUIRESTCODEC_CONTENT_MAX_LEN 	1000
#define HCU_HUIRESTCODEC_BODY_MAX_LEN 		300
#define HCU_HUIRESTCODEC_INSTANCE_DATA_SEND_TIME_OUT_IN_MS 500

//Global APIs
extern OPSTAT hcu_restful_HUIREST_ACTIONID_PRINTER_callcell_bfsc(StrRestMsgIn_HUIREST_ACTIONID_PRINTER_callcell_bfsc_t *in, StrRestMsgOut_HUIREST_ACTIONID_general_t *out);
extern OPSTAT hcu_restful_HUIREST_ACTIONID_PRINTER_callcell_bfdf(StrRestMsgIn_HUIREST_ACTIONID_PRINTER_callcell_bfdf_t *in, StrRestMsgOut_HUIREST_ACTIONID_general_t *out);
extern OPSTAT hcu_restful_HUIREST_ACTIONID_PRINTER_callcell_bfhs(StrRestMsgIn_HUIREST_ACTIONID_PRINTER_callcell_bfhs_t *in, StrRestMsgOut_HUIREST_ACTIONID_general_t *out);
extern OPSTAT hcu_restful_HUIREST_ACTIONID_PRINTER_fam_sdqx_md1(StrRestMsgIn_HUIREST_ACTIONID_PRINTER_fam_sdqx_md1_t *in, StrRestMsgOut_HUIREST_ACTIONID_general_t *out);
extern OPSTAT hcu_restful_HUIREST_ACTIONID_PRINTER_fam_sdqx_md2(StrRestMsgIn_HUIREST_ACTIONID_PRINTER_fam_sdqx_md2_t *in, StrRestMsgOut_HUIREST_ACTIONID_general_t *out);
extern OPSTAT hcu_restful_HUIREST_ACTIONID_DBA_yczx_temp_update(StrRestMsgIn_HUIREST_ACTIONID_DBA_yczx_temp_update_t *in, StrRestMsgOut_HUIREST_ACTIONID_general_t *out);



//local APIs
OPSTAT func_huirestcodec_curl_data_send(UINT8 serTag, char *buf, msg_struct_ethernet_cloudvela_data_rx_t* receiveBuffer);
size_t func_huirestcodec_curl_write_callback(void *buffer, size_t size, size_t nmemb, void *userp);

//高级定义，简化程序的可读性
#define HCU_ERROR_PRINT_HUIRESTCODEC(...)	           	do{HcuErrorPrint(__VA_ARGS__);  return FAILURE;}while(0)

//FIX DEFINATION
#define HCU_HUIREST_ENCODE_ACTIONHEAD_WITH_FIX_VALUE(svrTag, actionId) \
	do{\
	    jsonobj = json_object_new_object();\
	    cont_jsonobj = json_object_new_object();\
	    if (jsonobj == NULL) HCU_ERROR_PRINT_TASK(TASK_ID_CLOUDVELA, "HUIRESTCODEC: Failed to create json object!\n");\
	    if (cont_jsonobj == NULL){\
	    	json_object_put(jsonobj);\
	    	HCU_ERROR_PRINT_TASK(TASK_ID_CLOUDVELA, "HUIRESTCODEC: Failed to create json object!\n");\
	    }\
	    json_object_object_add(jsonobj, HUIREST_ACCESS_CONST_SERVICE_TAG, json_object_new_string(svrTag));\
	    json_object_object_add(jsonobj, HUIREST_ACCESS_CONST_ACTION_ID, json_object_new_int(actionId));\
	    json_object_object_add(jsonobj, HUIREST_ACCESS_CONST_PAR_FLAG, json_object_new_boolean(TRUE));\
	}while(0)

#define HCU_HUIREST_ENCODE_ACTIONHEAD_TRANSFER_TO_STRING() \
	do{\
	    json_object_object_add(jsonobj, HUIREST_ACCESS_CONST_PAR_CONTENT, cont_jsonobj);\
	    json_object_put(cont_jsonobj);\
	    memset(sendBuf, 0, sizeof(sendBuf));\
	    sprintf(sendBuf, "%s", json_object_to_json_string(jsonobj));\
	    json_object_put(jsonobj);\
		memset(&receiveBuffer, 0, sizeof(msg_struct_ethernet_cloudvela_data_rx_t));\
	}while(0)

#define HCU_HUIREST_DECODE_ACTION_HEAD_SERVICE_TAG(tag) \
	do{\
		jsonobj = json_tokener_parse(receiveBuffer.buf);\
		if (jsonobj == NULL) HCU_ERROR_PRINT_TASK(TASK_ID_CLOUDVELA, "HUIRESTCODEC: Failed to decode json object!\n");\
		cont_jsonobj = json_object_object_get(jsonobj, HUIREST_ACCESS_CONST_SERVICE_TAG);\
		if (cont_jsonobj == NULL)\
		{\
			json_object_put(jsonobj);\
			HCU_ERROR_PRINT_TASK(TASK_ID_CLOUDVELA, "HUIRESTCODEC: Failed to decode HUIREST_ACCESS_CONST_SERVICE_TAG object!\n");\
		}\
		t = strlen(json_object_get_string(cont_jsonobj));\
		memset(stmp, 0, sizeof(stmp));\
		strncpy(stmp, json_object_get_string(cont_jsonobj), t<HCU_HUIRESTCODEC_BODY_MAX_LEN?t:HCU_HUIRESTCODEC_BODY_MAX_LEN);\
		json_object_put(cont_jsonobj);\
		if (strcmp(stmp, tag) != 0){\
			json_object_put(jsonobj);\
			HCU_ERROR_PRINT_TASK(TASK_ID_CLOUDVELA, "HUIJSONCODEC: Failed to match receive SERVICE TAG!\n");\
		}\
	}while(0)

#define HCU_HUIREST_DECODE_ACTION_HEAD_ACTIONID(actionId) \
	do{\
		cont_jsonobj = json_object_object_get(jsonobj, HUIREST_ACCESS_CONST_ACTION_ID);\
		if (cont_jsonobj == NULL)\
		{\
			json_object_put(jsonobj);\
			HCU_ERROR_PRINT_TASK(TASK_ID_CLOUDVELA, "HUIRESTCODEC: Failed to decode HUIREST_ACCESS_CONST_ACTION_ID object!\n");\
		}\
		if (json_object_get_int(cont_jsonobj) != actionId){\
			json_object_put(jsonobj);\
			HCU_ERROR_PRINT_TASK(TASK_ID_CLOUDVELA, "HUIJSONCODEC: Failed to match receive ACTION ID!\n");\
		}\
	}while(0)

#define HCU_HUIREST_DECODE_ACTION_HEAD_PAR_FLAG(flag) \
	do{\
		cont_jsonobj = json_object_object_get(jsonobj, HUIREST_ACCESS_CONST_PAR_FLAG);\
		if (cont_jsonobj == NULL)\
		{\
			json_object_put(jsonobj);\
			HCU_ERROR_PRINT_TASK(TASK_ID_CLOUDVELA, "HUIRESTCODEC: Failed to decode HUIREST_ACCESS_CONST_PAR_FLAG object!\n");\
		}\
		if (json_object_get_boolean(cont_jsonobj) != flag){\
			json_object_put(jsonobj);\
			HCU_ERROR_PRINT_TASK(TASK_ID_CLOUDVELA, "HUIJSONCODEC: Failed to match receive ACTION ID!\n");\
		}\
	}while(0)

#define HCU_HUIREST_DECODE_ACTION_HEAD_PAR_CONTENT() \
	do{\
		cont_jsonobj = json_object_object_get(jsonobj, HUIREST_ACCESS_CONST_PAR_CONTENT);\
		if (cont_jsonobj == NULL)\
		{\
			json_object_put(jsonobj);\
			HCU_ERROR_PRINT_TASK(TASK_ID_CLOUDVELA, "HUIRESTCODEC: Failed to decode HUIREST_ACCESS_CONST_PAR_CONTENT object!\n");\
		}\
		json_object_put(jsonobj);\
	}while(0)

#define HCU_HUIREST_DECODE_ACTION_HEAD_PAR_GENERAL_FB() \
	do{\
		jsonobj = json_object_object_get(jsonobj, "sucFlag");\
		if (jsonobj == NULL)\
		{\
			json_object_put(cont_jsonobj);\
			HCU_ERROR_PRINT_TASK(TASK_ID_CLOUDVELA, "HUIRESTCODEC: Failed to decode HUIREST_ACCESS_CONST_PAR_CONTENT L2 object!\n");\
		}\
		out->sucFlag = json_object_get_boolean(jsonobj);\
		json_object_put(jsonobj);\
		jsonobj = json_object_object_get(jsonobj, "errCode");\
		if (jsonobj == NULL)\
		{\
			json_object_put(cont_jsonobj);\
			HCU_ERROR_PRINT_TASK(TASK_ID_CLOUDVELA, "HUIRESTCODEC: Failed to decode HUIREST_ACCESS_CONST_PAR_CONTENT L2 object!\n");\
		}\
		out->errCode = json_object_get_int(jsonobj);\
		json_object_put(jsonobj);\
		json_object_put(cont_jsonobj);\
	}while(0)



#endif /* L2FRAME_HUIRESTCODEC_H_ */
