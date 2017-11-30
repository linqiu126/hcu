/**
 ****************************************************************************************
 *
 * @file huijsoncodec.h
 *
 * @brief HUIJSONCODEC
 *
 * BXXH team
 * Created by ZJL, 20171125
 *
 ****************************************************************************************
 */

#ifndef L2FRAME_HUIJSONCODEC_H_
#define L2FRAME_HUIJSONCODEC_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "MQTTClient.h"
#include <json-c/json.h>
#include <json-c/json_object.h>
#include "../l2codec/huitp.h"
#include "../l0comvm/vmlayer.h"
#include "../l0comvm/sysconfig.h"
#include "../l1com/l1comdef.h"
#include "../l0service/mqtt.h"
#include "../l0comvm/vmlayer.h"
#include "../l2frame/cloudvela.h"

//State definition
//#define FSM_STATE_ENTRY  0x00
//#define FSM_STATE_IDLE  0x01
//#define FSM_STATE_COMMON  	0x02
enum FSM_STATE_HUIJSONCODEC
{
	FSM_STATE_HUIJSONCODEC_INITED = FSM_STATE_COMMON + 1,
	FSM_STATE_HUIJSONCODEC_ACTIVED,
	FSM_STATE_HUIJSONCODEC_MAX,
};
//#define FSM_STATE_END   0xFE
//#define FSM_STATE_INVALID 0xFF

//Global variables
extern HcuFsmStateItem_t HcuFsmHuijsoncodec[];

//Global APIs
extern OPSTAT fsm_huijsoncodec_task_entry(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_huijsoncodec_init(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_huijsoncodec_restart(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);
extern OPSTAT fsm_huijsoncodec_mqtt_rcv(UINT32 dest_id, UINT32 src_id, void * param_ptr, UINT32 param_len);

//发送API函数
extern OPSTAT hcu_encode_HUITP_JSON_MSGID_uni_earthquake_ctrl_resp(char *toUser, StrIe_HUITP_JSON_MSGID_uni_earthquake_ctrl_resp_t *buf);
extern OPSTAT hcu_encode_HUITP_JSON_MSGID_uni_earthquake_data_report(char *toUser, StrIe_HUITP_JSON_MSGID_uni_earthquake_data_report_t *buf);

//LOCAL APIs
//OPSTAT func_huijsoncodec_HUIJSON_CMDID_cui_uir2hcu_test_cmd_req_received_handle(msg_struct_huicobus_uir_test_cmd_req_t *snd, msg_struct_com_mqtt_rcv_t *rcv);

//高级定义，简化程序的可读性
#define HCU_ERROR_PRINT_HUIJSONCODEC(...)	           	do{zHcuSysStaPm.taskRunErrCnt[TASK_ID_CLOUDVELA]++;  HcuErrorPrint(__VA_ARGS__);  return FAILURE;}while(0)

//FIX DEFINATION
#define HCU_HUIJSON_ENCODE_MSGHEAD_WITH_FIX_VALUE(msgId) \
	do{\
	    jsonobj = json_object_new_object();\
	    cont_jsonobj = json_object_new_object();\
	    if (jsonobj == NULL) HCU_ERROR_PRINT_TASK(TASK_ID_CLOUDVELA, "HUIJSONCODEC: Failed to create json object!\n");\
	    if (cont_jsonobj == NULL) HCU_ERROR_PRINT_TASK(TASK_ID_CLOUDVELA, "HUIJSONCODEC: Failed to create json object!\n");\
	    json_object_object_add(jsonobj, HUITP_MSG_HUIJSON_CONSTANT_TO_USER, json_object_new_string(toUser));\
	    json_object_object_add(jsonobj, HUITP_MSG_HUIJSON_CONSTANT_FROM_USER, json_object_new_string(zHcuSysEngPar.hwBurnId.equLable));\
	    json_object_object_add(jsonobj, HUITP_MSG_HUIJSON_CONSTANT_CREATE_TIME, json_object_new_string(HUITP_MSG_HUIXML_MSGTYPE_HUIJSON_STRING));\
	    json_object_object_add(jsonobj, HUITP_MSG_HUIJSON_CONSTANT_MSG_ID, json_object_new_int(msgId));\
	}while(0)

#define HCU_HUIJSON_ENCODE_MSGBUF_SEND_TO_BH() \
	do{\
	    json_object_object_add(jsonobj, HUITP_MSG_HUIJSON_CONSTANT_IE_CONTENT, json_object_new_string(json_object_to_json_string(cont_jsonobj)));\
	    json_object_object_add(jsonobj, HUITP_MSG_HUIJSON_CONSTANT_MSG_LEN, json_object_new_int(strlen(json_object_to_json_string(cont_jsonobj))));\
	    json_object_put(cont_jsonobj);\
	    json_object_object_add(jsonobj, HUITP_MSG_HUIJSON_CONSTANT_FUNC_FLAG, json_object_new_int(0));\
		CloudDataSendBuf_t pMsgOutput;\
	    memset(pMsgOutput.curBuf, 0, sizeof(pMsgOutput.curBuf));\
	    sprintf((char*)pMsgOutput.curBuf, "%s", json_object_to_json_string(jsonobj));\
	    pMsgOutput.curLen = strlen(pMsgOutput.curBuf);\
	    json_object_put(cont_jsonobj);\
		if (func_cloudvela_send_data_to_cloud(&pMsgOutput) == FAILURE)\
			HCU_ERROR_PRINT_TASK(TASK_ID_CLOUDVELA, "CLOUDVELA: Send message error!\n");\
	}while(0)

//处理多个项目的过程
//#define HCU_HUIJSON_ENCODE_HCU2UIR_MSG_SND_UICOMM(msgid)\
//	do{\
//		if (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFDF_CBU_ID)\
//			HCU_MSG_SEND_GENERNAL_PROCESS(msgid, TASK_ID_BFDFUICOMM, TASK_ID_CLOUDVELA);\
//		else if (HCU_CURRENT_WORKING_PROJECT_ID_UNIQUE == HCU_WORKING_PROJECT_NAME_BFHS_CBU_ID)\
//			HCU_MSG_SEND_GENERNAL_PROCESS(msgid, TASK_ID_BFHSUICOMM, TASK_ID_CLOUDVELA);\
//		else\
//			HCU_ERROR_PRINT_HUIJSONCODEC("HUIJSONCODEC: Error set project name!\n");\
//	}while(0)

#endif /* L2FRAME_HUIJSONCODEC_H_ */
